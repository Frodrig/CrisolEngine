 /* 
  * CSCompiler - CrisolScript Compiler
  * Copyright (C) 2002 Fernando Rodríguez Martínez
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation; either version 2 of the License, or
  * (at your option) any later version.
  * 
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *  
  * You should have received a copy of the GNU General Public License
  * along with this program; if not, write to the Free Software
  * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
  */

 /* Gramatica para CrisolScript                                              */
 /* Autor: Fernando Rodriguez                                                */
 /* <frodrig99@yahoo.com | i9808685@petra.euitio.uniovi.es                   */
 /* bison -d crisolscript.y                                                  */

%{
 /* Includes */
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include "TypesDefs.h"
#include "ASTree.h"
#include "error.h"
 
 /* Prototipos de funciones extern */
extern void SetScriptFileBuffer(sbyte* FileName);
extern void SetImportFileBuffer(sbyte* FileName);
extern char* GetActFileName(void);

 /* Vbles extern */
extern sGlobal* pGlobal;
extern sbyte*   szGlobalsFile; 

%}

 /* Tipos */
%union {
 float               fNumber;      /* Valor numerico */
 sbyte*              szString;     /* Cadena         */
 sbyte*              szIdentifier; /* Identificador  */
 struct sGlobal*     pGlobal;      /* Espacio global */
 struct sConst*      pConst;       /* Constantes */
 struct sVar*        pVar;         /* Vbles */
 struct sScript*     pScript;      /* Scripts */
 struct sScriptType* pScriptType;  /* Tipos de scripts */
 struct sImport*     pImport;      /* Ficheros de funciones globales */
 struct sFunc*       pFunc;        /* Funciones */
 struct sType*       pType;        /* Tipos de datos */
 struct sArgument*   pArgument;    /* Argumentos */
 struct sStm*        pStm;         /* Sentencias */
 struct sExp*        pExp;         /* Expresiones */ 
};

 /* Tokens de las palabras reservadas generales */
%token tGLOBAL tCONST tVAR tFUNC tCOMPILE tSCRIPT tNUMBER tSTRING tENTITY
       tVOID tIF tELSE tWHILE tRETURN tREF tTHEN tDO tIMPORT tFUNCTION 
	   tBEGIN tEND tGAMEOBJ tWORLDOBJ

 /* Tokens asociados a constantes internas globales */
%token tNULL tTRUE tFALSE tRIGHT_HAND_SLOT tLEFT_HAND_SLOT 
       tNORTH_DIRECTION tNORTHEAST_DIRECTION tEAST_DIRECTION
       tSOUTHEAST_DIRECTION tSOUTH_DIRECTION tSOUTHWEST_DIRECTION
       tWEST_DIRECTION tNORTHWEST_DIRECTION tNO_COMBAT_ALINGMENT
	   tPLAYER_COMBAT_ALINGMENT tENEMYPLAYER_COMBAT_ALINGMENT
	   tBASE_VALUE tTEMP_VALUE tENTITY_PLAYER tENTITY_SCENE_OBJ
	   tENTITY_NPC tENTITY_WALL tENTITY_ITEM 
	   tTEXT_RIGHT_JUSTIFY tTEXT_UP_JUSTIFY tTEXT_LEFT_JUSTIFY
	   tKEY_ESC tKEY_F1 tKEY_F2 tKEY_F3 tKEY_F4 tKEY_F5 tKEY_F6
	   tKEY_F7 tKEY_F8 tKEY_F9 tKEY_F10 tKEY_F11 tKEY_F12 tKEY_0 tKEY_1
       tKEY_2 tKEY_3 tKEY_4 tKEY_5 tKEY_6 tKEY_7 tKEY_8 tKEY_9 tKEY_A
       tKEY_B tKEY_C tKEY_D tKEY_E tKEY_F tKEY_G tKEY_H tKEY_I tKEY_J  
       tKEY_K tKEY_L tKEY_M tKEY_N tKEY_O tKEY_P tKEY_Q tKEY_R tKEY_S  
       tKEY_T tKEY_U tKEY_V tKEY_W tKEY_X tKEY_Y tKEY_Z tKEY_BACK tKEY_TAB
       tKEY_RETURN tKEY_SPACE tKEY_LCONTROL tKEY_RCONTROL tKEY_LSHIFT tKEY_RSHIFT
       tKEY_ALT tKEY_ALTGR tKEY_INSERT tKEY_REPAG tKEY_AVPAG tKEY_MINUS_PAD 
       tKEY_ADD_PAD tKEY_DIV_PAD tKEY_MUL_PAD tKEY_0_PAD tKEY_1_PAD tKEY_2_PAD
       tKEY_3_PAD tKEY_4_PAD tKEY_5_PAD tKEY_6_PAD tKEY_7_PAD tKEY_8_PAD tKEY_9_PAD
       tKEY_UP tKEY_DOWN tKEY_RIGHT tKEY_LEFT 

 /* Tokens de los tipos de scripts / identificadores de constante*/
%token tONSTARTGAME tONCLICKHOURPANEL tONFLEECOMBAT tONKEYPRESSED
       tONSTARTCOMBATMODE tONENDCOMBATMODE
       tONNEWHOUR tONENTERINAREA tONWORLDIDLE tONSETINFLOOR tONSETOUTOFFLOOR 
	   tONGETITEM tONDROPITEM tONOBSERVEENTITY tONTALKTOENTITY tONMANIPULATEENTITY 
	   tONDEATH tONRESURRECT tONINSERTINEQUIPMENTSLOT tONREMOVEFROMEQUIPMENTSLOT 
	   tONUSEHABILITY tONACTIVATEDSYMPTOM tONDEACTIVATEDSYMPTOM tONHITENTITY 
	   tONSTARTCOMBATTURN tONCRIATUREINRANGE tONCRIATUREOUTOFRANGE
	   tONENTITYIDLE tONUSEITEM tONTRADEITEM tONENTITYCREATED 
	   
 
 /* Tokens del cualquier objeto (Game / World / Entity) */ 
%token tSETSCRIPT

 /* Tokens del objeto game */ 
%token tQUITGAME tWRITETOCONSOLE
       tACTIVEADVICEDIALOG tACTIVEQUESTIONDIALOG tACTIVETEXTREADERDIALOG tADDOPTIONTOTEXTSELECTORDIALOG
       tRESETOPTIONSINTEXTSELECTORDIALOG tACTIVETEXTSELECTORDIALOG tPLAYMIDIMUSIC
       tSTOPMIDIMUSIC tPLAYWAVAMBIENTSOUND tSTOPWAVAMBIENTSOUND tACTIVETRADEITEMSINTERFAZ
       tADDOPTIONTOCONVERSATORINTERFAZ tRESETOPTIONSINCONVERSATORINTERFAZ 
	   tACTIVECONVERSATORINTERFAZ tDESACTIVECONVERSATORINTERFAZ 
	   tGETOPTIONFROMCONVERSATORINTERFAZ tSHOWPRESENTATION tBEGINCUTSCENE 
	   tENDCUTSCENE tISKEYPRESSED

 /* Tokens del objeto world */ 
 /* Notas:                  */
 /* - El token tSETIDLESCRIPTTIME tambien para entity */
 /* - El token tGETITEMAT tambien para entity         */
