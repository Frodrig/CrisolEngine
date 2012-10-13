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
// CVirtualMachine.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clase:
// - CVirtualMachine
//
// Descripcion:
// - Maquina virtual del motor que gestionara la interpretacion del codigo
//   script que se haya asociado a los distintos eventos..
//
// Notas:
// - A la hora de ejecutar los eventos, se supondra que todas las entidades
//   llamara automaticamente a los metodos aunque no tengan ningun script 
//   asociado, dejando la responsabilidad de comprobar tal hecho en esta
//   clase.
// - El flag de ejecucion de script servira para deshabilitar completamente
//   la posibilidad de ejecutar scripts. No se deseara ejecutar scripts
//   desde los estados que no sean de juego y, ademas, cuando se halle 
//   activa la ventana de menu de juego.
// - A la hora de eliminar los scripts de un cliente, se insertara la 
//   direccion del cliente en una lista que se antendera cuando se comiencen
//   a ejecutar las solicitudes de ejecucion de scripts, para evitar
//   efectos laterales (borrar un CScript cuando se esta ejecutando).
///////////////////////////////////////////////////////////////////////////////
#ifndef _CVIRTUALMACHINE_H_
#define _CVIRTUALMACHINE_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _SYSENGINE_H_
#include "SYSEngine.h"
#endif
#ifndef _ICVIRTUALMACHINE_H_
#include "iCVirtualMachine.h"
#endif
#ifndef _ICWORLD_H_
#include "iCWorld.h"
#endif
#ifndef _SCRIPTDEFS_H_
#include "ScriptDefs.h"
#endif
#ifndef _RULESDEFS_H_
#include "RulesDefs.h"
#endif
#ifndef _CSCRIPT_H_
#include "CScript.h"
#endif
#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_
#include <algorithm>
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif
#ifndef _SET_H_
#define _SET_H_
#include <set>
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif

// Defincion de clases / estructuras / espacios de nombres
struct iCScriptClient;

// Clase CVirtualMachine
class CVirtualMachine: public iCVirtualMachine
{
private:
  // Estructuras forward
  struct sScript;

private:
  // Estructuras
  struct sScriptToExecute {
	// Scripts pendientes de ejecucion
	iCScriptClient*             pClient;        // Client del script
	std::string                 szScript;       // Nombre del script
	RulesDefs::eScriptEvents    Event;          // Evento
	ScriptDefs::ScriptParamList Params;         // Parametros del script
	dword                       udClientParams; // Parametros del cliente
	word                        uwIDArea;       // Identificador del area
	// Constructor
	sScriptToExecute(iCScriptClient* const pScriptClient,
					 const std::string& szScriptName,
					 const RulesDefs::eScriptEvents& ScriptEvent,
					 const ScriptDefs::ScriptParamList& ScriptParams,
					 const dword audClientParams,
					 const word auwIDArea): pClient(pScriptClient),
										     szScript(szScriptName),
											 Event(ScriptEvent),
											 Params(ScriptParams),
											 udClientParams(audClientParams),
											 uwIDArea(auwIDArea) { }	
	
	// Pool de memoria
	static CMemoryPool m_MPool;
	static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
	static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 	
  };

  struct sScriptsToRelease {
	// Clientes que desean liberar sus scripts
	iCScriptClient*          pClient;     // Cliente
	bool                     bAllScripts; // ¿Todos los scripts?
	RulesDefs::eScriptEvents Event;       // Evento, SOLO si NO son todos los scripts

	// Constructores
	sScriptsToRelease(iCScriptClient* const apClient): pClient(apClient),
													   bAllScripts(true) { }
	sScriptsToRelease(iCScriptClient* const apClient,
					  const RulesDefs::eScriptEvents& aEvent): pClient(apClient),
													           Event(aEvent),
														       bAllScripts(false) { }

