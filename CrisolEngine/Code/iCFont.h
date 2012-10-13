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
// iCFont.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCFont.
//
// Descripcion:
// - Interfaz a CFont
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICFONT_H_
#define _ICFONT_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"          
#endif
#ifndef _RESDEFS_H_
#include "ResDefs.h"
#endif
#ifndef _FONTDEFS_H_
#include "FontDefs.h"         
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif

// Definicion de clases / structuras / tipos referenciados

// Interfaz iCFont
struct iCFont
{
public:
  // Operaciones sobre la fuente
  virtual ResDefs::TextureHandle const GetIDFontImage(void) = 0;
  virtual FontDefs::eCharPoints GetCharPoints(void) const = 0;
  virtual byte GetCharQuadRes(void) const = 0;
  virtual byte GetCharWidth(const byte ubChar) const = 0;
  virtual GraphDefs::sTextureRegion* GetCharPos(const byte ubChar) = 0;
};

#endif // ~ #ifdef _ICFONT_H_
