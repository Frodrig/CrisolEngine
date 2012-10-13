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
// iCGUIWindowClient.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCGUIWindowClient
//
// Descripcion:
// - Interfaz para un cliente que desea recibir la notificacion de una
//   ventana de interfaz. Todas las ventanas de interfaz soportaran recibir
//   un cliente sin embargo no todas tendran necesidad de notificar algo a un
//   cliente, de ahi que CGUIManager se encargue de hacer de filtro en lo
//   relativo al envio de notificaciones.
//
// Notas:
// - La notifiacion estara compuesta por el identificador de la interfaz y
//   por un dword que tendra los parametros a enviar codificados segun
//   el formato que el interfaz origen impogan. Consultar el archivo .h de
//   dicho interfaz para mas informacion sobre la codificacion de los
//   parametros
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICGUIWINDOWCLIENT_H_
#define _ICGUIWINDOWCLIENT_H_

// Cabeceras
#ifndef _GUIMANAGERDEFS_H_
#include "GUIManagerDefs.h"
#endif

// Definicion de clases / estructuras / tipos referenciados

// Interfaz iCGUIWindowClient
struct iCGUIWindowClient
{ 
public:
  // Notificacion de un determinado suceso en un interfaz
  virtual void GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
							   const dword udParams) = 0;
};

#endif // ~ #ifdef _ICGUIWINDOWCLIENT_H_
