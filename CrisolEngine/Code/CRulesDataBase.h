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
// CRulesDataBase.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CRulesDataBase
//
// Descripcion:
// - Esta clase tiene como objetivo llevar a memoria informacion relacionada
//   con el archivo CBT que se pretende pasar a CBB. Esta informacion se
//   hallara especificada en el archivo de reglas, que siempre se va a
//   esperar que se llame RulesDefs.cbt.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CRULESDATABASE_H_
#define _CRULESDATABASE_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _ICRULESDATABASE_H_
#include "iCRulesDataBase.h"
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif
#ifndef _CCBTPARSER_H_
#include "CCBTParser.h"
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CRulesDataBase
class CRulesDataBase: public iCRulesDataBase
{
private:
  // Predicado para la busqueda en maps a partir del valor 
  // Nota: Todos los valores seran punteros
  template<class First, class Second>
  class CMapValueSearch {
  private:
	// Vbles de miembro
	Second m_Value; // Valor a localizar

  public:
	// Constructor
	CMapValueSearch(const Second& Value): m_Value(Value) { }

  public:
	// Predicado de busqueda
	bool operator() (std::pair<const First, Second*>& Node) {
	  return (*Node.second == m_Value);	  
	}  
  }; // ~ CMapValueSearch


private:
  // Estructuras forward
  struct sIDInfo;
  struct sIDSceneObjInfo;
  struct sIDItemInfo;
  struct sIDWallInfo;

private:
  // Tipos
  // Map general para el almacenamiento de (handle, identificadores de texto)
  typedef std::map<RulesDefs::BaseType, sIDInfo*> IDMap;
  typedef IDMap::iterator                         IDMapIt;
  typedef IDMap::value_type                       IDMapValType;
  // Map particular para objetos de escenario
  typedef std::map<RulesDefs::SceneObjType, sIDSceneObjInfo*> IDSceneObjMap;
  typedef IDSceneObjMap::iterator                             IDSceneObjMapIt;
  typedef IDSceneObjMap::value_type                           IDSceneObjMapValType;
  // Map particular para items
  typedef std::map<RulesDefs::ItemType, sIDItemInfo*> IDItemMap;
  typedef IDItemMap::iterator                         IDItemMapIt;
  typedef IDItemMap::value_type                       IDItemMapValType;
  // Map particular para paredes
  typedef std::map<RulesDefs::WallType, sIDWallInfo*> IDWallMap;
  typedef IDWallMap::iterator                         IDWallMapIt;
  typedef IDWallMap::value_type                       IDWallMapValType;
  // Map particular para habilidades usables
  typedef std::map<RulesDefs::eIDHability, RulesDefs::CriatureActionPoints> UsableHabMap;
  typedef UsableHabMap::iterator                                            UsableHabMapIt;
  typedef UsableHabMap::value_type                                          UsableHabValType;
  // Map particular para items en calidad de arma
  typedef std::map<RulesDefs::ItemType, RulesDefs::CriatureActionPoints> WeaponItemMap;
  typedef WeaponItemMap::iterator                                        WeaponItemMapIt;
  typedef WeaponItemMap::value_type										 WeaponItemMapValType;
  // Map para almacenar la minima distancia en la realizacion de ciertas acciones
  typedef dword MinDistanceKey;
  typedef std::map<MinDistanceKey, RulesDefs::MinDistance> MinDistForActionMap;
  typedef MinDistForActionMap::iterator					   MinDistForActionMapIt;
  typedef MinDistForActionMap::value_type                  MinDistForActionMapValType;
  
private:
  // Estructuras
  struct sGameControllerRules {
	// Info de reglas asociadas al controlador del juego
	byte SecondsPerMinute; // Segundos que tiene un minuto
  };

  struct sFloorRules {
    // Info de reglas asociadas a los floor (tiles de suelo)
	IDMap TypesIDs; // Mapeado de identificadores de tipo
  };

  struct sSceneObjRules {
	// Info de reglas asociadas a los objetos de escenario	
	IDSceneObjMap TypesIDs; // Mapeado de identificadores de tipo
  };

