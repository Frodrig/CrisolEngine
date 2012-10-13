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
// SymTableCheck.cpp
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Notas:
// - Consultar SymTableCheck.h para mas informacion
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "SymTableCheck.h"
#include "Error.h"
#include <stdio.h>
#include <assert.h>

// Enumerados
typedef enum { 
  FIRST_CHECK, 
  SECOND_CHECK 
} eCheckType;

// Definicion de funciones privadas
static void CheckParamsForRefArgs(sArgument* pArguments,
								  sExp* pParams,
								  sSymbolTable* pSymTable,
								  sbyte* szActFileName);

// Definicion de funciones privadas / chequeo del AST usando la tabla de simbolos
static void SymTableGlobalCheck(sGlobal* pGlobal,
								const eCheckType Check,
								sbyte* szActFileName);
static void SymTableConstCheck(sConst* pConst,
							  sSymbolTable* pSymTable,
							  const eCheckType Check,
							  sbyte* szActFileName);
static void SymTableVarCheck(sVar* pVar,
							 sVar* pTypeVar,
							 sSymbolTable* pSymTable,
							 const eCheckType Check,
							 sbyte* szActFileName);
static void SymTableScriptCheck(sScript* pScript,
								sSymbolTable* pPrevSymTable,
								const eCheckType Check,
								sbyte* szActFileName);
static void SymTableScriptTypeCheck(const sScriptType* pScriptType,
									sSymbolTable* pSymTable,
									const eCheckType Check,
								    sbyte* szActFileName);
static void SymTableImportCheck(sImport* pImport,
								sSymbolTable* pPrevSymTable,
								const eCheckType Check,
								sbyte* szActFileName);
static void SymTableStmCheck(const sStm* pStm,
							 sSymbolTable* pSymTable,
							 const eCheckType Check,
							 sbyte* szActFileName);
static void SymTableExpCheck(const sExp* pExp,
							 sSymbolTable* pSymTable,
							 const eCheckType Check,
							 sbyte* szActFileName);
static void SymTableTypeCheck(const sType* pType,
							  sSymbolTable* pSymTable,
							  const eCheckType Check,
							  sbyte* szActFileName);
static void SymTableFuncCheck(sFunc* pFunc,
							  sSymbolTable* pPrevSymTable,
							  sSymbolTable* pAuxSymTable,
							  const eCheckType Check,
							  sbyte* szActFileName);
static void SymTableArgumentCheck(sArgument* pArgument,
								  sSymbolTable* pSymTable,
								  const eCheckType Check,
								  sbyte* szActFileName);
static void SymTableParamsCheck(const sExp* pParam,
								sSymbolTable* pSymTable,
								const eCheckType Check,
								sbyte* szActFileName);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina las dos pasadas por el arbol AST.
