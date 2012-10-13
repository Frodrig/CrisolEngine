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
// iCCombatSystem.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCCombatSystem
//
// Descripcion:
// - Clase interfaz para CCombatSystem
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICCOMBATSYSTEM_H_
#define _ICCOMBATSYSTEM_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _COMBATSYSTEMDEFS_H_
#include "CombatSystemDefs.h"
#endif
#ifndef _RULESDEFS_H_
#include "RulesDefs.h"
#endif

// Defincion de clases / estructuras / espacios de nombres
struct iCEntitySelector;

// Clase iCCombatSystem
struct iCCombatSystem
{  
public:
  // Trabajo con metodos generales de combate 
  virtual void EndCombat(void) = 0;  
  virtual bool IsCombatActive(void) const = 0;
  virtual void NextTurn(const AreaDefs::EntHandle& hCriature) = 0;
  virtual AreaDefs::EntHandle GetCriatureInTurn(void) const = 0;
  virtual RulesDefs::CriatureActionPoints GetCriatureActionPoints(const AreaDefs::EntHandle& hCriature) = 0; 
  virtual AreaDefs::EntHandle GetCombatant(const CombatSystemDefs::eCombatAlingment& Alingment,
										   const word uwPos) = 0;
  virtual word GetNumberOfCombatants(const CombatSystemDefs::eCombatAlingment& Alingment) const = 0;
			   
public:
  // Trabajo en la alineacion de combate
  virtual void SetAlingment(const AreaDefs::EntHandle& hCriature,
						    const CombatSystemDefs::eCombatAlingment& Alingment) = 0;
  virtual bool SetAlingmentWith(const AreaDefs::EntHandle& hCriature,
								const AreaDefs::EntHandle& hCriatureFriend) = 0;
  virtual bool SetAlingmentAgainst(const AreaDefs::EntHandle& hCriature,
								   const AreaDefs::EntHandle& hCriatureEnemy) = 0;  
  virtual CombatSystemDefs::eCombatAlingment GetAlingment(const AreaDefs::EntHandle& hCriature) = 0;

public:
  // Obtencion del selector de combate
  virtual iCEntitySelector* GetCombatientSelector(const AreaDefs::EntHandle& hCriature) = 0;

public:
  // Bloqueo / desbloqueo de la contabilidad de acciones realizadas
  virtual void BlockUseOfActionPoints(const bool bBlock) = 0;
};

#endif // ~ #ifdef _ICCOMBATSYSTEM_H_