  struct sItemRules {
	// Info de reglas asociadas a los items
	IDItemMap     TypesIDs;         // Mapeado de identificadores de tipos
	IDMap         GlobalAttribsIDs; // Mapeado de identificadores de atributos globales	
	WeaponItemMap WeaponsItemsMap;  // Mapeado del coste de uso de items - arma
  }; 

  struct sWallRules {
	// Info asociada a reglas de las paredes
	IDWallMap TypesIDs; // Mapeado de identificadores de tipo
  };

  struct sCriatureRules {	
	// Info de reglas asociadas a criaturas
	// Mapeo de identificadores
	IDMap TypesIDs;          // Identificadores de tipos
	IDMap ClassesIDs;        // Identificadores de clases
	IDMap GenresIDs;         // Identificadores de generos
	IDMap ExtAttribsIDs;     // Identificadores de atributos extendidos
	IDMap HabilitiesIDs;     // Identificadores de habilidades
	IDMap EquipmentSlotsIDs; // Identificadores de slots de equipamiento
	IDMap SymptomsIDs;       // Identificadores de sintomas	
	// Info sobre habilidades usables
	UsableHabMap UsableHabilities; // Conjunto de habilidades usables	
  };

  struct sMinDistancesRules {
	// Reglas asociadas a minimas distancias
	RulesDefs::MinDistance Talk;          // Dist. minima para hablar
	MinDistForActionMap	   UseHability;   // Dist. minimas para habilidades
	MinDistForActionMap    HitWithWeapon; // Golpear con arma
  };

  struct sCombatRules {
	// Estructuras
	struct sTurnOrderInfo {
	  // Info con el que realizar la ordenacion en combate
	  // La ordenacion podra ser en base a orden de inclusion en alineacion o
	  // bien en base al uso de un atributo extendido
	  RulesDefs::eCombatOrderValue Value;     // Valor con el que ordenar
	  RulesDefs::eIDExAttrib       IDExAttrib; // Atributo extendido a usar
	};
	// Info relativa a las reglas especificas del modo combate	
	// Coste para acciones
	RulesDefs::CriatureActionPoints ActionsCost[RulesDefs::MAX_CRIATURECOMBATACTIONS];
	sTurnOrderInfo TurnOrderInfo; // Info para ordenacion en combate	
  };
  
private:
  // Valores asociados a las reglas definidas
  sGameControllerRules m_GameControllerRules; // Reglas asociadas al controlador
  sCriatureRules       m_CriatureRules;       // Reglas asociadas a las criaturas
  sMinDistancesRules   m_MinDistancesRules;   // Reglas miscelaneas para criaturas
  sFloorRules          m_FloorRules;          // Reglas asociadas a los floor
  sSceneObjRules       m_SceneObjRules;       // Reglas asociadas a los Obj. de escenario
  sItemRules           m_ItemRules;           // Reglas asociadas a los items.
  sWallRules           m_WallRules;           // Reglas asociadas a las paredes
  sCombatRules         m_CombatRules;         // Reglas asociadas al combate  

  // Resto de vbles  
  std::string m_szRulesFileName; // Nombre del archivo de reglas
  bool		  m_bIsInitOk;       // ¿Instancia inicializada correctamente?
   
public:
  // Constructor / Destructor
  CRulesDataBase(void): m_bIsInitOk(false) { }
  ~CRulesDataBase(void) { 
    End(); 
  }
  
public:
  // Protocolo de inicio y fin de instancia
  bool Init(const std::string& szRulesFileName);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }
private:
  // Metodos de apoyo
  bool TLoadGameControllerRules(CCBTParser& Parser);
  bool TLoadFloorRules(CCBTParser& Parser);
  bool TLoadSceneObjRules(CCBTParser& Parser);
  bool TLoadItemRules(CCBTParser& Parser);
  bool TLoadWallRules(CCBTParser& Parser);
  bool TLoadCriatureRules(CCBTParser& Parser);  
  bool TLoadMinDistancesRules(CCBTParser& Parser);
  bool TLoadCombatRules(CCBTParser& Parser);
  void TLoadIDs(CCBTParser& Parser, 
				const word uwMaxValue, IDMap& Map);  
  void ReleaseRules(void);
  void ReleaseIDMap(IDMap& Map);
  void ReleaseIDMap(IDSceneObjMap& Map);
  void ReleaseIDMap(IDItemMap& Map);
  void ReleaseIDMap(IDWallMap& Map);

