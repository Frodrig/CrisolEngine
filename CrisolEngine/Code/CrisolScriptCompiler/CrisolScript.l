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
  
 /* Analizador sintactico para CrisolScript                                  */
 /* Autor: Fernando Rodriguez                                                */
 /* <frodrig99@yahoo.com | i9808685@petra.euitio.uniovi.es                   */
 /*																			 */
 /* Notas:                                                                   */
 /* - Compilar con -i                                                        */
 /* - Todas las vbles locales comenzaran por l_                              */
 /* Hay que poner #include "TypesDefs.h" en el archivo CrisolScript_tab_h.h  */

 /* Estados */
%x COMMENTSTATE

%{
 /* Cabeceras */
#include "crisolscript_tab.h"
#include "memory.h"
#include "error.h"
#include "TypesDefs.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

 /* Constantes */
#define MAXACTIVEFILEBUFFERS 3

 /* Vbles externas */

 /* Enumerados */
typedef enum { 
 NO_BUFF = - 1,   /* Sin buffer asociado                                */
 GLOBAL_BUFF = 0, /* Buffer asociado al fichero de defs. globales       */
 SCRIPT_BUFF = 1, /* Buffer asociado a un fichero script                */ 
 IMPORT_BUFF = 2  /* Buffer asociado a un fichero con fuciones globales */
} eBuffState;

 /* Estructuras */
typedef struct sBuffFileInfo {
 YY_BUFFER_STATE BuffState;   /* Estado actual del buffer        */
 word            uwActLine;   /* Linea actual asociada al buffer */
 sbyte*          szFileName;  /* Nombre del fichero actual       */
} sBuffFileInfo;

 /* Vbles locales */
sword         l_swCommentScope = 0;              /* Controla el inicio / fin comentario multilinea */      
eBuffState    l_eBuffState = NO_BUFF;            /* Buffer de lectura actual */
sBuffFileInfo l_BuffStack[MAXACTIVEFILEBUFFERS]; /* Pila de buffers */

 /* Prototipos de funciones globales */
void SetGlobalFileBuffer(sbyte* szFileName);
void SetScriptFileBuffer(sbyte* szFileName);
void SetImportFileBuffer(sbyte* szFileName);
char* GetActFileName(void);
unsigned int GetActLine(void);

 /* Prototipos de funciones locales */
static void ChangeToBuffer(sbyte* szFileName,
                           eBuffState NewBuffState);
static void ChangeToPrevBuffer(void);
static void IncActLine(void);
static int yywrap(void);
%}

%%

 /* Estado COMMENTSTATE para reconocer comentarios multilinea */
<COMMENTSTATE>\n         { /* Se incrementa el contador de lineas*/
                          IncActLine();
				         }
<COMMENTSTATE>"*/"       { /* Se cierra el ambito de un comentario */
                          --l_swCommentScope; 
						  /* ¿Fin de comentarios? */
                          if (0 == l_swCommentScope) { 
					       BEGIN(INITIAL);
					      }
				         }
<COMMENTSTATE>"/*"       { /* Se incrementa el contador de ambito */
                          ++l_swCommentScope;
						 }
<COMMENTSTATE>"//"[^\n]* { /* Comentario unilinea */ }
<COMMENTSTATE><<EOF>>    { /* Fin de archivo y comentario sin cerrar -> error */
                           ReportErrorf(GetActFileName(), 
									    yytext, 
									    GetActLine(),
									    "Error> EOF y comentario multilinea sin cerrar.");
					     } 						  
<COMMENTSTATE>[^*/\n]*   { /* Se ignora (mas optimizado que caracter a caracter) */ }
<COMMENTSTATE>.          { /* Se ignora */ }

 /* Estado inicial */
[ \t]+     { /* Se ignora */ }
"/*"       { /* Inicio de comentario multilinea */
             /* Se incrementa el ambito y se pasa a estado de comentario */
             ++l_swCommentScope;
		     BEGIN(COMMENTSTATE);
           }
\n         { /* Se incrementa el contador de lineas */;
             IncActLine();
	       }
"//"[^\n]* { /* Comentario unilinea */ }
 
 /* Palabras reservadas, se retorna directamente el token */
