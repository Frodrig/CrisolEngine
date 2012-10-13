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
// iCCriatureObserver.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCCriatureObserver
//
// Descripcion:
// - Define la interfaz para todas aquellas clases que quieran ser 
//   observadoras de una entidad Criatura (CCriature).
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICCRIATUREOBSERVER_H_
#define _ICCRIATUREOBSERVER_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _CRIATUREOBSERVERDEFS_H_
#include "CriatureObserverDefs.h"
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif

// Defincion de clases / estructuras / espacios de nombres

// Interfaz iCCriatureObserver
struct iCCriatureObserver
{
public:
  // Operacion de notificacion
  virtual void CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
									  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
								      const dword udParam = 0) = 0;
}; // ~ iCCriatureObserver

#endif // ~ #ifdef _ICCRIATUREOBSERVER_H_
