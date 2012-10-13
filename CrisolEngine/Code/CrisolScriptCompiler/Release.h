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
// Release.h
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Este modulo se encarga de liberar toda la memoria utilizada, para dejar
//   el sistema tal y como estaba al iniciar el proceso de compilacion.
//   Internamente, se realizara un recorrido por el arbol AST y se liberaran
//   todas los enlaces que se puedan borrar desde aqui. En el caso de direcciones
//   a elementos guardados en otros modulos, se haran llamadas a las funciones
//   precisas en estos otros modulos para liberar la memoria utilizada.
//
// Notas:
////////////////////////////////////////////////////////////////////////////////

#ifndef _RELEASE_H_
#define _RELEASE_H_

// Includes
#include "ASTree.h"
#include "TypesDefs.h"

// Definicion de funciones
void Release(sGlobal* pGlobal);

#endif