"global"   { return tGLOBAL;   }
"const"	   { return tCONST;    }
"var"	   { return tVAR;      }
"func"	   { return tFUNC;     }
"compile"  { return tCOMPILE;  }
"script"   { return tSCRIPT;   }
"import"   { return tIMPORT;   } 
"number"   { return tNUMBER;   }
"string"   { return tSTRING;   }
"entity"   { return tENTITY;   }
"void"	   { return tVOID;     }
"if"	   { return tIF;       }
"else"	   { return tELSE;     }
"while"	   { return tWHILE;    }
"return"   { return tRETURN;   }
"ref"      { return tREF;      }
"function" { return tFUNCTION; }
"begin"    { return tBEGIN; }
"end"      { return tEND;   }
"do"       { return tDO; }
"then"     { return tTHEN; }

 /* Tipos de script / Identificadores de constantes */
"OnStartGame"				{ return tONSTARTGAME;               }
"OnClickHourPanel"			{ return tONCLICKHOURPANEL;          }
"OnFleeCombat"				{ return tONFLEECOMBAT;              }
"OnKeyPressed"              { return tONKEYPRESSED;              }
"OnStartCombatMode"         { return tONSTARTCOMBATMODE;         }
"OnEndCombatMode"           { return tONENDCOMBATMODE;           }
"OnNewHour"					{ return tONNEWHOUR;                 }
"OnEnterInArea"             { return tONENTERINAREA;             }
"OnWorldIdle"				{ return tONWORLDIDLE;               }
"OnSetInFloor"				{ return tONSETINFLOOR;              }
"OnSetOutOfFloor"			{ return tONSETOUTOFFLOOR;           }
"OnGetItem"					{ return tONGETITEM;                 }
"OnDropItem"				{ return tONDROPITEM;                }
"OnObserveEntity"			{ return tONOBSERVEENTITY;           }
"OnTalkToEntity"			{ return tONTALKTOENTITY;            }
"OnManipulateEntity"        { return tONMANIPULATEENTITY;        }
"OnDeath"                   { return tONDEATH;                   }
"OnResurrect"               { return tONRESURRECT;               }
"OnInsertInEquipmentSlot"   { return tONINSERTINEQUIPMENTSLOT;   }
"OnRemoveFromEquipmentSlot" { return tONREMOVEFROMEQUIPMENTSLOT; }
"OnUseHability"				{ return tONUSEHABILITY;             }
"OnActivatedSymptom"		{ return tONACTIVATEDSYMPTOM;        }
"OnDeactivatedSymptom"		{ return tONDEACTIVATEDSYMPTOM;      }
"OnHitEntity"				{ return tONHITENTITY;               }
"OnStartCombatTurn"			{ return tONSTARTCOMBATTURN;         }
"OnCriatureInRange"         { return tONCRIATUREINRANGE;         }
"OnCriatureOutOfRange"      { return tONCRIATUREOUTOFRANGE;      }
"OnEntityIdle"				{ return tONENTITYIDLE;              }
"OnUseItem"					{ return tONUSEITEM;                 }
"OnTradeItem"				{ return tONTRADEITEM;               }
"OnEntityCreated"           { return tONENTITYCREATED;           }

 /* Metodos comunes a todos los objetos (Game / World / Entity) */
"SetScript" { return tSETSCRIPT; }
 /* Metodos comunes a los objetos World / Entity */
"GetItemAt" { return tGETITEMAT; }
 
 /* Objeto Game / metodos */ 
"Game"                              { return tGAMEOBJ;                           }
"QuitGame"                          { return tQUITGAME;                          }
"WriteToConsole"                    { return tWRITETOCONSOLE;                    }
"ActiveAdviceDialog"                { return tACTIVEADVICEDIALOG;                }
"ActiveQuestionDialog"              { return tACTIVEQUESTIONDIALOG;              }
"ActiveTextReaderDialog"            { return tACTIVETEXTREADERDIALOG;            }
"AddOptionToTextSelectorDialog"     { return tADDOPTIONTOTEXTSELECTORDIALOG;     }
"ResetOptionsInTextSelectorDialog"  { return tRESETOPTIONSINTEXTSELECTORDIALOG;  }
"ActiveTextSelectorDialog"          { return tACTIVETEXTSELECTORDIALOG;          }
"PlayMIDIMusic"                     { return tPLAYMIDIMUSIC;                     }
"StopMIDIMusic"                     { return tSTOPMIDIMUSIC;                     }
"PlayWAVAmbientSound"               { return tPLAYWAVAMBIENTSOUND;               }
"StopWAVAmbientSound"               { return tSTOPWAVAMBIENTSOUND;               }
"ActiveTradeItemsInterfaz"          { return tACTIVETRADEITEMSINTERFAZ;          }
"AddOptionToConversatorInterfaz"    { return tADDOPTIONTOCONVERSATORINTERFAZ;    }
"ResetOptionsInConversatorInterfaz" { return tRESETOPTIONSINCONVERSATORINTERFAZ; }
"ActiveConversatorInterfaz"         { return tACTIVECONVERSATORINTERFAZ;         }
"DesactiveConversatorInterfaz"      { return tDESACTIVECONVERSATORINTERFAZ;      }
"GetOptionFromConversatorInterfaz"  { return tGETOPTIONFROMCONVERSATORINTERFAZ;  }
"ShowPresentation"                  { return tSHOWPRESENTATION;                  }
"BeginCutScene"                     { return tBEGINCUTSCENE;                     }
"EndCutScene"                       { return tENDCUTSCENE;                       }
"IsKeyPressed"                      { return tISKEYPRESSED;                      }

 /* Objeto World / metodos */ 
