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
// FontDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Tipos asociados al trabajo con fuentes (CFont)
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _FONTDEFS_H_
#define _FONTDEFS_H_

#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif

// Namespace FontDefs
namespace FontDefs
{
  // Constantes
  const byte CHAR_PIXEL_HEIGHT = 16;    // Pixels de altura para fuentes
  const byte CHAR_PIXEL_MAX_WIDTH = 16; // Anchura maxima en pixels de los caracteres

  // Enumerados
  enum eCharPoints { 
	// Puntos de resolucion asociados a la fuente en decipuntos
    FONT_CHAR_POINT_10 = 10,
    FONT_CHAR_POINT_9 = 9,  
    FONT_CHAR_POINT_8 = 8   
  };  
};


#endif // ~ #ifdef _FONTDEFS_H_