%token tGETAREANAME tGETAREAID tGETAREAWIDTH tGETAREAHEIGHT tGETHOUR tGETMINUTE 
       tSETHOUR tSETMINUTE tGETENTITY tGETPLAYER tISFLOORVALID tGETNUMITEMSAT
       tGETITEMAT tGETDISTANCE tCALCULEPATHLENGHT tLOADAREA tCHANGEENTITYLOCATION 
	   tATTACHCAMERATOENTITY tATTACHCAMERATOLOCATION 
       tISCOMBATMODEACTIVE tENDCOMBAT tGETCRIATUREINCOMBATTURN tGETCOMBATANT 
	   tGETNUMBEROFCOMBATANTS tGETAREALIGHTMODEL tSETIDLESCRIPTTIME
	   tDESTROYENTITY tCREATECRIATURE tCREATEWALL tCREATESCENARYOBJECT
	   tCREATEITEMABANDONED tCREATEITEMWITHOWNER tSETWORLDTIMEPAUSE 
	   tISWORLDTIMEINPAUSE tSETELEVATIONAT tGETELEVATIONAT 
	   tNEXTTURN tGETLIGHTAT tPLAYWAVSOUND tSETSCRIPTAT

 /* Objeto entity / metodos comunes */
%token tGETNAME tSETNAME tGETTYPE tGETENTITYTYPE tSAY tSHUTUP tISSAYING tATTACHGFX 
       tRELEASEGFX tRELEASEALLGFX tISGFXATTACHED tGETNUMITEMSINCONTAINER 
	   tISITEMINCONTAINER tTRANSFERITEMTOCONTAINER 
	   tINSERTITEMINCONTAINER tREMOVEITEMOFCONTAINER tSETANIMTEMPLATESTATE 
	   tSETPORTRAITANIMTEMPLATESTATE tSETLIGHT tGETLIGHT tGETXPOS tGETYPOS 
	   tGETELEVATION tSETELEVATION

 /* Objeto entity / metodos comunes / Items / Obj. Escenario / Paredes */
%token tGETLOCALATTRIBUTE tSETLOCALATTRIBUTE

 /* Objeto entity / items (tGETCLASS tambien para criaturas) */
%token tGETOWNER tGETCLASS tGETINCOMBATUSECOST tGETGLOBALATTRIBUTE
       tSETGLOBALATTRIBUTE

 /* Objeto entity / paredes */
%token tGETWALLORIENTATION tBLOCKACCESS tUNBLOCKACCESS tISACCESSBLOCKED

 /* Objeto entity / criaturas */
%token tSETSYMPTOM tISSYMPTOMACTIVE tGETGENRE tGETHEALTH tSETHEALTH tGETEXTENDEDATTRIBUTE
       tSETEXTENDEDATTRIBUTE tGETLEVEL tSETLEVEL tGETEXPERIENCE tSETEXPERIENCE tGETACTIONPOINTS
       tSETACTIONPOINTS tISHABILITYACTIVE tSETHABILITY tUSEHABILITY tISRUNMODEACTIVE
       tSETRUNMODE tMOVETO tISMOVING tSTOPMOVING tEQUIPITEM tREMOVEITEMEQUIPPED tISITEMEQUIPPED
       tDROPITEM tUSEITEM tMANIPULATE tSETTRANSPARENTMODE tISTRANSPARENTMODEACTIVE
       tCHANGEANIMORIENTATION tGETANIMORIENTATION tSETALINGMENT 
	   tSETALINGMENTWITH tSETALINGMENTAGAINST tGETALINGMENT tHITENTITY 
	   tGETITEMEQUIPPED tGETINCOMBATACTIONPOINTS 
	   tISGHOSTMOVEMODEACTIVE tSETGHOSTMOVEMODE
	   tGETRANGE tISINRANGE

 /* Funciones de API */
%token tAPIPASSTORGBCOLOR tAPIGETREDCOMPONENT tAPIGETGREENCOMPONENT tAPIGETBLUECOMPONENT
       tAPIRAND tAPIGETINTEGERVALUE tAPIGETDECIMALVALUE tAPIGETSTRINGSIZE 
	   tAPIWRITETOLOGGER tAPIENABLECRISOLSCRIPTWARNINGS tAPIDISABLECRISOLSCRIPTWARNINGS
	   tAPISHOWFPS tAPIWAIT

 /* Resto */
%token <fNumber> tNUMBER_VALUE
%token <szString> tSTRING_VALUE 
%token <szIdentifier> tIDENTIFIER

 /* Para resolver el shift / reduce del if / then / else */       
%nonassoc IFX
%nonassoc tELSE

 /* Precedencia de operadores */
%left tASSING
%left tOR tAND '!'
%left tEQ tNEQ tGEQ tLEQ '>' '<'
%left '+' '-'
%left '*' '/'
%left '%'

 /* Definicion de los tipos asociados a las producciones */
%type<pConst> const const_decl_seq const_decl
%type<pVar> var var_decl_seq var_decl var_identifier_seq
%type<pScript> scripts script_decl_seq script_decl
%type<pScriptType> script_type 
%type<szString> compile_script import_decl
%type<pImport> import import_seq
%type<pFunc> g_function l_function function_decl_seq function_decl
%type<pType> return_type type
%type<pArgument> arguments argument_decl_seq argument_decl
%type<pStm> scope_stm stm_seq stm
%type<pExp> var_init_decl exp params params_seq api_function_call
            gameobj_method_call worldobj_method_call entityobj_method_call


%%

 /* Producciones */
 
 /* Simbolo inicial */
global:   /* vacio */
        { pGlobal = NULL; }
        | tGLOBAL const var scripts 
		{ pGlobal = MakeGlobal(szGlobalsFile, $2, $3, $4); }		
;

 /* Tramo para el reconocimiento de la declaracion de constantes */
const:  /* vacio */
        { $$ = NULL; }
      | tCONST const_decl_seq 
	    { $$ = $2; }
; 

const_decl_seq:  const_decl               
                 { $$ = $1; }
               | const_decl_seq const_decl 
			     { $$ = MakeConstDeclSeq($1, $2); }
;

const_decl: type tIDENTIFIER tASSING exp ';' 
            { $$ = MakeConstDecl($1, $2, $4); }
;

 /* Tramo para el reconocimiento de la declaracion de variables */
var:  /* vacio */
      { $$ = NULL; }
    | tVAR var_decl_seq 
	  { $$ = $2; }
;

var_decl_seq:  var_decl              
               { $$ = $1; }
             | var_decl_seq var_decl 
			   { $$ = MakeVarTypeDeclSeq($1, $2); }
;

var_decl: type var_identifier_seq ';' 
          { $$ = MakeVarTypeDecl($1, $2); }
;

var_identifier_seq:  tIDENTIFIER var_init_decl                   
                     { $$ = MakeVarIdentDeclSeq($1, $2, NULL); }
                   | tIDENTIFIER var_init_decl ',' var_identifier_seq
				     { $$ = MakeVarIdentDeclSeq($1, $2, $4); }
;

var_init_decl:  /* vacio */
                { $$ = NULL; }
			  | tASSING exp 
			    { $$ = $2; }
;

  /* Tramo para la definicion de scripts */         
scripts:  /* vacio */
          { $$ = NULL; }
        | script_decl_seq 
		  { $$ = $1; }		
