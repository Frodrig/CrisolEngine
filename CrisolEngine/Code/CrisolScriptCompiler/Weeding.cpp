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
// ASTWeeding.cpp
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Notas:
// - Consultar Weeding.h para mas informacion
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "Weeding.h"
#include <stdio.h>
#include "Error.h"
#include "memory.h"
#include <assert.h>
#include <string.h>

// Funciones de utilidad
static sStm* InsertDefaultReturnInScriptDecl(sStm* pStm);

// Definicion de funciones privadas / Recorrido por el AST
static void WeedConst(sConst* pConst,
					  sbyte* szActFileName);
static void WeedVar(sVar* pVar,
					sType* pIdentDeclSeqType,
					sbyte* szActFileName);
static void WeedScript(sScript* pScript,
					   sbyte* szActFileName);
static void WeedScriptType(sScriptType* pScriptType,
						   sbyte* szActFileName);
static void WeedImport(sImport* pImport,
					   sbyte* szActFileName);
static void WeedStm(sStm* pStm,
					sbyte* szActFileName);
static void WeedExp(sExp* pExp,
					sbyte* szActFileName);
static void WeedType(sType* pType,
					 sbyte* szActFileName);
static void WeedFunc(sFunc* pFunc,
					 sbyte* szActFileName);
static void WeedArgument(sArgument* pArgument,
						 sbyte* szActFileName);
static void WeedParams(sExp* pParam,
					   sbyte* szActFileName);
static sword WeedStmReturn(sStm* pStm,
						 sword* pnWeedResultLine,
						 sbyte* szActFileName);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicia el recorrido por el arbol AST.
