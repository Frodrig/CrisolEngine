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
// CVirtualMachine.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CVirtualMachine.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CVirtualMachine.h"

#include "iCLogger.h"
#include "iCScriptClient.h"
#include "CPlayer.h"
#include "CMemoryPool.h"
#include "CScriptStackValue.h"

// Inicializacion de la unica instancia singlenton
CVirtualMachine* CVirtualMachine::m_pVirtualMachine = NULL;

struct CVirtualMachine::sScript {
  // Scripts en pausa
  CScript         Script;         // Script 
  iCScriptClient* pClient;        // Cliente asociado
  dword           udClientParams; // Parametros adjuntos para el cliente
  word            uwIDArea;       // Identificador del area al que esta asociado
  // Constructor
  sScript(iCScriptClient* const pScriptClient,
		  const dword audClientParams,
		  const word auwIDArea): pClient(pScriptClient),
							      udClientParams(audClientParams),
								  uwIDArea(auwIDArea) { }
 
  // Pool de memoria
  static CMemoryPool m_MPool;
  static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
  static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
};


// Inicializacion de los manejadores de memoria
CMemoryPool 
CVirtualMachine::sScriptToExecute::m_MPool(256, sizeof(CVirtualMachine::sScriptToExecute), true);
CMemoryPool 
CVirtualMachine::sScript::m_MPool(16, sizeof(CVirtualMachine::sScript), true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No permitira reinicializacion
///////////////////////////////////////////////////////////////////////////////
bool 
CVirtualMachine::Init(void)
{
  // ¿Ya esta inicializada la instancia?
  if (IsInitOk()) { 
	return false; 
  }
  
  #ifdef ENGINE_TRACE  
	SYSEngine::GetLogger()->Write("CVirtualMachine::Init> Inicializando máquina virtual.\n");
  #endif

  // Toma instancias a subsistemas
  m_pGDBase = SYSEngine::GetGameDataBase();
  ASSERT(m_pGDBase);

  // Establece resto de vbles de miembro
  m_bScriptExecution = true;
      
  // Todo correcto
  #ifdef ENGINE_TRACE  
    SYSEngine::GetLogger()->Write("                     | Ok.\n");
  #endif
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
CVirtualMachine::End(void)
{
  if (IsInitOk()) {
    #ifdef ENGINE_TRACE  
	  SYSEngine::GetLogger()->Write("CVirtualMachine::End> Finalizando subsistema de audio.\n");
    #endif

	// Finaliza posibles scripts activos
	EndScripts();
	
    // Se baja el flag
	#ifdef ENGINE_TRACE  
	  SYSEngine::GetLogger()->Write("                    | Ok.\n");
    #endif
    m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza toda la informacion relacionada con los scripts pendientes.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::EndScripts(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  ASSERT(!m_bInUpdateMode);

  // Resetea scripts pendientes de ser eliminados
  m_ReleaseClients.clear();

  // Libera informacion en lista de scripts pausados
  PausedScriptsListIt PausedIt(m_PausedScripts.begin());
  for (; PausedIt != m_PausedScripts.end(); PausedIt = m_PausedScripts.erase(PausedIt)) {
	delete *PausedIt;
  }  

  // Libera informacion sobre los scripts con solicitud de ser ejecutados
  ScriptsToExecuteListIt ToExecuteIt(m_ScriptsToExecute.begin());
  for (; ToExecuteIt != m_ScriptsToExecute.end(); ToExecuteIt = m_ScriptsToExecute.erase(ToExecuteIt)) {
	delete *ToExecuteIt;
  }

  // Finaliza el script global
  m_GlobalScript.End();  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Prepara el entorno para la ejecucion de scripts, liberando cualquier
//   tipo de informacion previa sobre scripts. Inicializara tambien el script
//   global.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::PrepareScripts(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Finaliza entorno de ejecucion de los scripts
  EndScripts();

  // Inicializa el script global y en caso de existir se ejecuta directamente
  if (m_GlobalScript.Init()) {
	ScriptDefs::ScriptParamList EmptyParamList;
	m_GlobalScript.Execute(EmptyParamList);
  }
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Prepara el entorno para la ejecucion de scripts, liberando cualquier
//   tipo de informacion previa sobre scripts. Inicializara tambien el script
//   global pero cargando los valores previamente almacenados en disco antes
//   de proceder a realizar la ejecucion del mismo.
// Parametros:
// - hFile. Handle al fichero de donde realizar la carga de informacion.
// - udOffset. Offset donde proceder a realizar la carga de informacion.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::PrepareScripts(const FileDefs::FileHandle& hFile,
								dword& udOffset)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Finaliza entorno de ejecucion de los scripts
  EndScripts();

  // Inicializa script global
  if (m_GlobalScript.Init()) {
	// Ejecuta el script
	ScriptDefs::ScriptParamList EmptyParamList;
	m_GlobalScript.Execute(EmptyParamList);

	// Carga valores y ejecuta el 
	LoadGlobals(hFile, udOffset);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Guarda todos los valores del espacio global de scripts.
//   Internamente se procedera a tomar la instancia CScript del espacio global
//   y para cada uno de los valores CScriptStackValue, ejecutar la operacion
//   Save.
// Parametros:
// - hFile. Handle al subsistema de ficheros.
// - udOffset. Offset donde comenzar a realizar la grabacion.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::SaveGlobals(const FileDefs::FileHandle& hFile,
							 dword& udOffset)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Se itera ordenando la operacion de almacenamiento
  // Nota: Solo si hay script global
  if (m_GlobalScript.IsInitOk()) {
	const word uwNumLocalsPos = m_GlobalScript.GetNumLocalsPos();
	dword udIt = 0;
	for (; udIt < uwNumLocalsPos; ++udIt) {
	  ASSERT(m_GlobalScript.GetValueAt(udIt));
	  m_GlobalScript.GetValueAt(udIt)->Save(hFile, udOffset);
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Operacion de carga de los valores almacenados del espacio global a
//   disco. Internamente, se encargara de iterar entre los valores de 
//   memoria del script global y realizar la peticion de carga.
// Parametros:
// - hFile. Handle al subsistema de ficheros.
// - udOffset. Offset donde comenzar a realizar la carga.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::LoadGlobals(const FileDefs::FileHandle& hFile,
							 dword& udOffset)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se itera ordenando la operacion de carga
  // Nota: Solo si hay script global
  if (m_GlobalScript.IsInitOk()) {
	const word uwNumLocalsPos = m_GlobalScript.GetNumLocalsPos();
	dword udIt = 0;
	for (; udIt < uwNumLocalsPos; ++udIt) {
	  ASSERT(m_GlobalScript.GetValueAt(udIt));
	  m_GlobalScript.GetValueAt(udIt)->Load(hFile, udOffset);
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Actualiza la ejecucion de los scripts. En primer lugar se continuara con
//   aquellos scripts que estando en estado de pausa en el Update anterior, 
//   hayan pasado a estado resume. Por ultimo se recorreran todas aquellas
//   peticiones de ejecucion de scripts existentes.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::Update(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se levanta flag de actualizacion de scripts
  m_bInUpdateMode = true;
  
  // Recorre los scripts que en la actualizacion anterior estaban en pausa
  UpdatePausedScripts();

  // Ejecuta las solicitudes de scripts
  UpdatePendingScripts();

  // Se baja flag de actualizacion de scripts
  m_bInUpdateMode = false;
  
  // Elimina los scripts pendientes
  UpdateReleaseScripts();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Peticion para eliminar TODOS los scripts asociados a un cliente
// Parametros:
// - pClient. Cliente.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::ReleaseScripts(iCScriptClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Se esta actualizando los scripts?
  if (m_bInUpdateMode) {
	// Si, se inserta la peticion en lista
	m_ReleaseClients.push_back(sScriptsToRelease(pClient));
  } else {
	// No, se borra directamente
	ReleaseScript(pClient);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Peticion para eliminar todos aquellos scripts asociados al cliente
//   recibido, que coincidan con el evento tambien indicado.
// Parametros:
// - pClient. Cliente sobre el que actuar.
// - Event. Evento a buscar para eliminar
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::ReleaseScripts(iCScriptClient* const pClient,
								const RulesDefs::eScriptEvents& Event)
{
  // ¿Se esta actualizando los scripts?
  if (m_bInUpdateMode) {
	// Si, se inserta la peticion en lista
	m_ReleaseClients.push_back(sScriptsToRelease(pClient, Event));
  } else {
	// No, se borra directamente
	ReleaseScript(pClient, Event);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina las peticiones y los scripts en ejecucion, asociados a los 
//   clientes script que solicitaran eliminarse.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::UpdateReleaseScripts(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Itera entre los clientes script
  ReleaseClientListIt It(m_ReleaseClients.begin());
  for (; It != m_ReleaseClients.end(); It = m_ReleaseClients.erase(It)) {
	// ¿Se borran TODOS los scripts asociados al cliente?
	if (It->bAllScripts) {
	  // Si, todos cualquiera que sea el evento
	  ReleaseScripts(It->pClient);
	} else {
	  // No, solo el evento indicado
	  ReleaseScripts(It->pClient, It->Event);
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina todas las peticiones y scripts pausados asociados al cliente
//   recibido SIEMPRE Y CUANDO coincidan con el evento tambien recibido.
// Parametros:
// - pClient. Cliente para el que eliminar scripts.
// - Event. Evento a localizar
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::ReleaseScript(iCScriptClient* const pClient,
							   const RulesDefs::eScriptEvents& Event)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se busca entre la lista de scripts solicitados
  ScriptsToExecuteListIt ToExecuteIt(m_ScriptsToExecute.begin());
  while (ToExecuteIt != m_ScriptsToExecute.end()) {
    // ¿Coincide con el cliente a borrar?
    if (pClient == (*ToExecuteIt)->pClient &&
	    Event == (*ToExecuteIt)->Event) {
  	  // Si, elimina
	  delete *ToExecuteIt;
  	  ToExecuteIt = m_ScriptsToExecute.erase(ToExecuteIt);
    } else {
  	  // No, sig. iteracion
  	  ++ToExecuteIt;
    }
  }	

  // Se busca entre la lista de scripts pausados en ejecucion
  PausedScriptsListIt PausedIt(m_PausedScripts.begin());
  while (PausedIt != m_PausedScripts.end()) {
    // ¿Coincide con el cliente a borrar?
    if (pClient == (*PausedIt)->pClient &&
	    Event == (*PausedIt)->Script.GetEvent()) {
  	  // Si, elimina
	  delete *PausedIt;
  	  PausedIt = m_PausedScripts.erase(PausedIt);
    } else {
  	  // No, sig. iteracion
  	  ++PausedIt;
    }
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina todas las peticiones y scripts pausados asociados al cliente
//   recibido, cualquiera que sea el evento en cuestion.
// Parametros:
// - pClient. Cliente para el que eliminar scripts.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::ReleaseScript(iCScriptClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se busca entre la lista de scripts solicitados
  ScriptsToExecuteListIt ToExecuteIt(m_ScriptsToExecute.begin());
  while (ToExecuteIt != m_ScriptsToExecute.end()) {
    // ¿Coincide con el cliente a borrar?
    if (pClient == (*ToExecuteIt)->pClient) {
  	  // Si, elimina
	  delete *ToExecuteIt;
  	  ToExecuteIt = m_ScriptsToExecute.erase(ToExecuteIt);
    } else {
  	  // No, sig. iteracion
  	  ++ToExecuteIt;
    }
  }	

  // Se busca entre la lista de scripts pausados en ejecucion
  PausedScriptsListIt PausedIt(m_PausedScripts.begin());
  while (PausedIt != m_PausedScripts.end()) {
    // ¿Coincide con el cliente a borrar?
    if (pClient == (*PausedIt)->pClient) {
  	  // Si, elimina
	  delete *PausedIt;
  	  PausedIt = m_PausedScripts.erase(PausedIt);
    } else {
  	  // No, sig. iteracion
  	  ++PausedIt;
    }
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre la lista de scripts pausados en la anterior actualizacion para
//   comprobar si el estado de pausa ya ha sido superado. Estos scripts se
//   habran establecido en estado de pausa debido a la espera por la conclusion
//   de una determinada accion. En caso de que esten en estado Resume (vuelta
//   de la pausa) se continuara con su ejecucion.
// - En caso de que el evento que represente el script este en pausa, lo
//   obviara.
// Parametros:
// Devuelve:
// Notas:
// - Si el flag de ejecucion de scripts no esta activo al comenzar una iteracion,
//   se abandonara inmediatamente el metodo.
// - Solo se notificara si el cliente NO ha solicitado eliminar sus scripts
//   asociados.
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::UpdatePausedScripts(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Recorre la lista
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  PausedScriptsListIt It(m_PausedScripts.begin());
  while (It != m_PausedScripts.end()) {	
	// ¿NO esta activo el flag de ejecucion de scripts?
	if (!m_bScriptExecution) {
	  break;
	}

	// ¿El script pertenece al area actual?
	if (pWorld->GetIDArea() == (*It)->uwIDArea) {
	  // ¿Esta esperando por continuar ejecucion?
	  bool bResult = false;
	  if ((*It)->Script.GetState() == ScriptDefs::SS_RESUME) {
		// Continua con la ejecucion
		//SYSEngine::GetLogger()->Write("Ejecutando despues de PAUSA...{\n");
		bResult = (*It)->Script.Execute();
		//SYSEngine::GetLogger()->Write("}\n");
	  }

	  // Evalua estado
	  switch((*It)->Script.GetState()) {
	    case ScriptDefs::SS_PAUSED: {
	  	  // El script ha vuelto a quedar en modo pausa, se pasa al sig.
		  //SYSEngine::GetLogger()->Write("sig en PAUSA...\n");
		  ++It;
		} break;

		case ScriptDefs::SS_STOPPED: {
		  // Script detenido.
		  // Nota: En el este caso, habra notificacion.
		  delete *It;
		  It = m_PausedScripts.erase(It);			
		} break;

		case ScriptDefs::SS_INACTIVE: {
		  // El script ha finalizado, se notifica y borra el nodo 
		  if (!IsClientToBeRelease((*It)->pClient)) {
		    (*It)->pClient->ScriptNotify((*It)->Script.GetEvent(), 
									     ScriptClientDefs::SN_SCRIPT_EXECUTED, 
										 (*It)->udClientParams + (bResult ? 1 : 0));
		  }
		  delete *It;
		  It = m_PausedScripts.erase(It);
		  //SYSEngine::GetLogger()->Write("Fin...\n");
		} break;

		case ScriptDefs::SS_ERROR: {
		  // Hubo un error ejecutando el script, se notifica y borra el nodo
		  if (!IsClientToBeRelease((*It)->pClient)) {
			(*It)->pClient->ScriptNotify((*It)->Script.GetEvent(), 
										 ScriptClientDefs::SN_SCRIPT_ERROR, 
										 (*It)->udClientParams);
		  }
		  delete *It;
		  It = m_PausedScripts.erase(It);
		  //SYSEngine::GetLogger()->Write("Inactivo...\n");
		} break;

		default:
		  ASSERT(false);
		}; // ~ switch	  
	} else {
	  // El script ya no pertenece al area actual, se eliminara
	  delete *It;
	  It = m_PausedScripts.erase(It);
	}
  } // ~ while
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre la lista de scripts solicitados para ejecucion y
//   procede a ejecutarlos. En caso de que a la vuelta NO hallan acabado por
//   una causa de pausa, se depositara en la lista de scripts en pausa.
// - En el caso de que un script solicitado se halle en estado de pausa,
//   no sera tenido en cuenta hasta que el flag de pausa sea eliminado.
// Parametros:
// Devuelve:
// Notas:
// - Si el flag de ejecucion de scripts no esta activo al comenzar una iteracion,
//   se abandonara inmediatamente el metodo.
// - Solo se notificara si el cliente NO ha solicitado eliminar sus scripts
//   asociados.
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::UpdatePendingScripts(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Recorre la lista
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  ScriptsToExecuteListIt It(m_ScriptsToExecute.begin());
  while (It != m_ScriptsToExecute.end()) {
	// ¿NO esta activo el flag de ejecucion de scripts?
	if (!m_bScriptExecution) {
	  break;
	}

	// ¿El script pertenece al area actual?
	if (pWorld->GetIDArea() == (*It)->uwIDArea) {	
	  // Si, crea nodo
	  CVirtualMachine::sScript* pScript = new CVirtualMachine::sScript((*It)->pClient,
																	   (*It)->udClientParams,
																	   (*It)->uwIDArea);
	  ASSERT(pScript);

	  // Inicializa script
	  if (pScript->Script.Init((*It)->szScript, (*It)->Event, &m_GlobalScript)) {	  
		// Ejecuta
		const bool bResult = pScript->Script.Execute((*It)->Params);

		// Evalua estado
		switch(pScript->Script.GetState()) {
		  case ScriptDefs::SS_PAUSED: {
			// El script ha quedado en modo pausa, se inserta en lista
			m_PausedScripts.push_back(pScript);
	  		pScript = NULL;	
		  } break;

		  case ScriptDefs::SS_INACTIVE: {
			// El script ha finalizado, se notifica y borra el nodo
			if (!IsClientToBeRelease((*It)->pClient)) {
			  (*It)->pClient->ScriptNotify((*It)->Event, 
										   ScriptClientDefs::SN_SCRIPT_EXECUTED, 
										   (*It)->udClientParams + (bResult ? 1 : 0));
			}
		  } break;

		  case ScriptDefs::SS_ERROR: {
			// Hubo un error ejecutando el script, se notifica y borra el nodo
			if (!IsClientToBeRelease((*It)->pClient)) {
			  (*It)->pClient->ScriptNotify((*It)->Event, 
										   ScriptClientDefs::SN_SCRIPT_ERROR, 
										   (*It)->udClientParams);
			}
		  } break;

		  case ScriptDefs::SS_STOPPED: {
			// El script detiene de forma incondicional su ejecucion
			// Nota: En este caso NO habra notificacion			
		  } break;	

		  default:
			ASSERT(false);
		}; // ~ switch	  
	  } else {
		// El script no existe, se notifica
		// Nota: Cuando no se halla un script, se entendera que se realice la
		// accion por defecto (+1)
		if (!IsClientToBeRelease((*It)->pClient)) {
		  (*It)->pClient->ScriptNotify((*It)->Event, 
									   ScriptClientDefs::SN_SCRIPT_NO_FOUND, 
									   (*It)->udClientParams + 1);	  	 
		}
	  }

	  // Borra script si procede
	  if (pScript) {
		// Si, se borra
		delete pScript;
		pScript = NULL;
	  }
	} // ~ if
	
	// Sig. solicitud
	delete *It;
	It = m_ScriptsToExecute.erase(It);	
  }  // ~ while
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Deposita solicitud de ejecucion del evento asociado al inicio del juego.
// Parametros:
// - szScript. Nombre del script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnStartGameEvent(iCScriptClient* const pClient,
								  const std::string& szScript)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONSTARTGAME);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Deposita solicitud de ejecucion del evento de pulsacion sobre el panel
//   horario.
// Parametros:
// - szScript. Nombre del script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnClickHourPanelEvent(iCScriptClient* const pClient,
									   const std::string& szScript)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONCLICKHOURPANEL);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Deposita solicitud de evento asociado a la huida de un combate.
// Parametros:
// - szScript. Nombre del script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnFleeCombatEvent(iCScriptClient* const pClient,
								   const std::string& szScript)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONFLEECOMBAT);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Deposita solicitud de evento asociado a la pulsacion de una tecla.
// Parametros:
// - szScript. Nombre del script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnKeyPressedEvent(iCScriptClient* const pClient,
								   const std::string& szScript)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONKEYPRESSED);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Deposita solicitud de evento asociado al inicio del modo combate
// Parametros:
// - szScript. Nombre del script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnStartCombatMode(iCScriptClient* const pClient,
								   const std::string& szScript)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONSTARTCOMBATMODE);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Deposita solicitud de evento asociado al fin del modo combate
// Parametros:
// - szScript. Nombre del script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnEndCombatMode(iCScriptClient* const pClient,
								 const std::string& szScript)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONENDCOMBATMODE);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Deposita solicitud de evento asociado a nueva hora.
// Parametros:
// - szScript. Nombre del script.
// - ubHour. Hora en la que ha sucedido
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnNewHourEvent(iCScriptClient* const pClient,
								const std::string& szScript,
								const byte ubHour)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(float(ubHour)));

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONNEWHOUR, ParamList);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Deposita solicitud de evento asociado a la entrada en un area.
// Parametros:
// - szScript. Nombre del script.
// - uwIDArea. Identificador del area.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnEnterInArea(iCScriptClient* const pClient,
							   const std::string& szScript,
							   const word uwIDArea)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(float(uwIDArea)));
  
  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONENTERINAREA, ParamList);
} 

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Deposita solicitud de evento asociado a Idle del universo de juego.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnWorldIdleEvent(iCScriptClient* const pClient,
								  const std::string& szScript)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);
  
  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONWORLDIDLE);
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Deposita solicitud de evento asociado al establecimiento de una entidad
//   sobre un floor.
// Parametros:
// - szScript. Nombre del script.
// - XPos. Posicion en X.
// - YPos. Posicion en Y.
// - hTheEntity. La entidad que se ha depositado / establecido.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnSetInFloor(iCScriptClient* const pClient,
							  const std::string& szScript,
							  const AreaDefs::TilePosType& XPos,
							  const AreaDefs::TilePosType& YPos,
							  const AreaDefs::EntHandle& hTheEntity)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(float(XPos)));
  ParamList.push_back(ScriptDefs::sScriptParam(float(YPos)));
  ParamList.push_back(ScriptDefs::sScriptParam(hTheEntity));

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONSETIN, ParamList);
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Deposita solicitud de evento asociado a la accion de quitar una entidad.
// Parametros:
// - szScript. Nombre del script.
// - XPos. Posicion en X.
// - YPos. Posicion en Y.
// - hTheEntity. La entidad que se ha quitado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnSetOutFloor(iCScriptClient* const pClient,
							   const std::string& szScript,
							   const AreaDefs::TilePosType& XPos,
							   const AreaDefs::TilePosType& YPos,
							   const AreaDefs::EntHandle& hTheEntity)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(float(XPos)));
  ParamList.push_back(ScriptDefs::sScriptParam(float(YPos)));
  ParamList.push_back(ScriptDefs::sScriptParam(hTheEntity));

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONSETOUT, ParamList);
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Deposita solicitud de evento relacionado con coger un item.
// Parametros:
// - szScript. Nombre del script.
// - hTheItem. El item que se ha cogido.
// - hTheCriature. La criatura que ha realizado la accion.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnGetItem(iCScriptClient* const pClient,
						   const std::string& szScript,
						   const AreaDefs::EntHandle& hTheItem,
						   const AreaDefs::EntHandle& hTheCriature)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheItem));
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriature));

  // Se configura los parametros del cliente, que tendra en la parte
  // alta el handle del item a recoger (handle == word)
  const dword udClientParams = hTheItem << 16;

  // Se inserta
  InsertScriptToExecute(pClient, 
						szScript,
						RulesDefs::SE_ONGET, 
						ParamList,
						udClientParams);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Deposita solicitud de evento de depositar un item.
