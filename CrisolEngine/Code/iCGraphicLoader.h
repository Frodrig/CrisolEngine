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
// iCGraphicLoader.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Intefaces:
// - iCGraphicLoader
//
// Descripcion:
// - Interfaz para iCGraphicLoader
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICGRAPHICLOADER_H_
#define _ICGRAPHICLOADER_H_

// Cabeceras
#ifndef _DXD3DDEVICE_H_
#include "DXWrapper\\DXD3DDevice.h"
#define _DXD3DDEVICE_H_
#endif
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"  
#endif
#ifndef _STRING_H_
#include <string>     
#define _STRING_H_
#endif

// Definicion de clases / estructuras / tipos referenciados
struct iCTextureRes;

// Interfaz iCGraphicSystem
struct iCGraphicLoader
{  
public:
  // Carga de graficos
  virtual bool TGALoader(const std::string& szFileName) = 0;

public:
  // Obtencion de informacion
  virtual word GetWidth(void) const = 0;
  virtual word GetHeight(void) const = 0;
  virtual bool IsImageLoaded(void) const = 0;

public:
  // Operaciones
  virtual void UnLoad(void) = 0;
  virtual bool Draw(DXDDSurfaceTexture* const pTexture, 
                    const sRect* prSrcRect = NULL, 
                    const sRect* prDestRect = NULL) = 0;  
};

#endif // ~ #ifdef _ICGRAPHICLOADER_H_
