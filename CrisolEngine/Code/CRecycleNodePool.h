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
// CRecycleNodePool.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clase:
// - CRecycleNodePool
//
// Descripcion:
// - Esta clase se encarga de manejar un pool de instancias a un determinado
//   tipo de nodo, clase, etc. Pero con la particularidad de que estos
//   nodos son completamente reciclabes desde el mismo momento en que se
//   invoca al metodo ResetPool. En este momento, todos los nodos que se
//   hayan conseguido con el metodo GetNewNode comenzaran a ser reutilizados.
// - El uso practico de esta clase se hallara cuando se tengan que crear muchas
//   instancias a estructuras o clases que, al mismo tiempo, seran liberadas tan
//   rapidamente como sean utilizadas. En lugar de usar un delete, que es costoso
//   incluso cuando se este trabajando con CMemoryPool, esta clase comenzara a 
//   reutilizar los nodos evitando ningun tipo de desalojo de memoria.
// - El funcionamiento de la clase podra ser manual o bien automatico. En el
//   primer caso habra que llamar a ResetPool de forma manual mientras que en el
//   segundo caso, se establecera un tope maximo de tal forma que cuando la
//   clase detecte que se sobrepasa, llamara automaticamente a ResetPool.
// Notas:
// - Recordar que hay que incluir el cpp al tratarse de un template definido
//   en un .h y un .cpp
///////////////////////////////////////////////////////////////////////////////
#ifndef _CRECYCLENODEPOOL_H_
#define _CRECYCLENODEPOOL_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif

template<class Node> 
class CRecycleNodePool
{
private:
  // Tipos
  typedef std::list<Node*> List;   // Lista de nodos
  typedef List::iterator   ItList; // Iterador a la lista de nodos
	
private:
  // Vbles de miembro
  dword  m_udMaxSize; // Tamaño maximo para automatico. Si 0 es manual.
  List   m_Pool;      // Pool
  ItList m_ItPool;    // Iterador al pool
  word   m_uwItPos;   // Posicion del iterador
  bool   m_bIsInitOk; // ¿Instancia inicializada?
	
public:
  // Constructor / destructor
  CRecycleNodePool(void): m_bIsInitOk(false) { }
  ~CRecycleNodePool(void) { End(); }
	
public:
  // Inicializacion / finalizacion
  bool Init(const dword udMaxSize = 0);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }
	
public:
  // Trabajo con el Pool
  Node* GetNewNode(void);
  void ResetPool(void);
  inline word GetPoolSize(void) const { 
	ASSERT(IsInitOk());
	// Devuelve el tamaño del pool
	return m_Pool.size(); 
  }
}; // ~ CRecycleNodePool

#endif
