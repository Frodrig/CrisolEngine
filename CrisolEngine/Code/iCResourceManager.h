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
// iCResourceManager.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCResourceManager
//
// Descripcion:
// - Clase base interfaz del servidor de recursos.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICRESOURCEMANAGER_H_
#define _ICRESOURCEMANAGER_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _RESDEFS_H_
#include "ResDefs.h" 
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"   
#endif
#ifndef _STRING_H_
#include <string> 
#define _STRING_H_
#endif               

// Definicion de clases / estructuras / tipos referenciados
class DXDSWAVBuffer;
class DXDSWAVSound;
class DXDDSurfaceTexture;
class CAnimTemplate;
struct iCAnimTemplateData;

// Interfaz iCResourceManager
struct iCResourceManager
{ 
public:
  // Metodos para el trabajo con Texturas
  virtual ResDefs::TextureHandle TextureRegister(const std::string& szTextureName,
									             const GraphDefs::eBpp& Bpp,
										         const GraphDefs::eABpp& AlphaBits)= 0;
  virtual ResDefs::TextureHandle TextureRegister(const std::string& szTextureName,
												 const word udTextureWidth,
												 const word udTextureHeight,
												 const GraphDefs::eBpp& Bpp,
												 const GraphDefs::eABpp& AlphaBits) = 0;  
  virtual ResDefs::TextureHandle TextureRegister(const std::string& szTextureName,
												 const sRect& rDimension,
												 const GraphDefs::eBpp& Bpp,
												 const GraphDefs::eABpp& AlphaBits) = 0;    
  virtual DXDDSurfaceTexture* const GetTexture(const ResDefs::TextureHandle& hTexture) = 0;
  virtual void ReleaseTexture(const ResDefs::TextureHandle& hTexture) = 0;  
  virtual std::string GetTextureFileName(const ResDefs::TextureHandle& hTexture) = 0;

public:
  // Metodos de trabajo con sonido WAV
  virtual ResDefs::WAVSoundHandle WAVSoundRegister(const std::string& szWAVFileName,
												   const bool bCtrlVolume = false,
												   const bool bCtrlFrequency = false,
												   const bool bCtrlPan = false) = 0;
  virtual DXDSWAVSound* const GetWAVSound(const ResDefs::WAVSoundHandle& hWAVSound) = 0;
  virtual void ReleaseWAVSound(const ResDefs::WAVSoundHandle& hWAVSound) = 0;
  virtual std::string GetWAVSoundFileName(const ResDefs::WAVSoundHandle& hWAVSound) = 0;    
  virtual bool IsWAVSoundPlaying(const ResDefs::WAVSoundHandle& hWAVSound) = 0;

public:
  // Metodos de trabajo con las plantillas de animacion
  virtual iCAnimTemplateData* const RegisterAnimTemplateData(const std::string& szAnimTemplate,
															 CAnimTemplate* const pAnimTemplate) = 0;
  virtual void ReleaseAnimTemplateData(const std::string& szAnimTemplate,
									   CAnimTemplate* const pAnimTemplate) = 0;
};

#endif // ~ #ifdef _ICRESOURCEMANAGER_H_
