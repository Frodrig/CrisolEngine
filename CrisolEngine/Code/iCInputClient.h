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
// iCInputClient.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCInputClient
//
// Descripcion:
// - Esta clase define el interfaz que toda clase que quiera poderse instalar
//   como cliente en CInputManager, debera de poseer. Para ello, bastara con
//   heredar de iCInputClient e insertar codigo en los metodos virtuales 
//   puros.
//
// Notas:
// - DispatchEvent debera de devolver true en caso de que el cliente se quede
//   con el evento y false en caso de que permita que pase al siguiente
//   cliente
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICINPUTCLIENT_H_
#define _ICINPUTCLIENT_H_

// Includes
#ifndef _INPUTDEFS_H_
#include "InputDefs.h"
#endif

// Interfaz iCInputClient
struct iCInputClient
{  
public:
  // Operaciones de instalacion de clientes y manejo de eventos  
  virtual bool DispatchEvent(const InputDefs::sInputEvent& aInputEvent) = 0;  
};

#endif // ~ #ifdef _ICINPUTCLIENT_H_
