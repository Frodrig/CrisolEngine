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
// OpcodeAssembling.cpp
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Notas:
// - Consultar OpcodeAssembling.h para mas informacion
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "OpcodeAssembling.h"
#include "Memory.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

// Enumerados
enum {
  SCRIPT_EVENT = 0,
  LOCAL_FUNC = 1,
  GLOBAL_FUNC = 2
};

// Constantes
const byte ubHighVersion = 1;
const byte ubLowVersion = 0;

// Estructuras
typedef struct sStringNode {
  // Nodo para el control de un string en la tabla de strings
  // Nota: Se guardara solo la direccion del string NO se mantendra aqui
  // el string en memoria.
  sbyte*               pszString; // Direccion del string
  struct sStringNode* pNextNode; // Enlace a sig. nodo
} sStringNode;

typedef struct sStringTable {
  // Tabla de strings
  dword        unNumStrings; // Numero de nodos en la lista
  sStringNode* pRoot;        // Enlace al primer nodo de la lista
} sStringTable;

// Funciones privadas / apoyo
static void EmitBinaryOpcodes(sOpcode* pOpList,						   
							  sLabel* pLabelList,
	   						  sSymbolTable* pSymTable,
							  FILE* pFile);
static void WriteScriptEventSignature(sScriptType* pType,
									  FILE* pFile);									  
static void WriteSignature(sbyte* szSignature,
						   FILE* pFile);
static void WriteScriptIndex(sScript* pScript,
							 word unNumScripts,
							 FILE* pFile);
static void WriteScriptsIndexOffsets(sScript* pScript,
									 FILE* pFile);

// Funciones privadas / Recorrido del AST
static void OpcodeAssemblingScript(sScript* pScript,
								   word* punNumScripts,
								   FILE* pFile);
static void OpcodeAssemblingImport(sImport* pImport,
								   word unFuncType,
								   sSymbolTable* pSymbolTable,
								   FILE* pFile);
static void OpcodeAssemblingFunc(sFunc* pFunc,
								 word unFuncType,
								 sSymbolTable* pSymbolTable,
								 FILE* pFile);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre una lista de opcodes y emite el codigo binario de los mismos a
