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
// WorldObserverDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Espacio de nombres con todos los tipos asociada a la interface
//   iCWorldObserver
///////////////////////////////////////////////////////////////////////////////
#ifndef _WORLDOBSERVERDEFS_H_
#define _WORLDOBSERVERDEFS_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif

namespace WorldObserverDefs 
{  
  // Enumerados
  enum eObserverNotifyType {
	// Codigos de notificacion para los observadores
	NEW_HOUR,            // Se ha alcanzado una nueva hora
	NEW_SESION,          // Se ha iniciado una nueva sesion (se notifica al final)
	END_SESION,          // Se va a finalizar una sesion (se notifica al principio)
	PAUSE_ON,            // Se ha pausado el universo de juego
	PAUSE_OFF,           // Se ha quitado la pausa del universo de juego
	ENTITY_CREATE,       // Una entidad ha sido creada
	ENTITY_DESTROY,      // Una entidad ha sido destruida
	VISIBLE_WORLD_UPDATE // El area visible del universo de juego actualizada
  };
}; // ~ namespace

#endif // ~ _WORLDOBSERVERDEFS_H_