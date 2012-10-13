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
// OpcodeGen.cpp
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Notas:
// - Consultar OpcodeGen.h para mas informacion
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "OpcodeGen.h"
#include "Memory.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

// Vbles globales
sOpcode* pOpRootList = NULL;  // Enlace al comienzo de la lista de opcodes actual
sOpcode* pOpTailList = NULL;  // Enlace al final de la lista de opcodes actual
dword    udOpcodePos = 0;     // Controla las posiciones de los opcodes
sLabel*  pLabelList = NULL;   // Lista actual de labels con las que trabajar

// Funciones de apoyo
static sOpcode* SetLabel(sbyte* szLabelName,
						 word unLabelValue);
static void AttachCode(sOpcode* pNewCode);
static word GetVarIdentifierOffset(sVar* pVarTypeDecl,
										   sbyte* szIdentifier);
static sbyte PassTypeToSignature(sType* pType);
static sbyte* CalculeFuncSignature(sArgument* pArg, 
								  sType* pReturnType);
static sword GetStackModifyBySignature(sbyte* szSignature);
static sword GetStackModifyByArgPassMode(sArgument* pArgs);
static word GetMax(word unFirst,
						   word unSecond);
static void FindStackSize(sOpcode* pOpList,
			  			  sLabel* pLabels,
						  word unActStackSize,
						  word* punMaxStackSize,
						  sSymbolTable* pSymTable);
static void PushValuesOfFuncRefVars(sFunc* pFuncDecl);
static void MakeIdentifierOpcodeLoad(sbyte* szIdentifier,
									 sSymbolTable* pSymTable);

// Funciones privadas / Recorrido del AST para la generacion de Opcodes
static void OpcodeGenConst(sConst* pConst,
						   sSymbolTable* pSymTable);
static void OpcodeGenVar(sVar* pVar,
						 sSymbolTable* pSymTable);
static void OpcodeGenScript(sScript* pScript);
static void OpcodeGenScriptType(sScriptType* pScriptType,
						        sSymbolTable* pSymTable);
static void OpcodeGenImport(sImport* pImport);
static void OpcodeGenStm(sStm* pStm,
						 sFunc* pFuncDecl,
					     sSymbolTable* pSymTable);
static void OpcodeGenExp(sExp* pExp,
						 sSymbolTable* pSymTable);
static void OpcodeGenType(sType* pType);
static void OpcodeGenFunc(sFunc* pFunc);
static void OpcodeGenArgument(sArgument* pArgument,
						      sSymbolTable* pSymTable);
static void OpcodeGenParams(sExp* pParam,
						    sSymbolTable* pSymTable);

