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
// ASTPrinting.cpp
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Notas:
// - Consultar ASTPrinting.h para mas informacion
////////////////////////////////////////////////////////////////////////////////
// Includes
#include "ASTPrinting.h"
#include <stdio.h>

// Definicion de funciones privadas
static void ASTPrintConst(const sConst* pConst,
						  FILE* pOutputFile);
static void ASTPrintVar(const sVar* pVar,
						FILE* pOutputFile);
static void ASTPrintScript(const sScript* pScript,
						   FILE* pOutputFile);
static void ASTPrintScriptType(const sScriptType* pScriptType,
							   FILE* pOutputFile);
static void ASTPrintImport(const sImport* pImport,
						   FILE* pOutputFile);
static void ASTPrintStm(const sStm* pStm,
						FILE* pOutputFile);
static void ASTPrintExp(const sExp* pExp,
						FILE* pOutputFile);
static void ASTPrintType(const sType* pType,
						 FILE* pOutputFile);
static void ASTPrintFunc(const sFunc* pFunc,
						 FILE* pOutputFile);
static void ASTPrintArgument(const sArgument* pArgument,
							 FILE* pOutputFile);
static void ASTPrintParams(sExp* pParam,
						   FILE* pOutputFile);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicia el recorrido por el arbol AST, abriendo ademas el fichero de