;

script_decl_seq:  compile_script 
                  { SetScriptFileBuffer($1); } 
				  script_decl 
				  { $$ = $3; }
                | script_decl_seq compile_script 
				  { SetScriptFileBuffer($2); } 
				  script_decl
				  { $$ = MakeScriptSeq($1, $4); }
;

compile_script: tCOMPILE tSTRING_VALUE ';' 
                { $$ = $2; }
;

script_decl: tSCRIPT script_type import const var l_function scope_stm 
             { $$ = MakeScriptDecl(GetActFileName(), $2, $3, $4, $5, $6, $7); }
;

script_type:   tONSTARTGAME '(' tVOID ')' 
               { $$ = MakeScriptTypeOnStartGame(); }
             | tONCLICKHOURPANEL '(' tVOID ')'
			   { $$ = MakeScriptTypeOnClickHourPanel(); }
			 | tONFLEECOMBAT '(' tVOID ')'
			   { $$ = MakeScriptTypeOnFleeCombat(); }
			 | tONKEYPRESSED '(' tVOID ')' 
			   { $$ = MakeScriptTypeOnKeyPressed(); }
			 | tONSTARTCOMBATMODE '(' tVOID ')' 
			   { $$ = MakeScriptTypeOnStartCombatMode(); }
			 | tONENDCOMBATMODE '(' tVOID ')' 
			   { $$ = MakeScriptTypeOnEndCombatMode(); }
			 | tONNEWHOUR '(' tNUMBER tIDENTIFIER ')'
			   { $$ = MakeScriptTypeOnNewHour($4); }
			 | tONENTERINAREA '(' tNUMBER tIDENTIFIER ')'
			   { $$ = MakeScriptTypeOnEnterInArea($4); }
			 | tONWORLDIDLE '(' tVOID ')'
			   { $$ = MakeScriptTypeOnWorldIdle(); }
			 | tONSETINFLOOR '(' tNUMBER tIDENTIFIER ',' 
			                     tNUMBER tIDENTIFIER ',' 
								 tENTITY tIDENTIFIER ')'
			   { $$ = MakeScriptTypeOnSetInFloor($4, $7, $10); }
			 | tONSETOUTOFFLOOR '(' tNUMBER tIDENTIFIER ',' 
			                        tNUMBER tIDENTIFIER ',' 
								    tENTITY tIDENTIFIER ')'
			   { $$ = MakeScriptTypeOnSetOutOfFloor($4, $7, $10); }
			 | tONGETITEM '(' tENTITY tIDENTIFIER ','
			                  tENTITY tIDENTIFIER ')'
			   { $$ = MakeScriptTypeOnGetItem($4, $7); }
			 | tONDROPITEM '(' tENTITY tIDENTIFIER ','
			                   tENTITY tIDENTIFIER ')'
			   { $$ = MakeScriptTypeOnDropItem($4, $7); }
			 | tONOBSERVEENTITY '(' tENTITY tIDENTIFIER ','
			                        tENTITY tIDENTIFIER ')'
			   { $$ = MakeScriptTypeOnObserveEntity($4, $7); }
			 | tONTALKTOENTITY '(' tENTITY tIDENTIFIER ','
			                       tENTITY tIDENTIFIER ')'
				{ $$ = MakeScriptTypeOnTalkToEntity($4, $7); }
			 | tONMANIPULATEENTITY '(' tENTITY tIDENTIFIER ','
			                           tENTITY tIDENTIFIER ')'
			    { $$ = MakeScriptTypeOnManipulateEntity($4, $7); }
			 | tONDEATH '(' tENTITY tIDENTIFIER ')'
			    { $$ = MakeScriptTypeOnDeath($4); }
			 | tONRESURRECT '(' tENTITY tIDENTIFIER ')'
			    { $$ = MakeScriptTypeOnResurrect($4); }
			 | tONINSERTINEQUIPMENTSLOT '(' tENTITY tIDENTIFIER ','
			                                tENTITY tIDENTIFIER ','
								            tNUMBER tIDENTIFIER ')'
			    { $$ = MakeScriptTypeOnInsertInEquipmentSlot($4, $7, $10); }
			 | tONREMOVEFROMEQUIPMENTSLOT '(' tENTITY tIDENTIFIER ','
			                                  tENTITY tIDENTIFIER ','
								              tNUMBER tIDENTIFIER ')'
				{ $$ = MakeScriptTypeOnRemoveFromEquipmentSlot($4, $7, $10); }
			 | tONUSEHABILITY '(' tENTITY tIDENTIFIER ','
			                      tNUMBER tIDENTIFIER ','
								  tENTITY tIDENTIFIER ')'
				{ $$ = MakeScriptTypeOnUseHability($4, $7, $10); }
		     | tONACTIVATEDSYMPTOM '(' tENTITY tIDENTIFIER ','
			                           tNUMBER tIDENTIFIER ')'
				{ $$ = MakeScriptTypeOnActivatedSymptom($4, $7); }
			 | tONDEACTIVATEDSYMPTOM '(' tENTITY tIDENTIFIER ','
			                             tNUMBER tIDENTIFIER ')'
				{ $$ = MakeScriptTypeOnDeactivatedSymptom($4, $7); }
			 | tONHITENTITY '(' tENTITY tIDENTIFIER ','
			                    tNUMBER tIDENTIFIER ','
								tENTITY tIDENTIFIER ')'
				{ $$ = MakeScriptTypeOnHitEntity($4, $7, $10); }
			 | tONSTARTCOMBATTURN '(' tENTITY tIDENTIFIER ')'
			    { $$ = MakeScriptTypeOnStartCombatTurn($4); }
			 | tONENTITYIDLE '(' tENTITY tIDENTIFIER ')'
			    { $$ = MakeScriptTypeOnEntityIdle($4); }
			 | tONUSEITEM '(' tENTITY tIDENTIFIER ','
			                  tENTITY tIDENTIFIER ','
							  tENTITY tIDENTIFIER ')'
			    { $$ = MakeScriptTypeOnUseItem($4, $7, $10); }
			 | tONTRADEITEM '(' tENTITY tIDENTIFIER ','
			                    tENTITY tIDENTIFIER ','
							    tENTITY tIDENTIFIER ')'
			    { $$ = MakeScriptTypeOnTradeItem($4, $7, $10); }
			 | tONENTITYCREATED '(' tENTITY tIDENTIFIER ')'
			    { $$ = MakeScriptTypeOnEntityCreated($4); }
			 | tONCRIATUREINRANGE '(' tENTITY tIDENTIFIER ','
			                          tENTITY tIDENTIFIER ')'
			    { $$ = MakeScriptTypeOnCriatureInRange($4, $7); }
			 | tONCRIATUREOUTOFRANGE '(' tENTITY tIDENTIFIER ','
			                             tENTITY tIDENTIFIER ')'
			    { $$ = MakeScriptTypeOnCriatureOutOfRange($4, $7); }
;

 /* Tramo para la declaracion de las clausulas import */
import:  /* vacio */
         { $$ = NULL; }
       | import_seq 
	     { $$ = $1; }
;

import_seq:  import_decl 
             { SetImportFileBuffer($1); } 
			 g_function
			 { $$ = MakeImportFunc($1, $3); }
           | import_seq import_decl 
		     { SetImportFileBuffer($2); } 
			 g_function
			 { $$ = MakeImportSeq($1, MakeImportFunc($2, $4)); }
