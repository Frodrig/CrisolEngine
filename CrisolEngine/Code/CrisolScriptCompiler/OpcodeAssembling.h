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
// OpcodeAssembling.h
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Este modulo se encargara de emitir el codigo en formato binario. Como
//   resultado, creara dos archivos, uno de ellos contendra a las variables
//   que formaran el ambito global, mientras que el segundo contendra a todos
//   los archivos script compilados.
// - El archivo con el ambito global tendra el sig. formato:
//    * Codigo Numerico: 0XCT (1 byte)
//    * Version Mayor (1 byte)
//    * Version Menor (1 byte)
//    * Numero de vbles globales
//    - Por cada tipo de variable global, dos valores:
//      * Tipo de variable (1 sbyte)
//      * Datos iniciales de la variable.
//   El tipo de la variable podra ser 'n' si es number, 'e' si es entity y 
//   's' si es string, mientras que los datos iniciales variaran dependiendo del 
//   tipo. Los string siempre se leeran de tal forma que primero venga el tamaño 
//   en un word y luego todos los caracteres.
// - Los scripts propiamente dichos, iran almacenados en un archivo. El formato 
//   de dicho archivo sera el siguiente:
//    * Codigo Numerico: 0XCB (1 byte)
//    * Version Mayor (1 byte)
//    * Version Menor (1 byte)
//    * Offset a la tabla de indices a scripts
//    - Scripts, por cada script se guardara:
//       * Num. de porciones de codigo (Codigo del evento + funciones), 1 word
//       * Offset al indice de posiciones de codigo (1 word)
//       - Codigos, por cada codigo se guardara:
//          * Tipo de codigo, que podra ser 0 de evento, 1 de func. local y 
//            2 func. global (1 byte)
//          * Firma, compuesta por el tipo de retorno 'v' void, 'e' entity, 
//            's' string o bien 'n' para number (1 sbyte), el numero de 
//            parametros (1 word) y los tipos de los mismos (1 sbyte por cada 
//            uno)
//          - Datos de la memoria local:
//             * Num. de slots de memoria (1 word)
//             * Offset al primer slot, con respecto al ambito global
//             * Por cada slot de memoria su tipo (1 sbyte por cada uno)
//          - Datos de los strings constantes:
//             * Numero de strings (1 word)
//             * Strings, por cada uno su tamaño (word) y sus caracteres (sbyte)
//          - Codigo intermedio, indicando:
//             * Numero de instrucciones ( 1 word)
//             * Instrucciones y sus operados (1 word por instruccion y dependiendo
//               del operando 1 word o 1 float)
//       - Indice de posiciones en codigo, por cada entrada:
//          * Offset a un codigo, teniendo en cuenta que haran referencia al
//            codigo 0, codigo 1,... codigo n siendo 0 el codigo del 
//            evento del script (1 word)
//    - Tabla de indices a scripts, por cada entrada:
//       * Nombre del archivo script (tamaño, word, mas caracteres, sbyte)
//       * Offset donde localizar los datos del script.
//       
// Notas:
////////////////////////////////////////////////////////////////////////////////

#ifndef _OPCODEASSEMBLING_H_
#define _OPCODEASSEMBLING_H_

// Includes
#include "ASTree.h"
#include "TypesDefs.h"

// Definicion de funciones
void OpcodeAssemblingGlobal(sGlobal* pGlobal,
						    sbyte* szOutputFileName);

#endif