"World"                   { return tWORLDOBJ;                }
"GetAreaName"             { return tGETAREANAME;             }
"GetAreaID"               { return tGETAREAID;               }
"GetAreaWidth"            { return tGETAREAWIDTH;            }
"GetAreaHeight"           { return tGETAREAHEIGHT;           }
"GetHour"                 { return tGETHOUR;                 }
"GetMinute"               { return tGETMINUTE;               }
"SetHour"                 { return tSETHOUR;                 }
"SetMinute"               { return tSETMINUTE;               }
"GetEntity"               { return tGETENTITY;               }
"GetPlayer"               { return tGETPLAYER;               }
"IsFloorValid"            { return tISFLOORVALID;            }
"GetNumberOfItemsAt"      { return tGETNUMITEMSAT;           }
"GetDistance"             { return tGETDISTANCE;             }
"CalculePathLenght"       { return tCALCULEPATHLENGHT;       }
"LoadArea"                { return tLOADAREA;                }
"ChangeEntityLocation"    { return tCHANGEENTITYLOCATION;    }
"AttachCameraToEntity"    { return tATTACHCAMERATOENTITY;    }
"AttachCameraToLocation"  { return tATTACHCAMERATOLOCATION;  }
"IsCombatModeActive"      { return tISCOMBATMODEACTIVE;      }
"EndCombat"               { return tENDCOMBAT;               }
"GetCriatureInCombatTurn" { return tGETCRIATUREINCOMBATTURN; }
"GetCombatant"            { return tGETCOMBATANT;            }
"GetNumberOfCombatants"   { return tGETNUMBEROFCOMBATANTS;   }
"GetAreaLightModel"       { return tGETAREALIGHTMODEL;       }  
"SetIdleScriptTime"       { return tSETIDLESCRIPTTIME;       }
 /* Nota: SetIdleScriptTime es usado en Entity tambien */  
"DestroyEntity"           { return tDESTROYENTITY;           }
"CreateCriature"          { return tCREATECRIATURE;          }
"CreateWall"              { return tCREATEWALL;              }
"CreateScenaryObject"     { return tCREATESCENARYOBJECT;     }
"CreateItemAbandoned"     { return tCREATEITEMABANDONED;     }
"CreateItemWithOwner"     { return tCREATEITEMWITHOWNER;     }
"SetWorldTimePause"       { return tSETWORLDTIMEPAUSE;       }
"IsWorldTimeInPause"      { return tISWORLDTIMEINPAUSE;      }
"SetElevationAt"          { return tSETELEVATIONAT;          }
"GetElevationAt"          { return tGETELEVATIONAT;          }
"NextTurn"                { return tNEXTTURN;                }
"GetLightAt"              { return tGETLIGHTAT;              }
"PlayWAVSound"            { return tPLAYWAVSOUND;            }
"SetScriptAt"             { return tSETSCRIPTAT;             }

 /* Objeto Entity / Metodos comunes */
