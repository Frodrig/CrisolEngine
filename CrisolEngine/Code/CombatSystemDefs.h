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
// CombatSystemDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Espacio de nombres destinado al trabajo con CCombatSystem
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _COMBATSYSTEMDEFS_H_
#define _COMBATSYSTEMDEFS_H_

#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif

namespace CombatSystemDefs
{  
  // Enumerados
  enum eCombatAlingment {
	// Tipos de alineacion en combate
	NO_ALINGMENT = 0,  // Sin alineacion
	PLAYER_ALINGMENT,     // Alineacion con jugador
	ENEMYPLAYER_ALINGMENT // Alineacion como enemigo del jugador
  };
};

#endif // ~ #ifdef _COMBATSYSTEMDEFS_H_