;

import_decl: tIMPORT tSTRING_VALUE ';' { $$ = $2; }
;

 /* Tramo para la declaracion de las funciones */
g_function:  /* vacio */
             { $$ = NULL; }
           | function_decl_seq 
		     { $$ = $1; }		   
;

l_function:  /* vacio */
             { $$ = NULL; }
           | tFUNC function_decl_seq 
		     { $$ = $2; }
;

function_decl_seq:  function_decl
                    { $$ = $1; }
                  | function_decl_seq function_decl
				    { $$ = MakeFuncSeq($1, $2); }
;
 
function_decl: return_type tFUNCTION tIDENTIFIER '(' arguments ')' const var scope_stm
               { $$ = MakeFuncDecl($1, $3, $5, $7, $8, $9); }
;

return_type:  tVOID 
              { $$ = MakeTypeVoid(); }
            | type			 
			  { $$ = $1; }
;

arguments:  tVOID          		  
            { $$ = NULL }
           | argument_decl_seq 		  
		    { $$ = $1; }
;

argument_decl_seq:  argument_decl                                  
                    { $$ = $1; }
                  | argument_decl ',' argument_decl_seq
				    { $1->pSigArgument = $3; $$ = $1; }
;

argument_decl:  tREF type tIDENTIFIER               
               { $$ = MakeArgumentByRef($2, $3, NULL); }
		      | type tIDENTIFIER
		       { $$ = MakeArgumentByValue($1, $2, NULL); }
;	  

 /* Tipos de datos disponibles (no se considerara tipo el void) */
type:  tNUMBER 
       { $$ = MakeTypeNumber(); }
     | tSTRING 
	   { $$ = MakeTypeString(); }
	 | tENTITY 
	   { $$ = MakeTypeEntity(); }
;

 /* Trabajo con las sentencias y la declaracion de ambitos */
scope_stm:  tBEGIN tEND 
            { $$ = NULL; }		  
          | tBEGIN stm_seq tEND 
		    { $$ = $2; }
;

stm_seq:  stm           
          { $$ = $1; }     
		 | stm_seq stm 		   
		   { $$ = MakeStmSeq($1, $2); }
;

stm:  ';'                            
     { $$ = MakeStmExp(NULL); }                         
    | tRETURN ';'
	 { $$ = MakeStmReturn(NULL); }	   
	| tRETURN exp ';'	   
	  { $$ = MakeStmReturn($2); }	   
	| tIF '(' exp ')' tTHEN stm %prec IFX	   
	  { $$ = MakeStmIf($3, $6); }	   
	| tIF '(' exp ')' tTHEN stm tELSE stm              	   
	  { $$ = MakeStmIfElse($3, $6, $8); }	   
	| tWHILE '(' exp ')' tDO stm               
	  { $$ = MakeStmWhile($3, $6); }	           	   
	| scope_stm	   
	  { $$ = $1; }
	| exp ';'	  
	  { $$ = MakeStmExp($1); } 
;

 /* Trabajo con las expresiones */
