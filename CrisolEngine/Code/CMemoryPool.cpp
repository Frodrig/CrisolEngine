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
// CMemoryPool.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CMemoryPool.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CMemoryPool.h"

#ifdef _CRISOLENGINE
 #include "SYSEngine.h"
 #include "iCLogger.h" 
#else
 #include <iostream>
#endif
#include "SYSAssert.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se prepara el pool de memoria, la pila de bloques liberados y los punteros
//   de caracter auxiliar.
// Parametros:
// Devuelve:
// - El pool completamente inicializado
// Notas:
// - Este metodo debera de llamarse desde los dos constructores DESPUES de 
//   haber inicializado correctamente el resto de vbles de miembros. 
///////////////////////////////////////////////////////////////////////////////
void 
CMemoryPool::PreparePool(void)
{  
  // Se crea el pool de memoria
  m_pubMemoryPool = reinterpret_cast<byte*>(::operator new(m_nItemSize * m_uwNumBlocks));
  
  // Se crea la pila a bloques 
  m_pFreeBlocksStack = new byte*[m_uwNumBlocks];
  
  // Se inicializan punteros auxiliares
  m_pubAvailableBlocks = m_pubMemoryPool;
  m_pubLastByte = m_pubMemoryPool + m_nItemSize * m_uwNumBlocks;

  // Se comprueba incializacion correcta
  if (!m_pubMemoryPool) { 
    // Si no se puede inicializar el pool, se abandona con error fatal
    #ifdef _CRISOLENGINE
     SYSEngine::FatalError("CMemoryPool::PreparePool> No se pudo inicializar el pool de %d bloques y %d bytes creado\n", m_uwNumBlocks, m_nItemSize);
    #else 
	std::cout << "CMemoryPool::PreparePool> No se pudo inicializar el pool de "
	          << m_uwNumBlocks
			  << " bloques y "
			  << m_nItemSize
			  << " bytes creado\n";
	 exit(1);
    #endif
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se libera el pool de memoria, previamente reservado con PreparePool.
// Parametros:
// Devuelve:
// Notas:
// - Este metodo sera llamado desde el desctructor.
///////////////////////////////////////////////////////////////////////////////
void 
CMemoryPool::ReleasePool(void)
{
  // Se libera posible pool enlazado  
  if (m_pNextPool) { 
	delete m_pNextPool; 
	m_pNextPool = NULL;
  }

  // Se libera el pool de memoria  
  if (m_pubMemoryPool) { 
	delete m_pubMemoryPool; 
	m_pubMemoryPool = NULL;
  }

  // Se libera la pila  
  if (m_pFreeBlocksStack) { 
	delete[m_uwNumBlocks] m_pFreeBlocksStack; 
	m_pFreeBlocksStack = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este metodo sera el encargado de sustituir la funcionalidad del operador
//   new. Internamente, comprobara si hay bloques libres, en caso de que los
//   haya devolvera uno de ellos y en caso de que no existan, ni en el pool ni
//   en la pila, comprobara si se puede expandir para crear un pool asociado.
// Parametros:
// - nItemSize: Tamaño del item para el que se va a pedir memoria.
// Devuelve:
// - Puntero a un bloque de memoria.
// Notas:
///////////////////////////////////////////////////////////////////////////////
void* 
CMemoryPool::AllocMem(const size_t& nItemSize)
{
  // En caso de que el tamaño del item por el que se pida memoria no coincida
  // con el tamaño de m_nItemSize, se dejara la responsabilidad de crear memoria
  // al operador new global. Esto puede ocurrir cuando se intente crear memoria
  // de una clase derivada de otra que mantiene el CMemoryPool.
  if (nItemSize != m_nItemSize) {  
	return ::operator new(nItemSize); 
  }

  if (0 != m_uwStackTop) { 
    // Si hay bloques previamente liberados, se reutilizaran
    return reinterpret_cast<void*>(m_pFreeBlocksStack[--m_uwStackTop]);
  }

  // En caso de que haya bloques disponibles en el pool, se devolvera
  // un puntero a uno de ellos. En cualquier otro caso, se dejara la
  // responsabilidad de devolver un bloque al pool expandido asociado, si
  // es que hay alguno
  if (m_pubAvailableBlocks < m_pubLastByte) {
    // Se obtiene bloque
    byte* pubReturnBlock = m_pubAvailableBlocks;

    // Se actualiza puntero a sig. bloque disponible 
    m_pubAvailableBlocks += m_nItemSize;
    
    // Se devuelve puntero al bloque
    return reinterpret_cast<void*>(pubReturnBlock);
   }

  // No hay bloques disponibles
  // ¿Hay pool enlazado?
  if (m_pNextPool) { 
    // Se pasa la responsabilidad al pool enlazado
    return m_pNextPool->AllocMem(nItemSize);
  }
  else { 
    // Si es posible, se intenta crear un pool enlazado
    if (m_bExpandPool) {
      m_pNextPool = new CMemoryPool(this);
	  ASSERT(m_pNextPool);
      if (m_pNextPool) {        
        return m_pNextPool->AllocMem(nItemSize); 
      }
    }
  }

  // Han existido problemas al intentar obtener un bloque
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este es el metodo que sustitura al operador delete. Se encargara de
//   liberar un bloque de memoria e incorporarlo a la pila de bloques liberados
//   para su posterior reutilizacion. En caso de que el pool quede vacio y
//   sea un pool expandido, tambien se eliminara a si mismo, contrayendose
//   y permitiendo un ahorro de memoria.
// Parametros:
// - pFreeItem: Es la direccion de elemento que se desea liberar de memoria.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CMemoryPool::FreeMem(void* pFreeItem)
{
  // En caso de que el puntero sea nulo no se hace nada
  if (NULL == pFreeItem) { 
	return; 
  }

  // Se realiza el cast
  byte* pubFreeBlock = reinterpret_cast<byte*>(pFreeItem);

  // Si el bloque a liberar no pertenece a este pool
  if (pubFreeBlock < m_pubMemoryPool || pubFreeBlock >= m_pubLastByte) { 
    // Se comprueba si hay pool enlazado y se le deja la tarea.
    // En caso contrario, se deja la responsabilidad al operador
    // delete global    
    if (m_pNextPool) { 
	  m_pNextPool->FreeMem(pubFreeBlock); 
	} else { 
	  ::operator delete(pFreeItem); 
	}
  }
  else {
    // Vbles
    static sdword sdStackSize; // Tamaño ocupado por la pila
    static bool bPoolEmpty;    // Flag de pool vacio
    
    // El bloque pertence al pool actual. Se incorpora a la pila
    // de bloques reutilizables
    m_pFreeBlocksStack[m_uwStackTop++] = pubFreeBlock;

    // Se comprueba si el pool actual queda totalmente vacio, en cuyo
    // caso se destruira. Esto courrira siempre y cuando el Pool actual
    // sea un Pool enlazado
	// Se comprueba si hay que eliminar
    if (m_pPrevPool && 
	    !m_pNextPool) { 
      sdStackSize = static_cast<sdword>(m_uwStackTop * m_nItemSize);
      bPoolEmpty = sdStackSize == (m_pubAvailableBlocks - m_pubMemoryPool);    
      if (bPoolEmpty) { 
        // Hay que borrar la instancia actual. Como medida de seguridad
        // el pool enlazado a la izq. es utilizado para poner a NULL su
        // instancia a la dcha. que no es otra que la instancia actual a borrar
        m_pPrevPool->m_pNextPool = NULL;
        delete (this); 
      }
    }
  }
}
