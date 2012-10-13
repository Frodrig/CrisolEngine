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
// AnimTemplateDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Definiciones para el trabajo con plantillas de animacion
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ANIMTEMPLATEDEFS_H_
#define _ANIMTEMPLATEDEFS_H_

// Cabeceras

// Espacio de nombres
namespace AnimTemplateDefs
{  
  // Tipos
  typedef sword AnimFrame;       // Posicion de un frame
  typedef byte  AnimOrientation; // Posicion de una orientacion
  typedef byte  AnimState;       // Posicion de un estado
  typedef byte  FramesPerSecond; // Frames por segundo

  // Constantes
  const word MAX_ANIMFRAMES = 256;     // Max. numero de frames
  const word MAX_ANIMORIENTATIONS = 8; // Max. numero de orientaciones
  const word MAX_ANIMSTATES = 256;     // Max. numero de estados    
};

#endif // ~ #ifdef _AnimTemplateDefs_H_
