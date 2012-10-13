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
// SymTableCheck.h
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - En este modulo se realizara el chequeo del AST para la construccion de
//   las tablas de simbolos.
//   Existiran dos pasadas, en una primera pasada, se chequearan todos los
//   identificadores de constantes y variables y en el caso de las funciones,
//   simplemente seran almacenadas en la tabla de simbolos. En la segunda 
//   pasada se comprobara que todos los identificadores de las funciones
//   se hallen en la tabla de simbolos.
// - En este modulo tambien se comprobara que funciones son llamadas y cuales
//   no. Todas aquellas funciones que no sean llamadas seran descartadas de los
//   procesos que tengan que llevarse en los modulos siguientes.
// - Se usara una tabla hash para el control de la tabla de simbolos, cuyos
//   nodos e interfaces estan definidos en "ASTree.h"
//
// Notas:
////////////////////////////////////////////////////////////////////////////////

#ifndef _SYMTABLECHECK_H_
#define _SYMTABLECHECK_H_

// Includes
#include "ASTree.h"
#include "TypesDefs.h"

// Funciones para la realizacion del chequeo del AST usando la tabla de simbolos
void SymTableStartCheck(sGlobal* pGlobal);

#endif