///////////////////////////////////////////////////////////////////////////////
// CrisolEngine - Computer Role-Play Game Engine
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

///////////////////////////////////////////////////////////////////////////////
// CScript.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CScript.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CScript.h"

#include "SYSEngine.h"
#include "iCLogger.h"
#include "iCGameDataBase.h"
#include "iCFileSystem.h"
#include "CScriptInstructions.h"

// Inicializacion de Memory Pools
CMemoryPool 
CScript::sCodeInfo::m_MPool(16, sizeof(CScript::sCodeInfo), true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia para un script que no sea el relativo al espacio
//   global. En este caso, se aceptaran nombres de scripts vacios que 
//   simplemente seran tomados como inicializaciones no validas o scripts
//   no hallados.
// Parametros:
// - szScriptFileName. Nombre del script.
// - ScriptEvent. Tipo de evento asociado al script.
// - pGlobalScript. Enlace al script global
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CScript::Init(const std::string& szScriptFileName,
		      const RulesDefs::eScriptEvents& ScriptEvent,
		      iCScript* pGlobalScript)
{
  // SOLO si parametros correctos
  ASSERT(pGlobalScript);
  
  // ¿Se intenta reinicializar?
  if (IsInitOk()) {
	// Finaliza
	End();
  }

  // ¿Nombre de script NO valido?
  if (szScriptFileName.empty()) {
	// Si, retorna
	return false;
  }

  // Se obtiene handle a fichero con los scripts
  iCGameDataBase* const pGDBase = SYSEngine::GetGameDataBase();
  ASSERT(pGDBase);
  const FileDefs::FileHandle hFile = pGDBase->GetScriptsFileHandle();
  if (hFile) {
	// Se localiza el offset donde comenzar a leer
	dword udOffset = pGDBase->GetEventScriptOffset(szScriptFileName);
	if (udOffset) {
	  // Obtiene el numero de porciones totales con codigo
	  iCFileSystem* const pFileSys = SYSEngine::GetFileSystem();
	  ASSERT(pFileSys);
	  word uwNumCodeParts;
	  udOffset += pFileSys->Read(hFile,
								 (sbyte *)(&uwNumCodeParts),
								 sizeof(word),
								 udOffset);

	  // Lee el codigo del evento al que representa el script
	  word uwEvent;
	  udOffset += pFileSys->Read(hFile,
								 (sbyte *)(&uwEvent),
								 sizeof(word),
								 udOffset);	  
	  
	  // ¿No coincide el codigo?
	  if (RulesDefs::eScriptEvents(uwEvent) != ScriptEvent) {
		// Se abandona
		#ifdef ENGINE_TRACE    
		  SYSEngine::GetLogger()->Write("CScript::Init> Script \"%s\" (%u) no corresponde al evento (%u).\n", 
										szScriptFileName.c_str(), 
										RulesDefs::eScriptEvents(uwEvent),
										ScriptEvent);
		#endif 
		return false;
	  }

	  // Se procede a leer todo el codigo del script
	  word uwCodeIt = 0;
	  for (; uwCodeIt < uwNumCodeParts; ++uwCodeIt) {
		// Se lee Idx del codigo
		word uwCodeIdx;
		udOffset += pFileSys->Read(hFile,
								   (sbyte *)(&uwCodeIdx),
								   sizeof(word),
								   udOffset);
	
		// Se crea nodo para guardar la informacion
		sCodeInfo* const pCodeInfo = new sCodeInfo;
		ASSERT(pCodeInfo);

		// ¿Es una funcion?
		if (uwCodeIdx > 0) {
		  // Si, se lee el codigo de tipo de funcion
		  word uwFuncType;
		  udOffset += pFileSys->Read(hFile, 
								   (sbyte *)(&uwFuncType),
								   sizeof(word),
								   udOffset);
		  pCodeInfo->Type = sCodeInfo::eCodeType(uwFuncType);
		} else {
		  // No, es un script
		  pCodeInfo->Type = sCodeInfo::SCRIPT;
		}

		// Se lee la firma
		// Retorno
		sbyte sbReturnType;
		udOffset += pFileSys->Read(hFile, &sbReturnType, sizeof(sbyte), udOffset);

		// El resto de la firma, se leera como un string pues el formato sera
		// Cantidad de parametros seguido de un sbyte por cada uno de ellos
		udOffset += pFileSys->ReadStringFromBinary(hFile, udOffset, pCodeInfo->szSignature);
		pCodeInfo->szSignature.insert(pCodeInfo->szSignature.begin(), sbReturnType);
				
		// Lee cantidad de offsets (slots de memoria)
		udOffset += pFileSys->Read(hFile, 
								   (sbyte *)(&pCodeInfo->uwNumOffsets),
								   sizeof(word),
								   udOffset);
		
		// Lee la posicion del primer offset
		udOffset += pFileSys->Read(hFile,
						           (sbyte *)(&pCodeInfo->uwInitOffset),
								   sizeof(word),
								   udOffset);
		
		// Tamaño de la pila
		udOffset += pFileSys->Read(hFile,
								   (sbyte *)(&pCodeInfo->uwMaxStackSize),
								   sizeof(word),
								   udOffset);
		
		// Lee el codigo propiamente dicho, creando el vector de instrucciones
		ReadCode(hFile, udOffset, pCodeInfo->Code);

		// Lee la tabla de strings
		ReadStringTable(hFile, udOffset, pCodeInfo->StrTable);
		
		// Mapea la informacion recogida
		m_CodeInfo.insert(CodeInfoMapValType(uwCodeIdx, pCodeInfo));
	  }

	  // Se establecen resto de vbles de miembro
	  m_pGlobalScript = pGlobalScript;
	  m_szScriptFile = szScriptFileName;
	  m_Event = ScriptEvent;
	  m_State = ScriptDefs::SS_INACTIVE;
	  
	  // Todo correcto
	  m_bIsInitOk = true;
	  return true;
	}
  }

  // No se pudo inicializar
  #ifdef ENGINE_TRACE    
     SYSEngine::GetLogger()->Write("CScript::Init> No se pudo inicializar el script \"%s\".\n", szScriptFileName.c_str());
  #endif 
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicia instancia relativa a un script de ambito global.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CScript::Init(void)
{
  // ¿Se intenta reinicializar?
  if (IsInitOk()) {
	// Finaliza
	End();
  }

  // Se obtiene handle a fichero con los scripts
  iCGameDataBase* const pGDBase = SYSEngine::GetGameDataBase();
  ASSERT(pGDBase);
  const FileDefs::FileHandle hFile = pGDBase->GetScriptsFileHandle();
  if (hFile) {
	// Se localiza el offset donde comenzar a leer el script global
	dword udOffset = pGDBase->GetGlobalScriptOffset();
	if (udOffset) {
	  // Se crea nodo para guardar la informacion
	  sCodeInfo* const pCodeInfo = new sCodeInfo;
	  ASSERT(pCodeInfo);
	  
	  // Signatura vacia (V)V
	  pCodeInfo->szSignature = "VV";
	  
	  // Lee cantidad de offsets (slots de memoria)
	  iCFileSystem* const pFileSys = SYSEngine::GetFileSystem();
	  ASSERT(pFileSys);
	  udOffset += pFileSys->Read(hFile, 
								 (sbyte *)(&pCodeInfo->uwNumOffsets),
								 sizeof(word),
								 udOffset);
	  
	  // Lee la posicion del primer offset
	  udOffset += pFileSys->Read(hFile,
						         (sbyte *)(&pCodeInfo->uwInitOffset),
								 sizeof(word),
								 udOffset);
	  
	  // Lee el codigo propiamente dicho, creando el vector de instrucciones
	  ReadCode(hFile, udOffset, pCodeInfo->Code);
	  
	  // Lee la tabla de strings
	  ReadStringTable(hFile, udOffset, pCodeInfo->StrTable);
	  
	  // Mapea la informacion recogida
	  // Nota: para el codigo del script global, el idx utilizado siempre sera 0
	  m_CodeInfo.insert(CodeInfoMapValType(0, pCodeInfo));	  
	}

	// Se establecen resto de vbles de miembro
	m_pGlobalScript = NULL;
	m_szScriptFile = "Global";
	m_Event = RulesDefs::SE_GLOBAL_SCRIPT;
	m_State = ScriptDefs::SS_INACTIVE;
	  
	// Todo correcto
	m_bIsInitOk = true;
	return true;
  }

  // No se pudo inicializar
  #ifdef ENGINE_TRACE    
     SYSEngine::GetLogger()->Write("CScript::Init> No se pudo inicializar el script global.\n");
  #endif 
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia liberando recursos de memoria
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CScript::End(void)
{
  // Finaliza si procede
  if (m_bIsInitOk) {
	// Finaliza el map de informacion sobre codigo
	CodeInfoMapIt MapIt(m_CodeInfo.begin());
	while (MapIt != m_CodeInfo.end()) {
	  // Se vacia trabla de strings
	  MapIt->second->StrTable.clear();

	  // Se borra vector de codigos
	  CodeVectorIt CodeIt(MapIt->second->Code.begin());
	  while (CodeIt != MapIt->second->Code.end()) {
		// Se borra nodo y entrada
		delete *CodeIt;
		CodeIt = MapIt->second->Code.erase(CodeIt);
	  }

	  // Se borra nodo y entrada
	  delete MapIt->second;
	  MapIt = m_CodeInfo.erase(MapIt);
	}
	
	// Finaliza pila
	StackVectorIt StackIt(m_RunTimeStack.begin());
	while (StackIt != m_RunTimeStack.end()) {
	  // Borra nodo y entrada
	  delete *StackIt;
	  StackIt = m_RunTimeStack.erase(StackIt);
	}

	// Resto vbles de miembro
    m_pGlobalScript = NULL;
	m_State = ScriptDefs::SS_INACTIVE;
	
	// Baja flag
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Lee la tabla de strings asociada a la porcion de codigo actual.
// Parametros:
// - hFile. Handle al fichero.
// - udOffset. Offset en el fichero.
// - StrTabla. Map donde alojar los strings
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CScript::ReadStringTable(const FileDefs::FileHandle& hFile,
						 dword& udOffset,
						 StrTableMap& StrTable)
{
  // SOLO si parametros correctos
  ASSERT(hFile);
  ASSERT(StrTable.empty());

  // Lee la cantidad de strings en tabla
  iCFileSystem* const pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys);
  dword udNumStrings;
  udOffset += pFileSys->Read(hFile, 
						     (sbyte *)(&udNumStrings), 
							 sizeof(dword), 
							 udOffset);

  // Procede a mapear los strings
  dword udIt = 0;
  for (; udIt < udNumStrings; ++udIt) {
	// Lee string y mapea
	std::string szString;
	udOffset += pFileSys->ReadStringFromBinary(hFile, udOffset, szString);
	StrTable.insert(StrTableMapValType(udIt, szString));
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Lee desde archivo el codigo perteneciente a una funcion o cuerpo de un
//   evento script, creando a su vez la lista de instrucciones.
// Parametros:
// - hFile. Handle al fichero.
// - udOffset. Offset en el fichero.
// - Code. Vector donde alojar el codigo
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CScript::ReadCode(const FileDefs::FileHandle& hFile,
				  dword& udOffset,
				  CodeVector& Code)
{
  // SOLO si los parametros son correctos
  ASSERT(hFile);
  ASSERT(Code.empty());

  // Lee el numero de instrucciones a leer
  iCFileSystem* const pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys);
  dword udNumInstructions;  
  udOffset += pFileSys->Read(hFile, 
							 (sbyte *)(&udNumInstructions), 
							 sizeof(dword), 
							 udOffset);

  // Se proceden a leer las instrucciones
  word uwIt = 0;
  Code.reserve(udNumInstructions);
  while (udNumInstructions--) {
	// Lee codigo de instruccion
	dword udInstrType;
	udOffset += pFileSys->Read(hFile, (sbyte *)(&udInstrType), sizeof(dword), udOffset);	

	// Segun sea el tipo de instruccion, se creara la instancia de codigo que
	// corresponda y se completara la lectura de informacion para inicializarla
	CScriptInstruction* pInstr;
	switch(udInstrType) {
	  case ScriptDefs::SI_NOP: {
		pInstr = new CNopInstr;
	  } break;
	
	  case ScriptDefs::SI_NNEG: {
		pInstr = new CNNegInstr;
	  } break;

	  case ScriptDefs::SI_NMUL: {
		pInstr = new CNMulInstr;
	  } break;

	  case ScriptDefs::SI_NADD: {
		pInstr = new CNAddInstr;
	  } break;

	  case ScriptDefs::SI_NMOD: {
		pInstr = new CNModInstr;
	  } break;

	  case ScriptDefs::SI_NDIV: {
		pInstr = new CNDivInstr;
	  } break;

	  case ScriptDefs::SI_NSUB: {
		pInstr = new CNSubInstr;
	  } break;

	  case ScriptDefs::SI_SADD: {
		pInstr = new CSAddInstr;
	  } break;

	  case ScriptDefs::SI_JMP: {
		pInstr = new CJmpInstr;
	  } break;

	  case ScriptDefs::SI_JMP_FALSE: {
		pInstr = new CJmpFalseInstr;
	  } break;

	  case ScriptDefs::SI_JMP_TRUE: {
		pInstr = new CJmpTrueInstr;
	  } break;

	  case ScriptDefs::SI_NJMP_EQ: {
		pInstr = new CNJmpEQInstr;
	  } break;

	  case ScriptDefs::SI_NJMP_NE: {
		pInstr = new CNJmpNEInstr;
	  } break;

	  case ScriptDefs::SI_NJMP_GE: {
		pInstr = new CNJmpGEInstr;
	  } break;

	  case ScriptDefs::SI_NJMP_GT: {
		pInstr = new CNJmpGTInstr;
	  } break;

	  case ScriptDefs::SI_NJMP_LT: {
		pInstr = new CNJmpLTInstr;
	  } break;

	  case ScriptDefs::SI_NJMP_LE: {
		pInstr = new CNJmpLEInstr;
	  } break;

	  case ScriptDefs::SI_SJMP_EQ: {
		pInstr = new CSJmpEQInstr;
	  } break;

	  case ScriptDefs::SI_SJMP_NE: {
		pInstr = new CSJmpNEInstr;
	  } break;

	  case ScriptDefs::SI_EJMP_EQ: {
		pInstr = new CEJmpEQInstr;
	  } break;

	  case ScriptDefs::SI_EJMP_NE: {
		pInstr = new CEJmpNEInstr;
	  } break;

	  case ScriptDefs::SI_DUP: {
		pInstr = new CDupInstr;
	  } break;

	  case ScriptDefs::SI_POP: {
		pInstr = new CPopInstr;
	  } break;

	  case ScriptDefs::SI_NRETURN:	  
	  case ScriptDefs::SI_SRETURN:
	  case ScriptDefs::SI_ERETURN:
	  case ScriptDefs::SI_RETURN: {
		pInstr = new CReturnInstr;
	  } break;

	  case ScriptDefs::SI_NLOAD: {
		pInstr = new CNLoadInstr;
	  } break;

	  case ScriptDefs::SI_SLOAD: {
		pInstr = new CSLoadInstr;
	  } break;

	  case ScriptDefs::SI_ELOAD: {
		pInstr = new CELoadInstr;
	  } break;

	  case ScriptDefs::SI_NSTORE: {
		pInstr = new CNStoreInstr;
	  } break;

	  case ScriptDefs::SI_SSTORE: {
		pInstr = new CSStoreInstr;
	  } break;

	  case ScriptDefs::SI_ESTORE: {
		pInstr = new CEStoreInstr;
	  } break;

	  case ScriptDefs::SI_NPUSH: {
		pInstr = new CNPushInstr;
	  } break;

	  case ScriptDefs::SI_SPUSH: {
		pInstr = new CSPushInstr;
	  } break;

	  case ScriptDefs::SI_EPUSH: {
		pInstr = new CEPushInstr;
	  } break;

	  case ScriptDefs::SI_NSCAST: {
		pInstr = new CNSCastInstr;
	  } break;

	  case ScriptDefs::SI_SNCAST: {
		pInstr = new CSNCastInstr;
	  } break;

	  case ScriptDefs::SI_CALL_FUNC: {
		pInstr = new CCallFuncInstr;
	  } break;

	  case ScriptDefs::SI_API_PASSTORGBCOLOR: {
		pInstr = new CAPIPassToRGBColorInstr;
	  } break;
	
	  case ScriptDefs::SI_API_GETREDCOMPONENT: {
		pInstr = new CAPIGetRedComponentInstr;
	  } break;

	  case ScriptDefs::SI_API_GETGREENCOMPONENT: {
	    pInstr = new CAPIGetGreenComponentInstr;
	 } break;

	  case ScriptDefs::SI_API_GETBLUECOMPONENT: {
	    pInstr = new CAPIGetBlueComponentInstr;
	  } break;

	  case ScriptDefs::SI_API_RAND: {
	    pInstr = new CAPIRandInstr;
	  } break;

	  case ScriptDefs::SI_API_GETINTEGERVALUE: {
	    pInstr = new CAPIGetIntegerValueInstr;
	  } break;
	
	  case ScriptDefs::SI_API_GETDECIMALVALUE: {
	    pInstr = new CAPIGetDecimalValueInstr;
	  } break;

	  case ScriptDefs::SI_API_GETSTRINGSIZE: {
	    pInstr = new CAPIGetStringSizeInstr;
	  } break;

	  case ScriptDefs::SI_API_WRITETOLOGGER: {
	    pInstr = new CAPIWriteToLoggerInstr;
	  } break;

	  case ScriptDefs::SI_API_ENABLECRISOLSCRIPTWARNINGS: {
	    pInstr = new CAPIEnableCrisolScriptWarningsInstr;
	  } break;
	
	  case ScriptDefs::SI_API_DISABLECRISOLSCRIPTWARNINGS: {
	    pInstr = new CAPIDisableCrisolScriptWarningsInstr;
	  } break;

	  case ScriptDefs::SI_API_SHOWFPS: {
	    pInstr = new CAPIShowFPSInstr;
	  } break;

	  case ScriptDefs::SI_API_WAIT: {
	    pInstr = new CAPIWaitInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_QUITGAME: {
	    pInstr = new CGOQuitGameInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_WRITETOCONSOLE: {
		pInstr = new CGOWriteToConsoleInstr;
	  } break;

      case ScriptDefs::SI_GAMEOBJ_ACTIVEADVICEDIALOG: {
	    pInstr = new CGOActiveAdviceDialogInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_ACTIVEQUESTIONDIALOG: {
	    pInstr = new CGOActiveQuestionDialogInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_ACTIVETEXTREADERDIALOG: {
	    pInstr = new CGOActiveTextReaderDialogInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_ADDOPTIONTOTEXTSELECTORDIALOG: {
	    pInstr = new CGOAddOptionToTextSelectorDialogInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_RESETOPTIONSINTEXTSELECTORDIALOG: {
	    pInstr = new CGOResetOptionsInTextSelectorDialogInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_ACTIVETEXTSELECTORDIALOG: {
	    pInstr = new CGOActiveTextSelectorDialogInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_PLAYMIDIMUSIC: {
	    pInstr = new CGOPlayMidiMusicInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_STOPMIDIMUSIC: {
	    pInstr = new CGOStopMidiMusicInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_PLAYWAVAMBIENTSOUND: {
	    pInstr = new CGOPlayWavAmbientSoundInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_STOPWAVAMBIENTSOUND: {
	    pInstr = new CGOStopWavAmbientSoundInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_ACTIVETRADEITEMSINTERFAZ: {
	    pInstr = new CGOActiveTradeItemsInterfazInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_ADDOPTIONTOCONVERSATORINTERFAZ: {
	    pInstr = new CGOAddOptionToConversatorInterfazInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_RESETOPTIONSINCONVERSATORINTERFAZ: {
	    pInstr = new CGOResetOptionsInConversatorInterfazInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_ACTIVECONVERSATORINTERFAZ: {
	    pInstr = new CGOActiveConversatorInterfazInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_DEACTIVECONVERSATORINTERFAZ: {
	    pInstr = new CGODeactiveConversatorInterfazInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_GETOPTIONFROMCONVERSATORINTERFAZ: {
	    pInstr = new CGOGetOptionFromConversatorInterfazInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_SHOWPRESENTATION: {
	    pInstr = new CGOShowPresentationInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_BEGINCUTSCENE: {
	    pInstr = new CGOBeginCutSceneInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_ENDCUTSCENE: {
	    pInstr = new CGOEndCutSceneInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_SETSCRIPT: {
	    pInstr = new CGOSetScriptInstr;
	  } break;

	  case ScriptDefs::SI_GAMEOBJ_ISKEYPRESSED: {
	    pInstr = new CGOIsKeyPressedInstr;
	  } break;
	  
	  case ScriptDefs::SI_WORLDOBJ_GETAREANAME: {
	    pInstr = new CWOGetAreaNameInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_GETAREAID: {
	    pInstr = new CWOGetAreaIDInstr;
	  } break;
	  
	  case ScriptDefs::SI_WORLDOBJ_GETAREAWIDTH: {
	    pInstr = new CWOGetAreaWidthInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_GETAREAHEIGHT: {
	    pInstr = new CWOGetAreaHeightInstr;
	  } break;

  	  case ScriptDefs::SI_WORLDOBJ_GETHOUR: {
		pInstr = new CWOGetHourInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_GETMINUTE: {
		pInstr = new CWOGetMinuteInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_SETHOUR: {
	    pInstr = new CWOSetHourInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_SETMINUTE: {
	    pInstr = new CWOSetMinuteInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_GETENTITY: {
	    pInstr = new CWOGetEntityInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_GETPLAYER: {
	    pInstr = new CWOGetPlayerInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_ISFLOORVALID: {
	    pInstr = new CWOIsFloorValidInstr;
	  } break;
	  
	  case ScriptDefs::SI_WORLDOBJ_GETITEMAT: {
	    pInstr = new CWOGetItemAtInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_GETNUMITEMSAT: {		
	    pInstr = new CWOGetNumItemsAtInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_GETDISTANCE: {
	    pInstr = new CWOGetDistanceInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_CALCULEPATHLENGHT: {
	    pInstr = new CWOCalculePathLenghtInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_LOADAREA: {
	    pInstr = new CWOLoadAreaInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_CHANGEENTITYLOCATION: {
	    pInstr = new CWOChangeEntityLocationInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_ATTACHCAMERATOENTITY: {
	    pInstr = new CWOAttachCameraToEntityInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_ATTACHCAMERATOLOCATION: {
	    pInstr = new CWOAttachCameraToLocationInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_ISCOMBATMODEACTIVE: {
	    pInstr = new CWOIsCombatModeActiveInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_ENDCOMBAT: {
	    pInstr = new CWOEndCombatInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_GETCRIATUREINCOMBATTURN: {
	    pInstr = new CWOGetCriatureInCombatTurnInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_GETCOMBATANT: {
	    pInstr = new CWOGetCombatantInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_GETNUMBEROFCOMBATANTS: {
	    pInstr = new CWOGetNumberOfCombatantsInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_GETAREALIGHTMODEL: {
	    pInstr = new CWOGetAreaLightModelInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_SETSCRIPT: {
	    pInstr = new CWOSetScriptInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_SETIDLESCRIPTTIME: {
	    pInstr = new CWOSetIdleScriptTimeInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_DESTROYENTITY: {
		pInstr = new CWODestroyEntityInstr;
	  } break;
  
	  case ScriptDefs::SI_WORLDOBJ_CREATECRIATURE: {
		pInstr = new CWOCreateCriatureInstr;
	  } break;
	  
	  case ScriptDefs::SI_WORLDOBJ_CREATEWALL: {
		pInstr = new CWOCreateWallInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_CREATESCENARYOBJECT: {
		pInstr = new CWOCreateScenaryObjectInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_CREATEITEMABANDONED: {
		pInstr = new CWOCreateItemAbandonedInstr;
	  } break;
  
	  case ScriptDefs::SI_WORLDOBJ_CREATEITEMWITHOWNER: {
		pInstr = new CWOCreateItemWithOwnerInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_SETWORLDTIMEPAUSE: {
		pInstr = new CWOSetWorldTimePauseInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_ISWORLDTIMEINPAUSE: {
		pInstr = new CWOIsWorldTimeInPauseInstr;
	  } break;

  	  case ScriptDefs::SI_WORLDOBJ_GETELEVATIONAT: {
	    pInstr = new CWOGetElevationAtInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_SETELEVATIONAT: {
		pInstr = new CWOSetElevationAtInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_NEXTTURN: {
		pInstr = new CWONextTurnInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_GETLIGHTAT: {
		pInstr = new CWOGetLightAtInstr;
	  } break;

  	  case ScriptDefs::SI_WORLDOBJ_PLAYWAVSOUND: {
		pInstr = new CWOPlayWAVSoundInstr;
	  } break;

	  case ScriptDefs::SI_WORLDOBJ_SETSCRIPTAT: {
		pInstr = new CWOSetScriptAtInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETNAME: {
	    pInstr = new CEOGetNameInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETNAME: {
	    pInstr = new CEOSetNameInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETENTITYTYPE: {
	    pInstr = new CEOGetEntityTypeInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETTYPE: {
	    pInstr = new CEOGetTypeInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SAY: {
	    pInstr = new CEOSayInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SHUTUP: {
	    pInstr = new CEOShutUpInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_ISSAYING: {
	    pInstr = new CEOIsSayingInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_ATTACHGFX: {
	    pInstr = new CEOAttachGFXInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_RELEASEGFX: {
	    pInstr = new CEOReleaseGFXInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_RELEASEALLGFX: {
	    pInstr = new CEOReleaseAllGFXInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_ISGFXATTACHED: {
	    pInstr = new CEOIsGFXAttachedInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETNUMITEMSINCONTAINER: {
	    pInstr = new CEOGetNumItemsInContainerInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETITEMFROMCONTAINER: {
	    pInstr = new CEOGetItemFromContainerInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_ISITEMINCONTAINER: {
	    pInstr = new CEOIsItemInContainerInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_TRANSFERITEMTOCONTAINER: {
	    pInstr = new CEOTransferItemToContainerInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_INSERTITEMINCONTAINER: {
	    pInstr = new CEOInsertItemInContainerInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_REMOVEITEMOFCONTAINER: {
	    pInstr = new CEORemoveItemOfContainerInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETANIMTEMPLATESTATE: {
	    pInstr = new CEOSetAnimTemplateStateInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETPORTRAITANIMTEMPLATESTATE: {
	    pInstr = new CEOSetPortraitAnimTemplateStateInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETIDLESCRIPTTIME: {
	    pInstr = new CEOSetIdleScriptTimeInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETLIGHT: {
	    pInstr = new CEOSetLightInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETLIGHT: {
	    pInstr = new CEOGetLightInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETXPOS: {
	    pInstr = new CEOGetXPosInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETYPOS: {
	    pInstr = new CEOGetYPosInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETELEVATION: {
	    pInstr = new CEOGetElevationInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETELEVATION: {
	    pInstr = new CEOSetElevationInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETLOCALATTRIBUTE: {
	    pInstr = new CEOGetLocalAttributeInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETLOCALATTRIBUTE: {
	    pInstr = new CEOSetLocalAttributeInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETOWNER: {
	    pInstr = new CEOGetOwnerInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETCLASS: {
	    pInstr = new CEOGetClassInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETINCOMBATUSECOST: {
	    pInstr = new CEOGetInCombatUseCostInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETGLOBALATTRIBUTE: {
	    pInstr = new CEOGetGlobalAttributeInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETGLOBALATTRIBUTE: {
	    pInstr = new CEOSetGlobalAttributeInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETWALLORIENTATION: {
	    pInstr = new CEOGetWallOrientationInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_BLOCKACCESS: {
	    pInstr = new CEOBlockAccessInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_UNBLOCKACCESS: {
	    pInstr = new CEOUnblockAccessInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_ISACCESSBLOCKED: {
	    pInstr = new CEOIsAccessBlockedInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETSYMPTOM: {
	    pInstr = new CEOSetSymptomInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_ISSYMPTOMACTIVE: {
	    pInstr = new CEOIsSymptomActiveInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETGENRE: {
	    pInstr = new CEOGetGenreInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETHEALTH: {
	    pInstr = new CEOGetHealthInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETHEALTH: {
	    pInstr = new CEOSetHealthInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETEXTENDEDATTRIBUTE: {
	    pInstr = new CEOGetExtendedAttributeInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETEXTENDEDATTRIBUTE: {
	    pInstr = new CEOSetExtendedAttributeInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETLEVEL: {
	    pInstr = new CEOGetLevelInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETLEVEL: {
	    pInstr = new CEOSetLevelInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETEXPERIENCE: {
	    pInstr = new CEOGetExperienceInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETEXPERIENCE: {
	    pInstr = new CEOSetExperienceInstr;
	  } break;
	
	  case ScriptDefs::SI_ENTITYOBJ_GETINCOMBATACTIONPOINTS: {
	    pInstr = new CEOGetInCombatActionPointsInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETACTIONPOINTS: {
	    pInstr = new CEOGetActionPointsInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETACTIONPOINTS: {
	    pInstr = new CEOSetActionPointsInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_ISHABILITYACTIVE: {
	    pInstr = new CEOIsHabilityActiveInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETHABILITY: {
	    pInstr = new CEOSetHabilityInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_USEHABILITY: {
	    pInstr = new CEOUseHabilityInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_ISRUNMODEACTIVE: {
	    pInstr = new CEOIsRunModeActiveInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETRUNMODE: {
	    pInstr = new CEOSetRunModeInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_MOVETO: {
	    pInstr = new CEOMoveToInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_ISMOVING: {
	    pInstr = new CEOIsMovingInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_STOPMOVING: {
	    pInstr = new CEOStopMovingInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_EQUIPITEM: {
	    pInstr = new CEOEquipItemInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_REMOVEITEMEQUIPPED: {
	    pInstr = new CEORemoveItemEquippedInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETITEMEQUIPPED: {
	    pInstr = new CEOGetItemEquippedInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_ISITEMEQUIPPED: {
	    pInstr = new CEOIsItemEquippedInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_DROPITEM: {
	    pInstr = new CEODropItemInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_USEITEM: {
	    pInstr = new CEOUseItemInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_MANIPULATE: {
	    pInstr = new CEOManipulateInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETTRANSPARENTMODE: {
	    pInstr = new CEOSetTransparentModeInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_ISTRANSPARENTMODEACTIVE: {
	    pInstr = new CEOIsTransparentModeActiveInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_CHANGEANIMORIENTATION: {
	    pInstr = new CEOChangeAnimOrientationInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETANIMORIENTATION: {
	    pInstr = new CEOGetAnimOrientationInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETALINGMENT: {
	    pInstr = new CEOSetAlingmentInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETALINGMENTWITH: {
	    pInstr = new CEOSetAlingmentWithInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETALINGMENTAGAINST: {
	    pInstr = new CEOSetAlingmentAgainstInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETALINGMENT: {
	    pInstr = new CEOGetAlingmentInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_HITENTITY: {
	    pInstr = new CEOHitEntityInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETSCRIPT: {
	    pInstr = new CEOSetScriptInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_ISGHOSTMOVEMODEACTIVE: {
	    pInstr = new CEOIsGhostMoveModeActiveInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_SETGHOSTMOVEMODE: {
	    pInstr = new CEOSetGhostMoveModeInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_GETRANGE: {
	    pInstr = new CEOGetRangeInstr;
	  } break;

	  case ScriptDefs::SI_ENTITYOBJ_ISINRANGE: {
	    pInstr = new CEOIsInRangeInstr;
	  } break;
	  	  
	  default: {
		SYSEngine::FatalError("CScript> Código de script %u no válido.\n",
		                      udInstrType);
		pInstr = NULL;
	  } break;
	}; // ~ switch

	// Se inicializa e inserta en el vector de codigo
	ASSERT(pInstr);
	pInstr->Init(hFile, udOffset);
	ASSERT(pInstr->IsInitOk());
	Code.push_back(pInstr);
  } // ~ while  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina el ultimo elemento de la pila (debe de existir al menos un
//   elemento) y lo retorna.
// Parametros:
// Devuelve:
// - La direccion al elemento que estaba en el tope de la pila.
// Notas:
///////////////////////////////////////////////////////////////////////////////
CScriptStackValue*
CScript::Pop(void)
{
  // SOLO si instancia inicializa
  ASSERT(IsInitOk());
  
  // Procede a tomar el ultimo elemento, eliminandolo de la pila
  ASSERT(!m_RunTimeStack.empty());
  CScriptStackValue* const pValue = m_RunTimeStack.back();
  m_RunTimeStack.pop_back();
 
  // Retorna
  return pValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de quitar un Stack Frame de la pila. Este metodo se llamara
//   siempre que se haya regresado de la ejecucion de una porcion de codigo.
//   Habra que tener en cuenta, que en caso de que la porcion de codigo regrese
//   un valor, en la parte final de la pila este se hallara.
// - Cuando el Stack Frame extraido sea el ultimo, pasara el estado del script
//   a inactivo, esto permitira que el metodo RunCode pueda tomar el valor que
//   haya en la pila (si es un evento script) y devolverlo como un boolean
//   para indicar si hacer o no la accion por defecto.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CScript::PopStackFrame(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se toman el numero de slots a quitar y el inicio del Stack Frame actual
  // Nota: Sera el de Stack Frame Header y los de memoria
  word uwNumSlotsToDelete = 4 + m_Registers.CodeInfoIt->second->uwNumOffsets;
  dword udIt = m_Registers.udStackFramePos;  
  ASSERT(((m_RunTimeStack.size() - udIt) >= uwNumSlotsToDelete) != 0);  
  const bool bLastStackFrame = (udIt == 0);
  
  // Se toman los valores del Stack Frame Header
  CScriptStackValue* const pActSFHPos = m_RunTimeStack[udIt];
  ASSERT(pActSFHPos);
  CScriptStackValue* const pActCodeIdx = m_RunTimeStack[udIt + 1];
  ASSERT(pActCodeIdx);
  CScriptStackValue* const pActCodePos = m_RunTimeStack[udIt + 2];
  ASSERT(pActCodePos);
  CScriptStackValue* const pActLocalMemPos = m_RunTimeStack[udIt + 3];
  ASSERT(pActLocalMemPos);

  // Se restaura los registros previos
  m_Registers.udStackFramePos = pActSFHPos->GetDWordValue();
  m_Registers.CodeInfoIt = m_CodeInfo.find(pActCodeIdx->GetDWordValue());
  m_Registers.udCodePos = pActCodePos->GetDWordValue();
  m_Registers.udLocalsPos = pActLocalMemPos->GetDWordValue();
  
  // Se procede a eliminar el contenido relativo al Stack Frame actual
  StackVectorIt SFIt(m_RunTimeStack.begin());
  std::advance(SFIt, udIt);
  while (uwNumSlotsToDelete--) {
	delete *SFIt;
	SFIt = m_RunTimeStack.erase(SFIt);
  }
  
  // ¿Era el ultimo Stack Frame?
  if (bLastStackFrame) {
	// Si, se establece estado inactivo
	m_State = ScriptDefs::SS_INACTIVE;
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coloca el Stack Frame de la porcion de codigo de indice uwCodeIdx.
//   Este metodo se ejecutara siempre que se desee lanzar una porcion de codigo.
// - Colocar el Stack Frame supondra dejar tres zonas en la pila para la porcion
//   de codigo a ejecutar. La primera porcion estara destinada a la cabecera que
//   almacenara la posicion del Stack Frame anterior, la posicion donde comienzan
//   los slots de memoria local del Stack Frame anterior y la posicion en el
//   vector de codigo a ejecutar en donde el Stack Frame anterior se encontraba.
//   A continuacion vendra la zona reservada a la memoria local y despues la
//   utilizada para operaciones intermedias.
// Parametros:
// - uwCodeIdx. Indice a la porcion de codigo para la que situar el 
//   Stack Frame.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CScript::PushStackFrame(const word uwCodeIdx)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  #ifdef _SYSASSERT
	if (!uwCodeIdx && !m_RunTimeStack.empty()) {
	  ASSERT_MSG(false, "El codigo inicial del script solo se ejecuta al comienzo");
	}
  #endif

  // Obtiene nodo de informacion
  const CodeInfoMapIt CodeIt(m_CodeInfo.find(uwCodeIdx));
  ASSERT((CodeIt != m_CodeInfo.end()) != 0);

  // Obtiene el numero de parametros que tiene el codigo a ejecutar
  const word uwNumParams = GetNumParams(CodeIt->second->szSignature);
  
  // ¿NO hay suficiente espacio para el nuevo Stack Frame?
  if (m_RunTimeStack.max_size() < 
	  m_RunTimeStack.size() + 4 + CodeIt->second->uwNumOffsets - uwNumParams + CodeIt->second->uwMaxStackSize) {
	// No se puede establecer el Stack Frame
	#ifdef ENGINE_TRACE    
	   SYSEngine::GetLogger()->Write("CScript::Execute> No hay espacio en pila para llamar a código.\n");
	   SYSEngine::GetLogger()->Write("                | * Script: %s\n", m_szScriptFile.c_str());
	   SYSEngine::GetLogger()->Write("                | * Idx. del código: %u\n", uwCodeIdx);
	#endif 
	return false;
  }

  // Obtiene iterador donde colocar el header y su pos. relativa
  StackVectorIt HeaderInsertIt(m_RunTimeStack.begin()); 
  dword udSFHeaderPos;
  
  // ¿Codigo de funcion local / importada?
  if (uwCodeIdx > 0) {
	// Si, se establece posicion
	// Nota: En caso de que tenga parametros, se descontaran
	ASSERT((m_RunTimeStack.size() >= uwNumParams) != 0);
	udSFHeaderPos = m_RunTimeStack.size() - uwNumParams;	
	std::advance(HeaderInsertIt, udSFHeaderPos);
  } else {
	// No, cuerpo de script al comienzo
	udSFHeaderPos = 0;
	HeaderInsertIt = m_RunTimeStack.end(); 
  }  

  // Establecen los datos del Stack Frame header.
  // Nota: En caso de que se ponga el Stack Frame de una porcion de codigo 0,
  // no habra porcion de codigo anterior a esta, por lo que los valores nunca
  // seran utilizados al quitar el Stack Frame con PopStackFrame
  CScriptStackValue* const pPrevSFHPos = new CScriptStackValue(m_Registers.udStackFramePos);
  ASSERT(pPrevSFHPos);
  CScriptStackValue* const pPrevCodeIdx = new CScriptStackValue(dword(uwCodeIdx ? m_Registers.CodeInfoIt->first : 0));
  ASSERT(pPrevCodeIdx);
  CScriptStackValue* const pPrevCodePos = new CScriptStackValue(m_Registers.udCodePos);
  ASSERT(pPrevCodePos);
  CScriptStackValue* const pPrevLocalMemPos = new CScriptStackValue(m_Registers.udLocalsPos);
  ASSERT(pPrevLocalMemPos);
  
  // Se establecen valores actuales de registros
  m_Registers.CodeInfoIt = CodeIt;
  m_Registers.udCodePos = 0;
  m_Registers.udStackFramePos = udSFHeaderPos;
  m_Registers.udLocalsPos = udSFHeaderPos + 4;
  
  // Se inserta el StackFrame Header y obtiene el num. de slots de memoria
  // ¿Llamada al cuerpo del script?
  word uwMemSlots;
  if (HeaderInsertIt != m_RunTimeStack.end()) {
	// No, es una llamada a una funcion perteneciente a un script
	HeaderInsertIt = m_RunTimeStack.insert(HeaderInsertIt, pPrevLocalMemPos);
	HeaderInsertIt = m_RunTimeStack.insert(HeaderInsertIt, pPrevCodePos);
	HeaderInsertIt = m_RunTimeStack.insert(HeaderInsertIt, pPrevCodeIdx);
	m_RunTimeStack.insert(HeaderInsertIt, pPrevSFHPos);		

	// Establece slots de memoria
	// Nota: Al ser una llamada a una funcion, los parametros no se consideraran
	// al estar estos depositados en la pila.
	uwMemSlots = m_Registers.CodeInfoIt->second->uwNumOffsets - uwNumParams;	
  } else {
	// Si, es una llamada al cuerpo del script
	m_RunTimeStack.push_back(pPrevSFHPos);
	m_RunTimeStack.push_back(pPrevCodeIdx);
	m_RunTimeStack.push_back(pPrevCodePos);
	m_RunTimeStack.push_back(pPrevLocalMemPos);
	
	// Establece slots de memoria 
	// Nota: Se consideraran los parametos	
	uwMemSlots = m_Registers.CodeInfoIt->second->uwNumOffsets;
  }
  
  // Se inicializa a valores nulos, la porcion de la pila destinada a la 
  // zona de memoria local y retorna
  while (uwMemSlots--) {
	m_RunTimeStack.push_back(NULL);
  }
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece un valor number en la memoria. Para ello, obtendra el iterador
//   a un slot del vector. Si en esa zona hay valor NULL, se creara una
//   estructura que le de soporte.
// Parametros:
// - uwOffset. Posicion de memoria donde se halla el elemento.
// - fValue. Valor number
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CScript::SetValueAt(const word uwOffset,
					const float fValue)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Obtiene iterador al elemento de la pila con el valor
  StackVectorIt SlotIt(GetValueSlotIt(uwOffset, m_Registers.udStackFramePos));
  if (m_RunTimeStack.end() == SlotIt) {
	// No se hallo, se realiza operacion en el ambito global
	ASSERT(m_pGlobalScript);
	m_pGlobalScript->SetValueAt(uwOffset, fValue);
  }

  // ¿Estructura NO creada?
  if (NULL == (*SlotIt)) {
	// Se crea
	*SlotIt = new CScriptStackValue(fValue);
	ASSERT(*SlotIt)
  } else {
	// Se cambia
	**SlotIt = fValue;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece un valor entity en la memoria. Para ello, obtendra el iterador
//   a un slot del vector. Si en esa zona hay valor NULL, se creara una
//   estructura que le de soporte.
// Parametros:
// - uwOffset. Posicion de memoria donde se halla el elemento.
// - hValue. Valor entity
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CScript::SetValueAt(const word uwOffset,
					const AreaDefs::EntHandle& hValue)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());  

  // Obtiene iterador al elemento de la pila con el valor
  StackVectorIt SlotIt(GetValueSlotIt(uwOffset, m_Registers.udStackFramePos));
  if (m_RunTimeStack.end() == SlotIt) {
	// No se hallo, se realiza operacion en el ambito global
	ASSERT(m_pGlobalScript);
	m_pGlobalScript->SetValueAt(uwOffset, hValue);
  }

  // ¿Estructura NO creada?
  if (!(*SlotIt)) {
	// Se crea
	*SlotIt = new CScriptStackValue(dword(hValue));
	ASSERT(*SlotIt)
  } else {
	// Se cambia
	**SlotIt = dword(hValue);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece un valor string en la memoria. Para ello, obtendra el iterador
//   a un slot del vector. Si en esa zona hay valor NULL, se creara una
//   estructura que le de soporte.
// Parametros:
// - uwOffset. Posicion de memoria donde se halla el elemento.
// - szValue. Valor string a introducir.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CScript::SetValueAt(const word uwOffset, 
					const std::string& szValue)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());  

  // Obtiene iterador al elemento de la pila con el valor
  StackVectorIt SlotIt(GetValueSlotIt(uwOffset, m_Registers.udStackFramePos));
  if (m_RunTimeStack.end() == SlotIt) {
	// No se hallo, se realiza operacion en el ambito global
	ASSERT(m_pGlobalScript);
	m_pGlobalScript->SetValueAt(uwOffset, szValue);
  }

  // ¿Estructura NO creada?
  if (!(*SlotIt)) {
	// Se crea
	*SlotIt = new CScriptStackValue(szValue);
	ASSERT(*SlotIt)
  } else {
	// Se cambia
	**SlotIt = szValue;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la direccion del valor que se halla en la posicion de memoria
//   con offset uwOffset.
// Parametros:
// - uwOffset. Posicion de memoria donde se halla el elemento.
// Devuelve:
// - Direccion a dicho elemento.
// Notas:
///////////////////////////////////////////////////////////////////////////////
CScriptStackValue* const
CScript::GetValueAt(const word uwOffset)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Obtiene iterador al elemento de la pila con el valor
  //SYSEngine::GetLogger()->Write("Toma valor desde: %u\n", m_Registers.udStackFramePos);
  StackVectorIt SlotIt(GetValueSlotIt(uwOffset, m_Registers.udStackFramePos));
  if (m_RunTimeStack.end() == SlotIt) {
	// No se hallo, se busca en el ambito global
	ASSERT(m_pGlobalScript);
	return m_pGlobalScript->GetValueAt(uwOffset);
  }

  // Se retorna
  return *SlotIt;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el iterador a un slot de la pila, que correspondera con una zona
//   de memoria local de un Stack Frame asociado a la posicion udStackFramePos.
//   En caso de no hallar nada en dicha posicion, se comprobara en el ambito
//   global (si es una porcion de codigo script o funcion global) o bien en
//   el Stack Frame del script si es una funcion local. No podra fallar cuando
//   se trate de un ambito global.
// Parametros:
// - uwOffset. Offset donde localizar el slot de memoria.
// - udStackFramePos. Posicion donde se halla el comienzo del Stack Frame en
//   donde buscar.
// Devuelve:
// - Un iterador a la zona de memoria donde esta el slot con el valor asociado
//   al offset recibido. En caso de retorna iterador no valido (end()) se 
//   comunicara al exterior que se debe de localizar el valor en el entorno
//   global.
// Notas:
///////////////////////////////////////////////////////////////////////////////
CScript::StackVectorIt 
CScript::GetValueSlotIt(const word uwOffset,
						const dword udStackFramePos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT((m_RunTimeStack.size() > udStackFramePos) != 0);

  // ¿Estara en este Stack Frame el valor buscado?
  if (uwOffset >= m_Registers.CodeInfoIt->second->uwInitOffset) {
	// Si, se calcula el iterador a esa posicion y se retorna
	StackVectorIt StackIt(m_RunTimeStack.begin());
	std::advance(StackIt, udStackFramePos + 4 + uwOffset - m_Registers.CodeInfoIt->second->uwInitOffset);
	ASSERT((StackIt != m_RunTimeStack.end()) != 0);
	return StackIt;
  } else {
	// No, ¿La porcion de codigo actual es de una funcion local?
	if (m_Registers.CodeInfoIt->second->Type == sCodeInfo::LOCAL_FUNC) {
	  // ¿La vble esta dentro del cuerpo del script del que depende la funcion?
	  CodeInfoMapIt ScriptIt(m_CodeInfo.find(0));
	  ASSERT((ScriptIt != m_CodeInfo.end()) != 0);		
	  if (uwOffset >= ScriptIt->second->uwInitOffset) {
		// Si, se localiza teniendo en cuenta que la posicion de la que se parte es 0
		StackVectorIt StackIt(m_RunTimeStack.begin());
		std::advance(StackIt, 4 + uwOffset - ScriptIt->second->uwInitOffset);
		ASSERT((StackIt != m_RunTimeStack.end()) != 0);
		return StackIt;
	  }
	}
  }

  // No se pudo encontrar valor, se retorna iterador no valido para que
  // desde el exterior se busque en el ambito global
  return m_RunTimeStack.end();
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo llamado para iniciar la primera ejecucion, por lo que se debera de
//   estar en modo inactivo.
// Parametros:
// - Params. Lista de posibles parametros a utilizar por el script.
// Devuelve:
// - Si despues de la ejecucion ha finalizado esta, un valor que nos indicara
//   si ejecutar (true) o no (false) la accion por defecto. En el resto de los
//   casos y cuando se ejecute el script global, false.
// Notas:
// - Segun se vayan tomando los parametros, estos seran quitados de la lista
//   de parametros recibidos.
// - El tipo string como parametro no se contemplara ya que no habra ningun
//   evento script que lo reciba.
// - El ambito global NO tendra parametros.
///////////////////////////////////////////////////////////////////////////////
bool
CScript::Execute(ScriptDefs::ScriptParamList& Params)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Establece el Stack Frame inicial
  ASSERT((m_State == ScriptDefs::SS_INACTIVE) != 0);
  PushStackFrame(0);

  // ¿Es un script de evento?
  if (m_pGlobalScript) {	
	// Se obtiene la sucesion de parametros que se deberan de esperar
	// Nota: Esta informacion estara en la porcion de codigo 0
	//SYSEngine::GetLogger()->Write("COMENZAMOS A INSERTAR PARAMETROS ENVIADOS %u\n",Params.size());
	word uwMemSlot = m_Registers.CodeInfoIt->second->uwInitOffset;
	ScriptDefs::ScriptParamListIt ParamsIt(Params.begin());
	for (; ParamsIt != Params.end(); ++ParamsIt, ++uwMemSlot) {	
	  // Segun sea el tipo del parametro, asi se establecera	  
	  // Nota: Los parametros de los scripts se estableceran poniendolos con push_back
	  ASSERT((ParamsIt != Params.end()) != 0);
	  switch(ParamsIt->Type) {
		case ScriptDefs::sScriptParam::PT_ENTITY: {	
		  SetValueAt(uwMemSlot, ParamsIt->hEntity);
		} break;

		case ScriptDefs::sScriptParam::PT_NUMBER: {		
		  SetValueAt(uwMemSlot, ParamsIt->fNumber);
		} break;

		default:
		  ASSERT(false);
	  }; // ~ switch
	}
  }

  // Se establece estado de ejecucion y se proceden a ejecutar instrucciones
  m_State = ScriptDefs::SS_RUNNING;
  return RunCode();      
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Bucle de ejecucion del codigo. Procedera a ejecutar todo el codigo 
//   intermedio mientras no se pase a estado de pausa o bien mientras no
//   finalice 
// Parametros:
// Devuelve:
// - Si se ha de realizar la accion por defecto true, en caso contrario 
//   false. Esto solo tendra sentido cuando el estado del script sea inactivo
//   pues en el resto de los casos se retornara false siempre.
// Notas:
// - Un script finalizara cuando al hacer PopStackFrame solo quede un
//   StackFrame.
///////////////////////////////////////////////////////////////////////////////
bool
CScript::RunCode(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // ¿Hay codigo que ejecutar?
  if (!m_Registers.CodeInfoIt->second->Code.empty()) {
	// Si, se ejecutan codigos mientras no se este en pausa o inactivo  
	while (ScriptDefs::SS_RUNNING == m_State) {
	  // Se ejecutan
	  m_Registers.CodeInfoIt->second->Code[m_Registers.udCodePos++]->Execute(this);	  
	}
  } else {
	// No, se hace como si se hubiera ejecutado
	m_State = ScriptDefs::SS_INACTIVE;
  }

  // ¿Finalizo la ejecucion?
  if (ScriptDefs::SS_INACTIVE == m_State) {
	// Si, se retorna el valor del tope de la pila SOLO si este
	// script no pertenece al entorno global
	if (m_pGlobalScript) {
	  ASSERT((m_RunTimeStack.size() == 1) != 0);
	  CScriptStackValue* const pReturn = m_RunTimeStack[0];
	  ASSERT(pReturn);
	  m_RunTimeStack.pop_back();
	  const bool bReturn = (pReturn->GetFloatValue() >= 1.0f);
	  delete pReturn;
	  return bReturn;
	} 
  }

  // Se esta en modo pausa o bien termino script global
  return false;
}