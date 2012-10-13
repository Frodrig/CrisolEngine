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
// Release.cpp
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Notas:
// - Consultar Release.h para mas informacion
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "Release.h"
#include <stdio.h>
#include "Memory.h"
#include <assert.h>
#include <string.h>

// Funciones extern
extern void EndAuxTypes(void);

// Definicion de funciones privadas / Recorrido por el AST
static void ReleaseConst(sConst* pConst);
static void ReleaseVar(sVar* pVar);
static void ReleaseScript(sScript* pScript);
static void ReleaseScriptType(sScriptType* pScriptType);
static void ReleaseImport(sImport* pImport);
static void ReleaseStm(sStm* pStm);
static void ReleaseExp(sExp* pExp);
static void ReleaseType(sType* pType);
static void ReleaseFunc(sFunc* pFunc);
static void ReleaseArgument(sArgument* pArgument);
static void ReleaseParams(sExp* pParam);
static void ReleaseLabelList(sLabel* pLabel,
							 word unNumLabels);
static void ReleaseOpcodeList(sOpcode* pOpcode);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicia el recorrido por el arbol AST para borrar los datos y para
//   coordinar el borrado de elementos localizados en otros modulos.
// Parametros:
// - pGlobal. Direccion a la estructura global.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
Release(sGlobal* pGlobal)
{
  // ¿Hay datos?
  if (pGlobal) {
	// Recorrido y demas borrados
	ReleaseConst(pGlobal->pConst);
	ReleaseVar(pGlobal->pVar);
	ReleaseStm(pGlobal->pReturnStm);
	ReleaseScript(pGlobal->pScript);
	ReleaseOpcodeList(pGlobal->pOpcodeList);	
	Mem_Free(pGlobal->szFileName);
	SymTableRelease(pGlobal->pSymTable);

	// Se borra direccion a estructura global
	Mem_Free(pGlobal);

	// Se llama a las funciones de liberacion de otros modulos
	EndAuxTypes();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre declaraciones const
// Parametros:
// - pConst. Declaraciones const
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ReleaseConst(sConst* pConst)
{
  // ¿Hay declaraciones?
  if (pConst) {
	switch(pConst->ConstType) {
	  case CONST_DECLSEQ: {
		ReleaseConst(pConst->ConstSeq.pFirst);
		ReleaseConst(pConst->ConstSeq.pSecond);		
	  } break;

	  case CONST_DECL: {
		ReleaseType(pConst->ConstDecl.pType);
		ReleaseExp(pConst->ConstDecl.pExp);		
		Mem_Free(pConst->ConstDecl.szIdentifier);
	  } break;
	}; // ~ switch

	// Borra nodo
	Mem_Free(pConst);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre delcaracion de variables.
// - En caso de que las vbles no tengan inicializacion por defecto, se les
//   asociara segun sea su tipo.
// Parametros:
// - pVar. Declaracion de variables
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ReleaseVar(sVar* pVar)
{
  // ¿Hay variables?
  if (pVar) {
	switch(pVar->VarType) {
	  case VAR_TYPEDECL_SEQ: {
		ReleaseVar(pVar->VarTypeDeclSeq.pFirst);
		ReleaseVar(pVar->VarTypeDeclSeq.pSecond);
	  } break;

	  case VAR_TYPEDECL: {
		ReleaseType(pVar->VarTypeDecl.pType);
		ReleaseVar(pVar->VarTypeDecl.pVarIdentSeq);
	  } break;

	  case VAR_IDENTIFIERDECL_SEQ: {
		ReleaseExp(pVar->VarIdentDeclSeq.pValue);			
		ReleaseVar(pVar->VarIdentDeclSeq.pSigIdent);
		Mem_Free(pVar->VarIdentDeclSeq.szIdentifier);
	  } break;
	}; // ~ switch
	
	// Borra nodo
	Mem_Free(pVar);
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
ReleaseScript(sScript* pScript)
{
  // ¿Hay script a imprimir?
  if (pScript) {
	switch(pScript->ScriptType) {
	  case SCRIPT_SEQ: {
		ReleaseScript(pScript->ScriptSeq.pFirst);
		ReleaseScript(pScript->ScriptSeq.pSecond);
	  } break;

	  case SCRIPT_DECL: {
		ReleaseScriptType(pScript->ScriptDecl.pType);		
		ReleaseImport(pScript->ScriptDecl.pImport);		
	    ReleaseConst(pScript->ScriptDecl.pConst);
		ReleaseVar(pScript->ScriptDecl.pVar);
		ReleaseFunc(pScript->ScriptDecl.pFunc);
		ReleaseStm(pScript->ScriptDecl.pStm);				
		ReleaseLabelList(pScript->ScriptDecl.pLabelList, 
						 pScript->ScriptDecl.unNumLabels);
		ReleaseOpcodeList(pScript->ScriptDecl.pOpcodeList);
		Mem_Free(pScript->ScriptDecl.szFileName);
		SymTableRelease(pScript->ScriptDecl.pSymTable);		
	  } break;	
	}; // ~ switch	

	// Borra nodo
	Mem_Free(pScript);
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
ReleaseScriptType(sScriptType* pScriptType)
{
  // ¿Hay tipo?
  if (pScriptType) {
	ReleaseArgument(pScriptType->pArguments);
	Mem_Free(pScriptType);
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
ReleaseImport(sImport* pImport)
{
  // ¿Hay import?
  if (pImport) {
	switch(pImport->ImportType) {
	  case IMPORT_SEQ: {
		ReleaseImport(pImport->ImportSeq.pFirst);
		ReleaseImport(pImport->ImportSeq.pSecond);
	  } break;

	  case IMPORT_FUNC: {
		ReleaseFunc(pImport->ImportFunc.pFunctions);
		Mem_Free(pImport->ImportFunc.szFileName);
		SymTableRelease(pImport->ImportFunc.pSymTable);
	  } break;
	}; // ~ switch

	// Borra nodo
	Mem_Free(pImport);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre una sentencia
// Parametros:
// - pStm. Sentencia a imprimir.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ReleaseStm(sStm* pStm)
{
  // ¿Hay sentencia?
  if (pStm) { 
	switch(pStm->StmType) {
	  case STM_SEQ: {
		// Recorrido
		ReleaseStm(pStm->StmSeq.pFirst);		
		ReleaseStm(pStm->StmSeq.pSecond);		
	  } break;

	  case STM_RETURN: {
		ReleaseExp(pStm->StmReturn.pExp);		
	  } break;
  
	  case STM_IF: {
		ReleaseExp(pStm->If.pExp);
		ReleaseStm(pStm->If.pThenStm);
	  } break;

	  case STM_IFELSE: {
		ReleaseExp(pStm->IfElse.pExp);
		ReleaseStm(pStm->IfElse.pThenStm);
		ReleaseStm(pStm->IfElse.pElseStm);
	  } break;

	  case STM_WHILE: {
		ReleaseExp(pStm->While.pExp);
		ReleaseStm(pStm->While.pDoStm);
	  } break;

	  case STM_EXP: {
		ReleaseExp(pStm->Exp.pExp);
	  } break;
	}; // ~ switch

	// Borra nodo
	Mem_Free(pStm);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre una expresion.
// Parametros:
// - pExp. Expresion a imprimir
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ReleaseExp(sExp* pExp)
{
  // ¿Hay alguna expresion para imprimir?
  if (pExp) {
    switch(pExp->ExpType) {
  	  case EXP_IDENTIFIER: {
		Mem_Free(pExp->ExpIdentifier.szIdentifier);
	  } break;

  	  case EXP_GLOBAL_CONST_ENTITY: {
	  } break;

	  case EXP_GLOBAL_CONST_NUMBER: {
	  } break;
		
	  case EXP_NUMBER_VALUE: {
	  } break;

	  case EXP_STRING_VALUE: {
		Mem_Free(pExp->ExpStrValue.szStrValue);		
	  } break;

	  case EXP_ASSING: {
	    ReleaseExp(pExp->ExpAssing.pRightExp);		  
		Mem_Free(pExp->ExpAssing.szIdentifier);
	  } break;

	  case EXP_EQUAL: {
	    ReleaseExp(pExp->ExpEqual.pLeftExp);
	    ReleaseExp(pExp->ExpEqual.pRightExp);
	  } break;

	  case EXP_NOEQUAL: {
	    ReleaseExp(pExp->ExpNoEqual.pLeftExp);
	    ReleaseExp(pExp->ExpNoEqual.pRightExp);
	  } break;
		
	  case EXP_LESS: {
	    ReleaseExp(pExp->ExpLess.pLeftExp);
	    ReleaseExp(pExp->ExpLess.pRightExp);
	  } break;
		
	  case EXP_LESSEQUAL: {
	    ReleaseExp(pExp->ExpLEqual.pLeftExp);
	    ReleaseExp(pExp->ExpLEqual.pRightExp);
	  } break;
		
	  case EXP_GREATER: {
	    ReleaseExp(pExp->ExpGreater.pLeftExp);
	    ReleaseExp(pExp->ExpGreater.pRightExp);
	  } break;
		
	  case EXP_GREATEREQUAL: {
	    ReleaseExp(pExp->ExpGEqual.pLeftExp);
	    ReleaseExp(pExp->ExpGEqual.pRightExp);
	  } break;
		
	  case EXP_ADD: {
	    ReleaseExp(pExp->ExpAdd.pLeftExp);
	    ReleaseExp(pExp->ExpAdd.pRightExp);
	  } break;
		
	  case EXP_MINUS: {
	    ReleaseExp(pExp->ExpMinus.pLeftExp);
	    ReleaseExp(pExp->ExpMinus.pRightExp);
	  } break;
		
	  case EXP_MULT: {
	    ReleaseExp(pExp->ExpMult.pLeftExp);
	    ReleaseExp(pExp->ExpMult.pRightExp);
	  } break;
		
	  case EXP_DIV: {
		ReleaseExp(pExp->ExpDiv.pLeftExp);		
		ReleaseExp(pExp->ExpDiv.pRightExp);
	  } break;
		
	  case EXP_MODULO: {
	    ReleaseExp(pExp->ExpModulo.pLeftExp);
	    ReleaseExp(pExp->ExpModulo.pRightExp);
	  } break;
		
	  case EXP_UMINUS: {
	    ReleaseExp(pExp->ExpUMinus.pExp);
	  } break;
		
	  case EXP_NOT: {
	    ReleaseExp(pExp->ExpNot.pExp);
	  } break;
		
	  case EXP_AND: {
	    ReleaseExp(pExp->ExpAnd.pLeftExp);
	    ReleaseExp(pExp->ExpAnd.pRightExp);
	  } break;
		
	  case EXP_OR: {
	    ReleaseExp(pExp->ExpOr.pLeftExp);
	    ReleaseExp(pExp->ExpOr.pRightExp);
	  } break;
		
	  case EXP_FUNC_INVOKE: {
	    ReleaseParams(pExp->ExpFuncInvoke.pParams);		  
		Mem_Free(pExp->ExpFuncInvoke.szIdentifier);
	  } break;
		
	  case EXP_APIFUNC_INVOKE: {
		ReleaseParams(pExp->ExpAPIFuncInvoke.pParams);
	  } break;
		
	  case EXP_GAMEOBJ_INVOKE: {
		ReleaseParams(pExp->ExpGameObjInvoke.pParams);	    
	  } break;
		
	  case EXP_WORLDOBJ_INVOKE: {
		ReleaseParams(pExp->ExpWorldObjInvoke.pParams);	    
	  } break;

	  case EXP_ENTITYOBJ_INVOKE: {
		ReleaseParams(pExp->ExpEntityObjInvoke.pParams);	    
		Mem_Free(pExp->ExpEntityObjInvoke.szIdentifier);
	  } break;

	  case EXP_CAST: {			
		ReleaseExp(pExp->ExpCast.pExp);
	  } break;
	}; // ~ switch  

	// Borra nodo
	Mem_Free(pExp);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Esta funcion se llamara siempre que se quieran recorrer los parametros
//   que se le pasan a una funcion o metodo.
// - Para el recorrido, haremos sucesivas llamadas recursivas para colocarnos
//   en la ultima expresion, luego se procedera a recorrer esa expresion,
//   teniendo en cuenta que a la vuelta se habra borrado el nodo al que 
//   apuntan. Como se hizo una llamada recursiva, al retornar aun podremos
//   acceder al nodo anterior al ultimo borrado.
// Parametros:
// - pParam. Puntero al parametro
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ReleaseParams(sExp* pParam)
{
  // Se eliman parametros
  sExp* pAuxParam;
  while (pParam) {
	pAuxParam = pParam;
	pParam = pParam->pSigExp;
	ReleaseExp(pAuxParam);
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
ReleaseType(sType* pType)
{
  // Borra nodo
  if (pType) {	
	Mem_Free(pType);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre una funcion.
// Parametros:
// - pFunc. Funcion a imprimir.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ReleaseFunc(sFunc* pFunc)
{
  // ¿Hay funcion?
  if (pFunc) {
	switch (pFunc->eFuncType) {
	  case FUNC_SEQ: {
		ReleaseFunc(pFunc->FuncSeq.pFirst);
		ReleaseFunc(pFunc->FuncSeq.pSecond);
	  } break;

	  case FUNC_DECL: {
		ReleaseType(pFunc->FuncDecl.pType);
		ReleaseArgument(pFunc->FuncDecl.pArguments);
		ReleaseConst(pFunc->FuncDecl.pConst);
		ReleaseVar(pFunc->FuncDecl.pVar);
		ReleaseStm(pFunc->FuncDecl.pStm);		
		Mem_Free(pFunc->FuncDecl.szIdentifier);
		Mem_Free(pFunc->FuncDecl.szSignature);
		SymTableRelease(pFunc->FuncDecl.pSymTable);
		ReleaseLabelList(pFunc->FuncDecl.pLabelList, 
						 pFunc->FuncDecl.unNumLabels);
		ReleaseOpcodeList(pFunc->FuncDecl.pOpcodeList);
	  } break;
	}; // ~ switch

	// Borra el nodo
	Mem_Free(pFunc);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre argumentos
// Parametros:
// - pArgument. Argumentos a imprimir
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ReleaseArgument(sArgument* pArgument)
{
  // ¿Hay argumentos para imprimir?
  if (pArgument) {
	ReleaseArgument(pArgument->pSigArgument);
	ReleaseType(pArgument->pType);
	Mem_Free(pArgument->szIdentifier);
	Mem_Free(pArgument);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina una lista de etiquetas
// Parametros:
// - pLabel. Inicio de la lista de etiquetas.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ReleaseLabelList(sLabel* pLabel,
				 word unNumLabels)
{
  // Procede a liberar nombres con las etiquetas
  word unIt = 0;
  for (; unIt < unNumLabels; ++unIt) {
	Mem_Free(pLabel[unIt].szLabelName);
  }  

  // Se libera el array de etiquetas
  Mem_Free(pLabel);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina la lista de opcodes
// Parametros:
// - pOpcode. Puntero al comienzo de la lista de opcodes.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ReleaseOpcodeList(sOpcode* pOpcode)
{
  // Procede a borrar la lista
  sOpcode* pAuxOpcode;
  while (pOpcode) {
	pAuxOpcode = pOpcode;
	pOpcode = pOpcode->pSigOpcode;
	Mem_Free(pAuxOpcode);
  }
}