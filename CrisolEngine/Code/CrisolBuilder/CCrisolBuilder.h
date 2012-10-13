///////////////////////////////////////////////////////////////////////////////
// CrisolBuilder - CrisolEngine's Config files processor
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
// CCrisolBuilder.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CCrisolBuilder
//
// Descripcion:
// - Clase que encarna la logica de la utilidad CrisolBuilder, aqui se 
//   procesara el archivo de configuracion para la utilidad y se transformaran
//   los archivos .cbt en archivos .cbb que seran los mismos pero en formato
//   binario.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CCRISOLBUILDER_H_
#define _CCRISOLBUILDER_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _SYSDEFS_H_
#include "..\\SYSDefs.h"
#endif
#ifndef _CCBTTOOLPARSER_H_
#include "..\\CCBTToolParser.h"
#endif
#ifndef _CRULESDATABASE_H_
#include "..\\CRulesDataBase.h"
#endif
#ifndef _FSTREAM_H_
#define _FSTREAM_H_
#include <fstream>
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif
#ifndef _SET_H_
#define _SET_H_
#include <set>
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CCrisolBuilder
class CCrisolBuilder
{
private:
  // Enumerados
  enum eMsgType {
	// Tipos de mensaje
	MSG_WARNING, // Advertencia
	MSG_ERROR,   // Error
	MSG_NORMAL   // Normal
  };

private:
  // Tipos
  // Lista de strings para guardar secciones
  typedef std::list<std::string> StringList;
  typedef StringList::iterator   StringListIt;
  // Lista para guardar posiciones
  typedef std::list<word>       FilePosList;
  typedef FilePosList::iterator FilePosListIt;
  // Lista de posiciones en stream para guardar offsets
  typedef std::list<std::streampos> StreamPosList;
  typedef StreamPosList::iterator   StreamPosListIt;
  // Map de identificadores de habitacion
  typedef std::map<std::string, AreaDefs::RoomID> IdRoomMap;
  typedef IdRoomMap::iterator					  IdRoomMapIt;
  typedef IdRoomMap::const_iterator				  IdRoomMapConstIt;
  typedef IdRoomMap::value_type					  IdRoomMapValType;
  // Set de strings
  typedef std::set<std::string> StrSet;
  typedef StrSet::iterator      StrSetIt;

private:
  // Estructuras
  struct sMsgInfo {
	// Info asociada a los mensajes
	std::ofstream MsgFile;         // Fichero donde enviar los posibles mensajes  
	word		  uwNumErrors;     // Contador de errores
	word		  uwNumWarnings;   // Contador de advertencias
  };

  struct sSectionSets {
	// Nombre del perfil de presentacion para los creditos
	std::string szCreditsPresentationProfile;
  };

  struct sParamsInfo {
	// Parametros leidos desde el archivo de configuracion
	bool bWriteMsgInFile;         // ¿Escribir los mensajes en archivo?	
	bool bReportAnimTemplates;    // ¿Informar de los perfiles de plantillas de anim.?
	bool bReportCriatureProfiles; // ¿Informar de los perfiles de criaturas?
	bool bReportItemProfiles;     // ¿Informar de los perfiles de items?
	bool bReportWallProfiles;     // ¿Informar de los perfiles de paredes?
	bool bReportSceneObjProfiles; // ¿Informar de los perfiles de obj. de escenario?
	bool bReportFloorProfiles;    // ¿Informar de los perfiles de suelos?
	bool bReportRoofProfiles;     // ¿Informar de los perfiles de techos?	
  };

private:
  // Vbles de miembro
  CRulesDataBase m_RulesDataBase;  // Reglas
  std::string    m_szInitFileName; // Nombre del archivo de inicializacion  
  sMsgInfo		 m_MsgInfo;        // Info asociada a mensajes
  sParamsInfo    m_ParamsInfo;     // Parametros leidos desde archivo .ini
  sSectionSets   m_SectionSets;    // Conjuntos de secciones
  bool			 m_bIsInitOk;      // ¿Instancia inicializada correctamente?
   
public:
  // Constructor / Destructor
  CCrisolBuilder(void): m_bIsInitOk(false) { }
  ~CCrisolBuilder(void) { 
	End(); 
  }
  
public:
  // Protocolo de inicio y fin de instancia
  bool Init(const std::string& szInitFileName);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }
private:
  // Metodos de apoyo
  void InitMsgMap(void);

public:
  // Construccion de archivos de salida
  bool Build(void);
private:
  // Metodos de apoyo
  // Construccion de areas
  bool BuildAreaFiles(const word uwNumAreas);
  void WriteAreaFloorInfo(CCBTToolParser& CBTParser,
						  std::ofstream& CBBFile,
						  const IdRoomMap& RoomMap,
						  StrSet& TagSet);
  bool WriteAreaRoofInfo(CCBTToolParser& CBTParser,
						 std::ofstream& CBBFile);
  bool WriteAreaSceneObjInfo(CCBTToolParser& CBTParser,
							 std::ofstream& CBBFile,
							 StrSet& TagSet);
  bool WriteAreaCriatureInfo(CCBTToolParser& CBTParser,
						     std::ofstream& CBBFile,
							 StrSet& TagSet);
  bool WriteAreaWallInfo(CCBTToolParser& CBTParser,
						 std::ofstream& CBBFile,
						 StrSet& TagSet);
  void WriteAreaLightInfo(CCBTToolParser& CBTParser,
						  std::ofstream& CBBFile,
						  const std::string szVar);
  void WriteAreaTagInfo(CCBTToolParser& CBTParser,
						std::ofstream& CBBFile,
						StrSet& TagSet,
						const std::string& szTmpPrefix = "");

