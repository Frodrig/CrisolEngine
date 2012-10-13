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
// CInputManager.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CInputManager
//
// Descripcion:
// - Este sera el modulo encargado de coordinar todos los eventos que sucedan
//   en la entrada. Por entrada se entenderan aquellos eventos relativos al
//   teclado o al raton. El sistema de eventos estara orientado a clientes, de
//   tal forma que solo se comprobaran aquellos eventos que tenga un cliente 
//   asociado. Los demas eventos no seran atendidos en la actualizacion.
// - Esta clase trabajara directamente con todos los modulos de DirectX 
//   destinados a la entrada. En concreto, con DXInput, DXDIMouse y 
//   DXDIKeyboard.
//
// Notas:
// - La estructura interna se resumiria a grandes rasgos de la forma siguiente:
//   * Existira un array que contendra, para cada posible evento que pueda
//     ocurrir (relativo a la pulsacion de una tecla, un boton del raton o 
//     al movimiento del raton), una celda con toda la
//     informacion referida al mismo y que estara orientada a las necesidades
//     del cliente que este pendiente de dicho evento. Aunque tambien se
//     incluira informacion extra para trabajar con las DirectX y la lista 
//     misma de todos aquellos clientes asociados al evento.
//   * Por otro lado, existira una lista con aquellos eventos que deberan de
//     comprobarse de tal forma que, despues de una llamada a UpdateInput, 
//     solo se actualizaran aquellos eventos que esten en esa lista pues se
//     entendera que solo existiran clientes asociados a los codigos de eventos
//     de dicha lista.
//   * Cuando se realice una llamada a UpdateInput, se recorrera la la lista de
//     los eventos activos y se actualizaran estos. Despues se propagaran los
//     eventos entre las listas de clientes asociados hasta que uno de ellos
//     decida quedarse con el evento, o bien se llegue al final de la lista
//     de clientes.
// - Todas las estructuras y constantes base estan en InputSysDefs.
// - ADVERTENCIA: Es importante poner los includes a DX al comienzo, pues hay
//   un conflicto de tipos cuando se define byte.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CINPUTMANAGER_H_
#define _CINPUTMANAGER_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _DXINPUT_H_
#include "DXWrapper\\DXInput.h"     
#define _DXINPUT_H_
#endif
#ifndef _DXDIKEYBOARD_H_
#include "DXWrapper\\DXDIKeyboard.h"
#define _DXDIKEYBOARD_H_
#endif
#ifndef _DXDIMOUSE_H_
#include "DXWrapper\\DXDIMouse.h" 
#define _DXDIMOUSE_H_
#endif
#ifndef _ICINPUTMANAGER_H_
#include "iCInputManager.h"
#define _ICINPUTMANAGER_H_
#endif
#ifndef _ICINPUTCLIENT_H_
#include "iCInputClient.h" 
#define _ICINPUTCLIENT_H_
#endif
#ifndef _CMEMORYPOOL_H_
#include "CMemoryPool.h"   
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list> 
#endif
#ifndef _SET_H_
#define _SET_H_
#include <set>
#endif  
#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_
#include <algorithm>
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CInputManager
class CInputManager: public iCInputManager
{ 
private:    
  // Estructuras
  struct sNInputCharClient {
	// Cliente asociado a la entrada de caracteres
	iCInputClient* pClient; // Direccion del cliente
	
	// Constructores
	sNInputCharClient(void): pClient(NULL) { }
	sNInputCharClient(iCInputClient* const apClient): pClient(apClient) { }
	
	// Operadores
	// Asignacion
	sNInputCharClient& operator=(const sNInputCharClient& NInputCharClient) {
	  pClient = NInputCharClient.pClient;
	  return *this;
	}
	// Igualdad
	bool operator==(iCInputClient* const apClient) const {
	  return (pClient == apClient);
	}
  };

  struct sClientNode { 
	// Nodo con los datos relativos al cliente asociado a un codigo de evento
	// Datos
    iCInputClient*              pInputClient; // Puntero al cliente asociado
	InputDefs::eInputEventState StateToCheck; // Estados a chequear