// Parametros:
// - szScript. Nombre del script.
// - hTheItem. El item ha depositar.
// - hTheCriature. La criatura que realiza la accion.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnDropItem(iCScriptClient* const pClient,
							const std::string& szScript,
							const AreaDefs::EntHandle& hTheItem,
							const AreaDefs::EntHandle& hTheCriature)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheItem));
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriature));

  // Se configura los parametros del cliente, que tendra en la parte
  // alta el handle del item a soltar (handle == word)
  const dword udClientParams = hTheItem << 16;
  
  // Se inserta
  InsertScriptToExecute(pClient, 
					    szScript, 
						RulesDefs::SE_ONDROP, 
						ParamList,
						udClientParams);
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Solicitud de evento relacionado con la observacion de una entidad.
// Parametros:
// - szScript. Nombre del script.
// - hTheEntity. La entidad que es observada.
// - hTheCriaure. La criatura que realiza la observacion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnObserveEntity(iCScriptClient* const pClient,
								 const std::string& szScript,
								 const AreaDefs::EntHandle& hTheEntity,
								 const AreaDefs::EntHandle& hTheCriature)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheEntity));
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriature));

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONOBSERVE, ParamList);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Solicitud de evento de hablar a una entidad.
// Parametros:
// - szScript. Nombre del script.
// - hTheEntity. Entidad a la que hablamos
// - hTheCriature. Criatura que realiza la accion de hablar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnTalkToEntity(iCScriptClient* const pClient,
								const std::string& szScript,
								const AreaDefs::EntHandle& hTheEntity,
								const AreaDefs::EntHandle& hTheCriature)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheEntity));
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriature));

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONTALK, ParamList);
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Solicitud de evento relacionado con la manipulacion de una entidad por
//   parte de una criatura.
// Parametros:
// - szScript. Nombre del script.
// - hTheEntity. Entidad manipulada.
// - hTheCriature. Criatura que realiza la accion de manipular.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnManipulateEntity(iCScriptClient* const pClient,
									const std::string& szScript,
									const AreaDefs::EntHandle& hTheEntity,
									const AreaDefs::EntHandle& hTheCriature)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheEntity));
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriature));
    
  // Se configura los parametros del cliente, que tendra en la parte
  // alta el handle de la entidad a manipular
  const dword udClientParams = hTheEntity << 16;

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONMANIPULATE, ParamList, udClientParams);
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Solicitud de evento relacionado con la muerte de una criatura.
// Parametros:
// - szScript. Nombre del script.
// - hTheCriature. Criatura que resulta muerta
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnDeath(iCScriptClient* const pClient,
						 const std::string& szScript,
						 const AreaDefs::EntHandle& hTheCriature)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriature));

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONDEATH, ParamList);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Solicitud del evento relacionado con la resurreccion de una criatura.
// Parametros:
// - szScript. El nombre del script.
// - hTheCriature. La criatura que muere.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnResurrect(iCScriptClient* const pClient,
							 const std::string& szScript,
							 const AreaDefs::EntHandle& hTheCriature)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriature));

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONRESURRECT, ParamList);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Solicitud del evento relacionado con el equipamiento de la criatura.
// Parametros:
// - szScript. Nombre del script.
// - hTheItem. El item equipado.
// - hTheCriature. La criatura que se equipa el item.
// - Slot. El slot donde se equipa el item.
// Devuelve:
// Notas:
// - Se añadira una unidad al valor del slot, pues debera de estar entre 1 y
//   el numero maximo de slots.
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnEquippedItem(iCScriptClient* const pClient,
								const std::string& szScript,
								const AreaDefs::EntHandle& hTheItem,
								const AreaDefs::EntHandle& hTheCriature,
								const RulesDefs::eIDEquipmentSlot& Slot)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea parametros para cliente
  const dword udClientParams = (hTheItem << 16);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheItem));
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriature));
  ParamList.push_back(ScriptDefs::sScriptParam(float(Slot - 1)));

  // Se inserta
  InsertScriptToExecute(pClient, 
						szScript, 
						RulesDefs::SE_ONINSERTINEQUIPMENTSLOT, 
						ParamList,
						udClientParams);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Solicitud del evento relacionado con la accion de quitar un item equipado
