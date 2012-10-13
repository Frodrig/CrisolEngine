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
// CGUIWEntityInteract.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWEntityInteract.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWEntityInteract.h"

#include "SYSEngine.h"
#include "iCGraphicSystem.h"
#include "iCInputManager.h"
#include "iCLogger.h"
#include "iCGameDataBase.h"
#include "iCGUIManager.h"
#include "iCCombatSystem.h"
#include "CMemoryPool.h"
#include "CCBTEngineParser.h"
#include "CCriature.h"

// Definicion de estructuras forward
struct CGUIWEntityInteract::sNOption {
  // Nodo asociado a una opcion en el map de opciones
  CGUICButton Option; // Boton / icono con la opcion
  // Manejador de memoria
  static CMemoryPool  MPool;
  static void* operator new(const size_t size) { return MPool.AllocMem(size); }
  static void operator delete(void* pItem) { MPool.FreeMem(pItem); } 
};

// Inicializacion del pool de memoria
CMemoryPool 
CGUIWEntityInteract::sNOption::MPool(4,
								     sizeof(CGUIWEntityInteract::sNOption), 
								     true);


///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se dejara reinicializar.
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWEntityInteract::Init(void)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("CGUIWEntityInteract::Init> Inicializando interfaz de interaccion con entidades.\n");
  #endif

  // Se inicializa clase base
  if (Inherited::Init()) {
	// Se obtienen instancias a modulos
	m_pWorld = SYSEngine::GetWorld();
	ASSERT(m_pWorld);
	m_pGDBase = SYSEngine::GetGameDataBase();
	ASSERT(m_pGDBase);
	m_pGUIManager = SYSEngine::GetGUIManager();
	ASSERT(m_pGUIManager);
	
	// Se cargan datos
	if (TLoad()) {
	  // Se reserva espacio en los menus de opciones
	  m_MenusInfo.MainMenu.reserve(6);
	  m_MenusInfo.HabilityMenu.reserve(9);

	  // Se establece menu actual nulo
	  m_MenusInfo.pActMenu = NULL;

	  // Todo correcto
 	  #ifdef ENGINE_TRACE
		SYSEngine::GetLogger()->Write("                         | Ok.\n");        
	  #endif	  
	  return true;
	}
  }
  
  // Hubo problemas
  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("                         | Error.\n");        
  #endif
  End();
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWEntityInteract::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	// Desactiva
	Deactive();	

	// Libera y finaliza menus y componentes
	ReleaseMenus();
	ReleaseOptions();	
	
	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga los botones de todas las posibles opciones, llamando al metodo
