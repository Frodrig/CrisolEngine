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
// DXDIDefs.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Se define el espacio de nombres con todas las definiciones y 
//   tipos existentes en el ambito DXInput
//
// Notas:
///////////////////////////////////////////////////////////////////////
#ifndef __DXDIDEFS_H_
#define __DXDIDEFS_H_

// Estoy trabajando con DX8, asi que hay que declarar este
// define para que tire con versiones anteriores
#define DIRECTINPUT_VERSION 0x0700
#include <dinput.h>

// Se define el espacio de nombres
namespace DXDIDefs
{
  // Defines
  const BYTE MOUSEBUFFERSIZE = 16; // Buffer para la recogida de datos desde el raton
  const BYTE MOUSEBUTTONS = 4;     // Botones maximos a controlar en el raton
  const BYTE LEFTBUTTON = 0;       // Boton izquierdo
  const BYTE RIGHTBUTTON = 1;      // Boton derecho

  // Macros
  #define KEYDOWN(Keyboard,Key) (Keyboard[Key] & 0x80) // ¿Key esta pulsada?

  // Enumerados 
  enum DXDIMouseState
  {// Estados posibles del raton
    MOUSE_STATE_UNPRESSED = 0,    // Raton sin pulsar
    MOUSE_STATE_PRESSEDDOWN,      // Raton se acaba de pulsar
    MOUSE_STATE_HOLDDOWN,         // Raton se mantiene pulsado
    MOUSE_STATE_RELEASED          // Raton recien liberado
  };
}
#endif