//   de un slot.
// Parametros:
// - szScript. Nombre del script.
// - hTheItem. Handle al item a quitar.
// - hTheCriature. Handle a la criatura que se quiere quitar el item.
// - Slot. Slot de equipamiento en donde estara el item.
// Devuelve:
// Notas:
// - Se añadira una unidad al valor del slot, pues debera de estar entre 1 y
//   el numero maximo de slots.
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnRemoveEquippedItem(iCScriptClient* const pClient,
									  const std::string& szScript,
									  const AreaDefs::EntHandle& hTheItem,
									  const AreaDefs::EntHandle& hTheCriature,
									  const RulesDefs::eIDEquipmentSlot& Slot)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea parametros para cliente
  const dword udClientParams = (hTheItem << 16);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheItem));
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriature));
  ParamList.push_back(ScriptDefs::sScriptParam(float(Slot - 1)));

  // Se inserta
  InsertScriptToExecute(pClient, 
						szScript, 
						RulesDefs::SE_ONREMOVEFROMEQUIPMENTSLOT, 
						ParamList,
						udClientParams);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Solicitud del evento relacionado con el uso de una habilidad.
// Parametros:
// - szScript. Nombre del script.
// - hTheCriature. Handle a la criatura.
// - Hability. Habilidad a utilizar.
// - hTheEntity. La entidad sobre la que se actua.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnUseHability(iCScriptClient* const pClient,
							   const std::string& szScript,
							   const AreaDefs::EntHandle& hTheCriature,
							   const RulesDefs::eIDHability& Hability,
							   const AreaDefs::EntHandle& hTheEntity)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Parametros del cliente
  const dword udClientParam = Hability << 16;	

  // Se pasa el identificador de habilidad a formato numerico
  dword udIDHability = Hability;
  byte ubHability = 1;
  while (udIDHability) {
	udIDHability >>= 1;
	++ubHability;
  }

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriature));
  ParamList.push_back(ScriptDefs::sScriptParam(float(ubHability)));  
  ParamList.push_back(ScriptDefs::sScriptParam(hTheEntity));

  // Se inserta
  InsertScriptToExecute(pClient, 
						szScript, 
						RulesDefs::SE_ONUSEHABILITY, 
						ParamList,
						udClientParam);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Evento relacioado con la activacion de un sintoma en una criatura.
