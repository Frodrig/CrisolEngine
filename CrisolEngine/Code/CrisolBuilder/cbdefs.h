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
// CBDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Define un espacio de nombres con los tipos y constantes asociados al
//   CrisolBuilder.
// - Tambien incluye todos aquellos archivos externos necesarios para
//   CrisolBuilder
//
// Notas:
///////////////////////////////////////////////////////////////////////////////

#ifndef _CBDEFS_H_
#define _CBDEFS_H_

// Includes
#ifndef _SYSDEFS_H_
#include "..\\SYSDefs.h"
#endif

// Espacio
namespace CBDefs {
  // CB = CrisolBuilder

  // Constantes
  // Tipos de ficheros
  const byte CBBAnimTemplateFile		 = 1;  // Fichero de plantillas de animacion
  const byte CBBFloorProfilesFile		 = 2;  // Fichero de perfiles floor
  const byte CBBWallProfilesFile		 = 3;  // Fichero de perfiles wall
  const byte CBBSceneObjProfilesFile	 = 4;  // Fichero de perfiles scene obj
  const byte CBBItemProfilesFile		 = 5;  // Fichero de perfiles item
  const byte CBBCriatureProfilesFile	 = 6;  // Fichero de perfiles criature
  const byte CBBRoofProfilesFile		 = 7;  // Fichero de perfiles roof
  const byte CBBAreaBaseFile			 = 8;  // Fichero de area base
  const byte CBBAreaTmpFile				 = 9;  // Fichero de area temporal

  // Versiones
  const byte HVersion = 1; // Version alta
  const byte LVersion = 0; // Version baja


  // Enumerados
  enum eFileTypes {
	// Define tipos de archivos
	CBT_ANIM_TEMPLATES = 1, // Archivo de plantillas de animacion
  };  
};

#endif