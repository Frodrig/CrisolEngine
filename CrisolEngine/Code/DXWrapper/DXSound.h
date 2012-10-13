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
// DXSound.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
// Descripcion:
//
// - Clase que encapsula la creacion de un objeto DirectSound.
//
// Notas:
// - Clase basada en DirectX 7.0.
// - Para su correcta compilacion hara falta incluir la libreria
//   "DSound.lib" y el archivos de cabecera "DSound.h". Todos estos 
//   recursos pertenecen al componente DirectSound y se encuentran 
//   definidos en "DXDSDefs.h"
// - Se recuerda que todas las clases DX deben de heredar de la clase
//   base DXWrapper y reescribir los metodos Init y Clean. Para mas
//   informacion ver documentacion de DXWrapper.
// - Todas las estructuras, constantes, tipos enumerados, etc que
//   utilice la libreria de clases para DirectSound estaran en el
//   archivo de cabecera "DXDSDefs.h".
///////////////////////////////////////////////////////////////////////
#ifndef __DXSOUND_H_
#define __DXSOUND_H_

// Includes
#include "DXWrapper.h"
#include "DXDSDefs.h"

// Clase DXSound
class DXSound: public DXWrapper
{
protected:
  // Variables de miembro
  LPDIRECTSOUND m_lpDSObj;  // Objeto DirectSound

public:
  // Constructores y destructores
  DXSound(void);
  ~DXSound(void);

public:
  // Operaciones de creacion / destruccion
  bool Create(const HWND& hWnd, 
              const DXDSDefs::DXDSFrequency& freqMode = DXDSDefs::FREQUENCY_44, 
              const DXDSDefs::DXDSChannel& chanMode = DXDSDefs::CHANNEL_STEREO, 
              const DXDSDefs::DXDSBPM& bpm = DXDSDefs::BPS_16,
              GUID* const lpGuid = NULL);
  void Destroy(void) { Clean(); Init(); }

public:
  // Operaciones de configuracion
  bool SetSpeakers(const dword dwType, 
                   const dword dwGeometry = DSSPEAKER_GEOMETRY_WIDE);

public:
  // Obtencion objeto DirectSound  
  inline LPDIRECTSOUND GetObj(void) const { return m_lpDSObj; }

protected:
  // Inicializacion y liberacion de recursos <DXWrapper>
  void Init(void);
  void Clean(void);  
};
#endif