// Parametros:
// - szScript. Nombre del script.
// - hTheCriature. Handle a la criatura sobre la que se activa el sintoma.
// - Symptom. ID al sintoma activado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnActivatedSymptom(iCScriptClient* const pClient,
									const std::string& szScript,
									const AreaDefs::EntHandle& hTheCriature,
									const RulesDefs::eIDSymptom& Symptom)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Parametros del cliente
  const dword udClientParams = (Symptom << 16);

  // Se pasa el identificador de habilidad a formato numerico
  dword udIDSymptom = Symptom;
  byte ubSymptom = 1;
  while (udIDSymptom) {
	udIDSymptom >>= 1;
	++ubSymptom;
  }

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriature));
  ParamList.push_back(ScriptDefs::sScriptParam(float(ubSymptom)));  

  // Se inserta
  InsertScriptToExecute(pClient, 
						szScript, 
						RulesDefs::SE_ONACTIVATEDSYMPTOM, 
						ParamList,
						udClientParams);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Solicitud de evento relacionado con la desactivacion de un sintoma.
// Parametros:
// - szScript. Nombre del script.
// - hTheCriature. Handle a la criatura sobre la que se desactiva evento.
// - Symptom. Sintoma desactivado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnDesactivatedSymptom(iCScriptClient* const pClient,
									   const std::string& szScript,
									   const AreaDefs::EntHandle& hTheCriature,
									   const RulesDefs::eIDSymptom& Symptom)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Parametros del cliente
  const dword udClientParams = (Symptom << 16);

  // Se pasa el identificador de habilidad a formato numerico
  dword udIDSymptom = Symptom;
  byte ubSymptom = 1;
  while (udIDSymptom) {
	udIDSymptom >>= 1;
	++ubSymptom;
  }

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriature));
  ParamList.push_back(ScriptDefs::sScriptParam(float(ubSymptom)));  

  // Se inserta
  InsertScriptToExecute(pClient, 
						szScript, 
						RulesDefs::SE_ONDEACTIVATEDSYMPTOM, 
						ParamList,
						udClientParams);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Solicitud de evento relacionado con el golpeo a una entidad.