"GetName"                      { return tGETNAME;                      }
"SetName"                      { return tSETNAME;                      }
"GetType"                      { return tGETTYPE;                      }
"GetEntityType"                { return tGETENTITYTYPE;                }
"Say"                          { return tSAY;                          }
"ShutUp"                       { return tSHUTUP;                       }
"IsSaying"                     { return tISSAYING;                     }
"AttachGFX"                    { return tATTACHGFX;                    }
"ReleaseGFX"                   { return tRELEASEGFX;                   }
"ReleaseAllGFX"                { return tRELEASEALLGFX;                }
"IsGFXAttached"                { return tISGFXATTACHED;                }
"GetNumberOfItemsIn"           { return tGETNUMITEMSINCONTAINER;       }
"IsItemIn"                     { return tISITEMINCONTAINER;            }
"TransferItemToEntity"         { return tTRANSFERITEMTOCONTAINER;      }
"InsertItemIn"                 { return tINSERTITEMINCONTAINER;        }
"ReleaseItem"                  { return tREMOVEITEMOFCONTAINER;        }
"SetAnimTemplateState"         { return tSETANIMTEMPLATESTATE;         }
"SetPortraitAnimTemplateState" { return tSETPORTRAITANIMTEMPLATESTATE; }
"SetLight"                     { return tSETLIGHT;                     }
"GetLight"                     { return tGETLIGHT;                     }
"GetXPos"                      { return tGETXPOS;                      }
"GetYPos"                      { return tGETYPOS;                      }
"GetElevation"                 { return tGETELEVATION;                 }
"SetElevation"                 { return tSETELEVATION;                 }

 /* Objeto Entity / Metodos comunes para Items / Obj. de escenario / Paredes */
"GetLocalAttribute"            { return tGETLOCALATTRIBUTE;            }
"SetLocalAttribute"            { return tSETLOCALATTRIBUTE;            }

 /* Objeto Entity / Items / Metodos */
"GetOwner"            { return tGETOWNER;           }
"GetClass"            { return tGETCLASS;           } /* Valida tambien para criaturas */
"GetInCombatUseCost"  { return tGETINCOMBATUSECOST; }
"GetGlobalAttribute"  { return tGETGLOBALATTRIBUTE; }
"SetGlobalAttribute"  { return tSETGLOBALATTRIBUTE; }

 /* Objeto Entity / Paredes / Metodos */
"GetWallOrientation"  { return tGETWALLORIENTATION; }
"BlockAccess"         { return tBLOCKACCESS;        }
"UnblockAccess"       { return tUNBLOCKACCESS;      }
"IsAccessBlocked"     { return tISACCESSBLOCKED;    }

 /* Objeto Entity / Criaturas / Metodos */
"SetSymptom"              { return tSETSYMPTOM;              }
"IsSymptomActive"         { return tISSYMPTOMACTIVE;         }
"GetGenre"                { return tGETGENRE;                }
"GetHealth"               { return tGETHEALTH;               }
"SetHealth"               { return tSETHEALTH;               }
"GetExtendedAttribute"    { return tGETEXTENDEDATTRIBUTE;    }
"SetExtendedAttribute"    { return tSETEXTENDEDATTRIBUTE;    }
"GetLevel"                { return tGETLEVEL;                }
"SetLevel"                { return tSETLEVEL;                }
"GetExperience"           { return tGETEXPERIENCE;           }
"SetExperience"           { return tSETEXPERIENCE;           }
"GetInCombatActionPoints" { return tGETINCOMBATACTIONPOINTS; }
"GetActionPoints"         { return tGETACTIONPOINTS;         }
"SetActionPoints"         { return tSETACTIONPOINTS;         }
"IsHabilityActive"        { return tISHABILITYACTIVE;        }
"SetHability"             { return tSETHABILITY;             }
"UseHability"             { return tUSEHABILITY;             }         
"IsRunModeActive"         { return tISRUNMODEACTIVE;         }
"SetRunMode"              { return tSETRUNMODE;              }
"MoveTo"                  { return tMOVETO;                  }
"IsMoving"                { return tISMOVING;                }
"StopMoving"              { return tSTOPMOVING;              }
"EquipItem"               { return tEQUIPITEM;               }
"RemoveItemEquipped"      { return tREMOVEITEMEQUIPPED;      }
"GetItemEquipped"         { return tGETITEMEQUIPPED;         }
"IsItemEquipped"          { return tISITEMEQUIPPED;          }
"DropItem"                { return tDROPITEM;                }
"UseItem"                 { return tUSEITEM;                 }
"Manipulate"              { return tMANIPULATE;              }
"SetTransparentMode"      { return tSETTRANSPARENTMODE;      }
"IsTransparentModeActive" { return tISTRANSPARENTMODEACTIVE; }
"ChangeAnimOrientation"   { return tCHANGEANIMORIENTATION;   }
"GetAnimOrientation"      { return tGETANIMORIENTATION;      }
"SetAlingment"            { return tSETALINGMENT;            }
"SetAlingmentWith"        { return tSETALINGMENTWITH;        }
"SetAlingmentAgainst"     { return tSETALINGMENTAGAINST;     }
"GetAlingment"            { return tGETALINGMENT;            }
"HitEntity"               { return tHITENTITY;               }
"IsGhostMoveModeActive"   { return tISGHOSTMOVEMODEACTIVE;   }
"SetGhostMoveMode"        { return tSETGHOSTMOVEMODE;        }
"GetRange"                { return tGETRANGE;                }
"IsInRange"               { return tISINRANGE;               }

 /* Funciones de API */
