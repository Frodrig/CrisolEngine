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
// iCCommandClient.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - iCCommandClient
//
// Descripcion:
// - Interfaz para todas las clases que quieran actuar como clientes de la
//   notificacion en la finalizacion de un comando.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICCOMMANDCLIENT_H_
#define _ICCOMMANDCLIENT_H_

// Includes
#ifndef _COMMANDDEFS_H_
#include "CommandDefs.h"
#endif
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif

// Interfaz iCCommandClient
struct iCCommandClient
{
  // Operacion de notificacion, para cuando acabe un comando
  virtual void EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
							const dword udInstant,
							const dword udExtraParam) = 0;  
};

#endif // ~ #ifdef _ICCOMMANDCLIENT_H_
