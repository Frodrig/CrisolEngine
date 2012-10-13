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
// GUIManagerDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Definiciones asociadas al manager de GUI.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _GUIMANAGERDEFS_H_
#define _GUIMANAGERDEFS_H_

// Espacio de nombres
namespace GUIManagerDefs
{
  // Enumerados
  enum eGUICursorType {
	// Tipos de cursores
	NO_CURSOR = 0,					        // Sin cursor
	MOVEMENT_CURSOR,				        // Cursor de movimiento
	NOPOSSIBLE_MOVEMENT_CURSOR,             // Cursor de movimiento NO permitido
	ENTITYINTERACT_CURSOR,			        // Cursor de interaccion
	NOPOSSIBLE_ENTITYINTERACT_CURSOR,       // Cursor de interaccion NO permitida
	ENTITYINTERACTFORHIT_CURSOR,		    // Cursor de interaccion para golpear
	NOPOSSIBLE_ENTITYINTERACTFORHIT_CURSOR, // Cursor de interaccion para golpear NO permitida
	WINDOWINTERFAZ_CURSOR,			        // Cursor de interfaz para ventanas	
	WAITING_CURSOR,                         // Cursor de espera
	SCROLL_NORTH_CURSOR,		 	        // Cursor de scroll
	SCROLL_NORTHEAST_CURSOR,		        // Cursor de scroll
	SCROLL_NORTHWEST_CURSOR,		        // Cursor de scroll			
	SCROLL_SOUTH_CURSOR,			        // Cursor de scroll
	SCROLL_SOUTHEAST_CURSOR,		        // Cursor de scroll
	SCROLL_SOUTHWEST_CURSOR,		        // Cursor de scroll
	SCROLL_WEST_CURSOR,				        // Cursor de scroll
	SCROLL_EAST_CURSOR				        // Cursor de scroll	
  };

  enum eGUIWindowType {
	// Identificadores de ventanas de interfaz
	// Interfaces concretos a un estado de juego
	GUIW_NO_TYPE = 0,            // No hay interfaz activo
	GUIW_MAINMENU,               // Menu principal <arranque del motor>
	GUIW_CHARACTERCREATOR,       // Creacion de un caracter <Nuevo juego>
	GUIW_MAININTERFAZ,           // Interfaz de ventana principal
	GUIW_ENTITYINTERACT,         // Interfaz de interaccion con entidades
	GUIW_INVENTORYUSE,           // Uso de un obj. del inventario del jugador
	GUIW_PLAYERPROFILE,          // Player profile interfaz
	GUIW_ITEMINTERACT,           // Interaccion con entidades que seguro son items
	GUIW_ITEMSELECTTOUSE,        // Seleccion de un item para usarlo sobre entidad
	GUIW_GAMEMENU,               // Interfaz de menu de juego
	GUIW_GAMELOADING,            // Ventana indicativa de carga de juego
	GUIW_CONVERSATOR,            // Ventana para dialogos / conversaciones
	GUIW_TRADE,                  // Ventana de intercambio
	GUIW_ENTITYTOPLAYERTRADE,    // Ventana de intercambio con items de la entidad
	GUIW_PLAYERTOENTITYTRADE,    // Ventana de intercambio con items del jugador
	GUIW_COMBAT,                 // Ventana de informacion de combate
	GUIW_PRESENTATION,           // Ventana de presentacion
	GUIW_LOADERSAVEDFILEMANAGER, // Manager para cargar partidas
	GUIW_SAVERSAVEDFILEMANAGER,  // Manager para guardar partidas
	// Interfaces generales
	GUIW_ADVICEDIALOG,        // Dialogo de notificacion de un aviso
	GUIW_QUESTIONDIALOG,      // Dialogo de pregunta si/no/cancel
	GUIW_TEXTREADER,          // Interfaz de lectura de texto
	GUIW_GENERICTEXTSELECTOR, // Seleccionador de opciones de texto general
  }; 

  enum eFloatingTextType {
	// Tipos de texto flotante
	NORMAL_FLOATINGTEXT,   // El texto se muestra normal
	DISSOLVE_FLOATINGTEXT, // El texto oscila entre transparente y el valor alpha recibido
  };
};

#endif // ~ #ifdef _GUIMANAGERDEFS_H_
