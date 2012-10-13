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
// CGUIComponent.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIComponent.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIComponent.h"

#include "iCGUIManager.h"
#include "iCMathUtil.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la inicializacion de la clase, asignado un ID a la misma. Tambien
//   asocia cliente de entrada al componente, si es que tiene cliente de 
//   entrada.
// Parametros:
// - InitData. Datos de configuracion.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - La instancia siempre estara desactivada despues de ser inicializada.
// - En caso de que ya estuviera inicializada, se debera de finalizar pues
//   no se permitira reinicializaciones.
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIComponent::Init(const sGUIComponentInitData& InitData)
{
  // SOLO si parametros validos
  ASSERT(InitData.ID);  
  ASSERT((InitData.ubDrawPlane < GraphDefs::MAX_GUI_DRAWPLANES) != 0);

  // ¿Esta inicializada?
  if (IsInitOk()) { 
	return false; 
  }

  // Se toman parametros
  m_ComponentInfo.ID = InitData.ID;  
  m_ComponentInfo.ubDrawPlane = InitData.ubDrawPlane;  
  m_ComponentInfo.Position = InitData.Position;
  m_ComponentInfo.bIsCursorInArea = false;    
  m_ComponentInfo.bIsActive = false;  
  m_ComponentInfo.pGUIWindow = InitData.pGUIWindow;
  m_ComponentInfo.GUIEvents = InitData.GUIEvents;
  m_ComponentInfo.bClickWithSelect = InitData.bClickWithSelect;
  
  // Se levanta flag de inicializacion y se retorna
  m_bIsInitOk = true;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIComponent::End(void)
{
  // Se finaliza instancia
  if (IsInitOk()) {
	// Se desinstala cliente de entrada si procede	
	if (m_ComponentInfo.pGUIWindow) {
	  UnistallClient();
	}
	
	// Se desvinculan vbles
	m_ComponentInfo.ID = 0;
	m_ComponentInfo.bIsActive = false;
	m_ComponentInfo.GUIEvents = 0;
	m_ComponentInfo.pGUIWindow = NULL;

	// Se baja flag
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se activa / desactiva el componente. Al activar se mandara un evento de
//   seleccion si el cursor del raton se halla sobre el area del componente
//   y en caso de que no este, se mandara un evento de deseleccion. Cuando el
//   componente se deactive, se mandara un evento de deseleccion siempre
//   que este seleccionado. En todos los casos, siempre se mandaran eventos
//   si procede.
// Parametros:
// Devuelve:
// Notas:
// - Al activar el cliente, siempre tendra el estado de bloqueo a off.
// - Al activar el cliente, siempre se ejecutara el codigo asociado al 
//   movimiento del raton con las ultimas coordenadas de este en el 
//   CInputManager. Esto permitira que se pueda seleccionar automaticamente un
//   componente si el cursor se halla encima de el
///////////////////////////////////////////////////////////////////////////////
void 
CGUIComponent::SetActive(const bool bActive) 
{	
  // SOLO si instancia activa.
  ASSERT(IsInitOk());

  // ¿NO es redundante la operacion?
  if (IsActive() != bActive) {	
	// Se instala / desinstala cliente de entrada segun proceda
	if (bActive) {
	  if (m_ComponentInfo.pGUIWindow) { 
		// Se instala cliente y se refresca entrada
		// Nota, se establecera el flag DESPUES de instalar el cliente
		// para que no existan efectos laterales		
		InstallClient(); 
		m_ComponentInfo.bIsActive = bActive;
		RefreshInput();	  
		return;
	  }
	} else if (m_ComponentInfo.pGUIWindow) { 	
	  // Si hay seleccion y hay evento de deseleccion, se notifica deseleccion	
	  if (IsSelect() && 
		  IsEvent(GUIDefs::GUIC_UNSELECT)) {		
		SendEventToGUIWindow(GUIDefs::GUIC_UNSELECT);
	  }
	  
	  // Se desinstala cliente e inicializan flags	
	  m_ComponentInfo.bIsCursorInArea = false;
	  UnistallClient(); 

	  // Nota, se establecera el flag DESPUES de desinstalar el cliente
	  // para que no existan efectos laterales		
	  m_ComponentInfo.bIsActive = bActive;
	  return;
	} 

	// Asocia el valor para el resto de casos
	m_ComponentInfo.bIsActive = bActive;
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se refresca la entrada para comprobar si la posicion actual del cursor del
//   raton recae sobre el componente. Este metodo tendra su utilidad cuando
//   se halla podido desactivar manualmente el componente y se quiera comprobar
//   si hay evento de entrada sobre el mismo
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGUIComponent::RefreshInput(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Si componente activo y cliente asociado
  if (IsActive() &&
	  m_ComponentInfo.pGUIWindow) { 
	// Simula movimiento para ver si hay seleccion 
	// Nota: El flag de cursor en area se establecera a false tal y como
	// si se acabara de realizar una activacion reciente	
  	iCInputManager* const pInputManager = SYSEngine::GetInputManager();
	ASSERT(pInputManager);	  	
	m_ComponentInfo.bIsCursorInArea = false;	
	OnMouseMovement(pInputManager->GetLastMousePos());
	  
	// Si no hay seleccion y esta el evento de deseleccion, se notifica
	if (!IsSelect() && 
	    IsEvent(GUIDefs::GUIC_UNSELECT)) {
	  SendEventToGUIWindow(GUIDefs::GUIC_UNSELECT);
	}
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Bloquea / desbloquea al componente de cara al uso de entrada.
// Parametros:
// - bActive. Si vale true, recibira entrada si vale false dejara de recibir
//   actualizaciones de entrada.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIComponent::SetInput(const bool bActive)
{
  // Solo si instancia inicializada
  ASSERT(IsInitOk());
  
  // Activa / desactiva la entrada segun proceda
  if (bActive) {
	// Se desbloquea y se refresca entrada
	SYSEngine::GetInputManager()->UnblockInputClient(this);
	RefreshInput();
  } else {
	// Se bloquea entrada
	SYSEngine::GetInputManager()->BlockInputClient(this);
  }     
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Despacha los eventos activos. Se cumpliara que solo llegaran aquellos
//   eventos asociados a los flags que se pasaron en la inicializacion.
// Parametros:
// Devuelve:
// Notas:
// - El codigo asociado a los distintos eventos de entrada solo se ejecutaran
//   si el componente tiene asociado eventos para comunicar a la ventana de
//   interfaz (GUIWindow) a la que este asociado.
// - Para los eventos de movimiento, siempre habra que comprobar si se quiere
//   o no seleccion, ya que los eventos de entrada enviados desde el 
//   InputManager no podran orientarnos al respecto.
///////////////////////////////////////////////////////////////////////////////
bool
CGUIComponent::DispatchEvent(const InputDefs::sInputEvent& aInputEvent)
{
  // Si la instancia no esta inicializada, se permite distribuir la peticion
  // Nota: Necesario, no poner ASSERT.
  if (!IsInitOk()) { 
	return InputDefs::SHARE_INPUT_EVENT; 
  }

  // Vbles
  bool bShareEvent; // ¿Hay que compartir el evento?

  // Inicializaciones
  bShareEvent = InputDefs::NO_SHARE_INPUT_EVENT;  

  // Evalua el evento producido 
  switch(aInputEvent.EventType) {	
	case InputDefs::BUTTON_EVENT: {	
	  // Evento de pulsacion 
	  // ¿Esta el cursor en el area?
	  if (!m_ComponentInfo.bIsCursorInArea) { 
		bShareEvent = InputDefs::SHARE_INPUT_EVENT;  
		break;
	  }

	  // ¿Se esta manteniendo pulsado el boton?
	  if (aInputEvent.EventButton.State == InputDefs::BUTTON_HOLD_DOWN) {
		// Solo se dejara pasar el evento si ha pasado un tiempo de espera que
		// garantice la posibilidad de realizar clicks aislados sobre el componente
		if (aInputEvent.EventButton.dTime - m_ComponentInfo.udInitClickTime < GUIDefs::CLICK_ELAPSED) {
		  break; 
		}
	  } else {
		// Se toma el tiempo en el que ha sucedido el click
		m_ComponentInfo.udInitClickTime = aInputEvent.EventButton.dTime;
	  }

	  // Se evaluan subeventos
	  switch (aInputEvent.EventButton.Code) {
		case InputDefs::IE_BUTTON_MOUSE_RIGHT: {
		  OnButtonMouseClick(GUIDefs::GUIC_BUTTONRIGHT_PRESSED);
		} break;

		case InputDefs::IE_BUTTON_MOUSE_LEFT: {
		  OnButtonMouseClick(GUIDefs::GUIC_BUTTONLEFT_PRESSED);		  
		} break;
	  }; // ~ switch

	} break;
	
	case InputDefs::MOVEMENT_EVENT:	{
	  // Evento de movimiento del raton.
	  // Se evaluan subeventos
	  switch(aInputEvent.EventMovements.State) {		
		case InputDefs::MOVEMENT_MOVED: {
		  // En los eventos de movimiento SIEMPRE hay que compartir el evento
		  bShareEvent = InputDefs::SHARE_INPUT_EVENT;
		  OnMouseMovement(sPosition(aInputEvent.EventMovements.udXAct,
									aInputEvent.EventMovements.udYAct));
		} break;
	  }; // ~ switch
	} break;	 

	default: {
	  bShareEvent = InputDefs::SHARE_INPUT_EVENT;
	} break;
  }; // ~ switch

  // Se devuelve resultado
  return bShareEvent;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la pulsacion de un boton del raton
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIComponent::OnButtonMouseClick(const GUIDefs::eGUICEventType& Button)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT((Button == GUIDefs::GUIC_BUTTONRIGHT_PRESSED ||
		  Button == GUIDefs::GUIC_BUTTONLEFT_PRESSED) != 0);

  // ¿Los click son solo con seleccion?
  if (m_ComponentInfo.bClickWithSelect) {
	// Se mandara evento solo si el componente esta seleccionado
	if (IsSelect()) {			
	  SendEventToGUIWindow(Button);
	}
  } else {
	// Se mandara evento directamente
	SendEventToGUIWindow(Button);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada al movimiento del raton.
// Parametros:
// - MousePos. Coordenadas.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIComponent::OnMouseMovement(const sPosition& MousePos)
{ 
  // Se obtiene el area del componente
  const sRect rArea(GetXPos(), GetXPos() + GetWidth(), 
					GetYPos(), GetYPos() + GetHeight());
  
  // ¿Se halla el cursor en area?
  if (SYSEngine::GetMathUtil()->IsPointInRect(MousePos, rArea)) {			
	// ¿Es la primera vez que ocurre?
	if (!m_ComponentInfo.bIsCursorInArea) {
	  // Se levanta el flag y se manda evento			  			  
	  m_ComponentInfo.bIsCursorInArea = true;		

	  // Se notifica solo si hay que hacerlo			  
	  if (IsEvent(GUIDefs::GUIC_SELECT) /*&& !IsSelect()*/) {				
		SendEventToGUIWindow(GUIDefs::GUIC_SELECT);
	  }
	}			
  } else if (m_ComponentInfo.bIsCursorInArea) {
	// El cursor no se halla en la zona por primera vez
	// Se baja el flag y se manda evento
	m_ComponentInfo.bIsCursorInArea = false;
	
	// Se notifica solo si hay que hacerlo
	if (IsEvent(GUIDefs::GUIC_UNSELECT) /*&& IsSelect()*/) {			  
	  SendEventToGUIWindow(GUIDefs::GUIC_UNSELECT);
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se instala el cliente segun los eventos que hay asociados. Este metodo se
//   Llamara simpre que se asocie una nueva ventana de interfaz.
// Parametros:
// Devuelve:
// Notas:
// - Este metodo se llamara desde AttachGUIWindow.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIComponent::InstallClient(void)
{
  // SOLO si instancia activa
  ASSERT(IsInitOk());

  // ¿Esta el componente desactivado?
  if (!IsActive()) {  
	// Movimiento del raton
	iCInputManager* const pInputManager = SYSEngine::GetInputManager();
	ASSERT(pInputManager);
	if (IsEvent(GUIDefs::GUIC_SELECT) || 
		IsEvent(GUIDefs::GUIC_UNSELECT)) {
	  pInputManager->SetInputEvent(InputDefs::IE_MOVEMENT_MOUSE, 
								   this, 
								   InputDefs::MOVEMENT_MOVED);
	}

	// Pulsacion boton dcho raton
	if (IsEvent(GUIDefs::GUIC_BUTTONRIGHT_PRESSED)) {
	  pInputManager->SetInputEvent(InputDefs::IE_BUTTON_MOUSE_RIGHT, 
								   this, 
								   InputDefs::BUTTON_PRESSED_DOWN);
	  /*
	  Nota: Mientras no se haga que la pulsacion continuada pueda ser
	        selectiva, mejor tenerlo desactivado.
	  pInputManager->SetInputEvent(InputDefs::IE_BUTTON_MOUSE_RIGHT, 
								   this, 
								   InputDefs::BUTTON_HOLD_DOWN);
	  */
	}


	// Pulsacion izq dcho raton
	if (IsEvent(GUIDefs::GUIC_BUTTONLEFT_PRESSED)) {
	  pInputManager->SetInputEvent(InputDefs::IE_BUTTON_MOUSE_LEFT, 
								   this, 
								   InputDefs::BUTTON_PRESSED_DOWN);
	  /*
	  Nota: Mientras no se haga que la pulsacion continuada pueda ser
	        selectiva, mejor tenerlo desactivado.	  
	  pInputManager->SetInputEvent(InputDefs::IE_BUTTON_MOUSE_LEFT, 
								   this, 
								   InputDefs::BUTTON_HOLD_DOWN);
	  */
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se desinstalan los clientes de entrada que procedan. 
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIComponent::UnistallClient(void)
{
  // SOLO si instancia activa
  ASSERT(IsInitOk());

  // ¿Esta el componente activo?
  if (IsActive()) {  
	// Movimiento del raton
	iCInputManager* const pInputManager = SYSEngine::GetInputManager();
	ASSERT(pInputManager);
	if (IsEvent(GUIDefs::GUIC_SELECT) || 
  		IsEvent(GUIDefs::GUIC_UNSELECT)) {
	  pInputManager->UnsetInputEvent(InputDefs::IE_MOVEMENT_MOUSE, 
									 this, 
									 InputDefs::MOVEMENT_MOVED);
	}

	// Eventos asociados al boton dcho.
	if (IsEvent(GUIDefs::GUIC_BUTTONRIGHT_PRESSED)) {
	  /*
	  Nota: Mientras no se haga que la pulsacion continuada pueda ser
	        selectiva, mejor tenerlo desactivado.	  
	  pInputManager->UnsetInputEvent(InputDefs::IE_BUTTON_MOUSE_RIGHT, 
									 this, 
									 InputDefs::BUTTON_HOLD_DOWN);
	  */
	  pInputManager->UnsetInputEvent(InputDefs::IE_BUTTON_MOUSE_RIGHT, 
									 this, 
									 InputDefs::BUTTON_PRESSED_DOWN);
	}
  
	// Eventos asociados al boton izq.
	if (IsEvent(GUIDefs::GUIC_BUTTONLEFT_PRESSED)) {
	  /*
	  Nota: Mientras no se haga que la pulsacion continuada pueda ser
	        selectiva, mejor tenerlo desactivado.	  
	  pInputManager->UnsetInputEvent(InputDefs::IE_BUTTON_MOUSE_LEFT, 
									 this, 
									 InputDefs::BUTTON_HOLD_DOWN);
	  */
	  pInputManager->UnsetInputEvent(InputDefs::IE_BUTTON_MOUSE_LEFT, 
									 this, 
									 InputDefs::BUTTON_PRESSED_DOWN);
	}  
  }  
}


