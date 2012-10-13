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
// CMemoryPool.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clase:
// - CMemoryPool
//
// Descripcion:
// - Esta clase esta destinada a la implementacion de "pool" de memoria para
//   el almacenamiento y obtencion rapida de un conjunto elevado de objetos,
//   asi como para la eficiencia en el uso de memoria cuando los objetos sobre
//   los que se realicen las instancias, no sean muy grandes (si bien, esta
//   orientado para ser usado en cualquier clase de uso masivo).
//   La filosifia general del uso es la creacion de un bloque grande de memoria
//   reservado para las instancias de un tipo de clase. Esta reserva se 
//   realizara desde el constructor de CMemoryPool. Asi, la obtencion de una
//   instancia a traves de new o delete, por parte de la clase que use esta
//   clase, sera inmediata.
//
// Notas:
// - Para poder utilizar esta clase, sera necesario definir CMemoryPool como
//   estatica en la clase anfitriona (static CMemoryPool m_MemoryManager). 
//   dicha instancia debera de ser luego, inicializada desde el exterior
//   mediante una llamada al constructor 
//   (static Class::m_MemoryManager(512, sizeof(Class), true). Donde 512 es
//   el numero de elementos a albergar, sizeof(Class) el tamaño de los 
//   objetos a albergar y true indica que en caso de que se usen todos los
//   bloques CMemoryPool se expanda.
//   Una vez hecho lo anterior, se deberan de sobrecargar los operadores new
//   y delete de la clase anfitriona para que conecten directamente con los
//   metodos de CMemoryPool. Se hara asi para new:
//   static void* operator new(const size_t size) { 
//      return m_MemoryManager.AllocMem(size) 
//   }
//   y asi para delete:
//   static void operator delete(void* pItem) { 
//      m_MemoryManager.FreeMem(pItem) 
//   }
//
// - Quedaria por implementar una mejora en el tratamiento de los bloques
//   usados y libres.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CMEMORYPOOL_H_
#define _CMEMORYPOOL_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif

// Clase CMemoryPool
class CMemoryPool
{  
private:
  // Vbles de miembro
  byte*        m_pubMemoryPool;      // Puntero al inicio del pool de memoria
  byte*        m_pubAvailableBlocks; // Puntero al primer bloque libre del pool
  byte**       m_pFreeBlocksStack;   // Pila de bloques liberados
  word         m_uwStackTop;         // Tope de la pila (0 = libre)
  byte*        m_pubLastByte;        // Puntero al ultimo byte del ultimo bloque
  CMemoryPool* m_pNextPool;          // Siguiente pool <caso de haberse llenado>
  CMemoryPool* m_pPrevPool;          // Anterior pool <caso de haberse llenado>
  word         m_uwNumBlocks;        // Numero de bloques de memoria reservados
  size_t       m_nItemSize;          // Tamaño de un bloque de memoria
  bool         m_bExpandPool;        // Flag de pool con capacidad de expansion
  
  // Constructor / Destructor
public: 
  CMemoryPool(const word uwNumBlocks, 
			  const size_t& nItemSize, 
              const bool bExpandPool = true): m_uwNumBlocks(uwNumBlocks),
                                               m_nItemSize(nItemSize),
                                               m_bExpandPool(bExpandPool),
                                               m_uwStackTop(0),
                                               m_pNextPool(NULL),
                                               m_pPrevPool(NULL) { 
	// Se prepara el pool de memoria
    PreparePool(); 
  }
protected:
  CMemoryPool(CMemoryPool* const pPrevPool): m_pPrevPool(pPrevPool),
                                             m_pNextPool(NULL),
                                             m_uwNumBlocks(pPrevPool->m_uwNumBlocks),
                                             m_nItemSize(pPrevPool->m_nItemSize),
                                             m_bExpandPool(true),
                                             m_uwStackTop(0) {
    // Se prepara el pool de memoria 
    PreparePool(); 
  }
public:
  ~CMemoryPool() { 
    // Se libera el pool de memoria
    ReleasePool();
  }

  // Metodos para alojar y liberar bloques
public:
  void* AllocMem(const size_t& ItemSize);
  void FreeMem(void* pFreeItem);

  // Metodos de apoyo
protected:
  void PreparePool(void);
  void ReleasePool(void);
};

#endif // ~ CMemoryPool
