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
// CGameDataBase.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGameDataBase.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGameDataBase.h"

#include "SYSEngine.h"
#include "iCLogger.h"
#include "iCFileSystem.h"
#include "CMemoryPool.h"
#include "CCBTEngineParser.h"
#include "CrisolBuilder\\CBDefs.h"
#include <algorithm>

// Inicializacion de la unica instancia singlenton
CGameDataBase* CGameDataBase::m_pGameDataBase = NULL;

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa recibiendo el nombre del archivo con la configuracion de
//   inicializacion del motor.
// Parametros:
// - szInitFileName. Nombre del archivo con la configuracion del motor.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar
// - El nombre del archivo de reglas y el de scripts, se supondra que son
//   "Rules.cbt" y "CrisolGameScripts.csb" respectivamente.
///////////////////////////////////////////////////////////////////////////////
bool 
CGameDataBase::Init(const std::string& szInitFileName)
{
  // SOLO si parametros validos
  ASSERT(szInitFileName.size());

  // ¿Se intenta reinicializar?
  if (IsInitOk()) {
	return false;
  }

  #ifdef ENGINE_TRACE  
     SYSEngine::GetLogger()->Write("CGameDataBase::Init> Inicializando base de datos del juego.\n");
  #endif
  
  // Se abre fichero de inicializacion y se situa en seccion apropiada
  CCBTEngineParser Parser;
  if (Parser.Init(szInitFileName)) {
	if (Parser.SetSection("[GameDataBase]")) {
	  // Se toman interfaces a otros subsistemas
	  m_pFileSys = SYSEngine::GetFileSystem();
	  ASSERT(m_pFileSys);
	
	  // Se inicializan ficheros CBB	  
	  Parser.SetVarPrefix("Files.CBB.");
	  if (InitCBBFileInfo(GameDataBaseDefs::CBBF_ANIMTEMPLATES,
						  Parser.ReadString("AnimTemplates")) &&
		  InitCBBFileInfo(GameDataBaseDefs::CBBF_FLOORPROFILES,
						  Parser.ReadString("FloorProfiles")) &&
		  InitCBBFileInfo(GameDataBaseDefs::CBBF_WALLPROFILES,
						  Parser.ReadString("WallProfiles")) &&
		  InitCBBFileInfo(GameDataBaseDefs::CBBF_SCENEOBJPROFILES,
						  Parser.ReadString("SceneObjProfiles")) &&
		  InitCBBFileInfo(GameDataBaseDefs::CBBF_ITEMPROFILES,
						  Parser.ReadString("ItemProfiles")) &&
		  InitCBBFileInfo(GameDataBaseDefs::CBBF_CRIATUREPROFILES,
						  Parser.ReadString("CriatureProfiles")) &&
		  InitCBBFileInfo(GameDataBaseDefs::CBBF_ROOFPROFILES,
						  Parser.ReadString("RoofProfiles"))) {
	
		// Se toma el posible path para los archivos de area
		m_szAreaFilesPath = Parser.ReadString("AreaFilesPath", false);

		// Inicializa archivos .CBT
		// Nota: El archivo .ini siempre tendra el mismo nombre
		Parser.SetVarPrefix("Files.CBT.");
		if (InitCBTFileInfo(GameDataBaseDefs::CBTF_CRISOLENGINE_INI, 
							"CrisolEngine.ini") &&
			InitCBTFileInfo(GameDataBaseDefs::CBTF_STATIC_TEXT, 
							Parser.ReadString("StaticText") + ".cbt") &&
			InitCBTFileInfo(GameDataBaseDefs::CBTF_PLAYER_PROFILES, 
							Parser.ReadString("PlayerProfiles") + ".cbt") &&
			InitCBTFileInfo(GameDataBaseDefs::CBTF_INTERFACES_CFG, 
							Parser.ReadString("InterfacesCfg") + ".cbt") &&
			InitCBTFileInfo(GameDataBaseDefs::CBTF_GFX_DEFS, 
							Parser.ReadString("GFXDefs") + ".cbt") &&
			InitCBTFileInfo(GameDataBaseDefs::CBTF_PRESENTATION_PROFILES, 
							Parser.ReadString("PresentationProfiles") + ".cbt") &&
			InitCBTFileInfo(GameDataBaseDefs::CBTF_GLOBALSCRIPTEVENTS, 
							Parser.ReadString("GlobalScriptEvents") + ".cbt")) {

		  // Se cargan las reglas
		  if (m_RulesDataBase.Init(Parser.ReadString("Rules") + ".cbt")) {
			// Se lee el nombre del fichero asociado al TileRegionMask
			Parser.SetVarPrefix("Files.");
			m_szTileRegionMaskFileName = Parser.ReadString("TileRegionMask");

			// Se crean directorios temporales y de guardar partidas si no existen
			CreateDirectory("tmp", NULL);
			CreateDirectory("save", NULL);

			// Se inicializa estructura de informacion sobre los archivos script
			if (InitScriptFileInfo(Parser.ReadString("Scripts") + ".csb")) {
			  // Todo correcto
			  m_bIsInitOk = true;
			  #ifdef ENGINE_TRACE  
				 SYSEngine::GetLogger()->Write("                   | Ok.\n");
			  #endif
			  return true;
			}
		  }
		}
	  }
	}
  }  

  // Hubo problemas
  #ifdef ENGINE_TRACE  
     SYSEngine::GetLogger()->Write("                   | Error.\n");
  #endif
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia.
// Parametros:
// Devuelve:
// - true: Si todo ha tenido exito. Y false en caso contrario.
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGameDataBase::End(void)
{
  // Finaliza
  if (IsInitOk()) {
	#ifdef ENGINE_TRACE  
       SYSEngine::GetLogger()->Write("CGameDataBase::End> Finalizando base de datos del juego.\n");
    #endif

	// Finaliza reglas
	m_RulesDataBase.End();

	// Se finalizan ficheros CBB y CBT
	EndCBBFilesInfo();
	EndCBTFilesInfo();

	// Finaliza fichero de scripts
	EndScriptFileInfo();

	// Desvincula interfaces
    m_pFileSys = NULL;

	// Se finaliza 
	m_bIsInitOk = false;
	#ifdef ENGINE_TRACE  
       SYSEngine::GetLogger()->Write("                   | Ok.\n");
    #endif
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el parser CBT que proceda.
// Parametros:
// - CBTFile. Parser CBT a inicializar.
// - szFileInfo. Nombre del fichero CBT a inicializar
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGameDataBase::InitCBTFileInfo(const GameDataBaseDefs::eCBTFile& CBTFile,
							   const std::string& szFileInfo)
{
  // SOLO si parametros validos
  ASSERT((CBTFile < GameDataBaseDefs::CBTF_MAXFILES) != 0);
  ASSERT(!szFileInfo.empty());

  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("                   | Inicializando parser asociado a fichero: %s.\n", szFileInfo.c_str());  
  #endif

  // Se inicializa
  if (!m_CBTFiles[CBTFile].Init(szFileInfo)) {
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("                   | Problemas\n");
	#endif
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la informacion relativa a los ficheros CBB. Esta informacion
//   se referira a:
//    * Mapeo de los offsets en donde hallar a las secciones.
//    * Obtencion de handle a los archivos segun tipo.
// Parametros:
// - CBBFile. Fichero CBB a inicializar.
// - szFileInfo. Nombre / path del fichero
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
// - Consultar CrisolBuilder para conocer mejor el formato de cada fichero
// - Los ficheros .cbb tendran siempre un nombre especifico.
///////////////////////////////////////////////////////////////////////////////
bool
CGameDataBase::InitCBBFileInfo(const GameDataBaseDefs::eCBBFile& CBBFile,
							   const std::string& szFileInfo)
{
  // SOLO si parametros validos
  ASSERT((CBBFile < GameDataBaseDefs::CBBF_MAXFILES) != 0);
  ASSERT(!szFileInfo.empty());

  // Se obtiene el nombre del fichero con el que trabajar y el codigo del mismo
  byte ubActFileType;
  switch(CBBFile) {
	case GameDataBaseDefs::CBBF_ANIMTEMPLATES: {
	  ubActFileType = CBDefs::CBBAnimTemplateFile;	  
	} break;

	case GameDataBaseDefs::CBBF_FLOORPROFILES: {
	  ubActFileType = CBDefs::CBBFloorProfilesFile;
	} break;

	case GameDataBaseDefs::CBBF_WALLPROFILES: {
	  ubActFileType = CBDefs::CBBWallProfilesFile;
	} break;

	case GameDataBaseDefs::CBBF_SCENEOBJPROFILES: {
	  ubActFileType = CBDefs::CBBSceneObjProfilesFile;
	} break;

  	case GameDataBaseDefs::CBBF_ITEMPROFILES: {
	  ubActFileType = CBDefs::CBBItemProfilesFile;
	} break;

	case GameDataBaseDefs::CBBF_CRIATUREPROFILES: {
	  ubActFileType = CBDefs::CBBCriatureProfilesFile;
	} break;

  	case GameDataBaseDefs::CBBF_ROOFPROFILES: {
	  ubActFileType = CBDefs::CBBRoofProfilesFile;
	} break;
	
	default:
	  ASSERT_MSG(false, "El tipo es incorrecto");
  };

  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("                   | Registrando archivo: %s.\n", szFileInfo.c_str());  
  #endif

  // Se obtiene el nombre del fichero
  // Se abre el fichero de plantillas de animacion
  m_CBBFiles[CBBFile].hFile = m_pFileSys->Open(szFileInfo + ".cbb");
  if (!m_CBBFiles[CBBFile].hFile) {
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("                   | Problemas\n");
	#endif
	return false;	
  } 

  // Se comprueba que el codigo del archivo sea el correcto
  ASSERT(m_pFileSys);
  byte ubFileType;
  m_pFileSys->Read(m_CBBFiles[CBBFile].hFile, (sbyte *)(&ubFileType), sizeof(byte));
  if (ubActFileType != ubFileType) {
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("                   | El archivo no es del tipo esperado.\n");
	#endif
	return false;
  }

  // Se lee la posicion donde se halla el indice de offsets
  // Nota: La vble Offset se usara a partir de ahora para ir localizando las
  // posiciones y despues de cada lectura sera incrementada.
  std::streampos Offset;
  m_pFileSys->Read(m_CBBFiles[CBBFile].hFile, (sbyte *)(&Offset), sizeof(std::streampos), sizeof(byte) * 3);
  
  // Se lee el numero de perfiles 
  word uwValue;
  Offset += m_pFileSys->Read(m_CBBFiles[CBBFile].hFile, (sbyte *)(&uwValue), sizeof(word), Offset);
  
  // Se procede a leer todos los perfiles y a mapearlos
  std::streampos ProfileOffset = 0;
  sbyte* pStrBuff = NULL;
  word uwStrBuffSize = 0;
  for (; uwValue; --uwValue) {
	// Se offset asociado al perfil
	Offset += m_pFileSys->Read(m_CBBFiles[CBBFile].hFile, (sbyte *)(&ProfileOffset), sizeof(std::streampos), Offset);
	
	// Se lee cadena y se mapea pasandola antes a minusculas
	dword udOffset = Offset;
	std::string szIDStrLower;
	Offset += m_pFileSys->ReadStringFromBinary(m_CBBFiles[CBBFile].hFile,
											   udOffset,
											   szIDStrLower);	
	SYSEngine::MakeLowercase(szIDStrLower);		
	m_CBBFiles[CBBFile].SectionMap.insert(FileOffsetIndexMapValType(szIDStrLower, ProfileOffset));	

	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("                   | * Perfil: %s\n", szIDStrLower.c_str());  
	#endif
  }

  // Se sale liberando buffer para string
  delete[uwStrBuffSize] pStrBuff;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la info asociada a los ficheros CBB
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGameDataBase::EndCBBFilesInfo(void)
{
  // Recorre y libera handles a ficheros y maps
  word uwIt = 0;
  for (; uwIt < GameDataBaseDefs::CBBF_MAXFILES; ++uwIt) {
	if (m_CBBFiles[uwIt].hFile) {
	  m_pFileSys->Close(m_CBBFiles[uwIt].hFile);
	  m_CBBFiles[uwIt].hFile = 0;
	  m_CBBFiles[uwIt].SectionMap.clear();	  
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre y finaliza los parsers asociados
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGameDataBase::EndCBTFilesInfo(void)
{
  // Recorre y libera handles a ficheros y maps
  word uwIt = 0;
  for (; uwIt < GameDataBaseDefs::CBTF_MAXFILES; ++uwIt) {	
	m_CBTFiles[uwIt].End();
  }
}  

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa base de datos relativa a los ficheros scripts.
// Parametros:
// - szFileName. Nombre del fichero con los scripts.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGameDataBase::InitScriptFileInfo(const std::string& szFileName)
{
  // SOLO si parametros correctos
  ASSERT(szFileName.size());

  #ifdef ENGINE_TRACE
  	SYSEngine::GetLogger()->Write("                   | Registrando archivo de ficheros scripts:\n");  
  #endif

  // Intenta abrir el fichero
  m_ScriptFile.hFile = m_pFileSys->Open(szFileName);
  if (m_ScriptFile.hFile) {
	// Existe fichero script
	// Lee encabezado / version	
	dword udFileOffset = 0;
	byte ubVersion;
	udFileOffset = m_pFileSys->Read(m_ScriptFile.hFile, 
									(sbyte *)(&ubVersion), 
									sizeof(byte));
	ASSERT_MSG((ubVersion == 1) != 0, "Versión mayor del fichero de scripts no reconocida");
	udFileOffset += m_pFileSys->Read(m_ScriptFile.hFile, 
									 (sbyte *)(&ubVersion), 
									 sizeof(byte),
									 udFileOffset);
	ASSERT_MSG((ubVersion == 0) != 0, "Versión menor del fichero de scripts no reconocida");

	// Lee offset a la tabla de indices de scripts
	sdword sdScriptIndexOffset;
	udFileOffset += m_pFileSys->Read(m_ScriptFile.hFile,
									 (sbyte *)(&sdScriptIndexOffset),
									 sizeof(sdword),
									 udFileOffset);

	// A continuacion viene la info relativa al script global, se guarda offset.
	m_ScriptFile.GlobalScriptOffset = udFileOffset;	

	// Procede a leer la tabla de scripts
	// Lee el numero de scripts
	word uwNumScripts;
	udFileOffset = sdScriptIndexOffset;
	udFileOffset += m_pFileSys->Read(m_ScriptFile.hFile,
								     (sbyte *)(&uwNumScripts),
									 sizeof(word),
									 udFileOffset);

	// Mapea los offsets a script
  	word uwIt = 0;
	for (; uwIt < uwNumScripts; ++uwIt) {
	  // Se lee el string
	  std::string szScriptFileName;
	  udFileOffset += m_pFileSys->ReadStringFromBinary(m_ScriptFile.hFile, 
													   udFileOffset, 
													   szScriptFileName);
	  // Lee el offset
	  sdword sdOffsetToScript;
	  udFileOffset += m_pFileSys->Read(m_ScriptFile.hFile,
								       (char *)(&sdOffsetToScript),
									   sizeof(sdword),
									   udFileOffset);
	  // Se mapea
	  SYSEngine::MakeLowercase(szScriptFileName);
	  m_ScriptFile.ScriptsMap.insert(FileOffsetIndexMapValType(szScriptFileName, sdOffsetToScript));
	  #ifdef ENGINE_TRACE
		SYSEngine::GetLogger()->Write("                   | * Registrando fichero script \"%s\".\n", szScriptFileName.c_str());  
	  #endif
	}	
  } else {
	// No se hallo fichero de scripts
	m_ScriptFile.GlobalScriptOffset = 0;
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("                   | * No hay scripts.\n");  
	#endif
  }

  // No existieron problemas
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la informacion relacionada con el fichero script
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGameDataBase::EndScriptFileInfo(void)
{
  // Finaliza
  if (m_ScriptFile.hFile) {
	m_ScriptFile.ScriptsMap.end();
	m_pFileSys->Close(m_ScriptFile.hFile);
	m_ScriptFile.hFile = 0;
  }  
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve un parser de tipo CBT, habiendolo situado antes en la seccion
//   szSection.
// Parametros:
// - File. Fichero de datos de texto.
// - szSection. Nombre de la seccion
// Devuelve:
// - Direccion del parser si es que se ha podido colocar en la seccion recibida.
//   En caso contrario NULL.
// Notas:
///////////////////////////////////////////////////////////////////////////////
CCBTEngineParser* const 
CGameDataBase::GetCBTParser(const GameDataBaseDefs::eCBTFile& File,
							const std::string& szSection)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT((File < GameDataBaseDefs::CBTF_MAXFILES) != 0);

  // Se obtiene el parser y se situa en su seccion
  ASSERT(m_CBTFiles[File].IsInitOk());
  if (m_CBTFiles[File].SetSection(szSection)) {
	return &m_CBTFiles[File];
  }

  // No se pudo establecer seccion
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retorna el handle asociado al fichero CBB CBBFile.
// Parametros:
// - CBBFile. Fichero CBB del que retonar el handle
// Devuelve:
// - El handle al fichero
// Notas:
///////////////////////////////////////////////////////////////////////////////
FileDefs::FileHandle 
CGameDataBase::GetCBBFileHandle(const GameDataBaseDefs::eCBBFile& CBBFile)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  ASSERT((CBBFile < GameDataBaseDefs::CBBF_MAXFILES) != 0);

  // Se retorna handle
  return m_CBBFiles[CBBFile].hFile;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Localiza el offset asociado a un perfil para un tipo de fichero CBB y lo
//   retorna.
// Parametros:
// - CBBFile. Tipo de fichero .cbb.
// - szSection. Section de la que recuperar el offset, que podra ser un
//   perfil o cualquier otro tipo de etiqueta acorde al fichero sobre el que
//   trabajar.
// Devuelve:
// - El handle situado al fichero .
// Notas:
///////////////////////////////////////////////////////////////////////////////
dword
CGameDataBase::GetCBBFileOffset(const GameDataBaseDefs::eCBBFile& CBBFile,
							    const std::string& szSection)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  ASSERT(!szSection.empty());
  ASSERT((CBBFile < GameDataBaseDefs::CBBF_MAXFILES) != 0);

  // Se establecera el nombre de la seccion y luego se pasara a minusculas
  ASSERT(!szSection.empty());  
  std::string szIDStrLower = "[Profile<" + szSection + ">]";
  SYSEngine::MakeLowercase(szIDStrLower);

  // Se retorna
  const FileOffsetIndexMapIt It(m_CBBFiles[CBBFile].SectionMap.find(szIDStrLower));
  ASSERT_MSG((It != m_CBBFiles[CBBFile].SectionMap.end()) != 0, szIDStrLower.c_str());
  return It->second;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si una seccion asociado a un fichero existe o no.
// Parametros:
// - CBBFile. Tipo de fichero .cbb.
// - szSection. Section de la que recuperar el offset, que podra ser un
//   perfil o cualquier otro tipo de etiqueta acorde al fichero sobre el que
//   trabajar.
// Devuelve:
// - El handle situado al fichero .
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGameDataBase::IsValidCBBFileSection(const GameDataBaseDefs::eCBBFile& CBBFile,
									 const std::string& szSection)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  ASSERT(!szSection.empty());
  ASSERT((CBBFile < GameDataBaseDefs::CBBF_MAXFILES) != 0);

  // Se establecera el nombre de la seccion y luego se pasara a minusculas
  ASSERT(!szSection.empty());  
  std::string szIDStrLower = "[Profile<" + szSection + ">]";
  SYSEngine::MakeLowercase(szIDStrLower);

  // Se retorna
  const FileOffsetIndexMapIt It(m_CBBFiles[CBBFile].SectionMap.find(szIDStrLower));
  return (It != m_CBBFiles[CBBFile].SectionMap.end());
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el offset a un script de evento. En caso de que no exista el 
//   fichero con los scripts o bien no se halle ese script especificado,
//   se retornara 0, realizando un aviso via logger.
// Parametros:
// - szScriptFile. Nombre del fichero script.
// Devuelve:
// - La posicion donde hallarlo o 0 si no es posible.
// Notas:
///////////////////////////////////////////////////////////////////////////////
std::streampos 
CGameDataBase::GetEventScriptOffset(const std::string& szScriptFile)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(!szScriptFile.empty());

  // Pasa a minusculas e intenta localizar el script en el map
  std::string szLowerScriptFile(szScriptFile);
  SYSEngine::MakeLowercase(szLowerScriptFile);
  const FileOffsetIndexMapIt It(m_ScriptFile.ScriptsMap.find(szLowerScriptFile));
  if (It != m_ScriptFile.ScriptsMap.end()) {
	// Se retorna offset
	return It->second;
  }

  // No se hallo
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el texto estatico requerido
// Parametros:
// - StaticText. Texto estatico.
// - uwIdxMain. Indice principal (por defecto a 0).
// - uwIdxAux. Indice auxiliar (por defecto a 0).
// Devuelve:
// - El texto que se pidio. En caso de no hallarse se devolvera la cadena
//   "¡StaticTextNoFound!"
// Notas:
// - Los indices principal y auxiliar se utilizaran en algunos casos para
//   concretar la informacion a leer. En otros no sera necesario
///////////////////////////////////////////////////////////////////////////////
std::string 
CGameDataBase::GetStaticText(const GameDataBaseDefs::eStaticText& StaticText,
							 const word uwIdxMain,
							 const word uwIdxAux)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Vbles
  std::string szSection; // Seccion
  std::string szVar;     // Nombre de variable

  // Se toma alias al parser
  CCBTEngineParser& Parser =  m_CBTFiles[GameDataBaseDefs::CBTF_STATIC_TEXT];
  
  // Se carga el texto estatico que proceda
  bool bMustExist = true;
  switch (StaticText) {
	case GameDataBaseDefs::ST_CRIATURE_TYPE: {
	  szSection = "[CriatureTypes]";
	  SYSEngine::PassToString(szVar, "Type[%u]", uwIdxMain);	  	  
	} break;

	case GameDataBaseDefs::ST_CRIATURE_CLASS: {
	  szSection = "[CriatureClasses]";
	  SYSEngine::PassToString(szVar, "Class[%u]", uwIdxMain);
	} break;

	case GameDataBaseDefs::ST_CRIATURE_GENRE: {
	  szSection = "[CriatureGenres]";
	  SYSEngine::PassToString(szVar, "Genre[%u]", uwIdxMain);
	} break;

	case GameDataBaseDefs::ST_CRIATURE_EXT_ATTRIB: {
	  szSection = "[CriatureAttributes]";
	  SYSEngine::PassToString(szVar, "ExtendedAttribute[%u]", uwIdxMain);	
	} break;

	case GameDataBaseDefs::ST_CRIATURE_EQUIPMENT_SLOT: {
	  szSection = "[CriatureEquipmentSlots]";
	  SYSEngine::PassToString(szVar, "EquipmentSlot[%u]", uwIdxMain);	
	} break;

	case GameDataBaseDefs::ST_CRIATURE_LEVEL: {
	  szSection = "[CriatureAttributes]";
	  szVar = "Level";
	} break;

	case GameDataBaseDefs::ST_CRIATURE_EXPERIENCE: {
	  szSection = "[CriatureAttributes]";
	  szVar = "Experience";
	} break;

	case GameDataBaseDefs::ST_CRIATURE_HEALTH: {
	  szSection = "[CriatureAttributes]";
	  szVar = "Health";
	} break;

	case GameDataBaseDefs::ST_CRIATURE_COMBATPOINTS: {
	  szSection = "[CriatureAttributes]";
	  szVar = "CombatPoints";
	} break;

	case GameDataBaseDefs::ST_CRIATURE_HABILITY: {
	  szSection = "[CriatureHabilities]";
	  SYSEngine::PassToString(szVar, "Hability[%u]", uwIdxMain);	
	} break;

	case GameDataBaseDefs::ST_CRIATURE_SYMPTOMS: {
	  szSection = "[CriatureSymptoms]";
	  SYSEngine::PassToString(szVar, "Symptom[%u]", uwIdxMain);
	} break;

	case GameDataBaseDefs::ST_INTERFAZ_MAINMENU_VERSIONINFO: {
	  // El texto de version, sera el unico no obligatorio
	  szSection = "[Interfaces]";
	  szVar = "MainMenu.VersionInfo";
	  bMustExist = false;
	} break;

	case GameDataBaseDefs::ST_INTERFAZ_SAVEDFILEMANAGER_SAVEDFILEDESC: {
	  szSection = "[Interfaces]";
	  szVar = "SaverSavedFileManager.SavedFileDesc";
	} break;

	case GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_MAINMENU_OBSERVE: {
	  szSection = "[Interfaces]";
	  szVar = "EntityInteract.MainMenu.Option[Observe]";
	} break;

	case GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_MAIMENU_TALK: {
	  szSection = "[Interfaces]";
	  szVar = "EntityInteract.MainMenu.Option[Talk]";
	} break;
	
	case GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_MAINMENU_GET: {
	  szSection = "[Interfaces]";
	  szVar = "EntityInteract.MainMenu.Option[Get]";
	} break;
	
	case GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_MAINMENU_REGISTER: {
	  szSection = "[Interfaces]";
	  szVar = "EntityInteract.MainMenu.Option[Register]";
	} break;
	
	case GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_MAINMENU_MANIPULATE: {
	  szSection = "[Interfaces]";
	  szVar = "EntityInteract.MainMenu.Option[Manipulate]";
	} break;

	case GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_MAINMENU_DROPITEM: {
	  szSection = "[Interfaces]";
	  szVar = "EntityInteract.MainMenu.Option[DropItem]";
	} break;
	
	case GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_MAINMENU_USEINVENTORY: {
	  szSection = "[Interfaces]";
	  szVar = "EntityInteract.MainMenu.Option[UseInventory]";
	} break;
	
	case GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_MAINMENU_USEHABILITY: {
	  szSection = "[Interfaces]";
	  szVar = "EntityInteract.MainMenu.Option[UseHability]";
    } break;

	case GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_MAINMENU_EXIT: {
	  szSection = "[Interfaces]";
	  szVar = "EntityInteract.MainMenu.Option[Exit]";
	} break;
	
	case GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_HABILITYMENU_HABILITY: {
	  szSection = "[Interfaces]";
	  // Se obtiene el identificador asociado a la habilidad
	  const std::string szHability(m_pGameDataBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_HABILITY,
																					   uwIdxMain));
	  szVar = "EntityInteract.HabilityMenu.Option[" + szHability + "]";
	} break;
	
	case GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_HABILITYMENU_RETURN: {
	  szSection = "[Interfaces]";
	  szVar = "EntityInteract.HabilityMenu.Option[ReturnToMainMenu]";
	} break;

	case GameDataBaseDefs::ST_INTERFAZ_PLAYERCREATION_NAME: {
	  szSection = "[Interfaces]";
	  szVar = "PlayerCreation.Name";
	} break;
	
	case GameDataBaseDefs::ST_INTERFAZ_PLAYERCREATION_GENRE: {
	  szSection = "[Interfaces]";
	  szVar = "PlayerCreation.Genre";
	} break;
	
	case GameDataBaseDefs::ST_INTERFAZ_PLAYERCREATION_TYPE: {
	  szSection = "[Interfaces]";
	  szVar = "PlayerCreation.Type";
	} break;
	
	case GameDataBaseDefs::ST_INTERFAZ_PLAYERCREATION_CLASS: {
	  szSection = "[Interfaces]";
	  szVar = "PlayerCreation.Class";
	} break;

	case GameDataBaseDefs::ST_GENERAL_INSUFICIENTACTIONPOINTS: {
	  szSection = "[Messages]";
	  szVar = "InsuficientActionPoints";
	} break;

	case GameDataBaseDefs::ST_GENERAL_AREA_LOADING: {
	  szSection = "[Messages]";
	  szVar = "AreaLoading";
	} break;

	case GameDataBaseDefs::ST_GENERAL_CANT_MOVE_TO: {
	  szSection = "[Messages]";
	  szVar = "CantMoveTo";
	} break;	  

	case GameDataBaseDefs::ST_GENERAL_GAME_LOADING: {
	  szSection = "[Messages]";
	  szVar = "GameLoading";
	} break;

	case GameDataBaseDefs::ST_GENERAL_NO_SAVEGAME_PERMISSION: {
	  szSection = "[Messages]";
	  szVar = "NoSaveGamePermission";
	} break;

	case GameDataBaseDefs::ST_GENERAL_EXIT_GAME_CONFIRM: {
	  szSection = "[Messages]";
	  szVar = "ExitGameConfirm";
	} break;

	default:
	  ASSERT_MSG(false, "El texto estatico a cargar no existe");
  }; // ~ switch

  // Se carga el texto
  Parser.SetSection(szSection);
  Parser.SetVarPrefix("");
  const std::string szStaticText(Parser.ReadString(szVar, bMustExist));

  // Retorna el texto
  return Parser.WasLastParseOk() ? szStaticText : "";  
}