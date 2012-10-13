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
// IsoDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Espacio de nombres con todas las definiciones, tipos y estructuras 
//   generales para el trabajo con CIsoEngine.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ISODEFS_H_
#define _ISODEFS_H_

#include <cmath>

namespace IsoDefs
{
  // Constantes   
  // Anchura de los tiles
  const byte TILE_WIDTH = 64;
  // Altura de los tiles    
  const byte TILE_HEIGHT = 32;
  // Dimension de un tile
  const word TILE_DIM = TILE_WIDTH * TILE_HEIGHT; 
  // Mitad de la anchura de un tile
  const byte TILE_WIDTH_DIV = TILE_WIDTH >> 1;    
  // Mitad de la altura de un tile  
  const byte TILE_HEIGHT_DIV = TILE_HEIGHT >> 1;  
  
  const float ISO_ANGLE = 26.56505117f;
  
  // Num. maximo de orientaciones
  const byte MAX_DIRECTIONS = 8; 
  
  // Tipos enumerados
  enum eDirectionIndex {
	// Indices para las distintas direcciones.
	// Los indices sirven para ser usados en arrays, contadores y similares
	NORTH_INDEX = 0,
    NORTHEAST_INDEX,
	EAST_INDEX,
	SOUTHEAST_INDEX,
	SOUTH_INDEX,
	SOUTHWEST_INDEX,
	WEST_INDEX,
	NORTHWEST_INDEX,
	NO_DIRECTION_INDEX
  };

  enum eDirectionFlag { 
	// Flags de direcciones posibles
	// Los flags sirven para trabajar a nivel de bit
    NORTH_FLAG       = 0X01,	
    NORTHEAST_FLAG   = 0X02,	
    EAST_FLAG        = 0X04, 
    SOUTHEAST_FLAG   = 0X08, 
    SOUTH_FLAG       = 0X10, 
    SOUTHWEST_FLAG   = 0X20, 
    WEST_FLAG        = 0X40, 
    NORTHWEST_FLAG   = 0X80,
	NODIRECTION_FLAG = 0x00
  };
  
  enum eDrawPlane {
	// Planos de dibujado para cada tipo de entidad
	FLOOR_DRAW_PLANE =       0x00,
	FLOOR_ADDON_DRAW_PLANE = 0X00,
	WORLDENTITY_DRAW_PLANE = 0X00,
	ROOF_DRAW_PLANE        = 0X00,
  };

  enum eSelectCellType {
	// Tipo de celda seleccionada
	// Nota: SCT = SelectCellType
	SCT_NOVALIDCELL = -1, // Celda no valida (no existe o no tiene contenido)
	SCT_NOACCESIBLECELL,  // Celda valida pero NO accesible
	SCT_ACCESIBLECELL     // Celda valida y accesible

  };
};

#endif // ~ #ifdef _ISODEFS_H_
