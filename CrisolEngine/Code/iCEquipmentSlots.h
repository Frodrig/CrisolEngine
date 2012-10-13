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
// iCEquipmentSlots.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCEquipmentSlots
// - iCEquipmentSlotsObserver
//
// Descripcion:
// - Define la interfaz para la clase CEquipmentSlots y la interfaz para 
//   todos los obervadores de la misma.
// - Se declarara CArea como friend de iCEquipmentSlot para permitir que pueda
//   realizar llamadas a la extraccion directa de items equipados. Esto sera
//   necesario en el proceso de destruccion de una entidad
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICEQUIPMENTSLOTS_H_
#define _ICEQUIPMENTSLOTS_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _EQUIPMENTSLOTSDEFS_H_
#include "EquipmentSlotsDefs.h"
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _RULESDEFS_H_
#include "RulesDefs.h"
#endif

// Defincion de clases / estructuras / espacios de nombres
struct iCEquipmentSlotsObserver;
class CArea;

// Interfaz iCEquipmentSlots
struct iCEquipmentSlots
{
private:
  friend CArea;

public:
  // Obtencion de informacion
  virtual AreaDefs::EntHandle GetItemEquipedAt(const RulesDefs::EquipmentSlot& Slot) const = 0;
  virtual bool IsItemEquiped(const AreaDefs::EntHandle& hItem) const = 0;
  virtual bool WhereIsItemEquiped(const AreaDefs::EntHandle& hItem,
								  RulesDefs::EquipmentSlot& Slot) = 0;

public:
  // Desequipar
  virtual AreaDefs::EntHandle UnequipItem(const RulesDefs::EquipmentSlot& Slot) = 0;   

public:
  // Trabajo con observadores
  virtual inline bool AddObserver(iCEquipmentSlotsObserver* const pObserver) = 0;
  virtual inline bool RemoveObserver(iCEquipmentSlotsObserver* const pObserver) = 0; 
}; // ~ iCEquipmentSlots

// Interfaz iCEquipmentSlotsObserver
struct iCEquipmentSlotsObserver 
{
public:
  // Operacion de notificacion
  virtual void EquipmentSlotNotify(const EquipmentSlotsDefs::eObserverNotifyType& NotifyType,
								   const RulesDefs::EquipmentSlot& Slot) = 0;
}; // ~ iCEquipmentSlotsObserver

#endif // ~ #ifdef _ICEQUIPMENTSLOTS_H_