	// Operadores
	// Comparacion
	bool operator==(iCScriptClient* const apClient) const {
	  // Retorna resultado
	  return (pClient == apClient);
	}
	// Asignacion
	sScriptsToRelease& operator=(const sScriptsToRelease& ScriptsToRelease) {
	  pClient = ScriptsToRelease.pClient;
	  bAllScripts = ScriptsToRelease.bAllScripts;
	  Event = ScriptsToRelease.Event;
	}
  };

private:
  // Tipos
  // Lista de scripts pendientes a comenzar ejecucion
  typedef std::list<sScriptToExecute*>   ScriptsToExecuteList;
  typedef ScriptsToExecuteList::iterator ScriptsToExecuteListIt;  
  // Lista de scripts en pausa
  typedef std::list<sScript*>         PausedScriptsList;
  typedef PausedScriptsList::iterator PausedScriptsListIt;
  // Lista de clientes a ser liberados
  typedef std::list<sScriptsToRelease> ReleaseClientList;
  typedef ReleaseClientList::iterator  ReleaseClientListIt;
  
private:
  // Instancia singlenton
  static CVirtualMachine* m_pVirtualMachine; // Unica instancia a la clase

private:
  // Vbles de miembro 
  iCGameDataBase*      m_pGDBase;          // Base de datos del juego
  CScript              m_GlobalScript;     // Script global 
  ScriptsToExecuteList m_ScriptsToExecute; // Lista de scripts a ejecutar
  PausedScriptsList    m_PausedScripts;    // Lista de scripts pausados
  ReleaseClientList    m_ReleaseClients;   // Lista de clientes scripts a borrar
  bool                 m_bScriptExecution; // Flag de ejecuion de scripts
  bool                 m_bInUpdateMode;    // ¿Actualizando los scripts?
  bool				   m_bIsInitOk;        // ¿Clase inicializada correctamente?     
  
protected:
  // Constructor / Destructor
  CVirtualMachine(void): m_pGDBase(NULL),
						 m_bIsInitOk(false) { }
public:
  ~CVirtualMachine(void) { 
    End();
  }               
  
public:
  // Protocolo de inicio y fin de instancia
  bool Init(void);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // Obtencion y destruccion de la instancia singlenton
  static inline CVirtualMachine* const GetInstance(void) {
    if (!m_pVirtualMachine) {
      m_pVirtualMachine = new CVirtualMachine; 
      ASSERT(m_pVirtualMachine)
    }
    return m_pVirtualMachine;
  } 
  static inline DestroyInstance(void) {
    if (m_pVirtualMachine) {
      delete m_pVirtualMachine;
      m_pVirtualMachine = NULL;
    }
  }

private:
  // iCVirtualMachine / Preparacion / finalizacion del entorno de ejecucion
  // Nota: Solo disponible a traves de CWorld.
  void PrepareScripts(void);  
  void PrepareScripts(const FileDefs::FileHandle& hFile,
					  dword& udOffset);
  void EndScripts(void);

public:
  // Actualizacion / ejecucion de los scripts
  void Update(void);
private:
  // Metodos de apoyo
  void UpdatePausedScripts(void);
  void UpdatePendingScripts(void);

public:
  // iCVirtualMachine / Trabajo con el flag de ejecucion de scripts
  void SetScriptExecution(const bool bSet) {
	ASSERT(IsInitOk());
	// Establece flag
	m_bScriptExecution = bSet;
  }

private:
  // iCVirtualMachine / Almacenamiento y recuperacion de los valores globales
  // Nota: Solo disponible a traves de CWorld.
  void SaveGlobals(const FileDefs::FileHandle& hFile,
				   dword& udOffset);
private:  
  void LoadGlobals(const FileDefs::FileHandle& hFile,
				   dword& udOffset);

public:
  // iCVirtualMachine / Liberacion de scripts asociados a un cliente
  void ReleaseScripts(iCScriptClient* const pClient);
  void ReleaseScripts(iCScriptClient* const pClient,
					  const RulesDefs::eScriptEvents& Event);
private:
  // Metodos de apoyo
  void UpdateReleaseScripts(void);
  void ReleaseScript(iCScriptClient* const pClient,
	  			     const RulesDefs::eScriptEvents& Event);
  void ReleaseScript(iCScriptClient* const pClient);
  inline bool IsClientToBeRelease(iCScriptClient* const pClient) {
	ASSERT(IsInitOk());
	// Comprueba si un cliente esta en la lista de clientes que han
	// solicitado eliminar sus scripts registrados
	return (std::find(m_ReleaseClients.begin(),
					  m_ReleaseClients.end(),
					  pClient) != m_ReleaseClients.end());
  }

public:
  // iCVirtualMachine / Metodos de ejecucion de los eventos scripts
  void OnStartGameEvent(iCScriptClient* const pClient,
						const std::string& szScript);
  void OnClickHourPanelEvent(iCScriptClient* const pClient,
							 const std::string& szScript);
  void OnFleeCombatEvent(iCScriptClient* const pClient,
						 const std::string& szScript);
  void OnKeyPressedEvent(iCScriptClient* const pClient,
						 const std::string& szScript);  
  void OnStartCombatMode(iCScriptClient* const pClient,
						 const std::string& szScript);
  void OnEndCombatMode(iCScriptClient* const pClient,
					   const std::string& szScript);
  void OnNewHourEvent(iCScriptClient* const pClient,
					  const std::string& szScript,
					  const byte ubHour);
  void OnEnterInArea(iCScriptClient* const pClient,
					 const std::string& szScript,
					 const word uwIDArea);  
  void OnWorldIdleEvent(iCScriptClient* const pClient,
						const std::string& szScript);
  void OnSetInFloor(iCScriptClient* const pClient,
					const std::string& szScript,
					const AreaDefs::TilePosType& XPos,
					const AreaDefs::TilePosType& YPos,
					const AreaDefs::EntHandle& hTheEntity);
  void OnSetOutFloor(iCScriptClient* const pClient,
					 const std::string& szScript,
					 const AreaDefs::TilePosType& XPos,
					 const AreaDefs::TilePosType& YPos,
					 const AreaDefs::EntHandle& hTheEntity);
  void OnGetItem(iCScriptClient* const pClient,
				 const std::string& szScript,
				 const AreaDefs::EntHandle& hTheItem,
				 const AreaDefs::EntHandle& hTheCriature);
  void OnDropItem(iCScriptClient* const pClient,
				  const std::string& szScript,
				  const AreaDefs::EntHandle& hTheItem,
				  const AreaDefs::EntHandle& hTheCriature);
  void OnObserveEntity(iCScriptClient* const pClient,
					   const std::string& szScript,
					   const AreaDefs::EntHandle& hTheEntity,
					   const AreaDefs::EntHandle& hTheCriature);
  void OnTalkToEntity(iCScriptClient* const pClient,
					  const std::string& szScript,
					  const AreaDefs::EntHandle& hTheEntity,
					  const AreaDefs::EntHandle& hTheCriature);
  void OnManipulateEntity(iCScriptClient* const pClient,
						  const std::string& szScript,
						  const AreaDefs::EntHandle& hTheEntity,
					      const AreaDefs::EntHandle& hTheCriature);
  void OnDeath(iCScriptClient* const pClient,
			   const std::string& szScript,
			   const AreaDefs::EntHandle& hTheCriature);
  void OnResurrect(iCScriptClient* const pClient, 
				   const std::string& szScript,
				   const AreaDefs::EntHandle& hTheCriature);
  void OnEquippedItem(iCScriptClient* const pClient,
					  const std::string& szScript,
					  const AreaDefs::EntHandle& hTheItem,
					  const AreaDefs::EntHandle& hTheCriature,
					  const RulesDefs::eIDEquipmentSlot& Slot);
  void OnRemoveEquippedItem(iCScriptClient* const pClient,
							const std::string& szScript,
							const AreaDefs::EntHandle& hTheItem,
							const AreaDefs::EntHandle& hTheCriature,
							const RulesDefs::eIDEquipmentSlot& Slot);
  void OnUseHability(iCScriptClient* const pClient,
					 const std::string& szScript,
					 const AreaDefs::EntHandle& hTheCriature,
					 const RulesDefs::eIDHability& Hability,
					 const AreaDefs::EntHandle& hTheEntity);
  void OnActivatedSymptom(iCScriptClient* const pClient,
						  const std::string& szScript,
						  const AreaDefs::EntHandle& hTheCriature,
						  const RulesDefs::eIDSymptom& Symptom);
  void OnDesactivatedSymptom(iCScriptClient* const pClient,
							 const std::string& szScript,
							 const AreaDefs::EntHandle& hTheCriature,
							 const RulesDefs::eIDSymptom& Symptom);
  void OnHitEntity(iCScriptClient* const pClient,
				   const std::string& szScript,
				   const AreaDefs::EntHandle& hTheCriature,
				   const RulesDefs::eIDEquipmentSlot& Slot,
				   const AreaDefs::EntHandle& hTheEntity);
  void OnStartCombatTurn(iCScriptClient* const pClient,
						 const std::string& szScript,
						 const AreaDefs::EntHandle& hTheCriature);
  void OnEntityIdle(iCScriptClient* const pClient,
					const std::string& szScript,
					const AreaDefs::EntHandle& hTheEntity);
  void OnUseItem(iCScriptClient* const pClient,
				 const std::string& szScript,
				 const AreaDefs::EntHandle& hTheItem,
				 const AreaDefs::EntHandle& hTheEntity,
				 const AreaDefs::EntHandle& hTheCriature);
  void OnTradeItem(iCScriptClient* const pClient,
				   const std::string& szScript,
				   const AreaDefs::EntHandle& hTheItem,
				   const AreaDefs::EntHandle& hTheEntitySrc,
				   const AreaDefs::EntHandle& hTheEntityDest);
  void OnEntityCreated(iCScriptClient* const pClient,
					   const std::string& szScript,
					   const AreaDefs::EntHandle& hTheEntity);
  void OnCriatureInRange(iCScriptClient* const pClient,
						 const std::string& szScript,
						 const AreaDefs::EntHandle& hTheCriatureWithRange,
						 const AreaDefs::EntHandle& hTheCriatureInRange);
  void OnCriatureOutOfRange(iCScriptClient* const pClient,
						    const std::string& szScript,
						    const AreaDefs::EntHandle& hTheCriatureWithRange,
						    const AreaDefs::EntHandle& hTheCriatureOutOfRange);

private:
  // Metodos de apoyo
  inline void InsertScriptToExecute(iCScriptClient* const pClient,
									const std::string& szFileName,	
									const RulesDefs::eScriptEvents& ScriptEvent,
									const dword udClientParams = 0) {
	ASSERT(IsInitOk());
	ASSERT(pClient);
	// Caso de scripts sin parametros
	const ScriptDefs::ScriptParamList EmptyParamList;
	InsertScriptToExecute(pClient, 
						  szFileName, 
						  ScriptEvent, 
						  EmptyParamList, 
						  udClientParams);
  }
  inline void InsertScriptToExecute(iCScriptClient* const pClient,
									const std::string& szFileName,	
									const RulesDefs::eScriptEvents& ScriptEvent,
									const ScriptDefs::ScriptParamList& ParamList,
									const dword udClientParams = 0) {
	ASSERT(IsInitOk());
	ASSERT(pClient);
	// Se crea nodo e inserta
	sScriptToExecute* const pScript = new sScriptToExecute(pClient,
														   szFileName, 
														   ScriptEvent, 
														   ParamList,
														   udClientParams,
														   SYSEngine::GetWorld()->GetIDArea());
	ASSERT(pScript);
	m_ScriptsToExecute.push_back(pScript);	
  }
};

#endif // ~ CVirtualMachine
