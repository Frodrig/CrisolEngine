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
// TimerDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Define un espacio de nombres en el que se incluiran todas aquellos tipos
//   de datos relacionados con CTimer.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _TIMERDEFS_H_
#define _TIMERDEFS_H_

// Cabeceras
//#include "SYSDefs.h" // Definicion tipos de datos

// TimerDefs
namespace TimerDefs
{
  // Tipos enumerados
  enum TimeUnits
  { // Unidades de tiempo
    TIMER_UNITS_SEC = 1,  // Unidades en segundos
    TIMER_UNITS_MS = 1000 // Unidades en milisegundos    
  };
}
#endif // ~ #ifdef _TIMERDEFS_H_