exp:  tIDENTIFIER   
      { $$ = MakeExpIdentifier($1); }                       
	| tNULL
	  { $$ = MakeExpGlobalConstEntity(0); }
	| tTRUE
	  { $$ = MakeExpGlobalConstNumber(1.0f); }
	| tFALSE
	  { $$ = MakeExpGlobalConstNumber(0.0f); }
	| tRIGHT_HAND_SLOT
	  { $$ = MakeExpGlobalConstNumber(0.0f); }
	| tLEFT_HAND_SLOT 
	  { $$ = MakeExpGlobalConstNumber(1.0f); }
	| tNORTH_DIRECTION
	  { $$ = MakeExpGlobalConstNumber(0.0f); }
	| tNORTHEAST_DIRECTION
	  { $$ = MakeExpGlobalConstNumber(1.0f); }
	| tEAST_DIRECTION
	  { $$ = MakeExpGlobalConstNumber(2.0f); }
	| tSOUTHEAST_DIRECTION
	  { $$ = MakeExpGlobalConstNumber(3.0f); }
	| tSOUTH_DIRECTION
	  { $$ = MakeExpGlobalConstNumber(4.0f); }
	| tSOUTHWEST_DIRECTION
	  { $$ = MakeExpGlobalConstNumber(5.0f); }
	| tWEST_DIRECTION
	  { $$ = MakeExpGlobalConstNumber(6.0f); }
	| tNORTHWEST_DIRECTION
	  { $$ = MakeExpGlobalConstNumber(7.0f); }	  
	| tNO_COMBAT_ALINGMENT
	  { $$ = MakeExpGlobalConstNumber(0.0f); }
	| tPLAYER_COMBAT_ALINGMENT
	  { $$ = MakeExpGlobalConstNumber(1.0f); }
	| tENEMYPLAYER_COMBAT_ALINGMENT
	  { $$ = MakeExpGlobalConstNumber(2.0f); }
	| tBASE_VALUE
	  { $$ = MakeExpGlobalConstNumber(1.0f); }
	| tTEMP_VALUE
	  { $$ = MakeExpGlobalConstNumber(0.0f); }
	| tENTITY_PLAYER 
	  { $$ = MakeExpGlobalConstNumber(1.0f); }
	| tENTITY_SCENE_OBJ
	  { $$ = MakeExpGlobalConstNumber(2.0f); }
	| tENTITY_NPC 
	  { $$ = MakeExpGlobalConstNumber(3.0f); }
	| tENTITY_WALL 
	  { $$ = MakeExpGlobalConstNumber(4.0f); }
	| tENTITY_ITEM
	  { $$ = MakeExpGlobalConstNumber(5.0f); }
	| tTEXT_RIGHT_JUSTIFY 
	  { $$ = MakeExpGlobalConstNumber(0.0f); }
	| tTEXT_UP_JUSTIFY 
	  { $$ = MakeExpGlobalConstNumber(1.0f); }
	| tTEXT_LEFT_JUSTIFY
	  { $$ = MakeExpGlobalConstNumber(2.0f); }
	| tONSTARTGAME
	  { $$ = MakeExpGlobalConstNumber(0.0f); }
	| tONCLICKHOURPANEL
	  { $$ = MakeExpGlobalConstNumber(1.0f); }
	| tONFLEECOMBAT
	  { $$ = MakeExpGlobalConstNumber(2.0f); }
	| tONKEYPRESSED
	  { $$ = MakeExpGlobalConstNumber(3.0f); }
	| tONSTARTCOMBATMODE
	  { $$ = MakeExpGlobalConstNumber(4.0f); }
	| tONENDCOMBATMODE
	  { $$ = MakeExpGlobalConstNumber(5.0f); }
	| tONNEWHOUR
	  { $$ = MakeExpGlobalConstNumber(6.0f); }
	| tONENTERINAREA
	  { $$ = MakeExpGlobalConstNumber(7.0f); }
	| tONWORLDIDLE
	  { $$ = MakeExpGlobalConstNumber(8.0f); }
	| tONSETINFLOOR
	  { $$ = MakeExpGlobalConstNumber(9.0f); }
	| tONSETOUTOFFLOOR
	  { $$ = MakeExpGlobalConstNumber(10.0f); }
	| tONGETITEM
	  { $$ = MakeExpGlobalConstNumber(11.0f); }
	| tONDROPITEM
	  { $$ = MakeExpGlobalConstNumber(12.0f); }
	| tONOBSERVEENTITY
	  { $$ = MakeExpGlobalConstNumber(13.0f); }
	| tONTALKTOENTITY
	  { $$ = MakeExpGlobalConstNumber(14.0f); }
	| tONMANIPULATEENTITY
	  { $$ = MakeExpGlobalConstNumber(15.0f); }
	| tONDEATH
	  { $$ = MakeExpGlobalConstNumber(16.0f); }
	| tONRESURRECT
	  { $$ = MakeExpGlobalConstNumber(17.0f); }
	| tONINSERTINEQUIPMENTSLOT
	  { $$ = MakeExpGlobalConstNumber(18.0f); }
	| tONREMOVEFROMEQUIPMENTSLOT
	  { $$ = MakeExpGlobalConstNumber(19.0f); }
	| tONUSEHABILITY
	  { $$ = MakeExpGlobalConstNumber(20.0f); }
	| tONACTIVATEDSYMPTOM
	  { $$ = MakeExpGlobalConstNumber(21.0f); }
	| tONDEACTIVATEDSYMPTOM
	  { $$ = MakeExpGlobalConstNumber(22.0f); }
	| tONHITENTITY
	  { $$ = MakeExpGlobalConstNumber(23.0f); }
	| tONSTARTCOMBATTURN
	  { $$ = MakeExpGlobalConstNumber(24.0f); }
  	| tONCRIATUREINRANGE
	  { $$ = MakeExpGlobalConstNumber(25.0f); }
	| tONCRIATUREOUTOFRANGE
	  { $$ = MakeExpGlobalConstNumber(26.0f); }
	| tONENTITYIDLE
	  { $$ = MakeExpGlobalConstNumber(27.0f); }
	| tONUSEITEM
	  { $$ = MakeExpGlobalConstNumber(28.0f); }
	| tONTRADEITEM
	  { $$ = MakeExpGlobalConstNumber(29.0f); }
	| tONENTITYCREATED
	  { $$ = MakeExpGlobalConstNumber(30.0f); }
	| tKEY_ESC 
	  { $$ = MakeExpGlobalConstNumber(0.0f); }
	| tKEY_F1 
	  { $$ = MakeExpGlobalConstNumber(1.0f); }
	| tKEY_F2 
      { $$ = MakeExpGlobalConstNumber(2.0f); }
	| tKEY_F3 
	  { $$ = MakeExpGlobalConstNumber(3.0f); }
	| tKEY_F4 
	  { $$ = MakeExpGlobalConstNumber(4.0f); }
	| tKEY_F5 
	  { $$ = MakeExpGlobalConstNumber(5.0f); }
	| tKEY_F6
	  { $$ = MakeExpGlobalConstNumber(6.0f); }
	| tKEY_F7 
	  { $$ = MakeExpGlobalConstNumber(7.0f); }
	| tKEY_F8 
	  { $$ = MakeExpGlobalConstNumber(8.0f); }
	| tKEY_F9 
	  { $$ = MakeExpGlobalConstNumber(9.0f); }
	| tKEY_F10 
	  { $$ = MakeExpGlobalConstNumber(10.0f); }
	| tKEY_F11 
      { $$ = MakeExpGlobalConstNumber(11.0f); }
	| tKEY_F12 
	  { $$ = MakeExpGlobalConstNumber(12.0f); }
	| tKEY_0 
	  { $$ = MakeExpGlobalConstNumber(13.0f); }
	| tKEY_1
	  { $$ = MakeExpGlobalConstNumber(14.0f); }
	| tKEY_2 
	  { $$ = MakeExpGlobalConstNumber(15.0f); }
	| tKEY_3 
	  { $$ = MakeExpGlobalConstNumber(16.0f); }
	| tKEY_4 
	  { $$ = MakeExpGlobalConstNumber(17.0f); }
	| tKEY_5 
	  { $$ = MakeExpGlobalConstNumber(18.0f); }
	| tKEY_6 
	  { $$ = MakeExpGlobalConstNumber(19.0f); }
	| tKEY_7 
	  { $$ = MakeExpGlobalConstNumber(20.0f); }
	| tKEY_8 
	  { $$ = MakeExpGlobalConstNumber(21.0f); }
	| tKEY_9 
	  { $$ = MakeExpGlobalConstNumber(22.0f); }
	| tKEY_A
	  { $$ = MakeExpGlobalConstNumber(23.0f); }
	| tKEY_B 
	  { $$ = MakeExpGlobalConstNumber(24.0f); }
	| tKEY_C 
	  { $$ = MakeExpGlobalConstNumber(25.0f); }
	| tKEY_D 
	  { $$ = MakeExpGlobalConstNumber(26.0f); }
	| tKEY_E 
	  { $$ = MakeExpGlobalConstNumber(27.0f); }
	| tKEY_F 
	  { $$ = MakeExpGlobalConstNumber(28.0f); }
	| tKEY_G 
	  { $$ = MakeExpGlobalConstNumber(29.0f); }
	| tKEY_H 
	  { $$ = MakeExpGlobalConstNumber(30.0f); }
	| tKEY_I 
	  { $$ = MakeExpGlobalConstNumber(31.0f); }
	| tKEY_J  
	  { $$ = MakeExpGlobalConstNumber(32.0f); }
	| tKEY_K 
	  { $$ = MakeExpGlobalConstNumber(33.0f); }
	| tKEY_L 
	  { $$ = MakeExpGlobalConstNumber(34.0f); }
	| tKEY_M 
	  { $$ = MakeExpGlobalConstNumber(35.0f); }
	| tKEY_N 
	  { $$ = MakeExpGlobalConstNumber(36.0f); }
	| tKEY_O 
	  { $$ = MakeExpGlobalConstNumber(37.0f); }
	| tKEY_P 
	  { $$ = MakeExpGlobalConstNumber(38.0f); }
	| tKEY_Q 
	  { $$ = MakeExpGlobalConstNumber(39.0f); }
	| tKEY_R 
	  { $$ = MakeExpGlobalConstNumber(40.0f); }
	| tKEY_S  
	  { $$ = MakeExpGlobalConstNumber(41.0f); }
	| tKEY_T 
	  { $$ = MakeExpGlobalConstNumber(42.0f); }
	| tKEY_U 
	  { $$ = MakeExpGlobalConstNumber(43.0f); }
	| tKEY_V 
	  { $$ = MakeExpGlobalConstNumber(44.0f); }
	| tKEY_W 
	  { $$ = MakeExpGlobalConstNumber(45.0f); }
	| tKEY_X 
	  { $$ = MakeExpGlobalConstNumber(46.0f); }
	| tKEY_Y 
	  { $$ = MakeExpGlobalConstNumber(47.0f); }
	| tKEY_Z
	  { $$ = MakeExpGlobalConstNumber(48.0f); }
	| tKEY_BACK 
	  { $$ = MakeExpGlobalConstNumber(49.0f); }
	| tKEY_TAB
	  { $$ = MakeExpGlobalConstNumber(50.0f); }
	| tKEY_RETURN 
	  { $$ = MakeExpGlobalConstNumber(51.0f); }
	| tKEY_SPACE
	  { $$ = MakeExpGlobalConstNumber(52.0f); }
	| tKEY_LCONTROL 
	  { $$ = MakeExpGlobalConstNumber(53.0f); }
	| tKEY_RCONTROL 
	  { $$ = MakeExpGlobalConstNumber(54.0f); }
	| tKEY_LSHIFT 
	  { $$ = MakeExpGlobalConstNumber(55.0f); }
	| tKEY_RSHIFT
	  { $$ = MakeExpGlobalConstNumber(56.0f); }
	| tKEY_ALT 
	  { $$ = MakeExpGlobalConstNumber(57.0f); }
	| tKEY_ALTGR 
	  { $$ = MakeExpGlobalConstNumber(58.0f); }
	| tKEY_INSERT 
	  { $$ = MakeExpGlobalConstNumber(59.0f); }
	| tKEY_REPAG 
	  { $$ = MakeExpGlobalConstNumber(60.0f); }
	| tKEY_AVPAG 
	  { $$ = MakeExpGlobalConstNumber(61.0f); }
	| tKEY_MINUS_PAD 
	  { $$ = MakeExpGlobalConstNumber(62.0f); }
	| tKEY_ADD_PAD 
	  { $$ = MakeExpGlobalConstNumber(63.0f); }
	| tKEY_DIV_PAD 
	  { $$ = MakeExpGlobalConstNumber(64.0f); }
	| tKEY_MUL_PAD 
	  { $$ = MakeExpGlobalConstNumber(65.0f); }
	| tKEY_0_PAD 
	  { $$ = MakeExpGlobalConstNumber(66.0f); }
	| tKEY_1_PAD
	  { $$ = MakeExpGlobalConstNumber(67.0f); }
	| tKEY_2_PAD
	  { $$ = MakeExpGlobalConstNumber(68.0f); }
	| tKEY_3_PAD 
	  { $$ = MakeExpGlobalConstNumber(69.0f); }
	| tKEY_4_PAD 
	  { $$ = MakeExpGlobalConstNumber(70.0f); }
	| tKEY_5_PAD 
	  { $$ = MakeExpGlobalConstNumber(71.0f); }
	| tKEY_6_PAD 
	  { $$ = MakeExpGlobalConstNumber(72.0f); }
	| tKEY_7_PAD 
	  { $$ = MakeExpGlobalConstNumber(73.0f); }
	| tKEY_8_PAD 
	  { $$ = MakeExpGlobalConstNumber(74.0f); }
	| tKEY_9_PAD
	  { $$ = MakeExpGlobalConstNumber(75.0f); }
	| tKEY_UP 
	  { $$ = MakeExpGlobalConstNumber(76.0f); }
	| tKEY_DOWN 
	  { $$ = MakeExpGlobalConstNumber(77.0f); }
	| tKEY_RIGHT
	  { $$ = MakeExpGlobalConstNumber(78.0f); }
	| tKEY_LEFT
	  { $$ = MakeExpGlobalConstNumber(79.0f); }
    | tNUMBER_VALUE              
	  { $$ = MakeExpNumberValue($1); }         	   
	| tSTRING_VALUE                   
	  { $$ = MakeExpStringValue($1); }    	   
	| tIDENTIFIER tASSING exp     
	  { $$ = MakeExpAssing($1, $3); }    	   
	| exp tEQ exp                   	   
	  { $$ = MakeExpEqual($1, $3); }
	| exp tNEQ exp                  
	  { $$ = MakeExpNoEqual($1, $3); }	   
	| exp '<' exp                 
	  { $$ = MakeExpLess($1, $3); }  	   
	| exp tLEQ exp             
	  { $$ = MakeExpLessEqual($1, $3); }      	   
	| exp '>' exp                  	 
	  { $$ = MakeExpGreater($1, $3); }  
	| exp tGEQ exp                 
	  { $$ = MakeExpGreaterEqual($1, $3); }  	   
	| exp '+' exp                   	
	  { $$ = MakeExpAdd($1, $3); }
	| exp '-' exp                 
	  { $$ = MakeExpMinus($1, $3); }  	   
	| exp '*' exp                   
	  { $$ = MakeExpMult($1, $3); }	   
	| exp '/' exp                  
	  { $$ = MakeExpDiv($1, $3); } 	   
	| exp '%' exp                 
	  { $$ = MakeExpModulo($1, $3); }  	   	
	| '-' exp                       
	  { $$ = MakeExpUMinus($2); }	   
	| '!' exp                    
	  { $$ = MakeExpNot($2); }   	   
	| exp tAND exp                
	  { $$ = MakeExpAnd($1, $3); }  	   
	| exp tOR exp                 
	  { $$ = MakeExpOr($1, $3); }  	   
	| '(' exp ')' 
	  { $$ = $2; }
	| tIDENTIFIER '(' params ')'	   
	  { $$ = MakeExpUserFuncInvoke($1, $3); }
	| api_function_call             	   
	  { $$ = $1; }
	| tGAMEOBJ '.' gameobj_method_call           	   
	  { $$ = $3; }
	| tWORLDOBJ '.' worldobj_method_call          	   
	  { $$ = $3; }
	| tIDENTIFIER '.' entityobj_method_call         	 
	  { $3->ExpEntityObjInvoke.szIdentifier = $1; $$ = $3; }	  