// Funciones privadas / Recorrido del AST para calcular altura de pila
static void CalculeMaxStackGlobal(sGlobal* pGlobal);
static void CalculeMaxStackScript(sScript* pScript);
static void CalculeMaxStackImport(sImport* pImport);
static void CalculeMaxStackFunc(sFunc* pFunc);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece los valores de una posicion en la lista de variables
// Parametros:
// - szLabelName. Nombre del label
// - unLabelValue. Valor de la label para su localizacion en la lista de
//   labels.
// Devuelve:
// - El opcode de tipo Label creado.
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
SetLabel(sbyte* szLabelName,
		 word unLabelValue)
{
  assert(szLabelName);
  assert(pLabelList);
  assert((pLabelList + unLabelValue) != NULL);
  
  // Se establece nombre y source
  pLabelList[unLabelValue].szLabelName = (sbyte *) Mem_Alloc(strlen(szLabelName) + 1);
  strcpy(pLabelList[unLabelValue].szLabelName, szLabelName);
  pLabelList[unLabelValue].nSources = 1;
  
  // Se crea el opcode y se retorna
  AttachCode(MakeLabelOpcode(unLabelValue, NULL));
  pLabelList[unLabelValue].pPos = pOpTailList;
  return pOpTailList;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Concatena un nodo sCode al final de la lista de opcodes. La lista de
//   opcodes sera referenciada por pOpRootList y el ultimo nodo de la misma
//   sera apuntado por pOpTailList.
// - Como ultimo paso, asociara al opcode su posicion en la lista general a
//   traves del contador de posiciones udOpcodePos. Este contador, sera 
//   inicializado desde fuera cada vez que se proceda a trabajar con un nuevo
//   bloque de codigo.
// Parametros:
// - El nuevo nodo con opcode a asociar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
AttachCode(sOpcode* pNewCode)
{
  assert(pNewCode);
  // ¿Lista vacia?
  if (!pOpRootList) {
	pOpRootList = pNewCode;
	pOpTailList = pNewCode;
  } else {
	pOpTailList->pSigOpcode = pNewCode;
	pOpTailList = pNewCode;
  }

  // Se asocia la posicion del opcode SOLO si el opcode no es Label
  if (pNewCode->OpcodeType != OP_LABEL) {
	pNewCode->udOpcodePos = udOpcodePos++;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dado un nodo sVar de tipo VarTypeDecl, recorre la secuencia de 
//   identificadores declarados para un tipo determinado, en busca de aquel
//   cuyo identificador sea igual a szIdentifier. Una vez localizado devolvera
//   el valor de su offset, calculado en el modulo Resource. 
// Parametros:
// - pVarTypeDecl. Enlace al nodo sVar.
// - szIdentifier. Nombre del identificador a localizar
// Devuelve:
// - El offset
// Notas:
// - Es seguro que el identificador debera de existir.
///////////////////////////////////////////////////////////////////////////////
word
GetVarIdentifierOffset(sVar* pVarTypeDecl,
					   sbyte* szIdentifier)
{
  // Vbles
  word unOffsetReturn = 0;
  sVar* pIt = pVarTypeDecl;
  assert(pVarTypeDecl);
  assert(szIdentifier);

  // Localiza el nombre del identificador y devuelve el offset
  for (; pIt; pIt = pIt->VarIdentDeclSeq.pSigIdent) {
    if (0 == strcmpi(pIt->VarIdentDeclSeq.szIdentifier, szIdentifier)) {
	  unOffsetReturn = pIt->VarIdentDeclSeq.unOffsetDecl;
	  break;
	 }
  }

  // Retorna
  assert(pIt);
  return unOffsetReturn;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Pasa el tipo recibido a su valor como "signature" que no sera otro que
//   la inicial del tipo en cuestion.
// Parametros:
// - pType. Tipo a utilizar.
// Devuelve:
// - Cadena con el tipo.
// Notas:
///////////////////////////////////////////////////////////////////////////////
sbyte 
PassTypeToSignature(sType* pType)
{
  assert(pType);
  switch(pType->ValueType) {
	case TYPE_VOID: {
	  return 'V';
	} break;

	case TYPE_NUMBER: {
	  return 'N';
	} break;

	case TYPE_ENTITY: {
	  return 'E';
	} break;

	case TYPE_STRING: {
	  return 'S';
	} break;
  }; // ~ switch

  return '\0';
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Calcula una cadena de caracteres con la firma de una funcion. La firma
//   de una funcion estara compuesta por las iniciales de los tipos de sus
//   argumentos entre parenteisis, seguido del tipo de retorno de la misma.
// Parametros:
// - pArg. Lista de argumentos.
// - pReturnType. Tipo de retorno
// Devuelve:
// - Firma.
// Notas:
///////////////////////////////////////////////////////////////////////////////
sbyte*
CalculeFuncSignature(sArgument* pArg, 
					 sType* pReturnType) 
{
  // Vbles
  sbyte* szSignature = NULL;
  word unNumArgs = 0;
  word unIt = 0;
  sArgument* pArgIt = NULL;

  // ¿Hay argumentos?
  if (pArg) {
	// Se obtiene el num. de argumentos
	pArgIt = pArg;
	for(; pArgIt; pArgIt = pArgIt->pSigArgument) {
	  ++unNumArgs;
	}	
  } else {
	// No hay, sin embargo se pondra el contador a 1 pues se considerara el
	// tipo void como un argumento (aunque realmente no lo sea)
	unNumArgs = 1;
  }

  // Se reserva espacio para la firma (Args)Return = 1 + x + 1 + 1 + 1(\0)
  szSignature = (sbyte*)Mem_Alloc((unNumArgs + 4) * sizeof(sbyte));
  assert(szSignature);

  // Ahora se van colocando los argumentos
  // Nota: En caso de no haber argumento, el tipo void se colocara manualmente
  szSignature[0] = '(';
  unIt = 1;	
  if (pArg) {
	pArgIt = pArg;
	for(; pArgIt; pArgIt = pArgIt->pSigArgument) {
	  szSignature[unIt++] = PassTypeToSignature(pArgIt->pType);
	}
  } else {
	szSignature[unIt++] = 'V';
  }
  assert(pReturnType);
  szSignature[unIt++] = ')';
  szSignature[unIt++] = PassTypeToSignature(pReturnType);  
  szSignature[unIt] = '\0';
  
  // Returna
  return szSignature;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Calcula los cambios que producira una funcion al ser llamada y retornar
//   a partir de su firma. Hay que tener en cuenta que por cada argumento que
//   posea la funcion, la pila decrecera de tamaño (pues se tomara el valor
//   de la pila para los argumentos) y que si la funcion tiene tipo distinto
//   a void, aumentara en una unidad la pila al retornar.
// Parametros:
// - szSignature. Firma de la funcion.
// Devuelve:
// - La variacion que produce en la pila
// Notas:
///////////////////////////////////////////////////////////////////////////////
sword 
GetStackModifyBySignature(sbyte* szSignature)
{
  // Vbles
  sword nStackModify = 0;

  // Se pasa '('
  assert(szSignature);
  ++szSignature;

  // Mientras no se pasen los argumentos
  for (; *szSignature != ')'; ++szSignature) {
	--nStackModify;
  }

  // Se pasa ')'
  ++szSignature;

  // ¿Es un retorno distino a void?
  if (*szSignature != 'v' ||
	  *szSignature != 'V') {
	++nStackModify;
  }

  // Retorna
  return nStackModify;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dada una secuencia de argumentos de una funcion, comprobara cuales de
//   ellos son de tipo ref, devolviendo la modificacion que esto supondra
//   en la pila despues de la llamada a la funcion con los argumentos recibidos.
// Parametros:
// - pArgs. Lista de argumentos.
// Devuelve:
// - La modificacion que supone en la pila
// Notas:
// - Recordar que despues de una llamada a una funcion que tiene uno o mas
//   argumentos por referencia (aquellos que modifican a la variable exterior
//   que es pasada como parametro) la funcion dejara en la pila los nuevos
//   valores para las variables exteriores.
///////////////////////////////////////////////////////////////////////////////
sword 
GetStackModifyByArgPassMode(sArgument* pArgs)
{
  // Calcula el numero de argumentos por referencia y los retorna  
  sword nNumRefArgs = 0;
  if (pArgs) {
	for (; pArgs; pArgs = pArgs->pSigArgument) {
	  if (ARGUMENT_REF == pArgs->ArgumentPassType) {
		nNumRefArgs += 1;
	  }
	}	
  }

  // Retorna
  return nNumRefArgs;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retorna el valor mayor entre los dos recibidos
// Parametros:
// - unFirst, unSecond. Valores a comparar para retornar el mayor.
// Devuelve:
// - El valor mayor.
// Notas:
///////////////////////////////////////////////////////////////////////////////
word 
GetMax(word unFirst,
	   word unSecond) 
{
  // Retorna el valor mayor
  return (unFirst > unSecond) ? unFirst : unSecond;
}	   

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre de forma recursiva la lista de opcodes y halla el tamaño maximo
//   que alcanza la pila. El tamaño maximo sera devuelto en punMaxStackSize.
// Parametros:
// - pOpList. Puntero a la lista de opcodes.
// - pLabels. Lista de labels
// - unActStackSize. Tamaño de la pila.
// - punMaxStackSize. Direccion de la vble donde depositar el tamaño maximo
//   de la pila.
// - pSymTable. Tabla de simbolos (necesaria para obtener la declaracion de
//   una funcion definida por el usuario y asi saber su signature)
// Devuelve:
// Notas:
// - Cada vez que se visite un Opcode se marcara (existe un campo en la 
//   estructura sOpcode a tal efecto)
// - Solo se podra modificara el tamaño maximo de la pila cuando esta crezca
///////////////////////////////////////////////////////////////////////////////
void 
FindStackSize(sOpcode* pOpList,
			  sLabel* pLabels,
			  word unActStackSize,
			  word* punMaxStackSize,
			  sSymbolTable* pSymTable)
{
  assert(punMaxStackSize);

  // ¿Opcode no marcado?
  if (pOpList && 
	  !pOpList->nWasVisit) {
	pOpList->nWasVisit = 1;
	switch(pOpList->OpcodeType) {
	  case OP_NOP: {
	 } break;
	
	  case OP_NNEG: {
	  } break;
	
	  case OP_NMUL: {
		unActStackSize -= 1;
	  } break;

	  case OP_NADD: {
		unActStackSize -= 1;
	  } break;

	  case OP_NMOD: {
		unActStackSize -= 1;
	  } break;

	  case OP_NDIV: {
		unActStackSize -= 1;
	  } break;

	  case OP_NSUB: {
		unActStackSize -= 1;
	  } break;

	  case OP_SADD: {
		unActStackSize -= 1;
	  } break;

	  case OP_JMP: {
		assert(pLabels);
		assert((pLabels + pOpList->JmpArg.unLabel));
		FindStackSize(pLabels[pOpList->JmpArg.unLabel].pPos, 
					  pLabels,
					  unActStackSize, 
					  punMaxStackSize,
					  pSymTable);
	  } break;

	  case OP_JMP_FALSE: {
		assert(pLabels);
		assert((pLabels + pOpList->JmpArg.unLabel));		
		unActStackSize -= 1;
		FindStackSize(pLabels[pOpList->JmpArg.unLabel].pPos, 
					  pLabels,
					  unActStackSize, 
					  punMaxStackSize,
					  pSymTable);
	  } break;

	  case OP_JMP_TRUE: {
		assert(pLabels);
		assert((pLabels + pOpList->JmpArg.unLabel));		
		unActStackSize -= 1;
		FindStackSize(pLabels[pOpList->JmpArg.unLabel].pPos, 
					  pLabels,
					  unActStackSize, 
					  punMaxStackSize,
					  pSymTable);
	  } break;

	  case OP_NJMP_EQ: {
		assert(pLabels);
		assert((pLabels + pOpList->JmpArg.unLabel));		
		unActStackSize -= 1;
		FindStackSize(pLabels[pOpList->JmpArg.unLabel].pPos, 
					  pLabels,
					  unActStackSize, 
					  punMaxStackSize,
					  pSymTable);
	  } break;

	  case OP_NJMP_NE: {
		assert(pLabels);
		assert((pLabels + pOpList->JmpArg.unLabel));		
		unActStackSize -= 1;
		FindStackSize(pLabels[pOpList->JmpArg.unLabel].pPos, 
					  pLabels,
					  unActStackSize, 
					  punMaxStackSize,
					  pSymTable);
	  } break;

	  case OP_NJMP_GE: {
		assert(pLabels);
		assert((pLabels + pOpList->JmpArg.unLabel));		
		unActStackSize -= 1;
		FindStackSize(pLabels[pOpList->JmpArg.unLabel].pPos, 
					  pLabels,
					  unActStackSize, 
					  punMaxStackSize,
					  pSymTable);
	  } break;

	  case OP_NJMP_GT: {
		assert(pLabels);
		assert((pLabels + pOpList->JmpArg.unLabel));		
		unActStackSize -= 1;
		FindStackSize(pLabels[pOpList->JmpArg.unLabel].pPos, 
					  pLabels,
					  unActStackSize, 
					  punMaxStackSize,
					  pSymTable);
	  } break;

	  case OP_NJMP_LT: {
		assert(pLabels);
		assert((pLabels + pOpList->JmpArg.unLabel));		
		unActStackSize -= 1;
		FindStackSize(pLabels[pOpList->JmpArg.unLabel].pPos, 
					  pLabels,
					  unActStackSize, 
					  punMaxStackSize,
					  pSymTable);
	  } break;

	  case OP_NJMP_LE: {
		assert(pLabels);
		assert((pLabels + pOpList->JmpArg.unLabel));		
		unActStackSize -= 1;
		FindStackSize(pLabels[pOpList->JmpArg.unLabel].pPos, 
					  pLabels,
					  unActStackSize, 
					  punMaxStackSize,
					  pSymTable);
	  } break;

	  case OP_SJMP_EQ: {
		assert(pLabels);
		assert((pLabels + pOpList->JmpArg.unLabel));		
		unActStackSize -= 1;
		FindStackSize(pLabels[pOpList->JmpArg.unLabel].pPos, 
					  pLabels,
					  unActStackSize, 
					  punMaxStackSize,
					  pSymTable);
	  } break;

	  case OP_SJMP_NE: {
		assert(pLabels);
		assert((pLabels + pOpList->JmpArg.unLabel));		
		unActStackSize -= 1;
		FindStackSize(pLabels[pOpList->JmpArg.unLabel].pPos, 
					  pLabels,
					  unActStackSize, 
					  punMaxStackSize,
					  pSymTable);
	  } break;

	  case OP_EJMP_EQ: {
		assert(pLabels);
		assert((pLabels + pOpList->JmpArg.unLabel));		
		unActStackSize -= 1;
		FindStackSize(pLabels[pOpList->JmpArg.unLabel].pPos, 
					  pLabels,
					  unActStackSize, 
					  punMaxStackSize,
					  pSymTable);
	  } break;

	  case OP_EJMP_NE: {
		assert(pLabels);
		assert((pLabels + pOpList->JmpArg.unLabel));		
		unActStackSize -= 1;
		FindStackSize(pLabels[pOpList->JmpArg.unLabel].pPos, 
					  pLabels,
					  unActStackSize, 
					  punMaxStackSize,
					  pSymTable);
	  } break;

	  case OP_DUP: {
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_POP: {
		unActStackSize -= 1;
	  } break;

	  case OP_NRETURN: {
	  } break;

	  case OP_SRETURN: {
	  } break;

	  case OP_ERETURN: {
	  } break;

	  case OP_RETURN: {
	  } break;

	  case OP_NLOAD: {
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_SLOAD: {
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_ELOAD: {
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_NSTORE: {
		unActStackSize -= 1;
	  } break;

	  case OP_SSTORE: {
		unActStackSize -= 1;
	  } break;

	  case OP_ESTORE: {
		unActStackSize -= 1;
	  } break;

	  case OP_NPUSH: {
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_SPUSH: {
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_EPUSH: {
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_NSCAST: {
	  } break;

	  case OP_SNCAST: {
	  } break;

	  case OP_CALL_FUNC: {
		// Se localiza la declaracion en la tabla de simbolos
		sSymTableNode* pSymNode = SymTableGetNode(pSymTable, 
												  pOpList->CallFuncArg.szIdentifier);
		assert(pSymNode);
		assert(SYM_TYPE_FUNC == pSymNode->SymTableNodeType);
		assert(pSymNode->pIdFunc->FuncDecl.szSignature);
		// Se añade la variacion segun la firma
		unActStackSize += GetStackModifyBySignature(pSymNode->pIdFunc->FuncDecl.szSignature);
		// Se añade la variacion segun la forma de recibir los parametros
		unActStackSize += GetStackModifyByArgPassMode(pSymNode->pIdFunc->FuncDecl.pArguments);
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_LABEL: {
	  } break;

	  case OP_API_PASSTORGBCOLOR: {
		// (NNN)N
		unActStackSize -= 3;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_API_GETREDCOMPONENT: {
		// (N)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_API_GETGREENCOMPONENT: {
		// (N)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_API_GETBLUECOMPONENT: {
		// (N)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_API_RAND: {
		// (NN)N
		unActStackSize -= 2;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_API_GETINTEGERVALUE: {
		// (N)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_API_GETDECIMALVALUE: {
		// (N)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_API_GETSTRINGSIZE: {
		// (S)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_API_WRITETOLOGGER: {
		// (S)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_API_ENABLECRISOLSCRIPTWARNINGS: {		
		// ()V
	  } break;
	  
	  case OP_API_DISABLECRISOLSCRIPTWARNINGS: {
		// ()V
	  } break;
	  

	  case OP_API_SHOWFPS: {
		// (N)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;	  

	  case OP_API_WAIT: {
		// (N)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_GAMEOBJ_QUITGAME: {
		// ()V
	  } break;
	  
	  case OP_GAMEOBJ_WRITETOCONSOLE: {
		// (S)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_GAMEOBJ_ACTIVEADVICEDIALOG: {
		// (S)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_GAMEOBJ_ACTIVEQUESTIONDIALOG: {
		// (SN)N
		unActStackSize -= 2;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_GAMEOBJ_ACTIVETEXTREADERDIALOG: {
		// (SS)V
		unActStackSize -= 2;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_GAMEOBJ_ADDOPTIONTOTEXTSELECTORDIALOG: {
		// (NSN)N
		unActStackSize -= 3;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_GAMEOBJ_RESETOPTIONSINTEXTSELECTORDIALOG: {
		// ()V
	  } break;
	  
	  case OP_GAMEOBJ_ACTIVETEXTSELECTORDIALOG: {
		// (SN)N
		unActStackSize -= 2;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_GAMEOBJ_PLAYMIDIMUSIC: {
		// (SN)V
		unActStackSize -= 2;		
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_GAMEOBJ_STOPMIDIMUSIC: {
		// ()V	
	  } break;
	  
	  case OP_GAMEOBJ_PLAYWAVAMBIENTSOUND: {
		// (S)V
		unActStackSize -= 1;		
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_GAMEOBJ_STOPWAVAMBIENTSOUND: {
		// ()V
	  } break;
	  
	  case OP_GAMEOBJ_ACTIVETRADEITEMSINTERFAZ: {
		// (E)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_GAMEOBJ_ADDOPTIONTOCONVERSATORINTERFAZ: {
		// (NS)N
		unActStackSize -= 2;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_GAMEOBJ_RESETOPTIONSINCONVERSATORINTERFAZ: {
		// ()V
	  } break;
	  
	  case OP_GAMEOBJ_ACTIVECONVERSATORINTERFAZ: {
		// (E)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_GAMEOBJ_DESACTIVECONVERSATORINTERFAZ: {
		// ()V
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_GAMEOBJ_GETOPTIONFROMCONVERSATORINTERFAZ: {
		// ()N
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_GAMEOBJ_SHOWPRESENTATION: {
		// (S)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_GAMEOBJ_BEGINCUTSCENE: {
		// ()V
	  } break;
	  
	  case OP_GAMEOBJ_ENDCUTSCENE: {
		// ()V
	  } break;

	  case OP_GAMEOBJ_SETSCRIPT: {
		// (NS)V
		unActStackSize -= 2;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_GAMEOBJ_ISKEYPRESSED: {
		// (N)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_GETAREANAME: {
		// ()S
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_WORLDOBJ_GETAREAID: {
		// ()N
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_GETAREAWIDTH: {
		// ()N
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_GETAREAHEIGHT: {
		// ()N
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_GETHOUR: {
		// ()N
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_GETMINUTE: {
		// ()N
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_SETHOUR: {
		// (N)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_SETMINUTE: {
		// (N)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_GETENTITY: {
		// (S)E
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_GETPLAYER: {
		// ()E
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_ISFLOORVALID: {
		// (NN)N
		unActStackSize -= 2;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_GETITEMAT: {
		// (NNN)E
		unActStackSize -= 3;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_GETNUMITEMSAT: {
		// (NN)N
		unActStackSize -= 2;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;		
	  
	  case OP_WORLDOBJ_GETDISTANCE: {
		// (NNNN)N
		unActStackSize -= 4;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_CALCULEPATHLENGHT: {
		// (NNNN)N
		unActStackSize -= 4;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_LOADAREA: {
		// (NNN)V
		unActStackSize += 3;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_CHANGEENTITYLOCATION: {
		// (ENN)N
		unActStackSize -= 3;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_ATTACHCAMERATOENTITY: {
		// (EN)V
		unActStackSize -= 2;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_ATTACHCAMERATOLOCATION: {
		// (NNN)V
		unActStackSize -= 3;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_ISCOMBATMODEACTIVE: {
		// ()N
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

  	  case OP_WORLDOBJ_ENDCOMBAT: {
		// ()V		
	  } break;

	  case OP_WORLDOBJ_GETCRIATUREINCOMBATTURN: {
		// ()E
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
  
	  case OP_WORLDOBJ_GETCOMBATANT: {
		// (NN)E
		unActStackSize -= 2;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_WORLDOBJ_GETNUMBEROFCOMBATANTS: {
		// (N)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_GETAREALIGHTMODEL: {
		// ()N
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_SETIDLESCRIPTTIME: {
		// (N)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_WORLDOBJ_SETSCRIPT: {
		// (NS)V
		unActStackSize -= 2;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_DESTROYENTITY: {
		// (E)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
  
	  case OP_WORLDOBJ_CREATECRIATURE: {
		// (SNNSNNN)E
		unActStackSize -= 7;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_WORLDOBJ_CREATEWALL: {
		// (SNNSNNN)E
		unActStackSize -= 7;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_WORLDOBJ_CREATESCENARYOBJECT: {
		// (SNNSNN)E
		unActStackSize -= 6;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_WORLDOBJ_CREATEITEMABANDONED: {
		// (SNNSN)E
		unActStackSize -= 5;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
  
	  case OP_WORLDOBJ_CREATEITEMWITHOWNER: {
		// (SNSN)E
		unActStackSize -= 4;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_WORLDOBJ_SETWORLDTIMEPAUSE: {
		// (N)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_WORLDOBJ_ISWORLDTIMEINPAUSE: {
		// (V)N
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

  	  case OP_WORLDOBJ_SETELEVATIONAT: {
		// (NNN)V
		unActStackSize -= 3;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_WORLDOBJ_GETELEVATIONAT: {
		// (NN)N
		unActStackSize -= 2;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_WORLDOBJ_NEXTTURN: {
		// (E)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_WORLDOBJ_GETLIGHTAT: {
		// (NN)N
		unActStackSize -= 2;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_WORLDOBJ_PLAYWAVSOUND: {
		// (S)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_WORLDOBJ_SETSCRIPTAT: {
		// (NNNS)V
		unActStackSize -= 4;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  // Nota: A partir de aqui se contabilizan la llamada a metodos de
	  // entidades. En estos casos, siempre se pasara como primer parametro
	  // y de forma invisible para el programador, el identificador de la
	  // entidad que llama al metodo, luego en todo caso habra un aumento de
	  // una unidad en la pila.
	  // En los comentarios e denotara el handle de la entidad que invoca
	  case OP_ENTITYOBJ_GETNAME: {
		// (e)S
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_ENTITYOBJ_SETNAME: {
		// (eS)V
		unActStackSize -= 1 + 1;		
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_GETENTITYTYPE: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_GETTYPE: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_SAY: {
		// (eSSNNNN)V
		unActStackSize -= 1 + 6;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_SHUTUP: {
		// (e)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_ISSAYING: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_ATTACHGFX: {
		// (eS)V
		unActStackSize -= 1 + 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_RELEASEGFX: {
		// (eS)V
		unActStackSize -= 1 + 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_RELEASEALLGFX: {
		// (e)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_ISGFXATTACHED: {
		// (eS)N
		unActStackSize -= 1 + 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_GETNUMITEMSINCONTAINER: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_ENTITYOBJ_GETITEMFROMCONTAINER: {
		// (eN)E
		unActStackSize -= 2;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;				
	  
	  case OP_ENTITYOBJ_ISITEMINCONTAINER: {
		// (eE)N
		unActStackSize -= 1 + 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_TRANSFERITEMTOCONTAINER: {
		// (eEE)V
		unActStackSize -= 1 + 2;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_INSERTITEMINCONTAINER: {
		// (eE)V
		unActStackSize -= 1 + 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_REMOVEITEMOFCONTAINER: {
		// (eENN)V
		unActStackSize -= 1 + 3;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_SETANIMTEMPLATESTATE: {
		// (eN)V
		unActStackSize -= 2;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_SETPORTRAITANIMTEMPLATESTATE: {
		// (eN)V
		unActStackSize -= 2;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;	  
	  
	  case OP_ENTITYOBJ_SETIDLESCRIPTTIME: {
		// (eN)V
		unActStackSize -= 1 + 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_ENTITYOBJ_SETLIGHT: {
		// (eN)V
		unActStackSize -= 1 + 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	
	  case OP_ENTITYOBJ_GETLIGHT: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);		
	  } break;

	  case OP_ENTITYOBJ_GETXPOS: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	
	  case OP_ENTITYOBJ_GETYPOS: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_ENTITYOBJ_GETELEVATION: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_ENTITYOBJ_SETELEVATION: {
		// (eN)V
		unActStackSize -= 1 + 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_ENTITYOBJ_GETLOCALATTRIBUTE: {
		// (eN)N
		unActStackSize -= 1 + 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_ENTITYOBJ_SETLOCALATTRIBUTE: {
		// (eNN)V
		unActStackSize -= 1 + 2;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_GETOWNER: {
		// (e)E
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_GETCLASS: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_ENTITYOBJ_GETINCOMBATUSECOST: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_ENTITYOBJ_GETGLOBALATTRIBUTE: {
		// (eN)N
		unActStackSize -= 1 + 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_ENTITYOBJ_SETGLOBALATTRIBUTE: {
		// (eNN)V
		unActStackSize -= 1 + 2;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_GETWALLORIENTATION: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_BLOCKACCESS: {
		// (e)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_UNBLOCKACCESS: {
		// (e)V
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_ISACCESSBLOCKED: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_SETSYMPTOM: {
		// (eNN)V
		unActStackSize -= 1 + 2;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_ISSYMPTOMACTIVE: {
		// (eN)N
		unActStackSize -= 1 + 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_GETGENRE: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_GETHEALTH: {
		// (eN)N
		unActStackSize -= 1 + 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_SETHEALTH: {
		// (eNN)V
		unActStackSize -= 1 + 2;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_GETEXTENDEDATTRIBUTE: {
		// (eNN)N
		unActStackSize -= 1 + 2;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_SETEXTENDEDATTRIBUTE: {
		// (eNNN)V
		unActStackSize -= 1 + 3;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_GETLEVEL: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_SETLEVEL: {
		// (eN)
		unActStackSize -= 1 + 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_GETEXPERIENCE: {	
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_SETEXPERIENCE: {
		// (eN)V
		unActStackSize -= 1 + 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_GETINCOMBATACTIONPOINTS: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_SETACTIONPOINTS: {
		// (eN)V
		unActStackSize -= 1 + 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_ISHABILITYACTIVE: {
		// (eN)N
		unActStackSize -= 1 + 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_SETHABILITY: {
		// (eNN)V
		unActStackSize -= 1 + 2;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_USEHABILITY: {
		// (eNEN)V
		unActStackSize -= 1 + 3;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_ISRUNMODEACTIVE: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_SETRUNMODE: {
		// (eN)V
		unActStackSize -= 1 + 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_MOVETO: {
		// (eNNN)N
		unActStackSize -= 1 + 3;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_ISMOVING: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_STOPMOVING: {
		// (e)N
		unActStackSize -= 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_EQUIPITEM: {
		// (eNEN)V
		unActStackSize -= 1 + 3;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_REMOVEITEMEQUIPPED: {
		// (eNN)V
		unActStackSize -= 1 + 2;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_GETITEMEQUIPPED: {
		// (eN)E
		unActStackSize -= 1 + 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_ISITEMEQUIPPED: {
		// (eE)N
		unActStackSize -= 1 + 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_DROPITEM: {
		// (eEN)V
		unActStackSize -= 1 + 2;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_USEITEM: {
		// (eEEN)V
		unActStackSize -= 1 + 3;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_MANIPULATE: {
		// (eEN)V
		unActStackSize -= 1 + 2;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_SETTRANSPARENTMODE: {
		// (eN)V
		unActStackSize -= 1 + 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_ISTRANSPARENTMODEACTIVE: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_CHANGEANIMORIENTATION: {
		// (eN)V
		unActStackSize -= 1 + 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_ENTITYOBJ_GETANIMORIENTATION: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_SETALINGMENT: {
		// (eN)V
		unActStackSize -= 1 + 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_SETALINGMENTWITH: {
		// (eE)V
		unActStackSize -= 1 + 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_SETALINGMENTAGAINST: {
		// (eE)V
		unActStackSize -= 1 + 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_GETALINGMENT: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;
	  
	  case OP_ENTITYOBJ_HITENTITY: {
		// (eEEN)V
		unActStackSize -= 1 + 3;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_ENTITYOBJ_SETSCRIPT: {
		// (eNS)V
		unActStackSize -= 1 + 2;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);
	  } break;

	  case OP_ENTITYOBJ_ISGHOSTMOVEMODEACTIVE: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);		
	  } break;
	  
	  case OP_ENTITYOBJ_SETGHOSTMOVEMODE: {
		// (eN)V
		unActStackSize -= 1 + 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);		
	  } break;

	  case OP_ENTITYOBJ_GETRANGE: {
		// (e)N
		unActStackSize -= 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);		
	  } break;

	  case OP_ENTITYOBJ_ISINRANGE: {
		// (eE)N
		unActStackSize -= 1 + 1;
		unActStackSize += 1;
		*punMaxStackSize = GetMax(unActStackSize, *punMaxStackSize);		
	  } break;
	}; // ~ switch

	// Sig opcode
	FindStackSize(pOpList->pSigOpcode, 
				  pLabels, 
				  unActStackSize, 
				  punMaxStackSize,
				  pSymTable);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Esta funcion se llamara para generar un opcode de tipo OP_XLOAD ante un
//   identificador de constante, variable o argumento. Es necesario crear
//   una funcion por separado porque esta sera llamada desde una expresion 
//   cuando se alcance un identificador y cuando se tengan que generar el
//   Opcode de llamada a un metodo de una entidad, pues el identificador de
//   la entidad se pondra como primer parametro invisible de llamada al metodo.
// Parametros:
// - szIdentifier. Identificador a llamar.
// - pSymTable. Tabla de simbolos.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
MakeIdentifierOpcodeLoad(sbyte* szIdentifier,
						 sSymbolTable* pSymTable)
{
  // Se toma el nodo en la tabla de simbolos, obteniendo el tipo
  // del identificador y el offset asociado
  sType* pType = NULL;
  word unOffset = 0;
  sSymTableNode* pSymNode = SymTableGetNode(pSymTable, szIdentifier);
  assert(pSymNode);
  
  switch(pSymNode->SymTableNodeType) {
    case SYM_TYPE_CONST: {			
  	  unOffset = pSymNode->pIdConst->ConstDecl.unOffsetDecl;
	  pType = pSymNode->pIdConst->ConstDecl.pType;			
	} break;
		  
	case SYM_TYPE_VAR: {
	  // Se halla el offset asociado al indentificador de la vble
	  unOffset = GetVarIdentifierOffset(pSymNode->pIdVar->VarTypeDecl.pVarIdentSeq,
										szIdentifier);
	  pType = pSymNode->pIdVar->VarTypeDecl.pType;
	} break;

	case SYM_TYPE_ARG: {
	  unOffset = pSymNode->pIdArg->unOffsetDecl;
	  pType = pSymNode->pIdArg->pType;
	} break;

	default:
	  assert(0);
  }; // ~ switch

  // Ahora se genera el opcode
  assert(pType);
  switch(pType->ValueType) {
    case TYPE_NUMBER: {
  	  AttachCode(MakeNLoadOpcode(unOffset, NULL));
	} break;

	case TYPE_STRING: {
	  AttachCode(MakeSLoadOpcode(unOffset, NULL));
	} break;

	case TYPE_ENTITY: {
	  AttachCode(MakeELoadOpcode(unOffset, NULL));
	} break;

	default:
	  assert(0);
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicia el recorrido por el arbol AST.
// Parametros:
// - pGlobal. Direccion a la estructura global.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeGenGlobal(sGlobal* pGlobal)
{
  if (pGlobal) {
	// Se inicializan punteros a la lista de opcodes a generar
	pOpRootList = NULL;
	pOpTailList = NULL;		
	
	// Recorrido const y vars globales
	udOpcodePos = 0;
	OpcodeGenConst(pGlobal->pConst, pGlobal->pSymTable);
	OpcodeGenVar(pGlobal->pVar, pGlobal->pSymTable);
	OpcodeGenStm(pGlobal->pReturnStm, NULL, pGlobal->pSymTable);
	
	// Scripts
	OpcodeGenScript(pGlobal->pScript);
	
	// Se guarda la lista de opcodes generada y se desvinculan punteros
	pGlobal->pOpcodeList = pOpRootList;
	pOpRootList = NULL;
	pOpTailList = NULL;			

	// Ahora se procede a recorrer de nuevo el AST pero con el fin de
	// hallar la altura maxima que alcanza la pila en los cuerpos de los
	// scripts y funciones. Es necesario hacerlo aqui porque en el calculo
	// esta involucrado el uso de la firma de las funciones y hasta este
	// punto no hay seguridad de que todas las funciones tengan firma.
	CalculeMaxStackGlobal(pGlobal);	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre declaraciones const.
// Parametros:
// - pConst. Declaraciones const
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeGenConst(sConst* pConst,
			   sSymbolTable* pSymTable)
{
  // ¿Hay declaraciones?
  if (pConst) {
	switch(pConst->ConstType) {
	  case CONST_DECLSEQ: {
		// Recorrido
		OpcodeGenConst(pConst->ConstSeq.pFirst, pSymTable);
		OpcodeGenConst(pConst->ConstSeq.pSecond, pSymTable);
	  } break;

	  case CONST_DECL: {
		// Recorrido
		OpcodeGenType(pConst->ConstDecl.pType);		
		OpcodeGenExp(pConst->ConstDecl.pExp, pSymTable);
		// Nota: es seguro que habra inicializacion al tratarse de constante
		// Se genera el opcode de carga de dicho valor en la constante
		assert(pConst->ConstDecl.pExp);
		switch(pConst->ConstDecl.pType->ValueType) {
		  case TYPE_NUMBER: {
			AttachCode(MakeNStoreOpcode(pConst->ConstDecl.unOffsetDecl, NULL));
		  } break;

		  case TYPE_ENTITY: {
			AttachCode(MakeEStoreOpcode(pConst->ConstDecl.unOffsetDecl, NULL));
		  } break;

		  case TYPE_STRING: {
			AttachCode(MakeSStoreOpcode(pConst->ConstDecl.unOffsetDecl, NULL));
		  } break;

		  default:
			assert(0);
		}; // ~ switch		
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre delcaracion de variables.
// Parametros:
// - pVar. Declaracion de variables
// - pType. Tipo de la variable, desde el exterior se pasara NULL.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeGenVar(sVar* pVar,
			 sSymbolTable* pSymTable)
{
  // ¿Hay variables?
  if (pVar) {
	switch(pVar->VarType) {
	  case VAR_TYPEDECL_SEQ: {
		OpcodeGenVar(pVar->VarTypeDeclSeq.pFirst, pSymTable);
		OpcodeGenVar(pVar->VarTypeDeclSeq.pSecond, pSymTable);
	  } break;

	  case VAR_TYPEDECL: {
		OpcodeGenType(pVar->VarTypeDecl.pType);
		OpcodeGenVar(pVar->VarTypeDecl.pVarIdentSeq, pSymTable);
	  } break;

	  case VAR_IDENTIFIERDECL_SEQ: {		
		sVar* pIt = pVar;		
		for (; pIt; pIt = pIt->VarIdentDeclSeq.pSigIdent) {
		  OpcodeGenExp(pIt->VarIdentDeclSeq.pValue, pSymTable);
		  // ¿Hay valor de inicializacion?
		  if (pIt->VarIdentDeclSeq.pValue) {
			// Se genera la carga de dicho valor
			sSymTableNode* pSymNode = SymTableGetNode(pSymTable, 
												      pIt->VarIdentDeclSeq.szIdentifier);
			assert(SYM_TYPE_VAR == pSymNode->SymTableNodeType);
			switch(pSymNode->pIdVar->VarTypeDecl.pType->ValueType) {
			  case TYPE_NUMBER: {
				AttachCode(MakeNStoreOpcode(pIt->VarIdentDeclSeq.unOffsetDecl, NULL));
			  } break;

			  case TYPE_ENTITY: {
				AttachCode(MakeEStoreOpcode(pIt->VarIdentDeclSeq.unOffsetDecl, NULL));
			  } break;

			  case TYPE_STRING: {
				AttachCode(MakeSStoreOpcode(pIt->VarIdentDeclSeq.unOffsetDecl, NULL));
			  } break;

			  default:
				assert(0);
			}; // ~ switch		
		  }		
		}
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre un script.
// Parametros:
// - pScript. Script a imprimir
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeGenScript(sScript* pScript)
{
  if (pScript) {
	switch(pScript->ScriptType) {
	  case SCRIPT_SEQ: {
		// Recorrido
		OpcodeGenScript(pScript->ScriptSeq.pFirst);
		OpcodeGenScript(pScript->ScriptSeq.pSecond);
	  } break;

	  case SCRIPT_DECL: {		
		// Se guardan los punteros a la lista anterior de opcodes y se
		// inicializan los mismos
		sOpcode* pPrevOpRootList = pOpRootList;
		sOpcode* pPrevOpTailList = pOpTailList;
		dword udPrevOpcodePos = udOpcodePos;
		pOpRootList = NULL;
		pOpTailList = NULL;
		
		// Recorrido
		udOpcodePos = 0;			
		OpcodeGenScriptType(pScript->ScriptDecl.pType, pScript->ScriptDecl.pSymTable);		
		OpcodeGenImport(pScript->ScriptDecl.pImport);		
		OpcodeGenConst(pScript->ScriptDecl.pConst, pScript->ScriptDecl.pSymTable);
		OpcodeGenVar(pScript->ScriptDecl.pVar, pScript->ScriptDecl.pSymTable);
		OpcodeGenFunc(pScript->ScriptDecl.pFunc);
		
		// Al recorrer las sentencias se preparara la lista de labels
		pLabelList = (sLabel*) Mem_Alloc(pScript->ScriptDecl.unNumLabels * sizeof(sLabel));
		assert(pLabelList);		
		OpcodeGenStm(pScript->ScriptDecl.pStm, NULL, pScript->ScriptDecl.pSymTable);	
	
		// Se asocia a la funcion y se desvincula lista de labels general
		pScript->ScriptDecl.pLabelList = pLabelList;
		pLabelList = NULL;
		
		// Se guarda la lista actual y se reponen punteros a la lista previa
		pScript->ScriptDecl.pOpcodeList = pOpRootList;
		pOpRootList = pPrevOpRootList;
		pOpTailList = pPrevOpTailList;
		udOpcodePos = udPrevOpcodePos;
	  } break;	
	}; // ~ switch	
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre los tipos de scripts
// Parametros:
// - pScriptType. Tipos de scripts
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeGenScriptType(sScriptType* pScriptType,
					sSymbolTable* pSymTable)
{
  // ¿Hay tipo?
  if (pScriptType) {
	switch(pScriptType->ScriptEventType) {
	  case SCRIPTEVENT_ONSTARTGAME: {
	  } break;

	  case SCRIPTEVENT_ONCLICKHOURPANEL: {
	  } break;

	  case SCRIPTEVENT_ONFLEECOMBAT: {
	  } break;

	  case SCRIPTEVENT_ONKEYPRESSED: {
	  } break;

	  case SCRIPTEVENT_ONSTARTCOMBATMODE: {
	  } break;

	  case SCRIPTEVENT_ONENDCOMBATMODE: {
	  } break;

	  case SCRIPTEVENT_ONNEWHOUR: {
	  } break;

	  case SCRIPTEVENT_ONENTERINAREA: {
	  } break;

	  case SCRIPTEVENT_ONWORLDIDLE: {
	  } break;

	  case SCRIPTEVENT_ONSETINFLOOR: {
	  } break;

	  case SCRIPTEVENT_ONSETOUTOFFLOOR: {
	  } break;

	  case SCRIPTEVENT_ONGETITEM: {
	  } break;

	  case SCRIPTEVENT_ONDROPITEM: {
	  } break;

	  case SCRIPTEVENT_ONOBSERVEENTITY: {
	  } break;

	  case SCRIPTEVENT_ONTALKTOENTITY: {
	  } break;

	  case SCRIPTEVENT_ONMANIPULATEENTITY: {
	  } break;

	  case SCRIPTEVENT_ONDEATH: {
	  } break;

	  case SCRIPTEVENT_ONRESURRECT: {
	  } break;

	  case SCRIPTEVENT_ONINSERTINEQUIPMENTSLOT: {
	  } break;

	  case SCRIPTEVENT_ONREMOVEFROMEQUIPMENTSLOT: {
	  } break;

	  case SCRIPTEVENT_ONUSEHABILITY: {
	  } break;

	  case SCRIPTEVENT_ONACTIVATEDSYMPTOM: {
	  } break;

	  case SCRIPTEVENT_ONDEACTIVATEDSYMPTOM: {
	  } break;

	  case SCRIPTEVENT_ONHITENTITY: {
	  } break;

	  case SCRIPTEVENT_ONSTARTCOMBATTURN: {
	  } break;

	  case SCRIPTEVENT_ONCRIATUREINRANGE: {
	  } break;

	  case SCRIPTEVENT_ONCRIATUREOUTOFRANGE: {
	  } break;

	  case SCRIPTEVENT_ONENTITYIDLE: {
	  } break;

	  case SCRIPTEVENT_ONUSEITEM: {
	  } break;

	  case SCRIPTEVENT_ONTRADEITEM: {
	  } break;

	  case SCRIPTEVENT_ONENTITYCREATED: {
	  } break;
	}; // ~ switch

	OpcodeGenArgument(pScriptType->pArguments, pSymTable);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre imports
// Parametros:
// - pImport. Import a imprimir.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeGenImport(sImport* pImport)
{
  if (pImport) {
	switch(pImport->ImportType) {
	  case IMPORT_SEQ: {
		OpcodeGenImport(pImport->ImportSeq.pFirst);
		OpcodeGenImport(pImport->ImportSeq.pSecond);
	  } break;

	  case IMPORT_FUNC: {
		OpcodeGenFunc(pImport->ImportFunc.pFunctions);
  	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre una sentencia
// - Cuando las sentencias pertenezcan a una funcion (pFuncDecl != NULL) y
//   dicha funcion admita argumentos por referencia, antes de retornar con
//   return se dejara el contenido de las variables que recogieron los parametros
//   pasados para que desde el exterior las variables por referencia los tomen.
// Parametros:
// - pStm. Sentencia a imprimir.
// - pFuncDecl. Declaracion de la funcion a la que pertenencen las sentencias.
//   Si en lugar de a una funcion pertenecieran a un script su valor sera NULL.
// - pSymTable. Tabla de simbolos perteneciente al ambito sobre el que trabajar
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeGenStm(sStm* pStm,
			 sFunc* pFuncDecl,
			 sSymbolTable* pSymTable)
{
  // ¿Hay sentencia?
  if (pStm) { 
	switch(pStm->StmType) {
	  case STM_SEQ: {
		// Recorrido
		OpcodeGenStm(pStm->StmSeq.pFirst, pFuncDecl, pSymTable);
		OpcodeGenStm(pStm->StmSeq.pSecond, pFuncDecl, pSymTable);		
	  } break;

	  case STM_RETURN: {		
		// Recorrido / opcode return
		OpcodeGenExp(pStm->StmReturn.pExp, pSymTable);		

		// Se dejan los posibles valores de las vbles locales por ref para 
		// que sean tomados a las vbles pasadas (siempre que pFuncDecl != NULL)
		PushValuesOfFuncRefVars(pFuncDecl);	

		if (pStm->StmReturn.pExp) {
		  switch(pStm->StmReturn.pExp->pExpType->ValueType) {
			case TYPE_NUMBER: {
			  AttachCode(MakeNReturnOpcode(NULL));
			} break;

  			case TYPE_STRING: {
			  AttachCode(MakeSReturnOpcode(NULL));
			} break;

  			case TYPE_ENTITY: {
			  AttachCode(MakeEReturnOpcode(NULL));
			} break;

			default:
			  assert(0);
		  }; // ~ switch		  
		} else {
		  AttachCode(MakeReturnOpcode(NULL));
		}
	  } break;

	  case STM_IF: {
		// Recorrido / opcodes
		OpcodeGenExp(pStm->If.pExp, pSymTable);
		AttachCode(MakeJmpFalseOpcode(pStm->If.unEndLabel, NULL));
		OpcodeGenStm(pStm->If.pThenStm, pFuncDecl, pSymTable);		
		SetLabel("end", pStm->If.unEndLabel);
	  } break;

	  case STM_IFELSE: {
		// Recorrido / opcodes
		OpcodeGenExp(pStm->IfElse.pExp, pSymTable);
		AttachCode(MakeJmpFalseOpcode(pStm->IfElse.unElseLabel, NULL));
		OpcodeGenStm(pStm->IfElse.pThenStm, pFuncDecl, pSymTable);
		AttachCode(MakeJmpOpcode(pStm->IfElse.unEndLabel, NULL));
		SetLabel("else", pStm->IfElse.unElseLabel);
		OpcodeGenStm(pStm->IfElse.pElseStm, pFuncDecl, pSymTable);
		SetLabel("end", pStm->IfElse.unEndLabel);
	  } break;

	  case STM_WHILE: {
		// Recorrido / opcodes
		SetLabel("start", pStm->While.unStartLabel);
		OpcodeGenExp(pStm->While.pExp, pSymTable);		
		AttachCode(MakeJmpFalseOpcode(pStm->While.unEndLabel, NULL));
		OpcodeGenStm(pStm->While.pDoStm, pFuncDecl, pSymTable);
		AttachCode(MakeJmpOpcode(pStm->While.unStartLabel, NULL));
		SetLabel("end", pStm->While.unEndLabel);
	  } break;
	  
	  case STM_EXP: {
		// Recorrido
	    OpcodeGenExp(pStm->Exp.pExp, pSymTable);	
		// En caso de que haya una expresion, se debera de sacar el valor
		// que esta pueda dejar para seguir cumpliendo la norma de que la
		// altura de la pila en un STM debe de quedar invariante.
		// Expresiones que sean STM seran aquellas que dejen algun valor
		// en la pila
		if (pStm->Exp.pExp &&
		    TYPE_VOID != pStm->Exp.pExp->pExpType->ValueType) {		  
		  AttachCode(MakePopOpcode(NULL));
		}
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este metodo sera llamado siempre que se llegue a un return en una
//   funcion o bien cuando se llegue al final y no exista return con el 
//   objetivo de depositar el valor de las variables argumento que sean
//   por referencia. Luego, desde el exterior (justo despues de la llamada a
//   funcion) dichos valores seran recogidos.
// Parametros:
// - pFuncDecl. Declaracion de la funcion sobre la que trabajar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
PushValuesOfFuncRefVars(sFunc* pFuncDecl) 
{
  // ¿Se esta ante un retorno desde una funcion?
  if (pFuncDecl) {
    // Si, luego en primer lugar se debera de recorrer 
    // la lista de argumentos para comprobar si existe alguno por referencia
    // en cuyo caso, se dejara su valor en la pila antes de retornar. Luego
    // desde el exterior se recogera el valor por las vbles pasadas como
    // ref
    sArgument* pArgIt = pFuncDecl->FuncDecl.pArguments;
    assert(FUNC_DECL == pFuncDecl->eFuncType);
    for(; pArgIt; pArgIt = pArgIt->pSigArgument) {
  	  if (ARGUMENT_REF == pArgIt->ArgumentPassType) {
	    switch(pArgIt->pType->ValueType) {
	  	  case TYPE_NUMBER: {
		    AttachCode(MakeNLoadOpcode(pArgIt->unOffsetDecl, NULL));
		  } break;

  		  case TYPE_ENTITY: {
		    AttachCode(MakeELoadOpcode(pArgIt->unOffsetDecl, NULL));
		  } break;

  		  case TYPE_STRING: {
		    AttachCode(MakeSLoadOpcode(pArgIt->unOffsetDecl, NULL));
		  } break;
		 }; // ~ switch
	  }
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre una expresion.
// Parametros:
// - pExp. Expresion a imprimir
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeGenExp(sExp* pExp,
			 sSymbolTable* pSymTable)
{
  // ¿Hay alguna expresion para imprimir?
  if (pExp) {
    switch(pExp->ExpType) {
  	  case EXP_IDENTIFIER: {  		
		// Se genera el opcode asociado al indentificador
		MakeIdentifierOpcodeLoad(pExp->ExpIdentifier.szIdentifier, pSymTable);
	  } break;

	  case EXP_GLOBAL_CONST_ENTITY: {
		AttachCode(MakeEPushOpcode(pExp->ExpGlobalConstEntity.unValue, NULL));		
	  } break;

	  case EXP_GLOBAL_CONST_NUMBER: {
		AttachCode(MakeNPushOpcode(pExp->ExpGlobalConstNumber.fValue, NULL));
	  } break;

	  case EXP_NUMBER_VALUE: {
		// Opcodes
		AttachCode(MakeNPushOpcode(pExp->ExpNumValue.fNumValue, NULL));
	  } break;

	  case EXP_STRING_VALUE: {
		// Opcodes
		AttachCode(MakeSPushOpcode(pExp->ExpStrValue.szStrValue, NULL));
	  } break;

	  case EXP_ASSING: {
		// Vbles
		sType* pType = NULL;
		word unOffset = 0;
		sSymTableNode* pSymNode = SymTableGetNode(pSymTable, 
												  pExp->ExpAssing.szIdentifier);
		assert(pSymNode);		
		
		// Recorrido
	    OpcodeGenExp(pExp->ExpAssing.pRightExp, pSymTable);
		
		// Antes de generar el opcode de tipo STORE (para alojar en una variable
		// el valor generado por la expresion) este se dupliara con un opcode
		// de tipo DUP, de tal forma que lo que verdaderamente se almacene sea
		// la copia. Esto se hace asi para que se sigua cumpliendo el principio
		// que dice que la expresion debe de dejar la altura de la pila en un
		// tamaño superior a 1.
		AttachCode(MakeDupOpcode(NULL));

		switch(pSymNode->SymTableNodeType) {
		  case SYM_TYPE_VAR: {
			pType = pSymNode->pIdVar->VarTypeDecl.pType;
			unOffset = GetVarIdentifierOffset(pSymNode->pIdVar->VarTypeDecl.pVarIdentSeq,
			                                  pExp->ExpAssing.szIdentifier);
		  } break;

		  case SYM_TYPE_ARG: {
			pType = pSymNode->pIdArg->pType;
			unOffset = pSymNode->pIdArg->unOffsetDecl;
		  } break;

		  default:
			assert(0);
		}; // ~ switch

		// Ahora ya se genera el opcode store segun el tipo
		assert(pType);
		switch(pType->ValueType) {
		  case TYPE_NUMBER: {			
			AttachCode(MakeNStoreOpcode(unOffset, NULL));
		  } break;

		  case TYPE_STRING: {
			AttachCode(MakeSStoreOpcode(unOffset, NULL));
		  } break;

		  case TYPE_ENTITY: {
			AttachCode(MakeEStoreOpcode(unOffset, NULL));
		  } break;
		}; // ~ switch
	  } break;

	  case EXP_EQUAL: {
		// Recorrido
		OpcodeGenExp(pExp->ExpEqual.pLeftExp, pSymTable);
	    OpcodeGenExp(pExp->ExpEqual.pRightExp, pSymTable);
		// Opcode
		switch (pExp->ExpEqual.pLeftExp->pExpType->ValueType) {
		  case TYPE_NUMBER: {
			AttachCode(MakeNJmpEqOpcode(pExp->ExpEqual.unTrueLabel, NULL));
		  } break;

		  case TYPE_STRING: {
			AttachCode(MakeSJmpEqOpcode(pExp->ExpEqual.unTrueLabel, NULL));
		  } break;

		  case TYPE_ENTITY: {
			AttachCode(MakeEJmpEqOpcode(pExp->ExpEqual.unTrueLabel, NULL));
		  } break;

		  default:
			assert(0);
		}; // ~ switch
		AttachCode(MakeNPushOpcode(0, NULL));
		AttachCode(MakeJmpOpcode(pExp->ExpEqual.unEndLabel, NULL));
		SetLabel("true", pExp->ExpEqual.unTrueLabel);
		AttachCode(MakeNPushOpcode(1, NULL));
		SetLabel("end", pExp->ExpEqual.unEndLabel);
	  } break;

	  case EXP_NOEQUAL: {
		// Recorrido
	    OpcodeGenExp(pExp->ExpNoEqual.pLeftExp, pSymTable);
	    OpcodeGenExp(pExp->ExpNoEqual.pRightExp, pSymTable);
		// Opcode
		switch (pExp->ExpNoEqual.pLeftExp->pExpType->ValueType) {
		  case TYPE_NUMBER: {
			AttachCode(MakeNJmpNeOpcode(pExp->ExpNoEqual.unTrueLabel, NULL));
		  } break;

		  case TYPE_STRING: {
			AttachCode(MakeSJmpNeOpcode(pExp->ExpNoEqual.unTrueLabel, NULL));
		  } break;

		  case TYPE_ENTITY: {
			AttachCode(MakeEJmpNeOpcode(pExp->ExpNoEqual.unTrueLabel, NULL));
		  } break;

		  default:
			assert(0);
		}; // ~ switch
		AttachCode(MakeNPushOpcode(0, NULL));
		AttachCode(MakeJmpOpcode(pExp->ExpNoEqual.unEndLabel, NULL));
		SetLabel("true", pExp->ExpNoEqual.unTrueLabel);
		AttachCode(MakeNPushOpcode(1, NULL));
		SetLabel("end", pExp->ExpNoEqual.unEndLabel);		
	  } break;
		
	  case EXP_LESS: {
		// Recorrido
		OpcodeGenExp(pExp->ExpLess.pLeftExp, pSymTable);
	    OpcodeGenExp(pExp->ExpLess.pRightExp, pSymTable);
		// Opcode
		AttachCode(MakeNJmpLtOpcode(pExp->ExpLess.unTrueLabel, NULL));
		AttachCode(MakeNPushOpcode(0, NULL));
		AttachCode(MakeJmpOpcode(pExp->ExpLess.unEndLabel, NULL));
		SetLabel("true", pExp->ExpLess.unTrueLabel);		
		AttachCode(MakeNPushOpcode(1, NULL));
		SetLabel("end", pExp->ExpLess.unEndLabel);
	  } break;
		
	  case EXP_LESSEQUAL: {
		// Recorrido
		OpcodeGenExp(pExp->ExpLEqual.pLeftExp, pSymTable);
	    OpcodeGenExp(pExp->ExpLEqual.pRightExp, pSymTable);
		// Opcode
		AttachCode(MakeNJmpLeOpcode(pExp->ExpLEqual.unTrueLabel, NULL));
		AttachCode(MakeNPushOpcode(0, NULL));
		AttachCode(MakeJmpOpcode(pExp->ExpLEqual.unEndLabel, NULL));
		SetLabel("true", pExp->ExpLEqual.unTrueLabel);
		AttachCode(MakeNPushOpcode(1, NULL));
		SetLabel("end", pExp->ExpLEqual.unEndLabel);
	  } break;
		
	  case EXP_GREATER: {
		// Recorrido
		OpcodeGenExp(pExp->ExpGreater.pLeftExp, pSymTable);
	    OpcodeGenExp(pExp->ExpGreater.pRightExp, pSymTable);
		// Opcode
		AttachCode(MakeNJmpGtOpcode(pExp->ExpGreater.unTrueLabel, NULL));
		AttachCode(MakeNPushOpcode(0, NULL));
		AttachCode(MakeJmpOpcode(pExp->ExpGreater.unEndLabel, NULL));
		SetLabel("true", pExp->ExpGreater.unTrueLabel);
		AttachCode(MakeNPushOpcode(1, NULL));
		SetLabel("true", pExp->ExpGreater.unEndLabel);
	  } break;
	  		
	  case EXP_GREATEREQUAL: {
		// Recorrido
		OpcodeGenExp(pExp->ExpGEqual.pLeftExp, pSymTable);
	    OpcodeGenExp(pExp->ExpGEqual.pRightExp, pSymTable);
		// Opcode
		AttachCode(MakeNJmpGeOpcode(pExp->ExpGEqual.unTrueLabel, NULL));
		AttachCode(MakeNPushOpcode(0, NULL));
		AttachCode(MakeJmpOpcode(pExp->ExpGEqual.unEndLabel, NULL));
		SetLabel("true", pExp->ExpGEqual.unTrueLabel);
		AttachCode(MakeNPushOpcode(1, NULL));
		SetLabel("end", pExp->ExpGEqual.unEndLabel);
	  } break;
		
	  case EXP_ADD: {
		// Recorrido
	    OpcodeGenExp(pExp->ExpAdd.pLeftExp, pSymTable);
	    OpcodeGenExp(pExp->ExpAdd.pRightExp, pSymTable);
		// Opcode
		switch(pExp->pExpType->ValueType) {
		  case TYPE_NUMBER: {
			AttachCode(MakeNAddOpcode(NULL));
		  } break;

		  case TYPE_STRING: {
			AttachCode(MakeSAddOpcode(NULL));
		  } break;

		  default:
			assert(0);
		}; // ~ switch
	  } break;
		
	  case EXP_MINUS: {		
		// Recorrido
	    OpcodeGenExp(pExp->ExpMinus.pLeftExp, pSymTable);
	    OpcodeGenExp(pExp->ExpMinus.pRightExp, pSymTable);
		// Opcode
		AttachCode(MakeNSubOpcode(NULL));
	  } break;
		
	  case EXP_MULT: {
		// Recorrido
	    OpcodeGenExp(pExp->ExpMult.pLeftExp, pSymTable);
	    OpcodeGenExp(pExp->ExpMult.pRightExp, pSymTable);
		// Opcode
		AttachCode(MakeNMulOpcode(NULL));
	  } break;
		
	  case EXP_DIV: {
		// Recorrido
		OpcodeGenExp(pExp->ExpDiv.pLeftExp, pSymTable);
	    OpcodeGenExp(pExp->ExpDiv.pRightExp, pSymTable);
		// Opcode
		AttachCode(MakeNDivOpcode(NULL));
	  } break;
		
	  case EXP_MODULO: {
		// Recorrido / opcodes
	    OpcodeGenExp(pExp->ExpModulo.pLeftExp, pSymTable);
	    OpcodeGenExp(pExp->ExpModulo.pRightExp, pSymTable);
		AttachCode(MakeNModOpcode(NULL));
	  } break;
		
	  case EXP_UMINUS: {
		// Recorrido
	    OpcodeGenExp(pExp->ExpUMinus.pExp, pSymTable);
		// Opcode
		AttachCode(MakeNNegOpcode(NULL));
	  } break;
		
	  case EXP_NOT: {
		// Recorrido / Opcodes
		// Basicamente, se comprobara si la expresion es false, si es 
		// false se saltara a una etiqueta en donde se pondra un valor true
		// y viceversa.
	    OpcodeGenExp(pExp->ExpNot.pExp, pSymTable);
		AttachCode(MakeJmpFalseOpcode(pExp->ExpNot.unFalseLabel, NULL));
		AttachCode(MakeNPushOpcode(0, NULL));
		AttachCode(MakeJmpOpcode(pExp->ExpNot.unEndLabel, NULL));
		SetLabel("false", pExp->ExpNot.unFalseLabel);
		AttachCode(MakeNPushOpcode(1, NULL));
		SetLabel("end", pExp->ExpNot.unEndLabel);
	  } break;
		
	  case EXP_AND: {
		// Recorrido / opcodes
		OpcodeGenExp(pExp->ExpAnd.pLeftExp, pSymTable);
		// Se generara el Dup
		AttachCode(MakeDupOpcode(NULL));
		AttachCode(MakeJmpFalseOpcode(pExp->ExpAnd.unFalseLabel, NULL));
		AttachCode(MakePopOpcode(NULL));
		OpcodeGenExp(pExp->ExpAnd.pRightExp, pSymTable);
		SetLabel("false", pExp->ExpAnd.unFalseLabel);		
	  } break;
		
	  case EXP_OR: {
		// Recorrido / opcodes
	    OpcodeGenExp(pExp->ExpOr.pLeftExp, pSymTable);
		// Se generara el Dup 
		AttachCode(MakeDupOpcode(NULL));
		AttachCode(MakeJmpTrueOpcode(pExp->ExpOr.unTrueLabel, NULL));
		AttachCode(MakePopOpcode(NULL));
		OpcodeGenExp(pExp->ExpOr.pRightExp, pSymTable);
		SetLabel("true", pExp->ExpOr.unTrueLabel);
	  } break;
		
	  case EXP_FUNC_INVOKE: {	
		// Vbles
		sSymTableNode* pFuncDecl = SymTableGetNode(pSymTable, 
												   pExp->ExpFuncInvoke.szIdentifier);
		sArgument* pArgIt = NULL;
		sExp*      pParamIt = NULL;
		assert(pFuncDecl);

		// Recorrido y Opcode basico
		OpcodeGenParams(pExp->ExpFuncInvoke.pParams, pSymTable);		
		AttachCode(MakeCallFuncOpcode(pExp->ExpFuncInvoke.szIdentifier, NULL));

		// En el caso de que la funcion tenga algun argumento por referencia deberemos
		// de colocar a continuacion instruciones store a las variables que pasamos
		// pues la funcion antes de volver con return, habra colocado los nuevos
		// valores para las mismas en la pila.
		pArgIt = pFuncDecl->pIdFunc->FuncDecl.pArguments;
		pParamIt = pExp->ExpFuncInvoke.pParams;
		while (pArgIt && 
			   pParamIt) {		  
		  // ¿Argumento por referencia?
		  if (ARGUMENT_REF == pArgIt->ArgumentPassType) {
			// Vbles
			sSymTableNode* pParamDecl = NULL;
			word unParamOffset = 0;	
			
			// Se toma el nodo en la tabla de simbolos
			assert(EXP_IDENTIFIER == pParamIt->ExpType);		  
			 pParamDecl = SymTableGetNode(pSymTable, 
								          pParamIt->ExpIdentifier.szIdentifier);			

			// Se halla el offset del identificador que se pasara como parametro
			// Nota: es seguro que el identificador del parametro sera el de
			// una variable (bien declarada, bien argumento)
			assert(pParamDecl);
			switch (pParamDecl->SymTableNodeType) {
			  case SYM_TYPE_VAR: {				
				unParamOffset = GetVarIdentifierOffset(pParamDecl->pIdVar->VarTypeDecl.pVarIdentSeq,
														pParamIt->ExpIdentifier.szIdentifier);
			  } break;

			  case SYM_TYPE_ARG: {
				unParamOffset = pParamDecl->pIdArg->unOffsetDecl;
			  } break;

			  default:
				assert(0);
			}; // ~ switch

			// Si, luego se inserta el opcode pertinente
			switch(pArgIt->pType->ValueType) {
			  case TYPE_NUMBER: {				
				AttachCode(MakeNStoreOpcode(unParamOffset, NULL));
			  } break;

			  case TYPE_STRING: {
				AttachCode(MakeSStoreOpcode(unParamOffset, NULL));
			  } break;

			  case TYPE_ENTITY: {
				AttachCode(MakeEStoreOpcode(unParamOffset, NULL));
			  } break;

			  default:
				assert(0);
			}; // ~ switch
		  }
		  // Sig
		  pArgIt = pArgIt->pSigArgument;
		  pParamIt = pParamIt->pSigExp;
		}
		assert(!pArgIt && !pParamIt);
		
	  } break;
		
	  case EXP_APIFUNC_INVOKE: {
		// Recorrido / opcodes
		OpcodeGenParams(pExp->ExpAPIFuncInvoke.pParams, pSymTable);
		AttachCode(MakeAPICallOpcode(pExp->ExpAPIFuncInvoke.ExpAPIFunc, NULL));		
	  } break;

	  case EXP_GAMEOBJ_INVOKE: {
		// Recorrido / opcodes
		OpcodeGenParams(pExp->ExpGameObjInvoke.pParams, pSymTable);
		AttachCode(MakeGameObjMethodCallOpcode(pExp->ExpGameObjInvoke.ExpGameObjMethod, NULL));		
	  } break;

	  case EXP_WORLDOBJ_INVOKE: {
		// Recorrido / opcodes
		OpcodeGenParams(pExp->ExpWorldObjInvoke.pParams, pSymTable);
		AttachCode(MakeWorldObjMethodCallOpcode(pExp->ExpWorldObjInvoke.ExpWorldObjMethod, NULL));		
	  } break;
		
	  case EXP_ENTITYOBJ_INVOKE: {
		// Recorrido / opcodes
		// Nota: en el caso de la llamada a entidades, se considerara que 
		// el propio identificador de la entidad sera un parametro que se pasara
		// en primer lugar.
		MakeIdentifierOpcodeLoad(pExp->ExpEntityObjInvoke.szIdentifier, pSymTable);
		OpcodeGenParams(pExp->ExpEntityObjInvoke.pParams, pSymTable);
		AttachCode(MakeEntityObjMethodCallOpcode(pExp->ExpEntityObjInvoke.ExpEntityObjMethod, NULL));		
	  } break;

  	  case EXP_CAST: {				
		// Recorrido
	    OpcodeGenExp(pExp->ExpCast.pExp, pSymTable);
		// Opcodes
		// Se insertara el opcode de cast solo si los tipos distintos
		if (pExp->ExpCast.pExp->pExpType->ValueType != pExp->ExpCast.pType->ValueType) {
		  AttachCode(MakeCastOpcode(pExp->ExpCast.pExp->pExpType, 
								    pExp->ExpCast.pType, 
									NULL));
		}
	  } break;
	}; // ~ switch  
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorrido de los parametros de una funcion
// Parametros:
// - pParam. Puntero al parametro
// Devuelve:
// Notas:
// - Funcion privada
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeGenParams(sExp* pParam,
				sSymbolTable* pSymTable)
{
  // ¿Es parametro valido?
  if (pParam) {
	sExp* pIt = pParam;
	for (; pIt != NULL; pIt = pIt->pSigExp) {
	  OpcodeGenExp(pIt, pSymTable);
	}  
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre un tipo
// Parametros:
// - pType. Tipo a imprimir
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeGenType(sType* pType)
{
  // ¿Hay tipo?
  if (pType) {
	switch(pType->ValueType) {
	  case TYPE_VOID: {
	  } break;

	  case TYPE_NUMBER: {		
	  } break;

	  case TYPE_STRING: {		
	  } break;

	  case TYPE_ENTITY: {		
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre una funcion.
// Parametros:
// - pFunc. Funcion a imprimir.
// - pSymTable. Tabla de simbolos.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeGenFunc(sFunc* pFunc)
{
  if (pFunc) {
	switch (pFunc->eFuncType) {
	  case FUNC_SEQ: {
		// Recorrido
		OpcodeGenFunc(pFunc->FuncSeq.pFirst);
		OpcodeGenFunc(pFunc->FuncSeq.pSecond);
	  } break;

	  case FUNC_DECL: {				  		
		// ¿Fue invocada?
		if (pFunc->FuncDecl.unWasInvoked) {		
		  // Se guardan los punteros a la lista anterior de opcodes y se
		  // inicializan los mismos
		  sOpcode* pPrevOpRootList = pOpRootList;
		  sOpcode* pPrevOpTailList = pOpTailList;
		  dword udPrevOpcodePos = udOpcodePos;
		  pOpRootList = NULL;
		  pOpTailList = NULL;
		  
		  // Recorrido
		  udOpcodePos = 0;			
		  OpcodeGenType(pFunc->FuncDecl.pType);
		  OpcodeGenArgument(pFunc->FuncDecl.pArguments, pFunc->FuncDecl.pSymTable);
  		  OpcodeGenConst(pFunc->FuncDecl.pConst, pFunc->FuncDecl.pSymTable);		
		  OpcodeGenVar(pFunc->FuncDecl.pVar, pFunc->FuncDecl.pSymTable);			

		  // Al recorrer las sentencias se preparara la lista de labels
		  pLabelList = (sLabel*) Mem_Alloc(pFunc->FuncDecl.unNumLabels * sizeof(sLabel));
		  assert(pLabelList);
		  OpcodeGenStm(pFunc->FuncDecl.pStm, pFunc, pFunc->FuncDecl.pSymTable);		

		  // Se asocia a la funcion y se desvincula lista de labels general
		  pFunc->FuncDecl.pLabelList = pLabelList;
		  pLabelList = NULL;

		  // ¿El tipo es void?
		  if (TYPE_VOID == pFunc->FuncDecl.pType->ValueType) {
			// Si, luego se genera un return vacio pero antes se depositaran
			// los posibles valores asociados a las vbles por referencia			
			PushValuesOfFuncRefVars(pFunc);		
			AttachCode(MakeReturnOpcode(NULL));			
		  } else {
			// Se asocia opcode nulo
			AttachCode(MakeNopOpcode(NULL));
		  }
		  
		  // Se guarda la lista actual y se reponen punteros a la lista previa
		  pFunc->FuncDecl.pOpcodeList = pOpRootList;
		  pOpRootList = pPrevOpRootList;
		  pOpTailList = pPrevOpTailList;
		  udOpcodePos = udPrevOpcodePos;

		  // Se calcula la firma de la funcion
		  pFunc->FuncDecl.szSignature = CalculeFuncSignature(pFunc->FuncDecl.pArguments,
															 pFunc->FuncDecl.pType);
		  assert(pFunc->FuncDecl.szSignature);
		}
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre argumentos
// - Los argumentos seran considerados como variables locales a una funcion
//   o script y en consecuencia deberan de ser declarados como tales.
// Parametros:
// - pArgument. Argumentos a imprimir
// - pSymTable. Tabla de simbolos.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeGenArgument(sArgument* pArgument,
				  sSymbolTable* pSymTable)
{
  // ¿Hay argumentos para imprimir?
  if (pArgument) {
	sArgument* pIt = pArgument;
	for (; pIt != NULL; pIt = pIt->pSigArgument) {
	  // Recorrido
	  OpcodeGenType(pIt->pType);
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Calcula la altura maxima de la pila comienzando por el enlace global
// Parametros:
// - pGlobal. Enlace al nodo global del AST
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CalculeMaxStackGlobal(sGlobal* pGlobal)
{
  if (pGlobal) {
	// Recorrido
	CalculeMaxStackScript(pGlobal->pScript);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorrido por los scripts para el calculo de la altura maxima de la
//   pila.
// Parametros:
// - pScript. Enlace al script
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CalculeMaxStackScript(sScript* pScript)
{
    if (pScript) {
	switch(pScript->ScriptType) {
	  case SCRIPT_SEQ: {
		// Recorrido
		CalculeMaxStackScript(pScript->ScriptSeq.pFirst);
		CalculeMaxStackScript(pScript->ScriptSeq.pSecond);
	  } break;

	  case SCRIPT_DECL: {		
		// Recorrido
		CalculeMaxStackImport(pScript->ScriptDecl.pImport);		
		CalculeMaxStackFunc(pScript->ScriptDecl.pFunc);
		
		// Se halla la altura maxima de la pila durante la ejecucion del script
		FindStackSize(pScript->ScriptDecl.pOpcodeList,
		              pScript->ScriptDecl.pLabelList,
					  0,
					  &pScript->ScriptDecl.unStackSize,
					  pScript->ScriptDecl.pSymTable);
	  } break;	
	}; // ~ switch	
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorrido por los enlaces import para hallar la maxima altura en cada
//   una de las funciones.
// Parametros:
// - pImport. Enlace al nodo import.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CalculeMaxStackImport(sImport* pImport)
{
  if (pImport) {
	switch(pImport->ImportType) {
	  case IMPORT_SEQ: {
		CalculeMaxStackImport(pImport->ImportSeq.pFirst);
		CalculeMaxStackImport(pImport->ImportSeq.pSecond);
	  } break;

	  case IMPORT_FUNC: {
		CalculeMaxStackFunc(pImport->ImportFunc.pFunctions);
  	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Calcula la altura maxima de la pila que se alcanza en las funciones
// Parametros:
// - pFunc. Enlace a funcion.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CalculeMaxStackFunc(sFunc* pFunc)
{
  if (pFunc) {
	switch (pFunc->eFuncType) {
	  case FUNC_SEQ: {
		// Recorrido
		CalculeMaxStackFunc(pFunc->FuncSeq.pFirst);
		CalculeMaxStackFunc(pFunc->FuncSeq.pSecond);
	  } break;

	  case FUNC_DECL: {				  						
		// Se halla la altura maxima de la pila durante la ejecucion de la funcion
		// ¿Fue invocada?
		if (pFunc->FuncDecl.unWasInvoked) {		
		  FindStackSize(pFunc->FuncDecl.pOpcodeList,
						pFunc->FuncDecl.pLabelList,
						0,
						&pFunc->FuncDecl.unStackSize,
						pFunc->FuncDecl.pSymTable);
		}
	  } break;
	}; // ~ switch
  }
}

