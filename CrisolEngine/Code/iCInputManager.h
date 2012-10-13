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
// iCInputManager.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCInputManager
//
// Descripcion:
// - Define la clase que hara de interfaz general (clase base) para 
//   CInputManager. Tambien se incluiran aqui las estructuras, constantes y
//   tipos enumerados que puedan ser necesarios para otros modulos que 
//   hagan uso de la interfaz iCInputManager.
//
// Notas:
// - Las clases que actuen de interfaz global seran definidas como estructuras
//   por motivos de claridad de lectura y porque las estructuras declaran todos
//   sus miembros como publicos.
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICINPUTMANAGER_H_
#define _ICINPUTMANAGER_H_

// Cabeceras
#ifndef _INPUTDEFS_H_
#include "InputDefs.h"
#endif
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif

// Definicion de clases / estructuras / tipos referenciados
struct iCInputClient;
class CSprite;

// Interfaz CInputManager
struct iCInputManager
{ 
public:
  // Trabajo con clientes de entrada de DirectInput
  virtual bool SetInputEvent(const InputDefs::eInputEventCode& EventCode, 
                             iCInputClient* const pInputClient, 
                             const InputDefs::eInputEventState& StatesToCheck) = 0;
  virtual void UnsetInputEvent(const InputDefs::eInputEventCode& EventCode, 
                               iCInputClient* const pInputClient,
							   const InputDefs::eInputEventState& State) = 0;
  virtual void BlockInputClient(iCInputClient* const pInputClient) = 0;
  virtual void UnblockInputClient(iCInputClient* const pInputClient) = 0;
  virtual bool IsInputClientBlocked(iCInputClient* const pInputClient) = 0;

public:
  // Trabajo con clientes de entrada de caracteres
  virtual bool SetInputCharClient(iCInputClient* const pInputClient) = 0;
  virtual bool UnsetInputCharClient(iCInputClient* const pInputClient) = 0;  

public:
  // Trabajo con el raton
  virtual void SetMousePos(const sPosition& NewPos) = 0;
  virtual sPosition GetLastMousePos(void) const = 0;
};

#endif // ~ #ifdef _ICINPUTMANAGER_H_