public:
  // iCRulesDataBase / Reglas / Controlador de juego
  byte GetSecondsPerMinute(void) const {
	ASSERT(IsInitOk());
	// Retorna los minutos que equivalen a una hora de juego
	return m_GameControllerRules.SecondsPerMinute;
  }

public:
  // iCRulesDataBase / Reglas / Combate
  RulesDefs::CriatureActionPoints GetCombatActionCost(const RulesDefs::eCriatureAction& Action) {
	ASSERT(IsInitOk());
	ASSERT((Action < RulesDefs::MAX_CRIATURECOMBATACTIONS) != 0);
	// Devuelve los puntos que cuesta usar la accion Action
	return m_CombatRules.ActionsCost[Action];
  }
  RulesDefs::CriatureActionPoints GetUseHabilityCost(const RulesDefs::eIDHability& Hability) {
	ASSERT(IsInitOk());	
	// Obtiene iterador a posible nodo
	const UsableHabMapIt It(m_CriatureRules.UsableHabilities.find(Hability));
	ASSERT((It != m_CriatureRules.UsableHabilities.end()) != 0);
	return It->second;
  }
  RulesDefs::CriatureActionPoints
	GetUseWeaponItemCost(const RulesDefs::ItemType& ItemType) {
	// ASSERT(IsInitOk()); neccesiaro para poder usarse desde TLoadMinDistances
	// Se intenta obtener iterador a nodo con la info del item
	const WeaponItemMapIt It(m_ItemRules.WeaponsItemsMap.find(ItemType)); 
	// Si no hay nodo registrado el coste base sera de 0
	return (It == m_ItemRules.WeaponsItemsMap.end()) ? 0 : It->second;
  }
  RulesDefs::eCombatOrderValue GetCombatOrderValue(RulesDefs::eIDExAttrib& IDExtAttrib) {
	ASSERT(IsInitOk());
	// Retorna el tipo de valor usado para ordenar en combate
	// En caso de que la ordenacion sea usando un atributo extendido, alojara
	// en el parametro recibido el atributo extendido
	if (RulesDefs::COV_BYEXATTRIB == m_CombatRules.TurnOrderInfo.Value) {
	  IDExtAttrib = m_CombatRules.TurnOrderInfo.IDExAttrib;
	}
	return m_CombatRules.TurnOrderInfo.Value;
  }

public:
  // iCRulesDataBase / Reglas / Entidades / Obtencion de informacion a partir de identificadores
  word GetNumEntitiesIDs(const RulesDefs::eIDRuleType& IDEntityRule,
						 const RulesDefs::BaseType& BaseType = 0);
  sword GetEntityIDNumeric(const RulesDefs::eIDRuleType& IDEntityRule,
						   const std::string& szIDStr,
						   const RulesDefs::BaseType& BaseType = 0);
  std::string GetEntityIDStr(const RulesDefs::eIDRuleType& IDEntityRule,
							 const RulesDefs::BaseType& IDNumeric,
							 const RulesDefs::BaseType& BaseType = 0);  
  RulesDefs::MinDistance GetMinDistanceFor(const RulesDefs::eMinDistance& Action,
										   const dword udParam = 0,
										   const dword udExtraParam = 0);
  
private:
  // Metodos de apoyo
  IDMap* const GetIDMap(const RulesDefs::eIDRuleType& IDEntityRule,
						const RulesDefs::BaseType& BaseType = 0);

public:
  // iCRulesDataBase / Reglas / Entidades / Obtencion de informacion concreta a entidades
  bool IsSceneObjContainer(const RulesDefs::SceneObjType& SceneObj);
  bool CanSetItemAtEquipmentSlot(const RulesDefs::ItemType& ItemType,
								 const RulesDefs::EquipmentSlot& Slot);
  bool IsUsableHability(const RulesDefs::eIDHability& Hability) {
	ASSERT(IsInitOk());
	// Retorna flag
	const UsableHabMapIt It(m_CriatureRules.UsableHabilities.find(Hability));
	return (It != m_CriatureRules.UsableHabilities.end());
  }


private:
  // Metodos para el trabajo con los errores
  void FatalError(const std::string& szMsg);
};

#endif // ~ CRulesDataBase


