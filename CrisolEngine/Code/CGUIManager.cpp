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
// CGUIManager.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIManager.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIManager.h"

#include "SYSEngine.h"
#include "iCGraphicSystem.h"
#include "iCCombatSystem.h"
#include "iCCommandManager.h"
#include "iCGameDataBase.h"
#include "iCWorld.h"
#include "iCLogger.h"
#include "iCInputManager.h"
#include "iCVirtualMachine.h"
#include "CCBTEngineParser.h"
#include "CSprite.h"

// Inicializacion de la unica instancia singlenton
CGUIManager* CGUIManager::m_pGUIWindowManager = NULL;

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia. Todos los menus que sean de uso habitual seran
//   inicializados sin activar desde este metodo. 
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. Si ha exitido algun problema false.
// Notas:
// - Todos los subsistemas de interfaz que se inicialicen se dejaran inactivos.
// - No todos los subsistemas de interfaz seran alojados en memoria, solo 
//   seran llevados a  memoria (se cargaran los datos) los interfaces que sean
//   de uso habitual durante el juego. El resto, se llevaran a memoria segun
//   sean necesitados.
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIManager::Init(void)
  
{
  // ¿Se intenta reicinializar?
  if (IsInitOk()) { 
	return false; 
  }

  // Se toman interfaces a otros modulos del motor  
  m_pWorld = SYSEngine::GetWorld();
  ASSERT(m_pWorld);
  m_pGraphSys = SYSEngine::GetGraphicSystem();
  ASSERT(m_pGraphSys);
  m_pInputManager = SYSEngine::GetInputManager();
  ASSERT(m_pInputManager);
  m_pCmdManager = SYSEngine::GetCommandManager();
  ASSERT(m_pCmdManager);
  
  #ifdef ENGINE_TRACE
     SYSEngine::GetLogger()->Write("CGUIManager::Init> Inicializando manager de interfaz.\n");
  #endif

  // Se levanta flag para permitir llamadas a End en caso de fallo
  m_bIsInitOk = true;

  // Se toma parser al archivo de configuraciones de interfaz y se inicializan
  // los cursores de GUI
  CCBTEngineParser* const pParser = SYSEngine::GetGameDataBase()->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
																			   "[Cursors]");
  ASSERT(pParser);

  // Se inicializan cursores de GUI
  pParser->SetVarPrefix("");
  if (InitGUICursors(pParser->ReadStringID("Movement.AnimTemplate"),
				     pParser->ReadStringID("EntityInteract.AnimTemplate"),
					 pParser->ReadStringID("EntityHit.AnimTemplate"),
					 pParser->ReadStringID("Waiting.AnimTemplate"),
				     pParser->ReadStringID("MapScroll.AnimTemplate"),
				     pParser->ReadStringID("GUIInteract.AnimTemplate"))) {	
	// Se inicializa la cortina de cine
	if (InitCutSceneInfo()) { 	  
	  // Se inicializa selector de entidades
	  CCBTEngineParser* const pParser = SYSEngine::GetGameDataBase()->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
																			       "[Selectors]");
	  
	  ASSERT(pParser);
	  pParser->SetVarPrefix("Map.");
	  m_MapSelector.Init(pParser->ReadStringID("AnimTemplate"));

	  // Se inicializan datos base de texto flotante
  	  InitFloatingTextCfg();

	  // Se establecen datos asociados a interfaces de juego
	  m_GameInterfaces.GUIWindowState = GUIManagerDefs::GUIW_NO_TYPE;
	  m_GameInterfaces.bInputActive = false;

	  // Todo correcto  
	  #ifdef ENGINE_TRACE
	    SYSEngine::GetLogger()->Write("                 | Ok.\n");
	  #endif 
	  return true;	  	    
	}	
  }
 
  // Hubo problemas
  End();    
  #ifdef ENGINE_TRACE
     SYSEngine::GetLogger()->Write("                 | Error.\n");
  #endif
  return false;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa los datos asociados al efecto de una cutscene
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIManager::InitCutSceneInfo(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Inicializa cortina de cine y retorna
  m_CutSceneInfo.CinemaCurtain.Init(CGUIManager::CINEMACURTAIN_TOPHEIGHT,
									CGUIManager::CINEMACURTAIN_BASEHEIGHT);
  m_CutSceneInfo.pClient = NULL;
  return m_CutSceneInfo.CinemaCurtain.IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece la informacion asociada a los cursores de interfaz.
// Parametros:
// - szATGUIMovementCursor. Template al cursor de movimiento
// - szATGUIEntityInteractCursor. Template al cursor de interactuar con 
//   entidades
// - szATGUIEntityInteractForHitCursor. Archivo al cursor de interactuar 
//   con entidades pero teniendo como objetivo el golpear.
// - szATGUIWaitingCursor. Template al cursor de espera
// - szATGUIScrollCursor. Template al cursor de scroll
// - szATGUIWindowInteractCursor. Template al cursor de interactuar con 
//   ventanas de interfaz.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
// - Todos los cursores se inicializaran con el estado de animacion desactivado
//   y este se activara unicamente cuando un cursor sea seleccionado.
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIManager::InitGUICursors(const std::string& szATGUIMovementCursor,
							const std::string& szATGUIEntityInteractCursor,
							const std::string& szATGUIEntityInteractForHitCursor,
							const std::string& szATGUIWaitingCursor,
							const std::string& szATGUIScrollCursor,
							const std::string& szATGUIWindowInteractCursor)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Inicializa cursor de movimiento
  if (!m_GUICursorsInfo.GUIMovementCursor.Init(szATGUIMovementCursor, 0, 0, false)) {
	return false; 
  }

  // Inicializa cursor de interaccion con entidades  
  if (!m_GUICursorsInfo.GUIEntityInteractCursor.Init(szATGUIEntityInteractCursor, 0, 0, false)) { 	
	return false; 
  }

  // Inicializa cursor de interaccion con entidades para golpear
  if (!m_GUICursorsInfo.GUIEntityInteractForHitCursor.Init(szATGUIEntityInteractForHitCursor, 0, 0, false)) {
	return false;
  }

  // Inicializa cursor de espera
  if (!m_GUICursorsInfo.GUIWaitingCursor.Init(szATGUIWaitingCursor, 0, 0, false)) {
	return false;
  }

  // Inicializa cursor de scroll
  if (!m_GUICursorsInfo.GUIIsoScrollCursor.Init(szATGUIScrollCursor, 0, 0, false)) { 	
	return false; 
  }

  // Inicializa cursor de interactuar con ventanas de interfaz
  if (!m_GUICursorsInfo.GUIWindowInteractCursor.Init(szATGUIWindowInteractCursor, 0, 0, false)) { 	
	return false; 
  }
  
  // No establece ningun cursor como actual y 
  // bloquea el control de eventos de raton
  m_GUICursorsInfo.ActCursor = GUIManagerDefs::NO_CURSOR;
  m_GUICursorsInfo.pActCursor = NULL;
  
  // Se obtiene resolucion de pantalla para optimizar calculos
  m_GUICursorsInfo.uwGUICursorsWidth = m_GUICursorsInfo.GUIIsoScrollCursor.GetWidth();
  m_GUICursorsInfo.uwGUICursorsHeight = m_GUICursorsInfo.GUIIsoScrollCursor.GetHeight();
  m_GUICursorsInfo.uwScrollRightTopPos = m_pGraphSys->GetVideoWidth() - 
	                                       m_GUICursorsInfo.uwGUICursorsWidth;
  m_GUICursorsInfo.uwScrollBottomTopPos = m_pGraphSys->GetVideoHeight() - 
											m_GUICursorsInfo.uwGUICursorsHeight;  
  
  // Sin sprite asociado
  m_GUICursorsInfo.pSpriteAttached = NULL;

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la configuracion base para el texto flotante.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGUIManager::InitFloatingTextCfg(void)
{
  // SOLO si intancia inicializada
  ASSERT(IsInitOk());

  // Establece valores base para la inicialacion del componente de texto
  m_FloatingTextInfo.CfgBase.ID = CGUIManager::ID_FLOATINGTEXT;
  m_FloatingTextInfo.CfgBase.szFont = "Arial";	
  m_FloatingTextInfo.CfgBase.uwWidthJustify = 96;
  m_FloatingTextInfo.CfgBase.ubDrawPlane = 2;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::End(void)
{
  // Finaliza si procede
  if (IsInitOk()) {
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CGUIManager::End> Finalizando el manager de ventanas de interfaz.\n");
	#endif

	// Se vacia lista de menus activos
	m_ActiveInterfaces.clear();

	// Finaliza selector de entidades
	m_MapSelector.End();

    // Finaliza cursores
	EndGUICursors();

	// Finaliza posible texto flotante
	UnsetFloatingText();

	// Finaliza cortina cine
	EndCutScene();

	// Finaliza interfaces
	m_GameInterfaces.MainMenu.End();
	m_GameInterfaces.GameLoading.End();
	m_GameInterfaces.GameMenu.End();
	m_GameInterfaces.MainInterfaz.End();
	m_GameInterfaces.EntityInteract.End();
	m_GameInterfaces.ItemSelectToUse.End();
	m_GameInterfaces.PlayerProfile.End();
	m_GameInterfaces.Conversator.End();
	m_GameInterfaces.Trade.End();
	m_GameInterfaces.Presentation.End();
	m_GameInterfaces.CharacterCreator.End();
	m_GameInterfaces.Combat.End();
	m_GameInterfaces.Presentation.End();
	m_GameInterfaces.LoaderSavedFile.End();
	
	// Se baja el flag
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("                | Ok.\n");
	#endif
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza datos relacionados con las cortinas de cine.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGUIManager::EndCutSceneInfo(void)
{
  // Finaliza
  m_CutSceneInfo.CinemaCurtain.End();
  m_CutSceneInfo.CloseCurtainCmd.End();
  m_CutSceneInfo.OpenCurtainCmd.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la info asociada a los cursores de GUI
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGUIManager::EndGUICursors(void)
{
  // Finaliza
  m_GUICursorsInfo.GUIMovementCursor.End();
  m_GUICursorsInfo.GUIEntityInteractCursor.End();
  m_GUICursorsInfo.GUIEntityInteractForHitCursor.End();
  m_GUICursorsInfo.GUIWaitingCursor.End();
  m_GUICursorsInfo.GUIIsoScrollCursor.End();
  m_GUICursorsInfo.GUIWindowInteractCursor.End();
  m_GUICursorsInfo.pActCursor = NULL;
  m_GUICursorsInfo.pSpriteAttached = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece ventana de menu principal. Esta es la ventana de interfaz que
//   saldra nada mas arrancar el juego.
// Parametros:
// Devuelve:
// Notas:
// - Transiciones validas desde:
//   * Estado nulo
//   * Menu de juego
//   * Creacion de personajes / cancelacion
//   * Carga de personajes / cancelacion
//   * Presentacion (creditos)
//   * MainInterfaz / PlayerProfile (Cuando sea solicitado el quitar el
//     juego desde CrisolScript).
// - Al regresar desde el menu de juego, se finalizaran todos los interfaces
//   que halla en este cargados en memoria.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::SetMainMenuWindow(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se efectua logica segun estado actual
  switch (m_GameInterfaces.GUIWindowState) {
	case GUIManagerDefs::GUIW_NO_TYPE: {
	} break;

	case GUIManagerDefs::GUIW_GAMEMENU: {
	  // Desactiva GameMenu, MainInterfaz y combate si estuviera activo
	  DeactiveGameMenu();
	  DeactiveMainInterfaz();	  
	  SetCombatWindow(0);	  

	  // Finaliza interfaces de juego que pudieran estar activos
	  m_GameInterfaces.MainInterfaz.End();
	  m_GameInterfaces.EntityInteract.End();
	  m_GameInterfaces.ItemSelectToUse.End();
	  m_GameInterfaces.Conversator.End();	  
	  m_GameInterfaces.Trade.End();	  	  
	  m_GameInterfaces.Combat.End();
	} break;
	  
	case GUIManagerDefs::GUIW_MAININTERFAZ: {
	  // Desactiva MainInterfaz y combate si estuviera activo
	  DeactiveMainInterfaz();
	  SetCombatWindow(0);

	  // Cierra cutscene de forma inmediata si procede
	  if (m_CutSceneInfo.CinemaCurtain.IsOpen()) {		
		m_CutSceneInfo.CinemaCurtain.CloseCurtain(m_pGraphSys->GetVideoHeight());
	  }
	  
	  // Finaliza interfaces de juego que pudieran estar activos
	  m_GameInterfaces.MainInterfaz.End();
	  m_GameInterfaces.EntityInteract.End();
	  m_GameInterfaces.ItemSelectToUse.End();
	  m_GameInterfaces.Conversator.End();	  
	  m_GameInterfaces.Trade.End();	  	  
	  m_GameInterfaces.Combat.End();
	} break;
	
	case GUIManagerDefs::GUIW_PLAYERPROFILE: {
	  // Desactiva PlayerProfile y combate si estuviera activo
	  DeactivePlayerProfile();
	  SetCombatWindow(0);

	  // Cierra cutscene de forma inmediata si procede
	  if (m_CutSceneInfo.CinemaCurtain.IsOpen()) {		
		m_CutSceneInfo.CinemaCurtain.CloseCurtain(m_pGraphSys->GetVideoHeight());
	  }	  
	  
	  // Finaliza interfaces de juego que pudieran estar activos
	  m_GameInterfaces.MainInterfaz.End();
	  m_GameInterfaces.PlayerProfile.End();		
	  m_GameInterfaces.PlayerProfile.End();
	  m_GameInterfaces.EntityInteract.End();
	  m_GameInterfaces.ItemSelectToUse.End();
	  m_GameInterfaces.Conversator.End();	  
	  m_GameInterfaces.Trade.End();	  	  
	  m_GameInterfaces.Combat.End();
	} break;
	
	case GUIManagerDefs::GUIW_CHARACTERCREATOR: {
	  // Desactiva el interfaz de creacion de caracteres
	  DeactiveCharacterCreator();
	} break;

	case GUIManagerDefs::GUIW_PRESENTATION: {
	  // Desactiva interfaz de presentacion
	  DeactivePresentation();
	} break;

	case GUIManagerDefs::GUIW_LOADERSAVEDFILEMANAGER: {
	  // Desactiva interfaz de carga de partidas guardadas
	  DeactiveLoaderSavedFile();
	} break;

	default: {
	  // Transicion no valida
	  ASSERT(false);
	} break;
  }; // ~ switch

  // Establece ventana de menu principal
  ActiveMainMenu();	  

  // Se establece estado actual
  m_GameInterfaces.GUIWindowState = GUIManagerDefs::GUIW_MAINMENU;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece la ventana de creacion de caracter para comenzar un juego nuevo.
// Parametros:
// Devuelve:
// Notas:
// - Transiciones validas desde:
//   * Menu principal
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::SetCharacterCreatorWindow(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se efectua logica segun estado actual
  switch (m_GameInterfaces.GUIWindowState) {	
	case GUIManagerDefs::GUIW_MAINMENU: {
	  // Desactiva main interfaz
	  DeactiveMainMenu();

	  // Establece ventana de creacion de caracteres
	  ActiveCharacterCreator();	    
	} break;

	default:
	  // Estado de transicion no valido
	  ASSERT(false);
  }; // ~ switch

  // Se establece estado actual
  m_GameInterfaces.GUIWindowState = GUIManagerDefs::GUIW_CHARACTERCREATOR;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece la ventana indicativa de carga de un juego nuevo
// Parametros:
// - uwIDArea. Identificador del area.
// - pPlayer. Instancia al jugador. Si su valor es NULL significara que se
//   esta cargando un juego a partir de una partida guardada y no se esta
//   comenzando una nueva partida.
// - PlayerPos. Posicion donde situar al jugador.
// Devuelve:
// Notas:
// - Transiciones validas desde:
//   * Creacion de personaje.
//   * Carga de un area
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::SetGameLoadingWindow(const word uwIDArea,
								  CPlayer* const pPlayer,
								  const AreaDefs::sTilePos& PlayerPos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se efectua logica segun estado actual
  switch (m_GameInterfaces.GUIWindowState) {
	case GUIManagerDefs::GUIW_CHARACTERCREATOR: {
	  // Quita ventana de menu principal
	  DeactiveCharacterCreator();	  
	} break;

  	case GUIManagerDefs::GUIW_LOADERSAVEDFILEMANAGER: {
	  // Desactiva interfaz de carga de archivos
	  DeactiveLoaderSavedFile();	  
	} break;	
  }; // ~ switch

  // Establece ventana indicativa de carga de un area
  ActiveGameLoading(uwIDArea, pPlayer, PlayerPos);	  

  // Se establece estado actual
  m_GameInterfaces.GUIWindowState = GUIManagerDefs::GUIW_GAMELOADING;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece ventana de menu principal
// Parametros:
// Devuelve:
// Notas:
// - Transiciones validas desde:
//   * Ventana de interfaz principal
//   * Interfaz de cargar partidas (al dar a salir)
//   * Interfaz de guardar partidas (al dar a salir)
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::SetGameMenuWindow(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se efectua logica segun estado actual
  switch (m_GameInterfaces.GUIWindowState) {
	case GUIManagerDefs::GUIW_MAININTERFAZ: {	  
	  // Bloquea entrada en la ventana de interfaz principal
	  m_GameInterfaces.MainInterfaz.SetInput(false);
	  ASSERT(!m_GameInterfaces.MainInterfaz.IsInput());
	} break;

  	case GUIManagerDefs::GUIW_LOADERSAVEDFILEMANAGER: {
	  // Desactiva interfaz de carga de archivos
	  DeactiveLoaderSavedFile();
	  
	  // Se activa el dibujado del area
	  m_pWorld->SetDrawArea(true);	  
	  
	  // Establece ventana de menu principal sin input
	  ActiveMainInterfaz(false);	  
	  m_GameInterfaces.MainInterfaz.SetInput(false);
	  ASSERT(!m_GameInterfaces.MainInterfaz.IsInput());	  

	  // Si el combate estaba activo, se activa la ventana
	  iCCombatSystem* const pCombatSys = SYSEngine::GetCombatSystem();
	  ASSERT(pCombatSys);
	  if (pCombatSys->IsCombatActive()) {
		ActiveCombatWindow(pCombatSys->GetCriatureInTurn());
	  }
	} break;

	case GUIManagerDefs::GUIW_SAVERSAVEDFILEMANAGER: {
	  // Desactiva interfaz de carga de archivos
	  DeactiveSaverSavedFile();
	  
	  // Se activa el dibujado del area
	  m_pWorld->SetDrawArea(true);	  
	  
	  // Establece ventana de menu principal sin input
	  ActiveMainInterfaz(false);    
	  m_GameInterfaces.MainInterfaz.SetInput(false);
	  ASSERT(!m_GameInterfaces.MainInterfaz.IsInput());

	  // Si el combate estaba activo, se activa la ventana
	  iCCombatSystem* const pCombatSys = SYSEngine::GetCombatSystem();
	  ASSERT(pCombatSys);
	  if (pCombatSys->IsCombatActive()) {
		ActiveCombatWindow(pCombatSys->GetCriatureInTurn());
	  }
	} break;

	default:
	  // Estado de transicion no valido
	  ASSERT(false);
  }; // ~ switch

  // Establece menu de juego
  ActiveGameMenu();

  // Se establece estado actual
  m_GameInterfaces.GUIWindowState = GUIManagerDefs::GUIW_GAMEMENU;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece ventana de interfaz principal
// Parametros:
// Devuelve:
// Notas:
// - Transiciones validas desde:
//   * Estado nulo
//   * Ventana de interaccion con entidades
//   * Ventana de menu de juego
//   * Perfil del jugador
//   * Ventana de seleccion de item para usar sobre entidad seleccionada
//   * Ventana de Conversacion
//   * Ventana de intercambio
//   * Ventana de presentacion
// - Al activar el interfaz principal desde un area loading, se llevaran a
//   memoria por primera vez los interfaces principales para el juego. Esto
//   excluira al menu de juego y a la pantalla del perfil del jugador.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::SetMainInterfazWindow(void)
{
  // Se efectua logica segun estado actual
  switch (m_GameInterfaces.GUIWindowState) {
	case GUIManagerDefs::GUIW_GAMELOADING: {
	  // Desactiva ventana de carga de area
	  DeactiveGameLoading();

	  // ¿El interfaz principal, ya estaba inicializado?
	  if (m_GameInterfaces.MainInterfaz.IsInitOk()) {
		// Si, se activa sin mas
		// Nota: Se estara cargando desde dentro de otra partida
		// Se activa el dibujado del area
		m_pWorld->SetDrawArea(true);	  
	  } else {
		// No, luego se inicializan todos los interfaces de juego
		// Nota: Se esta cargando despues de haber creado un personaje o
		// desde el menu principal, usando una partida salvada
		// Se llevan a memoria los interfaces permanentes de juego
		// Interfaz principal
		m_GameInterfaces.MainInterfaz.Init();
		ASSERT(m_GameInterfaces.MainInterfaz.IsInitOk());

		// Inventario
		m_GameInterfaces.PlayerProfile.Init();
		ASSERT(m_GameInterfaces.PlayerProfile.IsInitOk());
		
		// Interaccion con entidades
		m_GameInterfaces.EntityInteract.Init();
		ASSERT(m_GameInterfaces.EntityInteract.IsInitOk());		
		
		// Seleccion de un item para usar
		m_GameInterfaces.ItemSelectToUse.Init();
		ASSERT(m_GameInterfaces.ItemSelectToUse.IsInitOk());
		
		// Ventana de Conversacion
		m_GameInterfaces.Conversator.Init();
		ASSERT(m_GameInterfaces.Conversator.IsInitOk());	    
		
		// Ventana de intercamio
		m_GameInterfaces.Trade.Init();
		ASSERT(m_GameInterfaces.Trade.IsInitOk());
		
		// Ventana de combate
		m_GameInterfaces.Combat.Init();
		ASSERT(m_GameInterfaces.Combat.IsInitOk());
	  }	  
	  
	  // Establece ventana de menu principal
	  ActiveMainInterfaz(true);
	} break;

	case GUIManagerDefs::GUIW_ENTITYINTERACT: {	  
	  // Desactiva interfaz de interaccion con entidades	  
	  DeactiveEntityInteract();
	  
	  // Desbloquea entrada en ventana de interfaz principal	  
	  m_GameInterfaces.MainInterfaz.SetInput(true);
	} break;

	case GUIManagerDefs::GUIW_GAMEMENU: {
	  // Desactiva menu de juego
	  DeactiveGameMenu();

  	  // Desbloquea entrada en la ventana de interfaz principal
	  m_GameInterfaces.MainInterfaz.SetInput(true);
	  ASSERT(m_GameInterfaces.MainInterfaz.IsInput());
	} break;

	case GUIManagerDefs::GUIW_PLAYERPROFILE: {
	  // Desactiva perfil del jugador
	  DeactivePlayerProfile();	  

	  // Se activa el dibujado del area	  
	  m_pWorld->SetDrawArea(true);	  
	  
	  // Establece ventana de menu principal
	  ActiveMainInterfaz(false);    
	} break;

	case GUIManagerDefs::GUIW_ITEMSELECTTOUSE: {
	  // Se desactiva la ventana de seleccion de item
	  DeactiveItemSelectToUse();	
	  
	  // Desbloquea entrada en ventana de interfaz principal
	  m_GameInterfaces.MainInterfaz.SetInput(true);
	} break;

	case GUIManagerDefs::GUIW_CONVERSATOR: {
	  // Se desactiva ventana de Conversacion
	  DeactiveConversatorWindow();
	  
	  // Establece ventana de menu principal
	  ActiveMainInterfaz(false);    
	} break;

	case GUIManagerDefs::GUIW_TRADE: {
	  // Se desactiva ventana de intercambio
	  DeactiveTradeWindow();

	  // Desbloquea entrada en ventana de interfaz principal
	  m_GameInterfaces.MainInterfaz.SetInput(true);
	} break;

  	case GUIManagerDefs::GUIW_PRESENTATION: {
	  // Desactiva interfaz de presentacion
	  DeactivePresentation();

	  // La interfaz principal SOLO se activara en caso de que NO
	  // este el modo CutScene activo.
	  if (!IsCutSceneActive()) {
		// Activa MainInterfaz
		ActiveMainInterfaz(true);
	  }

	  // Se activa el dibujado del universo de juego y se quita la pausa
	  m_pWorld->SetDrawArea(true);
	} break;
    
	default: {
	  // Estado de transicion no valido
	  return;
	  #ifdef _SYSASSERT
		std::string szText;
		SYSEngine::PassToString(szText, "Estado: %u", m_GameInterfaces.GUIWindowState);
		ASSERT_MSG(false, szText.c_str());
	  #endif
	} break;
  }; // ~ switch
  
  // Se establece estado actual
  m_GameInterfaces.GUIWindowState = GUIManagerDefs::GUIW_MAININTERFAZ;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece ventana de interaccion con entidades de juego.
// - A la hora de establecer esta interfaz se realizara una diferenciacion a
//   la hora de indicar el estado de interfaz, pues si se activa desde
//   el MainInterfaz se entendera que se esta haciendo una interaccion a una
//   entidad (no se sabe que tipo), pero en caso de que se active desde
//   cualquier otro sitio, se sabra que la entidad sera seguro un item.
// Parametros:
// - InitDrawPos. Posicion inicial de dibujado.
// - pEntitySrc. Instancia a entidad origen.
// - pEntityDest. Intancia a entidad destino de la accion
// Devuelve:
// Notas:
// - Transiciones validas desde:
//   * Ventana de interfaz
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::SetEntityInteractWindow(const sPosition& InitDrawPos,
									 CCriature* const pEntitySrc,
									 CWorldEntity* const pEntityDest)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
    
  // Se efectua logica segun estado actual
  switch (m_GameInterfaces.GUIWindowState) {
	case GUIManagerDefs::GUIW_MAININTERFAZ: {	  
	  // Bloquea entrada en ventana de interfaz principal
	  m_GameInterfaces.MainInterfaz.SetInput(false);	  
	} break;

    case GUIManagerDefs::GUIW_PLAYERPROFILE: {	 
	  // Bloque entrada en interfaz de perfil de jugador
	  m_GameInterfaces.PlayerProfile.SetInput(false);
	} break;

	case GUIManagerDefs::GUIW_TRADE: {	 
	  // Bloque entrada en interfaz de intercambio
	  m_GameInterfaces.Trade.SetInput(false);
	} break;

	default:
	  // Estado de transicion no valido
	  ASSERT(false);
  }; // ~ switch
  
  // Se establece estado actual y se activa interfaz
  const GUIManagerDefs::eGUIWindowType ActiveContext = m_GameInterfaces.GUIWindowState;
  m_GameInterfaces.GUIWindowState = GUIManagerDefs::GUIW_ENTITYINTERACT;
  ActiveEntityInteract(InitDrawPos, pEntitySrc, pEntityDest, ActiveContext);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece ventana de seleccion de un item para usarlo sobre una 
//   entidad previamente seleccionada. El item se seleccionara desde inventario.
// Parametros:
// - pCriatureSrc. Criatura que posee el inventario con los items.
// - pEntityDest. Entidad que recibe la accion del uso del item.
// Devuelve:
// Notas:
// - Transiciones validas desde:
//   * Interfaz de interaccion con entidades
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::SetItemSelectToUseWindow(CCriature* const pCriatureSrc,
									  CWorldEntity* const pEntityDest)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pCriatureSrc);
  ASSERT(pEntityDest);

  // Se efectua logica segun estado actual
  switch (m_GameInterfaces.GUIWindowState) {
	case GUIManagerDefs::GUIW_ENTITYINTERACT: {	  
	  // Se desactiva interfaz de interaccion con entidades
	  DeactiveEntityInteract();
	} break;

	default:
	  // Estado de transicion no valido
	  ASSERT(false);
  }; // ~ switch

  // Activa interfaz de seleccion de entidad
  ActiveItemSelectToUse(pCriatureSrc, pEntityDest);	  

  // Se establece estado actual
  m_GameInterfaces.GUIWindowState = GUIManagerDefs::GUIW_ITEMSELECTTOUSE;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece ventana de perfil de jugador.
// Parametros:
// Devuelve:
// Notas:
// - Transiciones validas desde:
//   * Ventana de interfaz principal.
//   * Ventana de interaccion con items.
//   * Ventana de presentacion
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::SetPlayerProfileWindow(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se efectua logica segun estado actual
  switch (m_GameInterfaces.GUIWindowState) {
	case GUIManagerDefs::GUIW_MAININTERFAZ: {
	  // Desactiva ventana de interfaz principal	  
	  DeactiveMainInterfaz();

	  // Se desactiva el dibujado del area	  
	  m_pWorld->SetDrawArea(false);
	  
	  // Activa interfaz de perfil de jugador	  
	  ActivePlayerProfile(false);
	} break;

	case GUIManagerDefs::GUIW_ENTITYINTERACT: {  
	  // Desbloquea la entrada en interfaz de perfil de jugador
	  m_GameInterfaces.PlayerProfile.SetInput(true);

	  // Desactiva interfaz de interaccion con items
	  DeactiveEntityInteract();
	} break;

  	case GUIManagerDefs::GUIW_PRESENTATION: {
	  // Desactiva interfaz de presentacion
	  DeactivePresentation();

	  // Activa PlayerProfile
	  ActivePlayerProfile(true);
	} break;

	default:
	  // Estado de transicion no valido
	  ASSERT(false);
  }; // ~ switch

  // Se establece estado actual
  m_GameInterfaces.GUIWindowState = GUIManagerDefs::GUIW_PLAYERPROFILE;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece ventana para la gestion de Conversaciones.
// - Al trabajar con este interfaz, la activacion no estara asegurada pues
//   sera necesario que se hayan añadido opciones.
// Parametros:
// - pMainConv. Conversador principal.
// - pSecondaryConv. Conversador secondario
// Devuelve:
// - Si se pudo situar true, en caso contrario false.
// Notas:
// - Transiciones validas desde:
//   * Ventana de interaccion con entidad
///////////////////////////////////////////////////////////////////////////////
bool
CGUIManager::SetConversatorWindow(CWorldEntity* const pMainConv,
								  CWorldEntity* const pSecondaryConv)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pMainConv);
  ASSERT(pSecondaryConv);

  // Se efectua logica segun estado actual
  switch (m_GameInterfaces.GUIWindowState) {
	case GUIManagerDefs::GUIW_MAININTERFAZ: {
	  // Intenta activar interfaz de conversacion
	  ActiveConversatorWindow(pMainConv, pSecondaryConv);	  
	  if (m_GameInterfaces.Conversator.IsActive()) {
	    // Se desactiva la ventan de interfaz principal
	    DeactiveMainInterfaz();
		  
	    // Se desactiva interfaz de interaccion con entidades
  	    DeactiveEntityInteract();
		  
	    // Se establece estado actual y retorna
	    m_GameInterfaces.GUIWindowState = GUIManagerDefs::GUIW_CONVERSATOR;
	    return true;
	  }	  
	} break;	
  }; // ~ switch  

  // No fue posible activar el estado
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera todas las posibles opciones asociadas al interfaz de conversacion
//   siempre y cuando se halle activo.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGUIManager::ResetOptionsInConversatorWindow(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Interfaz inicializado?
  if (m_GameInterfaces.Conversator.IsInitOk()) {
	// ¿Esta activo?
	if (m_GameInterfaces.Conversator.IsActive()) {
	  m_GameInterfaces.Conversator.ReleaseOptions();
	}
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece la interfaz asociada al intercambio de datos.
// Parametros:
// - pMainCriature. Instancia a la interfaz de criatura que inicia el 
//   intercambio / exploracion de un contenedor.
// - pEntity. Instancia a le entidad sobre la que se inica la exploracion
//   de su contenedor.
// - pClient. Cliente que sera avisado cuando se cierre el interfaz.
// Devuelve:
// - Si se logro establecer el estado true, en caso contrario false.
// Notas:
// - Transiciones validas desde:
//   * Ventana de interfaz principal
//   * Ventana de interaccion con entidades (items)
///////////////////////////////////////////////////////////////////////////////
bool
CGUIManager::SetTradeWindow(CCriature* const pMainCriature,
							CWorldEntity* const pEntity,
							iCGUIWindowClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pMainCriature);
  ASSERT(pEntity);

  // Se efectua logica segun estado actual
  switch (m_GameInterfaces.GUIWindowState) {
	case GUIManagerDefs::GUIW_MAININTERFAZ: {	  
	  // Bloquea entrada en ventana de interfaz principal
	  m_GameInterfaces.MainInterfaz.SetInput(false);	  
	  
	  // Activa interfaz y en caso de problemas retorna
	  ActiveTradeWindow(pMainCriature, pEntity, pClient);
	  if (!m_GameInterfaces.Trade.IsActive()) {
		m_GameInterfaces.MainInterfaz.SetInput(true);	  
		return false;
	  }
	} break;
	
	case GUIManagerDefs::GUIW_ENTITYINTERACT: {  
	  // Desactiva interfaz de interaccion con items
	  DeactiveEntityInteract();
	  
	  // Desbloquea la entrada en interfaz de perfil de jugador
	  m_GameInterfaces.Trade.SetInput(true);	  
	} break;
	
	default:
	  // Estado de transicion no valido
	  return false;
  }; // ~ switch  

  // Se establece estado actual
  m_GameInterfaces.GUIWindowState = GUIManagerDefs::GUIW_TRADE;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece interfaz de combate.
// - El interfaz de combate podra establecerse y quitarse SOLO cuando se este 
//   en estado de MainInterfaz. A diferencia que el resto de interfaces de
//   juego, este sera capaz de convivir con otros interfaces y su desactivacion
//   se llevara a cabo pasando como handle el valor nulo (0). Por este motivo,
//   no se trabajara con un estado de GUI relativo a la activacion de este
//   interfaz.
// Parametros:
// - hCriature. Handle a la criatura.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::SetCombatWindow(const AreaDefs::EntHandle& hCriature)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Se desea asociar criatura a la ventana de combate?
  if (hCriature) {
	// ¿Interfaz ya activo?
	if (m_GameInterfaces.Combat.IsActive()) {
	  // Si, luego se cambiara la entidad sin mas
	  m_GameInterfaces.Combat.SetCriature(hCriature);
	} else {
	  // No, se activara pasandola la criatura
	  ActiveCombatWindow(hCriature);  
	}
  } else {
	// No, se desactiva
	DeactiveCombatWindow();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece la interfaz de cargar partidas.
// Parametros:
// Devuelve:
// Notas:
// - Transiciones validas desde:
//   * Menu principal.
//   * Menu de juego.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::SetLoaderSavedFileWindow(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se efectua logica segun estado actual
  switch (m_GameInterfaces.GUIWindowState) {	
	case GUIManagerDefs::GUIW_MAINMENU: {
	  // Menu principal
	  // Desactiva main interfaz
	  DeactiveMainMenu();	  	  
	  
	} break;

	case GUIManagerDefs::GUIW_GAMEMENU: {
	  // Menu de juego
  	  // Desactiva ventana de interfaz principal	  
	  DeactiveMainInterfaz();

	  // Desactiva ventana de combate, si procediera
	  DeactiveCombatWindow();
	  
	  // Desactiva el dibujado del area	  
	  m_pWorld->SetDrawArea(false);
	  
	  // Desactiva menu de juego
	  DeactiveGameMenu();  
	} break;

	default:
	  // Estado de transicion no valido
	  ASSERT(false);
  }; // ~ switch

  // Activa interfaz y establece estado actual
  ActiveLoaderSavedFile(GetGameGUIWindowState());
  m_GameInterfaces.GUIWindowState = GUIManagerDefs::GUIW_LOADERSAVEDFILEMANAGER;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa la interfaz destinada a la carga de archivos
// Parametros:
// - PrevInterfaz. Interfaz previa, antes de pasar a activarla.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::ActiveLoaderSavedFile(const GUIManagerDefs::eGUIWindowType& PrevInterfaz)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());   

  // Activa interfaz
  m_GameInterfaces.LoaderSavedFile.Init(PrevInterfaz);
  ASSERT(m_GameInterfaces.LoaderSavedFile.IsInitOk());
  
  // Se activa el interfaz y se inserta en la lista
  ASSERT(!m_GameInterfaces.LoaderSavedFile.IsActive());
  m_GameInterfaces.LoaderSavedFile.Active();
  InsertInterfazInList(&m_GameInterfaces.LoaderSavedFile);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva interfaz de carga de partidas guardadas
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::DeactiveLoaderSavedFile(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se desactiva y extrae de la lista
  m_GameInterfaces.LoaderSavedFile.Deactive();    		
  ExtractInterfazFromList(&m_GameInterfaces.LoaderSavedFile);	

  // Se finaliza instancia
  m_GameInterfaces.LoaderSavedFile.End();  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el interfaz de guardar y borrar archivos de partidas guardadas
// Parametros:
// Devuelve:
// Notas:
// - Transiciones validas desde:
//   * Menu de juego.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::SetSaverSavedFileWindow(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se efectua logica segun estado actual
  switch (m_GameInterfaces.GUIWindowState) {		
	case GUIManagerDefs::GUIW_GAMEMENU: {
	  // Menu de juego
  	  // Desactiva ventana de interfaz principal	  
	  DeactiveMainInterfaz();
  
	  // Desactiva ventana de combate, si procediera
	  DeactiveCombatWindow();

	  // Desactiva menu de juego
	  DeactiveGameMenu();

	  // Desactiva el dibujado del area	  
	  m_pWorld->SetDrawArea(false);
	} break;

	default:
	  // Estado de transicion no valido
	  ASSERT(false);
  }; // ~ switch

  // Activa interfaz y establece estado actual
  ActiveSaverSavedFile();
  m_GameInterfaces.GUIWindowState = GUIManagerDefs::GUIW_SAVERSAVEDFILEMANAGER;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa interfaz de guardar partidas
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::ActiveSaverSavedFile(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());   

  // Activa interfaz
  m_GameInterfaces.SaverSavedFile.Init();
  ASSERT(m_GameInterfaces.SaverSavedFile.IsInitOk());
  
  // Se activa el interfaz y se inserta en la lista
  ASSERT(!m_GameInterfaces.SaverSavedFile.IsActive());
  m_GameInterfaces.SaverSavedFile.Active();
  InsertInterfazInList(&m_GameInterfaces.SaverSavedFile);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva interfaz de partidas guardadas
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::DeactiveSaverSavedFile(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se desactiva y extrae de la lista
  m_GameInterfaces.SaverSavedFile.Deactive();    		
  ExtractInterfazFromList(&m_GameInterfaces.SaverSavedFile);	

  // Se finaliza instancia
  m_GameInterfaces.SaverSavedFile.End();  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Añade opcion al interfaz de conversaciones.
// Parametros:
// - IDOption. Identificador de la opcion.
// - szText. Texto a añadir.
// Devuelve:
// - En caso de haberse añadido true. En caso contrario false.
// Notas:
// - Las opciones de dialogo SIEMPRE se podran seleccionar.
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIManager::AddOptionToConversatorWindow(const GUIDefs::GUIIDComponent& IDOption,
										  const std::string& szText)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Intenta añadir opciones y retorna
  if (m_GameInterfaces.Conversator.IsInitOk() &&
	  m_GameInterfaces.Conversator.IsActive()) {
	return m_GameInterfaces.Conversator.AddOption(IDOption, szText, true);
  }

  // No se pudo añadir
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ordena que se pregunte al jugador por una de las opciones que se hallen
//   en el interfaz de conversacion.
// Parametros:
// - pClient. Cliente al que notificar la seleccion de una pregunta
// Devuelve:
// - En caso de haberse añadido true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIManager::GetOptionFromConversatorWindow(iCGUIWindowClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pClient);

  // Instala a un cliente para recibir codigo de opcion escogida
  if (m_GameInterfaces.Conversator.IsInitOk() &&
	  m_GameInterfaces.Conversator.IsActive()) {
	return m_GameInterfaces.Conversator.GetOption(pClient);
  }

  // No se pudo preguntar
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el interfaz de menu de juego.
// Parametros:
// Devuelve:
// Notas:
// - El interfaz de menu de juego no estara permanentemente en memoria, sino
//   que sera llevado a esta tan pronto sea necesario, luego desde este
//   metodo se debera de llevar a memoria.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::ActiveGameMenu(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());   

  // Lleva a memoria el menu de juego
  m_GameInterfaces.GameMenu.Init();
  ASSERT(m_GameInterfaces.GameMenu.IsInitOk());
  
  // Se activa el interfaz principal y se inserta en la lista
  ASSERT(!m_GameInterfaces.GameMenu.IsActive());
  m_GameInterfaces.GameMenu.Active();
  InsertInterfazInList(&m_GameInterfaces.GameMenu);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el menu principal de juego.
// - Este menu sera inicializado desde este metodo, cargando y llevando a mem.
//   todos sus datos asociados.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::ActiveMainMenu(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Se Inicializa interfaz
  m_GameInterfaces.MainMenu.Init();
  ASSERT(m_GameInterfaces.MainMenu.IsInitOk());

  // Se activa e inserta en lista
  m_GameInterfaces.MainMenu.Active();
  InsertInterfazInList(&m_GameInterfaces.MainMenu);    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el interfaz para la creacion de caracteres
// - Este interfaz sera inicializado desde este metodo, cargando y llevando a mem.
//   todos sus datos asociados.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::ActiveCharacterCreator(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Se Inicializa interfaz
  m_GameInterfaces.CharacterCreator.Init();
  ASSERT(m_GameInterfaces.CharacterCreator.IsInitOk());

  // Se activa e inserta en lista
  m_GameInterfaces.CharacterCreator.Active();
  InsertInterfazInList(&m_GameInterfaces.CharacterCreator);    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa la ventana indicativa de carga de un area.
// - Esta ventana sera llevada a memoria bajo demanda.
// Parametros:
// - uwIDArea. Identificador del area a cargar.
// - pPlayer. Instancia al jugador
// - PlayerPos. Posicion del jugador en el area a cargar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::ActiveGameLoading(const word uwIDArea,
							   CPlayer* const pPlayer,
							   const AreaDefs::sTilePos& PlayerPos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Se Inicializa interfaz
  m_GameInterfaces.GameLoading.Init(uwIDArea, pPlayer, PlayerPos);
  ASSERT(m_GameInterfaces.GameLoading.IsInitOk());

  // Se activa e inserta en lista
  m_GameInterfaces.GameLoading.Active();  
  InsertInterfazInList(&m_GameInterfaces.GameLoading);       
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el interfaz de ventana principal.
// Parametros:
// Devuelve:
// - bDoFadeIn. ¿Activacion con efecto de FadeIn?. Por defecto valdra true
// Notas:
// - El interfaz debera de estar inicializado
// - Se comprobara via ASSERT que el interfaz no se halle activo.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::ActiveMainInterfaz(const bool bDoFadeIn)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Se activa el interfaz principal y se inserta en la lista  
  m_GameInterfaces.MainInterfaz.Active(bDoFadeIn);
  InsertInterfazInList(&m_GameInterfaces.MainInterfaz);    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el interfaz asociado a la hoja de personaje. 
// Parametros:
// - bDoFadeIn. ¿Se desea realizar el FadeIn?
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::ActivePlayerProfile(const bool bDoFadeIn)
{
  // SOLO si clase inicializada
  ASSERT(IsInitOk());

  // Se activa e inserta en la lista de interfaces activos  
  ASSERT(m_GameInterfaces.PlayerProfile.IsInitOk());
  m_GameInterfaces.PlayerProfile.Active(bDoFadeIn);  
  InsertInterfazInList(&m_GameInterfaces.PlayerProfile);       
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el interfaz para la seleccion de un item a usar.
// Parametros:
// - pCriatureSrc. Criatura que posee el inventario con los items.
// - pEntityDest. Entidad que recibe la accion del uso del item.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::ActiveItemSelectToUse(CCriature* const pCriatureSrc,
								   CWorldEntity* const pEntityDest)
{
  // SOLO si clase inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pCriatureSrc);
  ASSERT(pEntityDest);
  
  // Se activa e inserta en la lista de interfaces activos
  ASSERT(!m_GameInterfaces.ItemSelectToUse.IsActive());
  m_GameInterfaces.ItemSelectToUse.Active(pCriatureSrc, pEntityDest);  
  InsertInterfazInList(&m_GameInterfaces.ItemSelectToUse);      
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el interfaz de interaccion entre entidad principal (jugador) y el
//   resto de entidades del universo.
// Parametros:
// - InitDrawPos. Posicion inicial de dibujado.
// - pEntitySrc. Instancia a entidad origen.
// - pEntityDest. Intancia a entidad destino de la accion
// - ActiveContext. Contexto de activacion.
// Devuelve:
// Notas:
// - En la activacion del menu de interaccion se debera de comprobar si se 
//   activo el menu de interaccion y SOLO en tal caso, insertar el interfaz
//   en la lista de interfaces activos.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::ActiveEntityInteract(const sPosition& InitDrawPos,
								  CCriature* const pEntitySrc,
								  CWorldEntity* const pEntityDest,
								  const GUIManagerDefs::eGUIWindowType& ActiveContext)
{
  // SOLO si clase inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pEntitySrc);
  ASSERT(pEntityDest);
  
  // Se activa interfaz
  ASSERT(!m_GameInterfaces.EntityInteract.IsActive());
  m_GameInterfaces.EntityInteract.Active(InitDrawPos, 
										 pEntitySrc, 
										 pEntityDest, 
										 ActiveContext); 

  // ¿Llego a activarse?
  if (m_GameInterfaces.EntityInteract.IsActive()) {
	// Si, se inserta en la lista de interfaces activos
	InsertInterfazInList(&m_GameInterfaces.EntityInteract);      
  } else {
	// No, se regresa al estado anterior
	switch (ActiveContext) {
	  case GUIManagerDefs::GUIW_MAININTERFAZ: {	  
	    SetMainInterfazWindow();
	  } break;

	  case GUIManagerDefs::GUIW_PLAYERPROFILE: {	 
	    SetPlayerProfileWindow();
	  } break;

	  case GUIManagerDefs::GUIW_TRADE: {	 
	    SetTradeWindow(pEntitySrc, pEntityDest, NULL);
	  } break;
	}; // ~ switch
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el interfaz asociado a la ventana de Conversatoro
// - Al trabajar con este interfaz, la activacion no estara asegurada pues
//   sera necesario que se hayan añadido opciones.
// Parametros:
// - pMainConv. Conversador principal.
// - pSeconsaryConv. Conversador secundario.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::ActiveConversatorWindow(CWorldEntity* const pMainConv,
									 CWorldEntity* const pSecondaryConv)
{
  // SOLO si clase inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pMainConv);
  ASSERT(pSecondaryConv);
  
  // Se intenta activar
  m_GameInterfaces.Conversator.Active(pMainConv, pSecondaryConv);  
  
  // Si se logro activar, se insertar aen la lista de interfaces
  if (m_GameInterfaces.Conversator.IsActive()) {
	InsertInterfazInList(&m_GameInterfaces.Conversator);      
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa ventana de intercambio. El cliente asociado a la misma siempre 
//   sera NULL.
// Parametros:
// - pMainCriature. Instancia a la interfaz de criatura que inicia el 
//   intercambio / exploracion de un contenedor.
// - pEntity. Instancia a le entidad sobre la que se inica la exploracion
//   de su contenedor.
// - pClient. Cliente asociado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::ActiveTradeWindow(CCriature* const pMainCriature,
							   CWorldEntity* const pEntity,
							   iCGUIWindowClient* const pClient)
{
  // SOLO si clase inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pMainCriature);
  ASSERT(pEntity);
  
  // Se activa y se inserta si procede
  m_GameInterfaces.Trade.Active(pMainCriature, pEntity, pClient);
  if (m_GameInterfaces.Trade.IsActive()) {	
	InsertInterfazInList(&m_GameInterfaces.Trade);        
  }
}  

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva la interfaz asociada al menu principal, desalojandolo de 
//   memoria.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGUIManager::DeactiveMainMenu(void)
{  
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se extrae de la lista y finaliza instancia
  ExtractInterfazFromList(&m_GameInterfaces.MainMenu);	
  m_GameInterfaces.MainMenu.End();  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa la ventana de combate.
// Parametros:
// - hCriature. Activa la ventana de combate.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::ActiveCombatWindow(const AreaDefs::EntHandle& hCriature)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hCriature);

  // Se activa e inserta en lista
  ASSERT(!m_GameInterfaces.Combat.IsActive());
  m_GameInterfaces.Combat.Active(hCriature);
  if (m_GameInterfaces.Combat.IsActive()) {
	InsertInterfazInList(&m_GameInterfaces.Combat);    
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva el interfaz para la creacion de caracteres, descargando todos 
//   los datos de memoria.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGUIManager::DeactiveCharacterCreator(void)
{  
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se desactiva y extrae de la lista
  m_GameInterfaces.CharacterCreator.Deactive();    		
  ExtractInterfazFromList(&m_GameInterfaces.CharacterCreator);	

  // Se finaliza instancia
  m_GameInterfaces.CharacterCreator.End();  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva el interfaz de ventana principal.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGUIManager::DeactiveMainInterfaz(void)
{  
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
   
  // Se desactiva y extrae de la lista  
  m_GameInterfaces.MainInterfaz.Deactive();    	
  ExtractInterfazFromList(&m_GameInterfaces.MainInterfaz);	    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva la ventana indicativa de carga de un area
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGUIManager::DeactiveGameLoading(void)
{  
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se desactiva y extrae de la lista
  m_GameInterfaces.GameLoading.Deactive();    	
  ExtractInterfazFromList(&m_GameInterfaces.GameLoading);	

  // Se finaliza instancia
  m_GameInterfaces.GameLoading.End();  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva el interfaz asociado a la hoja del perfil del jugador.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::DeactivePlayerProfile(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Se desactiva el interfaz y extrae de la lista
  m_GameInterfaces.PlayerProfile.Deactive();
  ExtractInterfazFromList(&m_GameInterfaces.PlayerProfile);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva el interfaz de interaccion con entidad.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::DeactiveEntityInteract(void)
{
  // SOLO si instancia activa
  ASSERT(IsInitOk());

  // Se desactiva y se extrae de la lista	
  m_GameInterfaces.EntityInteract.Deactive();
  ExtractInterfazFromList(&m_GameInterfaces.EntityInteract);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva el interfaz de seleccion de un item a usar sobre entidad.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::DeactiveItemSelectToUse(void)
{
  // SOLO si instancia activa
  ASSERT(IsInitOk());

  // Se desactiva y se extrae de la lista
  m_GameInterfaces.ItemSelectToUse.Deactive();
  ExtractInterfazFromList(&m_GameInterfaces.ItemSelectToUse);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva el interfaz de menu de juego.
// Parametros:
// Devuelve:
// Notas:
// - El menu de juego estara en memoria solo cuando sea necesario, luego al
//   llamar a este metodo se desalojara tambien de esta.
///////////////////////////////////////////////////////////////////////////////
void
CGUIManager::DeactiveGameMenu(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se desactiva y extrae de la lista  
  m_GameInterfaces.GameMenu.Deactive();    
  ExtractInterfazFromList(&m_GameInterfaces.GameMenu);  

  // Se desaloja de memoria  
  m_GameInterfaces.GameMenu.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva ventana de Conversatoro
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::DeactiveConversatorWindow(void)
{
  // SOLO si instancia activa
  ASSERT(IsInitOk());

  // Se desactiva y se extrae de la lista
  m_GameInterfaces.Conversator.Deactive();
  ExtractInterfazFromList(&m_GameInterfaces.Conversator);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva ventana de intercambio
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::DeactiveTradeWindow(void)
{
  // SOLO si instancia activa
  ASSERT(IsInitOk());

  // Se desactiva y se extrae de la lista
  m_GameInterfaces.Trade.Deactive();
  ExtractInterfazFromList(&m_GameInterfaces.Trade);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece la interfaz asociada a las presentaciones.
// - En el caso de MainInterfaz y PlayerProfile, no existira una desactivacion
//   directa, sino que seguiran activas para que el FadeOut de la interfaz de
//   presentacion se dibuje sobre las mismas. Una vez que dicho FadeOut finalice,
//   esta clase recibira una notificacion para que si que se lleve a cabo la
//   desactivacion de la interfaz que proceda. En el caso de MainInterfaz, 
//   ademas se dejara de dibujar el universo de juego y se pondra en pausa.
//   Lo anterior no sera necesario para cuando la activacion sea desde el
//   MainMenu (creditos) ya que el FadeOut se habra hecho de antemano.
// Parametros:
// - szPresentationProfile. Perfil de la presentacion a activar.
// Devuelve:
// - Si se pudo establecer true, en caso contrario false.
// Notas:
// - Transiciones validas desde:
//   * Ventana de interfaz principal.
//   * Ventana de perfil de jugador.
//   * Ventana de menu principal.
///////////////////////////////////////////////////////////////////////////////
bool
CGUIManager::SetPresentationWindow(const std::string& szPresentationProfile)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(!szPresentationProfile.empty());

  // Se efectua logica segun estado actual
  switch (m_GameInterfaces.GUIWindowState) {
	case GUIManagerDefs::GUIW_MAININTERFAZ: {
	  // Se bloquea entrada
	  m_GameInterfaces.MainInterfaz.SetInput(false);	  	  
	} break;
	
	case GUIManagerDefs::GUIW_PLAYERPROFILE: {  
	  // Se bloquea entrada
	  m_GameInterfaces.PlayerProfile.SetInput(false);			  
	} break;

	case GUIManagerDefs::GUIW_MAINMENU: {
	  // Se desactiva el MainMenu 
	  DeactiveMainMenu();
	} break;
	
	default:
	  // Estado de transicion no valido
	  return false;
  }; // ~ switch  

  // Se activa ventana de presentacion
  ActivePresentation(szPresentationProfile, 
					 m_GameInterfaces.GUIWindowState);
  if (m_GameInterfaces.Presentation.IsActive()) {
	m_GameInterfaces.GUIWindowState = GUIManagerDefs::GUIW_PRESENTATION;
	return true;
  } 

  // No se pudo establecer, se retorna al estado anterior
  // Se efectua logica segun estado actual.
  // Nota: Este caso no podra pasar nunca en el menu principal
  switch (m_GameInterfaces.GUIWindowState) {
	case GUIManagerDefs::GUIW_MAININTERFAZ: {	 
	  m_GameInterfaces.MainInterfaz.SetInput(true);	  
	} break;
	
	case GUIManagerDefs::GUIW_PLAYERPROFILE: {  
	  m_GameInterfaces.PlayerProfile.SetInput(true);
	} break;

	case GUIManagerDefs::GUIW_MAINMENU: {
	  SetMainMenuWindow();	
	} break;
  }; // ~ switch  
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa la interfaz de presentacion. Esta interfaz, en su activacion,
//   tambien se inicializara.
// Parametros:
// - szPresentationProfile. Perfil de presentacion a activar.
// - PrevInterfaz. Interfaz previa.
// Devuelve:
// Notas:
// - En el caso de que se reciba un cliente asociado, se establecera el
//   recibido en caso contrario, se establecera GUIManager como cliente pues
//   se entendera que NO ha sido lanzado desde el script.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::ActivePresentation(const std::string& szPresentationProfile,
							    const GUIManagerDefs::eGUIWindowType& PrevInterfaz)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(!szPresentationProfile.empty());
  ASSERT((PrevInterfaz == GUIManagerDefs::GUIW_MAINMENU ||
		  PrevInterfaz == GUIManagerDefs::GUIW_MAININTERFAZ ||
		  PrevInterfaz == GUIManagerDefs::GUIW_PLAYERPROFILE) != 0);

  // Se inicializa y activa
  ASSERT(!m_GameInterfaces.Presentation.IsInitOk());
  m_GameInterfaces.Presentation.Init();
  m_GameInterfaces.Presentation.Active(szPresentationProfile, 
									   PrevInterfaz,
									   this);
  if (m_GameInterfaces.Presentation.IsActive()) {
	 // Se inserta interfaz en lista de dibujado y se retorna
	InsertInterfazInList(&m_GameInterfaces.Presentation);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva a la interfaz de prentacion, sacandola tambien de la lista de
//   interfaces activa y finalizandola.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::DeactivePresentation(void)
{
  // SOLO si instancia inicailizada
  ASSERT(IsInitOk());

  // Si esta activo desactiva
  if (m_GameInterfaces.Presentation.IsActive()) {
	// Se desactiva instancia, se saca de la lista y se finaliza
	m_GameInterfaces.Presentation.Deactive();	
	ExtractInterfazFromList(&m_GameInterfaces.Presentation);
	m_GameInterfaces.Presentation.End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva el interfaz de combate
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::DeactiveCombatWindow(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se desactiva y extrae de la lista
  if (m_GameInterfaces.Combat.IsActive()) {
	m_GameInterfaces.Combat.Deactive();    		
	ExtractInterfazFromList(&m_GameInterfaces.Combat);	  
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el cuadro de dialogo de aviso.
// Parametros:
// - szText. Texto a establecer en el cuadro de dialogo.
// - pClient. Cliente asociado al cuadro de dialogo.
// Devuelve:
// - En caso de que se haya podido establecer true. En caso contrario false.
// Notas:
// - La comprobacion se hara con IsInitOk debido a que los cuadros de dialogo
//   seguiran la regla de activacion si y solo si estan inicializados.
///////////////////////////////////////////////////////////////////////////////
bool
CGUIManager::ActiveAdviceDialog(const std::string& szText,
								iCGUIWindowClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(szText.size());

  // ¿Se puede activar el cuadro de dialogo?
  if (CanActiveGeneralInterfaz(GUIManagerDefs::GUIW_ADVICEDIALOG)) {
	// Se bloquea la entrada e interfaces
	SetGameInterfacesInput(false);
	SetGeneralInterfacesInput(false);

	// Se procede a inicializar y activar el cuadro de dialogo de aviso
	m_GenInterfaces.AdviceDialog.Init();
	ASSERT(m_GenInterfaces.AdviceDialog.IsInitOk());
	m_GenInterfaces.AdviceDialog.Active(szText, pClient);
	
	// Se inserta interfaz en lista de dibujado y se retorna
	InsertInterfazInList(&m_GenInterfaces.AdviceDialog);

	// Se pausara el universo de juego si procede y detiene scripts
	if (GetGameGUIWindowState() != GUIManagerDefs::GUIW_PLAYERPROFILE &&
		GetGameGUIWindowState() != GUIManagerDefs::GUIW_GAMEMENU) {
	  m_pWorld->SetPause(true);
	  SYSEngine::GetVirtualMachine()->SetScriptExecution(false);  
	}
  
	// Retorna
	return true;
  }

  // No se pudo
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el cuadro de dialogo de pregunta.
// Parametros:
// - szText. Texto asociado.
// - bCancelPresent. ¿Se desea mostrar la opcion de cancelar el cuadro de
//   dialogo?
// - pClient. Cliente asociado al cuadro de dialogo.
// Devuelve:
// Notas:
// - Es necesario bloquear input antes de inicializar
///////////////////////////////////////////////////////////////////////////////
bool
CGUIManager::ActiveQuestionDialog(const std::string& szText,
								  const bool bCancelPresent,
								  iCGUIWindowClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(szText.size());

  // ¿Se puede activar el cuadro de dialogo?
  if (CanActiveGeneralInterfaz(GUIManagerDefs::GUIW_QUESTIONDIALOG)) {
	// Se bloquea la entrada e interfaces	
	SetGameInterfacesInput(false);
	SetGeneralInterfacesInput(false);
	
	// Se procede a inicializar y activar el cuadro de dialogo de pregunta	
	m_GenInterfaces.QuestionDialog.Init();	
	ASSERT(m_GenInterfaces.QuestionDialog.IsInitOk());	
	m_GenInterfaces.QuestionDialog.Active(szText, bCancelPresent, pClient);	
	
	// Se inserta interfaz en lista de dibujado
	InsertInterfazInList(&m_GenInterfaces.QuestionDialog);

	// Se pausara el universo de juego si procede y detiene scripts
	if (GetGameGUIWindowState() != GUIManagerDefs::GUIW_PLAYERPROFILE &&
		GetGameGUIWindowState() != GUIManagerDefs::GUIW_GAMEMENU) {
	  m_pWorld->SetPause(true);
	  SYSEngine::GetVirtualMachine()->SetScriptExecution(false);  
	}

	// Retorna
	return true;
  }

  // No se pudo
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva cuadro de dialogo de notificacion, si es que estuviera activo.
// Parametros:
// Devuelve:
// Notas:
// - La desactivacion supondra desalojo de memoria.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::DeactiveAdviceDialog(void)
{
  // SOLO si instancia inicailizada
  ASSERT(IsInitOk());

  // Si esta activo desactiva
  if (m_GenInterfaces.AdviceDialog.IsInitOk()) {
	// Se finaliza instancia y se saca de la lista
	m_GenInterfaces.AdviceDialog.End();
	ExtractInterfazFromList(&m_GenInterfaces.AdviceDialog);

	// Se desbloquean interfaces
	SetGameInterfacesInput(true);
	SetGeneralInterfacesInput(true);

	// Se quita pausa del universo de juego y scripts, i procede
	if (GetGameGUIWindowState() != GUIManagerDefs::GUIW_PLAYERPROFILE &&
		GetGameGUIWindowState() != GUIManagerDefs::GUIW_GAMEMENU) {
	  m_pWorld->SetPause(false);
	  SYSEngine::GetVirtualMachine()->SetScriptExecution(true);  
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva cuadro de dialogo de pregunta, si es que estuviera activo.
// Parametros:
// Devuelve:
// Notas:
// - La desactivacion supondra desalojo de memoria.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::DeactiveQuestionDialog(void)
{
  // SOLO si instancia inicailizada
  ASSERT(IsInitOk());

  // Si esta activo desactiva
  if (m_GenInterfaces.QuestionDialog.IsInitOk()) {
	// Se finaliza instancia y se saca de la lista
	m_GenInterfaces.QuestionDialog.End();
	ExtractInterfazFromList(&m_GenInterfaces.QuestionDialog);

	// Se desbloquean interfaces
	SetGameInterfacesInput(true);
	SetGeneralInterfacesInput(true);	

	// Se quita pausa del universo de juego y scripts, i procede
	if (GetGameGUIWindowState() != GUIManagerDefs::GUIW_PLAYERPROFILE &&
		GetGameGUIWindowState() != GUIManagerDefs::GUIW_GAMEMENU) {
	  m_pWorld->SetPause(false);
	  SYSEngine::GetVirtualMachine()->SetScriptExecution(true);  
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el interfaz general destinado a la muestra de un archivo de texto.
// Parametros:
// - szTitle. Titulo asociado. No obligatorio.
// - szTextFileName. Nombre del archivo de texto.
// - pClient. Cliente a asociar.
// Devuelve:
// - Si se pudo inicializar y activar true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CGUIManager::ActiveTextReader(const std::string& szTitle,
							  const std::string& szTextFileName,
							  iCGUIWindowClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(szTextFileName.size());

  // ¿Se puede activar el cuadro de dialogo?
  if (CanActiveGeneralInterfaz(GUIManagerDefs::GUIW_TEXTREADER)) {
	// Se bloquea la entrada e interfaces	
	SetGameInterfacesInput(false);
	SetGeneralInterfacesInput(false);
	
	// Se procede a inicializar y activar el cuadro de dialogo de pregunta	
	m_GenInterfaces.TextReader.Init();	
	ASSERT(m_GenInterfaces.TextReader.IsInitOk());	
	m_GenInterfaces.TextReader.Active(szTitle, szTextFileName, pClient);	

	// La activacion ha podido fallar si el archivo no existe o es muy grande
	if (m_GenInterfaces.TextReader.IsActive()) {
	  // Se inserta interfaz en lista de dibujado
	  InsertInterfazInList(&m_GenInterfaces.TextReader);

	  // Se pausara el universo de juego si procede y detiene scripts
	  if (GetGameGUIWindowState() != GUIManagerDefs::GUIW_PLAYERPROFILE &&
		  GetGameGUIWindowState() != GUIManagerDefs::GUIW_GAMEMENU) {
		m_pWorld->SetPause(true);
		SYSEngine::GetVirtualMachine()->SetScriptExecution(false);  
	  }

	  // Retorna
	  return true;
	} else {
	  // No se pudo activar, luego se finaliza desbloqueandose entradas
	  m_GenInterfaces.TextReader.End();
	  SetGameInterfacesInput(true);
	  SetGeneralInterfacesInput(true);	
	}	
  }

  // No se pudo
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva el interfaz de lectura de archivos
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::DeactiveTextReader(void)
{
  // SOLO si instancia inicailizada
  ASSERT(IsInitOk());

  // Si esta activo desactiva
  if (m_GenInterfaces.TextReader.IsInitOk()) {
	// Se finaliza instancia y se saca de la lista
	m_GenInterfaces.TextReader.End();
	ExtractInterfazFromList(&m_GenInterfaces.TextReader);

	// Se desbloquean interfaces
	SetGameInterfacesInput(true);
	SetGeneralInterfacesInput(true);	

	// Se quita pausa del universo de juego y scripts, i procede
	if (GetGameGUIWindowState() != GUIManagerDefs::GUIW_PLAYERPROFILE &&
		GetGameGUIWindowState() != GUIManagerDefs::GUIW_GAMEMENU) {
	  m_pWorld->SetPause(false);
	  SYSEngine::GetVirtualMachine()->SetScriptExecution(true);  
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el interfaz general para la seleccion de opciones de texto.
// Parametros:
// - szTitle. Titulo del interfaz.
// - bCanUseCancel. Flag de posibilidad de cancelacion del interfaz.
// - pClient. Posible cliente que recibira notificacion de opcion seleccionada
//   o pulsacion de cancelacion.
// Devuelve:
// - Si se pudo activar true. En caso contrario false.
// Notas:
// - El interfaz se llevara a memoria al añadirse una opcion y no estar en
//   memoria, en lugar de hacerse dicho proceso en este metodo.
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIManager::ActiveTextOptionsSelector(const std::string& szTitle,
									   const bool bCanUseCancel,
									   iCGUIWindowClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // ¿Se puede activar el cuadro de dialogo?
  if (CanActiveGeneralInterfaz(GUIManagerDefs::GUIW_GENERICTEXTSELECTOR)) {
	// Se bloquea la entrada e interfaces	
	SetGameInterfacesInput(false);
	SetGeneralInterfacesInput(false);
	
	// Se procede a activar el cuadro de dialogo de seleccion de texto
	ASSERT(m_GenInterfaces.TextOptionSelector.IsInitOk());		
	m_GenInterfaces.TextOptionSelector.Active(szTitle, bCanUseCancel);	
	
	// ¿Se pudo activar?
	if (m_GenInterfaces.TextOptionSelector.IsActive()) {	
	  // Si, se intenta preguntar por una opcion
	  if (m_GenInterfaces.TextOptionSelector.GetOption(pClient)) {
		// Todo correcto, se añade interfaz a lista
		InsertInterfazInList(&m_GenInterfaces.TextOptionSelector);

		// Se pausara el universo de juego si procede y detiene scripts
		if (GetGameGUIWindowState() != GUIManagerDefs::GUIW_PLAYERPROFILE &&
			GetGameGUIWindowState() != GUIManagerDefs::GUIW_GAMEMENU) {
		  m_pWorld->SetPause(true);
		  SYSEngine::GetVirtualMachine()->SetScriptExecution(false);  
		}

		// Retorna
		return true;
	  }
	}

	// No se pudo activar o preguntar, se vuelve a estado anterior
	m_GenInterfaces.TextOptionSelector.End();
	SetGameInterfacesInput(true);
	SetGeneralInterfacesInput(true);			
  }

  // No se pudo
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva el selector general de opciones de texto
// - Al desactivarse el selector general de opciones de texto, todas las
//   opciones que estuvieran registradas, desapareceran.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::DeactiveTextOptionsSelector(void)
{
  // SOLO si instancia inicailizada
  ASSERT(IsInitOk());

  // Si esta inicializada, finaliza la interfaz
  if (m_GenInterfaces.TextOptionSelector.IsInitOk()) {
	// Se finaliza instancia y se saca de la lista
	m_GenInterfaces.TextOptionSelector.End();
	ASSERT(!m_GenInterfaces.TextOptionSelector.IsInitOk());
	ExtractInterfazFromList(&m_GenInterfaces.TextOptionSelector);

	// Se desbloquean interfaces
	SetGameInterfacesInput(true);
	SetGeneralInterfacesInput(true);	

	// Se quita pausa del universo de juego y scripts, i procede
	if (GetGameGUIWindowState() != GUIManagerDefs::GUIW_PLAYERPROFILE &&
		GetGameGUIWindowState() != GUIManagerDefs::GUIW_GAMEMENU) {
	  m_pWorld->SetPause(false);
	  SYSEngine::GetVirtualMachine()->SetScriptExecution(true);  
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Añade opciones al selector general de texto. En caso de el identificador
//   de opcion sea 0 o bien tenga un numero de lineas excesivo, la opcion sera
//   rechazada.
// Parametros:
// - IDOption. Identificador de la opcion.
// - szText. Texto asociado a la opcion.
// - bCanSelect. Flag que indica si la opcion puede ser seleccionada. Por
//   defecto valdra true.
// Devuelve:
// - Si la opcion ha sido registrada true. En caso contrario false.
// Notas:
// - Si no esta inicializado, se inicializara.
// - Si el interfaz se hallara activo, no se dejara activarlo.
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIManager::AddOptionToTextOptionsSelector(const GUIDefs::GUIIDComponent& IDOption,
											const std::string& szText,
											const bool bCanSelect)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Esta inicializado?
  if (!m_GenInterfaces.TextOptionSelector.IsInitOk()) {
	// No, luego se inicializa
	m_GenInterfaces.TextOptionSelector.Init();
  }


  // Se intenta asociar opcion
  ASSERT(m_GenInterfaces.TextOptionSelector.IsInitOk());  
  return m_GenInterfaces.TextOptionSelector.AddOption(IDOption, szText, bCanSelect);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera todas las opcioens registradas en el interfaz
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGUIManager::ReleaseTextOptionsSelector(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Esta inicializado?
  if (m_GenInterfaces.TextOptionSelector.IsInitOk()) {
	// Se liberan opciones
    m_GenInterfaces.TextOptionSelector.ReleaseOptions();
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba el estado de interfaz de juego actual y decide si se puede
//   activar un interfaz de caracter general. En caso de que se pueda,
//   se comprobara si el interfaz a activar ya se encuentra activo, en cuyo
//   caso NO se podra.
// Parametros:
// Devuelve:
// - Si se puede activar true. En caso contrario false.
// Notas:
// - Se podra activar desde el menu principal, la generacion de personajes,
//   el interfaz principal de juego (siempre y cuando no se este trabajando
//   con un interfaz dependiente de este), inventario y ficha de personaje y
//   el menu de juego
///////////////////////////////////////////////////////////////////////////////
bool
CGUIManager::CanActiveGeneralInterfaz(const GUIManagerDefs::eGUIWindowType& Interfaz) const
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Se comprueba si nos hallamos en un interfaz de juego validos
  switch (m_GameInterfaces.GUIWindowState) {
	case GUIManagerDefs::GUIW_MAINMENU:
	case GUIManagerDefs::GUIW_CHARACTERCREATOR:
	case GUIManagerDefs::GUIW_MAININTERFAZ:
	case GUIManagerDefs::GUIW_PLAYERPROFILE:
	case GUIManagerDefs::GUIW_GAMEMENU: {	  	 
	  // Ahora se comprueba si el interfaz a activar no se halla activo
	  switch(Interfaz) {
		case GUIManagerDefs::GUIW_ADVICEDIALOG: 
		case GUIManagerDefs::GUIW_QUESTIONDIALOG: {
		  // Cuadros de dialogo no son validos simultaneamente
		  if (!m_GenInterfaces.AdviceDialog.IsInitOk() ||
			  !m_GenInterfaces.QuestionDialog.IsInitOk()) {
			// Se puede
			return true;
		  }
		} break;		

		case GUIManagerDefs::GUIW_TEXTREADER: {
		  // Lector de texto (archivo de texto)
		  if (!m_GenInterfaces.TextReader.IsInitOk()) {
			// Se puede 
			return true;
		  }
		} break;

		case GUIManagerDefs::GUIW_GENERICTEXTSELECTOR: {
		  // Selector general de opciones de texto
		  // Para activarse, ha de estar inicializado y NO activo
		  if (m_GenInterfaces.TextOptionSelector.IsInitOk() &&
			  !m_GenInterfaces.TextOptionSelector.IsActive()) {			
			// Se puede
			return true;
		  }
		} break;
	  }; // ~ switch	
	} break;
  }; // ~ switch

  // No se puede
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Bloquea / desbloquea los interfaces de juego que se hallen activos en
//   un determinado momento. Este metodo sera llamado en la activacion de
//   cualquier cuadro de dialogo de caracter general.
// Parametros:
// - bInput. Flag asociado a la activacion / desactivacion del input.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::SetGameInterfacesInput(const bool bInput)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se procede a bloquear / desbloquear los interfaces de juego que proceda
  switch (m_GameInterfaces.GUIWindowState) {
	case GUIManagerDefs::GUIW_MAINMENU: {
	  // Menu principal
	  m_GameInterfaces.MainMenu.SetInput(bInput);
	} break;

	case GUIManagerDefs::GUIW_CHARACTERCREATOR: {
	  // Creacion del caracter
	  m_GameInterfaces.CharacterCreator.SetInput(bInput);
	} break;

	case GUIManagerDefs::GUIW_MAININTERFAZ: {
	  // Interfaz principal de juego
	  m_GameInterfaces.MainInterfaz.SetInput(bInput);
	} break;

	case GUIManagerDefs::GUIW_PLAYERPROFILE: {
	  // Inventario / ficha del personaje
	  m_GameInterfaces.PlayerProfile.SetInput(bInput);
	} break;

	case GUIManagerDefs::GUIW_GAMEMENU: {
	  // Menu de juego
	  m_GameInterfaces.GameMenu.SetInput(bInput);
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba cuales son los interfaces generales que se hallan activos y
//   de entre ellos, bloquea o desbloquea su entrada.
// Parametros:
// - bInput. Flag con la indicacion de bloqueo / desbloqueo de la entrada.
// Devuelve:
// Notas:
// - Los interfaces de caracter general seran llevados a memoria cuando se
//   activen, asi que se comprobara el flag IsInitOk.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::SetGeneralInterfacesInput(const bool bInput)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Procede a comprobar interfaces
  // Cuadro de dialogo de notificacion de un aviso
  if (m_GenInterfaces.AdviceDialog.IsInitOk()) {
	m_GenInterfaces.AdviceDialog.SetInput(bInput);
  }

  // Cuadro de dialogo de pregunta
  if (m_GenInterfaces.QuestionDialog.IsInitOk()) {
	m_GenInterfaces.QuestionDialog.SetInput(bInput);
  }

  // Lector de archivos de texto
  if (m_GenInterfaces.TextReader.IsInitOk()) {
	m_GenInterfaces.TextReader.SetInput(bInput);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Invocado cuando un comando llegue a su fin. 
//   El metodo sera utilizado para indicar la finalizacion de los comandos de:
//   * Apertura de cortinas de cine.
//   * Cierre de cortinas de cine
// Parametros:
// - IDCommand. ID del comando.
// - udInstant. Instante de tiempo en que ocurrio la finalizacion del comando.
// - udExtraParam. Parametro extra.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
						  const dword udInstant,
						  const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se comprueba el ID recibido
  switch(IDCommand) {
	case sCutSceneInfo::ID_OPENCURTAIN_CMD: {
	  // Comando de apertura de cortinas finalizado
	  OnOpenCinemaCurtainCmdEnd();	  

	  // Notifica a posible cliente pasado
	  if (m_CutSceneInfo.pClient) {
		m_CutSceneInfo.pClient->EndCmdNotify(m_CutSceneInfo.IDCommand,
											 udInstant,
											 m_CutSceneInfo.udExtraParam);
		m_CutSceneInfo.pClient = NULL;
	  }
	} break;

	case sCutSceneInfo::ID_CLOSECURTAIN_CMD: {
	  // Comando de cierre de cortinas finalizado
	  OnCloseCinemaCurtainCmdEnd();

	  // Notifica a posible cliente pasado
	  if (m_CutSceneInfo.pClient) {
		m_CutSceneInfo.pClient->EndCmdNotify(m_CutSceneInfo.IDCommand,
											 udInstant,
											 m_CutSceneInfo.udExtraParam);
		m_CutSceneInfo.pClient = NULL;
	  }
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la finalizacion del comando de apertura de cortinas
//   de cine. Al abrirse, se variara el area de dibujado del motor isometrico
//   para optimizar render.
// Parametros:
// Devuelve:
// Notas:
// - Por el momento solo se puede modificar el area de dibujado del isometrico
//   en la parte inferior, estaria pendiente el poder indicar el area de
//   dibujado excluyente por arriba.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::OnOpenCinemaCurtainCmdEnd(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());  

  // Se cambia area de dibujado en el isometrico y desactiva MainInterfaz
  ASSERT(m_pWorld);
  m_pWorld->SetDrawWindow(m_pGraphSys->GetVideoWidth(),
						  m_pGraphSys->GetVideoHeight() - CGUIManager::CINEMACURTAIN_BASEHEIGHT);
  DeactiveMainInterfaz();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la finalizacion del comando de cierre de cortinas
//   de cine. Se restaurara el area original de dibujado del isometrico y se
//   activara la ventana de interfaz principal. Tambien se desbloqueara el
//   motor isometrico para que pueda recibir entrada.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::OnCloseCinemaCurtainCmdEnd(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se deja en estado original al motor isometrico
  ASSERT(m_pWorld);
  m_pWorld->SetDrawWindow(m_pGraphSys->GetVideoWidth(),
						  m_pGraphSys->GetVideoHeight() - 96);
  
  // Se activa entrada en MainInterfaz
  m_GameInterfaces.MainInterfaz.SetInput(true);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el efecto de cine visualmente y coordinando el trabajo con los
//   distintos interfaces. 
// - La ventana de interfaz principal se desactivara y el motor isometrico pasara
//   a dibujarse en toda la pantalla (para suplir la falta de la ventana de 
//   interfaz principal). Este ultimo, quedara bloqueado para no recibir entrada
//   alguna.
// - Tambien se inicializa el comando de apertura de cortinas de cine.
// Parametros:
// - pCmdClient. Posible comando externo al que enviar la notificacion
// - IDCommand. Identificador del comando.
// - udExtraParam. Parametro extra a pasar.
// Devuelve:
// Notas:
// - En caso de que las cortinas esten abiertas o se esten abriendo, se
//   ignorara por complento la llamada a este metodo.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::BeginCutScene(iCCommandClient* const pCmdClient,
						   const CommandDefs::IDCommand& IDCommand,
						   const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Se puede realizar el efecto?
  if (!IsCutSceneActive()) {
	// Se bloquea motor isometrico y se establece el area entera
	ASSERT(m_pWorld);
	m_pWorld->SetDrawWindow(m_pGraphSys->GetVideoWidth(),
							m_pGraphSys->GetVideoHeight());

	// Se inicializa y postea el comando de apertura de ventana
	ASSERT(!m_CutSceneInfo.OpenCurtainCmd.IsActive());
	m_CutSceneInfo.OpenCurtainCmd.Init(&m_CutSceneInfo.CinemaCurtain, 128);
	SYSEngine::GetCommandManager()->PostCommand(&m_CutSceneInfo.OpenCurtainCmd,
												this,
												sCutSceneInfo::ID_OPENCURTAIN_CMD);

	// Se guarda informacion recibida
	m_CutSceneInfo.pClient = pCmdClient;
	m_CutSceneInfo.IDCommand = IDCommand;
	m_CutSceneInfo.udExtraParam = udExtraParam;

	// Se bloquea entrada en MainInterfaz
	m_GameInterfaces.MainInterfaz.SetInput(false);

	// Se oculta cualquier tipo de cursor
	SetGUICursor(GUIManagerDefs::NO_CURSOR);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el efecto de cierre de CutScene.
// - Se establecera todo el area para dibujar el motor isometrico (pues las
//   cortinas se "descorreran").
// - Se posteara el comando de cerrar cortinas.
// Parametros:
// - pCmdClient. Posible comando externo al que enviar la notificacion
// - IDCommand. Identificador del comando.
// - udExtraParam. Parametro extra a pasar.
// Devuelve:
// Notas:
// - En caso de que las cortinas esten abiertas o se esten abriendo, se
//   ignorara por complento la llamada a este metodo.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::EndCutScene(iCCommandClient* const pCmdClient,
						 const CommandDefs::IDCommand& IDCommand,
						 const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se cierran si procede
  if (IsCutSceneActive()) {
	// Se restablece el area de dibujado para el motor isometrico  
	m_pWorld->SetDrawWindow(m_pGraphSys->GetVideoWidth(),
							m_pGraphSys->GetVideoHeight());
	
	// Se activa el MainInterfaz y entrada en el mismo
	ActiveMainInterfaz(false);
	m_GameInterfaces.MainInterfaz.SetInput(false);
	
	// Se inicializa y postea el comando de cierre de ventana
	ASSERT(!m_CutSceneInfo.CloseCurtainCmd.IsActive());
	m_CutSceneInfo.CloseCurtainCmd.Init(&m_CutSceneInfo.CinemaCurtain, 128);
	SYSEngine::GetCommandManager()->PostCommand(&m_CutSceneInfo.CloseCurtainCmd,
												this,
												sCutSceneInfo::ID_CLOSECURTAIN_CMD);

	// Se guarda informacion recibida
	m_CutSceneInfo.pClient = pCmdClient;
	m_CutSceneInfo.IDCommand = IDCommand;
	m_CutSceneInfo.udExtraParam = udExtraParam;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece un cursor de GUI, que sera el dibujado en pantalla desde 
//   este modulo. Tambien podra desvincular cualquier tipo de cursor que 
//   este como actual.
// Parametros:
// - Cursor. Tipo de cursor a establecer.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::SetGUICursor(const GUIManagerDefs::eGUICursorType& Cursor)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Se quiere establecer el cursor que ya esta establecido?
  if (m_GUICursorsInfo.ActCursor == Cursor) { 
	return; 
  }  

  // Se finaliza el estado de animacion del cursor actual
  // Nota: aunque se cambie a un estado de un mismo cursor (CSprite), no habra 
  // perdida de coherencia, tan solo una despreciable perdida de velocidad que
  // permite poner todo muchisimo mas sencillo y legible
  if (m_GUICursorsInfo.pActCursor) {
    m_GUICursorsInfo.pActCursor->GetAnimTemplate()->ActiveAnimCmd(false);
  }
    
  // Establece el cursor indicado  
  m_GUICursorsInfo.ActCursor = Cursor;  
  AnimTemplateDefs::AnimState CursorState;
  switch (Cursor) {
	case GUIManagerDefs::NO_CURSOR: {
	  // A la hora de quitar un cursor, el posible texto flotante se finalizara	  	  
	  m_GUICursorsInfo.pActCursor = NULL;
	  UnsetFloatingText();	  	  
	} break;

	case GUIManagerDefs::MOVEMENT_CURSOR: {	  
	  m_GUICursorsInfo.pActCursor = &m_GUICursorsInfo.GUIMovementCursor;
	  CursorState = CGUIManager::MOV_POSSIBLE_ANIMSTATE;	  
	} break;
	  
	case GUIManagerDefs::NOPOSSIBLE_MOVEMENT_CURSOR: {
	  m_GUICursorsInfo.pActCursor = &m_GUICursorsInfo.GUIMovementCursor;
	  CursorState = CGUIManager::MOV_NO_POSSIBLE_ANIMSTATE;	  
	} break;

	case GUIManagerDefs::ENTITYINTERACT_CURSOR: {	
	  m_GUICursorsInfo.pActCursor = &m_GUICursorsInfo.GUIEntityInteractCursor;
	  CursorState = CGUIManager::ENTITY_INTERACT_POSSIBLE_ANIMSTATE;
	} break;

	case GUIManagerDefs::NOPOSSIBLE_ENTITYINTERACT_CURSOR: {	
	  m_GUICursorsInfo.pActCursor = &m_GUICursorsInfo.GUIEntityInteractCursor;
	  CursorState = CGUIManager::ENTITY_INTERACT_NO_POSSIBLE_ANIMSTATE;
	} break;

  	case GUIManagerDefs::ENTITYINTERACTFORHIT_CURSOR: {	
	  m_GUICursorsInfo.pActCursor = &m_GUICursorsInfo.GUIEntityInteractForHitCursor;
	  CursorState = CGUIManager::ENTITY_INTERACT_FORHIT_POSSIBLE_ANIMSTATE;
	} break;

	case GUIManagerDefs::NOPOSSIBLE_ENTITYINTERACTFORHIT_CURSOR: {	
	  m_GUICursorsInfo.pActCursor = &m_GUICursorsInfo.GUIEntityInteractForHitCursor;
	  CursorState = CGUIManager::ENTITY_INTERACT_FORHIT_NO_POSSIBLE_ANIMSTATE;
	} break;

	case GUIManagerDefs::WAITING_CURSOR: {	  
	  m_GUICursorsInfo.pActCursor = &m_GUICursorsInfo.GUIWaitingCursor;
	  CursorState = CGUIManager::WAITING_CURSOR_ANIMSTATE;	  
	} break;  
  
	case GUIManagerDefs::SCROLL_NORTH_CURSOR: {
	  m_GUICursorsInfo.pActCursor = &m_GUICursorsInfo.GUIIsoScrollCursor;
	  CursorState = CGUIManager::NORTHSCROLL_ANIMSTATE;
	} break;

	case GUIManagerDefs::SCROLL_NORTHEAST_CURSOR: {
	  m_GUICursorsInfo.pActCursor = &m_GUICursorsInfo.GUIIsoScrollCursor;
	  CursorState = CGUIManager::NORTHEASTSCROLL_ANIMSTATE;	  
	} break;
	
	case GUIManagerDefs::SCROLL_EAST_CURSOR: {
	  m_GUICursorsInfo.pActCursor = &m_GUICursorsInfo.GUIIsoScrollCursor;
	  CursorState = CGUIManager::EASTSCROLL_ANIMSTATE;	  	  
	} break;

	case GUIManagerDefs::SCROLL_SOUTHEAST_CURSOR: {	  
	  m_GUICursorsInfo.pActCursor = &m_GUICursorsInfo.GUIIsoScrollCursor;
	  CursorState = CGUIManager::SOUTHEASTSCROLL_ANIMSTATE;	  
	} break;

	case GUIManagerDefs::SCROLL_SOUTH_CURSOR: {	  
	  m_GUICursorsInfo.pActCursor = &m_GUICursorsInfo.GUIIsoScrollCursor;
	  CursorState = CGUIManager::SOUTHSCROLL_ANIMSTATE;
	} break;

	case GUIManagerDefs::SCROLL_SOUTHWEST_CURSOR: {	  
	  m_GUICursorsInfo.pActCursor = &m_GUICursorsInfo.GUIIsoScrollCursor;
	  CursorState = CGUIManager::SOUTHWESTSCROLL_ANIMSTATE;
	} break;

	case GUIManagerDefs::SCROLL_WEST_CURSOR: {
	  m_GUICursorsInfo.pActCursor = &m_GUICursorsInfo.GUIIsoScrollCursor;
	  CursorState = CGUIManager::WESTSCROLL_ANIMSTATE;
	} break;

	case GUIManagerDefs::SCROLL_NORTHWEST_CURSOR: {		  
	  m_GUICursorsInfo.pActCursor = &m_GUICursorsInfo.GUIIsoScrollCursor;
	  CursorState = CGUIManager::NORTHWESTSCROLL_ANIMSTATE;
	} break;

	case GUIManagerDefs::WINDOWINTERFAZ_CURSOR: {	  
	  m_GUICursorsInfo.pActCursor = &m_GUICursorsInfo.GUIWindowInteractCursor;
	  CursorState = CGUIManager::WINDOWINTERFAZ_ANIMSTATE;	  
	} break;

	default:
	  ASSERT(false);
  }; // ~ switch  

  // Se establece la animacion del cursor elegido, si es que se eligio alguno
  if (m_GUICursorsInfo.pActCursor) {	
	m_GUICursorsInfo.pActCursor->GetAnimTemplate()->ActiveAnimCmd(true);
	m_GUICursorsInfo.pActCursor->GetAnimTemplate()->SetState(CursorState);	
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia un sprite al cursor de GUI actual, devolviendo la direccion
//   del sprite que estuviera previamente asociado. El metodo se podra 
//   utilizara tanto si hay cursor de GUI activo como si no.
// - En caso de pasaar NULL, cualquier cursor grafico que hubiera asociado,
//   se desvinculara.
// - Al asociar un sprite, se dibujara en pantalla el sprite y encima el
//   cursor de GUI que se halle activo. Este metodo sera muy interesante para
//   desplazar objetos de un sitio a otro con la interfaz clasica.
// Parametros:
// - pSprite. Sprite a asociar.
// Devuelve:
// - La direccion del sprite que estuviera previamente asociado.
//   En caso de que no hubiera ninguno NULL.
// Notas:
///////////////////////////////////////////////////////////////////////////////
CSprite*
CGUIManager::AttachSpriteToGUICursor(CSprite* const pSprite) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se obtiene sprite asociado previo y se asocia nuevo
  CSprite* pOldSprite = m_GUICursorsInfo.pSpriteAttached;
  m_GUICursorsInfo.pSpriteAttached = pSprite;

  // Se retorna sprite previo
  return pOldSprite;  
}


///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el texto flotante indicado, sustituyendo el que pudiera estar
//   establecido.
// Parametros:
// - szText. Texto a establecer.
// - RGBColor. Color a utilizar.
// - Alpha. Valor alpha.
// - Type. Tipo de formato de salida para el texto. Consultar en GUIManagerDefs
//   por el enumerado.
// Devuelve:
// Notas:
// - En caso de que no se pase texto alguno y se pudiera asociar el texto al
//   haber un cursor, simplemente se finalizara el posible texto flotante que
//   hubiera pero no se hara nada mas.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::SetFloatingText(const std::string& szText,
							 const GraphDefs::sRGBColor& RGBColor,
							 const GraphDefs::sAlphaValue& Alpha,
							 const GUIManagerDefs::eFloatingTextType& Type)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // ¿Hay un cursor asociado?
  if (m_GUICursorsInfo.ActCursor != GUIManagerDefs::NO_CURSOR) {	
	// Se finaliza cualquier texto flotante que pudiera estar asociado
	UnsetFloatingText();

	// ¿Hay texto que asociar?
	if (!szText.empty()) {
	  // Se configura el componente de texto
	  m_FloatingTextInfo.CfgBase.szText = szText;
	  m_FloatingTextInfo.CfgBase.RGBUnselectColor = RGBColor;
	  m_FloatingTextInfo.CfgBase.RGBSelectColor = RGBColor;
	  m_FloatingTextInfo.CfgBase.Alpha = Alpha;
	  
	  // Se inicializa y activa
	  m_FloatingTextInfo.Text.Init(m_FloatingTextInfo.CfgBase);
	  ASSERT(m_FloatingTextInfo.Text.IsInitOk());
	  m_FloatingTextInfo.Text.SetActive(true);
	  ASSERT(m_FloatingTextInfo.Text.IsActive());

	  // Se configura el FXBack del texto
	  // La posicion sera arbritaria pues cambiara con el movimiento del cursor
	  m_FloatingTextInfo.FXBack.Init(sPosition(0,0),
									 2,
									 m_FloatingTextInfo.Text.GetWidth() + 6,
									 FontDefs::CHAR_PIXEL_HEIGHT * m_FloatingTextInfo.Text.GetVisibleTextLines(),
									 GraphDefs::sRGBColor(0, 0, 0),
									 GraphDefs::sAlphaValue(75));
	  ASSERT(m_FloatingTextInfo.FXBack.IsInitOk());

	  // Se comprueba el tipo de formato de salida para el texto flotante
	  // para ver si hay que realizar algun tipo de tratamiento especial
	  switch (Type) {
		case GUIManagerDefs::DISSOLVE_FLOATINGTEXT: {
		  // El texto sale con un comando de fade
		  m_FloatingTextInfo.Text.ActiveAlphaFade(GraphDefs::sAlphaValue(75));
		} break;

		case GUIManagerDefs::NORMAL_FLOATINGTEXT: {
		  // El texto sale sin ningun tratamiento especial
		} break;		
	  }; // ~ switch
	}		
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Mandara el texto recibido a la consola de interfaz que proceda. Desde el
//   exterior solo existira una consola, cuando en la practica tendremos a las
//   mismas alojadas en cada uno de los interfaces que hacen uso de ellas. Este
//   metodo simplemente redireccionara el texto a la consola del interfaz que 
//   se halle activo (suponiendo, naturalmente, unas prioridades de envio)
// Parametros:
// - szText. Texto a enviar.
// - bPlaySound. ¿Se quiere hacer sonar sonido al enviar el texto?.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::WriteToConsole(const std::string& szText, 
						    const bool bPlaySound)
{
  // SOLO si instancia activa
  ASSERT(IsInitOk());

  // Se redirecciona el envio
  if (m_GameInterfaces.PlayerProfile.IsActive()) {
	m_GameInterfaces.PlayerProfile.WriteToConsole(szText, bPlaySound);
  } else if (m_GameInterfaces.MainInterfaz.IsActive()) {
	m_GameInterfaces.MainInterfaz.WriteToConsole(szText, bPlaySound);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se resetea la informacion que se halle en consola, tanto en MainInterfaz
//   como en PlayerProfile si es que esta activos dicho interfaces.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::ResetConsoleInfo(void)
{
  // SOLO si instancia activa
  ASSERT(IsInitOk());

  // Interfaz de inventario del jugador
  if (m_GameInterfaces.PlayerProfile.IsInitOk()) {
	m_GameInterfaces.PlayerProfile.ResetConsoleInfo();
  } 

  // Intefaz del MainGameInterfaz
  if (m_GameInterfaces.MainInterfaz.IsInitOk()) {
	m_GameInterfaces.MainInterfaz.ResetConsoleInfo();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inserta el interfaz recibido en la lista de interfaces.
// Parametros:
// - pInterfaz. Direccion del interfaz a insertar en la lista de interfaces.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::InsertInterfazInList(CGUIWindow* const pInterfaz)
{
  // SOLO si instancia activa
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pInterfaz);
  ASSERT(pInterfaz->IsInitOk());  
  ASSERT(pInterfaz->IsActive());  

  // Se inserta en la lista
  m_ActiveInterfaces.push_back(pInterfaz);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Busca y extrae de la lista de interfaces activos / dibujables, el
//   recibido.
// Parametros:
// - pInterfaz. Direccion del interfaz a sacar de la lista de interfaces.
// Devuelve:
// - Si se ha sacado true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIManager::ExtractInterfazFromList(CGUIWindow* const pInterfaz)
{
  // SOLO si instancia activa
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pInterfaz);

  // Localiza y extrae el interfaz
  InterfazListIt It(m_ActiveInterfaces.begin());
  for (; It != m_ActiveInterfaces.end(); ++It) {
	// ¿Es el interfaz buscado?
	if (pInterfaz == (*It)) {
	  // Interfaz localizado
	  m_ActiveInterfaces.erase(It); 
	  return true;
	}
  }

  // No se localizo
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dibuja los interfaces activos asi como el cursor de GUI seleccionado (si
//   es que hay alguno) conjuntamente al posible grafico que tenga solapado y
//   texto flotante.
// - Tambien se encargara de dibujar las cortinas de cine si estas se hallan
//   activas.
// Parametros:
// Devuelve:
// Notas:
// - El texto flotante siempre ira por encima del cursor de GUI principal.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se procede a dibujar los interfaces activos
  InterfazListIt It(m_ActiveInterfaces.begin());
  for (; It != m_ActiveInterfaces.end(); ++It) {
	// Dibujado de interfaz
	ASSERT((*It)->IsInitOk());
	(*It)->Draw(); 	
  }

  // Se dibuja el cursor que proceda
  if (m_GUICursorsInfo.pActCursor) {	
	// Se obtienen posiciones de dibujado
	sPosition DrawPos(m_pInputManager->GetLastMousePos());
	
	// Clipping solo si hay sprite asociado es de scroll
	if (m_GUICursorsInfo.pActCursor == &m_GUICursorsInfo.GUIIsoScrollCursor) {	  
	  if (DrawPos.swXPos > m_GUICursorsInfo.uwScrollRightTopPos) {
		DrawPos.swXPos = m_GUICursorsInfo.uwScrollRightTopPos;
	  }
	  if (DrawPos.swYPos > m_GUICursorsInfo.uwScrollBottomTopPos) {
		DrawPos.swYPos = m_GUICursorsInfo.uwScrollBottomTopPos;
	  }	
	}

	// Se dibuja sprite asociado si es que hay
	// El sprite asociado se supondra que viene de una accion de GUI, luego
	// estara en la zona GUI y en el plano mas elevado
	if (m_GUICursorsInfo.pSpriteAttached) {
	  m_pGraphSys->Draw(GraphDefs::DZ_GUI,
						2,
						DrawPos.swXPos,
						DrawPos.swYPos,
						m_GUICursorsInfo.pSpriteAttached);
	}

	// Se dibuja cursor
	m_pGraphSys->Draw(GraphDefs::DZ_CURSOR,
					  0,
					  DrawPos.swXPos,
					  DrawPos.swYPos,
					  m_GUICursorsInfo.pActCursor);

	// Se dibuja texto flotante si procede
	if (m_FloatingTextInfo.Text.IsInitOk()) {	  
	  // Se establece posicion de texto y FXBack	  
	  // Nota: La posicion del texto flotante sera tal que haga que siempre sea
	  // visualizado el texto al completo.
	  sword swXFloatingTextPos = DrawPos.swXPos + m_GUICursorsInfo.pActCursor->GetWidth();
	  const word uwTextAreaWidth = swXFloatingTextPos + m_FloatingTextInfo.Text.GetWidth() + 6;
	  if (uwTextAreaWidth > m_pGraphSys->GetVideoWidth()) {
		swXFloatingTextPos -= (uwTextAreaWidth - m_pGraphSys->GetVideoWidth());
	  }
	  sword swYFloatingTextPos = DrawPos.swYPos + m_GUICursorsInfo.pActCursor->GetHeight();
	  const word uwTextAreaHeight = swYFloatingTextPos + m_FloatingTextInfo.Text.GetHeight();
	  if (uwTextAreaHeight > m_pGraphSys->GetVideoHeight()) {
		swYFloatingTextPos -= (uwTextAreaHeight - m_pGraphSys->GetVideoHeight());
	  }	  
	  ASSERT(m_FloatingTextInfo.Text.IsActive());
	  m_FloatingTextInfo.Text.SetXPos(swXFloatingTextPos);
	  m_FloatingTextInfo.Text.SetYPos(swYFloatingTextPos);
	  
	  // La posicion del FXBack siempre buscara centrarse con respecto al texto
	  m_FloatingTextInfo.FXBack.SetPosition(sPosition(swXFloatingTextPos - 4, 
													  swYFloatingTextPos - 1));
	  // Se dibuja FXBack y Texto
	  m_FloatingTextInfo.FXBack.Draw();
	  m_FloatingTextInfo.Text.Draw();	  
	}
  }

  // Se dibuja el efecto de cine si procede
  // Nota: Solo se dibujara si no esta cerrada y no se esta en estado de
  // presentacion
  if (m_GameInterfaces.GUIWindowState != GUIManagerDefs::GUIW_PRESENTATION &&
      !m_CutSceneInfo.CinemaCurtain.IsClosed()) {
	m_CutSceneInfo.CinemaCurtain.Draw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo destinado a recibir notificaciones de las ventanas de interfaz.
//   * De CGUIPresentation -> Para indicar el fin del FadeOut de inicio.
// Parametros:
// - IDGUIWindow. ID de la ventana de interfaz.
// - udParams. Parametros asociados.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIManager::GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
							 const dword udParams)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se comprueba la ventana de donde viene la notificacion
  switch(IDGUIWindow) {
	case GUIManagerDefs::GUIW_PRESENTATION: {
	  // De la pantalla de presentacion.
	  // Sera SEGURO la notificacion indicativa de que ha finalizado el FadeOut
	  // de inicio. 
	  // En el parametro se indicara la interfaz previa a la activacion de la
	  // interfaz de presentacion.
	  switch (udParams) {
		case GUIManagerDefs::GUIW_MAININTERFAZ: {
		  // Se desactiva MainInterfaz y se deja de dibujar el universo de juego
		  DeactiveMainInterfaz();
		  m_pWorld->SetDrawArea(false);
		} break;

		case GUIManagerDefs::GUIW_PLAYERPROFILE: {
		  // Se desactiva el interfaz de perfil de jugador 
		  DeactivePlayerProfile();
		} break;
	  }; // ~ switch
	} break;
  }; // ~ switch
}