"APIPassToRGBColor"              { return tAPIPASSTORGBCOLOR;              }
"APIGetRedComponent"             { return tAPIGETREDCOMPONENT;             }
"APIGetGreenComponent"           { return tAPIGETGREENCOMPONENT;           }
"APIGetBlueComponent"            { return tAPIGETBLUECOMPONENT;            }
"APIRand"                        { return tAPIRAND;                        }
"APIGetIntegerValue"             { return tAPIGETINTEGERVALUE;             }
"APIGetDecimalValue"             { return tAPIGETDECIMALVALUE;             }
"APIGetStringSize"               { return tAPIGETSTRINGSIZE;               }
"APIWriteToLogger"               { return tAPIWRITETOLOGGER;               }
"APIEnableCrisolscriptWarnings"  { return tAPIENABLECRISOLSCRIPTWARNINGS;  }
"APIDisableCrisolscriptWarnings" { return tAPIDISABLECRISOLSCRIPTWARNINGS; }
"APIShowFPS"                     { return tAPISHOWFPS;                     }
"APIWait"                        { return tAPIWAIT;                        }

 /* Constantes internas globales */
"NULL"						   { return tNULL;                         }
"FALSE"						   { return tFALSE;                        }
"TRUE"						   { return tTRUE;                         }  
"RIGHT_HAND_SLOT"			   { return tRIGHT_HAND_SLOT;              }
"LEFT_HAND_SLOT"			   { return tLEFT_HAND_SLOT;               }
"NORTH_DIRECTION"			   { return tNORTH_DIRECTION;              }
"NORTHEAST_DIRECTION"		   { return tNORTHEAST_DIRECTION;          }
"EAST_DIRECTION"			   { return tEAST_DIRECTION;               }
"SOUTHEAST_DIRECTION"		   { return tSOUTHEAST_DIRECTION;          }
"SOUTH_DIRECTION"			   { return tSOUTH_DIRECTION;              }
"SOUTHWEST_DIRECTION"          { return tSOUTHWEST_DIRECTION;          }
"WEST_DIRECTION"               { return tWEST_DIRECTION;               }
"NORTHWEST_DIRECTION"          { return tNORTHWEST_DIRECTION;          }
"NO_COMBAT_ALINGMENT"          { return tNO_COMBAT_ALINGMENT;          }
"PLAYER_COMBAT_ALINGMENT"      { return tPLAYER_COMBAT_ALINGMENT;      }
"ENEMYPLAYER_COMBAT_ALINGMENT" { return tENEMYPLAYER_COMBAT_ALINGMENT; }
"BASE_VALUE"                   { return tBASE_VALUE;                   }
"TEMP_VALUE"                   { return tTEMP_VALUE;                   }
"ENTITY_PLAYER"                { return tENTITY_PLAYER;                }
"ENTITY_SCENE_OBJ"             { return tENTITY_SCENE_OBJ;             }
"ENTITY_NPC"                   { return tENTITY_NPC;                   }
"ENTITY_WALL"                  { return tENTITY_WALL;                  }
"ENTITY_ITEM"                  { return tENTITY_ITEM;                  }
"TEXT_RIGHT_JUSTIFY"           { return tTEXT_RIGHT_JUSTIFY;           }
"TEXT_UP_JUSTIFY"              { return tTEXT_UP_JUSTIFY;              }
"TEXT_LEFT_JUSTIFY"            { return tTEXT_LEFT_JUSTIFY;            }
"KEY_ESC"                      { return tKEY_ESC;                      } 
"KEY_F1"                       { return tKEY_F1;                       }
"KEY_F2"                       { return tKEY_F2;                       }
"KEY_F3"                       { return tKEY_F3;                       }
"KEY_F4"                       { return tKEY_F4;                       }
"KEY_F5"                       { return tKEY_F5;                       }
"KEY_F6"                       { return tKEY_F6;                       }
"KEY_F7"                       { return tKEY_F7;                       }
"KEY_F8"                       { return tKEY_F8;                       }
"KEY_F9"                       { return tKEY_F9;                       }
"KEY_F10"                      { return tKEY_F10;                      }
"KEY_F11"                      { return tKEY_F11;                      }
"KEY_F12"                      { return tKEY_F12;                      }
"KEY_0"                        { return tKEY_0;                        }
"KEY_1"                        { return tKEY_1;                        }
"KEY_2"						   { return tKEY_2;                        }
"KEY_3"						   { return tKEY_3;                        }
"KEY_4"                        { return tKEY_4;                        }
"KEY_5"                        { return tKEY_5;                        }
"KEY_6"                        { return tKEY_6;                        }
"KEY_7"                        { return tKEY_7;                        }
"KEY_8"                        { return tKEY_8;                        }
"KEY_9"                        { return tKEY_9;                        } 
"KEY_A"                        { return tKEY_A;                        } 
"KEY_B"                        { return tKEY_B;                        } 
"KEY_C"                        { return tKEY_C;                        } 
"KEY_D"                        { return tKEY_D;                        } 
"KEY_E"                        { return tKEY_E;                        } 
"KEY_F"                        { return tKEY_F;                        } 
"KEY_G"                        { return tKEY_G;                        }
"KEY_H"                        { return tKEY_H;                        } 
"KEY_I"                        { return tKEY_I;                        } 
"KEY_J"                        { return tKEY_J;                        }  
"KEY_K"                        { return tKEY_K;                        } 
"KEY_L"                        { return tKEY_L;                        } 
"KEY_M"                        { return tKEY_M;                        }  
"KEY_N"                        { return tKEY_N;                        }  
"KEY_O"                        { return tKEY_O;                        }  
"KEY_P"                        { return tKEY_P;                        }  
"KEY_Q"                        { return tKEY_Q;                        }  
"KEY_R"                        { return tKEY_R;                        }  
"KEY_S"                        { return tKEY_S;                        }  
"KEY_T"                        { return tKEY_T;                        }  
"KEY_U"                        { return tKEY_U;                        }  
"KEY_V"                        { return tKEY_V;                        }  
"KEY_W"                        { return tKEY_W;                        }  
"KEY_X"                        { return tKEY_X;                        }  
"KEY_Y"                        { return tKEY_Y;                        }  
"KEY_Z"                        { return tKEY_Z;                        } 
"KEY_BACK"                     { return tKEY_BACK;                     }  
"KEY_TAB"                      { return tKEY_TAB;                      }  
"KEY_RETURN"                   { return tKEY_RETURN;                   }  
"KEY_SPACE"                    { return tKEY_SPACE;                    }  
"KEY_LCONTROL"                 { return tKEY_LCONTROL;                 }   
"KEY_RCONTROL"                 { return tKEY_RCONTROL;                 }  
"KEY_LSHIFT"                   { return tKEY_LSHIFT;                   }  
"KEY_RSHIFT"                   { return tKEY_RSHIFT;                   }  
"KEY_ALT"                      { return tKEY_ALT;                      }  
"KEY_ALTGR"                    { return tKEY_ALTGR;                    }  
"KEY_INSERT"                   { return tKEY_INSERT;                   }   
"KEY_REPAG"                    { return tKEY_REPAG;                    }  
"KEY_AVPAG"                    { return tKEY_AVPAG;                    }  
"KEY_MINUS_PAD"                { return tKEY_MINUS_PAD;                }  
"KEY_ADD_PAD"                  { return tKEY_ADD_PAD;                  }  
"KEY_DIV_PAD"                  { return tKEY_DIV_PAD;                  }  
"KEY_MUL_PAD"                  { return tKEY_MUL_PAD;                  }  
"KEY_0_PAD"                    { return tKEY_0_PAD;                    }   
"KEY_1_PAD"                    { return tKEY_1_PAD;                    }    
"KEY_2_PAD"                    { return tKEY_2_PAD;                    }    
"KEY_3_PAD"                    { return tKEY_3_PAD;                    }    
"KEY_4_PAD"                    { return tKEY_4_PAD;                    }    
"KEY_5_PAD"                    { return tKEY_5_PAD;                    }    
"KEY_6_PAD"                    { return tKEY_6_PAD;                    }    
"KEY_7_PAD"                    { return tKEY_7_PAD;                    }    
"KEY_8_PAD"                    { return tKEY_8_PAD;                    }    
"KEY_9_PAD"                    { return tKEY_9_PAD;                    }   
"KEY_UP"                       { return tKEY_UP;                       }   
"KEY_DOWN"                     { return tKEY_DOWN;                     }   
"KEY_RIGHT"                    { return tKEY_RIGHT;                    }   
"KEY_LEFT"                     { return tKEY_LEFT;                     }   

 /* Operadores */
