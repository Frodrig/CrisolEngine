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
// GUIDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Espacio de nombres para definiciones, constantes y tipos relativos al
//   subsistema de menus, en concreto, para los componentes GUI.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _GUIDEFS_H_
#define _GUIDEFS_H_

#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif

namespace GUIDefs
{
  // Tipos
  typedef word GUIIDComponent; // ID asociado a un componente GUI
  typedef byte GUIEvents;      // Recoge eventos de tipo GUI por OR
  typedef word GUIEventParam;  // Parametro asociado a un evento  

  // Constantes
  const GraphDefs::sAlphaValue ALPHA_TRANSP(155); // Valor para transparencias
  const word CLICK_ELAPSED = 1000 / 3;            // Espera hasta un HOLDDOWN

  // Enumerados
  enum eGUICEventType {
	// Tipos de eventos GUI asociados a componentes (C)
	// Comunes
	GUIC_NO_EVENT            = 0X0000, // No hay evento asociado
	GUIC_SELECT              = 0x0001, // Se esta seleccionado en primera ocasion
	GUIC_UNSELECT            = 0X0002, // De estar seleccionado se pasa a no estarlo
	GUIC_BUTTONRIGHT_PRESSED = 0X0004, // Boton derecho presionado
	GUIC_BUTTONLEFT_PRESSED  = 0X0008, // Boton izquierdo presionado
	// Del componente CGUICInput
	GUIC_INPUT_ACCEPT_ACQUIRING   = 0X0010, // Finalizacion de entrada aceptando
	GUIC_INPUT_NOACCEPT_ACQUIRING = 0X0020  // Finalizacion entrada NO aceptando
  };

  struct sGUICEvent {
	// Define un evento de GUI entre componentes (C)
	// Datos GUIEvent
	eGUICEventType Event;       // Identificador del tipo de evento
	GUIIDComponent IDComponent; // ID del componente
	GUIEventParam  Param;       // Parametro asociado
	// Constructores
	sGUICEvent(const eGUICEventType& aEvent, 
			  const GUIIDComponent& ID,
			  const GUIEventParam& aParam): Event(aEvent), 
										    IDComponent(ID),
										    Param(aParam) { }
	sGUICEvent(const sGUICEvent& GUIEventObj): Event(GUIEventObj.Event),
										       Param(GUIEventObj.Param),
										       IDComponent(GUIEventObj.IDComponent) { }
	// Sobrecarga asignacion
	sGUICEvent& operator=(const sGUICEvent& GUICEvent) { 
	  Event = GUICEvent.Event;
	  IDComponent = GUICEvent.IDComponent;
	  Param = GUICEvent.Param;
	}
  };
}; // ~ GUIDefs

#endif // ~ #ifdef _GUIDEFS_H_
