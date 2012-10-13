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
// CMathUtil.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CMathUtil
//
// Descripcion:
// - Clase que implementa una serie de metodos matematicos.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CMATHUTIL_H_
#define _CMATHUTIL_H_

// Cabeceras
#ifndef _ICMATHUTIL_H_
#include "iCMathUtil.h"
#endif
#ifndef _SYSENGINE_H_
#include "SYSEngine.h"
#endif
#ifndef _MATH_H_
#define _MATH_H_
#include "math.h"
#define _MATH_H_
#endif
#ifndef _TIME_H_
#include "time.h"
#define _TIME_H_
#endif

#include "iCLogger.h"

// Clase CMath
class CMathUtil: public iCMathUtil
{
private:
  // Constantes  
  const float PI; // Numero PI

private:  
  // Vbles de miembro
  float SinTable[360]; // Tabla trigonometrica del seno
  float CosTable[360]; // Tabla trigonometrica del coseno
  bool  m_bIsInitOk;   // ¿Clase inicializada correctamente?
  
public:
  // Constructor / destructor
  CMathUtil(void): PI(3.141592654f),
				   m_bIsInitOk(false) { }
  ~CMathUtil(void) { End(); }

public:
  // Protocolo de inicializacion y fin
  bool Init(void);
  void End(void);
  bool IsInitOk(void) const { return m_bIsInitOk; }    
private:
  // Metodos de apoyo
  void InitTrigonometricTables(void);  

public:
  // iCMathUtil / Operaciones trigonometricas
  float Sin(const word uwAngle) const {    
    ASSERT((uwAngle < 360) != 0);
	// Retorna valor de la tabla
    return SinTable[uwAngle]; 
  }
  float Cos(const word uwAngle) const {      
    ASSERT((uwAngle < 360) != 0);
	// Retorna valor de la tabla
    return CosTable[uwAngle]; 
  }

public:
  // iCMahtUtil / Numeros aleatorios
  void GenSeed(void) const { 
	ASSERT(IsInitOk());    
    // Genera semilla usando el time    
    srand((unsigned)time(NULL));
  }
  void GenSeed(const word uwSeed) const {
	ASSERT(IsInitOk());    
    // Genera semilla a partir de un valor de usuario    
    srand(uwSeed);
  }
  sword GetRandValue(const sword& swMin, 
					 const sword& swMax) const {  
	ASSERT(IsInitOk());    
    ASSERT((swMin <= swMax) != 0);        
    // Retorna un valor aleatorio entre un minimo y un maximo  
	//uwMin + ((uwMax - uwMin) * rand() / RAND_MAX);		
	return (swMin + (word)((word) swMax * rand() / (RAND_MAX + 1.0f)));
  }

public:
  // iCMathUtil / Operaciones con areas
  bool IsPointInRect(const sPosition& Position,
					 const sRect& rRect) const {
	ASSERT(IsInitOk());
  // ¿Esta el punto en el rectangulo?
  // Nota: No se consideraran incluidos los extremos derecho e inferior
  return (Position.swXPos >= rRect.swLeft && 
		  Position.swXPos < rRect.swRight &&
		  Position.swYPos >= rRect.swTop && 
		  Position.swYPos < rRect.swBottom) ? true : false;

  }
  bool RectsCollision(const sRect& FirstRect,
					  const sRect& SecondRect) const {
	ASSERT(IsInitOk());
    // ¿NO hay colision entre los rectangulos recibidos?
    return (FirstRect.swLeft > SecondRect.swRight || 
			SecondRect.swLeft > FirstRect.swRight ||
			FirstRect.swTop > SecondRect.swBottom || 
			SecondRect.swTop > FirstRect.swBottom) ? false : true;
  }

public:
  // iCMathUtil / Distancias
  sword GetEuclideanDistance(const sPosition& SrcPos,
							 const sPosition& DestPos) const {
	ASSERT(IsInitOk());
	// Se calcula la distancia euclidea entre dos puntos
	const sword swXDelta = (SrcPos.swXPos > DestPos.swXPos) ?
							SrcPos.swXPos - DestPos.swXPos :
						    DestPos.swXPos - SrcPos.swXPos;
	const sword swYDelta = (SrcPos.swYPos > DestPos.swYPos) ?
							SrcPos.swYPos - DestPos.swYPos :
						    DestPos.swYPos - SrcPos.swYPos;
	return sqrt((swXDelta * swXDelta) + (swYDelta * swYDelta));
  }
  
public:
  // iCMathUtil / Trabajo con valores sueltos
  bool IsEven(const sword& swValue) const {
	ASSERT(IsInitOk());
	// Retorna flag de valor impar
	return (0 == (swValue & 1)) ? true : false;
  }
};

#endif // ~ #ifdef _CMATHUTIL_H_