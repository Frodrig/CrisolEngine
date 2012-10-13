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
// iCScriptClient.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - iCScriptClient
//
// Descripcion:
// - Interfaz para todas las clases que tengan que trabajar con scripts, 
//   heredaran de esta interfaz.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICSCRIPTCLIENT_H_
#define _ICSCRIPTCLIENT_H_

// Includes
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _SCRIPTDEFS_H_
#include "ScriptDefs.h"
#endif
#ifndef _SCRIPTCLIENTDEFS_H_
#include "ScriptClientDefs.h"
#endif

// Interfaz iCScriptClient
struct iCScriptClient
{
public:
  // Operacion de notificacion, para cuando acabe un comando
  virtual void ScriptNotify(const RulesDefs::eScriptEvents& ScriptEvent,
							const ScriptClientDefs::eScriptNotify& Notify,							
							const dword udParams) = 0;  
};

#endif
