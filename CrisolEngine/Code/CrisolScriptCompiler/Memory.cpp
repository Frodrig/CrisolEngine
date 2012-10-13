///////////////////////////////////////////////////////////////////////////////
// CSCompiler - CrisolScript Compiler
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

////////////////////////////////////////////////////////////////////////////////
// Memory.cpp
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Notas:
// - Consultar Memory.h para mas informacion
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "Memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Consigue memoria y devuelve la direccion. En caso de que no haya 
//   suficiente memoria, interrumpira la ejecucion.
// Parametros:
// - unMemSize. Tamaño a reservar.
// Devuelve:
// - Puntero void a la posicion de memoria que se ha conseguido.
// Notas:
///////////////////////////////////////////////////////////////////////////////
void* 
Mem_Alloc(dword unMemSize)
{
  // Se intenta alojar la memoria
  void* pMem = malloc(unMemSize);
  if (!pMem) {
	// No hay mem. suficiente
	fprintf(stderr, "MemError> No hay memoria suficiente. La petición era de: %d bytes.\n", unMemSize);
	fflush(stderr);
	abort();
  }

  // Se retorna puntero a dir. de memoria
  return pMem;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera la memoria reservada con Mem_Alloc
// Parametros:
// - pPointer. Puntero a la zona de memoria reservada.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
Mem_Free(void* pPointer)
{  
  // ¿Puntero valido?
  if (pPointer) {
	// Si, se libera	
	free(pPointer);
  }
}