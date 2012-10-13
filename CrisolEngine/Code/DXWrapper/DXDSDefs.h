///////////////////////////////////////////////////////////////////////////////
// DX7Wrapper - DirectX 7 Wrapper
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

///////////////////////////////////////////////////////////////////////
// DXDSDefs.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Realiza la inclusion de cabeceras necesarias y globales para
//   todos los objetos DirectSound.
// - Incorpora las declaraciones de constantes, tipos enumerados, etc
//   utilizados en la libreria de clases de DirectSound.
//
// Notas:
///////////////////////////////////////////////////////////////////////
#ifndef __DXDSDEFS_H_
#define __DXDSDEFS_H_

// Includes
#include <mmsystem.h>
#include <DSound.h>

namespace DXDSDefs {
  // Enumerados 
  enum DXDSFrequency
  {// Frecuencias para buffer primario
	FREQUENCY_11 = 11025,
	FREQUENCY_22 = 22050,
	FREQUENCY_44 = 44100
  };

  enum DXDSChannel
  {// Canales para buffer primario
	CHANNEL_MONO = 1,
	CHANNEL_STEREO  
  };

  enum DXDSBPM
  {// Bits por muestra
	BPS_8 = 8,
	BPS_16 = 16 
  };
}

#endif