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
// Resource.cpp
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Notas:
// - Consultar Resource.h para mas informacion
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "Resource.h"
#include <stdio.h>

// Vbles globales
word unActOffset = 0;  // Offset para declaraciones de vars y const
word unActLabel = 0;   // Label para sentencias y expresiones
word unActFuncIdx = 1; // Indice numerico para funciones

// Funciones privadas / Funciones de apoyo
static word GetOffset(void);
static word GetLabel(void);
static word GetFuncIdx(void);
static void InitFuncIdx(void);

// Funciones privadas / Recorrido del AST
static void ResourceConst(sConst* pConst);
static void ResourceVar(sVar* pVar);
static void ResourceScript(sScript* pScript);
static void ResourceScriptType(sScriptType* pScriptType);
static void ResourceImport(sImport* pImport,
						   word* punNumFunctions);
static void ResourceStm(sStm* pStm);
static void ResourceExp(sExp* pExp);
static void ResourceType(sType* pType);
static void ResourceFunc(sFunc* pFunc,
						 word* punNumFunctions);
static void ResourceArgument(sArgument* pArgument);
static void ResourceParams(sExp* pParam);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retorna el valor del offset actual Y LUEGO incrementa el mismo.
// Parametros:
// Devuelve:
// - El valor actual del offset
// Notas:
///////////////////////////////////////////////////////////////////////////////
word 
GetOffset(void)
{
  // Retorna e incrementa
  return unActOffset++;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retorna el valor de la etiqueta actual y luego incrementa la misma
// Parametros:
// Devuelve:
// - El valor actual de label
// Notas:
///////////////////////////////////////////////////////////////////////////////
word 
GetLabel(void)
{
  // Retorna e incrementa
  return unActLabel++;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el siguiente indice numerico para funciones
// Parametros:
// Devuelve:
// - Un valor de indice para numerar a una fucion
// Notas:
///////////////////////////////////////////////////////////////////////////////
word GetFuncIdx(void)
{
  // Retorna e incrementa
  return unActFuncIdx++;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la vble global para indices de funciones
// Parametros:
// Devuelve:
// Notas:
// - Al inicializar, se pondra el valor 1 pues el valor de indice 0 sera para
//   el codigo que representara al evento al cual perteneceran las funciones
//   que tomaran sus valores Idx de esta funcion.
///////////////////////////////////////////////////////////////////////////////
void InitFuncIdx(void)
{
  // Establece inicializacion
  unActFuncIdx = 1;
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
ResourceGlobal(sGlobal* pGlobal)
{
  // ¿Hay datos?
  if (pGlobal) {
	unActOffset = 0;
	ResourceConst(pGlobal->pConst);
	ResourceVar(pGlobal->pVar);
	ResourceStm(pGlobal->pReturnStm);
	ResourceScript(pGlobal->pScript);
	pGlobal->unNumOffsets = unActOffset;
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
ResourceConst(sConst* pConst)
{
  // ¿Hay declaraciones?
  if (pConst) {
	switch(pConst->ConstType) {
	  case CONST_DECLSEQ: {
		ResourceConst(pConst->ConstSeq.pFirst);
		ResourceConst(pConst->ConstSeq.pSecond);
	  } break;

	  case CONST_DECL: {
		ResourceType(pConst->ConstDecl.pType);		
		ResourceExp(pConst->ConstDecl.pExp);
		pConst->ConstDecl.unOffsetDecl = GetOffset();
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre delcaracion de variables.
// Parametros:
// - pVar. Declaracion de variables
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ResourceVar(sVar* pVar)
{
  // ¿Hay variables?
  if (pVar) {
	switch(pVar->VarType) {
	  case VAR_TYPEDECL_SEQ: {
		ResourceVar(pVar->VarTypeDeclSeq.pFirst);
		ResourceVar(pVar->VarTypeDeclSeq.pSecond);
	  } break;

	  case VAR_TYPEDECL: {
		ResourceType(pVar->VarTypeDecl.pType);
		ResourceVar(pVar->VarTypeDecl.pVarIdentSeq);
	  } break;

	  case VAR_IDENTIFIERDECL_SEQ: {		
		sVar* pIt = pVar;		
		for (; pIt; pIt = pIt->VarIdentDeclSeq.pSigIdent) {
		  ResourceExp(pIt->VarIdentDeclSeq.pValue);
		  pIt->VarIdentDeclSeq.unOffsetDecl = GetOffset();
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
ResourceScript(sScript* pScript)
{
  // ¿Hay script a imprimir?
  if (pScript) {
	switch(pScript->ScriptType) {
	  case SCRIPT_SEQ: {
		ResourceScript(pScript->ScriptSeq.pFirst);
		ResourceScript(pScript->ScriptSeq.pSecond);
	  } break;

	  case SCRIPT_DECL: {		
		// Se guarda offset actual
	    pScript->ScriptDecl.unFirstOffset = unActOffset;

		// Inicializa valor de Idx para funciones usadas en script
		InitFuncIdx();

		// Recorrido
		// Nota: Las funciones globales se recorren antes del propio cuerpo del 
		// script y de las funciones locales, pues no contemplaran las variables
		// y constantes definidas en el ambito del script.
		ResourceScriptType(pScript->ScriptDecl.pType);		
		pScript->ScriptDecl.unNumFunctions = 0;
		ResourceImport(pScript->ScriptDecl.pImport, &pScript->ScriptDecl.unNumFunctions);		
		ResourceConst(pScript->ScriptDecl.pConst);
		ResourceVar(pScript->ScriptDecl.pVar);

		// Antes de la llamada a funcion, se calcula el numero de vbles
		// locales al script
		pScript->ScriptDecl.unNumOffsets = unActOffset - pScript->ScriptDecl.unFirstOffset;

		// Funcion
		ResourceFunc(pScript->ScriptDecl.pFunc, &pScript->ScriptDecl.unNumFunctions);

		// Al retorno se restaura el offset previo al comienzo del script
		// y se recorre el cuerpo del mismo, guardando luego el numero de
		// etiquetas
		unActOffset = pScript->ScriptDecl.unFirstOffset;
		unActLabel = 0;		
		ResourceStm(pScript->ScriptDecl.pStm);								
		pScript->ScriptDecl.unNumLabels = unActLabel;
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
ResourceScriptType(sScriptType* pScriptType)
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

	// Recorre la lista de argumentos asociados al tipo.
	ResourceArgument(pScriptType->pArguments);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre imports
// Parametros:
// - pImport. Import a imprimir.
// - punNumFunctions. Direccion a la vble que contabiliza el numero de
//   funciones que tiene el script, contando las globales (import) y las
//   locales
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ResourceImport(sImport* pImport,
			   word* punNumFunctions)
{
  // ¿Hay import?
  if (pImport) {
	switch(pImport->ImportType) {
	  case IMPORT_SEQ: {
		ResourceImport(pImport->ImportSeq.pFirst, punNumFunctions);
		ResourceImport(pImport->ImportSeq.pSecond, punNumFunctions);
	  } break;

	  case IMPORT_FUNC: {
		ResourceFunc(pImport->ImportFunc.pFunctions, punNumFunctions);
  	  } break;
	}; // ~ switch
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
ResourceStm(sStm* pStm)
{
  // ¿Hay sentencia?
  if (pStm) { 
	switch(pStm->StmType) {
	  case STM_SEQ: {
		ResourceStm(pStm->StmSeq.pFirst);
		ResourceStm(pStm->StmSeq.pSecond);		
	  } break;

	  case STM_RETURN: {
		ResourceExp(pStm->StmReturn.pExp);		
	  } break;

	  case STM_IF: {
		pStm->If.unEndLabel = GetLabel();
		ResourceExp(pStm->If.pExp);
		ResourceStm(pStm->If.pThenStm);		
	  } break;

	  case STM_IFELSE: {
		pStm->IfElse.unElseLabel = GetLabel();
		pStm->IfElse.unEndLabel = GetLabel();
		ResourceExp(pStm->IfElse.pExp);
		ResourceStm(pStm->IfElse.pThenStm);
		ResourceStm(pStm->IfElse.pElseStm);		
	  } break;

	  case STM_WHILE: {
		pStm->While.unStartLabel = GetLabel();
		pStm->While.unEndLabel = GetLabel();
		ResourceExp(pStm->While.pExp);
		ResourceStm(pStm->While.pDoStm);
	  } break;
	  
	  case STM_EXP: {
	    ResourceExp(pStm->Exp.pExp);	
	  } break;
	}; // ~ switch
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
ResourceExp(sExp* pExp)
{
  // ¿Hay alguna expresion para imprimir?
  if (pExp) {
    switch(pExp->ExpType) {
  	  case EXP_IDENTIFIER: {  		
	  } break;

	  case EXP_GLOBAL_CONST_ENTITY: {
	  } break;

	  case EXP_GLOBAL_CONST_NUMBER: {		
	  } break;
		
	  case EXP_NUMBER_VALUE: {
	  } break;

	  case EXP_STRING_VALUE: {
	  } break;

	  case EXP_ASSING: {
	    ResourceExp(pExp->ExpAssing.pRightExp);		  	
	  } break;

	  case EXP_EQUAL: {
		pExp->ExpEqual.unTrueLabel = GetLabel();
		pExp->ExpEqual.unEndLabel = GetLabel();
		ResourceExp(pExp->ExpEqual.pLeftExp);
	    ResourceExp(pExp->ExpEqual.pRightExp);
	  } break;

	  case EXP_NOEQUAL: {
		pExp->ExpNoEqual.unTrueLabel = GetLabel();
		pExp->ExpNoEqual.unEndLabel = GetLabel();
	    ResourceExp(pExp->ExpNoEqual.pLeftExp);
	    ResourceExp(pExp->ExpNoEqual.pRightExp);
	  } break;
		
	  case EXP_LESS: {
		pExp->ExpLess.unTrueLabel = GetLabel();
		pExp->ExpLess.unEndLabel = GetLabel();
		ResourceExp(pExp->ExpLess.pLeftExp);
	    ResourceExp(pExp->ExpLess.pRightExp);
	  } break;
		
	  case EXP_LESSEQUAL: {
		pExp->ExpLEqual.unTrueLabel = GetLabel();
		pExp->ExpLEqual.unEndLabel = GetLabel();
		ResourceExp(pExp->ExpLEqual.pLeftExp);
	    ResourceExp(pExp->ExpLEqual.pRightExp);
	  } break;
		
	  case EXP_GREATER: {
		pExp->ExpGreater.unTrueLabel = GetLabel();
		pExp->ExpGreater.unEndLabel = GetLabel();
		ResourceExp(pExp->ExpGreater.pLeftExp);
	    ResourceExp(pExp->ExpGreater.pRightExp);
	  } break;
	  		
	  case EXP_GREATEREQUAL: {
		pExp->ExpGEqual.unTrueLabel = GetLabel();
		pExp->ExpGEqual.unEndLabel = GetLabel();
		ResourceExp(pExp->ExpGEqual.pLeftExp);
	    ResourceExp(pExp->ExpGEqual.pRightExp);
	  } break;
		
	  case EXP_ADD: {
	    ResourceExp(pExp->ExpAdd.pLeftExp);
	    ResourceExp(pExp->ExpAdd.pRightExp);
	  } break;
		
	  case EXP_MINUS: {
	    ResourceExp(pExp->ExpMinus.pLeftExp);
	    ResourceExp(pExp->ExpMinus.pRightExp);
	  } break;
		
	  case EXP_MULT: {
	    ResourceExp(pExp->ExpMult.pLeftExp);
	    ResourceExp(pExp->ExpMult.pRightExp);
	  } break;
		
	  case EXP_DIV: {
		ResourceExp(pExp->ExpDiv.pLeftExp);		
	    ResourceExp(pExp->ExpDiv.pRightExp);
	  } break;
		
	  case EXP_MODULO: {
	    ResourceExp(pExp->ExpModulo.pLeftExp);
	    ResourceExp(pExp->ExpModulo.pRightExp);
	  } break;
		
	  case EXP_UMINUS: {
	    ResourceExp(pExp->ExpUMinus.pExp);
	  } break;
		
	  case EXP_NOT: {
		pExp->ExpNot.unFalseLabel = GetLabel();
		pExp->ExpNot.unEndLabel = GetLabel();
	    ResourceExp(pExp->ExpNot.pExp);
	  } break;
		
	  case EXP_AND: {
		pExp->ExpAnd.unFalseLabel = GetLabel();
		ResourceExp(pExp->ExpAnd.pLeftExp);
	    ResourceExp(pExp->ExpAnd.pRightExp);
	  } break;
		
	  case EXP_OR: {
		pExp->ExpOr.unTrueLabel = GetLabel();
		ResourceExp(pExp->ExpOr.pLeftExp);
	    ResourceExp(pExp->ExpOr.pRightExp);
	  } break;
		
	  case EXP_FUNC_INVOKE: {	
		ResourceParams(pExp->ExpFuncInvoke.pParams);
	  } break;
		
	  case EXP_APIFUNC_INVOKE: {
		ResourceParams(pExp->ExpAPIFuncInvoke.pParams);
	  } break;

	  case EXP_GAMEOBJ_INVOKE: {
		ResourceParams(pExp->ExpGameObjInvoke.pParams);	    
	  } break;

	  case EXP_WORLDOBJ_INVOKE: {
		ResourceParams(pExp->ExpWorldObjInvoke.pParams);	    
	  } break;
		
	  case EXP_ENTITYOBJ_INVOKE: {
		ResourceParams(pExp->ExpEntityObjInvoke.pParams);	    
	  } break;

  	  case EXP_CAST: {				
	    ResourceExp(pExp->ExpCast.pExp);		  
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
// Devuelve:
// Notas:
// - Funcion privada
///////////////////////////////////////////////////////////////////////////////
void 
ResourceParams(sExp* pParam)
{
  // ¿Es parametro valido?
  if (pParam) {
	sExp* pIt = pParam;
	for (; pIt != NULL; pIt = pIt->pSigExp) {
	  ResourceExp(pIt);
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
ResourceType(sType* pType)
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
// - punNumFunctions. Direccion a la vble donde alojar el numero de funciones
//   hallados en la secuencia pasada.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ResourceFunc(sFunc* pFunc,
			 word* punNumFunctions)
{
  // ¿Hay funcion?
  if (pFunc) {
	switch (pFunc->eFuncType) {
	  case FUNC_SEQ: {
		ResourceFunc(pFunc->FuncSeq.pFirst, punNumFunctions);
		ResourceFunc(pFunc->FuncSeq.pSecond, punNumFunctions);
	  } break;

	  case FUNC_DECL: {				  
		// ¿Fue invocada?
		if (pFunc->FuncDecl.unWasInvoked) {
		  // Se guarda el offset previo recorrido
		  pFunc->FuncDecl.unFirstOffset = unActOffset;

		  // Se asigna valor Idx para la funcion
		  pFunc->FuncDecl.unFuncIdx = GetFuncIdx();

		  // Incrementa el num. de funciones
		  (*punNumFunctions)++;

		  // Recorrido
		  ResourceType(pFunc->FuncDecl.pType);
		  ResourceArgument(pFunc->FuncDecl.pArguments);
  		  ResourceConst(pFunc->FuncDecl.pConst);		
		  ResourceVar(pFunc->FuncDecl.pVar);			

		  // Se calcula el numero de offsets y se restaura el previo
		  pFunc->FuncDecl.unNumOffsets = unActOffset - pFunc->FuncDecl.unFirstOffset;
		  unActOffset = pFunc->FuncDecl.unFirstOffset;
		  
		  // Comienza el recorrido del cuerpo de la funcion guardando el 
		  // numero de etiquetas
		  unActLabel = 0;	 
		  ResourceStm(pFunc->FuncDecl.pStm);							
		  pFunc->FuncDecl.unNumLabels = unActLabel;
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
ResourceArgument(sArgument* pArgument)
{
  // ¿Hay argumentos para imprimir?
  if (pArgument) {
	sArgument* pIt = pArgument;
	for (; pIt; pIt = pIt->pSigArgument) {
	  ResourceType(pIt->pType);
	  pIt->unOffsetDecl = GetOffset();	  
	}
  }
}
