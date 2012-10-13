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
// InputDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Tipos asociados al subistema de entrada CInputManager.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _INPUTDEFS_H_
#define _INPUTDEFS_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h" 
#endif

namespace InputDefs
{
  // Constantes
  const bool SHARE_INPUT_EVENT    = false; // El evento se compArte con otros clientes
  const bool NO_SHARE_INPUT_EVENT = true;  // El evento no se comparte con otros clientes

  // Enumerados
  enum eInputEventCode { 
	// Eventos de teclado y raton
    // Nota: IE = InputEvent
    IE_KEY_ESC = 0, 
    IE_KEY_F1, 
	IE_KEY_F2, 
	IE_KEY_F3, 
	IE_KEY_F4, 
	IE_KEY_F5, 
	IE_KEY_F6, 
	IE_KEY_F7, 
    IE_KEY_F8, 
	IE_KEY_F9, 
	IE_KEY_F10, 
	IE_KEY_F11, 
	IE_KEY_F12,
    IE_KEY_0, 
	IE_KEY_1, 
	IE_KEY_2, 
	IE_KEY_3, 
	IE_KEY_4, 
	IE_KEY_5, 
	IE_KEY_6, 
	IE_KEY_7, 
    IE_KEY_8, 
	IE_KEY_9, 
    IE_KEY_A_HI, 
	IE_KEY_B_HI, 
	IE_KEY_C_HI, 
	IE_KEY_D_HI, 
	IE_KEY_E_HI, 
	IE_KEY_F_HI,
    IE_KEY_G_HI, 
	IE_KEY_H_HI, 
	IE_KEY_I_HI, 
	IE_KEY_J_HI, 
	IE_KEY_K_HI, 
	IE_KEY_L_HI,
    IE_KEY_M_HI, 
	IE_KEY_N_HI, 
	IE_KEY_O_HI, 
	IE_KEY_P_HI, 
	IE_KEY_Q_HI, 
	IE_KEY_R_HI, 
    IE_KEY_S_HI, 
	IE_KEY_T_HI, 
	IE_KEY_U_HI, 
	IE_KEY_V_HI, 
	IE_KEY_W_HI, 
	IE_KEY_X_HI, 
    IE_KEY_Y_HI, 
	IE_KEY_Z_HI,
    IE_KEY_BACK, 
	IE_KEY_TAB, 
	IE_KEY_RETURN, 
	IE_KEY_SPACE, 
	IE_KEY_LCONTROL, 
    IE_KEY_RCONTROL, 
	IE_KEY_LSHIFT, 
	IE_KEY_RSHIFT, 
	IE_KEY_ALT, 
	IE_KEY_ALTGR,
    IE_KEY_INSERT, 
	IE_KEY_REPAG, 
	IE_KEY_AVPAG,
    IE_KEY_MINUS_PAD, 
	IE_KEY_ADD_PAD, 
	IE_KEY_DIV_PAD, 
	IE_KEY_MUL_PAD,
    IE_KEY_0_PAD, 
	IE_KEY_1_PAD, 
	IE_KEY_2_PAD, 
	IE_KEY_3_PAD, 
	IE_KEY_4_PAD, 
    IE_KEY_5_PAD, 
	IE_KEY_6_PAD, 
	IE_KEY_7_PAD, 
	IE_KEY_8_PAD, 
	IE_KEY_9_PAD,
    IE_KEY_UP, 
	IE_KEY_DOWN, 
	IE_KEY_RIGHT, 
	IE_KEY_LEFT,
    
	// Eventos de Boton / Raton
    IE_BUTTON_MOUSE_LEFT, 
	IE_BUTTON_MOUSE_RIGHT, 
	IE_BUTTON_MOUSE_CENTER,
    
	// Eventos de Movimiento / Raton
    IE_MOVEMENT_MOUSE,	
    
	// Numero total de eventos
    MAX_EVENT_CODES
  };
  
  enum eInputEventType { 
	// Tipo de evento
    BUTTON_EVENT = 0, // Evento de boton (teclado o raton)
    MOVEMENT_EVENT,   // Evento de movimiento
	CHAR_EVENT        // Evento de entrada de un caracter
  };

  enum eInputSystem { 
	// Dispositivos de entrada
    KEYBOARD_DISP, // Teclado
    MOUSE_DISP     // Raton
  };

  enum eInputEventState { 
	// Estados posibles de los diferentes tipos de eventos
    // Eventos de boton
    BUTTON_UNPRESSED    = 0x01, // Sin pulsar
    BUTTON_PRESSED_DOWN = 0x02, // Recien pulsado
    BUTTON_HOLD_DOWN    = 0x04, // Pulsado y mantenido
    BUTTON_RELEASED     = 0x08, // Reciendo soltado    
	
	// Eventos de movimiento
    MOVEMENT_UNMOVED    = 0x10, // Si no hay movimiento
    MOVEMENT_MOVED      = 0x20, // Solo si hay movimiento    
  };  

  // Estructuras
  struct sInputInfoButtons { 
	// Informacion relativa a un evento de boton (tecla o bien boton del raton).
    eInputEventCode  Code;  // Codigo del evento
    eInputEventState State; // Estado del boton
    double           dTime; // Tiempo asociado a los estados
  };

  struct sInputInfoMovements { 
	// Informacion relativa a un evento de movimiento (raton)
	// Nota: No se usara sPosition para permitir struct en union
    dword            udXOld, udYOld; // Posiciones X e Y anteriores
    dword            udXAct, udYAct; // Posiciones X e Y actuales
	eInputEventState State;          // Estado del movimiento
  };

  struct sInputCharInfo {
	// Info asociada al evento de entrada de un caracter	
	byte ubChar; // Caracter que ha sido pulsado
  };

  struct sInputEvent { 
	// Estructura con toda la informacion del
    // evento de entrada a enviar a los clientes
    eInputEventType EventType; // Tipo de evento
    union { 
	  // Para ahorrar memoria se introduce en una union los campos 
      // con la informacion del evento de boton o de raton
      sInputInfoButtons   EventButton;    // Info. del evento de boton
      sInputInfoMovements EventMovements; // Info. del evento de movimiento
	  sInputCharInfo      CharEvent;      // Evento de entrada de un caracter
    };    
  };
};

#endif // ~ #ifdef _INPUTDEFS_H_
