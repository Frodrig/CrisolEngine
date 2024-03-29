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
// ItemContainerDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Espacio de nombres donde se definiran contantes, enumerados, tipos, etc
//   asociados al trabajo con la clase CItemContainer
///////////////////////////////////////////////////////////////////////////////
#ifndef _ITEMCONTAINERDEFS_H_
#define _ITEMCONTAINERDEFS_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif

namespace ItemContainerDefs 
{  
  // Enumerados
  enum eObserverNotifyType {
	// Codigos de notificacion para los observadores de inventario
	ITEM_INSERTED, // Un item ha sido insertado
	ITEM_REMOVED   // Un item ha sido eliminado
  };
}; // ~ namespace

#endif // ~ _ITEMCONTAINERDEFS_H_