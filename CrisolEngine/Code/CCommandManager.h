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
// CCommandManager.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CCommandManager
//
// Descripcion:
// - Modulo en el que se almacenaran los comandos logicos y fisicos a ejecutar
//   cada tick de AI y cada tiempo transcurrido en un tick de AI para 
//   implementar la interpolacion. Las peticiones se almacenaran en todos colas
//   de prioridad distintas y se ejecutaran atendiendo a un tiempo maximo, 
//   es decir, a la hora de dar la orden de ejecutar los comandos, se 
//   proporcionara a la cola un tiempo indicativo de que solo podra ejecutar 
//   aquellos comandos que se hayan introducido antes de ese tiempo. Esto 
//   evitara que aquellos comandos generados por los que son ejecutados, sean
//   usados en esa misma llamada. Con este sistema, se dejaran postpuestos para
//   los siguientes ticks.
//
// Notas:
// - La clase sera definida como un singlenton.
// - El manejador sera declarada friend de las clases CCommand, principalmente
//   para acceder al flag indicativo de si el comando esta o no en la cola.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CCOMMANDMANAGER_H_
#define _CCOMMANDMANAGER_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _ICCOMMANDMANAGER_H_
#include "iCCommandManager.h"
#endif
#ifndef _ICTIMER_H_
#include "iCTimer.h"
#endif
#ifndef _CCOMMAND_H_
#include "CCommand.h"
#endif
#ifndef _QUEUE_H_
#define _QUEUE_H_
#include <queue>
#endif
#ifndef _STACK_H_
#define _STACK_H_
#include <stack>
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CCommandManager
class CCommandManager: public iCCommandManager
{  
private:
  // Estructuras  
  struct sNCommand {
	// Nodo asociado a un comando
	// Datos
	dword                  udTime;       // Tiempo de insercion
	CCommand*              pCmd;         // Comando
	iCCommandClient*       pCmdClient;   // Cliente asociado al comando
	CommandDefs::IDCommand IDCmd;        // ID del comando asociado al cliente    
	dword                  udExtraParam; // Parametro extra
  
	// Constructor	
	sNCommand(void): udTime(0), 
					 pCmdClient(NULL), 
					 pCmd(NULL), 
					 IDCmd(NULL),
					 udExtraParam(0) { }
	sNCommand(const dword audTime,
			  CCommand* const apCmd, 
			  iCCommandClient* const apCmdClient, 
			  const CommandDefs::IDCommand& aIDCmd,
			  const dword audExtraParam): udTime(audTime),
										   pCmd(apCmd),
										   pCmdClient(apCmdClient),
										   IDCmd(aIDCmd),
										   udExtraParam(audExtraParam) { }
	sNCommand(const sNCommand& NCommand) {
	  udTime = NCommand.udTime;
	  pCmd = NCommand.pCmd;
	  pCmdClient = NCommand.pCmdClient;
	  IDCmd = NCommand.IDCmd;
	  udExtraParam = NCommand.udExtraParam;
	}

	// Operador de comparacion para la cola de prioridad
	bool operator<(const sNCommand& ObjContainer) const {
	  // La comparacion se realizara usando el valor udTime
	  // Los valores menores seran los que ocupen el puesto top ( de ahi > true)
	  return (udTime > ObjContainer.udTime) ? true : false;
	}

	// Operador de asignacion
	sNCommand& operator=(const sNCommand& NCommand) {
	  // Se copian valores
	  udTime = NCommand.udTime;
	  pCmd = NCommand.pCmd;
	  pCmdClient = NCommand.pCmdClient;
	  IDCmd = NCommand.IDCmd;
	  udExtraParam = NCommand.udExtraParam;
	  // Retorna instancia
	  return *this;
	}
  };

private:
  // Tipos
  // Cola de prioridad para los comandos
  typedef std::priority_queue<sNCommand> CmdQueue; 
  // Pila de apoyo para la busqueda de comandos
  typedef std::stack<sNCommand> CmdStack;
  // Cola simple de peticiones pendientes
  typedef std::queue<sNCommand> CmdPendingQueue;
  
private:
  // Vbles de miembro

  // Instancia singlenton
  static CCommandManager* m_pCommandManager; // Unica instancia a la clase  

  // Interfaces a otros modulos
  iCTimer* m_pTimer; // Timer del sistema

  // Cola de prioridad de comandos
  CmdPendingQueue m_PendingQueue; // Cola de post pendientes
  CmdQueue		  m_CmdQueue;     // Cola de comandos 
  sNCommand       m_ActCmd;       // Comando actualmente tratado
  bool            m_bIsExecuting; // ¿Se estan ejecutando comandos?

  // Resto vbles
  bool m_bIsInitOk; // ¿Clase inicializada correctamente?   
  
protected:
  // Constructor / Destructor
  CCommandManager(void): m_bIsExecuting(false),
						 m_pTimer(NULL),
						 m_bIsInitOk(false) { }
public:
  ~CCommandManager(void) { 
    End();
  }               
  
public:
  // Protocolo de inicio y fin de instancia
  bool Init(void);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }
  
public:
  // Obtencion y destruccion de la instancia singlenton
  static inline CCommandManager* const GetInstance(void) {
    if (NULL == m_pCommandManager) { 
      // Se crea la instancia
      m_pCommandManager = new CCommandManager; 
      ASSERT(m_pCommandManager)
    }
    return m_pCommandManager;
  } 

  static inline DestroyInstance(void) {
    if (m_pCommandManager) {
      delete m_pCommandManager;
      m_pCommandManager = NULL;
    }
  }

public:
  // Ejecucion de comandos
  word ExecuteCommands(const dword udTime, const float fDelta);

public:
  // iCCommandManager / operaciones para insercion de comandos
  void PostCommand(CCommand* const pCommand,
				   iCCommandClient* const pClient = NULL,
				   const CommandDefs::IDCommand& IDCommand = 0,
				   const dword udExtraParam = 0);

public:
  // Borrado de comandos en cola
  void QuitCommand(CCommand* const pCommand);
};
#endif // ~ CCommandManager
