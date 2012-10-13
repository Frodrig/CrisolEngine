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
// iCVirtualMachine.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCVirtualMachine.
//
// Descripcion:
// - Interfaz a CVirtualMachine
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICVIRTUALMACHINE_H_
#define _ICVIRTUALMACHINE_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"          
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _RULESDEFS_H_
#include "RulesDefs.h"
#endif
#ifndef _FILEDEFS_H_
#include "FileDefs.h"
#endif
#ifndef _INPUTDEFS_H_
#include "InputDefs.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif

// Definicion de clases / structuras / tipos referenciados
class CWorld;
struct iCScriptClient;

// Interfaz iCVirtualMachine
struct iCVirtualMachine
{
private:
  // Clases amigas
  friend class CWorld;

private:
  // Preparacion / finalizacion del entorno de ejecucion
  virtual void PrepareScripts(void) = 0;  
  virtual void PrepareScripts(const FileDefs::FileHandle& hFile,
							  dword& udOffset) = 0;  
  virtual void EndScripts(void) = 0;

private:
  // Almacenamiento y recuperacion de los valores globales
  virtual void SaveGlobals(const FileDefs::FileHandle& hFile,
				           dword& udOffset) = 0;

public:
  // Trabajo con el flag de ejecucion de scripts
  virtual void SetScriptExecution(const bool bSet) = 0;

public:
  // Liberacion de scripts asociados a un cliente
  virtual void ReleaseScripts(iCScriptClient* const pClient) = 0;
  virtual void ReleaseScripts(iCScriptClient* const pClient,
						      const RulesDefs::eScriptEvents& Event) = 0;
public:
  // Metodos de ejecucion de los eventos scripts
  virtual void OnStartGameEvent(iCScriptClient* const pClient,
								const std::string& szScript) = 0;
  virtual void OnClickHourPanelEvent(iCScriptClient* const pClient,
									 const std::string& szScript) = 0;
  virtual void OnFleeCombatEvent(iCScriptClient* const pClient,
								 const std::string& szScript) = 0;
  virtual void OnKeyPressedEvent(iCScriptClient* const pClient,
								 const std::string& szScript) = 0;
  virtual void OnStartCombatMode(iCScriptClient* const pClient,
								 const std::string& szScript) = 0;
  virtual void OnEndCombatMode(iCScriptClient* const pClient,
							   const std::string& szScript) = 0;
  virtual void OnNewHourEvent(iCScriptClient* const pClient,
							  const std::string& szScript,
							  const byte ubHour) = 0;
  virtual void OnEnterInArea(iCScriptClient* const pClient,
							 const std::string& szScript,
							 const word uwHour) = 0;
  virtual void OnWorldIdleEvent(iCScriptClient* const pClient,
								const std::string& szScript) = 0;
  virtual void OnSetInFloor(iCScriptClient* const pClient,
							const std::string& szScript,
							const AreaDefs::TilePosType& XPos,
							const AreaDefs::TilePosType& YPos,
							const AreaDefs::EntHandle& hTheEntity) = 0;
  virtual void OnSetOutFloor(iCScriptClient* const pClient,
							 const std::string& szScript,
							 const AreaDefs::TilePosType& XPos,
							 const AreaDefs::TilePosType& YPos,
							 const AreaDefs::EntHandle& hTheEntity) = 0;
  virtual void OnGetItem(iCScriptClient* const pClient,
						 const std::string& szScript,
						 const AreaDefs::EntHandle& hTheItem,
						 const AreaDefs::EntHandle& hTheCriature) = 0;
  virtual void OnDropItem(iCScriptClient* const pClient,
						  const std::string& szScript,
						  const AreaDefs::EntHandle& hTheItem,
						  const AreaDefs::EntHandle& hTheCriature) = 0;
  virtual void OnObserveEntity(iCScriptClient* const pClient,
							   const std::string& szScript,
							   const AreaDefs::EntHandle& hTheEntity,
							   const AreaDefs::EntHandle& hTheCriature) = 0;
  virtual void OnTalkToEntity(iCScriptClient* const pClient,
							  const std::string& szScript,
							  const AreaDefs::EntHandle& hTheEntity,
							  const AreaDefs::EntHandle& hTheCriature) = 0;
  virtual void OnManipulateEntity(iCScriptClient* const pClient,
								  const std::string& szScript,
								  const AreaDefs::EntHandle& hTheEntity,
								  const AreaDefs::EntHandle& hTheCriature) = 0;
  virtual void OnDeath(iCScriptClient* const pClient,
					   const std::string& szScript,
					   const AreaDefs::EntHandle& hTheCriature) = 0;
  virtual void OnResurrect(iCScriptClient* const pClient, 
						   const std::string& szScript,
						   const AreaDefs::EntHandle& hTheCriature) = 0;
  virtual void OnEquippedItem(iCScriptClient* const pClient,
							  const std::string& szScript,
							  const AreaDefs::EntHandle& hTheItem,
							  const AreaDefs::EntHandle& hTheCriature,
							  const RulesDefs::eIDEquipmentSlot& Slot) = 0;
  virtual void OnRemoveEquippedItem(iCScriptClient* const pClient,
									const std::string& szScript,
									const AreaDefs::EntHandle& hTheItem,
									const AreaDefs::EntHandle& hTheCriature,
									const RulesDefs::eIDEquipmentSlot& Slot) = 0;
  virtual void OnUseHability(iCScriptClient* const pClient,
							 const std::string& szScript,
							 const AreaDefs::EntHandle& hTheCriature,
							 const RulesDefs::eIDHability& Hability,
							 const AreaDefs::EntHandle& hTheEntity) = 0;
  virtual void OnActivatedSymptom(iCScriptClient* const pClient,
								  const std::string& szScript,
								  const AreaDefs::EntHandle& hTheCriature,
								  const RulesDefs::eIDSymptom& Symptom) = 0;
  virtual void OnDesactivatedSymptom(iCScriptClient* const pClient,
									 const std::string& szScript,
									 const AreaDefs::EntHandle& hTheCriature,
									 const RulesDefs::eIDSymptom& Symptom) = 0;
  virtual void OnHitEntity(iCScriptClient* const pClient,
						   const std::string& szScript,
						   const AreaDefs::EntHandle& hTheCriature,
						   const RulesDefs::eIDEquipmentSlot& Slot,
						   const AreaDefs::EntHandle& hTheEntity) = 0;
  virtual void OnEntityIdle(iCScriptClient* const pClient,
							const std::string& szScript,
							const AreaDefs::EntHandle& hTheEntity) = 0;
  virtual void OnStartCombatTurn(iCScriptClient* const pClient,
								 const std::string& szScript,
							     const AreaDefs::EntHandle& hTheCriature) = 0;
  virtual void OnUseItem(iCScriptClient* const pClient,
						 const std::string& szScript,
						 const AreaDefs::EntHandle& hTheItem,
						 const AreaDefs::EntHandle& hTheEntity,
						 const AreaDefs::EntHandle& hTheCriature) = 0;
  virtual void OnTradeItem(iCScriptClient* const pClient,
						   const std::string& szScript,
						   const AreaDefs::EntHandle& hTheItem,
						   const AreaDefs::EntHandle& hTheEntitySrc,
						   const AreaDefs::EntHandle& hTheEntityDest) = 0;
  virtual void OnEntityCreated(iCScriptClient* const pClient,
							   const std::string& szScript,
							   const AreaDefs::EntHandle& hTheEntity) = 0; 
  virtual void OnCriatureInRange(iCScriptClient* const pClient,
								 const std::string& szScript,
								 const AreaDefs::EntHandle& hTheCriatureWithRange,
								 const AreaDefs::EntHandle& hTheCriatureInRange) = 0;
  virtual void OnCriatureOutOfRange(iCScriptClient* const pClient,
								    const std::string& szScript,
								    const AreaDefs::EntHandle& hTheCriatureWithRange,
								    const AreaDefs::EntHandle& hTheCriatureOutOfRange) = 0;
};

#endif // ~ #ifdef _ICVIRTUALMACHINE_H_