;

api_function_call:   tAPIPASSTORGBCOLOR '(' params ')'
                     { $$ = MakeExpAPIFunc(API_PASSTORGBCOLOR, $3); }
                   | tAPIGETREDCOMPONENT '(' params ')'
				     { $$ = MakeExpAPIFunc(API_GETREDCOMPONENT, $3); }
				   | tAPIGETGREENCOMPONENT '(' params ')'
				     { $$ = MakeExpAPIFunc(API_GETGREENCOMPONENT, $3); }
				   | tAPIGETBLUECOMPONENT '(' params ')'
				     { $$ = MakeExpAPIFunc(API_GETBLUECOMPONENT, $3); }
				   | tAPIRAND '(' params ')'
				     { $$ = MakeExpAPIFunc(API_RAND, $3); }
				   | tAPIGETINTEGERVALUE '(' params ')'
				     { $$ = MakeExpAPIFunc(API_GETINTEGERVALUE, $3); }
				   | tAPIGETDECIMALVALUE '(' params ')'
				     { $$ = MakeExpAPIFunc(API_GETDECIMALVALUE, $3); }
				   | tAPIGETSTRINGSIZE '(' params ')'
				     { $$ = MakeExpAPIFunc(API_GETSTRINGSIZE, $3); }
				   | tAPIWRITETOLOGGER '(' params ')'
				     { $$ = MakeExpAPIFunc(API_WRITETOLOGGER, $3); }
				   | tAPIENABLECRISOLSCRIPTWARNINGS '(' params ')'
				     { $$ = MakeExpAPIFunc(API_ENABLECRISOLSCRIPTWARNINGS, $3); }
				   | tAPIDISABLECRISOLSCRIPTWARNINGS '(' params ')'
				     { $$ = MakeExpAPIFunc(API_DISABLECRISOLSCRIPTWARNINGS, $3); }
				   | tAPISHOWFPS '(' params ')'
				     { $$ = MakeExpAPIFunc(API_SHOWFPS, $3); }
				   | tAPIWAIT '(' params ')'
				     { $$ = MakeExpAPIFunc(API_WAIT, $3); }
