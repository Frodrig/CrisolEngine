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
// WorldDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Espacio de nombres con todas las definiciones, tipos y estructuras 
//   generales para el trabajo con CWorld.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _WORLDDEFS_H_
#define _WORLDDEFS_H_

#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif

namespace WorldDefs
{  
  // Constantes
  const byte SaveFileType     = 255; // Tipo de archivo de partida guardada
  const byte SaveFileHVersion = 1;   // Version superior
  const byte SaveFileLVersion = 0;   // Version inferior

  // Enumerados
  enum eWorldMode {
	// Modos en los que podra hallarse el universo de juego
	REAL_MODE, // Modo a tiempo real (modo por defecto)
	TURN_MODE  // Modo por turnos
  };
};

#endif // ~ #ifdef _WORLDDEFS_H_
