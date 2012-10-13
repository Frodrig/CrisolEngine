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
// Resource.h
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Este modulo se encarga de recorrer el arbol y de calcular el numero de
//   variables y constantes usadas en cada funcion / script. 
// - Se encargara de hallar las etiquetas de salto para las sentencias
//   de control y las expresiones logicas. 
// - Los dos acciones, calculo de variables (offsets en la pila de la VM) y
//   etiquetas seran necesarios para poder generar mas tarde el codigo 
//   intermedio.
// - Se encargara de asignar valores numericos a las distintas funciones
//   usadas por un script, de tal forma que la instruccion de llamada a 
//   funcion usara dichos codigos aqui asignados en lugar del nombre de la
//   funcion.
//
// Notas:
// - La asignacion de offsets se realizara utilizando una variable global.
//   Cada vez que se entre en una funcion se guardara el valor actual de offset
//   que sera restaurado a la salida. Con los scripts pasara igual.
// - En el caso de la asignacion de labels el valor actual sera mantenido con
//   una variable global que se reseteara cuando se comience a recorrer el
//   cuerpo de una funcion / script.
////////////////////////////////////////////////////////////////////////////////

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

// Includes
#include "ASTree.h"
#include "TypesDefs.h"

// Definicion de funciones
void ResourceGlobal(sGlobal* pGlobal);

#endif