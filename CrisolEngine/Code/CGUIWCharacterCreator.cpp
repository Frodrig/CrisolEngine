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
// CGUIWCharacterCreator.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWCharacterCreator.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

#include "CGUIWCharacterCreator.h"

#include "SYSEngine.h"
#include "iCGraphicSystem.h"
#include "iCGameDataBase.h"
#include "iCAudioSystem.h"
#include "iCInputManager.h"
#include "iCLogger.h"
#include "iCGUIManager.h"
#include "iCGameDataBase.h"
#include "iCFontSystem.h"
#include "CPlayer.h"
#include "CCBTEngineParser.h"
#include "CMemoryPool.h"

// Definicion de estructuras forward
struct CGUIWCharacterCreator::sNSpritePortrait {
  // Nodo de un sprite asociado a un retrato  
  std::string szAnimTemplate; // Retrato
  std::string szSprite;       // Representacion del jugador
  std::string szShadowImage;  // Posible sombra asociada
  CSprite     Portrait;       // Retrato modelado como un sprite  
  // Constructor
  sNSpritePortrait(void) { }
  sNSpritePortrait(const std::string& szPortrait,
				   const std::string& aszSprite,
				   const std::string& szShadow): szAnimTemplate(szPortrait),
											     szSprite(aszSprite),
												 szShadowImage(szShadow) { }  
  // Manejador de memoria
  static CMemoryPool  MPool;
  static void* operator new(const size_t size) { return MPool.AllocMem(size); }
  static void operator delete(void* pItem) { MPool.FreeMem(pItem); } 
};

struct CGUIWCharacterCreator::sNProfileTypeInfo {
  // Nodo de informacion asociado a un tipo de perfil de jugador
  RulesDefs::CriatureType IdType;    // Identificador tipo  
  ProfileClassInfoList    ClassInfo; // Clases asociadas
  // Constructores
  sNProfileTypeInfo(const RulesDefs::CriatureType& aIdType): IdType(aIdType) { }
  // Manejador de memoria
  static CMemoryPool  MPool;
  static void* operator new(const size_t size) { return MPool.AllocMem(size); }
  static void operator delete(void* pItem) { MPool.FreeMem(pItem); } 
};

struct CGUIWCharacterCreator::sNProfileClassInfo {
  // Nodo de informacion asociado a una clase de perfil de jugador
  RulesDefs::CriatureClass IdClass;                    // Identificador clase
  word					   uwAttribsDistribPoints;     // Ptos distribuibles en atributos  
  word					   uwHabilitiesDistribPoints;  // Ptos distribuibles en habilidades
  bool                     bMustDistribAttribPoints;   // ¿Se deben distribuir ptos de atributos?
  bool                     bMustDistribHabilityPoints; // ¿Se deben distribuir ptos de habilidad?
  sProfileAttribInfo       HealthInfo;                 // Info asociada a la salud
  sProfileAttribInfo       CombatPointsInfo;           // Info asociada a los puntos de combate
  // Info asociada a los atributos extendidos
  sProfileAttribInfo ExtAttribsInfo[RulesDefs::MAX_CRIATURE_EXATTRIB];  
  // Info asociada a las habilidades
  sProfileImproveCost HabilitiesInfo[RulesDefs::MAX_CRIATURE_HABILITIES];
  // Constructores  
  sNProfileClassInfo(const RulesDefs::CriatureClass& aIdClass): IdClass(aIdClass) { }
  sNProfileClassInfo(const sNProfileClassInfo& NProfileClassInfo) {
	// Identificador
	IdClass = NProfileClassInfo.IdClass;
	// Ptos distribuibles
	uwAttribsDistribPoints = NProfileClassInfo.uwAttribsDistribPoints;
	uwHabilitiesDistribPoints = NProfileClassInfo.uwHabilitiesDistribPoints;
	bMustDistribAttribPoints = NProfileClassInfo.bMustDistribAttribPoints;
	bMustDistribHabilityPoints = NProfileClassInfo.bMustDistribHabilityPoints;
	// Atributos y habilidades
	HealthInfo = NProfileClassInfo.HealthInfo;
	CombatPointsInfo = NProfileClassInfo.CombatPointsInfo;
	byte ubIt = 0;
	for (; ubIt < RulesDefs::MAX_CRIATURE_EXATTRIB; ++ubIt) {
	  ExtAttribsInfo[ubIt] = NProfileClassInfo.ExtAttribsInfo[ubIt];
	}
	ubIt = 0;
	for (; ubIt < RulesDefs::MAX_CRIATURE_HABILITIES; ++ubIt) {
	  HabilitiesInfo[ubIt] = NProfileClassInfo.HabilitiesInfo[ubIt];
	}
  }
  // Manejador de memoria
  static CMemoryPool  MPool;
  static void* operator new(const size_t size) { return MPool.AllocMem(size); }
  static void operator delete(void* pItem) { MPool.FreeMem(pItem); } 
};

// Inicializacion del manejador de memoria
CMemoryPool 
CGUIWCharacterCreator::sNSpritePortrait::MPool(2,
											   sizeof(CGUIWCharacterCreator::sNSpritePortrait), 
											   true);
CMemoryPool 
CGUIWCharacterCreator::sNProfileTypeInfo::MPool(2,
											    sizeof(CGUIWCharacterCreator::sNProfileTypeInfo), 
											    true);
CMemoryPool 
CGUIWCharacterCreator::sNProfileClassInfo::MPool(2,
												sizeof(CGUIWCharacterCreator::sNProfileClassInfo), 
												true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar.
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWCharacterCreator::Init(void)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  #ifdef ENGINE_TRACE
    SYSEngine::GetLogger()->Write("CGUIWCharacterCreator::Init> Inicializando interfaz para la creación de personajes.\n");
  #endif

  // Se inicializa clase base
  if (Inherited::Init()) {	
	// Se toman interfaces a otros componentes
	m_pGUIManager = SYSEngine::GetGUIManager();
	ASSERT(m_pGUIManager);
	m_pInputManager = SYSEngine::GetInputManager();
	ASSERT(m_pInputManager);	
	m_pGraphSys = SYSEngine::GetGraphicSystem();
	ASSERT(m_pGraphSys);
	m_pGDBase = SYSEngine::GetGameDataBase();
	ASSERT(m_pGDBase);

	// Se cargan datos, inicializandose componentes
	if (TLoad()) {	  
	  // Todo correcto
	  #ifdef ENGINE_TRACE
		SYSEngine::GetLogger()->Write("                           | Ok.\n");
	  #endif
	  return true;
	}	
  }

  // Hubo problemas
  #ifdef ENGINE_TRACE
    SYSEngine::GetLogger()->Write("                           | Error.\n");
  #endif
  End();
  return false;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la carga datos e inicializacion de componentes.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool
CGUIWCharacterCreator::TLoad(void)
{
  // Se obtiene el parser
  CCBTEngineParser* const pParser = m_pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG, 
													        "[PlayerCreator]");
  ASSERT(pParser);

  // Procede a cargar e inicializar componentes
  if (!TLoadFloatingTextInfo(pParser)) { 	
	return false;
  }

  // Carga info de perfiles de jugador
  if (!TLoadPlayerProfilesInfo()) { 
	return false;
  }

  // Carga e inicializa imagen de fondo
  if (!TLoadBackImg(pParser)) {
	return false;
  }

  // Carga e inicializa boton de cancelacion
  if (!TLoadCancelInfo(pParser)) { 
	return false;
  }

  // Carga e inicializa boton de aceptacion
  if (!TLoadAcceptInfo(pParser)) { 
	return false;
  }

  // Carga e inicializa componente asociado al nombre
  if (!TLoadNameInfo(pParser)) { 
	return false;
  }

  // Carga e inicializa componente asociado al genero
  if (!TLoadGenreInfo(pParser)) { 
	return false;
  }

  // Carga e inicializa componente asociado al tipo y clase
  if (!TLoadTypeClassInfo(pParser)) { 
	return false;
  }

  // Carga e inicializa componentes asociados a atributos
  if (!TLoadAttributesInfo(pParser)) { 
	return false;
  }

  // Carga e inicializa componentes asociados a habilidades
  if (!TLoadHabilitiesInfo(pParser)) { 
	return false;
  }

  // Carga e inicializa info comun al perfil de jugador
  if (!TLoadPlayerCommonInfo()) {
	return false;
  }

  // Carga datos asociados a la musica
  if (!TLoadMusicInfo(pParser)) { 
	return false;
  }

  // Carga indo de fade
  if (!TLoadFadeInfo(pParser)) { 
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga datos asociados al texto flotante
// Parametros:
// - pParser. Parser a utilizar
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool
CGUIWCharacterCreator::TLoadFloatingTextInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Situa prefijo
  pParser->SetVarPrefix("FloatingText.");

  // Color
  m_FloatingTextInfo.Color = pParser->ReadRGBColor();
  
  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la informacion de fade desde disco
// Parametros:
// - pParser. Parser a utilizar
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWCharacterCreator::TLoadFadeInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Fade inicial (desde color a imagen)
  pParser->SetVarPrefix("Fade."); 
  m_FadeInfo.StartFade.RGBColor = pParser->ReadRGBColor("Start.");
  m_FadeInfo.StartFade.uwSpeed = pParser->ReadNumeric("In.Speed");
  ASSERT(m_FadeInfo.StartFade.uwSpeed);

  // Fade de cancelacion (desde imagen a color)
  m_FadeInfo.CancelFade.RGBColor = pParser->ReadRGBColor("Out.OnCancel.");
  m_FadeInfo.CancelFade.uwSpeed = pParser->ReadNumeric("Out.OnCancel.Speed");
  ASSERT(m_FadeInfo.CancelFade.uwSpeed);

  // Fade de aceptacion de caracter (desde imagen a color)
  m_FadeInfo.AcceptFade.RGBColor = pParser->ReadRGBColor("Out.OnAccept.");
  m_FadeInfo.AcceptFade.uwSpeed = pParser->ReadNumeric("Out.OnAccept.Speed");
  ASSERT(m_FadeInfo.AcceptFade.uwSpeed);

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga todos los datos referidos a los perfiles que se pueden crear de
//   jugador. Estos datos seran almacenados para poder consultarlos en cualquier
//   momento.
// Parametros:
// Devuelve:
// - Si existe al menos un tipo con una clase asociada true. En caso contrario
//   false.
// Notas:
// - Los datos asociados al perfil del jugador se hallaran en un archivo
//   distinto al de la configuracion de la ventana, por lo que se debera de
//   solicitar el parser al mismo
// - En la salud y los puntos de combate, el valor base debera de ser mayor 
//   de 0.
// - Cuando se lea un "Atributo.ImproveCost" si no se halla informacion 
//   se entendera que cuesta 1 punto. Si es valor 0, se entendera que no se
//   podra incrementar.
// - Cuando se lea un "Habilidad.SetCost" si no se halla informacion se
//   entendera que no se puede establecer. En caso de leer 0 indicara 
//   igualmente que no se podra establecer
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWCharacterCreator::TLoadPlayerProfilesInfo(void)
{
  // Obtiene parser al archivo con los perfiles
  CCBTEngineParser* const pParser = m_pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_PLAYER_PROFILES,
															"[Profiles]");	
  if (!pParser) { 
    SYSEngine::FatalError("No se localizó la sección [PlayerProfiles] en archivo de perfiles de jugador.\n");
  } 

  // Obtiene el numero maximo de tipos y criaturas
  const word uwMaxTypes = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_TYPE);
  const word uwMaxGenres = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_GENRE);	
  const word uwMaxClasses = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_CLASS);
  const word uwMaxExtAttribs = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB);
  const word uwMaxHabilities = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);
  
  // Navega entre los tipos de criaturas
  byte ubTypeIt = 0;
  for (; ubTypeIt < uwMaxTypes; ++ubTypeIt) {
	// Vbles
	sNProfileTypeInfo* pNTypeInfo = NULL; // Nodo de tipo

	// Crea el prefijo de tipo
	std::string szTypePrefix;
	SYSEngine::PassToString(szTypePrefix,
						    "Type[%s].",
							m_pGDBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_TYPE,
																		  ubTypeIt).c_str());

	// Navega entre las clases de criaturas
	byte ubClassIt = 0;
	for (; ubClassIt < uwMaxClasses; ++ubClassIt) {
	  // Crea el prefijo de clase
	  std::string szClassPrefix;
	  SYSEngine::PassToString(szClassPrefix,
						      "Class[%s].",
							  m_pGDBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_CLASS,
																			ubClassIt).c_str());
	  // Establece el prefijo global
	  pParser->SetVarPrefix(szTypePrefix + szClassPrefix);
	  
	  // Si comienza a alojarse informacion
	  sNProfileClassInfo ClassInfo(ubClassIt); // Estructura donde alojar la info
		  	
	  // Salud
	  // Nota: Debera de existir un valor de salud, en caso contrario se pasa a sig.
	  ClassInfo.HealthInfo.swBaseValue = pParser->ReadNumeric("Health.BaseValue", false);
	  if (!pParser->WasLastParseOk()) {
		continue;
	  }
	  ASSERT(ClassInfo.HealthInfo.swBaseValue);
	  ClassInfo.HealthInfo.uwCost = pParser->ReadNumeric("Health.ImproveCost", false);
	  // if (!pParser->WasLastParseOk()) { ClassInfo.HealthInfo.uwCost = 1; }
		
	  // Puntos de combate		
	  ClassInfo.CombatPointsInfo.swBaseValue = pParser->ReadNumeric("CombatPoints.BaseValue");
	  ClassInfo.CombatPointsInfo.uwCost = pParser->ReadNumeric("CombatPoints.ImproveCost", false);
	  //if (!pParser->WasLastParseOk()) { ClassInfo.CombatPointsInfo.uwCost = 1; }

	  // Atributos extendidos
	  byte ubTmpIt = 0;
	  for (; ubTmpIt < uwMaxExtAttribs; ++ubTmpIt) {
	    // Se forma variable
	    std::string szVar;
	    SYSEngine::PassToString(szVar, 
	  							"ExtendedAttribute[%s].",
								m_pGDBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_EXTATTRIB,
																			  ubTmpIt).c_str());
		// Se lee informacion
		ClassInfo.ExtAttribsInfo[ubTmpIt].swBaseValue = pParser->ReadNumeric(szVar + "BaseValue", false);
		ClassInfo.ExtAttribsInfo[ubTmpIt].uwCost = pParser->ReadNumeric(szVar + "ImproveCost", false);
	  }

	  // Coste de establecer habilidades
	  ubTmpIt = 0;
	  for (; ubTmpIt < uwMaxHabilities; ++ubTmpIt) {
	    // Se forma variable
	    std::string szVar;
	    SYSEngine::PassToString(szVar, 
								"Hability[%s].SetCost",
								m_pGDBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_HABILITY,
																			  ubTmpIt).c_str());
		// Se lee informacion
		ClassInfo.HabilitiesInfo[ubTmpIt].uwCost = pParser->ReadNumeric(szVar, false);
	  }

	   // Ptos. distribucion de atributos y habilidades
	  ClassInfo.uwAttribsDistribPoints = pParser->ReadNumeric("DistributionPoints.Attributes.Value", false);	
	  ClassInfo.uwHabilitiesDistribPoints = pParser->ReadNumeric("DistributionPoints.Habilities.Value", false);		
	  ClassInfo.bMustDistribAttribPoints = pParser->ReadFlag("DistributionPoints.Attributes.MustUseFlag", false);
	  ClassInfo.bMustDistribHabilityPoints = pParser->ReadFlag("DistributionPoints.Habilities.MustUseFlag", false);	
	  
	  // ¿No habia nodo de tipo creado?
	  if (!pNTypeInfo) {
	    // Se crea
	    pNTypeInfo = new sNProfileTypeInfo(ubTypeIt);
	    ASSERT(pNTypeInfo);
	  }

	  // Se asocia al nodo de tipo la informacion de clase leida
	  sNProfileClassInfo* pNClassInfo = new sNProfileClassInfo(ClassInfo);
	  ASSERT(pNClassInfo);
	  pNTypeInfo->ClassInfo.push_back(pNClassInfo);
	}

	// ¿Se creo nodo de tipo?
	if (pNTypeInfo) {
	  // Se inserta en lista de tipos
	  m_ProfileInfo.TypeInfo.push_back(pNTypeInfo);
    }
  }     

  // ¿Existe al menos un tipo con una clase?
  return m_ProfileInfo.TypeInfo.size() && 
		 m_ProfileInfo.TypeInfo.front()->ClassInfo.size() ? true : false;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga datos asociados al componente de la imagen de fondo e inicializa.
