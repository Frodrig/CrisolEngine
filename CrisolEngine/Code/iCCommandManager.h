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
// iCCommandManager.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - iCCommandManager
//
// Descripcion:
// - Interfaz CCommandManager
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICCOMMANDMANAGER_H_
#define _ICCOMMANDMANAGER_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSdefs.h"
#endif
#ifndef _COMMANDDEFS_H_
#include "CommandDefs.h"
#endif
//#include "iCLogicCmd.h"

// Definicion de clases / estructuras / tipos referenciados
class CCommand;
struct iCCommandClient;

// Interfaz CInputManager
struct iCCommandManager
{
public:
  // Insercion de comandos
  virtual void PostCommand(CCommand* const pCommand,
						   iCCommandClient* const pClient = NULL,
						   const CommandDefs::IDCommand& IDCommand = 0,
						   const dword udExtraParam = 0) = 0;

public:
  // Borrado de comandos en cola
  virtual void QuitCommand(CCommand* const pCommand) = 0;
};

#endif // ~ #ifdef _ICCOMMANDMANAGER_H_
