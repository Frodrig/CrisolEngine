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
// OpcodeEmit.h
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Modulo encargado de emitir el codigo intermedio generado. El codigo se
//   emitira por razones de inspeccion del mismo.
// - Se emitira un unico archivo en el que se hallaran los opcodes de todos 
//   los script compilados. Los opcodes se presentaran separados por bloques 
//   mediante el nombre del archivo al que pertenecen.
//
// Notas:
////////////////////////////////////////////////////////////////////////////////

#ifndef _OPCODEEMIT_H_
#define _OPCODEEMIT_H_

// Includes
#include "ASTree.h"
#include "TypesDefs.h"

// Definicion de funciones
void OpcodeEmitGlobal(sGlobal* pGlobal,
					  sbyte* szEmitFileName);

#endif