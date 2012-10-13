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
// CEntitySelector.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CEntitySelector.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CEntitySelector.h"

#include "SYSEngine.h"
#include "iCGraphicSystem.h"
#include "iCCommandManager.h"
#include "IsoDefs.h"

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia, recibiendo el nombre de la plantilla de animacion.
// Parametros:
// - szAnimTemplate. Plantilla de animacion.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CEntitySelector::Init(const std::string& szAnimTemplate)
{
  // Finaliza por si se intenta reinicalizar
  End();

  // Se inicializa clase base
  if (Inherited::Init(szAnimTemplate)) {
	// Se toma instancia al subsistema grafico
	m_pGraphSys = SYSEngine::GetGraphicSystem();
	ASSERT(m_pGraphSys);
	// Se inicializan fades
	if (InitAlphaFade()) {
	  // Todo correcto
	  return true;
	}
  }

  // Se retorna resultado
  return Inherited::IsInitOk();
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEntitySelector::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	// Finaliza fades
	EndAlphaFade();

	// Propaga
	Inherited::End();
  }  
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el fade de Alpha.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CEntitySelector::InitAlphaFade(void)
{
  // Se establecen valores alpha
  m_FadeSelect.AlphaSelectFade = GraphDefs::sAlphaValue(55);
  m_FadeSelect.AlphaSource = GraphDefs::sAlphaValue(255);
	
  // Se establece los valores RGB a modificar en cada uno de los vertices
  Inherited::SetAlphaValue(GraphDefs::VERTEX_0, &m_FadeSelect.AlphaSource);
  Inherited::SetAlphaValue(GraphDefs::VERTEX_1, &m_FadeSelect.AlphaSource);
  Inherited::SetAlphaValue(GraphDefs::VERTEX_2, &m_FadeSelect.AlphaSource);
  Inherited::SetAlphaValue(GraphDefs::VERTEX_3, &m_FadeSelect.AlphaSource);
	
  // Se inicializa comando de fade y se coloca el la cola de comandos
  m_FadeSelect.FadeCmd.Init(&m_FadeSelect.AlphaSource, 
  							m_FadeSelect.AlphaSelectFade,
							96);
  SYSEngine::GetCommandManager()->PostCommand(&m_FadeSelect.FadeCmd);

  // Se pausa hasta que se seleccione
  m_FadeSelect.FadeCmd.SetPause(true);

  // Todo correcto 
  return true;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza el fade de RGB.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEntitySelector::EndAlphaFade(void)
{
  // Se desvincula el color de la entidad previa	
  Inherited::SetAlphaValue(GraphDefs::VERTEX_0, NULL);
  Inherited::SetAlphaValue(GraphDefs::VERTEX_1, NULL);
  Inherited::SetAlphaValue(GraphDefs::VERTEX_2, NULL);
  Inherited::SetAlphaValue(GraphDefs::VERTEX_3, NULL);   

  // Se finaliza comando de fade y se saca de la cola
  m_FadeSelect.FadeCmd.End();
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado del selector en pantalla, sobre la posicion DrawPos.
// Parametros:
// - DrawPos. Posicion de dibujado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEntitySelector::Draw(const sPosition& DrawPos)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Realiza la operacion de dibujado
  m_pGraphSys->Draw(GraphDefs::DZ_GAME_SCENE,
				    IsoDefs::WORLDENTITY_DRAW_PLANE,
					DrawPos.swXPos,
					DrawPos.swYPos,
					this);
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la seleccion / deseleccion del fade RGB, el proceso supondra
//   desactivar / activar la pausa respectivamente.
// Parametros:
// - bSelect. Flag de seleccion / deseleccion.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEntitySelector::SelectWithAlphaFade(const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Estado a poner distinto del actual?
  if (bSelect != !m_FadeSelect.FadeCmd.IsInPause()) {
	// ¿Se desea activar?
	if (bSelect) {
	  // Se desactiva el modo pausa
	  m_FadeSelect.FadeCmd.SetPause(false);
	} else {
	  // Se activa el modo pausa PERO se reestablece el valor alpha
	  m_FadeSelect.AlphaSource = GraphDefs::sAlphaValue(255);	  
	  m_FadeSelect.FadeCmd.SetPause(true);
	}
  }
}