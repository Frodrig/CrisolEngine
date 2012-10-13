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
// DXDSWAVBuffer.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Esta clase se encargara de mantener una instancia de sonido
//   que trabaje sobre un buffer (DXDSWAVBuffer) previamente creado.
//   Desde aqui se podra controlar la reproduccion del fichero WAV segun
//   los flags con los que haya sido creado el buffer asociado.
//   Podran existir una o mas instancias DXDSWAVSound que utilicen un
//   mismo buffer de sonido perservando cada una de ellas su propio 
//   estado de reproduccion.
// - Se debera de incluir la libreria "winmm.lib".
//
// Notas:
// - Clase optimizada para DirectX 7.0.
// - Se recuerda que todas las clases DX deben de heredar de la clase
//   base DXWrapper y reescribir los metodos Init y Clean. Para mas
//   informacion ver documentacion de DXWrapper.
// - La funcionalidad para cargar archivos de sonido WAV en el buffer
//   estara embebida en esta clase.
///////////////////////////////////////////////////////////////////////
#ifndef __DXDSWAVSOUND_H_
#define __DXDSWAVSOUND_H_

// Includes
#include "DXWrapper.h"
#include "DXDSDefs.h"
#include <mmsystem.h>

class DXDSWAVBuffer;
class DXSound;

// Clase DXDSWAVBuffer
class DXDSWAVSound: public DXWrapper
{
protected:
  // Variables de miembro
  LPDIRECTSOUNDBUFFER  m_lpDSBuffer;    // Objeto DirectSoundBuffer.
  
public:
  // Constructores y destructores
  DXDSWAVSound(void);
  ~DXDSWAVSound(void);

public:
  // Creacion / Destruccion del buffer
  bool Create(DXSound* const poDXSound, 
              DXDSWAVBuffer* const poDSBufferSource);
  void Destroy(void) { Clean(); Init(); }
  
public:
  // Restauracion buffer perdido
  bool Restore(void);

public:
  // Operaciones sobre el sonido
  bool Play(const dword dwFlags = 0);
  bool Stop(const bool bInitPlayPos = true);   
  bool SetPlayPosition(const dword dwPlayPos = 0);
  bool GetPlayPosition(DWORD& dwPlayPos);    
  bool SetVolume(const LONG& lVolume = DSBVOLUME_MIN / 6);
  bool GetVolume(LONG& lVolumen);
  bool IsPlaying(void);
    
public:
  // Obtencion del objeto DirectSoundBuffer
  inline LPDIRECTSOUNDBUFFER GetObj(void) const { return m_lpDSBuffer; }

public:
  // Operaciones de consulta
  bool IsInHardware(void);
      
protected:
  // Inicializacion y liberacion de recursos <DXWrapper>
  void Init(void);
  void Clean(void);  
};
#endif