// Parametros:
// - szScript. Nombre del script.
// - hTheCriature. Handle a la criatura golpeada.
// - Slot. Slot donde esta el item (o mano) con la que se golpea.
// - hTheEntity. Entidad que recibe la accion.
// Devuelve:
// Notas:
// - Se añadira una unidad al valor del slot, pues debera de estar entre 1 y
//   el numero maximo de slots.
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnHitEntity(iCScriptClient* const pClient,
							 const std::string& szScript,
							 const AreaDefs::EntHandle& hTheCriature,
							 const RulesDefs::eIDEquipmentSlot& Slot,
							 const AreaDefs::EntHandle& hTheEntity)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Parametros para el cliente
  const dword udClientParams = (Slot << 16);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriature));
  ParamList.push_back(ScriptDefs::sScriptParam(float(Slot)));  
  ParamList.push_back(ScriptDefs::sScriptParam(hTheEntity));

  // Se inserta
  InsertScriptToExecute(pClient, 
						szScript, 
						RulesDefs::SE_ONHIT, 
						ParamList,
						udClientParams);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Solicita evento de inicio de combate.
// Parametros:
// - szScript. Nombre del script.
// - hTheCriature. Handle a la criatura
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnStartCombatTurn(iCScriptClient* const pClient,
								   const std::string& szScript,
								   const AreaDefs::EntHandle& hTheCriature)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriature));

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONSTARTCOMBATTURN, ParamList);
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Solicitud del evento relacionado con el script Idle de la criatura.
// Parametros:
// - szScript. Nombre del script.
// - hTheEntity. Handle a la criatura.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnEntityIdle(iCScriptClient* const pClient,
							  const std::string& szScript,
							  const AreaDefs::EntHandle& hTheEntity)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheEntity));

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONENTITYIDLE, ParamList);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Solicitud del evento relacionado con el script sobre creacion de una
//   entidad.
// Parametros:
// - szScript. Nombre del script.
// - hTheEntity. Handle a la criatura.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnEntityCreated(iCScriptClient* const pClient,
								 const std::string& szScript,
								 const AreaDefs::EntHandle& hTheEntity)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheEntity));

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONENTITYCREATED, ParamList);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Solicitud del evento relacionado con el script de uso de un item sobre
//   otra entidad.
// Parametros:
// - szScript. Nombre del script.
// - hTheItem. Handle a la entidad que se usa
// - hTheEntity. Handle a la entidad SOBRE la que se usa el item.
// - hTheEntity. Handle a la criatura que lo usa
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnUseItem(iCScriptClient* const pClient,
						   const std::string& szScript,
						   const AreaDefs::EntHandle& hTheItem,
						   const AreaDefs::EntHandle& hTheEntity,
						   const AreaDefs::EntHandle& hTheCriature)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheItem));
  ParamList.push_back(ScriptDefs::sScriptParam(hTheEntity));
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriature));

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONUSEITEM, ParamList);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Solicitud del evento relacionado con el script de intercambio de un
//   item entre un contenedor fuente a un contenedor destino
// Parametros:
// - szScript. Nombre del script.
// - hTheItem. Handle al item que se quiere 
// - hTheEntitySrc. Handle a la criatura origen
// - hTheEntityDest. Handle a la criatura destino
// Devuelve:
// Notas:
// - El parametro incluira en la parte alta el handle al item a intercambiar
//   pero en la parte baja, en el primer byte, se incluira un valor de 1 si
//   la entidad destino es el jugador y un 0 si la entidad destino NO es el
//   jugador. Esto es asi, porque este evento correspondera a la accion de
//   intercambio de items entre el jugador y otra entidad contenedora.
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnTradeItem(iCScriptClient* const pClient,
							 const std::string& szScript,
							 const AreaDefs::EntHandle& hTheItem,
							 const AreaDefs::EntHandle& hTheEntitySrc,
							 const AreaDefs::EntHandle& hTheEntityDest)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Se configura los parametros del cliente,
  // Se inserta handle al item y en caso de que la entidad destino sea
  // el jugador, se pondra un valor '1' en el byte alto del word bajo.
  dword udClientParams = hTheItem << 16;						  
  if (hTheEntityDest == SYSEngine::GetWorld()->GetPlayer()->GetHandle()) {
	udClientParams += 0x00000100;
  } 

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheItem));
  ParamList.push_back(ScriptDefs::sScriptParam(hTheEntitySrc));
  ParamList.push_back(ScriptDefs::sScriptParam(hTheEntityDest));

  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONTRADEITEM, ParamList, udClientParams);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Solicitud del evento asociado a la entrada de una criatura en el rango
