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
// iCGraphicSystem.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces
// - iCGraphicSystem
//
// Descripcion:
// - Define la clase que hara de interfaz general (clase base) 
//   para CGraphicSystem
//
// Notas:
// - Las clases que actuen de interfaz global seran definidas como estructuras
//   por motivos de claridad de lectura y porque las estructuras declaran todos
//   sus miembros como publicos.
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICGRAPHICSYSTEM_H_
#define _ICGRAPHICSYSTEM_H_

// Cabeceras
#ifndef _DXDRAW_H_
#include "DXWrapper\\DXDraw.h" 
#endif
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"  
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif
#ifndef _FONTDEFS_H_
#include "FontDefs.h"
#endif
#ifndef _COMMANDDEFS_H_
#include "CommandDefs.h"
#endif
#ifndef _STRING_H_
#include <string>
#define _STRING_H_
#endif

// Definicion de clases / estructuras / tipos referenciados
class DXDraw;
class DXD3DDevice;
struct iCDrawable;
struct iCFont;
struct iCQuadForm;
struct iCCommandClient;

// Interfaz iCGraphicSystem
struct iCGraphicSystem
{  
public:
  // Dibujado
  virtual void SetDarkFactor(const GraphDefs::Light& DarkFactor) = 0;
  virtual void Draw(const GraphDefs::eDrawZone& Zone, 
					const byte ubPlane,
					const sword& swXPos,
					const sword& swYPos,
					iCDrawable* const pDrawObj,
					const GraphDefs::sLight& Light) = 0;  
  virtual void Draw(const GraphDefs::eDrawZone& Zone, 
					const byte ubPlane,
					const sword& swXPos,
					const sword& swYPos,
					iCDrawable* const pDrawObj) = 0;  
public:
  // Trabajo con DX
  virtual DXDraw* const GetDXDraw(void) = 0;
  virtual bool IsHardwareDevice(void) const = 0;
  virtual bool IsPow2Texture(void) const = 0;     
  virtual bool IsSquareTextureDevice(void) const = 0;
  virtual DDPIXELFORMAT* const GetTexturePixelFormat(const GraphDefs::eBpp& Bpp, 
                                                     const GraphDefs::eABpp& AlphaBits) = 0;

public:
  // Obtencion de informacion
  virtual  dword GetVideoWidth(void) const = 0;
  virtual dword GetVideoHeight(void) const = 0;
  virtual dword GetVideoBpp(void) const = 0;
};
#endif // ~ #ifdef _ICGRAPHICSYSTEM_H_
