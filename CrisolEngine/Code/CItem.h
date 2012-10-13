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
// CItem.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CItem
//
// Descripcion:
// - Representa a una entidad de tipo item.
//
// Notas:
// - El item debera de tener siempre un dueño en caso de que se halle en el
//   inventario de una criatura o forme parte del contenido de un objeto de
//   escenario contenedor. En caso de que no tenga dueño, estara sobre el
//   suelo y el handle al dueño sera 0.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CITEM_H_
#define _CITEM_H_

// Cabeceras
#ifndef _WORLDDEFS_H_
#include "WorldDefs.h"
#endif
#ifndef _CWORLDENTITY_H_
#include "CWorldEntity.h"
#endif
#ifndef _ICWORLD_H_
#include "iCWorld.h"
#endif

// Definicion de clases / estructuras / espacios de nombres

// Clase CItem
class CItem: public CWorldEntity
{
private:
  // Tipos
  typedef CWorldEntity Inherited; // Clase base

private:
  // Vbles de miembro
  RulesDefs::sItemAttrib m_Attributes; // Atributos
  
public:
  // Constructor / destructor
  CItem(void) { }
  ~CItem(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const FileDefs::FileHandle& hFile,
			dword& udOffset,
			const AreaDefs::EntHandle& hItem);
  void End(void);

public:
  // Operacion de almacenamiento
  void Save(const FileDefs::FileHandle& hFile, 
			dword& udOffset);

public:
  // Obtencion de atributos
  inline AreaDefs::EntHandle GetOwner(void) const { 
	ASSERT(Inherited::IsInitOk());
	// Retorna el dueño
	return m_Attributes.Owner; 
  }

public:
  // Establecimiento de valores
  inline void SetOwner(const AreaDefs::EntHandle& hOwner) {
	ASSERT(Inherited::IsInitOk());
	// ASSERT(hOwner); > Supongo que 0 es sin dueño
	// Establece el dueño
	m_Attributes.Owner = hOwner;
  }

public:
  // Obtencion de la localizacion de la entidad
  AreaDefs::sTilePos GetTilePos(void) { 
	ASSERT(Inherited::IsInitOk());
	// ¿El item NO tiene dueño?
	if (0 == m_Attributes.Owner) {
	  // No, luego devuelve la posicion del item
	  return Inherited::GetTilePos();
	} else {
	  // Si, devuelve la posicion del dueño
	  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(m_Attributes.Owner);
	  ASSERT(pWorldEntity);
	  return pWorldEntity->GetTilePos();
	}
  }

public:
  // CSprite / Operaciones sobre desplazamiento / posicion  
  float GetXPos(void) const { 
	ASSERT(IsInitOk());
	// ¿El item NO tiene dueño?
	if (0 == m_Attributes.Owner) {
	  // No, luego devuelve la posicion en X del item
	  return Inherited::GetXPos();
	} else {
	  // Si, devuelve la posicion en X del dueño
	  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(m_Attributes.Owner);
	  ASSERT(pWorldEntity);
	  return pWorldEntity->GetXPos();
	}
  }
  float GetYPos(void) const { 
	ASSERT(IsInitOk());
	// ¿El item NO tiene dueño?
	if (0 == m_Attributes.Owner) {
	  // No, luego devuelve la posicion en X del item
	  return Inherited::GetYPos();
	} else {
	  // Si, devuelve la posicion en X del dueño
	  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(m_Attributes.Owner);
	  ASSERT(pWorldEntity);
	  return pWorldEntity->GetYPos();
	}
  }

public:
  // Obtencion de informacion particular de la instancia
  inline RulesDefs::ItemClass GetClass(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna la clase
	return m_Attributes.Class;
  }

public:
  // CWorldEntity / Obtencion del atributo Type
  RulesDefs::BaseType GetType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el atributo tipo de la entidad
	return m_Attributes.Type;
  }

public:
  // CWorldEntity / Obtencion del contenedor
  inline iCItemContainer* const GetItemContainer(void) {
	ASSERT(Inherited::IsInitOk());
	// Los items no tendran contenedor
	return NULL;
  }
  inline AreaDefs::MaskTileAccess GetObstacleMask(void) const { 
	ASSERT(Inherited::IsInitOk());
	// Se retorna obstaculizacion en tiles
	return AreaDefs::ALL_TILE_ACCESS;
  }

public:
  // CWorldEntity / Obtencion de flag de interaccion
  bool IsInteractuable(void) const {
	ASSERT(Inherited::IsInitOk());
	// Los items siempre seran interactuables
	return true;
  }

public:
  // CEntity / Obtencion de informacion
  inline RulesDefs::eEntityType GetEntityType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se devuelve tipo de entidad
	return RulesDefs::ITEM;
  }

public:
  // Obtencion de valores propios
  inline RulesDefs::ItemGlobalAttrib GetGlobalAttribute(const byte ubIdxGlobal) const {
	ASSERT((ubIdxGlobal < RulesDefs::MAX_ITEM_GLOBALATTRIB) != 0);
	// Retorna valor
	return m_Attributes.GlobalAttribs[ubIdxGlobal];
  }
  inline void SetGlobalAttribute(const byte ubIdxGlobal,
								 const RulesDefs::ItemGlobalAttrib& Value) {
	ASSERT((ubIdxGlobal < RulesDefs::MAX_ITEM_GLOBALATTRIB) != 0);
	// Establece valor
	m_Attributes.GlobalAttribs[ubIdxGlobal] = Value;
  }
  inline RulesDefs::ItemLocalAttrib GetLocalAttribute(const byte ubIdxLocal) const {
	ASSERT((ubIdxLocal < RulesDefs::MAX_ITEM_LOCALATTRIB) != 0);
	// Retorna valor
	return m_Attributes.LocalAttribs[ubIdxLocal];
  }
  inline void SetLocalAttribute(const byte ubIdxLocal,
								const RulesDefs::ItemLocalAttrib& Value) {
	ASSERT((ubIdxLocal < RulesDefs::MAX_ITEM_LOCALATTRIB) != 0);
	// Establece valor
	m_Attributes.LocalAttribs[ubIdxLocal] = Value;
  }
};

#endif // ~ #ifdef _CITEM_H_
