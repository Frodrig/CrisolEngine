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
// CSceneObj.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CSceneObj
//
// Descripcion:
// - Representa a una entidad de tipo objeto de escenario contenedor y no
//   contenedor. La clase no contenedor simplemente posee un contenedor
//   de items, sobrecargando el metodo que devuelve la direccion del contenedor
//   asociado. En el caso de un objeto de escenario no contenedor, lo que se
//   hara sera devolver NULL.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CSCENEOBJ_H_
#define _CSCENEOBJ_H_

// Cabeceras
#ifndef _CWORLDENTITY_H_
#include "CWorldEntity.h"
#endif
#ifndef _CMEMORYPOOL_H_
#include "CMemoryPool.h"
#endif
#ifndef _CITEMCONTAINER_H_
#include "CItemContainer.h"
#endif

// Definicion de clases / estructuras / espacios de nombres

// Clase CSceneObj
class CSceneObj: public CWorldEntity
{
private:
  // Tipos
  typedef CWorldEntity Inherited; // Clase base

private:
  // Estructuras 
  struct sContainer {
	// Contenedor
	CItemContainer Content; // Contenido
	// Manejador de memoria  
	static CMemoryPool m_MPool;
	static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
	static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); }   
  };

private:
  // Vbles de miembro
  RulesDefs::sSceneObjAttrib m_Attributes;       // Atributos
  sContainer*                m_pContainer;       // Contenedor
  bool                       m_bIsInteractuable; // Flag de interaccion
  bool                       m_bAccesibleFloor;  // Flag de acceso a la celda
    
public:
  // Constructor / destructor
  CSceneObj(void): m_pContainer(NULL) { }
  ~CSceneObj(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const FileDefs::FileHandle& hFile,
			dword& udOffset,
			const AreaDefs::EntHandle& hSceneObj);
  void End(void);

public:
  // Operacion de almacenamiento
  void Save(const FileDefs::FileHandle& hFile, 
			dword& udOffset);

public:
  // CWorldEntity / Obtencion de informacion  
  inline AreaDefs::MaskTileAccess GetObstacleMask(void) const { 
	ASSERT(Inherited::IsInitOk());
	// Se retorna mascara segun el flag de acceso a floor que se posea
	return m_bAccesibleFloor ? AreaDefs::ALL_TILE_ACCESS : AreaDefs::NO_TILE_ACCESS;
  }
  
public:
  // CWorldEntity / Obtencion del contenedor
  inline iCItemContainer* const GetItemContainer(void) {
	ASSERT(Inherited::IsInitOk());
	// Se retorna la direccion del contenedor si es que lo hay
	return (m_pContainer) ? &m_pContainer->Content : NULL;	
  } 

public:
  // CWorldEntity / Obtencion de flag de interaccion
  bool IsInteractuable(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el flag de interaccion
	return m_bIsInteractuable;
  }

public:
  // Obtencion de informacion
  inline RulesDefs::SceneObjClass GetClass(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se devuelve el identificador de tipo
	return m_Attributes.Class;
  }

public:
  // CEntity / Obtencion de informacion
  inline RulesDefs::eEntityType GetEntityType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se devuelve tipo de entidad
	return RulesDefs::SCENE_OBJ;
  }
  
public:
  // CWorldEntity / Obtencion del atributo Type
  RulesDefs::BaseType GetType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el atributo tipo de la entidad
	return m_Attributes.Type;
  }

public:
  // Obtencion de valores propios
  inline RulesDefs::SceneObjLocalAttrib GetLocalAttribute(const byte ubIdxLocal) const {
	ASSERT((ubIdxLocal < RulesDefs::MAX_SCENEOBJ_LOCALATTRIB) != 0);
	// Retorna valor
	return m_Attributes.LocalAttribs[ubIdxLocal];
  }
  inline void SetLocalAttribute(const byte ubIdxLocal,
								const RulesDefs::SceneObjLocalAttrib& Value) {
	ASSERT((ubIdxLocal < RulesDefs::MAX_SCENEOBJ_LOCALATTRIB) != 0);
	// Establece valor
	m_Attributes.LocalAttribs[ubIdxLocal] = Value;
  }
};
#endif