	// Constructores
	sClientNode(iCInputClient* const apInputClient,
				const InputDefs::eInputEventState& aStateToCheck): pInputClient(apInputClient),
																   StateToCheck(aStateToCheck) { }
	// Operadores
	// Igualdad
	bool operator==(const sClientNode& ClientNode) const {
	  // No se comprara el campo ClientState
	  return (pInputClient == ClientNode.pInputClient &&
			  StateToCheck == ClientNode.StateToCheck);
	}
	bool operator==(iCInputClient* const apInputClient) const {
	  return (pInputClient == apInputClient);
	}
  };

private:
  // Tipos
  // Lista con los clientes instalados 
  typedef std::list<sClientNode>    InputClientList;
  typedef InputClientList::iterator InputClientListIt;

private:
  // Estructuras
  struct sInputInfo { 
	// Celda del array con la informacion de todos los eventos a gestionar
    InputDefs::eInputEventType EventType;     // Tipo de evento
    InputDefs::eInputSystem    Dispositive;   // Dispositivo concreto desde el que viene    
	dword                      udDXCode;      // Codigo DX asociado al evento    
	InputClientList            InputClients;  // Lista con los clientes instalados    
    union { 
	  // Eventos asociados a los dispositivos de boton o movimiento (raton)
      InputDefs::sInputInfoButtons   EventButton;    // Boton
      InputDefs::sInputInfoMovements EventMovements; // Movimiento
    };
    
    // Manejador de memoria
    static CMemoryPool MPool;
    static void* operator new(const size_t size) { return MPool.AllocMem(size); }
    static void operator delete(void* pItem)  { MPool.FreeMem(pItem); } 
  };

  struct sNUnsetClient {
	// Nodo con la info sobre un cliente pendiente de ser desinstalado
	InputDefs::eInputEventCode  EventCode;    // Codigo de evento
	iCInputClient*              pInputClient; // Direccion del cliente
	InputDefs::eInputEventState State;        // Estado a liberar
	
	// Constructores
	sNUnsetClient(void): pInputClient(NULL) { }
	sNUnsetClient(const InputDefs::eInputEventCode& aEventCode,
				  iCInputClient* const apInputClient,
				  const InputDefs::eInputEventState& aState): EventCode(aEventCode), 
															  pInputClient(apInputClient),
															  State(aState) { }
	
	// Operadores
	sNUnsetClient& operator=(const sNUnsetClient& UnsetClient) { 
	  EventCode = UnsetClient.EventCode;
	  pInputClient = UnsetClient.pInputClient;
	  State = UnsetClient.State;
	  return *this;
	}
	bool operator==(const sNUnsetClient& UnsetClient) const {
	  return (EventCode == UnsetClient.EventCode &&
		      pInputClient == UnsetClient.pInputClient &&
			  State == UnsetClient.State);
	}
  };

private:
  // Tipos
  // Lista de clientes asociados a la entrada de caracteres
  typedef std::list<sNInputCharClient>  InputCharClientList;
  typedef InputCharClientList::iterator InputCharClientListIt;  
  
  // Lista de eventos pendientes de ser eliminados
  typedef std::list<sNUnsetClient>   UnsetPendingList;
  typedef UnsetPendingList::iterator UnsetPendingIt;  
  
  // Lista de los eventos a chequear en cada actualizacion
  typedef std::list<InputDefs::eInputEventCode> EventsToCheckList;
  typedef EventsToCheckList::iterator           EventsToCheckListIt;

  // Conjunto de clientes bloqueados
  typedef std::set<iCInputClient*>    ClientsBlockedSet;
  typedef ClientsBlockedSet::iterator ClientsBlockedSetIt;

private:    
  // Vbles de miembro  
  static CInputManager* m_pInputManager; // Unica instancia a la clase

  // Instancias DX
  DXInput      m_DXInput;      // Wrapper DX principal para input
  DXDIKeyboard m_DXDIKeyboard; // Wrapper DX para el trabajo con teclado
  DXDIMouse    m_DXDIMouse;    // Wrapper DX para el trabajo con raton

  // Mantenimiento de clientes
  InputCharClientList m_InputCharClients; // Clientes para entrada de caracteres
  UnsetPendingList    m_UnsetPendingList; // Lista de clientes pendientes de eliminar
  ClientsBlockedSet   m_ClientsBlocked;   // Clientes bloqueados

  // Control del eventos
  EventsToCheckList m_EventsToCheck;                           // Eventos a chequear por actualizacion
  sInputInfo        m_InputEvents[InputDefs::MAX_EVENT_CODES]; // Estado de los eventos  
  
