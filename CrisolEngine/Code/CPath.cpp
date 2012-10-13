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
// CPath.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// - Consultar CPath.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CPath.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia
// Parametros:
// - TilePosSrc. Tile en la posicion inicial.
// Devuelve:
// Notas:
// - Para ver la necesidad de recibir TilePosSrc consultar las notas generales
///////////////////////////////////////////////////////////////////////////////
bool 
CPath::Init(const AreaDefs::sTilePos& TilePosSrc)
{
  // Se finaliza por si se intenta reinicializar
  End();

  // Se inserta el nodo con el tile en posicion inicial
  m_Path.push_front(sTileInfo(TilePosSrc, IsoDefs::NO_DIRECTION_INDEX));

  // Se inicializan iteradores
  m_ActPositionIt = m_Path.begin();  
  m_LastPositionRevIt = m_Path.rend();

  // Todo correcto
  m_bIsInitOk = true;
  return true;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CPath::End(void)
{
  // Finaliza
  if (IsInitOk()) {
	// Libera lista baja flag
	m_Path.clear();
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Añade un nuevo nodo de informacion al final, actualizando el iterador 
// Parametros:
// - TilePos. Posicion del tile.
// - DirToTilePos. Direccion para llegar a este tile con respecto al tile
//   anterior.
// Devuelve:
// Notas:
// - DirToTilePos. Sera importante para que el comando de movimiento sepa
//   orientar correctamente a la criatura.
///////////////////////////////////////////////////////////////////////////////
void 
CPath::AddPosition(const AreaDefs::sTilePos& TilePos,
				   const IsoDefs::eDirectionIndex& DirToTilePos)
{
  // SOLO si intancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT((DirToTilePos != IsoDefs::NO_DIRECTION_INDEX) != 0);

  // Se inserta por el final
  m_Path.push_back(sTileInfo(TilePos, DirToTilePos));

  // Se actualiza iterador a ultimo nodo
  m_LastPositionRevIt = m_Path.rbegin();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Avanza de forma el iterador que denota sobre que posicion del
//   camino estamos. Quedara en responsabilidad del exterior comprobar que 
//   se esta sobre la ultima posicion.
// - Cuando se este en el ultimo nodo NO se avanzara.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CPath::Walk(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿NO se esta en el ultimo nodo?
  if (!IsWalkCompleted()) {
	// Se avanza
	++m_ActPositionIt;
  }
}