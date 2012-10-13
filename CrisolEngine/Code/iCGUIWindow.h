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
// iCGUIWindow.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces
// - iCGUIWindow
//
// Descripcion:
// - Interfaz a interfaz de ventana GUI.
//
// Notas:
// - Un interfaz de ventana GUI es un elemento logico que recoge distintos
//   compontentes CGUIComponent para implementar un sistema de interfaz
//   concreto.
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICGUIWINDOW_H_
#define _ICGUIWINDOW_H_

#ifndef _GUIDEFS_H_
#include "GUIDefs.h"
#endif

// Interfaz iCGUIWindow
struct iCGUIWindow 
{
public:
  // Notificacion de eventos para comandos y ventanas
  virtual void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent) = 0;  
};

#endif