  // Resto 
  sPosition m_MaxCursorPos;  // Posicion maxima del cursor
  sPosition m_ActCursorPos;  // Posicion actual del cursor
  bool      m_bInUpdateMode; // ¿Se esta en modo de actualizacion?  
  bool      m_bInitOk;       // ¿Clase inicializada correctamente?  
  
protected:
  // Constructor / Destructor
  CInputManager(void): m_bInitOk(false) { }
public:
  ~CInputManager(void) { 
    End(); 
  }
  
public:
  // Protocolo de inicio y fin de instancia
  bool Init(const sRect& rMouseArea);
  void End(void);
  inline bool IsInitOk(void) const { return m_bInitOk; }

public:
  // Obtencion y destruccion de la instancia singlenton
  static inline CInputManager* const GetInstance(void) {
    if (NULL == m_pInputManager) { 
      // Se crea la instancia
      m_pInputManager = new CInputManager; 
      ASSERT(m_pInputManager)
    }
    return m_pInputManager;
  }
  static inline DestroyInstance(void) {
    if (m_pInputManager) {
      delete m_pInputManager;
      m_pInputManager = NULL;
    }
  }
  
public:
  // Operaciones sobre CInputManagar
  void Update(void);  
private:
  // Metodos de apoyo
  void UpdateDXInput(void);
  
public:
 // iCInputManager / Operaciones de interfaz iCInputManager / instalacion de clientes input
 bool SetInputEvent(const InputDefs::eInputEventCode& EventCode, 
                    iCInputClient* const pInputClient, 
                    const InputDefs::eInputEventState& StatesToCheck);
 void UnsetInputEvent(const InputDefs::eInputEventCode& EventCode, 
                      iCInputClient* const pInputClient,
					  const InputDefs::eInputEventState& State);
 void BlockInputClient(iCInputClient* const pInputClient);
 void UnblockInputClient(iCInputClient* const pInputClient);
 bool IsInputClientBlocked(iCInputClient* const pInputClient) {
   ASSERT(IsInitOk());
   // Retorna flag
   return (m_ClientsBlocked.find(pInputClient) != m_ClientsBlocked.end());
 }
private:
  // Metodos de apoyo
  void ReleaseUnsetClients(void);
  void ReleaseClient(const InputDefs::eInputEventCode& EventCode, 
					 iCInputClient* const pInputClient,
					 const InputDefs::eInputEventState& State);

public:
 // iCInputManager / trabajo con el raton
 void SetMousePos(const sPosition& NewPos) {
   ASSERT(m_DXDIMouse.GetObj());
   // Establece el valor
   m_DXDIMouse.SetMousePos(NewPos.swXPos, NewPos.swYPos);
 }
 sPosition GetLastMousePos(void) const {
   ASSERT(IsInitOk());
   // Retorna ultima posicion 
   return m_ActCursorPos;
 }
 
private:
  // Metodos de apoyo para la comprobacion de eventos
  bool IsStateToCheckValid(const InputDefs::eInputEventType& aEventType, 
                           const InputDefs::eInputEventState& auwStatesToCheck);
  void UpdateButton(const InputDefs::eInputEventCode& aEventCode);
  void UpdateMovement(const InputDefs::eInputEventCode& aEventCode);
  void InitInputEvents(void);
  void DestroyDXInstances(void);
  bool IsClientSet(const InputDefs::eInputEventCode& EventCode, 
				  iCInputClient* const pInputClient,
				  const InputDefs::eInputEventState& StatesToCheck);
  inline bool IsEventSet(const InputDefs::eInputEventCode& EventCode) {
	ASSERT(IsInitOk());
	// Comprueba si el evento recibido esta instalado para ser comprobado
	const EventsToCheckListIt It(std::find(m_EventsToCheck.begin(),
										   m_EventsToCheck.end(),
										   EventCode));
	return (It != m_EventsToCheck.end());
  }
  
public:
  // iCInputManager / Trabajo con clientes de entrada de caracteres
  bool SetInputCharClient(iCInputClient* const pInputClient);
  bool UnsetInputCharClient(iCInputClient* const pInputClient);

public:
  // Trabajo con los caracteres en entrada  
  void InputCharEvent(const byte ubChar);
private:
  // Metodos de apoyo
  inline bool IsInputCharClientSet(iCInputClient* const pInputClient) {
	ASSERT(IsInitOk());
	ASSERT(pInputClient);
	// Comprueba si el cliente pasado se halla instalado
	const InputCharClientListIt It(std::find(m_InputCharClients.begin(),
										     m_InputCharClients.end(),
										     pInputClient));  
	return (It != m_InputCharClients.end());
  }

public:
  // Restablece control 
  inline void Restore(void) {
	ASSERT(IsInitOk());
	// Adquiere el control de entrada sobre teclado y rátón
    m_DXDIMouse.Acquire();
    m_DXDIKeyboard.Acquire();
  }
};

#endif // ~ CInputManager
