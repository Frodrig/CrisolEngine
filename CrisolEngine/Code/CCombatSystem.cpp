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
// CCombatSystem.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CCombatSystem.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CCombatSystem.h"

#include "SYSEngine.h"
#include "iCLogger.h"
#include "iCWorld.h"
#include "iCGameDataBase.h"
#include "iCGUIManager.h"
#include "iCVirtualMachine.h"
#include "CCBTEngineParser.h"
#include "CWorldEntity.h"
#include "CCriature.h"
#include "CItem.h"
#include "CPlayer.h"
#include "CEntitySelector.h"
#include "CMemoryPool.h"
#include "RulesDefs.h"

// Se inicializan los miembros static
CCombatSystem* CCombatSystem::m_pCombatSys = NULL;

// Declaracion de estructuras forward
struct CCombatSystem::sNAlingmentInfo {
  CEntitySelector CombatSelector; // Selector de combate de criatura
  // Manejador de memoria
  static CMemoryPool m_MPool;
  static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
  static void operator delete(void* pItem)  { m_MPool.FreeMem(pItem); } 
};

// Inicializacion de los manejadores de memoria
CMemoryPool 
CCombatSystem::sNAlingmentInfo::m_MPool(16,
                                        sizeof(CCombatSystem::sNAlingmentInfo),
                                        true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar
///////////////////////////////////////////////////////////////////////////////
bool 
CCombatSystem::Init(void)
{
  // ¿Se intenta reinicializar?
  if (IsInitOk()) {
	return false;
  }

  #ifdef ENGINE_TRACE
    SYSEngine::GetLogger()->Write("CCombatSystem::Init> Inicializando subsistema de combate.\n");
  #endif

  // Se toman instancias a otros modulos
  m_pWorld = SYSEngine::GetWorld();
  ASSERT(m_pWorld);
  m_pGDBase = SYSEngine::GetGameDataBase();
  ASSERT(m_pGDBase);
  m_pGUIManager = SYSEngine::GetGUIManager();
  ASSERT(m_pGUIManager);
  m_pVMachine = SYSEngine::GetVirtualMachine();
  ASSERT(m_pVMachine);

  // Se inicializan datos de alineacion
  CCBTEngineParser* const pParser = m_pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
														    "[Selectors]");
  ASSERT(pParser);
  pParser->SetVarPrefix("Combat.");
  m_AlingmentInfo.szPlayerAlingmentSelector = pParser->ReadStringID("PlayerAlingment.AnimTemplate");
  m_AlingmentInfo.szEnemyPlayerAlingmentSelector = pParser->ReadStringID("EnemyPlayerAlingment.AnimTemplate");

  // Se inicializan datos de combate
  m_CombatTurnInfo.CriatureInTurn.CriatureIt = m_CombatTurnInfo.TurnList.end();
  m_CombatTurnInfo.CriatureInTurn.ActionPoints = 0;

  // Se instala como observador del World
  m_pWorld->AddObserver(this);

  #ifdef ENGINE_TRACE
    SYSEngine::GetLogger()->Write("                   | Ok.\n");
  #endif

  // Todo correcto
  m_bIsInitOk = true;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCombatSystem::End(void)
{
  // Finaliza si procede
  if (IsInitOk()) {
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CCombatSystem::End> Finalizando subsistema de combate.\n");
	#endif

	// Finaliza posible combate
	EndCombat();

	// Libera cualquier tipo de script asociado
	m_pVMachine->ReleaseScripts(this);

	// Libera conjuntos de alineacion
	m_AlingmentInfo.PlayerMap.clear();
	m_AlingmentInfo.EnemyPlayerMap.clear();	

	// Se desinstala como observador del World
	m_pWorld->RemoveObserver(this);

	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("                  | Ok.\n");
	#endif
	
	  // Baja flag
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza el combate de tal forma que todas las criaturas que estuvieran
//   alineadas dejaran de estarlo a excepcion del jugador, que seguira estando
//   alineado en su conjunto de alineacion en combate.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCombatSystem::EndCombat(void)
{  
  // ¿Esta activo el modo combate?
  if (IsCombatActive()) {
	// Se toma la alineacion actual del jugador
	const CombatSystemDefs::eCombatAlingment PlayerAlingment = GetAlingment(m_pWorld->GetPlayer()->GetHandle());

	// Se procede a quitar la alineacion de todas las criaturas en combate
	// asi como la informacion relativa a la lista de turnos de combate
	RemoveAlingment(m_AlingmentInfo.PlayerMap);
	RemoveAlingment(m_AlingmentInfo.EnemyPlayerMap);
	m_CombatTurnInfo.TurnList.clear();
	m_CombatTurnInfo.WaitingForCombat.clear();
		
	// Se desvincula cualquier iterador de combate	
	m_CombatTurnInfo.CriatureInTurn.CriatureIt = m_CombatTurnInfo.TurnList.end();
	m_CombatTurnInfo.CriatureInTurn.ActionPoints = 0;

	// Se pasa el universo de juego a modo real
	m_pWorld->SetMode(WorldDefs::REAL_MODE);

	// Se pasa interfaz principal a modo real y se desactiva interfaz de combate
	m_pGUIManager->MainInterfazSetMode(WorldDefs::REAL_MODE);
	m_pGUIManager->SetCombatWindow(0);

	// Se baja flag
	m_bIsCombatActive = false;

	// ¿Estaba alineado el jugador?
	// Nota: es muy importante llamar despues de bajar el flag de combate
	if (PlayerAlingment) {
	  ASSERT((PlayerAlingment == CombatSystemDefs::PLAYER_ALINGMENT) != 0);
	  SetAlingment(m_pWorld->GetPlayer()->GetHandle(), PlayerAlingment);
	}

	// Notifica la finalizacion del modo combate
	ASSERT(SYSEngine::GetWorld()->GetPlayer());
	m_pVMachine->OnEndCombatMode(this, 
			                     m_pWorld->GetScriptEventFile(RulesDefs::SE_ONENDCOMBATMODE));	
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el handle a la criatura cuya alineacion sea Alingment y ocupe
//   la posicion uwPos dentro de dicha alineacion. Si la alineacion recibida
//   es nula, no se esta en modo combate o bien no se halla criatura alguna
//   en dicha posicion, se retornara el handle nulo.
// Parametros:
// - Alingment. Alineacion con la que trabajar.
// - uwPos. Posicion en donde buscar
// Devuelve:
// - El handle a la criatura o handle nulo si no se ha hallado.
// Notas:
///////////////////////////////////////////////////////////////////////////////
AreaDefs::EntHandle
CCombatSystem::GetCombatant(const CombatSystemDefs::eCombatAlingment& Alingment,
							const word uwPos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Se esta en modo combate y se ha recibido alineacion valida?
  if (IsCombatActive() &&
	  Alingment != CombatSystemDefs::NO_ALINGMENT) {
	// Se toma iterador donde buscar dependiendo de la alineacion
	word uwActPos = 0;
	CombatientTurnListIt It(m_CombatTurnInfo.TurnList.begin());
	for (; It != m_CombatTurnInfo.TurnList.end(); ++It) {
	  // ¿Alineacion coincide?
	  if (GetAlingment(It->hCriature) == Alingment) {
		// Se incrementa contador y se comprueba si se ha localizado
		if (++uwActPos == uwPos) {
		  // Localizado
		  return It->hCriature;
		} else if (uwActPos > uwPos) {
		  // Se rompe el bucle si sobrepasamos la posicion
		  break;
		}
	  }
	}
  }

  // No se pudo hallar entidad
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el numero de combatientes para una determinada alineacion. En caso
//   de alineacion no valida o que no se este en combate, el numero sera 0.
// Parametros:
// - Alingment. Alineacion con la que trabajar.
// Devuelve:
// - El handle a la criatura o handle nulo si no se ha hallado.
// Notas:
///////////////////////////////////////////////////////////////////////////////
word 
CCombatSystem::GetNumberOfCombatants(const CombatSystemDefs::eCombatAlingment& Alingment) const 
{
    // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Se esta en modo combate y se ha recibido alineacion valida?
  if (IsCombatActive()) {
	// Segun sea el tipo de alineacion, se retornara el valor
	switch(Alingment) {
	  case CombatSystemDefs::PLAYER_ALINGMENT: {
		return m_AlingmentInfo.PlayerMap.size();
	  } break;

	  case CombatSystemDefs::ENEMYPLAYER_ALINGMENT: {
		return m_AlingmentInfo.EnemyPlayerMap.size();
	  } break;
	}; // ~ switch
  }

  // No hay condiciones de encontrar el valor
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comienza el modo combate. Desde el exterior se supondra que se habra
//   comprobado la condicion de inicio de combate.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCombatSystem::StartCombat(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si no se esta en modo combate
  ASSERT(!IsCombatActive());
  // SOLO si lista de combatientes no esta vacia
  ASSERT(!m_CombatTurnInfo.TurnList.empty());
  
  // Se levanta flag
  m_bIsCombatActive = true;	  	  

  // Se cambia a estado por turnos
  m_pWorld->SetMode(WorldDefs::TURN_MODE);	  
	  
  // Se establece iterador a lista de criaturas en turno y la criatura con turno
  m_CombatTurnInfo.CriatureInTurn.CriatureIt = m_CombatTurnInfo.TurnList.begin();
  SetActCriatureInTurn();

  // Se notifica el inicio del modo combate
  m_pVMachine->OnStartCombatMode(this, 
							     m_pWorld->GetScriptEventFile(RulesDefs::SE_ONSTARTCOMBATMODE));
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se establece la criatura que toque, con la posesion del turno.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCombatSystem::SetActCriatureInTurn(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se guardan sus puntos de accion iniciales
  // En caso de que la criatura este muerta, se asociaran 0 puntos ya que
  // estara en camino la vuelta del evento de muerte.
  CCriature* const pCriature = m_pWorld->GetCriature(m_CombatTurnInfo.CriatureInTurn.CriatureIt->hCriature);
  ASSERT(pCriature);
  if (pCriature->GetHealth(RulesDefs::TEMP_VALUE) <= 0) {
	m_CombatTurnInfo.CriatureInTurn.ActionPoints = 0;
  } else {
	m_CombatTurnInfo.CriatureInTurn.ActionPoints = pCriature->GetActionPoints();
  }

  // Se establece criatura con el turno en la interfaz de combate y principal
  ASSERT((m_CombatTurnInfo.CriatureInTurn.CriatureIt != m_CombatTurnInfo.TurnList.end()) != 0);
  m_pGUIManager->MainInterfazSetMode(WorldDefs::TURN_MODE,
									 m_CombatTurnInfo.CriatureInTurn.CriatureIt->hCriature);
  m_pGUIManager->SetCombatWindow(m_CombatTurnInfo.CriatureInTurn.CriatureIt->hCriature); 
  
  // Se selecciona el selector de la criatura con el turno de combate
  GetCombatSelector(pCriature->GetHandle())->SelectWithAlphaFade(true);

  // Se desbloquea el gasto de los puntos de accion
  BlockUseOfActionPoints(false);

  // Se ordena que comience el turno
  pCriature->OnStartCombatTurn();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina la alineacion de todas aquellas entidades que se hallen alineadas
//   en el map recibido.
// - Al eliminar alineacion, tambien se establecera orden nula.
// Parametros:
// - Map. Map de alineacion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCombatSystem::RemoveAlingment(AlingmentMap& Map)
{
  // Procede a eliminar contenido de alineacion
  AlingmentMapIt It(Map.begin());
  while (It != Map.end()) {
	// Se desvincula modulo como observer de la criatura
	CCriature* const pCriature = m_pWorld->GetCriature(It->first);
	if (pCriature) {
	  pCriature->RemoveObserver(this);
	  pCriature->m_ActOrder = CCriature::AO_NONE;
	}

	// Se borra nodo en memoria y map
	delete It->second;
	It = Map.erase(It);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo de actualizacion del subsistema de combate. Se llamara en cada
//   ciclo de actualizacion desde CCRISOLEngine.
// Parametros:
// Devuelve:
// Notas:
// - A la hora de establecer el modo combate, cuando se verifiquen las 
//   condiciones, sera necesario que se este en estado de MainInterfaz para 
//   que no se produzcan incosistencias.
///////////////////////////////////////////////////////////////////////////////
void 
CCombatSystem::Update(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Se esta en modo combate?
  if (IsCombatActive()) {
	// ¿NO se cumplen las condiciones de combate?
	if (!IsCombatCondition()) {
	  // Se finaliza el modo combate
	  EndCombat();
	}
  } else {
	// Se comprueba si hay condicion de combate
	if (IsCombatCondition()) {
	  // Se inicia combate SOLO si se esta en estado de MainInterfaz
	  if (m_pGUIManager->GetGameGUIWindowState() == GUIManagerDefs::GUIW_MAININTERFAZ) {
		StartCombat();
	  }	  
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Pasa de turno a la siguiente criatura SOLO si hCriature es la criatura con
//   el turno actual.
// Parametros:
// - hCriature. Criatura que desea pasar de turno. 
// Devuelve:
// Notas:
// - El metodo solo tendra sentido si se esta en modo combate.
///////////////////////////////////////////////////////////////////////////////
void 
CCombatSystem::NextTurn(const AreaDefs::EntHandle& hCriature)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos  
  ASSERT_MSG(IsValidCombatient(hCriature), "NextTurn");

  // ¿Se esta en modo combate?
  if (IsCombatActive()) {	
	ASSERT((m_CombatTurnInfo.CriatureInTurn.CriatureIt != m_CombatTurnInfo.TurnList.end()) != 0);
	// ¿hCriature es la criatura que tiene el turno?
	if (hCriature == m_CombatTurnInfo.CriatureInTurn.CriatureIt->hCriature) {	  	  
	  // Se toma el selector de la criatura actual y se deselecciona
	  GetCombatSelector(hCriature)->SelectWithAlphaFade(false);

	  // Se avanzara el iterador a sig. nodo / criatura en turno hasta
	  // que se encuentre una criatura que no se halle en la lista de
	  // criaturas que desean abandonar el combate o se llegue al final
	  bool bExit = false;
	  do {
		// ¿La sig. criatura NO esta en la lista de criaturas que desean salirse?		
		++m_CombatTurnInfo.CriatureInTurn.CriatureIt;
		if (m_CombatTurnInfo.CriatureInTurn.CriatureIt != m_CombatTurnInfo.TurnList.end()) {
		  const CombatientLeavingListIt It(std::find(m_CombatTurnInfo.CombatientLeaving.begin(),
													 m_CombatTurnInfo.CombatientLeaving.end(),
													 m_CombatTurnInfo.CriatureInTurn.CriatureIt->hCriature));
		  if (It == m_CombatTurnInfo.CombatientLeaving.end()) {
			// La criatura pueda actuar en turno, rompe el bucle
			bExit = true;
		  }
		} else {
		  // Rompe por vuelta completa
		  bExit = true;
		}
	  }	while (!bExit);

	  // ¿Se ha dado una vuelta completa?
	  if (m_CombatTurnInfo.CriatureInTurn.CriatureIt == m_CombatTurnInfo.TurnList.end()) {
		// SI, se eliminan de la lista de criaturas con turno, las que pidieron salir
		CombatientLeavingListIt It(m_CombatTurnInfo.CombatientLeaving.begin());
		for (; It != m_CombatTurnInfo.CombatientLeaving.end(); It = m_CombatTurnInfo.CombatientLeaving.erase(It)) {
		  // Se borra de la lista de turno de combate
		  RemoveFromCombatTurnList(*It);		  
		}

		// Se incorporan combatientes en espera
		MoveInWaitingToCombatTurnList();
		
		// Se inicializa iterador a la lista con las criaturas en turno
		m_CombatTurnInfo.CriatureInTurn.CriatureIt = m_CombatTurnInfo.TurnList.begin();
		
		// ¿NO hay condicion de combate?
		if (!IsCombatCondition()) {
		  // Se finaliza combate y se retorna
		  EndCombat();
		  return;
		}		
	  }

 	  // Se toma el handle a la nueva criatura y se establece como actual	  
	  SetActCriatureInTurn();
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el handle a la criatura que posee el turno. En caso de que no
//   este activo el modo combate, devolvera handle 0.
// Parametros:
// Devuelve:
// - Handle a la criatura que posee el turno de combate
// Notas:
///////////////////////////////////////////////////////////////////////////////
AreaDefs::EntHandle 
CCombatSystem::GetCriatureInTurn(void) const
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Esta el modo combate?
  if (IsCombatActive()) {
	// Se retorna la criatura en turno de combate
	ASSERT((m_CombatTurnInfo.TurnList.end() != m_CombatTurnInfo.CriatureInTurn.CriatureIt) != 0);
	return m_CombatTurnInfo.CriatureInTurn.CriatureIt->hCriature;
  } 
  
  // El combate no esta activo, luego no habra criatura con turno
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si hay alguna criatura en el mapa de alineacion recibido, que
//   se halle en el rango de la criatura pasada.
// Parametros:
// - hCriature. Handle a la criatura a utilizar para la comprobacion.
// - Map. Map donde realizar la busqueda
// Devuelve:
// - Si hay alguna criatura en el rango true, en caso contrario false.
// Notas:
// - Este metodo sera utilizado para validar alineaciones pues en caso de
//   que una criatura quiera alinearse en combate y no tenga a otras ya 
//   alineadas a la vista, no se permitira su alineacion
///////////////////////////////////////////////////////////////////////////////
bool 
CCombatSystem::IsSomeCriatureInRange(const AreaDefs::EntHandle& hCriature,
									 AlingmentMap& Map)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hCriature);

  // Recorre el map y comprueba si hay alguien en el rango
  CCriature* const pCriature = m_pWorld->GetCriature(hCriature);
  ASSERT(pCriature);
  AlingmentMapIt It(Map.begin());
  for (; It != Map.end(); ++It) {
	// ¿Esta en rango?
	if (pCriature->IsInRange(It->first)) {
	  // Si, retorna
	  return true;
	}
  }

  // No hay nadie en el rango
  return false;
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Alinea a la criatura hCriature en combate, dentro del conjunto de
//   combatientes AlingmentSet.
// - En caso de que la alineacion sea nula, la entidad se eliminara de cualquier
//   conjunto de combate
// Parametros:
// - hCriature. Entidad a alinear en combate.
// - Alingment. Conjunto de combatientes donde alinear.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCombatSystem::SetAlingment(const AreaDefs::EntHandle& hCriature,
							const CombatSystemDefs::eCombatAlingment& Alingment)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos    
  ASSERT_MSG(IsValidCombatient(hCriature), "SetAlingment");

  // Se halla la alineacion de la criatura
  // En caso de que este alineada y se quiera insertar en una alineacion
  // distinta, se realizara el cambio
  const CombatSystemDefs::eCombatAlingment OldAlingment = GetAlingment(hCriature);
  
  // ¿Tenia alineacion?
  if (OldAlingment != CombatSystemDefs::NO_ALINGMENT) {
	// Se toma instancia al map y se borra del mismo
	AlingmentMap* const pAlingmentMap = GetAlingmentMap(OldAlingment);
	ASSERT(pAlingmentMap);		
	const AlingmentMapIt It(pAlingmentMap->find(hCriature));
	ASSERT((It != pAlingmentMap->end()) != 0);
	delete It->second;
	pAlingmentMap->erase(It);
  }

  // ¿Se desea alinear a la criatura?
  if (Alingment != CombatSystemDefs::NO_ALINGMENT) {	
	// ¿La criatura a alinear NO ES el jugador?
	if (m_pWorld->GetEntityType(hCriature) != RulesDefs::PLAYER) {
	  // Si, entonces si la criatura NO TIENE en su rango a NINGUNA otra
	  // criatura ya alineada, se cancelara la accion
	  if (!IsSomeCriatureInRange(hCriature, m_AlingmentInfo.PlayerMap) &&
		  !IsSomeCriatureInRange(hCriature, m_AlingmentInfo.EnemyPlayerMap)) {
		// La criatura NO se puede alinear, retorna
		return;
	  }	  
	}

	// Se toma el map
	AlingmentMap* const pAlingment = GetAlingmentMap(Alingment);
	ASSERT(pAlingment);
	
	// Se crea nodo de informacion, se inicializa sprite con selector e inserta
	sNAlingmentInfo* const pNAlingmentInfo = new sNAlingmentInfo;
	if (Alingment == CombatSystemDefs::PLAYER_ALINGMENT) {
	  pNAlingmentInfo->CombatSelector.Init(m_AlingmentInfo.szPlayerAlingmentSelector);
	} else {
	  pNAlingmentInfo->CombatSelector.Init(m_AlingmentInfo.szEnemyPlayerAlingmentSelector);
	}
	ASSERT(pNAlingmentInfo->CombatSelector.IsInitOk());	
	pAlingment->insert(AlingmentMapValType(hCriature, pNAlingmentInfo));

	// ¿NO estaba alineada previamente?
	if (OldAlingment == CombatSystemDefs::NO_ALINGMENT) {
	  // Se instala el modulo como observador de la criatura
	  CCriature* const pCriature = m_pWorld->GetCriature(hCriature);
	  ASSERT(pCriature);
	  pCriature->AddObserver(this);
	  
	  // Se inserta en la lista de turno de combate que proceda
	  // ¿Esta el modo combate activo?
	  if (IsCombatActive()) {
		// Si, se inserta en la lista de espera
		InsertInWaitingForCombatTurnList(hCriature);
	  } else {
		// No, se inserta en la lista principal
		InsertInCombatTurnList(hCriature);
	  }
	}
  } else {
	// No se desea alinear
	// ¿Estaba alineada?
	if (OldAlingment != CombatSystemDefs::NO_ALINGMENT) {
	  // Si, se desinstala el modulo como observador de la criatura
	  CCriature* const pCriature = m_pWorld->GetCriature(hCriature);
	  ASSERT(pCriature);
	  pCriature->RemoveObserver(this);

	  // Se insertara en la lista de de criaturas pendientes de ser
	  // eliminadas de la lisat de turnos de combate y ademas
	  m_CombatTurnInfo.CombatientLeaving.push_back(hCriature);
	  
	  // ¿Tenia el turno?
	  if (m_CombatTurnInfo.CriatureInTurn.CriatureIt != m_CombatTurnInfo.TurnList.end()) {
		if (hCriature == m_CombatTurnInfo.CriatureInTurn.CriatureIt->hCriature) {
		  // Si, se pasara automaticamente		
		  NextTurn(hCriature);
		}
	  }
	}
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Alinea a la criatura en el mismo conjunto en donde este alineada 
//   la criatura hCriatureFriend. En caso de que dicha criatura no este
//   alineada en ningun grupo, la alineacion no se llevara a cabo.
// Parametros:
// - hCriature. Entidad a la que se desea alinear.
// - hCriatureFriend. Entidad con la que se desea alinear.
// Devuelve:
// - Si ha sido posible realizar la operacion true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CCombatSystem::SetAlingmentWith(const AreaDefs::EntHandle& hCriature,
								const AreaDefs::EntHandle& hCriatureFriend)
{
  // SOLO si insancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT_MSG(IsValidCombatient(hCriature), "SetAlingmentWith_1");
  ASSERT_MSG(IsValidCombatient(hCriatureFriend), "SetAlingmentWith_2");

  // Se obtiene la alineacion de hCriatureFriend
  const CombatSystemDefs::eCombatAlingment FriendAlingment = GetAlingment(hCriatureFriend);

  // ¿Hay alineacion?
  if (FriendAlingment) {
	// Se alinea a la entidad en dicho bando y retorna
	SetAlingment(hCriature, FriendAlingment);
	return true;
  }

  // No se pudo realizar alineacion
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Alinea a la criatura en el conjunto contrario en donde este alineada 
//   la criatura hCriatureEnemy. En caso de que dicha criatura no este
//   alineada en ningun grupo, la alineacion no se llevara a cabo.
// Parametros:
// - hCriature. Entidad a la que se desea alinear.
// - hCriatureEnemy. Entidad contra la que se quiere alinear
// Devuelve:
// - Si ha sido posible realizar la operacion true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CCombatSystem::SetAlingmentAgainst(const AreaDefs::EntHandle& hCriature,
								   const AreaDefs::EntHandle& hCriatureEnemy)
{
  // SOLO si insancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT_MSG(IsValidCombatient(hCriature), "SetAlingmentAgainst_1");
  ASSERT_MSG(IsValidCombatient(hCriatureEnemy), "SetAlingmentAgainst_2");

  // Se obtiene la alineacion de hCriatureEnemy
  const CombatSystemDefs::eCombatAlingment EnemyAlingment = GetAlingment(hCriatureEnemy);

  // ¿Hay alineacion?
  if (EnemyAlingment) {	
	// Se alinea a la entidad en el bando contrario y se retorna
	if (EnemyAlingment == CombatSystemDefs::PLAYER_ALINGMENT) {
	  SetAlingment(hCriature, CombatSystemDefs::ENEMYPLAYER_ALINGMENT);
	} else {
	  SetAlingment(hCriature, CombatSystemDefs::PLAYER_ALINGMENT);
	}
	return true;
  }

  // No se pudo realizar alineacion
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si la entidad hCriature esta alineada como un combatiente y 
//   retorna su alineacion.
// Parametros:
// - hCriature. Handle a la entidad sobre la que realizar la comprobacion.
// Devuelve:
// - Alineacion de la entidad si es que tiene alguna
// Notas:
///////////////////////////////////////////////////////////////////////////////
CombatSystemDefs::eCombatAlingment 
CCombatSystem::GetAlingment(const AreaDefs::EntHandle& hCriature)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT_MSG(IsValidCombatient(hCriature), "GetAlingment");

  // Se busca la entidad en los conjuntos  
  // ¿Alineacion con jugador?
  AlingmentMapIt It(m_AlingmentInfo.PlayerMap.find(hCriature));
  if (It != m_AlingmentInfo.PlayerMap.end()) {
	return CombatSystemDefs::PLAYER_ALINGMENT;
  }
  
  // ¿Alineacion con enemigos del jugador?
  It = m_AlingmentInfo.EnemyPlayerMap.find(hCriature);
  if (It != m_AlingmentInfo.EnemyPlayerMap.end()) {
	return CombatSystemDefs::ENEMYPLAYER_ALINGMENT;
  }

  // La entidad no esta alineada
  return CombatSystemDefs::NO_ALINGMENT;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si para el conjunto Set existe alguna criatura que este 
//   viva.
// Parametros:
// - Set. Conjunto en donde realizar la busqueda.
// Devuelve:
// - Si encuentra alguna true. En caso contrario false.
// Notas:
// - Este metodo sera utilizado de apoyo para comprobar si existen la 
//   condicion de combate. Ver metodo IsCombatCondition.
///////////////////////////////////////////////////////////////////////////////
bool 
CCombatSystem::IsSomeCriatureAlive(AlingmentMap& Map)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se busca criatura en conjunto de enemigos del jugador
  AlingmentMapIt It(Map.begin());
  for (; It != Map.end(); ++It) {
	const CCriature* const pCriature = m_pWorld->GetCriature(It->first);
	ASSERT(pCriature);
	if (pCriature->GetHealth(RulesDefs::TEMP_VALUE) > 0) {
	  // Si existe 
	  return true;
	}	
  }

  // No se encontro ninguna criatura viva
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve la interfaz al selector de combate que proceda.
// Parametros:
// - hCriature. Handle a la criatura para la que obtener el selector.
// Devuelve:
// - Direccion a la interfaz del selector de combate.
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCEntitySelector* 
CCombatSystem::GetCombatientSelector(const AreaDefs::EntHandle& hCriature) 
{
  ASSERT(IsInitOk());
  
  // Returna interfaz al CEntitySelector
  return GetCombatSelector(hCriature);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el selector asociado al combatiente hCriature.
// Parametros:
// - hCriature. Handle a la criatura de la que queremos obtener el selector.
// Devuelve:
// - Si el selector se ha localizado (debido a que la criatura esta alineada
//   en combate) la direccion del selector que lo mantiene. En caso contrario
//   NULL.
// Notas:
///////////////////////////////////////////////////////////////////////////////
CEntitySelector*
CCombatSystem::GetCombatSelector(const AreaDefs::EntHandle& hCriature)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hCriature);

  // Obtiene la alineacion de la criatura
  const CombatSystemDefs::eCombatAlingment Alingment = GetAlingment(hCriature);
  if (Alingment != CombatSystemDefs::NO_ALINGMENT) {
	// Si hay alineacion, se obtiene el map
	AlingmentMap* const pAlingmentMap = GetAlingmentMap(Alingment);
	ASSERT(pAlingmentMap);
	const AlingmentMapIt It(pAlingmentMap->find(hCriature));
	ASSERT((It != pAlingmentMap->end()) != 0);	
	
	// Retorna
	return &It->second->CombatSelector;	
  }

  // No se hallo
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inserta a una criatura en la lista de combates por turnos teniendo como
//   referencia el sistema de ordenacion elegido en las reglas de juego.
// - Las ordenaciones posibles podran ser en base al orden de alineacion en
//   combate (con lo que nos limitaremos a introducir a la entidad por el final)
//   o bien en base a un valor de atributo extendido. En este caso, ordenaremos
//   siempre de mayor a menor valor.
// Parametros:
// - hCriature. Handle a la criatura a insertar.
// Devuelve:
// Notas:
// - Este metodo sera llamado desde SetAlingment cuando proceda.
///////////////////////////////////////////////////////////////////////////////
void 
CCombatSystem::InsertInCombatTurnList(const AreaDefs::EntHandle& hCriature)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT_MSG(IsValidCombatient(hCriature), "InsertInCombatTurnList");
  ASSERT((GetAlingment(hCriature) != CombatSystemDefs::NO_ALINGMENT) != 0);

  // Se obtiene el tipo de ordenacion que hay que realizar
  RulesDefs::eIDExAttrib IDExtAttrib;
  const RulesDefs::eCombatOrderValue OrderType = m_pGDBase->GetRulesDataBase()->GetCombatOrderValue(IDExtAttrib);

  // ¿Ordenacion basada en orden de alineacion?
  if (OrderType == RulesDefs::COV_BYINCLUSION) {
	// Se inserta por el final sin mas
	m_CombatTurnInfo.TurnList.push_back(hCriature);
  } else {
	// Insercion basada en la ordenacion en base a los atributos extendidos	
	// Se toma el valor de la criatura
	const sword swValue = m_pWorld->GetCriature(hCriature)->GetExAttribute(IDExtAttrib, 
																		   RulesDefs::TEMP_VALUE);

	// Se recorre la lista de turnos para insertar a la criatura
	CombatientTurnListIt It(m_CombatTurnInfo.TurnList.begin());
	for (; It != m_CombatTurnInfo.TurnList.end(); ++It) {
	  const CCriature* const pCriature = m_pWorld->GetCriature(It->hCriature);
	  ASSERT(pCriature);
	  // Los valores se ordenaran de mayor a menor
	  if (swValue > pCriature->GetExAttribute(IDExtAttrib, RulesDefs::TEMP_VALUE)) {
		// Se encontro localidad donde insertar
		break;
	  }
	}

	// Se inserta
	m_CombatTurnInfo.TurnList.insert(It, hCriature);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina de la lista de turnos de combate a la criatura hCriatura. El 
//   metodo buscara tanto en la lista temporal como en la lista principal.
// Parametros:
// - hCriature. 
// Devuelve:
// Notas:
// - Se entendera que la criatura DEBERA de existir en una de las listas. De no
//   ser asi, se dara error via assert.
///////////////////////////////////////////////////////////////////////////////
void 
CCombatSystem::RemoveFromCombatTurnList(const AreaDefs::EntHandle& hCriature)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT_MSG(IsValidCombatient(hCriature), "RemoveFromCombatTurnList");

  // ¿Esta en la lista de espera?
  CombatientTurnListIt It(std::find(m_CombatTurnInfo.WaitingForCombat.begin(),
							        m_CombatTurnInfo.WaitingForCombat.end(),
									hCriature));
  if (It != m_CombatTurnInfo.WaitingForCombat.end()) {
	// Criatura localizada, se borra
	m_CombatTurnInfo.WaitingForCombat.erase(It);
  } else {
	// No esta en la lista de espera, se busca en la oficial
	It = std::find(m_CombatTurnInfo.TurnList.begin(),
				   m_CombatTurnInfo.TurnList.end(),
				   hCriature);
	if (It != m_CombatTurnInfo.TurnList.end()) {
	  // Criatura localizada, se borra
	  m_CombatTurnInfo.TurnList.erase(It);
	} else {
	  ASSERT(false);
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Mueve a todas las criaturas que esten en espera a la lista de combate
//   por turnos. Este metodo se llamara siempre que se halla recorrido la 
//   lista de combatientes en turno de combate.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCombatSystem::MoveInWaitingToCombatTurnList(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Recorre la lista de combatientes en espera y los va insertando
  CombatientTurnListIt It(m_CombatTurnInfo.WaitingForCombat.begin());
  for (; It != m_CombatTurnInfo.WaitingForCombat.end(); It = m_CombatTurnInfo.WaitingForCombat.erase(It)) {
	// Inserta a la criatura en combate
	InsertInCombatTurnList(It->hCriature);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si hCriature es una criatura o bien es el jugador, en cuyo caso
//   se considerara un combatiente valido.
// - Este metodo no tendra otra utilidad mas que ser usado en las comprobaciones
//   ASSERT para verificar que el handle recibido es valido.
// Parametros:
// - hCriature. Entidad sobre la que realizar la comprobacion.
// Devuelve:
// - Si el combatiente es valido true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCombatSystem::IsValidCombatient(const AreaDefs::EntHandle& hCriature) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hCriature);

  // Una entidad es un combatiente valido si es una criatura o el jugador
  const RulesDefs::eEntityType Type = m_pWorld->GetEntityType(hCriature);
  if (Type == RulesDefs::CRIATURE ||
	  Type == RulesDefs::PLAYER) {
    // Valido
    return true;
  } else {
	// No valido
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CCombatSystem::IsValidCombatient> Combatiente no valido.\n");
	  SYSEngine::GetLogger()->Write("                                | Handle: %u.\n", hCriature);
	  SYSEngine::GetLogger()->Write("                                | Tipo: %d.\n", Type);
	  CWorldEntity* const pEntity = m_pWorld->GetWorldEntity(hCriature);
	  if (pEntity) {
		SYSEngine::GetLogger()->Write("                                | Nombre: %s.\n", pEntity->GetName().c_str());
	  }
	#endif	
	return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe la notificacion de una criatura sobre un determinado suceso en
//   la misma o cambio de estado.
// - A la hora de recibir la notificacion de la realizacion de una accion,
//   se tendra en cuenta si esta bloqueado o no el uso de los puntos de accion.
// Parametros:
// - hCriature. Handle a la criatura.
// - NotifyType. Tipo de suceso.
// - udParam. Posible parametro.
// Devuelve:
// Notas:
// - En caso de que no se este en modo combate, se obviaran las notificaciones
// - Cuando la notificacion provenga del jugador, se tendra en cuenta los
//   sig. aspectos:
//    * Si hay ptos de accion insuficientes -> NO se pasara de turno.
//    * Si al restar puntos, estos quedan a 0 -> NO se pasara SI se esta
//      en la interfaz PlayerProfile.
// - El codigo de la accion a realizar en udParam, SIEMPRE, ira en el word
//   inferior.
///////////////////////////////////////////////////////////////////////////////
void 
CCombatSystem::CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
									  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
									  const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT_MSG(IsValidCombatient(hCriature), "CriatureObserverNotify");

  // ¿NO se esta en modo combate?
  if (!IsCombatActive()) {
	// Si, se abandona
	return;
  }

  // Se comprueba el tipo de notificacion recibida
  switch (NotifyType) {	
	case CriatureObserverDefs::IS_DEATH: {
	  // La criatura ejecuto el evento de muerte
	  CCriature* const pCriature = m_pWorld->GetCriature(hCriature);
	  // ¿Es el jugador?
	  if (pCriature->GetEntityType() == RulesDefs::PLAYER) {
	    // Si, se finaliza el combate
	    EndCombat();
	  } else {
	    // No, se desvincula a la criatura del combate
	    SetAlingment(hCriature, CombatSystemDefs::NO_ALINGMENT);
	  }	  
	} break;

	case CriatureObserverDefs::ACTION_REALICE: {
	  // ¿Criatura con turno?
	  if (hCriature == GetCriatureInTurn()) {
		// ¿NO esta bloqueado el uso de puntos de accion?
		if (!m_bActionPointsBlocked) {
		  // Se toma la accion a realizar y sus puntos de accion
		  const RulesDefs::eCriatureAction Action = RulesDefs::eCriatureAction(udParam & 0x0000FFFF);
		  
		  // Se comprueba si se esta realizando alguna accion especial o
		  // bien una accion no considerada como que gasta puntos de combate
		  RulesDefs::CriatureActionPoints ActionCost = 0;
		  switch(Action) {
			case RulesDefs::CA_USEHABILITY: {
			  // Se obtiene el identificador de la habilidad en los primeros 16 bits
			  const RulesDefs::eIDHability IDHability = RulesDefs::eIDHability(udParam >> 16);
			  ActionCost = m_pGDBase->GetRulesDataBase()->GetUseHabilityCost(IDHability);
			} break;  

			case RulesDefs::CA_HIT: {
			  // Se obtiene el slot con el que se golpea y los ptos asociados
			  const RulesDefs::eIDEquipmentSlot Slot = RulesDefs::eIDEquipmentSlot(udParam >> 16);
			  CCriature* const pCriature = m_pWorld->GetCriature(hCriature);
			  ASSERT(pCriature);
			  CItem* const pItem = m_pWorld->GetItem(pCriature->GetEquipmentSlots()->GetItemEquipedAt(Slot));
			  ActionCost = (NULL == pItem) ? 0 : m_pGDBase->GetRulesDataBase()->GetUseWeaponItemCost(pItem->GetType());
			} break;

			case RulesDefs::CA_DROPITEM:
			case RulesDefs::CA_STOPWALK: {
			  // No se considerara gasto en puntos de combate
			  return;
			} break;
		  }; // ~ switch

		  // Se añaden los puntos base de coste de la accion realizada
		  ActionCost += m_pGDBase->GetRulesDataBase()->GetCombatActionCost(Action);
		  
		  // Se decrementan los puntos de accion y se actualizan en ventana
		  ASSERT((ActionCost <= m_CombatTurnInfo.CriatureInTurn.ActionPoints) != 0);
		  m_CombatTurnInfo.CriatureInTurn.ActionPoints -= ActionCost;	  
		  ASSERT((m_CombatTurnInfo.CriatureInTurn.ActionPoints >= 0) != 0);
  		  m_pGUIManager->CombatSetActionPoints(m_CombatTurnInfo.CriatureInTurn.ActionPoints);

		  // ¿Ya no quedan puntos de accion?
		  if (0 == m_CombatTurnInfo.CriatureInTurn.ActionPoints) {
			// Se notifica de tal forma que se ejecute el case INSUFICIENT_ACTION_POINTS
			CCriature* const pCriature = m_pWorld->GetCriature(hCriature);
			ASSERT(pCriature);
			
			// Si la entidad es un NPC, se pasara de turno automaticamente
			if (RulesDefs::CRIATURE == pCriature->GetEntityType()) {			
			  NextTurn(hCriature);
			} else {
			  // En caso contrario se trata del jugador y se pasara de turno
			  // SOLO si NO se esta en el interfaz de inventario.
			  if (m_pGUIManager->GetGameGUIWindowState() != GUIManagerDefs::GUIW_PLAYERPROFILE) {			  
				NextTurn(hCriature);
			  }
			}
		  } 
		}	  
	  }
	} break;	
  
	case CriatureObserverDefs::INSUFICIENT_ACTION_POINTS: {	  
  	  // ¿Criatura con turno?
	  if (hCriature == GetCriatureInTurn()) {
		// Se pasara de turno si la entidad no es el jugador
		if (m_pWorld->GetPlayer()->GetHandle() != hCriature) {		
		  NextTurn(hCriature);
		} else {
		  // En el caso de que sea el jugador se mandara mensaje a consola
		  SYSEngine::GetGUIManager()->WriteToConsole(m_pGDBase->GetStaticText(GameDataBaseDefs::ST_GENERAL_INSUFICIENTACTIONPOINTS),
													 true);
		}	  
	  }
	} break;
  }; // ~ switch    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Bloquea / desbloquea el gasto de los puntos de accion sobre la entidad
//   que posea el turno. 
// Parametros:
// - bBlock. Flag para bloquear / desbloquear.
// Devuelve:
// Notas:
// - El metodo solo tendra sentido sobre la entidad que posee el turno, ya que
//   cada vez que se pase de turno, se desbloqueara.
///////////////////////////////////////////////////////////////////////////////
void 
CCombatSystem::BlockUseOfActionPoints(const bool bBlock)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se establece flag solo si esta en modo combate
  if (IsCombatActive()) {
	m_bActionPointsBlocked = bBlock;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retorna los puntos de accion relativos a combate de una entidad. En caso
//   que la criatura no este combatiendo, no este activo el modo combate o ya
//   haya pasado su turno, se retornara 0. En caso contrario los puntos de
//   accion disponibles (si posee el turno, los que esta utilizando)
// Parametros:
// - hCriature. Handle a la criatura
// Devuelve:
// - Los puntos de accion en combate.
// Notas:
///////////////////////////////////////////////////////////////////////////////
RulesDefs::CriatureActionPoints 
CCombatSystem::GetCriatureActionPoints(const AreaDefs::EntHandle& hCriature) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hCriature);

  // ¿Se esta en modo combate?
  if (IsCombatActive()) {
	// ¿La criatura recibida es la que tiene el turno?
	if (GetCriatureInTurn() == hCriature) {
	  // Se retornan sus puntos
	  return m_CombatTurnInfo.CriatureInTurn.ActionPoints;
	} else {
	  // Se busca desde la pos. actual a ver si se halla a la criatura
	  // devolviendose sus puntos originales, pues aun no habra movido
	  CombatientTurnListIt It(m_CombatTurnInfo.CriatureInTurn.CriatureIt);
	  for (; It != m_CombatTurnInfo.TurnList.end(); ++It) {
		// ¿Es la criatura buscada?
		if (It->hCriature == hCriature) {
		  // Si, se retornan sus puntos originales
		  CCriature* const pCriature = m_pWorld->GetCriature(hCriature);
		  ASSERT(pCriature);
		  return pCriature->GetActionPoints();
		}
	  }
	}
  }

  // No se esta en modo combate, la criatura no esta combatiendo o bien
  // la criatura ya paso su turno de combate
  return 0;
}  

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion de la finalizacion de un evento script.
// Parametros:
// - ScriptEvent. Tipo de evento script.
// - Notify. Notifiacion asociada.
// - udParams. Parametros extra
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCombatSystem::ScriptNotify(const RulesDefs::eScriptEvents& ScriptEvent,
							const ScriptClientDefs::eScriptNotify& Notify,
							const dword udParams)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se evalua
  switch(ScriptEvent) {
	case RulesDefs::SE_ONSTARTCOMBATMODE: {
	  // Script de notificacion del inicio del modo combate
	} break;	

	case RulesDefs::SE_ONENDCOMBATMODE: {
	  // Script de notificacion del fin del modo combate
	} break;	
  };
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Atendera a las notificaciones de destruccion de entidades. Si una entidad
//   va a destruirse y se halla alineada en combate, se tomara como si hubiera
//   muerto, quitando su alineacion de combate.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCombatSystem::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
								   const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Comprueba que la notificacion sea la buscada
  switch(NotifyType) {
    case WorldObserverDefs::ENTITY_DESTROY: {
	  // ¿La entidad destruida es una criatura?
	  const RulesDefs::eEntityType Type = m_pWorld->GetEntityType(udParam);
	  if (Type == RulesDefs::CRIATURE ||
		  Type == RulesDefs::PLAYER) {
  		// ¿La entidad destruida se halla alineada?
		const CombatSystemDefs::eCombatAlingment Alingment = GetAlingment(udParam);	
		if (Alingment != CombatSystemDefs::NO_ALINGMENT) {
		  // Si, estaba alineada, se quita su alineacion
		  // Nota: Es seguro que la criatura NO sera el jugador
		  SetAlingment(udParam, CombatSystemDefs::NO_ALINGMENT);
		}
	  }
	} break;	  
  }; // ~ switch
}