// Parametros:
// - pParser. Parser con el que obtener la informacion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool
CGUIWCharacterCreator::TLoadBackImg(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Vbles
  CGUICBitmap::sGUICBitmapInitData InitData; // Datos de inicializacion

  // Se cargan datos
  pParser->SetVarPrefix("BackImage.");    
  InitData.BitmapInfo.szFileName = pParser->ReadString("FileName");  

  // Se completan datos
  InitData.ID = CGUIWCharacterCreator::ID_BACKIMG;  
  InitData.BitmapInfo.WidthTextureDim = GraphDefs::TEXTURE_DIM_32;
  InitData.BitmapInfo.HeightTextureDim = GraphDefs::TEXTURE_DIM_32;
  InitData.BitmapInfo.Bpp = GraphDefs::BPP_16;
  InitData.BitmapInfo.ABpp = GraphDefs::ALPHA_BPP_0;  
  
  // Se realiza la inicializacion
  if (!m_BackImgInfo.BackImg.Init(InitData)) {
	return false;
  }

  // Se añaden las posibles animaciones asociadas
  word uwIt = 0;
  for (; uwIt < CGUICAnimBitmap::MAX_ANIMS; ++uwIt) {
	// Se lee la posible plantilla de animacion asociada a animacion
	std::string szVar;
	SYSEngine::PassToString(szVar, "Anim[%u].", uwIt);
	const std::string szAnim(pParser->ReadString(szVar + "AnimTemplate", false));
	
	// ¿Se leyo plantilla de animacion?
	if (pParser->WasLastParseOk()) {
	  // Si, se lee posicion y alpha y se añade a la imagen
	  // Nota: La posicion sera obligatoria, el valor alpha no.
	  m_BackImgInfo.BackImg.AddAnim(szAnim, 
								    pParser->ReadPosition(szVar), 
									pParser->ReadAlpha(szVar, false));
	} else {
	  // No, se abandona
	  break;
	}
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga datos asociados al boton de cancelacion de creacion de caracteres
// Parametros:
// - pParser. Parser con el que obtener la informacion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool
CGUIWCharacterCreator::TLoadCancelInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);  

  // Vbles
  CGUICButton::sGUICButtonInitData InitData; // Datos inicializacion

  // Carga datos 
  // Boton de cancelacion
  pParser->SetVarPrefix("Options.CancelButton.");  
  TLoadOptionButton(InitData, 
				    CGUIWCharacterCreator::ID_CANCELBUTTON, 
					pParser);
  if (!m_CancelInfo.Button.Init(InitData)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la informacion del boton de aceptacion
// Parametros:
// - pParser. Boton a utilizar
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWCharacterCreator::TLoadAcceptInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);  

  // Vbles
  CGUICButton::sGUICButtonInitData InitData; // Datos inicializacion

  // Carga datos 
  // Boton de cancelacion
  pParser->SetVarPrefix("Options.AcceptButton.");  
  TLoadOptionButton(InitData, 
				    CGUIWCharacterCreator::ID_ACCEPTBUTTON, 
					pParser);
  if (!m_AcceptInfo.Button.Init(InitData)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga datos asociados al componente nombre y lo inicializa.
// Parametros:
// - pParser. Parser con el que obtener la informacion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool
CGUIWCharacterCreator::TLoadNameInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);  

  // Vbles
  CGUICLineText::sGUICLineTextInitData InitLineOption; // Init. opcion de introduccion

  // Carga de datos de la opcion de introduccion de texto
  pParser->SetVarPrefix("Name.");  
  // Texto estatico
  InitLineOption.szLine = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_INTERFAZ_PLAYERCREATION_NAME);
  TLoadLineOption(InitLineOption,
				  CGUIWCharacterCreator::ID_NAMEOPTION,
				  pParser);
  if (!m_NameInputInfo.NameText.Init(InitLineOption)) {
	return false;
  }

  // Vbles
  CGUICInput::sGUICInputInitData InputInitData;  // Datos inicializacion entrada

  // Carga de datos del componente de entrada del nombre
  pParser->SetVarPrefix("Name.");  
  InputInitData.Position = pParser->ReadPosition("Input.");  
  InputInitData.uwMaxCharacters = pParser->ReadNumeric("Input.MaxCharacters");  
  InputInitData.RGBTextSelect = pParser->ReadRGBColor("Input.Select.");
  InputInitData.RGBTextUnselect = pParser->ReadRGBColor("Input.Unselect.");  
  
  
  // Se completan datos
  InputInitData.szFont = "Arial";
  InputInitData.ID = CGUIWCharacterCreator::ID_NAMEINPUT;
  InputInitData.pGUIWindow = this;
  InputInitData.GUIEvents = GUIDefs::GUIC_INPUT_ACCEPT_ACQUIRING |
							GUIDefs::GUIC_INPUT_NOACCEPT_ACQUIRING;
  InputInitData.ubDrawPlane = 0;

  // Procede a inicializar
  if (!m_NameInputInfo.NameInput.Init(InputInitData)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga e inicializa componentes asociados a la seleccion del genero
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool
CGUIWCharacterCreator::TLoadGenreInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Vbles
  CGUICLineText::sGUICLineTextInitData InitLineData;   // Inicializacion de lineas
  CGUICButton::sGUICButtonInitData     ButtonInitData; // Inicializacion de botones
  
  // Se leen datos asociados a la configuracion de la opción genero
  pParser->SetVarPrefix("Genre.");
  InitLineData.szLine = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_INTERFAZ_PLAYERCREATION_GENRE);  
  TLoadLineOption(InitLineData,
				  CGUIWCharacterCreator::ID_GENREOPTION,
				  pParser);  
  if (!m_GenreInfo.GenreOption.Init(InitLineData)) { 
	return false;
  }
  
  // Se cargan datos asociados a la linea que representa el genero escogido
  // estableciendo el genero (0) como genero actual asociado
  TLoadLineSelect(InitLineData,
				  m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_GENRE, 0),
				  CGUIWCharacterCreator::ID_GENRESELECT,
				  pParser);      
  if (!m_GenreInfo.GenreSelect.Init(InitLineData)) { 
	return false;
  }  

  // Se leen los datos del quad de fx (la inicializacion al activar)
  m_GenreInfo.FXBack.RGBQuad = pParser->ReadRGBColor("Selection.FXBack.");
  m_GenreInfo.FXBack.AlphaQuad = pParser->ReadAlpha("Selection.FXBack.");
  // Se ajusta la posicion con margen estetico y segun componente seleccionado
  m_GenreInfo.FXBack.Position.swXPos = InitLineData.Position.swXPos - 5;
  m_GenreInfo.FXBack.Position.swYPos = InitLineData.Position.swYPos - 1;
  
  // Inicializacion del boton a sig. retrato
  pParser->SetVarPrefix("PlayerPortrait.Options.NextButton.");
  TLoadOptionButton(ButtonInitData,
					CGUIWCharacterCreator::ID_NEXTPORTRAITBUTTON,
					pParser);
  if (!m_GenreInfo.NextButton.Init(ButtonInitData)) {
	return false;
  }

  // Inicializacion del boton a anterior retrato
  pParser->SetVarPrefix("PlayerPortrait.Options.PrevButton.");
  TLoadOptionButton(ButtonInitData,
					CGUIWCharacterCreator::ID_PREVPORTRAITBUTTON,
					pParser);
  if (!m_GenreInfo.PrevButton.Init(ButtonInitData)) {
	return false;
  } 
  
  // Se carga la posicion donde se hallan los retratos
  pParser->SetVarPrefix("PlayerPortrait.");
  m_GenreInfo.PortraitsDrawPos = pParser->ReadPosition();

  // Procede a cargar los retratos para cada tipo de genero  
  // Se obtiene parser
  CCBTEngineParser* const pPortraitParser = m_pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_PLAYER_PROFILES,
															        "[Portraits]");
  ASSERT(pPortraitParser);
  
  const word uwMaxGenres = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_GENRE);
  RulesDefs::CriatureGenre GenreIt = 0;
  for (; GenreIt < uwMaxGenres; ++GenreIt) {
	// Se estblece prefijo
	std::string szPrefix;	
	SYSEngine::PassToString(szPrefix, 
							"Genre[%s].",
							m_pGDBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_GENRE,
																		  GenreIt).c_str());
	pPortraitParser->SetVarPrefix(szPrefix);
	// Se inicializa iterador a ultimo nodo
	m_GenreInfo.EndPortraitsIt[GenreIt] = m_GenreInfo.PortraitsList[GenreIt].end();
	// Se navega por los retratos asociados al genero
	pPortraitParser->ResetParseFlag();
	bool bExit = false;
	byte ubPortraitIt = 0;		
	while (!bExit) {
	  // Se localiza la plantilla de animacion asociada
	  std::string szVar;
	  SYSEngine::PassToString(szVar, "Portrait[%u].", ubPortraitIt);
	  const std::string szAnimTemplate = pPortraitParser->ReadString(szVar + "AnimTemplate", false);	  
	  // ¿Se leyeron datos?
	  if (pPortraitParser->WasLastParseOk()) {
		// Si, se lee resto de informacion
		const std::string szATSprite = pPortraitParser->ReadString(szVar + "Player.AnimTemplate");
		const std::string szShadow = pPortraitParser->ReadString(szVar + "Player.ShadowImage", false);

		// Se crea nodo y se introduce en lista
		sNSpritePortrait* const pSpritePortrait = new sNSpritePortrait(szAnimTemplate, szATSprite, szShadow);
		ASSERT(pSpritePortrait);		  
		m_GenreInfo.PortraitsList[GenreIt].push_back(pSpritePortrait);
		
		// Se actualiza iterador a ultimo nodo de la lista
		if (m_GenreInfo.PortraitsList[GenreIt].end() == m_GenreInfo.EndPortraitsIt[GenreIt]) {
		  m_GenreInfo.EndPortraitsIt[GenreIt] = m_GenreInfo.PortraitsList[GenreIt].begin();
		} else {
		  ++m_GenreInfo.EndPortraitsIt[GenreIt];
		}

		// Se actualiza iterador a sig. retrato
		++ubPortraitIt;
	  } else {
		// No, se abandona
		bExit = true;
	  }
	}

	// Se comprueba si hay suficientes retratos por cada identificador
	ASSERT(!m_GenreInfo.PortraitsList[GenreIt].empty());
  }    
  
  // Todo correcto
  return true;
}  

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la informacion asociada a los componentes para la seleccion de 
//  tipos y clase del caracter.
// Parametros:
// - pParser. Parser a utilizar
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - El texto asociado al tipo y clase seleccionado se establecera cuando
//   se active el interfaz.
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWCharacterCreator::TLoadTypeClassInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Vbles
  CGUICLineText::sGUICLineTextInitData LineInitData; // Inicializacion de lineas
    
  // Datos de configuracion para tipo / opcion
  pParser->SetVarPrefix("Type.");
  LineInitData.szLine = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_INTERFAZ_PLAYERCREATION_TYPE);      
  TLoadLineOption(LineInitData,
				  CGUIWCharacterCreator::ID_TYPEOPTION,
				  pParser);
  std::string szNumTypes;
  if (!m_TypeClassInfo.TypeOption.Init(LineInitData)) { 
	return false;  }

  // Datos de configuracion para tipo / seleccion
  TLoadLineSelect(LineInitData,
				  "-",
				  CGUIWCharacterCreator::ID_TYPESELECT,
				  pParser);
  if (!m_TypeClassInfo.TypeSelect.Init(LineInitData)) { 
	return false;
  }

  // Se leen los datos del quad de fx de tipo / seleccion
  m_TypeClassInfo.FXTypeBack.RGBQuad = pParser->ReadRGBColor("Selection.FXBack.");
  m_TypeClassInfo.FXTypeBack.AlphaQuad = pParser->ReadAlpha("Selection.FXBack.");

  // Datos de configuracion para clase / opcion
  pParser->SetVarPrefix("Class.");
  LineInitData.szLine = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_INTERFAZ_PLAYERCREATION_CLASS);        
  TLoadLineOption(LineInitData,
				  CGUIWCharacterCreator::ID_CLASSOPTION,
				  pParser);
  if (!m_TypeClassInfo.ClassOption.Init(LineInitData)) { 
	return false;
  }

  // Datos de configuracion para clase / seleccion
  TLoadLineSelect(LineInitData,
				  "-",
				  CGUIWCharacterCreator::ID_CLASSSELECT,
				  pParser);
  if (!m_TypeClassInfo.ClassSelect.Init(LineInitData)) { 
	return false;
  }

  // Se leen los datos del quad de fx de clase / seleccion
  m_TypeClassInfo.FXClassBack.RGBQuad = pParser->ReadRGBColor("Selection.FXBack.");
  m_TypeClassInfo.FXClassBack.AlphaQuad = pParser->ReadAlpha("Selection.FXBack.");

  // Se desvinculan los iteradores a tipo y clase
  m_TypeClassInfo.ActTypeInfoIt = m_ProfileInfo.TypeInfo.end();
  m_TypeClassInfo.ActClassInfoIt = m_ProfileInfo.TypeInfo.front()->ClassInfo.end();

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de cargar la informacion relativa a los atributos.
// Parametros:
// - pParser. Parser a utilizar
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - La musica de fondo NO sera obligatoria
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWCharacterCreator::TLoadAttributesInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);
  
  // Constantes asociada a colores globales
  pParser->SetVarPrefix("Attributes.");
  const GraphDefs::sRGBColor RGBNameSelect = pParser->ReadRGBColor("DrawFormat.Names.Select.");
  const GraphDefs::sRGBColor RGBNameUnselect = pParser->ReadRGBColor("DrawFormat.Names.Unselect.");  
  m_AttributesInfo.RGBBaseValue = pParser->ReadRGBColor("DrawFormat.BaseValues.");  
  m_AttributesInfo.FXBack.RGBQuad = pParser->ReadRGBColor("DrawFormat.BaseValues.FXBack.");
  m_AttributesInfo.FXBack.AlphaQuad = pParser->ReadAlpha("DrawFormat.BaseValues.FXBack.");
  m_AttributesInfo.RGBImproveValue = pParser->ReadRGBColor("DrawFormat.ImproveValues.");  
  
  // Vbles
  CGUICLineText::sGUICLineTextInitData           InitLine;   // Init de lineas
  CGUICPercentageBar::sGUICPercentageBarInitData InitPBar;   // Init de barras de porcentaje
  CGUICButton::sGUICButtonInitData               InitButton; // Init de botones
  
  // Componente de puntos distribuibles (el texto se asociara al inicializar)
  pParser->SetVarPrefix("Attributes.");
  InitLine.szLine = "-";
  InitLine.Position = pParser->ReadPosition("DistributionPoints.");
  InitLine.ID = CGUIWCharacterCreator::ID_DISTRIBUTEPOINTS;
  InitLine.ubDrawPlane = 0;
  InitLine.GUIEvents = GUIDefs::GUIC_NO_EVENT;
  InitLine.pGUIWindow = NULL;
  InitLine.RGBUnselectColor = pParser->ReadRGBColor("DrawFormat.DistributionPoints.");
  InitLine.RGBSelectColor = InitLine.RGBUnselectColor;
  InitLine.szFont = "Arial";
  if (!m_AttributesInfo.ImprovePoints.Init(InitLine)) {
	return false;
  }

  // Se leen datos asociados a la barra de porcentaje y se inicializa
  pParser->SetVarPrefix("Attributes.DistributionPoints.");
  TLoadPercentBar(InitPBar, pParser);
  if (!m_AttributesInfo.ImprovePointsPBar.Init(InitPBar)) {
	return false;
  }

  // Boton de incremento
  pParser->SetVarPrefix("Attributes.Options.IncrementValueButton.");
  TLoadOptionButton(InitButton, 
					CGUIWCharacterCreator::ID_INCATTRIBPOINTS, 
					pParser);
  if (!m_AttributesInfo.IncButton.Init(InitButton)) {
	return false;
  }

  // Boton de decremento
  pParser->SetVarPrefix("Attributes.Options.DecrementValueButton.");
  TLoadOptionButton(InitButton, 
					CGUIWCharacterCreator::ID_DECATTRIBPOINTS, 
					pParser); 
  if (!m_AttributesInfo.DecButton.Init(InitButton)) {
	return false;
  }

  // Inicializacion de atributos
  // Constante con los eventos de seleccion
  const GUIDefs::GUIEvents Events = GUIDefs::GUIC_SELECT |
								    GUIDefs::GUIC_UNSELECT |
								    GUIDefs::GUIC_BUTTONLEFT_PRESSED;

  // Se establecen valores base para las estructuras de inicializacion
  CGUICLineText::sGUICLineTextInitData AttribInit; // Nombre atributo
  CGUICLineText::sGUICLineTextInitData ValueInit;  // Valor base del atributo

  // Atributo
  AttribInit.GUIEvents = GUIDefs::GUIC_SELECT |
						 GUIDefs::GUIC_UNSELECT |
						 GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  AttribInit.pGUIWindow = this;
  AttribInit.RGBUnselectColor = RGBNameUnselect;
  AttribInit.RGBSelectColor = RGBNameSelect;
  AttribInit.szFont = "Arial";

  // Valor (la cantidad se asociara al activar)
  ValueInit.szLine = "-";
  ValueInit.ID = CGUIWCharacterCreator::ID_ATTRIBVALUE;
  ValueInit.RGBUnselectColor = m_AttributesInfo.RGBBaseValue;
  ValueInit.RGBSelectColor = m_AttributesInfo.RGBBaseValue;
  ValueInit.szFont = "Arial";

  // Salud
  pParser->SetVarPrefix("Attributes.Health.");
  // Nombre atributo
  AttribInit.szLine = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_HEALTH);
  AttribInit.Position = pParser->ReadPosition("Name.");
  AttribInit.ID = CGUIWCharacterCreator::ID_HEALTH;
  if (!m_AttributesInfo.Health.AttribName.Init(AttribInit)) {
	return false;
  }
  // Valor base asociado
  ValueInit.Position = pParser->ReadPosition("BaseValue.");
  if (!m_AttributesInfo.Health.AttribValue.Init(ValueInit)) {
	return false;
  }  

  // Puntos de combate
  pParser->SetVarPrefix("Attributes.CombatPoints.");
  // Nombre atributo
  AttribInit.szLine = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_COMBATPOINTS);
  AttribInit.Position = pParser->ReadPosition("Name.");
  AttribInit.ID = CGUIWCharacterCreator::ID_COMBATPOINTS;
  if (!m_AttributesInfo.CombatPoints.AttribName.Init(AttribInit)) {
	return false;
  }
  // Valor base asociado
  ValueInit.Position = pParser->ReadPosition("BaseValue.");
  if (!m_AttributesInfo.CombatPoints.AttribValue.Init(ValueInit)) {
	return false;
  }

  // Atributos extendidos
  const word uwMaxExtAttribs = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB);
  byte ubIt = 0;
  for (; ubIt < uwMaxExtAttribs; ++ubIt) {
	// Se establece prefijo
	std::string szPrefix;
	SYSEngine::PassToString(szPrefix,
						    "Attributes.ExtendedAttribute[%s].",
							m_pGDBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_EXTATTRIB,
																		  ubIt).c_str());
	pParser->SetVarPrefix(szPrefix);
	
	// Nombre atributo
	AttribInit.szLine = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_EXT_ATTRIB, 
												 ubIt);
	AttribInit.Position = pParser->ReadPosition("Name.");
	AttribInit.ID = CGUIWCharacterCreator::ID_EXATTRIB_00 + ubIt;
	if (!m_AttributesInfo.ExtAttribs[ubIt].AttribName.Init(AttribInit)) {
	  return false;
	}
	// Valor base asociado
	ValueInit.Position = pParser->ReadPosition("BaseValue.");
	if (!m_AttributesInfo.ExtAttribs[ubIt].AttribValue.Init(ValueInit)) {
	  return false;
	}
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la informacion asociada a las habilidades.
// Parametros:
// - pParser. Parser a utilizar, con la seccion situada.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWCharacterCreator::TLoadHabilitiesInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Obtiene datos de inicializacion para quads de fx
  pParser->SetVarPrefix("Habilities.DrawFormat.Names."); 
  const GraphDefs::sRGBColor FXColor = pParser->ReadRGBColor("FXBack.");
  const GraphDefs::sAlphaValue FXAlpha = pParser->ReadAlpha("FXBack.");
 
  // Vbles
  CGUICButton::sGUICButtonInitData     HabilityIconInit; // Init. icono de habilidad
  CGUICLineText::sGUICLineTextInitData HabilityNameInit; // Init nombre de habilidad
  std::string                          szTmp;            // Texto temporal

  // Prepara valores globales para estructuras de inicializacion
  // Iconos de habilidad
  HabilityIconInit.ID = CGUIWCharacterCreator::ID_HABILITYICON;
  HabilityIconInit.ubDrawPlane = 0;  
  // Nombres de habilidad
  HabilityNameInit.pGUIWindow = this;
  HabilityNameInit.GUIEvents = GUIDefs::GUIC_SELECT |
							   GUIDefs::GUIC_UNSELECT |
							   GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  HabilityNameInit.RGBSelectColor = pParser->ReadRGBColor("Select.");
  HabilityNameInit.RGBUnselectColor = pParser->ReadRGBColor("Unselect.");
  HabilityNameInit.szFont = "Arial";

  // Se procede a inicializar habilidades
  const word uwMaxHabilities = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);  
  byte ubIt = 0;
  for (; ubIt < uwMaxHabilities; ++ubIt) {
	// Se establece prefijo
	SYSEngine::PassToString(szTmp,
						    "Habilities.Hability[%s].",
							m_pGDBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_HABILITY,
																		  ubIt).c_str());
	pParser->SetVarPrefix(szTmp);
	
	// Se inicializa icono de habilidad
	HabilityIconInit.szATButton = pParser->ReadString("Icon.AnimTemplate");
	HabilityIconInit.Position = pParser->ReadPosition("Icon.");
	if (!m_HabilitiesInfo.Habilities[ubIt].HabilityIcon.Init(HabilityIconInit)) {
	  return false;
	}
	
	// Se inicializa nombre del icono de habilidad
	HabilityNameInit.ID = CGUIWCharacterCreator::ID_HABILITY_00 + ubIt;
	HabilityNameInit.Position = pParser->ReadPosition("Name.");
	HabilityNameInit.szLine = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_HABILITY,
													   ubIt);
	if (!m_HabilitiesInfo.Habilities[ubIt].HabilityName.Init(HabilityNameInit)) {
	  return false;
	}

	// Se toman los datos asociados a los quads
	sPosition FXPos(HabilityNameInit.Position.swXPos - 5,
					HabilityNameInit.Position.swYPos - 1);
	m_HabilitiesInfo.Habilities[ubIt].FXBackName.AlphaQuad = FXAlpha;
	m_HabilitiesInfo.Habilities[ubIt].FXBackName.RGBQuad = FXColor;	
	m_HabilitiesInfo.Habilities[ubIt].FXBackName.Position = FXPos;	
  }

  // Se lee informacion sobre los puntos de reparto  
  CGUICLineText::sGUICLineTextInitData           DistPointsInit; // Ptos de distribuicion
  CGUICPercentageBar::sGUICPercentageBarInitData PBarInit;       // Barra de porcentaje

  // Puntos de reparto (el texto se asociara al inicializar)
  pParser->SetVarPrefix("Habilities.");
  DistPointsInit.ID = CGUIWCharacterCreator::ID_DISTRIBUTEPOINTS;
  DistPointsInit.ubDrawPlane = 0;
  DistPointsInit.szFont = "Arial";
  DistPointsInit.szLine = "-";
  DistPointsInit.RGBUnselectColor = pParser->ReadRGBColor("DrawFormat.DistributionPoints.");
  DistPointsInit.RGBSelectColor = DistPointsInit.RGBUnselectColor;
  DistPointsInit.Position = pParser->ReadPosition("DistributionPoints.");
  if (!m_HabilitiesInfo.HabDistributePoints.Init(DistPointsInit)) {
	return false;
  }

  // Barra de porcentaje asociada a los puntos de reparto
  // Los valores base y temporal verdaderos se asociaran al activar
  pParser->SetVarPrefix("Habilities.DistributionPoints.");
  TLoadPercentBar(PBarInit, pParser);    
  if (!m_HabilitiesInfo.HabDistributePointsPBar.Init(PBarInit)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la informacion comun asociada a los perfiles de jugador
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo correcto true. En caso de algun problema false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWCharacterCreator::TLoadPlayerCommonInfo(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
 
  // Obtiene parser al archivo con los perfiles
  CCBTEngineParser* const pParser = m_pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_PLAYER_PROFILES,
													        "[Common]");	
  if (!pParser) { 
    SYSEngine::FatalError("No se localizó la sección [Common] en archivo de perfiles de jugador.\n");
  } 
  
  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga datos relacionados con la musica de fondo
// Parametros:
// - pParser. Parser a utilizar
// Devuelve:
// Notas:
// - La musica de fondo NO sera obligatoria
//////////////////////////////////////////////////////////////////////////////
bool
CGUIWCharacterCreator::TLoadMusicInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Obtiene datos
  pParser->SetVarPrefix("MIDIMusic.");
  // Nombre del archivo musical
  m_BackMIDIInfo.szBackMIDIMusic = pParser->ReadString("FileName", false);
  // ¿Se leyo informacion?
  if (pParser->WasLastParseOk()) {
	// Si, se procede a leer flag de reproduccion en bucle.
	// Por defecto sera true
	const bool bPlayLoop = pParser->ReadFlag("LoopFlag", false);
	// Se establece el modo de reproduccion
	if (bPlayLoop || !pParser->WasLastParseOk()) {
	  m_BackMIDIInfo.PlayMode = AudioDefs::MIDI_PLAY_LOOP;
	} else {
	  m_BackMIDIInfo.PlayMode = AudioDefs::MIDI_PLAY_NORMAL;
	}
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga de disco los datos asociados a una linea de opcion. Una linea de
//   opcion es aquella que pulsada hace que se efectue algun tipo de accion.
// Parametros:
// - LineOption. Estructura donde se depositaran los datos de inicializacion
//   para la linea de opcion.
// - Identificador a asociar
// - pParser. Parser a utilizar. El parser debera de estar situado en
//   seccion y con el prefijo adecuado. Todas las lineas de opcion utilizaran
//   los mismos nombres para vbles.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::TLoadLineOption(CGUICLineText::sGUICLineTextInitData& InitLineOption, 
									   const GUIDefs::GUIIDComponent& IDComponent,
									   CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(IDComponent);
  ASSERT(pParser);

  // Se leen datos de disco
  // Color de seleccion / deseleccion
  InitLineOption.RGBSelectColor = pParser->ReadRGBColor("InfoText.Select.");  
  InitLineOption.RGBUnselectColor = pParser->ReadRGBColor("InfoText.Unselect.");
  InitLineOption.Position = pParser->ReadPosition("InfoText.");
  
  // Se completan datos
  InitLineOption.szFont = "Arial";
  InitLineOption.ID = IDComponent;
  InitLineOption.pGUIWindow = this;
  InitLineOption.GUIEvents = GUIDefs::GUIC_SELECT |
							 GUIDefs::GUIC_UNSELECT |
							 GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  InitLineOption.ubDrawPlane = 0;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga de disco los datos asociados a una linea de seleccion. Una linea de
//   seleccion es aquella que muestra una seleccion determinada, esto es, un
//   valor de texto sin mas.
// Parametros:
// - LineOption. Estructura donde se depositaran los datos de inicializacion
//   para la linea de opcion.
// - szText. Texto a asociar
// - Identificador a asociar
// - pParser. Parser a utilizar. El parser debera de estar situado en
//   seccion y con el prefijo adecuado. Todas las lineas de opcion utilizaran
//   los mismos nombres para vbles.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::TLoadLineSelect(CGUICLineText::sGUICLineTextInitData& InitLineOption, 
									   const std::string szText,
									   const GUIDefs::GUIIDComponent& IDComponent,
									   CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(IDComponent);
  ASSERT(pParser);

  // Se leen datos de disco
  // Palabra asociada
  InitLineOption.szLine = szText;
  // Color de seleccion / deseleccion
  InitLineOption.RGBUnselectColor = pParser->ReadRGBColor("Selection.");
  InitLineOption.RGBSelectColor = InitLineOption.RGBUnselectColor;
  InitLineOption.Position = pParser->ReadPosition("Selection.");
  
  // Se completan datos
  InitLineOption.szFont = "Arial";
  InitLineOption.ID = IDComponent;
  InitLineOption.pGUIWindow = NULL;
  InitLineOption.GUIEvents = GUIDefs::GUIC_NO_EVENT;
  InitLineOption.ubDrawPlane = 0;  
} 

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga de disco los datos asociados a un boton de opcion.
// Parametros:
// - InitButton. Estructura donde se depositaran los datos de inicializacion
// - Identificador a asociar
// - pParser. Parser a utilizar. El parser debera de estar situado en
//   seccion y con el prefijo adecuado. Todas las lineas de opcion utilizaran
//   los mismos nombres para vbles.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWCharacterCreator::TLoadOptionButton(CGUICButton::sGUICButtonInitData& InitButton,
										 const GUIDefs::GUIIDComponent& IDComponent,
										 CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(IDComponent);
  ASSERT(pParser);

  // Inicializacion del boton a sig. retrato
  InitButton.szATButton = pParser->ReadString("AnimTemplate");
  InitButton.Position = pParser->ReadPosition();

  // Se completan datos
  InitButton.ID = IDComponent;
  InitButton.pGUIWindow = this;
  InitButton.ubDrawPlane = 0;
  InitButton.GUIEvents = GUIDefs::GUIC_SELECT |
						 GUIDefs::GUIC_UNSELECT |
						 GUIDefs::GUIC_BUTTONLEFT_PRESSED;
}   

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga desde disco datos de inicializacion de una barra de porcentaje.
// Parametros:
// - InitPercentBar. Donde depositar los datos
// - pParser. Parser a utilizar con seccion y prefijos situados
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::TLoadPercentBar(CGUICPercentageBar::sGUICPercentageBarInitData& InitPercentBar,
									   CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Obtiene datos
  InitPercentBar.Position = pParser->ReadPosition("PercentageBar.");
  InitPercentBar.bDrawHoriz = pParser->ReadFlag("PercentageBar.HorizontalDrawFlag");
  InitPercentBar.uwLenght = pParser->ReadNumeric("PercentageBar.Lenght");
  InitPercentBar.uwWidth = pParser->ReadNumeric("PercentageBar.Width");
  InitPercentBar.Alpha = pParser->ReadAlpha("PercentageBar.");  
  InitPercentBar.swBaseValue = 1;
  InitPercentBar.swTempValue = 0;

  // Se completan datos
  InitPercentBar.ID = CGUIWCharacterCreator::ID_PERCENTAGEBAR;
  InitPercentBar.ubDrawPlane = 0;  
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la operacion de inicializacion de un quad atendiendo a la
//   longitud del texto recibido y asociando a dicho quad, las direcciones
//   de los componentes RGB y Alpha anteriormente cargados.
// Parametros:
// - FXBack. Estructura donde depositar la informacion del color y alpha.
// - szText. Texto con el que hallar la anchura del quad.
// Devuelve:
// Notas:
// - Se supondra que el parser esta perfectamente localizado
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::InitFXBack(sFXBack& FXBack, 
								  const std::string& szText)
{
  // SOLO si parametros validos
  ASSERT(szText.size());

  // Se obtiene dimension del texto
  word uwWidth;
  word uwHeight;
  SYSEngine::GetFontSystem()->GetTextDim(szText, "Arial", uwWidth, uwHeight);

  // Se reinicializa FXBack
  FXBack.FXBack.Init(FXBack.Position, 
					 GraphDefs::DZ_GUI, 
					 uwWidth + 10, 
					 uwHeight, 
					 FXBack.RGBQuad, 
					 FXBack.AlphaQuad);
  ASSERT(FXBack.FXBack.IsInitOk());
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CGUIWCharacterCreator::End> Finalizando interfaz para la creación de personajes.\n");
	#endif

	// Se desactiva si procede
	Deactive();	
	
	// Finaliza componentes e informacion
	EndPlayerProfileInfo();
	EndBackImg();
	EndCancelInfo();
	EndAcceptInfo();
	EndNameInfo();
	EndGenreInfo();		
	EndTypeClassInfo();	
	EndAttributesInfo();
	EndHabilitiesInfo();

	// Se propaga finalizacion
	Inherited::End();
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("                           | Ok.\n");
	#endif
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera de memoria toda la informacion almacenada sobre los perfiles
//   de jugador creables.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWCharacterCreator::EndPlayerProfileInfo(void)
{
  // Navega entre los tipos de perfiles
  ProfileTypeInfoListIt TypeIt = m_ProfileInfo.TypeInfo.begin();
  while (TypeIt != m_ProfileInfo.TypeInfo.end()) {
	// Navega entre las clases asociados a los perfiles de tipo registrados
	ProfileClassInfoListIt ClassIt = (*TypeIt)->ClassInfo.begin();
	while (ClassIt != (*TypeIt)->ClassInfo.end()) {
	  // Borra nodo de clase de memoria y de lista
	  delete *ClassIt;
	  ClassIt = (*TypeIt)->ClassInfo.erase(ClassIt);
	}
	// Borra nodo de tipo de memoria y de lista
	delete *TypeIt;
	TypeIt = m_ProfileInfo.TypeInfo.erase(TypeIt);
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza imagen de fondo
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::EndBackImg(void)
{
  // Finaliza
  m_BackImgInfo.BackImg.End();	
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza componente de cancelacion
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::EndCancelInfo(void)
{
  // Finaliza
  m_CancelInfo.Button.End();	
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza componente de aceptacion
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::EndAcceptInfo(void)
{
  // Finaliza
  m_AcceptInfo.Button.End();	
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza componentes relacionados con el nombre
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::EndNameInfo(void)
{
  // Finaliza
  m_NameInputInfo.NameText.End();
  m_NameInputInfo.NameInput.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza componentes relacionados con la seleccion del genero
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::EndGenreInfo(void)
{
  // Finaliza componentes
  m_GenreInfo.GenreOption.End();
  m_GenreInfo.GenreSelect.End();
  m_GenreInfo.NextButton.End();
  m_GenreInfo.PrevButton.End();  

  // Libera lista de retratos
  // Navega entre las listas de retratos
  RulesDefs::CriatureGenre ActGenre = 0;
  for (; ActGenre < RulesDefs::MAX_CRIATURE_GENRES; ++ActGenre) {
	// Navega por los retratos asociados, liberandolos de la lista y memoria
	SpritePortraitListIt It = m_GenreInfo.PortraitsList[ActGenre].begin();
	while (It != m_GenreInfo.PortraitsList[ActGenre].end()) {	  
	  delete *It;
	  It = m_GenreInfo.PortraitsList[ActGenre].erase(It);
	}

	// Desvincula iteradores a ultima posicion de la lista
	m_GenreInfo.EndPortraitsIt[ActGenre] = m_GenreInfo.PortraitsList[ActGenre].end();
  }

  // Desvincula iterador a lista actual
  m_GenreInfo.ItPortrait = m_GenreInfo.PortraitsList[0].end();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza los componentes asociados al tipo y clase.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWCharacterCreator::EndTypeClassInfo(void)
{
  // Finaliza componentes
  m_TypeClassInfo.TypeOption.End();
  m_TypeClassInfo.TypeSelect.End();
  m_TypeClassInfo.ClassOption.End();
  m_TypeClassInfo.ClassSelect.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza los componentes asociados a las habilidades
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWCharacterCreator::EndAttributesInfo(void)
{
  // Finaliza componentes
  m_AttributesInfo.ImprovePoints.End();
  m_AttributesInfo.Health.AttribName.End();
  m_AttributesInfo.Health.AttribValue.End();
  m_AttributesInfo.CombatPoints.AttribName.End();
  m_AttributesInfo.CombatPoints.AttribValue.End();
  const word uwMaxExtAttribs = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB);
  byte ubIt = 0;
  for (; ubIt < uwMaxExtAttribs; ++ubIt) {
	m_AttributesInfo.ExtAttribs[ubIt].AttribName.End();
	m_AttributesInfo.ExtAttribs[ubIt].AttribValue.End();
  }
  m_AttributesInfo.DecButton.End();
  m_AttributesInfo.IncButton.End();
  m_AttributesInfo.ImprovePointsPBar.End();

  // Desvincula punteros
  m_AttributesInfo.ActAttribute.pAttribute = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza componentes asociados a la seleccion de habiliades
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWCharacterCreator::EndHabilitiesInfo(void)
{
  // Finaliza componentes asociados a las habilidades
  const word uwMaxHabilities = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);  
  byte ubIt = 0;
  for (; ubIt < uwMaxHabilities; ++ubIt) {
	m_HabilitiesInfo.Habilities[ubIt].HabilityIcon.End();
	m_HabilitiesInfo.Habilities[ubIt].HabilityName.End();
	m_HabilitiesInfo.Habilities[ubIt].FXBackName.FXBack.End();
  }
  m_HabilitiesInfo.HabDistributePointsPBar.End();
  m_HabilitiesInfo.HabDistributePoints.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Instala el comando de fade para iniciar el interfaz. Cuando finalice,
//   sera ejecutado el metodo de activacion propiamente dicho.
// - Todos los componentes seran activados.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::Active(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se activa si procede
  if (!Inherited::IsActive()) {
    // Se indica que no hay ningun texto flotante activo
    m_FloatingTextInfo.ActIDFloatingText = 0;
	
	// Se indica que no hay atributo establecido
	m_AttributesInfo.IDAttributeSet = 0;

	// Se vincula iterador al conjunto de retratos actuales y se establece texto  
	m_GenreInfo.ActGenre = 0;
	NextGenre();

	// Se establecen los iteradores al tipo y clase actual  
	m_TypeClassInfo.ActTypeInfoIt = m_ProfileInfo.TypeInfo.end();
	NextType();

	// Se bloquea la entrada y esconde cursor
	// Nota: Se realizara antes de activar componentes para evitar eventos
	SetInput(false);  
	m_pGUIManager->SetGUICursor(GUIManagerDefs::NO_CURSOR);  
	
	// Se activan los componentes
	// Nota: Es necesario activar los componentes una vez que se han
	// establecido los iteradores a tipo y genero.
	SetComponentsActive(true);

	// Reproduce musica de fondo si procede
	if (m_BackMIDIInfo.szBackMIDIMusic.size()) {
	  SYSEngine::GetAudioSystem()->PlayMIDIMusic(m_BackMIDIInfo.szBackMIDIMusic,
												 m_BackMIDIInfo.PlayMode);
	}

	// Se realiza el fade in
	Inherited::DoFadeIn(CGUIWCharacterCreator::ID_ENDSTARTFADE,
						 m_FadeInfo.StartFade.uwSpeed);

	// Se propaga activacion
	Inherited::Active();
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva componentes
// - Desasocia cualquier tipo de cursor de GUI
// - Detiene musica MIDI si procede
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Desactiva si procede
  if (Inherited::IsActive()) {
	// Se desactivan los componentes
	SetComponentsActive(false);

	// Se descargan retratos actuales y se desvincula iterador
	EndPortraitSet(m_GenreInfo.ActGenre);
	m_GenreInfo.ItPortrait = m_GenreInfo.PortraitsList[m_GenreInfo.ActGenre].end();    

	// Quita cursores de GUI
	m_pGUIManager->SetGUICursor(GUIManagerDefs::NO_CURSOR);
  
	// Detiene musica MIDI si procede
	if (m_BackMIDIInfo.szBackMIDIMusic.size()) {
	  SYSEngine::GetAudioSystem()->StopMIDIMusic();
	}

	// Propaga
	Inherited::Deactive();
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa / desactiva todos los componentes asociados a la ventana de
//   interfaz.
// Parametros:
// - bActive. Flag de activacion / finalizacion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::SetComponentsActive(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se activan / desactivan los componentes
  // Imagen de fondo
  m_BackImgInfo.BackImg.SetActive(bActive);

  // Cancelacion
  m_CancelInfo.Button.SetActive(bActive);

  // Aceptacion
  m_AcceptInfo.Button.SetActive(bActive);

  // Nombre
  m_NameInputInfo.NameText.SetActive(bActive);
  m_NameInputInfo.NameInput.SetActive(bActive); 

  // Genero
  m_GenreInfo.GenreOption.SetActive(bActive);
  m_GenreInfo.GenreSelect.SetActive(bActive);
  m_GenreInfo.NextButton.SetActive(bActive);
  m_GenreInfo.PrevButton.SetActive(bActive);  

  // Tipo y clase
  m_TypeClassInfo.TypeOption.SetActive(bActive);
  m_TypeClassInfo.TypeSelect.SetActive(bActive);
  m_TypeClassInfo.ClassOption.SetActive(bActive);
  m_TypeClassInfo.ClassSelect.SetActive(bActive);

  // Atributos
  m_AttributesInfo.ImprovePoints.SetActive(bActive);
  m_AttributesInfo.ImprovePointsPBar.SetActive(bActive);
  m_AttributesInfo.Health.AttribName.SetActive(bActive);
  m_AttributesInfo.Health.AttribValue.SetActive(bActive);
  m_AttributesInfo.CombatPoints.AttribName.SetActive(bActive);
  m_AttributesInfo.CombatPoints.AttribValue.SetActive(bActive);
  const word uwMaxExtAttribs = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB);
  byte ubIt = 0;
  for (; ubIt < uwMaxExtAttribs; ++ubIt) {
	m_AttributesInfo.ExtAttribs[ubIt].AttribName.SetActive(bActive);
	m_AttributesInfo.ExtAttribs[ubIt].AttribValue.SetActive(bActive);
  }
  m_AttributesInfo.DecButton.SetActive(bActive);
  m_AttributesInfo.IncButton.SetActive(bActive);
  
  // Habilidades  
  const word uwMaxHabilities = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);  
  ubIt = 0;
  for (; ubIt < uwMaxHabilities; ++ubIt) {
	m_HabilitiesInfo.Habilities[ubIt].HabilityIcon.SetActive(bActive);
	m_HabilitiesInfo.Habilities[ubIt].HabilityName.SetActive(bActive);
  }
  m_HabilitiesInfo.HabDistributePointsPBar.SetActive(bActive);
  m_HabilitiesInfo.HabDistributePoints.SetActive(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la notificacion de eventos relacionados con componentes del
//   interfaz.
// Parametros:
// - GUICEvent. Evento recibido.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // Comprueba el tipo de evento y se propaga al metodo adecuado
  switch (GUICEvent.Event) {
	case GUIDefs::GUIC_SELECT: {	  
	  OnSelectNotify(GUICEvent.IDComponent, true);
	} break;

	case GUIDefs::GUIC_UNSELECT: {
	  OnSelectNotify(GUICEvent.IDComponent, false);
	} break;

	case GUIDefs::GUIC_BUTTONLEFT_PRESSED: {
	  OnLeftClickNotify(GUICEvent.IDComponent);
	} break;

	case GUIDefs::GUIC_INPUT_ACCEPT_ACQUIRING:
	case GUIDefs::GUIC_INPUT_NOACCEPT_ACQUIRING: {
	  OnEndNameInput();
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la notificacion de seleccion / deseleccion de un componente.
// Parametros:
// - IDComponent. ID del componente.
// - bSelect. Flag de seleccion / deseleccion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::OnSelectNotify(const GUIDefs::GUIIDComponent& IDComponent,
									  const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Comprueba el tipo de componente seleccionado y actua en consecuencia
  switch (IDComponent) {
	case CGUIWCharacterCreator::ID_CANCELBUTTON: {	  
	  // Seleccion del boton de cancelacion
	  m_CancelInfo.Button.Select(bSelect);	  
	} break;

	case CGUIWCharacterCreator::ID_ACCEPTBUTTON: {	  
	  // Seleccion del boton de aceptacion 
	  // Debe de haberse insertado al menos el nombre
	  if (CanAcceptCharacter()) {
		m_AcceptInfo.Button.Select(bSelect);
	  }
	} break;

	case CGUIWCharacterCreator::ID_NAMEOPTION: {
	  // Selecion del nombre. 
	  // Se seleccionara tambien el componente de entrada.
	  m_NameInputInfo.NameText.Select(bSelect);
	  m_NameInputInfo.NameInput.Select(bSelect);
	} break;

	case CGUIWCharacterCreator::ID_NEXTPORTRAITBUTTON: {
	  // Selecciona boton de avance al sig. retrato
	  m_GenreInfo.NextButton.Select(bSelect);
	} break;

	case CGUIWCharacterCreator::ID_PREVPORTRAITBUTTON: {
	  // Selecciona boton de avance al anterior retrato
	  m_GenreInfo.PrevButton.Select(bSelect);
	} break;

	case CGUIWCharacterCreator::ID_GENREOPTION: {
	  // Selecciona texto de genero para cambiarlo
	  m_GenreInfo.GenreOption.Select(bSelect);
	} break;

	case CGUIWCharacterCreator::ID_TYPEOPTION: {
	  // Selecciona texto de tipo para cambiarlo
	  m_TypeClassInfo.TypeOption.Select(bSelect);
	} break;

	case CGUIWCharacterCreator::ID_CLASSOPTION: {
	  // Selecciona texto de clase para cambiarlo
	  m_TypeClassInfo.ClassOption.Select(bSelect);
	} break;

	case CGUIWCharacterCreator::ID_INCATTRIBPOINTS: {
	  // Boton de incremento de valor en atributo
	  m_AttributesInfo.IncButton.Select(bSelect);
	} break;

	case CGUIWCharacterCreator::ID_DECATTRIBPOINTS: {
	  // Boton de decremento de valor en atributo
	  m_AttributesInfo.DecButton.Select(bSelect);
	} break;

	case CGUIWCharacterCreator::ID_HEALTH: {
	  // Atributo de salud
	  // ¿Se puede incrementar?
	  if ((*m_TypeClassInfo.ActClassInfoIt)->HealthInfo.uwCost) {
		// Se selecciona / deselecciona y establece / quita el texto flotante
		if (!bSelect) {
		  if (IDComponent != m_AttributesInfo.ActAttribute.IDAttribute) {
			m_AttributesInfo.Health.AttribName.Select(bSelect);			
		  }
		} else {
		  m_AttributesInfo.Health.AttribName.Select(bSelect);
		}		
		SetFloatingText(IDComponent, bSelect);
	  }
	} break;

	case CGUIWCharacterCreator::ID_COMBATPOINTS: {
	  // Atributo de puntos de combate	  
	  // ¿Se puede incrementar?
	  if ((*m_TypeClassInfo.ActClassInfoIt)->CombatPointsInfo.uwCost) {
		// Se selecciona / deselecciona y establece / quita el texto flotante		
		if (!bSelect) {
		  if (IDComponent != m_AttributesInfo.ActAttribute.IDAttribute) {
			m_AttributesInfo.CombatPoints.AttribName.Select(bSelect);		
		  }
		} else {
		  m_AttributesInfo.CombatPoints.AttribName.Select(bSelect);		
		}		
		SetFloatingText(IDComponent, bSelect);
	  }
	} break;

	case CGUIWCharacterCreator::ID_EXATTRIB_00: 
	case CGUIWCharacterCreator::ID_EXATTRIB_01: 
	case CGUIWCharacterCreator::ID_EXATTRIB_02: 
	case CGUIWCharacterCreator::ID_EXATTRIB_03:
	case CGUIWCharacterCreator::ID_EXATTRIB_04: 
	case CGUIWCharacterCreator::ID_EXATTRIB_05: 
	case CGUIWCharacterCreator::ID_EXATTRIB_06: 
	case CGUIWCharacterCreator::ID_EXATTRIB_07: {
	  // Atributo extendido
	  const byte ubIt = IDComponent - CGUIWCharacterCreator::ID_EXATTRIB_00;	  
	  // ¿Se puede incrementar?
	  if ((*m_TypeClassInfo.ActClassInfoIt)->ExtAttribsInfo[ubIt].uwCost) {
		// Se selecciona / deselecciona y establece / quita el texto flotante
		if (!bSelect) {
		  if (IDComponent != m_AttributesInfo.ActAttribute.IDAttribute) {
			m_AttributesInfo.ExtAttribs[ubIt].AttribName.Select(bSelect);		
		  }
		} else {
		  m_AttributesInfo.ExtAttribs[ubIt].AttribName.Select(bSelect);		
		}				
		SetFloatingText(IDComponent, bSelect);
	  }
	} break;

	case CGUIWCharacterCreator::ID_HABILITY_00: 
	case CGUIWCharacterCreator::ID_HABILITY_01:
	case CGUIWCharacterCreator::ID_HABILITY_02:
	case CGUIWCharacterCreator::ID_HABILITY_03:
	case CGUIWCharacterCreator::ID_HABILITY_04:
	case CGUIWCharacterCreator::ID_HABILITY_05: 
	case CGUIWCharacterCreator::ID_HABILITY_06: 
	case CGUIWCharacterCreator::ID_HABILITY_07: {
	  // Habilidad
	  const byte ubIt = IDComponent - CGUIWCharacterCreator::ID_HABILITY_00;
	  if ((*m_TypeClassInfo.ActClassInfoIt)->HabilitiesInfo[ubIt].uwCost) {
		// Se selecciona / deselecciona y establece / quita el texto flotante
		m_HabilitiesInfo.Habilities[ubIt].HabilityIcon.Select(bSelect);
		m_HabilitiesInfo.Habilities[ubIt].HabilityName.Select(bSelect);		
		SetFloatingText(IDComponent, bSelect);
	  }
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la notificacion de pulsacion del boton izquierdo sobre
//   un componente.
// Parametros:
// - IDComponent. ID del componente.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::OnLeftClickNotify(const GUIDefs::GUIIDComponent& IDComponent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Comprueba el tipo de componente seleccionado y actua en consecuencia
  switch (IDComponent) {
	case CGUIWCharacterCreator::ID_CANCELBUTTON: {	  	  
	  // Se realiza el fade out, se bloquea entrada y se esconde cursor
	  Inherited::DoFadeOut(CGUIWCharacterCreator::ID_ENDCANCELFADE,
						   m_FadeInfo.CancelFade.RGBColor,
						   m_FadeInfo.CancelFade.uwSpeed);
	  SetInput(false);  
	  m_pGUIManager->SetGUICursor(GUIManagerDefs::NO_CURSOR);  	  
	} break;

	case CGUIWCharacterCreator::ID_ACCEPTBUTTON: {	  
	  // Se realiza el fade in, se bloquea entrada y se esconde cursor
	  Inherited::DoFadeOut(CGUIWCharacterCreator::ID_ENDACCEPTFADE,
						   m_FadeInfo.AcceptFade.RGBColor,
						   m_FadeInfo.AcceptFade.uwSpeed);
	  SetInput(false);  
	  m_pGUIManager->SetGUICursor(GUIManagerDefs::NO_CURSOR);  	  
	} break;

	case CGUIWCharacterCreator::ID_NAMEOPTION: {	  
	  // Se bloquea entrada para todos los componentes salvo input
	  SetInput(false);
	  m_NameInputInfo.NameInput.SetInput(true);
	  m_NameInputInfo.NameInput.AcquireInput(true);	  
	  ASSERT(m_NameInputInfo.NameInput.IsAcquiringInput());	  
	  // Se ocultara cursor de GUI
	  m_pGUIManager->SetGUICursor(GUIManagerDefs::NO_CURSOR);
	  // Se toma la posicion del raton
	  m_NameInputInfo.LastMousePos = m_pInputManager->GetLastMousePos();
	} break;

  	case CGUIWCharacterCreator::ID_NEXTPORTRAITBUTTON: {
	  // Avance de retrato
	  NextPortrait();
	} break;

	case CGUIWCharacterCreator::ID_PREVPORTRAITBUTTON: {
	  // Retroceso de retrato
	  PrevPortrait();
	} break;

	case CGUIWCharacterCreator::ID_GENREOPTION: {	  
	  // Sig. genero
	  NextGenre();
	} break;

	case CGUIWCharacterCreator::ID_TYPEOPTION: {
	  // Pasa de forma circular el tipo de la criatura
	  NextType();
	} break;

	case CGUIWCharacterCreator::ID_CLASSOPTION: {
	  // Pasa de forma circular las clases asociadas al tipo de la criatura
	  NextClass();
	} break;

 	case CGUIWCharacterCreator::ID_INCATTRIBPOINTS: {
	  // Boton de incremento de valor en atributo
	  IncAttributePoints();
	} break;

	case CGUIWCharacterCreator::ID_DECATTRIBPOINTS: {
	  // Boton de decremento de valor en atributo
	  DecAttributePoints();
	} break;

	case CGUIWCharacterCreator::ID_HEALTH: {
	  // Atributo de salud
	  SelectAttribute(IDComponent,
				      &m_AttributesInfo.Health,
					  (*m_TypeClassInfo.ActClassInfoIt)->HealthInfo.uwCost,
					  (*m_TypeClassInfo.ActClassInfoIt)->HealthInfo.swBaseValue); 	  
	} break;

	case CGUIWCharacterCreator::ID_COMBATPOINTS: {
	  // Atributo de puntos de combate
	  SelectAttribute(IDComponent,
				      &m_AttributesInfo.CombatPoints,
					  (*m_TypeClassInfo.ActClassInfoIt)->CombatPointsInfo.uwCost,
					  (*m_TypeClassInfo.ActClassInfoIt)->CombatPointsInfo.swBaseValue); 	  	  
	} break;

	case CGUIWCharacterCreator::ID_EXATTRIB_00: 
	case CGUIWCharacterCreator::ID_EXATTRIB_01: 
	case CGUIWCharacterCreator::ID_EXATTRIB_02: 
	case CGUIWCharacterCreator::ID_EXATTRIB_03:
	case CGUIWCharacterCreator::ID_EXATTRIB_04: 
	case CGUIWCharacterCreator::ID_EXATTRIB_05: 
	case CGUIWCharacterCreator::ID_EXATTRIB_06: 
	case CGUIWCharacterCreator::ID_EXATTRIB_07: {
	  // Atributo extendido
	  const byte ubIt = IDComponent - CGUIWCharacterCreator::ID_EXATTRIB_00;
	  SelectAttribute(IDComponent,
				      &m_AttributesInfo.ExtAttribs[ubIt],
					  (*m_TypeClassInfo.ActClassInfoIt)->ExtAttribsInfo[ubIt].uwCost,
					  (*m_TypeClassInfo.ActClassInfoIt)->ExtAttribsInfo[ubIt].swBaseValue);
	} break;

	case CGUIWCharacterCreator::ID_HABILITY_00: 
	case CGUIWCharacterCreator::ID_HABILITY_01:
	case CGUIWCharacterCreator::ID_HABILITY_02:
	case CGUIWCharacterCreator::ID_HABILITY_03:
	case CGUIWCharacterCreator::ID_HABILITY_04:
	case CGUIWCharacterCreator::ID_HABILITY_05: 
	case CGUIWCharacterCreator::ID_HABILITY_06: 
	case CGUIWCharacterCreator::ID_HABILITY_07: {
	  // Habilidad
	  const byte ubIt = IDComponent - CGUIWCharacterCreator::ID_HABILITY_00;
	  SelectHability(ubIt);
	} break;
  }; // ~ switch
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se utilizara para conocer cuando han finalizado los fades pertinentes y
//   asi realizar la accion que proceda.
// Parametros:
// - IDCommand. ID del comando.
// - udInstant. Instante de finalizacion.
// - udExtraParam. Parametro extra.
// Devuelve:
// - Si todo bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
									const dword udInstant,
									const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se comprueba codigo de fade recibido
  switch(IDCommand) {
	case CGUIWCharacterCreator::ID_ENDSTARTFADE: {
	  // Establece cursores de GUI
	  m_pGUIManager->SetGUICursor(GUIManagerDefs::WINDOWINTERFAZ_CURSOR);  
	  
	  // Se desbloquea entrada, pero dejando el componente de input a false
	  SetInput(true);
  	  m_NameInputInfo.NameInput.SetInput(false);	  
	} break;

	case CGUIWCharacterCreator::ID_ENDCANCELFADE: {
	  // Se simula deseleccion
	  OnSelectNotify(CGUIWCharacterCreator::ID_CANCELBUTTON, false);	  

	  // Se finaliza musica
	  SYSEngine::GetAudioSystem()->StopMIDIMusic();
	  
	  // Se cambia a estado de menu principal
	  SYSEngine::SetMainMenuState();
	} break;

	case CGUIWCharacterCreator::ID_ENDACCEPTFADE: {
  	  // Se simula deseleccion
	  OnSelectNotify(CGUIWCharacterCreator::ID_ACCEPTBUTTON, false);	  

	  // Se finaliza musica
	  SYSEngine::GetAudioSystem()->StopMIDIMusic();
	  
	  // Caracter aceptado
	  AcceptCharacter();
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la finalizacion de la entrada de datos sobre el 
//   componente de entrada.
// Parametros:
// Devuelve:
// Notas:
// - Se simula seleccion del nombre porque se situara el cursor de GUI en la
//   misma posicion en donde se realizo el "Click" sobre el nombre.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::OnEndNameInput(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se bloquea la entrada en el componente de entrada
  m_NameInputInfo.NameInput.SetInput(false);

  // Se desbloqueara entrada para todos los componentes salvo input
  SetInput(true);
  m_NameInputInfo.NameInput.SetInput(false);

  // Se simulara seleccion del nombre
  OnSelectNotify(CGUIWCharacterCreator::ID_NAMEOPTION, true);

  // Se mostrara cursor de GUI
  m_pGUIManager->SetGUICursor(GUIManagerDefs::WINDOWINTERFAZ_CURSOR);  
  
  // Se restaurara la anterior posicion del raton
  m_pInputManager->SetMousePos(m_NameInputInfo.LastMousePos);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Registra todos los textos flotantes asociados a la clase seleccionada.
//   Los textos estaran relacionados a informacion referida al coste en puntos
//   para establecer un atributo o una habilidad.
// Parametros:
// Devuelve:
// Notas:
// - Los handles seran guardados segun un par (IDComponente, handle) donde
//   el identificador del componente hara referencia justamente al
//   componente al que esta asociado.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::RegisterFloatingText(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Procede a registrar y mapear texto asociado atributos
  // Salud
  MapFloatingText(CGUIWCharacterCreator::ID_HEALTH, 
				  (*m_TypeClassInfo.ActClassInfoIt)->HealthInfo.uwCost);

  // Puntos de combate
  MapFloatingText(CGUIWCharacterCreator::ID_COMBATPOINTS, 
				  (*m_TypeClassInfo.ActClassInfoIt)->CombatPointsInfo.uwCost);

  // Atributos extendidos
  const word uwMaxExtAttributes = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB);
  byte ubIt = 0;
  for (; ubIt < uwMaxExtAttributes; ++ubIt) {
	MapFloatingText(CGUIWCharacterCreator::ID_EXATTRIB_00 + ubIt,
				    (*m_TypeClassInfo.ActClassInfoIt)->ExtAttribsInfo[ubIt].uwCost);
  }

  // Habilidades
  const word uwMaxHabilities  = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);
  ubIt = 0;
  for (; ubIt < uwMaxHabilities; ++ubIt) {
	MapFloatingText(CGUIWCharacterCreator::ID_HABILITY_00 + ubIt,
				    (*m_TypeClassInfo.ActClassInfoIt)->HabilitiesInfo[ubIt].uwCost);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Mapea o inserta en el map el texto flotante recibido. Antes realizara
//   el proceso de transformar el coste del mismo a texto y registrarlo en
//   el servidor de recursos.
// - El metodo se encargara de descartar el registrar cuando el coste 
//   recibido sea de 0.
// Parametros:
// - IDComponent. Identificador del componente que se utilizara como clave.
// - uwCost. Coste asociado al texto flotante.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWCharacterCreator::MapFloatingText(const GUIDefs::GUIIDComponent& IDComponent,
									   const word uwCost)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(IDComponent);
  
  // ¿Es necesario registrar?
  if (uwCost) {
	// Registra el texto y lo mapea
	std::string szText;
	SYSEngine::PassToString(szText, "(%up)", uwCost);  	
	m_FloatingTextInfo.TextMap.insert(FloatingTextMapValType(IDComponent, szText));
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera todos los handles de texto flotante registrados.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::ReleaseFloatingText(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Procede a liberar los handles asociados
  FloatingTextMapIt It(m_FloatingTextInfo.TextMap.begin());
  while (It != m_FloatingTextInfo.TextMap.end()) {	
	It = m_FloatingTextInfo.TextMap.erase(It);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece / quita el texto flotante el asociado a IDComponent. Al establecer
//   un texto flotante, se quitara SIEMPRE el que estuviera asociado.
// - A la hora de quitar el texto SOLO se quitara si el texto flotante
//   actual es igual a IDComponent. En caso contrario no hara nada. Esto es
//   necesario para mantener una sincronizacion a la hora de poner y quitar 
//   textos flotantes.
// Parametros:
// - IDComponente. Identificador del texto flotante a establecer, si es que
//   existe.
// - bSet. Flag de poner / quitar el texto
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::SetFloatingText(const GUIDefs::GUIIDComponent& IDComponent,
									   const bool bSet)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se obtiene el texto flotante 
  const FloatingTextMapIt It(m_FloatingTextInfo.TextMap.find(IDComponent));
	
  // ¿Se desea establecer?
  if (bSet) {
	// Si, Establece texto flotante si procede
	if (It != m_FloatingTextInfo.TextMap.end()) {
	  // Se establece y se guarda su identificador
	  m_FloatingTextInfo.ActIDFloatingText = IDComponent;
	  m_pGUIManager->SetFloatingText(It->second, 
									 m_FloatingTextInfo.Color,
									 m_FloatingTextInfo.Alpha,
									 GUIManagerDefs::DISSOLVE_FLOATINGTEXT);
	}
  } else {
	// No, pero ¿el identificador actual es IGUAL al recibido?  
	if (IDComponent == m_FloatingTextInfo.ActIDFloatingText) {
	  // Si, luego se puede quitar el texto flotante actual	  
	  if (It != m_FloatingTextInfo.TextMap.end()) {
		m_pGUIManager->UnsetFloatingText(It->second);	  
	  }
	  m_FloatingTextInfo.ActIDFloatingText = 0;
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Avanza al siguiente genero
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::NextGenre(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se descarga banco de retratos actual
  EndPortraitSet(m_GenreInfo.ActGenre);

  // Se cambia banco de retratos
  ++m_GenreInfo.ActGenre;
  if (RulesDefs::MAX_CRIATURE_GENRES == m_GenreInfo.ActGenre) {
	m_GenreInfo.ActGenre = 0;
  }
  InitPortraitSet(m_GenreInfo.ActGenre);
  
  // Se vincula iterador a banco de generos actuales
  m_GenreInfo.ItPortrait = m_GenreInfo.PortraitsList[m_GenreInfo.ActGenre].begin();
  
  // Se establece texto asociado al genero escogido	  
  const std::string szGenreText = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_GENRE, 
														   m_GenreInfo.ActGenre);
  m_GenreInfo.GenreSelect.ChangeText(szGenreText);

  // Se inicializa el quad de fondo  
  InitFXBack(m_GenreInfo.FXBack, szGenreText);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Avanza al siguiente retrato.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::NextPortrait(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Avanza de forma circular
  ++m_GenreInfo.ItPortrait;
  // ¿Se ha llegado al final?
  if (m_GenreInfo.PortraitsList[m_GenreInfo.ActGenre].end() == m_GenreInfo.ItPortrait) {
	// Se situa al comienzo
	m_GenreInfo.ItPortrait = m_GenreInfo.PortraitsList[m_GenreInfo.ActGenre].begin();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retrocede al retrato anterior
// Parametros:
// Devuelve:a
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::PrevPortrait(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Retrocede de forma circular
  if (m_GenreInfo.PortraitsList[m_GenreInfo.ActGenre].begin() == m_GenreInfo.ItPortrait) {
	// Situa el iterador en el ultimo nodo
	m_GenreInfo.ItPortrait = m_GenreInfo.EndPortraitsIt[m_GenreInfo.ActGenre];
  } else {
	// Retrocede el iterador
	--m_GenreInfo.ItPortrait;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Lleva a memoria el set de retratos asociado al genero cuyo identificador
//   es Genre.
// Parametros:
// - Genre. Identificador del genero sobre el que queremos llevar a memoria
//   los retratos.
// Devuelve:
// Notas:
// - En caso de que el primer elemento de la lista este inicializado, se
//   entendera que todos los retratos asociados a la misma tambien se hallan
//   inicializados y se saldra sin realizar accion alguna.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::InitPortraitSet(const RulesDefs::CriatureGenre& Genre)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT((Genre < RulesDefs::MAX_CRIATURE_GENRES) != 0);

  // ¿El primer nodo de la lista de generos NO esta inicializado?
  ASSERT(m_GenreInfo.PortraitsList[Genre].size());
  if (!m_GenreInfo.PortraitsList[Genre].front()->Portrait.IsInitOk()) {
	// Procede a inicializar retratos
	SpritePortraitListIt It = m_GenreInfo.PortraitsList[Genre].begin();
	for (; It != m_GenreInfo.PortraitsList[Genre].end(); ++It) {
	  (*It)->Portrait.Init((*It)->szAnimTemplate);
	  ASSERT((*It)->Portrait.IsInitOk());
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza todos los retratos asociados al genero Genre.
// Parametros:
// - Genre. Genero sobre el que actuar.
// Devuelve:
// Notas:
// - En caso de que el primer nodo de la lista NO ESTE inicializado, se
//   retornara pues se entendera que todos los demas retratos en la lista
//   tambien se hallan finalizados.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::EndPortraitSet(const RulesDefs::CriatureGenre& Genre)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT((Genre < RulesDefs::MAX_CRIATURE_GENRES) != 0);

  // ¿El primer nodo de la lista de generos esta inicializado?
  ASSERT(m_GenreInfo.PortraitsList[Genre].size());
  if (m_GenreInfo.PortraitsList[Genre].front()->Portrait.IsInitOk()) {
	// Procede a finalizar retratos
	SpritePortraitListIt It = m_GenreInfo.PortraitsList[Genre].begin();
	for (; It != m_GenreInfo.PortraitsList[Genre].end(); ++It) {
	  (*It)->Portrait.End();
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Pasa de forma circular el tipo de la criatura. 
// - Al pasar de tipo, se establecera un nuevo set de clases que seran las
//   asociadas a dicho tipo.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWCharacterCreator::NextType(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se avanza iterador a sig. tipo de forma circular
  // Nota: Sera necesaria la comprobacion end() != It que viene a continuacion,
  // para permitir la activacion de la ventana sin problemas por primera vez.
  ASSERT(m_ProfileInfo.TypeInfo.size());
  if (m_ProfileInfo.TypeInfo.end() != m_TypeClassInfo.ActTypeInfoIt) {
	++m_TypeClassInfo.ActTypeInfoIt;	
  }    
  if (m_ProfileInfo.TypeInfo.end() == m_TypeClassInfo.ActTypeInfoIt) {
	m_TypeClassInfo.ActTypeInfoIt = m_ProfileInfo.TypeInfo.begin();
  }  
  
  // Se establece el texto de seleccion para el tipo y clase
  const std::string szType = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_TYPE, 
													  (*m_TypeClassInfo.ActTypeInfoIt)->IdType);
  m_TypeClassInfo.TypeSelect.ChangeText(szType);

  // Establece posicion y quad de fx
  m_TypeClassInfo.FXTypeBack.Position = sPosition(m_TypeClassInfo.TypeSelect.GetXPos() - 5,
												  m_TypeClassInfo.TypeSelect.GetYPos() - 1);
  InitFXBack(m_TypeClassInfo.FXTypeBack, szType);
  
  // Se asocian clases
  m_TypeClassInfo.ActClassInfoIt = (*m_TypeClassInfo.ActTypeInfoIt)->ClassInfo.end();
  NextClass();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Pasa de forma circular la clase de la criatura, ajustando todos los
//   valores que esten supeditados como atributos y habilidades.
// - Desde este metodo se registraran tambien todos los textos flotantes
//   asociados a la propia clase para que esten listos para utilizarse.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWCharacterCreator::NextClass(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se avanza iterador a sig. tipo de forma circular
  ASSERT((m_ProfileInfo.TypeInfo.end() != m_TypeClassInfo.ActTypeInfoIt) != 0)
  ASSERT((*m_TypeClassInfo.ActTypeInfoIt)->ClassInfo.size());
  ++m_TypeClassInfo.ActClassInfoIt;
  if ((*m_TypeClassInfo.ActTypeInfoIt)->ClassInfo.end() == m_TypeClassInfo.ActClassInfoIt) {
	m_TypeClassInfo.ActClassInfoIt = (*m_TypeClassInfo.ActTypeInfoIt)->ClassInfo.begin();
  }
  
  // Se establece el texto de seleccion la clase
  const std::string szClass = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_CLASS, 
													  (*m_TypeClassInfo.ActClassInfoIt)->IdClass);
  m_TypeClassInfo.ClassSelect.ChangeText(szClass);

  // Establece posicion y quad de fx
  m_TypeClassInfo.FXClassBack.Position = sPosition(m_TypeClassInfo.ClassSelect.GetXPos() - 5,
												   m_TypeClassInfo.ClassSelect.GetYPos() - 1);
  InitFXBack(m_TypeClassInfo.FXClassBack, szClass);
  
  // Vbles
  std::string szTmp; // Texto temporal

  // Se asocian los valores de los distintos atributos
  // Salud
  m_AttributesInfo.Health.uwAsingPoints = 0;
  SYSEngine::PassToString(szTmp,
						  "%d (+ %u)",
						  (*m_TypeClassInfo.ActClassInfoIt)->HealthInfo.swBaseValue,
						  m_AttributesInfo.Health.uwAsingPoints);
  m_AttributesInfo.Health.AttribValue.ChangeText(szTmp);  
  m_AttributesInfo.Health.AttribValue.SetUnselectColor(m_AttributesInfo.RGBBaseValue);
  
  // Puntos de combate
  m_AttributesInfo.CombatPoints.uwAsingPoints = 0;
  SYSEngine::PassToString(szTmp,
						  "%d (+ %u)",
						  (*m_TypeClassInfo.ActClassInfoIt)->CombatPointsInfo.swBaseValue,
						  m_AttributesInfo.CombatPoints.uwAsingPoints);
  m_AttributesInfo.CombatPoints.AttribValue.ChangeText(szTmp);
  m_AttributesInfo.CombatPoints.AttribValue.SetUnselectColor(m_AttributesInfo.RGBBaseValue);

  // Atributos extendidos
  const word uwMaxExtAttribs = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB);
  byte ubIt = 0;
  for (; ubIt < uwMaxExtAttribs; ++ubIt) {
	m_AttributesInfo.ExtAttribs[ubIt].uwAsingPoints = 0;
	SYSEngine::PassToString(szTmp,
						  "%d (+ %u)",
						  (*m_TypeClassInfo.ActClassInfoIt)->ExtAttribsInfo[ubIt].swBaseValue,
						  m_AttributesInfo.ExtAttribs[ubIt].uwAsingPoints);
	m_AttributesInfo.ExtAttribs[ubIt].AttribValue.ChangeText(szTmp);
	m_AttributesInfo.ExtAttribs[ubIt].AttribValue.SetUnselectColor(m_AttributesInfo.RGBBaseValue);
  }  

  // Se toman puntos de reparto y se ajusta la barra de porcentaje
  m_AttributesInfo.uwImprovePoints = (*m_TypeClassInfo.ActClassInfoIt)->uwAttribsDistribPoints;
  m_AttributesInfo.ImprovePointsPBar.ChangeBaseAndTempValue(m_AttributesInfo.uwImprovePoints,
															m_AttributesInfo.uwImprovePoints);

  // Establece texto asociado a la cantidad de puntos restantes
  SYSEngine::PassToString(szTmp, 
						  "%u \\ %u", 
						  m_AttributesInfo.ImprovePointsPBar.GetTempValue(),
						  m_AttributesInfo.ImprovePointsPBar.GetBaseValue());
  m_AttributesInfo.ImprovePoints.ChangeText(szTmp);

  // Establece salud como atributo actual
  m_AttributesInfo.Health.AttribName.Select(true);    
  SelectAttribute(CGUIWCharacterCreator::ID_HEALTH,
				  &m_AttributesInfo.Health,
				  (*m_TypeClassInfo.ActClassInfoIt)->HealthInfo.uwCost,
				  (*m_TypeClassInfo.ActClassInfoIt)->HealthInfo.swBaseValue);  

  // Se resetea cualquier habilidad 
  const word uwMaxHabilities = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);
  ubIt = 0;
  for (; ubIt < uwMaxHabilities; ++ubIt) {
	m_HabilitiesInfo.Habilities[ubIt].HabilityName.Select(false);
	m_HabilitiesInfo.Habilities[ubIt].FXBackName.FXBack.End();
  }

  // Establece base de las habilidades y temporal
  m_HabilitiesInfo.uwHabDistributePoints = ((*m_TypeClassInfo.ActClassInfoIt)->uwHabilitiesDistribPoints);
  UpdateHabilityPercentageBar();    

  // Se liberan textos flotantes previos y se cargan los nuevos
  ReleaseFloatingText();
  RegisterFloatingText();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada al incremento en los puntos del atributo establecido
//   como atributo actual.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::IncAttributePoints(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // ¿NO es posible realizar el incremento?
  if (m_AttributesInfo.ActAttribute.uwImproveCost > m_AttributesInfo.uwImprovePoints) {	
	return;	
  }

  // Se decrementan puntos actuales y se incrementan puntos de atributo actual  
  m_AttributesInfo.uwImprovePoints -= m_AttributesInfo.ActAttribute.uwImproveCost;
  ++m_AttributesInfo.ActAttribute.pAttribute->uwAsingPoints;
  
  // Se ajusta la barra de porcentaje
  m_AttributesInfo.ImprovePointsPBar.ChangeTempValue(m_AttributesInfo.uwImprovePoints);

  // Se establece el texto asociado
  SetAttributeTextPoints();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Decrementa los posibles puntos que se hayan asociado a un atributo y
//   los restaura al total de puntos repartibles.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::DecAttributePoints(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿NO es posible realizar el decremento?
  if (!m_AttributesInfo.ActAttribute.pAttribute->uwAsingPoints) {	
	return;	
  }

  // Se devuelve un punto
  m_AttributesInfo.uwImprovePoints += m_AttributesInfo.ActAttribute.uwImproveCost;
  --m_AttributesInfo.ActAttribute.pAttribute->uwAsingPoints;
  
  // Se ajusta la barra de porcentaje
  m_AttributesInfo.ImprovePointsPBar.ChangeTempValue(m_AttributesInfo.uwImprovePoints);

  // Se establece el texto asociado
  SetAttributeTextPoints(); 
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece en pantalla el texto asociado a los puntos que tiene un atributo
//   asi como a los puntos que quedan por repartir.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWCharacterCreator::SetAttributeTextPoints(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se establece el texto asociado a la cantidad de puntos actuales del atributo
  std::string szTmp;
  SYSEngine::PassToString(szTmp, 
						  "%u (+ %u)", 
						  m_AttributesInfo.ActAttribute.swBaseValue,
						  m_AttributesInfo.ActAttribute.pAttribute->uwAsingPoints);
  m_AttributesInfo.ActAttribute.pAttribute->AttribValue.ChangeText(szTmp);
  if (m_AttributesInfo.ActAttribute.pAttribute->uwAsingPoints) {
	m_AttributesInfo.ActAttribute.pAttribute->AttribValue.SetUnselectColor(m_AttributesInfo.RGBImproveValue);	
  } else {
	m_AttributesInfo.ActAttribute.pAttribute->AttribValue.SetUnselectColor(m_AttributesInfo.RGBBaseValue);	
  }

  // Establece texto asociado a la cantidad de puntos restantes
  SYSEngine::PassToString(szTmp, 
						  "%u \\ %u", 
						  m_AttributesInfo.ImprovePointsPBar.GetTempValue(),
						  m_AttributesInfo.ImprovePointsPBar.GetBaseValue());
  m_AttributesInfo.ImprovePoints.ChangeText(szTmp);

  // Reajusta quad de fx
  m_AttributesInfo.FXBack.Position = sPosition(m_AttributesInfo.ActAttribute.pAttribute->AttribValue.GetXPos() - 5,
										       m_AttributesInfo.ActAttribute.pAttribute->AttribValue.GetYPos() - 1);
  InitFXBack(m_AttributesInfo.FXBack,
			 m_AttributesInfo.ActAttribute.pAttribute->AttribValue.GetLineText());  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece un nuevo atributo como el seleccionado, quitando el que estuviera
//   antes.
// Parametros:
// - IDComponent. Componente a seleccionar.
// - Attrib. Componente de linea que muestra el valor del atributo.
// - uwImproveCost. Coste asociado al atributo para incrementarlo
// - swBaseValue. Valor base.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWCharacterCreator::SelectAttribute(const GUIDefs::GUIIDComponent& IDComponent,
									   sAttributesInfo::sModifyAttribInfo* const pAttribute,
									   const word uwImproveCost,
									   const sword& swBaseValue)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(IDComponent);

  // ¿El atributo a establecer NO es el actualmente establecido?
  if (m_AttributesInfo.IDAttributeSet != IDComponent) {	
	// Selecciona el nuevo atributo
	const GUIDefs::GUIIDComponent PrevAttribute = m_AttributesInfo.ActAttribute.IDAttribute;	  
	m_AttributesInfo.ActAttribute.IDAttribute = IDComponent;	  
	OnSelectNotify(PrevAttribute, false);  

	// Obtiene los datos asociados al atributo a establecer como actual
	m_AttributesInfo.ActAttribute.pAttribute = pAttribute;
	m_AttributesInfo.ActAttribute.uwImproveCost = uwImproveCost;
	m_AttributesInfo.ActAttribute.swBaseValue = swBaseValue;
  
	// Establece posicion y quad de fx
	m_AttributesInfo.FXBack.Position = sPosition(pAttribute->AttribValue.GetXPos() - 5,
												 pAttribute->AttribValue.GetYPos() - 1);
	InitFXBack(m_AttributesInfo.FXBack,
			   pAttribute->AttribValue.GetLineText());
	
	// Se toma ID
	m_AttributesInfo.IDAttributeSet = IDComponent;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Selecciona / deselecciona la habilidad cuyo indice sea ubIdxHability. La
//   decision se realizara teniendo en cuenta el estado de inicializacion del
//   quad de fx asociado. Si estuviera inicializado, se entendera que se desea
//   deselecionar la habilidad y en caso de que no lo estuviera justamente
//   lo contrario.
// - Deseleccionar supondra quitar la habilidad y retornar los puntos que
//   esta costara y seleccionar, asociar la habilidad SOLO si se tuvieran
//   puntos suficientes.
// Parametros:
// - ubIdxHability. Indice de la habilidad
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::SelectHability(const byte ubIdxHability)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT((ubIdxHability < m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY)) != 0);

  // Se obtiene alias al coste de asociar la habilidad
  const word uwCost = (*m_TypeClassInfo.ActClassInfoIt)->HabilitiesInfo[ubIdxHability].uwCost;

  // ¿Hay que deseleccionar?
  if (m_HabilitiesInfo.Habilities[ubIdxHability].FXBackName.FXBack.IsInitOk()) {
	// Si, se quiere desechar la habilidad
	// Se finaliza quad, se reponen puntos y se acualiza porcentaje
	m_HabilitiesInfo.Habilities[ubIdxHability].FXBackName.FXBack.End();
	m_HabilitiesInfo.uwHabDistributePoints += uwCost;
	UpdateHabilityPercentageBar();
  } else {
	// No, se quiere tomar la habilidad
	// ¿Hay suficientes puntos de reparto?
	if (uwCost <= m_HabilitiesInfo.uwHabDistributePoints) {
	  // Si, se habilita quad, se restan puntos y se actualiza barra de porcentaje
	  m_HabilitiesInfo.uwHabDistributePoints -= uwCost;
	  InitFXBack(m_HabilitiesInfo.Habilities[ubIdxHability].FXBackName,
				 m_HabilitiesInfo.Habilities[ubIdxHability].HabilityName.GetLineText());
	  ASSERT(m_HabilitiesInfo.Habilities[ubIdxHability].FXBackName.FXBack.IsInitOk());
	  UpdateHabilityPercentageBar();
	}	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Actualiza la barra de porcentaje que lleva la cuenta de puntos de
//   repartos posibles para habilidades
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::UpdateHabilityPercentageBar(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Establece el texto con el recuento de puntos
  std::string szTmp;
  SYSEngine::PassToString(szTmp,
						  "%u \\ %u",
						  m_HabilitiesInfo.uwHabDistributePoints,
						  (*m_TypeClassInfo.ActClassInfoIt)->uwHabilitiesDistribPoints);
  m_HabilitiesInfo.HabDistributePoints.ChangeText(szTmp);

  // Establece el nuevo valor temporal para la barra de porcentaje
  m_HabilitiesInfo.HabDistributePointsPBar.ChangeTempValue(m_HabilitiesInfo.uwHabDistributePoints);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Bloquea o desbloquea la entrada para componentes e interfaz
// Parametros:
// - bInput. Flag para bloquear o desbloquear interfaz.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::SetInput(const bool bInput)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se activan / desactivan los componentes
  
  // Imagen de fondo
  m_BackImgInfo.BackImg.SetInput(bInput);
  
  // Cancelacion
  m_CancelInfo.Button.SetInput(bInput);

  // Aceptacion
  m_AcceptInfo.Button.SetInput(bInput);
  
  // Nombre
  m_NameInputInfo.NameText.SetInput(bInput);
  m_NameInputInfo.NameInput.SetInput(bInput); 
  
  // Genero
  m_GenreInfo.GenreOption.SetInput(bInput);
  m_GenreInfo.GenreSelect.SetInput(bInput);
  m_GenreInfo.NextButton.SetInput(bInput);
  m_GenreInfo.PrevButton.SetInput(bInput);  

  // Tipo y clase
  m_TypeClassInfo.TypeOption.SetInput(bInput);
  m_TypeClassInfo.TypeSelect.SetInput(bInput);
  m_TypeClassInfo.ClassOption.SetInput(bInput);
  m_TypeClassInfo.ClassSelect.SetInput(bInput);

  // Atributos
  m_AttributesInfo.ImprovePoints.SetInput(bInput);
  m_AttributesInfo.ImprovePointsPBar.SetInput(bInput);
  m_AttributesInfo.Health.AttribName.SetInput(bInput);
  m_AttributesInfo.Health.AttribValue.SetInput(bInput);
  m_AttributesInfo.CombatPoints.AttribName.SetInput(bInput);
  m_AttributesInfo.CombatPoints.AttribValue.SetInput(bInput);
  const word uwMaxExtAttribs = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB);
  byte ubIt = 0;
  for (; ubIt < uwMaxExtAttribs; ++ubIt) {
	m_AttributesInfo.ExtAttribs[ubIt].AttribName.SetInput(bInput);
	m_AttributesInfo.ExtAttribs[ubIt].AttribValue.SetInput(bInput);
  }
  m_AttributesInfo.DecButton.SetInput(bInput);
  m_AttributesInfo.IncButton.SetInput(bInput);

  // Habilidades  
  const word uwMaxHabilities = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);  
  ubIt = 0;
  for (; ubIt < uwMaxHabilities; ++ubIt) {
	m_HabilitiesInfo.Habilities[ubIt].HabilityIcon.SetInput(bInput);
	m_HabilitiesInfo.Habilities[ubIt].HabilityName.SetInput(bInput);
  }
  m_HabilitiesInfo.HabDistributePointsPBar.SetInput(bInput);
  m_HabilitiesInfo.HabDistributePoints.SetInput(bInput);

  // Propaga llamada
  Inherited::SetInput(bInput);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si la aceptacion, dadas las condiciones actuales de edicion,
//   es posible
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWCharacterCreator::CanAcceptCharacter(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿NO se ha insertado el nombre?
  if (!m_NameInputInfo.NameInput.IsInputInserted()) {
	return false;
  }

  // ¿NO se han repartido todos los puntos de atributo cuando procede?
  if ((*m_TypeClassInfo.ActClassInfoIt)->bMustDistribAttribPoints &&
	  m_AttributesInfo.uwImprovePoints) {
	return false;
  }

  // ¿NO se han repartido todos los puntos de habilidades cuando procede?
  if ((*m_TypeClassInfo.ActClassInfoIt)->bMustDistribHabilityPoints &&
	  m_HabilitiesInfo.uwHabDistributePoints) {
	return false;
  }

  // Se puede aceptar el caracter
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este metodo sera llamado cuando finalmente un caracter haya sido aceptado.
//   Aqui se generaran de forma formal los elementos que generaran al 
//   jugador y se cambiara al estado de transicion "StartNewGame".
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::AcceptCharacter(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se forma la estructura Attrib
  RulesDefs::sCriatureAttrib Attribs; // Estructura  
  
  // Tipo, clase y nombre
  Attribs.Type = (*m_TypeClassInfo.ActTypeInfoIt)->IdType;
  Attribs.Class = (*m_TypeClassInfo.ActClassInfoIt)->IdClass;
  Attribs.Genre = m_GenreInfo.ActGenre;
  
  // Nivel y experiencia
  Attribs.Level = 1;
  Attribs.Experience = 0;
  
  // Atributos base
  Attribs.Health.swBase = (*m_TypeClassInfo.ActClassInfoIt)->HealthInfo.swBaseValue + 
					       m_AttributesInfo.Health.uwAsingPoints;
  Attribs.Health.swTemp = Attribs.Health.swBase;
  Attribs.ActionPoints = (*m_TypeClassInfo.ActClassInfoIt)->CombatPointsInfo.swBaseValue + 
						  m_AttributesInfo.CombatPoints.uwAsingPoints;	  
  
  // Atributos extendidos
  const word uwNumExtAttribs = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB);
  byte ubIt = 0;
  for (; ubIt < uwNumExtAttribs; ++ubIt) { 
	Attribs.ExAttribs[ubIt].swBase = (*m_TypeClassInfo.ActClassInfoIt)->ExtAttribsInfo[ubIt].swBaseValue +
									 m_AttributesInfo.ExtAttribs[ubIt].uwAsingPoints;
	Attribs.ExAttribs[ubIt].swTemp = Attribs.ExAttribs[ubIt].swBase;
  } 

  // Habilidades
  const word uwNumHabilities = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);
  ubIt = 0;
  for (; ubIt < uwNumHabilities; ++ubIt) {
	Attribs.Habilities |= m_HabilitiesInfo.Habilities[ubIt].FXBackName.FXBack.IsInitOk() ? 
						  (RulesDefs::HABILITY_01 << ubIt) : 0x00;	
  }

  // Sin sintomas 
  Attribs.Symptoms = 0;

  // Rango maximo
  Attribs.Range = RulesDefs::MAX_CRIATURE_RANGE;

  // Se carga la informacion asociada al area de comienzo y posicion
  CCBTEngineParser* const pParser = m_pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_PLAYER_PROFILES, 
													        "[Common]");
  ASSERT(pParser);
  pParser->SetVarPrefix("StartLocation.");
  const word uwIDArea = pParser->ReadNumeric("IDArea");
  const AreaDefs::sTilePos TilePos(pParser->ReadNumeric("Cell.XPos"),
								   pParser->ReadNumeric("Cell.YPos"));  

  // Se crea instancia del jugador y se inicializa
  CPlayer* pPlayer = new CPlayer;
  ASSERT(pPlayer);
  if (!pPlayer->Init((*m_GenreInfo.ItPortrait)->szSprite,
					  m_NameInputInfo.NameInput.GetActInput(),
					  (*m_GenreInfo.ItPortrait)->szAnimTemplate,
					  (*m_GenreInfo.ItPortrait)->szShadowImage,
					  Attribs)) {
	SYSEngine::FatalError("Fallo creando la instancia al jugador creado");
  }

  
  // Por ultimo, se cambia de estado  
  SYSEngine::SetStartNewGameState(uwIDArea, pPlayer, TilePos);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dibuja componentes.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCharacterCreator::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Si flag de dibujado
  if (Inherited::IsActive()) {
	// Dibuja imagen de fondo  
	m_BackImgInfo.BackImg.Draw();

	// Boton de cancelacion
	m_CancelInfo.Button.Draw();

	// Boton de aceptacion
	m_AcceptInfo.Button.Draw();

	// Componentes para la introduccion del nombre
	m_NameInputInfo.NameText.Draw();
	m_NameInputInfo.NameInput.Draw();

	// Componentes para la seleccion del genero
	m_GenreInfo.GenreOption.Draw();
	if (m_GenreInfo.GenreOption.IsSelect()) {
	  m_GenreInfo.FXBack.FXBack.Draw();
	}
	m_GenreInfo.GenreSelect.Draw();
	m_GenreInfo.NextButton.Draw();
	m_GenreInfo.PrevButton.Draw();  
	m_pGraphSys->Draw(GraphDefs::DZ_GUI,
					  0,
					  m_GenreInfo.PortraitsDrawPos.swXPos,
					  m_GenreInfo.PortraitsDrawPos.swYPos,
					  &(*m_GenreInfo.ItPortrait)->Portrait);
	
	// Componentes para seleccion de tipo y clase	
	if (m_TypeClassInfo.TypeOption.IsSelect()) {
	  m_TypeClassInfo.FXTypeBack.FXBack.Draw();
	} else if (m_TypeClassInfo.ClassOption.IsSelect()) {
	  m_TypeClassInfo.FXClassBack.FXBack.Draw();
	}
	m_TypeClassInfo.TypeOption.Draw();
	m_TypeClassInfo.TypeSelect.Draw();		
	m_TypeClassInfo.ClassOption.Draw();
	m_TypeClassInfo.ClassSelect.Draw();

	// Atributos
	m_AttributesInfo.ImprovePointsPBar.Draw();
	m_AttributesInfo.ImprovePoints.Draw();
	m_AttributesInfo.FXBack.FXBack.Draw();
	m_AttributesInfo.Health.AttribName.Draw();
	m_AttributesInfo.Health.AttribValue.Draw();
	m_AttributesInfo.CombatPoints.AttribName.Draw();
	m_AttributesInfo.CombatPoints.AttribValue.Draw();
	const uwMaxExtAttribs = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB);
	byte ubIt = 0;
	for (; ubIt < uwMaxExtAttribs; ++ubIt) {
	  m_AttributesInfo.ExtAttribs[ubIt].AttribName.Draw();
	  m_AttributesInfo.ExtAttribs[ubIt].AttribValue.Draw();
	}
	m_AttributesInfo.DecButton.Draw();
	m_AttributesInfo.IncButton.Draw();

	// Habilidades
	const word uwMaxHabilities = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);  
	ubIt = 0;
	for (; ubIt < uwMaxHabilities; ++ubIt) {
	  m_HabilitiesInfo.Habilities[ubIt].HabilityIcon.Draw();
  	  if (m_HabilitiesInfo.Habilities[ubIt].FXBackName.FXBack.IsInitOk()) {
		m_HabilitiesInfo.Habilities[ubIt].FXBackName.FXBack.Draw();
	  }
	  m_HabilitiesInfo.Habilities[ubIt].HabilityName.Draw();
	}
	m_HabilitiesInfo.HabDistributePointsPBar.Draw();
	m_HabilitiesInfo.HabDistributePoints.Draw();
  }
}