//   de otra. 
// Parametros:
// - pClient. El cliente al que notificar la finalizacion
// - szScript. Nombre del script.
// - TheCriatureWithRange. La criatura con el rango.
// - TheCriatureInRange. La criautura que entra en el rango de la otra
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnCriatureInRange(iCScriptClient* const pClient,
								   const std::string& szScript,
								   const AreaDefs::EntHandle& hTheCriatureWithRange,
								   const AreaDefs::EntHandle& hTheCriatureInRange)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriatureWithRange));
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriatureInRange));
 
  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONCRIATUREINRANGE, ParamList);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// Parametros:
// - pClient. El cliente al que notificar la finalizacion
// - szScript. Nombre del script.
// - TheCriatureWithRange. La criatura con el rango.
// - TheCriatureOutOfRange. La criautura que sale del rango de la otra
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CVirtualMachine::OnCriatureOutOfRange(iCScriptClient* const pClient,
									  const std::string& szScript,
									  const AreaDefs::EntHandle& hTheCriatureWithRange,
									  const AreaDefs::EntHandle& hTheCriatureOutOfRange)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);

  // Crea lista de parametros
  ScriptDefs::ScriptParamList ParamList;
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriatureWithRange));
  ParamList.push_back(ScriptDefs::sScriptParam(hTheCriatureOutOfRange));
 
  // Se inserta
  InsertScriptToExecute(pClient, szScript, RulesDefs::SE_ONCRIATUREOUTOFRANGE, ParamList);
}