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
// Weeding.h
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Se encarga de recorrer el arbol AST buscando posibles errores basicos,
//   esto se corresponderan a:
//   * Error al no retornar de una funcion correctamente
//   * Aviso ante retornos de funciones con codigo despues de dichos
//     retornos que no sera alcanzable.
//   * Error en divisiones por cero (siempre que se divida por una constante)
// - Tambien se encargara de asociar valores por defecto:
//   * Todas las variables que no se hallen inicializadas quedaran 
//     inicializadas a:
//      - Si number 0.
//      - Si entity NULL.
//      - Si string ""
//
// Notas:
////////////////////////////////////////////////////////////////////////////////

#ifndef _WEEDING_H_
#define _WEEDING_H_

// Includes
#include "ASTree.h"
#include "TypesDefs.h"

// Definicion de funciones
void WeedGlobal(sGlobal* pGlobal);

#endif