//   CreateOptions para crear los componentes y mapearos en el map.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWEntityInteract::TLoad(void)
{
  // Se obtiene el parser
  CCBTEngineParser* const pParser = m_pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG, 
												      "[EntityInteract]");
  if (!pParser) { 
    SYSEngine::FatalError("No se localizó la sección [EntityInteract] en archivo de configuracion de interfaces.\n");
  } 

  // Se toma el valor asociado para el texto flotante
  pParser->SetVarPrefix("FloatingText.");
  m_MenusInfo.RGBFlotaingText = pParser->ReadRGBColor();

  // Se toma valor alpha para opciones no seleccionadas
  pParser->SetVarPrefix("DrawFormat.");
  m_MenusInfo.NoSelectAlpha = pParser->ReadAlpha("Options.Unselect.");

  // Vbles
  CGUICButton::sGUICButtonInitData CfgOption; // Cfg para opciones

  // Se establece valores comunes en la configuracion
  CfgOption.pGUIWindow = this;
  CfgOption.GUIEvents = GUIDefs::GUIC_SELECT |
						GUIDefs::GUIC_UNSELECT |
						GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  CfgOption.ubDrawPlane = 2;

  // Se cargan opciones de menu principal
  
  // Nombres de opciones
  const std::string szMainMenuOptions[] = {
	"Option[Observe].AnimTemplate", "Option[Talk].AnimTemplate",
	"Option[Register].AnimTemplate", "Option[Get].AnimTemplate",
	"Option[Manipulate].AnimTemplate", "Option[Exit].AnimTemplate",
	"Option[UseInventory].AnimTemplate", "Option[UseHability].AnimTemplate",
	"Option[DropItem].AnimTemplate",
  };

  // Se carga
  pParser->SetVarPrefix("MainMenu.");
  byte ubIt = 0;
  for (; ubIt < CGUIWEntityInteract::ID_DROPITEM; ++ubIt) {
	CfgOption.ID = CGUIWEntityInteract::ID_OBSERVE + ubIt;
	CfgOption.szATButton = pParser->ReadString(szMainMenuOptions[ubIt]);
	if (!CreateOption(CfgOption)) {
	  return false;
	}
  } 	

  // Carga de botones asociados a las habilidades (solo las que sean usables)
  pParser->SetVarPrefix("HabilityMenu.");
  std::string szTmp;
  const word uwMaxHabilities = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);
  ubIt = 0;
  for (; ubIt < uwMaxHabilities; ++ubIt) {
	// ¿Habilidad usable?
	if (m_pGDBase->GetRulesDataBase()->IsUsableHability(RulesDefs::eIDHability(0x01 << ubIt))) {
	  // Se obtiene el nombre y se configura estructura
	  SYSEngine::PassToString(szTmp,
							  "Option[%s].AnimTemplate",
							  m_pGDBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_HABILITY,
																			ubIt).c_str());
	  CfgOption.ID = CGUIWEntityInteract::ID_HABILITY00 + ubIt;
	  CfgOption.szATButton = pParser->ReadString(szTmp);	  

	  // Se crea
	  if (!CreateOption(CfgOption)) {
	    return false; 
	  }			  
	}
  }  

  // Carga el boton de retorno del menu de habilidades
  CfgOption.ID = CGUIWEntityInteract::ID_HABILITYMENUEXIT;
  CfgOption.szATButton = pParser->ReadString("Option[ReturnToMainMenu].AnimTemplate");
  if (!CreateOption(CfgOption)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nuevo nodo que represente la opcion cuya configuracion es 
//   CfgButton y la inserta en el map a partir de su identificador.
// Parametros:
// - CfgButton. Configuracion de la opcion a crear.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWEntityInteract::CreateOption(const CGUICButton::sGUICButtonInitData& CfgButton)
{
  // SOLO si parametros validos
  ASSERT(CfgButton.ID);
  ASSERT((m_Options.find(CfgButton.ID) == m_Options.end()) != 0);

  // Se crea un nuevo nodo
  sNOption* const pNewOption = new sNOption;
  ASSERT(pNewOption);

  // Se inicializa el nodo
  if (!pNewOption->Option.Init(CfgButton)) {
	// Progblemas
	delete pNewOption;
	return false;
  }

  // Se mapea a partir de su identificador y se retorna
  m_Options.insert(OptionsMapValType(CfgButton.ID, pNewOption));
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva cualquier set de menus que estuviera establecido.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWEntityInteract::ReleaseMenus(void)
{
  // Vacia contenedores de menus
  m_MenusInfo.MainMenu.clear();
  m_MenusInfo.HabilityMenu.clear();

  // Establece estado nulo en menu
  m_MenusInfo.pActMenu = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera las opciones que esten alojadas en el map.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWEntityInteract::ReleaseOptions(void)
{
  // Procede a liberar opciones de menu
  OptionsMapIt It(m_Options.begin());
  while (It != m_Options.end()) {	
	delete It->second;
	It = m_Options.erase(It);
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa la instancia.
// - La activacion podra llevarse a cabo dentro del juego o bien en la pantalla
//   de inventario. Sera en este ultimo caso cuando la instancia pEntityDest
//   sea un item y ademas tenga dueño.
// Parametros:
// - sInitDrawPos. Posicion inicial de dibujado.
// - pEntitySrc. Entidad que inicia la interaccion. Siempre sera una criatura
//   y en concreto, el jugador.
// - pEntityDest. Entidad que recibira la interaccion
// - ActiveContext. Contexto de activacion
// Devuelve:
// Notas:
// - Sera muy importante haber establecido en primer lugar el menu de 
//   habilidades pues los menus principales trabajaran con la cantidad de
//   habilidades posibles de utilizara para inicializar la opcion de uso
//   de habilidades.
// - Solo se lograra una activacion cuando el menu principal tenga al menos
//   una opcion de interaccion.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWEntityInteract::Active(const sPosition& InitDrawPos,
							CCriature* const pEntitySrc,
							CWorldEntity* const pEntityDest,
							const GUIManagerDefs::eGUIWindowType& ActiveContext)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(pEntitySrc);
  ASSERT(pEntityDest);

  // Activa si procede
  if (!Inherited::IsActive()) {		
	// Establece cursor de interaccion con interPortraits
	m_pGUIManager->SetGUICursor(GUIManagerDefs::WINDOWINTERFAZ_CURSOR);	
	
	// Se desasocia cualquier texto flotante actual
	m_MenusInfo.szActFloatingText = "";

	// Se activa el menu de habilidades
	CreateHabilityMenu(pEntitySrc);	

	// Se crea el menu principal comprobando el contexto de activacion
	bool bInInvContext = true;
	switch (ActiveContext) {
	  case GUIManagerDefs::GUIW_MAININTERFAZ: {
		// Se esta interactuando en juego
		CreateInGameMainMenu(pEntityDest);	  
		bInInvContext = false;
	  } break;
	  
	  case GUIManagerDefs::GUIW_PLAYERPROFILE: {
		// Se esta interactuando desde inventario siendo el item propiedad de
		// la entidad pEntitySrc
		CreateInInventoryMainMenu(pEntityDest, true);
	  } break;
	  
	  case GUIManagerDefs::GUIW_TRADE: {
		// Se esta actuando desde interfaz de intercambio, luego pEntityDest
		// es seguro una entidad de tipo ITEM.
		// ¿Es el item propiedad de pEntitySrc?
		const CItem* const pItem = m_pWorld->GetItem(pEntityDest->GetHandle());
		ASSERT(pItem);
		if (pItem->GetOwner() == pEntitySrc->GetHandle()) {
		  // Si, luego se esta en la ventana de inv. de pEntitySrc
		  CreateInInventoryMainMenu(pEntityDest, true);
		} else {
		  // No, el item esta en la ventana de inv. del contenedor que se
		  // esta examinando luego solo se activara la opcion de observar si procede
		  CreateInInventoryMainMenu(pEntityDest, false);
		}		
	  } break;

	  default:
		ASSERT_MSG(false, "Contexto de activacion incorrecto");
	}; // ~ switch

	// ¿Hay al menos una opcion en el menu principal?
	if (!m_MenusInfo.MainMenu.empty()) {
	  // Se toman las intancias a las entidades
	  m_EntitiesInfo.pEntitySrc = pEntitySrc;
	  m_EntitiesInfo.pEntityDest = pEntityDest;
	
	  // Se calcula la posicion correcta de dibujado y se establece en opciones
	  const sPosition CorrectDrawPos(GetCorrectInitDrawPos(InitDrawPos));
	  SetMenuOptionsDrawPos(m_MenusInfo.MainMenu, CorrectDrawPos);
	  SetMenuOptionsDrawPos(m_MenusInfo.HabilityMenu, CorrectDrawPos);

	  // Se establece menu principal como menu actual
	  m_MenusInfo.pActMenu = &m_MenusInfo.MainMenu;

	  // Se activa menu actual
	  ActiveMenu(*m_MenusInfo.pActMenu, true);

	  // Se establece el contexto de activacion
	  m_ActiveContext = ActiveContext;

	  // Se seleccionara al item sobre el que se esta actuando, para resaltarlo
	  m_EntitiesInfo.pEntityDest->SetFadeSelect(true);
	  
	  // Se instala como observer del World
	  iCWorld* const pWorld = SYSEngine::GetWorld();
	  ASSERT(pWorld);
	  pWorld->AddObserver(this);

	  // Se instala como observador de la entidad destino, si
	  // fuera una criatura o jugador, y se toma su posicion original
	  // y se instala la interfaz como observadora
	  CCriature* const pCriature = pWorld->GetCriature(pEntityDest->GetHandle());
	  if (pCriature) {	  
		pCriature->AddObserver(this);
		m_EntitiesInfo.EntityDestPos = pCriature->GetTilePos();		
	  }
	
	  // Propaga
	  Inherited::Active(NULL);
	}	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva.
// - Al desactivar se liberaran los contenedores con las opciones escogidas
//   en la activacion.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWEntityInteract::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se desactiva si procede
  if (Inherited::IsActive()) {
	// Se desactiva el menu principal y el de habilidades
	ActiveMenu(m_MenusInfo.MainMenu, false);
	ActiveMenu(m_MenusInfo.HabilityMenu, false);

	// Se liberan menus
	ReleaseMenus();

	// Se desinstala como observer del World
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);	  
	pWorld->RemoveObserver(this);	  

	// Se desinstala como observer de la entidad destino si procede
	ASSERT(m_EntitiesInfo.pEntityDest);
	CCriature* const pCriature = pWorld->GetCriature(m_EntitiesInfo.pEntityDest->GetHandle());		
	if (pCriature) {
	  pCriature->RemoveObserver(this);
	}
	
	// Propaga
	Inherited::Deactive();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Abandona el interfaz teniendo en cuenta el contexto de activacion. El
//   contexto de activacion indicara simplemente desde donde fue activado
//   el interfaz.
// - Los posibles contextos de activacion estara establecidos en el enumerado
//   eActiveContext.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWEntityInteract::ExitFromInterfaz(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Comprueba el contexto de activacion y retorna
  switch(m_ActiveContext) {
	case GUIManagerDefs::GUIW_MAININTERFAZ: {
	  // La interfaz fue activada desde dentro del juego activo
	  // Antes de nada, se deseleccionara entidad
	  m_EntitiesInfo.pEntityDest->SetFadeSelect(false);
	  m_pGUIManager->SetMainInterfazWindow();
	} break;

	case GUIManagerDefs::GUIW_PLAYERPROFILE: {
	  // La interfaz fue activada desde dentro del inventario del jugador
	  // Se deselecciona el item y se establece interfaz
	  m_EntitiesInfo.pEntityDest->SetFadeSelect(false);
	  m_pGUIManager->SetPlayerProfileWindow();	  
	} break;

	case GUIManagerDefs::GUIW_TRADE: {
	  // La interfaz fue activada desde el la de intercambio de items	  
	  // Se deselecciona el item y se establece interfaz
	  m_EntitiesInfo.pEntityDest->SetFadeSelect(false);
	  m_pGUIManager->SetTradeWindow(m_EntitiesInfo.pEntitySrc,
								    m_EntitiesInfo.pEntityDest,
									NULL);	  	  
	} break;

	default:
	  ASSERT_MSG(false, "El contexto de activacion NO ES correcto");
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se crea el menu de habilidades a partir de la criatura que inicia
//   la accion. El menu de habilidades contendra el total de habilidades que
//   este podra lanzar. 
// - El menu estara construido a partir de las referencias a las opciones que
//   representan a la habilidad que se puede lanzar.
// Parametros:
// - pEntitySrc. Entidad que inicia la accion.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWEntityInteract::CreateHabilityMenu(const CCriature* const pEntitySrc)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(pEntitySrc);

  // Se comienza a comprobar que habilidades USABLES posee la entidad
  const word uwMaxHabilities = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);
  byte ubIt = 0;
  for (; ubIt < uwMaxHabilities; ++ubIt) {
	// ¿Habilidad usable y ademas poseida por la entidad?
	const RulesDefs::eIDHability ActHability = RulesDefs::eIDHability(0x01 << ubIt);
	if (m_pGDBase->GetRulesDataBase()->IsUsableHability(ActHability) &&
	    pEntitySrc->IsHabilityActive(ActHability)) {
	  // Se registra
	  m_MenusInfo.HabilityMenu.push_back(CGUIWEntityInteract::ID_HABILITY00 + ubIt);
	}
  }  

  // ¿Hay alguna habilidad registrada?
  if (!m_MenusInfo.HabilityMenu.empty()) {
	  // Se registra
	  m_MenusInfo.HabilityMenu.push_back(CGUIWEntityInteract::ID_HABILITYMENUEXIT);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea el menu de interaccion principal en el interior del juego.
// - La opcion de usar desde inventario siempre estara presente, otra cosa sera
//   que dentro de inventario se pueda seleccionar una entidad usable.
// Parametros:
// - pEntityDest. Entidad que recibe la accion.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWEntityInteract::CreateInGameMainMenu(CWorldEntity* const pEntityDest)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(pEntityDest);
  
  // Se registra la de observar si la entidad destino tiene asociado un
  // script asociado al evento de observacion
  if (!pEntityDest->GetScriptEvent(RulesDefs::SE_ONOBSERVE).empty()) {
	m_MenusInfo.MainMenu.push_back(CGUIWEntityInteract::ID_OBSERVE);
  }
  
  // Se registra la opcion de usar
  // En el caso de la opcion de uso, esta variara dependiendo de la entidad
  // destino en cuanto a su boton de representacion:
  // * Si es una criatura sera REGISTRAR SOLO si no esta HABLAR
  // * Si es un item sera RECOGER
  // * Cualquier otro caso sera MANIPULAR
  switch (pEntityDest->GetEntityType()) {
    case RulesDefs::CRIATURE: { 
  	  // Registrar, ¿NO esta activo el combate?
	  iCCombatSystem* const pCombatSys = SYSEngine::GetCombatSystem();
	  ASSERT(pCombatSys);	  
  	  if (!pCombatSys->IsCombatActive()) {
		// No, ¿la criatura esta viva?
		CCriature* const pCriatureDest = m_pWorld->GetCriature(pEntityDest->GetHandle());
		ASSERT(pCriatureDest);
		if (pCriatureDest->GetHealth(RulesDefs::TEMP_VALUE) < 1) {
		  // No, se añade opcionde registro
		  m_MenusInfo.MainMenu.push_back(CGUIWEntityInteract::ID_REGISTER);
		} else {
		  // Si, se añade opcion de hablar solo si hay script asociado
		  if (!pCriatureDest->GetScriptEvent(RulesDefs::SE_ONTALK).empty()) {	
			m_MenusInfo.MainMenu.push_back(CGUIWEntityInteract::ID_TALK);
		  }
		}
  	  }
	} break;

	case RulesDefs::ITEM: { 
	  // Se registra la opcion de hablar si no hay combate activo y si la
	  // entidad destino tiene asociado un script asociado al evento
	  iCCombatSystem* const pCombatSys = SYSEngine::GetCombatSystem();
	  ASSERT(pCombatSys);	  
	  if (!pCombatSys->IsCombatActive() &&
		  !pEntityDest->GetScriptEvent(RulesDefs::SE_ONTALK).empty()) {	
		m_MenusInfo.MainMenu.push_back(CGUIWEntityInteract::ID_TALK);
	  }

	  // Recoger
	  m_MenusInfo.MainMenu.push_back(CGUIWEntityInteract::ID_GET);
	} break;

	case RulesDefs::SCENE_OBJ:
	case RulesDefs::WALL: {
	  // Se registra la opcion de hablar si no hay combate activo y si la
	  // entidad destino tiene asociado un script asociado al evento
	  iCCombatSystem* const pCombatSys = SYSEngine::GetCombatSystem();
	  ASSERT(pCombatSys);
	  if (!pCombatSys->IsCombatActive() &&
		  !pEntityDest->GetScriptEvent(RulesDefs::SE_ONTALK).empty()) {	
		m_MenusInfo.MainMenu.push_back(CGUIWEntityInteract::ID_TALK);
	  }

	  // Manipular
	  // ¿Se esta en modo combate?
	  if (pCombatSys->IsCombatActive()) {
		// Solo se podra manipular en caso de que la entidad NO SEA un
		// contenedor y ademas, tenga un script asociado
		if (!pEntityDest->GetItemContainer() &&
		    !pEntityDest->GetScriptEvent(RulesDefs::SE_ONMANIPULATE).empty()) {
		  m_MenusInfo.MainMenu.push_back(CGUIWEntityInteract::ID_MANIPULATE);
		}
	  } else {
		// En este caso, si estamos ante un objeto de escenario que sea un
		// contenedor, asociaremos sin mas, en caso contrario SOLO si hay
		// un script asociado
		if (pEntityDest->GetItemContainer() ||
		    !pEntityDest->GetScriptEvent(RulesDefs::SE_ONMANIPULATE).empty()) {
		  m_MenusInfo.MainMenu.push_back(CGUIWEntityInteract::ID_MANIPULATE);
		}
	  }
	} break;
  }; // ~ switch    

  // Se registra la opcion de usar habilidades si procede
 if (!m_MenusInfo.HabilityMenu.empty()) {
	m_MenusInfo.MainMenu.push_back(CGUIWEntityInteract::ID_USEHABILITY);
  }

  // Se registra la opcion de usar desde inventario siempre
  m_MenusInfo.MainMenu.push_back(CGUIWEntityInteract::ID_USEINVENTORY);
  
  // Se establece la opcion de salir
  m_MenusInfo.MainMenu.push_back(CGUIWEntityInteract::ID_MAINMENUEXIT);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se crea el menu principal de uso en inventario.
// - Las opciones en inventario seran:
//   * Tirar. Obligatoria si no bIsItemOfEntitySrc.
//   * Observar. Si se puede observar dicho item (hay script).
//   * Manipular. Si se puede manipular dicho item (hay script y bIsItemOfEntitySrc)
//   * Usar Habilidad. Si se formo el menu de habilidades y bIsItemOfEntitySrc.
// Parametros:
// - pEntityDest. Entidad (es seguro un item) que recibe la accion.
// - bIsItemOfEntitySrc. Indica si el item pEntityDest pertenece o no a la
//   entidad que inicia la accion de interaccion sobre el.
// Devuelve:
// Notas:
// - La opcion de hablar no se podra desde el menu de interaccion dentro
//   de un inventario.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWEntityInteract::CreateInInventoryMainMenu(CWorldEntity* const pEntityDest,
											   const bool bIsItemOfEntitySrc)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(pEntityDest);
  ASSERT((RulesDefs::ITEM == pEntityDest->GetEntityType()) != 0);

  // ¿El item pEntityDest pertenece a la criatura pEntitySrc?
  if (bIsItemOfEntitySrc) {
	// Se registra la opcion de tirar item
	m_MenusInfo.MainMenu.push_back(CGUIWEntityInteract::ID_DROPITEM);

	// Se registra la de manipular si el item tiene asociado un script
	if (!pEntityDest->GetScriptEvent(RulesDefs::SE_ONMANIPULATE).empty()) {
	  m_MenusInfo.MainMenu.push_back(CGUIWEntityInteract::ID_MANIPULATE);
	}

	// Se registra la opcion de usar habilidades si procede
	if (!m_MenusInfo.HabilityMenu.empty()) {
	  m_MenusInfo.MainMenu.push_back(CGUIWEntityInteract::ID_USEHABILITY);
	}
  }

  // Se registra la de observar si la entidad destino tiene asociado script
  if (!pEntityDest->GetScriptEvent(RulesDefs::SE_ONOBSERVE).empty()) {
	m_MenusInfo.MainMenu.push_back(CGUIWEntityInteract::ID_OBSERVE);
  }
    
  // ¿Se registro alguna opcion?
  if (!m_MenusInfo.MainMenu.empty()) {
	// Si, se registra la opcion de salir
	m_MenusInfo.MainMenu.push_back(CGUIWEntityInteract::ID_MAINMENUEXIT);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa / desactiva las opciones del menu Menu.
// Parametros:
// - Menu. Referencia al menu sobre el que actuar.
// - bActive. Flag de activacion / desactivacion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWEntityInteract::ActiveMenu(OptionsVector& Menu,
								const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Se recorre menu activando / desactivando
  OptionsVectorIt MenuIt(Menu.begin());
  for (; MenuIt != Menu.end(); ++MenuIt) {
	// Se obtiene instancia a opcion y se activa / desactiva
	GetOption(*MenuIt)->Option.SetActive(bActive);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Calcula la posicion correcta para los componentes que se hallan en los
//   menus principal y de habilidades.
// - La posicion correcta a la izq. supondra tener una posicion en X superior
//   a 10 y a la dcha. que la posicion de la ultima opcion en el menu
//   de maximas opciones no sobrepase AnchoPantalla - 10.
// Parametros:
// - InitDrawPos. Posicion inicial de dibujado sin corregir.
// Devuelve:
// - La posicion correcta de inicio de dibujado
// Notas:
//////////////////////////////////////////////////////////////////////////////
sPosition
CGUIWEntityInteract::GetCorrectInitDrawPos(const sPosition& InitDrawPos) 
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Vbles
  sPosition CorrectInitDrawPos(InitDrawPos); // Pos. inicial correcta

  // Correccion en horizontal
  // ¿Hay que corregir posicion por la izq.?
  if (CorrectInitDrawPos.swXPos < 10) {
	// Si, se establece en posicion 10
	CorrectInitDrawPos.swXPos = 10;
  } else {
	// Se comprueba la posicion maxima del menu con mas opciones
	const word uwMaxSize = (m_MenusInfo.MainMenu.size() > m_MenusInfo.HabilityMenu.size()) ?
					        m_MenusInfo.MainMenu.size() : m_MenusInfo.HabilityMenu.size();
	const word uwMaxXPos = InitDrawPos.swXPos + (uwMaxSize * GetOption(m_MenusInfo.MainMenu.front())->Option.GetWidth());
	// ¿Hay que corregir posicion por la dcha.?
	const word uwMaxXTopPos = SYSEngine::GetGraphicSystem()->GetVideoWidth() - 10;
	if (uwMaxXPos > uwMaxXTopPos) {
	  CorrectInitDrawPos.swXPos -= (uwMaxXPos - uwMaxXTopPos);
	}
  }

  // Correccion en vertical
  // ¿El interfaz se sale por arriba?
  if (CorrectInitDrawPos.swYPos < 10) {
	CorrectInitDrawPos.swYPos = 10;
  } else {
	// ¿Lo hace por abajo?
	const sword swMaxYPos = CorrectInitDrawPos.swYPos + 							
							GetOption(m_MenusInfo.MainMenu.front())->Option.GetHeight();
	const sword swMaxYTopPos = (SYSEngine::GetGraphicSystem()->GetVideoHeight() - 96) - 10;
	if (swMaxYPos > swMaxYTopPos) {
	  CorrectInitDrawPos.swYPos -= (swMaxYPos - swMaxYTopPos);
	}
  }

  // Se devuelve la posicion correcta de inicio de dibujado
  return CorrectInitDrawPos;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece para el menu Menu la posicion de dibujado de las mismas a partir
//   de la posicion de inicio InitDrawPos.
// Parametros:
// - Menu. Referencia al menu para el que establecer la posicion de dibujado.
// - InitDrawPos. Posicion inicial de dibujado.
// Devuelve:
// Notas:
// - Este metodo se esperara que sea llamado despues de que el metodo
//   GetCorrectInitDrawPos haya sido ejecutado.
//////////////////////////////////////////////////////////////////////////////
void
CGUIWEntityInteract::SetMenuOptionsDrawPos(OptionsVector& Menu,
										   const sPosition& InitDrawPos)
{
  // SOLO si instancia ha sido inicializada
  ASSERT(Inherited::IsInitOk());

  // Vbles
  sPosition ActDrawPos(InitDrawPos); // Pos. actual de dibujado

  // Se recorre menu activando / desactivando
  OptionsVectorIt MenuIt(Menu.begin());
  for (; MenuIt != Menu.end(); ++MenuIt) {
	// Se obtiene instancia a opcion
	sNOption* const pOption = GetOption(*MenuIt);
	ASSERT(pOption);

	// Se estblece posicion y actualiza
	pOption->Option.SetXPos(ActDrawPos.swXPos);
	pOption->Option.SetYPos(ActDrawPos.swYPos);
	ActDrawPos.swXPos += pOption->Option.GetWidth();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Responde a la notificacion del evento recibido. Segun sea la notificacion
// - En el caso de la notificacion de seleccion / deseleccion, se asociara
//   el componente alpha solido si hay seleccion y el valor que se hubiera
//   establecido en el fichero de configuracion si hay deseleccion.
// Parametros:
// - GUICEvent. Evento con el ID del boton.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWEntityInteract::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
    
  // Se comprueba el evento recibido
  switch (GUICEvent.Event) {
	case GUIDefs::GUIC_SELECT: {
	  // Opcion seleccionada
	  OnOptionSelect(GUICEvent.IDComponent, true);
	} break;

	case GUIDefs::GUIC_UNSELECT: {
	  // Opcion no seleccionada	  
	  OnOptionSelect(GUICEvent.IDComponent, false);
	} break;  
	
	case GUIDefs::GUIC_BUTTONLEFT_PRESSED: {	
	  // Pulsacion con el boton izquierdo
	  OnOptionLeftClick(GUICEvent.IDComponent);
	} break;	  
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Selecciona / deselecciona el boton de opcion que proceda, estableciendo
//   ademas el texto flotante adecuado.
// Parametros:
// - IDOption. Opcion seleccionada.
// - bSelect. Flag de seleccion / deseleccion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWEntityInteract::OnOptionSelect(const GUIDefs::GUIIDComponent& IDOption,
									const bool bSelect)
{
  // SOLO si instancia activa
  ASSERT(Inherited::IsInitOk());

  // Dependiendo de la opcion, se toma el string asociado para el texto flotante
  std::string szFloatingText;
  switch(IDOption) {
	case CGUIWEntityInteract::ID_USEHABILITY: {
	  szFloatingText = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_MAINMENU_USEHABILITY);	  
	} break;

	case CGUIWEntityInteract::ID_MAINMENUEXIT: {
	  szFloatingText = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_MAINMENU_EXIT);
	} break;

	case CGUIWEntityInteract::ID_HABILITYMENUEXIT: {
	  szFloatingText = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_HABILITYMENU_RETURN);
	} break;

	case CGUIWEntityInteract::ID_OBSERVE: {	  
	  szFloatingText = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_MAINMENU_OBSERVE);	  
	} break;

	case CGUIWEntityInteract::ID_TALK: {	  
	  szFloatingText = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_MAIMENU_TALK);
	} break;
	
	case CGUIWEntityInteract::ID_REGISTER: {
	  szFloatingText = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_MAINMENU_REGISTER);	  	  
	} break;
	
	case CGUIWEntityInteract::ID_GET: {	  
	  szFloatingText = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_MAINMENU_GET);	  	  	  
	} break;
	
	case CGUIWEntityInteract::ID_MANIPULATE: {
	  szFloatingText = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_MAINMENU_MANIPULATE);
	} break;

	case CGUIWEntityInteract::ID_DROPITEM: { 
	  szFloatingText = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_MAINMENU_DROPITEM);	  
	} break;

	case CGUIWEntityInteract::ID_USEINVENTORY: {
	  szFloatingText = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_MAINMENU_USEINVENTORY);	  	  
	} break;

	case CGUIWEntityInteract::ID_HABILITY00:
	case CGUIWEntityInteract::ID_HABILITY01:
	case CGUIWEntityInteract::ID_HABILITY02:
	case CGUIWEntityInteract::ID_HABILITY03:
	case CGUIWEntityInteract::ID_HABILITY04:
	case CGUIWEntityInteract::ID_HABILITY05:
	case CGUIWEntityInteract::ID_HABILITY06:
	case CGUIWEntityInteract::ID_HABILITY07:{
	  szFloatingText = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_INTERFAZ_ENTITYINTERACT_HABILITYMENU_HABILITY,
												IDOption - CGUIWEntityInteract::ID_HABILITY00);
	} break;

	default: {
	  ASSERT_MSG(false, "Identificador de opción no reconocido");
	} break;
  }; // ~ switch

  // ¿Hay deseleccion?
  GraphDefs::sAlphaValue Alpha;
  if (!bSelect) {
	// Si, se deselecciona solo si procede	
	if (0 == m_MenusInfo.szActFloatingText.compare(szFloatingText)) {
	  m_pGUIManager->UnsetFloatingText(szFloatingText);
	  m_MenusInfo.szActFloatingText = "";
	}
	
	// Se establece grado del alpha
	Alpha = m_MenusInfo.NoSelectAlpha;
  } else {
	// No, se establece el nuevo texto
	m_pGUIManager->SetFloatingText(szFloatingText, 
								   m_MenusInfo.RGBFlotaingText,
								   GraphDefs::sAlphaValue(255),
								   GUIManagerDefs::DISSOLVE_FLOATINGTEXT);
	m_MenusInfo.szActFloatingText = szFloatingText;

	// Se establece grado del alpha
	Alpha = GraphDefs::sAlphaValue(255);
  }

  // Se toma la opcion y se selecciona / deselecciona
  sNOption* const pOption = GetOption(IDOption);
  pOption->Option.Select(bSelect);

  // Se establece el valor alpha  
  pOption->Option.SetAlpha(Alpha);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la pulsacion de una opcion. Desde aqui se tomaran
