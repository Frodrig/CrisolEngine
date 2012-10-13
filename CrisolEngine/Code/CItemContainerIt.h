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
// - CItemContainerIt.
//
// Descripcion:
// - Representa un iterador a una instancia CItemContainer.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CITEMCONTAINERIT_H_
#define _CITEMCONTAINERIT_H_

// Cabeceras
#ifndef _CITEMCONTAINER_H_
#include "CItemContainer.h"
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Definicion de clases / estructuras / espacios de nombres

// Clase CItItemContainerIt
class CItemContainerIt
{	
private:
  // Clases amigas
  // Contenedor
  friend class CItemContainer;

private:
  // Vbles de miembro 
  CItemContainer::ItemList*  m_pItems;    // Lista con los items
  CItemContainer::ItemListIt m_It;        // Iterador a la lista de items	  
  bool                       m_bIsInitOk; // ¿Iterador inicializado bien?
  
public:
  // Constructor / destructor
  CItemContainerIt(void): m_bIsInitOk(false),
						  m_pItems(NULL) { }
  CItemContainerIt(const CItemContainerIt& ItemContainerIt): m_bIsInitOk(false) {
	// Copia objeto recibido
	CopyContainerItToThis(ItemContainerIt);
  }
  CItemContainerIt(iCItemContainer& ItemContainer): m_bIsInitOk(false) {
	Init(ItemContainer);
  }
  ~CItemContainerIt(void) { End(); }
  
public:
  // Protocolos de inicializacion / finalizacion
  bool Init(iCItemContainer& ItemContainer);
  void End(void);
  bool IsInitOk(void) const { return m_bIsInitOk; }
  
public:
  // Localizacion del iterador
  inline void SetAtFront(void) { 
	ASSERT(IsInitOk());
	// Al inicio
	m_It = m_pItems->begin();
  }
  inline void SetAtBack(void) {
	ASSERT(IsInitOk());
	// Localiza al final
	SetAtFront();
	if (m_It != m_pItems->end()) {
	  std::advance(m_It, m_pItems->size() - 1);
	}
  }
  
public:
  // Navegacion
  inline void Next(void) { 
	ASSERT(IsInitOk()); 
	// Elemento siguiente
	if (IsValid()) { 
	  ++m_It; 
	}
  }  
  inline void Prev(void) { 
	ASSERT(IsInitOk());	
	// Elemento anterior
	if (IsValid()) { 
	  --m_It;	
	}
  }
  
public:
  // Obtencion de informacion
  inline AreaDefs::EntHandle GetItem(void) const {	
	ASSERT(IsInitOk());
	ASSERT(m_pItems);
	// Devuelve flag	
	return IsValid() ? *m_It : 0;
  }
  inline AreaDefs::EntHandle GetSize(void) const { 
	ASSERT(IsInitOk()); 
	// Devuelve numero de elementos
	return m_pItems->size(); 
  }
  inline bool IsValid(void) const { 
	ASSERT(IsInitOk());
	ASSERT(m_pItems);	
	// Devuelve flag de posicion valida	
	return (m_It != m_pItems->end());
  }

public:
  // Sobrecarga operador asignacion
  CItemContainerIt& operator=(const CItemContainerIt& ItemContainerIt) {	
	// Copia el objeto recibido
	CopyContainerItToThis(ItemContainerIt);
	// Retorna instancia
	return *this;
  }

private:
  // Metodos de apoyo
  void CopyContainerItToThis(const CItemContainerIt& ItemContainerIt) {
	// Copia las vbles
	m_bIsInitOk = ItemContainerIt.m_bIsInitOk;
	if (IsInitOk()) {
	  m_pItems = ItemContainerIt.m_pItems;
	  m_It = ItemContainerIt.m_It; 	
	}	
  }
}; // ~ CItemContanerIt

#endif // ~ #ifdef _CITEMCONTAINERIT_H_
