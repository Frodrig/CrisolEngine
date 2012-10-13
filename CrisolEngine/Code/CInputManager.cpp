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
// CInputManager.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CInputManager.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CInputManager.h"

#include "SYSEngine.h"     
#include "iCLogger.h"      
#include "iCTimer.h"       

// Inicializacion de la unica instancia singlenton
CInputManager* CInputManager::m_pInputManager = NULL;

// Inicializacion del manejador de memoria
CMemoryPool CInputManager::sInputInfo::MPool(InputDefs::MAX_EVENT_CODES,
                                             sizeof(CInputManager::sInputInfo), 
                                             false);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia
// Parametros:
// - rMouseArea. Area donde trabajara el raton.
// Devuelve:
// - Si todo correcto true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CInputManager::Init(const sRect& rMouseArea)
{  
  // ¿Esta ya inicializada la instancia?
  if (IsInitOk()) { 
	return false; 
  }

  #ifdef ENGINE_TRACE  
	SYSEngine::GetLogger()->Write("CInputManager> Inicializando manejador de entrada.\n");
  #endif
  
  // Inicializacion de DirectInput
  if (m_DXInput.Create(SYSEngine::GethInstance())) {   
	// Inicializacion de DXDIKeyboard 
	if (m_DXDIKeyboard.Create(&m_DXInput, SYSEngine::GethWnd())) { 
	  // Inicializacion de DXDIMouse // Trabajo con el raton  
	  if (m_DXDIMouse.Create(&m_DXInput, 
							 SYSEngine::GethWnd(), 
							 rMouseArea.swLeft, 
							 rMouseArea.swTop,
							 rMouseArea.swRight,
							 rMouseArea.swBottom)) { 
		// Todo correcto
		// Se establece cursor del raton en zona predeterminada
		m_DXDIMouse.SetMousePos(100,100);

	    // Se inicializa array estatico con los posibles eventos de entrada
		InitInputEvents();

		// Resto de vbles
		m_ActCursorPos.swXPos = 0;
		m_ActCursorPos.swYPos = 0;
		m_bInUpdateMode = false;

		// Todo correcto
		#ifdef ENGINE_TRACE  
		  SYSEngine::GetLogger()->Write("             | Ok.\n");
		#endif
		m_bInitOk = true;
		return true;  
	  } else {
		// Problemas con DXDIMouse
		#ifdef ENGINE_TRACE  
		  SYSEngine::GetLogger()->Write("                   | Error al crear objeto DXDIMouse.\n");
		  SYSEngine::GetLogger()->Write("                   | Código del error: %s.\n",m_DXDIMouse.GetLastStrCode().c_str());
		#endif    
	  }
	} else {
	  // Problemas con DXDIKeyboard
	  #ifdef ENGINE_TRACE  
		SYSEngine::GetLogger()->Write("                   | Error al crear objeto DXDIKeyboard.\n");
		SYSEngine::GetLogger()->Write("                   | Código del error: %s.\n", m_DXDIKeyboard.GetLastStrCode().c_str());
	  #endif
	}
  } else {
   // Problemas con DirectInput
   #ifdef ENGINE_TRACE  
	 SYSEngine::GetLogger()->Write("                   | Error al crear objeto DXInput.\n");
	 SYSEngine::GetLogger()->Write("                   | Código del error: %s.\n",m_DXInput.GetLastStrCode().c_str());
   #endif
  }
  
  // Hubo problemas
  DestroyDXInstances();
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CInputManager::End(void)
{
  if (IsInitOk()) { 
    // Se destruyen las instancias DX
	DestroyDXInstances();

	// Vacia lista de clientes asociados a entrada
	m_InputCharClients.clear();

    // Vacia la lista de eventos activos
    m_EventsToCheck.clear();
        
    // Baja el flag de inicializacion
    m_bInitOk = false;

    // Traza
	#ifdef ENGINE_TRACE  
	  SYSEngine::GetLogger()->Write("CInputManager::End> Subsistema de entrada desinstalado.\n");
	#endif
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destruye las instancias DX.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CInputManager::DestroyDXInstances(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se desvinculan raton y keyboard
  m_DXDIMouse.Unacquire();
  m_DXDIKeyboard.Unacquire();

  // Destruye las instancias DX
  m_DXDIMouse.Destroy();
  m_DXDIKeyboard.Destroy();
  m_DXInput.Destroy();
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Actualizara el estado de todos los eventos activos y para cada uno de 
//   ellos realizara una llamada al metodo DispachEvent de los clientes que
//   tengan asociados, siempre y cuando dicho cliente quiera ser notificado
//   para el estado actual en que se encuentre el evento. De lo contrario,
//   no se realizara ninguna llamada a DispatchEvent.
// Parametros:
// Devuelve:
// - Si la actualizacion se ha hecho sin problemas true. En caso de que haya
//   existido algun fallo, false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CInputManager::Update(void)
{  
  // SOLO si la clase esta inicializada
  ASSERT(IsInitOk());

  // Se establece el flag del modo de actualizacion
  m_bInUpdateMode = true;

  // Se actualizan dispositivos de DX
  UpdateDXInput();
  
  // Se procede a recorrer la lista de eventos activos y para cada uno
  // de ellos, se actualizara su informacion / estado y se despacharan
  // a traves de los clientes asociados
  InputDefs::sInputEvent      InputEvent; // Estructura con el evento a propagar
  InputDefs::eInputEventState EventState; // Estado del evento de entrada
  EventsToCheckListIt EventIt(m_EventsToCheck.begin());
  for (; EventIt != m_EventsToCheck.end(); ++EventIt) {
    // Dependiendo del tipo de evento, asi sera la actualizacion
    switch (m_InputEvents[*EventIt].EventType) {
	  case InputDefs::BUTTON_EVENT: {
		// Evento de boton
        // Se actualiza, se construye la estructura a propagar y guarda estado
        UpdateButton(*EventIt);
        InputEvent.EventType = InputDefs::BUTTON_EVENT;
        InputEvent.EventButton = m_InputEvents[*EventIt].EventButton;
        EventState = m_InputEvents[*EventIt].EventButton.State;
	  } break;

      case InputDefs::MOVEMENT_EVENT: {
		// Evento de movimiento
		// Se actualiza, se construye la estructura a propagar y guarda estado      
		UpdateMovement(*EventIt);
		InputEvent.EventType = InputDefs::MOVEMENT_EVENT;
		InputEvent.EventMovements = m_InputEvents[*EventIt].EventMovements;    
		EventState = m_InputEvents[*EventIt].EventMovements.State;		
	  } break;
    };  // ~ switch

    // Ahora se procede a recorrer la lista de clientes asociados y a
    // propagar el evento de entrada hasta la primera devolucion true o final.
	InputClientListIt ClientIt(m_InputEvents[*EventIt].InputClients.begin());
    for (; ClientIt != m_InputEvents[*EventIt].InputClients.end(); ++ClientIt) {
	  // ¿NO esta el cliente bloqueado?	  
	  if (m_ClientsBlocked.find(ClientIt->pInputClient) == m_ClientsBlocked.end()) { 
		// ¿Se desea chequear el evento?
		if (ClientIt->StateToCheck & EventState) { 		
		  // Si, ¿NO esta pendiente de ser eliminado?
		  const UnsetPendingIt RemoveIt(std::find(m_UnsetPendingList.begin(), 
												  m_UnsetPendingList.end(),
												  sNUnsetClient(*EventIt, ClientIt->pInputClient, EventState)));
		  if (RemoveIt == m_UnsetPendingList.end()) {
			// No, se propaga y en caso de quedarselo rompera el bucle
			if (ClientIt->pInputClient->DispatchEvent(InputEvent)) {
			  break;
			} // ~ if
		  } // ~ if
		} // ~ if		
	  } // ~ if      
    } // ~ for
  } // ~ for

  // Se baja flag de actualizacion
  m_bInUpdateMode = false;

  // Se desalojan clientes que hayan hecho una peticion UnsetInputClient
  ReleaseUnsetClients(); 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo de apoyo para actualizar los dispositivos de entrada en la 
//   operacion Update
// Parametros:
// Devuelve:
// Notas:
// - En caso de no poderse obtener, se producira un error fatal.
///////////////////////////////////////////////////////////////////////////////
void 
CInputManager::UpdateDXInput(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Actualizacion del teclado
  if (!m_DXDIKeyboard.Update()) { 
	// Falla y se intenta adquirir.
    m_DXDIKeyboard.Acquire();
    if (!m_DXDIKeyboard.Update()) { 
	  // Hay problemas graves
	  SYSEngine::FatalError("No se pudo recuperar el teclado");	  
    }
  }

  // Actualizacion del raton
  if (!m_DXDIMouse.Update()) { 
	// Falla y se intenta adquirir.
    m_DXDIMouse.Acquire();
    if (!m_DXDIMouse.Update()) { 
	  // Hay problemas graves
	  SYSEngine::FatalError("No se pudo recuperar el ratón");	  
    }
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Actualiza el estado de un evento de tipo boton (tecla del teclado o bien
//   boton del raton).
// Parametros:
// - aEventCode: Referencia al codigo del evento de entrada, de tal forma que
//   se pueda encontrar en el array de eventos.
// Devuelve:
// Notas:
// - Los tiempos se tomaran en milisegundos
// - Las actualizaciones de estado y demas elementos, se realizaran segun los
//   siguientes convenios:
//   - Si estaba soltado y DX dice que soltado -> sigue soltado
//   - Si estaba soltado y DX dice pulsado -> recien pulsado y a 0 tiempo
//   - Si estaba recien pulsado y DX dice pulsado -> mantenido
//   - Si estaba recien pulsado y DX dice soltado -> recien soltado y toma tiempo
//   - Si estaba mantenido y DX dice pulsado -> mantenido y toma tiempo
//   - Si estaba mantenido y DX dice soltado -> recien soltado y toma tiempo
//   - Si recien soltado y DX dice soltado -> soltado
//   - Si recien soltado y DX dice pulsado -> recien pulsado y 0 tiempo
///////////////////////////////////////////////////////////////////////////////
void 
CInputManager::UpdateButton(const InputDefs::eInputEventCode& aEventCode)
{
  // SOLO si la clase esta inicializada
  ASSERT(IsInitOk());

  // Vbles
  //static bool   bPress; // Almacena consulta a DX
  static double dTime;  // Almacena tiempos

  // Se obtiene el estado del boton
  bool bButtonPressed;
  if (InputDefs::KEYBOARD_DISP == m_InputEvents[aEventCode].Dispositive) { 
    bButtonPressed = m_DXDIKeyboard.IsKeyPressed(m_InputEvents[aEventCode].udDXCode);
  } else if (InputDefs::MOUSE_DISP == m_InputEvents[aEventCode].Dispositive) { 
    bButtonPressed = m_DXDIMouse.IsButtonPressed(m_InputEvents[aEventCode].udDXCode);
  }
    
  // Actualiza estado del evento de entrada (boton) segun notas.          
  switch (m_InputEvents[aEventCode].EventButton.State) { 
	case InputDefs::BUTTON_UNPRESSED: {
      // Boton soltado
      if (!bButtonPressed) { break; }
      m_InputEvents[aEventCode].EventButton.State = InputDefs::BUTTON_PRESSED_DOWN;
      m_InputEvents[aEventCode].EventButton.dTime = 0;                                  
	} break;
          
    case InputDefs::BUTTON_PRESSED_DOWN: {
      // Boton recien pulsado
      if (bButtonPressed) { 
        m_InputEvents[aEventCode].EventButton.State = InputDefs::BUTTON_HOLD_DOWN;
        dTime = SYSEngine::GetTimer()->GetTime();
      } else { 
        m_InputEvents[aEventCode].EventButton.State = InputDefs::BUTTON_RELEASED; 
      }    
	} break;
          
    case InputDefs::BUTTON_HOLD_DOWN: {
      // Boton pulsado y mantenido
      if (!bButtonPressed) { 
        m_InputEvents[aEventCode].EventButton.State = InputDefs::BUTTON_RELEASED;         
      }      
      m_InputEvents[aEventCode].EventButton.dTime += SYSEngine::GetTimer()->GetTime() - dTime;
      dTime = SYSEngine::GetTimer()->GetTime(); 
	} break;
          
    case InputDefs::BUTTON_RELEASED: {
      // Boton recien soltado
	  if (!bButtonPressed) { 
        m_InputEvents[aEventCode].EventButton.State = InputDefs::BUTTON_UNPRESSED;
        m_InputEvents[aEventCode].EventButton.dTime += SYSEngine::GetTimer()->GetTime() - dTime;
        dTime = SYSEngine::GetTimer()->GetTime();
      } else {
        m_InputEvents[aEventCode].EventButton.State = InputDefs::BUTTON_PRESSED_DOWN;
        m_InputEvents[aEventCode].EventButton.dTime = 0;
      }
	} break;
  };
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Actualiza el estado de un evento de tipo movimiento (raton).
// Parametros:
// - aEventCode: Referencia al codigo del evento de entrada, de tal forma que
//   se pueda encontrar en el array de eventos.
// Devuelve:
// Notas:
// - Las actualizaciones de estado y demas elementos, se realizaran segun los
//   siguientes convenios:
//   - Se tomaran los valores actuales de las coordenadas y se les asignaran
//     a los antiguos.
//   - Se tomaran por DX los nuevos valores y se asignaran a los actuales.
///////////////////////////////////////////////////////////////////////////////
void 
CInputManager::UpdateMovement(const InputDefs::eInputEventCode& aEventCode)
{
  // SOLO si la clase esta inicializada
  ASSERT(IsInitOk());

  // Se toman las posiciones actuales del raton via DX
  m_ActCursorPos.swXPos = m_DXDIMouse.GetXPos();
  m_ActCursorPos.swYPos = m_DXDIMouse.GetYPos();

  // Se actualizan los valores en la celda y el estado del raton
  m_InputEvents[aEventCode].EventMovements.udXOld = m_InputEvents[aEventCode].EventMovements.udXAct;
  m_InputEvents[aEventCode].EventMovements.udYOld = m_InputEvents[aEventCode].EventMovements.udYAct;
  m_InputEvents[aEventCode].EventMovements.udXAct = m_ActCursorPos.swXPos;
  m_InputEvents[aEventCode].EventMovements.udYAct = m_ActCursorPos.swYPos;

  // ¿NO ha habido movimiento?
  if (m_InputEvents[aEventCode].EventMovements.udXOld == m_ActCursorPos.swXPos &&
	  m_InputEvents[aEventCode].EventMovements.udYOld == m_ActCursorPos.swYPos) {	
    m_InputEvents[aEventCode].EventMovements.State = InputDefs::MOVEMENT_UNMOVED;
  } else { 
	// Si ha existido movimiento
    m_InputEvents[aEventCode].EventMovements.State = InputDefs::MOVEMENT_MOVED;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encargara de asociar un cliente a un evento en concreto. En caso de que
//   dicho evento no se encuentre con clientes asociados, se activara y pasara
//   a la lista de eventos activos. Si se quiere instalar un cliente que ya
//   lo este, se podra, lo unico que se modificaran los estados a comprobar.
// Parametros:
// - EventCode: Codigo del evento al que se le va a asociar el cliente.
// - pInputClient: Instancia al cliente a asociar al evento.
// - udStatesToCheck: En esta vble se almacenaran los estados que deberan de
//   comprobarse.
// Devuelve:
// - En caso de que el cliente se asocie sin problemas true. Si existiera algun
//   tipo de problema se devolvera el valor false.
// Notas:
// - Se entendera que en condiciones normales, pInputClient no debera de estar
//   instalado antes de llamar a este metodo.
// - Los clientes nuevos se insertaran siempre por el principio, en la lista de
//   clientes asociados al codigo de evento.
///////////////////////////////////////////////////////////////////////////////
bool 
CInputManager::SetInputEvent(const InputDefs::eInputEventCode& EventCode, 
							 iCInputClient* const pInputClient,
							 const InputDefs::eInputEventState& StatesToCheck)
{
  // SOLO si la instancia ya esta inicializada
  ASSERT(IsInitOk());
  // SOLO si la instancia al cliente recbidia es valida
  ASSERT(pInputClient);
  
  // ¿El cliente NO esta instalado?
  if (!IsClientSet(EventCode, pInputClient, StatesToCheck)) {
	// Se insertara en la lista    
    ASSERT(IsStateToCheckValid(m_InputEvents[EventCode].EventType, StatesToCheck));
    m_InputEvents[EventCode].InputClients.push_front(sClientNode(pInputClient, StatesToCheck));
  } else {
	// El cliente esta instalado.
	// Si estuviera en la lista de clientes a desinstalar se extraeria
	const UnsetPendingIt It(std::find(m_UnsetPendingList.begin(),
									  m_UnsetPendingList.end(),
									  sNUnsetClient(EventCode, pInputClient, StatesToCheck)));
	if (It != m_UnsetPendingList.end()) {
	  // Hallado, se elimina y retorna
	  m_UnsetPendingList.erase(It);
	  return true;
	}
  }

  // Si solo hay un cliente, habra que inicializar el codigo del evento
  if (1 == m_InputEvents[EventCode].InputClients.size()) { 
	// Se inicializan los campos atendiendo al tipo de
    // codigo de evento del que se trate. Despues se insertara
    // el codigo de evento en la lista de eventos activos
    switch (m_InputEvents[EventCode].EventType) {
	  case InputDefs::BUTTON_EVENT: {
        // Evento de boton. 
        // Se actualizara el estado del boton como soltado, 
        // y el tiempo a 0 <en este estado no se usa>.
        m_InputEvents[EventCode].EventButton.State = InputDefs::BUTTON_UNPRESSED;
        m_InputEvents[EventCode].EventButton.dTime = 0;                
	  } break;

      case InputDefs::MOVEMENT_EVENT: {
        // Evento de movimiento. 
        // Se tomaran las coordenadas actuales del raton nada mas.        
        m_InputEvents[EventCode].EventMovements.udXAct = m_DXDIMouse.GetXPos();
        m_InputEvents[EventCode].EventMovements.udYAct = m_DXDIMouse.GetYPos();        
	  } break;
    }; 

	// Se procede a insertar codigo de evento en la lista de eventos activos
    m_EventsToCheck.push_back(EventCode);    
  }

  // Todo correcto  
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desvincula un cliente para un determinado evento y estado de chequeo. En
//   caso de que se este en periodo de actualizacion, la peticion se almacenara
//   en una lista para que cuando se finalice la actualizacion la peticion sea
//   atendida. De no estarse actualizando, la desinstalacion sera inmediata.
// Parametros:
// - EventCode. Codigo del evento en donde localizar el cliente.
// - pInputClient. Cliente a eliminar de la lista de clientes asociados.
// - State. Estado de chequeo para el que asociar el cliente.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CInputManager::UnsetInputEvent(const InputDefs::eInputEventCode& EventCode, 
							   iCInputClient* const pInputClient,
							   const InputDefs::eInputEventState& State)
{
  // SOLO si la instancia ya esta inicializada
  ASSERT(IsInitOk());
  // SOLO si la instancia al cliente recbidia es valida
  ASSERT(pInputClient);
  
  // ¿Se esta en modo de actualizacion?
  if (m_bInUpdateMode) {
    // Si, luego se inserta el client en la lista de clientes pendientes	
    m_UnsetPendingList.push_back(sNUnsetClient(EventCode, pInputClient, State));
  } else {
    // No, se desinstala instantaneamente	
    ReleaseClient(EventCode, pInputClient, State);
  }    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre la lista de clientes pendientes de ser eliminados y los desaloja.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CInputManager::ReleaseUnsetClients(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Procede a recorrer la lista de clientes pendientes de ser eliminados  
  UnsetPendingIt It(m_UnsetPendingList.begin());
  for (; It != m_UnsetPendingList.end(); It = m_UnsetPendingList.erase(It)) {
	ReleaseClient(It->EventCode, It->pInputClient, It->State);
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina un cliente instalado. Al eliminarlo, el evento para el que estaba
//   asociando tendra un cliente menos. Si resulta que ya no tiene clientes
//   debera de sacarse de la lista de eventos a chequear en el proceso de
//   actualizacion.
// Parametros:
// - EventCode. Codigo del evento al que esta asociado el cliente.
// - pInputClient. Cliente.
// - State. Estado para el que liberar el cliente.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CInputManager::ReleaseClient(const InputDefs::eInputEventCode& EventCode, 
							 iCInputClient* const pInputClient,
							 const InputDefs::eInputEventState& State)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pInputClient);

  // Se localiza y elimina el cliente
  const InputClientListIt ClientIt(std::find(m_InputEvents[EventCode].InputClients.begin(),
										     m_InputEvents[EventCode].InputClients.end(),
											 sClientNode(pInputClient, State)));
  ASSERT((ClientIt != m_InputEvents[EventCode].InputClients.end()) != 0)
  m_InputEvents[EventCode].InputClients.erase(ClientIt);

  // Si ya no hay ningun cliente pendiente del evento al que estaba el
  // que hemos eliminado, deberemos de ponerlo como inactivo.
  if (m_InputEvents[EventCode].InputClients.empty()) { 
    // Se procede a eliminar el codigo de eventos de la entrada
    const EventsToCheckListIt EventIt(std::find(m_EventsToCheck.begin(),
												m_EventsToCheck.end(),
												EventCode));  
	 ASSERT((EventIt != m_EventsToCheck.end()) != 0);
	 m_EventsToCheck.erase(EventIt);
  }	

  // ¿El cliente quitado estaba bloqueado en la entrada?
  if (IsInputClientBlocked(pInputClient)) {
	// Si, en caso de que no se encuentre escuchando otro evento se desbloqueara
    EventsToCheckListIt EventIt(m_EventsToCheck.begin());
	for (; EventIt != m_EventsToCheck.end(); ++EventIt) {
	  // ¿El cliente esta asociado al evento actual?
	  if (std::find(m_InputEvents[*EventIt].InputClients.begin(),
					m_InputEvents[*EventIt].InputClients.end(),
					pInputClient) != m_InputEvents[*EventIt].InputClients.end()) {
		// Si, luego se desbloquea automaticamente y abandona
		UnblockInputClient(pInputClient);
		break;
	  }
	}	
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Bloquea al cliente introduciendole en el conjunto de clientes bloqueados
// Parametros:
// - pInputClient. Direccion del cliente sobre el que bloquear la entrada.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CInputManager::BlockInputClient(iCInputClient* const pInputClient)
{ 
  // SOLO si la instancia ya esta inicializada
  ASSERT(IsInitOk());
  // SOLO si la instancia al cliente recbidia es valida
  ASSERT(pInputClient);

  // Inserta en el conjunto de clientes bloqueados
  m_ClientsBlocked.insert(pInputClient);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Saca el cliente del conjunto de clientes bloqueados
// Parametros:
// - pInputClient. Cliente a desbloquear.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CInputManager::UnblockInputClient(iCInputClient* const pInputClient)
{
  // SOLO si la instancia ya esta inicializada
  ASSERT(IsInitOk());
  // SOLO si la instancia al cliente recbidia es valida
  ASSERT(pInputClient);

  // Extrae del conjunto al cliente y lo elemina si lo encontro
  const ClientsBlockedSetIt It(m_ClientsBlocked.find(pInputClient));
  if (It != m_ClientsBlocked.end()) {
	m_ClientsBlocked.erase(It);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dado un tipo de evento, comprueba que los flags de estado a comprobar
//   esten relacionados.
// Parametros:
// - aEventType: Referencia al tipo de evento (de boton o movimiento).
// - auwStatesToCheck: Flags de los estados a comprobar.
// Devuelve:
// - Si los flags son coherentes con el tipo de estado true. En caso contrario
//   false.
// Notas:
// - Este metodo sera utilizado a la hora de llamar a SetInputEvent, para
//   comprobar que los flags son correctos.
///////////////////////////////////////////////////////////////////////////////
bool 
CInputManager::IsStateToCheckValid(const InputDefs::eInputEventType& aEventType, 
                                   const InputDefs::eInputEventState& auwStatesToCheck)
{  
  // SOLO si la clase esta inicializada
  ASSERT(IsInitOk());

  // Se diferencia tipo de evento
  switch (aEventType) {
    case InputDefs::BUTTON_EVENT:
      if (auwStatesToCheck & InputDefs::BUTTON_UNPRESSED ||
          auwStatesToCheck & InputDefs::BUTTON_PRESSED_DOWN ||
          auwStatesToCheck & InputDefs::BUTTON_HOLD_DOWN ||
          auwStatesToCheck & InputDefs::BUTTON_RELEASED) { 
        return true; 
      }
      break;
    case InputDefs::MOVEMENT_EVENT:
      if (auwStatesToCheck & InputDefs::MOVEMENT_UNMOVED ||
          auwStatesToCheck & InputDefs::MOVEMENT_MOVED) { 
        return true; 
      }
      break;
  };
  
  // Es incorrecto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inserta un cliente asociado a la entrada de caracteres.
// Parametros:
// - pInputClient. Cliente.
// Devuelve:
// Notas:
// - Si el cliente esta ya asociado, devolvera false y no lo insertara
///////////////////////////////////////////////////////////////////////////////
bool 
CInputManager::SetInputCharClient(iCInputClient* const pInputClient)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pInputClient);

  // ¿El cliente pasado NO figura como instalado?
  if (!IsInputCharClientSet(pInputClient)) {
	// Instala el cliente al final y retorna
	m_InputCharClients.push_back(CInputManager::sNInputCharClient(pInputClient));
	return true;
  }

  // Figura como intalado y no se establece
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina de la lista de clientes asociados a entrada de caracterer, el 
//   recibido.
// Parametros:
// - pInputClient. Cliente a localizar y eliminar
// Devuelve:
// - En caso de que se haya quitado true. En cualquier otro caso false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CInputManager::UnsetInputCharClient(iCInputClient* const pInputClient)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pInputClient);

  // Procede a localizar el cliente
  const InputCharClientListIt It = std::find(m_InputCharClients.begin(),
										 m_InputCharClients.end(),
										 pInputClient);

  // ¿Cliente localizado?
  if (m_InputCharClients.end() != It) {
	// Se borra y retorna
	m_InputCharClients.erase(It);
	return true;	
  }

  // No se hallo
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de comprobar si el cliente pInputClient recibido, esta asociado
//   o no al codigo de entrada EventCode. 
// Parametros:
// - EventCode: Codigo de evento de entrada a consultar.
// - pInputClient: Puntero a la instancia del cliente a comprobar
// Devuelve:
// - Si el cliente ya esta asociado devuelve true. Si el cliente no esta
//   asociado devuelve false.
// Notas:
// - Este metodo es de apoyo a la hora de hacer comprobaciones en SetInputEvent
//   y UnsetInputEvent, principalmente.
///////////////////////////////////////////////////////////////////////////////
bool 
CInputManager::IsClientSet(const InputDefs::eInputEventCode& EventCode, 
						   iCInputClient* const pInputClient,
						   const InputDefs::eInputEventState& StatesToCheck)
{
  // SOLO si la clase esta inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pInputClient);

  // Comprueba si el cliente pasado ya esta registrado
  InputClientListIt It(m_InputEvents[EventCode].InputClients.begin());
  for (; It != m_InputEvents[EventCode].InputClients.end(); ++It) {
    if (It->pInputClient == pInputClient && It->StateToCheck == StatesToCheck) { 
	  // El cliente pInputClient ya se encuentra instalado
      return true;
    }
  }

  // El cliente no se halla instalado
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este metodo se encarga de recibir un evento asociado a la insercion de
//   un caracter desde Win32 y propagarlo a traves de los clientes que esten
//   asociados.
// Parametros:
// - sbChar. Caracter introducido
// Devuelve:
// Notas:
// - En caso de que un cliente retorne la no comparticion del evento, se
//   dentendra la propagacion en la entrada.
///////////////////////////////////////////////////////////////////////////////
void 
CInputManager::InputCharEvent(const byte ubChar)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Forma el evento a propagar.
  InputDefs::sInputEvent Event;
  Event.EventType = InputDefs::CHAR_EVENT;
  Event.CharEvent.ubChar = ubChar;

  // Procede a propagarlo
  InputCharClientListIt It(m_InputCharClients.begin());
  for (; It != m_InputCharClients.end(); ++It) {
	// Se propaga, si el cliente se queda o no con el evento abandona
	if (InputDefs::NO_SHARE_INPUT_EVENT == It->pClient->DispatchEvent(Event)) {
	  return;
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa, manualmente, todas las celdas del array con los eventos de 
//   entrada controlados por CInputManager.
// Parametros:
// Devuelve:
// Notas:
// - Se esperara que m_InputEvents se haya creado en memoria.
///////////////////////////////////////////////////////////////////////////////
void 
CInputManager::InitInputEvents(void)
{
  // SOLO si m_InputEvents es un array creado
  ASSERT(m_InputEvents);

  // Namespaces
  using namespace InputDefs;

  // Se procede a inicializar los nodos del array manualmente    
  // Teclado: Tipo boton y DIK code de DirectInput
  m_InputEvents[IE_KEY_ESC].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_ESC].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_ESC].EventButton.Code = IE_KEY_ESC;
  m_InputEvents[IE_KEY_ESC].udDXCode = DIK_ESCAPE;
  
  m_InputEvents[IE_KEY_F1].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_F1].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_F1].EventButton.Code = IE_KEY_F1;
  m_InputEvents[IE_KEY_F1].udDXCode = DIK_F1;
  
  m_InputEvents[IE_KEY_F2].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_F2].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_F2].EventButton.Code = IE_KEY_F2;  
  m_InputEvents[IE_KEY_F2].udDXCode = DIK_F2;

  m_InputEvents[IE_KEY_F3].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_F3].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_F3].EventButton.Code = IE_KEY_F3;
  m_InputEvents[IE_KEY_F3].udDXCode = DIK_F3;

  m_InputEvents[IE_KEY_F4].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_F4].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_F4].EventButton.Code = IE_KEY_F4;
  m_InputEvents[IE_KEY_F4].udDXCode = DIK_F4;

  m_InputEvents[IE_KEY_F5].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_F5].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_F5].EventButton.Code = IE_KEY_F5;
  m_InputEvents[IE_KEY_F5].udDXCode = DIK_F5;

  m_InputEvents[IE_KEY_F6].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_F6].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_F6].EventButton.Code = IE_KEY_F6;
  m_InputEvents[IE_KEY_F6].udDXCode = DIK_F6;

  m_InputEvents[IE_KEY_F7].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_F7].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_F7].EventButton.Code = IE_KEY_F7;
  m_InputEvents[IE_KEY_F7].udDXCode = DIK_F7;

  m_InputEvents[IE_KEY_F8].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_F8].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_F8].EventButton.Code = IE_KEY_F8;
  m_InputEvents[IE_KEY_F8].udDXCode = DIK_F8;

  m_InputEvents[IE_KEY_F9].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_F9].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_F9].EventButton.Code = IE_KEY_F9;
  m_InputEvents[IE_KEY_F9].udDXCode = DIK_F9;

  m_InputEvents[IE_KEY_F10].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_F10].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_F10].EventButton.Code = IE_KEY_F10;
  m_InputEvents[IE_KEY_F10].udDXCode = DIK_F10;

  m_InputEvents[IE_KEY_F11].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_F11].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_F11].EventButton.Code = IE_KEY_F11;
  m_InputEvents[IE_KEY_F11].udDXCode = DIK_F11;

  m_InputEvents[IE_KEY_F12].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_F12].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_F12].EventButton.Code = IE_KEY_F12;
  m_InputEvents[IE_KEY_F12].udDXCode = DIK_F12;

  m_InputEvents[IE_KEY_0].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_0].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_0].EventButton.Code = IE_KEY_0;
  m_InputEvents[IE_KEY_0].udDXCode = DIK_0;

  m_InputEvents[IE_KEY_1].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_1].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_1].EventButton.Code = IE_KEY_1;
  m_InputEvents[IE_KEY_1].udDXCode = DIK_1;

  m_InputEvents[IE_KEY_2].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_2].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_2].EventButton.Code = IE_KEY_2;
  m_InputEvents[IE_KEY_2].udDXCode = DIK_2;

  m_InputEvents[IE_KEY_3].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_3].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_3].EventButton.Code = IE_KEY_3;
  m_InputEvents[IE_KEY_3].udDXCode = DIK_3;

  m_InputEvents[IE_KEY_4].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_4].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_4].EventButton.Code = IE_KEY_4;
  m_InputEvents[IE_KEY_4].udDXCode = DIK_4;

  m_InputEvents[IE_KEY_5].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_5].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_5].EventButton.Code = IE_KEY_5;
  m_InputEvents[IE_KEY_5].udDXCode = DIK_5;

  m_InputEvents[IE_KEY_6].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_6].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_6].EventButton.Code = IE_KEY_6;  
  m_InputEvents[IE_KEY_6].udDXCode = DIK_6;
  
  m_InputEvents[IE_KEY_7].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_7].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_7].EventButton.Code = IE_KEY_7;  
  m_InputEvents[IE_KEY_7].udDXCode = DIK_7;

  m_InputEvents[IE_KEY_8].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_8].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_8].EventButton.Code = IE_KEY_8;  
  m_InputEvents[IE_KEY_8].udDXCode = DIK_8;

  m_InputEvents[IE_KEY_9].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_9].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_9].EventButton.Code = IE_KEY_9;  
  m_InputEvents[IE_KEY_9].udDXCode = DIK_9;

  m_InputEvents[IE_KEY_A_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_A_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_A_HI].EventButton.Code = IE_KEY_A_HI;
  m_InputEvents[IE_KEY_A_HI].udDXCode = DIK_A;

  m_InputEvents[IE_KEY_B_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_B_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_B_HI].EventButton.Code = IE_KEY_B_HI;
  m_InputEvents[IE_KEY_B_HI].udDXCode = DIK_B;

  m_InputEvents[IE_KEY_C_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_C_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_C_HI].EventButton.Code = IE_KEY_C_HI;
  m_InputEvents[IE_KEY_C_HI].udDXCode = DIK_C;

  m_InputEvents[IE_KEY_D_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_D_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_D_HI].EventButton.Code = IE_KEY_D_HI;
  m_InputEvents[IE_KEY_D_HI].udDXCode = DIK_D;

  m_InputEvents[IE_KEY_E_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_E_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_E_HI].EventButton.Code = IE_KEY_E_HI;
  m_InputEvents[IE_KEY_E_HI].udDXCode = DIK_E;

  m_InputEvents[IE_KEY_F_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_F_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_F_HI].EventButton.Code = IE_KEY_F_HI;
  m_InputEvents[IE_KEY_F_HI].udDXCode = DIK_F;

  m_InputEvents[IE_KEY_G_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_G_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_G_HI].EventButton.Code = IE_KEY_G_HI;
  m_InputEvents[IE_KEY_G_HI].udDXCode = DIK_G;

  m_InputEvents[IE_KEY_H_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_H_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_H_HI].EventButton.Code = IE_KEY_H_HI;
  m_InputEvents[IE_KEY_H_HI].udDXCode = DIK_H;

  m_InputEvents[IE_KEY_I_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_I_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_I_HI].EventButton.Code = IE_KEY_I_HI;
  m_InputEvents[IE_KEY_I_HI].udDXCode = DIK_I;

  m_InputEvents[IE_KEY_J_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_J_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_J_HI].EventButton.Code = IE_KEY_J_HI;
  m_InputEvents[IE_KEY_J_HI].udDXCode = DIK_J;

  m_InputEvents[IE_KEY_K_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_K_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_K_HI].EventButton.Code = IE_KEY_K_HI;
  m_InputEvents[IE_KEY_K_HI].udDXCode = DIK_K;

  m_InputEvents[IE_KEY_L_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_L_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_L_HI].EventButton.Code = IE_KEY_L_HI;
  m_InputEvents[IE_KEY_L_HI].udDXCode = DIK_L;

  m_InputEvents[IE_KEY_M_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_M_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_M_HI].EventButton.Code = IE_KEY_M_HI;
  m_InputEvents[IE_KEY_M_HI].udDXCode = DIK_M;

  m_InputEvents[IE_KEY_N_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_N_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_N_HI].EventButton.Code = IE_KEY_N_HI;
  m_InputEvents[IE_KEY_N_HI].udDXCode = DIK_N;

  m_InputEvents[IE_KEY_O_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_O_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_O_HI].EventButton.Code = IE_KEY_O_HI;
  m_InputEvents[IE_KEY_O_HI].udDXCode = DIK_O;

  m_InputEvents[IE_KEY_P_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_P_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_P_HI].EventButton.Code = IE_KEY_P_HI;
  m_InputEvents[IE_KEY_P_HI].udDXCode = DIK_P;

  m_InputEvents[IE_KEY_Q_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_Q_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_Q_HI].EventButton.Code = IE_KEY_Q_HI;
  m_InputEvents[IE_KEY_Q_HI].udDXCode = DIK_Q;

  m_InputEvents[IE_KEY_R_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_R_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_R_HI].EventButton.Code = IE_KEY_R_HI;
  m_InputEvents[IE_KEY_R_HI].udDXCode = DIK_R;

  m_InputEvents[IE_KEY_S_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_S_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_S_HI].EventButton.Code = IE_KEY_S_HI;
  m_InputEvents[IE_KEY_S_HI].udDXCode = DIK_S;

  m_InputEvents[IE_KEY_T_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_T_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_T_HI].EventButton.Code = IE_KEY_T_HI;
  m_InputEvents[IE_KEY_T_HI].udDXCode = DIK_T;

  m_InputEvents[IE_KEY_U_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_U_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_U_HI].EventButton.Code = IE_KEY_U_HI;
  m_InputEvents[IE_KEY_U_HI].udDXCode = DIK_U;

  m_InputEvents[IE_KEY_V_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_V_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_V_HI].EventButton.Code = IE_KEY_V_HI;
  m_InputEvents[IE_KEY_V_HI].udDXCode = DIK_V;

  m_InputEvents[IE_KEY_W_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_W_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_W_HI].EventButton.Code = IE_KEY_W_HI;
  m_InputEvents[IE_KEY_W_HI].udDXCode = DIK_W;

  m_InputEvents[IE_KEY_X_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_X_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_X_HI].EventButton.Code = IE_KEY_X_HI;
  m_InputEvents[IE_KEY_X_HI].udDXCode = DIK_X;

  m_InputEvents[IE_KEY_Y_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_Y_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_Y_HI].EventButton.Code = IE_KEY_Y_HI;
  m_InputEvents[IE_KEY_Y_HI].udDXCode = DIK_Y;

  m_InputEvents[IE_KEY_Z_HI].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_Z_HI].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_Z_HI].EventButton.Code = IE_KEY_Z_HI;
  m_InputEvents[IE_KEY_Z_HI].udDXCode = DIK_Z;

  m_InputEvents[IE_KEY_BACK].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_BACK].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_BACK].EventButton.Code = IE_KEY_BACK;  
  m_InputEvents[IE_KEY_BACK].udDXCode = DIK_BACK;

  m_InputEvents[IE_KEY_TAB].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_TAB].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_TAB].EventButton.Code = IE_KEY_TAB;  
  m_InputEvents[IE_KEY_TAB].udDXCode = DIK_TAB;

  m_InputEvents[IE_KEY_RETURN].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_RETURN].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_RETURN].EventButton.Code = IE_KEY_RETURN;  
  m_InputEvents[IE_KEY_RETURN].udDXCode = DIK_RETURN;

  m_InputEvents[IE_KEY_SPACE].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_SPACE].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_SPACE].EventButton.Code = IE_KEY_SPACE;  
  m_InputEvents[IE_KEY_SPACE].udDXCode = DIK_SPACE;

  m_InputEvents[IE_KEY_LCONTROL].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_LCONTROL].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_LCONTROL].EventButton.Code = IE_KEY_LCONTROL;  
  m_InputEvents[IE_KEY_LCONTROL].udDXCode = DIK_LCONTROL;

  m_InputEvents[IE_KEY_RCONTROL].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_RCONTROL].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_RCONTROL].EventButton.Code = IE_KEY_RCONTROL;  
  m_InputEvents[IE_KEY_RCONTROL].udDXCode = DIK_RCONTROL;

  m_InputEvents[IE_KEY_LSHIFT].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_LSHIFT].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_LSHIFT].EventButton.Code = IE_KEY_LSHIFT;  
  m_InputEvents[IE_KEY_LSHIFT].udDXCode = DIK_LSHIFT;

  m_InputEvents[IE_KEY_RSHIFT].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_RSHIFT].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_RSHIFT].EventButton.Code = IE_KEY_RSHIFT;  
  m_InputEvents[IE_KEY_RSHIFT].udDXCode = DIK_RSHIFT;

  m_InputEvents[IE_KEY_ALT].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_ALT].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_ALT].EventButton.Code = IE_KEY_ALT;  
  m_InputEvents[IE_KEY_ALT].udDXCode = DIK_LALT;

  m_InputEvents[IE_KEY_ALTGR].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_ALTGR].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_ALTGR].EventButton.Code = IE_KEY_ALTGR;  
  m_InputEvents[IE_KEY_ALTGR].udDXCode = DIK_RALT;

  m_InputEvents[IE_KEY_INSERT].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_INSERT].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_INSERT].EventButton.Code = IE_KEY_INSERT;
  m_InputEvents[IE_KEY_INSERT].udDXCode = DIK_INSERT;

  m_InputEvents[IE_KEY_REPAG].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_REPAG].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_REPAG].EventButton.Code = IE_KEY_REPAG;  
  m_InputEvents[IE_KEY_REPAG].udDXCode = DIK_PGUP;

  m_InputEvents[IE_KEY_AVPAG].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_AVPAG].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_AVPAG].EventButton.Code = IE_KEY_AVPAG;  
  m_InputEvents[IE_KEY_AVPAG].udDXCode = DIK_PGDN;

  m_InputEvents[IE_KEY_MINUS_PAD].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_MINUS_PAD].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_MINUS_PAD].EventButton.Code = IE_KEY_MINUS_PAD;
  m_InputEvents[IE_KEY_MINUS_PAD].udDXCode = DIK_SUBTRACT;

  m_InputEvents[IE_KEY_ADD_PAD].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_ADD_PAD].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_ADD_PAD].EventButton.Code = IE_KEY_ADD_PAD;
  m_InputEvents[IE_KEY_ADD_PAD].udDXCode = DIK_ADD;

  m_InputEvents[IE_KEY_DIV_PAD].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_DIV_PAD].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_DIV_PAD].EventButton.Code = IE_KEY_DIV_PAD;
  m_InputEvents[IE_KEY_DIV_PAD].udDXCode = DIK_DIVIDE;

  m_InputEvents[IE_KEY_MUL_PAD].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_MUL_PAD].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_MUL_PAD].EventButton.Code = IE_KEY_MUL_PAD;
  m_InputEvents[IE_KEY_MUL_PAD].udDXCode = DIK_MULTIPLY;

  m_InputEvents[IE_KEY_0_PAD].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_0_PAD].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_0_PAD].EventButton.Code = IE_KEY_0_PAD;
  m_InputEvents[IE_KEY_0_PAD].udDXCode = DIK_NUMPAD0;

  m_InputEvents[IE_KEY_1_PAD].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_1_PAD].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_1_PAD].EventButton.Code = IE_KEY_1_PAD;
  m_InputEvents[IE_KEY_1_PAD].udDXCode = DIK_NUMPAD1;

  m_InputEvents[IE_KEY_2_PAD].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_2_PAD].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_2_PAD].EventButton.Code = IE_KEY_2_PAD;
  m_InputEvents[IE_KEY_2_PAD].udDXCode = DIK_NUMPAD2;

  m_InputEvents[IE_KEY_3_PAD].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_3_PAD].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_3_PAD].EventButton.Code = IE_KEY_3_PAD;
  m_InputEvents[IE_KEY_3_PAD].udDXCode = DIK_NUMPAD3;

  m_InputEvents[IE_KEY_4_PAD].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_4_PAD].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_4_PAD].EventButton.Code = IE_KEY_4_PAD;
  m_InputEvents[IE_KEY_4_PAD].udDXCode = DIK_NUMPAD4;

  m_InputEvents[IE_KEY_5_PAD].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_5_PAD].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_5_PAD].EventButton.Code = IE_KEY_5_PAD;
  m_InputEvents[IE_KEY_5_PAD].udDXCode = DIK_NUMPAD5;

  m_InputEvents[IE_KEY_6_PAD].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_7_PAD].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_6_PAD].EventButton.Code = IE_KEY_6_PAD;
  m_InputEvents[IE_KEY_6_PAD].udDXCode = DIK_NUMPAD6;

  m_InputEvents[IE_KEY_7_PAD].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_7_PAD].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_7_PAD].EventButton.Code = IE_KEY_7_PAD;
  m_InputEvents[IE_KEY_7_PAD].udDXCode = DIK_NUMPAD7;

  m_InputEvents[IE_KEY_8_PAD].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_8_PAD].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_8_PAD].EventButton.Code = IE_KEY_8_PAD;
  m_InputEvents[IE_KEY_8_PAD].udDXCode = DIK_NUMPAD8;

  m_InputEvents[IE_KEY_9_PAD].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_9_PAD].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_9_PAD].EventButton.Code = IE_KEY_9_PAD;
  m_InputEvents[IE_KEY_9_PAD].udDXCode = DIK_NUMPAD9;

  m_InputEvents[IE_KEY_UP].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_UP].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_UP].EventButton.Code = IE_KEY_UP;
  m_InputEvents[IE_KEY_UP].udDXCode = DIK_UP;

  m_InputEvents[IE_KEY_DOWN].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_DOWN].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_DOWN].EventButton.Code = IE_KEY_DOWN;  
  m_InputEvents[IE_KEY_DOWN].udDXCode = DIK_DOWN;

  m_InputEvents[IE_KEY_RIGHT].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_RIGHT].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_RIGHT].EventButton.Code = IE_KEY_RIGHT;  
  m_InputEvents[IE_KEY_RIGHT].udDXCode = DIK_RIGHT;

  m_InputEvents[IE_KEY_LEFT].EventType = BUTTON_EVENT;
  m_InputEvents[IE_KEY_LEFT].Dispositive = KEYBOARD_DISP;
  m_InputEvents[IE_KEY_LEFT].EventButton.Code = IE_KEY_LEFT;  
  m_InputEvents[IE_KEY_LEFT].udDXCode = DIK_LEFT;

  // Raton: Tipo boton y codigo de DXDIMouse para botones
  m_InputEvents[IE_BUTTON_MOUSE_LEFT].EventType = BUTTON_EVENT;
  m_InputEvents[IE_BUTTON_MOUSE_LEFT].Dispositive = MOUSE_DISP;
  m_InputEvents[IE_BUTTON_MOUSE_LEFT].EventButton.Code = IE_BUTTON_MOUSE_LEFT;  
  m_InputEvents[IE_BUTTON_MOUSE_LEFT].udDXCode = DXDIDefs::LEFTBUTTON;
  
  m_InputEvents[IE_BUTTON_MOUSE_RIGHT].EventType = BUTTON_EVENT;
  m_InputEvents[IE_BUTTON_MOUSE_RIGHT].Dispositive = MOUSE_DISP;
  m_InputEvents[IE_BUTTON_MOUSE_RIGHT].EventButton.Code = IE_BUTTON_MOUSE_RIGHT;  
  m_InputEvents[IE_BUTTON_MOUSE_RIGHT].udDXCode = DXDIDefs::RIGHTBUTTON;

  // Raton: Tipo movimiento
  m_InputEvents[IE_MOVEMENT_MOUSE].EventType = MOVEMENT_EVENT; 
  m_InputEvents[IE_MOVEMENT_MOUSE].Dispositive = MOUSE_DISP;
}