//   las acciones pertinentes y se produciran los eventos script que procedan.
// Parametros:
// - IDOption. Opcion sobre la que se actuo.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWEntityInteract::OnOptionLeftClick(const GUIDefs::GUIIDComponent& IDOption)
{
  // SOLO si instancia activa
  ASSERT(Inherited::IsInitOk());

  // Dependiendo del boton pulsado, se escoge la accion
  switch(IDOption) {
	case CGUIWEntityInteract::ID_USEHABILITY: {
	  // Se cambia a menu de habilidades
	  m_MenusInfo.pActMenu = &m_MenusInfo.HabilityMenu;	  
	  ActiveMenu(m_MenusInfo.MainMenu, false);
	  ActiveMenu(m_MenusInfo.HabilityMenu, true);	  
	} break;

	case CGUIWEntityInteract::ID_MAINMENUEXIT: {
	  // Se abandona al interfaz de ventana principal
	 ExitFromInterfaz();
	} break;

	case CGUIWEntityInteract::ID_HABILITYMENUEXIT: {
	  // Se cambia a menu principal
	  m_MenusInfo.pActMenu = &m_MenusInfo.MainMenu;	  	  
	  ActiveMenu(m_MenusInfo.HabilityMenu, false);	  
	  ActiveMenu(m_MenusInfo.MainMenu, true);
	} break;

	case CGUIWEntityInteract::ID_OBSERVE: {	  
	  // Se ordena a la criatura observar y abandona
	  ExitFromInterfaz();
	  m_EntitiesInfo.pEntitySrc->Observe(m_EntitiesInfo.pEntityDest->GetHandle());
	} break;

	case CGUIWEntityInteract::ID_TALK: {	  
	  // Se manadara un evento de hablar al script entre la entidad origen
	  // y la entidad destino para que si hay codigo asociado, el usuario
	  // pueda activar el interfaz si asi lo desea y si no hay codigo asociado,
	  // se active automaticamente

	  // Finaliza cualquier tipo de seleccion en la entidad que recibe la accion
	  // y se envia el evento script	  	  
	  ExitFromInterfaz();
	  m_EntitiesInfo.pEntityDest->OnTalk(m_EntitiesInfo.pEntitySrc->GetHandle());
	} break;
	
	case CGUIWEntityInteract::ID_REGISTER: {
	  // El registro sobre una criatura supondra que esta se hallara muerta
	  // y se activara automaticamente el interfaz de intercambio de items
	  ExitFromInterfaz();	  
	  m_EntitiesInfo.pEntitySrc->Manipulate(m_EntitiesInfo.pEntityDest->GetHandle());
	} break;
	
	case CGUIWEntityInteract::ID_GET: {	  
	  // Se ordena a la criatura recoger item y abandona
	  ExitFromInterfaz();	  
	  m_EntitiesInfo.pEntitySrc->GetItem(m_EntitiesInfo.pEntityDest->GetHandle());
	} break;
	
	case CGUIWEntityInteract::ID_MANIPULATE: {
	  // Se ordena a la criatura manipular entidad y abandona
	  ExitFromInterfaz();	  
	  m_EntitiesInfo.pEntitySrc->Manipulate(m_EntitiesInfo.pEntityDest->GetHandle());
	} break;

	case CGUIWEntityInteract::ID_DROPITEM: { 
	  // Se ordena a la criatura tirar el item y se abandona	  	  
	  ExitFromInterfaz();	  
	  m_EntitiesInfo.pEntitySrc->DropItem(m_EntitiesInfo.pEntityDest->GetHandle());
	} break;

	case CGUIWEntityInteract::ID_USEINVENTORY: {
	  // Uso de un item de inventario sobre la entidad que proceda	  
	  // Se activa la interfaz de seleccion del item y una vez activada y si
	  // se escoge un item, se realizara la operacion de usar el item
	  m_pGUIManager->SetItemSelectToUseWindow(m_EntitiesInfo.pEntitySrc,
											  m_EntitiesInfo.pEntityDest);
	} break;

	case CGUIWEntityInteract::ID_HABILITY00:
	case CGUIWEntityInteract::ID_HABILITY01:
	case CGUIWEntityInteract::ID_HABILITY02:
	case CGUIWEntityInteract::ID_HABILITY03:
	case CGUIWEntityInteract::ID_HABILITY04:
	case CGUIWEntityInteract::ID_HABILITY05:
	case CGUIWEntityInteract::ID_HABILITY06:
	case CGUIWEntityInteract::ID_HABILITY07:{
	  // Uso de una habilidad, se halla el identificador y se ordena la accion
	  const RulesDefs::eIDHability IDHab = RulesDefs::eIDHability(RulesDefs::HABILITY_01 << (IDOption - CGUIWEntityInteract::ID_HABILITY00));
	  ExitFromInterfaz();	  	  
	  m_EntitiesInfo.pEntitySrc->UseHability(m_EntitiesInfo.pEntityDest->GetHandle(),
											 IDHab);	  
	} break;
  }; // ~ switch

  // Desactiva texto flotante asociado
  m_pGUIManager->UnsetFloatingText(m_MenusInfo.szActFloatingText);	  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de saber si se ha pulsado ESC o el boton derecho y se debe de 
//   abandonar el menu que actualmente este seleccionado.
// Parametros:
// - aInputEvent. Informacion sobre el evento de entrada.
// Devuelve:
// - Flag para indicar si el evento debe de compartirse o no.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWEntityInteract::DispatchEvent(const InputDefs::sInputEvent& aInputEvent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Los dos unicos eventos que pueden venir son el de la tecla ESC o bien
  // el de la pulsacion del boton derecho. En ambos casos, el menu flotante
  // se cerrara o bien se retornara al nivel superior en caso de hallarse
  // en el menu de habilidades.
  // Evalua el evento producido 
  switch(aInputEvent.EventType) {	
	case InputDefs::BUTTON_EVENT: {	
	  // Evento de pulsacion; se evaluan subeventos
	  switch (aInputEvent.EventButton.Code) {
		case InputDefs::IE_KEY_ESC: 
		case InputDefs::IE_BUTTON_MOUSE_RIGHT: {
		  // Se pulso la tecla ESC o el boton derecho del raton		  
		  // Se simulara la salida del menu activo que proceda
		  ASSERT(m_MenusInfo.pActMenu);
		  if (m_MenusInfo.pActMenu == &m_MenusInfo.MainMenu) {
			// Se esta en menu principal
			OnOptionLeftClick(CGUIWEntityInteract::ID_MAINMENUEXIT);
		  } else {
			// Se esta en el menu de habilidades
			OnOptionLeftClick(CGUIWEntityInteract::ID_HABILITYMENUEXIT);
		  }
		} break;
	  }; // ~ switch
	} break;	
  }; // ~ switch

  // No se comparte el evento
  return InputDefs::NO_SHARE_INPUT_EVENT;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encargara de capturar la pulsacion de la tecla ESC para salir de forma
//   rapida del menu de juego.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWEntityInteract::InstallClient(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se instalan eventos
  iCInputManager* const pInputManager = SYSEngine::GetInputManager();
  ASSERT(pInputManager);
  pInputManager->SetInputEvent(InputDefs::IE_KEY_ESC, 
							   this, 
							   InputDefs::BUTTON_PRESSED_DOWN);
  pInputManager->SetInputEvent(InputDefs::IE_BUTTON_MOUSE_RIGHT, 
							   this, 
							   InputDefs::BUTTON_PRESSED_DOWN);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desinstala eventos de entrada para los que es cliente.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWEntityInteract::UnistallClient(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se desinstalan eventos
  iCInputManager* const pInputManager = SYSEngine::GetInputManager();
  ASSERT(pInputManager);  
  pInputManager->UnsetInputEvent(InputDefs::IE_KEY_ESC, 
								 this, 
								 InputDefs::BUTTON_PRESSED_DOWN);  
  pInputManager->UnsetInputEvent(InputDefs::IE_BUTTON_MOUSE_RIGHT, 
								 this, 
								 InputDefs::BUTTON_PRESSED_DOWN);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la notificacion de un suceso en CWorld.
// - Seran de interes las notificaciones:
//    * Destruccion de una entidad.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWEntityInteract::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
										 const dword udParam)
{
  // SOLO si instancia inicializada y activada
  ASSERT(Inherited::IsInitOk());
  ASSERT(Inherited::IsActive());

  // Comprueba el tipo de notificacion
  switch(NotifyType) {
	case WorldObserverDefs::ENTITY_DESTROY: {
	  // ¿Es la entidad destruida la asociada?
	  if (udParam == m_EntitiesInfo.pEntityDest->GetHandle()) {
		// Si, se cierra de inmediato la interfaz
		ExitFromInterfaz();
	  }
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion sobre un suceso sobre la entidad sobre la que se desea
//   interactuar. El objetivo sera salir de la interfaz si la criatura 
//   cambia de posicion.
// Parametros:
// - hCriature. Handle a la criatura.
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWEntityInteract::CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
										    const CriatureObserverDefs::eObserverNotifyType& NotifyType,
										    const dword udParam)
{
  // SOLO si intancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Si, se captura el suceso
  switch (NotifyType) {
	case CriatureObserverDefs::ACTION_REALICE: {
	  // Se ha realizado una accion
	  // Se comprueba la accion realizada
	  const RulesDefs::eCriatureAction Action = RulesDefs::eCriatureAction(udParam & 0x0000FFFF);
	  switch(Action) {
		case RulesDefs::CA_WALK: {
		  // La entidad destino se ha movido, si resulta que ha cambiado su
		  // pos. original la interfaz se desactivara
		  if (m_EntitiesInfo.EntityDestPos != m_EntitiesInfo.pEntityDest->GetTilePos()) {
			// Se cierra de inmediato la interfaz
			ExitFromInterfaz();
		  }			  
		} break;			
	  }; // ~ switch
	}
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dibuja todos los componentes asociados al menu actual
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWEntityInteract::Draw(void)
{
  // SOLO si instancia inicializadas
  ASSERT(Inherited::IsInitOk());

  // Dibuja si procede
  if (IsActive()) {
	// Recorre menu actual
	OptionsVectorIt It(m_MenusInfo.pActMenu->begin());
	for (; It != m_MenusInfo.pActMenu->end(); ++It) {
	  // Obtiene opcion y la dibuja
	  GetOption(*It)->Option.Draw();
	}
  }
}
