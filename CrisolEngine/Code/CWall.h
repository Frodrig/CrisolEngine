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
// CWall.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CWall
//
// Descripcion:
// - Representa una entidad de tipo pared. Las paredes
//   podran ser de tres orientaciones posibiles: al suroeste, al frente y al
//   sureste. 
// - Por defecto las parades se hallaran bloqueadas
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CWALL_H_
#define _CWALL_H_

// Cabeceras
#ifndef _CWORLDENTITY_H_
#include "CWorldEntity.h"
#endif

// Definicion de clases / estructuras / espacios de nombres

// Clase CWall
class CWall: public CWorldEntity
{
private:
  // Tipos
  typedef CWorldEntity Inherited; // Nombre de la clase base

private:
  // Vbles de miembro
  RulesDefs::sWallAttrib   m_Attributes;       // Atributos
  AreaDefs::MaskTileAccess m_WallAccess;       // Acceso segun bloqueo / desbloqueo
  bool                     m_bIsInteractuable; // Flag de interaccion

public:
  // Constructor / destructor
  CWall(void) { }
  ~CWall(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const FileDefs::FileHandle& hFile,
			dword& udOffset,
			const AreaDefs::EntHandle hWall);
  void End(void);

public:
  // Operacion de almacenamiento
  void Save(const FileDefs::FileHandle& hFile, 
			dword& udOffset);

public:
  // Bloqueo / desbloqueo del tile donde se halla la pared
  void BlockAccess(void);
  inline void UnblockAccess(void) {
	ASSERT(Inherited::IsInitOk());
	// Simplemente, permitimos todos los accesos
	m_WallAccess = AreaDefs::ALL_TILE_ACCESS;
  }
  inline bool IsBlocked(void) const {
	ASSERT(Inherited::IsInitOk());
	return (AreaDefs::ALL_TILE_ACCESS != m_WallAccess);
  }

public:
  // CWorldEntity / Obtencion de informacion  
  inline AreaDefs::MaskTileAccess GetObstacleMask(void) const { 
	ASSERT(Inherited::IsInitOk());
	// Se retorna obstaculizacion
	return m_WallAccess;
  }

public:
  // CWorldEntity Obtencion del contenedor
  inline iCItemContainer* const GetItemContainer(void) {
	ASSERT(Inherited::IsInitOk());
	// Las paredes no tendran contenedor
	return NULL;
  }

public:
  // Obtencion de la orientacion
  inline RulesDefs::eWallOrientation GetOrientation(void) const {
	ASSERT(Inherited::IsInitOk());
	// Devuelve la orientacion
	return m_Attributes.Orientation;
  }

public:
  // CWorldEntity / Obtencion de flag de interaccion
  bool IsInteractuable(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el flag de interaccion
	return m_bIsInteractuable;
  }

public:
  // CWorldEntity / Obtencion del atributo Type
  RulesDefs::BaseType GetType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el atributo tipo de la entidad
	return m_Attributes.Type;
  }

public:
  // CEntity / Obtencion de informacion
  inline RulesDefs::eEntityType GetEntityType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se devuelve tipo de entidad
	return RulesDefs::WALL;
  }

public:
  // Obtencion de valores propios
  inline RulesDefs::WallLocalAttrib GetLocalAttribute(const byte ubIdxLocal) const {
	ASSERT((ubIdxLocal < RulesDefs::MAX_WALL_LOCALATTRIB) != 0);
	// Retorna valor
	return m_Attributes.LocalAttribs[ubIdxLocal];
  }
  inline void SetLocalAttribute(const byte ubIdxLocal,
							    const RulesDefs::WallLocalAttrib& Value) {
	ASSERT((ubIdxLocal < RulesDefs::MAX_WALL_LOCALATTRIB) != 0);
	// Establece valor
	m_Attributes.LocalAttribs[ubIdxLocal] = Value;
  }
};
#endif