"&&" { return tAND;    }
"||" { return tOR;     }
":=" { return tASSING; }
"==" { return tEQ;     }
"!=" { return tNEQ;    }
">=" { return tGEQ;    }
"<=" { return tLEQ;    }
"<"  { return '<';     }
">"  { return '>';     }
"!"  { return '!';     }
"+"  { return '+';     }
"-"  { return '-';     }
"*"  { return '*';     }
"/"  { return '/';     }
"%"  { return '%';     }
"("  { return '(';     }
")"  { return ')';     }
","  { return ',';     }
";"  { return ';';     }
"."  { return '.';     }

 /* Valor numerico / Cadenas / Identificadores */
[0-9]+                 { /* Valor numerico entero */
                        yylval.fNumber = (float)atoi(yytext); 
						return tNUMBER_VALUE;
                       }
[0-9]+"."[0-9]+        { /* Valor numerico real */
                        yylval.fNumber = (float) atof(yytext);
						return tNUMBER_VALUE;
                       }
\"[^\"]*\"            { /* Cadena de caracteres */
                        /* Se devolvera sin las comillas */
				        yytext[strlen(yytext) - 1] = '\0';
						yylval.szString = strdup(yytext + 1);
						/*
						yylval.szString = Mem_Alloc(strlen(yytext + 1) + 1);
						strcpy(yylval.szString, yytext + 1);
						*/
					    return tSTRING_VALUE;
                       }
