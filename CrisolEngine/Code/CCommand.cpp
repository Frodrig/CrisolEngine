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
// CCommand.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CCommand.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CCommand.h"

#include "SYSEngine.h"
#include "iCCommandManager.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destructor. Realizara una llamada a End y desinstalara el comando de la
//   cola si procede.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
CCommand::~CCommand(void) 
{
  // Lo desistala de cola si procede
  if (m_bInQueue) {	
	SYSEngine::GetCommandManager()->QuitCommand(this);	
  }

  // Finaliza comando
  End(); 
}