private:
  // Chequeo del archivo de perfil de entidades
  bool BuildFloorProfilesFile(const std::string& szCBTFile);
  bool BuildWallProfilesFile(const std::string& szCBTFile);
  bool BuildSceneObjProfilesFile(const std::string& szCBTFile);
  bool BuildItemProfilesFile(const std::string& szCBTFile);  
  bool BuildCriatureProfilesFile(const std::string& szCBTFile);
  bool BuildRoofProfilesFile(const std::string& szCBTFile);
private:
  // Metodos de apoyo
  void WriteEntityScriptEvents(const RulesDefs::eScriptEvents* const pScriptEvents,
						 const byte NumEvents,
						 CCBTToolParser& CBTParser,
						 std::ofstream& CBBFile);

private:
  // Construccion del archivo de configuracion de interfaces
  bool CheckInterfazFile(const std::string& szCBTFile);
  bool CheckInterfazCursors(CCBTToolParser& Parser);
  bool CheckInterfazSelectors(CCBTToolParser& Parser);
  bool CheckInterfazAdviceDialog(CCBTToolParser& Parser);
  bool CheckInterfazQuestionDialog(CCBTToolParser& Parser);
  bool CheckInterfazTextFileReader(CCBTToolParser& Parser);
  bool CheckInterfazTextOptionsSelector(CCBTToolParser& Parser);
  bool CheckInterfazMainMenu(CCBTToolParser& Parser);
  bool CheckInterfazLoaderSavedFilesManager(CCBTToolParser& Parser);
  bool CheckInterfazSaverSavedFilesManager(CCBTToolParser& Parser);
  bool CheckInterfazGameLoading(CCBTToolParser& Parser);
  bool CheckInterfazEntityInteract(CCBTToolParser& Parser);
  bool CheckInterfazInventorySelector(CCBTToolParser& Parser);
  bool CheckInterfazCombatientInfo(CCBTToolParser& Parser);
  bool CheckInterfazGameMenu(CCBTToolParser& Parser);
  bool CheckInterfazConversationWindow(CCBTToolParser& Parser);
  bool CheckInterfazTradeItemsManager(CCBTToolParser& Parser);
  bool CheckInterfazMainGameInterfaz(CCBTToolParser& Parser);
  bool CheckInterfazPlayerProfileWindow(CCBTToolParser& Parser);
  bool CheckInterfazPlayerCreatorWindow(CCBTToolParser& Parser);
private:
  // Metodos de apoyo
  void CheckBackImgComponent(CCBTToolParser& Parser,
							 const bool bWithAnims);
  void CheckOptionsButtonComponents(CCBTToolParser& Parser,
								    const std::string* pszOptions,
									const word uwNumOptions);

private:
  // Chequeo del archivo de texto estatico
  bool CheckStaticTextFile(const std::string& szCBTFile);
private:
  // Metodos de apoyo
  void CheckSecuenceOfStaticText(CCBTToolParser& Parser,
								 const word uwMaxValue,
								 const std::string& szSeqVar);

private:
  //  Chequeo del archivo de perfiles del jugador
  bool CheckPlayerProfilesFile(const std::string& szCBTFile,
							   const word uwNumAreas);

private:
  // Chequeo del archivo de scripts globales
  bool CheckGlobalScriptEventsFile(const std::string& szCBTFile);

private:
  // Construccion del archivo de definiciones GFX
  bool CheckGFXDefsFile(const std::string& szCBTFile);

private:
  // Construccion del archivo de presentaciones
  bool CheckPresentationsFile(const std::string& szCBTFile);

private:
  // Construccion del archivo de plantillas de animacion
  bool BuildAnimTemplateFile(const std::string& szCBTFile);

private:
  // Trabajo con el proceso de enviar mensajes
  void WriteMsg(const std::string& szMsg,
				const eMsgType& Type);

public:
  inline word GetNumErrors(void) const {
	ASSERT(IsInitOk());
	// Retorna valor
	return m_MsgInfo.uwNumErrors;
  }
  inline word GetNumWarnings(void) const {
	ASSERT(IsInitOk());
	// Retorna valor
	return m_MsgInfo.uwNumWarnings;
  }

private:
  // Ajuste de valores
  inline bool CheckValue(const float fValue,
						 const float fMinValue,
						 const float fMaxValue) const {
	ASSERT(IsInitOk());
	// Retorna valor
	return (fValue < fMinValue || fValue > fMaxValue) ? false : true;
  }

private:
  // Metodos de apoyo en escritura
  bool InitBuild(CCBTToolParser& CBTParser,
			     std::ofstream& CBBFile,
				 const std::string& szInputFileName,
				 const std::string& szOutputFileName);
  void WriteIndexOfSections(std::ofstream& CBBFile,
							StrSet& SetOfSections,
							StreamPosList& ListOfOffsets);
  
  void WriteString(std::ofstream& CBBFile,
				   const std::string& szString);
};

#endif // ~ CCrisolBuilder


