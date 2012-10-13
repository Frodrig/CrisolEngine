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
// iCFontSystem.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCFontSystem
//
// Descripcion:
// - Interfaz para CFontSytem.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICFONTSYSTEM_H_
#define _ICFONTSYSTEM_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif

// Defincion de clases / estructuras / espacios de nombres
struct iCFont;

// Interfaz iCFontSystem
struct iCFontSystem
{
private:
  // Clases amigas para interfaz privado
  friend class CGraphicSystem;
  virtual void ResetPoolMemory(void) = 0;

public:
  // Escritura de texto
  virtual void Write(const sword& swXPos,
					 const sword& swYPos,
					 const byte ubPlane,
					 const std::string& szText,
					 const GraphDefs::sRGBColor& RGBColor = GraphDefs::sRGBColor(255, 255, 255),
					 const GraphDefs::sAlphaValue& Alpha = GraphDefs::sAlphaValue(255),
					 const std::string& szFontName = "") = 0;

public:
  // Obtencion de instancias a fuentes
  virtual iCFont* const GetFont(const std::string szFontName) = 0;

public:
  // Metodos de utilidad general
  virtual void GetTextDim(const std::string& szText,
						  const std::string& szFontName,
						  word& uwWidth,
						  word& uwHeight) = 0;
};

#endif // ~ #ifdef _ICFONTSYSTEM_H_
