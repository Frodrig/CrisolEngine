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
// CCommandManager.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CCommandManager.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CCommandManager.h"

#include "SYSEngine.h"
#include "iCTimer.h"
#include "iCLogger.h"
#include "iCGraphicSystem.h"
#include "iCCommandClient.h"

// Inicializacion de la unica instancia singlenton
CCommandManager* CCommandManager::m_pCommandManager = NULL;

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el subsistema, creando en memoria la estructura que 
//   mantiene la info general y la cola de prioridad para los comandos.
// Parametros:
// Devuelve:
// - Si todo bien true. Si hay algun problema false.
// Notas:
// - No se permitira reinicializar
///////////////////////////////////////////////////////////////////////////////
bool 
CCommandManager::Init(void)
{
  // ¿Se intenta reinicilizar?
  if (IsInitOk()) { 
	return false; 
  }

  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("CCommandManager::Init> Inicializando manager de comandos.\n");    
  #endif
  
  // Se toman interfaces a otros modulos del motor
  m_pTimer = SYSEngine::GetTimer();
  ASSERT(m_pTimer);

  // Se inicializan resto de vbles
  m_ActCmd.pCmd = NULL;
  m_ActCmd.pCmdClient = NULL;
  m_bIsExecuting = false;
  
  // Todo correcto
  m_bIsInitOk = true;
  #ifdef ENGINE_TRACE
    SYSEngine::GetLogger()->Write("                     | Ok.\n");    
  #endif
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCommandManager::End(void)
{  
  if (IsInitOk()) {    
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CCommandManager::End> Finalizando manager de comandos.\n");
	#endif
		
    // Se baja el flag
    m_bIsInitOk = false;

	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("                    | Ok.\n");      
	#endif
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ejecuta los comandos insertados en un instante anterior a udTime. 
//   Cuando un comando termine, no se volvera a insertar en la cola. 
//   Cuando no termine, se insertara con el instante de tiempo udTime.
// - Cuando el comando tenga asociado un cliente y el comando haya finalizado,
//   se notificara tal hecho al cliente.
// Parametros:
// - udTime. Valor utilizado para determinar que comandos deben de ejecutarse.
// - fDelta. Delta asociado para realizar la interpolacion. Indicara el % que
//   ha transcurrido en el tick de AI actual.
// Devuelve:
// - Numero de comandos que han terminado la ejecucion
// Notas:
///////////////////////////////////////////////////////////////////////////////
word 
CCommandManager::ExecuteCommands(const dword udTime, 
								 const float fDelta)
{ 
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se procede a ejecutar comandos HASTA que se halle uno cuyo tiempo
  // de ejecucion sea mayor que udTime o bien hasta que la cola este vacia.  
  m_bIsExecuting = true;
  word uwNoFinishedCmds = 0;
  while (!m_CmdQueue.empty()) {    
    // Se toma el siguiente comando a ejecutar
    m_ActCmd = m_CmdQueue.top();	

	// ¿El commando actual esta fuera del tiempo de procesamiento?
	if (!(m_ActCmd.udTime < udTime)) { 
	  // Abandona ejecucion de comandos
	  break; 
	}
	
	// No, se extrae de la cola para comenzar a tratarlo
	m_CmdQueue.pop();
		
	// ¿Esta el comando activo?
	if (m_ActCmd.pCmd->IsActive()) {
	  // ¿NO esta en modo pausa?
	  if (!m_ActCmd.pCmd->IsInPause()) {
		// Se ejecuta si no ha sido cancelado desde el exterior
		m_ActCmd.pCmd->Execute(fDelta);	  	  
	  }

	  // ¿Aun no ha terminado de ejecutarse?
	  if (m_ActCmd.pCmd->IsActive()) {
	  // Se cambia el valor temporal y se inserta
	    m_ActCmd.udTime = udTime;
	    m_CmdQueue.push(m_ActCmd);
	  } else {
	    // El comando concluyo.		
		// Se baja el flag de pertenencia a cola
	    m_ActCmd.pCmd->m_bInQueue = false;
	    
		// ¿Tiene el comando asociado un cliente?
	    if (m_ActCmd.pCmdClient) {
	  	  // Se notifica al cliente que su comando ha finalizado
	  	  m_ActCmd.pCmdClient->EndCmdNotify(m_ActCmd.IDCmd, 
											udTime, 
											m_ActCmd.udExtraParam);
	    }	  
		
		// Se incrementa contador de Cmds finalizados
	    ++uwNoFinishedCmds;
	  }	  	  
	} else {	  
	  // El comando no esta activo
	  // Se baja el flag de pertenencia a cola
	  m_ActCmd.pCmd->m_bInQueue = false;
	  
	  // ¿Tiene el comando asociado un cliente?
	  if (m_ActCmd.pCmdClient) {
		// Se notifica al cliente que su comando ha finalizado
		m_ActCmd.pCmdClient->EndCmdNotify(m_ActCmd.IDCmd, 
										  udTime, 
										  m_ActCmd.udExtraParam);
	  }
	  // Se incrementa contador de Cmds finalizados
	  ++uwNoFinishedCmds;
	}	
  } // ~ while

  // Se baja el flag de ejecucion de comandos y se resetea info
  m_bIsExecuting = false;
  m_ActCmd.pCmd = NULL;
  m_ActCmd.pCmdClient = NULL;
  
  // Se postean todas las peticiones que aun no han terminado
  while (!m_PendingQueue.empty()) {
	// Se obtiene peticion pendiente
	const sNCommand NCommand(m_PendingQueue.front());
	m_PendingQueue.pop();

	// Se postea
	PostCommand(NCommand.pCmd,
				NCommand.pCmdClient,
				NCommand.IDCmd,
				NCommand.udExtraParam);
  }
  ASSERT(m_PendingQueue.empty());

  // Se retorna el numero de comandos ejecutados y finalizados
  return uwNoFinishedCmds;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coloca un comando en la cola. Si el comando ya estuviera en cola, 
//   lo sacara y cambiara el instante de insercion, asi como los valores de
//   cliente y codigo de comando. 
//   En caso de que no este en la cola, lo insertara tal cual.
// Parametros:
// - pCommand. Comando a insertar.
// - pClient. Posible cliente al que notificar la finalizacion del comando.
// - IDCommand. Si hay cliente, este sera el identificador del comando.
// - udExtraParam. Parametro extra.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCommandManager::PostCommand(CCommand* const pCommand,
							 iCCommandClient* const pClient,
							 const CommandDefs::IDCommand& IDCommand,
							 const dword udExtraParam) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pCommand);
  #ifdef _SYSASSERT
    // En caso de que exista un cliente asociado al comando, no
    // se podra recibir un ID para el comando distinto de 0
    if (pClient) {
  	  ASSERT_MSG(IDCommand, "¡Se ha recibido un cliente con identificador de comadno 0!");
	}
  #endif

  // ¿Esta activa la ejecucion de comandos?	
  if (m_bIsExecuting) {
	// Si, se inserta en la cola de peticiones pendientes, estas peticiones
	// se insertaran en el momento en que finalice la ejecucion de comandos
	m_PendingQueue.push(sNCommand(0, pCommand, pClient, IDCommand, udExtraParam));
	return;
  }

  // ¿El comando continua activo?
  // Nota: Pudiera ser que se hubiera ejecutado una peticion de insercion
  // en periodo de ejecucion y, aun dentro de ese periodo, se hubiera
  // finalizado el comando.
  if (pCommand->IsActive()) {
	// ¿Esta el comando en la cola?
	if (pCommand->m_bInQueue) {
	  // Se procede a localizar el comando
	  CmdStack  Stack;  // Pila de comandos
	  sNCommand ActCmd; // Comando a localizar
	  while (!m_CmdQueue.empty()) {    
		// Se saca comando de la cola
		ActCmd = m_CmdQueue.top();
		m_CmdQueue.pop();
		
		// ¿Es el buscado?
		if (ActCmd.pCmd == pCommand) {
		  // Si, se abandona bucle		
		  break;
		}
		// No, se inserta en la pila
		Stack.push(ActCmd);
	  }

	  // Se vuelven a insertar los comandos sacados
	  while (!Stack.empty()) {
		// Se saca comando de pila y se inserta en cola
		m_CmdQueue.push(Stack.top());
		Stack.pop();
	  }

	  // Se cambian valores al comando y se vuelve a insertar
	  ActCmd.udTime = m_pTimer->GetTime();
	  ActCmd.pCmdClient = pClient;
	  ActCmd.IDCmd = IDCommand;
	  ActCmd.udExtraParam = udExtraParam;
	  m_CmdQueue.push(ActCmd);	
	} else {
	  // El comando no esta en cola, se inserta por primera vez
	  m_CmdQueue.push(sNCommand(m_pTimer->GetTime(), 
								pCommand, 
								pClient, 
								IDCommand, 
								udExtraParam));
	  pCommand->m_bInQueue = true;	
	}  
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Localiza un comando y lo elimina de la cola. Se usara una pila de apoyo
//   para desalojar los elementos de la cola. Una vez localizado y eliminado
//   el nodo, se restauraran todos los elementos de la cola, pasandolos de la
//   pila a la cola.
// Parametros:
// - pCommand. Direccion del comando a quitar de la cola.
// Devuelve:
// - Si el comando se localizo y borro true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCommandManager::QuitCommand(CCommand* const pCommand)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pCommand);

  // ¿El comando NO esta siendo actualmente tratado?
  if (m_ActCmd.pCmd != pCommand) { 
	// Si, se procede a buscar el comando
	ASSERT(!m_CmdQueue.empty());
	CmdStack Stack;
	while (!m_CmdQueue.empty()) {    
	  // Se saca comando de la cola	
	  const sNCommand ActCmd(m_CmdQueue.top());	
	  m_CmdQueue.pop();

	  // ¿Es el buscado?
	  if (ActCmd.pCmd == pCommand) {
		// Si, se abandona bucle
		pCommand->m_bInQueue = false;	
		break;
	  }

	  // No, se inserta en la pila
	  Stack.push(ActCmd);
	}

	// Se trasladan comandos sacados de vuelta a la cola
	ASSERT(!pCommand->m_bInQueue);
	while (!Stack.empty()) {
	  // Se saca comando de pila y se inserta en cola
	  m_CmdQueue.push(Stack.top());
	  Stack.pop();
	}
  } else {
	// No, es el comando actual. Como lo que se pretende es
	// sacarlo de la cola para EVITAR una NOTIFICACION al posible
	// cliente asociado, se desvinculara cualquier tipo de informacion sobre
	// el mismo (el cliente)
	m_ActCmd.pCmdClient = NULL;
  }
}
