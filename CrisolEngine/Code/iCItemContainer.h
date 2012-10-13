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
// iCItemContainer.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCItemContainer
// - iCItemContainerObserver
//
// Descripcion:
// - Se define la interfaz asociada a los contenedores de items que seran las
//   clases utilizadas para representar inventarios y/o contenido de objetos
//   de escenario que sean contenedores (por ejemplo, un baul)
// - Tambien se definira la interfaz para todos aquellas clases que sean
//   observadores de un contenedor. Ser observador de un contenedor supondra
//   recibir una notificacion cuando sobre este se inserte o quite un elemento.
//   Los observadores seran principalmente ventanas de interfaz que tengan que
//   actualizar su contenido visual ante tal hecho.
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICITEMCONTAINER_H_
#define _ICITEMCONTAINER_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _ITEMCONTAINERDEFS_H_
#include "ItemContainerDefs.h"
#endif

// Defincion de clases / estructuras / espacios de nombres
class CItemContainerIt;
struct iCItemContainerObserver;

// Interfaz iCItemContainer
struct iCItemContainer
{
public:
  // Trabajo con operaciones de insercion
  virtual void Insert(const AreaDefs::EntHandle& hItem) = 0;  

public:
  // Trabajo con operaciones de extraccion
  virtual void Extract(const AreaDefs::EntHandle& hItem) = 0;
  virtual void ExtractAll(void) = 0;

public:
  // Trabajo con observadores
  virtual inline bool AddObserver(iCItemContainerObserver* const pObserver) = 0;
  virtual inline bool RemoveObserver(iCItemContainerObserver* const pObserver) = 0;

public:
  // Metodos de apoyo a iteradores a contenedor
  virtual void InitItemContainerIt(CItemContainerIt& It) = 0;

public:
  // Obtencion de informacion
  virtual inline AreaDefs::EntHandle GetOwner(void) const = 0;
  virtual inline word GetSize(void) const = 0;
  virtual inline bool IsItemIn(const AreaDefs::EntHandle& hItem) const = 0;
}; // ~ iCItemContainer

// Interfaz iCItemContainerObserver
struct iCItemContainerObserver 
{
public:
  // Operacion de notificacion
  virtual void ItemContainerNotify(const ItemContainerDefs::eObserverNotifyType& NotifyType,
								   const AreaDefs::EntHandle& hItem) = 0;
}; // ~ iCItemContainerObserver

#endif // ~ #ifdef _ICITEMCONTAINER_H_
