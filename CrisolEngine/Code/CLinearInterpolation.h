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
// CLinearInterpolation.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CLinearInterpolation
//
// Descripcion:
// - Se encarga de realizar una interpolacion linean entre un punto origen y
//   un punto destino. Necesariamente, a este metodo se le debera de llamar
//   en cada actualizacion. Esperara recibir el valor:
//   (fActDelta - fLastDelta) * AISecTickTime
//   que sera calculado desde CCRISOLEngine.
// - La clase trabajara con direcciones de movimiento positivas (To > From) y
//   negativas (To < From).
//
// Notas:
// - El valor fRemainingTime se podra utilizar para ajustar la instancia cuando
//   una vez que haya acabado, se quiera continuar con la interpolacion
//   moviendo a otro punto. Bastara con inicializar asi:
//   Init(From, To, Time - (GetRemainingTime() - Time)
// - Cuando se quieran medir frames por segundo, la forma de inicializar
//   la clase sera asi: Init(0, 1, 1/fps), donde 1/fps indica que queremos
//   ir de 0 a 1 en la fraccion de tiempo en la que se divide 1 segundo al
//   usar los fps dados.
//   Cuando se quiera medir desplazamiento entre una distancia en pixels por
//   segundo: Init(posInit, posInit + PixelsADesplazar, 1), donde 1 indica
//   que ese desplazamiento se quiere hacer en 1 segundo.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CLINEARINTERPOLATION_H_
#define _CLINEARINTERPOLATION_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif

// Definicion de clases / estructuras / espacios de nombres

// Clase CLinearInterpolation
class CLinearInterpolation
{
private:
  // Enums
  enum eDirection { 
	// Direccion de interpolacion
	RIGHT_TO_LEFT, // Cuando To es mayor que From
    LEFT_TO_RIGHT  // Cuando To es menor que From
  };

private:
  // Vbles de miembro
  float      m_fRemainingTime; // Tiempo que queda para finalizar interpolacion
  float      m_fActPos;        // Posicion actual
  float      m_fLastStep;      // Ultima distancia recorrida
  float      m_fDistPerSec;    // Distancia recorrida en un segundo	  
  eDirection m_DirMov;         // Sentido de movimiento

  
public:
  // Constructor / destructor
  CLinearInterpolation(void): m_fRemainingTime(0.0f),
							  m_fActPos(0.0f),
							  m_fDistPerSec(0.0f),
							  m_fLastStep(0.0f) { }
  ~CLinearInterpolation(void) { }

public:
  // Protocolos de inicializacion / fin
  void Init(const float fInitPos, const float fEndPos, const float fTime);  

public:
  // Realizacion de la interpolacion
  bool Interpolate(const float fDeltaTime);

public:
  // Obtencion de informacion
  inline float GetActPos(void) const { return m_fActPos; }
  inline float GetLastStep(void) const { return m_fLastStep; }
  inline float GetRemainingTime(void) const { return m_fRemainingTime; }
};

#endif // ~ CLinearInterpolation

