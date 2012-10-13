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
// CriatureObserverDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Espacio de nombres con todos los tipos asociada a la interface
//   iCCriatureObserver
///////////////////////////////////////////////////////////////////////////////
#ifndef _CRIATUREOBSERVERDEFS_H_
#define _CRIATUREOBSERVERDEFS_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif

namespace CriatureObserverDefs 
{  
  // Enumerados
  enum eObserverNotifyType {
	// Codigos de notificacion para los observadores	
	HEALTH_MODIFY = 1,         // Se ha modificado la salud
	INSUFICIENT_ACTION_POINTS, // Puntos de accion insuficientes
	ACTION_REALICE,            // Accion realizada
	WALK_ORDER,                // Se ha recibido orden de movimiento
	MOVING,                    // La criatura se esta moviendo
	INTERRUPT_MOVING,          // Movimiento interrumpido
	INTERRUPT_HIT,             // Golpe interrumpido (no cuenta)
	WALK_COMPLETE,             // Trayecto de mov. completo
	IS_DEATH,                  // La criatura esta muerta
	IS_RESURRECT,              // La criatura ha resucitado
	SYMPTOM_ACTIVE,            // Se ha activado un sintoma
	SYMPTOM_DEACTIVE,          // Se ha desactivado un sintoma
	HABILITY_SET,              // Se ha establecido una habilidad
	HABILITY_UNSET,            // Se ha desestablecido una habilidad
	EXTENDED_ATTRIBUTE_MODIFY, // Atributo extendido modificado
	LEVEL_MODIFY,              // Nivel modificado
	EXPERIENCE_MODIFY,         // Experiencia modificada
	ACTION_POINTS_MODIFY,      // Puntos de accion modificados
	DESTROY                    // La criatura va a destruirse
  };
}; // ~ namespace

#endif // ~ _CRIATUREOBSERVERDEFS_H_