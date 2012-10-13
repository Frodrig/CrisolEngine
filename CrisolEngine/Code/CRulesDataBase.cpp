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
// CRulesDataBase.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CRulesDataBase.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CRulesDataBase.h"

#ifdef _CRISOLENGINE
 #include "SYSEngine.h"
 #include "iCLogger.h"
 #include "CCBTEngineParser.h"
#else
 #include "CCBTToolParser.h"
#endif
#include <iostream>
#include <sstream>
#include <algorithm>

struct CRulesDataBase::sIDInfo {
  // Info basica asociada a cualquier identificador
  std::string szIDStr; // Valor del identificador en string
  // Constructor
  sIDInfo(const std::string aszIDStr): szIDStr(aszIDStr) { }
  // Operador de comparacion
  bool operator==(const sIDInfo& aIDInfo) {
    // NOTA: La comparacion tendra en cuenta mayusculas o minusculas pero
    // desde el exterior siempre se habra insertado en minusculas		
    return (0 == stricmp(szIDStr.c_str(), aIDInfo.szIDStr.c_str()));
  }
};

struct CRulesDataBase::sIDSceneObjInfo: public sIDInfo { 
  // Nodo asociado a identificador de tipo de un objeto de escenario  
  IDMap ClassesIDs;      // Mapeado de identificadores de clases
  IDMap LocalAttribsIDs; // Mapeado de identificadores de atributos locales
  bool  bContainerFlag;  // ¿Contenedor?
  // Constructores
  sIDSceneObjInfo(const std::string& szIDStr): sIDInfo(szIDStr) { }
  // Operador de comparacion
  bool operator==(const sIDSceneObjInfo& IDInfo) {
    return sIDInfo::operator==(IDInfo.szIDStr);
  }
};

struct CRulesDataBase::sIDItemInfo: public sIDInfo {
  // Nodo asociado a identificador de tipo item
  byte  ValidEquipSlotsMask; // Mascara de slots de equipamiento validos
  IDMap ClassesIDs;          // Mapeado de identificadores de clases
  IDMap LocalAttribsIDs;     // Mapeado de identificadores de atributos locales
  // Constructores
  sIDItemInfo(const std::string& szIDStr): sIDInfo(szIDStr),
										   ValidEquipSlotsMask(0) { }
  // Operador de comparacion
  bool operator==(const sIDItemInfo& IDInfo) {
    return sIDInfo::operator==(IDInfo.szIDStr);
  }
};

