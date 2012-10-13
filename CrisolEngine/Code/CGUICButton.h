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
// CGUICButton.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUICButton
//
// Descripcion:
// - Define un componente de tipo boton. Estos componentes tendran dos
//   estados: seleccionados o no seleccionados. Siempre tendran asociados
//   un sprite con dos estados de animacion, que se corresponderan con los
//   estados anteriormente citados.
//
// Notas:
// - Los sprites asociados a los botones SIEMPRE deberan de tener al menos
//   dos estados de animacion.
// - El hecho de que la instancia no este activa afectara a los metodos de
//   dibujado y resolucion de eventos de entrada.
// - Cuando un componente boton resulte desactivado, el comando de animacion
//   asociado al boton se desactivara y solo se activara cuando se active
//   el componente.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIBUTTON_H_
#define _CGUIBUTTON_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CGUICOMPONENT_H_
#include "CGUIComponent.h"
#endif
#ifndef _RESDEFS_H_
#include "ResDefs.h"
#endif
#ifndef _CSPRITE_H_
#include "CSprite.h"
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CGUICButton
class CGUICButton: public CGUIComponent
{
private:
  // Tipos
  typedef CGUIComponent Inherited; // Acceso a la clase base

public:
  // Estructuras
  struct sGUICButtonInitData: public CGUIComponent::sGUIComponentInitData {
	// Inicializacion de CGUICButton
	// Datos
	std::string szATButton; // Plantilla de anim. para el boton	
  };

private:
  // Enumerados
  enum eAnimState { 
	// Estados de animacion para el boton
	ANIM_NOSELECT = 0x00, // Estado no seleccionado
	ANIM_SELECT   = 0x01  // Estado seleccionado
  };

private:
  // Estructuras
  struct ButtonInfo {
	// Informacion sobre el componente de boton
	CSprite                BSprite;     // Sprite asociado al boton	
	GraphDefs::sAlphaValue Alpha;       // Valor alpha asociado  	
	bool                   bIsSelect;   // ¿Seleccionado?
	bool                   bChangeAnim; // ¿Hay que cambiar estado de anim.?
  };

private:
  // Vbles de miembro
  ButtonInfo m_ButtonInfo; // Info asociada al boton

public:
  // Constructor / destructor
  CGUICButton(void) { }
  ~CGUICButton(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const sGUICButtonInitData& InitData);
  void End(void);

public:
  // CGUIComponent / Dibujado
  void Draw(void);

public:
  // Operaciones sobre el aspecto del boton
  inline void SetAlpha(const GraphDefs::sAlphaValue& Alpha) {
	ASSERT(Inherited::IsInitOk());
	// Se asocia el valor alpha
	m_ButtonInfo.Alpha = Alpha;
  }
  inline GraphDefs::sAlphaValue GetAlpha(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el alpha
	return m_ButtonInfo.Alpha;
  }

public:
  // CGUICComponent / Trabajo con el area del componente
  word GetWidth(void) {
	ASSERT(Inherited::IsInitOk());
	// Se devuelve la anchura
	return m_ButtonInfo.BSprite.GetWidth();
  }
  word GetHeight(void) {
	ASSERT(Inherited::IsInitOk());
	// Se devuelve la altura
	return m_ButtonInfo.BSprite.GetHeight();
  }

public:
  // CGUIComponent / Establecimiento de componente activo / inactivo
  void SetActive(const bool bActive);

public:
  // CGUIComponent / Operaciones sobre el estado de la seleccion
  void Select(const bool bSelect);
  inline bool IsSelect(void) const { 
	ASSERT(Inherited::IsInitOk());
	// ¿Boton seleccionado?
	return m_ButtonInfo.bIsSelect;
  } 

public:
  // Trabajo sobre el sprite que representa el boton
  inline void StopAnim(const bool bStop) {
	ASSERT(Inherited::IsInitOk());
	// Establece flag de pausa en el sprite asociado
	ASSERT(m_ButtonInfo.BSprite.GetAnimTemplate());	
	m_ButtonInfo.BSprite.GetAnimTemplate()->ActiveAnimCmd(!bStop);
  }
  inline bool IsAnimActive(void) {
	ASSERT(Inherited::IsInitOk());
	// Retorna flag
	ASSERT(m_ButtonInfo.BSprite.GetAnimTemplate());
	return m_ButtonInfo.BSprite.GetAnimTemplate()->IsAnimCmdActive();
  }
}; // ~ CGUICButton

#endif _CGUIBUTTON_H_
