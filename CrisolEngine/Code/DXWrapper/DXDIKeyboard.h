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
// DXDIKeyboard.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion: 
// - Clase para el manejo del dispositivo de teclado a traves de
//   DirectInput.
//
// Notas:
// - Clase optimizada para DirectX 7.0.
// - Para su correcta compilacion hara falta incluir la libreria
//   "DInput.lib". 
// - Se recuerda que todas las clases DX deben de heredar de la clase
//   base DXWrapper e implementar los metodos Init y Clean. Para mas
//   informacion ver documentacion de DXWrapper.
// - Todas las estructuras, constantes, tipos enumerados, etc que
//   utilice la libreria de clases para DirectInput, estaran en el
//   archivo de cabecera "DXDIDefs.h".
///////////////////////////////////////////////////////////////////////
#ifndef __DXDIKEYBOARD_H_
#define __DXDIKEYBOARD_H_

// Includes
#include "DXWrapper.h"
#include "DXDIDefs.h"

class DXInput;

// Clase DXDIKeyboard
class DXDIKeyboard: public DXWrapper
{
protected:
  // Vbles de miembro
  LPDIRECTINPUTDEVICE7 m_lpDIDKeyboard;	      // Dispositivo de teclado
  char                 m_KeyboardStatus[256]; // Estado el teclado
  bool                 m_bAcquired;           // Flag de adquisicion

public:			
  // Constructor y destructor
  DXDIKeyboard(void);				
  ~DXDIKeyboard(void);

public:
  // Operaciones sobre el dispositivo teclado
  bool Create(const DXInput* lpoDXInput, const HWND hWnd);
  void Destroy(void) { Clean(); Init(); }
  inline const LPDIRECTINPUTDEVICE7 GetObj(void) const { return m_lpDIDKeyboard; };
  inline const char* GetKeyboardStatus(void) const { return m_KeyboardStatus; }
  bool Update(void);
  bool Acquire(void);
  bool Unacquire(void);

public:
  // Operaciones de consulta    
  inline bool IsKeyPressed(const byte Key) const { 
	return KEYDOWN(m_KeyboardStatus, Key) ? true : false; 
  }
  inline bool IsDetected(void) const { return (m_lpDIDKeyboard != NULL); }
  inline bool IsAcquired(void) const { return m_bAcquired; }  
    
protected:
  // Inicializacion y liberacion de recursos <DXWrapper>
  void Init(void);
  void Clean(void);  
};
#endif