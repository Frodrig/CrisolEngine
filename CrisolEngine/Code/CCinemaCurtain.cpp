///////////////////////////////////////////////////////////////////////////////
// CrisolEngine - Computer Role-Play Game Engine
// Copyright (C) 2002 Fernando Rodríguez Martínez
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// CCinemaCurtain.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CCinemaCurtain.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CCinemaCurtain.h"

#include "SYSEngine.h"
#include "iCGraphicSystem.h"
#include "CMemoryPool.h"
#include "CQuadForm.h"

// Estructuras
struct CCinemaCurtain::sCurtainInfo {
  // Info asociada a una cortina  
  CQuadForm Curtain;    // Cortina superior
  sword     swXDrawPos; // Posicion de dibujado en X
  sword     swYDrawPos; // Posicion de dibujado en Y
  // Manejador de memoria
  static CMemoryPool m_MPool;
  static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
  static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
};

// Manejadores de memoria
CMemoryPool 
CCinemaCurtain::sCurtainInfo::m_MPool(2,
									  sizeof(CCinemaCurtain::sCurtainInfo),
									  false);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la clase, estableciendo la altura de los bloques que actuaran
//   de cortina.
// - Se crearan en memoria las estructuras con la info asociada a la cortina
//   superior e inferior.
// Parametros:
// - ubTopHeightCurtain. Altura cortina tope.
// - ubBaseHeightCurtain. Altura cortina base.
// - ubScrollStep. Paso de scroll de la cortina.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
// - No se permitira reinicializar
// - Inicialmente, las cortinas estaran cerradas.
///////////////////////////////////////////////////////////////////////////////
bool 
CCinemaCurtain::Init(const byte ubTopHeightCurtain,
					 const byte ubBaseHeightCurtain)
{
  // SOLO si parametros correctos
  ASSERT(ubTopHeightCurtain);
  ASSERT(ubBaseHeightCurtain);

  // ¿Se intenta reinicializar?
  if (IsInitOk()) { 
	return false; 
  }

  // Se levanta flag para permitir llamada a End
  m_bIsInitOk = true;

  // Se tomara instancia a subsistema grafico
  m_pGraphicSys = SYSEngine::GetGraphicSystem();
  ASSERT(m_pGraphicSys);

  // Inicializa color de cortina
  GraphDefs::sRGBColor CurtainColor(0, 0, 0);
  m_CurtainColor = CurtainColor;
  
  // Crea en mem. estructuras con info de las cortinas
  ASSERT(!m_pTopCurtain);
  m_pTopCurtain = new sCurtainInfo;
  ASSERT(m_pTopCurtain);
  ASSERT(!m_pBottomCurtain);
  m_pBottomCurtain = new sCurtainInfo;
  ASSERT(m_pBottomCurtain);

  // Inicializa datos asociados a cortina superior
  if (!m_pTopCurtain->Curtain.Init(m_pGraphicSys->GetVideoWidth(), 
								   ubTopHeightCurtain)) {
	// Problemas
	End();
	return false;
  }
  m_pTopCurtain->swXDrawPos = 0;
  m_pTopCurtain->swYDrawPos = -ubTopHeightCurtain;   

  // Inicializa datos asociados a cortina inferior
  if (!m_pBottomCurtain->Curtain.Init(m_pGraphicSys->GetVideoWidth(), 
									  ubBaseHeightCurtain)) {
	// Problemas
	End();
	return false;
  }
  m_pBottomCurtain->swXDrawPos = 0;
  m_pBottomCurtain->swYDrawPos = m_pGraphicSys->GetVideoHeight();  

  // Asocia el color de cortina <negro> a las cortinas
  byte ubIt = 0;
  for (; ubIt < 4; ++ubIt) {
	// Obtiene ID a vertice
	GraphDefs::eVertex Vertex = GraphDefs::eVertex(ubIt);

	// Establece color en ambas cortinas
	m_pTopCurtain->Curtain.SetRGBColor(Vertex, &m_CurtainColor);
	m_pBottomCurtain->Curtain.SetRGBColor(Vertex, &m_CurtainColor);
  }

  // Se establecen resto de vbles de miembro
  m_ubTopHeightCurtain = ubTopHeightCurtain;
  m_ubBaseHeightCurtain = ubBaseHeightCurtain;
  m_State = CCinemaCurtain::CLOSED_STATE;

  // Todo correcto  
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia liberando todos los recursos.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCinemaCurtain::End(void)
{
  if (IsInitOk()) {
	// Libera info asociada a cortina superior
	if (m_pTopCurtain) {
	  delete m_pTopCurtain;
	  m_pTopCurtain = NULL;
	}

	// Libera info asociada a cortina inferior
	if (m_pBottomCurtain) {
	  delete m_pBottomCurtain;
	  m_pBottomCurtain = NULL;
	}

	// Baja flag de inicializacion
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Mueve a la cortina superior hacia abajo y a la inferior hacia arriba 
//   uwStepScroll unidades. Cuando dichas cortinas consigan llegar a una pos. 
//   en Y de m_ubHeightCurtain y SYSEngine::GetVideoHeight - m_ubHeightCurtain 
//   respectivamente, la cortina estara abierta. En dicho momento, se cambiara
//   a estado de cortina cerrada abierta y no se podra realizar ningun tipo
//   de operacion OpenCurtain.
// - Siempre que se realice una llamada a este metodo con exito, se establecera
//   el estado actual a OPENING, siempre y cuando, no se haya abierto 
//   completamente la cortina.
// Parametros:
// Devuelve:
// - Si se ha realizado la accion true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCinemaCurtain::OpenCurtain(const word uwStepScroll)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿La cortina ya NO esta abierta?
  if (CCinemaCurtain::OPENING_STATE == m_State ||
	  CCinemaCurtain::CLOSED_STATE == m_State) { 	
	// Se varia la posicion de la cortina superior
	if (m_pTopCurtain->swYDrawPos < 0) {
	  m_pTopCurtain->swYDrawPos += uwStepScroll;
	  if (m_pTopCurtain->swYDrawPos > 0) {
		m_pTopCurtain->swYDrawPos = 0;
	  }
	}

	// Se varia la posicion de la cortina inferior
	const sword swMaxYPos = m_pGraphicSys->GetVideoHeight() - m_ubBaseHeightCurtain;
	if (m_pBottomCurtain->swYDrawPos > swMaxYPos) {
	  m_pBottomCurtain->swYDrawPos -= uwStepScroll;
	  if (m_pBottomCurtain->swYDrawPos < swMaxYPos) {
		m_pBottomCurtain->swYDrawPos = swMaxYPos;
	  }
	}

	// Se establece estado acorde a la accion y retorna
	m_State = (0 == m_pTopCurtain->swYDrawPos && 
			   swMaxYPos == m_pBottomCurtain->swYDrawPos) ? 
			  CCinemaCurtain::OPEN_STATE : CCinemaCurtain::OPENING_STATE;
	return true;
  }

  // La accion no se realizo
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Mueve a la cortina superior hacia arriba y a la inferior hacia abajo 
//   uwStepScroll unidades. Cuando dichas cortinas consigan llegar a una pos
//   en Y de -m_ubHeightCurtain y SYSEngine::GetVideoHeight + m_ubHeightCurtain 
//   respectivamente, la cortina estara cerrada. En dicho momento, se cambiara
//   a estado de cortina abierta a cerrada y no se podra realizar ningun tipo
//   de operacion CloseCurtain.
// - Siempre que se realice una llamada a este metodo con exito, se establecera
//   el estado actual a CLOSENING, siempre y cuando, no se haya abierto 
//   completamente la cortina.
// Parametros:
// Devuelve:
// - Si se ha realizado la accion true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCinemaCurtain::CloseCurtain(const word uwStepScroll)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿La cortina ya NO esta abierta?
  if (CCinemaCurtain::CLOSING_STATE == m_State ||
	  CCinemaCurtain::OPEN_STATE == m_State) { 	
	// Se varia la posicion de la cortina superior
	if (m_pTopCurtain->swYDrawPos > -m_ubTopHeightCurtain) {
	  m_pTopCurtain->swYDrawPos -= uwStepScroll;
	  if (m_pTopCurtain->swYDrawPos < -m_ubTopHeightCurtain) {
		m_pTopCurtain->swYDrawPos = -m_ubTopHeightCurtain;
	  }
	}

	// Se varia la posicion de la cortina inferior	
	if (m_pBottomCurtain->swYDrawPos < m_pGraphicSys->GetVideoHeight()) {
	  m_pBottomCurtain->swYDrawPos += uwStepScroll;
	  if (m_pBottomCurtain->swYDrawPos > m_pGraphicSys->GetVideoHeight()) {
		m_pBottomCurtain->swYDrawPos = m_pGraphicSys->GetVideoHeight();
	  }
	}

	// Se establece estado acorde a la accion y retorna
	m_State = (-m_ubTopHeightCurtain == m_pTopCurtain->swYDrawPos && 
			   m_pGraphicSys->GetVideoHeight() == m_pBottomCurtain->swYDrawPos) ? 
			  CCinemaCurtain::CLOSED_STATE : CCinemaCurtain::CLOSING_STATE;
	return true;
  }

  // La accion no se realizo
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - El dibujado solo se realizara si la cortina se halla en estado distinto
//   a Closed. En tal caso, la cortina sera dibujada.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCinemaCurtain::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Realiza el dibujado si procede
  if (CCinemaCurtain::CLOSED_STATE != m_State) {
	// Cortina superior 
	m_pGraphicSys->Draw(GraphDefs::DZ_FX,
						0,
						m_pTopCurtain->swXDrawPos,
						m_pTopCurtain->swYDrawPos,
						&m_pTopCurtain->Curtain);
	// Cortina inferior
	m_pGraphicSys->Draw(GraphDefs::DZ_FX,
						0,
						m_pBottomCurtain->swXDrawPos,
						m_pBottomCurtain->swYDrawPos,
						&m_pBottomCurtain->Curtain);
  }
}