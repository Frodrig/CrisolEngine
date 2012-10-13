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
// TypeCheck.h
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Modulo destinado a la realizacion de un chequeo de los tipos. Este modulo
//   es uno de los mas complejos del FrontEnd, ya que se deben de controlar
//   de forma exhaustiva el tipo de las expresiones. Solo sera necesario 
//   realizar una pasada.
// - Durante el chequeo de tipos se creara un tipo de nodo sExp destinado a
//   hacer explicitos los "cast" entre los diferentes tipos. Asi mismo,
//   se usara un tipo indefinido (valor NULL) que ayudara a evitar los 
//   errores surgidos a partir de un punto en donde dos tipos son no 
//   asignables o no comparables y que repercuten a que salgan errores
//   laterales que no tienen porque serlo. Para solucionar esto, siempre que
//   el tipeado de dos valores sea distinto y diferente a indefinido,
//   tanto si ha habido error como si no, se creara un nodo cast explicito.
//
// Notas:
////////////////////////////////////////////////////////////////////////////////

#ifndef _TYPECHECK_H_
#define _TYPECHECK_H_

// Includes
#include "ASTree.h"

// Funciones interfaz para iniciar el chequeo
void TypeCheckGlobal(sGlobal* pGlobal);
void EndAuxTypes(void);

#endif