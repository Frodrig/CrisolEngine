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
// iCMathUtil.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCMathUtil
//
// Descripcion:
// - Interfaz de CMathUtil.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICMATHUTIL_H_
#define _ICMATHUTIL_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif

// Definicion de clases / estructuras / espacios de nombres

// Interfaz iCMath
struct iCMathUtil
{
public:
  // Operaciones trigonometricas
  virtual float Sin(const word uwAngle) const = 0; 
  virtual float Cos(const word uwAngle) const = 0;   

public:
  // Operaciones con numeros aleatorios
  virtual void GenSeed(void) const = 0;
  virtual void GenSeed(const word uwSeed) const = 0;
  virtual sword GetRandValue(const sword& swMin, 
							 const sword& swMax) const = 0;
  
public:
  // iCMathUtil / Operaciones con areas
  virtual bool IsPointInRect(const sPosition& Position,
							 const sRect& rRect) const = 0;
  virtual bool RectsCollision(const sRect& FirstRect,
							  const sRect& SecondRect) const = 0;
public:
  // iCMathUtil / Distancias
  virtual sword GetEuclideanDistance(const sPosition& SrcPos,
									 const sPosition& DestPos) const = 0;
  
public:
  // iCMathUtil / Trabajo con valores sueltos
  virtual bool IsEven(const sword& swValue) const = 0;
};

#endif // ~ #ifdef _ICMATHUTIL_H_
