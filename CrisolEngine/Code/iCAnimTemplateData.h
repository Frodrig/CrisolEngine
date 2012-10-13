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
// iCAnimTemplateData.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaz:
// - iCAnimTemplateData
//
// Descripcion:
// - Interfaz para CAnimTemplateData
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICANIMTEMPLATEDATA_H_
#define _ICANIMTEMPLATEDATA_H_

// Cabeceras
#ifndef _RESDEFS_H_
#include "ResDefs.h"
#endif
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _ANIMTEMPLATEDEFS_H_
#include "AnimTemplateDefs.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif

// Definicion de clases / estructuras / espacios de nombres
class CAnimTemplate;

// Interfaz CAnimTemplate
struct iCAnimTemplateData
{
public:
  // Obtencion del nombre de la plantilla de animacion
  virtual std::string GetAnimTemplateName(void) const = 0;

public:
  // Obtencion de info referida a handles a textura
  virtual const ResDefs::TextureHandle& GetIDTexture(const AnimTemplateDefs::AnimState& State,
													 const AnimTemplateDefs::AnimOrientation& Orientation,
													 const AnimTemplateDefs::AnimFrame& Frame) = 0;

public:
  // Obtencion de info referida a sonido
  virtual ResDefs::WAVSoundHandle GetWAVSoundHandle(CAnimTemplate* const pAnimTemplate,
													const AnimTemplateDefs::AnimState& State) = 0;
  virtual bool GetWAVSoundFlag(CAnimTemplate* const pAnimTemplate,
							   const AnimTemplateDefs::AnimState& State) = 0;
  
public:
  // Obtencion de info referida a frames
  virtual word GetNumFrames(const AnimTemplateDefs::AnimState& State) = 0;
  virtual bool IsReversibleState(const AnimTemplateDefs::AnimState& State) = 0;

public:
  // Obtencion de info referida a orientaciones
  virtual word GetNumOrientations(void) = 0;

public:
  // iCAnimTemplate / Obtencion de info referida a estados
  virtual word GetNumStates(void) = 0;
  virtual bool IsStateWithAnim(const AnimTemplateDefs::AnimState& State) = 0;
  virtual AnimTemplateDefs::AnimState GetLinkState(const AnimTemplateDefs::AnimState& State) = 0;
  virtual const float GetTimeBetweenCycles(const AnimTemplateDefs::AnimState& State) = 0;

public:
  //Obtecion de informacion general  
  virtual AnimTemplateDefs::FramesPerSecond GetFPS(void) = 0;
  virtual word GetFramesWidth(void) = 0;
  virtual word GetFramesHeight(void) = 0;
};

#endif // ~ #ifdef _CANIMTEMPLATE_H_