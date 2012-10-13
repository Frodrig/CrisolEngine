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
// GameDataBaseDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Definiciones asociadas a la base de datos del juego, CGameDataBase.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _GAMEDATABASEDEFS_H_
#define _GAMEDATABASEDEFS_H_

// Cabeceras

// Espacio de nombres
namespace GameDataBaseDefs
{ 
  // Enumerados
  enum eStaticText {
	// Texto estatico a recuperar
	// ST = StaticText
	// Relacionados con criaturas
	ST_CRIATURE_TYPE = 0, 
	ST_CRIATURE_CLASS, 
	ST_CRIATURE_GENRE, 
	ST_CRIATURE_EXT_ATTRIB,
	ST_CRIATURE_EQUIPMENT_SLOT,
	ST_CRIATURE_HABILITY,
	ST_CRIATURE_HEALTH,
	ST_CRIATURE_LEVEL,
	ST_CRIATURE_EXPERIENCE,
	ST_CRIATURE_COMBATPOINTS,
	ST_CRIATURE_SYMPTOMS,
	// Interfaces
	ST_INTERFAZ_MAINMENU_VERSIONINFO,
	ST_INTERFAZ_SAVEDFILEMANAGER_SAVEDFILEDESC,
	ST_INTERFAZ_ENTITYINTERACT_MAINMENU_OBSERVE,
	ST_INTERFAZ_ENTITYINTERACT_MAIMENU_TALK,
	ST_INTERFAZ_ENTITYINTERACT_MAINMENU_GET,
	ST_INTERFAZ_ENTITYINTERACT_MAINMENU_REGISTER,
	ST_INTERFAZ_ENTITYINTERACT_MAINMENU_MANIPULATE,
	ST_INTERFAZ_ENTITYINTERACT_MAINMENU_DROPITEM,
	ST_INTERFAZ_ENTITYINTERACT_MAINMENU_USEINVENTORY,
	ST_INTERFAZ_ENTITYINTERACT_MAINMENU_USEHABILITY,
	ST_INTERFAZ_ENTITYINTERACT_MAINMENU_EXIT,
	ST_INTERFAZ_ENTITYINTERACT_HABILITYMENU_HABILITY,
	ST_INTERFAZ_ENTITYINTERACT_HABILITYMENU_RETURN,
	ST_INTERFAZ_PLAYERCREATION_NAME,
	ST_INTERFAZ_PLAYERCREATION_GENRE,
	ST_INTERFAZ_PLAYERCREATION_TYPE,
	ST_INTERFAZ_PLAYERCREATION_CLASS,
	// Mensajes generales
	ST_GENERAL_INSUFICIENTACTIONPOINTS,
	ST_GENERAL_CANT_MOVE_TO,
	ST_GENERAL_AREA_LOADING,
	ST_GENERAL_GAME_LOADING,
	ST_GENERAL_NO_SAVEGAME_PERMISSION,
	ST_GENERAL_EXIT_GAME_CONFIRM,
  };

  enum eCBTFile {
	// Indica un tipo de fichero CBT manejado por el modulo
	// CBTF = CBTFile
	CBTF_CRISOLENGINE_INI = 0,  // Archivo de inicializacion del motor
	CBTF_STATIC_TEXT,           // Texto estatico
	CBTF_PLAYER_PROFILES,       // Perfiles de jugador
	CBTF_INTERFACES_CFG,        // Configuracion de interfaces
	CBTF_GFX_DEFS,              // Definiciones de GFX
	CBTF_PRESENTATION_PROFILES, // Perfiles de presentaciones
	CBTF_GLOBALSCRIPTEVENTS,    // Scripts asociados a eventos World y Game
	// Num. maximo de archivos
	CBTF_MAXFILES,
  };

  enum eCBBFile {
	// Indica un tipo de fichero CBB manejado por el modulo
	// CBBF = CBBFile
	CBBF_ANIMTEMPLATES = 0, // Plantillas de animacion
	CBBF_FLOORPROFILES,     // Perfiles floor
	CBBF_WALLPROFILES,      // Perfil de pared
	CBBF_SCENEOBJPROFILES,  // Perfil de objetos de escenario
	CBBF_ITEMPROFILES,      // Perfiles de items
	CBBF_CRIATUREPROFILES,  // Perfiles de criaturas
	CBBF_ROOFPROFILES,      // Perfiles de techos
	// Numero maximo de archivos
	CBBF_MAXFILES,
  };
};

#endif // ~ #ifdef _GAMEDATABASEDEFS_H_