[A-Za-z_][A-Za-z0-9_]* { /* Identificador */
                         yylval.szIdentifier = strdup(yytext);
					     /*
						 yylval.szIdentifier = Mem_Alloc(strlen(yytext) + 1);
						 strcpy(yylval.szIdentifier, yytext);
						 */
						 return tIDENTIFIER;
                       }
 /* Fin de archivo */
<<EOF>> { /* Se cambiara unicamente al buffer previo */		  
          /* Estamos en un estado distinto al global */		  
		  if (l_eBuffState != NO_BUFF) {
		   /* Si, luego regresamos al buffer previo */
		   ChangeToPrevBuffer();
		   if (l_eBuffState == NO_BUFF) {
		    yy_delete_buffer(YY_CURRENT_BUFFER);  
		    yyterminate();
		   }
		  } else {
		    /* No, estamos en el global y terminamos el reconocimiento */
		    yy_delete_buffer(YY_CURRENT_BUFFER);  
		    yyterminate();
          }		  
        }   

 /* Resto */
. { /* Error, caracter no reconocible */
    ReportErrorf(GetActFileName(), 
				 yytext, 
				 GetActLine(),
	             "Error> Caracter %s no reconocibile.", yytext);
  }

%%
 
 /* Establece el buffer primario donde comenzar a compilar */
void SetGlobalFileBuffer(sbyte* szFileName)
{
 /* El parametro es valido */
 if (szFileName) {
  /* ¿Se puede cambiar de buffer? */
  if (NO_BUFF == l_eBuffState) {
    /* Si, se intenta cambiar de buffer, inicializando estructuras antes */
	ChangeToBuffer(szFileName, GLOBAL_BUFF);
  } else {	 
    ReportErrorf(GetActFileName(), 
				 yytext, 
				 GetActLine(),
				 "Error> No es permitido el cambio al fichero global %s.", szFileName);
  }
 } else {
   ReportErrorf(GetActFileName(), 
				yytext, 
				GetActLine(),
				"Error> Puntero szFileName no valido en SetGlobalFileBuffer.");
 }
}

 /* Establece un buffer asociado a un fichero script */
