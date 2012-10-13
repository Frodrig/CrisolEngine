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
// CPath.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CPath
//
// Descripcion:
// - Mantiene y permite gestionar un camino creador por el CPathFinder.
//
// Notas:
// - La clase funcionara mediante llamadas al metodo Walk, que recorrera
//   el camino generado. Cada info asociada a una posicion en
//   el camino, tendra posicion del tile al que hace referencia y direccion
//   que hay que tomar desde el tile anterior.
// - El tile origen se requerira unicamente para mantener la coherencia 
//   al llamarse por primera vez a Walk, que pasara de esta posicion a la
//   posicion siguiente.
// - Cuando se llegue al ultimo tile, el metodo WALK no realizara accion alguna.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CPATH_H_
#define _CPATH_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _ISODEFS_H_
#include "IsoDefs.h"
#endif
#ifndef _LIST_H_
#include <list>
#define _LIST_H_
#endif

// Definicion de clases / estructuras / espacios de nombres

// Clase CPath
class CPath
{
private:
  // Estructuras
  struct sTileInfo {
	// Informacion referida a un tile a visitar
	AreaDefs::sTilePos       TilePos;           // Posicion del tile de viaje	
	IsoDefs::eDirectionIndex DirFromPrevToThis; // Direccion para llegar al tile de viaje
	// Constructor
	sTileInfo(const AreaDefs::sTilePos& aTilePos, 
			  const IsoDefs::eDirectionIndex& Dir): TilePos(aTilePos), 
											  DirFromPrevToThis(Dir) { }
	// Operadores
	bool operator==(const sTileInfo& TileInfo) const {
	  return (TilePos == TileInfo.TilePos && 
			  DirFromPrevToThis == TileInfo.DirFromPrevToThis);
	}
  };

private:
  // Tipos
  // Lista a los nodos de informacion
  typedef std::list<sTileInfo>       PathList;
  typedef PathList::iterator         PathListIt;
  typedef PathList::reverse_iterator PathListRevIt;
  
private:
  // Vbles de miembro
  PathList      m_Path;              // Camino
  PathListIt    m_ActPositionIt;     // Iterador a la posicion actual en el camino
  PathListRevIt m_LastPositionRevIt; // Iterador a la ultima posicion en el camino
  bool          m_bIsInitOk;         // ¿Clase inicializada?

public:
  // Constructor / destructor
  CPath(void): m_bIsInitOk(false) { }
  ~CPath(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const AreaDefs::sTilePos& TilePosSrc);
  void End(void);
  bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // Insercion de una nueva posicion
  void AddPosition(const AreaDefs::sTilePos& TilePos,
				   const IsoDefs::eDirectionIndex& DirToTilePos);

public:
  // Operacion de desplazamiento
  void Walk(void);

public:
  // Obtencion de informacion en la posicion actual
  inline AreaDefs::sTilePos GetActTilePos(void) const {
	ASSERT(IsInitOk());
	// Se retorna la posicion actual
	return m_ActPositionIt->TilePos;
  }
  inline IsoDefs::eDirectionIndex GetDirToActTilePos(void) const {
	ASSERT(IsInitOk());
	// Se retorna la direccion al tile actual
	return m_ActPositionIt->DirFromPrevToThis;
  }

public:
  // Comprobacion del estado del "viaje"
  inline bool IsWalkCompleted(void) const {
	ASSERT(IsInitOk());
	// Retorna flag con la comprobacion
	return (*m_ActPositionIt == *m_LastPositionRevIt);
  }

public:
  // Devuelve el tamaño del camino
  inline word GetSize(void) const {
	ASSERT(IsInitOk());
	// Retorna el tamaño
	return m_Path.size();
  }
};

#endif // ~ #ifdef _CPATH_H_
