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
// CEquipmentSlots.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CEquipmentSlots.
//
// Descripcion:
// - Clase que representa los slots de equipamiento de una criatura.
// - Internamente no sera mas que un contenedor modelado con un vector.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CEQUIPMENTSLOTS_H_
#define _CEQUIPMENTSLOTS_H_

// Cabeceras
#ifndef _ICEQUIPMENTSLOTS_H_
#include "iCEquipmentSlots.h"
#endif
#ifndef _VECTOR_H_
#define _VECTOR_H_
#include <vector>
#endif
#ifndef _ALGORTIHM_H_
#define _ALGORTIHM_H_
#include <algorithm>
#endif
#ifndef _FILEDEFS_H_
#include "FileDefs.h"
#endif

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Definicion de clases / estructuras / espacios de nombres

// Clase CEquipmentSlots
class CEquipmentSlots: public iCEquipmentSlots
{
private:
  // Tipos  
  // Vector con los slots de equipamiento
  typedef std::vector<AreaDefs::EntHandle> EquipSlotsVector;
  typedef EquipSlotsVector::iterator       EquipSlotsVectorIt;

private:
  // Estructuras
  struct sEquipmentSlotsInfo {
	// Informacion asociada al container
	AreaDefs::EntHandle hOwner;     // Propietario del contenedor
	EquipSlotsVector    EquipSlots; // Contenedor con los slots de equipamiento
  };

private:
  // Vbles de miembro
  sEquipmentSlotsInfo       m_EquipSlotsInfo; // Info asociada a los slots de equip.
  iCEquipmentSlotsObserver* m_pObserver;      // Posible observer asociado
  bool				 	    m_bIsInitOk;      // ¿Clase inicializada?
    
public:
  // Constructor / destructor
  CEquipmentSlots(void): m_pObserver(NULL),
					     m_bIsInitOk(false)/*,
						 m_bIsInPause(false)*/ { }
  ~CEquipmentSlots(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const AreaDefs::EntHandle& hOwner);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // Operacion de almacenamiento
  void Save(const FileDefs::FileHandle& hFile, 
			dword& udOffset);

public:
  // Equipar
  AreaDefs::EntHandle EquipItem(const AreaDefs::EntHandle& hItem,
							    const RulesDefs::EquipmentSlot& Slot);

public:
  // iCEquipmentSlot / Desequipar
  AreaDefs::EntHandle UnequipItem(const RulesDefs::EquipmentSlot& Slot);   

public:
  // iCEquipmentSlots / Obtencion de informacion
  bool WhereIsItemEquiped(const AreaDefs::EntHandle& hItem,
						  RulesDefs::EquipmentSlot& Slot);
  inline AreaDefs::EntHandle GetItemEquipedAt(const RulesDefs::EquipmentSlot& Slot) const {
	ASSERT(IsInitOk());
	// Obtiene el item equipado en Slot
	return m_EquipSlotsInfo.EquipSlots[Slot];
  }
  inline bool IsItemEquiped(const AreaDefs::EntHandle& hItem) const {
	ASSERT(IsInitOk());
	// Comprueba si hItem esta equipado
	return (std::find(m_EquipSlotsInfo.EquipSlots.begin(),
					  m_EquipSlotsInfo.EquipSlots.end(),
					  hItem) != m_EquipSlotsInfo.EquipSlots.end());
  }    
  
public:
  // iCEquipmentSlots / Trabajo con observadores
  inline bool AddObserver(iCEquipmentSlotsObserver* const pObserver) {
	ASSERT(IsInitOk());
	// Se añade solo si no hay asociado y retorna
	if (!m_pObserver) {
	  m_pObserver = pObserver;	  
	}
	return m_pObserver ? true : false;
  }
  inline bool RemoveObserver(iCEquipmentSlotsObserver* const pObserver) {
	ASSERT(IsInitOk());
	// Elimina solo si el observer actual es igual a pObserver y retorna
	if (pObserver == m_pObserver) {
	  m_pObserver = NULL;
	}
	return (!m_pObserver) ? true : false;
  }
private:
  // Metodos de apoyo
  inline void ObserverNotify(const EquipmentSlotsDefs::eObserverNotifyType& NotifyType,
							 const RulesDefs::EquipmentSlot& Slot) {
	ASSERT(IsInitOk());
	// Notifica si procede
	if (m_pObserver) {
	  m_pObserver->EquipmentSlotNotify(NotifyType, Slot);
	}
  }

public:
  // Trabajo con la pausa
  void SetPause(const bool bPause);
}; // ~ CEquipmentSlots

#endif