;

gameobj_method_call:   tQUITGAME '(' params ')'
                       { $$ = MakeExpGameObj(GAMEOBJ_QUITGAME, $3); }
					 | tWRITETOCONSOLE '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_WRITETOCONSOLE, $3); }
					 | tACTIVEADVICEDIALOG '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_ACTIVEADVICEDIALOG, $3); }
					 | tACTIVEQUESTIONDIALOG '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_ACTIVEQUESTIONDIALOG, $3); }
					 | tACTIVETEXTREADERDIALOG '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_ACTIVETEXTREADERDIALOG, $3); }
					 | tADDOPTIONTOTEXTSELECTORDIALOG '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_ADDOPTIONTOTEXTSELECTORDIALOG, $3); }
					 | tRESETOPTIONSINTEXTSELECTORDIALOG '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_RESETOPTIONSINTEXTSELECTORDIALOG, $3); }
					 | tACTIVETEXTSELECTORDIALOG '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_ACTIVETEXTSELECTORDIALOG, $3); }
					 | tPLAYMIDIMUSIC '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_PLAYMIDIMUSIC, $3); }
					 | tSTOPMIDIMUSIC '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_STOPMIDIMUSIC, $3); }
					 | tPLAYWAVAMBIENTSOUND '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_PLAYWAVAMBIENTSOUND, $3); }
					 | tSTOPWAVAMBIENTSOUND '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_STOPWAVAMBIENTSOUND, $3); }
					 | tACTIVETRADEITEMSINTERFAZ '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_ACTIVETRADEITEMSINTERFAZ, $3); }
					 | tADDOPTIONTOCONVERSATORINTERFAZ '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_ADDOPTIONTOCONVERSATORINTERFAZ, $3); }
					 | tRESETOPTIONSINCONVERSATORINTERFAZ '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_RESETOPTIONSINCONVERSATORINTERFAZ, $3); }
					 | tACTIVECONVERSATORINTERFAZ '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_ACTIVECONVERSATORINTERFAZ, $3); }
				     | tDESACTIVECONVERSATORINTERFAZ '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_DESACTIVECONVERSATORINTERFAZ, $3); }
					 | tGETOPTIONFROMCONVERSATORINTERFAZ '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_GETOPTIONFROMCONVERSATORINTERFAZ, $3); }
					 | tSHOWPRESENTATION '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_SHOWPRESENTATION, $3); }
					 | tBEGINCUTSCENE '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_BEGINCUTSCENE, $3); }
					 | tENDCUTSCENE '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_ENDCUTSCENE, $3); }
					 | tSETSCRIPT '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_SETSCRIPT, $3); }
					 | tISKEYPRESSED '(' params ')'
					   { $$ = MakeExpGameObj(GAMEOBJ_ISKEYPRESSED, $3); }
;

worldobj_method_call:   tGETAREANAME '(' params ')'
                        { $$ = MakeExpWorldObj(WORLDOBJ_GETAREANAME, $3); }
					  | tGETAREAID '(' params ')'
                        { $$ = MakeExpWorldObj(WORLDOBJ_GETAREAID, $3); }
                      | tGETAREAWIDTH '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_GETAREAWIDTH, $3); }
					  | tGETAREAHEIGHT '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_GETAREAHEIGHT, $3); }
					  | tGETHOUR '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_GETHOUR, $3); }
					  | tGETMINUTE '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_GETMINUTE, $3); }
					  | tSETHOUR '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_SETHOUR, $3); }
					  | tSETMINUTE '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_SETMINUTE, $3); }
					  | tGETENTITY '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_GETENTITY, $3); }
					  | tGETPLAYER '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_GETPLAYER, $3); }
					  | tISFLOORVALID '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_ISFLOORVALID, $3); }
					  | tGETNUMITEMSAT '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_GETNUMITEMSAT, $3); }
					  | tGETITEMAT '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_GETITEMAT, $3); }
					  | tGETDISTANCE '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_GETDISTANCE, $3); }
					  | tCALCULEPATHLENGHT '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_CALCULEPATHLENGHT, $3); }
					  | tLOADAREA '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_LOADAREA, $3); }
					  | tCHANGEENTITYLOCATION '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_CHANGEENTITYLOCATION, $3); }
					  | tATTACHCAMERATOENTITY '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_ATTACHCAMERATOENTITY, $3); }
					  | tATTACHCAMERATOLOCATION '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_ATTACHCAMERATOLOCATION, $3); }
					  | tISCOMBATMODEACTIVE '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_ISCOMBATMODEACTIVE, $3); }
					  | tENDCOMBAT '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_ENDCOMBAT, $3); }
					  | tGETCRIATUREINCOMBATTURN '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_GETCRIATUREINCOMBATTURN, $3); }
					  | tGETCOMBATANT '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_GETCOMBATANT, $3); }
					  | tGETNUMBEROFCOMBATANTS '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_GETNUMBEROFCOMBATANTS, $3); }
					  | tGETAREALIGHTMODEL '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_GETAREALIGHTMODEL, $3); }
					  | tSETIDLESCRIPTTIME '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_SETIDLESCRIPTTIME, $3); }
					  | tSETSCRIPT '(' params ')'
					   { $$ = MakeExpWorldObj(WORLDOBJ_SETSCRIPT, $3); }
					  |  tDESTROYENTITY  '(' params ')'
						{ $$ = MakeExpWorldObj(WORLDOBJ_DESTROYENTITY, $3); }
					  | tCREATECRIATURE '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_CREATECRIATURE, $3); }
					  | tCREATEWALL '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_CREATEWALL, $3); }
					  | tCREATESCENARYOBJECT '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_CREATESCENARYOBJECT, $3); }
					  | tCREATEITEMABANDONED '(' params ')' 
					    { $$ = MakeExpWorldObj(WORLDOBJ_CREATEITEMABANDONED, $3); }
					  | tCREATEITEMWITHOWNER '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_CREATEITEMWITHOWNER, $3); }
					  | tSETWORLDTIMEPAUSE '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_SETWORLDTIMEPAUSE, $3); }
					  | tISWORLDTIMEINPAUSE '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_ISWORLDTIMEINPAUSE, $3); }
					  | tSETELEVATIONAT '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_SETELEVATIONAT, $3); }
					  | tGETELEVATIONAT '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_GETELEVATIONAT, $3); }
					  | tNEXTTURN '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_NEXTTURN, $3); }
					  | tGETLIGHTAT '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_GETLIGHTAT, $3); }
					  | tPLAYWAVSOUND '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_PLAYWAVSOUND, $3); }
					  | tSETSCRIPTAT '(' params ')'
					    { $$ = MakeExpWorldObj(WORLDOBJ_SETSCRIPTAT, $3); }
;

