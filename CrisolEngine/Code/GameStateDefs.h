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
// GameStateDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Definiciones para el trabajo con areas
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _GAMESTATEDEFS_H_
#define _GAMESTATEDEFS_H_

// Cabeceras

// Espacio de nombres
namespace GameStateDefs
{  
  // Tipos enumerados
  enum eGameState {
    // Principales
    GS_NULL = 0X00,      // Sin estado asociado         
    GS_MAIN_MENU,        // Menu principal
	GS_CREATE_CHARACTER, // Creacion de un caracter para comenzar a jugar
    GS_LOAD_GAME,        // Cargando juego
    GS_SAVE_GAME,        // Guardando juego
    GS_CONFIGURE_GAME,   // Configurando el juego
    GS_INGAME,           // En el juego
    GS_CREDITS,          // Viendo los creditos	  
    // Secundarios <transicion automatica>
    GS_AREA_LOADING // Cargando de un area
  };

};

#endif // ~ #ifdef _GAMESTATEDEFS_H_
