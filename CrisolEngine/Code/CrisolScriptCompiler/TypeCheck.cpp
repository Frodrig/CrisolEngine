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
// TypeCheck.cpp
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Notas:
// - Consultar TypeCheck.h para mas informacion
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "TypeCheck.h"
#include <stdio.h>
#include <assert.h>
#include "Memory.h"
#include "Error.h"

// Vbles globales
static sType* pUndefAuxType = NULL;
static sType* pVoidAuxType = NULL;
static sType* pEntityAuxType = NULL;
static sType* pNumberAuxType = NULL;
static sType* pStringAuxType = NULL;

// Funciones privadas / Utilidades
static void InitAuxTypes(void);
static sType* IsLeftGEqualType(sType* pLeft, sType* pRight);
static sType* GetTheGEqualType(sType* pLeft, sType* pRight);
static sword IsRightAssignableToLeft(sType* pLeft, sType* pRight);
static sword IsValidCast(sType* pSource, sType* pDest);
static sword IsVoidType(sType* pType);
static sword IsEntityType(sType* pType);
static sword IsNumberType(sType* pType);
static sword IsStringType(sType* pType);
static sbyte* PassTypeToString(sType* pType);
static sExp* ReturnExpCastNode(sType* pType,
							   sExp* pExp);
static void CheckArguments(sType** pArgTypes,
						   sExp** pParams,
						   word unSrcLine,
						   sbyte* szActFileName);

// Funciones privadas / Recorrido del AST
static void TypeCheckConst(sConst* pConst, 
						   sSymbolTable* pSymTable,
						   sbyte* szActFileName);
static void TypeCheckVar(sVar* pVar, 
						 sType* pVarType,
						 sSymbolTable* pSymTable,
						 sbyte* szActFileName);
static void TypeCheckScript(sScript* pScript, 
							sbyte* szActFileName);
static void TypeCheckScriptType(sScriptType* pScriptType, 
								sSymbolTable* pSymTable,
								sbyte* szActFileName);
static void TypeCheckImport(sImport* pImport, 
							sbyte* szActFileName);
static void TypeCheckStm(sStm* pStm, 
						 sType* pReturnStmType,
						 sSymbolTable* pSymTable,
						 sbyte* szActFileName);
static void TypeCheckExp(sExp* pExp, 
						 sSymbolTable* pSymTable,
						 sbyte* szActFileName);
static void TypeCheckType(sType* pType, 
						  sSymbolTable* pSymTable,
						  sbyte* szActFileName);
static void TypeCheckFunc(sFunc* pFunc, 
						  sbyte* szActFileName);
static void TypeCheckArgument(sArgument* pArgument,
							  sSymbolTable* pSymTable,
							  sbyte* szActFileName);
static void TypeCheckParams(sExp* pParam,
							sSymbolTable* pSymTable,
							sbyte* szActFileName);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa los tipos auxiliares. Los tipos auxiliares se utilizaran para
