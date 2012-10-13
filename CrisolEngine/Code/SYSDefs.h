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
// SYSDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Este fichero de cabecera incluira todos aquellos elementos que sean de
//   uso comun en cualquier parte del engine. 
//
// Notas:
// - Los tipos de datos se estableceran con #defines en lugar de con typedefs
//   para evitar conflictos de nombres en modulos con definiciones similares.
///////////////////////////////////////////////////////////////////////////////
#ifndef _SYSDEFS_H_
#define _SYSDEFS_H_

// Archivos de cabecera
// Windows
#include "stdafx.h"

extern "C"
{ // Para la implementacion de Write
  #include <stdlib.h>
  #include <stdio.h>
  #include <stdarg.h>
}

// Generales
#include "SYSAssert.h"     // Modulo para asertos
using namespace SYSAssert; // Espacio de nombres de ASSERT y sASSERT conocido
						   
typedef unsigned char byte;    // 8 bits  (ub)
typedef char sbyte;            //         (sb)
typedef unsigned short word;   // 16 bits (uw)
typedef short sword;           //         (sw)
typedef unsigned long dword;   // 32 bits (ud)
typedef long sdword;           //         (sd)
typedef unsigned __int64 qword; // 64 bits (uq)
typedef __int64 sqword;        //         (sq)
#define NULL 0

// Estructuras de uso comun
struct sRect { 
  // Define un rectangulo
  // Datos
  sword swLeft, swTop;     
  sword swRight, swBottom; 

  // Constructores
  sRect(void): swLeft(0), swTop(0), swRight(0), swBottom(0) { }
  sRect(const sRect& rRectObj): swLeft(rRectObj.swLeft), 
								swTop(rRectObj.swTop),
								swRight(rRectObj.swRight), 
								swBottom(rRectObj.swBottom) { }
  sRect(const sword& aswLeft, const sword& aswRight, 
		const sword& aswTop, const sword& aswBottom): swLeft(aswLeft),
													  swRight(aswRight),
													  swTop(aswTop),
													  swBottom(aswBottom) { }
  // Operador de asignacion  
  sRect& operator=(const sRect& rRectObj) {
	swLeft = rRectObj.swLeft;
	swRight = rRectObj.swRight;
	swTop = rRectObj.swTop;
	swBottom = rRectObj.swBottom;
	return *this;
  };

  // Operador de comparacion
  const bool operator==(const sRect& rRectObj) const {
	return (rRectObj.swLeft == swLeft && 
			rRectObj.swRight == swRight &&
			rRectObj.swTop == swTop &&
			rRectObj.swBottom == swBottom);
  }
};

struct sPosition {
  // Define una posicion
  // Datos
  sword swXPos; // Pos en X
  sword swYPos; // Pos en Y
  sword swZPos; // Pos en Z
  // Constructores
  sPosition(void): swXPos(0), swYPos(0), swZPos(0) { }  
  sPosition(const sPosition& Position) {
	swXPos = Position.swXPos;
	swYPos = Position.swYPos;
	swZPos = Position.swZPos;
  }
  sPosition(const sword& aswXPos,
			const sword& aswYPos,
			const sword& aswZPos = 0): swXPos(aswXPos), 
									   swYPos(aswYPos), 
									   swZPos(aswZPos) { }
  // Operadores
  bool operator==(const sPosition& Position) const {
	return (swXPos == Position.swXPos &&
	        swYPos == Position.swYPos &&
			swZPos == Position.swZPos);
  }
  sPosition& operator=(const sPosition& Position) {
	swXPos = Position.swXPos;
	swYPos = Position.swYPos;
	swZPos = Position.swZPos;
	return *this;
  }
  bool operator!=(const sPosition& Position) const {
	return (swXPos != Position.swXPos ||
	        swYPos != Position.swYPos ||
			swZPos != Position.swZPos);
  }
};

struct sDelta {
  // Valores deltas asociados a una posicion
  sbyte sbXDelta; // Delta en X
  sbyte sbYDelta; // Delta en Y
};

#endif // ~ #define _SYSDEFS_H_
