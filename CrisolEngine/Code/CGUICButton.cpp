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
// CGUICButton.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUICButton.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUICButton.h"

#include "SYSEngine.h"
#include "iCGraphicSystem.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el componente a traves de los parametros recibidos y apoyandose
//   en la clase base.
// Parametros:
// - InitData. Datos de inicializacion.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
// - No se dejara reinicilizar. Habra que llamar antes a End.
// - Todos los botones seran representandos por sprites de velocidad 0 y de
//   frame rate de 18 fps. Todos los sprites tendra orientacion 0.
// - Por defecto, el valor alpha sera de 255.
///////////////////////////////////////////////////////////////////////////////
bool 
CGUICButton::Init(const sGUICButtonInitData& InitData)
{
  // SOLO si parametros correctos
  ASSERT(InitData.ID);
  ASSERT(InitData.szATButton.size()); 

  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) { 
	return false; 
  }
  
  // Se iniciliza clase base
  if (!Inherited::Init(InitData)) { 
	return false;  
  }

  // Se inicializa sprite
  // Nota: Se inicializara con el comando de animacion desactivado
  if (!m_ButtonInfo.BSprite.Init(InitData.szATButton, 0, 0, false)) {
	// Problemas	
	End();
	return false;
  }  

  // Se comprueba que el sprite sea correcto
  if (m_ButtonInfo.BSprite.GetAnimTemplate()->GetNumStates() <= 1 ||
	  m_ButtonInfo.BSprite.GetAnimTemplate()->GetNumOrientations() != 1) {
	SYSEngine::FatalError("Los componentes de tipo botón han de tener 2 o mas estados y 1 orientación.");
	return false;
  }

  // Se establece estado de animacion
  m_ButtonInfo.BSprite.GetAnimTemplate()->SetState(CGUICButton::ANIM_NOSELECT);	
  
  // Se asocia el valor alpha
  m_ButtonInfo.Alpha.ubAlpha = 255;
  m_ButtonInfo.BSprite.SetAlphaValue(GraphDefs::VERTEX_0, &m_ButtonInfo.Alpha);
  m_ButtonInfo.BSprite.SetAlphaValue(GraphDefs::VERTEX_1, &m_ButtonInfo.Alpha);
  m_ButtonInfo.BSprite.SetAlphaValue(GraphDefs::VERTEX_2, &m_ButtonInfo.Alpha);
  m_ButtonInfo.BSprite.SetAlphaValue(GraphDefs::VERTEX_3, &m_ButtonInfo.Alpha);

  // Se actualizan resto de vbles
  m_ButtonInfo.bIsSelect = false;
  m_ButtonInfo.bChangeAnim = m_ButtonInfo.BSprite.GetAnimTemplate()->GetNumStates() > 1;

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia y libera recursos.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICButton::End(void)
{
  // Finaliza la intancia si procede
  if (Inherited::IsInitOk()) {
	// Se finaliza sprite
	m_ButtonInfo.BSprite.End();	

	// Se finaliza clase base
	Inherited::End();	  
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa / desactiva el componente
// Parametros:
// - bActive. Flag de activacion / desactivacion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICButton::SetActive(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Se actua si procede
  if (bActive != Inherited::IsActive()) {	
	// Se actuara sobre el estado de animacion del sprite asociado
	// Nota: Tendra preferencia este metodo sobre el StopAnim particular
	StopAnim(!bActive);

	// Si se desactiva, se establece estado de animacion de NO seleccion
	if (!bActive) {
	  m_ButtonInfo.BSprite.GetAnimTemplate()->SetState(CGUICButton::ANIM_NOSELECT);
	}

	// Se propaga
	Inherited::SetActive(bActive);
  }    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado del boton, solo si este se halla activo.
// Parametros:
// Devuelve:
// Notas:
// - No se apliaran ni efectos de luces ni de oscuridad al GUI.
///////////////////////////////////////////////////////////////////////////////
void
CGUICButton::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Realiza el dibujado si procede
  if (Inherited::IsActive()) {
	SYSEngine::GetGraphicSystem()->Draw(GraphDefs::DZ_GUI,
										Inherited::GetDrawPlane(),
										Inherited::GetXPos(),
										Inherited::GetYPos(),
										&m_ButtonInfo.BSprite);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Cambiara el estado de animacion asociado al boton, segun sea pase true o
//   false para indicar seleccion y no seleccion respectivamente.
// Parametros:
// - bSelect. Si true se selecciona y si false no se selecciona.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICButton::Select(const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se toma flag de seleccion
  m_ButtonInfo.bIsSelect = bSelect;

  // ¿Hay mas de un estado de seleccion?
  if (!m_ButtonInfo.bChangeAnim) { 
	return; 
  }

  // Se cambia estado de animacion
  if (bSelect) {
	m_ButtonInfo.BSprite.GetAnimTemplate()->SetState(CGUICButton::ANIM_SELECT);	
  } else {
	m_ButtonInfo.BSprite.GetAnimTemplate()->SetState(CGUICButton::ANIM_NOSELECT);		
  }
}