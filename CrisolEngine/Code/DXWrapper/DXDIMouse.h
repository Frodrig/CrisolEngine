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
// DXDIMouse.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion: 
// - Clase para el manejo del dispositivo de raton a traves de
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
// - Esta version no hace gestion especifica de la posicion Z del raton,
//   simplemente controla el desplazamiento por cada movimiento.
///////////////////////////////////////////////////////////////////////
#ifndef __DXDIMOUSE_H_
#define __DXDIMOUSE_H_

// Includes
#include "DXWrapper.h"
#include "DXDIDefs.h"

class DXInput;

// Clase DXDIMouse
class DXDIMouse: public DXWrapper
{
protected:
  // Vbles de miembro  
  LPDIRECTINPUTDEVICE7 m_lpDIDMouse;	 // Dispositivo raton 
  HANDLE			   m_hMouseEvent;    // Handle a los eventos    
  bool				   m_bMouseAcquired; // Flag de raton adquirido
  RECT				   m_MovArea;        // Area de movimiento
  int				   m_nPosX;          // Posicion X del raton
  int				   m_nPosY;          // Posicion Y del raton  
  int				   m_nXDelta;        // Ultimo desp. en X
  int				   m_nYDelta;        // Ultimo desp. en Y
  int	               m_nZDelta;        // Ultimo desp. en Z
  // ¿Boton presionado?
  bool m_bButtonsState[DXDIDefs::MOUSEBUTTONS];    
  // Estado de boton  
  DXDIDefs::DXDIMouseState m_nButtonsSubState[DXDIDefs::MOUSEBUTTONS]; 
  	
public:	
  // Constructor y destructor
  DXDIMouse(void);
  ~DXDIMouse(void);

public:
  // Operaciones sobre el dispositivo raton
  bool Create(DXInput* const lpoDXI, 
			  const HWND& hWnd, 
			  const int& nAreaLeft, const int& nTop,
			  const int& nAreaRight, const int& nBottom);
  void Destroy(void) { Clean(); Init(); }
  bool SetMovArea(const RECT& MovArea);
  void SetMousePos(const int& nPosX, 
				   const int& nPosY);
  bool Acquire(void);
  bool Unacquire(void);
  bool Update(void);
  inline const LPDIRECTINPUTDEVICE7 GetObj(void) const { return m_lpDIDMouse; };    
  
protected:
  // Operaciones de apoyo sobre el dispositivo raton
  bool SetEventNotification(void);       
  bool SetBufferedInput(void);

public:
  // Operaciones de consulta y toma de datos
  inline bool IsDetected(void) const { return (m_lpDIDMouse != NULL); }
  inline bool IsAcquired(void) const { return m_bMouseAcquired; }      
  inline bool IsButtonPressed (const word wButton) const { 
	DXASSERT(wButton < DXDIDefs::MOUSEBUTTONS); return m_bButtonsState[wButton]; 
  }
  inline int GetButtonState (const WORD wButton) const { 
	DXASSERT(wButton < DXDIDefs::MOUSEBUTTONS); return m_nButtonsSubState[wButton]; 
  }  
  inline RECT GetMovArea(void) const { return m_MovArea; }
  inline int GetXPos(void) const { return m_nPosX; }
  inline int GetYPos(void) const { return m_nPosY; }  
  inline int GetXDelta(void) const { return m_nXDelta; }
  inline int GetYDelta(void) const { return m_nYDelta; }
  inline int GetZDelta(void) const { return m_nZDelta; }

protected:
  // Inicializacion y liberacion de recursos <DXWrapper>
  void Init(void);
  void Clean(void);
};
#endif