//   realizar las comparaciones de tipos a un nivel basico.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
InitAuxTypes(void)
{
  // Inicializa los tipos auxiliares
  // Sin determinar
  assert(!pUndefAuxType);
  pUndefAuxType = NULL;

  // Void
  assert(!pVoidAuxType);
  pVoidAuxType = MakeTypeVoid();
  
  // Entity
  assert(!pEntityAuxType);
  pEntityAuxType = MakeTypeEntity();

  // Number
  assert(!pNumberAuxType);
  pNumberAuxType = MakeTypeNumber();

  // String
  assert(!pStringAuxType);
  pStringAuxType = MakeTypeString();  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera los tipos auxiliares que se hallan podido crear en memoria.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
EndAuxTypes(void)
{
  // Libera los tipos
  // Void
  if (pVoidAuxType) {
	Mem_Free(pVoidAuxType);
	pVoidAuxType = NULL;
  }
  
  // Entity
  if (pEntityAuxType) {
	Mem_Free(pEntityAuxType);
	pEntityAuxType = NULL;
  }

  // Number
  if (pNumberAuxType) {
	Mem_Free(pNumberAuxType);
	pNumberAuxType = NULL;
  }

  // String
  if (pStringAuxType) {
	Mem_Free(pStringAuxType);
	pStringAuxType = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el tipo a la izquierda es mayor o igual que el de la dcha
//   devolviendo el valor del tipo mayor. 
// Parametros:
// - pLeft. Tipo a la izquierda.
// - pRight. Tipo a la derecha.
// Devuelve:
// - El valor del tipo mayor. En caso de que no haya tipo mayor o igual se
//   devolvera UndefAuxType.
// Notas:
// - En el caso de CrisolScript, el tipo de mayor peso sera siempre el
//   string y en caso de que alguno de los dos tipos recibidos no este 
//   definido, se devolvera el tipo no definido.
///////////////////////////////////////////////////////////////////////////////
sType* 
IsLeftGEqualType(sType* pLeft, 
				 sType* pRight)
{
  // ¿Valores indefinidos?
  if (pLeft == pUndefAuxType ||
	  pRight == pUndefAuxType) {
	return pUndefAuxType;
  }

  // ¿Izquierda valor entity?
  if (pLeft->ValueType == pEntityAuxType->ValueType &&
	  pRight->ValueType == pEntityAuxType->ValueType) {
	return pEntityAuxType;
  }

  // ¿Izq. valor numerico?
  if (pLeft->ValueType == pNumberAuxType->ValueType &&
	  pRight->ValueType == pNumberAuxType->ValueType) {
	return pNumberAuxType;
  }

  // ¿Izq. string?
  if (pLeft->ValueType == pStringAuxType->ValueType &&
	  (pRight->ValueType == pStringAuxType->ValueType ||
	   pRight->ValueType == pNumberAuxType->ValueType)) {
	return pStringAuxType;
  }

  // El valor a la izq. no es mayor o igual en importancia
  return pUndefAuxType;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba cual de los dos tipos es de mayor o igual importancia, 
//   devolviendo enlace al tipo axiliar que proceda.
// Parametros:
// - pLeft. Tipo a la izquierda.
// - pRight. Tipo a la derecha.
// Devuelve:
// - El tipo de mayor o igual importancia. O bien UndefAuxType si no hay 
//   ningun tipo en tales condiciones.
// Notas:
///////////////////////////////////////////////////////////////////////////////
sType* 
GetTheGEqualType(sType* pLeft, 
				 sType* pRight)
{
  // Calcula el tipo de mayor importancia y lo retorna
  sType* pReturnType = IsLeftGEqualType(pLeft, pRight);
  if (pReturnType == pUndefAuxType) {
	pReturnType = IsLeftGEqualType(pRight, pLeft);
  }
  return pReturnType;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el tipo pRight es un tipo asignable al tipo pLeft. 
// Parametros:
// - pLeft. Tipo a la izquierda.
// - pRight. Tipo a la derecha.
// Devuelve:
// - Si pRight se puede asignar a pLeft 1. En caso contrario 0.
// Notas:
// - En caso de trabajar con valores indefinidos, se considerara que si se
//   podra realizar la asignacion.
///////////////////////////////////////////////////////////////////////////////
sword 
IsRightAssignableToLeft(sType* pLeft, 
						sType* pRight)
{
  // ¿Valores indefinidos?
  if (pLeft == pUndefAuxType ||
	  pRight == pUndefAuxType) {
	return 1;
  }

  // El tipo de pLeft ha de ser mayor
  return (GetTheGEqualType(pLeft, pRight) != pUndefAuxType) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Defina las reglas cast. Podremos:
//   * Pasar un entity a entity
//   * Pasar un string a string / number
//   * Paser un number a number / string
//   * Pasar un void a nada.
//   * Resto (pUndefAuxType) a nada
// Parametros:
// - pSource. Es el tipo origen que queremos covertir
// - pDest. Es el tipo destino al que queremos convertir pSource
// Devuelve:
// - Si es un cast valido 1, en caso contrario 0.
// Notas:
// - El cast posible entre strings y valores numericos se utilizara en
//   las operaciones de asignacion y de suma.
///////////////////////////////////////////////////////////////////////////////
sword 
IsValidCast(sType* pSource, 
			sType* pDest)
{
  assert(pSource);
  assert(pDest);
  
  // Caso de ser una entidad
  if (pSource->ValueType == pEntityAuxType->ValueType) {
	return (pDest->ValueType == pEntityAuxType->ValueType) ? 1 : 0;
  }

  // Caso de ser un numero
  if (pSource->ValueType == pNumberAuxType->ValueType) {
	return (pDest->ValueType == pNumberAuxType->ValueType ||			
			pDest->ValueType == pStringAuxType->ValueType) ? 1 : 0;
  }

  // Caso de ser una cadena
  if (pSource->ValueType == pStringAuxType->ValueType) {
	return (pDest->ValueType == pStringAuxType->ValueType ||
			pDest->ValueType == pNumberAuxType->ValueType) ? 1 : 0;
  }
  
  // No se puede
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el tipo pType es void.
// - Se considerara aceptable el tipo UndefAuxType.
// Parametros:
// - pType. Tipo a comprobar.
// Devuelve:
// - 1 si es tipo aceptable y 0 en caso contrario.
// Notas:
///////////////////////////////////////////////////////////////////////////////
sword 
IsVoidType(sType* pType)
{
  // Retorna resultado
  return (pType == pUndefAuxType ||  
		  pType->ValueType == pVoidAuxType->ValueType) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el tipo pType es entity.
// - Se considerara aceptable el tipo UndefAuxType.
// Parametros:
// - pType. Tipo a comprobar.
// Devuelve:
// - 1 si es tipo aceptable y 0 en caso contrario.
// Notas:
///////////////////////////////////////////////////////////////////////////////
sword 
IsEntityType(sType* pType)
{
  // Retorna resultado
  return (pType == pUndefAuxType ||  
		  pType->ValueType == pEntityAuxType->ValueType) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el tipo pType es number.
// - Se considerara aceptable el tipo UndefAuxType.
// Parametros:
// - pType. Tipo a comprobar.
// Devuelve:
// - 1 si es tipo aceptable y 0 en caso contrario.
// Notas:
///////////////////////////////////////////////////////////////////////////////
sword 
IsNumberType(sType* pType)
{
  // Retorna resultado
  return (pType == pUndefAuxType ||  
		  pType->ValueType == pNumberAuxType->ValueType) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el tipo pType es string.
// - Se considerara aceptable el tipo UndefAuxType.
// Parametros:
// - pType. Tipo a comprobar.
// Devuelve:
// - 1 si es tipo aceptable y 0 en caso contrario.
// Notas:
///////////////////////////////////////////////////////////////////////////////
sword 
IsStringType(sType* pType)
{
  // Retorna resultado
  return (pType == pUndefAuxType ||  
		  pType->ValueType == pStringAuxType->ValueType) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Pasa el tipo recibido a una cadena de caracteres para que sea 
//   posible escribirlo en pantalla.
// Parametros:
// - pType. Tipo a pasar a cadena.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
sbyte* 
PassTypeToString(sType* pType)
{
  // Segun el tipo, se devolvera la cadena
  assert(pType);
  switch(pType->ValueType) {
	case TYPE_VOID: {
	  return "void";
	} break;

  	case TYPE_ENTITY: {
	  return "entity";
	} break;

  	case TYPE_NUMBER: {
	  return "number";
	} break;

  	case TYPE_STRING: {
	  return "string";
	} break;
  }; // ~ switch

  // Tipo no definido  
  return "(¿?>sin tipo)";
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sExp de tipo EXP_CAST que tenga un tipo igual al recibido
//   en pType y con la expresion pExp.
// Parametros:
// - pType. Tipo a establecer para realizar el cast.
// - pExp. Expresion a asociar al nodo.
// Devuelve:
// - Direccion del nodo sExp creado.
// Notas:
// - Esta funcion unicamente hara la tarea de filtrado con respecto a la
//   verdadera funcion que se hallara en ASTree. Para ello, segun sea
//   el tipo de pType, llamara a su vez a la funcion MakeType que convenga.
///////////////////////////////////////////////////////////////////////////////
sExp* 
ReturnExpCastNode(sType* pType,
				  sExp* pExp)
{
  // Crea el nodo sExp de tipo EXP_CAST segun sea pType
  sExp* pNode = NULL;
  assert(pType);
  assert(pExp);
  switch(pType->ValueType) {
	case TYPE_VOID: {	  
	  pNode = MakeExpCast(pVoidAuxType, pExp);
	} break;

  	case TYPE_ENTITY: {
	  pNode = MakeExpCast(pEntityAuxType, pExp);
	} break;

  	case TYPE_NUMBER: {
	  pNode = MakeExpCast(pNumberAuxType, pExp);
	} break;

  	case TYPE_STRING: {
	  pNode = MakeExpCast(pStringAuxType, pExp);
	} break;

	default:
	  assert(0);
  }; // ~ switch

  // Se asocia el tipo a la expresion creada (igual al tipo del cast) y retorna
  pNode->pExpType = pType;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe un array con las direcciones a los tipos de los argumentos de
//   una funcion de API o metodo de un objeto para comparlos con los tipos de 
//   los parametros pasados a la misma y asi descubrir posibles errores.
// Parametros:
// - pArgTypes. Direccion al array con los tipos de los argumentos a chequear.
//   En el momento en que se halle NULL estaremos al final del array.
// - pParams. Direccion al puntero donde se hallan los parametros
// - unSrcLine. Linea a utilizar para indicar el error
// Devuelve:
// Notas:
// - El metodo sacara los errores automaticamente por la pantalla.
///////////////////////////////////////////////////////////////////////////////
void
CheckArguments(sType** pArgTypes,
			   sExp** pParams,
			   word unSrcLine,
			   sbyte* szActFileName)
{

  // Recorre los tipos de argumento y los parametros
  sExp* pPrevParamIt = NULL;
  sExp* pParamIt = *pParams;
  sword nIt = 0;
  while(pArgTypes[nIt] &&
		pParamIt) {
	// ¿NO se puede asignar parametro a argumento?
	if (!IsRightAssignableToLeft(pArgTypes[nIt], pParamIt->pExpType)) {
	  ReportErrorf(szActFileName,
				   "(...)",
				   unSrcLine,
				   "Error> No se puede asignar %s a %s",
				   PassTypeToString(pParamIt->pExpType),
				   PassTypeToString(pArgTypes[nIt]));	
	} else if (pParamIt->pExpType != pUndefAuxType &&
			   pArgTypes[nIt]->ValueType != pParamIt->pExpType->ValueType) {
	  // ¿Cast en el primer parametro?
	  if (NULL == pPrevParamIt) {
	    *pParams = ReturnExpCastNode(pArgTypes[nIt], pParamIt);
	    pParamIt = *pParams;
	  } else {
	    pPrevParamIt->pSigExp = ReturnExpCastNode(pArgTypes[nIt], pParamIt);
	    pParamIt = pPrevParamIt->pSigExp;
	  }
	}

	// Sig
	++nIt;
	pPrevParamIt = (NULL == pPrevParamIt) ? *pParams : pPrevParamIt->pSigExp;
	pParamIt = pParamIt->pSigExp;
  }

  // ¿Faltaron parametros?
  if (pArgTypes[nIt] && !pParamIt) {
	ReportErrorf(szActFileName,
				 "(...)",
				 unSrcLine,
				 "Error> Faltan parametros");	
  } else if(!pArgTypes[nIt] && pParamIt) {
	// Sobraron parametros
	ReportErrorf(szActFileName,
				 "(...)",
				 unSrcLine,
				 "Error> Sobran parametros");
  }
  /**/
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicia el recorrido por el arbol AST.
// Parametros:
// - pGlobal. Direccion a la estructura global.
// - pSymTable. Tabla de simbolos.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
TypeCheckGlobal(sGlobal* pGlobal)
{
  // ¿Hay datos?
  if (pGlobal) {
	sbyte* szActFileName = pGlobal->szFileName;
	InitAuxTypes();
	TypeCheckConst(pGlobal->pConst, pGlobal->pSymTable, szActFileName);
	TypeCheckVar(pGlobal->pVar, NULL, pGlobal->pSymTable, szActFileName);
	TypeCheckStm(pGlobal->pReturnStm, NULL, pGlobal->pSymTable, szActFileName);
	TypeCheckScript(pGlobal->pScript, szActFileName);
	//EndAuxTypes();
	szActFileName = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre declaraciones const.
// Parametros:
// - pConst. Declaraciones const
// - pSymTable. Tabla de simbolos.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
TypeCheckConst(sConst* pConst,
			   sSymbolTable* pSymTable,
			   sbyte* szActFileName)
{
  // ¿Hay declaraciones?
  if (pConst) {
	switch(pConst->ConstType) {
	  case CONST_DECLSEQ: {
		TypeCheckConst(pConst->ConstSeq.pFirst, pSymTable, szActFileName);
		TypeCheckConst(pConst->ConstSeq.pSecond, pSymTable, szActFileName);
	  } break;

	  case CONST_DECL: {
		TypeCheckType(pConst->ConstDecl.pType, pSymTable, szActFileName);		
		TypeCheckExp(pConst->ConstDecl.pExp, pSymTable, szActFileName);		
		
		// ¿NO es asignable la expresion al tipo de la constante?
		if (!IsRightAssignableToLeft(pConst->ConstDecl.pType,
									 pConst->ConstDecl.pExp->pExpType)) {
		  ReportErrorf(szActFileName,
					   pConst->ConstDecl.szIdentifier,
					   pConst->unSrcLine,
					   "Error> No se puede asignar %s a %s",
					   PassTypeToString(pConst->ConstDecl.pExp->pExpType),
   					   PassTypeToString(pConst->ConstDecl.pType));
		} else if (pConst->ConstDecl.pType != pUndefAuxType &&
				   pConst->ConstDecl.pExp->pExpType != pUndefAuxType &&
				   pConst->ConstDecl.pType->ValueType != pConst->ConstDecl.pExp->pExpType->ValueType) {		  
			pConst->ConstDecl.pExp = ReturnExpCastNode(pConst->ConstDecl.pType,
													   pConst->ConstDecl.pExp);			
		}
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre delcaracion de variables.
// Parametros:
// - pVar. Declaracion de variables
// - pVarType. Tipo de la vble. En caso de que se venga de una llamada 
//   distinta a VAR_TYPEDECL sera NULL.
// - pSymTable. Tabla de simbolos.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
TypeCheckVar(sVar* pVar,
			 sType* pVarType,
			 sSymbolTable* pSymTable,
			 sbyte* szActFileName)
{
  // ¿Hay variables?
  if (pVar) {
	switch(pVar->VarType) {
	  case VAR_TYPEDECL_SEQ: {
		TypeCheckVar(pVar->VarTypeDeclSeq.pFirst, 
					 pVarType, 
					 pSymTable,
					 szActFileName);
		TypeCheckVar(pVar->VarTypeDeclSeq.pSecond, 
					 pVarType, 
					 pSymTable,
					 szActFileName);
	  } break;

	  case VAR_TYPEDECL: {
		TypeCheckType(pVar->VarTypeDecl.pType, 
					  pSymTable,
					  szActFileName);
		TypeCheckVar(pVar->VarTypeDecl.pVarIdentSeq, 
					 pVar->VarTypeDecl.pType, 
					 pSymTable,
					 szActFileName);		
	  } break;

	  case VAR_IDENTIFIERDECL_SEQ: {		
		sVar* pIt = pVar;		
		for (; pIt; pIt = pIt->VarIdentDeclSeq.pSigIdent) {		  		
		  if (pIt->VarIdentDeclSeq.pValue) {
			TypeCheckExp(pIt->VarIdentDeclSeq.pValue, pSymTable, szActFileName);
			// ¿Se asigna expresion?
			if (pIt->VarIdentDeclSeq.pValue) {
			  // ¿NO es asignable la expresion al tipo de la vble?
			  if (!IsRightAssignableToLeft(pVarType,
										   pIt->VarIdentDeclSeq.pValue->pExpType)) {
				ReportErrorf(szActFileName,
							 pIt->VarIdentDeclSeq.szIdentifier,
							 pIt->unSrcLine,
							 "Error> No se puede asignar %s a %s",
							 PassTypeToString(pIt->VarIdentDeclSeq.pValue->pExpType),
							 PassTypeToString(pVarType));
			  } else if (pVarType != pUndefAuxType &&
						 pIt->VarIdentDeclSeq.pValue->pExpType != pUndefAuxType &&
						 pVarType->ValueType != pIt->VarIdentDeclSeq.pValue->pExpType->ValueType) {
				// Se hace un cast explicito, si procede
				pIt->VarIdentDeclSeq.pValue = ReturnExpCastNode(pVarType,
															    pIt->VarIdentDeclSeq.pValue);				
			  }			  
			}		
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
// - pSymTable. Tabla de simbolos.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
TypeCheckScript(sScript* pScript,
				sbyte* szActFileName)
{
  // ¿Hay script a imprimir?
  if (pScript) {
	switch(pScript->ScriptType) {
	  case SCRIPT_SEQ: {
		TypeCheckScript(pScript->ScriptSeq.pFirst, szActFileName);
		TypeCheckScript(pScript->ScriptSeq.pSecond, szActFileName);
	  } break;

	  case SCRIPT_DECL: {		
		szActFileName = pScript->ScriptDecl.szFileName;
		TypeCheckScriptType(pScript->ScriptDecl.pType, 
		                    pScript->ScriptDecl.pSymTable,
						    szActFileName);		
		TypeCheckImport(pScript->ScriptDecl.pImport,
						szActFileName);		
		TypeCheckConst(pScript->ScriptDecl.pConst,
					   pScript->ScriptDecl.pSymTable,
					   szActFileName);
		TypeCheckVar(pScript->ScriptDecl.pVar,
		             NULL,
					 pScript->ScriptDecl.pSymTable,
					 szActFileName);
		TypeCheckFunc(pScript->ScriptDecl.pFunc,		                  
					  szActFileName);
		TypeCheckStm(pScript->ScriptDecl.pStm,
		             pNumberAuxType,
					 pScript->ScriptDecl.pSymTable,
					 szActFileName);				
	  } break;	
	}; // ~ switch	
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre los tipos de scripts
// Parametros:
// - pScriptType. Tipos de scripts
// - pSymTable. Tabla de simbolos.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
TypeCheckScriptType(sScriptType* pScriptType,
					sSymbolTable* pSymTable,
					sbyte* szActFileName)
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

	TypeCheckArgument(pScriptType->pArguments, pSymTable, szActFileName);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre imports
// Parametros:
// - pImport. Import a imprimir.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
TypeCheckImport(sImport* pImport,
				sbyte* szActFileName)
{
  // ¿Hay import?
  if (pImport) {
	switch(pImport->ImportType) {
	  case IMPORT_SEQ: {
		TypeCheckImport(pImport->ImportSeq.pFirst, szActFileName);
		TypeCheckImport(pImport->ImportSeq.pSecond, szActFileName);
	  } break;

	  case IMPORT_FUNC: {
		sbyte* szPrevFileName = szActFileName;
		szActFileName = pImport->ImportFunc.szFileName;
		TypeCheckFunc(pImport->ImportFunc.pFunctions,		              
					  szActFileName);
		szActFileName = szPrevFileName;
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre una sentencia
// Parametros:
// - pStm. Sentencia a imprimir.
// - pSymTable. Tabla de simbolos de un script o de una funcion.
// - pReturnStmType. Indica el tipo de retorno para la setencia. Esto sera
//   necesario cuando se alcance una setencia de tipo return.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
TypeCheckStm(sStm* pStm,
			 sType* pReturnStmType,
			 sSymbolTable* pSymTable,
			 sbyte* szActFileName)
{
  // ¿Hay sentencia?
  if (pStm) { 
	switch(pStm->StmType) {
	  case STM_SEQ: {
		TypeCheckStm(pStm->StmSeq.pFirst, pReturnStmType, pSymTable, szActFileName);
		TypeCheckStm(pStm->StmSeq.pSecond, pReturnStmType, pSymTable, szActFileName);		
	  } break;

	  case STM_RETURN: {
		TypeCheckExp(pStm->StmReturn.pExp, pSymTable, szActFileName);		
		// ¿Se retorna expresion?
		if (pReturnStmType) {
		  // ¿Se espera retorno por parte de la sentencia?
		  if (pStm->StmReturn.pExp) {
			if (!IsRightAssignableToLeft(pReturnStmType, 
									     pStm->StmReturn.pExp->pExpType)) {
			  ReportErrorf(szActFileName,
						   "return",
						   pStm->unSrcLine,
						   "Error> No se puede asignar %s a %s",
						   PassTypeToString(pStm->StmReturn.pExp->pExpType),
   						   PassTypeToString(pReturnStmType));		  
			} else if (pReturnStmType != pUndefAuxType &&
					   pStm->StmReturn.pExp->pExpType != pUndefAuxType &&
					   pReturnStmType->ValueType != pStm->StmReturn.pExp->pExpType->ValueType) {
			  pStm->StmReturn.pExp = ReturnExpCastNode(pReturnStmType,
													   pStm->StmReturn.pExp);			
			}
		  } else {
			ReportErrorf(szActFileName,
					     "return",
					     pStm->unSrcLine,
					     "Error> Se deberia de retornar una expresion de tipo %s",					   
   					     PassTypeToString(pReturnStmType));
		  }
		} else {
		  // ¿Se retorna expresion cuando no se espera?
		  if (pStm->StmReturn.pExp) {
			ReportErrorf(szActFileName,
					     "return",
					     pStm->unSrcLine,
					     "Error> No se puede devolver nada");		  
		  }		  
		}	  
	  } break;

	  case STM_IF: {
		TypeCheckExp(pStm->If.pExp, pSymTable, szActFileName);
		TypeCheckStm(pStm->If.pThenStm, pReturnStmType, pSymTable, szActFileName);
		
		// ¿La expresion logica NO es numbery?
		if (!IsNumberType(pStm->If.pExp->pExpType)) {
		  ReportErrorf(szActFileName,
					   "if",
					   pStm->unSrcLine,
					   "Error> La condicion de un if debe de ser de tipo number");
		}
	  } break;

	  case STM_IFELSE: {
		TypeCheckExp(pStm->IfElse.pExp, pSymTable, szActFileName);
		TypeCheckStm(pStm->IfElse.pThenStm, pReturnStmType, pSymTable, szActFileName);
		TypeCheckStm(pStm->IfElse.pElseStm, pReturnStmType, pSymTable, szActFileName);
		
		// ¿La expresion logica NO es numbery?
		if (!IsNumberType(pStm->IfElse.pExp->pExpType)) {
		  ReportErrorf(szActFileName,
					   "if / else",
					   pStm->unSrcLine,
					   "Error> La condicion de un if / else debe de ser de tipo number");
		}
	  } break;

	  case STM_WHILE: {
		TypeCheckExp(pStm->While.pExp, pSymTable, szActFileName);
		TypeCheckStm(pStm->While.pDoStm, pReturnStmType, pSymTable, szActFileName);
		
		// ¿La expresion logica NO es numbery?
		if (!IsNumberType(pStm->While.pExp->pExpType)) {
		  ReportErrorf(szActFileName,
					   "while",
					   pStm->unSrcLine,
					   "Error> La condicion de un while debe de ser de tipo number");
		}
	  } break;
	  
	  case STM_EXP: {
		if (pStm->Exp.pExp) {
		  TypeCheckExp(pStm->Exp.pExp, pSymTable, szActFileName);
		}
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre una expresion.
// Parametros:
// - pExp. Expresion a imprimir
// - pSymTable. Enlace a la tabla de simbolos.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
TypeCheckExp(sExp* pExp,
			 sSymbolTable* pSymTable,
			 sbyte* szActFileName)
{
  // ¿Hay alguna expresion para imprimir?
  if (pExp) {
    switch(pExp->ExpType) {
  	  case EXP_IDENTIFIER: {
  		// Se establece el tipo del identificador (que sera vble o cte)
		sSymTableNode* pNode = SymTableGetNode(pSymTable,
											   pExp->ExpIdentifier.szIdentifier);
		assert(pNode);
		switch(pNode->SymTableNodeType) {
		  case SYM_TYPE_CONST: {
			pExp->pExpType = pNode->pIdConst->ConstDecl.pType;
		  } break;

		  case SYM_TYPE_VAR: {
			pExp->pExpType = pNode->pIdVar->VarTypeDecl.pType;
		  } break;

		  case SYM_TYPE_ARG: {
			pExp->pExpType = pNode->pIdArg->pType;
		  } break;

		  default:
			assert(0);
		}; // ~ switch		
	  } break;

	  case EXP_GLOBAL_CONST_ENTITY: {
		pExp->pExpType = pEntityAuxType;
	  } break;

	  case EXP_GLOBAL_CONST_NUMBER: {
		pExp->pExpType = pNumberAuxType;
	  } break;
		
	  case EXP_NUMBER_VALUE: {
		pExp->pExpType = pNumberAuxType;
	  } break;

	  case EXP_STRING_VALUE: {
		pExp->pExpType = pStringAuxType;
	  } break;

	  case EXP_ASSING: {
		// Vbles
		sSymTableNode* pNode = NULL;
		sType*         pLeftType = NULL;

	    TypeCheckExp(pExp->ExpAssing.pRightExp, pSymTable, szActFileName);		  
		
		// Se obtiene el tipo del identificador de la vble a la que asignar
		pNode = SymTableGetNode(pSymTable, pExp->ExpIdentifier.szIdentifier);
		assert(pNode);
		switch(pNode->SymTableNodeType) {
		  case SYM_TYPE_VAR: {
			pLeftType = pNode->pIdVar->VarTypeDecl.pType;
		  } break;

		  case SYM_TYPE_ARG: {
			pLeftType = pNode->pIdArg->pType;
		  } break;

		  default:
			assert(0);
		}; // ~ switch
		
		// ¿Es asignable?
		if (!IsRightAssignableToLeft(pLeftType, 
									 pExp->ExpAssing.pRightExp->pExpType)) {
		  ReportErrorf(szActFileName,
					   ":=",
					   pExp->unSrcLine,
					   "Error> No se puede asignar %s a %s",
					   PassTypeToString(pExp->ExpAssing.pRightExp->pExpType),
					   PassTypeToString(pLeftType));		
		} else if (pExp->ExpAssing.pRightExp->pExpType != pUndefAuxType &&
		           pExp->ExpAssing.pRightExp->pExpType->ValueType != pLeftType->ValueType) {
		  pExp->ExpAssing.pRightExp = ReturnExpCastNode(pLeftType,
														pExp->ExpAssing.pRightExp);		
		}
		pExp->pExpType = pLeftType;
	  } break;

	  case EXP_EQUAL: {
		// Vbles
		sType* pGEqualType = NULL;

	    TypeCheckExp(pExp->ExpEqual.pLeftExp, pSymTable, szActFileName);
	    TypeCheckExp(pExp->ExpEqual.pRightExp, pSymTable, szActFileName);

		// Se halla el mayor tipo comun
		pGEqualType = GetTheGEqualType(pExp->ExpEqual.pLeftExp->pExpType,
		                               pExp->ExpEqual.pRightExp->pExpType);
		
		if (pGEqualType == pUndefAuxType) {
		  if (pExp->ExpEqual.pLeftExp->pExpType != pUndefAuxType &&
			  pExp->ExpEqual.pRightExp->pExpType != pUndefAuxType) {
			ReportErrorf(szActFileName,
						 "==",
						 pExp->unSrcLine,
						 "Error> No se puede comparar %s con %s",
						 PassTypeToString(pExp->ExpEqual.pLeftExp->pExpType),
						 PassTypeToString(pExp->ExpEqual.pRightExp->pExpType));		
		  }
		} else {
		  if (pGEqualType->ValueType != pExp->ExpEqual.pLeftExp->pExpType->ValueType) {
			pExp->ExpEqual.pLeftExp = ReturnExpCastNode(pGEqualType,
													    pExp->ExpEqual.pLeftExp);
		  }
		  if (pGEqualType->ValueType != pExp->ExpEqual.pRightExp->pExpType->ValueType) {
			pExp->ExpEqual.pRightExp = ReturnExpCastNode(pGEqualType,
													     pExp->ExpEqual.pRightExp);
		  }
		}

		// La expresion sera number
		pExp->pExpType = pNumberAuxType;
	  } break;

	  case EXP_NOEQUAL: {
		// Vbles
		sType* pGEqualType = NULL;

	    TypeCheckExp(pExp->ExpNoEqual.pLeftExp, pSymTable, szActFileName);
	    TypeCheckExp(pExp->ExpNoEqual.pRightExp, pSymTable, szActFileName);
		
		// Se halla el mayor tipo comun
		pGEqualType = GetTheGEqualType(pExp->ExpNoEqual.pLeftExp->pExpType,
		                                 pExp->ExpNoEqual.pRightExp->pExpType);
		
		if (pGEqualType == pUndefAuxType) {
		  if (pExp->ExpNoEqual.pLeftExp->pExpType != pUndefAuxType &&
			  pExp->ExpNoEqual.pRightExp->pExpType != pUndefAuxType) {
			ReportErrorf(szActFileName,
						 "!=",
						 pExp->unSrcLine,
						 "Error> No se puede comparar %s con %s",
						 PassTypeToString(pExp->ExpNoEqual.pLeftExp->pExpType),
						 PassTypeToString(pExp->ExpNoEqual.pRightExp->pExpType));		
		  }
		} else {
		  if (pGEqualType->ValueType != pExp->ExpNoEqual.pLeftExp->pExpType->ValueType) {
			pExp->ExpNoEqual.pLeftExp = ReturnExpCastNode(pGEqualType,
													      pExp->ExpNoEqual.pLeftExp);
		  }
		  if (pGEqualType->ValueType != pExp->ExpNoEqual.pRightExp->pExpType->ValueType) {
			pExp->ExpNoEqual.pRightExp = ReturnExpCastNode(pGEqualType,
													       pExp->ExpNoEqual.pRightExp);
		  }
		}

		// La expresion sera number
		pExp->pExpType = pNumberAuxType;
	  } break;
		
	  case EXP_LESS: {
		TypeCheckExp(pExp->ExpLess.pLeftExp, pSymTable, szActFileName);
	    TypeCheckExp(pExp->ExpLess.pRightExp, pSymTable, szActFileName);

		// Solo si expresiones numericas
		if (!IsNumberType(pExp->ExpLess.pLeftExp->pExpType) ||
		    !IsNumberType(pExp->ExpLess.pRightExp->pExpType)) {
		  	ReportErrorf(szActFileName,
						 "<",
						 pExp->unSrcLine,
						 "Error> No se puede comparar %s con %s",
						 PassTypeToString(pExp->ExpLess.pLeftExp->pExpType),
						 PassTypeToString(pExp->ExpLess.pRightExp->pExpType));		
		}

		// La expresion sera number
		pExp->pExpType = pNumberAuxType;
	  } break;
		
	  case EXP_LESSEQUAL: {
		TypeCheckExp(pExp->ExpLEqual.pLeftExp, pSymTable, szActFileName);
	    TypeCheckExp(pExp->ExpLEqual.pRightExp, pSymTable, szActFileName);
		
		// Solo si expresiones numericas
		if (!IsNumberType(pExp->ExpLEqual.pLeftExp->pExpType) ||
		    !IsNumberType(pExp->ExpLEqual.pRightExp->pExpType)) {
		  	ReportErrorf(szActFileName,
						 "<=",
						 pExp->unSrcLine,
						 "Error> No se puede comparar %s con %s",
						 PassTypeToString(pExp->ExpLEqual.pLeftExp->pExpType),
						 PassTypeToString(pExp->ExpLEqual.pRightExp->pExpType));		
		}

		// La expresion sera number
		pExp->pExpType = pNumberAuxType;
	  } break;
		
	  case EXP_GREATER: {
		TypeCheckExp(pExp->ExpGreater.pLeftExp, pSymTable, szActFileName);
	    TypeCheckExp(pExp->ExpGreater.pRightExp, pSymTable, szActFileName);

		// Solo si expresiones numericas
		if (!IsNumberType(pExp->ExpGreater.pLeftExp->pExpType) ||
		    !IsNumberType(pExp->ExpGreater.pRightExp->pExpType)) {
		  	ReportErrorf(szActFileName,
						 ">",
						 pExp->unSrcLine,
						 "Error> No se puede comparar %s con %s",
						 PassTypeToString(pExp->ExpGreater.pLeftExp->pExpType),
						 PassTypeToString(pExp->ExpGreater.pRightExp->pExpType));		
		}

		// La expresion sera number
		pExp->pExpType = pNumberAuxType;
	  } break;
	  		
	  case EXP_GREATEREQUAL: {
		TypeCheckExp(pExp->ExpGEqual.pLeftExp, pSymTable, szActFileName);
	    TypeCheckExp(pExp->ExpGEqual.pRightExp, pSymTable, szActFileName);

		// Solo si expresiones numericas
		if (!IsNumberType(pExp->ExpGEqual.pLeftExp->pExpType) ||
		    !IsNumberType(pExp->ExpGEqual.pRightExp->pExpType)) {
		  	ReportErrorf(szActFileName,
						 ">=",
						 pExp->unSrcLine,
						 "Error> No se puede comparar %s con %s",
						 PassTypeToString(pExp->ExpGEqual.pLeftExp->pExpType),
						 PassTypeToString(pExp->ExpGEqual.pRightExp->pExpType));		
		}

		// La expresion sera number
		pExp->pExpType = pNumberAuxType;
	  } break;
		
	  case EXP_ADD: {
	    TypeCheckExp(pExp->ExpAdd.pLeftExp, pSymTable, szActFileName);
	    TypeCheckExp(pExp->ExpAdd.pRightExp, pSymTable, szActFileName);

		// Se obtiene el tipo mayor y se le asocia al tipo de la expresion
		pExp->pExpType = GetTheGEqualType(pExp->ExpAdd.pLeftExp->pExpType,
										  pExp->ExpAdd.pRightExp->pExpType);
		if (pUndefAuxType == pExp->pExpType) {
		  if (pUndefAuxType != pExp->ExpAdd.pLeftExp->pExpType &&
			  pUndefAuxType != pExp->ExpAdd.pRightExp->pExpType) {
			ReportErrorf(szActFileName,
						 "+",
						 pExp->unSrcLine,
						 "Error> No se puede sumar %s con %s",
						 PassTypeToString(pExp->ExpAdd.pLeftExp->pExpType),
						 PassTypeToString(pExp->ExpAdd.pRightExp->pExpType));		
		  }
		} else {
		  if (pExp->pExpType->ValueType != pExp->ExpAdd.pLeftExp->pExpType->ValueType) {
			pExp->ExpAdd.pLeftExp = ReturnExpCastNode(pExp->pExpType,
													  pExp->ExpAdd.pLeftExp);		
		  }

		  if (pExp->pExpType->ValueType != pExp->ExpAdd.pRightExp->pExpType->ValueType) {
			pExp->ExpAdd.pRightExp = ReturnExpCastNode(pExp->pExpType,
													   pExp->ExpAdd.pRightExp);			
		  }
		}
	  } break;
		
	  case EXP_MINUS: {
	    TypeCheckExp(pExp->ExpMinus.pLeftExp, pSymTable, szActFileName);
	    TypeCheckExp(pExp->ExpMinus.pRightExp, pSymTable, szActFileName);

		// Solo se permitiran valores numericos
		if (!IsNumberType(pExp->ExpMinus.pLeftExp->pExpType) ||
		    !IsNumberType(pExp->ExpMinus.pRightExp->pExpType)) {
		  ReportErrorf(szActFileName,
					   "-",
					   pExp->unSrcLine,
					   "Error> No se puede restar %s con %s",
					   PassTypeToString(pExp->ExpMinus.pLeftExp->pExpType),
					   PassTypeToString(pExp->ExpMinus.pRightExp->pExpType));		
		}

		// Se asocia tipo numerico
		pExp->pExpType = pNumberAuxType;
	  } break;
		
	  case EXP_MULT: {
	    TypeCheckExp(pExp->ExpMult.pLeftExp, pSymTable, szActFileName);
	    TypeCheckExp(pExp->ExpMult.pRightExp, pSymTable, szActFileName);
		
		// Solo se permitiran valores numericos
		if (!IsNumberType(pExp->ExpMult.pLeftExp->pExpType) ||
		    !IsNumberType(pExp->ExpMult.pRightExp->pExpType)) {
		  ReportErrorf(szActFileName,
					   "*",
					   pExp->unSrcLine,
					   "Error> No se puede multiplicar %s por %s",
					   PassTypeToString(pExp->ExpMult.pLeftExp->pExpType),
					   PassTypeToString(pExp->ExpMult.pRightExp->pExpType));		
		}

		// Se asocia tipo numerico
		pExp->pExpType = pNumberAuxType;	  
	  } break;
		
	  case EXP_DIV: {
		TypeCheckExp(pExp->ExpDiv.pLeftExp, pSymTable, szActFileName);		
	    TypeCheckExp(pExp->ExpDiv.pRightExp, pSymTable, szActFileName);
		
		// Solo se permitiran valores numericos
		if (!IsNumberType(pExp->ExpDiv.pLeftExp->pExpType) ||
		    !IsNumberType(pExp->ExpDiv.pRightExp->pExpType)) {
		  ReportErrorf(szActFileName,
					   "/",
					   pExp->unSrcLine,
					   "Error> No se puede dividir %s por %s",
					   PassTypeToString(pExp->ExpDiv.pLeftExp->pExpType),
					   PassTypeToString(pExp->ExpDiv.pRightExp->pExpType));		
		}

		// Se asocia tipo numerico
		pExp->pExpType = pNumberAuxType;	  
	  } break;
		
	  case EXP_MODULO: {
	    TypeCheckExp(pExp->ExpModulo.pLeftExp, pSymTable, szActFileName);
	    TypeCheckExp(pExp->ExpModulo.pRightExp, pSymTable, szActFileName);
		
		// Solo se permitiran valores numericos
		if (!IsNumberType(pExp->ExpModulo.pLeftExp->pExpType) ||
		    !IsNumberType(pExp->ExpModulo.pRightExp->pExpType)) {
		  ReportErrorf(szActFileName,
					   "%",
					   pExp->unSrcLine,
					   "Error> No se puede dividir %s por %s",
					   PassTypeToString(pExp->ExpModulo.pLeftExp->pExpType),
					   PassTypeToString(pExp->ExpModulo.pRightExp->pExpType));		
		}

		// Se asocia tipo numerico
		pExp->pExpType = pNumberAuxType;	  
	  } break;
		
	  case EXP_UMINUS: {
	    TypeCheckExp(pExp->ExpUMinus.pExp, pSymTable, szActFileName);

		// Solo con valores numericos
		if (!IsNumberType(pExp->ExpUMinus.pExp->pExpType)) {
		  ReportErrorf(szActFileName,
					   "-",
					   pExp->unSrcLine,
					   "Error> Se esperaba valor number para el '-' unario");		
		}
		pExp->pExpType = pExp->ExpUMinus.pExp->pExpType;
	  } break;
		
	  case EXP_NOT: {
	    TypeCheckExp(pExp->ExpNot.pExp, pSymTable, szActFileName);
		
		// Solo con valores numericos
		if (!IsNumberType(pExp->ExpNot.pExp->pExpType)) {
		  ReportErrorf(szActFileName,
					   "!",
					   pExp->unSrcLine,
					   "Error> Se esperaba valor number para el ! (not)");		
		}
		pExp->pExpType = pExp->ExpNot.pExp->pExpType;
	  } break;
		
	  case EXP_AND: {
	    TypeCheckExp(pExp->ExpAnd.pLeftExp, pSymTable, szActFileName);
	    TypeCheckExp(pExp->ExpAnd.pRightExp, pSymTable, szActFileName);

		// Solo se permitiran valores numericos
		if (!IsNumberType(pExp->ExpAnd.pLeftExp->pExpType) ||
		    !IsNumberType(pExp->ExpAnd.pRightExp->pExpType)) {
		  ReportErrorf(szActFileName,
					   "&&",
					   pExp->unSrcLine,
					   "Error> No se puede realizar un Y logico entre %s y %s",
					   PassTypeToString(pExp->ExpAnd.pLeftExp->pExpType),
					   PassTypeToString(pExp->ExpAnd.pRightExp->pExpType));		
		}

		// Se asocia tipo numerico
		pExp->pExpType = pNumberAuxType;	  
	  } break;
		
	  case EXP_OR: {
	    TypeCheckExp(pExp->ExpOr.pLeftExp, pSymTable, szActFileName);
	    TypeCheckExp(pExp->ExpOr.pRightExp, pSymTable, szActFileName);

		// Solo se permitiran valores numericos
		if (!IsNumberType(pExp->ExpOr.pLeftExp->pExpType) ||
		    !IsNumberType(pExp->ExpOr.pRightExp->pExpType)) {
		  ReportErrorf(szActFileName,
					   "&&",
					   pExp->unSrcLine,
					   "Error> No se puede realizar un O logico entre %s y %s",
					   PassTypeToString(pExp->ExpOr.pLeftExp->pExpType),
					   PassTypeToString(pExp->ExpOr.pRightExp->pExpType));		
		}

		// Se asocia tipo numerico
		pExp->pExpType = pNumberAuxType;	  
	  } break;
		
	  case EXP_FUNC_INVOKE: {	
		// Vbles
		sSymTableNode* pNode = NULL;
		sArgument*     pArgIt = NULL;
		sExp*          pParamIt = NULL;
		sExp*          pPrevParamIt = NULL;
		
		TypeCheckParams(pExp->ExpFuncInvoke.pParams, pSymTable, szActFileName);

		// Se halla declaracion de la funcion en tabla de simbolos
		pNode = SymTableGetNode(pSymTable, pExp->ExpFuncInvoke.szIdentifier);
		assert(pNode);
		assert(pNode->SymTableNodeType == SYM_TYPE_FUNC);

		// Se comprueba coincidencia entre argumentos y parametros
		pArgIt = pNode->pIdFunc->FuncDecl.pArguments;
		pParamIt = pExp->ExpFuncInvoke.pParams;
		while (pArgIt &&
			   pParamIt) {
		  // ¿NO se puede asignar parametro a argumento?
		  if (!IsRightAssignableToLeft(pArgIt->pType, pParamIt->pExpType)) {
			ReportErrorf(szActFileName,
						 "(...)",
						 pParamIt->unSrcLine,
						 "Error> No se puede asignar %s a %s",
						 PassTypeToString(pParamIt->pExpType),
						 PassTypeToString(pArgIt->pType));	
		  } else if (pParamIt->pExpType != pUndefAuxType &&
					 pArgIt->pType->ValueType != pParamIt->pExpType->ValueType) {
			// ¿Cast en el primer parametro?
			if (NULL == pPrevParamIt) {
			  pExp->ExpFuncInvoke.pParams = ReturnExpCastNode(pArgIt->pType, pParamIt);
			  pParamIt = pExp->ExpFuncInvoke.pParams;			   
			} else {
			  pPrevParamIt->pSigExp = ReturnExpCastNode(pArgIt->pType, pParamIt);
			  pParamIt = pPrevParamIt->pSigExp;
			}
		  }

		  // Sig
		  pArgIt = pArgIt->pSigArgument;
		  pPrevParamIt = (NULL == pPrevParamIt) ? 
						  pExp->ExpFuncInvoke.pParams : pPrevParamIt->pSigExp;
		  pParamIt = pParamIt->pSigExp;
		}

		// ¿Faltaron parametros?
		if (pArgIt && !pParamIt) {
		  ReportErrorf(szActFileName,
					   "(...)",
					   pExp->unSrcLine,
					   "Error> Faltan parametros");	
		} else if(!pArgIt && pParamIt) {
		  // Sobraron parametros
		  ReportErrorf(szActFileName,
					   "(...)",
					   pExp->unSrcLine,
					   "Error> Sobran parametros");
		}

		// El tipo sera el que se definicion en la declaracion de la funcion		
		pExp->pExpType = pNode->pIdFunc->FuncDecl.pType;
	  } break;
		
	  case EXP_APIFUNC_INVOKE: {
		TypeCheckParams(pExp->ExpAPIFuncInvoke.pParams, pSymTable, szActFileName);
	    switch(pExp->ExpAPIFuncInvoke.ExpAPIFunc) {
	  	  case API_PASSTORGBCOLOR: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpAPIFuncInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case API_GETREDCOMPONENT: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpAPIFuncInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case API_GETGREENCOMPONENT: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpAPIFuncInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case API_GETBLUECOMPONENT: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpAPIFuncInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case API_RAND: {			
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, NULL };			
			CheckArguments(pArgs, &pExp->ExpAPIFuncInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case API_GETINTEGERVALUE: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpAPIFuncInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case API_GETDECIMALVALUE: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpAPIFuncInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case API_GETSTRINGSIZE: {
			sType* pArgs[] = { pStringAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpAPIFuncInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case API_WRITETOLOGGER: {
			sType* pArgs[] = { pStringAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpAPIFuncInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case API_ENABLECRISOLSCRIPTWARNINGS: {
			sType* pArgs[] = { NULL	};
			CheckArguments(pArgs, &pExp->ExpAPIFuncInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case API_DISABLECRISOLSCRIPTWARNINGS: {
			sType* pArgs[] = { NULL	};
			CheckArguments(pArgs, &pExp->ExpAPIFuncInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case API_SHOWFPS: {
			sType* pArgs[] = { pNumberAuxType, NULL	};
			CheckArguments(pArgs, &pExp->ExpAPIFuncInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case API_WAIT: {
			sType* pArgs[] = { pNumberAuxType, NULL	};
			CheckArguments(pArgs, &pExp->ExpAPIFuncInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
		}; // ~ switch	    
	  } break;

	  case EXP_GAMEOBJ_INVOKE: {
		TypeCheckParams(pExp->ExpGameObjInvoke.pParams, pSymTable, szActFileName);	    
	    switch(pExp->ExpGameObjInvoke.ExpGameObjMethod) {
	  	  case GAMEOBJ_QUITGAME: {
			sType* pArgs[] = { NULL	};
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case GAMEOBJ_WRITETOCONSOLE: {
			sType* pArgs[] = { pStringAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
		  
		  case GAMEOBJ_ACTIVEADVICEDIALOG: {
			sType* pArgs[] = { pStringAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
		  
		  case GAMEOBJ_ACTIVEQUESTIONDIALOG: {
			sType* pArgs[] = { pStringAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
		  
		  case GAMEOBJ_ACTIVETEXTREADERDIALOG: {
			sType* pArgs[] = { pStringAuxType, pStringAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
		  
		  case GAMEOBJ_ADDOPTIONTOTEXTSELECTORDIALOG: {
			sType* pArgs[] = { pNumberAuxType, pStringAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
		  
		  case GAMEOBJ_RESETOPTIONSINTEXTSELECTORDIALOG: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
		  
		  case GAMEOBJ_ACTIVETEXTSELECTORDIALOG: {
			sType* pArgs[] = { pStringAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
		  
		  case GAMEOBJ_PLAYMIDIMUSIC: {
			sType* pArgs[] = { pStringAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
		  
		  case GAMEOBJ_STOPMIDIMUSIC: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
		  
		  case GAMEOBJ_PLAYWAVAMBIENTSOUND: {
			sType* pArgs[] = { pStringAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
		  
		  case GAMEOBJ_STOPWAVAMBIENTSOUND: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
		  
		  case GAMEOBJ_ACTIVETRADEITEMSINTERFAZ: {
			sType* pArgs[] = { pEntityAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
		  
		  case GAMEOBJ_ADDOPTIONTOCONVERSATORINTERFAZ: {
			sType* pArgs[] = { pNumberAuxType, pStringAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
		  
		  case GAMEOBJ_RESETOPTIONSINCONVERSATORINTERFAZ: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
		  
		  case GAMEOBJ_ACTIVECONVERSATORINTERFAZ: {
			sType* pArgs[] = { pEntityAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case GAMEOBJ_DESACTIVECONVERSATORINTERFAZ: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case GAMEOBJ_GETOPTIONFROMCONVERSATORINTERFAZ: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
		  
		  case GAMEOBJ_SHOWPRESENTATION: {
			sType* pArgs[] = { pStringAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
		  
		  case GAMEOBJ_BEGINCUTSCENE: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
		  
		  case GAMEOBJ_ENDCUTSCENE: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case GAMEOBJ_SETSCRIPT: {
			sType* pArgs[] = { pNumberAuxType, pStringAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case GAMEOBJ_ISKEYPRESSED: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpGameObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
		}; // ~ switch	    
	  } break;

	  case EXP_WORLDOBJ_INVOKE: {
		TypeCheckParams(pExp->ExpWorldObjInvoke.pParams, pSymTable, szActFileName);	    
		switch(pExp->ExpWorldObjInvoke.ExpWorldObjMethod) {
	  	  case WORLDOBJ_GETAREANAME: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pStringAuxType;
		  } break;

		  case WORLDOBJ_GETAREAID: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case WORLDOBJ_GETAREAWIDTH: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case WORLDOBJ_GETAREAHEIGHT: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case WORLDOBJ_GETHOUR: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case WORLDOBJ_GETMINUTE: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case WORLDOBJ_SETHOUR: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case WORLDOBJ_SETMINUTE: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case WORLDOBJ_GETENTITY: {
			sType* pArgs[] = { pStringAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pEntityAuxType;
		  } break;

		  case WORLDOBJ_GETPLAYER: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pEntityAuxType;
		  } break;

		  case WORLDOBJ_ISFLOORVALID: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
		  
		  case WORLDOBJ_GETITEMAT: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case WORLDOBJ_GETNUMITEMSAT: {	
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pEntityAuxType;
		  } break;

		  case WORLDOBJ_GETDISTANCE: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, 
							   pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case WORLDOBJ_CALCULEPATHLENGHT: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, 
							   pNumberAuxType, pNumberAuxType, NULL };			
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);		
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case WORLDOBJ_LOADAREA: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);		
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case WORLDOBJ_CHANGEENTITYLOCATION: {
			sType* pArgs[] = { pEntityAuxType, pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);					
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case WORLDOBJ_ATTACHCAMERATOENTITY: {
			sType* pArgs[] = { pEntityAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case WORLDOBJ_ATTACHCAMERATOLOCATION: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case WORLDOBJ_ISCOMBATMODEACTIVE: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case WORLDOBJ_ENDCOMBAT: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case WORLDOBJ_GETCRIATUREINCOMBATTURN: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pEntityAuxType;
		  } break;
  
		  case WORLDOBJ_GETCOMBATANT: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case WORLDOBJ_GETNUMBEROFCOMBATANTS: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case WORLDOBJ_GETAREALIGHTMODEL: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case WORLDOBJ_SETIDLESCRIPTTIME: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case WORLDOBJ_SETSCRIPT: {
			sType* pArgs[] = { pNumberAuxType, pStringAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case WORLDOBJ_DESTROYENTITY: {
			sType* pArgs[] = { pEntityAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
  
		  case WORLDOBJ_CREATECRIATURE: {
			sType* pArgs[] = { pStringAuxType, 
			                   pNumberAuxType, pNumberAuxType, 
							   pStringAuxType,
							   pNumberAuxType, pNumberAuxType, 
							   pNumberAuxType, NULL 
			};
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pEntityAuxType;
		  } break;
		  
		  case WORLDOBJ_CREATEWALL: {
			sType* pArgs[] = { pStringAuxType,
			                   pNumberAuxType, pNumberAuxType, 
							   pStringAuxType,
							   pNumberAuxType, pNumberAuxType, 
							   pNumberAuxType, NULL 
			};
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pEntityAuxType;
		  } break;

		  case WORLDOBJ_CREATESCENARYOBJECT: {
			sType* pArgs[] = { pStringAuxType, 
			                   pNumberAuxType, pNumberAuxType, 
							   pStringAuxType,
							   pNumberAuxType, pNumberAuxType, 
							   NULL 
			};
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pEntityAuxType;
		  } break;

		  case WORLDOBJ_CREATEITEMABANDONED: {
			sType* pArgs[] = { pStringAuxType, 
			                   pNumberAuxType, pNumberAuxType, 
							   pStringAuxType,							   
							   pNumberAuxType, NULL 
			};
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pEntityAuxType;
		  } break;
  
		  case WORLDOBJ_CREATEITEMWITHOWNER: {
			sType* pArgs[] = { pStringAuxType, 
			                   pEntityAuxType, pStringAuxType, 
							   pNumberAuxType, 
							   NULL 
			};
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pEntityAuxType;
		  } break;

		  case WORLDOBJ_SETWORLDTIMEPAUSE: {		
			sType* pArgs[] = { pNumberAuxType, NULL	};
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case WORLDOBJ_ISWORLDTIMEINPAUSE: {		
			sType* pArgs[] = { NULL	};
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case WORLDOBJ_SETELEVATIONAT: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, pNumberAuxType, NULL	};
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case WORLDOBJ_GETELEVATIONAT: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, NULL	};
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case WORLDOBJ_NEXTTURN: {
			sType* pArgs[] = { pEntityAuxType, NULL	};
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case WORLDOBJ_GETLIGHTAT: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, NULL	};
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case WORLDOBJ_PLAYWAVSOUND: {
			sType* pArgs[] = { pStringAuxType, NULL	};
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case WORLDOBJ_SETSCRIPTAT: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, pNumberAuxType, pStringAuxType, NULL	};
			CheckArguments(pArgs, &pExp->ExpWorldObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
		}; // ~ switch  	    
	  } break;
		
	  case EXP_ENTITYOBJ_INVOKE: {
		TypeCheckParams(pExp->ExpEntityObjInvoke.pParams, pSymTable, szActFileName);	    
	    switch(pExp->ExpEntityObjInvoke.ExpEntityObjMethod) {
	  	  case ENTITYOBJ_GETNAME: {			  
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pStringAuxType;
		  } break;

		  case ENTITYOBJ_SETNAME: {
			sType* pArgs[] = { pStringAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_GETENTITYTYPE: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
		  
		  case ENTITYOBJ_GETTYPE: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case ENTITYOBJ_SAY: {
			sType* pArgs[] = { pStringAuxType, pStringAuxType, pNumberAuxType,
							   pNumberAuxType, pNumberAuxType, pNumberAuxType, NULL 
			};
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_SHUTUP: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_ISSAYING: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case ENTITYOBJ_ATTACHGFX: {
			sType* pArgs[] = { pStringAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_RELEASEGFX: {
			sType* pArgs[] = { pStringAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_RELEASEALLGFX: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_ISGFXATTACHED: {
			sType* pArgs[] = { pStringAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case ENTITYOBJ_GETNUMITEMSINCONTAINER: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case ENTITYOBJ_GETITEMFROMCONTAINER: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;		
			
		  case ENTITYOBJ_ISITEMINCONTAINER: {
			sType* pArgs[] = { pEntityAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case ENTITYOBJ_TRANSFERITEMTOCONTAINER: {
			sType* pArgs[] = { pEntityAuxType, pEntityAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_INSERTITEMINCONTAINER: {
			sType* pArgs[] = { pEntityAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_REMOVEITEMOFCONTAINER: {
			sType* pArgs[] = { pEntityAuxType, pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_SETANIMTEMPLATESTATE: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_SETPORTRAITANIMTEMPLATESTATE: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_SETIDLESCRIPTTIME: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case ENTITYOBJ_SETLIGHT: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case ENTITYOBJ_GETLIGHT: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case ENTITYOBJ_GETXPOS: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
	
		  case ENTITYOBJ_GETYPOS: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case ENTITYOBJ_GETELEVATION: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case ENTITYOBJ_SETELEVATION: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case ENTITYOBJ_GETLOCALATTRIBUTE: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case ENTITYOBJ_SETLOCALATTRIBUTE: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_GETOWNER: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pEntityAuxType;
		  } break;
			
		  case ENTITYOBJ_GETCLASS: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case ENTITYOBJ_GETINCOMBATUSECOST: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case ENTITYOBJ_GETGLOBALATTRIBUTE: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case ENTITYOBJ_SETGLOBALATTRIBUTE: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_GETWALLORIENTATION: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case ENTITYOBJ_BLOCKACCESS: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
 		
		  case ENTITYOBJ_UNBLOCKACCESS: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_ISACCESSBLOCKED: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case ENTITYOBJ_SETSYMPTOM: {		
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_ISSYMPTOMACTIVE: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;		
		  } break;
			
		  case ENTITYOBJ_GETGENRE: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;		
		  } break;
			
		  case ENTITYOBJ_GETHEALTH: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case ENTITYOBJ_SETHEALTH: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;		
		  } break;
			
		  case ENTITYOBJ_GETEXTENDEDATTRIBUTE: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case ENTITYOBJ_SETEXTENDEDATTRIBUTE: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;		
		  } break;
			
		  case ENTITYOBJ_GETLEVEL: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case ENTITYOBJ_SETLEVEL: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;		
		  } break;
			
		  case ENTITYOBJ_GETEXPERIENCE: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case ENTITYOBJ_SETEXPERIENCE: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;		
		  } break;
			
		  case ENTITYOBJ_GETINCOMBATACTIONPOINTS: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
		  
		  case ENTITYOBJ_GETACTIONPOINTS: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case ENTITYOBJ_SETACTIONPOINTS: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;		
		  } break;
		
		  case ENTITYOBJ_ISHABILITYACTIVE: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case ENTITYOBJ_SETHABILITY: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;		
		  } break;
			
		  case ENTITYOBJ_USEHABILITY: {
			sType* pArgs[] = { pNumberAuxType, pEntityAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;		
		  } break;
			
		  case ENTITYOBJ_ISRUNMODEACTIVE: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case ENTITYOBJ_SETRUNMODE: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;		
		  } break;
			
		  case ENTITYOBJ_MOVETO: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case ENTITYOBJ_ISMOVING: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case ENTITYOBJ_STOPMOVING: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;		
		  } break;
			
		  case ENTITYOBJ_EQUIPITEM: {
			sType* pArgs[] = { pNumberAuxType, pEntityAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;		
		  } break;
			
		  case ENTITYOBJ_REMOVEITEMEQUIPPED: {
			sType* pArgs[] = { pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;		
		  } break;
			
		  case ENTITYOBJ_GETITEMEQUIPPED: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pEntityAuxType;
		  } break;
			
		  case ENTITYOBJ_ISITEMEQUIPPED: {
			sType* pArgs[] = { pEntityAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case ENTITYOBJ_DROPITEM: {
			sType* pArgs[] = { pEntityAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;		
		  } break;
			
		  case ENTITYOBJ_USEITEM: {
			sType* pArgs[] = { pEntityAuxType, pEntityAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_MANIPULATE: {
			sType* pArgs[] = { pEntityAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_SETTRANSPARENTMODE: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_ISTRANSPARENTMODEACTIVE: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case ENTITYOBJ_CHANGEANIMORIENTATION: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case ENTITYOBJ_GETANIMORIENTATION: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case ENTITYOBJ_SETALINGMENT: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_SETALINGMENTWITH: {
			sType* pArgs[] = { pEntityAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_SETALINGMENTAGAINST: {
			sType* pArgs[] = { pEntityAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;
			
		  case ENTITYOBJ_GETALINGMENT: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
			
		  case ENTITYOBJ_HITENTITY: {
			sType* pArgs[] = { pEntityAuxType, pNumberAuxType, pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case ENTITYOBJ_SETSCRIPT: {
			sType* pArgs[] = { pNumberAuxType, pStringAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case ENTITYOBJ_ISGHOSTMOVEMODEACTIVE: {		
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
	  
		  case ENTITYOBJ_SETGHOSTMOVEMODE: {
			sType* pArgs[] = { pNumberAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pVoidAuxType;
		  } break;

		  case ENTITYOBJ_GETRANGE: {
			sType* pArgs[] = { NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;

		  case ENTITYOBJ_ISINRANGE: {
			sType* pArgs[] = { pEntityAuxType, NULL };
			CheckArguments(pArgs, &pExp->ExpEntityObjInvoke.pParams, pExp->unSrcLine, szActFileName);
			pExp->pExpType = pNumberAuxType;
		  } break;
		}; // ~ switch	    
	  } break;

  	  case EXP_CAST: {				
	    TypeCheckExp(pExp->ExpCast.pExp, pSymTable, szActFileName);		  
	  } break;

	}; // ~ switch  
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Esta funcion se llamara siempre que se quiera imprimir los parametros
//   que se le pasan a una funcion o metodo.
// Parametros:
// - pParam. Puntero al parametro
// - pSymTable. Tabla de simbolos.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
// - Funcion privada
///////////////////////////////////////////////////////////////////////////////
void 
TypeCheckParams(sExp* pParam,
				sSymbolTable* pSymTable,
			    sbyte* szActFileName)
{
  // ¿Es parametro valido?
  if (pParam) {
	sExp* pIt = pParam;
	for (; pIt != NULL; pIt = pIt->pSigExp) {
	  TypeCheckExp(pIt, pSymTable, szActFileName);
	}  
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre un tipo
// Parametros:
// - pType. Tipo a imprimir
// - pSymTable. Tabla de simbolos.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
TypeCheckType(sType* pType,
			  sSymbolTable* pSymTable,
			  sbyte* szActFileName)
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
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
// - Solo se contemplara el chequeo de la declaracion de una funcion si
//   fue invocada en al menos una ocasion.
///////////////////////////////////////////////////////////////////////////////
void 
TypeCheckFunc(sFunc* pFunc,
			  sbyte* szActFileName)
{
  // ¿Hay funcion?
  if (pFunc) {
	switch (pFunc->eFuncType) {
	  case FUNC_SEQ: {
		TypeCheckFunc(pFunc->FuncSeq.pFirst, szActFileName);
		TypeCheckFunc(pFunc->FuncSeq.pSecond, szActFileName);
	  } break;

	  case FUNC_DECL: {				  
		// ¿Fue invocada?
		if (pFunc->FuncDecl.unWasInvoked) {
		  TypeCheckType(pFunc->FuncDecl.pType, 
		                pFunc->FuncDecl.pSymTable,
					    szActFileName);
		  TypeCheckArgument(pFunc->FuncDecl.pArguments, 
							pFunc->FuncDecl.pSymTable,
							szActFileName);
  		  TypeCheckConst(pFunc->FuncDecl.pConst, 
						 pFunc->FuncDecl.pSymTable,
						 szActFileName);		
		  TypeCheckVar(pFunc->FuncDecl.pVar, 					
					   NULL,
					   pFunc->FuncDecl.pSymTable,
					   szActFileName);				
		  TypeCheckStm(pFunc->FuncDecl.pStm, 
					   pFunc->FuncDecl.pType,
					   pFunc->FuncDecl.pSymTable,
					   szActFileName);		
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
// - szActFileName. Nombre del fichero actual
// - pSymTable. Tabla de simbolos.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
TypeCheckArgument(sArgument* pArgument,
				  sSymbolTable* pSymTable,
				  sbyte* szActFileName)
{
  // ¿Hay argumentos para imprimir?
  if (pArgument) {
	sArgument* pIt = pArgument;
	for (; pIt; pIt = pIt->pSigArgument) {
	  TypeCheckType(pIt->pType, pSymTable, szActFileName);
	}
  }
}
