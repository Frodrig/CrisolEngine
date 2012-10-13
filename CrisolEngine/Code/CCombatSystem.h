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
// CCombatSystem.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CCombatSystem
//
// Descripcion:
// - Implementa el modulo encargado de gestionar todo lo relacionado con el
//   sistema de combate. 
// - El combate estara organizado en base a dos conjuntos en donde las
//   criaturas se podran alinear. Por un lado estara el conjunto donde
//   se insertara el jugador (y todas aquellas criaturas que se consideren
//   amigas del mismo) y por otro lado, el conjunto donde se insertara 
//   aquellas criaturas que se consideren enemigas del jugador.
// - Un combate comenzara cuando resulte que haya criaturas tanto en el
//   conjunto del jugador como en el conjunto de enemigos del jugador. Cuando
//   el jugador este activo, siempre se hallara insertado en el conjunto suyo.
// - El combate siempre transcurria con el modo del universo de juego en
//   turnos.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CCOMBATSYSTEM_H_
#define _CCOMBATSYSTEM_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _ICCOMBATSYSTEM_H_
#include "iCCombatSystem.h"
#endif
#ifndef _ICCRIATUREOBSERVER_H_
#include "iCCriatureObserver.h"
#endif
#ifndef _ICSCRIPTCLIENT_H_
#include "iCScriptClient.h"
#endif
#ifndef _ICGUIMANAGER_H_
#include "iCGUIManager.h"
#endif
#ifndef _ICWORLDOBSERVER_H_
#include "iCWorldObserver.h"
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif

// Defincion de clases / estructuras / espacios de nombres
struct iCWorld;
struct iCGameDataBase;
struct iCEntitySelector;
struct iCVirtualMachine;
class CEntitySelector;

// Clase CCombatSystem
class CCombatSystem: public iCCombatSystem,
					 public iCCriatureObserver,
					 public iCScriptClient,
					 public iCWorldObserver
{ 
private:
  // Estructuras forward
  struct sNAlingmentInfo;

private:
  // Tipos
  // Conjunto para la alineacion de criaturas
  typedef std::map<AreaDefs::EntHandle, sNAlingmentInfo*> AlingmentMap;
  typedef AlingmentMap::iterator						  AlingmentMapIt;
  typedef AlingmentMap::value_type                        AlingmentMapValType;

private:
  // Estructuras
  struct sNCombatient { 
	// Nodo asociado a un combatiente en lista de combate
	AreaDefs::EntHandle hCriature; // Criatura combatiente
	// Constructor
	sNCombatient(const AreaDefs::EntHandle& ahCriature): hCriature(ahCriature) { }
	
	// Operadores
	bool operator==(const AreaDefs::EntHandle& ahCriature) const {
	  return (hCriature == ahCriature);
	}
	bool operator==(const sNCombatient& Combatient) const {
	  return (hCriature == Combatient.hCriature);
	}

  };

private:
  // Tipos
  // Combatientes en la lista de turnos de combate
  typedef std::list<sNCombatient>      CombatientTurnList;
  typedef CombatientTurnList::iterator CombatientTurnListIt;
  // Handles a criaturas (combatientes) que desean salirse del combate
  typedef std::list<AreaDefs::EntHandle>  CombatientLeavingList;
  typedef CombatientLeavingList::iterator CombatientLeavingListIt;

private:
  // Estructuras
  struct sAlingmentInfo {
	// Info asociada con la alineacion
	AlingmentMap PlayerMap;      // Alineacion con jugador
	AlingmentMap EnemyPlayerMap; // Alineacion en contra del jugador
	// Nombres de los archivos selectores
	std::string szPlayerAlingmentSelector;      // Selector para jugadores
	std::string szEnemyPlayerAlingmentSelector; // Selector para enemigos de jugador
  };

  struct sCombatTurnInfo {
	// Estructuras
	struct sCriatureInTurnInfo {
	  // Info asociada a la criatura en turno de combate
	  CombatientTurnListIt            CriatureIt;   // Iterador a la criatura con turno
	  RulesDefs::CriatureActionPoints ActionPoints; // Puntos de accion
	};
	// Info asociada a la lista de turnos de combate
	CombatientTurnList    TurnList;          // Lista de turnos de combate  	
	CombatientTurnList    WaitingForCombat;  // Lista de entidades esperando incorporacion
	CombatientLeavingList CombatientLeaving; // Combatientes deseando marcharse
	sCriatureInTurnInfo   CriatureInTurn;    // Info asociada a la criatura con turno
  };

private:    
  // Vbles de miembro  
  static CCombatSystem* m_pCombatSys; // Unica instancia a la clase

  // Interfaces a otros modulos
  iCWorld*          m_pWorld;      // Universo 
  iCGameDataBase*   m_pGDBase;     // Base de datos del juego
  iCGUIManager*     m_pGUIManager; // Manager de interfaces
  iCVirtualMachine* m_pVMachine;   // Maquina virtual

  // Info concreta de combate
  sAlingmentInfo  m_AlingmentInfo;        // Info de alineacion de combate
  sCombatTurnInfo m_CombatTurnInfo;       // Info asociada a los turnos de combate
  bool            m_bIsCombatActive;      // ¿Hay combate activo?
  bool            m_bActionPointsBlocked; // ¿Puntos de accion bloqueados?

  // Resto de vbles de miembro  
  bool m_bIsInitOk; // ¿Instancia inicializada?
  
protected:
  // Constructor / Destructor
  CCombatSystem(void): m_pWorld(NULL),
					   m_pGDBase(NULL),
					   m_pGUIManager(NULL),
					   m_pVMachine(NULL),
					   m_bIsCombatActive(false),
					   m_bIsInitOk(false),
					   m_bActionPointsBlocked(false) { }
public:
  ~CCombatSystem(void) { End(); }
  
public:
  // Protocolo de inicio y fin de instancia
  bool Init(void);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }
