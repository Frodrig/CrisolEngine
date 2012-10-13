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
// Error.cpp
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Notas:
// - Consultar Error.h para mas informacion
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "Error.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <string.h>

// Funciones externas
extern sbyte* GetActFileName(void);
extern sdword GetActLine(void);

// Vbles externas
extern sbyte* yytext;
extern FILE* pOutputInfo;

// Vbles privadas
static word unNumErrors = 0;   // Num. de errores producidos
static word unNumWarnings = 0; // Num. de avisos producidos

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Funcion encargada de reportar los errores que ocurran a la salida
//   estandar de error recibiendo por parametro una cadena con formato.
// Parametros:
// - szFileName. Nombre del archivo.
// - szText. Texto donde esta el error.
// - nLine. Linea en donde se ha producido
// - szMsg. Mensaje de error con formato printf
// - ... Resto de parametros
// Devuelve:
// Notas:
// - Al reportar un error con esta funcion no se abandonara la aplicacion.
///////////////////////////////////////////////////////////////////////////////
void 
ReportErrorf(const sbyte* szFileName,
		 	 const sbyte* szText,
			 const sdword nLine,
			 const sbyte *szMsg, ...)
{
  // Se vuelca al buffer de salida estandar el texto recibido
  va_list argList;  
  va_start(argList, szMsg);
  vfprintf(pOutputInfo, szMsg, argList);
  va_end(argList);
  
  // Se vuelca el contenido del buffer de error
  assert(pOutputInfo);
  fprintf(pOutputInfo, "\n");
  fprintf(pOutputInfo, "Error> Fichero: %s, Texto: %s, Línea: %d\n", szFileName, szText, nLine);
  
  // Incrementa contador
  ++unNumErrors;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Funcion de notificacion de error estandar.
// Parametros:
// - szMsg. Mensaje de error.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
int
yyerror(char *szMsg)
{
  // Se vuelca el contenido del buffer de error
  if (GetActFileName()) {
	fprintf(pOutputInfo, "Error> Fichero: %s, Línea: %d\n",  GetActFileName(), GetActLine());
    fprintf(pOutputInfo, "     | Código: %s.\n", yytext);
  } else {
	fprintf(pOutputInfo, "Error> Se alcazó el final del fichero con problemas.\n");
	fprintf(pOutputInfo, "     | El error puede haberse provocado por no cerrar\n");
	fprintf(pOutputInfo, "     | una secuencia begin / end en el último fichero\n");
	fprintf(pOutputInfo, "     | visitado o en anteriores.\n");
  }
  	
  // Se incrementa contador
  ++unNumErrors;

  fprintf(pOutputInfo, "     | Interrupción de la compilación.\n");    

  // Retorna
  return 1; 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Funcion para la notificacion de un aviso.
// Parametros:
// - szFileName. Nombre del archivo donde ocurre el aviso.
// - nLinea. Linea en donde ocurre.
// - szMsg. Mensaje de aviso
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ReportWarningf(const sbyte* szFileName,
			   const sdword nLine,
			   const sbyte* szMsg, ...)
{
  // Se vuelca al buffer de salida estandar el texto recibido
  va_list argList;  
  va_start(argList, szMsg);
  vfprintf(pOutputInfo, szMsg, argList);
  va_end(argList);
  
  // Se vuelca el contenido del buffer de error y se sale
  assert(pOutputInfo);
  fprintf(pOutputInfo, "\n");
  fprintf(pOutputInfo, "Advertencia> Fichero: %s, Línea: %d\n", szFileName, nLine);    

  // Se incrementa contador
  ++unNumWarnings;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Muestra el recuento de errores y warnings
// Parametros:
// Devuelve:
// - El numero de errores encontrados.
// Notas:
///////////////////////////////////////////////////////////////////////////////
word
ErrorReport(void)
{
  // Muestra el recuento de errores y warnings
  assert(pOutputInfo); 
  fprintf(pOutputInfo, "\nErrores localizados: %d.\n", unNumErrors);
  fprintf(pOutputInfo, "Advertencias localizadas: %d.\n", unNumWarnings);
  return unNumErrors;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retorna el numero de errores actuales.
// Parametros:
// Devuelve:
// - El numero de errores actuales
// Notas:
///////////////////////////////////////////////////////////////////////////////
word 
GetNumErrors(void)
{
  // Retorna el num. de errores actuales
  return unNumErrors;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Indica el archivo sobre el que se esta realizando la compilacion
// Parametros:
// - szFileName. Nombre del archivo.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ReportSetFileBuffer(sbyte* szFileName)
{
  // Indica el nombre del archivo
  //fprintf(pOutputInfo, "Parseando <%s>\n", szFileName);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Indica el archivo sobre el que se ha retornado para seguir realizando la 
//   compilacion
// Parametros:
// - szFileName. Nombre del archivo.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ReportReturnToFileBuffer(sbyte* szFileName)
{
  // Indica el nombre del archivo
  //fprintf(pOutputInfo, "Continúa parseando <%s>\n", szFileName);
}