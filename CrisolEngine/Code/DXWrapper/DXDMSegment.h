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
// DXDMSegment.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Clase que trabaja con los segmentos de DirectMusic. Encapsula los
//   objetos IDirectMusicSegment e IDirectMusicSegmentState.
//
// Notas:
// - Clase preparada para DirectX 7.0.
// - Este API utiliza al 100% COM por lo que no hay libreria linkada.
// - Se recuerda que todas las clases DX deben de heredar de la clase
//   base DXWrapper y reescribir los metodos Init y Clean. Para mas
//   informacion ver documentacion de DXWrapper.
// - Todas las estructuras, constantes, tipos enumerados, etc que
//   utilice la libreria de clases para DirectMusic estaran en el
//   archivo de cabecera "DXDMDefs.h".
// - Para poder trabajar con la funcion que transforma codigos de error
//   de DirectMusic a cadenas de caracteres se incluye "DXDMError.h".
///////////////////////////////////////////////////////////////////////
#ifndef __DXDMSEGMENT_H_
#define __DXDMSEGMENT_H_

// Includes
#include "DXWrapper.h"
#include "DXDMDefs.h"
#include <string>

class DXDMManager;

// Clase DXDMSegment
class DXDMSegment: public DXWrapper
{
protected:
  // Variables de miembro
  IDirectMusicSegment*      m_pSegment;      // Objeto DirectMusicSegment
  IDirectMusicSegmentState* m_pSegState;     // Objeto DirectMusicSegmentState
  DXDMManager*              m_poDXDMManager; // Necesario para tener el performance
  
public:
  // Constructor / Destructor
  DXDMSegment(void);
  ~DXDMSegment(void);

public:
  // Creacion / destruccion
  bool Create(DXDMManager* const poDXDMManager);  
  void Destroy(void) { Clean(); Init(); }
  void Unload(void);
  bool LoadMIDISegment(const std::string& oMIDFile);  
  bool PlayNow(void);
  bool PlayQueue(void);
  bool PlayElapsed(void);
  bool Stop(void);
  bool IsPlaying(void);

  // Operaciones inline
  inline IDirectMusicSegment* const GetSegmentObj(void) const { return m_pSegment; }  
  inline IDirectMusicSegmentState* const GetSegmentState(void) const { return m_pSegState; }
  inline DXDMManager* const GetDXManager(void) const { return m_poDXDMManager; }

protected:
  // Inicializacion y liberacion de recursos <DXWrapper>
  void Init(void);
  void Clean(void);
};

#endif
