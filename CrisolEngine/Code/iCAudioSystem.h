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
// iCAudioSystem.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCAlarmManager
//
// Descripcion:
// - Interfaz de acceso al subsistema de audio CAudioSystem.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICAUDIOSYSTEM_H_
#define _ICAUDIOSYSTEM_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h" 
#endif
#ifndef _AUDIODEFS_H_
#include "AudioDefs.h" 
#endif
#ifndef _RESDEFS_H_
#include "ResDefs.h"
#endif

// Definicion de clases / estructuras / tipos referenciados
class DXSound;

// Interfaz iCAudioSystem
struct iCAudioSystem
{ 
public:
  // Operaciones sobre ficheros WAV
  virtual void PlayWAVSound(const ResDefs::WAVSoundHandle& hWAVSound, 
							const AudioDefs::eWAVPlayMode& Mode) = 0;
  virtual void StopWAVSound(const ResDefs::WAVSoundHandle& hWAVSound) = 0;
  virtual void PauseWAVSound(const ResDefs::WAVSoundHandle& hWAVSound) = 0;
  virtual void SetWAVSoundVolume(const ResDefs::WAVSoundHandle& hWAVSound,
								 const AudioDefs::eWAVVolume& Volume) = 0;
  virtual bool IsWAVSoundPlaying(const ResDefs::WAVSoundHandle& hWAVSound) = 0;
  
public:
  // Operaciones sobre ficheros MIDI
  virtual void PlayMIDIMusic(const std::string& szFileName, 
							 const AudioDefs::eMIDIPlayMode& Mode = AudioDefs::MIDI_PLAY_NORMAL) = 0;
  virtual bool GetMIDIInfo(std::string& szMIDIFileName,
						   AudioDefs::eMIDIPlayMode& MIDIPlayMode) = 0;
  virtual bool IsPlayingMIDIMusic(void) = 0;
  virtual void StopMIDIMusic(void) = 0;

public:
  // Operaciones de obtenciond de informacion
  virtual DXSound* const GetDXSound(void) = 0;
};

#endif // ~ #ifdef _ICAUDIOSYSTEM_H_