struct CRulesDataBase::sIDWallInfo: public sIDInfo {
  // Nodo asociado a identificador de tipo pared
  IDMap LocalAttribsIDs; // Mapeado de identificadores de atributos locales
  // Constructores
  sIDWallInfo(const std::string& szIDStr): sIDInfo(szIDStr) { }
  // Operador de comparacion
  bool operator==(const sIDWallInfo& IDInfo) {
    return sIDInfo::operator==(IDInfo.szIDStr);
  }
};

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la clase llevando a memoria toda la informacion posible.
// Parametros:
// - szRulesFileName. Nombre del archivo de reglas.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - El flag bUsedByTool servira para crear el parser adecuado
///////////////////////////////////////////////////////////////////////////////
bool 
CRulesDataBase::Init(const std::string& szRulesFileName)
{
  // SOLO si parametros validos
  ASSERT(!szRulesFileName.empty());

  // ¿Se intenta reinicializar?
  if (IsInitOk()) {
	End();
  }

  // Se prepara el parser de obtencion de informacion
  #ifdef _CRISOLENGINE
	CCBTParser* pParser = new CCBTEngineParser;
  #else
    CCBTParser* pParser = new CCBTToolParser;
  #endif
  ASSERT(pParser);
  if (pParser->Init(szRulesFileName)) {
	#ifdef _CRISOLENGINE
	  #ifdef ENGINE_TRACE
		  SYSEngine::GetLogger()->Write("CRulesDataBase> Cargando reglas\n");
	  #endif
	#endif
	// Se cargan reglas
	if (TLoadGameControllerRules(*pParser) &&
		TLoadCriatureRules(*pParser) &&  	
		TLoadCombatRules(*pParser) &&
		TLoadFloorRules(*pParser) &&
		TLoadSceneObjRules(*pParser) &&
		TLoadItemRules(*pParser) &&
		TLoadWallRules(*pParser) &&
		TLoadMinDistancesRules(*pParser)) {
	  
	  // Se establecen vbles de miembro
	  m_szRulesFileName = szRulesFileName;

	  // Todo correcto	
	  ASSERT(pParser);
	  delete pParser;
	  pParser = NULL;

	  m_bIsInitOk = true;
   	  #ifdef _CRISOLENGINE
		#ifdef ENGINE_TRACE
			SYSEngine::GetLogger()->Write("              | Ok.\n");
		#endif
	  #endif
	  return true;
	}		
  }

  // Hubo problemas
  #ifdef _CRISOLENGINE
	#ifdef ENGINE_TRACE
		SYSEngine::GetLogger()->Write("              | Error.\n");
	#endif
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
CRulesDataBase::End(void)
{
  // Finaliza
  if (IsInitOk()) {	
	// Se descargan reglas
	ReleaseRules();

	// Se finaliza 
	m_bIsInitOk = false;
  }
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga desde un parser situado en seccion y prefijo identificadores que
//   seran expresiones de la forma .ID = identificador. Una vez leidos los
//   mapeara en el map recibido.
// Parametros:
// - Parser. Parser desde donde leer la informacion. El parser debera de
//   tener asociado una seccion y prefijo.
// - uwMaxValue. Valor maximo de localizacion para identificadores.
// - Map. Map donde mapear los identificadores que se lean.
// Devuelve:
// Notas:
// - Desde el exterior, el parser ha de haber sido configurado en seccion y
//   prefijo. En el metodo se añadira siempre como vble: "[x].ID" donde x sera
//   el orden asociado al identificador que toque leer.
// - La lectura permitira el que no exista el identificador asociado al
//   iterador actual, en cuyo caso se retornara de inmediato.
///////////////////////////////////////////////////////////////////////////////
void 
CRulesDataBase::TLoadIDs(CCBTParser& Parser,
						 const word uwMaxValue,
						 IDMap& Map)
{
  // Vbles
  std::string szTmp; // String temporal
 
  // Procede a leer y a mapear  
  RulesDefs::BaseType It = 0;
  for (; It < uwMaxValue; ++It) {
	// Se lee identificador	
	std::ostringstream StrTmp;
	StrTmp << "[" << word(It) << "].ID";
	std::string szID(Parser.ReadStringID(StrTmp.str(), false));  
	std::transform(szID.begin(), szID.end(), szID.begin(), tolower);
	
	// ¿NO se leyo informacion?
	if (!Parser.WasLastParseOk()) { 
	  break; 
	}

	// Se mapea, comprobando que no exista ese mismo identificador
	IDMapIt ItMap(Map.begin());
	for (; ItMap != Map.end(); ++ItMap) {
	  if (0 == ItMap->second->szIDStr.compare(szID)) {
		FatalError("El identificador " + szID + " ya se halla definido");
		break;
	  }
	}
	sIDInfo* const pIDInfo = new sIDInfo(szID);
	ASSERT(pIDInfo);	
	Map.insert(IDMapValType(It, pIDInfo));		
  }
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga datos asociados al controlador de juego.
// Parametros:
// - Parser. Parser a utilizar
// Devuelve:
// - Si todo correcto true, en caso contrario false.
// Notas:
// - Los valores deberán de estar obligatoriamente entre 15 y 60.
///////////////////////////////////////////////////////////////////////////////
bool
CRulesDataBase::TLoadGameControllerRules(CCBTParser& Parser)
{
  // Establece seccion
  if (!Parser.SetSection("[GameController]")) {
	FatalError("No se pudo estblecer la sección [GameController] en el archivo de reglas.");
  }  

  // Minutos por hora (pasados a milisegundos)
  Parser.SetVarPrefix("Time.");
  m_GameControllerRules.SecondsPerMinute = Parser.ReadNumeric("SecondsPerMinute");
  if (m_GameControllerRules.SecondsPerMinute < 15 ||
	  m_GameControllerRules.SecondsPerMinute > 60) {
	FatalError("El valor de Time.SecondsPerMinute en el archivo de reglas es incorrecto.");  
	return false;
  }

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga los datos asociados a los Floor.
// Parametros:
// - Parser. Parser a utilizar
// Devuelve:
// - Si todo correcto true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CRulesDataBase::TLoadFloorRules(CCBTParser& Parser)
{
  // Se establece seccion y prefijo
  if (!Parser.SetSection("[FloorTypes]")) {
	FatalError("No se pudo estblecer la sección [FloorTypes] en el archivo de reglas.");
  }
  Parser.SetVarPrefix("Type");
  
  // Se leen identificadores
  TLoadIDs(Parser,
		   RulesDefs::MAX_FLOOR_TYPES, 
		   m_FloorRules.TypesIDs);

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga las reglas asociadas a los objetos de escenario.
// Parametros:
// - Parser. Parser a utilizar
// Devuelve:
// - Si todo correcto true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CRulesDataBase::TLoadSceneObjRules(CCBTParser& Parser)
{
  // Establece seccion
  if (!Parser.SetSection("[SceneObjectTypes]")) {
	FatalError("No se pudo estblecer la sección [SceneObjTypes] en el archivo de reglas.");
  }

  // Localiza identificador de tipo  
  RulesDefs::SceneObjType Type = 0;
  for (; Type < RulesDefs::MAX_SCENEOBJ_TYPES; ++Type) {
	// Se establece prefijo
	std::ostringstream StrTypePrefix;
	StrTypePrefix << "Type[" << word(Type) << "].";
	Parser.SetVarPrefix(StrTypePrefix.str());				
	
	// Se obtiene identificador de tipo
	std::string szTmp(Parser.ReadStringID("ID", false));		
	
	// ¿NO se leyo identificador?
	if (!Parser.WasLastParseOk()) { 
	  break; 
	}	
	
	// Se crea nodo principal y se mapea
	std::transform(szTmp.begin(), szTmp.end(), szTmp.begin(), tolower);	  	  
	sIDSceneObjInfo* const pTypeNode = new sIDSceneObjInfo(szTmp);
	ASSERT(pTypeNode);	
	m_SceneObjRules.TypesIDs.insert(IDSceneObjMapValType(Type, pTypeNode));			
	
	// Localiza el flag contenedor (por defecto a false)
	pTypeNode->bContainerFlag = Parser.ReadFlag("ContainerFlag", false);
	
	// Se leen identificadores de clases asociadas
	Parser.SetVarPrefix(StrTypePrefix.str() + "Class");
	TLoadIDs(Parser,
			 RulesDefs::MAX_SCENEOBJ_CLASSES, 
			 pTypeNode->ClassesIDs);

	// Se leen identificadores de atributos locales asociados
	Parser.SetVarPrefix(StrTypePrefix.str() + "LocalAttribute");
	TLoadIDs(Parser,
			 RulesDefs::MAX_SCENEOBJ_LOCALATTRIB,
			 pTypeNode->LocalAttribsIDs);
  }	

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la carga de los datos asociados a las reglas de los items y 
//   almacena toda la informacion.
// Parametros:
// - Parser. Parser a utilizar
// Devuelve:
// - Si todo correcto true, en caso contrario false.
// Notas:
// - Se permitira no definir atributos globales.
// - Los tipos de items podran no existir, en cuyo caso no apareceran items
//   en el juego.
// - A la hora de construir la mascara de slots de equipamiento valido para
//   un item, se considerara que cuando no exista el flag indicativo para
//   un determinado slot, el item NO se podra equipar.
///////////////////////////////////////////////////////////////////////////////
bool
CRulesDataBase::TLoadItemRules(CCBTParser& Parser)
{
  // Se obtienen identificadores de atributos globales
  if (!Parser.SetSection("[ItemGlobalAttributes]")) {
	FatalError("No se pudo estblecer la sección [ItemGlobalAttributes] en el archivo de reglas.");
  }
  Parser.SetVarPrefix("GlobalAttribute");
  TLoadIDs(Parser,
		   RulesDefs::MAX_ITEM_GLOBALATTRIB,
		   m_ItemRules.GlobalAttribsIDs);

  // Se obtienen identificadores de tipo
  if (!Parser.SetSection("[ItemTypes]")) {
	FatalError("No se pudo estblecer la sección [ItemTypes] en el archivo de reglas.");
  }
  
  // Localiza identificadores de tipo  
  RulesDefs::ItemType Type = 0;
  for (; Type < RulesDefs::MAX_ITEM_TYPES; ++Type) {
	// Se establece prefijo
	std::ostringstream StrTypePrefix;
	StrTypePrefix << "Type[" << word(Type) << "].";
	Parser.SetVarPrefix(StrTypePrefix.str());		
	
	// Se obtiene identificador de tipo
	std::string szTmp(Parser.ReadStringID("ID", false));	
	
	// ¿NO se leyo identificador?
	if (!Parser.WasLastParseOk()) { 
	  break; 
	}

	// Se crea nodo principal y se mapea
	std::transform(szTmp.begin(), szTmp.end(), szTmp.begin(), tolower);
	sIDItemInfo* const pTypeNode = new sIDItemInfo(szTmp);
	ASSERT(pTypeNode);
	m_ItemRules.TypesIDs.insert(IDItemMapValType(Type, pTypeNode));	

	// Se procede a comprobar los slots de equipamiento para los que
	// se puede equipar el item del tipo Type. Es vital que la informacion
	// sobre las reglas de equipamiento esten cargadas
	byte ubIt = 0;
	for (; ubIt < m_CriatureRules.EquipmentSlotsIDs.size(); ++ubIt) {
	  // Se construye vble
	  const IDMapIt It(m_CriatureRules.EquipmentSlotsIDs.find(ubIt));
	  ASSERT((It != m_CriatureRules.EquipmentSlotsIDs.end()) != 0);
	  std::ostringstream StrTmp;
	  StrTmp << "Slot[" << It->second->szIDStr << "].CanEquipFlag";

	  // Se lee el flag y se establece en la mascara si procede	  
	  if (Parser.ReadFlag(StrTmp.str(), false)) {
		pTypeNode->ValidEquipSlotsMask |= (0x01 << ubIt);
	  }
	}

	// Si el item es equipable en uno o los dos slots principales, significara 
	// que el item sera un arma y por lo tanto podra tener asociado un coste
	// de uso en calidad de arma
	if (pTypeNode->ValidEquipSlotsMask & 0x01 ||
	    pTypeNode->ValidEquipSlotsMask & 0x02) {
	  // Se lee el coste
	  const sword swInCombatUseCost = Parser.ReadNumeric("InCombatUseCost", false);
	  
	  // Si el coste es distinto de cero se registra en caso contrario se
	  // sobreentendera que valdra 0
	  if (swInCombatUseCost > 0) {
		m_ItemRules.WeaponsItemsMap.insert(WeaponItemMapValType(Type, swInCombatUseCost));
	  }	  
	}
		
	// Se procede a mapear todas las clases asocidas
	Parser.SetVarPrefix(StrTypePrefix.str() + "Class");
	TLoadIDs(Parser,
		     RulesDefs::MAX_ITEM_CLASSES,
		     pTypeNode->ClassesIDs);

	// Se procede a mapear todos los atributos locales asociados
	Parser.SetVarPrefix(StrTypePrefix.str() + "LocalAttribute");
	TLoadIDs(Parser,
		     RulesDefs::MAX_ITEM_LOCALATTRIB,
		     pTypeNode->LocalAttribsIDs);	  	
  }	

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga datos de la reglas asociadas a las paredes
// Parametros:
// - Parser. Parser a utilizar
// Devuelve:
// - Si todo correcto true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CRulesDataBase::TLoadWallRules(CCBTParser& Parser)
{
  // Se obtiene parser y situa seccion
  if (!Parser.SetSection("[WallTypes]")) {
	FatalError("No se pudo estblecer la sección [WallTypes] en el archivo de reglas.");
  }
  
  // Localiza identificadores de tipo  
  RulesDefs::ItemType Type = 0;
  for (; Type < RulesDefs::MAX_WALL_TYPES; ++Type) {
	// Se establece prefijo
	std::ostringstream StrTypePrefix;
	StrTypePrefix << "Type[" << word(Type) << "].";
	Parser.SetVarPrefix(StrTypePrefix.str());		
	
	// Se obtiene identificador de tipo
	std::string szTmp(Parser.ReadStringID("ID", false));	
	
	// ¿NO se leyo identificador?
	if (!Parser.WasLastParseOk()) { 
	  break; 
	}

	// Se crea nodo principal y se mapea
	std::transform(szTmp.begin(), szTmp.end(), szTmp.begin(), tolower);
	sIDWallInfo* const pTypeNode = new sIDWallInfo(szTmp);
	ASSERT(pTypeNode);	
	m_WallRules.TypesIDs.insert(IDWallMapValType(Type, pTypeNode));	
		
	// Se procede a mapear todos los atributos locales asociados
	Parser.SetVarPrefix(StrTypePrefix.str() + "LocalAttribute");
	TLoadIDs(Parser,
			 RulesDefs::MAX_WALL_LOCALATTRIB,
		     pTypeNode->LocalAttribsIDs);
  }	

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga datos de la reglas asociadas a las criaturas.
// Parametros:
// - Parser. Parser a utilizar
// Devuelve:
// - Si todo correcto true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CRulesDataBase::TLoadCriatureRules(CCBTParser& Parser)
{
  // Se obtienen los tipos de criaturas
  if (!Parser.SetSection("[CriatureTypes]")) {
	FatalError("No se pudo estblecer la sección [CriatureTypes] en el archivo de reglas.");
  }
  Parser.SetVarPrefix("Type");
  TLoadIDs(Parser,
		   RulesDefs::MAX_CRIATURE_TYPES, 
		   m_CriatureRules.TypesIDs);

  // Se obtienen las clases de criaturas
  if (!Parser.SetSection("[CriatureClasses]")) {
	FatalError("No se pudo estblecer la sección [CriatureClasses] en el archivo de reglas.");
  }
  Parser.SetVarPrefix("Class");
  TLoadIDs(Parser,
		   RulesDefs::MAX_CRIATURE_CLASSES, 
		   m_CriatureRules.ClassesIDs);

  // Se obtienen generos de las criaturas
  if (!Parser.SetSection("[CriatureGenres]")) {
		FatalError("No se pudo estblecer la sección [CriatureGenres] en el archivo de reglas.");
  }
  Parser.SetVarPrefix("Genre");
  TLoadIDs(Parser,
		   RulesDefs::MAX_CRIATURE_GENRES, 
		   m_CriatureRules.GenresIDs);

  // Se obtienen atributos extendidos de las criaturas
  if (!Parser.SetSection("[CriatureExtendedAttributes]")) {
	FatalError("No se pudo estblecer la sección [CriatureExtendedAttributes] en el archivo de reglas.");
  }
  Parser.SetVarPrefix("ExtendedAttribute");
  TLoadIDs(Parser,
		   RulesDefs::MAX_CRIATURE_EXATTRIB,
		   m_CriatureRules.ExtAttribsIDs);

  // Se obtienen slots de equipamiento
  if (!Parser.SetSection("[CriatureEquipmentSlots]")) {
	FatalError("No se pudo estblecer la sección [CriatureEquipmentSlots] en el archivo de reglas.");
  }
  Parser.SetVarPrefix("EquipmentSlot");
  TLoadIDs(Parser,
		   RulesDefs::MAX_CRIATURE_EQUIPMENTSLOT,
		   m_CriatureRules.EquipmentSlotsIDs);

  // Se obtienen las habilidades de las criaturas
  if (!Parser.SetSection("[CriatureHabilities]")) {
	FatalError("No se pudo estblecer la sección [CriatureHabilities] en el archivo de reglas.");;
  }
  Parser.SetVarPrefix("Hability");
  TLoadIDs(Parser,
		   RulesDefs::MAX_CRIATURE_HABILITIES,
		   m_CriatureRules.HabilitiesIDs);

  // Se leen flags de habilidades usables  
  RulesDefs::CriatureHabilities ItHab = 0;
  for (; ItHab < m_CriatureRules.HabilitiesIDs.size(); ++ItHab) {	
	// Se establece prefijo (no existir .UsableFlag significara que no es usable)
	std::ostringstream StrPrefix;	
	StrPrefix << "Hability[" << ItHab << "].";
	Parser.SetVarPrefix(StrPrefix.str());
	if (Parser.ReadFlag("UseFlag", false)) {
	  // La habilidad es usable, se lee el coste de puntos de accion de uso
	  sword swUseCost = Parser.ReadNumeric("InCombatUseCost", false);
	  if (swUseCost < 0) { 
		swUseCost = 0; 
	  }

	  // Se registra la informacion asociada a la habilidad usable en un
	  // map <identificador habilidad usable, coste de uso>
	  const RulesDefs::eIDHability IDHab = RulesDefs::eIDHability(0x01 << ItHab);	  
	  m_CriatureRules.UsableHabilities.insert(UsableHabValType(IDHab, swUseCost));
	}
  }

  // Se obtienen los sintomas de la criatura
  if (!Parser.SetSection("[CriatureSymptoms]")) {
	FatalError("No se pudo estblecer la sección [CriatureSymptoms] en el archivo de reglas.");
  }
  Parser.SetVarPrefix("Symptom");
  TLoadIDs(Parser,
		   RulesDefs::MAX_CRIATURE_SYMPTOMS,
		   m_CriatureRules.SymptomsIDs);

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de cargar la informacion referida a las acciones de minimas
//   distancias. Sera necesario que se hayan cargado las reglas de las
//   criaturas e items.
// - La codificacion en el arbol de items supondra poner en los 16 primeros
//   bits de la clave el tipo del item y en los ultimos la clase.
// Parametros:
// - Parser. Parser a utilizar
// Devuelve:
// - Si todo correcto true, en caso contrario false.
// Notas:
// - Las distancias minimas estaran en el intervalo:
//   [MIN_ACTIONS_DISTANCE_MIN..MIN_ACTIONS_DISTANCE_MAX]
//   y en el caso de que no se lea nada, se supondra 0. Cuando se lea un
//   valor superior al limite permitido, se acotara.
///////////////////////////////////////////////////////////////////////////////
bool
CRulesDataBase::TLoadMinDistancesRules(CCBTParser& Parser)
{
  // Se establece seccion
  if (!Parser.SetSection("[MinDistances]")) {
	FatalError("No se pudo estblecer la sección [MinDistances] en el archivo de reglas.");
  }

  // Minimas distancias
  Parser.SetVarPrefix("");    

  // Hablar
  m_MinDistancesRules.Talk = Parser.ReadNumeric("Talk", false);
  if (m_MinDistancesRules.Talk > RulesDefs::MIN_ACTIONS_DISTANCE_MAX) {
	m_MinDistancesRules.Talk = RulesDefs::MIN_ACTIONS_DISTANCE_MAX;
  } else if (m_MinDistancesRules.Talk < RulesDefs::MIN_ACTIONS_DISTANCE_MIN) {
	m_MinDistancesRules.Talk = RulesDefs::MIN_ACTIONS_DISTANCE_MIN;
  }
  
  // Habilidades
  IDMapIt ItHab(m_CriatureRules.HabilitiesIDs.begin());
  for (; ItHab != m_CriatureRules.HabilitiesIDs.end(); ++ItHab) {
	// ¿La habilidad es usable?
	const RulesDefs::eIDHability IDHab = RulesDefs::eIDHability(RulesDefs::HABILITY_01 << ItHab->first);
	const UsableHabMapIt HabUsableIt(m_CriatureRules.UsableHabilities.find(IDHab));	
	if (HabUsableIt != m_CriatureRules.UsableHabilities.end()) {
	  // Es habilidad usable, se lee la minima distancia
	  std::ostringstream StrHabilItHaby;
	  StrHabilItHaby << "UseHability[" << ItHab->second->szIDStr << "]";
	  RulesDefs::MinDistance Value = Parser.ReadNumeric(StrHabilItHaby.str(), false);
	  if (Value > RulesDefs::MIN_ACTIONS_DISTANCE_MAX) {
		Value = RulesDefs::MIN_ACTIONS_DISTANCE_MAX;
	  } else if (Value < RulesDefs::MIN_ACTIONS_DISTANCE_MIN) {
		Value = RulesDefs::MIN_ACTIONS_DISTANCE_MIN;
	  }
	  m_MinDistancesRules.UseHability.insert(MinDistForActionMapValType(IDHab, Value));
	}	
  }  

  // Items
  IDItemMapIt ItemIt(m_ItemRules.TypesIDs.begin());
  for (; ItemIt != m_ItemRules.TypesIDs.end(); ++ItemIt) {
	// ¿Se puede equipar el item en algun slot principal?
	if (CanSetItemAtEquipmentSlot(ItemIt->first, RulesDefs::EQUIPMENT_SLOT_0) ||
		CanSetItemAtEquipmentSlot(ItemIt->first, RulesDefs::EQUIPMENT_SLOT_1)) {
	  // ¿Es un arma ofensiva el item?
	  if (GetUseWeaponItemCost(ItemIt->first) > 0) {
		// Si, se lee el rango de uso para cada una de las clases asociadas
		IDMapIt ClassIt(ItemIt->second->ClassesIDs.begin());
		for (; ClassIt != ItemIt->second->ClassesIDs.end(); ++ClassIt) {
		  std::ostringstream StrItem;	  
		  StrItem << "UseWeapon[" << ItemIt->second->szIDStr << "].Class[" << ClassIt->second->szIDStr << "]";
		  RulesDefs::MinDistance Value = Parser.ReadNumeric(StrItem.str(), false);		  		  
		  if (Value > RulesDefs::MIN_ACTIONS_DISTANCE_MAX) {
			Value = RulesDefs::MIN_ACTIONS_DISTANCE_MAX;			
		  }	else if (Value < RulesDefs::MIN_ACTIONS_DISTANCE_MIN) {
			Value = RulesDefs::MIN_ACTIONS_DISTANCE_MIN;
		  }  
		  const MinDistanceKey Key = (ItemIt->first << 16) | ClassIt->first;		
		  m_MinDistancesRules.HitWithWeapon.insert(MinDistForActionMapValType(Key, Value));
		}
	  }	
	}
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la informacion referida a las reglas de combate.
// Parametros:
// - Parser. Parser a utilizar
// Devuelve:
// - Si todo correcto true, en caso contrario false.
// Notas:
// - Los costes asociados al uso de habilidades y ataque seran costes base,
//   es decir, que su coste real sera el coste aqui leido mas el coste
//   particular para la habilidad concreta e item concreto.
// - Los costes podran ser mayores o iguales a cero
// - A la hora de leer el atributo extendido a utilizarse para la ordenacion
//   de las criaturas en lista de combate, si no se halla o bien dicho 
//   atributo extendido es incorrecto, se establecera como medio de ordenacion
//   el uso del orden derivado de la inclusion en combate.
///////////////////////////////////////////////////////////////////////////////
bool
CRulesDataBase::TLoadCombatRules(CCBTParser& Parser)
{
  // Se obtiene parser
  if (!Parser.SetSection("[Combat]")) {
	FatalError("No se pudo estblecer la sección [Combat] en el archivo de reglas.");
  }
  Parser.SetVarPrefix("ActionPoints.");  

  // Acciones a leer
  const std::string szActions[] = {
	"Walk", "Equip", "Unequip", "UseFromInventory", 
	"Observe", "GetItem", "Manipule", 
	"UseHability", "Attack"
  };
	
  // Se procede a leer los costes asociados a las distintas acciones
  byte ubIt = 0;
  for (; ubIt < RulesDefs::MAX_CRIATURECOMBATACTIONS; ++ubIt) {	  
	const sword swValue = Parser.ReadNumeric(szActions[ubIt]);
	m_CombatRules.ActionsCost[ubIt] = (swValue < 0) ? 0 : swValue;
  }

  // Se lee el atributo extendido a utilizar para la ordenacion en combate
  m_CombatRules.TurnOrderInfo.Value = RulesDefs::COV_BYINCLUSION;
  Parser.SetVarPrefix("");
  std::string szOrderValue(Parser.ReadString("TurnListOrderBy", false));  
  if (Parser.WasLastParseOk()) {
	// Se comprueba que atributo extendido se ha leido		
	std::transform(szOrderValue.begin(), szOrderValue.end(), szOrderValue.begin(), tolower);
	IDMapIt It(m_CriatureRules.ExtAttribsIDs.begin());
	for (; It != m_CriatureRules.ExtAttribsIDs.end(); ++It) {
	  // ¿El atributo extendido It es el leido?
	  if (0 == stricmp(szOrderValue.c_str(), It->second->szIDStr.c_str())) {
		// Si, se guarda indice y abandona bucle de lectura
		m_CombatRules.TurnOrderInfo.Value = RulesDefs::COV_BYEXATTRIB;		
		m_CombatRules.TurnOrderInfo.IDExAttrib = RulesDefs::eIDExAttrib(It->first);
		break;
	  }	  
	}
  } 

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la liberacion de todos los datos relacionados con las reglas.
// Parametros:
// Devuelve:
// Notas:
// - Se usara un metodo sobre cargado ReleaseIDMap usado para liberar los
//   distintos tipos de Maps asociados a las reglas.
///////////////////////////////////////////////////////////////////////////////
void 
CRulesDataBase::ReleaseRules(void) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Libera maps de reglas
  ReleaseIDMap(m_FloorRules.TypesIDs);
  ReleaseIDMap(m_CriatureRules.TypesIDs);
  ReleaseIDMap(m_CriatureRules.ClassesIDs);
  ReleaseIDMap(m_CriatureRules.GenresIDs);
  ReleaseIDMap(m_CriatureRules.ExtAttribsIDs);
  ReleaseIDMap(m_CriatureRules.EquipmentSlotsIDs);
  ReleaseIDMap(m_CriatureRules.HabilitiesIDs);
  ReleaseIDMap(m_CriatureRules.SymptomsIDs);
  m_CriatureRules.UsableHabilities.clear();
  m_MinDistancesRules.UseHability.clear();
  m_MinDistancesRules.HitWithWeapon.clear();
  ReleaseIDMap(m_SceneObjRules.TypesIDs);  
  ReleaseIDMap(m_ItemRules.TypesIDs);
  ReleaseIDMap(m_ItemRules.GlobalAttribsIDs);
  ReleaseIDMap(m_WallRules.TypesIDs);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera de memoria el maps de tipo IDMap recibido. Esto incluira el puntero
//   al nodo de informacion almacenado, asi como el propio nodo en el map.
// Parametros:
// - Map. Map donde se halla la informacion almacenada.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CRulesDataBase::ReleaseIDMap(IDMap& Map)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Procede a realizar la liberacion
  IDMapIt It = Map.begin();
  while (It != Map.end()) {
	// Borra puntero al nodo de informacion en memoria y en map
	delete It->second;
	It = Map.erase(It);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera de memoria el maps de tipo IDSceneObjMap recibido. Esto incluira 
//   el puntero al nodo de informacion almacenado, asi como el propio nodo 
//   en el map.
// Parametros:
// - IDSceneObjMap. Map donde se halla la informacion almacenada.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CRulesDataBase::ReleaseIDMap(IDSceneObjMap& Map)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Procede a realizar la liberacion
  IDSceneObjMapIt It = Map.begin();
  while (It != Map.end()) {
	// Borra maps almacenados en el nodo
	ReleaseIDMap(It->second->ClassesIDs);
	ReleaseIDMap(It->second->LocalAttribsIDs);

	// Borra puntero al nodo de informacion en memoria y map
	delete It->second;
	It = Map.erase(It);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera de memoria el maps de tipo IDItemMap recibido. Esto incluira 
//   el puntero al nodo de informacion almacenado, asi como el propio nodo 
//   en el map.
// Parametros:
// - IDItemMap. Map donde se halla la informacion almacenada.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CRulesDataBase::ReleaseIDMap(IDItemMap& Map)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Procede a realizar la liberacion
  IDItemMapIt It = Map.begin();
  while (It != Map.end()) {
	// Borra maps almacenados en el nodo
	ReleaseIDMap(It->second->ClassesIDs);
	ReleaseIDMap(It->second->LocalAttribsIDs);
	
	// Borra puntero al nodo de informacion en memoria y map
	delete It->second;
	It = Map.erase(It);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera de memoria el maps de tipo IDWallMap recibido. Esto incluira 
//   el puntero al nodo de informacion almacenado, asi como el propio nodo 
//   en el map.
// Parametros:
// - IDWallMap. Map donde se halla la informacion almacenada.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CRulesDataBase::ReleaseIDMap(IDWallMap& Map)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Procede a realizar la liberacion
  IDWallMapIt It = Map.begin();
  while (It != Map.end()) {
	// Borra maps almacenados en el nodo
	ReleaseIDMap(It->second->LocalAttribsIDs);
	
	// Borra puntero al nodo de informacion en memoria y map
	delete It->second;
	It = Map.erase(It);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dado el tipo de identificador de entidad IDEntityRule devolvera la 
//   direccion del map de identificadores correspondiente. No siempre se podra
//   devolver, en ese caso se retornara NULL.
// Parametros:
// - IDEntityRules. Entidad sobre la que se desea obtener el map.
// - BaseType. Tipo base del que depende el identificador. Este parametro 
//   vale por defecto 0 y es usado solo en ciertas reglas.
// Devuelve:
// - Direccion del map
// Notas:
///////////////////////////////////////////////////////////////////////////////
CRulesDataBase::IDMap* const 
CRulesDataBase::GetIDMap(const RulesDefs::eIDRuleType& IDEntityRule,
						const RulesDefs::BaseType& BaseType)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

    // Retorna el numero de identificadores
  switch (IDEntityRule) {
	case RulesDefs::ID_FLOOR_TYPE: {
	  return &m_FloorRules.TypesIDs;
	} break;

	case RulesDefs::ID_CRIATURE_TYPE: {
	  return &m_CriatureRules.TypesIDs;
	} break;

	case RulesDefs::ID_CRIATURE_CLASS: {
	  return &m_CriatureRules.ClassesIDs;
	} break;

	case RulesDefs::ID_CRIATURE_GENRE: {
	  return &m_CriatureRules.GenresIDs;
	} break;

	case RulesDefs::ID_CRIATURE_EXTATTRIB: {
	  return &m_CriatureRules.ExtAttribsIDs;
	} break;

	case RulesDefs::ID_CRIATURE_EQUIPMENTSLOT: {
	  return &m_CriatureRules.EquipmentSlotsIDs;
	} break;

	case RulesDefs::ID_CRIATURE_HABILITY: {
	  return &m_CriatureRules.HabilitiesIDs;
	} break;

	case RulesDefs::ID_CRIATURE_SYMPTOM: {
	  return &m_CriatureRules.SymptomsIDs;
	} break;
	
	case RulesDefs::ID_SCENEOBJ_CLASS: {
	  IDSceneObjMapIt It = m_SceneObjRules.TypesIDs.find(BaseType);
	  ASSERT((It != m_SceneObjRules.TypesIDs.end()) != 0);	  	  
	  return &It->second->ClassesIDs;
	} break;

    case RulesDefs::ID_SCENEOBJ_LOCALATTRIB: {
	  IDSceneObjMapIt It = m_SceneObjRules.TypesIDs.find(BaseType);
	  ASSERT((It != m_SceneObjRules.TypesIDs.end()) != 0);
	  return &It->second->LocalAttribsIDs;
	} break;
	
	case RulesDefs::ID_WALL_LOCALATTRIB: {
	  IDWallMapIt It = m_WallRules.TypesIDs.find(BaseType);
	  ASSERT((It != m_WallRules.TypesIDs.end()) != 0);
	  return &It->second->LocalAttribsIDs;
	} break;

	case RulesDefs::ID_ITEM_GLOBALATTRIB: {
	  return &m_ItemRules.GlobalAttribsIDs;
	} break;

	case RulesDefs::ID_ITEM_LOCALATTRIB: {
	  IDItemMapIt It(m_ItemRules.TypesIDs.find(BaseType));
	  ASSERT((It != m_ItemRules.TypesIDs.end()) != 0);	  	  
	  return &It->second->LocalAttribsIDs;
	} break;

	case RulesDefs::ID_ITEM_CLASS: {
	  IDItemMapIt It(m_ItemRules.TypesIDs.find(BaseType));
	  ASSERT((It != m_ItemRules.TypesIDs.end()) != 0);	  	  
	  return &It->second->ClassesIDs;
	} break;

  }; // ~ switch

  // Casos en los que no se podra devolver direccion del map
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el numero de identificadores asociados a la entidad IDentityRule.
// Parametros:
// - IDEntityRules. Entidad sobre la que se desea obtener el identificador.
// - BaseType. Tipo base del que depende el identificador. Este parametro 
//   vale por defecto 0 y es usado solo en ciertas reglas.
// Devuelve:
// - El numero de identificadores pedido. 
// Notas:
///////////////////////////////////////////////////////////////////////////////
word 
CRulesDataBase::GetNumEntitiesIDs(const RulesDefs::eIDRuleType& IDEntityRule,
								  const RulesDefs::BaseType& BaseType)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se obtiene direccion a map de identificadores basico
  IDMap* const pIDMap = GetIDMap(IDEntityRule, BaseType);

  // Vbles
  word uwSize = 0; // Tamaño a devolver
  
  // ¿Se obtuvo identifcador?
  if (pIDMap) {	
	// Obtiene tamaño
	uwSize = pIDMap->size();
  } else {
	// No se pudo obtener map, estamos ante identificador compuesto
	// o un caso especial
	switch (IDEntityRule) {
	  case RulesDefs::ID_SCENEOBJ_TYPE: {
		return m_SceneObjRules.TypesIDs.size();
	  } break;

	  case RulesDefs::ID_WALL_TYPE: {
		return m_WallRules.TypesIDs.size();
	  } break;

	  case RulesDefs::ID_ITEM_TYPE: {
		return m_ItemRules.TypesIDs.size();
	  } break;
	  
	  case RulesDefs::ID_CRIATURE_USABLEHABILITY: {
		// Se quiere conseguir el numero de habilidades usables
		const word uwNumHabilities = GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);		
		byte ubIt = 0;
		for (; ubIt < uwNumHabilities; ++ubIt) {
		  if (IsUsableHability(RulesDefs::eIDHability(RulesDefs::HABILITY_01 << ubIt))) {
			++uwSize;
		  }
		}
	  } break;
	}; // ~ switch
  }
  
  // Se retorna valor
  return uwSize;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - A partir del identificador de texto de una entidad definida en las reglas
//   se obtendra el identificador numerico.
// Parametros:
// - IDEntityRule. Entidad sobre la que obtener la informacion.
// - szIDStr. Identificador de texto
// - BaseType. Tipo base auxiliar. Por defecto 0 y usado solo con determinadas
//   entidades.
// Devuelve:
// - Identificador numerico igual o superior a 0 si es valido y menor que 0
//   si no es valido. 
// Notas:
// - Se devolvera sword en lugar de BaseType porque esta sera una manera de
//   indicar si el identificador szIDStr existe o no
///////////////////////////////////////////////////////////////////////////////
sword
CRulesDataBase::GetEntityIDNumeric(const RulesDefs::eIDRuleType& IDEntityRule,
								   const std::string& szIDStr,
								   const RulesDefs::BaseType& BaseType)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se pasa a minusculas
  std::string szIDStrLower(szIDStr);
  std::transform(szIDStrLower.begin(), szIDStrLower.end(), szIDStrLower.begin(), tolower);

  // Se obtiene direccion a map de identificadores basico
  sword swIDNumeric;
  IDMap* const pIDMap = GetIDMap(IDEntityRule, BaseType);
  if (pIDMap) {	
	// Obtiene identificador
    CMapValueSearch<RulesDefs::BaseType, sIDInfo> Predicate(szIDStrLower);
    const IDMapIt It = std::find_if(pIDMap->begin(), pIDMap->end(), Predicate);
	swIDNumeric = (It != pIDMap->end()) ? It->first : -1;
  } else {
	// No se pudo obtener map, estamos ante identificador compuesto
	switch (IDEntityRule) {
	  case RulesDefs::ID_SCENEOBJ_TYPE: {
		// Obtiene identificador
		CMapValueSearch<RulesDefs::SceneObjType, sIDSceneObjInfo> Predicate(szIDStrLower);
		const IDSceneObjMapIt It = std::find_if(m_SceneObjRules.TypesIDs.begin(), 
												m_SceneObjRules.TypesIDs.end(), 
												Predicate);
		swIDNumeric = (It != m_SceneObjRules.TypesIDs.end()) ? It->first : -1;
	  } break;

	  case RulesDefs::ID_WALL_TYPE: {
		CMapValueSearch<RulesDefs::WallType, sIDWallInfo> Predicate(szIDStrLower);
		const IDWallMapIt It = std::find_if(m_WallRules.TypesIDs.begin(), 
											m_WallRules.TypesIDs.end(), 
											Predicate);
		swIDNumeric = (It != m_WallRules.TypesIDs.end()) ? It->first : -1;
	  } break;

	  case RulesDefs::ID_ITEM_TYPE: {
		CMapValueSearch<RulesDefs::ItemType, sIDItemInfo> Predicate(szIDStrLower);
		const IDItemMapIt It = std::find_if(m_ItemRules.TypesIDs.begin(), 
											m_ItemRules.TypesIDs.end(), 
											Predicate);
		swIDNumeric = (It != m_ItemRules.TypesIDs.end()) ? It->first : -1;
	  } break;
	}; // ~ switch
  }
  
  // Se retorna valor
  return swIDNumeric;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el identificador de texto asociado al identificador numerico
//   de una determinada entidad de las reglas.
// Parametros:
// - IDEntityRule. Entidad sobre la que obtener la informacion.
// - IDNumeric. Identificador numerico.
// - BaseType. Tipo base auxiliar. Por defecto 0 y usado solo con determinadas
//   entidades.
// Devuelve:
// - El string asociado al identificador. Si se retorna string vacio, 
//   significara que el identificador IDNumeric no es valido.
// Notas:
///////////////////////////////////////////////////////////////////////////////
std::string 
CRulesDataBase::GetEntityIDStr(const RulesDefs::eIDRuleType& IDEntityRule,
							   const RulesDefs::BaseType& IDNumeric,
							   const RulesDefs::BaseType& BaseType)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se obtiene direccion a map de identificadores basico
  IDMap* const pIDMap = GetIDMap(IDEntityRule, BaseType);

  // ¿Se obtuvo identificador?
  std::string szIDStr;
  if (pIDMap) {	
	// Obtiene identificador
	IDMapIt It = pIDMap->find(IDNumeric);
	szIDStr = (It != pIDMap->end()) ? It->second->szIDStr : "";
  } else {
	// No se pudo obtener map, estamos ante identificador compuesto
	switch (IDEntityRule) {
	  case RulesDefs::ID_SCENEOBJ_TYPE: {
		IDSceneObjMapIt It = m_SceneObjRules.TypesIDs.find(IDNumeric);
		szIDStr = (It != m_SceneObjRules.TypesIDs.end()) ? It->second->szIDStr : "";
	  } break;

   	  case RulesDefs::ID_WALL_TYPE: {
		IDWallMapIt It = m_WallRules.TypesIDs.find(IDNumeric);
		szIDStr = (It != m_WallRules.TypesIDs.end()) ? It->second->szIDStr : "";
	  } break;

	  case RulesDefs::ID_ITEM_TYPE: {
		IDItemMapIt It = m_ItemRules.TypesIDs.find(IDNumeric);		
		szIDStr = (It != m_ItemRules.TypesIDs.end()) ? It->second->szIDStr : "";		
	  } break;
	}; // ~ switch
  }
  
  // Se retorna valor
  return szIDStr;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la minima distancia en la realizacion de una determinada accion.
//   Las acciones estaran delimitadas a las indicadas en eMinDistance.
// - Las posibles acciones con minima distancia seran:
//    * Hablar.
//    * Usar habilidad. Se usara udParam para indicar la habilidad. Las 
//      habilidades se pasaran el el formato normal (no bitflag).
//    * Usar un arma ofensiva. Se usara udParam para indicar tipo y
//      udExtraParam para indicar clase del arma (item)
// Parametros:
// - Action. Accion para la que deseamos conocer la minima distancia.
// - udParam. Parametro basico. Por defecto 0.
// - udExtraParam. Parametro extra. Por defecto 0.
// Devuelve:
// - La minima distancia.
// Notas:
// - En caso de pasar un parametro ubParam para una habilidad no usable,
//   se producira un error via ASSERT.
// - El parametro debera de permanecer 0 si la accion no lo necesita, en caso
//   contrario se producira error.
///////////////////////////////////////////////////////////////////////////////
RulesDefs::MinDistance
CRulesDataBase::GetMinDistanceFor(const RulesDefs::eMinDistance& Action,
								  const dword udParam,								 
								  const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Vbles
  RulesDefs::MinDistance MinDistanceValue; // Valor a retornar

  // Se comprueba el tipo de habilidad recibida por si hay que hacer tratamiento
  switch(Action) {
	case RulesDefs::MD_TALK: {
	  // Hablar
	  MinDistanceValue = m_MinDistancesRules.Talk;
	} break;

	case RulesDefs::MD_USEHABILITY: {
	  // Usar habilidad
	  ASSERT(!m_MinDistancesRules.UseHability.empty());
	  const MinDistForActionMapIt It(m_MinDistancesRules.UseHability.find(udParam));
	  ASSERT((It != m_MinDistancesRules.UseHability.end()) != 0);
	  MinDistanceValue = It->second;
	} break;

	case RulesDefs::MD_HITWITHWEAPON: {
	  // Golpear con arma
	  const MinDistanceKey Key = (udParam << 16) | udExtraParam;
	  const MinDistForActionMapIt It(m_MinDistancesRules.HitWithWeapon.find(Key));
	  ASSERT((It != m_MinDistancesRules.HitWithWeapon.end()) != 0);
	  MinDistanceValue = It->second;
	} break;

	default: 
	  ASSERT(false);
  }; // ~ switch

  // Retorna resultad
  return MinDistanceValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el objeto de escenario de identificador SceneObj es o no
//   contenedor.
// Parametros:
// - SceneObj. Identificador al objeto de escenario.
// Devuelve:
// - Flag de objeto contenedor.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CRulesDataBase::IsSceneObjContainer(const RulesDefs::SceneObjType& SceneObj) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Retorna el flag
  IDSceneObjMapIt It = m_SceneObjRules.TypesIDs.find(SceneObj);
  return (It != m_SceneObjRules.TypesIDs.end()) ? It->second->bContainerFlag : false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el item de tipo ItemType puede ser establecido en el 
//   slot de equipamiento Slot.
// Parametros:
// - ItemType. Tipo de item.
// - Slot. Slot de equipamiento.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CRulesDataBase::CanSetItemAtEquipmentSlot(const RulesDefs::ItemType& ItemType,
								          const RulesDefs::EquipmentSlot& Slot)
{
  // SOLO si instancia inicializada
  // ASSERT(IsInitOk()); neccesiaro para poder usarse desde TLoadMinDistances
  // SOLO si parametros validos
  ASSERT((Slot < m_CriatureRules.EquipmentSlotsIDs.size()) != 0);
  ASSERT((ItemType < m_ItemRules.TypesIDs.size()) != 0);

  // Localiza el nodo asociado al tipo de item
  IDItemMapIt It(m_ItemRules.TypesIDs.find(ItemType));
  ASSERT((It != m_ItemRules.TypesIDs.end()) != 0);

  // Comprueba si el slot Slot esta habilitado
  const byte ubBitFlag = (0x01 << Slot);
  return (It->second->ValidEquipSlotsMask & ubBitFlag) ? true : false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Emitira un mensaje de error fatal y abandonará de forma inmediata.
// Parametros:
// - szMsg. Mensaje a emitir.
// Devuelve:
// Notas:
// - Si está activo el motor, dejará en manos de FatalError la salida, en caso
//   de estar asociado a una utilidad, se saldra via exit.
///////////////////////////////////////////////////////////////////////////////
void 
CRulesDataBase::FatalError(const std::string& szMsg)
{
  // Se muestra mensaje segun proceda
  #ifdef _CRISOLENGINE
    SYSEngine::FatalError("%s", szMsg.c_str());  
  #else
	std::cout << szMsg << "\n";
	exit(1);
  #endif  
}
