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
// CrisolScriptCompiler.cpp
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Funcion main del compilador
//
// Notas:
// - Se debera de proporcionar el nombre del fichero con las definiciones
//   globales para compilar.
// - Recordar colocar siempre #include "TypesDefs.h" en el archivo 
//   "crisolscript_tab.h" generado por flex y bison.
////////////////////////////////////////////////////////////////////////////////

// Includes
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include "ASTree.h"
#include "ASTPrinting.h"
#include "Weeding.h"
#include "SymTableCheck.h"
#include "TypeCheck.h"
#include "Resource.h"
#include "OpcodeGen.h"
#include "OpcodeEmit.h"
#include "OpcodeAssembling.h"
#include "Release.h"
#include "Error.h"

// Funciones externas
extern void SetGlobalFileBuffer(const char* FileName);
extern int yyparse(void);

// Vbles globales
sGlobal* pGlobal = NULL;       // Representa el espacio global (constantes, vbles y scripts)
char*    szGlobalsFile = NULL; // Fichero de definiciones globales
FILE*    pOutputInfo = NULL;   // Salida de la informacion producida

// Definicion de funciones locales
static void WriteHead(void);
static void WriteHelp(void);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Funcion main, punto de entrada. 
// Parametros:
// - argc. Numero de parametros incluido el propio nombre del ejecutable.
// - argv. Lista con los strings de los parametros
// Devuelve:
// - Codigo de devolucion estandar.
// Notas:
// - Se debe de usar CrisolScriptCompiler NombreArchivo
///////////////////////////////////////////////////////////////////////////////
int 
main(int argc, 
	 char* argv[])
{
  // Constantes
  const int F_FLAG = 0;
  const int E_FLAG = 1;
  const int ParamsInserted = argc - 2;	  

  // Vbles
  // Cada campo correspondera, por orden, a los parametros posibles
  int ParamsFlag[] = { 0, 0 };  
  int nIt = 0;
  
  #ifdef _DEBUG
	// Para la comprobacion de perdida de memoria via VC, usando ventana debug
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); 
	flag |= _CRTDBG_LEAK_CHECK_DF;
	_CrtSetDbgFlag(flag);
  #endif

  // Escribe la cabecera
  WriteHead();

  // ¿Se han pasado los parametros exactos?
  if (argc < 2 || argc > 4) {
	WriteHelp();
	exit(1);
	return 0;
  }
  
  // Se comprueban los argumentos recibidos y se establecen los flags oportunos	
  // Un unico parametro recibido, podra ser cualquiera
  nIt = 0;
  for (; nIt < ParamsInserted; nIt++) {
    char* szParam = argv[nIt + 1];
    if (0 == strcmpi("-f", szParam)) {
      ParamsFlag[F_FLAG] = 1;
    } else if (0 == strcmpi("-e", szParam)) {
      ParamsFlag[E_FLAG] = 1;
    } else {
  	  fprintf(stderr, "Error> El parámetro %s no esta reconocido\n\n", szParam);
	  WriteHelp();
	  exit(1);
	  return 0;
	}
  }
  	 

  // ¿Se desea enviar la informacion de salida a fichero?
  if (ParamsFlag[F_FLAG]) {
    // Si, se abre fichero
    pOutputInfo = fopen("CSResult.txt", "w");
    if (!pOutputInfo) {
  	  fprintf(stderr, "Error> Error abriendo CSResult.txt\n");
	  exit(1);
	}
  } else {
	// No, se establece como salida, la salida extandar
	pOutputInfo = stdout;
  }

  // Se establece el fichero global
  printf("Compilando...\n\n");	
  szGlobalsFile = argv[nIt + 1];
  SetGlobalFileBuffer(szGlobalsFile);	

  // Se parsea 	
  if (0 == yyparse()) {
    // Weeding
    fprintf(pOutputInfo, "Recorriendo AST por chequeos básicos (Weeding)...");
    WeedGlobal(pGlobal);
    fprintf(pOutputInfo, "Ok.\n");
    if (!GetNumErrors()) {
  	  // Chequeo de la tabla de simbolos
	  fprintf(pOutputInfo, "Construyendo la tabla de símbolos...");
	  SymTableStartCheck(pGlobal);
	  fprintf(pOutputInfo, "Ok.\n");
	  if (!GetNumErrors()) {
		// Chequeo de tipos
		fprintf(pOutputInfo, "Realizando el chequeo de tipos...");
		TypeCheckGlobal(pGlobal);
		if (!GetNumErrors()) {
		  fprintf(pOutputInfo, "Ok.\n");

		  // Calculo de recursos / etiquetas
		  fprintf(pOutputInfo, "Cálculo de recursos / etiquetas...");
		  ResourceGlobal(pGlobal);
		  if (!GetNumErrors()) {
		    fprintf(pOutputInfo, "Ok.\n");

		    // Generacion del codigo intermedio
		    fprintf(pOutputInfo, "Generación de código intermedio...");
		    OpcodeGenGlobal(pGlobal);
		    if (!GetNumErrors()) {
		  	  fprintf(pOutputInfo, "Ok.\n");
			   
			  // ¿Se desea emitir codigo intermedio?
			  if (ParamsFlag[E_FLAG]) {
			    fprintf(pOutputInfo, "Emitiendo código intermedio...");
			    OpcodeEmitGlobal(pGlobal, "CSOpcodes.txt");
			    fprintf(pOutputInfo, "Ok.\n");
			  }	  

			  // Se emite el archivo con el codigo script
			  fprintf(pOutputInfo, "Ensamblando el código intermedio generado...");
			  OpcodeAssemblingGlobal(pGlobal, "CrisolGameScripts.csb");
			  fprintf(pOutputInfo, "Ok.\n");
			} // GenGlobal
		  } // ResourceGlobal
		} // TypeCheckGlobal
	  } // SymbTableStartCheck
	} // WeedGlobal
  } // yyparse
	  
  // Se libera toda la memoria ocupada
  Release(pGlobal);

  // Muestra el resultado
  if (!ErrorReport()) {
	printf("Todo correcto.\n\n");
  } else {
	printf("\nHubo errores.\n");
  }

  // Cierra flujo de entrada
  if (ParamsFlag[F_FLAG]) {
	fclose(pOutputInfo);
  }		  

  // Finaliza
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Escribe la cabecera de ejecucion del compilador.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
WriteHead(void)
{
  // Escribe la cabecera
  printf("\n\nCSCompiler / Fernando Rodríguez <frodrig99@yahoo.com>.\n");  
  printf("Crisolengine web: <http://usuarios.lycos.es/crisolengine>.\n");  
  printf("<V:1.0>\n\n");
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Escribe la ayuda sobre la utilizacion del compilador.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
WriteHelp(void)
{
  // Escribe la ayuda
  printf("Usar: CSCompiler -f -e Fichero\n");
  printf("Siendo: -f: Escribir los mensajes en el archivo \"CSResult.txt\".\n");
  printf("        -e: Mostrar el código intermedio generado en el archivo \"CSOpcodes.txt\".\n");
  printf("Las opciones serán optativas y dara igual el orden en que se pongan\n");
  printf("siempre y cuando vayan antes de \"Fichero\".\n\n");
}