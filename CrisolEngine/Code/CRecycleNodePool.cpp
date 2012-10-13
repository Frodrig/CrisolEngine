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
// CRecycleNodePool.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CRecycleNodePool.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////

// Necesario al utilizar templates en archivo .h y .cpp por separado
#ifndef _CRECYCLENODEPOOL_CPP_
#define _CRECYCLENODEPOOL_CPP_
#include "CRecycleNodePool.h"

#include "SYSEngine.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el pool de nodos, estableciendo la posicion y el iterador.
//   Si udMaxSize es 0, el reciclamento sera manual. Si es mayor sera 
//   automatico y estara en relacion al propio valor de udMaxSize.
// Parametros:
// - udMaxSize. Tamaño maximo para el reciclamiento si es que vale distinto
//   de 0.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar
///////////////////////////////////////////////////////////////////////////////
template<class Node> 
bool 
CRecycleNodePool<Node>::Init(const dword udMaxSize)
{
  // ¿Se intenta reinicializar?
  if (IsInitOk()) { return false; }

  // Guarda vbles
  m_udMaxSize = udMaxSize;

  // Inicializa iteradores
  ResetPool();
  m_bIsInitOk = true;

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia, liberando de memoria todos los punteros a los nodos
//   que se hallen en el pool de memoria.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
template<class Node> 
void 
CRecycleNodePool<Node>::End(void)
{
  // Finaliza instancia si procede
  if (m_bIsInitOk) {
	// Procede a borrar todos los nodos
	ItList It = m_Pool.begin();
	while (It != m_Pool.end()) {
	  ASSERT(*It);
	  delete *It;
	  It = m_Pool.erase(It);
	}
	// Inicializa iteradores
	ResetPool();
	// Baja flag
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se obtiene un nuevo nodo del pool. En caso de que el pool este vacio o
//   o todos los elementos que contiene esten siendo usados, se creara un
//   nuevo nodo, se insertara en pool y se devolvera su direccion.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
template<class Node> 
Node* 
CRecycleNodePool<Node>::GetNewNode(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Hay reciclado automatico?
  if (m_udMaxSize && m_udMaxSize == m_uwItPos) {	  
	  ResetPool();	
  }

  // Vbles
  Node* pNode = NULL; // Nodo a devolver

  // ¿Hay algun nodo en el pool que se pueda usar?  
  if (!m_uwItPos || m_Pool.size() == m_uwItPos) {
	// No hay nodos disponibles, hay que crear uno e insertarlo en el pool
	pNode = new Node;
	ASSERT(pNode);	
	m_Pool.push_back(pNode);
	// Se actualiza el iterador y posicion segun proceda
	if (!m_uwItPos) { 
	  m_ItPool = m_Pool.begin(); 
	} else { 
	  ++m_ItPool; 
	}		
	++m_uwItPos;
	ASSERT((m_ItPool != m_Pool.end()) != 0);
  } else {
	// Hay nodos disponibles, se toma el actual libre
	pNode = *m_ItPool;

	// Se actualiza iterador y posicion
	++m_ItPool;
	++m_uwItPos;
  }  

  // Se retorna nodo conseguido
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa posicion iteradores, segun sea el tamaño de la lista de nodos.
// Parametros:
// Devuelve:
// Notas:
// - OJO, recordar que en reciclado automatico, este metodo se llamara de forma
//   autonoma
///////////////////////////////////////////////////////////////////////////////
template<class Node> 
void 
CRecycleNodePool<Node>::ResetPool(void)
{  
  // ¿Hay elementos en lista?
  if (m_Pool.size()) {
	// Se vinculan a la primera posicion
	m_ItPool = m_Pool.begin();
	m_uwItPos = 1;
  } else {
	// Se desvinculan completamente
	m_ItPool = m_Pool.end();
	m_uwItPos = 0;
  }  
}

#endif