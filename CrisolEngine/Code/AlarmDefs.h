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
// AlarmDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Definiciones y tipos para el manager de alarmas.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ALARMDEFS_H_
#define _ALARMDEFS_H_

// Defincion de clases / estructuras / espacios de nombres

// Espacio de nombres
namespace AlarmDefs
{
  // Tipos
  // El handle estara constituido:
  // 4 Primeros bits indican el tipo de alarma (2^4 - 1)
  // 12 Ultimos bits indican el identificador asociado a ese tipo (0..2^12)
  typedef word AlarmHandle; // Handle a alarma

  // Constantes
  // Maximo numero de alarmas asociadas a un tipo determinado
  const word MAX_HANDLE_VALUE = 0X0FFF; 
  const word TYPE_MASK = 0xF000;
  
  // Enumerados
  enum eAlarmType {
	// Tipo de alarma
	TIME_ALARM = 0x1000, // Alarma de tiempo
	WAV_ALARM  = 0x2000, // Alarma de finalizacion de un WAV
  };
};
#endif // ~ #ifdef _ALARMDEFS_H_
