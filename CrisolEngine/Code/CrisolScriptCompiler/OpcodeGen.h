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
// OpcodeGen.h
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Modulo destinado a la generacion de la lista de opcodes de la seccion
//   global, scripts y funciones. Estos opcodes generados estaran sin 
//   ningun tipo de optimizacion.
// - A la hora de generar el codigo se seguira el siguiente principio:
//   * Toda sentencia (STM) dejara invariante la altura de la pila.
//   * Toda expresion aumentara en uno la altura de la pila.
// - Una vez que se haya generado la lista de opcodes de una funcion y script,
//   se recorrera para hallar la altura maxima que alcanza la pila. Este valor
//   se almacenara pues permitira omitir la llamada a una funcion o script 
//   cuando no haya suficiente espacio en la pila.
//
// Notas:
////////////////////////////////////////////////////////////////////////////////

#ifndef _OPCODEGEN_H_
#define _OPCODEGEN_H_

// Includes
#include "ASTree.h"
#include "TypesDefs.h"

// Definicion de funciones
void OpcodeGenGlobal(sGlobal* pGlobal);

#endif