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
// CItemContainer.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CItemContainer.
//
// Descripcion:
// - Clase que representa una lista con el contenido del inventario de una
//   criatura o bien el contenido de un objeto de escenario que es considerado
//   contenedor (un baul, por ejemplo). 
// - Todos los contenedores tendran un dueño que estara definido por el handle
//   de la criatura a la que pertenece o el handle al objeto de escenario.
// - En el contenedor se almacenaran handles a items.
// - Para poder almacenar un item este no ha de tener dueño (ha de estar sobre
//   el suelo). Del mismo modo, al quitar un item del contenedor se dejara 
//   este sobre el tile en donde se halla su dueño.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CITEMCONTAINER_H_
#define _CITEMCONTAINER_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _ICITEMCONTAINER_H_
#include "iCItemContainer.h"
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif
#ifndef _ALGORTIHM_H_
#define _ALGORTIHM_H_
#include <algorithm>
#endif
#ifndef _FILEDEFS_H_
#include "FileDefs.h"
#endif

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Definicion de clases / estructuras / espacios de nombres

// Clase CItemContainer
class CItemContainer: public iCItemContainer
{
private:
  // Clases amigas
  // Iterador
  friend class CItemContainerIt;

private:
  // Tipos  
  typedef std::list<AreaDefs::EntHandle> ItemList;   // Lista de items
  typedef ItemList::iterator             ItemListIt; // Iterador a la lista

private:
  // Estructuras
  struct sContainerInfo {
	// Informacion asociada al container
	AreaDefs::EntHandle hOwner; // Propietario del contenedor
	ItemList            Items;  // Contenedor de items
  };

private:
  // Vbles de miembro
  sContainerInfo           m_ContainerInfo; // Info asociada al container
  iCItemContainerObserver* m_pObserver;     // Posible observer asociado
  bool					   m_bIsInitOk;     // ¿Clase inicializada?
  //bool                     m_bIsInPause;    // ¿Esta la pausa activada?
    
public:
  // Constructor / destructor
  CItemContainer(void): m_pObserver(NULL),
					    m_bIsInitOk(false)/*,
						m_bIsInPause(false)*/ { }
  ~CItemContainer(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const AreaDefs::EntHandle& hOwner);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // Operacion de almacenamiento
  void Save(const FileDefs::FileHandle& hFile, 
			dword& udOffset);

public:
  // iCItemContainer/ Trabajo con operaciones de insercion
  void Insert(const AreaDefs::EntHandle& hItem);  

public:
  // iCItemContainer / Trabajo con operaciones de extraccion
  void Extract(const AreaDefs::EntHandle& hItem);
  void ExtractAll(void);  

public:
  // iCItemContainer / Trabajo con observadores
  inline bool AddObserver(iCItemContainerObserver* const pObserver) {
	ASSERT(IsInitOk());
	// Se añade solo si no hay asociado y retorna
	if (!m_pObserver) {
	  m_pObserver = pObserver;	  
	}
	return m_pObserver ? true : false;
  }
  inline bool RemoveObserver(iCItemContainerObserver* const pObserver) {
	ASSERT(IsInitOk());
	// Elimina solo si el observer actual es igual a pObserver y retorna
	if (pObserver == m_pObserver) {
	  m_pObserver = NULL;
	}
	return (!m_pObserver) ? true : false;
  }
private:
  // Metodos de apoyo
  inline void ObserverNotify(const ItemContainerDefs::eObserverNotifyType& NotifyType,
							 const AreaDefs::EntHandle& hItem) {
	ASSERT(IsInitOk());
	ASSERT(hItem);
	// Notifica si procede
	if (m_pObserver) {
	  m_pObserver->ItemContainerNotify(NotifyType, hItem);
	}
  }

public:
  // iCItemContainer / Metodos de apoyo a iteradores a contenedor
  void InitItemContainerIt(CItemContainerIt& It);

public:
  // iCItemContainer / Obtencion de informacion
  inline AreaDefs::EntHandle GetOwner(void) const {
	ASSERT(IsInitOk());
	// Devuelve el propietario del contenedor
	return m_ContainerInfo.hOwner;
  }
  inline word GetSize(void) const {
	ASSERT(IsInitOk());
	// Retorna el tamaño del contenedor
	return m_ContainerInfo.Items.size();
  }
  inline bool IsItemIn(const AreaDefs::EntHandle& hItem) const {
	ASSERT(IsInitOk());
	// Comprueba si el item hItem se halla en el inventario
	return (std::find(m_ContainerInfo.Items.begin(),
					  m_ContainerInfo.Items.end(),
					  hItem) != m_ContainerInfo.Items.end());
  }

public:
  // Trabajo con la pausa
  void SetPause(const bool bPause);

}; // ~ CItemContainer

#endif