entityobj_method_call:  tGETNAME '(' params ')'
                        { $$ = MakeExpEntityObj(ENTITYOBJ_GETNAME, $3); }
					  | tSETNAME '(' params ')'
                        { $$ = MakeExpEntityObj(ENTITYOBJ_SETNAME, $3); }
					  | tGETENTITYTYPE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETENTITYTYPE, $3); }
					  | tGETTYPE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETTYPE, $3); }
					  | tSAY '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SAY, $3); }
					  | tSHUTUP '(' params ')'
					    { $$ =  MakeExpEntityObj(ENTITYOBJ_SHUTUP, $3); }
					  | tISSAYING '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_ISSAYING, $3); }
					  | tATTACHGFX '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_ATTACHGFX, $3); }
					  | tRELEASEGFX '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_RELEASEGFX, $3); }
					  | tRELEASEALLGFX '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_RELEASEALLGFX, $3); }
					  | tISGFXATTACHED '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_ISGFXATTACHED, $3); }
					  | tGETNUMITEMSINCONTAINER '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETNUMITEMSINCONTAINER, $3); }
					  | tISITEMINCONTAINER '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_ISITEMINCONTAINER, $3); }
					  | tGETITEMAT '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETITEMFROMCONTAINER, $3); }
					  | tTRANSFERITEMTOCONTAINER '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_TRANSFERITEMTOCONTAINER, $3); }
					  | tINSERTITEMINCONTAINER '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_INSERTITEMINCONTAINER, $3); }
					  | tREMOVEITEMOFCONTAINER '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_REMOVEITEMOFCONTAINER, $3); }
					  | tSETANIMTEMPLATESTATE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SETANIMTEMPLATESTATE, $3); }
					  | tSETPORTRAITANIMTEMPLATESTATE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SETPORTRAITANIMTEMPLATESTATE, $3); }
					  | tSETIDLESCRIPTTIME '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SETIDLESCRIPTTIME, $3); }
					  | tSETLIGHT '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SETLIGHT, $3); }
					  | tGETLIGHT '(' params ')'
						{ $$ = MakeExpEntityObj(ENTITYOBJ_GETLIGHT, $3); }
					  | tGETXPOS  '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETXPOS, $3); }
					  | tGETYPOS '(' params ')'
						{ $$ = MakeExpEntityObj(ENTITYOBJ_GETYPOS, $3); }
					  | tGETELEVATION '(' params ')'
						{ $$ = MakeExpEntityObj(ENTITYOBJ_GETELEVATION, $3); }
					  | tSETELEVATION '(' params ')'
						{ $$ = MakeExpEntityObj(ENTITYOBJ_SETELEVATION, $3); }
					  | tGETLOCALATTRIBUTE '(' params ')'
						{ $$ = MakeExpEntityObj(ENTITYOBJ_GETLOCALATTRIBUTE, $3); }
					  | tSETLOCALATTRIBUTE '(' params ')'
						{ $$ = MakeExpEntityObj(ENTITYOBJ_SETLOCALATTRIBUTE, $3); }
					  | tGETOWNER '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETOWNER, $3); }
					  | tGETCLASS '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETCLASS, $3); }
					  | tGETINCOMBATUSECOST '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETINCOMBATUSECOST, $3); }
					  | tGETGLOBALATTRIBUTE '(' params ')' 
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETGLOBALATTRIBUTE, $3); }
					  | tSETGLOBALATTRIBUTE '(' params ')' 
						{ $$ = MakeExpEntityObj(ENTITYOBJ_SETGLOBALATTRIBUTE, $3); }
					  | tGETWALLORIENTATION '(' params ')'
                        { $$ = MakeExpEntityObj(ENTITYOBJ_GETWALLORIENTATION, $3); }
					  | tBLOCKACCESS '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_BLOCKACCESS, $3); }
					  | tUNBLOCKACCESS '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_UNBLOCKACCESS, $3); }
					  | tISACCESSBLOCKED '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_ISACCESSBLOCKED, $3); }
					  | tSETSYMPTOM '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SETSYMPTOM, $3); }
					  | tISSYMPTOMACTIVE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_ISSYMPTOMACTIVE, $3); }
					  | tGETGENRE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETGENRE, $3); }
					  | tGETHEALTH '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETHEALTH, $3); }
					  | tSETHEALTH '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SETHEALTH, $3); }
					  | tGETEXTENDEDATTRIBUTE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETEXTENDEDATTRIBUTE, $3); }
					  | tSETEXTENDEDATTRIBUTE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SETEXTENDEDATTRIBUTE, $3); }
					  | tGETLEVEL '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETLEVEL, $3); }
					  | tSETLEVEL '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SETLEVEL, $3); }
					  | tGETEXPERIENCE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETEXPERIENCE, $3); }
					  | tSETEXPERIENCE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SETEXPERIENCE, $3); }
					  | tGETINCOMBATACTIONPOINTS '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETINCOMBATACTIONPOINTS, $3); }
					  | tGETACTIONPOINTS '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETACTIONPOINTS, $3); }
					  | tSETACTIONPOINTS '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SETACTIONPOINTS, $3); }
					  | tISHABILITYACTIVE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_ISHABILITYACTIVE, $3); }
					  | tSETHABILITY '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SETHABILITY, $3); }
					  | tUSEHABILITY '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_USEHABILITY, $3); }
					  | tISRUNMODEACTIVE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_ISRUNMODEACTIVE, $3); }
					  | tSETRUNMODE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SETRUNMODE, $3); }
					  | tMOVETO '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_MOVETO, $3); }
					  | tISMOVING '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_ISMOVING, $3); }
					  | tSTOPMOVING '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_STOPMOVING, $3); }
					  | tEQUIPITEM '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_EQUIPITEM, $3); }
					  | tREMOVEITEMEQUIPPED '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_REMOVEITEMEQUIPPED, $3); }
					  | tGETITEMEQUIPPED '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETITEMEQUIPPED, $3); }
					  | tISITEMEQUIPPED '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_ISITEMEQUIPPED, $3); }
					  | tDROPITEM '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_DROPITEM, $3); }
					  | tUSEITEM '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_USEITEM, $3); }
					  | tMANIPULATE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_MANIPULATE, $3); }
					  | tSETTRANSPARENTMODE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SETTRANSPARENTMODE, $3); }
					  | tISTRANSPARENTMODEACTIVE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_ISTRANSPARENTMODEACTIVE, $3); }
					  | tCHANGEANIMORIENTATION '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_CHANGEANIMORIENTATION, $3); }
					  | tGETANIMORIENTATION '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETANIMORIENTATION, $3); }
					  | tSETALINGMENT '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SETALINGMENT, $3); }
					  | tSETALINGMENTWITH '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SETALINGMENTWITH, $3); }
					  | tSETALINGMENTAGAINST '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SETALINGMENTAGAINST, $3); }
					  | tGETALINGMENT '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETALINGMENT, $3); }
					  | tHITENTITY '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_HITENTITY, $3); }
					  | tSETSCRIPT '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SETSCRIPT, $3); }
				      | tISGHOSTMOVEMODEACTIVE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_ISGHOSTMOVEMODEACTIVE, $3); }
					  | tSETGHOSTMOVEMODE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_SETGHOSTMOVEMODE, $3); }
					  | tGETRANGE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_GETRANGE, $3); }
					  | tISINRANGE '(' params ')'
					    { $$ = MakeExpEntityObj(ENTITYOBJ_ISINRANGE, $3); }


						
;

params:  /* vacio */
         { $$ = NULL; }
	    | params_seq 		     
		 { $$ = $1; }
;

params_seq:  exp    
            { $$ = $1; }              
           | exp ',' params_seq
		    { $1->pSigExp = $3; $$ = $1;  }
;


%%