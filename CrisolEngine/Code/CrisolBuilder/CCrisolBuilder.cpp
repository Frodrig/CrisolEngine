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
// CCrisolBuilder.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CCrisolBuilder.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CCrisolBuilder.h"

#include <iostream>
#include "..\\AnimTemplateDefs.h"
#include "..\\RulesDefs.h"
#include "..\\IsoDefs.h"
#include "..\\AreaDefs.h"
#include "..\\GraphDefs.h"
#include "..\\AudioDefs.h"
#include "..\\CRulesDataBase.h"
#include "CBDefs.h"
#include <algorithm>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia, tomando los valores especificados en el archivo
//   de configuracion.
// Parametros:
// - szInitFileName.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::Init(const std::string& szInitFileName)
{
  // SOLO si parametros validos
  ASSERT(!szInitFileName.empty());

  // ¿Se intenta reinicializar?
  if (IsInitOk()) {
	return false;
  }

  // Se toman datos asociados a la configuracion
  CCBTToolParser Parser;
  m_ParamsInfo.bWriteMsgInFile = false;
  WriteMsg("Procesando...", CCrisolBuilder::MSG_NORMAL);		
  if (Parser.Init(szInitFileName)) {
	// Se leen flags de configuracion
	if (!Parser.SetSection("[Params]")) {
	  WriteMsg(Parser.GetFileName() + " / No se halló la sección [Params]", CCrisolBuilder::MSG_ERROR);
	  return false;
	}

	// Flag de escritura de mensajes en disco
	Parser.SetVarPrefix("");	
	m_ParamsInfo.bWriteMsgInFile = Parser.ReadFlag("WriteMsgInFileFlag", false);
	if (m_ParamsInfo.bWriteMsgInFile) {
	  // Se desea enviar mensajes a archivo, se abre
	  m_MsgInfo.MsgFile.open("CBOutput.txt");
	  if (m_MsgInfo.MsgFile.fail()) {
	    return false;
	  }
	}

	// Flags de escritura de perfiles localizados
	Parser.SetVarPrefix("ReportProfilesUsed.");
	m_ParamsInfo.bReportAnimTemplates = Parser.ReadFlag("AnimTemplates", false);
	m_ParamsInfo.bReportCriatureProfiles = Parser.ReadFlag("Criatures", false);
	m_ParamsInfo.bReportFloorProfiles = Parser.ReadFlag("Floors", false);
	m_ParamsInfo.bReportItemProfiles = Parser.ReadFlag("Items", false);
	m_ParamsInfo.bReportRoofProfiles = Parser.ReadFlag("Roofs", false);
	m_ParamsInfo.bReportSceneObjProfiles = Parser.ReadFlag("SceneObjects", false);
	m_ParamsInfo.bReportWallProfiles = Parser.ReadFlag("Walls", false);

	// Se inicializan reglas
	if (!Parser.SetSection("[CBTFileNames]")) {
	  WriteMsg(Parser.GetFileName() + " / No se halló la sección [CBTFileNames]", CCrisolBuilder::MSG_ERROR);
	  return false;
	}
	ASSERT(Parser.IsSectionActive());
	Parser.SetVarPrefix("");
	if (m_RulesDataBase.Init(Parser.ReadString("RulesDefsFileName") + ".cbt"),
							 true) {	  	
	  // Se inicializan resto de vbles de miembro
	  WriteMsg("Reglas inicializadas.", CCrisolBuilder::MSG_NORMAL);		
	  m_szInitFileName = szInitFileName;

	  // Todo correcto
	  m_bIsInitOk = true;
	  return true;
	}			
  }
  
  // Hubo problemas
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia si procede
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCrisolBuilder::End(void)
{
  // Finaliza
  if (IsInitOk()) {
	// Libera reglas
	m_RulesDataBase.End();

	// Cierra fichero donde enviar mensajes y libera map de msg
	if (m_ParamsInfo.bWriteMsgInFile) {
	  m_MsgInfo.MsgFile.close();	
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo que coordina la construccion de los archivos .cbb
// Parametros:
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::Build(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Parser para obtener nombres de los archivos cbt con datos
  CCBTToolParser Parser;
  Parser.Init(m_szInitFileName);
  if (!Parser.SetSection("[CBTFileNames]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [CBTFileNames]", CCrisolBuilder::MSG_ERROR);
	return false;
  }
  ASSERT(Parser.IsInitOk());

  // Restea valores de contadores de mensajes
  m_MsgInfo.uwNumErrors = 0;
  m_MsgInfo.uwNumWarnings = 0;

  // Construye las areas
  const word uwNumAreas = Parser.ReadNumeric("NumberOfAreas");
  WriteMsg("Construyendo archivos de áreas.", CCrisolBuilder::MSG_NORMAL);
  if (!BuildAreaFiles(uwNumAreas)) {
	return false;
  }

  // Construye archivos de perfil
  // Floors
  WriteMsg("Construyendo archivo FloorProfiles.cbb", CCrisolBuilder::MSG_NORMAL);
  if (!BuildFloorProfilesFile(Parser.ReadString("FloorProfilesFileName"))) {
	return false;
  }

  // Walls
  WriteMsg("Construyendo archivo WallProfiles.cbb", CCrisolBuilder::MSG_NORMAL);    
  if (!BuildWallProfilesFile(Parser.ReadString("WallProfilesFileName"))) {
	return false;
  }

  // Objetos de escenario
  WriteMsg("Construyendo archivo SceneObjProfiles.cbb", CCrisolBuilder::MSG_NORMAL);  
  if (!BuildSceneObjProfilesFile(Parser.ReadString("SceneObjProfilesFileName"))) {
	return false;
  }

  // Items
  WriteMsg("Construyendo archivo ItemProfiles.cbb", CCrisolBuilder::MSG_NORMAL);
  if (!BuildItemProfilesFile(Parser.ReadString("ItemProfilesFileName"))) {
	return false;
  } 

  // Criaturas
  WriteMsg("Construyendo archivo CriatureProfiles.cbb", CCrisolBuilder::MSG_NORMAL);  
  if (!BuildCriatureProfilesFile(Parser.ReadString("CriatureProfilesFileName"))) {
	return false;
  }

  // Roofs
  WriteMsg("Construyendo archivo RoofProfiles.cbb", CCrisolBuilder::MSG_NORMAL);    
  if (!BuildRoofProfilesFile(Parser.ReadString("RoofProfilesFileName"))) {
	return false;
  }

  // Chequeo de los perfiles de jugador
  std::string szCBTFile(Parser.ReadString("PlayerProfilesFileName"));
  WriteMsg("Chequeando archivo " + szCBTFile + ".cbt", CCrisolBuilder::MSG_NORMAL);    
  if (!CheckPlayerProfilesFile(szCBTFile, uwNumAreas)) {
	return false;
  }

  // Chequea interfaces
  szCBTFile = Parser.ReadString("InterfacesCfgFileName");
  WriteMsg("Chequeando archivo " + szCBTFile + ".cbt", CCrisolBuilder::MSG_NORMAL);      
  if (!CheckInterfazFile(szCBTFile)) {
	return false;
  }

  // Chequea definiciones de GFX
  szCBTFile = Parser.ReadString("GFXDefsFileName");
  WriteMsg("Chequeando archivo " + szCBTFile + ".cbt", CCrisolBuilder::MSG_NORMAL);      
  if (!CheckGFXDefsFile(szCBTFile)) {
	return false;
  }

  // Chequeando presentaciones
  szCBTFile = Parser.ReadString("PresentationsFileName");
  WriteMsg("Chequeando archivo " + szCBTFile + ".cbt", CCrisolBuilder::MSG_NORMAL);      
  if (!CheckPresentationsFile(szCBTFile)) {
	return false;
  }

  // Chequeando texto estatico
  szCBTFile = Parser.ReadString("StaticTextFileName");
  WriteMsg("Chequeando archivo " + szCBTFile + ".cbt", CCrisolBuilder::MSG_NORMAL);      
  if (!CheckStaticTextFile(szCBTFile)) {
	return false;
  }

  // Chequeando scripts globales
  szCBTFile = Parser.ReadString("GlobalScriptEventsFileName");
  WriteMsg("Chequeando archivo " + szCBTFile + ".cbt", CCrisolBuilder::MSG_NORMAL);      
  if (!CheckGlobalScriptEventsFile(szCBTFile)) {
	return false;
  }  
  
  // Plantillas de animacion
  WriteMsg("Construyendo archivo AnimTemplates.cbb", CCrisolBuilder::MSG_NORMAL);        
  if (!BuildAnimTemplateFile(Parser.ReadString("AnimTemplatesFileName"))) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Construye los archivos de areas
// Parametros:
// - uwNumAreas. Numero de areas (archivos) que construir.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - A partir del numero maximo de areas (que debera de ser mayor que 0) se
//   construiran los nombres de los archivos de areas esperables.
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::BuildAreaFiles(const word uwNumAreas)
{
  // ¿Parametro incorrecto?
  if (!uwNumAreas) {
	WriteMsg("Al menos debe de existir un archivo de área", CCrisolBuilder::MSG_ERROR);  
	return false;
  }

  // Se procede a navegar entre los distintos archivos de area
  word uwIDArea = 0;  
  for (; uwIDArea < uwNumAreas; ++uwIDArea) {
	// Se crea el nombre del area
	std::ostringstream StrAreaFileName; 
	StrAreaFileName << "Area" << uwIDArea;

	// Se inicializa CBTParser y fichero de escritura
	CCBTToolParser CBTParser;
	std::ofstream CBBFile;
	if (!InitBuild(CBTParser, 
				   CBBFile, 
				   StrAreaFileName.str() + ".cbt", 
				   StrAreaFileName.str() + ".cbb")) {
	  return false;
	}
  
	// Se escribe codigo de archivo / version
	WriteMsg("Construyendo archivo " + StrAreaFileName.str() + ".cbb", 
			  CCrisolBuilder::MSG_NORMAL);  
	CBBFile.write((sbyte *)(&CBDefs::CBBAreaBaseFile), sizeof(byte));
	CBBFile.write((sbyte *)(&CBDefs::HVersion), sizeof(byte));
	CBBFile.write((sbyte *)(&CBDefs::LVersion), sizeof(byte));

	// Se guardan datos generales del area
	if (!CBTParser.SetSection("[AreaInfo]")) {
	  WriteMsg(CBTParser.GetFileName() + " / No se halló la sección [AreaInfo]", CCrisolBuilder::MSG_ERROR);
	  return false;
	}
	CBTParser.SetVarPrefix("");

	// Codigo del area y nombre del area
	CBBFile.write((sbyte *)(&uwIDArea), sizeof(word));
	WriteString(CBBFile, CBTParser.ReadString("Name"));
  
	// Dimensiones
	// Nota: Si las dimensiones son mayores que las maximas permitidas, 
	// habra un ajuste al valor mas grande posible.
	// Ancho
	word uwWidth = CBTParser.ReadNumeric("Width");  
	if (!CheckValue(uwWidth, 1, AreaDefs::MAX_AREA_WIDTH)) {
	  WriteMsg(CBTParser.GetSectionName() + " / Width / Valor no válido.",
			   CCrisolBuilder::MSG_ERROR);
	  return false;
	}  
	CBBFile.write((sbyte *)(&uwWidth), sizeof(word));
	// Alto
	word uwHeight = CBTParser.ReadNumeric("Height");  
	if (!CheckValue(uwHeight, 1, AreaDefs::MAX_AREA_HEIGHT)) {
	  WriteMsg(CBTParser.GetSectionName() + " / Height / Valor no válido.",
			   CCrisolBuilder::MSG_ERROR);
	  return false;
	}  
	CBBFile.write((sbyte *)(&uwHeight), sizeof(word));

	// Numero de celdas en total
	const dword udMaxCells = uwWidth * uwHeight;

	// Modelo del luz. Por defecto 0
	const GraphDefs::Light AmbientLight = CBTParser.ReadNumeric("AmbientLight", false);	
	CBBFile.write((sbyte *)(&AmbientLight), sizeof(GraphDefs::Light));  

	// Se crea el map de identificadores de habitacion, necesario para
	// insertar identificadores numericos en las celdas que pertenecen a alguna
	// habitacion
	IdRoomMap RoomMap;
	word uwRoomIt = 0;	
	if (CBTParser.SetSection("[RoomProfiles]")) {
	  // Procede a cargar informacion de perfiles habitacion
	  CBTParser.SetVarPrefix("");
	  for (; uwRoomIt < 0XFF; ++uwRoomIt) {
		// Se procede a leer informacion y a mapearla
		std::ostringstream StrVar;	  
		StrVar << "Room[" << uwRoomIt << "].Profile";
		std::string szValue(CBTParser.ReadStringID(StrVar.str(), false));
		
		// ¿Se leyo informacion?
		if (CBTParser.WasLastParseOk()) {	  
		  // Si, se registra la informacion si NO existe un nodo con igual
		  std::transform(szValue.begin(), szValue.end(), szValue.begin(), tolower);	  
		  const IdRoomMapIt It(RoomMap.find(szValue));
		  if (It == RoomMap.end()) {		
			// Nota: Se incrementara en una unidad el identificador para evitar el 0
			RoomMap.insert(IdRoomMapValType(szValue, uwRoomIt + 1));
		  }
		} else {
		  // No, se abandona
		  break;
		}	
	  }  	  
	}

	// Se escribe la cantidad de habitaciones existentes
	CBBFile.write((sbyte *)(&uwRoomIt), sizeof(word));

	// Se insertan todas las secciones del archivo en el un conjunto
	StrSet SectionSet;
	CBTParser.CreateSetOfSections(SectionSet);

	// Se prepara el set de tags con el fin de registrar en el todos los
	// tags y comprobar si un determinado tag existe o no
	StrSet TagSet;
	
	// Se comienzan a escribir los datos asociados a celdas.
	// Nota: Las celdas se guardaran por filas
	// Nota: Despues de escribir datos, se tomara la actual posicion final,
	// esto es necesario porque pudiera ser que en los metodos se retrocediera
	// posiciones con lo que la posicion (0, std::ios::end) podria no ser la
	// posicion final.	
	dword udNumCells = 0;
	std::streampos EndFilePos = CBBFile.tellp();
	word uwHeightIt = 0;
	for (; uwHeightIt < uwHeight; ++uwHeightIt) {	
	  // Se procede a guardar informacion
	  word uwWidthIt = 0;
	  for (; uwWidthIt < uwWidth; ++uwWidthIt) {	  
		// Vbles
		bool bSectionFlags[] = { 
		  // Array con los flags indicativos de las secciones existentes
		  false, // 0 / Seccion floor
		  false, // 1 / Seccion roof
		  false, // 2 / Seccion scenary obj.
		  false, // 3 / Seccion criature
		  false  // 4 / Seccion wall
		};
		
		// Se guarda el offset de la zona en donde se escriben los flags
		// estos de forma temporal (o final si no hubiera datos)
		std::streampos SectionFlagsOffset = CBBFile.tellp();
		CBBFile.write((sbyte *)(bSectionFlags), sizeof(bool) * 5);	  
		EndFilePos = CBBFile.tellp();

		// Se localiza seccion
		// Nota: En caso de no hallar seccion, se entendera que no hay 
		// info asociada y se pasara a la siguiente iteracion. Si se
		// hallara seccion, al menos debera de existir la informacion
		// relativa a Floor.
  		std::ostringstream StrSection;
		StrSection << "[Cell<" << uwWidthIt << "," << uwHeightIt << ">]";
		std::string szSection(StrSection.str());
		std::transform(szSection.begin(), szSection.end(), szSection.begin(), tolower);
		StrSetIt SectionIt(SectionSet.find(szSection));
		if (SectionIt == SectionSet.end()) { 
		  continue;
		}

		// Existe la seccion, luego se establece y se borra del set
		CBTParser.SetSection(szSection);
		SectionSet.erase(SectionIt);
		
		// Localizada celda
		++udNumCells;

		// Se escribe zona floor
		// Nota: La informacion floor siempre ha de existir si existe la
		// celda como seccion.
		WriteAreaFloorInfo(CBTParser, CBBFile, RoomMap, TagSet);
		bSectionFlags[0] = true;
		EndFilePos = CBBFile.tellp();

		// Se carga posible informacion de techo
		// Nota: Esta informacion no es excluyente
		bSectionFlags[1] = WriteAreaRoofInfo(CBTParser, CBBFile);
		EndFilePos = CBBFile.tellp();

		// Se carga posible informacion de objetos de escenario
		// Nota: En caso de que existan objetos de escenario no podran
		// incluirse criaturas sobre la celda.
		bSectionFlags[2] = WriteAreaSceneObjInfo(CBTParser, CBBFile, TagSet);
		EndFilePos = CBBFile.tellp();

		// Criaturas, solo si no existen objetos de escenario
		if (!bSectionFlags[2]) {
		  // Se escriben
		  bSectionFlags[3] = WriteAreaCriatureInfo(CBTParser, CBBFile, TagSet);		
		  EndFilePos = CBBFile.tellp();
		}

		// Paredes
		bSectionFlags[4] = WriteAreaWallInfo(CBTParser, CBBFile, TagSet);	  
		EndFilePos = CBBFile.tellp();

		// Finalmente se escribe el valor de los flags tal y como hayan quedado
		// Nota: Es muy importante
		CBBFile.seekp(SectionFlagsOffset);
		CBBFile.write((sbyte *)(bSectionFlags), sizeof(bool) * 5);
		CBBFile.seekp(EndFilePos, std::ios::beg);  	  
	  }
	}  

	// Todo correcto
	CBBFile.close();

	// En caso de que no se haya encontrado ninguna celda error
	if (!udNumCells) {
	  WriteMsg("No hay editada ninguna celda en el mapa", CCrisolBuilder::MSG_ERROR);
	} else {
	  std::ostringstream StrText;
	  StrText << "* Editadas " 
			  << udNumCells 
			  << " celdas de " 
			  << uwWidth * uwHeight 
			  << ".";
	  WriteMsg(StrText.str(), CCrisolBuilder::MSG_NORMAL);
	}
  }
  
  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de leer la informacion de tag de un perfil y escribirla a 
//   disco. En caso de que el tag procesado ya exista, se producira un error.
// Parametros:
// - CBTParser. CBTParser al archivo donde leer el valor
// - CBBFile. Fichero de salida donde escribir el valor
// - TagSet. Conjunto de tags
// - szTmpPrefix. Posible prefijo temporal para leer el valor
// Devuelve:
// Notas:
// - Metodo de apoyo a BuilAreaFile para cada uno de los perfiles a procesar
// - Los tags estaran disponibles SOLO para Items, Criaturas, Objetos de
//   escenario y Paredes.
///////////////////////////////////////////////////////////////////////////////
void 
CCrisolBuilder::WriteAreaTagInfo(CCBTToolParser& CBTParser,
								 std::ofstream& CBBFile,
								 StrSet& TagSet,
								 const std::string& szTmpPrefix)
{
  // SOLO si parametros validos
  ASSERT(CBBFile.is_open());

  // Lee posible Tag asociado
  std::string szTag(CBTParser.ReadStringID(szTmpPrefix + "Tag", false));
  if (CBTParser.WasLastParseOk()) {
	// Se leyo tag, comprueba que el tag no exista
	std::transform(szTag.begin(), szTag.end(), szTag.begin(), tolower);
	if (TagSet.find(szTag) != TagSet.end()) {
	  // Ya existe / error
	  WriteMsg("El tag \"" + szTag + "\" asociado a \"" + CBTParser.GetPrefixName() + szTmpPrefix + "Tag\" ya está registrado.", 
			   CCrisolBuilder::MSG_ERROR);	  
	} else {	  
	  // Inserta el tag
	  TagSet.insert(szTag);
	}
  }

  // Escribe
  WriteString(CBBFile, szTag);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Escribe la informacion asociada a una celda dentro del proceso de la
//   construccion del archivo de area base.
// - El cotenido que escribira sera:
//   - Nombre del perfil floor (tamaño string + string)
//   - Elevacion del terreno (RulesDefs::Elevation)
//   - Numero de items sobre el terreno (word)
//     * Por cada item
//        - Perfil del item (tamaño string + string)
//        - Tag asociado (tamaño string + string)
//        - Informacion luminosa asociada (byte)
//   - Identificador de la posible habitacion al que esta asociado (word)
// Parametros:
// - CBTParser. CBTParser al archivo donde leer el valor
// - CBBFile. Fichero de salida donde escribir el valor
// - szVar. String con la vble a la que añadir el sufijo para leer la info.
// - TagSet. Conjunto de tags
// Devuelve:
// Notas:
// - Metodo de apoyo a BuilAreaFile
///////////////////////////////////////////////////////////////////////////////
void
CCrisolBuilder::WriteAreaFloorInfo(CCBTToolParser& CBTParser,
								   std::ofstream& CBBFile,
								   const IdRoomMap& RoomMap,
								   StrSet& TagSet)
{
  // Nombre perfil
  CBTParser.SetVarPrefix("Floor.");
  const std::string szProfile(CBTParser.ReadStringID("Profile"));
  WriteString(CBBFile, szProfile);  
	  
  // Elevacion terreno
  RulesDefs::Elevation Elevation = CBTParser.ReadNumeric("Elevation", false);
  CBBFile.write((sbyte *)(&Elevation), sizeof(RulesDefs::Elevation));
	  
  // Items sobre el terreno (como maximo 256)
  const std::streampos NumItemsAtFloorOffset = CBBFile.tellp();
  word uwIt = 0;	  
  CBBFile.write((sbyte *)(&uwIt), sizeof(word));
  for (; uwIt < 0xFF; ++uwIt) {
	// Se obtiene posible perfil de item sobre el suelo
	std::ostringstream StrVar;	  
	StrVar << "ItemAbandoned[" << uwIt << "].";
	const std::string szValue(CBTParser.ReadStringID(StrVar.str() + "Profile", false));
	// ¿Se hallo informacion?
	if (CBTParser.WasLastParseOk()) {		  
	  // Si, se escribe perfil del item, nombre tag y el valor luminoso asociado	  
	  WriteString(CBBFile, szValue);
	  WriteAreaTagInfo(CBTParser, CBBFile, TagSet, StrVar.str());
	  WriteAreaLightInfo(CBTParser, CBBFile, StrVar.str());
	} else {
	  // No, se abandona el bucle
	  break;
	}
  }

  // Se guarda el numero final de items hallados
  CBBFile.seekp(NumItemsAtFloorOffset, std::ios::beg);
  CBBFile.write((sbyte *)(&uwIt), sizeof(word));
  CBBFile.seekp(0, std::ios::end);

  // Se guarda identificador de posible habitacion asociada
  // Nota: Si no posee identificador de habitacion se guardara el valor 0
  CBTParser.SetVarPrefix("Room.");
  std::string szValue(CBTParser.ReadStringID("Profile", false));
  AreaDefs::RoomID Room;
  if (CBTParser.WasLastParseOk()) {
	// Se lee identificador numerico asociado a dicho perfil
	std::transform(szValue.begin(), szValue.end(), szValue.begin(), tolower);	  	  
	const IdRoomMapConstIt MapIt(RoomMap.find(szValue));
	if (MapIt == RoomMap.end()) {
	  WriteMsg("No existe identificador de habitación " + szValue, 
			   CCrisolBuilder::MSG_WARNING);	  	  
	  Room = 0;	 
	} else {
	  Room = MapIt->second;
	}
  } else {
	// No se leyo informacion alguna
	Room = 0;
  }
  CBBFile.write((sbyte *)(&Room), sizeof(AreaDefs::RoomID));  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Escribe la informacion asociada a un roof dentro del proceso de la
//   construccion del archivo de area base.
// - El contenido que escribira sera:
//   - Nombre del perfil (tamaño string + string)
//   - Elevacion (RulesDefs::Elevation)
//   - Flag de ocultacion de lo que hay por debajo (bool)
// Parametros:
// - CBTParser. CBTParser al archivo donde leer el valor
// - CBBFile. Fichero de salida donde escribir el valor
// Devuelve:
// - Si ha encontrado informacion true, en caso contrario false.
// Notas:
// - Metodo de apoyo a BuilAreaFile
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::WriteAreaRoofInfo(CCBTToolParser& CBTParser,
								  std::ofstream& CBBFile)
{
  // Carga el posible perfil del techo
  CBTParser.SetVarPrefix("Roof.");
  std::string szRoofProfile(CBTParser.ReadStringID("Profile", false));  
  
  // ¿Se leyo algo?
  if (CBTParser.WasLastParseOk()) {
	// Se escribe el perfil
	WriteString(CBBFile, szRoofProfile);	
	
	// Se lee la elevacion, que debera de existir obligatoriamente
	const RulesDefs::Elevation RoofElevation = CBTParser.ReadNumeric("Elevation");
	CBBFile.write((sbyte *)(&RoofElevation), sizeof(RulesDefs::Elevation));

	// Se lee flag que indica si lo que hay por debajo del techo debe verse
	// Nota: Por defecto NO mostraran lo que tienen por debajo
	const bool bFlag = CBTParser.ReadFlag("ShowUnderFlag", false);
    CBBFile.write((sbyte *)(&bFlag), sizeof(bool));

	// Se escribo roof
	return true;
  } 
  
  // No se escribio roof
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Escribe informacion asociada a un objeto de escenario.
// - El contenido que escribira sera:
//   - Numero de objetos de escenario (word)
//   * Por cada objeto de escenario
//   - Nombre del perfil (tamaño string + string)
//   - Tag asociado (tamaño string + string)
//   - Elevacion (RulesDefs::Elevation)
//   - Luz asociada (byte)
//   * Contenido (solo sera valido si el objeto es contenedor)
//     - Numero de items
//     * Por cada item
//       - Perfil (tamaño string + string)
//       - Tag asociado (tamaño string + string)
//       - Luz asociada a item (byte)
// Parametros:
// - CBTParser. CBTParser al archivo donde leer el valor
// - CBBFile. Fichero de salida donde escribir el valor
// - TagSet. Conjunto de tags ya registrados
// Devuelve:
// - Si ha encontrado informacion true, en caso contrario false.
// Notas:
// - Metodo de apoyo a BuilAreaFile
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::WriteAreaSceneObjInfo(CCBTToolParser& CBTParser,
									  std::ofstream& CBBFile,
									  StrSet& TagSet)
{
  // Se buscan objetos de escenario, dejando antes espacio para
  // guardar posterior mente el numero de los mismos y tomando el offset a la
  // posicion donde se dejara dicho valor
  word uwIdx = 0;
  std::streampos NumSceneObjsOffset = CBBFile.tellp();
  CBBFile.write((sbyte *)(&uwIdx), sizeof(word));
  for (; uwIdx < 0xFF; ++uwIdx) {  
	std::ostringstream StrVar;	  	
	StrVar << "SceneObj[" << uwIdx << "].";
	CBTParser.SetVarPrefix(StrVar.str());
	
	// Se obtiene perfil, en caso de no leerse se abandonara bucle
	std::string szValue(CBTParser.ReadStringID("Profile", false));
	
	if (CBTParser.WasLastParseOk()) {   	  
	  // Si se hallo perfil, se escribe	  
	  WriteString(CBBFile, szValue);

	  // Tag asociado
	  WriteAreaTagInfo(CBTParser, CBBFile, TagSet);

	  // Elevacion sobre el terreno
	  const RulesDefs::Elevation Elevation = CBTParser.ReadNumeric("Elevation", false);
	  CBBFile.write((sbyte *)(&Elevation), sizeof(RulesDefs::Elevation));

	  // Luz asociada
	  WriteAreaLightInfo(CBTParser, CBBFile, "");

	  // Se guardan los items que contenga. En caso de que el perfil asociado
	  // al objeto de escenario sea el de un no contenedor, dichos items seran
	  // obviados cuando se cargue el nivel.
	  const std::streampos NumItemsInSceneObjOffset = CBBFile.tellp();
	  word uwIt = 0;	  
	  CBBFile.write((sbyte *)(&uwIt), sizeof(word));
	  for (; uwIt < 0xFF; ++uwIt) {
		// Se obtiene posible perfil de item sobre el suelo
		std::ostringstream StrVar;	  
		StrVar << "Content[" << uwIt << "].";
		const std::string szValue(CBTParser.ReadStringID(StrVar.str() + "Profile", false));
		
		// ¿Se hallo informacion?
		if (CBTParser.WasLastParseOk()) {		  
		  // Si, se escribe perfil del item y el valor luminoso asociado
		  WriteString(CBBFile, szValue);
		  WriteAreaTagInfo(CBTParser, CBBFile, TagSet, StrVar.str());
		  WriteAreaLightInfo(CBTParser, CBBFile, StrVar.str());
		} else {
		  // No, se abandona el bucle
		  break;
		}
	  }

	  // Se guarda el numero final de items hallados
	  CBBFile.seekp(NumItemsInSceneObjOffset, std::ios::beg);
	  CBBFile.write((sbyte *)(&uwIt), sizeof(word));
	  CBBFile.seekp(0, std::ios::end);
	} else {
	  // No se halló perfil, se abandona bucle
	  break;
	}
  } 

  // ¿Se hallo algun objeto de escenario?
  if (uwIdx) {
	// Si, luego se guarda el numero de objetos de escenario hallados y retorna
	CBBFile.seekp(NumSceneObjsOffset);
	CBBFile.write((sbyte *)(&uwIdx), sizeof(word));
	CBBFile.seekp(0, std::ios::end);
	return true;
  } else {
	// No, luego se deja el offset donde se guardo el numero de entidades y retorna
	CBBFile.seekp(NumSceneObjsOffset);
	return false;
  }
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Escribe informacion asociada a una criatura
// - El contenido que escribira sera:
//   - Numero de criaturas (word) 
//   - Nombre del perfil (tamaño string + string)
//   - Tag asociado (tamaño string + string)
//   - Flag de criatura temporal (bool)
//   - Elevacion (RulesDefs::Elevation)
//   - Luz asociada (byte)
//   * Contenido del inventario
//     - Numero de items
//     * Por cada item
//       - Perfil (tamaño string + string)
//       - Tag asociado (tamaño string + string)
//       - Luz asociada a item (byte)
//   * Slots de equipamiento
//     - Numero de slots de equipamiento con datos (word)
//     * Por cada slot de equipamiento
//     - Identirficador del slot (RulesDefs::eIDEquipmentSlot)
//     - Perfil del item (tamaño string + string)
//     - Tag asociado (tamaño string + string)
//     - Luz asociada al item (byte)
// Parametros:
// - CBTParser. CBTParser al archivo donde leer el valor
// - CBBFile. Fichero de salida donde escribir el valor
// - TagSet. Conjunto de tags registrados
// Devuelve:
// - Si ha encontrado informacion true, en caso contrario false.
// Notas:
// - Metodo de apoyo a BuilAreaFile
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::WriteAreaCriatureInfo(CCBTToolParser& CBTParser,
									  std::ofstream& CBBFile,
									  StrSet& TagSet)
{
  // Se establece prefijo y se lee el posible perfil
  CBTParser.SetVarPrefix("Criature.");
  std::string szValue(CBTParser.ReadStringID("Profile", false));  
  if (CBTParser.WasLastParseOk()) {
	// Se escribe la cantidad de criaturas (1)    
	word uwValue = 1;
	CBBFile.write((sbyte *)(&uwValue), sizeof(word));

	// Se escribe perfil
	WriteString(CBBFile, szValue);

	// Flag de criatura temporal
	const bool bTmpCriature = CBTParser.ReadFlag("TemporaryCriatureFlag", false);
    CBBFile.write((sbyte *)(&bTmpCriature), sizeof(bool));
		
    // Tag asociado
	WriteAreaTagInfo(CBTParser, CBBFile, TagSet);
	
    // Elevacion sobre el terreno
    const RulesDefs::Elevation Elevation = CBTParser.ReadNumeric("Elevation", false);
    CBBFile.write((sbyte *)(&Elevation), sizeof(RulesDefs::Elevation));

    // Luz asociada
    WriteAreaLightInfo(CBTParser, CBBFile, "");
	
    // Se guardan los items que contenga en inventario. 
	const std::streampos NumItemsInInventoryOffset = CBBFile.tellp();
    word uwIt = 0;	  
    CBBFile.write((sbyte *)(&uwIt), sizeof(word));
    for (; uwIt < 0xFF; ++uwIt) {
  	  // Se obtiene posible perfil de item sobre el suelo
	  std::ostringstream StrVar;	  
	  StrVar << "Inventory[" << uwIt << "].";
	  const std::string szValue(CBTParser.ReadStringID(StrVar.str() + "Profile", false));

	  // ¿Se hallo informacion?
	  if (CBTParser.WasLastParseOk()) {		  
	    // Si, se escribe perfil del item y el valor luminoso asociado
		WriteString(CBBFile, szValue);
		WriteAreaTagInfo(CBTParser, CBBFile, TagSet, StrVar.str());
	    WriteAreaLightInfo(CBTParser, CBBFile, StrVar.str());
	  } else {
	    // No, se abandona el bucle
	    break;
	  }
	}

	// Se guarda el numero final de items hallados si procede
	if (uwIt) {
	  CBBFile.seekp(NumItemsInInventoryOffset, std::ios::beg);
	  CBBFile.write((sbyte *)(&uwIt), sizeof(word));
	  CBBFile.seekp(0, std::ios::end);
	}

	// Se procede a guardar la informacion relativa a los items equipados	
	const word uwMaxCriatureDefSlots = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT);
	word uwNumItemEquipped = 0;
	const std::streampos NumItemsEquippedOffset = CBBFile.tellp();	
	CBBFile.write((sbyte *)(&uwNumItemEquipped), sizeof(word));
	uwIt = 0;	
	for (; uwIt < uwMaxCriatureDefSlots; ++uwIt) {
	  // Se lee identificador de posible item equipado
	  std::ostringstream StrVar;
	  StrVar << "EquipmentSlot[" 
			 << m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT,
											   uwIt)
		     << "].";
	  const std::string szProfile = CBTParser.ReadStringID(StrVar.str() + "Profile", false);
	  if (CBTParser.WasLastParseOk()) {
		// Se lee tag, luz y se establece identificador de slot
		WriteAreaTagInfo(CBTParser, CBBFile, TagSet, StrVar.str());
		const byte ubLight = CBTParser.ReadNumeric(StrVar.str() + "LightValue", false);
		const RulesDefs::eIDEquipmentSlot EquipmentSlot = RulesDefs::eIDEquipmentSlot(uwIt);

		// Se escriben datos
		CBBFile.write((sbyte *)(&EquipmentSlot), sizeof(RulesDefs::eIDEquipmentSlot));
		WriteString(CBBFile, szProfile);
		CBBFile.write((sbyte *)(&ubLight), sizeof(byte));

		// Se incrementa contador de items equipados
		++uwNumItemEquipped;
	  }	  
	 }

	// Se guarda el numero final de items equipados si procede
	if (uwNumItemEquipped) {
	  CBBFile.seekp(NumItemsEquippedOffset);
	  CBBFile.write((sbyte *)(&uwNumItemEquipped), sizeof(word));
	  CBBFile.seekp(0, std::ios::end);	
	}

	// Retorna
	return true;
  } 

  // No se halló informacion
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Escribe informacion asociada a un objeto de escenario.
// - El contenido que escribira sera:
//   - Numero de paredes (word)
//   * Por cada pared
//   - Nombre del perfil (tamaño string + string)
//   - Tag asociado (tamaño string + string)
//   - Elevacion (RulesDefs::Elevation)
//   - Luz asociada (byte)
//   - Flag de bloqueo (bool)
// Parametros:
// - CBTParser. CBTParser al archivo donde leer el valor
// - CBBFile. Fichero de salida donde escribir el valor
// - TagSet. Conjunto de tags registrados
// Devuelve:
// - Si ha encontrado informacion true, en caso contrario false.
// Notas:
// - Metodo de apoyo a BuilAreaFile
///////////////////////////////////////////////////////////////////////////////bool
bool
CCrisolBuilder::WriteAreaWallInfo(CCBTToolParser& CBTParser,
								  std::ofstream& CBBFile,
								  StrSet& TagSet)
{
  // Orientaciones posibles de las paredes
  std::string szOrientations[] = {
	"SouthWest", "Front", "SouthEast"
  };

  // Se buscan paredes, dejando antes espacio para
  // guardar posterior mente el numero de los mismos y tomando el offset a la
  // posicion donde se dejara dicho valor
  word uwIdx = 0;
  std::streampos NumWallsOffset = CBBFile.tellp();
  CBBFile.write((sbyte *)(&uwIdx), sizeof(word));  
  for (; uwIdx < 0xFF; ++uwIdx) {  
	// Se establece prefijo
	std::ostringstream StrPrefix;
	StrPrefix << "Wall[" << uwIdx << "].";
	CBTParser.SetVarPrefix(StrPrefix.str());
	  
	// Se lee el flag de orientacion
	RulesDefs::eWallOrientation Orientation = RulesDefs::WO_NONE;
	std::string szValue;
	word uwIt = 0;
	for (; uwIt < 3; uwIt++) {
	  // Se lee el perfil asociado a la orientacion actual
	  std::ostringstream StrVar;
	  StrVar << szOrientations[uwIt] << "Flag";
	  if (CBTParser.ReadFlag(StrVar.str(), false)) {
		// Orientacion leida, se establece la orientacion y abandona el bucle
		// Nota: Consultar RulesDefs para ver la relacion enumerados - valores
		Orientation = RulesDefs::eWallOrientation(uwIt + 1);
		break;
	  }
	}

	// ¿Se leyo informacion de orientacion?
	if (Orientation != RulesDefs::WO_NONE) {
	  // Si, se lee el perfil asociado
	  szValue = CBTParser.ReadStringID("Profile");
	  
	  // Se escribe perfil de la pared
	  WriteString(CBBFile, szValue);

	  // Tag asociado
	  WriteAreaTagInfo(CBTParser, CBBFile, TagSet);

      // Elevacion sobre el terreno
	  const RulesDefs::Elevation Elevation = CBTParser.ReadNumeric("Elevation", false);
	  CBBFile.write((sbyte *)(&Elevation), sizeof(RulesDefs::Elevation));

	  // Luz asociada
	  WriteAreaLightInfo(CBTParser, CBBFile, "");

	  // Se lee flag de bloqueo de acceso. Por defecto estaran bloqueadas.
	  bool bFlag = CBTParser.ReadFlag("BlockAccessFlag", false); 
      if (!bFlag && 
		  CBTParser.WasLastParseOk()) {
		bFlag = true;
	  }
	  CBBFile.write((sbyte *)(&bFlag), sizeof(bool));
	} else {
	  // No, luego se abandona el bucle general
	  break;
	}
  }

  // ¿Se encontro algun tipo de informacion?
  if (uwIdx) {
	// Si, luego se guarda el numero de objetos de escenario hallados y retorna
	CBBFile.seekp(NumWallsOffset);
	CBBFile.write((sbyte *)(&uwIdx), sizeof(word));
	CBBFile.seekp(0, std::ios::end);
	return true;	
  } else {
	// No, luego se deja el offset donde se guardo el numero de entidades y retorna
	CBBFile.seekp(NumWallsOffset);
	return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Escribe un valor luminoso asociado a una entidad. 
// Parametros:
// - CBTParser. CBTParser al archivo donde leer el valor
// - CBBFile. Fichero de salida donde escribir el valor
// - szVar. String con la vble a la que añadir el sufijo para leer la info.
// Devuelve:
// Notas:
// - Metodo de apoyo a BuilAreaFile
// - Se acotaran los valores automaticamente
///////////////////////////////////////////////////////////////////////////////
void 
CCrisolBuilder::WriteAreaLightInfo(CCBTToolParser& CBTParser,
								   std::ofstream& CBBFile,
								   const std::string szVar)
{
  // Lee la informacion relativa a una luz asociada
  float fLight = CBTParser.ReadNumeric(szVar + "LightValue", false);

  // Acota el valor y lo almacena en un byte
  GraphDefs::Light Light;
  if (CBTParser.WasLastParseOk()) {
	if (fLight < 0.0f) {
	  Light = 0;
  	  WriteMsg(CBTParser.GetSectionName() + " / " + szVar + "LightValue / Ajustando valor a 0.",
			   CCrisolBuilder::MSG_WARNING);  
	} else if (fLight > 255.0f) {
	  WriteMsg(CBTParser.GetSectionName() + " / " + szVar + "LightValue / Ajustando valor a 255.",
			   CCrisolBuilder::MSG_WARNING);
	  Light = 255;
	} else {
	  Light = fLight;
	}
  } else {
	Light = 0;
  }

  // Escribe el valor
  CBBFile.write((sbyte *)(&Light), sizeof(GraphDefs::Light));  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Construye el archivo de perfiles floor.
// * El formato del archivo sera:
//   - Tipo de fichero (byte)
//   - Version alta (byte)
//   - Version baja (byte)
//   - Offset a indice de plantillas (streampos)
//   * Datos de una plantilla:
//     - Nombre de la plantilla de animacion (tamaño string + string)
//     - Estado inicial de anim. (siempre 0; AnimTemplateDefs::AnimState)
//     - Orientacion inicial. (siempre 0; AnimTemplateDefs::AnimOrientation)
//   * Modo de responder ante la no visibilidad en pantalla 
//     (RulesDefs::eNoVisibleInScreenMode).
//   * Eventos asociados. Por cada evento, aunque no se haya asociado nada,
//     un string. El orden de los eventos sera:
//       - OnSetIn
//       - OnSetOut
//     - Tipo de floor (byte)
//     - Nombre del posible WAV asociado (tamaño string + string)
//     * Mascaras de acceso:
//       - Norte (bool)
//       - Noreste (bool)
//       - Este (bool)
//       - Sudeste (bool)
//       - Sur (bool)
//       - Sudoeste (bool)
//       - Oeste (bool)
//       - Noroeste (bool)
//   * Indice de perfiles:
//     - Numero de perfiles (word)
//     * Por perfil:
//       - Offset (streampos)
//       - Nombre del perfil (tamaño string + string)
// Parametros:
// - szCBTFile. Nombre del archivo donde leer y guardar los datos.
// Devuelve:
// - Si existio algun problema false, en caso contrario true.
// Notas:
// - El archivo de salida siempre sera "FloorProfiles.cbb"
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::BuildFloorProfilesFile(const std::string& szCBTFile)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se inicializa CBTParser y fichero de escritura
  CCBTToolParser CBTParser;
  std::ofstream CBBFile;
  if (!InitBuild(CBTParser, CBBFile, szCBTFile + ".cbt", szCBTFile + ".cbb")) {
	return false;
  }

  // Se escribe codigo de archivo / version
  CBBFile.write((sbyte *)(&CBDefs::CBBFloorProfilesFile), sizeof(byte));
  CBBFile.write((sbyte *)(&CBDefs::HVersion), sizeof(byte));
  CBBFile.write((sbyte *)(&CBDefs::LVersion), sizeof(byte));

  // Se deja espacio para guardar posteriormente el offset a zona de indices
  std::streampos IndexOffset = 0;
  CBBFile.write((sbyte *)(&IndexOffset), sizeof(std::streampos));

  // Orientaciones posibles para la lectura de las mascaras de acceso
  const std::string szOrientations[] = {
	"North", "NorthEast", "East", "SouthEast", 
	"South", "SouthWest", "West", "NorthWest" 
  };

  // Se procede a guardar la informacion, guardandose los offsets.
  // Por cada tipo, se saltara a la seccion de ficheros wav asociados y si se
  // halla uno asociado a ese tipo, se insertara.
  StreamPosList ListOfOffsets;
  StrSet Profiles;
  CBTParser.CreateSetOfSections(Profiles);
  Profiles.erase("[walkwavsound]");
  StrSetIt SectionIt(Profiles.begin());
  for (; SectionIt != Profiles.end(); ++SectionIt) {
	// Se indica la plantilla que se escribe si procede
	if (m_ParamsInfo.bReportFloorProfiles) {
	  WriteMsg("Escribiendo plantilla de suelo: " + *SectionIt, 
			   CCrisolBuilder::MSG_NORMAL);
	}
	
	// Se guarda el offset asociado a la seccion
	ListOfOffsets.push_back(CBBFile.tellp());

	// Se establece seccion actual	
	if (!CBTParser.SetSection(*SectionIt)) {
	  WriteMsg(CBTParser.GetFileName() + " / No se halló la sección " + *SectionIt, CCrisolBuilder::MSG_ERROR);
	  return false;
	}
	ASSERT(CBTParser.IsSectionActive());
	CBTParser.SetVarPrefix("");	
	
	// Se guarda el nombre de la plantilla de animacion
	std::string szAnimTemplate(CBTParser.ReadStringID("AnimTemplate"));
	WriteString(CBBFile, szAnimTemplate);
	
	// Se guarda estado inicial de la plantilla (siempre 0)
	const AnimTemplateDefs::AnimState AnimState = 0;
	CBBFile.write((sbyte *)(&AnimState), sizeof(AnimTemplateDefs::AnimState));

	// Guarda orientacion inicial de la plantilla
	const AnimTemplateDefs::AnimOrientation AnimOrientation = 0;
	CBBFile.write((sbyte *)(&AnimOrientation), sizeof(AnimTemplateDefs::AnimOrientation));

	// Se lee el flag asociado a la accion por defecto cuando se este
	// fuera de pantalla. Por defecto se PAUSARA.
	RulesDefs::eNoVisibleInScreenMode NoVisibleScreenMode;
	if (CBTParser.ReadFlag("NoVisibleInScreenMode.PauseFlag", false)) {
	  NoVisibleScreenMode = RulesDefs::NVS_PAUSE;
	} else if(CBTParser.ReadFlag("NoVisibleInScreenMode.SilenceFlag", false)) {
	  NoVisibleScreenMode = RulesDefs::NVS_SILENCE;
	} else {
	  NoVisibleScreenMode = RulesDefs::NVS_PAUSE;
	}	
	CBBFile.write((sbyte *)(&NoVisibleScreenMode), sizeof(RulesDefs::eNoVisibleInScreenMode));

	// Se guarda los nombres de los scripts asociados
	const RulesDefs::eScriptEvents ScriptEvents[] = {
	  RulesDefs::SE_ONSETIN, RulesDefs::SE_ONSETOUT
	};
	WriteEntityScriptEvents(ScriptEvents, RulesDefs::NUM_FLOOR_EVENTS, CBTParser, CBBFile);	

	// Tipo
	const std::string szType(CBTParser.ReadStringID("Type"));
	sword swIDValue = m_RulesDataBase.GetEntityIDNumeric(RulesDefs::ID_FLOOR_TYPE,
														 szType); 
	if (!CheckValue(swIDValue, 0, m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_FLOOR_TYPE))) {
	  WriteMsg(CBTParser.GetSectionName() + " / Type / Identificador no válido",
			   CCrisolBuilder::MSG_ERROR);
	  return false;
	}
	const RulesDefs::BaseType Type = swIDValue;	
	CBBFile.write((sbyte *)(&Type), sizeof(RulesDefs::BaseType));

	// Se comprueba si hay WAV asociado
	if (!CBTParser.SetSection("[WalkWAVSound]")) {
	  WriteMsg(CBTParser.GetFileName() + " / No se halló la sección [WalkWAVSound]", CCrisolBuilder::MSG_ERROR);
	  return false;
	}
	std::ostringstream StrVar; 
	StrVar << "Floor[" << szType << "]";
	WriteString(CBBFile, CBTParser.ReadStringID(StrVar.str(), false));
		
	// Mascaras de acceso
	// Nota: Se guardara al final del todo porque esta informacion sera 
	// almacenada en CArea despues de que CFloor se haya inicializado con
	// los datos precedentes
	// Se forma la mascara de acceso
	if (!CBTParser.SetSection(*SectionIt)) {
	  WriteMsg(CBTParser.GetFileName() + " / No se halló la sección " + *SectionIt, CCrisolBuilder::MSG_ERROR);
	  return false;
	}
	word uwIt = 0;
	for (; uwIt < 8; ++uwIt) {
	  // Se prepara nombre vble
	  std::ostringstream StrVar; 
	  StrVar << "Orientation[" << szOrientations[uwIt] << "].ObstacleFlag";
	  const bool bAccess = CBTParser.ReadFlag(StrVar.str(), false);
	  CBBFile.write((sbyte *)(&bAccess), sizeof(bool));
	} 
  }

  // Se guarda el offset donde se alojara el indice de ficheros
  IndexOffset = CBBFile.tellp();
  CBBFile.seekp(3 * sizeof(byte), std::ios::beg);
  CBBFile.write((sbyte *)(&IndexOffset), sizeof(std::streampos));

  // Se escribe el indice de secciones
  CBBFile.seekp(0, std::ios::end);
  WriteIndexOfSections(CBBFile, Profiles, ListOfOffsets);

  // Cierra y retorna
  CBBFile.close();
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Construye el archivo de perfiles wall.
// * El formato del archivo sera:
//   - Tipo de fichero (byte)
//   - Version alta (byte)
//   - Version baja (byte)
//   - Offset a indice de perfiles (streampos)
//   * Datos de una plantilla:
//     - Plantilla animacion (tamaño string + string)
//     - Estado inicial de anim. (siempre 0; AnimTemplateDefs::AnimState)
//     - Orientacion inicial. (siempre 0; AnimTemplateDefs::AnimOrientation)
//   * Modo de responder ante la no visibilidad en pantalla 
//     (RulesDefs::eNoVisibleInScreenMode).
//   * Eventos asociados. Por cada evento, aunque no se haya asociado nada,
//     un string. El orden de los eventos sera:
//       - OnObserve
//       - OnTalk
//       - OnManipulate
//       - OnEntityIdle
//       - OnHit
//       - OnEntityCreated
//     - Nombre (tamaño string + string)
//     - Retrato (tamaño string + string)
//     - Sombra (tamaño string + string)
//     - Tiempo script idle (float)
//     - Tipo (RulesDefs::BaseType)
//     * Atributos locales
//       - Todos los atributos que puede tener un Wall como maximo (sword)
//     - Orientacion (RulesDefs::eWallOrientation)
//     - Flag de interaccion (bool)
//   * Indice de perfiles:
//     - Numero de perfiles (word)
//     * Por perfil:
//       - Offset (streampos)
//       - Nombre del perfil (tamaño string + string)
// Parametros:
// - szFileName. Nombre del archivo donde leer y guardar los datos.
// Devuelve:
// - Si existio algun problema false, en caso contrario true.
// Notas:
// - El archivo de salida siempre sera "WallProfiles.cbb"
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::BuildWallProfilesFile(const std::string& szCBTFile)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se inicializa CBTParser y fichero de escritura
  CCBTToolParser CBTParser;
  std::ofstream CBBFile;
  if (!InitBuild(CBTParser, CBBFile, szCBTFile + ".cbt", szCBTFile + ".cbb")) {
	return false;
  }

  // Se escribe codigo de archivo / version
  CBBFile.write((sbyte *)(&CBDefs::CBBWallProfilesFile), sizeof(byte));
  CBBFile.write((sbyte *)(&CBDefs::HVersion), sizeof(byte));
  CBBFile.write((sbyte *)(&CBDefs::LVersion), sizeof(byte));

  // Se deja espacio para guardar posteriormente el offset a zona de indices
  std::streampos IndexOffset = 0;
  CBBFile.write((sbyte *)(&IndexOffset), sizeof(std::streampos));

  // Se procede a guardar la informacion, guardandose los offsets.
  StreamPosList ListOfOffsets;
  StrSet Profiles;
  CBTParser.CreateSetOfSections(Profiles);  
  StrSetIt SectionIt(Profiles.begin());
  for (; SectionIt != Profiles.end(); ++SectionIt) {
	// Se indica la plantilla que se escribe si procede
	if (m_ParamsInfo.bReportWallProfiles) {
	  WriteMsg("Escribiendo plantilla de pared: " + *SectionIt, 
			   CCrisolBuilder::MSG_NORMAL);
	}

	// Se guarda el offset asociado a la seccion
	ListOfOffsets.push_back(CBBFile.tellp());

	// Se establece seccion actual	
	if (!CBTParser.SetSection(*SectionIt)) {
	  WriteMsg(CBTParser.GetFileName() + " / No se halló la sección " + *SectionIt, CCrisolBuilder::MSG_ERROR);
	  return false;
	}
	CBTParser.SetVarPrefix("");		

	// Plantilla de animacion
	std::string szAnimTemplate(CBTParser.ReadStringID("AnimTemplate"));
	WriteString(CBBFile, szAnimTemplate);
	
	// Estado inicial de la plantilla (siempre 0)
	const AnimTemplateDefs::AnimState AnimState = 0;
	CBBFile.write((sbyte *)(&AnimState), sizeof(AnimTemplateDefs::AnimState));

	// Guarda orientacion inicial de la plantilla
	const AnimTemplateDefs::AnimOrientation AnimOrientation = 0;
	CBBFile.write((sbyte *)(&AnimOrientation), sizeof(AnimTemplateDefs::AnimOrientation));

	// Se lee el flag asociado a la accion por defecto cuando se este
	// fuera de pantalla. Por defecto se PAUSARA.
	RulesDefs::eNoVisibleInScreenMode NoVisibleScreenMode;
	if (CBTParser.ReadFlag("NoVisibleInScreenMode.PauseFlag", false)) {
	  NoVisibleScreenMode = RulesDefs::NVS_PAUSE;
	} else if(CBTParser.ReadFlag("NoVisibleInScreenMode.SilenceFlag", false)) {
	  NoVisibleScreenMode = RulesDefs::NVS_SILENCE;
	} else {
	  NoVisibleScreenMode = RulesDefs::NVS_PAUSE;
	}	
	CBBFile.write((sbyte *)(&NoVisibleScreenMode), sizeof(RulesDefs::eNoVisibleInScreenMode));
	
	// Se guarda los nombres de los scripts asociados
	const RulesDefs::eScriptEvents ScriptEvents[] = {
	  RulesDefs::SE_ONOBSERVE, RulesDefs::SE_ONTALK, RulesDefs::SE_ONMANIPULATE,
	  RulesDefs::SE_ONHIT, RulesDefs::SE_ONENTITYCREATED, RulesDefs::SE_ONENTITYIDLE
	};
	WriteEntityScriptEvents(ScriptEvents, RulesDefs::NUM_WALL_EVENTS, CBTParser, CBBFile);
	
	// Nombre, Retrato y Sombra
	WriteString(CBBFile, CBTParser.ReadString("Name", false));
	szAnimTemplate = CBTParser.ReadStringID("Portrait.AnimTemplate", false);	
	WriteString(CBBFile, szAnimTemplate);
	WriteString(CBBFile, CBTParser.ReadStringID("ShadowImage", false));

	// Tiempo script idle (float), inicialmente sera siempre 0
	const float fIdleScriptTime = 0;
	CBBFile.write((sbyte *)(&fIdleScriptTime), sizeof(float));

	// Tipo
	sword swIDValue = m_RulesDataBase.GetEntityIDNumeric(RulesDefs::ID_WALL_TYPE,
														 CBTParser.ReadStringID("Type")); 
	if (!CheckValue(swIDValue, 0, m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_WALL_TYPE))) {
	  WriteMsg(CBTParser.GetSectionName() + " / Type / Identificador no válido",
			   CCrisolBuilder::MSG_ERROR);
	  return false;
	}
	const RulesDefs::BaseType Type = swIDValue;
	CBBFile.write((sbyte *)(&Type), sizeof(RulesDefs::BaseType));

	// Atributos locales	
	// Nota: Se escribiran todos los posibles atributos que puede tener una
	// pared pero aquellos para los que el tipo en concreto no considere, 
	// seran escritos con valor 0 nada mas.
	const byte ubNumLocalAttribs = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_WALL_LOCALATTRIB,
																	 Type);
	word uwIt = 0;
	for (; uwIt < RulesDefs::MAX_WALL_LOCALATTRIB; ++uwIt) {
	  // ¿Idx de atributo contemplado?
	  RulesDefs::WallLocalAttrib LocalAttrib;
	  if (uwIt < ubNumLocalAttribs) {
		// Se establece vble y se lee el atributo local
		// Nota: por defecto inicializacion a 0
		std::ostringstream StrVar; 
		StrVar << "LocalAttribute[" 
			   << m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_WALL_LOCALATTRIB, 
												 uwIt, 
												 Type)
			   << "]";
		LocalAttrib = CBTParser.ReadNumeric(StrVar.str(), false);
	  } else {
		LocalAttrib = 0;
	  }

	  // Se escribe atributo local
	  CBBFile.write((sbyte *)(&LocalAttrib), sizeof(RulesDefs::WallLocalAttrib));
	}

    // Orientacion
    // Se localiza orientacion: SouthWest / Front / SouthEast y se establece
	std::string szValue(CBTParser.ReadString("Orientation"));
	RulesDefs::eWallOrientation Orientation;
	if (0 == strcmpi(szValue.c_str(), "SouthWest")) {
	  Orientation = RulesDefs::WO_SOUTHWEST;
	} else if (0 == strcmpi(szValue.c_str(), "Front")) {
	  Orientation = RulesDefs::WO_FRONT;
	} else if (0 == strcmpi(szValue.c_str(), "SouthEast")) {
	  Orientation = RulesDefs::WO_SOUTHEAST;
	} else {
	  std::cout << "Error> La orientacion " << szValue << " no es valida\n";
	  std::cout << "     | Seccion << " << *SectionIt << "\n";
	  return false;
	}  
	CBBFile.write((sbyte *)(&Orientation), sizeof(RulesDefs::eWallOrientation));

	// Flag de interactuacion
	bool bFlag = CBTParser.ReadFlag("IsInteractuableFlag", false);
	CBBFile.write((sbyte *)(&bFlag), sizeof(bool));	
  }
  
  // Se guarda el offset donde se alojara el indice de ficheros
  IndexOffset = CBBFile.tellp();
  CBBFile.seekp(3 * sizeof(byte), std::ios::beg);
  CBBFile.write((sbyte *)(&IndexOffset), sizeof(std::streampos));

  // Se escribe el indice de secciones
  CBBFile.seekp(0, std::ios::end);
  WriteIndexOfSections(CBBFile, Profiles, ListOfOffsets);

  // Cierra y retorna
  CBBFile.close();
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Construye el archivo de perfiles SceneObj.
// * El formato del archivo sera:
//   - Tipo de fichero (byte)
//   - Version alta (byte)
//   - Version baja (byte)
//   - Offset a indice de perfiles (streampos)
//   * Datos de una plantilla:
//     - Plantilla animacion (tamaño string + string)
//     - Estado inicial de anim. (siempre 0; AnimTemplateDefs::AnimState)
//     - Orientacion inicial. (siempre 0; AnimTemplateDefs::AnimOrientation)
//   * Modo de responder ante la no visibilidad en pantalla 
//     (RulesDefs::eNoVisibleInScreenMode).
//   * Eventos asociados. Por cada evento, aunque no se haya asociado nada,
//     un string. El orden de los eventos sera:
//       - OnObserve
//       - OnTalk
//       - OnManipulate
//       - OnHit
//       - OnEntityCreated
//       - OnEntityIdle
//     - Nombre (tamaño string + string)
//     - Retrato (tamaño string + string)
//     - Sombra (tamaño string + string)
//     - Tiempo script idle (float)
//     - Tipo (RulesDefs::BaseType)
//     - Clase (RulesDefs::BaseType)
//     - Flag contenedor (bool)
//     * Atributos locales
//       - Todos los atributos que puede tener un SceneObj como maximo (sword)
//       - Flag de interaccion (bool)
//       - Flag de acceso (bool)
//   * Indice de perfiles:
//     - Numero de perfiles (word)
//     * Por perfil:
//       - Offset (streampos)
//       - Nombre del perfil (tamaño string + string)
// Parametros:
// - szCBTFile. Nombre del archivo donde leer y guardar los datos.
// Devuelve:
// - Si existio algun problema false, en caso contrario true.
// Notas:
// - El archivo de salida siempre sera "SceneObjProfiles.cbb"
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::BuildSceneObjProfilesFile(const std::string& szCBTFile)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se inicializa CBTParser y fichero de escritura
  CCBTToolParser CBTParser;
  std::ofstream CBBFile;
  if (!InitBuild(CBTParser, CBBFile, szCBTFile + ".cbt", szCBTFile + ".cbb")) {
	return false;
  }

  // Se escribe codigo de archivo / version
  CBBFile.write((sbyte *)(&CBDefs::CBBSceneObjProfilesFile), sizeof(byte));
  CBBFile.write((sbyte *)(&CBDefs::HVersion), sizeof(byte));
  CBBFile.write((sbyte *)(&CBDefs::LVersion), sizeof(byte));

  // Se deja espacio para guardar posteriormente el offset a zona de indices
  std::streampos IndexOffset = 0;
  CBBFile.write((sbyte *)(&IndexOffset), sizeof(std::streampos));

  // Se procede a guardar la informacion, guardandose los offsets.
  StreamPosList ListOfOffsets;
  StrSet Profiles;
  CBTParser.CreateSetOfSections(Profiles);  
  StrSetIt SectionIt(Profiles.begin());
  for (; SectionIt != Profiles.end(); ++SectionIt) {
	// Se indica la plantilla que se escribe si procede
	if (m_ParamsInfo.bReportSceneObjProfiles) {
	  WriteMsg("Escribiendo plantilla de objeto de escenario: " + *SectionIt, 
			   CCrisolBuilder::MSG_NORMAL);
	}

	// Se guarda el offset asociado a la seccion
	ListOfOffsets.push_back(CBBFile.tellp());

	// Se establece seccion actual	
	if (!CBTParser.SetSection(*SectionIt)) {
	  WriteMsg(CBTParser.GetFileName() + " / No se halló la sección " + *SectionIt, CCrisolBuilder::MSG_ERROR);
	  return false;
	}
	CBTParser.SetVarPrefix("");		

	// Plantilla de animacion
	std::string szAnimTemplate(CBTParser.ReadStringID("AnimTemplate"));
	WriteString(CBBFile, szAnimTemplate);
	
	// Estado inicial de la plantilla (siempre 0)
	const AnimTemplateDefs::AnimState AnimState = 0;
	CBBFile.write((sbyte *)(&AnimState), sizeof(AnimTemplateDefs::AnimState));

	// Guarda orientacion inicial de la plantilla
	const AnimTemplateDefs::AnimOrientation AnimOrientation = 0;
	CBBFile.write((sbyte *)(&AnimOrientation), sizeof(AnimTemplateDefs::AnimOrientation));

	// Se lee el flag asociado a la accion por defecto cuando se este
	// fuera de pantalla. Por defecto se PAUSARA.
	RulesDefs::eNoVisibleInScreenMode NoVisibleScreenMode;
	if (CBTParser.ReadFlag("NoVisibleInScreenMode.PauseFlag", false)) {
	  NoVisibleScreenMode = RulesDefs::NVS_PAUSE;
	} else if(CBTParser.ReadFlag("NoVisibleInScreenMode.SilenceFlag", false)) {
	  NoVisibleScreenMode = RulesDefs::NVS_SILENCE;
	} else {
	  NoVisibleScreenMode = RulesDefs::NVS_PAUSE;
	}	
	CBBFile.write((sbyte *)(&NoVisibleScreenMode), sizeof(RulesDefs::eNoVisibleInScreenMode));
	
	// Se guarda los nombres de los scripts asociados
	const RulesDefs::eScriptEvents ScriptEvents[] = {
	  RulesDefs::SE_ONOBSERVE, RulesDefs::SE_ONTALK, RulesDefs::SE_ONMANIPULATE,
	  RulesDefs::SE_ONHIT, RulesDefs::SE_ONENTITYCREATED, RulesDefs::SE_ONENTITYIDLE
	};
	WriteEntityScriptEvents(ScriptEvents, RulesDefs::NUM_SCENEOBJ_EVENTS, CBTParser, CBBFile);	

	// Nombre, Retrato y Sombra
	WriteString(CBBFile, CBTParser.ReadString("Name", false));
	szAnimTemplate = CBTParser.ReadStringID("Portrait.AnimTemplate", false);
	WriteString(CBBFile, szAnimTemplate);
	WriteString(CBBFile, CBTParser.ReadStringID("ShadowImage", false));

	// Tiempo script idle (float), inicialmente sera siempre 0
	const float fIdleScriptTime = 0;
	CBBFile.write((sbyte *)(&fIdleScriptTime), sizeof(float));

	// Tipo
	//std::string szValue(CBTParser.ReadStringID("Type"));
	sword swIDValue = m_RulesDataBase.GetEntityIDNumeric(RulesDefs::ID_SCENEOBJ_TYPE,
														 CBTParser.ReadStringID("Type")); 
	if (!CheckValue(swIDValue, 0, m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_SCENEOBJ_TYPE))) {
	  WriteMsg(CBTParser.GetSectionName() + " / Type / Identificador no válido",
			   CCrisolBuilder::MSG_ERROR);
	  return false;
	}
	const RulesDefs::BaseType Type = swIDValue;
	CBBFile.write((sbyte *)(&Type), sizeof(RulesDefs::BaseType));

	// Clase, supeditada al tipo
	swIDValue = m_RulesDataBase.GetEntityIDNumeric(RulesDefs::ID_SCENEOBJ_CLASS,
												   CBTParser.ReadStringID("Class"),
												   Type); 
	if (!CheckValue(swIDValue, 0, m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_SCENEOBJ_CLASS, Type))) {
	  WriteMsg(CBTParser.GetSectionName() + " / Class / Identificador no válido",
			   CCrisolBuilder::MSG_ERROR);
	  return false;
	}	
	const RulesDefs::BaseType Class = swIDValue;
	CBBFile.write((sbyte *)(&Class), sizeof(RulesDefs::BaseType));

	// Flag de objeto contenedor	  
	bool bFlag = m_RulesDataBase.IsSceneObjContainer(Type);
	CBBFile.write((sbyte *)(&bFlag), sizeof(bool));

	// Atributos locales	
	// Nota: Se escribiran todos los posibles atributos que puede tener
	// pero aquellos para los que el tipo en concreto no considere, 
	// seran escritos con valor 0 nada mas.
	const byte ubNumLocalAttribs = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_SCENEOBJ_LOCALATTRIB,
																	 Type);
	word uwIt = 0;
	for (; uwIt < RulesDefs::MAX_SCENEOBJ_LOCALATTRIB; ++uwIt) {
	  // ¿Idx de atributo contemplado?
	  RulesDefs::SceneObjLocalAttrib LocalAttrib;	  
	  if (uwIt < ubNumLocalAttribs) {
		// Se establece vble y se lee el atributo local
		// Nota: por defecto inicializacion a 0
		std::ostringstream StrVar; 
		StrVar << "LocalAttribute[" 
			   << m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_SCENEOBJ_LOCALATTRIB, 
												 uwIt, 
												 Type)
			   << "]";
		LocalAttrib = CBTParser.ReadNumeric(StrVar.str(), false);
	  } else {
		LocalAttrib = 0;
	  }

	  // Se escribe atributo local
	  CBBFile.write((sbyte *)(&LocalAttrib), sizeof(RulesDefs::SceneObjLocalAttrib));
	}

	// Flag de interactuacion, por defecto seran interactuables
	bFlag = CBTParser.ReadFlag("IsInteractuableFlag", false);
	if (!CBTParser.WasLastParseOk()) {
	  bFlag = true;
	}
	CBBFile.write((sbyte *)(&bFlag), sizeof(bool));

	// Flag de acceso a celda, por defecto no permitira el acceso a otras entidades
	bFlag = CBTParser.ReadFlag("AccesibleFloorFlag", false);
	CBBFile.write((sbyte *)(&bFlag), sizeof(bool));
  }
  
  // Se guarda el offset donde se alojara el indice de ficheros
  IndexOffset = CBBFile.tellp();
  CBBFile.seekp(3 * sizeof(byte), std::ios::beg);
  CBBFile.write((sbyte *)(&IndexOffset), sizeof(std::streampos));

  // Se escribe el indice de secciones
  CBBFile.seekp(0, std::ios::end);
  WriteIndexOfSections(CBBFile, Profiles, ListOfOffsets);

  // Cierra y retorna
  CBBFile.close();
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Construye el archivo de perfiles Item
// * El formato del archivo sera:
//   - Tipo de fichero (byte)
//   - Version alta (byte)
//   - Version baja (byte)
//   - Offset a indice de plantillas de animacion, offsets a datos (streampos)
//   * Datos de una plantilla:
//     - Plantilla animacion (tamaño string + string)
//     - Estado inicial de anim. (siempre 0; AnimTemplateDefs::AnimState)
//     - Orientacion inicial. (siempre 0; AnimTemplateDefs::AnimOrientation)
//   * Modo de responder ante la no visibilidad en pantalla 
//     (RulesDefs::eNoVisibleInScreenMode).
//   * Eventos asociados. Por cada evento, aunque no se haya asociado nada,
//     un string. El orden de los eventos sera:
//       - OnObserve
//       - OnTalk
//       - OnManipulate
//       - OnEntityIdle
//       - OnGet
//       - OnDrop
//       - OnUseItem
//       - OnTradeItem
//       - OnEntityCreated
//     - Nombre (tamaño string + string)
//     - Retrato (tamaño string + string)
//     - Sombra (tamaño string + string)
//     - Tiempo script idle (float)
//     - Tipo (RulesDefs::BaseType)
//     - Clase (RulesDefs::BaseType)
//     * Atributos globales
//       - Todos los atributos que puede tener un Item como maximo (sword)
//     * Atributos locales
//       - Todos los atributos que puede tener un Item como maximo (sword)
//   * Indice de perfiles:
//     - Numero de perfiles (word)
//     * Por perfil:
//       - Offset (streampos)
//       - Nombre del perfil (tamaño string + string)
// Parametros:
// - szCBTName. Nombre del archivo CBT
// Devuelve:
// - Si existio algun problema false, en caso contrario true.
// Notas:
// - El archivo de salida siempre sera "SceneObjProfiles.cbb"
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::BuildItemProfilesFile(const std::string& szCBTFile)
{

  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Se inicializa CBTParser y fichero de escritura
  CCBTToolParser CBTParser;
  std::ofstream CBBFile;
  if (!InitBuild(CBTParser, CBBFile, szCBTFile + ".cbt", szCBTFile + ".cbb")) {
	return false;
  }

  // Se escribe codigo de archivo / version
  CBBFile.write((sbyte *)(&CBDefs::CBBItemProfilesFile), sizeof(byte));
  CBBFile.write((sbyte *)(&CBDefs::HVersion), sizeof(byte));
  CBBFile.write((sbyte *)(&CBDefs::LVersion), sizeof(byte));

  // Se deja espacio para guardar posteriormente el offset a zona de indices
  std::streampos IndexOffset = 0;
  CBBFile.write((sbyte *)(&IndexOffset), sizeof(std::streampos));

  // Se procede a guardar la informacion, guardandose los offsets.
  // Nota: Solo se actuara en aquellos perfiles hallados en el archivo de areas
  StreamPosList ListOfOffsets;
  StrSet Profiles;
  CBTParser.CreateSetOfSections(Profiles);  
  StrSetIt SectionIt(Profiles.begin());
  for (; SectionIt != Profiles.end(); ++SectionIt) {
	// Se indica la plantilla que se escribe si procede
	if (m_ParamsInfo.bReportItemProfiles) {
	  WriteMsg("Escribiendo plantilla de ítem: " + *SectionIt, 
			   CCrisolBuilder::MSG_NORMAL);
	}

	// Se guarda el offset asociado a la seccion
	ListOfOffsets.push_back(CBBFile.tellp());

	// Se establece seccion actual	
	if (!CBTParser.SetSection(*SectionIt)) {
	  WriteMsg(CBTParser.GetFileName() + " / No se halló la sección " + *SectionIt, CCrisolBuilder::MSG_ERROR);
	  return false;	
	}
	CBTParser.SetVarPrefix("");		

	// Plantilla de animacion
	std::string szAnimTemplate(CBTParser.ReadStringID("AnimTemplate"));
	WriteString(CBBFile, szAnimTemplate);
	
	// Estado inicial de la plantilla (siempre 0)
	const AnimTemplateDefs::AnimState AnimState = 0;
	CBBFile.write((sbyte *)(&AnimState), sizeof(AnimTemplateDefs::AnimState));

	// Guarda orientacion inicial de la plantilla
	const AnimTemplateDefs::AnimOrientation AnimOrientation = 0;
	CBBFile.write((sbyte *)(&AnimOrientation), sizeof(AnimTemplateDefs::AnimOrientation));

	// Se lee el flag asociado a la accion por defecto cuando se este
	// fuera de pantalla. Por defecto se PAUSARA.
	RulesDefs::eNoVisibleInScreenMode NoVisibleScreenMode;
	if (CBTParser.ReadFlag("NoVisibleInScreenMode.PauseFlag", false)) {
	  NoVisibleScreenMode = RulesDefs::NVS_PAUSE;
	} else if(CBTParser.ReadFlag("NoVisibleInScreenMode.SilenceFlag", false)) {
	  NoVisibleScreenMode = RulesDefs::NVS_SILENCE;
	} else {
	  NoVisibleScreenMode = RulesDefs::NVS_PAUSE;
	}	
	CBBFile.write((sbyte *)(&NoVisibleScreenMode), sizeof(RulesDefs::eNoVisibleInScreenMode));
	
	// Se guarda los nombres de los scripts asociados
	const RulesDefs::eScriptEvents ScriptEvents[] = {
	  RulesDefs::SE_ONOBSERVE, RulesDefs::SE_ONTALK, RulesDefs::SE_ONMANIPULATE, 
	  RulesDefs::SE_ONENTITYIDLE, RulesDefs::SE_ONGET, RulesDefs::SE_ONDROP, 
	  RulesDefs::SE_ONUSEITEM, RulesDefs::SE_ONTRADEITEM, RulesDefs::SE_ONENTITYCREATED
	};
	WriteEntityScriptEvents(ScriptEvents, RulesDefs::NUM_ITEMS_EVENTS, CBTParser, CBBFile);

	// Nombre, Retrato y Sombra
	WriteString(CBBFile, CBTParser.ReadString("Name", false));
	szAnimTemplate = CBTParser.ReadStringID("Portrait.AnimTemplate", false);
	WriteString(CBBFile, szAnimTemplate);
	WriteString(CBBFile, CBTParser.ReadStringID("ShadowImage", false));

	// Tiempo script idle (float), inicialmente sera siempre 0
	const float fIdleScriptTime = 0;
	CBBFile.write((sbyte *)(&fIdleScriptTime), sizeof(float));

	// Tipo
	//std::string szValue(CBTParser.ReadStringID("Type"));	
	sword swIDValue = m_RulesDataBase.GetEntityIDNumeric(RulesDefs::ID_ITEM_TYPE,
														 CBTParser.ReadStringID("Type"));
	if (!CheckValue(swIDValue, 0, m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_ITEM_TYPE))) {
	  WriteMsg(CBTParser.GetSectionName() + " / Type / Identificador no válido",
			   CCrisolBuilder::MSG_ERROR);
	  return false;
	}
	const RulesDefs::BaseType Type = swIDValue;
	CBBFile.write((sbyte *)(&Type), sizeof(RulesDefs::BaseType));
	
	// Clase, supeditada al tipo	
	swIDValue = m_RulesDataBase.GetEntityIDNumeric(RulesDefs::ID_ITEM_CLASS,
												   CBTParser.ReadStringID("Class"),
												   Type);	
	if (!CheckValue(swIDValue, 0, m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_ITEM_CLASS, Type))) {
	  WriteMsg(CBTParser.GetSectionName() + " / Class / Identificador no válido",
			   CCrisolBuilder::MSG_ERROR);
	  return false;
	}
	const RulesDefs::BaseType Class = swIDValue;
	CBBFile.write((sbyte *)(&Class), sizeof(RulesDefs::BaseType));	
	
	// Atributos atributos globales y locales
	// Nota: Se escribiran todos los posibles atributos que puede tener
	// pero aquellos para los que el tipo en concreto no considere, 
	// seran escritos con valor 0 nada mas.	
	// Globales
	const byte ubNumGlobalAttribs = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_ITEM_GLOBALATTRIB,
																	  Type);
	word uwIt = 0;
	for (; uwIt < RulesDefs::MAX_ITEM_GLOBALATTRIB; ++uwIt) {
	  // ¿Idx de atributo contemplado?
	  RulesDefs::ItemGlobalAttrib GlobalAttrib;	  
	  if (uwIt < ubNumGlobalAttribs) {
		// Se establece vble y se lee el atributo global
		std::ostringstream StrVar; 
		StrVar << "GlobalAttribute[" 
			   << m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_ITEM_GLOBALATTRIB, 
												 uwIt, 
												 Type)
			   << "]";
		GlobalAttrib = CBTParser.ReadNumeric(StrVar.str(), false);
	  } else {
		GlobalAttrib = 0;
	  }

	  // Se escribe atributo global
	  CBBFile.write((sbyte *)(&GlobalAttrib), sizeof(RulesDefs::ItemGlobalAttrib));
	}

	// Locales
	const byte ubNumLocalAttribs = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_ITEM_LOCALATTRIB,
																	 Type);
	uwIt = 0;
	for (; uwIt < RulesDefs::MAX_ITEM_LOCALATTRIB; ++uwIt) {
	  // ¿Idx de atributo contemplado?
	  RulesDefs::ItemLocalAttrib LocalAttrib;	  
	  if (uwIt < ubNumLocalAttribs) {
		// Se establece vble y se lee el atributo local
		std::ostringstream StrVar; 
		StrVar << "LocalAttribute[" 
			   << m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_ITEM_LOCALATTRIB, 
												 uwIt, 
												 Type)
			   << "]";
		LocalAttrib = CBTParser.ReadNumeric(StrVar.str(), false);		
	  } else {
		LocalAttrib = 0;
	  }

	  // Se escribe atributo local	  
	  CBBFile.write((sbyte *)(&LocalAttrib), sizeof(RulesDefs::ItemLocalAttrib));
	}
  }
  
  // Se guarda el offset donde se alojara el indice de ficheros
  IndexOffset = CBBFile.tellp();
  CBBFile.seekp(3 * sizeof(byte), std::ios::beg);
  CBBFile.write((sbyte *)(&IndexOffset), sizeof(std::streampos));

  // Se escribe el indice de secciones
  CBBFile.seekp(0, std::ios::end);
  WriteIndexOfSections(CBBFile, Profiles, ListOfOffsets);

  // Cierra y retorna
  CBBFile.close();
  return true;	
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Construye el archivo de perfiles Criatures
// * El formato del archivo sera:
//   - Tipo de fichero (byte)
//   - Version alta (byte)
//   - Version baja (byte)
//   - Offset a indice de plantillas de animacion, offsets a datos (streampos)
//   * Datos de una plantilla:
//     - Plantilla animacion (tamaño string + string)
//     - Estado inicial de anim. (siempre 0; AnimTemplateDefs::AnimState)
//     - Orientacion inicial. (siempre 0; AnimTemplateDefs::AnimOrientation)
//   * Modo de responder ante la no visibilidad en pantalla 
//     (RulesDefs::eNoVisibleInScreenMode).
//   * Eventos asociados. Por cada evento, aunque no se haya asociado nada,
//     un string. El orden de los eventos sera:
//       - OnObserve
//       - OnTalk
//       - OnManipulate
//       - OnHit
//       - OnEntityIdle
//       - OnDeath
//       - OnResurrect
//       - OnActivatedSymptom
//       - OnDesactivatedSymptom
//       - OnStartCombatTurn
//       - OnUseHability
//       - OnInsertInEquipmentSlot
//       - OnRemoveFromEquipmentSlot
//       - OnEntityCreated
//       - OnCriatureInRange
//       - OnCriatureOutOfRange
//     - Nombre (tamaño string + string)
//     - Retrato (tamaño string + string)
//     - Sombra (tamaño string + string)
//     - Tiempo script idle (float)
//     - Tipo (RulesDefs::BaseType)
//     - Clase (RulesDefs::BaseType)
//     - Genero (RulesDefs::BaseType)
//     - Nivel (RulesDefs::CriatureLevel)
//     - Experiencia (RulesDefs::CriatureExp)
//     - Puntos de accion (RulesDefs::CriatureActionPoints)
//     - Salud (RulesDefs::sPairValue)
//     - Atributos extendidos (cada uno un RulesDefs::sPairValue)
//     - Habilidades (cada una un bool)
//     - Sintomas (cada uno un bool)
//     - Rango (RulesDefs::CriatureRange)
//     - Modo transparencia (bool)
//     - Modo correr (bool)
//     - Modo fantasma en movimiento (bool)
//     - Flag de acceso a otras criaturas (bool)
//   * Indice de perfiles:
//     - Numero de perfiles (word)
//     * Por perfil:
//       - Offset (streampos)
//       - Nombre del perfil (tamaño string + string)
// Parametros:
// - szCBTFile. Nombre del archivo donde leer y guardar los datos.
// Devuelve:
// - Si existio algun problema false, en caso contrario true.
// Notas:
// - El archivo de salida siempre sera "SceneObjProfiles.cbb"
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::BuildCriatureProfilesFile(const std::string& szCBTFile)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se inicializa CBTParser y fichero de escritura
  CCBTToolParser CBTParser;
  std::ofstream CBBFile;
  if (!InitBuild(CBTParser, CBBFile, szCBTFile + ".cbt", szCBTFile + ".cbb")) {
	return false;
  }

  // Se escribe codigo de archivo / version
  CBBFile.write((sbyte *)(&CBDefs::CBBCriatureProfilesFile), sizeof(byte));
  CBBFile.write((sbyte *)(&CBDefs::HVersion), sizeof(byte));
  CBBFile.write((sbyte *)(&CBDefs::LVersion), sizeof(byte));

  // Se deja espacio para guardar posteriormente el offset a zona de indices
  std::streampos IndexOffset = 0;
  CBBFile.write((sbyte *)(&IndexOffset), sizeof(std::streampos));

  // Se procede a guardar la informacion, guardandose los offsets.
  StreamPosList ListOfOffsets;
  StrSet Profiles;
  CBTParser.CreateSetOfSections(Profiles);  
  StrSetIt SectionIt(Profiles.begin());
  for (; SectionIt != Profiles.end(); ++SectionIt) {
	// Se indica la plantilla que se escribe si procede
	if (m_ParamsInfo.bReportCriatureProfiles) {
	  WriteMsg("Escribiendo plantilla de criatura: " + *SectionIt, 
			   CCrisolBuilder::MSG_NORMAL);
	}

	// Se guarda el offset asociado a la seccion
	ListOfOffsets.push_back(CBBFile.tellp());

	// Se establece seccion actual	
	if (!CBTParser.SetSection(*SectionIt)) {
	  WriteMsg(CBTParser.GetFileName() + " / No se halló la sección " + *SectionIt, CCrisolBuilder::MSG_ERROR);
	  return false;	
	}
	CBTParser.SetVarPrefix("");		

	// Plantilla de animacion 
	std::string szAnimTemplate(CBTParser.ReadStringID("AnimTemplate"));
	WriteString(CBBFile, szAnimTemplate);
	
	// Estado inicial de la plantilla (siempre 0)
	const AnimTemplateDefs::AnimState AnimState = 0;
	CBBFile.write((sbyte *)(&AnimState), sizeof(AnimTemplateDefs::AnimState));

	// Guarda orientacion inicial de la plantilla
	const AnimTemplateDefs::AnimOrientation AnimOrientation = 0;
	CBBFile.write((sbyte *)(&AnimOrientation), sizeof(AnimTemplateDefs::AnimOrientation));

	// Se lee el flag asociado a la accion por defecto cuando se este
	// fuera de pantalla. Por defecto se SILENCIARA.
	RulesDefs::eNoVisibleInScreenMode NoVisibleScreenMode;
	if (CBTParser.ReadFlag("NoVisibleInScreenMode.SilenceFlag", false)) {
	  NoVisibleScreenMode = RulesDefs::NVS_SILENCE;
	} else if(CBTParser.ReadFlag("NoVisibleInScreenMode.PauseFlag", false)) {
	  NoVisibleScreenMode = RulesDefs::NVS_PAUSE;
	} else {
	  NoVisibleScreenMode = RulesDefs::NVS_SILENCE;
	}	
	CBBFile.write((sbyte *)(&NoVisibleScreenMode), sizeof(RulesDefs::eNoVisibleInScreenMode));
	
	// Se guarda los nombres de los scripts asociados
	const RulesDefs::eScriptEvents ScriptEvents[] = {
	  RulesDefs::SE_ONOBSERVE, RulesDefs::SE_ONTALK, RulesDefs::SE_ONMANIPULATE,
	  RulesDefs::SE_ONHIT, RulesDefs::SE_ONENTITYIDLE, RulesDefs::SE_ONDEATH,
	  RulesDefs::SE_ONRESURRECT, RulesDefs::SE_ONACTIVATEDSYMPTOM,
	  RulesDefs::SE_ONDEACTIVATEDSYMPTOM, RulesDefs::SE_ONSTARTCOMBATTURN,
	  RulesDefs::SE_ONUSEHABILITY,
	  RulesDefs::SE_ONINSERTINEQUIPMENTSLOT, RulesDefs::SE_ONREMOVEFROMEQUIPMENTSLOT,
	  RulesDefs::SE_ONENTITYCREATED,
	  RulesDefs::SE_ONCRIATUREINRANGE, RulesDefs::SE_ONCRIATUREOUTOFRANGE
	};
	WriteEntityScriptEvents(ScriptEvents, RulesDefs::NUM_CRIATURE_EVENTS, CBTParser, CBBFile);	

	// Nombre, Retrato y Sombra
	WriteString(CBBFile, CBTParser.ReadString("Name", false));
	szAnimTemplate = CBTParser.ReadStringID("Portrait.AnimTemplate", false);
	WriteString(CBBFile, szAnimTemplate);
	WriteString(CBBFile, CBTParser.ReadStringID("ShadowImage", false));

	// Tiempo script idle (float), inicialmente sera siempre 0
	const float fIdleScriptTime = 0;
	CBBFile.write((sbyte *)(&fIdleScriptTime), sizeof(float));

    // Tipo
	sword swIDValue = m_RulesDataBase.GetEntityIDNumeric(RulesDefs::ID_CRIATURE_TYPE,
														 CBTParser.ReadStringID("Type"));
	if (!CheckValue(swIDValue, 0, m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_TYPE))) {
	  WriteMsg(CBTParser.GetSectionName() + " / Type / Identificador no válido",
			   CCrisolBuilder::MSG_ERROR);
	  return false;
	}
	const RulesDefs::BaseType Type = swIDValue;
	CBBFile.write((sbyte *)(&Type), sizeof(RulesDefs::BaseType));	
		
	// Clase	
	swIDValue = m_RulesDataBase.GetEntityIDNumeric(RulesDefs::ID_CRIATURE_CLASS,
												   CBTParser.ReadStringID("Class"));  
	if (!CheckValue(swIDValue, 0, m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_CLASS))) {
	  WriteMsg(CBTParser.GetSectionName() + " / Class / Identificador no válido",
			   CCrisolBuilder::MSG_ERROR);
	  return false;
	}
	const RulesDefs::BaseType Class = swIDValue;
	CBBFile.write((sbyte *)(&Class), sizeof(RulesDefs::BaseType));	

	// Genero
	swIDValue = m_RulesDataBase.GetEntityIDNumeric(RulesDefs::ID_CRIATURE_GENRE,
												   CBTParser.ReadStringID("Genre"));  
	if (!CheckValue(swIDValue, 0, m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_GENRE))) {
	  WriteMsg(CBTParser.GetSectionName() + " / Genre / Identificador no válido",
			   CCrisolBuilder::MSG_ERROR);
	  return false;
	}
	const RulesDefs::BaseType Genre = swIDValue;
	CBBFile.write((sbyte *)(&Genre), sizeof(RulesDefs::BaseType));

	//  Nivel / Experiencia
	RulesDefs::CriatureLevel Level = CBTParser.ReadNumeric("Level");
	CBBFile.write((sbyte *)(&Level), sizeof(RulesDefs::CriatureLevel));	
	RulesDefs::CriatureExp Experience = CBTParser.ReadNumeric("Experience");
	CBBFile.write((sbyte *)(&Experience), sizeof(RulesDefs::CriatureExp));
	 
	// Atributos
	// Puntos de combate
	RulesDefs::CriatureActionPoints ActionPoints = CBTParser.ReadNumeric("CombatPoints");
	CBBFile.write((sbyte *)(&ActionPoints), sizeof(RulesDefs::CriatureActionPoints));

	// Salud
	RulesDefs::sPairValue PairValue;	
	PairValue.swBase = CBTParser.ReadNumeric("Health.Base");
	PairValue.swTemp = CBTParser.ReadNumeric("Health.Temp", false);  
	if (!CBTParser.WasLastParseOk()) {
	  PairValue.swTemp = PairValue.swBase;
	}
	CBBFile.write((sbyte *)(&PairValue), sizeof(RulesDefs::sPairValue));	

	// Atributos extendidos
	// Nota, todos los posibles atributos seran inicializados. En el caso de
	// los atributos no definidos, estos se inicilizaran con valor 0.
	const byte ubNumExtAttribs = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB);
	word uwIt = 0;
	for (; uwIt < RulesDefs::MAX_CRIATURE_EXATTRIB; ++uwIt) {
	  // Se leeran valores solo cuando se este en el rango definido en Rules
	  if (uwIt < ubNumExtAttribs) {
		std::ostringstream StrVar; 
		StrVar << "ExtendedAttribute[" 
			   <<  m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_EXTATTRIB,
												  uwIt)
			   << "].";
		PairValue.swBase = CBTParser.ReadNumeric(StrVar.str() + "Base");
		PairValue.swTemp = CBTParser.ReadNumeric(StrVar.str() + "Temp", false);
		if (!CBTParser.WasLastParseOk()) {
		  PairValue.swTemp = PairValue.swBase;
		}
	  } else {
		PairValue.swBase = 0;
		PairValue.swBase = 0;
	  }

	  // Se escribe
	  CBBFile.write((sbyte *)(&PairValue), sizeof(RulesDefs::sPairValue));
	}

	// Habilidades. 
	// Nota: Por defecto, no encontrar una habilidad supone que la criatura
	// no poseera dicha habilidad
	bool bFlag;
	const byte ubNumHabilities = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);
	uwIt = 0;
	for (; uwIt < RulesDefs::MAX_CRIATURE_HABILITIES; ++uwIt) {
	  // Se leeran valores solo cuando se este en el rango definido en Rules
	  if (uwIt < ubNumHabilities) {
		std::ostringstream StrVar; 
		StrVar << "Hability["
			   << m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_HABILITY,
												 uwIt)
			   << "].ActiveFlag"; 
		bFlag = CBTParser.ReadFlag(StrVar.str(), false);
	  } else {
		bFlag = false;
	  }  

	  // Se escribe
	  CBBFile.write((sbyte *)(&bFlag), sizeof(bool));
	}

    // Sintomas.
	// Nota: Por defecto, no encontrar una habilidad supone que la criatura
	// no poseera dicha habilidad
	const byte ubNumSymptoms = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_SYMPTOM);
	uwIt = 0;
	for (; uwIt < RulesDefs::MAX_CRIATURE_SYMPTOMS; ++uwIt) {
	  // Se leeran valores solo cuando se este en el rango definido en Rules
	  if (uwIt < ubNumSymptoms) {
		std::ostringstream StrVar; 
		StrVar << "Symptom["
			   << m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_SYMPTOM,
												 uwIt)
			   << "].ActiveFlag";
		bFlag = CBTParser.ReadFlag(StrVar.str(), false);
	  } else {
		bFlag = false;
	  }

  	  // Se escribe
	  CBBFile.write((sbyte *)(&bFlag), sizeof(bool));	  
	}  

	// Rango (entre 0 y el maximo + 1)
	const RulesDefs::CriatureRange Range = CBTParser.ReadNumeric("Range");
	CBBFile.write((sbyte *)(&Range), sizeof(RulesDefs::CriatureRange));	
	if (!CheckValue(Range, 0, RulesDefs::MAX_CRIATURE_RANGE)) {
	  WriteMsg(CBTParser.GetSectionName() + " / Range / Valor asociado no válido",
			   CCrisolBuilder::MSG_ERROR);
	  return false;
	}	

	// Flag de modo de transparencia, por defecto desactivado
	bFlag = CBTParser.ReadFlag("TransparentModeFlag", false);
	CBBFile.write((sbyte *)(&bFlag), sizeof(bool));	

	// Flag de modo correr, por defecto desactivado
	bFlag = CBTParser.ReadFlag("RunModeFlag", false);
	CBBFile.write((sbyte *)(&bFlag), sizeof(bool));	

	// Flag de modo fantasma en movimiento, por defecto desactivado
	bFlag = CBTParser.ReadFlag("GhostMoveModeFlag", false);
	CBBFile.write((sbyte *)(&bFlag), sizeof(bool));
	
	// Flag de acceso a celda, por defecto no permitira el acceso a otras entidades
	bFlag = CBTParser.ReadFlag("AccesibleFloorFlag", false);
	CBBFile.write((sbyte *)(&bFlag), sizeof(bool));
  }
  
  // Se guarda el offset donde se alojara el indice de ficheros
  IndexOffset = CBBFile.tellp();
  CBBFile.seekp(3 * sizeof(byte), std::ios::beg);
  CBBFile.write((sbyte *)(&IndexOffset), sizeof(std::streampos));

  // Se escribe el indice de secciones
  CBBFile.seekp(0, std::ios::end);
  WriteIndexOfSections(CBBFile, Profiles, ListOfOffsets);

  // Cierra y retorna
  CBBFile.close();
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Construye el archivo de perfiles floor.
// * El formato del archivo sera:
//   - Tipo de fichero (byte)
//   - Version alta (byte)
//   - Version baja (byte)
//   - Offset a indice de plantillas de animacion, offsets a datos (streampos)
//   * Datos de una plantilla:
//     - Nombre de la plantilla de animacion (tamaño string + string)
//     - Estado inicial de anim. (siempre 0; AnimTemplateDefs::AnimState)
//     - Orientacion inicial. (siempre 0; AnimTemplateDefs::AnimOrientation)
//   * Modo de responder ante la no visibilidad en pantalla 
//     (RulesDefs::eNoVisibleInScreenMode).
//   * Eventos asociados. Por cada evento, aunque no se haya asociado nada,
//     un string. En el caso de los Roof no habra eventos asociados
//   * Indice de perfiles:
//     - Numero de perfiles (word)
//     * Por perfil:
//       - Offset (streampos)
//       - Nombre del perfil (tamaño string + string)
// Parametros:
// - szCBTFile. Nombre del archivo donde leer y guardar los datos.
// Devuelve:
// - Si existio algun problema false, en caso contrario true.
// Notas:
// - El archivo de salida siempre sera "FloorProfiles.cbb"
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::BuildRoofProfilesFile(const std::string& szCBTFile)
{  
  // Se inicializa CBTParser y fichero de escritura
  CCBTToolParser CBTParser;
  std::ofstream CBBFile;
  if (!InitBuild(CBTParser, CBBFile, szCBTFile + ".cbt", szCBTFile + ".cbb")) {
	return false;
  }

  // Se escribe codigo de archivo / version
  CBBFile.write((sbyte *)(&CBDefs::CBBRoofProfilesFile), sizeof(byte));
  CBBFile.write((sbyte *)(&CBDefs::HVersion), sizeof(byte));
  CBBFile.write((sbyte *)(&CBDefs::LVersion), sizeof(byte));

  // Se deja espacio para guardar posteriormente el offset a zona de indices
  std::streampos IndexOffset = 0;
  CBBFile.write((sbyte *)(&IndexOffset), sizeof(std::streampos));

  // Se procede a guardar la informacion, guardandose los offsets.
  // Por cada tipo, se saltara a la seccion de ficheros wav asociados y si se
  // halla uno asociado a ese tipo, se insertara.
  StreamPosList ListOfOffsets;
  StrSet Profiles;
  CBTParser.CreateSetOfSections(Profiles);  
  StrSetIt SectionIt(Profiles.begin());
  for (; SectionIt != Profiles.end(); ++SectionIt) {
	// Se indica la plantilla que se escribe si procede
	if (m_ParamsInfo.bReportRoofProfiles) {
	  WriteMsg("Escribiendo plantilla de techo: " + *SectionIt, 
			   CCrisolBuilder::MSG_NORMAL);
	}

	// Se guarda el offset asociado a la seccion
	ListOfOffsets.push_back(CBBFile.tellp());

	// Se establece seccion actual	
	if (!CBTParser.SetSection(*SectionIt)) {
	  WriteMsg(CBTParser.GetFileName() + " / No se halló la sección " + *SectionIt, CCrisolBuilder::MSG_ERROR);
	  return false;	
	}
	CBTParser.SetVarPrefix("");	
	
	// Se guarda el nombre de la plantilla de animacion
	std::string szAnimTemplate(CBTParser.ReadStringID("AnimTemplate"));
	WriteString(CBBFile, szAnimTemplate);
	
	// Estado inicial de la plantilla (siempre 0)
	const AnimTemplateDefs::AnimState AnimState = 0;
	CBBFile.write((sbyte *)(&AnimState), sizeof(AnimTemplateDefs::AnimState));

	// Guarda orientacion inicial de la plantilla
	const AnimTemplateDefs::AnimOrientation AnimOrientation = 0;
	CBBFile.write((sbyte *)(&AnimOrientation), sizeof(AnimTemplateDefs::AnimOrientation));

	// Se lee el flag asociado a la accion por defecto cuando se este
	// fuera de pantalla. Por defecto se PAUSARA.
	RulesDefs::eNoVisibleInScreenMode NoVisibleScreenMode;
	if (CBTParser.ReadFlag("NoVisibleInScreenMode.PauseFlag", false)) {
	  NoVisibleScreenMode = RulesDefs::NVS_PAUSE;
	} else if(CBTParser.ReadFlag("NoVisibleInScreenMode.SilenceFlag", false)) {
	  NoVisibleScreenMode = RulesDefs::NVS_SILENCE;
	} else {
	  NoVisibleScreenMode = RulesDefs::NVS_PAUSE;
	}	
	CBBFile.write((sbyte *)(&NoVisibleScreenMode), sizeof(RulesDefs::eNoVisibleInScreenMode));
	
	// Se guarda los nombres de los scripts asociados
	WriteEntityScriptEvents(NULL, RulesDefs::NUM_ROOF_EVENTS, CBTParser, CBBFile);	
  }	
  
  // Se guarda el offset donde se alojara el indice de ficheros
  IndexOffset = CBBFile.tellp();
  CBBFile.seekp(3 * sizeof(byte), std::ios::beg);
  CBBFile.write((sbyte *)(&IndexOffset), sizeof(std::streampos));

  // Se escribe el indice de secciones
  CBBFile.seekp(0, std::ios::end);
  WriteIndexOfSections(CBBFile, Profiles, ListOfOffsets);

  // Cierra y retorna
  CBBFile.close();
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - A partir de un array de strings, escribe la secuencia de eventos asociados
//   a una determinada entidad en su perfil. El metodo se encargara de
//   comprobar que eventos tienen asociados un script para aumentar un contador
//   al final guardara el numero de eventos con script hallados y a continuacion
//   un par de codigo de evento script y del nombre del script.
// Parametros:
// - pScriptEvents. Array de los eventos a escribir.
// - NumEvents. Numero de eventos MAXIMOS a localizar (tamaño del array)
// - CBTParser. Parser desde donde leer la informacion.
// - CBBFile. Fichero donde escribir la informacion.
// Devuelve:
// Notas:
// - Se supondra que el parser tiene el prefijo correctamente asignado.
// - El array podra ser NULL si no hay scripts a asociar
///////////////////////////////////////////////////////////////////////////////
void 
CCrisolBuilder::WriteEntityScriptEvents(const RulesDefs::eScriptEvents* const pScriptEvents,
								        const byte NumEvents,
								        CCBTToolParser& CBTParser,
								        std::ofstream& CBBFile)
{
  // SOLO si parametros correctos
  ASSERT(CBTParser.IsInitOk());
  ASSERT(CBBFile.is_open());

  // Obtiene el offset donde se alojara el numero de scripts hallados finalmente
  // y se guarda un valor temporal de 0
  const std::streampos NumScriptEvents = CBBFile.tellp();
  byte ubNumEvents = 0;
  CBBFile.write((sbyte *)(&ubNumEvents), sizeof(byte));
  
  // Procede a leer y escribir la informacion hallada
  word uwIt = 0;
  for (; uwIt < NumEvents; ++uwIt) {
	// Segun sea el codigo, asi se establecera el nombre del evento
	std::string szScriptEventName;
	ASSERT(pScriptEvents);
	switch(pScriptEvents[uwIt]) {
	  case RulesDefs::SE_ONSETIN: {
		szScriptEventName = "OnSetIn";
	  } break;

	  case RulesDefs::SE_ONSETOUT: {
		szScriptEventName = "OnSetOut";
	  } break;

	  case RulesDefs::SE_ONGET: {
		szScriptEventName = "OnGet";
	  } break;

	  case RulesDefs::SE_ONDROP: {
		szScriptEventName = "OnDrop";
	  } break;

	  case RulesDefs::SE_ONOBSERVE: {
		szScriptEventName = "OnObserve";
	  } break;

	  case RulesDefs::SE_ONTALK: {
		szScriptEventName = "OnTalk";
	  } break;

	  case RulesDefs::SE_ONMANIPULATE: {
		szScriptEventName = "OnManipulate";
	  } break;

	  case RulesDefs::SE_ONDEATH: {
		szScriptEventName = "OnDeath";
	  } break;

	  case RulesDefs::SE_ONRESURRECT: {
		szScriptEventName = "OnResurrect";
	  } break;

	  case RulesDefs::SE_ONINSERTINEQUIPMENTSLOT: {
		szScriptEventName = "OnInsertInEquipmentSlot";
	  } break;

	  case RulesDefs::SE_ONREMOVEFROMEQUIPMENTSLOT: {
		szScriptEventName = "OnRemoveFromEquipmentSlot";
	  } break;

	  case RulesDefs::SE_ONUSEHABILITY: {
		szScriptEventName = "OnUseHability";
	  } break;

	  case RulesDefs::SE_ONACTIVATEDSYMPTOM: {
		szScriptEventName = "OnActivatedSymptom";
	  } break;

	  case RulesDefs::SE_ONDEACTIVATEDSYMPTOM: {
		szScriptEventName = "OnDeactivatedSymptom";
	  } break;

	  case RulesDefs::SE_ONHIT: {
		szScriptEventName = "OnHit";
	  } break;

	  case RulesDefs::SE_ONSTARTCOMBATTURN: {
		szScriptEventName = "OnStartCombatTurn";
	  } break;
	  
	  case RulesDefs::SE_ONENTITYIDLE: {
		szScriptEventName = "OnEntityIdle";
	  } break;

	  case RulesDefs::SE_ONUSEITEM: {
		szScriptEventName = "OnUseItem";
	  } break;

	  case RulesDefs::SE_ONTRADEITEM: {
		szScriptEventName = "OnTradeItem";
	  } break;

	  case RulesDefs::SE_ONENTITYCREATED: {
		szScriptEventName = "OnEntityCreated";
	  } break;

	  case RulesDefs::SE_ONCRIATUREINRANGE: {
		szScriptEventName = "OnCriatureInRange";
	  } break;

	  case RulesDefs::SE_ONCRIATUREOUTOFRANGE: {
		szScriptEventName = "OnCriatureOutOfRange";
	  } break;

	  default:
		ASSERT(false);
	}; // ~ switch

	// Se toma el nombre del archivo asociado al evento que proceda
	std::ostringstream StrVar; 
	StrVar << "ScriptEvent[" << szScriptEventName << "]";
	const std::string szScriptEventFile(CBTParser.ReadStringID(StrVar.str(), false));
	
	// ¿Se leyo el nombre del evento script?
	if (CBTParser.WasLastParseOk()) {
	  // Si, se escribe el codigo, nombre del archivo e incrementa el contador
	  CBBFile.write((sbyte *)(&pScriptEvents[uwIt]), sizeof(RulesDefs::eScriptEvents));
	  WriteString(CBBFile, szScriptEventFile);
	  ++ubNumEvents;
	}	  
  }

  // ¿Se leyo el nombre de algun archivo script asociado a evento?
  if (ubNumEvents) {
	// Si, se guardara el numero de ellos
	CBBFile.seekp(NumScriptEvents, std::ios::beg);
	CBBFile.write((sbyte *)(&ubNumEvents), sizeof(byte));
	CBBFile.seekp(0, std::ios::end);
  }
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la construccion del archivo de configuracion de los interfaces.
// Parametros:
// - szCBTFile. Nombre del archivo .cbt con los datos
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
// - El orden en el que se guardara las secciones sera:
//   * [Cursors]
//   * [Selectors]
//   * [AdviceDialog]
//   * [QuestionDialog]
//   * [TextFileReader]
//   * [TextOptionsSelector]
//   * [MainMenu]
//   * [LoaderSavedFileManager]
//   * [SaverSavedFileManager]
//   * [PlayerCreator]
//   * [GameLoading]
//   * [ConversationWindow]
//   * [EntityInteractMenu]
//   * [InventorySelector]
//   * [TradeItemManager]
//   * [CombatientInfo]
//   * [MainGameInterfaz]
//   * [GameMenu]
//   * [PlayerProfileWindow]
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazFile(const std::string& szCBTFile)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(!szCBTFile.empty());

  // Se inicializa CBTParser
  CCBTToolParser CBTParser;
  CBTParser.Init(szCBTFile + ".cbt");
  // Se proceden a codificar las distintas secciones. 
  // Nota: Los offsets de cada una de estas secciones se iran almacenando en lista

  // [Cursors]
  if (!CheckInterfazCursors(CBTParser)) {
	return false;
  }

  // [Selectors]
  if (!CheckInterfazSelectors(CBTParser)) {
	return false;
  }

  // [AdviceDialog]  
  if (!CheckInterfazAdviceDialog(CBTParser)) {
	return false;
  }

  // [QuestionDialog]  
  if (!CheckInterfazQuestionDialog(CBTParser)) {
	return false;
  }

  // [TextFileReader]
  if (!CheckInterfazTextFileReader(CBTParser)) {
	return false;
  }

  // [TextOptionsSelector]
  if (!CheckInterfazTextOptionsSelector(CBTParser)) {
	return false;
  }

  // [MainMenu]
  if (!CheckInterfazMainMenu(CBTParser)) {
	return false;
  }  
  
  // [LoaderSavedFileManager]
  if (!CheckInterfazLoaderSavedFilesManager(CBTParser)) {
	return false;
  }

  // [SaverSavedFileManager]
  if (!CheckInterfazSaverSavedFilesManager(CBTParser)) {
	return false;
  }  

  // [PlayerCreatorWindow]
  if (!CheckInterfazPlayerCreatorWindow(CBTParser)) {
	return false;
  }  

  // [GameLoading]
  if (!CheckInterfazGameLoading(CBTParser)) {
	return false;
  }  

  // [ConversationWindow]
  if (!CheckInterfazConversationWindow(CBTParser)) {
	return false;
  }    

  // [EntityInteract]
  if (!CheckInterfazEntityInteract(CBTParser)) {
	return false;
  }  

  // [InventorySelector]
  if (!CheckInterfazInventorySelector(CBTParser)) {
	return false;
  }  
  
  // [TradeItemsManager]
  if (!CheckInterfazTradeItemsManager(CBTParser)) {
	return false;
  }    

  // [CombatientInfo]
  if (!CheckInterfazCombatientInfo(CBTParser)) {
	return false;
  }  

  // [MainGameInterfaz]
  if (!CheckInterfazMainGameInterfaz(CBTParser)) {
	return false;
  }  

  // [GameMenu]
  if (!CheckInterfazGameMenu(CBTParser)) {
	return false;
  }    

  // [PlayerProfileWindow]
  if (!CheckInterfazPlayerProfileWindow(CBTParser)) {
	return false;
  }  
  
  // Retorna
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion de cursores del archivo de configuracion de interfaz
// Parametros:
// - Parser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazCursors(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());
  
  // Se estblece seccion
  if (!Parser.SetSection("[Cursors]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [Cursors]", CCrisolBuilder::MSG_ERROR);
	return false;		
  }
  
  // Se chequean los cursores
  Parser.SetVarPrefix("");	  
  const std::string szCursors[] = {
	"Movement.", "EntityInteract.", "EntityHit.", 
	"Waiting.", "MapScroll.", "GUIInteract."
  };
  byte ubIt = 0;
  for (; ubIt < 6; ++ubIt) {
	const std::string szAnimTemplate(Parser.ReadStringID(szCursors[ubIt] + "AnimTemplate"));	
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion de selectores
// Parametros:
// - Parser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazSelectors(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());
  
  // Se estblece seccion
  if (!Parser.SetSection("[Selectors]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [Selectors]", CCrisolBuilder::MSG_ERROR);
	return false;		
  }

  // Selector de mapa
  Parser.SetVarPrefix("Map.");
  std::string szAnimTemplate(Parser.ReadStringID("AnimTemplate"));
  
  // Selectores de combate
  Parser.SetVarPrefix("Combat.");  
  // Alineacion jugador
  szAnimTemplate = Parser.ReadStringID("PlayerAlingment.AnimTemplate");
  // Alineacion enemigos del jugador
  szAnimTemplate = Parser.ReadStringID("EnemyPlayerAlingment.AnimTemplate");
  
  // Todo correcto
  return true;
}  

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion del archivo de interfaces [AdviceDialog]  
// Parametros:
// - Parser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazAdviceDialog(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());
  
  // Se leen datos de la imagen de fondo y se escribe
  if (!Parser.SetSection("[AdviceDialog]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [AdviceDialog]", CCrisolBuilder::MSG_ERROR);
	return false;		
  }
  Parser.SetVarPrefix("");	
  CheckBackImgComponent(Parser, false);

  // Se lee formato del texto
  Parser.SetVarPrefix("TextFormat.");
  Parser.ReadPosition();
  if (!CheckValue(Parser.ReadNumeric("AreaWidth"), 64, 256)) {
	WriteMsg("[AdviceDialog] / AdviceFormat.AreaWidth / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }
  Parser.ReadRGBColor();
  
  // Se lee configuracion del FXBack
  Parser.ReadRGBColor("FXBack.");
  Parser.ReadAlpha("FXBack.");
  
  // Se leen opciones
  Parser.SetVarPrefix("");	  
  const std::string szOptions[] = {
	"AcceptButton."
  };
  CheckOptionsButtonComponents(Parser, szOptions, 1);

  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion del archivo de interfaces [QuestionDialog]  
// Parametros:
// - Parser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazQuestionDialog(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());
  
  // Se leen datos de la imagen de fondo y se escribe
  if (!Parser.SetSection("[QuestionDialog]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [QuestionDialog]", CCrisolBuilder::MSG_ERROR);
	return false;		
  }
  Parser.SetVarPrefix("");	
  CheckBackImgComponent(Parser, false);

  // Se lee formato del texto
  Parser.SetVarPrefix("TextFormat.");
  Parser.ReadPosition();
  if (!CheckValue(Parser.ReadNumeric("AreaWidth"), 64, 256)) {
	WriteMsg("[QuestionDialog] / AreaWidth / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }
  Parser.ReadRGBColor();
  
  // Se lee configuracion del FXBack y se escribe
  Parser.ReadRGBColor("FXBack.");
  Parser.ReadAlpha("FXBack.");
  
  // Se cargan opciones
  Parser.SetVarPrefix("");	
  const std::string szOptions[] = {
	"YesButton.", "NoButton.", "CancelButton."
  };
  CheckOptionsButtonComponents(Parser, szOptions, 3);
  
  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion del archivo de interfaces [TextFileReader]  
// Parametros:
// - Parser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazTextFileReader(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());

  // Se leen datos de la imagen de fondo y se escribe
  if (!Parser.SetSection("[TextFileReader]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [TextFileReader]", CCrisolBuilder::MSG_ERROR);
	return false;		
  }
  Parser.SetVarPrefix("");	
  CheckBackImgComponent(Parser, false);

  // Se leen datos asociados al titulo
  Parser.SetVarPrefix("TitleFormat.");
  Parser.ReadPosition();
  Parser.ReadRGBColor();
  
  // Se cargan datos asociados al FXBack del titulo
  Parser.ReadRGBColor("FXBack.");
  Parser.ReadAlpha("FXBack.");
  
  // Se cargan datos asociados al texto
  Parser.SetVarPrefix("TextFileFormat.");
  Parser.ReadPosition();
  if (!CheckValue(Parser.ReadNumeric("AreaWidth"), 64, 1024)) {
	WriteMsg("[TextFileReader] / TextFileFormat.AreaWidth / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }  
  if (!CheckValue(Parser.ReadNumeric("MaxLines"), 1, 256)) {
	WriteMsg("[TextFileReader] / TextFileFormat.MaxLines / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }
  
  // Check
  Parser.ReadRGBColor();
  
  // Se cargan datos asociados al FXBack del texto
  Parser.ReadRGBColor("FXBack.");
  Parser.ReadAlpha("FXBack.");
  
  // Se cargan opciones
  Parser.SetVarPrefix("");	
  const std::string szOptions[] = {
	"UpButton.", "DownButton.", "ExitButton."
  };
  CheckOptionsButtonComponents(Parser, szOptions, 3);

  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion del archivo de interfaces [TextOptionsSelector]  
// Parametros:
// - Parser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazTextOptionsSelector(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());
  
  // Se leen datos de la imagen de fondo y se escribe
  if (!Parser.SetSection("[TextOptionsSelector]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [TextOptionsSelector]", CCrisolBuilder::MSG_ERROR);
	return false;		
  }
  Parser.SetVarPrefix("");	
  CheckBackImgComponent(Parser, false);

  // Se leen datos asociados al titulo
  Parser.SetVarPrefix("TitleFormat.");
  Parser.ReadPosition();
  Parser.ReadRGBColor();
  
  // Se cargan datos asociados al FXBack del titulo
  Parser.ReadRGBColor("FXBack.");
  Parser.ReadAlpha("FXBack.");

  // Se cargan datos asociados al texto
  Parser.SetVarPrefix("TextOptionsFormat.");
  Parser.ReadPosition();  
  if (!CheckValue(Parser.ReadNumeric("AreaWidth"), 64, 1024)) {
	WriteMsg("[TextOptionsSelector] / TextOptionsFormat.AreaWidth / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }
  Parser.ReadRGBColor("Select.");
  Parser.ReadRGBColor("Unselect.");
  
  // Se carga el num. maximo de lineas
  Parser.ReadNumeric("MaxLines");
  if (!CheckValue(Parser.ReadNumeric("MaxLines"), 1, 256)) {
	WriteMsg("[TextOptionsSelector] / TextOptionsFormat.MaxLines / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }
  
  // Se cargan datos asociados al FXBack del texto
  Parser.ReadRGBColor("FXBack.");
  Parser.ReadAlpha("FXBack.");
  
  // Se cargan opciones
  Parser.SetVarPrefix("");	  
  const std::string szOptions[] = {
	"UpButton.", "DownButton.", "CancelButton."
  };
  CheckOptionsButtonComponents(Parser, szOptions, 3);

  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion del archivo de interfaces [MainMenu]  
// Parametros:
// - Parser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazMainMenu(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());
  
  // Se leen datos de la imagen de fondo y se escribe
  if (!Parser.SetSection("[MainMenu]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [MainMenu]", CCrisolBuilder::MSG_ERROR);
	return false;		
  }
  Parser.SetVarPrefix("");	  
  CheckBackImgComponent(Parser, true);

  // Se leen datos sobre la musica
  Parser.SetVarPrefix("MIDIMusic.");  
  Parser.ReadString("FileName", false);
  Parser.ReadFlag("LoopFlag", false);

  // Se cargan opciones
  Parser.SetVarPrefix("");	  
  const std::string szOptions[] = {
	"StartNewGameButton.", "LoadGameButton.", "CreditsButton.", "ExitButton."
  };
  CheckOptionsButtonComponents(Parser, szOptions, 4);

  // Se comprueba el perfil de presentacion asociado a creditos
  m_SectionSets.szCreditsPresentationProfile = "[Profile<" + Parser.ReadStringID("Options.CreditsButton.PresentationProfile") + ">]";
  
  // Se cargan datos sobre la version
  Parser.SetVarPrefix("VersionInfo.");
  Parser.ReadPosition();
  Parser.ReadRGBColor();
  
  // Se cargan datos sobre los fades
  Parser.SetVarPrefix("Fade.");
  // Color del fade al comenzar
  Parser.ReadRGBColor("Start.");  
  if (!CheckValue(Parser.ReadNumeric("In.Speed"), 1, 256)) {
	WriteMsg("[MainMenu] / Fade.In.Speed / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }
  Parser.ReadRGBColor("Out.");
  if (!CheckValue(Parser.ReadNumeric("Out.Speed"), 1, 256)) {
	WriteMsg("[MainMenu] / Fade.Out.Speed / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }

  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion del archivo de interfaces [LoaderSavedFilesManager]  
// Parametros:
// - Parser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazLoaderSavedFilesManager(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());

  // Se carga, del interfaz de guardar partidas, el num. de caracteres posibles
  // en las descripciones asociadas a las partidas guardadas
  if (!Parser.SetSection("[SaverSavedFileManager]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [SaverSavedFileManager]", CCrisolBuilder::MSG_ERROR);
	return false;		
  }
  Parser.SetVarPrefix("");
  if (!CheckValue(Parser.ReadNumeric("SavedFileDesc.Input.MaxCharacters"), 1, 256)) {
	WriteMsg("[SaverSavedFilesManager] / SavedFileDesc.Input.MaxCharacters / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }

  // Lee imagen de fondo
  if (!Parser.SetSection("[LoaderSavedFileManager]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [LoaderSavedFileManager]", CCrisolBuilder::MSG_ERROR);
	return false;		
  }
  Parser.SetVarPrefix("");	  
  CheckBackImgComponent(Parser, true);

  // Datos asociados a la lista de ficheros guardados
  Parser.SetVarPrefix("SavedFilesList.");
  // Posicion
  Parser.ReadPosition();
  
  // Numero de ficheros visibles  
  if (!CheckValue(Parser.ReadNumeric("NumberOfVisibleFiles"), 1, 256)) {
	WriteMsg("[LoaderSavedFilesManager] / SavedFileList.NumberOfVisibleFiles / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }
   
  // FXBack de seleccion
  Parser.ReadRGBColor("File.FXBack.Select.");
  Parser.ReadAlpha("File.FXBack.Select.");
  
  // Cfg del texto de descripcion de una partida guardada
  Parser.ReadRGBColor("File.Name.Select.");
  Parser.ReadRGBColor("File.Name.Unselect.");
  
  // Cfg del texto de descripcion de una partida guardada
  Parser.SetVarPrefix("SelectSavedFileInfo.");
  const std::string szGenDescInfo[] = {
	"AreaName.", "PlayerName.", "PlayerHealth.", "WorldTime."
  };
  word uwIt = 0;
  for (; uwIt < 4; ++uwIt) {
	Parser.ReadPosition(szGenDescInfo[uwIt]);
	Parser.ReadRGBColor(szGenDescInfo[uwIt]);	
  }

  // Se cargan opciones
  Parser.SetVarPrefix("");	  
  const std::string szOptions[] = {
	"UpButton.", "DownButton.", "LoadButton.", "ExitButton."
  };
  CheckOptionsButtonComponents(Parser, szOptions, 4);

  // Datos del fade  
  Parser.SetVarPrefix("Fade.");
  Parser.ReadRGBColor("Out.");
  if (!CheckValue(Parser.ReadNumeric("Out.Speed"), 1, 256)) {
	WriteMsg("[LoaderSavedFilesManager] / Fade.Out.Speed / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }
  Parser.ReadNumeric("In.Speed");
  if (!CheckValue(Parser.ReadNumeric("In.Speed"), 1, 256)) {
	WriteMsg("[LoaderSavedFilesManager] / Fade.In.Speed / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }

  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion del archivo de interfaces [SaverSavedFilesManager]  
// Parametros:
// - Parser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazSaverSavedFilesManager(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());

  // Se carga en primer lugar  el num. de caracteres posibles en las descripciones 
  // asociadas a las partidas guardadas
  if (!Parser.SetSection("[SaverSavedFileManager]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [SaverSavedFileManager]", CCrisolBuilder::MSG_ERROR);
	return false;		
  }
  Parser.SetVarPrefix("");  
  if (!CheckValue(Parser.ReadNumeric("SavedFileDesc.Input.MaxCharacters"), 1, 256)) {
	WriteMsg("[SaverSavedFilesManager] / SavedFileDesc.Input.MaxCharacters / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }

  // Lee imagen de fondo
  Parser.SetVarPrefix("");	  
  CheckBackImgComponent(Parser, true);

  // Datos asociados a la lista de ficheros guardados
  Parser.SetVarPrefix("SavedFilesList.");
  // Posicion
  Parser.ReadPosition();
  
  // Numero de ficheros visibles  
  if (!CheckValue(Parser.ReadNumeric("NumberOfVisibleFiles"), 1, 256)) {
	WriteMsg("[SaverSavedFilesManager] / SavedFileList.NumberOfVisibleFiles / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }

  // FXBack de seleccion
  Parser.ReadRGBColor("File.FXBack.Select.");
  Parser.ReadAlpha("File.FXBack.Select.");
  
  // Cfg del texto de descripcion de una partida guardada
  Parser.ReadRGBColor("File.Name.Select.");
  Parser.ReadRGBColor("File.Name.Unselect.");
  
  // Cfg del texto de descripcion de una partida guardada
  Parser.SetVarPrefix("SelectSavedFileInfo.");
  const std::string szGenDescInfo[] = {
	"AreaName.", "PlayerName.", "PlayerHealth.", "WorldTime."
  };
  word uwIt = 0;
  for (; uwIt < 4; ++uwIt) {
	Parser.ReadPosition(szGenDescInfo[uwIt]);
	Parser.ReadRGBColor(szGenDescInfo[uwIt]);
  }

  // Datos para la entrada de la descripcion
  // Texto indicactivo para la insercion de datos
  Parser.SetVarPrefix("SavedFileDesc.");
  Parser.ReadRGBColor("InfoText.");
  Parser.ReadPosition("InfoText.");
  
  // Componente para controlar la entrada
  Parser.ReadRGBColor("Input.");
  Parser.ReadPosition("Input.");  
  
  // Se cargan opciones
  Parser.SetVarPrefix("");	  
  const std::string szOptions[] = {
	"UpButton.", "DownButton.", "SaveButton.", "RemoveButton.", "ExitButton."
  };
  CheckOptionsButtonComponents(Parser, szOptions, 5);

  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion del archivo de interfaces [GameLoading]  
// Parametros:
// - Parser. Parser a utilizar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazGameLoading(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());

  // Procesa los nombres de las imagenes de fondo, al menos una
  if (!Parser.SetSection("[GameLoading]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [GameLoading]", CCrisolBuilder::MSG_ERROR);
	return false;		
  }
  Parser.SetVarPrefix("");
  word uwIt = 0;
  for (; uwIt < 0xFF; ++uwIt) {
	// Se intenta leer nombre de archivo
	std::ostringstream StrText;
	StrText << "BackImg[" << uwIt << "].FileName";
	Parser.ReadString(StrText.str(), false);
	// ¿Se leyo algo?
	if (!Parser.WasLastParseOk()) {	  
	  // No, se abandona
	  break;
	}
  }

  // Se escriben datos del fade
  Parser.SetVarPrefix("Fade.");  
  if (!CheckValue(Parser.ReadNumeric("In.Speed"), 1, 256)) {
	WriteMsg("[GameLoading] / Fade.In.Speed / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }
  if (!CheckValue(Parser.ReadNumeric("Out.Speed"), 1, 256)) {
	WriteMsg("[GameLoading] / Fade.Out.Speed / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }
  Parser.ReadRGBColor("Out.");
  
  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion del archivo de interfaces [EntityInteract]  
// Parametros:
// - Parser. Parser a utilizar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazEntityInteract(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());

  // Lee los valores de dibujado
  if (!Parser.SetSection("[EntityInteract]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [EntityInteract]", CCrisolBuilder::MSG_ERROR);
	return false;		
  }
  Parser.SetVarPrefix("DrawFormat.");
  // Alpha asociado a las opciones no seleccionadas
  Parser.ReadAlpha("Options.Unselect.");
  
  // Plantillas de animacion de las opciones de interaccion principales
  Parser.SetVarPrefix("MainMenu.");
  const std::string szMainMenuOptions[] = {
	"Observe", "Talk", "Get", "Register", "Manipulate", 
	"DropItem", "UseInventory", "UseHability", "Exit"
  };
  word uwIt = 0;
  for (; uwIt < 9; ++uwIt) {
	std::ostringstream StrText;
	StrText << "Option[" << szMainMenuOptions[uwIt] <<"].AnimTemplate";
	const std::string szAnimTemplate(Parser.ReadString(StrText.str()));	
  }

  // Plantillas de animacion de las opciones del menu de habilidades
  Parser.SetVarPrefix("HabilityMenu.");
  const word uwMaxHabilities = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);
  uwIt = 0;
  for (; uwIt < uwMaxHabilities; ++uwIt) {
	// ¿Habilidad usable?
	if (m_RulesDataBase.IsUsableHability(RulesDefs::eIDHability(0x01 << uwIt))) {
	  // Se obtiene el nombre y se configura estructura
	  std::ostringstream StrText;
	  StrText << "Option[" 
			  << m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_HABILITY,
												uwIt)
			  << "].AnimTemplate";							  
	  const std::string szAnimTemplate(Parser.ReadString(StrText.str()));	  
	}
  }  

  // Por ultimo, se carga el boton de retorno del menu de habilidades
  const std::string szAnimTemplate(Parser.ReadString("Option[ReturnToMainMenu].AnimTemplate"));  
  
  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion del archivo de interfaces [InventorySelector]  
// Parametros:
// - Parser. Parser a utilizar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazInventorySelector(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());

  // Carga datos de la imagen de fondo y los escribe
  if (!Parser.SetSection("[InventorySelector]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [InventorySelector]", CCrisolBuilder::MSG_ERROR);
	return false;		
  }
  Parser.SetVarPrefix("");	  
  CheckBackImgComponent(Parser, false);

  // Obtiene ancho y alto del area de seleccion y los ajusta si procede
  // Nota: En lugar de mandar mensajes de error, se optara por ajustar valores
  Parser.SetVarPrefix("ItemSelector.");  
  if (!CheckValue(Parser.ReadNumeric("Area.Width"), 1, 8)) {
	WriteMsg("[InventorySelector] / ItemSelector.Area.Width / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }
  if (!CheckValue(Parser.ReadNumeric("Area.Height"), 1, 6)) {
	WriteMsg("[InventorySelector] / ItemSelector.Area.Height / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }

  // Se lee posicion inicial de dibujado y se establece la maxima
  Parser.ReadPosition();
  
  // Se guarda selector de items
  Parser.ReadRGBColor("FXBack.Select.");
  Parser.ReadAlpha("FXBack.Select.");
  
  // Texto flotante
  Parser.ReadRGBColor("FloatingText.");

  // Se cargan opciones
  Parser.SetVarPrefix("");	  
  const std::string szOptions[] = {
	"UpButton.", "DownButton.", "CancelButton."
  };
  CheckOptionsButtonComponents(Parser, szOptions, 3);

  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion del archivo de interfaces [CombatientInfo]  
// Parametros:
// - Parser. Parser a utilizar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazCombatientInfo(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());

  // Datos relativos a la muestra de los puntos de accion
  if (!Parser.SetSection("[CombatientInfo]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [CombatientInfo]", CCrisolBuilder::MSG_ERROR);
	return false;		
  }
  Parser.SetVarPrefix("ActionPoints.");
  
  // Barra de porcentaje
  Parser.ReadPosition("PercentageBar.");
  if (!CheckValue(Parser.ReadNumeric("PercentageBar.Lenght"), 1, 256)) {
	WriteMsg("[CombatientInfo] / ActionPoints.PercentageBar.Lenght / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }
  if (!CheckValue(Parser.ReadNumeric("PercentageBar.Width"), 1, 256)) {
	WriteMsg("[CombatientInfo] / ActionPoints.PercentageBar.Width / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }

  Parser.ReadAlpha("PercentageBar.");

  // RGB del texto a mostrar
  Parser.ReadRGBColor("Value.");
  
  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion del archivo de interfaces [GameMenu]  
// Parametros:
// - Parser. Parser a utilizar.
// - CBBFile. Fichero de escritura donde volcar la informacion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazGameMenu(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());

  // Se carga y escribe imagen de fondo
  if (!Parser.SetSection("[GameMenu]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [GameMenu]", CCrisolBuilder::MSG_ERROR);
	return false;		
  }
  Parser.SetVarPrefix("");	  
  CheckBackImgComponent(Parser, false);

  // Se cargan opciones
  Parser.SetVarPrefix("");	  
  const std::string szOptions[] = {
	"SaveButton.", "LoadButton.", "ReturnButton.", "ExitButton."
  };
  CheckOptionsButtonComponents(Parser, szOptions, 4);

  // Se escriben datos del fade out de salida
  Parser.SetVarPrefix("Fade.");
  Parser.ReadNumeric("Out.Speed");
  Parser.ReadRGBColor("Out.");
  
  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion del archivo de interfaces [ConversationWindow]  
// Parametros:
// - Parser. Parser a utilizar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazConversationWindow(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());

  // Se carga el nombre de la imagen de fondo y se escribe
  // Nota: No se llamara a CheckBackImgComponent ya que diferira en este caso
  if (!Parser.SetSection("[ConversationWindow]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [ConversationWindow]", CCrisolBuilder::MSG_ERROR);
	return false;		
  }
  Parser.SetVarPrefix("BackImage.");	  
  Parser.ReadString("FileName");
  
  // Info asociada a las lineas de dialogo
  Parser.SetVarPrefix("TextOptionsFormat.");
  
  // Num. maximo de lineas de dialogo visibles  
  if (!CheckValue(Parser.ReadNumeric("MaxLines"), 1, 6)) {
	WriteMsg("[ConversationWindows] / DialogLines.MaxLines / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }
  
  // Configuracion de cada una de las lineas
  Parser.ReadPosition();
  if (!CheckValue(Parser.ReadNumeric("AreaWidth"), 128, 512)) {
	WriteMsg("[ConversationWindow] / DialogLines.AreaWidth / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }
  Parser.ReadRGBColor("Select.");
  Parser.ReadRGBColor("Unselect.");  
  
  // Se lee configuracion del FXBack de fondo
  Parser.ReadRGBColor("FXBack.");
  Parser.ReadAlpha("FXBack.");
  
  // Info asociada a los conversadores (Jugador y resto)
  Parser.SetVarPrefix("Speakers.");	  
  const std::string szSpeakers[] = {
	"Player.", "Rest."
  };
  word uwIt = 0;
  for (; uwIt < 2; ++ uwIt) {
	// Retrato y nombre
	Parser.ReadPosition(szSpeakers[uwIt] + "Portrait.");
	Parser.ReadRGBColor(szSpeakers[uwIt] + "Name.");
	Parser.ReadPosition(szSpeakers[uwIt] + "Name.");
  }

  // Se cargan opciones
  Parser.SetVarPrefix("");	  
  const std::string szOptions[] = {
	"UpButton.", "DownButton."
  };
  CheckOptionsButtonComponents(Parser, szOptions, 2);

  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion del archivo de interfaces [ConversationWindow]  
// Parametros:
// - Parser. Parser a utilizar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazTradeItemsManager(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());

  // Ventana del jugador
  if (!Parser.SetSection("[TradeItemsManager]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [TradeItemsManager]", CCrisolBuilder::MSG_ERROR);
	return false;		
  }
  Parser.SetVarPrefix("PlayerWindow.");
  // Imagen de fondo
  CheckBackImgComponent(Parser, false);

  // Selector de items
  // Obtiene ancho y alto del area de seleccion y los ajusta si procede
  // Nota: En lugar de mandar mensajes de error, se optara por ajustar valores  
  if (!CheckValue(Parser.ReadNumeric("ItemSelector.Area.Width"), 1, 8)) {
	WriteMsg("[TradeItemsManager] / PlayerWindow.ItemSelector.Area.Width / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }  
  if (!CheckValue(Parser.ReadNumeric("ItemSelector.Area.Height"), 1, 6)) {
	WriteMsg("[TradeItemsManager] / PlayerWindow.ItemSelector.Area.Height / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }

  // Se lee posicion inicial de dibujado y se establece la maxima
  Parser.ReadPosition("ItemSelector.");
  
  // Se guarda selector de items
  Parser.ReadRGBColor("ItemSelector.FXBack.Select.");
  Parser.ReadAlpha("ItemSelector.FXBack.Select.");

  // Texto flotante
  Parser.ReadRGBColor("ItemSelector.FloatingText.");

  // Opciones
  const std::string szPlayerWindowOptions[] = {
	"UpButton.", "DownButton.", "CancelButton."
  };
  CheckOptionsButtonComponents(Parser, szPlayerWindowOptions, 3);

  // Ventana para el resto de entidades
  Parser.SetVarPrefix("RestWindow.");
  // Imagen de fondo
  CheckBackImgComponent(Parser, false);

  // Selector de items
  // Obtiene ancho y alto del area de seleccion y los ajusta si procede
  // Nota: En lugar de mandar mensajes de error, se optara por ajustar valores  
  if (!CheckValue(Parser.ReadNumeric("ItemSelector.Area.Width"), 1, 8)) {
	WriteMsg("[TradeItemsManager] / RestWindow.ItemSelector.Area.Width / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }  
  if (!CheckValue(Parser.ReadNumeric("ItemSelector.Area.Height"), 1, 8)) {
	WriteMsg("[TradeItemsManager] / RestWindow.ItemSelector.Area.Height / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }

  // Se lee posicion inicial de dibujado y se establece la maxima
  Parser.ReadPosition("ItemSelector.");
  
  // Se guarda selector de items
  Parser.ReadRGBColor("ItemSelector.FXBack.Select.");
  Parser.ReadAlpha("ItemSelector.FXBack.Select.");
  
  // Texto flotante
  Parser.ReadRGBColor("ItemSelector.FloatingText.");
  
  // Pos. de dibujado del retrato
  Parser.ReadPosition("Portrait.");
  
  // Nombre del retrato
  Parser.ReadPosition("Portrait.Name.");
  Parser.ReadRGBColor("Portrait.Name.");
  
  // Opciones
  const std::string szRestWindowOptions[] = {
	"UpButton.", "DownButton."
  };
  CheckOptionsButtonComponents(Parser, szRestWindowOptions, 2);

  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion del archivo de interfaces [MainGameInterfaz]  
// Parametros:
// - Parser. Parser a utilizar.
// - CBBFile. Fichero de escritura donde volcar la informacion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazMainGameInterfaz(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());
  
  // Se carga el nombre de la imagen de fondo y se escribe
  // Nota: No se llamara a CheckBackImgComponent ya que diferira en este caso
  if (!Parser.SetSection("[MainGameInterfaz]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [MainGameInterfaz]", CCrisolBuilder::MSG_ERROR);
	return false;		
  }
  Parser.SetVarPrefix("BackImage.");	  
  Parser.ReadString("FileName");
  
  // Datos asociados al texto flotante
  Parser.SetVarPrefix("FloatingText.");
  Parser.ReadRGBColor();

  // Consola
  // Datos para el control del texto
  Parser.SetVarPrefix("Console.");
  Parser.ReadRGBColor("Text.");
  Parser.ReadPosition();  
  if (!CheckValue(Parser.ReadNumeric("Width"), 128, 256)) {
	WriteMsg("[MainGameInterfaz] / Console.Width / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }
  Parser.ReadString("WAVBipFileName", false);
  
  // FXBack
  Parser.ReadRGBColor("FXBack.");
  Parser.ReadAlpha("FXBack.");

  // Opciones
  const std::string szConsoleOptions[] = {
	"UpButton.", "DownButton."
  };
  CheckOptionsButtonComponents(Parser, szConsoleOptions, 2);

  // Equipamiento
  // Formato de dibujado general para los slots
  Parser.SetVarPrefix("Equipment.");
  Parser.ReadRGBColor("DrawSlotsFormat.FXBack.Select.");
  Parser.ReadAlpha("DrawSlotsFormat.FXBack.Select.");
  Parser.ReadRGBColor("DrawSlotsFormat.FXBack.Unselect.");
  Parser.ReadAlpha("DrawSlotsFormat.FXBack.Unselect.");

  // Datos concretos asociados a los slots principales
  word uwIt = 0;
  for (; uwIt < 2; uwIt++) {
	// Se toma nombre slot con el que trabajar
	std::ostringstream StrText;
	StrText << "Slot[" 
			<< m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT, uwIt)
			<< "].";
	// Lee posicion slot y lo escribe
	Parser.ReadPosition(StrText.str());
	
	// Lee datos asociados al nombre del item equipado en slot
	Parser.ReadPosition(StrText.str() + "Name.");
	Parser.ReadRGBColor(StrText.str() + "Name.Select.");
	Parser.ReadRGBColor(StrText.str() + "Name.Unselect.");  
	
	// Lee nombre de la plantilla de animacion con el icono por defecto para el slot
	const std::string szAnimTemplate(Parser.ReadString(StrText.str() + "DefaultAnimTemplate"));	
  }   

  // Datos relacionados con el retrato
  Parser.SetVarPrefix("PlayerPortrait.");  
  Parser.ReadPosition();
  Parser.ReadRGBColor("FXBack.Select.");
  Parser.ReadAlpha("FXBack.Select.");
  Parser.ReadRGBColor("FXBack.Unselect.");
  Parser.ReadAlpha("FXBack.Unselect.");
  
  // Opciones de combate
  Parser.SetVarPrefix("Combat.");
  const std::string szCombatOptions[] = {
	"NextTurnButton.", "FleeButton."
  };
  CheckOptionsButtonComponents(Parser, szCombatOptions, 2);

  // Sintomas  
  Parser.SetVarPrefix("Symptoms.");
  
  // Componente alpha
  Parser.ReadAlpha();
  
  // Iconos
  const word uwNumSymptoms = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_SYMPTOM);
  uwIt = 0;
  for (; uwIt < uwNumSymptoms; ++uwIt) {
	// Se leen datos asociados al icono de sintoma que proceda
	std::ostringstream StrText;
	StrText << "Icon["
			<< 	m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_SYMPTOM, uwIt)
			<< "].";	
	const std::string szAnimTemplate(Parser.ReadString(StrText.str() + "AnimTemplate"));	
	Parser.ReadPosition(StrText.str());
  }  

  // Atributos
  Parser.SetVarPrefix("Attributes.");
  
  // Se establece el nombre de los atributos que pueden leerse desde disco
  const word uwMaxAttribs = 1 + m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB);
  std::string* const pszAttribs = new std::string[uwMaxAttribs]; 
  ASSERT(pszAttribs);
  uwIt = 0;
  pszAttribs[uwIt++] = "Health";
  for (; uwIt < uwMaxAttribs; ++uwIt) {
	pszAttribs[uwIt] = m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_EXTATTRIB,
											          uwIt - 1);
  }
  
  // Se podran mostrar hasta dos atributos simultaneamente que
  // seran el de salud y cualquiera de los atributos extendidos      
  std::string szTmp;          // Vble temporal	  
  byte ubNumAttribsFound = 0; // Contador de atributos encontrados  
  bool bResult = true;        // ¿Finalizacion correcta?
  uwIt = 0;
  while (uwIt < uwMaxAttribs && 
		 ubNumAttribsFound < 2) {
	// Se forma el nombre del atributo a leer
	std::ostringstream StrText;
	StrText << "Attribute[" << pszAttribs[uwIt]	<< "].";

	// Se intenta leer la posicion para mostrar el nombre, si se leyo se
	// leera el resto de informacion, guardandose en primer lugar el identificador
	// 0 = Salud, 1...MaxAttribs + 1 = atributos extendidos
	Parser.ReadPosition(StrText.str() + "Name.", false);	
	if (Parser.WasLastParseOk()) { 	  
	  // Se lee color de dibujado de los nombres
	  Parser.ReadRGBColor(StrText.str() + "Name.");	
  
	  // Lee posicion y color para los valores
	  Parser.ReadPosition(StrText.str() + "Value.");
	  Parser.ReadRGBColor(StrText.str() + "Value.");

	  // Lee posicion para el porcentaje y sus datos de configuracion
  	  Parser.ReadPosition(StrText.str() + "PercentageBar.");
	  Parser.ReadFlag(StrText.str() + "PercentageBar.HorizontalDrawFlag");
	  if (!CheckValue(Parser.ReadNumeric(StrText.str() + "PercentageBar.Lenght"), 1, 256)) {
		WriteMsg("[MainGameInterfaz] / Attributes.DrawFormat.PercentageBar.Lenght / Valor no válido.", 
				 CCrisolBuilder::MSG_ERROR);
	  }
	  if (!CheckValue(Parser.ReadNumeric(StrText.str() + "PercentageBar.Width"), 1, 256)) {
		WriteMsg("[MainGameInterfaz] / Attributes.DrawFormat.PercentageBar.Width / Valor no válido.", 
				 CCrisolBuilder::MSG_ERROR);
	  }	  
	  Parser.ReadAlpha(StrText.str() + "PercentageBar.");

	  // Inc. contador de atributos encontrados  
	  ++ubNumAttribsFound;
	}
	
	// Sig it.
	++uwIt;
  }

  // Datos para el panel de control horario
  Parser.SetVarPrefix("TimeController.");
  Parser.ReadAlpha("Unselect.");

  // Posicion de dibujado
  Parser.ReadPosition();

  // Nombres de las plantillas de animacion para cada uno de los paneles
  uwIt = 0;
  for (; uwIt < 24; ++uwIt) {
	// Se lee y escriben las plantillas de animacion
	std::ostringstream StrText;
	StrText << "TimeButton[" << uwIt << "].AnimTemplate";
	const std::string szAnimTemplate(Parser.ReadString(StrText.str()));	
  }
  
  // Retorna
  return !(GetNumErrors() > 0);
} 
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion del archivo de interfaces [PlayerProfileWindow]  
// Parametros:
// - Parser. Parser a utilizar.
// - CBBFile. Fichero de escritura donde volcar la informacion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazPlayerProfileWindow(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());

  // Imagen de fondo
  if (!Parser.SetSection("[PlayerProfileWindow]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [PlayerProfileWindow]", CCrisolBuilder::MSG_ERROR);
	return false;
  }
  Parser.SetVarPrefix("");	  
  CheckBackImgComponent(Parser, true);

  // Datos asociados al texto flotante
  Parser.SetVarPrefix("FloatingText.");
  Parser.ReadRGBColor();

  // Consola
  // Datos para el control del texto
  Parser.SetVarPrefix("Console.");
  Parser.ReadRGBColor("Text.");
  Parser.ReadPosition();
  Parser.ReadNumeric("Width");
  if (!CheckValue(Parser.ReadNumeric("Width"), 128, 256)) {
	WriteMsg("[PlayerProfileWindow] / Console.Width / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }
  Parser.ReadString("WAVBipFileName", false);
  
  // FXBack
  Parser.ReadRGBColor("FXBack.");
  Parser.ReadAlpha("FXBack.");

  // Opciones
  const std::string szConsoleOptions[] = {
	"UpButton.", "DownButton."
  };
  CheckOptionsButtonComponents(Parser, szConsoleOptions, 2);

  // Inventario
  // Selector de items
  // Obtiene ancho y alto del area de seleccion y los ajusta si procede
  // Nota: En lugar de mandar mensajes de error, se optara por ajustar valores
  Parser.SetVarPrefix("Inventory.");  
  if (!CheckValue(Parser.ReadNumeric("ItemSelector.Area.Width"), 1, 8)) {
	WriteMsg("[PlayerProfileWindow] / Inventory.ItemSelector.Area.Width / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }
  if (!CheckValue(Parser.ReadNumeric("ItemSelector.Area.Height"), 1, 6)) {
	WriteMsg("[PlayerProfileWindow] / ItemSelector.Area.Height / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }

  // Se lee posicion inicial de dibujado y se establece la maxima
  Parser.ReadPosition("ItemSelector.");
  
  // Se guarda selector de items
  Parser.ReadRGBColor("ItemSelector.FXBack.Select.");
  Parser.ReadAlpha("ItemSelector.FXBack.Select.");
  Parser.ReadRGBColor("ItemSelector.FXBack.Unselect.");
  Parser.ReadAlpha("ItemSelector.FXBack.Unselect.");
  
  // Texto flotante
  Parser.ReadRGBColor("ItemSelector.FloatingText.");
  
  // Opciones
  const std::string szInventoryOptions[] = {
	"UpButton.", "DownButton."
  };
  CheckOptionsButtonComponents(Parser, szInventoryOptions, 2);

  // Sintomas
  Parser.SetVarPrefix("Symptoms.");
  // Alpha para los sintomas no presentes
  Parser.ReadAlpha("NoPresent.");
  
  // Iconos de sintomas
  const word uwNumSymptoms = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_SYMPTOM);
  word uwIt = 0;
  for (; uwIt < uwNumSymptoms; ++uwIt) {
	// Se carga informacion relativa a los iconos de los sintomas y se escribe
	std::ostringstream StrText;
	StrText << "Symptom["
			<< m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_SYMPTOM,
											  uwIt)
			<< "].";
	const std::string szAnimTemplate(Parser.ReadString(StrText.str() + "AnimTemplate"));	
	Parser.ReadPosition(StrText.str());
  }  

  // Iconos de habilidades
  Parser.SetVarPrefix("Habilities.");
  const word uwNumHabilities = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);  
  uwIt = 0;
  for (; uwIt < uwNumHabilities; ++uwIt) {	
	// Se carga informacion relativa a los iconos de habilidades
	std::ostringstream StrText;
	StrText << "Hability[" 
			<< m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_HABILITY,
										      uwIt)
			<< "].";
	const std::string szAnimTemplate(Parser.ReadString(StrText.str() + "AnimTemplate"));	
	Parser.ReadPosition(StrText.str());
  }

  // Retrato del jugador
  Parser.SetVarPrefix("PlayerPortrait.");
  Parser.ReadPosition();
  Parser.ReadRGBColor("FXBack.Unselect.");
  Parser.ReadAlpha("FXBack.Unselect.");
  Parser.ReadRGBColor("FXBack.Select.");
  Parser.ReadAlpha("FXBack.Select.");
  
  // Slots de equipamiento
  Parser.SetVarPrefix("EquipmentSlots.");
  // Se lee formato de dibujado  
  // Flag de uso de nombres y formato de escritura de los mismos si procede
  const bUseNameInEquipSlots = Parser.ReadFlag("DrawFormat.Names.UseFlag");
  if (bUseNameInEquipSlots) {
	Parser.ReadRGBColor("DrawFormat.Names.Unselect.");
	Parser.ReadRGBColor("DrawFormat.Names.Select.");
  }  

  // Formato de los slots de seleccion
  Parser.ReadRGBColor("DrawFormat.FXBack.Unselect.");
  Parser.ReadAlpha("DrawFormat.FXBack.Unselect.");
  Parser.ReadRGBColor("DrawFormat.FXBack.Select.");
  Parser.ReadAlpha("DrawFormat.FXBack.Select.");

  // Se leen los datos concretos asociados a los distintos slots y se guardan
  // Nota: Los datos asociados a los nombres se guardaran AUNQUE no se usen
  const word uwNumEquipSlots = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT);
  uwIt = 0;  
  for (; uwIt < uwNumEquipSlots; ++uwIt) {
	std::ostringstream StrText;
	StrText << "Slot[" 
			<< m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT,
											  uwIt)
			<< "].";
	Parser.ReadPosition(StrText.str());
	if (bUseNameInEquipSlots) {
	  Parser.ReadPosition(StrText.str() + "Name.");
	}	
  } 

  // Atributos
  Parser.SetVarPrefix("Attributes.");
  // Datos de configuracion de dibujado
  // Para nombres
  Parser.ReadRGBColor("DrawFormat.Names.");
  // Para valores
  Parser.ReadRGBColor("DrawFormat.Values.");  
  // Para barras de porcentaje
  Parser.ReadFlag("DrawFormat.PercentageBars.HorizontalDrawFlag");  
  if (!CheckValue(Parser.ReadNumeric("DrawFormat.PercentageBars.Lenght"), 1, 256)) {
	WriteMsg("[PlayerProfileWindow] / Attributes.DrawFormat.PercentageBar.Lenght / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }  
  if (!CheckValue(Parser.ReadNumeric("DrawFormat.PercentageBars.Width"), 1, 256)) {
	WriteMsg("[PlayerProfileWindow] / Attributes.DrawFormat.PercentageBar.Width / Valor no válido.", 
	         CCrisolBuilder::MSG_ERROR);
  }  
  Parser.ReadAlpha("DrawFormat.PercentageBars.");

  // Se procesan cada uno de los atributos y se van guardando
  // Nombre
  Parser.ReadPosition("Player.Name.");
    
  // Genero / Tipo / Clase
  Parser.ReadPosition("GenreTypeClass.Name.");
    
  // Nivel
  Parser.ReadPosition("Level.Name.");  
  Parser.ReadPosition("Level.Value.");
  
  // Experiencia
  Parser.ReadPosition("Experience.Name.");  
  Parser.ReadPosition("Experience.Value.");
  
  // Puntos de combate
  Parser.ReadPosition("CombatPoints.Name.");
  Parser.ReadPosition("CombatPoints.Value.");
        
  // Salud
  Parser.ReadPosition("Health.Name.");
  Parser.ReadPosition("Health.Value.");
  Parser.ReadPosition("Health.PercentageBar.");
   
  // Atributos extendidos  
  const word uwNumExtAttribs = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB);
  uwIt = 0;
  for (; uwIt < uwNumExtAttribs; ++uwIt) {
	// Se establece nombre vble
	std::ostringstream StrText;
	StrText << "ExtendedAttribute["
			<<  m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_EXTATTRIB,
											   uwIt)
			<< "].";
	Parser.ReadPosition(StrText.str() + "Name.");
	Parser.ReadPosition(StrText.str() + "Value.");
	Parser.ReadPosition(StrText.str() + "PercentageBar.");	
  }

  // Opcion de salida
  Parser.SetVarPrefix("");	  
  const std::string szExitOption[] = {
	"ExitButton."
  };
  CheckOptionsButtonComponents(Parser, szExitOption, 1);

  // Retorna
  return !(GetNumErrors() > 0);
} 

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea la seccion del archivo de interfaces [PlayerCreatorWindow]  
// Parametros:
// - Parser. Parser a utilizar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckInterfazPlayerCreatorWindow(CCBTToolParser& Parser)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());

  // Imagen de fondo
  if (!Parser.SetSection("[PlayerCreator]")) {
	WriteMsg(Parser.GetFileName() + " / No se halló la sección [PlayerCreator]", CCrisolBuilder::MSG_ERROR);
	return false;
  }
  Parser.SetVarPrefix("");	  
  CheckBackImgComponent(Parser, true);

  // Datos asociados al texto flotante
  Parser.SetVarPrefix("FloatingText.");
  Parser.ReadRGBColor();

  // Se leen datos sobre la musica
  Parser.SetVarPrefix("MIDIMusic.");  
  Parser.ReadString("FileName", false);
  Parser.ReadFlag("LoopFlag", false);

  // Opciones generales
  Parser.SetVarPrefix("");	  
  const std::string szOptions[] = {
	"CancelButton.", "AcceptButton."
  };
  CheckOptionsButtonComponents(Parser, szOptions, 2);

  // Trabajo con el nombre del jugador
  // Texto indicativo
  Parser.SetVarPrefix("Name.");
  Parser.ReadPosition("InfoText.");
  Parser.ReadRGBColor("InfoText.Select.");  
  Parser.ReadRGBColor("InfoText.Unselect.");
  
  // Entrada  
  Parser.ReadPosition("Input.");    
  if (!CheckValue(Parser.ReadNumeric("Input.MaxCharacters"), 1, 256)) {
	WriteMsg("[PlayerCreatorWindow] / Name.Input.MaxCharacters / Valor no válido.", 
			 CCrisolBuilder::MSG_ERROR);
  } 
  Parser.ReadRGBColor("Input.Select.");
  Parser.ReadRGBColor("Input.Unselect.");
  
  // Retrato
  Parser.SetVarPrefix("PlayerPortrait.");
  // Posicion de dibujado
  Parser.ReadPosition();
  // Botones de avanzar y retroceder retratos
  const std::string szGenrePortraitOptions[] = {
	"NextButton.", "PrevButton."
  };
  CheckOptionsButtonComponents(Parser, szGenrePortraitOptions, 2);


  // Genero / Tipo / Clase
  const std::string szGenreTypeClass[] = {
	"Genre.", "Type.", "Class."
  };
  word uwIt = 0;
  for (; uwIt < 3; ++uwIt) {
	// Establece prefijo
	Parser.SetVarPrefix(szGenreTypeClass[uwIt]);
	
	// Texto de opcion
	Parser.ReadPosition("InfoText.");
	Parser.ReadRGBColor("InfoText.Select.");  
	Parser.ReadRGBColor("InfoText.Unselect.");
	
	// Texto seleccionado
	Parser.ReadPosition("Selection.");
	Parser.ReadRGBColor("Selection.");  
	
	// FXBack asociado al texto seleccionado
	Parser.ReadRGBColor("Selection.FXBack.");
	Parser.ReadAlpha("Selection.FXBack.");
  }  

  // Atributos
  Parser.SetVarPrefix("Attributes.");
  // Datos de configuracion de dibujado
  // Para nombres
  Parser.ReadRGBColor("DrawFormat.Names.Select.");
  Parser.ReadRGBColor("DrawFormat.Names.Select.");
  // Para valores base
  Parser.ReadRGBColor("DrawFormat.BaseValues.");
  Parser.ReadRGBColor("DrawFormat.BaseValues.FXBack.");
  Parser.ReadAlpha("DrawFormat.BaseValues.FXBack.");
  // Para valores incrementados
  Parser.ReadRGBColor("DrawFormat.ImproveValues.");
 
  // Se procede a escribir los atributos en disco
  // Salud
  Parser.ReadPosition("Health.Name.");
  Parser.ReadPosition("Health.BaseValue.");
  
  // Puntos de combate
  Parser.ReadPosition("CombatPoints.Name.");
  Parser.ReadPosition("CombatPoints.BaseValue.");
  
  // Atributos extendidos  
  const word uwNumExtAttribs = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB);
  uwIt = 0;
  for (; uwIt < uwNumExtAttribs; ++uwIt) {
	// Se establece nombre vble
	std::ostringstream StrText;
	StrText << "ExtendedAttribute["
			<<  m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_EXTATTRIB,
											   uwIt)
			<< "].";
	Parser.ReadPosition(StrText.str() + "Name.");
	Parser.ReadPosition(StrText.str() + "BaseValue.");	
  }

  // Se escriben los puntos de distribucion asociados a los atributos
  Parser.ReadPosition("DistributionPoints.");
  Parser.ReadRGBColor("DrawFormat.DistributionPoints.");
  
  // Se escriben la barra de porcentaje asociada a los puntos de distribuicion
  Parser.ReadPosition("DistributionPoints.PercentageBar.");
  Parser.ReadFlag("DistributionPoints.PercentageBar.HorizontalDrawFlag");  
  if (!CheckValue(Parser.ReadNumeric("DistributionPoints.PercentageBar.Lenght"), 1, 256)) {
	WriteMsg("[PlayerCreatorWindow] / Attributes.DistributionPoints.PercentageBar.Lenght / Valor no válido.", 
			 CCrisolBuilder::MSG_ERROR);
  } 
  Parser.ReadNumeric("DistributionPoints.PercentageBar.Width");
  if (!CheckValue(Parser.ReadNumeric("DistributionPoints.PercentageBar.Width"), 1, 256)) {
	WriteMsg("[PlayerCreatorWindow] / Attributes.DistributionPoints.PercentageBar.Width / Valor no válido.", 
			 CCrisolBuilder::MSG_ERROR);
  } 
  Parser.ReadAlpha("DistributionPoints.PercentageBar.");
  
  // Opciones para el control de los atributos
  const std::string szAttributesOptions[] = {
	"IncrementValueButton.", "DecrementValueButton."
  };
  CheckOptionsButtonComponents(Parser, szAttributesOptions, 2);

  // Habilidades
  Parser.SetVarPrefix("Habilities.");
  
  // Formato de dibujado
  Parser.ReadRGBColor("DrawFormat.Names.Select.");
  Parser.ReadRGBColor("DrawFormat.Names.Unselect.");
  Parser.ReadRGBColor("DrawFormat.Names.FXBack.");
  Parser.ReadAlpha("DrawFormat.Names.FXBack.");

  // Se escriben los iconos y nombres de habilidades
  const word uwNumHabilities = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);  
  uwIt = 0;
  for (; uwIt < uwNumHabilities; ++uwIt) {	
	// Se carga informacion relativa a los iconos de habilidades
	std::ostringstream StrText;
	StrText << "Hability[" 
			<< m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_HABILITY,
										      uwIt)
			<< "].";
	// Icono
	const std::string szAnimTemplate(Parser.ReadString(StrText.str() + "Icon.AnimTemplate"));	
	Parser.ReadPosition(StrText.str() + "Icon.");
	
	// Nombre
	Parser.ReadPosition(StrText.str() + "Name.");
  }

  // Puntos de distribucion para las habilidades
  // Puntos
  Parser.ReadPosition("DistributionPoints.");  
  Parser.ReadRGBColor("DrawFormat.DistributionPoints.");
  
  // Barra de porcentaje
  Parser.ReadPosition("DistributionPoints.PercentageBar.");
  Parser.ReadFlag("DistributionPoints.PercentageBar.HorizontalDrawFlag");
  if (!CheckValue(Parser.ReadNumeric("DistributionPoints.PercentageBar.Lenght"), 1, 256)) {
	WriteMsg("[PlayerCreatorWindow] / Habilities.DistributionPoints.PercentageBar.Lenght / Valor no válido.", 
			 CCrisolBuilder::MSG_ERROR);
  }   
  if (!CheckValue(Parser.ReadNumeric("DistributionPoints.PercentageBar.Width"), 1, 256)) {
	WriteMsg("[PlayerCreatorWindow] / Habilities.DistributionPoints.PercentageBar.Width / Valor no válido.", 
			 CCrisolBuilder::MSG_ERROR);
  }   
  Parser.ReadAlpha("DistributionPoints.PercentageBar.");

  // Fades
  // Color de la pantalla para el fade in
  Parser.SetVarPrefix("Fade.");
  Parser.ReadRGBColor("Start.");
  
  // Velocidad del FadeIn  
  if (!CheckValue(Parser.ReadNumeric("In.Speed"), 1, 256)) {
	WriteMsg("[PlayerCreatorWindow] / Fade.In.Speed / Valor no válido.", 
			 CCrisolBuilder::MSG_ERROR);
  } 
  // Fade out resultante de dar a cancelar
  Parser.ReadRGBColor("Out.OnCancel.");
  if (!CheckValue(Parser.ReadNumeric("Out.OnCancel.Speed"), 1, 256)) {
	WriteMsg("[PlayerCreatorWindow] / Fade.Out.OnCancel.Speed / Valor no válido.", 
			 CCrisolBuilder::MSG_ERROR);
  } 
  
  // Fade out resultante de dar a aceptar
  Parser.ReadRGBColor("Out.OnAccept.");  
  if (!CheckValue(Parser.ReadNumeric("Out.OnAccept.Speed"), 1, 256)) {
	WriteMsg("[PlayerCreatorWindow] / Fade.Out.OnAccept.Speed / Valor no válido.", 
			 CCrisolBuilder::MSG_ERROR);
  } 
  
  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo de apoyo para el chequeo de las imagenees de fondo
// Parametros:
// - Parser. Parser a utilizar.
// - bWithAnims. Flag para indicar si dicho componente tendra o no animaciones
//   asociadas.
// Devuelve:
// Notas:
// - El formato del texto sera "BackImage." + resto
///////////////////////////////////////////////////////////////////////////////
void 
CCrisolBuilder::CheckBackImgComponent(CCBTToolParser& Parser,
									  const bool bWithAnims)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());

  // ¿Animaciones asociadas?
  if (bWithAnims) {
	// Se leen datos de la imagen de fondo y se escribe
	// Imagen de fondo
	Parser.ReadString("BackImage.FileName");
	
	// Animaciones asociadas, si procede
	word uwIt = 0;
	for (; uwIt < 0XFF; ++uwIt) {
	  // Se lee la posible plantilla de animacion asociada a animacion
	  std::ostringstream StrText;
	  StrText << "BackImage.Anim[" << uwIt << "].";
	  const std::string szAnimTemplate(Parser.ReadString(StrText.str() + "AnimTemplate", false));	
	  // ¿Se leyo informacion?
	  if (Parser.WasLastParseOk()) {
		// Si, se lee posicion y alpha y se añade a la imagen
		Parser.ReadPosition(StrText.str());
		Parser.ReadAlpha(StrText.str(), false);		
	  } else {
		// No, se abandona
		break;
	  }
	}
  } else {
	// Se leen datos de la imagen de fondo y se escribe
	Parser.ReadString("BackImage.FileName");
	Parser.ReadPosition("BackImage.");
	Parser.ReadAlpha("BackImage.");
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo de apoyo para la lectura de botones de opciones 
// Parametros:
// - Parser. Parser a utilizar.
// - pszOptions. Array de strings con los nombres de los botones a cargar
// - uwNumOptions. Numero de opciones en pszOptions
// Devuelve:
// Notas:
// - El formato del texto sera "Options." + Opcion recibida + resto
///////////////////////////////////////////////////////////////////////////////
void 
CCrisolBuilder::CheckOptionsButtonComponents(CCBTToolParser& Parser,
											 const std::string* pszOptions,
											 const word uwNumOptions)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());
  ASSERT(pszOptions);

  // Se cargan opciones
  word uwIt = 0;
  for (; uwIt < uwNumOptions; ++uwIt) {
	const std::string szAnimTemplate(Parser.ReadString("Options." + pszOptions[uwIt] + "AnimTemplate"));
	Parser.ReadPosition("Options." + pszOptions[uwIt]);		
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Construye el archivo de plantillas de animacion.
// * El formato del archivo de plantillas sera:
//   - Codigo de tipo de archivo. (byte)
//   - Version alta (byte)
//   - Version baja (byte)
//   - Offset a indice de plantillas de animacion, offsets a datos (streampos)
//   - Datos de una plantilla:
//      - Estados (word)
//      - Orientaciones (word)
//      - Ancho frames (word)
//      - Alto frames (word)
//      - Fps (byte)
//      * Por cada estado:
//        - Estado de enlace (word)
//        - Ciclos de espera (float)
//        - Flag de modo reversible (bool)
//        - Numero frames (word)
//        - Nombre del wav asociado (tamaño string + string)
//        - Flag asociado a wav (bool)
//        * Por cada orientacion
//          - Localizacion del frame en X (sword)
//          - Localizacion del frame en Y (sword)
//          - El nombre del fichero (tamaño string + string)
//   * Indice de plantillas de animacion 
//      - Numero de plantillas
//      * Y por cada plantilla:
//         - Nombre plantilla (tamaño string + string)
//         - Offset en datos de plantilla (streampos)
// Parametros:
// - szCBTFile. Nombre del archivo donde leer y guardar los datos.
// Devuelve:
// - Si existio algun problema false, en caso contrario true.
// Notas:
// - El archivo de salida siempre sera "AnimTemplates.cbb"
// - El archivo se construira despues de que se hayan leido del resto de 
//   archivos CBT las plantillas de animacion requeridas.
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::BuildAnimTemplateFile(const std::string& szCBTFile)
{
  // Se inicializa CBTParser y fichero de escritura
  CCBTToolParser CBTParser;
  std::ofstream CBBFile;
  if (!InitBuild(CBTParser, CBBFile, szCBTFile + ".cbt", szCBTFile + ".cbb")) {
	return false;
  }

  // Se escribe codigo de archivo / version
  CBBFile.write((sbyte *)(&CBDefs::CBBAnimTemplateFile), sizeof(byte));
  CBBFile.write((sbyte *)(&CBDefs::HVersion), sizeof(byte));
  CBBFile.write((sbyte *)(&CBDefs::LVersion), sizeof(byte));

  // Se deja espacio para guardar posteriormente el offset a zona de indices
  std::streampos IndexOffset = 0;
  CBBFile.write((sbyte *)(&IndexOffset), sizeof(std::streampos));

  // Se escribiran las plantillas de animacion que se hayan utilizado
  StreamPosList ListOfOffsets;
  StrSet Profiles;
  CBTParser.CreateSetOfSections(Profiles);  
  StrSetIt SectionIt(Profiles.begin());
  for (; SectionIt != Profiles.end(); ++SectionIt) {
	// Se indica la plantilla que se escribe si procede
	if (m_ParamsInfo.bReportAnimTemplates) {
	  WriteMsg("Escribiendo plantilla de animación: " + *SectionIt, 
			   CCrisolBuilder::MSG_NORMAL);
	}

	
	// Se guarda offset y se localiza seccion con datos
	ListOfOffsets.push_back(CBBFile.tellp());
	if (!CBTParser.SetSection(*SectionIt)) {
	  WriteMsg(CBTParser.GetFileName() + " / No se halló la sección " + *SectionIt, CCrisolBuilder::MSG_ERROR);
	  return false;
	}
	
	// Estados	
	CBTParser.SetVarPrefix("Configuration.");	
	const word uwNumStates = CBTParser.ReadNumeric("NumStates");
	if (uwNumStates > AnimTemplateDefs::MAX_ANIMSTATES ||
		0 == uwNumStates) {
	  std::ostringstream StrText;
	  StrText << CBTParser.GetSectionName()
			  << " / Configuration.NumStates / El número de estados "
			  << uwNumStates
			  << " no es válido.";
	  WriteMsg(StrText.str(), CCrisolBuilder::MSG_ERROR);
	  return false;
	}   
	CBBFile.write((sbyte *)(&uwNumStates), sizeof(word));

	// Orientaciones
	const word uwNumOrients = CBTParser.ReadNumeric("NumOrientations");
	if (uwNumOrients > AnimTemplateDefs::MAX_ANIMORIENTATIONS ||
		0 == uwNumOrients) {
	  WriteMsg(*SectionIt + " / Configuration.NumOrientations / Número de orientaciones incorrectas.",
			   CCrisolBuilder::MSG_ERROR);
	  return false;
	} 
	CBBFile.write((sbyte *)(&uwNumOrients), sizeof(word));
	
	// Dimensiones de los frames
	word uwValue = CBTParser.ReadNumeric("FramesWidth");
	CBBFile.write((sbyte *)(&uwValue), sizeof(word));
	uwValue = CBTParser.ReadNumeric("FramesHeight");
	CBBFile.write((sbyte *)(&uwValue), sizeof(word));

	// Frames
	AnimTemplateDefs::FramesPerSecond Fps;
	Fps = CBTParser.ReadNumeric("FPS");
	CBBFile.write((sbyte *)(&Fps), sizeof(AnimTemplateDefs::FramesPerSecond));

	// Ahora se cargan los estados
	word uwStateIt = 0;
	for (; uwStateIt < uwNumStates; ++uwStateIt) {	  
	  // Se establece estado
	  std::ostringstream StrStream; 
	  StrStream << "State[" << word(uwStateIt) << "].";
	  CBTParser.SetVarPrefix(StrStream.str());

	  // Estado de enlace
	  // En caso de que no este, se tomara el estado actual como estado de enlace  
	  AnimTemplateDefs::AnimFrame LinkState = CBTParser.ReadNumeric("LinkState", false);	
	  if (!CBTParser.WasLastParseOk()) {
		LinkState = uwStateIt;
	  } else if (! (LinkState < uwNumStates)) {
		// El estado de enlace corresponde a un estado no valido
		std::ostringstream StrError;
		StrError << CBTParser.GetSectionName()
				 << " / " 
				 << StrStream.str() 
				 << "LinkState"
				 << " / El estado de enlace no es válido";
		WriteMsg(StrError.str(), CCrisolBuilder::MSG_ERROR);
		return false;		
	  }
	  CBBFile.write((sbyte *)(&LinkState), sizeof(AnimTemplateDefs::AnimFrame));
	  
	  // Se leera el tiempo de espera que podra tener asociado
	  // Nota: Solo podra tenerlo si el estado es ciclico y en caso de poder
	  // tenerlo, debera de ser superior a 1.0f para tenerse en cuenta
	  float fCycleWait = CBTParser.ReadNumeric("TimeBetweenCycles", false);
	  if (CBTParser.WasLastParseOk() && 
	  	  (fCycleWait < 1.0f || LinkState != uwStateIt)) {
		fCycleWait = 0.0f;		  
	  }
	  CBBFile.write((sbyte *)(&fCycleWait), sizeof(float));
	  
	  // Flag de modo reversible
	  // Nota: Por defecto, no reversible
	  bool bFlag = CBTParser.ReadFlag("ReversibleModeFlag", false);
	  CBBFile.write((sbyte *)(&bFlag), sizeof(bool));
  
	  // Numero de frames
	  const word uwNumFrames = CBTParser.ReadNumeric("NumFrames");	  
	  if (uwNumFrames < 1) {
		WriteMsg(*SectionIt + StrStream.str() + "NumFrames / Número de frames insuficientes.",
			     CCrisolBuilder::MSG_ERROR);
		return false;
	  }
	  CBBFile.write((sbyte *)(&uwNumFrames), sizeof(word));

	  // WAV asociado
	  // Si hay fichero asociado (hay nombre) se leera el flag
	  WriteString(CBBFile, CBTParser.ReadString("WAVFileName", false));
	  if (CBTParser.WasLastParseOk()) {
		bFlag = CBTParser.ReadFlag("WAVFlag");		
	  } else {
		bFlag = false;
	  }
	  CBBFile.write((sbyte *)(&bFlag), sizeof(bool));
	  
	  // Se proceden a registrar los frames de cada una de las orientaciones
	  AnimTemplateDefs::AnimOrientation OrientsIt = 0;
	  for (; OrientsIt < uwNumOrients; ++OrientsIt) {
		// Se establece orientacion actual
		std::ostringstream StrActOrient; 
		StrActOrient << "Orientation[" << word(OrientsIt) << "].";

		// Se leen las coordenadas de lectura para el inicio de lectura
		sword swPosValue = CBTParser.ReadNumeric(StrActOrient.str() + "CellStartXPos", false);
		CBBFile.write((sbyte *)(&swPosValue), sizeof(sword));
		swPosValue = CBTParser.ReadNumeric(StrActOrient.str() + "CellStartYPos", false);
		CBBFile.write((sbyte *)(&swPosValue), sizeof(sword));	

		// Se lee el archivo con los frames
		WriteString(CBBFile, CBTParser.ReadString(StrActOrient.str() + "FramesFileName"));		
	  }
	}  	
  }
  
  // Se guarda el offset donde se alojara el indice de ficheros
  IndexOffset = CBBFile.tellp();
  CBBFile.seekp(3 * sizeof(byte), std::ios::beg);
  CBBFile.write((sbyte *)(&IndexOffset), sizeof(std::streampos));

  // Se escribe el indice de secciones
  CBBFile.seekp(0, std::ios::end);
  WriteIndexOfSections(CBBFile, Profiles, ListOfOffsets);

  // Cierra y retorna
  CBBFile.close();
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea archivo de texto estatico
// Parametros:
// - szCBTFile. Nombre del fichero CBT sin extension
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
// - El texto estatico hace referencia a texto que en un momento dado sera
//   cogido automaticamente por el motor, no sera obligatorio pero se advertira
//   el hecho de que no se halle.
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckStaticTextFile(const std::string& szCBTFile)
{
  // Se inicializa CBTParser y fichero de escritura
  CCBTToolParser CBTParser;
  if (!CBTParser.Init(szCBTFile + ".cbt")) {
	WriteMsg("No se puede abrir el archivo " + szCBTFile + ".cbt", 
			 CCrisolBuilder::MSG_ERROR);
	return false;
  }

  // Tipos de criaturas
  if (!CBTParser.SetSection("[CriatureTypes]")) {
	WriteMsg(CBTParser.GetFileName() + " / No se halló la sección [CriatureTypes]", CCrisolBuilder::MSG_ERROR);
	return false;
  }
  CBTParser.SetVarPrefix("");
  CheckSecuenceOfStaticText(CBTParser, 
							m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_TYPE),
							"Type");
  
  // Clases de criaturas
  if (!CBTParser.SetSection("[CriatureClasses]")) {
	WriteMsg(CBTParser.GetFileName() + " / No se halló la sección [CriatureClasses]", CCrisolBuilder::MSG_ERROR);
	return false;
  }
  CBTParser.SetVarPrefix("");
  CheckSecuenceOfStaticText(CBTParser, 
							m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_CLASS),
							"Class");

  // Generos de criaturas
  if (!CBTParser.SetSection("[CriatureGenres]")) {
	WriteMsg(CBTParser.GetFileName() + " / No se halló la sección [CriatureGenres]", CCrisolBuilder::MSG_ERROR);
	return false;
  }
  CBTParser.SetVarPrefix("");
  CheckSecuenceOfStaticText(CBTParser, 
							m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_GENRE),
							"Genre");
  
  // Habilidades de criaturas
  if (!CBTParser.SetSection("[CriatureHabilities]")) {
	WriteMsg(CBTParser.GetFileName() + " / No se halló la sección [CriatureHabilities]", CCrisolBuilder::MSG_ERROR);
	return false;
  }
  CBTParser.SetVarPrefix("");
  CheckSecuenceOfStaticText(CBTParser, 
							m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY),
							"Hability");

  // Sintomas de criaturas
  if (!CBTParser.SetSection("[CriatureSymptoms]")) {
	WriteMsg(CBTParser.GetFileName() + " / No se halló la sección [CriatureSymptoms]", CCrisolBuilder::MSG_ERROR);
	return false;
  }
  CBTParser.SetVarPrefix("");
  CheckSecuenceOfStaticText(CBTParser, 
							m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_SYMPTOM),
							"Symptom");

  // Slots de equipamiento
  if (!CBTParser.SetSection("[CriatureEquipmentSlots]")) {
	WriteMsg(CBTParser.GetFileName() + " / No se halló la sección [CriatureEquipmentSlots]", CCrisolBuilder::MSG_ERROR);
	return false;
  }
  CBTParser.SetVarPrefix("");
  CheckSecuenceOfStaticText(CBTParser, 
							m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT),
							"EquipmentSlot");

  // Atributos de criaturas
  if (!CBTParser.SetSection("[CriatureAttributes]")) {
	WriteMsg(CBTParser.GetFileName() + " / No se halló la sección [CriatureAttributes]", CCrisolBuilder::MSG_ERROR);
	return false;
  }
  CBTParser.SetVarPrefix("");  
  // Atributos base
  const std::string szCriatureBaseAttribs[] = {
	"Level", "Experience", "Health", "CombatPoints"
  };
  word uwIt = 0;
  for (; uwIt < 4; ++uwIt) {
	const std::string szText(CBTParser.ReadString(szCriatureBaseAttribs[uwIt]));
	if (szText.empty()) {
	  WriteMsg("[CriatureAttributes] / " + szCriatureBaseAttribs[uwIt] + " / Sin texto.", 
			   CCrisolBuilder::MSG_WARNING);	  
	}
  }
  // Atributos extendidos
  CheckSecuenceOfStaticText(CBTParser, 
							m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB),
							"ExtendedAttribute");

  // Textos de interfaces
  if (!CBTParser.SetSection("[Interfaces]")) {
	WriteMsg(CBTParser.GetFileName() + " / No se halló la sección [Interfaces]", CCrisolBuilder::MSG_ERROR);
	return false;
  }
  CBTParser.SetVarPrefix("");
  const std::string szInterfaceVars[] = {
	"SaverSavedFileManager.SavedFileDesc",
	"EntityInteract.MainMenu.Option[Observe]",
	"EntityInteract.MainMenu.Option[Talk]",
	"EntityInteract.MainMenu.Option[Get]",
	"EntityInteract.MainMenu.Option[Register]",
	"EntityInteract.MainMenu.Option[Manipulate]",
	"EntityInteract.MainMenu.Option[DropItem]",
	"EntityInteract.MainMenu.Option[UseInventory]",
	"EntityInteract.MainMenu.Option[UseHability]",
	"EntityInteract.MainMenu.Option[Exit]",
	"EntityInteract.HabilityMenu.Option[ReturnToMainMenu]",
	"PlayerCreation.Name",
	"PlayerCreation.Genre",
	"PlayerCreation.Class"
  };
  uwIt = 0;
  for (; uwIt < 4; ++uwIt) {
	CBTParser.ReadString(szInterfaceVars[uwIt]);
	if (!CBTParser.WasLastParseOk()) {
	  WriteMsg("[Interfaces] / " + szInterfaceVars[uwIt] + " / Sin texto.", 
			   CCrisolBuilder::MSG_WARNING);	  
	}
  }
  
  // Ahora se debera de leer el texto asociado a las habilidades
  const word uwMaxHabilities = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);
  uwIt = 0;
  for (; uwIt < uwMaxHabilities; ++uwIt) {
	std::ostringstream StrText;
	StrText << "EntityInteract.HabilityMenu.Option["
			<< m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_HABILITY, uwIt)
			<< "]";
	CBTParser.ReadString(StrText.str());  
  }

  // El texto asociado a la version sera el unico que podra faltar
  CBTParser.ReadString("MainMenu.VersionInfo", false);  

  // Mensajes de informacion
  if (!CBTParser.SetSection("[Messages]")) {
	WriteMsg(CBTParser.GetFileName() + " / No se halló la sección [Messages]", CCrisolBuilder::MSG_ERROR);
	return false;
  }
  CBTParser.SetVarPrefix("");
  const std::string szMessageVars[] = {
	"InsuficientActionPoints",
	"CantMoveTo",
	"AreaLoading",
	"GameLoading",
	"NoSaveGamePermission",
	"ExitGameConfirm"
  };
  uwIt = 0;
  for (; uwIt < 5; ++uwIt) {
	CBTParser.ReadString(szMessageVars[uwIt], false);
	if (!CBTParser.WasLastParseOk()) {
	  WriteMsg("[Messages] / " + szMessageVars[uwIt] + " / Sin texto.", 
			   CCrisolBuilder::MSG_WARNING);	  
	}
  }  

  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea en disco una sucesion de texto estatico. Una sucesion de texto
//   estatico sera una secuencia de strings de la forma:
//   * Var[Id] = Texto
//   Donde Var sera un nombre de vble recibido e Id el identificador actual
//   de la secuencia. De la secuencia sabremos el identificador maximo.
//   La secuencia de texto se solicitara que exista obligatoriamente.
// Parametros:
// - Parser. Parser a utilizar.
// - uwMaxValue. Valor maximo para la secuencia
// - szSeqVar. Nombre de la vble asociada a la secuencia.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCrisolBuilder::CheckSecuenceOfStaticText(CCBTToolParser& Parser,
										  const word uwMaxValue,
										  const std::string& szSeqVar)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(Parser.IsInitOk());
  ASSERT(Parser.IsSectionActive());

  // Escribe la sucesion
  word uwIt = 0;
  for (; uwIt < uwMaxValue; ++uwIt) {
	std::ostringstream StrText;
	StrText << szSeqVar << "[" << uwIt << "]";
	Parser.ReadString(StrText.str());
	if (!Parser.WasLastParseOk()) {
	  WriteMsg(Parser.GetSectionName() + " / " + StrText.str() + " / Sin texto.", 
			   CCrisolBuilder::MSG_WARNING);	  
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea archivo de definiciones GFX
// Parametros:
// - szCBTFile. Nombre del archivo CBT con los datos.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckGFXDefsFile(const std::string& szCBTFile)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Se inicializa CBTParser y fichero de escritura
  CCBTToolParser CBTParser;
  if (!CBTParser.Init(szCBTFile + ".cbt")) {
	return false;
  }

  // Se localizan todas las secciones que hacen referencia a los perfiles GFX
  // y se elimna seccion [FXDefs]
  
  // Crea lista de secciones
  StrSet ProfileSectionsSet;
  if (!CBTParser.CreateSetOfSections(ProfileSectionsSet)) {
	WriteMsg("El archivo no se halla bien construido.", CCrisolBuilder::MSG_ERROR);
	return false;
  }

  // Elimina seccion "[FXDefs]"
  StrSetIt It(ProfileSectionsSet.begin());
  for (; It != ProfileSectionsSet.end(); ++It) {
	if (0 == strcmpi(It->c_str(), "[FXDefs]")) {
	  ProfileSectionsSet.erase(It);
	  break;
	}
  }
  if (It == ProfileSectionsSet.end()) {
	WriteMsg("El archivo no se halla bien construido.", CCrisolBuilder::MSG_ERROR);
	return false;
  }
  
  // Chequea informacion de la seccion [FXDefs]
  // Se procede a leer la informacion de la luz ambiente.
  if (!CBTParser.SetSection("[FXDefs]")) {
	WriteMsg(CBTParser.GetFileName() + " / No se halló la sección [FXDefs]", CCrisolBuilder::MSG_ERROR);
	return false;
  }
  CBTParser.SetVarPrefix("AmbientDark.");
  word uwIt = 0;
  for (; uwIt < 24; ++uwIt) {
	std::ostringstream StrText;
	StrText << "Hour[" << uwIt << "].Dark";	
	if (!CheckValue(CBTParser.ReadNumeric(StrText.str()), 0, 255)) {
	  WriteMsg("[FXDefs] / " + StrText.str() + " / Valor no válido.", 
			   CCrisolBuilder::MSG_ERROR);
	}
  }

  // Lee la informacion de los alphas asociados a las sombras
  CBTParser.SetVarPrefix("Shadows.");
  uwIt = 0;
  for (; uwIt < 24; ++uwIt) {
	std::ostringstream StrText;
	StrText << "Hour[" << uwIt << "].";
	CBTParser.ReadAlpha(StrText.str());
  }

  // Modo de transparencia asociado a las criaturas 
  CBTParser.SetVarPrefix("Criatures.");
  CBTParser.ReadAlpha("TransparentMode.");

  // Modo de transparencia asociado a las paredes
  CBTParser.SetVarPrefix("Walls.");
  CBTParser.ReadAlpha("AutoTransparentMode.");

  // Se guardan datos de los diferentes perfiles
  It = ProfileSectionsSet.begin();
  for (; It != ProfileSectionsSet.end(); ++It) {
	// Se establece seccion y se guarda el offset actual
	if (!CBTParser.SetSection(*It)) {
	  WriteMsg(CBTParser.GetFileName() + " / No se halló la sección " + *It, CCrisolBuilder::MSG_ERROR);
	  return false;
	}
	CBTParser.SetVarPrefix("");  

	// Nombre plantilla de animacion asociada
	const std::string szAnimTemplate(CBTParser.ReadString("AnimTemplate"));	
	
	// Flags de dibujado
	CBTParser.SetVarPrefix("DrawMode.");
	CBTParser.ReadFlag("OverEntityTopFlag", false);
	CBTParser.ReadFlag("AtEntityTopFlag", false);
	CBTParser.ReadFlag("AtEntityBaseFlag", false);
	CBTParser.ReadFlag("UnderEntityBaseFlag", false);
	
	// Efectos asociados a los vertices (color y alpha)
	CBTParser.SetVarPrefix("FX.");
	uwIt = 0;
	for (; uwIt < 4; ++uwIt) {
	  std::ostringstream StrText;
	  StrText << "Vertex[" << uwIt << "].";	  
	  CBTParser.ReadRGBColor(StrText.str(), false);
	  CBTParser.ReadAlpha(StrText.str(), false);
	}

	// Modo de finalizacion
	CBTParser.SetVarPrefix("EndMode.");
	CBTParser.ReadNumeric("ByTime", false);
  }

  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea el archivo de presentaciones
// Parametros:
// - szCBTFile. Nombre del archivo CBT con los datos.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckPresentationsFile(const std::string& szCBTFile)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Se inicializa CBTParser y fichero de escritura
  CCBTToolParser CBTParser;
  if (!CBTParser.Init(szCBTFile + ".cbt")) {
	WriteMsg("No se halló el fichero " + szCBTFile + ".cbt", CCrisolBuilder::MSG_ERROR);
	return false;
  }

  // Crea lista de secciones
  StrSet ProfileSectionsSet;
  if (!CBTParser.CreateSetOfSections(ProfileSectionsSet)) {
	WriteMsg("El archivo no se halla bien construido.", CCrisolBuilder::MSG_ERROR);
	return false;
  }

  // Se recorren las secciones, verificandolas
  bool bCreditsPresentationOk = false;
  StrSetIt SectionIt(ProfileSectionsSet.begin());
  for (; SectionIt != ProfileSectionsSet.end(); ++SectionIt) {
	// Se establece seccion
	if (!CBTParser.SetSection(*SectionIt)) {
	  WriteMsg(CBTParser.GetFileName() + " / No se halló la sección " + *SectionIt, CCrisolBuilder::MSG_ERROR);
	  return false;
	}

	// ¿Aun no se ha verificado el perfil de presentacion?
	if (!bCreditsPresentationOk) {
	  // Se comprueba si coincide
	  if (0 == strcmpi(SectionIt->c_str(), m_SectionSets.szCreditsPresentationProfile.c_str())) {
		bCreditsPresentationOk = true;
	  }
	}

	// Se comprueban datos de inicio
	CBTParser.SetVarPrefix("GenConfig.");
	const word uwNumSlides = CBTParser.ReadNumeric("NumberOfSlides");
	if (!CheckValue(uwNumSlides, 1, 256)) {
	  WriteMsg(CBTParser.GetSectionName() + " / NumberOfSlides / Valor no válido.",
			   CCrisolBuilder::MSG_ERROR);
	}

	// Se lee el flag que habilita o no el uso de la tecla ESC
	CBTParser.ReadFlag("ESCKeyActiveFlag");

	// Tiempo maximo de visualizacion
	CBTParser.ReadNumeric("MaxSlideVisualizationTime", false);

	// Valores iniciales de fade
	CBTParser.SetVarPrefix("");
	CBTParser.ReadRGBColor("Fade.Start.", false);
	if (!CheckValue(CBTParser.ReadNumeric("Fade.In.Speed"), 1, 256)) {
	   WriteMsg(CBTParser.GetSectionName() + " / Fade.In.Speed / Valor no válido.",
			    CCrisolBuilder::MSG_ERROR);
	}

	bool bPrevWithFadeOut = false;
	word uwSlidesIt = 0;
	for (; uwSlidesIt < uwNumSlides; ++uwSlidesIt) {
	  // Prefijo
	  std::ostringstream StrText;
	  StrText << "Slide[" << uwSlidesIt << "].";
	  CBTParser.SetVarPrefix(StrText.str());

	  // Imagen de fondo
	  // El primer slide debera de poseer necesariamente imagen de fondo
	  CBTParser.ReadString("BackImage.FileName", false);
	  if (!CBTParser.WasLastParseOk()) {
		if (0 == uwSlidesIt) {
		  WriteMsg("La primera Slide debe de poseer una imagen de fondo.", 
				   CCrisolBuilder::MSG_ERROR);
		}
	  }

	  // Posibles animaciones asociadas
	  word uwIt = 0;
	  for (; uwIt < 0XFF; ++uwIt) {
		// Se establece vble e intenta leer plantilla de anim.
		std::ostringstream StrText;
		StrText << "Anim[" << uwIt << "].";
		const std::string szAnimTemplate(CBTParser.ReadString(StrText.str() + "AnimTemplate", false));	
		// ¿Se leyo informacion?
		if (CBTParser.WasLastParseOk()) {
		  // Se leen resto de datos		  
		  CBTParser.ReadPosition(StrText.str());
		  CBTParser.ReadAlpha(StrText.str(), false);
		} else {
		  break;
		}
	  }

	  // Informacion MIDI
	  // ¿Flag de deteccion?
	  if (!CBTParser.ReadFlag("MIDIMusic.StopFlag", false)) {
		// Se lee el posible nuevo fichero
		CBTParser.ReadString("MIDIMusic.FileName", false);	
		if (CBTParser.WasLastParseOk()) {
		  // Se lee el posible flag de reproduccion ciclica
		  CBTParser.ReadFlag("MIDIMusic.CyclicPlayFlag", false);	
		}
	  }

	  // Informacion WAV
	  // Reproduccion linear
	  CBTParser.ReadString("WAVSound.Linear.FileName", false);
	  
	  // Reproduccion ciclia
	  if (!CBTParser.ReadFlag("WAVSound.Cyclic.StopFlag", false)) {
		// No se leyo flag de parar wav ciclico, luego pueden leerse
		CBTParser.ReadString("WAVSound.Cyclic.FileName", false);
	  }

	  // Informacion asociada al texto
	  uwIt = 0;
	  for (; uwIt < 0XFF; ++uwIt) {
		// Vble
		std::ostringstream StrText;
		StrText << "Text[" << uwIt << "]";

		// Se intenta leer info del texto actual (posicion)
		CBTParser.ReadPosition(StrText.str(), false);
		if (CBTParser.WasLastParseOk()) {
		  // Se leyo, procede a obtener resto de info
		  CBTParser.ReadRGBColor(StrText.str());
		  if(!CheckValue(CBTParser.ReadNumeric(StrText.str() + "AreaWidth"), 64, 1024)) {
			WriteMsg(CBTParser.GetSectionName() + " / " + CBTParser.GetPrefixName() + StrText.str() + " / Valor no válido",
				     CCrisolBuilder::MSG_ERROR);
		  }
		  CBTParser.ReadFlag(StrText.str() + "Justify.CenterFlag", false);
		  CBTParser.ReadFlag(StrText.str() + "Justify.RightFlag", false);
		  CBTParser.ReadFlag(StrText.str() + "Justify.LeftFlag", false);
		  
		  // Se lee el texto, que podra estar tal cual o en archivo
		  CBTParser.ReadString(StrText.str() + "Text", false);
		  if (!CBTParser.WasLastParseOk()) {
			CBTParser.ReadString(StrText.str() + "TextFileName");
		  }
		} else {
		  break;
		}
	  }

	  // Informacion relativa a los fades
	  // La velocidad del Fade In sera obligatoria si el slide anterior tenia
	  // un valor asociado de Fade Out
	  word uwValue = CBTParser.ReadNumeric("Fade.In.Speed", bPrevWithFadeOut);
	  if (CBTParser.WasLastParseOk()) {
		if (!CheckValue(uwValue, 1, 256)) {
		  WriteMsg(CBTParser.GetSectionName() + " / " + CBTParser.GetPrefixName() + "Fade.In.Speed / Valor no válido",
				   CCrisolBuilder::MSG_ERROR);
		}
	  }
	  CBTParser.ReadRGBColor("Fade.Out.", false);
	  if (CBTParser.WasLastParseOk()) {
		bPrevWithFadeOut = true;
	  }
	}
  }

  // ¿NO se hallo el perfil asociado a la presentacion para creditos?
  if (!bCreditsPresentationOk) {
	WriteMsg("No se halló el perfil " + m_SectionSets.szCreditsPresentationProfile + " asociado a la presentación de los créditos",
			 CCrisolBuilder::MSG_ERROR);
  }
  
  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea el archivo destinado a especificar los scripts globales 
//   relacionados con el universo de juego y con el juego.
// Parametros:
// - szCBTFile. Nombre del archivo CBT con los datos.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckGlobalScriptEventsFile(const std::string& szCBTFile)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(!szCBTFile.empty());

  // Se inicializa CBTParser y fichero de escritura
  CCBTToolParser CBTParser;
  if (!CBTParser.Init(szCBTFile + ".cbt")) {
	WriteMsg("No se halló el fichero " + szCBTFile + ".cbt", CCrisolBuilder::MSG_ERROR);
	return false;
  }
  CBTParser.SetVarPrefix("");

  // Seccion de eventos relacionados con el universo de juego
  if (!CBTParser.SetSection("[WorldEvents]")) {
	WriteMsg(CBTParser.GetFileName() + " / No se halló la sección [WorldEvents]", CCrisolBuilder::MSG_ERROR);
	return false;
  }

  // Se comprueban los eventos del universo
  const std::string szWorldEvents[] = {
	"OnStartGame", "OnNewHour", "OnEnterInArea", "OnWorldIdle"
  };
  word uwIt = 0;
  for (; uwIt < RulesDefs::NUM_WORLD_EVENTS; ++uwIt) {
	// Procesa
	std::ostringstream Str;
	Str << "ScriptEvent[" << szWorldEvents[uwIt] << "]";
	CBTParser.ReadString(Str.str(), false);	
  }

  // Seccion relacionada con el juego como aplicacion
  if (!CBTParser.SetSection("[GameEvents]")) {
	WriteMsg(CBTParser.GetFileName() + " / No se halló la sección [GameEvents]", CCrisolBuilder::MSG_ERROR);
	return false;
  }

  // Se comprueban los eventos del juego
  const std::string szGameEvents[] = {
	"OnClickHourPanel", "OnFleeCombat", "OnKeyPressed", 
	"OnStartCombatMode", "OnEndCombatMode"
  };
  uwIt = 0;
  for (; uwIt < RulesDefs::NUM_GAME_EVENTS; ++uwIt) {
	// Procesa
	std::ostringstream Str;
	Str << "ScriptEvent[" << szGameEvents[uwIt] << "]";
	CBTParser.ReadString(Str.str(), false);	
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea el archivo de perfiles de jugador
// Parametros:
// - szCBTFile. Nombre del archivo CBT con los datos.
// - uwNumAreas. Numero de areas disponibles
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
// - El jugador tendra asociado todos los eventos validos para una
//   criatura.
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::CheckPlayerProfilesFile(const std::string& szCBTFile,
										const word uwNumAreas)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(uwNumAreas);
  
  // Se inicializa CBTParser y fichero de escritura
  CCBTToolParser CBTParser;
  if (!CBTParser.Init(szCBTFile + ".cbt")) {
	WriteMsg("No se halló el fichero " + szCBTFile + ".cbt", CCrisolBuilder::MSG_ERROR);
	return false;
  }

  // Seccion de perfiles
  if (!CBTParser.SetSection("[Profiles]")) {
	WriteMsg(CBTParser.GetFileName() + " / No se halló la sección [Profiles]", CCrisolBuilder::MSG_ERROR);
	return false;
  }

  // Obtiene el numero maximo de tipos, generos, clases, atributos y hab. de criaturas
  const word uwMaxTypes = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_TYPE);
  const word uwMaxGenres = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_GENRE);	  
  const word uwMaxClasses = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_CLASS);
  const word uwMaxExtAttribs = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB);
  const word uwMaxHabilities = m_RulesDataBase.GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);

  // Navega entre todos los tipos de criaturas
  word uwNumProfilesFound = 0;
  word uwTypeIt = 0;
  for (; uwTypeIt < uwMaxTypes; ++uwTypeIt) {
	// Crea el prefijo de tipo y lo establece
	std::ostringstream StrTypePrefix;
	StrTypePrefix << "Type[" 
				  << m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_TYPE,
													uwTypeIt)
				  << "].";
	CBTParser.SetVarPrefix(StrTypePrefix.str());
	
	// Navega entre las clases de criaturas
	word uwClassIt = 0;
	for (; uwClassIt < uwMaxClasses; ++uwClassIt) {
	  // Crea vble
	  std::ostringstream StrClassPrefix;
	  StrClassPrefix << "Class[" 
					 <<	m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_CLASS,
													   uwClassIt)
					 << "].";
	
	  // Valores base y coste de los atributos
	  // Salud
	  // Nota: El valor base de la salud debe de existir para tener en cuenta
	  // el resto de datos
	  const word uwHealthBaseValue = CBTParser.ReadNumeric(StrClassPrefix.str() + "Health.BaseValue", false);
	  if (CBTParser.WasLastParseOk()) {
		++uwNumProfilesFound;
		if (!CheckValue(uwHealthBaseValue, 1, 0xEF)) {
		  WriteMsg("[Profiles] / " + CBTParser.GetPrefixName() + StrClassPrefix.str() + "Health.BaseValue / Valor no válido.",
				   CCrisolBuilder::MSG_ERROR);
		}
		CBTParser.ReadNumeric(StrClassPrefix.str() + "Health.ImproveCost", false);

		// Puntos de combate
		CBTParser.ReadNumeric(StrClassPrefix.str() + "CombatPoints.BaseValue");
		if (!CheckValue(uwHealthBaseValue, 1, 0xEF)) {
		  WriteMsg("[Profiles] / " + CBTParser.GetPrefixName() + StrClassPrefix.str() + "CombatPoints.BaseValue / Valor no válido.",
				   CCrisolBuilder::MSG_ERROR);
		}
		CBTParser.ReadNumeric(StrClassPrefix.str() + "CombatPoints.ImproveCost", false);

		// Atributos extendidos
		byte ubIt = 0;
		for (; ubIt < uwMaxExtAttribs; ++ubIt) {
		  // Se forma variable
		  std::ostringstream StrExtendedAttrib;
		  StrExtendedAttrib << "ExtendedAttribute["
							<< m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_EXTATTRIB,
															  ubIt)
							<< "].";

		  // Se lee
		  CBTParser.ReadNumeric(StrClassPrefix.str() + StrExtendedAttrib.str() + "BaseValue", false);
		  CBTParser.ReadNumeric(StrClassPrefix.str() + StrExtendedAttrib.str() + "ImproveCost", false);		 
		}

		// Habilidades
		ubIt = 0;
		for (; ubIt < uwMaxHabilities; ++ubIt) {
		  // Se forma variable
		  std::ostringstream StrHability;
		  StrHability << "Hability["
					  << m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_HABILITY,
														ubIt)
					  << "].";

		  // Se lee el coste de establecimiento
		  CBTParser.ReadNumeric(StrClassPrefix.str() + StrHability.str() + "SetCost", false);
		}

		// Datos acerca de los puntos de distribucion
		CBTParser.ReadNumeric(StrClassPrefix.str() + "Attributes.Value", false);
		CBTParser.ReadFlag(StrClassPrefix.str() + "Attributes.MustUseFlag", false);
		CBTParser.ReadNumeric(StrClassPrefix.str() + "Habilities.Value", false);
		CBTParser.ReadFlag(StrClassPrefix.str() + "Habilities.MustUseFlag", false);		
	  }
	}
  }  

  // ¿NO se encontro ningun perfil?
  if (!uwNumProfilesFound) {
	WriteMsg("[Profiles] / No se halló ningún perfil.", CCrisolBuilder::MSG_ERROR);
	return false;
  }

  // Retratos del jugador posibles
  if (!CBTParser.SetSection("[Portraits]")) {
	WriteMsg(CBTParser.GetFileName() + " / No se halló la sección [Portraits]", CCrisolBuilder::MSG_ERROR);
	return false;
  }

  // Al menos debera de existir uno de cada  
  word uwGenreIt = 0;
  for (; uwGenreIt < uwMaxGenres; ++uwGenreIt) {	  
    // Se crea prefijo y se establece
    std::ostringstream StrText;
    StrText << "Genre["
		    << m_RulesDataBase.GetEntityIDStr(RulesDefs::ID_CRIATURE_GENRE,
											  uwGenreIt)
			<< "].";
	CBTParser.SetVarPrefix(StrText.str());  

	// Se leen las posibles plantillas de animacion asociadas al retrato asi
	// como la representacion del personaje y su posible sombra
	word uwAnimIt = 0;
	for (; uwAnimIt < 0xFF; ++uwAnimIt) {
	  // Se establece vble 
	  std::ostringstream StrAnimText;
	  StrAnimText << "Portrait[" << uwAnimIt << "].";

	  // Se lee posible plantilla de anim. del retrato
	  std::string szAnimTemplate(CBTParser.ReadString(StrAnimText.str() + "AnimTemplate", false));
	  if (CBTParser.WasLastParseOk()) {
		// Inserta en conjunto		

		// Lee representacion del jugador y la inserta
		szAnimTemplate = CBTParser.ReadString(StrAnimText.str() + "Player.AnimTemplate");
		
		// Se lee la posible sombra asociada		
		CBTParser.ReadStringID(StrAnimText.str() + "Player.ShadowImage", false);
	  } else {
		break;
	  }
	}

	// Al menos se ha tenido que leer un retrato
	if (!uwAnimIt) {
	  WriteMsg("[Portraits] / Al menos debe de existir un retrato por género.", 
			   CCrisolBuilder::MSG_ERROR);
	  return false;
	}
  }

  // Datos comunes
  if (!CBTParser.SetSection("[Common]")) {
	WriteMsg(CBTParser.GetFileName() + " / No se halló la sección [Common]", CCrisolBuilder::MSG_ERROR);
	return false;
  }

  // Se lee localidad de inicio
  CBTParser.SetVarPrefix("StartLocation.");
  const word uwIDAreaInit = CBTParser.ReadNumeric("IDArea");
  if (!CheckValue(uwIDAreaInit, 0, uwNumAreas)) {
	WriteMsg("[Common] / StartLocation.IDArea / Valor no válido.", 
			 CCrisolBuilder::MSG_ERROR);
	return false;
  }
  
  // Posicion inicial
  const sPosition InitPos(CBTParser.ReadPosition("Cell."));
  
  // Se abrira el archivo CBT del area para comprobar si esa posicion es valida
  CCBTToolParser CBTAreaParser;
  std::ostringstream StrAreaFileName;
  StrAreaFileName << "Area" << uwIDAreaInit << ".cbt";
  if (!CBTAreaParser.Init(StrAreaFileName.str())) {
	WriteMsg("No se pudo abrir archivo " + StrAreaFileName.str() + " para verificar posición inicial.",
			 CCrisolBuilder::MSG_ERROR);
	return false;
  }
  std::ostringstream StrCellSection;
  StrCellSection << "[Cell<" << InitPos.swXPos << "," << InitPos.swYPos << ">]";
  if (!CBTAreaParser.SetSection(StrCellSection.str())) {
	WriteMsg("[Common] / StartLocation.InitPos / La posición inicial "  + 
			 StrCellSection.str() + 
			 " no existe en el área " + 
			 StrAreaFileName.str(),
			 CCrisolBuilder::MSG_ERROR);
	return false;
  }  

  // Se leen los posibles eventos script
  // Nota: Se desestima OnTalk y OnManipulate  
  CBTParser.ReadStringID("ScriptEvent[OnObserve]", false);
  CBTParser.ReadStringID("ScriptEvent[OnHit]", false);
  CBTParser.ReadStringID("ScriptEvent[OnEntityIdle]", false);
  CBTParser.ReadStringID("ScriptEvent[OnOnDeath]", false);
  CBTParser.ReadStringID("ScriptEvent[OnResurrect]", false);
  CBTParser.ReadStringID("ScriptEvent[OnActivatedSymptom]", false);
  CBTParser.ReadStringID("ScriptEvent[OnDeactivatedSymptom]", false);
  CBTParser.ReadStringID("ScriptEvent[OnInsertInEquipmentSlot]", false);
  CBTParser.ReadStringID("ScriptEvent[OnRemoveFromEquipmentSlot]", false);
  CBTParser.ReadStringID("ScriptEvent[OnCriatureInRange]", false);
  CBTParser.ReadStringID("ScriptEvent[OnCriatureOutOfRange]", false);
  CBTParser.ReadStringID("ScriptEvent[OnEntityCreated]", false);
  CBTParser.ReadStringID("ScriptEvent[OnUseHability]", false);
  CBTParser.ReadStringID("ScriptEvent[OnStartCombatTurn]", false);

  // Retorna
  return !(GetNumErrors() > 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el proceso de construccion guardando los campos de 
//   tipo de fichero y version e inicilizando el CBTParser de lectura y el fichero
//   de escritura
// Parametros:
// - CBTParser. CBTParser para la lectura del archivo cbt.
// - CBBFile. Fichero binario donde escribir la informacion.
// - szInputFileName. Nombre del fichero de entrada.
// - szOutputFileName. Nombre del fichero de salida.
// Devuelve:
// - Si no existio ningun problema y se encontro al menos un archivo true.
//   En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCrisolBuilder::InitBuild(CCBTToolParser& CBTParser,
						  std::ofstream& CBBFile,
						  const std::string& szInputFileName,
						  const std::string& szOutputFileName)
{
  // SOLO si nombres valido
  ASSERT(!szInputFileName.empty());
  ASSERT(!szOutputFileName.empty());

  // Se inicializa CBTParser de lectura
  if (!CBTParser.Init(szInputFileName)) {
	WriteMsg("No se pudo abrir el archivo \"" + szInputFileName + "\"", 
			 CCrisolBuilder::MSG_ERROR);
	return false;
  }

  // Se prepara el archivo binario donde enviar la informacion
  CBBFile.open(szOutputFileName.c_str(), std::ios::binary);
  if (CBBFile.fail()) {
	// Problemas
	WriteMsg("No se pudo abrir el archivo \"" + szOutputFileName + "\"",
			 CCrisolBuilder::MSG_ERROR);
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Escribe los indices de secciones de un fichero
// Parametros:
// - CBBFile. Fichero de salida.
// - SetOfSections. Conjunto con secciones
// - ListOfOffsets. Lista de offsets.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCrisolBuilder::WriteIndexOfSections(std::ofstream& CBBFile,
									 StrSet& SetOfSections,
									 StreamPosList& ListOfOffsets)
{
  // Se guarda la lista de indices
  word uwValue = SetOfSections.size();
  CBBFile.write((sbyte *)(&uwValue), sizeof(word));
  StreamPosListIt OffsetIt(ListOfOffsets.begin());
  StrSetIt SectionIt(SetOfSections.begin());  
  while (SectionIt != SetOfSections.end() &&
		 OffsetIt != ListOfOffsets.end()) {
	// Se escribe el par plantilla, offset	
	CBBFile.write((sbyte *)(&*OffsetIt), sizeof(std::streampos));
	WriteString(CBBFile, *SectionIt);

	// Sig
	++SectionIt;
	++OffsetIt;
  }
  ASSERT((ListOfOffsets.end() == OffsetIt) != 0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Escribe el mensaje de advertencia / error / normal. En el caso de los
//   mensajes de error y advertencia, se incrementara un contador asociado
//   a los mismos.
// Parametros:
// - szMsg. Texto del mensaje.
// - Type. Tipo del mensaje (Error / Advertencia / Normal)
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCrisolBuilder::WriteMsg(const std::string& szMsg,
						 const eMsgType& Type)
{
  // Se forma el prefijo segun sea el tipo del mensaje
  std::string szPrefix;
  switch (Type) {
	case CCrisolBuilder::MSG_WARNING: {
	  szPrefix = "Advertencia> ";
	  ++m_MsgInfo.uwNumWarnings;
	} break;

	case CCrisolBuilder::MSG_ERROR: {
	  szPrefix = "Error> ";
	  ++m_MsgInfo.uwNumErrors;
	} break;

	case CCrisolBuilder::MSG_NORMAL: {
	  szPrefix = "> ";
	} break;
  }; // ~ switch

  // ¿Hay que enviar a disco?
  if (m_ParamsInfo.bWriteMsgInFile) {
	// Si, se escribe en disco
	ASSERT(m_MsgInfo.MsgFile.is_open());
	m_MsgInfo.MsgFile << szPrefix << szMsg << std::endl;
  } else {
	// No, se escribe por salida estandar
	std::cout << szPrefix << szMsg << std::endl; 
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Guarda el el fichero CBBFile el string szString. La forma de guardar siempre
//   supondra incluir primero el tamaño del mismo y luego el string.
// Parametros:
// - CBBFile. Fichero.
// - szString. Cadena de caracteres.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCrisolBuilder::WriteString(std::ofstream& CBBFile,
							const std::string& szString)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se guarda
  ASSERT(CBBFile.is_open());
  const word uwStrSize = szString.size();
  CBBFile.write((char*)(&uwStrSize), sizeof(word));
  CBBFile.write(szString.c_str(), uwStrSize * sizeof(char));
}
