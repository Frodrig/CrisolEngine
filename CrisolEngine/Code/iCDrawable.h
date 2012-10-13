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
// iCDrawable.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - iCDrawable
//
// Descripcion:
// - Clase interfaz de CDrawable.
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICDRAWABLE_H_
#define _ICDRAWABLE_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif
#ifndef _RESDEFS_H_
#include "ResDefs.h"
#endif

// Defincion de clases / estructuras / espacios de nombres

// Interfaz iCDrawable
struct iCDrawable
{
public:
  // Obtencion de valores basicos
  virtual ResDefs::TextureHandle GetIDTexture(void) = 0;
  virtual GraphDefs::sTextureRegion* const GetTextureRegion(void) = 0;

public:
  // Obtencion de las dimensiones
  virtual word GetWidth(void) = 0;
  virtual word GetHeight(void) = 0;

public:
  // iCDrawable / Obtencion de componentes RGB y Alpha por vertice
  virtual GraphDefs::sRGBColor* const GetRGBColor(const GraphDefs::eVertex& Vert) const = 0;
  virtual GraphDefs::sAlphaValue* const GetAlphaValue(const GraphDefs::eVertex& Vert) const = 0;

public:
  // iCDrawable / Obtencion de parametros de dibujado
  virtual bool IsTextureAlphaBlending(void) const = 0;
  virtual bool IsVertexAlphaBlending(void) const = 0;
  virtual bool IsBilinearFiltering(void) const = 0;
  virtual bool IsColorKey(void) const = 0;
};

#endif // ~ #ifdef _ICDRAWABLE_H_
