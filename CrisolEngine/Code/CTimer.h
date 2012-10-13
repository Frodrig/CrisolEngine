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
// CTimer.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CTimer
//
// Descripcion:
// - Clase encargada de comprobar el tiempo transcurrido entre cada
//   frame de actualizacion. Externamente, dicho valor se utilizara
//   principalmente para controlar los FPS de las animaciones que se quieran
//   mostrar.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CTIMER_H_
#define _CTIMER_H_

// Cabeceras
#ifndef _ICTIMER_H_
#include "iCTimer.h"
#endif
#ifndef _TIMERDEFS_H_
#include "TimerDefs.h"
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CTimer
class CTimer: public iCTimer
{  
private:
  // Vbles de miembro
  bool    m_bInitOk;      // ¿Clase inicializada correctamente?
  bool    m_bPerfCounter; // ¿Esta activo el PerformanceCounter?
  sqword  m_sqInitTime;   // Tiempo transcurrido entre llamada a Update
  double  m_dTimeScale;   // Resolucion del contador (perido = 1/frecuencia)
  
public:
  // Constructor / Destructor
  CTimer(void): m_bInitOk(false) { }
  ~CTimer(void) { End(); }               

public:
  // Protocolo de inicio y fin de instancia
  bool Init(const bool bPerformanceOnly);
  void End(void);
  inline bool IsInitOk(void) const { return m_bInitOk; }  
  
public:
  // Operaciones de interfaz de CTimer
  inline bool IsPerformanceActive(void) const { return m_bPerfCounter; }
  
public:
  // Operaciones de interfaz iCTimer
  sqword GetTime(const TimerDefs::TimeUnits& Units = TimerDefs::TIMER_UNITS_MS);
  sqword GetTimeElapsedFromInit(const TimerDefs::TimeUnits& Units = TimerDefs::TIMER_UNITS_MS);
};

#endif // ~ CLogger