// Parametros:
// - pGlobal. Direccion a la estructura global.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
WeedGlobal(sGlobal* pGlobal)
{
  // ¿Hay datos?
  if (pGlobal) {
	// Recorrido
	WeedConst(pGlobal->pConst, pGlobal->szFileName);
	WeedVar(pGlobal->pVar, NULL, pGlobal->szFileName);
	WeedStm(pGlobal->pReturnStm, pGlobal->szFileName);
	WeedScript(pGlobal->pScript, pGlobal->szFileName);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre declaraciones const.
// Parametros:
// - pConst. Declaraciones const
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
WeedConst(sConst* pConst,
		  sbyte* szActFileName)
{
  // ¿Hay declaraciones?
  if (pConst) {
	switch(pConst->ConstType) {
	  case CONST_DECLSEQ: {
		WeedConst(pConst->ConstSeq.pFirst, szActFileName);
		WeedConst(pConst->ConstSeq.pSecond, szActFileName);
	  } break;

	  case CONST_DECL: {
		WeedType(pConst->ConstDecl.pType, szActFileName);
		WeedExp(pConst->ConstDecl.pExp, szActFileName);		
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre delcaracion de variables.
// - En caso de que las vbles no tengan inicializacion por defecto, se les
//   asociara segun sea su tipo.
// Parametros:
// - pVar. Declaracion de variables
// - pVarInitType. Tipo asociado a la secuencia de identificadors. Solo se
//   pasara cuando se este en VAR_TYPEDECL (desde el exterior NULL).
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
WeedVar(sVar* pVar,
		sType* pIdentDeclSeqType,
		sbyte* szActFileName)
{
  // ¿Hay variables?
  if (pVar) {
	switch(pVar->VarType) {
	  case VAR_TYPEDECL_SEQ: {
		WeedVar(pVar->VarTypeDeclSeq.pFirst, pIdentDeclSeqType, szActFileName);
		WeedVar(pVar->VarTypeDeclSeq.pSecond, pIdentDeclSeqType, szActFileName);
	  } break;

	  case VAR_TYPEDECL: {
		WeedType(pVar->VarTypeDecl.pType, szActFileName);
		WeedVar(pVar->VarTypeDecl.pVarIdentSeq, pVar->VarTypeDecl.pType, szActFileName);		
	  } break;

	  case VAR_IDENTIFIERDECL_SEQ: {
		sVar* pIt = pVar;
		for (; pIt != NULL; pIt = pIt->VarIdentDeclSeq.pSigIdent) {		  
		  // ¿Existe inicializacion?
		  if (pIt->VarIdentDeclSeq.pValue) {
			// Si, se recorre
			WeedExp(pIt->VarIdentDeclSeq.pValue, szActFileName);			
		  }	else {
			// No existe, luego habra inicializacion por defecto segun el tipo
			assert(pIdentDeclSeqType);
			switch(pIdentDeclSeqType->ValueType) {
			  case TYPE_NUMBER: {
				pIt->VarIdentDeclSeq.pValue = MakeExpNumberValue(0.0f);
			  } break;

			  case TYPE_STRING: {
				sbyte* szEmpty = Mem_Alloc(2);
				strcpy(szEmpty, "");				
				pIt->VarIdentDeclSeq.pValue = MakeExpStringValue(szEmpty);
			  } break;

			  case TYPE_ENTITY: {
				pIt->VarIdentDeclSeq.pValue = MakeExpGlobalConstEntity(0);
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
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
WeedScript(sScript* pScript,
		   sbyte* szActFileName)
{
  // ¿Hay script a imprimir?
  if (pScript) {
	switch(pScript->ScriptType) {
	  case SCRIPT_SEQ: {
		WeedScript(pScript->ScriptSeq.pFirst, szActFileName);
		WeedScript(pScript->ScriptSeq.pSecond, szActFileName);
	  } break;

	  case SCRIPT_DECL: {
		// Vbles
		sword nWeedResult;     // Resultado weeding
		sword nWeedResultLine; // Pos. donde esta la indicacion Weed
		
		szActFileName = pScript->ScriptDecl.szFileName;

		// Recorrido
		WeedScriptType(pScript->ScriptDecl.pType, szActFileName);		
		WeedImport(pScript->ScriptDecl.pImport, szActFileName);		
	    WeedConst(pScript->ScriptDecl.pConst, szActFileName);
		WeedVar(pScript->ScriptDecl.pVar, NULL, szActFileName);
		WeedFunc(pScript->ScriptDecl.pFunc, szActFileName);
		WeedStm(pScript->ScriptDecl.pStm, szActFileName);		

		// Se comprueba el tipo de retorno
		nWeedResultLine = pScript->unSrcLine;
		nWeedResult = WeedStmReturn(pScript->ScriptDecl.pStm,
									&nWeedResultLine, 
									szActFileName);

		// ¿NO hay un return asociado?
		if (0 == nWeedResult) {
		  // Se inserta el return por defecto
		  pScript->ScriptDecl.pStm = InsertDefaultReturnInScriptDecl(pScript->ScriptDecl.pStm);
		} else if (2 == nWeedResult) {
		  // No hay expresion asociada al return
		  ReportErrorf(szActFileName, 
					   "script", 
					   nWeedResultLine,
					   "Error> El return tener ser de >= a 1 para accion por defecto y < 1 en otro caso.");
		}
	  } break;	
	}; // ~ switch	
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este metodo recursivo sirve para insertar una sentencia return true en 
//   todos aquellos scripts que no terminen con un retorno (false o true).
// - SE SUPONDRA que el pStm que se envia no posee un return al finalizar
//   (se supone que no se ejecutara este metodo si existe un return 
//    completo o incompleto).
// Parametros:
// - pScriptType. Tipos de scripts
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
sStm*
InsertDefaultReturnInScriptDecl(sStm* pStm)
{
  // Casos
  if (!pStm) {
	// Sin enlace, luego se crea directamente el return
	return MakeStmReturn(MakeExpGlobalConstNumber(1));
  } else if (STM_SEQ != pStm->StmType) {
	// El ultimo enlace de la posible secuencia, luego se crea un
	// nuevo nodo de tipo seq que contenga a pStm y al return
	assert(pStm->StmType != STM_RETURN);
	return MakeStmSeq(pStm, MakeStmReturn(MakeExpGlobalConstNumber(1)));
  } else {
	// Se sigue realizando llamada recursiva, se retornara en este punto
	// la propia sentencia
	pStm->StmSeq.pSecond = InsertDefaultReturnInScriptDecl(pStm->StmSeq.pSecond);
	return pStm;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre los tipos de scripts
// Parametros:
// - pScriptType. Tipos de scripts
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
WeedScriptType(sScriptType* pScriptType,
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

	WeedArgument(pScriptType->pArguments, szActFileName);
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
WeedImport(sImport* pImport,
		   sbyte* szActFileName)
{
  // ¿Hay import?
  if (pImport) {
	switch(pImport->ImportType) {
	  case IMPORT_SEQ: {
		WeedImport(pImport->ImportSeq.pFirst, szActFileName);
		WeedImport(pImport->ImportSeq.pSecond, szActFileName);
	  } break;

	  case IMPORT_FUNC: {
		sbyte* szPrevFileName = szActFileName;
		szActFileName = pImport->ImportFunc.szFileName;
		WeedFunc(pImport->ImportFunc.pFunctions, szActFileName);
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
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
WeedStm(sStm* pStm,
		sbyte* szActFileName)
{
  // ¿Hay sentencia?
  if (pStm) { 
	switch(pStm->StmType) {
	  case STM_SEQ: {
		// Recorrido
		WeedStm(pStm->StmSeq.pFirst, szActFileName);		
		WeedStm(pStm->StmSeq.pSecond, szActFileName);		
	  } break;

	  case STM_RETURN: {
		if (pStm->StmReturn.pExp) {
		  WeedExp(pStm->StmReturn.pExp, szActFileName);
		}
	  } break;
  
	  case STM_IF: {
		WeedExp(pStm->If.pExp, szActFileName);
		WeedStm(pStm->If.pThenStm, szActFileName);
	  } break;

	  case STM_IFELSE: {
		WeedExp(pStm->IfElse.pExp, szActFileName);
		WeedStm(pStm->IfElse.pThenStm, szActFileName);
		WeedStm(pStm->IfElse.pElseStm, szActFileName);
	  } break;

	  case STM_WHILE: {
		WeedExp(pStm->While.pExp, szActFileName);
		WeedStm(pStm->While.pDoStm, szActFileName);
	  } break;

	  case STM_EXP: {
		if (pStm->Exp.pExp) {
		  WeedExp(pStm->Exp.pExp, szActFileName);
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
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
WeedExp(sExp* pExp,
		sbyte* szActFileName)
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
	    WeedExp(pExp->ExpAssing.pRightExp, szActFileName);		  
	  } break;

	  case EXP_EQUAL: {
	    WeedExp(pExp->ExpEqual.pLeftExp, szActFileName);
	    WeedExp(pExp->ExpEqual.pRightExp, szActFileName);
	  } break;

	  case EXP_NOEQUAL: {
	    WeedExp(pExp->ExpNoEqual.pLeftExp, szActFileName);
	    WeedExp(pExp->ExpNoEqual.pRightExp, szActFileName);
	  } break;
		
	  case EXP_LESS: {
	    WeedExp(pExp->ExpLess.pLeftExp, szActFileName);
	    WeedExp(pExp->ExpLess.pRightExp, szActFileName);
	  } break;
		
	  case EXP_LESSEQUAL: {
	    WeedExp(pExp->ExpLEqual.pLeftExp, szActFileName);
	    WeedExp(pExp->ExpLEqual.pRightExp, szActFileName);
	  } break;
		
	  case EXP_GREATER: {
	    WeedExp(pExp->ExpGreater.pLeftExp, szActFileName);
	    WeedExp(pExp->ExpGreater.pRightExp, szActFileName);
	  } break;
		
	  case EXP_GREATEREQUAL: {
	    WeedExp(pExp->ExpGEqual.pLeftExp, szActFileName);
	    WeedExp(pExp->ExpGEqual.pRightExp, szActFileName);
	  } break;
		
	  case EXP_ADD: {
	    WeedExp(pExp->ExpAdd.pLeftExp, szActFileName);
	    WeedExp(pExp->ExpAdd.pRightExp, szActFileName);
	  } break;
		
	  case EXP_MINUS: {
	    WeedExp(pExp->ExpMinus.pLeftExp, szActFileName);
	    WeedExp(pExp->ExpMinus.pRightExp, szActFileName);
	  } break;
		
	  case EXP_MULT: {
	    WeedExp(pExp->ExpMult.pLeftExp, szActFileName);
	    WeedExp(pExp->ExpMult.pRightExp, szActFileName);
	  } break;
		
	  case EXP_DIV: {
		// Si la expresion a la derecha es constante numerica, se comprobara
		// que sea distinta de 0
	    WeedExp(pExp->ExpDiv.pLeftExp, szActFileName);		
		if (EXP_NUMBER_VALUE == pExp->ExpDiv.pRightExp->ExpType &&
		    0.0f == pExp->ExpDiv.pRightExp->ExpNumValue.fNumValue) {
		  ReportErrorf(szActFileName,
					   "0",
					   pExp->ExpDiv.pRightExp->unSrcLine,
					   "Error> Division por 0");
		}
	    WeedExp(pExp->ExpDiv.pRightExp, szActFileName);
	  } break;
		
	  case EXP_MODULO: {
	    WeedExp(pExp->ExpModulo.pLeftExp, szActFileName);
	    WeedExp(pExp->ExpModulo.pRightExp, szActFileName);
	  } break;
		
	  case EXP_UMINUS: {
	    WeedExp(pExp->ExpUMinus.pExp, szActFileName);
	  } break;
		
	  case EXP_NOT: {
	    WeedExp(pExp->ExpNot.pExp, szActFileName);
	  } break;
		
	  case EXP_AND: {
	    WeedExp(pExp->ExpAnd.pLeftExp, szActFileName);
	    WeedExp(pExp->ExpAnd.pRightExp, szActFileName);
	  } break;
		
	  case EXP_OR: {
	    WeedExp(pExp->ExpOr.pLeftExp, szActFileName);
	    WeedExp(pExp->ExpOr.pRightExp, szActFileName);
	  } break;
		
	  case EXP_FUNC_INVOKE: {
	    WeedParams(pExp->ExpFuncInvoke.pParams, szActFileName);		  
	  } break;
		
	  case EXP_APIFUNC_INVOKE: {
		WeedParams(pExp->ExpAPIFuncInvoke.pParams, szActFileName);
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
		WeedParams(pExp->ExpGameObjInvoke.pParams, szActFileName);	    
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
		WeedParams(pExp->ExpWorldObjInvoke.pParams, szActFileName);	    
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
		WeedParams(pExp->ExpEntityObjInvoke.pParams, szActFileName);	    
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
			
		  case ENTITYOBJ_TRANSFERITEMTOCONTAINER: {
		  } break;
			
		  case ENTITYOBJ_INSERTITEMINCONTAINER: {
		  } break;

		  case ENTITYOBJ_GETITEMFROMCONTAINER: {
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
		WeedType(pExp->ExpCast.pType, szActFileName);
		WeedExp(pExp->ExpCast.pExp, szActFileName);
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
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
// - Funcion privada
///////////////////////////////////////////////////////////////////////////////
void 
WeedParams(sExp* pParam,
		   sbyte* szActFileName)
{
  // ¿Es parametro valido?
  if (pParam) {
	sExp* pIt = pParam;
	for (; pIt != NULL; pIt = pIt->pSigExp) {
	  WeedExp(pIt, szActFileName);
	}  
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre un tipo
// Parametros:
// - pType. Tipo a imprimir
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
WeedType(sType* pType,
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
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
WeedFunc(sFunc* pFunc,
		 sbyte* szActFileName)
{
  // ¿Hay funcion?
  if (pFunc) {
	switch (pFunc->eFuncType) {
	  case FUNC_SEQ: {
		WeedFunc(pFunc->FuncSeq.pFirst, szActFileName);
		WeedFunc(pFunc->FuncSeq.pSecond, szActFileName);
	  } break;

	  case FUNC_DECL: {
		// Vbles
		sword nWeedResult;     // Resultado weeding
		sword nWeedResultLine; // Pos. donde esta la indicacion Weed

		// Recorrido
		WeedType(pFunc->FuncDecl.pType, szActFileName);
		WeedArgument(pFunc->FuncDecl.pArguments, szActFileName);
		WeedConst(pFunc->FuncDecl.pConst, szActFileName);
		WeedVar(pFunc->FuncDecl.pVar, NULL, szActFileName);
		WeedStm(pFunc->FuncDecl.pStm, szActFileName);	
		
		// Se comprueba el tipo de retorno
		nWeedResultLine = pFunc->unSrcLine;
		nWeedResult = WeedStmReturn(pFunc->FuncDecl.pStm,
									&nWeedResultLine, 
									szActFileName);
		if ((0 == nWeedResult || 2 == nWeedResult) &&
			TYPE_VOID != pFunc->FuncDecl.pType->ValueType) {
		  ReportErrorf(szActFileName, 
					   pFunc->FuncDecl.szIdentifier, 
					   nWeedResultLine,
					   "Error> La funcion debe de retornar un valor.");
		} else if (1 == nWeedResult &&
			       TYPE_VOID == pFunc->FuncDecl.pType->ValueType) {
		  ReportErrorf(szActFileName, 
					   pFunc->FuncDecl.szIdentifier, 
					   nWeedResultLine,
					   "Error> La funcion no puede retornar ningun valor.");
		}
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre argumentos
// Parametros:
// - pArgument. Argumentos a imprimir
// - szActFileName. Nombre del fichero actual
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
WeedArgument(sArgument* pArgument,
			 sbyte* szActFileName)
{
  // ¿Hay argumentos para imprimir?
  if (pArgument) {
	sArgument* pIt = pArgument;
	for (; pIt != NULL; pIt = pIt->pSigArgument) {
	  WeedType(pIt->pType, szActFileName);
	}	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de comprobar si un script o funcion retorna
// Parametros:
// - pArgument. Argumentos a imprimir
// - pnWeedResultLine. Direccion donde localizar la linea en donde se halla
//   el ultimo result result.
// - szActFileName. Nombre del fichero actual
// Devuelve:
// - 1 si hay una expresion asociada a un return.
// - 2 si NO hay una expresion asociada a un return.
// - 0 si NO hay un return.
// Notas:
///////////////////////////////////////////////////////////////////////////////
sword 
WeedStmReturn(sStm* pStm,
			  sword* pnWeedResultLine,
			  sbyte* szActFileName)
{
  // ¿Hay sentencia?
  if (pStm) {
	switch(pStm->StmType) {
	  case STM_RETURN: {
		*pnWeedResultLine = pStm->StmReturn.pExp->unSrcLine;
		return (pStm->StmReturn.pExp) ? 1 : 2;		

	  } break;

	  case STM_IFELSE: {
		// En un if/else debera de haber retorno por ambos lados
		return (WeedStmReturn(pStm->IfElse.pThenStm, pnWeedResultLine, szActFileName) &&
		        WeedStmReturn(pStm->IfElse.pElseStm, pnWeedResultLine, szActFileName));
	  } break;

	  case STM_SEQ: {
		// Aqui se comprobara si hay un retorno en la primera secuencia y
		// en tal caso, se avisara reportando una advertencia
		sword nResult = WeedStmReturn(pStm->StmSeq.pFirst, pnWeedResultLine, szActFileName);
		if (nResult) {
		  if (pStm->StmSeq.pSecond) {
			ReportWarningf(szActFileName, pStm->StmSeq.pSecond->unSrcLine, "Advertencia> Codigo inalcanzable");
		  }
		  return nResult;
		} else {
		  return WeedStmReturn(pStm->StmSeq.pSecond, pnWeedResultLine, szActFileName);
		}
	  } break;

	  default: {
		return 0;
	  } break;
	}; // ~ switch
  }

  return 0;
}