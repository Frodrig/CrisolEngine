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
// AudioDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Espacio de nombres para definciones, constantes y tipos relativos al
//   subsistema de audio CAudioSystem.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _AUDIODEFS_H_
#define _AUDIODEFS_H_

namespace AudioDefs
{
  // Tipos enumerados    
  enum eWAVPlayMode { 
	// Flags para la reproduccion de un fichero WAV
    WAV_PLAY_NORMAL = 0, // Reproduccion normal
    WAV_PLAY_LOOP,       // Reproduccion con loop
  };

  enum eMIDIPlayMode { 
	// Flags para la reproduccion MIDI
    MIDI_PLAY_NORMAL = 0, // Reproduccion normal
    MIDI_PLAY_LOOP        // Reproduccion con loop
  };

  enum eWAVVolume {
	// Define los tres tipos de volumenes posibles
	WAV_VOLUME_MIN,    // Volumen minimo
	WAV_VOLUME_NORMAL, // Volumen normal
	WAV_VOLUME_MAX,    // Volumen maximo
  };

}; // ~ AudioSystemDefs

#endif // ~ #ifdef _AUDIODEFS_H_
