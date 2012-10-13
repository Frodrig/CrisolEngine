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
// iCRulesDataBase.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCRulesDataBase
//
// Descripcion:
// - Interfaz para la clase CRulesDataBase
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICRULESDATABASE_H_
#define _ICRULESDATABASE_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _RULESDEFS_H_
#include "RulesDefs.h"
#endif
#ifndef _GAMEDATABASEDEFS_H_
#include "GameDataBaseDefs.h"
#endif

// Definicion de clases / estructuras / tipos referenciados

// Interfaz iCGUIWindowClient
struct iCRulesDataBase
{ 
public:
  // Controlador de juego
  virtual byte GetSecondsPerMinute(void) const = 0;

public:
  // Combate
  virtual RulesDefs::CriatureActionPoints GetCombatActionCost(const RulesDefs::eCriatureAction& Action) = 0;
  virtual RulesDefs::CriatureActionPoints GetUseHabilityCost(const RulesDefs::eIDHability& Hability) = 0;
  virtual RulesDefs::CriatureActionPoints GetUseWeaponItemCost(const RulesDefs::ItemType& ItemType) = 0;
  virtual RulesDefs::eCombatOrderValue GetCombatOrderValue(RulesDefs::eIDExAttrib& IDExtAttrib) = 0;

public:
  // Obtencion de informacion a partir de identificadores
  virtual word GetNumEntitiesIDs(const RulesDefs::eIDRuleType& IDEntityRule,
								 const RulesDefs::BaseType& BaseType = 0) = 0;
  virtual sword GetEntityIDNumeric(const RulesDefs::eIDRuleType& IDEntityRule,
								   const std::string& szIDStr,
								   const RulesDefs::BaseType& BaseType = 0) = 0;
  virtual std::string GetEntityIDStr(const RulesDefs::eIDRuleType& IDEntityRule,
									 const RulesDefs::BaseType& IDNumeric,
									 const RulesDefs::BaseType& BaseType = 0) = 0;
  virtual RulesDefs::MinDistance GetMinDistanceFor(const RulesDefs::eMinDistance& Action,
												   const dword udParam = 0,
												   const dword udExtraParam = 0) = 0;
public:
  // Obtencion de informacion concreta a entidades
  virtual bool IsUsableHability(const RulesDefs::eIDHability& Hability) = 0;
  virtual bool IsSceneObjContainer(const RulesDefs::SceneObjType& SceneObj) = 0;
  virtual bool CanSetItemAtEquipmentSlot(const RulesDefs::ItemType& ItemType,
										 const RulesDefs::EquipmentSlot& Slot) = 0;
};

#endif // ~ #ifdef _ICRULESDATABASE_H_
