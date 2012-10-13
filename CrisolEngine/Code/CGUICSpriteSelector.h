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
// CGUICSpriteSelector.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases
// - CGUICSpriteSelector
//
// Descripcion:
// - Implementa un componente selector de sprites. Estara formado por dos 
//   FXBacks. Uno para cuando se este sin seleccionar y otro para cuando se
//   este seleccionado y, naturalmente, la direccion del sprite asociado que
//   podra ser NULL indicando que no hay ninguno.
//
// Notas:
// - El componente sera muy util para representar inventaros. Por ejemplo, el
//   clasico inventario de fondo negro y sobre el items seria perfectamente
//   representable poniendo el FXBack sin seleccion a negro (o bien con alpha
//   completamente transparente si la ventana de fondo es de color negro) y
//   el FXBack de seleccion a otro color.
// - El componente no es dueño del sprite que tenga asociado y en ese
//   sentido, aunque quede desactivado, NO desactivara el comando de animacion.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUICSPRITESELECTOR_H_
#define _CGUICSPRITESELECTOR_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CGUICOMPONENT_H_
#include "CGUIComponent.h"
#endif
#ifndef _CFXBACK_H_
#include "CFXBack.h"
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif

// Defincion de clases / estructuras / espacios de nombres
class CSprite;

// Clase CGUICSpriteSelector
class CGUICSpriteSelector: public CGUIComponent
{
private:
  // Tipos
  typedef CGUIComponent Inherited; // Acceso a la clase base

public:
  struct sGUICSpriteSelectorInitData: public CGUIComponent::sGUIComponentInitData {
	// Inicializacion de CGUICSpriteSelector	
	word                   uwSelectorWidth;       // Anchura del FXBack
	word                   uwSelectorHeight;      // Altura del FXBack
	GraphDefs::sRGBColor   RGBUnselect;           // Color en deseleccion
	GraphDefs::sAlphaValue AlphaUnselect;         // Alpha en deseleccion
	GraphDefs::sRGBColor   RGBSelect;             // Color en seleccion
	GraphDefs::sAlphaValue AlphaSelect;	          // Alpha en seleccion
	bool                   bSelectOnlyWithSprite; // ¿Seleccion solo con sprite?	
	// Constructor
	sGUICSpriteSelectorInitData(void): uwSelectorWidth(64),
									   uwSelectorHeight(64),
									   bSelectOnlyWithSprite(true) { }
  };

private:
  // Estructuras
  struct sSelectorInfo {
	// Informacion sobre el componente de boton
	// FXBack
	CFXBack  SelectFXBack;    // FXBack en seleccion
	CFXBack  UnselectFXBack;  // FXBack en deseleccion 
	CFXBack* pActFXBack;      // Direccion del FX actual
	// Sprite
	CSprite*            pAttachedSprite; // Direccion del sprite asociado
	AreaDefs::EntHandle hEntity;         // Si el sprite es de entidad, su handle	
	// Resto
	bool bSelectOnlyWithSprite; // ¿Seleccion solo con sprite?	
	bool bIsSelect;             // ¿Selector seleccionado?
  };

private:
  // Vbles de miembro
  sSelectorInfo m_SelectorInfo; // Info asociada al selector

public:
  // Constructor / destructor
  CGUICSpriteSelector(void) { }
  ~CGUICSpriteSelector(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const sGUICSpriteSelectorInitData& InitData);
  void End(void);

public:
  CSprite* const AttachSprite(CSprite* const pSprite,
							  const AreaDefs::EntHandle& hEntity);
  inline CSprite* const GetAttachedSprite(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el sprite asociado
	return m_SelectorInfo.pAttachedSprite;
  }

public:
  // CGUIComponent / Dibujado
  void Draw(void);

public:
  // CGUIComponent / Trabajo con el area del componente
  word GetWidth(void) {
	ASSERT(Inherited::IsInitOk());
	// Retorna anchura
	return m_SelectorInfo.pActFXBack->GetWidth();
  }
  word GetHeight(void) {
	ASSERT(Inherited::IsInitOk());
	// Retorna altura
	return m_SelectorInfo.pActFXBack->GetHeight();
  }

public:
  // CGUIComponent / Operaciones sobre el estado de la seleccion
  void Select(const bool bSelect);
  inline bool IsSelect(void) const { 
	ASSERT(Inherited::IsInitOk());
	// ¿Boton seleccionado?
	return m_SelectorInfo.bIsSelect;
  } 
}; // ~ CGUICSpriteSelector

#endif _CGUICSPRITESELECTOR_H_
