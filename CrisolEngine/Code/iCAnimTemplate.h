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
// iCAnimTemplate.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clase:
// - iCAnimTemplate
//
// Descripcion:
// - Interfaz para iCAnimTemplate
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICANIMTEMPLATE_H_
#define _ICANIMTEMPLATE_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _RESDEFS_H_
#include "ResDefs.h"
#endif
#ifndef _AUDIODEFS_H_
#include "AudioDefs.h"
#endif
#ifndef _ANIMTEMPLATEDEFS_H_
#include "AnimTemplateDefs.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif

// Defincion de clases / estructuras / espacios de nombres
struct iCAnimTemplateObserver;

// Clase iCAnimTemplate
struct iCAnimTemplate
{
private:
  // Clases amigas
  friend class CAnimCmd;

public:
  // Obtencion del nombre de la plantilla de animacion
  virtual std::string GetAnimTemplateName(void) const = 0;

private:
  // Trabajo con las frames
  // Nota: el metodo NextFrame se protege para que solo CAnimCmd pueda usarlo
  virtual void NextFrame(void) = 0;
public:
  virtual word GetNumFrames(const AnimTemplateDefs::AnimState& State) = 0;
  virtual bool IsReversibleState(const AnimTemplateDefs::AnimState& State) = 0;

public:
  // Trabajo con orientaciones de animacion
  virtual void SetOrientation(const AnimTemplateDefs::AnimOrientation& Orientation) = 0;
  virtual AnimTemplateDefs::AnimOrientation GetOrientation(void) const = 0;
  virtual word GetNumOrientations(void) = 0;
  virtual void NextOrientation(void) = 0;

public:
  // Trabajo con estados de animacion
  virtual void SetState(const AnimTemplateDefs::AnimState& State) = 0;
  virtual word GetNumStates(void) = 0;
  virtual AnimTemplateDefs::AnimState GetAnimState(void) const = 0;

public:
  // Obtencion del identificador del frame actual
  virtual ResDefs::TextureHandle GetIDTextureFrame(void) = 0;

public:
  // Obtecion de informacion general  
  virtual AnimTemplateDefs::FramesPerSecond GetFPS(void) = 0;
  virtual word GetFramesWidth(void) = 0;
  virtual word GetFramesHeight(void) = 0;

public:
  //  Trabajo con el control del comando de animacion
  virtual void ActiveAnimCmd(const bool bActive) = 0;
  virtual bool IsAnimCmdActive(void) const = 0;

public:
  // Trabajo con el volumen del sonido
  virtual void SetWAVSoundVolume(const AudioDefs::eWAVVolume& Volume) = 0;
  virtual AudioDefs::eWAVVolume GetWAVSoundVolume(void) const = 0;

public:
  // Trabajo con observers
  virtual void AddObserver(iCAnimTemplateObserver* const pObserver) = 0;
  virtual void RemoveObserver(iCAnimTemplateObserver* const pObserver) = 0;
};

#endif // ~ #ifdef _ICANIMTEMPLATE_H_
