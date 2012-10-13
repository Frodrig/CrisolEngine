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
// GFXManagerDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Definicion para el trabajo con el GFXManager
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _GFXMANAGERDEFS_H_
#define _GFXMANAGERDEFS_H_

// Cabeceras

// Espacio de nombres
namespace GFXManagerDefs
{ 
  // Enumerados
  enum eGFXDrawPosition {
	// Zona de dibujado del grafico de GFX, en relacion a la entidad
	OVER_ENTITY_TOP,     // Por encima de la entidad
	AT_ENTITY_TOP,       // Ajustado a la parte superior de la entidad
	AT_ENTITY_BOTTOM,    // Ajustado a la base de la entidad
	UNDER_ENTITY_BOTTOM  // Por debajo de la base de la entidad
  };  

  enum eFadeType {
	// Tipos de fades disponibles
	FADE_IN, // FadeIn (desde un color a pantalla)
	FADE_OUT // FadeOut (desde pantalla a un color)
  };
};

#endif // ~ #ifdef _GFXMANAGERDEFS_H_