// Parametros:
// - pGlobal. Direccion a la estructura global.
// - Check. Tipo de chequeo a realizar.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SymTableStartCheck(sGlobal* pGlobal)
{
  // Vbles
  sbyte* szActFileName = NULL; // Nombre de fichero actual

  // Realiza los dos chequeos
  SymTableGlobalCheck(pGlobal, FIRST_CHECK, szActFileName);
  SymTableGlobalCheck(pGlobal, SECOND_CHECK, szActFileName);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicia el recorrido por el arbol AST.
// Parametros:
// - pGlobal. Direccion a la estructura global.
// - Check. Tipo de chequeo a realizar.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SymTableGlobalCheck(sGlobal* pGlobal,
				    const eCheckType Check,
					sbyte* szActFileName)
{
  // ¿Hay datos?
  if (pGlobal) {
	szActFileName = pGlobal->szFileName;
	if (FIRST_CHECK == Check) {
	  pGlobal->pSymTable = SymTableCreate();
	}
	SymTableConstCheck(pGlobal->pConst, pGlobal->pSymTable, Check, szActFileName);
	SymTableVarCheck(pGlobal->pVar, NULL, pGlobal->pSymTable, Check, szActFileName);
	SymTableStmCheck(pGlobal->pReturnStm, pGlobal->pSymTable, Check, szActFileName);
	SymTableScriptCheck(pGlobal->pScript, pGlobal->pSymTable, Check, szActFileName);
	szActFileName = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre declaraciones const.
// Parametros:
// - pConst. Declaraciones const
// - Check. Tipo de chequeo a realizar.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SymTableConstCheck(sConst* pConst,
				   sSymbolTable* pSymTable,
				   const eCheckType Check,
				   sbyte* szActFileName)
{
  // ¿Hay declaraciones?
  if (pConst) {
	switch(pConst->ConstType) {
	  case CONST_DECLSEQ: {
		SymTableConstCheck(pConst->ConstSeq.pFirst, pSymTable, Check, szActFileName);
		SymTableConstCheck(pConst->ConstSeq.pSecond, pSymTable, Check, szActFileName);
	  } break;

	  case CONST_DECL: {
		SymTableTypeCheck(pConst->ConstDecl.pType, pSymTable, Check, szActFileName);
		if (FIRST_CHECK == Check) {
		  // ¿Identificador ya declarado?
		  if (SymTableIsPresent(pSymTable, pConst->ConstDecl.szIdentifier)) {
			// Si, luego hay error
			ReportErrorf(szActFileName, 
						 pConst->ConstDecl.szIdentifier,
						 pConst->unSrcLine,
						 "Error> Identificador duplicado");
		  } else {
			// No, luego se inserta en tabla de simbolos
			SymTableInsertConst(pSymTable, pConst->ConstDecl.szIdentifier, pConst);
		  }
		}
		SymTableExpCheck(pConst->ConstDecl.pExp, pSymTable, Check, szActFileName);		
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre delcaracion de variables.
// - En la tabla de simbolos se guardara un sVar de tipo VAR_TYPEDECL para
//   poder conocer el tipo de los identificadores. Con este nodo, encontrar
//   el enlace concreto de un identificador solo supondra navegar entre la
//   lista de identificadores y comparar, pues el nombre del identificador
//   siempre sera conocido desde el exterior.
// Parametros:
// - pVar. Declaracion de variables
// - pTypeVar. Enlace a la declaracion del tipo de un identificador. Solo 
//   valdra distinto de NULL cuando en la sig. llamada se pase un sVar de 
//   tipo VAR_IDENTIFIERDECL_SEQ.
// - pSymTable. Tabla de simbolos base.
// - Check. Tipo de chequeo a realizar.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SymTableVarCheck(sVar* pVar,
				 sVar* pTypeVar,
				 sSymbolTable* pSymTable,
				 const eCheckType Check,
				 sbyte* szActFileName)
{
  // ¿Hay variables?
  if (pVar) {
	switch(pVar->VarType) {
	  case VAR_TYPEDECL_SEQ: {
		SymTableVarCheck(pVar->VarTypeDeclSeq.pFirst, 
		                 pTypeVar,
						 pSymTable, 
						 Check, 
						 szActFileName);
		SymTableVarCheck(pVar->VarTypeDeclSeq.pSecond, 
		                 pTypeVar,
						 pSymTable, 
						 Check, 
						 szActFileName);
	  } break;

	  case VAR_TYPEDECL: {
		SymTableTypeCheck(pVar->VarTypeDecl.pType, 		                  
						  pSymTable, 
						  Check, 
						  szActFileName);
		SymTableVarCheck(pVar->VarTypeDecl.pVarIdentSeq, 
		                 pVar,
						 pSymTable, 
						 Check, 
						 szActFileName);		
	  } break;

	  case VAR_IDENTIFIERDECL_SEQ: {
		const sVar* pIt = pVar;
		for (; pIt; pIt = pIt->VarIdentDeclSeq.pSigIdent) {		  
		  if (FIRST_CHECK == Check) {			
			// ¿Identificador ya declarado?
			if (SymTableIsPresent(pSymTable, 
								  pIt->VarIdentDeclSeq.szIdentifier)) {
			  // Si, luego hay error
			  ReportErrorf(szActFileName, 
						   pIt->VarIdentDeclSeq.szIdentifier,
						   pIt->unSrcLine,
						   "Error> Identificador duplicado");
			} else {
			  // No, luego se inserta en tabla de simbolos
			  SymTableInsertVar(pSymTable, 
								pIt->VarIdentDeclSeq.szIdentifier, 
								pTypeVar);
			}
		  }
		  if (pIt->VarIdentDeclSeq.pValue) {
			SymTableExpCheck(pIt->VarIdentDeclSeq.pValue, 
							 pSymTable, 
							 Check, 
							 szActFileName);
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
// - pSymTable. Tabla de simbolos previa
// - Check. Tipo de chequeo a realizar.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SymTableScriptCheck(sScript* pScript,
				    sSymbolTable* pPrevSymTable,
					const eCheckType Check,					
					sbyte* szActFileName)
{
  // ¿Hay script a imprimir?
  if (pScript) {
	switch(pScript->ScriptType) {
	  case SCRIPT_SEQ: {
		SymTableScriptCheck(pScript->ScriptSeq.pFirst, pPrevSymTable, Check, szActFileName);
		SymTableScriptCheck(pScript->ScriptSeq.pSecond, pPrevSymTable, Check, szActFileName);
	  } break;

	  case SCRIPT_DECL: {		
		if (FIRST_CHECK == Check) {
		  pScript->ScriptDecl.pSymTable = SymTableCreate();
		  pScript->ScriptDecl.pSymTable->pParent = pPrevSymTable;
		}
		szActFileName = pScript->ScriptDecl.szFileName;
		SymTableScriptTypeCheck(pScript->ScriptDecl.pType, 
							    pScript->ScriptDecl.pSymTable,
								Check, 
								szActFileName);		
		SymTableImportCheck(pScript->ScriptDecl.pImport,
						    pScript->ScriptDecl.pSymTable,
						    Check, 
							szActFileName);		
		SymTableConstCheck(pScript->ScriptDecl.pConst,
						   pScript->ScriptDecl.pSymTable,
						   Check, 
						   szActFileName);
		SymTableVarCheck(pScript->ScriptDecl.pVar,
		                 NULL,
						 pScript->ScriptDecl.pSymTable,
						 Check, 
						 szActFileName);
		SymTableFuncCheck(pScript->ScriptDecl.pFunc,		                  
						  pScript->ScriptDecl.pSymTable,
						  NULL,
						  Check, 
						  szActFileName);		
		SymTableStmCheck(pScript->ScriptDecl.pStm,
						 pScript->ScriptDecl.pSymTable,
						 Check, 
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
// - pSymTable. Tabla de simbolos base.
// - Check. Tipo de chequeo a realizar.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SymTableScriptTypeCheck(const sScriptType* pScriptType,
					    sSymbolTable* pSymTable,
						const eCheckType Check,						
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

	SymTableArgumentCheck(pScriptType->pArguments, pSymTable, Check, szActFileName);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre imports
// Parametros:
// - pImport. Import a imprimir.
// - pSymTable. Tabla de simbolos previa
// - Check. Tipo de chequeo a realizar.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SymTableImportCheck(sImport* pImport,
					sSymbolTable* pPrevSymTable,
					const eCheckType Check,					
					sbyte* szActFileName)
{
  // ¿Hay import?
  if (pImport) {
	switch(pImport->ImportType) {
	  case IMPORT_SEQ: {
		SymTableImportCheck(pImport->ImportSeq.pFirst, pPrevSymTable, Check, szActFileName);
		SymTableImportCheck(pImport->ImportSeq.pSecond, pPrevSymTable, Check, szActFileName);
	  } break;

	  case IMPORT_FUNC: {
		sbyte* szPrevFileName = szActFileName;
		if (FIRST_CHECK == Check) {
		  pImport->ImportFunc.pSymTable = SymTableCreate();
		  pImport->ImportFunc.pSymTable->pParent = pPrevSymTable;		
		}
		szActFileName = pImport->ImportFunc.szFileName;
		SymTableFuncCheck(pImport->ImportFunc.pFunctions,		                  
						  pImport->ImportFunc.pSymTable,
						  pPrevSymTable,
						  Check, szActFileName);
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
// - pSymTable. Tabla de simbolos base.
// - Check. Tipo de chequeo a realizar.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SymTableStmCheck(const sStm* pStm,
				 sSymbolTable* pSymTable,
				 const eCheckType Check,
				 sbyte* szActFileName)
{
  // ¿Hay sentencia?
  if (pStm) { 
	switch(pStm->StmType) {
	  case STM_SEQ: {
		SymTableStmCheck(pStm->StmSeq.pFirst, pSymTable, Check, szActFileName);
		SymTableStmCheck(pStm->StmSeq.pSecond, pSymTable, Check, szActFileName);		
	  } break;

	  case STM_RETURN: {
		if (pStm->StmReturn.pExp) {
		  SymTableExpCheck(pStm->StmReturn.pExp, pSymTable, Check, szActFileName);
		}
	  } break;
  
	  case STM_IF: {
		SymTableExpCheck(pStm->If.pExp, pSymTable, Check, szActFileName);
		SymTableStmCheck(pStm->If.pThenStm, pSymTable, Check, szActFileName);
	  } break;

	  case STM_IFELSE: {
		SymTableExpCheck(pStm->IfElse.pExp, pSymTable, Check, szActFileName);
		SymTableStmCheck(pStm->IfElse.pThenStm, pSymTable, Check, szActFileName);
		SymTableStmCheck(pStm->IfElse.pElseStm, pSymTable, Check, szActFileName);
	  } break;

	  case STM_WHILE: {
		SymTableExpCheck(pStm->While.pExp, pSymTable, Check, szActFileName);
		SymTableStmCheck(pStm->While.pDoStm, pSymTable, Check, szActFileName);
	  } break;
	  
	  case STM_EXP: {
		if (pStm->Exp.pExp) {
		  SymTableExpCheck(pStm->Exp.pExp, pSymTable, Check, szActFileName);
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
// - pSymTable. Tabla de simbolos base.
// - Check. Tipo de chequeo a realizar.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SymTableExpCheck(const sExp* pExp,
				 sSymbolTable* pSymTable,
				 const eCheckType Check,
				 sbyte* szActFileName)
{
  // ¿Hay alguna expresion para imprimir?
  if (pExp) {
    switch(pExp->ExpType) {
  	  case EXP_IDENTIFIER: {
		if (FIRST_CHECK == Check) {
		  // El identificador debe de hallarse declarado
		  sSymTableNode* pNode = SymTableGetNode(pSymTable,
												 pExp->ExpIdentifier.szIdentifier);		  
		  if (pNode) {
			if (pNode->SymTableNodeType == SYM_TYPE_FUNC) {
			  ReportErrorf(szActFileName, 
						   pExp->ExpIdentifier.szIdentifier, 
						   pExp->unSrcLine,
						   "Error> El identificador %s es de funcion, no de variable / constante", 
						   pExp->ExpIdentifier.szIdentifier);
			}
		  } else {
			ReportErrorf(szActFileName, 
						 pExp->ExpIdentifier.szIdentifier, 
						 pExp->unSrcLine,
						 "Error> El identificador %s no se ha declarado", 
						 pExp->ExpIdentifier.szIdentifier);
		  }
		}
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
		if (FIRST_CHECK == Check) {
		  // El identificador en el lado izq. debe de ser una vble
    	  sSymTableNode* pNode = SymTableGetNode(pSymTable,
											     pExp->ExpAssing.szIdentifier);
		  if (pNode) {
			if (pNode->SymTableNodeType != SYM_TYPE_VAR &&
			    pNode->SymTableNodeType != SYM_TYPE_ARG) {
			  ReportErrorf(szActFileName, 
						   pExp->ExpIdentifier.szIdentifier, 
						   pExp->unSrcLine,
						   "Error> El identificador %s no es de variable", 
						   pExp->ExpIdentifier.szIdentifier);
			}
		  } else {
			 ReportErrorf(szActFileName, 
						  pExp->ExpAssing.szIdentifier, 
						  pExp->unSrcLine,
						  "Error> El identificador de variable %s no se ha declarado", 
						  pExp->ExpAssing.szIdentifier);
		  }	  
		}
	    SymTableExpCheck(pExp->ExpAssing.pRightExp, pSymTable, Check, szActFileName);		  
	  } break;

	  case EXP_EQUAL: {
	    SymTableExpCheck(pExp->ExpEqual.pLeftExp, pSymTable, Check, szActFileName);
	    SymTableExpCheck(pExp->ExpEqual.pRightExp, pSymTable, Check, szActFileName);
	  } break;

	  case EXP_NOEQUAL: {
	    SymTableExpCheck(pExp->ExpNoEqual.pLeftExp, pSymTable, Check, szActFileName);
	    SymTableExpCheck(pExp->ExpNoEqual.pRightExp, pSymTable, Check, szActFileName);
	  } break;
		
	  case EXP_LESS: {
	    SymTableExpCheck(pExp->ExpLess.pLeftExp, pSymTable, Check, szActFileName);
	    SymTableExpCheck(pExp->ExpLess.pRightExp, pSymTable, Check, szActFileName);
	  } break;
		
	  case EXP_LESSEQUAL: {
	    SymTableExpCheck(pExp->ExpLEqual.pLeftExp, pSymTable, Check, szActFileName);
	    SymTableExpCheck(pExp->ExpLEqual.pRightExp, pSymTable, Check, szActFileName);
	  } break;
		
	  case EXP_GREATER: {
	    SymTableExpCheck(pExp->ExpGreater.pLeftExp, pSymTable, Check, szActFileName);
	    SymTableExpCheck(pExp->ExpGreater.pRightExp, pSymTable, Check, szActFileName);
	  } break;
		
	  case EXP_GREATEREQUAL: {
	    SymTableExpCheck(pExp->ExpGEqual.pLeftExp, pSymTable, Check, szActFileName);
	    SymTableExpCheck(pExp->ExpGEqual.pRightExp, pSymTable, Check, szActFileName);
	  } break;
		
	  case EXP_ADD: {
	    SymTableExpCheck(pExp->ExpAdd.pLeftExp, pSymTable, Check, szActFileName);
	    SymTableExpCheck(pExp->ExpAdd.pRightExp, pSymTable, Check, szActFileName);
	  } break;
		
	  case EXP_MINUS: {
	    SymTableExpCheck(pExp->ExpMinus.pLeftExp, pSymTable, Check, szActFileName);
	    SymTableExpCheck(pExp->ExpMinus.pRightExp, pSymTable, Check, szActFileName);
	  } break;
		
	  case EXP_MULT: {
	    SymTableExpCheck(pExp->ExpMult.pLeftExp, pSymTable, Check, szActFileName);
	    SymTableExpCheck(pExp->ExpMult.pRightExp, pSymTable, Check, szActFileName);
	  } break;
		
	  case EXP_DIV: {
		// Si la expresion a la derecha es constante numerica, se comprobara
		// que sea distinta de 0
	    SymTableExpCheck(pExp->ExpDiv.pLeftExp, pSymTable, Check, szActFileName);		
	    SymTableExpCheck(pExp->ExpDiv.pRightExp, pSymTable, Check, szActFileName);
	  } break;
		
	  case EXP_MODULO: {
	    SymTableExpCheck(pExp->ExpModulo.pLeftExp, pSymTable, Check, szActFileName);
	    SymTableExpCheck(pExp->ExpModulo.pRightExp, pSymTable, Check, szActFileName);
	  } break;
		
	  case EXP_UMINUS: {
	    SymTableExpCheck(pExp->ExpUMinus.pExp, pSymTable, Check, szActFileName);
	  } break;
		
	  case EXP_NOT: {
	    SymTableExpCheck(pExp->ExpNot.pExp, pSymTable, Check, szActFileName);
	  } break;
		
	  case EXP_AND: {
	    SymTableExpCheck(pExp->ExpAnd.pLeftExp, pSymTable, Check, szActFileName);
	    SymTableExpCheck(pExp->ExpAnd.pRightExp, pSymTable, Check, szActFileName);
	  } break;
		
	  case EXP_OR: {
	    SymTableExpCheck(pExp->ExpOr.pLeftExp, pSymTable, Check, szActFileName);
	    SymTableExpCheck(pExp->ExpOr.pRightExp, pSymTable, Check, szActFileName);
	  } break;
		
	  case EXP_FUNC_INVOKE: {		
		if (SECOND_CHECK == Check) {
		  // El identificador debe de ser una funcion declarada		  
		  sSymTableNode* pNode = SymTableGetNode(pSymTable,
												 pExp->ExpFuncInvoke.szIdentifier);
		  if (pNode) {
			switch (pNode->SymTableNodeType) {
			  case SYM_TYPE_CONST: {
				ReportErrorf(szActFileName,
							 pExp->ExpFuncInvoke.szIdentifier,
							 pExp->unSrcLine,
							 "Error> Se esta intentando usar la constante %s como funcion", 
							 pExp->ExpFuncInvoke.szIdentifier);
			  } break;

			  case SYM_TYPE_VAR: 
			  case SYM_TYPE_ARG: {
				ReportErrorf(szActFileName,
							 pExp->ExpFuncInvoke.szIdentifier,
							 pExp->unSrcLine,
  							 "Error> Se esta intentando usar la variable %s como funcion", 
							 pExp->ExpFuncInvoke.szIdentifier);
			  } break;
			}; // ~ switch			
		  } else {
			ReportErrorf(szActFileName,
			             pExp->ExpFuncInvoke.szIdentifier,
						 pExp->unSrcLine,
						 "Error> El identificador de funcion %s no se halla declarado", 
						 pExp->ExpFuncInvoke.szIdentifier);
			break;
		  }

		  // Se recorren parametros pasados
		  SymTableParamsCheck(pExp->ExpFuncInvoke.pParams, pSymTable, Check, szActFileName);

		  // Se chequea coincidencia parametros - argumentos
		  CheckParamsForRefArgs(pNode->pIdFunc->FuncDecl.pArguments,
							    pExp->ExpFuncInvoke.pParams,
							    pSymTable,
							    szActFileName);
		  
		  // Se levanta el flag de funcion llamada si procede
		  if (!pNode->pIdFunc->FuncDecl.unWasInvoked) {
			// El levantar este flag nos permitira saber si una funcion es llamada
			// o no, pudiendo descartar realizar cualquier tipo de operacion en
			// los modulos siguientes.
			pNode->pIdFunc->FuncDecl.unWasInvoked = 1;
		  }
		} else {
		  SymTableParamsCheck(pExp->ExpFuncInvoke.pParams, pSymTable, Check, szActFileName);
		}
	  } break;
		
	  case EXP_APIFUNC_INVOKE: {
		SymTableParamsCheck(pExp->ExpAPIFuncInvoke.pParams, pSymTable, Check, szActFileName);
	    switch(pExp->ExpAPIFuncInvoke.ExpAPIFunc) {
	  	  case API_PASSTORGBCOLOR: {			
		  } break;
			
		  case API_GETREDCOMPONENT: {
		  } break;
			
		  case API_GETGREENCOMPONENT: {
		  } break;
			
		  case API_GETBLUECOMPONENT: {
		  } break;
			
		  case API_RAND: {
		  } break;
			
		  case API_GETINTEGERVALUE: {
		  } break;
			
		  case API_GETDECIMALVALUE: {
		  } break;
			
		  case API_GETSTRINGSIZE: {
		  } break;
			
		  case API_WRITETOLOGGER: {
		  } break;
			
		  case API_ENABLECRISOLSCRIPTWARNINGS: {
		  } break;
			
		  case API_DISABLECRISOLSCRIPTWARNINGS: {
		  } break;

		  case API_SHOWFPS: {
		  } break;

		  case API_WAIT: {
		  } break;
		}; // ~ switch	    
	  } break;
		
	  case EXP_GAMEOBJ_INVOKE: {
		SymTableParamsCheck(pExp->ExpGameObjInvoke.pParams, pSymTable, Check, szActFileName);	    
	    switch(pExp->ExpGameObjInvoke.ExpGameObjMethod) {
	  	  case GAMEOBJ_QUITGAME: {
		  } break;
		  
		  case GAMEOBJ_WRITETOCONSOLE: {
		  } break;
		  
		  case GAMEOBJ_ACTIVEADVICEDIALOG: {
		  } break;
		  
		  case GAMEOBJ_ACTIVEQUESTIONDIALOG: {
		  } break;
		  
		  case GAMEOBJ_ACTIVETEXTREADERDIALOG: {
		  } break;
		  
		  case GAMEOBJ_ADDOPTIONTOTEXTSELECTORDIALOG: {
		  } break;
		  
		  case GAMEOBJ_RESETOPTIONSINTEXTSELECTORDIALOG: {
		  } break;
		  
		  case GAMEOBJ_ACTIVETEXTSELECTORDIALOG: {
		  } break;
		  
		  case GAMEOBJ_PLAYMIDIMUSIC: {
		  } break;
		  
		  case GAMEOBJ_STOPMIDIMUSIC: {
		  } break;
		  
		  case GAMEOBJ_PLAYWAVAMBIENTSOUND: {
		  } break;
		  
		  case GAMEOBJ_STOPWAVAMBIENTSOUND: {
		  } break;
		  
		  case GAMEOBJ_ACTIVETRADEITEMSINTERFAZ: {
		  } break;
		  
		  case GAMEOBJ_ADDOPTIONTOCONVERSATORINTERFAZ: {
		  } break;
		  
		  case GAMEOBJ_RESETOPTIONSINCONVERSATORINTERFAZ: {
		  } break;
		  
		  case GAMEOBJ_ACTIVECONVERSATORINTERFAZ: {
		  } break;

		  case GAMEOBJ_DESACTIVECONVERSATORINTERFAZ: {
		  } break;
		  
		  case GAMEOBJ_GETOPTIONFROMCONVERSATORINTERFAZ: {
		  } break;
		  
		  case GAMEOBJ_SHOWPRESENTATION: {
		  } break;
		  
		  case GAMEOBJ_BEGINCUTSCENE: {
		  } break;
		  
		  case GAMEOBJ_ENDCUTSCENE: {
		  } break;

		  case GAMEOBJ_SETSCRIPT: {
		  } break;

		  case GAMEOBJ_ISKEYPRESSED: {
		  } break;
		}; // ~ switch	    
	  } break;
		
	  case EXP_WORLDOBJ_INVOKE: {
		SymTableParamsCheck(pExp->ExpWorldObjInvoke.pParams, pSymTable, Check, szActFileName);	    
		switch(pExp->ExpWorldObjInvoke.ExpWorldObjMethod) {
	  	  case WORLDOBJ_GETAREANAME: {
		  } break;

		  case WORLDOBJ_GETAREAID: {
		  } break;

		  case WORLDOBJ_GETAREAWIDTH: {
		  } break;

		  case WORLDOBJ_GETAREAHEIGHT: {
		  } break;

		  case WORLDOBJ_GETHOUR: {
		  } break;

		  case WORLDOBJ_GETMINUTE: {
		  } break;

		  case WORLDOBJ_SETHOUR: {
		  } break;

		  case WORLDOBJ_SETMINUTE: {
		  } break;

		  case WORLDOBJ_GETENTITY: {
		  } break;

		  case WORLDOBJ_GETPLAYER: {
		  } break;

		  case WORLDOBJ_ISFLOORVALID: {
		  } break;

		  case WORLDOBJ_GETITEMAT: {
		  } break;

		  case WORLDOBJ_GETNUMITEMSAT: {		
		  } break;

		  case WORLDOBJ_GETDISTANCE: {
		  } break;

		  case WORLDOBJ_CALCULEPATHLENGHT: {
		  } break;

		  case WORLDOBJ_LOADAREA: {
		  } break;

		  case WORLDOBJ_CHANGEENTITYLOCATION: {
		  } break;

		  case WORLDOBJ_ATTACHCAMERATOENTITY: {
		  } break;

		  case WORLDOBJ_ATTACHCAMERATOLOCATION: {
		  } break;

		  case WORLDOBJ_ISCOMBATMODEACTIVE: {
		  } break;

		  case WORLDOBJ_ENDCOMBAT: {
		  } break;

		  case WORLDOBJ_GETCRIATUREINCOMBATTURN: {
		  } break;
  
		  case WORLDOBJ_GETCOMBATANT: {
		  } break;

		  case WORLDOBJ_GETNUMBEROFCOMBATANTS: {
		  } break;

		  case WORLDOBJ_GETAREALIGHTMODEL: {
		  } break;

		  case WORLDOBJ_SETIDLESCRIPTTIME: {
		  } break;

		  case WORLDOBJ_SETSCRIPT: {
		  } break;

		  case WORLDOBJ_DESTROYENTITY: {
		  } break;
  
		  case WORLDOBJ_CREATECRIATURE: {
		  } break;
		  
		  case WORLDOBJ_CREATEWALL: {
		  } break;

		  case WORLDOBJ_CREATESCENARYOBJECT: {
		  } break;

		  case WORLDOBJ_CREATEITEMABANDONED: {
		  } break;
  
		  case WORLDOBJ_CREATEITEMWITHOWNER: {
		  } break;

		  case WORLDOBJ_SETWORLDTIMEPAUSE: {		
		  } break;

		  case WORLDOBJ_ISWORLDTIMEINPAUSE: {		
		  } break;

		  case WORLDOBJ_SETELEVATIONAT: {
		  } break;

		  case WORLDOBJ_GETELEVATIONAT: {
		  } break;

		  case WORLDOBJ_NEXTTURN: {
		  } break;

		  case WORLDOBJ_GETLIGHTAT: {
		  } break;

		  case WORLDOBJ_PLAYWAVSOUND: {
		  } break;

		  case WORLDOBJ_SETSCRIPTAT: {
		  } break;
		}; // ~ switch  	    
	  } break;
		
	  case EXP_ENTITYOBJ_INVOKE: {
		// El identificador de la entidad, debera de ser un identificador
		// previamente declarado 
		if (FIRST_CHECK == Check) {
		  // El identificador debe de ser una entidad declarada		  
		  sSymTableNode* pNode = SymTableGetNode(pSymTable,
												 pExp->ExpEntityObjInvoke.szIdentifier);
		  if (pNode) {
			// ¿Es un identificador de funcion?
			if (SYM_TYPE_FUNC == pNode->SymTableNodeType) {
			  ReportErrorf(szActFileName,
						   pExp->ExpEntityObjInvoke.szIdentifier,
						   pExp->unSrcLine,
						   "Error> Se esta intentando usar el identificador %s como función", 
						   pExp->ExpEntityObjInvoke.szIdentifier);
			}
		  } else {
			ReportErrorf(szActFileName,
			             pExp->ExpEntityObjInvoke.szIdentifier,
						 pExp->unSrcLine,
						 "Error> El identificador de entidad %s no se halla declarado", 
						 pExp->ExpEntityObjInvoke.szIdentifier);
		  }
		}

		// Se continua chequeando
		SymTableParamsCheck(pExp->ExpEntityObjInvoke.pParams, pSymTable, Check, szActFileName);
	    switch(pExp->ExpEntityObjInvoke.ExpEntityObjMethod) {
	  	  case ENTITYOBJ_GETNAME: {			  
		  } break;

		  case ENTITYOBJ_SETNAME: {			  
		  } break;
			
		  case ENTITYOBJ_GETENTITYTYPE: {
		  } break;
		  
		  case ENTITYOBJ_GETTYPE: {
		  } break;
			
		  case ENTITYOBJ_SAY: {
		  } break;
			
		  case ENTITYOBJ_SHUTUP: {
		  } break;
			
		  case ENTITYOBJ_ISSAYING: {
		  } break;
			
		  case ENTITYOBJ_ATTACHGFX: {
		  } break;
			
		  case ENTITYOBJ_RELEASEGFX: {
		  } break;
			
		  case ENTITYOBJ_RELEASEALLGFX: {
		  } break;
			
		  case ENTITYOBJ_ISGFXATTACHED: {
		  } break;
			
		  case ENTITYOBJ_GETNUMITEMSINCONTAINER: {
		  } break;
			
		  case ENTITYOBJ_ISITEMINCONTAINER: {
		  } break;

		  case ENTITYOBJ_GETITEMFROMCONTAINER: {
		  } break;				
			
		  case ENTITYOBJ_TRANSFERITEMTOCONTAINER: {
		  } break;
			
		  case ENTITYOBJ_INSERTITEMINCONTAINER: {
		  } break;
		  	
		  case ENTITYOBJ_REMOVEITEMOFCONTAINER: {
		  } break;
			
		  case ENTITYOBJ_SETANIMTEMPLATESTATE: {
		  } break;
			
		  case ENTITYOBJ_SETPORTRAITANIMTEMPLATESTATE: {
		  } break;
			
		  case ENTITYOBJ_SETIDLESCRIPTTIME: {
		  } break;

		  case ENTITYOBJ_SETLIGHT: {
		  } break;

		  case ENTITYOBJ_GETLIGHT: {
		  } break;
			
		  case ENTITYOBJ_GETXPOS: {
		  } break;
	
		  case ENTITYOBJ_GETYPOS: {
		  } break;

		  case ENTITYOBJ_GETELEVATION: {
		  } break;

		  case ENTITYOBJ_SETELEVATION: {
		  } break;

		  case ENTITYOBJ_GETLOCALATTRIBUTE: {
		  } break;

		  case ENTITYOBJ_SETLOCALATTRIBUTE: {
		  } break;

		  case ENTITYOBJ_GETOWNER: {
		  } break;
			
		  case ENTITYOBJ_GETCLASS: {
		  } break;

		  case ENTITYOBJ_GETINCOMBATUSECOST: {
		  } break;

		  case ENTITYOBJ_GETGLOBALATTRIBUTE: {
		  } break;

		  case ENTITYOBJ_SETGLOBALATTRIBUTE: {
		  } break;
			
		  case ENTITYOBJ_GETWALLORIENTATION: {
		  } break;
			
		  case ENTITYOBJ_BLOCKACCESS: {
		  } break;
			
		  case ENTITYOBJ_UNBLOCKACCESS: {
		  } break;
			
		  case ENTITYOBJ_ISACCESSBLOCKED: {
		  } break;
			
		  case ENTITYOBJ_SETSYMPTOM: {
		  } break;
			
		  case ENTITYOBJ_ISSYMPTOMACTIVE: {
		  } break;
			
		  case ENTITYOBJ_GETGENRE: {
		  } break;
			
		  case ENTITYOBJ_GETHEALTH: {
		  } break;
			
		  case ENTITYOBJ_SETHEALTH: {
		  } break;
			
		  case ENTITYOBJ_GETEXTENDEDATTRIBUTE: {
		  } break;
			
		  case ENTITYOBJ_SETEXTENDEDATTRIBUTE: {
		  } break;
			
		  case ENTITYOBJ_GETLEVEL: {
		  } break;
			
		  case ENTITYOBJ_SETLEVEL: {
		  } break;
			
		  case ENTITYOBJ_GETEXPERIENCE: {
		  } break;
			
		  case ENTITYOBJ_SETEXPERIENCE: {
		  } break;
			
		  case ENTITYOBJ_GETINCOMBATACTIONPOINTS: {
		  } break;
		  
		  case ENTITYOBJ_GETACTIONPOINTS: {
		  } break;
			
		  case ENTITYOBJ_SETACTIONPOINTS: {
		  } break;
		
		  case ENTITYOBJ_ISHABILITYACTIVE: {
		  } break;
			
		  case ENTITYOBJ_SETHABILITY: {
		  } break;
			
		  case ENTITYOBJ_USEHABILITY: {
		  } break;
			
		  case ENTITYOBJ_ISRUNMODEACTIVE: {
		  } break;
			
		  case ENTITYOBJ_SETRUNMODE: {
		  } break;
			
		  case ENTITYOBJ_MOVETO: {
		  } break;
			
		  case ENTITYOBJ_ISMOVING: {
		  } break;
			
		  case ENTITYOBJ_STOPMOVING: {
		  } break;
			
		  case ENTITYOBJ_EQUIPITEM: {
		  } break;
			
		  case ENTITYOBJ_REMOVEITEMEQUIPPED: {
		  } break;
			
		  case ENTITYOBJ_GETITEMEQUIPPED: {
		  } break;
			
		  case ENTITYOBJ_ISITEMEQUIPPED: {
		  } break;
			
		  case ENTITYOBJ_DROPITEM: {
		  } break;
			
		  case ENTITYOBJ_USEITEM: {
		  } break;
			
		  case ENTITYOBJ_MANIPULATE: {
		  } break;
			
		  case ENTITYOBJ_SETTRANSPARENTMODE: {
		  } break;
			
		  case ENTITYOBJ_ISTRANSPARENTMODEACTIVE: {
		  } break;
			
		  case ENTITYOBJ_CHANGEANIMORIENTATION: {
		  } break;

		  case ENTITYOBJ_GETANIMORIENTATION: {
		  } break;
			
		  case ENTITYOBJ_SETALINGMENT: {
		  } break;
			
		  case ENTITYOBJ_SETALINGMENTWITH: {
		  } break;
			
		  case ENTITYOBJ_SETALINGMENTAGAINST: {
		  } break;
			
		  case ENTITYOBJ_GETALINGMENT: {
		  } break;
			
		  case ENTITYOBJ_HITENTITY: {
		  } break;

		  case ENTITYOBJ_SETSCRIPT: {
		  } break;

		  case ENTITYOBJ_ISGHOSTMOVEMODEACTIVE: {
		  } break;
	  
		  case ENTITYOBJ_SETGHOSTMOVEMODE: {
		  } break;

		  case ENTITYOBJ_GETRANGE: {
		  } break;

		  case ENTITYOBJ_ISINRANGE: {
		  } break;
		}; // ~ switch	    
	  } break;

	   case EXP_CAST: {
		 SymTableTypeCheck(pExp->ExpCast.pType, pSymTable, Check, szActFileName);
		 SymTableExpCheck(pExp->ExpCast.pExp, pSymTable, Check, szActFileName);
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
// - pSymTable. Tabla de simbolos base.
// - Check. Tipo de chequeo a realizar.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
// - Funcion privada
///////////////////////////////////////////////////////////////////////////////
void 
SymTableParamsCheck(const sExp* pParam,
				    sSymbolTable* pSymTable,
					const eCheckType Check,
					sbyte* szActFileName)
{
  // ¿Es parametro valido?
  if (pParam) {
	const sExp* pIt = pParam;
	for (; pIt != NULL; pIt = pIt->pSigExp) {
	  SymTableExpCheck(pIt, pSymTable, Check, szActFileName);
	}  
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre un tipo
// Parametros:
// - pType. Tipo a imprimir
// - pSymTable. Tabla de simbolos base.
// - Check. Tipo de chequeo a realizar.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SymTableTypeCheck(const sType* pType,
				  sSymbolTable* pSymTable,
				  const eCheckType Check,
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
// - A la hora de insertar el identificador en la tabla de simbolos, este se
//   insertara en la tabla de simbolos previa que seguro pertenece a un
//   script o bien a un espacio import.
// - La tabla de simbolos auxiliar, pAuxSymTable, sera la tabla del simbolos
//   del script cuando se este procesando un fichero de funciones globales. 
//   En tal caso, ademas de declarar el identificador de funcion en la 
//   tabla de simbolos pPrevSymTable (que correspondera al espacio Import)
//   lo declararemos en pAuxSymTable permitiendo que dicho simbolo sea
//   sea reconocido en el ambito local del script. 
//   Cuando el pAuxSymTable sea NULL querra decir que se esta ordenando
//   introducir desde el script y tal caso, pPrevSymTable sera la tabla 
//   de simbolos del script y no hara falta insertar en ninguna otra 
//   auxiliar.
// Parametros:
// - pFunc. Funcion a imprimir.
// - pPrevSymTable. Tabla de simbolos previa
// - pAuxSymTable. Tabla de simbolos auxiliar.
// - Check. Tipo de chequeo a realizar.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SymTableFuncCheck(sFunc* pFunc,
				  sSymbolTable* pPrevSymTable,
				  sSymbolTable* pAuxSymTable,
				  const eCheckType Check,
				  sbyte* szActFileName)
{
  // ¿Hay funcion?
  if (pFunc) {
	switch (pFunc->eFuncType) {
	  case FUNC_SEQ: {
		SymTableFuncCheck(pFunc->FuncSeq.pFirst, 
						  pPrevSymTable, 
						  pAuxSymTable, 
						  Check, 
						  szActFileName);
		SymTableFuncCheck(pFunc->FuncSeq.pSecond, 
						  pPrevSymTable, 
						  pAuxSymTable, 
						  Check, 
						  szActFileName);
	  } break;

	  case FUNC_DECL: {
		if (FIRST_CHECK == Check) {
		  // Se comprueba si el identificador esta duplicado
		  if (SymTableIsPresent(pPrevSymTable, pFunc->FuncDecl.szIdentifier)) {
			ReportErrorf(szActFileName, 
						 pFunc->FuncDecl.szIdentifier,
						 pFunc->unSrcLine,
						 "Error> Identificador duplicado");
	
		  } else if (pAuxSymTable &&
					 SymTableIsPresent(pAuxSymTable, pFunc->FuncDecl.szIdentifier)) {
			ReportErrorf(szActFileName, 
						 pFunc->FuncDecl.szIdentifier,
						 pFunc->unSrcLine,
						 "Error> Identificador duplicado");	
		  }

		  pFunc->FuncDecl.pSymTable = SymTableCreate();
		  pFunc->FuncDecl.pSymTable->pParent = pPrevSymTable;		  
		  SymTableInsertFunc(pPrevSymTable,
							 pFunc->FuncDecl.szIdentifier,
							 pFunc);
		  if (pAuxSymTable) {
			SymTableInsertFunc(pAuxSymTable,
							   pFunc->FuncDecl.szIdentifier,
							   pFunc);		  
		  }
		}
		SymTableTypeCheck(pFunc->FuncDecl.pType,
						  pFunc->FuncDecl.pSymTable,
						  Check, 
						  szActFileName);
		SymTableArgumentCheck(pFunc->FuncDecl.pArguments,
							  pFunc->FuncDecl.pSymTable,
							  Check, 
							  szActFileName);
  		SymTableConstCheck(pFunc->FuncDecl.pConst,
						   pFunc->FuncDecl.pSymTable,
						   Check, 
						   szActFileName);		
		SymTableVarCheck(pFunc->FuncDecl.pVar,
		                 NULL,
						 pFunc->FuncDecl.pSymTable,
						 Check, 
						 szActFileName);				
		SymTableStmCheck(pFunc->FuncDecl.pStm,
						 pFunc->FuncDecl.pSymTable,
						 Check, 
						 szActFileName);			
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
// - pSymTable. Tabla de simbolos base.
// - Check. Tipo de chequeo a realizar.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SymTableArgumentCheck(sArgument* pArgument,
					  sSymbolTable* pSymTable,
					  const eCheckType Check,
					  sbyte* szActFileName)
{
  // ¿Hay argumentos para imprimir?
  if (pArgument) {
	sArgument* pIt = pArgument;
	for (; pIt != NULL; pIt = pIt->pSigArgument) {
	  SymTableTypeCheck(pIt->pType, pSymTable, Check, szActFileName);
	  if (FIRST_CHECK == Check) {			
		// ¿Identificador ya declarado?
		if (SymTableIsPresent(pSymTable, pIt->szIdentifier)) {
		  // Si, luego hay error
		  ReportErrorf(szActFileName, 
					   pIt->szIdentifier,
					   pIt->unSrcLine,
					   "Error> Identificador duplicado");
		} else {
		  // No, luego se inserta en tabla de simbolos
		  SymTableInsertArgument(pSymTable, pIt->szIdentifier, pIt);
		}
	  }	
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea los argumentos en la declaracion de una funcion de usuario y los
//   parametros que esta ha recibido. Si halla un argumento de tipo referencia,
//   el parametro recibido DEBERA de ser un identificador de variable. 
// Parametros:
// - pArguments. Argumentos de la funcion a chequear.
// - pParams. Parametros pasados a la funcion.
// - pSymTable. Tabla de simbolos.
// - szActFileName. Nombre del archivo actual.
// Devuelve:
// Notas:
// - Esta funcion sera llamada en el segundo pase por la invocacion de 
//   funciones de usuario.
// - El numero de argumentos y de parametros debera de ser el mismo y los
//   identificadores ya alojados en la tabla de simbolos. Esto es asi porque
//   se espera que antes de la llamada a esta funcion se haya realizado un
//   chequeo primero en los argumentos y parametros.
///////////////////////////////////////////////////////////////////////////////
void
CheckParamsForRefArgs(sArgument* pArguments,
					  sExp* pParams,
					  sSymbolTable* pSymTable,
					  sbyte* szActFileName)
{
  // Comienza a recorrer los argumentos y parametros
  while (pArguments &&
		 pParams) {
	// ¿Argumento por referencia?
	if (ARGUMENT_REF == pArguments->ArgumentPassType) {
	  // Si, ¿parametro identificador?
	  if (EXP_IDENTIFIER == pParams->ExpType) {
		// Si, ¿NO es identificador de variable?
		// Nota: Recordar que los argumentos de una funcion tambien son vbles.
		sSymTableNode* pNode = SymTableGetNode(pSymTable, 
											   pParams->ExpIdentifier.szIdentifier);
		assert(pNode);
		if (SYM_TYPE_VAR != pNode->SymTableNodeType &&
			SYM_TYPE_ARG != pNode->SymTableNodeType) {
		  ReportErrorf(szActFileName, 
					   "(...)",
					   pParams->unSrcLine,
					   "Error> El argumento es ref y se ha de pasar una variable");	
		}
	  } else {
		ReportErrorf(szActFileName, 
					 "(...)",
					 pParams->unSrcLine,
					 "Error> El argumento es ref y se ha de pasar una variable");	
	  }
	}

	// Sig
	pArguments = pArguments->pSigArgument;
	pParams = pParams->pSigExp;
  }
  assert(!pArguments && !pParams);
}