private:
  // Metodos de apoyo
  void RemoveAlingment(AlingmentMap& Map);

public:
  // Obtencion y destruccion de la instancia singlenton
  static inline CCombatSystem* const GetInstance(void) {
    if (NULL == m_pCombatSys) { 
      // Se crea la instancia
      m_pCombatSys = new CCombatSystem; 
      ASSERT(m_pCombatSys)
    }
    return m_pCombatSys;
  }
  static inline DestroyInstance(void) {
    if (m_pCombatSys) {
      delete m_pCombatSys;
      m_pCombatSys = NULL;
    }
  }

public:
  // Actualizacion del del estado del subsistema
  void Update(void);
private:
  // Metodo de apoyo
  void StartCombat(void);
  void SetActCriatureInTurn(void);

public:
  // iCCombatSystem / Trabajo con metodos generales de combate  
  void EndCombat(void);
  RulesDefs::CriatureActionPoints GetCriatureActionPoints(const AreaDefs::EntHandle& hCriature);
  AreaDefs::EntHandle GetCombatant(const CombatSystemDefs::eCombatAlingment& Alingment,
								   const word uwPos);
  void NextTurn(const AreaDefs::EntHandle& hCriature);
  AreaDefs::EntHandle GetCriatureInTurn(void) const;    
  bool IsCombatActive(void) const {
	ASSERT(IsInitOk());
	// Retorna flag
	return m_bIsCombatActive;
  }  

public:
  // iCCombatSystem / Trabajo en la alineacion de combate
  void SetAlingment(const AreaDefs::EntHandle& hCriature,
				    const CombatSystemDefs::eCombatAlingment& Alingment);
  bool SetAlingmentWith(const AreaDefs::EntHandle& hCriature,
						const AreaDefs::EntHandle& hCriatureFriend);
  bool SetAlingmentAgainst(const AreaDefs::EntHandle& hCriature,
						   const AreaDefs::EntHandle& hCriatureEnemy);  
  CombatSystemDefs::eCombatAlingment GetAlingment(const AreaDefs::EntHandle& hCriature);
  word GetNumberOfCombatants(const CombatSystemDefs::eCombatAlingment& Alingment) const;
private:
  // Metodos de apoyo
  bool IsSomeCriatureAlive(AlingmentMap& Map);
  bool IsSomeCriatureInRange(const AreaDefs::EntHandle& hCriature,
							 AlingmentMap& Map);
  inline AlingmentMap* GetAlingmentMap(const CombatSystemDefs::eCombatAlingment& AlingmentMap) {
	ASSERT(IsInitOk());
	ASSERT((CombatSystemDefs::NO_ALINGMENT != AlingmentMap) != 0);
	// Retonra la direccion del conjunto que proceda
	if (AlingmentMap == CombatSystemDefs::PLAYER_ALINGMENT) {
	  // Amigos del jugador
	  return &m_AlingmentInfo.PlayerMap;
	} else {
	  // Seguro es el de enemigos del jugador
	  return &m_AlingmentInfo.EnemyPlayerMap;	
	}
  }
  inline bool IsCombatCondition(void) {
	ASSERT(IsInitOk());
	// Comprueba si se cumple la condicion de combate
	// La condicion de combate implica que en los conjuntos de alineacion de
	// combate exista al menos una criatura viva
	// y ademas que NO este activo el modo CutScene
	if (m_pGUIManager->IsCutSceneActive() ||
	    m_AlingmentInfo.PlayerMap.empty() ||
		m_AlingmentInfo.EnemyPlayerMap.empty()) {
	  return false;
	} else {
	  return true;
	}	
  }  

public:
  // iCCombatSystem / Obtencion del selector de combate
  iCEntitySelector* GetCombatientSelector(const AreaDefs::EntHandle& hCriature);  
private:
  // Metodos de apoyo
  CEntitySelector* GetCombatSelector(const AreaDefs::EntHandle& hCriature);

private:
  // Trabajo con las listas de combate para turnos
  void InsertInCombatTurnList(const AreaDefs::EntHandle& hCriature);
  inline InsertInWaitingForCombatTurnList(const AreaDefs::EntHandle& hCriature) {
	ASSERT(IsInitOk());
	ASSERT_MSG(IsValidCombatient(hCriature), "InsertInWaitingForCombatTurnList");
	// Inserta en la lista de espera para ingreso en lista principal
	m_CombatTurnInfo.WaitingForCombat.push_back(hCriature);
  }
  void MoveInWaitingToCombatTurnList(void);
  void RemoveFromCombatTurnList(const AreaDefs::EntHandle& hCriature);

private:
  // Operacion de notificacion para criaturas
  void CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
							  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
							  const dword udParam);
private:
  // Metodos de apoyo generales
  bool IsValidCombatient(const AreaDefs::EntHandle& hCriature);

public:
  // Bloqueo / desbloqueo de la contabilidad de acciones realizadas
  void BlockUseOfActionPoints(const bool bBlock);

public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);

public:
  // iCScriptClient / Operacion de notificacion, para cuando acabe un comando
  void ScriptNotify(const RulesDefs::eScriptEvents& ScriptEvent,
					const ScriptClientDefs::eScriptNotify& Notify,							
					const dword udParams);  

};

#endif // ~ CCombatSystem
