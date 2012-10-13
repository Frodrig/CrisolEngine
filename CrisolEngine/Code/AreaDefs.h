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
// AreaDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Definiciones para el trabajo con areas
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _AREADEFS_H_
#define _AREADEFS_H_

// Cabeceras

// Espacio de nombres
namespace AreaDefs
{  
  // Tipos
  typedef word  EntHandle;      // Handle a tipo de entidad
  typedef sword TilePosType;    // Tipo asociado a una posicion de tile
  typedef word  TileIndex;      // Posicion absoluta de un tile 
  typedef byte  MaskTileAccess; // Mascara de acceso a tile
  typedef word  RoomID;         // Identificador de habitacion

  // Constantes
  const MaskTileAccess NO_TILE_ACCESS  = 0XFF;   // No hay acceso
  const MaskTileAccess ALL_TILE_ACCESS = 0X00;   // Todos los accesos  
  const word           MAXROOFS        = 0xFFFF; // Num. maximo de roofs  
  const word           MAX_AREA_WIDTH  = 256;    // Maxima anchura de un area
  const word           MAX_AREA_HEIGHT = 256;    // Maxima anchura de un area

  // Tipos enumerados
  enum {
	// Flags de dibujado para trabajo con el metodo Draw
	DRAW_FLOOR     = 0X01, // Dibuja el tile base
	DRAW_SCENE_OBJ = 0X02, // Dibuja objetos de escenario
	DRAW_ITEM      = 0X04, // Dibuja items
	DRAW_CRIATURE  = 0x08, // Dibuja criaturas
	DRAW_WALL      = 0X10, // Dibuja paredes
	DRAW_ROOF      = 0X20, // Dibuja techos
	DRAW_GFX       = 0x40  // Dibujado de GFXs
  };

  // Estructuras
  struct sTilePos {
	// Representa una posicion dentro del mapa
	TilePosType XTile;
	TilePosType YTile;
	// Constructores
	sTilePos(void): XTile(0), YTile(0) { }
	sTilePos (const sTilePos& Tile) {
	  XTile = Tile.XTile;
	  YTile = Tile.YTile;
	}
	sTilePos(const TilePosType& aXTile, 
		     const TilePosType& aYTile): XTile(aXTile), YTile(aYTile) { }
	// Operadores
	sTilePos& operator=(const sTilePos& Tile) {
	  XTile = Tile.XTile;
	  YTile = Tile.YTile;
	  return *this;
	}
	bool operator==(const sTilePos& Tile) const {
	  return (Tile.XTile == XTile && Tile.YTile == YTile);
	}
	bool operator!=(const sTilePos& Tile) const {
	  return (Tile.XTile != XTile || Tile.YTile != YTile);
	}
  };
};

#endif // ~ #ifdef _AREADEFS_H_