//   disco. A la vez que imprime, ira creando la tabla de strings. Una vez
//   emitidos todos los opcodes, hara lo propio con la tabla de strings generada.
// Parametros:
// - pOpList. Lista de opcodes.
// - pLabelList. Lista de labels.
// - pSymTable. Enlace a la tabla de simbolos con la que trabajara. Podra valer
//   NULL si se esta emitiendo el codigo del ambito global, pero en el caso de
//   la emision de los opcodes del script y funciones asociadas al mismo, se
//   esperara recibir la tabla de simbolos del script para hallar los nodos 
//   de los identificadores de funcion y saber asi su idx numerico.
// - pFile. Fichero donde emitir el codigo.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
EmitBinaryOpcodes(sOpcode* pOpList,						   
				  sLabel* pLabelList,
	   			  sSymbolTable* pSymTable,
				  FILE* pFile)
{
  // Se preparan vbles para la construccion de la tabla de string
  sStringTable StringTable;
  sStringNode* pLastStrNode = NULL;  

  // Vble para gestionar el numero de instrucciones escritas
  sdword sdOffsetToNumInstr; // Offset donde guardar el numero de instrucciones
  dword  udNumInstr = 0;     // Contador de instrucciones
  
  // Inicializa valores 
  StringTable.pRoot = NULL;
  StringTable.unNumStrings = 0;

  // Se deja espacio para guardar el numero de opcodes
  sdOffsetToNumInstr = ftell(pFile);
  fwrite((sbyte *)(&sdOffsetToNumInstr), 
		 sizeof(sdword),
		 1,
		 pFile);
   
  // Bucle a en la lista de opcodes
  assert(pFile);
  for (; pOpList; pOpList = pOpList->pSigOpcode) {
	// ¿El opcode no es de label?	  
	if (pOpList->OpcodeType != OP_LABEL) {
	  // Se escribe la instruccion
	  const dword udInstr = pOpList->OpcodeType;
	  fwrite((sbyte *)(&udInstr), sizeof(dword), 1, pFile);

  	  // Se incrementa contador de instr
	  ++udNumInstr;

	  // Se escriben operandos si el opcode los tiene
	  switch(pOpList->OpcodeType) {
		case OP_JMP:
		case OP_JMP_FALSE:
		case OP_JMP_TRUE:
		case OP_NJMP_EQ:
		case OP_NJMP_NE:
		case OP_NJMP_GE:
		case OP_NJMP_GT:
		case OP_NJMP_LT:
		case OP_NJMP_LE:
		case OP_SJMP_EQ:
		case OP_SJMP_NE:
		case OP_EJMP_EQ:
		case OP_EJMP_NE: {		  
		  // Se escribe el argumento, que es la posicion en codigo donde saltar
		  // Para hallar la posicion donde saltar se debera de explorar, desde
		  // la posicion actual en adelante, hasta llegar a la posicion que
		  // NO SEA un opcode de tipo label.
		  sOpcode* pPosIt = pLabelList[pOpList->JmpArg.unLabel].pPos;
		  while (pPosIt && 
				 pPosIt->OpcodeType == OP_LABEL) {
			pPosIt = pPosIt->pSigOpcode;
		  }		  
		  assert(pPosIt);
		  fwrite((sbyte *)(&pPosIt->udOpcodePos), sizeof(dword), 1, pFile);
		} break;	  
		
		case OP_NLOAD:
		case OP_SLOAD:
		case OP_ELOAD: {
		  // Se escribe el operando, referido a un slot de posicion en memoria		  
		  fwrite((sbyte *)(&pOpList->LoadArg.unAddress), 
				 sizeof(word), 
				 1, 
				 pFile);
		} break;

		case OP_NSTORE:
		case OP_SSTORE:
		case OP_ESTORE: {
		  // Se escribe el operando, referido a un slot de posicion en memoria		  
		  fwrite((sbyte *)(&pOpList->StoreArg.unAddress), 
				 sizeof(word), 
				 1, 
				 pFile);
		} break;

		case OP_NPUSH: {
		  // Se escribe el operando
		  fwrite((sbyte *)(&pOpList->NPushArg.fValue), 
				 sizeof(float), 
				 1, 
				 pFile);
		} break;

		case OP_EPUSH: {
		  // Se escribe el operando
		  fwrite((sbyte *)(&pOpList->EPushArg.unValue), 
				 sizeof(word), 
				 1, 
				 pFile);
		} break;

  		case OP_SPUSH: {
		  // Se crea nodo en la tabla de string
		  sStringNode* pStrTmpNode = ALLOC(sStringNode);
		  assert(pStrTmpNode);
		  pStrTmpNode->pszString = pOpList->SPushArg.szValue;
		  pStrTmpNode->pNextNode = NULL;

		  // Se inserta en la tabla de string
		  if (pLastStrNode) {
			// Existe al menos un elemento
			pLastStrNode->pNextNode = pStrTmpNode;
			pLastStrNode = pStrTmpNode;
		  } else {
			// Es el primer elemento introducido
			StringTable.pRoot = pStrTmpNode;
			pLastStrNode = pStrTmpNode;
		  }		  

		  // Se guarda el indice al string e incrementa el contador de strings
		  // Nota: El primer indice sera el 0
		  fwrite((sbyte *)(&StringTable.unNumStrings),
			     sizeof(dword),
				 1,
				 pFile);
		  StringTable.unNumStrings++;
		} break;

		case OP_CALL_FUNC: {
		  // Se escribe el operando
  		  sSymTableNode* pFuncDecl = SymTableGetNode(pSymTable, 
												     pOpList->CallFuncArg.szIdentifier);
		  assert(pFuncDecl);
		  fwrite((sbyte *)(&pFuncDecl->pIdFunc->FuncDecl.unFuncIdx), 
				  sizeof(word),
				  1,
				  pFile);
		} break;
	  }; // ~ switch
	}
  }

  // Escribe el numero de instrucciones halladas
  fseek(pFile, sdOffsetToNumInstr, SEEK_SET);
  fwrite((sbyte *)(&udNumInstr), sizeof(dword), 1, pFile);
  fseek(pFile, 0, SEEK_END);  

  // Procede a guardar la tabla de strings, mientras la guarda ira destruyendola
  // Nota: Los Idx de los strings se asociaran segun su posicion. El primer string
  // guardado tendra Idx 0, el siguiente 1 y asi sucesivamente.
  fwrite((sbyte *)(&StringTable.unNumStrings), sizeof(dword), 1, pFile);
  while (StringTable.pRoot) {
	// Vbles
	word unStrLenght; // Longitud del string

	// Obtiene sig. nodo y actualiza Root
	pLastStrNode = StringTable.pRoot;
	StringTable.pRoot = pLastStrNode->pNextNode;

	// Obtiene tamaño del string y escribe info del string
	unStrLenght = strlen(pLastStrNode->pszString);
	fwrite((sbyte *)(&unStrLenght), sizeof(word), 1, pFile);
	fwrite(pLastStrNode->pszString, sizeof(sbyte), unStrLenght, pFile);

	// Borra nodo
	Mem_Free(pLastStrNode);
	pLastStrNode = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea el archivo de ambito global y procede a recorrer todos los scripts
//   para crear el archivo binario con los scripts.
// Parametros:
// - pGlobal. Puntero al AST.
// - szOutputFileName. Nombre del fichero de salida.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeAssemblingGlobal(sGlobal* pGlobal,
					   sbyte* szOutputFileName)
{  
  if (pGlobal) {
	// Vbles
	sdword slOffsetToScriptIndexPos; // Pos. a offset a tabla de strings
	sdword slOffsetToScriptIndex;    // Offset a la tabla de strings	
	word   unStartOffset;	         // Comienzo del offset para mem. local
	word   unNumScripts;             // Num. de scripts
	
	// Escribe el fichero de ambito global	
	FILE* pFile = fopen(szOutputFileName, "wb");
	assert(pFile);

	// Encabezado
	// Version
	fwrite((sbyte *)(&ubHighVersion), sizeof(byte), 1, pFile);
	fwrite((sbyte *)(&ubLowVersion), sizeof(byte), 1, pFile);
	// Se guarda el offset donde se alojara el indice a tabla de scripts	
	slOffsetToScriptIndexPos = ftell(pFile);
	fwrite((sbyte *)(&slOffsetToScriptIndexPos), sizeof(sdword), 1, pFile);

	// Zona para el script global
	// Numero de variables y comienzo del offset
	fwrite((sbyte *)(&pGlobal->unNumOffsets), sizeof(word), 1, pFile);
	unStartOffset = 0;
	fwrite((sbyte *)(&unStartOffset), sizeof(word), 1, pFile);	
	// Se escribe el codigo intermedio y la tabla de strings asociada
	EmitBinaryOpcodes(pGlobal->pOpcodeList, NULL, NULL, pFile);
	
	// Emite el codigo de los scripts, obteniendo despues el num. de estos
	unNumScripts = 0;
	OpcodeAssemblingScript(pGlobal->pScript, &unNumScripts, pFile);

	// Se escribe el verdadero valor del offset en donde localizar el indice
	// de la tabla de scripts y la tabla de indices de scripts	
	slOffsetToScriptIndex = ftell(pFile);
	fseek(pFile, slOffsetToScriptIndexPos, SEEK_SET);
	fwrite((sbyte *)(&slOffsetToScriptIndex), sizeof(sdword), 1, pFile);
	
	// Se procede a escribir el indice de scripts
	fseek(pFile, 0, SEEK_END);
	WriteScriptIndex(pGlobal->pScript, unNumScripts, pFile);
	
	// Cierra archivo
	fclose(pFile);
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Escribe la firma de un evento script, apoyandose en WriteSignature.
// Parametros:
// - pType. Tipo de un script
// - pFile. Handle al fichero donde escribir los datos
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
WriteScriptEventSignature(sScriptType* pType,
						  FILE* pFile)
{
  // Procede a escribir firmas
  assert(pType);
  switch(pType->ScriptEventType) {
	case SCRIPTEVENT_ONSTARTGAME: {
	  WriteSignature("(V)N", pFile);
	} break;

	case SCRIPTEVENT_ONCLICKHOURPANEL: {
	  WriteSignature("(V)N", pFile);
	} break;

	case SCRIPTEVENT_ONFLEECOMBAT: {
	  WriteSignature("(V)N", pFile);
	} break;

	case SCRIPTEVENT_ONKEYPRESSED: {
	  WriteSignature("(V)N", pFile);
	} break;

	case SCRIPTEVENT_ONSTARTCOMBATMODE: {
	  WriteSignature("(V)N", pFile);
	} break;

	case SCRIPTEVENT_ONENDCOMBATMODE: {
	  WriteSignature("(V)N", pFile);
	} break;

	case SCRIPTEVENT_ONNEWHOUR: {
	  WriteSignature("(N)N", pFile);
	} break;

	case SCRIPTEVENT_ONENTERINAREA: {
	  WriteSignature("(N)N", pFile);
	} break;

	case SCRIPTEVENT_ONWORLDIDLE: {
	  WriteSignature("(V)N", pFile);
	} break;

	case SCRIPTEVENT_ONSETINFLOOR: {
	  WriteSignature("(NNE)N", pFile);
	} break;

	case SCRIPTEVENT_ONSETOUTOFFLOOR: {
	  WriteSignature("(NNE)N", pFile);
	} break;

	case SCRIPTEVENT_ONGETITEM: {
	  WriteSignature("(EE)N", pFile);
	} break;

	case SCRIPTEVENT_ONDROPITEM: {
	  WriteSignature("(EEN)N", pFile);
	} break;

	case SCRIPTEVENT_ONOBSERVEENTITY: {
	  WriteSignature("(EE)N", pFile);
	} break;

	case SCRIPTEVENT_ONTALKTOENTITY: {
	  WriteSignature("(EE)N", pFile);
	} break;

	case SCRIPTEVENT_ONMANIPULATEENTITY: {
	  WriteSignature("(EE)N", pFile);
	} break;

	case SCRIPTEVENT_ONDEATH: {
	  WriteSignature("(E)N", pFile);
	} break;

	case SCRIPTEVENT_ONRESURRECT: {
	  WriteSignature("(E)N", pFile);
	} break;

	case SCRIPTEVENT_ONINSERTINEQUIPMENTSLOT: {
	  WriteSignature("(EEN)N", pFile);
	} break;

	case SCRIPTEVENT_ONREMOVEFROMEQUIPMENTSLOT: {
	  WriteSignature("(EEN)N", pFile);
	} break;

	case SCRIPTEVENT_ONUSEHABILITY: {
	  WriteSignature("(ENE)N", pFile);
	} break;

	case SCRIPTEVENT_ONACTIVATEDSYMPTOM: {
	  WriteSignature("(EN)N", pFile);
	} break;

	case SCRIPTEVENT_ONDEACTIVATEDSYMPTOM: {
	  WriteSignature("(EN)N", pFile);
	} break;

	case SCRIPTEVENT_ONHITENTITY: {
	  WriteSignature("(ENE)N", pFile);
	} break;

	case SCRIPTEVENT_ONSTARTCOMBATTURN: {
	  WriteSignature("(E)N", pFile);
	} break;

	case SCRIPTEVENT_ONCRIATUREINRANGE: {
	  WriteSignature("(EE)N", pFile);
	} break;

	case SCRIPTEVENT_ONCRIATUREOUTOFRANGE: {
	  WriteSignature("(EE)N", pFile);
	} break;

	case SCRIPTEVENT_ONENTITYIDLE: {
	  WriteSignature("(E)N", pFile);
	} break;

    case SCRIPTEVENT_ONUSEITEM: {
	  WriteSignature("(EEE)N", pFile);
	} break;

	case SCRIPTEVENT_ONTRADEITEM: {
	  WriteSignature("(EEE)N", pFile);
	} break;

	case SCRIPTEVENT_ONENTITYCREATED: {
	  WriteSignature("(E)N", pFile);
	} break;

	default:
	  assert(0);
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dada una firma, la escribe en disco segun la secuencia:
//   * Tipo de retorno (sbyte)
//   * Numero de argumentos (word)
//   - Por cada argumento:
//      * Tipo del parametro (sbyte)
// - Se recordara que el formato de los parametros sera (P)R donde P seran
//   los tipos de los parametros ('V' si no tiene) y R el tipo de retorno,
//   tambien 'V' si no tiene.
// Parametros:
// - szSignature. Firma a escribir.
// - pFile. Handle al fichero donde escribir los datos
// Devuelve:
// Notas:
// - Los tipos se escribiran asociando caracteres en minuscula
//    * 'V' -> Tipo void
//    * 'S' -> Tipo string
//    * 'E' -> Tipo entity
//    * 'N' -> Tipo number
//   Cuando una firma no tenga tipo de retorno, se dira que tiene un parametro 
//   pero de tipo void ('V').
///////////////////////////////////////////////////////////////////////////////
void
WriteSignature(sbyte* szSignature,
			   FILE* pFile)
{
  // Halla el numero de parametros
  word unNumParams = 0;
  word unIt = 1;
  for (; ')' != szSignature[unIt]; ++unIt, ++unNumParams) { 
	assert(szSignature[unIt] != '\0');
  }
  assert(unNumParams);

  // Se avanza una posicion el iterador, y nos situamos en la zona de retorno,
  // escribiendo el tipo en disco
  ++unIt;
  fwrite((sbyte *)(&szSignature[unIt]), sizeof(sbyte), 1, pFile);

  // Escribe la cantidad de parametros y procede a recorrer la firma desde el
  // comienzo escribiendo los tipos de los parametros
  fwrite((sbyte *)(&unNumParams), sizeof(word), 1, pFile);    
  for (unIt = 1; ')' != szSignature[unIt]; ++unIt) {
	fwrite((sbyte *)(&szSignature[unIt]), sizeof(sbyte), 1, pFile);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Escribe la tabla de indices con la relacion de archivos scripts. En cada
//   posicion de la tabla, 
// Parametros:
// - pScript. Script a emitir.
// - unNumScripts. Numero de scripts a escribir.
// - pFile. Fichero donde emitir
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
WriteScriptIndex(sScript* pScript,
				 word unNumScripts,
				 FILE* pFile)
{
  // Escribe el numero de scripts
  fwrite((sbyte *)(&unNumScripts), sizeof(word), 1, pFile);

  // Procede a escribir la tabla de scripts usando un metodo auxiliar recursivo
  WriteScriptsIndexOffsets(pScript, pFile); 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Escribe la parte de la tabla de scripts relativa a indicar los scripts y
//   el offset en el archivo donde localizarlos. El formato sera, por cada
//   script:
//    * Tamaño del nombre del script. (word)
//    * Caracteres (sbyte)
//    * Offset donde localizarlo en el archivo (dword)
// Parametros:
// - pScript. Script a emitir.
// - pFile. Fichero donde emitir
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
WriteScriptsIndexOffsets(sScript* pScript,
						 FILE* pFile)
{
  if (pScript) {
	switch(pScript->ScriptType) {
	  case SCRIPT_SEQ: {
	    // Recorrido
	    WriteScriptsIndexOffsets(pScript->ScriptSeq.pFirst, pFile);
	    WriteScriptsIndexOffsets(pScript->ScriptSeq.pSecond, pFile);
	  } break;

	  case SCRIPT_DECL: {				
	    // Escribe los datos del script
	    const word unStrLenght = strlen(pScript->ScriptDecl.szFileName);
	    fwrite((sbyte *)(&unStrLenght), sizeof(word), 1, pFile);
	    fwrite(pScript->ScriptDecl.szFileName, sizeof(sbyte), unStrLenght, pFile);
		fwrite((sbyte *)(&pScript->ScriptDecl.slFileOffset), sizeof(sdword), 1, pFile);				
	  } break;	
	}; // ~ switch	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Emite el codigo asociado al script
// Parametros:
// - pScript. Script a emitir.
// - punNumScripts. Direccion a la vble donde depositar el num. de scripts.
// - pFile. Fichero donde emitir
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeAssemblingScript(sScript* pScript,
					   word* punNumScripts,
					   FILE* pFile)
{
  if (pScript) {
	switch(pScript->ScriptType) {
	  case SCRIPT_SEQ: {
		// Recorrido
		OpcodeAssemblingScript(pScript->ScriptSeq.pFirst, punNumScripts, pFile);
		OpcodeAssemblingScript(pScript->ScriptSeq.pSecond, punNumScripts, pFile);
	  } break;

	  case SCRIPT_DECL: {				
		// Vbles
		word unNumCodeParts; // Num. de porciones de codigo
		word uwCodeType;     // Tipo de la porcion de codigo
		word uwScriptIdx;    // Indice del script

		// Guarda el offset en donde va a comenzarse a emitirse el script
		assert(pFile);		
		pScript->ScriptDecl.slFileOffset = ftell(pFile);
		//fwrite((sbyte *)(&pScript->ScriptDecl.slFileOffset), sizeof(sdword), 1, pFile);

		// Guarda el numero de porciones de codigo totales, incluyendo al script
		unNumCodeParts = pScript->ScriptDecl.unNumFunctions + 1;
		fwrite((sbyte *)(&unNumCodeParts), sizeof(word), 1, pFile);
		
		// Escribe el codigo asociado al evento de script
		uwCodeType = pScript->ScriptDecl.pType->ScriptEventType;
		fwrite((sbyte *)(&uwCodeType), sizeof(word), 1, pFile);
		
		// Escribe el indice del script, siempre sera 0
		uwScriptIdx = 0;
		fwrite((sbyte *)(&uwScriptIdx), sizeof(word), 1, pFile);
		
		// Escribe firma del evento script
		WriteScriptEventSignature(pScript->ScriptDecl.pType, pFile);
		
		// Escribe datos asociados a la memoria local, num. vbles y offset inicial
		fwrite((sbyte *)(&pScript->ScriptDecl.unNumOffsets), sizeof(word), 1, pFile);
		fwrite((sbyte *)(&pScript->ScriptDecl.unFirstOffset), sizeof(word), 1, pFile);

		// Escribe tamaño de la pila
		fwrite((sbyte *)(&pScript->ScriptDecl.unStackSize), sizeof(word), 1, pFile);
		
		// Escribe codigo intermedio y tabla string
		EmitBinaryOpcodes(pScript->ScriptDecl.pOpcodeList,
						  pScript->ScriptDecl.pLabelList,
						  pScript->ScriptDecl.pSymTable,
						  pFile);
		
		// Se emiten los ficheros import
		OpcodeAssemblingImport(pScript->ScriptDecl.pImport, 
							   GLOBAL_FUNC, 
							   pScript->ScriptDecl.pSymTable,
							   pFile);		

		// Se emiten las funciones locales
		OpcodeAssemblingFunc(pScript->ScriptDecl.pFunc, 
							 LOCAL_FUNC, 
							 pScript->ScriptDecl.pSymTable,
							 pFile);

		// Finalmente se incrementa el contador de scripts
		assert(punNumScripts);
		(*punNumScripts)++;
	  } break;	
	}; // ~ switch	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre las funciones import para imprimirlas.
// Parametros:
// - pImport. Enlace a secuencia import.
// - unFunctType. Tipo de funcion.
// - pSymbolTable. Tabla de simbolos del script.
// - pFile. Fichero en donde imprimir.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeAssemblingImport(sImport* pImport,
					   word unFuncType,
					   sSymbolTable* pSymbolTable,
					   FILE* pFile)
{
  if (pImport) {
	switch(pImport->ImportType) {
	  case IMPORT_SEQ: {
		OpcodeAssemblingImport(pImport->ImportSeq.pFirst, unFuncType, pSymbolTable, pFile);
		OpcodeAssemblingImport(pImport->ImportSeq.pSecond, unFuncType, pSymbolTable, pFile);
	  } break;

	  case IMPORT_FUNC: {
		OpcodeAssemblingFunc(pImport->ImportFunc.pFunctions, unFuncType, pSymbolTable, pFile);
  	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Emite los opcodes asociados a funciones.
// Parametros:
// - pFunc. Enlace a la estructura con la funcion.
// - unFuncType. Tipo de funcion.
// - pSymbolTable. Tabla de simbolos del script.
// - pFile. Archivo en donde imprimir.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeAssemblingFunc(sFunc* pFunc,
					 word unFuncType,
					 sSymbolTable* pSymbolTable,
					 FILE* pFile)
{
    if (pFunc) {
	switch (pFunc->eFuncType) {
	  case FUNC_SEQ: {
		// Recorrido
		OpcodeAssemblingFunc(pFunc->FuncSeq.pFirst, unFuncType, pSymbolTable, pFile);
		OpcodeAssemblingFunc(pFunc->FuncSeq.pSecond, unFuncType, pSymbolTable, pFile);
	  } break;

	  case FUNC_DECL: {
		// ¿Fue invocada?
		if (pFunc->FuncDecl.unWasInvoked) {				  		  
		  // Escribe Idx de la funcion
		  fwrite((sbyte *)(&pFunc->FuncDecl.unFuncIdx), sizeof(word), 1, pFile);		  

		  // Escribe el codigo del tipo de funcion
		  fwrite((sbyte *)(&unFuncType), sizeof(word), 1, pFile);
		  
		  // Escribe firma de la funcion
		  WriteSignature(pFunc->FuncDecl.szSignature, pFile);
		  
		  // Escribe datos asociados a la memoria local, num. vbles y offset inicial
		  fwrite((sbyte *)(&pFunc->FuncDecl.unNumOffsets), sizeof(word), 1, pFile);
		  fwrite((sbyte *)(&pFunc->FuncDecl.unFirstOffset), sizeof(word), 1, pFile);

		  // Escribe tamaño de la pila
		  fwrite((sbyte *)(&pFunc->FuncDecl.unStackSize), sizeof(word), 1, pFile);
		  
		  // Escribe codigo intermedio y tabla string
		  EmitBinaryOpcodes(pFunc->FuncDecl.pOpcodeList,
							pFunc->FuncDecl.pLabelList,
							pSymbolTable,
							pFile);
		}
	  } break;
	}; // ~ switch
  }  
}