//   salida.
// Parametros:
// - pGlobal. Direccion a la estructura global.
// - szOutputFileName. Nombre del archivo de salida.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ASTPrintGlobal(const sGlobal* pGlobal,
			   const sbyte* szOutputFileName)
{
  // ¿Hay datos?
  if (pGlobal) {
	// Se abre el fichero de salida
	FILE* pOutputFile = fopen(szOutputFileName, "w");
	if (pOutputFile) {
	  // Se procede a recorrer el AST
	  fprintf(pOutputFile, "Reproduccion del AST\n\n");
	  fprintf(pOutputFile, "Global\n");
	  if (pGlobal->pConst) {
		fprintf(pOutputFile, "const\n");		
		ASTPrintConst(pGlobal->pConst, pOutputFile);
	  }
	  
	  if (pGlobal->pVar) {
		fprintf(pOutputFile, "var\n");		
		ASTPrintVar(pGlobal->pVar, pOutputFile);
	  }	  

	  ASTPrintScript(pGlobal->pScript, pOutputFile);

	  // Se cierra archivo
	  fclose(pOutputFile);
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Imprime declaraciones const.
// Parametros:
// - pConst. Declaraciones const
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ASTPrintConst(const sConst* pConst,
			  FILE* pOutputFile)
{
  // ¿Hay declaraciones?
  if (pConst) {
	switch(pConst->ConstType) {
	  case CONST_DECLSEQ: {
		ASTPrintConst(pConst->ConstSeq.pFirst, pOutputFile);
		ASTPrintConst(pConst->ConstSeq.pSecond, pOutputFile);
	  } break;

	  case CONST_DECL: {
		ASTPrintType(pConst->ConstDecl.pType, pOutputFile);
		fprintf(pOutputFile, pConst->ConstDecl.szIdentifier);
		fprintf(pOutputFile, ":= ");
		ASTPrintExp(pConst->ConstDecl.pExp, pOutputFile);
		fprintf(pOutputFile, ";\n");
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Imprime delcaracion de variables.
// Parametros:
// - pVar. Declaracion de variables
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ASTPrintVar(const sVar* pVar,
			FILE* pOutputFile)
{
  // ¿Hay variables?
  if (pVar) {
	switch(pVar->VarType) {
	  case VAR_TYPEDECL_SEQ: {
		ASTPrintVar(pVar->VarTypeDeclSeq.pFirst, pOutputFile);
		ASTPrintVar(pVar->VarTypeDeclSeq.pSecond, pOutputFile);
	  } break;

	  case VAR_TYPEDECL: {
		ASTPrintType(pVar->VarTypeDecl.pType, pOutputFile);
		ASTPrintVar(pVar->VarTypeDecl.pVarIdentSeq, pOutputFile);		
	  } break;

	  case VAR_IDENTIFIERDECL_SEQ: {
		const sVar* pIt = pVar;
		for (; pIt; pIt = pIt->VarIdentDeclSeq.pSigIdent) {		  
		  fprintf(pOutputFile, "%s", pIt->VarIdentDeclSeq.szIdentifier, pOutputFile);
		  if (pIt->VarIdentDeclSeq.pValue) {
			fprintf(pOutputFile, ":= ");
			ASTPrintExp(pIt->VarIdentDeclSeq.pValue, pOutputFile);
		  }		  
		  if (pIt->VarIdentDeclSeq.pSigIdent) {
			fprintf(pOutputFile, ", ");
		  }
		}
		fprintf(pOutputFile, ";\n");
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Imprime un script.
// Parametros:
// - pScript. Script a imprimir
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ASTPrintScript(const sScript* pScript,
			   FILE* pOutputFile)
{
  // ¿Hay script a imprimir?
  if (pScript) {
	switch(pScript->ScriptType) {
	  case SCRIPT_SEQ: {
		ASTPrintScript(pScript->ScriptSeq.pFirst, pOutputFile);
		ASTPrintScript(pScript->ScriptSeq.pSecond, pOutputFile);
	  } break;

	  case SCRIPT_DECL: {
		fprintf(pOutputFile, "<SCRIPT> \n");
		fprintf(pOutputFile, "script ");
		ASTPrintScriptType(pScript->ScriptDecl.pType, pOutputFile);
		
		ASTPrintImport(pScript->ScriptDecl.pImport, pOutputFile);		
		if (pScript->ScriptDecl.pConst) {
		  fprintf(pOutputFile, "const\n");		
		  ASTPrintConst(pScript->ScriptDecl.pConst, pOutputFile);
		}

		if (pScript->ScriptDecl.pVar) {
		  fprintf(pOutputFile, "var\n");		
		  ASTPrintVar(pScript->ScriptDecl.pVar, pOutputFile);
		}
		
		if (pScript->ScriptDecl.pFunc) {
		  fprintf(pOutputFile, "func\n");		
		  ASTPrintFunc(pScript->ScriptDecl.pFunc, pOutputFile);
		}
				
		fprintf(pOutputFile, "begin\n");		
		ASTPrintStm(pScript->ScriptDecl.pStm, pOutputFile);		
		fprintf(pOutputFile, "end\n");		
		fprintf(pOutputFile, "<\\SCRIPT> \n");
	  } break;	
	}; // ~ switch	
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Imprime los tipos de scripts
// Parametros:
// - pScriptType. Tipos de scripts
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ASTPrintScriptType(const sScriptType* pScriptType,
				   FILE* pOutputFile)
{
  // ¿Hay tipo?
  if (pScriptType) {
	switch(pScriptType->ScriptEventType) {
	  case SCRIPTEVENT_ONSTARTGAME: {
		fprintf(pOutputFile, "OnStartGame ");
	  } break;

	  case SCRIPTEVENT_ONCLICKHOURPANEL: {
		fprintf(pOutputFile, "OnClickHourPanel ");
	  } break;

	  case SCRIPTEVENT_ONFLEECOMBAT: {
		fprintf(pOutputFile, "OnFleeCombat ");
	  } break;

	  case SCRIPTEVENT_ONKEYPRESSED: {
		fprintf(pOutputFile, "OnKeyPressed ");
	  } break;

	  case SCRIPTEVENT_ONSTARTCOMBATMODE: {
		fprintf(pOutputFile, "OnStartCombatMode ");
	  } break;

	  case SCRIPTEVENT_ONENDCOMBATMODE: {
		fprintf(pOutputFile, "OnEndCombatMode ");
	  } break;

	  case SCRIPTEVENT_ONNEWHOUR: {
		fprintf(pOutputFile, "OnNewHour ");
	  } break;

	  case SCRIPTEVENT_ONENTERINAREA: {
		fprintf(pOutputFile, "OnEnterInArea ");
	  } break;

	  case SCRIPTEVENT_ONWORLDIDLE: {
		fprintf(pOutputFile, "OnWorldIdle ");
	  } break;

	  case SCRIPTEVENT_ONSETINFLOOR: {
		fprintf(pOutputFile, "OnSetInFloor ");
	  } break;

	  case SCRIPTEVENT_ONSETOUTOFFLOOR: {
		fprintf(pOutputFile, "OnSetOutFloor ");
	  } break;

	  case SCRIPTEVENT_ONGETITEM: {
		fprintf(pOutputFile, "OnGetItem ");
	  } break;

	  case SCRIPTEVENT_ONDROPITEM: {
		fprintf(pOutputFile, "OnDropItem ");
	  } break;

	  case SCRIPTEVENT_ONOBSERVEENTITY: {
		fprintf(pOutputFile, "OnObserveEntity ");
	  } break;

	  case SCRIPTEVENT_ONTALKTOENTITY: {
		fprintf(pOutputFile, "OnTalkToEntity ");
	  } break;

	  case SCRIPTEVENT_ONMANIPULATEENTITY: {
		fprintf(pOutputFile, "OnManipulateEntity ");
	  } break;

	  case SCRIPTEVENT_ONDEATH: {
		fprintf(pOutputFile, "OnDeath ");
	  } break;

	  case SCRIPTEVENT_ONRESURRECT: {
		fprintf(pOutputFile, "OnResurrect ");
	  } break;

	  case SCRIPTEVENT_ONINSERTINEQUIPMENTSLOT: {
		fprintf(pOutputFile, "OnInsertInEquipmentSlot");
	  } break;

	  case SCRIPTEVENT_ONREMOVEFROMEQUIPMENTSLOT: {
		fprintf(pOutputFile, "OnRemoveFromEquipmentSlot ");
	  } break;

	  case SCRIPTEVENT_ONUSEHABILITY: {
		fprintf(pOutputFile, "OnUseHability ");
	  } break;

	  case SCRIPTEVENT_ONACTIVATEDSYMPTOM: {
		fprintf(pOutputFile, "OnActivatedSymptom ");
	  } break;

	  case SCRIPTEVENT_ONDEACTIVATEDSYMPTOM: {
		fprintf(pOutputFile, "OnDeactivatedSymptom ");
	  } break;

	  case SCRIPTEVENT_ONHITENTITY: {
		fprintf(pOutputFile, "OnHitEntity ");
	  } break;

	  case SCRIPTEVENT_ONSTARTCOMBATTURN: {
		fprintf(pOutputFile, "OnStartCombatTurn ");
	  } break;

	  case SCRIPTEVENT_ONCRIATUREINRANGE: {
		fprintf(pOutputFile, "OnCriatureInRange ");
	  } break;

	  case SCRIPTEVENT_ONCRIATUREOUTOFRANGE: {
		fprintf(pOutputFile, "OnCriatureOutOfRange ");
	  } break;
  
	  case SCRIPTEVENT_ONENTITYIDLE: {
		fprintf(pOutputFile, "OnCriatureIdle ");
	  } break;

	  case SCRIPTEVENT_ONUSEITEM: {
		fprintf(pOutputFile, "OnUseItem ");
	  } break;

	  case SCRIPTEVENT_ONTRADEITEM: {
		fprintf(pOutputFile, "OnTradeItem ");
	  } break;

	  case SCRIPTEVENT_ONENTITYCREATED: {
		fprintf(pOutputFile, "OnEntityCreated ");
	  } break;
	}; // ~ switch

	ASTPrintArgument(pScriptType->pArguments, pOutputFile);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Imprime imports
// Parametros:
// - pImport. Import a imprimir.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ASTPrintImport(const sImport* pImport,
			   FILE* pOutputFile)
{
  // ¿Hay import?
  if (pImport) {
	switch(pImport->ImportType) {
	  case IMPORT_SEQ: {
		ASTPrintImport(pImport->ImportSeq.pFirst, pOutputFile);
		ASTPrintImport(pImport->ImportSeq.pSecond, pOutputFile);
	  } break;

	  case IMPORT_FUNC: {
		fprintf(pOutputFile, "\n<IMPORT> \n");
		ASTPrintFunc(pImport->ImportFunc.pFunctions, pOutputFile);
		fprintf(pOutputFile, "\n<\\IMPORT> \n");
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Imprime una sentencia
// Parametros:
// - pStm. Sentencia a imprimir.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ASTPrintStm(const sStm* pStm,
			FILE* pOutputFile)
{
  // ¿Hay sentencia?
  if (pStm) { 
	switch(pStm->StmType) {
	  case STM_SEQ: {
		ASTPrintStm(pStm->StmSeq.pFirst, pOutputFile);
		ASTPrintStm(pStm->StmSeq.pSecond, pOutputFile);		
	  } break;

	  case STM_RETURN: {
		fprintf(pOutputFile, "return ");
		if (pStm->StmReturn.pExp) {
		  ASTPrintExp(pStm->StmReturn.pExp, pOutputFile);
		}
		fprintf(pOutputFile, ";\n");
	  } break;
  
	  case STM_IF: {
		fprintf(pOutputFile, "if (");
		ASTPrintExp(pStm->If.pExp, pOutputFile);
		fprintf(pOutputFile, ") then\n");
		fprintf(pOutputFile, "begin\n");
		ASTPrintStm(pStm->If.pThenStm, pOutputFile);
		fprintf(pOutputFile, "end\n");
	  } break;

	  case STM_IFELSE: {
		fprintf(pOutputFile, "if (");
		ASTPrintExp(pStm->IfElse.pExp, pOutputFile);
		fprintf(pOutputFile, ") then\n");
		fprintf(pOutputFile, "begin\n");		
		ASTPrintStm(pStm->IfElse.pThenStm, pOutputFile);
		fprintf(pOutputFile, "end\n");		
		fprintf(pOutputFile, "else\n");
		fprintf(pOutputFile, "begin\n");		
		ASTPrintStm(pStm->IfElse.pElseStm, pOutputFile);
		fprintf(pOutputFile, "end\n");		
	  } break;

	  case STM_WHILE: {
		fprintf(pOutputFile, "while (");
		ASTPrintExp(pStm->While.pExp, pOutputFile);
		fprintf(pOutputFile, ") do\n");
		fprintf(pOutputFile, "begin\n");		
		ASTPrintStm(pStm->While.pDoStm, pOutputFile);
		fprintf(pOutputFile, "end\n");		
	  } break;
	  
	  case STM_EXP: {
		if (pStm->Exp.pExp) {
		  ASTPrintExp(pStm->Exp.pExp, pOutputFile);
		}
		fprintf(pOutputFile, ";\n");
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Imprime una expresion.
// Parametros:
// - pExp. Expresion a imprimir
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ASTPrintExp(const sExp* pExp,
			FILE* pOutputFile)
{
  // ¿Hay alguna expresion para imprimir?
  if (pExp) {
    switch(pExp->ExpType) {
  	  case EXP_IDENTIFIER: {
	    fprintf(pOutputFile, "%s ", pExp->ExpIdentifier.szIdentifier);
	  } break;

	  case EXP_GLOBAL_CONST_ENTITY: {
		fprintf(pOutputFile, "%u ", pExp->ExpGlobalConstEntity.unValue);
	  } break;

	  case EXP_GLOBAL_CONST_NUMBER: {
		fprintf(pOutputFile, "%f ", pExp->ExpGlobalConstNumber.fValue);
	  } break;
	
	  case EXP_NUMBER_VALUE: {
	    fprintf(pOutputFile, "%f ", pExp->ExpNumValue.fNumValue);
	  } break;

	  case EXP_STRING_VALUE: {
	    fprintf(pOutputFile, "\"%s\" ", pExp->ExpStrValue.szStrValue);
	  } break;

	  case EXP_ASSING: {
	    fprintf(pOutputFile, "%s := ", pExp->ExpIdentifier.szIdentifier);
	    ASTPrintExp(pExp->ExpAssing.pRightExp, pOutputFile);		  
	  } break;

	  case EXP_EQUAL: {
	    ASTPrintExp(pExp->ExpEqual.pLeftExp, pOutputFile);
	    fprintf(pOutputFile, "== ");
	    ASTPrintExp(pExp->ExpEqual.pRightExp, pOutputFile);
	  } break;

	  case EXP_NOEQUAL: {
	    ASTPrintExp(pExp->ExpNoEqual.pLeftExp, pOutputFile);
	    fprintf(pOutputFile, "!= ");
	    ASTPrintExp(pExp->ExpNoEqual.pRightExp, pOutputFile);
	  } break;
		
	  case EXP_LESS: {
	    ASTPrintExp(pExp->ExpLess.pLeftExp, pOutputFile);
	    fprintf(pOutputFile, "< ");
	    ASTPrintExp(pExp->ExpLess.pRightExp, pOutputFile);
	  } break;
		
	  case EXP_LESSEQUAL: {
	    ASTPrintExp(pExp->ExpLEqual.pLeftExp, pOutputFile);
	    fprintf(pOutputFile, "<= ");
	    ASTPrintExp(pExp->ExpLEqual.pRightExp, pOutputFile);
	  } break;
		
	  case EXP_GREATER: {
	    ASTPrintExp(pExp->ExpGreater.pLeftExp, pOutputFile);
	    fprintf(pOutputFile, "> ");
	    ASTPrintExp(pExp->ExpGreater.pRightExp, pOutputFile);
	  } break;
		
	  case EXP_GREATEREQUAL: {
	    ASTPrintExp(pExp->ExpGEqual.pLeftExp, pOutputFile);
	    fprintf(pOutputFile, ">= ");
	    ASTPrintExp(pExp->ExpGEqual.pRightExp, pOutputFile);
	  } break;
		
	  case EXP_ADD: {
	    ASTPrintExp(pExp->ExpAdd.pLeftExp, pOutputFile);
	    fprintf(pOutputFile, "+ ");
	    ASTPrintExp(pExp->ExpAdd.pRightExp, pOutputFile);
	  } break;
		
	  case EXP_MINUS: {
	    ASTPrintExp(pExp->ExpMinus.pLeftExp, pOutputFile);
	    fprintf(pOutputFile, "- ");
	    ASTPrintExp(pExp->ExpMinus.pRightExp, pOutputFile);
	  } break;
		
	  case EXP_MULT: {
	    ASTPrintExp(pExp->ExpMult.pLeftExp, pOutputFile);
	    fprintf(pOutputFile, "* ");
	    ASTPrintExp(pExp->ExpMult.pRightExp, pOutputFile);
	  } break;
		
	  case EXP_DIV: {
	    ASTPrintExp(pExp->ExpDiv.pLeftExp, pOutputFile);
	    fprintf(pOutputFile, "/ ");
	    ASTPrintExp(pExp->ExpDiv.pRightExp, pOutputFile);
	  } break;
		
	  case EXP_MODULO: {
	    ASTPrintExp(pExp->ExpModulo.pLeftExp, pOutputFile);
	    fprintf(pOutputFile, "% ");
	    ASTPrintExp(pExp->ExpModulo.pRightExp, pOutputFile);
	  } break;
		
	  case EXP_UMINUS: {
	    fprintf(pOutputFile, "- ");
	    ASTPrintExp(pExp->ExpUMinus.pExp, pOutputFile);
	  } break;
		
	  case EXP_NOT: {
	    fprintf(pOutputFile, "! ");
	    ASTPrintExp(pExp->ExpNot.pExp, pOutputFile);
	  } break;
		
	  case EXP_AND: {
	    ASTPrintExp(pExp->ExpAnd.pLeftExp, pOutputFile);
	    fprintf(pOutputFile, "&& ");
	    ASTPrintExp(pExp->ExpAnd.pRightExp, pOutputFile);
	  } break;
		
	  case EXP_OR: {
	    ASTPrintExp(pExp->ExpOr.pLeftExp, pOutputFile);
	    fprintf(pOutputFile, "|| ");
	    ASTPrintExp(pExp->ExpOr.pRightExp, pOutputFile);
	  } break;
		
	  case EXP_FUNC_INVOKE: {
	    fprintf(pOutputFile, "%s(", pExp->ExpFuncInvoke.szIdentifier);		  
	    ASTPrintParams(pExp->ExpFuncInvoke.pParams, pOutputFile);		  
	    fprintf(pOutputFile, ")");
	  } break;
		
	  case EXP_APIFUNC_INVOKE: {
	    switch(pExp->ExpAPIFuncInvoke.ExpAPIFunc) {
	  	  case API_PASSTORGBCOLOR: {
		    fprintf(pOutputFile, "APIPassToRGBColor(");		  
		  } break;
			
		  case API_GETREDCOMPONENT: {
		    fprintf(pOutputFile, "APIGetRedComponent(");
		  } break;
			
		  case API_GETGREENCOMPONENT: {
		    fprintf(pOutputFile, "APIGetGreenComponent(");
		  } break;
			
		  case API_GETBLUECOMPONENT: {
		    fprintf(pOutputFile, "APIGetBlueComponent(");
		  } break;
			
		  case API_RAND: {
		    fprintf(pOutputFile, "APIGetRand(");
		  } break;
			
		  case API_GETINTEGERVALUE: {
		    fprintf(pOutputFile, "APIGetIntegerValue(");
		  } break;
			
		  case API_GETDECIMALVALUE: {
		    fprintf(pOutputFile, "APIGetDecimalValue(");
		  } break;
			
		  case API_GETSTRINGSIZE: {
		    fprintf(pOutputFile, "APIGetStringSize(");
		  } break;
			
		  case API_WRITETOLOGGER: {
		    fprintf(pOutputFile, "APIWriteToLogger(");
		  } break;
			
		  case API_ENABLECRISOLSCRIPTWARNINGS: {
		    fprintf(pOutputFile, "APIEnableCrisolScriptWarnings(");
		  } break;
			
		  case API_DISABLECRISOLSCRIPTWARNINGS: {
		    fprintf(pOutputFile, "APIDisableCrisolScriptWarnings(");
		  } break;

		  case API_SHOWFPS: {
		    fprintf(pOutputFile, "APIShowFPS(");
		  } break;

		  case API_WAIT: {
		    fprintf(pOutputFile, "APIWait(");
		  } break;
	  }; // ~ switch
		  
	    ASTPrintParams(pExp->ExpAPIFuncInvoke.pParams, pOutputFile);
	    fprintf(pOutputFile, ")");
	  } break;
		
	  case EXP_GAMEOBJ_INVOKE: {
	    switch(pExp->ExpGameObjInvoke.ExpGameObjMethod) {
	  	  case GAMEOBJ_QUITGAME: {
		  fprintf(pOutputFile, "Game.QuitGame(");
		} break;

		case GAMEOBJ_WRITETOCONSOLE: {
		  fprintf(pOutputFile, "Game.WriteToConsole(");
		} break;

		case GAMEOBJ_ACTIVEADVICEDIALOG: {
		  fprintf(pOutputFile, "Game.ActiveAdviceDialog(");
		} break;

		case GAMEOBJ_ACTIVEQUESTIONDIALOG: {
		  fprintf(pOutputFile, "Game.ActiveQuestionDialog(");
		} break;

		case GAMEOBJ_ACTIVETEXTREADERDIALOG: {
		  fprintf(pOutputFile, "Game.ActiveTextReaderDialog(");
		} break;

		case GAMEOBJ_ADDOPTIONTOTEXTSELECTORDIALOG: {
		  fprintf(pOutputFile, "Game.AddOptionToTextSelectorDialog(");
		} break;

		case GAMEOBJ_RESETOPTIONSINTEXTSELECTORDIALOG: {
		  fprintf(pOutputFile, "Game.ResetOptionsInTextSelectorDialog(");
		} break;

		case GAMEOBJ_ACTIVETEXTSELECTORDIALOG: {
		  fprintf(pOutputFile, "Game.ActiveTextSelectorDialog(");
		} break;

		case GAMEOBJ_PLAYMIDIMUSIC: {
		  fprintf(pOutputFile, "Game.PlayMIDIMusic(");
		} break;

		case GAMEOBJ_STOPMIDIMUSIC: {
		  fprintf(pOutputFile, "Game.StopMIDIMusic(");
		} break;

		case GAMEOBJ_PLAYWAVAMBIENTSOUND: {
		  fprintf(pOutputFile, "Game.PlayWAVAmbientSound(");
		} break;

		case GAMEOBJ_STOPWAVAMBIENTSOUND: {
		  fprintf(pOutputFile, "Game.StopWAVAmbientSound(");
		} break;

		case GAMEOBJ_ACTIVETRADEITEMSINTERFAZ: {
		  fprintf(pOutputFile, "Game.ActiveTradeItemsInterfaz(");
		} break;

		case GAMEOBJ_ADDOPTIONTOCONVERSATORINTERFAZ: {
		  fprintf(pOutputFile, "Game.AddOptionToConversatorInterfaz(");
		} break;

		case GAMEOBJ_RESETOPTIONSINCONVERSATORINTERFAZ: {
		  fprintf(pOutputFile, "Game.ResetOptionsInConversatorInterfaz(");
		} break;

		case GAMEOBJ_ACTIVECONVERSATORINTERFAZ: {
		  fprintf(pOutputFile, "Game.ActiveConversatorInterfaz(");
		} break;

		case GAMEOBJ_DESACTIVECONVERSATORINTERFAZ: {
		  fprintf(pOutputFile, "Game.DesactiveConversatorInterfaz(");
		} break;

		case GAMEOBJ_GETOPTIONFROMCONVERSATORINTERFAZ: {
		  fprintf(pOutputFile, "Game.GetOptionFromConversatorInterfaz(");
		} break;

		case GAMEOBJ_SHOWPRESENTATION: {
		  fprintf(pOutputFile, "Game.ShowPresentation(");
		} break;

		case GAMEOBJ_BEGINCUTSCENE: {
		  fprintf(pOutputFile, "Game.BeginCutScene(");
		} break;

		case GAMEOBJ_ENDCUTSCENE: {
		  fprintf(pOutputFile, "Game.EndCutScene(");
		} break;

		case GAMEOBJ_SETSCRIPT: {
		  fprintf(pOutputFile, "Game.SetScript(");
		} break;

		case GAMEOBJ_ISKEYPRESSED: {
		  fprintf(pOutputFile, "Game.IsKeyPressed(");
		} break;
	  }; // ~ switch

	    ASTPrintParams(pExp->ExpGameObjInvoke.pParams, pOutputFile);
	    fprintf(pOutputFile, ")");
	  } break;
		
	  case EXP_WORLDOBJ_INVOKE: {
		switch(pExp->ExpWorldObjInvoke.ExpWorldObjMethod) {
	  	  case WORLDOBJ_GETAREANAME: {
		    fprintf(pOutputFile, "World.GetAreaName(");
		  } break;

		  case WORLDOBJ_GETAREAID: {
		    fprintf(pOutputFile, "World.GetAreaID(");
		  } break;

		  case WORLDOBJ_GETAREAWIDTH: {
		    fprintf(pOutputFile, "World.GetAreaWidth(");
		  } break;

		  case WORLDOBJ_GETAREAHEIGHT: {
		    fprintf(pOutputFile, "World.GetAreaHeight(");
		  } break;

		  case WORLDOBJ_GETHOUR: {
		    fprintf(pOutputFile, "World.GetHour(");
		  } break;

		  case WORLDOBJ_GETMINUTE: {
		    fprintf(pOutputFile, "World.GetMinute(");
		  } break;

		  case WORLDOBJ_SETHOUR: {
		    fprintf(pOutputFile, "World.SetHour(");
		  } break;

		  case WORLDOBJ_SETMINUTE: {
		    fprintf(pOutputFile, "World.SetMinute(");
		  } break;

		  case WORLDOBJ_GETENTITY: {
		    fprintf(pOutputFile, "World.GetEntity(");
		  } break;

		  case WORLDOBJ_GETPLAYER: {
		    fprintf(pOutputFile, "World.GetPlayer(");
		  } break;

		  case WORLDOBJ_ISFLOORVALID: {
		    fprintf(pOutputFile, "World.IsFloorValid(");
		  } break;

		  case WORLDOBJ_GETITEMAT: {
		    fprintf(pOutputFile, "World.GetItemAt(");
		  } break;

		  case WORLDOBJ_GETNUMITEMSAT: {		
		    fprintf(pOutputFile, "World.GetNumItemsAt(");
		  } break;

		  case WORLDOBJ_GETDISTANCE: {
		    fprintf(pOutputFile, "World.GetDistance(");
		  } break;

		  case WORLDOBJ_CALCULEPATHLENGHT: {
		    fprintf(pOutputFile, "World.CalculePathLenght(");
		  } break;

		  case WORLDOBJ_LOADAREA: {
		    fprintf(pOutputFile, "World.LoadArea(");
		  } break;

		  case WORLDOBJ_CHANGEENTITYLOCATION: {
		    fprintf(pOutputFile, "World.ChangeEntityLocation(");
		  } break;

		  case WORLDOBJ_ATTACHCAMERATOENTITY: {
		    fprintf(pOutputFile, "World.AttachCameraToEntity(");
		  } break;

		  case WORLDOBJ_ATTACHCAMERATOLOCATION: {
		    fprintf(pOutputFile, "World.AttachCameraToLocation(");
		  } break;

		  case WORLDOBJ_ISCOMBATMODEACTIVE: {
		    fprintf(pOutputFile, "World.IsCombatModeActive(");
		  } break;

		  case WORLDOBJ_ENDCOMBAT: {
			fprintf(pOutputFile, "World.EndCombat");
		  } break;

		  case WORLDOBJ_GETCRIATUREINCOMBATTURN: {
			fprintf(pOutputFile, "GetCriatureInCombatTurn");
		  } break;
  
		  case WORLDOBJ_GETCOMBATANT: {
			fprintf(pOutputFile, "GetCombatant");
		  } break;

		  case WORLDOBJ_GETNUMBEROFCOMBATANTS: {
			fprintf(pOutputFile, "GetNumberOfCombatants");
		  } break;

		  case WORLDOBJ_GETAREALIGHTMODEL: {
		    fprintf(pOutputFile, "World.GetAreaLightModel(");
		  } break;

		  case WORLDOBJ_SETIDLESCRIPTTIME: {
		    fprintf(pOutputFile, "World.SetIdleScriptTime(");
		  } break;

		  case WORLDOBJ_SETSCRIPT: {
		    fprintf(pOutputFile, "World.SetScript(");
		  } break;

		  case WORLDOBJ_DESTROYENTITY: {
			fprintf(pOutputFile, "World.DestroyEntity(");
		  } break;
		  
		  case WORLDOBJ_CREATECRIATURE: {
			fprintf(pOutputFile, "World.CreateCriature(");
		  } break;
		  
		  case WORLDOBJ_CREATEWALL: {
			fprintf(pOutputFile, "World.CreateWall(");
		  } break;

		  case WORLDOBJ_CREATESCENARYOBJECT: {
			fprintf(pOutputFile, "World.CreateScenaryObject(");
		  } break;

		  case WORLDOBJ_CREATEITEMABANDONED: {
			fprintf(pOutputFile, "World.CreateItemAbandoned(");
		  } break;

		  case WORLDOBJ_CREATEITEMWITHOWNER: {
			fprintf(pOutputFile, "World.CreateItemWithOwner(");
		  } break;

		  case WORLDOBJ_SETWORLDTIMEPAUSE: {
			fprintf(pOutputFile, "World.SetWorldTimePause(");
		  } break;

		  case WORLDOBJ_ISWORLDTIMEINPAUSE: {
			fprintf(pOutputFile, "World.IsWorldTimeInPause(");
		  } break;

		  case WORLDOBJ_SETELEVATIONAT: {
			fprintf(pOutputFile, "World.SetElevationAt(");
		  } break;
		  
		  case WORLDOBJ_GETELEVATIONAT: {
			fprintf(pOutputFile, "World.GetElevationAt(");
		  } break;

		  case WORLDOBJ_NEXTTURN: {
			fprintf(pOutputFile, "World.NextTurn(");
		  } break;

		  case WORLDOBJ_GETLIGHTAT: {
			fprintf(pOutputFile, "World.GetLightAt(");
		  } break;

		  case WORLDOBJ_PLAYWAVSOUND: {
			fprintf(pOutputFile, "World.PlayWAVSound(");
		  } break;

		  case WORLDOBJ_SETSCRIPTAT: {
			fprintf(pOutputFile, "World.SetScriptAt(");
		  } break;
	  }; // ~ switch

  	    ASTPrintParams(pExp->ExpWorldObjInvoke.pParams, pOutputFile);
	    fprintf(pOutputFile, ")");
	  } break;
		
	  case EXP_ENTITYOBJ_INVOKE: {
	    switch(pExp->ExpEntityObjInvoke.ExpEntityObjMethod) {
	  	  case ENTITYOBJ_GETNAME: {			  
		    fprintf(pOutputFile, "%s.GetName(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;

		  case ENTITYOBJ_SETNAME: {			  
		    fprintf(pOutputFile, "%s.SetName(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_GETENTITYTYPE: {
		    fprintf(pOutputFile, "%s.GetEntityType(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
		  
		  case ENTITYOBJ_GETTYPE: {
		    fprintf(pOutputFile, "%s.GetType(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_SAY: {
		    fprintf(pOutputFile, "%s.Say(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_SHUTUP: {
		    fprintf(pOutputFile, "%s.ShutUp(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_ISSAYING: {
		    fprintf(pOutputFile, "%s.IsSaying(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_ATTACHGFX: {
		    fprintf(pOutputFile, "%s.AttachGFX(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_RELEASEGFX: {
		    fprintf(pOutputFile, "%s.ReleaseGFX(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_RELEASEALLGFX: {
		    fprintf(pOutputFile, "%s.ReleaseAllGFX(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_ISGFXATTACHED: {
		    fprintf(pOutputFile, "%s.IsGFXAttached(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_GETNUMITEMSINCONTAINER: {
		    fprintf(pOutputFile, "%s.GetNumItemsInContainer(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;

		  case ENTITYOBJ_GETITEMFROMCONTAINER: {
			fprintf(pOutputFile, "%s.GetItemFromContainer(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_ISITEMINCONTAINER: {
		    fprintf(pOutputFile, "%s.IsItemInContainer(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_TRANSFERITEMTOCONTAINER: {
		    fprintf(pOutputFile, "%s.TransferItemToContainer(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_INSERTITEMINCONTAINER: {
		    fprintf(pOutputFile, "%s.InsertItemInContainer(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_REMOVEITEMOFCONTAINER: {
		    fprintf(pOutputFile, "%s.RemoveItemOfContainer(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_SETANIMTEMPLATESTATE: {
		    fprintf(pOutputFile, "%s.SetAnimTemplateState(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_SETPORTRAITANIMTEMPLATESTATE: {
		    fprintf(pOutputFile, "%s.SetPortraitAnimTemplateState(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_SETIDLESCRIPTTIME: {
		    fprintf(pOutputFile, "%s.SetIdleScriptTime(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_GETOWNER: {
		    fprintf(pOutputFile, "%s.GetOwner(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_GETCLASS: {
		    fprintf(pOutputFile, "%s.GetClass(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;

		  case ENTITYOBJ_GETINCOMBATUSECOST: {
		    fprintf(pOutputFile, "%s.GetInCombatUseCost(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;

		  case ENTITYOBJ_GETGLOBALATTRIBUTE: {
		    fprintf(pOutputFile, "%s.GetGlobalAttribute(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;

		  case ENTITYOBJ_SETGLOBALATTRIBUTE: {
		    fprintf(pOutputFile, "%s.SetGlobalAttribute(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_GETWALLORIENTATION: {
		    fprintf(pOutputFile, "%s.GetWallOrientation(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_BLOCKACCESS: {
		    fprintf(pOutputFile, "%s.BlockAccess(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_UNBLOCKACCESS: {
		    fprintf(pOutputFile, "%s.UnblockAccess(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_ISACCESSBLOCKED: {
		    fprintf(pOutputFile, "%s.IsAccessBlocked(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_SETSYMPTOM: {
		    fprintf(pOutputFile, "%s.SetSymptom(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_ISSYMPTOMACTIVE: {
		    fprintf(pOutputFile, "%s.IsSymptomActive(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_GETGENRE: {
		    fprintf(pOutputFile, "%s.GetGenre(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_GETHEALTH: {
		    fprintf(pOutputFile, "%s.GetHealth(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_SETHEALTH: {
		    fprintf(pOutputFile, "%s.SetHealth(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_GETEXTENDEDATTRIBUTE: {
		    fprintf(pOutputFile, "%s.GetExtendedAttribute(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_SETEXTENDEDATTRIBUTE: {
		    fprintf(pOutputFile, "%s.SetExtendedAttribute(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_GETLEVEL: {
		    fprintf(pOutputFile, "%s.GetLevel(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_SETLEVEL: {
		    fprintf(pOutputFile, "%s.SetLevel(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_GETEXPERIENCE: {
		    fprintf(pOutputFile, "%s.GetExperience(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_SETEXPERIENCE: {
		    fprintf(pOutputFile, "%s.SetExperience(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
		
		  case ENTITYOBJ_GETINCOMBATACTIONPOINTS: {
		    fprintf(pOutputFile, "%s.GetInCombatActionPoints(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;

		  case ENTITYOBJ_GETACTIONPOINTS: {
		    fprintf(pOutputFile, "%s.GetActionPoints(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_SETACTIONPOINTS: {
		    fprintf(pOutputFile, "%s.SetActionPoints(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
		
		  case ENTITYOBJ_ISHABILITYACTIVE: {
		    fprintf(pOutputFile, "%s.IsHabilityActive(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_SETHABILITY: {
		    fprintf(pOutputFile, "%s.SetHability(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_USEHABILITY: {
		    fprintf(pOutputFile, "%s.UseHability(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_ISRUNMODEACTIVE: {
		    fprintf(pOutputFile, "%s.IsRunModeActive(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_SETRUNMODE: {
		    fprintf(pOutputFile, "%s.SetRunMode(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_MOVETO: {
		    fprintf(pOutputFile, "%s.MoveTo(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_ISMOVING: {
		    fprintf(pOutputFile, "%s.IsMoving(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_STOPMOVING: {
		    fprintf(pOutputFile, "%s.StopMoving(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_EQUIPITEM: {
		    fprintf(pOutputFile, "%s.EquipExpem(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_REMOVEITEMEQUIPPED: {
		    fprintf(pOutputFile, "%s.RemoveItemEquipped(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_GETITEMEQUIPPED: {
		    fprintf(pOutputFile, "%s.GetItemEquipped(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_ISITEMEQUIPPED: {
		    fprintf(pOutputFile, "%s.IsItemEquipped(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_DROPITEM: {
		    fprintf(pOutputFile, "%s.DropExpem(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_USEITEM: {
		    fprintf(pOutputFile, "%s.UseItem(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_MANIPULATE: {
		    fprintf(pOutputFile, "%s.Manipulate(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_SETTRANSPARENTMODE: {
		    fprintf(pOutputFile, "%s.SetTransparentMode(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_ISTRANSPARENTMODEACTIVE: {
		    fprintf(pOutputFile, "%s.IsTransparentModeActive(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_CHANGEANIMORIENTATION: {
		    fprintf(pOutputFile, "%s.ChangeAnimOrientation(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;

		  case ENTITYOBJ_GETANIMORIENTATION: {
		    fprintf(pOutputFile, "%s.GetAnimOrientation(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_SETALINGMENT: {
		    fprintf(pOutputFile, "%s.SetAlingment(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_SETALINGMENTWITH: {
		    fprintf(pOutputFile, "%s.SetAlingmentWith(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_SETALINGMENTAGAINST: {
		    fprintf(pOutputFile, "%s.SetAlingmentAgainst(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_GETALINGMENT: {
		    fprintf(pOutputFile, "%s.GetAlingment(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
			
		  case ENTITYOBJ_HITENTITY: {
		    fprintf(pOutputFile, "%s.HitEntity(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;

		  case ENTITYOBJ_SETSCRIPT: {
		    fprintf(pOutputFile, "%s.SetScript(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;

		  case ENTITYOBJ_ISGHOSTMOVEMODEACTIVE: {
			fprintf(pOutputFile, "%s.IsGhostMoveModeActive(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
	  
		  case ENTITYOBJ_SETGHOSTMOVEMODE: {
			fprintf(pOutputFile, "%s.SetGhostMoveMode(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;

		  case ENTITYOBJ_GETRANGE: {
			fprintf(pOutputFile, "%s.GetRange(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;

		  case ENTITYOBJ_ISINRANGE: {
			fprintf(pOutputFile, "%s.IsInRange(", pExp->ExpEntityObjInvoke.szIdentifier);
		  } break;
	  }; // ~ switch

	    ASTPrintParams(pExp->ExpEntityObjInvoke.pParams, pOutputFile);
	    fprintf(pOutputFile, ")");
	  } break;

	  case EXP_CAST: {			
		fprintf(pOutputFile, "(");		
		ASTPrintType(pExp->ExpCast.pType, pOutputFile);		
		fprintf(pOutputFile, ")");
		ASTPrintExp(pExp->ExpCast.pExp, pOutputFile);	
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
ASTPrintParams(sExp* pParam,
			   FILE* pOutputFile)
{
  // ¿Es parametro valido?
  if (pParam) {
	sExp* pIt = pParam;
	for (; pIt != NULL; pIt = pIt->pSigExp) {
	  ASTPrintExp(pIt, pOutputFile);
	  if (pIt->pSigExp) {
		fprintf(pOutputFile, ", ");
  	  }
	}  
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Imprime un tipo
// Parametros:
// - pType. Tipo a imprimir
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ASTPrintType(const sType* pType,
			 FILE* pOutputFile)
{
  // ¿Hay tipo?
  if (pType) {
	switch(pType->ValueType) {
	  case TYPE_VOID: {
		fprintf(pOutputFile, "void ");
	  } break;

	  case TYPE_NUMBER: {
		fprintf(pOutputFile, "number ");
	  } break;

	  case TYPE_STRING: {
		fprintf(pOutputFile, "string ");
	  } break;

	  case TYPE_ENTITY: {
		fprintf(pOutputFile, "entity ");
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Imprime una funcion.
// Parametros:
// - pFunc. Funcion a imprimir.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ASTPrintFunc(const sFunc* pFunc,
			 FILE* pOutputFile)
{
  // ¿Hay funcion?
  if (pFunc) {
	switch (pFunc->eFuncType) {
	  case FUNC_SEQ: {
		ASTPrintFunc(pFunc->FuncSeq.pFirst, pOutputFile);
		ASTPrintFunc(pFunc->FuncSeq.pSecond, pOutputFile);
	  } break;

	  case FUNC_DECL: {
		ASTPrintType(pFunc->FuncDecl.pType, pOutputFile);
		fprintf(pOutputFile, " %s", pFunc->FuncDecl.szIdentifier);
		ASTPrintArgument(pFunc->FuncDecl.pArguments, pOutputFile);
		fprintf(pOutputFile, "\n");
		if (pFunc->FuncDecl.pConst) {
		  fprintf(pOutputFile, "const\n");		
		  ASTPrintConst(pFunc->FuncDecl.pConst, pOutputFile);
		}

		if (pFunc->FuncDecl.pVar) {
		  fprintf(pOutputFile, "var\n");		
		  ASTPrintVar(pFunc->FuncDecl.pVar, pOutputFile);
		}

		fprintf(pOutputFile, "begin\n");
		ASTPrintStm(pFunc->FuncDecl.pStm, pOutputFile);
		fprintf(pOutputFile, "end\n");		
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Imprime argumentos
// Parametros:
// - pArgument. Argumentos a imprimir
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
ASTPrintArgument(const sArgument* pArgument,
				 FILE* pOutputFile)
{
  // ¿Hay argumentos para imprimir?
  if (pArgument) {
	const sArgument* pIt = pArgument;
	fprintf(pOutputFile, "(");	
	for (; pIt != NULL; pIt = pIt->pSigArgument) {
	  if (pIt->ArgumentPassType == ARGUMENT_REF) {
		fprintf(pOutputFile, "ref ");
	  }
	  ASTPrintType(pIt->pType, pOutputFile);
	  if (pIt->pSigArgument) {
		fprintf(pOutputFile, "%s, ", pIt->szIdentifier);
	  } else {
		fprintf(pOutputFile, "%s", pIt->szIdentifier);
	  }
	}
	fprintf(pOutputFile, ")\n");
  } else {
	fprintf(pOutputFile, "(void)");	
  }
}
