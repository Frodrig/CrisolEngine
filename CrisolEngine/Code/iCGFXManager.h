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
// iCGFXManager.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCGFXManager
//
// Descripcion:
// - Interfaz para CGFXManager
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICGFXMANAGER_H_
#define _ICGFXMANAGER_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif
#ifndef _AREADEFS_H
#include "AreaDefs.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif
#ifndef _GFXMANGERDEFS_H_
#include "GFXManagerDefs.h"
#endif
#ifndef _COMMANDDEFS_H_
#include "CommandDefs.h"
#endif

// Defincion de clases / estructuras / espacios de nombres
struct iCCommandClient;

// Interfaz iCGFXManager
struct iCGFXManager
{
public:
  // Trabajo con el valor de luz ambiente
  virtual GraphDefs::Light GetAmbientLight(const byte ubHour) = 0;

public:
  // Trabajo con los valores alpha para sombras
  virtual GraphDefs::sAlphaValue* const GetAlphaShadow(void) = 0;

public:
  // Trabajo con el alpha para efecto de transparencia en criaturas
  virtual GraphDefs::sAlphaValue* const GetAlphaCriatures(void) = 0;

public:
  // Trabajo con el efecto de Fade
  virtual void SetFadeSpeed(const word uwFadeSpeed) = 0;
  virtual void SetFadeOutColor(const GraphDefs::sRGBColor& RGBColor) = 0;
  virtual bool DoFade(const GFXManagerDefs::eFadeType& Fade,
					  const bool bDoNow = false,
					  iCCommandClient* const pClient = NULL,
					  const CommandDefs::IDCommand& IDCommand = 0,
					  const GraphDefs::eDrawZone& DrawZone = GraphDefs::DZ_CURSOR,
					  const byte ubPlane = 0) = 0;  

public:
  // Trabajo con graficos de GFX
  virtual bool AttachGFX(const AreaDefs::EntHandle& hEntity,
						 const std::string& szGFXProfile) = 0;
  virtual void ReleaseGFX(const AreaDefs::EntHandle& hEntity,
						  const std::string& szGFXProfile) = 0;
  virtual void ReleaseAllGFX(const AreaDefs::EntHandle& hEntity) = 0;
  virtual bool IsGFXAttached(const AreaDefs::EntHandle& hEntity,
							 const std::string& szGFXProfile) = 0;
  virtual void DrawGFXAttached(const AreaDefs::EntHandle& hEntity,
							   const sword& swXDrawPos,
							   const sword& swYDrawPos,
							   const GraphDefs::eDrawZone& DrawZone,
							   const byte ubDrawPlane) = 0;
};

#endif // ~ #ifdef _ICGFXMANAGER_H_
