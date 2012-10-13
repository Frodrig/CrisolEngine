///////////////////////////////////////////////////////////////////////////////
// CrisolEngine - Computer Role-Play Game Engine
// Copyright (C) 2002 Fernando Rodr�guez Mart�nez
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
// iCAnimTemplateObserver.h
// Autor: Fernando Rodr�guez Mart�nez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCAnimTemplateObserver
//
// Descripcion:
// - Define la interfaz para todas aquellas clases que quieran ser 
//   observadoras de una plantilla de animacion
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICANIMTEMPLATEOBSERVER_H_
#define _ICANIMTEMPLATEOBSERVER_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _ANIMTEMPLATEOBSERVERDEFS_H_
#include "AnimTemplateObserverDefs.h"
#endif

// Defincion de clases / estructuras / espacios de nombres

// Interfaz iCAnimTemplateObserver
struct iCAnimTemplateObserver
{
public:
  // Operacion de notificacion
  virtual void AnimTemplateObserverNotify(const AnimTemplateObserverDefs::eObserverNotifyType& NotifyType,
										  const dword udParam = 0) = 0;
}; // ~ iCAnimTemplateObserver

#endif // ~ #ifdef _ICANIMTEMPLATEOBSERVER_H_