void SetScriptFileBuffer(sbyte* szFileName)
{
  /* ¿El parametro es valido? */
  if (szFileName) { 
    /* ¿Se puede cambiar de buffer ? */
	if (GLOBAL_BUFF == l_eBuffState) {
	  /* Si, se intenta cambiar de buffer */
	  ChangeToBuffer(szFileName, SCRIPT_BUFF);
	} else {	 
	 ReportErrorf(GetActFileName(), 
				  yytext, 
				  GetActLine(),
				  "Error> No es permitido el cambio al fichero script %s.", szFileName);
	}
  } else {
   ReportErrorf(GetActFileName(), 
				yytext, 
				GetActLine(),
				"Error> Puntero szFileName no valido en SetScriptFileBuffer.");
  }
}

 /* Establece un buffer asociado a un fichero importado */
void SetImportFileBuffer(sbyte* szFileName)
{
 /* ¿El parametro es valido? */
 if (szFileName) { 
   /* ¿Se puede cambiar de buffer ? */
   if (SCRIPT_BUFF == l_eBuffState) {
   /* Si, se intenta cambiar de buffer */
    ChangeToBuffer(szFileName, IMPORT_BUFF);
   } else {	 
   ReportErrorf(GetActFileName(), 
			    yytext, 
			    GetActLine(),
			    "Error> No es permitido el cambio al fichero import %s.", szFileName);
  }
 } else {
  ReportErrorf(GetActFileName(), 
			   yytext, 
			   GetActLine(),
			   "Error> Puntero szFileName no valido en SetImportFileBuffer.");
 }
}

 /* Funcion privada donde se halla la logica de intercambio de buffers */
void ChangeToBuffer(sbyte* szFileName,
                    eBuffState NewBuffState)
{
 /* ¿Estado a establecer NO es global? */
 l_eBuffState = NewBuffState;
 if (GLOBAL_BUFF != l_eBuffState) {
  /* Si, luego el compilador ya esta inicializado y guardamos estado actual */
  l_BuffStack[l_eBuffState].BuffState = YY_CURRENT_BUFFER;  
 }
 
 /* Se establecen datos asociados al archivo actual */
 l_BuffStack[l_eBuffState].uwActLine = 1; 
 l_BuffStack[l_eBuffState].szFileName = szFileName;

 /* Se intenta abrir el fichero pasado */
 yyin = fopen(szFileName, "r");
 if (!yyin) {
  /* Han existido problemas, se abandona con error */  
  ReportErrorf(GetActFileName(), 
			   yytext, 
			   GetActLine(),
	           "Error> No se pudo abrir el fichero %s.", szFileName);
  exit(0);
 } else {
  ReportSetFileBuffer(szFileName);
 }

 /* Se cambia de buffer */ 
 yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE)); 
}

 /* Funcion privada para el cambio al buffer previo */
void ChangeToPrevBuffer(void)
{
  /* Se desasocia string del estado actual */
  l_BuffStack[l_eBuffState].szFileName = NULL;

  /* ¿Se estaba en buffer NO global? */
  if (GLOBAL_BUFF != l_eBuffState) {
   /* Si, se cambia al buffer previo */
   yy_delete_buffer(YY_CURRENT_BUFFER);  
   yy_switch_to_buffer(l_BuffStack[l_eBuffState].BuffState);       
   
  }

  /* Se decrementa indice de anidamiento en buffers */
  --l_eBuffState;
  if (l_BuffStack[l_eBuffState].szFileName) {
   ReportReturnToFileBuffer(l_BuffStack[l_eBuffState].szFileName);
  }
}

 /* Retorno del nombre del fichero que se esta examinando */
char* GetActFileName(void)
{
 if (l_eBuffState >= GLOBAL_BUFF) {
  return l_BuffStack[l_eBuffState].szFileName;
 } else {
  return NULL;
 }
}
 /* Retorno de la linea actual asociada al fichero actual */
unsigned int GetActLine(void)
{
 if (l_eBuffState >= GLOBAL_BUFF) { 
  return l_BuffStack[l_eBuffState].uwActLine;
 } else {
   return -1;
 }
}

 /* Incremento del contador de linea actual asociada al fich. actual */
void IncActLine(void)
{
 if (l_eBuffState >= GLOBAL_BUFF) { 
  ++l_BuffStack[l_eBuffState].uwActLine;
 }
}

 /* Funcion llamada al finalizar */
int yywrap(void) {
 return 1;
}