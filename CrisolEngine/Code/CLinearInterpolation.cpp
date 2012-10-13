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
// CLinearInterpolation.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CLinearInterpolation.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CLinearInterpolation.h"

#include "SYSEngine.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia recibiendo la posicion de partida, la final y el
//   tiempo que se debera de invertir para alcanzarla.
// Parametros:
// - fInitPos. Posicion inicial.
// - fEndPos. Posicion final.
// - fTime. Tiempo a invertir.
// Devuelve:
// Notas:
// - La clase siempre trabajara con posiciones y distancias positivas. Si 
//   desde el exterior es necesario tener estos valores en negativo, se dejara
//   en responsabilidad del exterior realizar las conversiones oportunas.
///////////////////////////////////////////////////////////////////////////////
void
CLinearInterpolation::Init(const float fInitPos, 
						   const float fEndPos, 
						   const float fTime)
{
  // SOLO si parametros correctos  
  ASSERT((fInitPos != fEndPos) != 0);

  // Se comprueba la direccion de movimiento y se halla la distancia
  // recorrida en un segundo
  if (fInitPos < fEndPos) { 
	m_DirMov = RIGHT_TO_LEFT;
	m_fDistPerSec = (fEndPos - fInitPos) / fTime;
  } else { 
	m_DirMov = LEFT_TO_RIGHT;
	m_fDistPerSec = (fInitPos - fEndPos) / fTime;
  }

  // Se establece tiempo restante y posicion actual
  m_fRemainingTime = fTime;
  m_fActPos = fInitPos;
  m_fLastStep = 0.0f;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la operacion de interpolacion, recibiendo fDelta. Este valor sera
//   calculado desde CCRISOLEngine y pasado a CCommandManager el cual, se 
//   encargara de distribuirlo entre los comandos fisicos que utilicen esta
//   clase. Su valor se habra hallado asi:
//   (fActDelta - fLastDelta) *  AISecTickTime.
// Parametros:
// - fDeltaTime. Delta que viene a contener la fraccion de tiempo que ha pasado
//   desde la ultima llamada. Siempre medida con relacion al tiempo asignado
//   a cada tick de AI.
// Devuelve:
// - Si la interpolacion ha finalizado con esta actualizacion, true. En caso
//   contrario false. Cuando la interpolacion ha finalizado, se podra seguir
//   llamando a este metodo que devolvera siempre true y seguira 
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CLinearInterpolation::Interpolate(const float fDeltaTime)
{
  // Se actualiza el tiempo que queda para finalizar
  m_fRemainingTime -= fDeltaTime;  

  // Se halla la pos. actual asi como el paso realizado
  m_fLastStep = m_fActPos;
  if (RIGHT_TO_LEFT == m_DirMov) {
	m_fActPos += fDeltaTime * m_fDistPerSec;
	m_fLastStep = m_fActPos - m_fLastStep;
  } else {
	m_fActPos -= fDeltaTime * m_fDistPerSec;
	m_fLastStep = m_fLastStep - m_fActPos;
  }

  // ¿Se alcanzo la pos. destino?
  return (m_fRemainingTime <= 0.0f);
}
