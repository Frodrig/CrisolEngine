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
// CGUICAnimBitmap.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUICAnimBitmap.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUICAnimBitmap.h"

#include "SYSEngine.h"
#include "iCGraphicSystem.h"
#include "CSprite.h"
#include "CMemoryPool.h"

// Declaracion de estructuras forward
struct CGUICAnimBitmap::sNAnim {
  // Info asociada a un Anim
  CSprite				 Anim;  // Sprite que representa una animacion
  GraphDefs::sAlphaValue Alpha; // Valor alpha asociado a los vertices
  // Pool de memoria
  static CMemoryPool m_MPool;
  static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
  static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); }  
};

// Pools de memoria
CMemoryPool 
CGUICAnimBitmap::sNAnim::m_MPool(4, sizeof(CGUICAnimBitmap::sNAnim), true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICAnimBitmap::End(void)
{
  // Finaliza instancia si procede
  if (Inherited::IsInitOk()) {
	// Se eliminan todos los Anims instalados y se propaga
	RemoveAllAnims();
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado del componente. Siempre se dibujara ANTES el bitmap
//   que los sprites asociados.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICAnimBitmap::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Componente activo?
  if (Inherited::IsActive()) {
	// Si, se dibuja bitmap
	Inherited::Draw();
	// Se dibujan los posibles sprites asociados
	iCGraphicSystem* const pGraphSys = SYSEngine::GetGraphicSystem();
	AnimListIt It(m_Anims.begin());
	for (; It != m_Anims.end(); ++It) {
	  pGraphSys->Draw(GraphDefs::DZ_GUI,
					  Inherited::GetDrawPlane(),
					  (*It)->Anim.GetXPos(),
					  (*It)->Anim.GetYPos(),
					  &((*It)->Anim));
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activacion / desactivacion del componente. La repercusion anterior a la
//   activacion del bitmap (por propagacion) sera la activacion de los
//   comandos de animacion asociados a los sprites.
// Parametros:
// - bActive. Flag de activacion / desactivacion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICAnimBitmap::SetActive(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se recorre la lista de sprites, activando / desactivando el comando
  // de animacion asociado
  AnimListIt It(m_Anims.begin());
  for (; It != m_Anims.end(); ++It) {	  
	(*It)->Anim.GetAnimTemplate()->ActiveAnimCmd(bActive);
  }

  // Se propaga
  Inherited::SetActive(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nuevo nodo y lo inserta en la lista de Anims.
// Parametros:
// - szAnimTemplate. Plantilla de animacion.
// - sPosition. Posicion asociada.
// - Alpha. Valor de transparencia.
// Devuelve:
// - Si se ha añadido true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CGUICAnimBitmap::AddAnim(const std::string& szAnimTemplate,
						 const sPosition& Position,
						 const GraphDefs::sAlphaValue& Alpha)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(!szAnimTemplate.empty());

  // ¿El numero de animaciones asociado NO es maximo?
  if (m_Anims.size() < CGUICAnimBitmap::MAX_ANIMS) {
	// Se crea un nuevo nodo
	sNAnim* const pAnim = new sNAnim;
	ASSERT(pAnim);

	// Se inicializa sprite y se asocia posicion
	// Nota: La activacion de la animacion solo ocurrira si el componente esta activo	
	pAnim->Anim.Init(szAnimTemplate, 0, 0, Inherited::IsActive());
	ASSERT(pAnim->Anim.IsInitOk());
	pAnim->Anim.SetXPos(Position.swXPos);
	pAnim->Anim.SetYPos(Position.swYPos);
	// Se asocia el valor alpha al nodo y de ahi al sprite
	pAnim->Alpha = Alpha;
	byte ubIt = 0;
	for (; ubIt < 4; ++ubIt) {	
	  pAnim->Anim.SetAlphaValue(GraphDefs::eVertex(ubIt), &(pAnim->Alpha));
	}
  
	// Se añade el nodo al final de la lista de sprites
	m_Anims.push_back(pAnim);
	return true;
  }  

  // No se pudo añadir
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina todos los Anims que esten asociados en la instancia.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICAnimBitmap::RemoveAllAnims(void)
{
  // Se eliminan animaicones si procede
  if (Inherited::IsInitOk()) {
	// Recorre la lista de Anims, eliminando instancias y nodos
	AnimListIt It(m_Anims.begin());
	for (; It != m_Anims.end(); It = m_Anims.erase(It)) {
	  // Elimina instancia a nodo de informacion
	  delete *It;
	}
  }
}
