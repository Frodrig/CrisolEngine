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
// CGUIManager.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUIManager
//
// Descripcion:
// - Contendra y coordinara todas las intancias del tipo CGUIW[X], esto es, a
//   todos los interfaces con los que trabajara el motor. Estos interfaces 
//   seran conocidos como interfaces de juego, pues estaran dentro de un
//   estado de juego concreto.
// - Se encargara de manejar el efecto de cine en pantalla, activando y 
//   desactivando interfaces asi como reconfigurando el motor isometrico.
//   Tendra asociado el interfaz de cliente de comando para recibir notificacion
//   una vez que la pantalla de cine se haya activado / desactivado.
// - El GUIManager tambien gestionara un conjunto de interfaces independientes 
//   del estado de interfaz del juego. En este grupo estaran los cuadros de
//   dialogo de notificacion de un mensaje, pregunta y los interfaces de lectura
//   de un bloque de texto y seleccion. Todos estos cuadros de dialogo podran
//   lanzarse en muchos puntos distintos del motor. Se cumplira tambien que 
//   estos cuadros de dialogo seran alojados y desalojados de memoria segun sean
//   solicitados.
// - Gestionara el trabajo con el texto de ayuda flotante.
//
// Notas:
// - Clase singlenton.
// - Todos los menus seran tratados como ventanas de interfaz. Las ventanas
//   de interfaz estaran ancladas en una clase base llamada CGUIWindow. 
//   Las ventanas de interfaz, estaran compuestas a su vez de componentes que
//   seran los encargados de notificar a esta los eventos que se vayan 
//   produciendo. Con esos eventos, la ventana de interfaz debera de ser 
//   capaz de implementar la logica del menu / interfaz que representa.
// - Todos los menus activos se guardaran en una lista, de tal forma que 
//   se pueda saber en todo momento los menus que hay que dibujar y la
//   preferencia que tienen unos sobre otros. Tipicamente, existiran dos
//   menus en esa lista como maximo. Por un lado el de ventana de interfaz
//   principal y por otro el ultimo menu que se haya abierto.
// - En el texto flotante, se considerara que no hay texto asociado cuando 
//   este se halle vacio.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIMANAGER_H_
#define _CGUIMANAGER_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _ICGUIWINDOWMANAGER_H_
#include "iCGUIManager.h"
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _ICCOMMANDCLIENT_H_
#include "iCCommandClient.h"
#endif
#ifndef _GUIMANAGERDEFS_H_
#include "GUIManagerDefs.h"
#endif
#ifndef _CGUIWMAINMENU_H_
#include "CGUIWMainMenu.h"
#endif
#ifndef _CGUIWCHARACTERCREATOR_H_
#include "CGUIWCharacterCreator.h"
#endif
#ifndef _CGUIWGAMEMENU_H_
#include "CGUIWGameMenu.h"
#endif
#ifndef _CGUIWMAININTERFAZ_H_
#include "CGUIWMainInterfaz.h"
#endif
#ifndef _CGUIWENTITYINTERACT_H_
#include "CGUIWEntityInteract.h"
#endif
#ifndef _CGUIWITEMSELECTOUSE_H_
#include "CGUIWItemSelectToUse.h"
#endif
#ifndef _CGUIWPLAYERPROFILE_H_
#include "CGUIWPlayerProfile.h"
#endif
#ifndef _CGUIWGAMELOADING_H_
#include "CGUIWGameLoading.h"
#endif
#ifndef _CGUIWCONVERSATOR_H_
#include "CGUIWConversator.h"
#endif
#ifndef _CGUIWCOMBAT_H_
#include "CGUIWCombat.h"
#endif
#ifndef _CCINEMACURTAIN_H_
#include "CCinemaCurtain.h"
#endif
#ifndef _COPENCINEMACURTAINCMD_H_
#include "COpenCinemaCurtainCmd.h"
#endif
#ifndef _CCLOSECINEMACURTAINCMD_H_
#include "CCloseCinemaCurtainCmd.h"
#endif
#ifndef _CGUIWDIALOG_H_
#include "CGUIWDialog.h"
#endif
#ifndef _CGUIWTEXTREADER_H_
#include "CGUIWTextReader.h"
#endif
#ifndef _CGUIWTEXTOPTIONSSELECTOR_H_
#include "CGUIWTextOptionsSelector.h"
#endif
#ifndef _CGUIWTRADE_H_
#include "CGUIWTrade.h"
#endif
#ifndef _CGUIWPRESENTATION_H_
#include "CGUIWPresentation.h"
#endif
#ifndef _CGUIWSAVEDFILEMANAGER_H_
#include "CGUIWSavedFileManager.h"
#endif
#ifndef _CGUICSINGLETEXT_H_
#include "CGUICFlotaingText.h"
#endif
#ifndef _ICGUIWINDOWCLIENT_H_
#include "iCGUIWindowClient.h"
#endif
#ifndef _CENTITYSELECTOR_H_
#include "CEntitySelector.h"
#endif
#ifndef _CFXBACK_H_
#include "CFXBack.h"
#endif
#ifndef _LIST_H_
#include <list>
#define _LIST_H_
#endif
#ifndef _MAP_H_
#include <map>
#define _MAP_H_
#endif

// Defincion de clases / estructuras / espacios de nombres
class CGUIWindow;
class CCriature;
class CWorldEntity;
struct iCWorld;
struct iCGraphicSystem;
struct iCInputManager;
struct iCCommandManager;

// Clase CGUIManager
class CGUIManager: public iCGUIManager, 
				   public iCCommandClient,
				   public iCGUIWindowClient
{
private:
  // Tipos
  // Pila a interfaces activos
  typedef std::list<CGUIWindow*> InterfazList;   
  typedef InterfazList::iterator InterfazListIt;   

private:
  // Enumerados
  enum {
	// Estados de animacion asociados a los distintos cursores de GUI
	// Movimiento
	MOV_POSSIBLE_ANIMSTATE = 0,
	MOV_NO_POSSIBLE_ANIMSTATE = 1,
	// Interaccion con entidades
	ENTITY_INTERACT_POSSIBLE_ANIMSTATE = 0,
	ENTITY_INTERACT_NO_POSSIBLE_ANIMSTATE = 1,
	// Interaccion con entidades para golpear
	ENTITY_INTERACT_FORHIT_POSSIBLE_ANIMSTATE = 0,
	ENTITY_INTERACT_FORHIT_NO_POSSIBLE_ANIMSTATE = 1,
	// Estado de espera
	WAITING_CURSOR_ANIMSTATE = 0,
	// Scroll
	NORTHSCROLL_ANIMSTATE = 0,
	NORTHEASTSCROLL_ANIMSTATE = 1,
	EASTSCROLL_ANIMSTATE = 2,
	SOUTHEASTSCROLL_ANIMSTATE = 3,
	SOUTHSCROLL_ANIMSTATE = 4,
	SOUTHWESTSCROLL_ANIMSTATE = 5,
	WESTSCROLL_ANIMSTATE = 6,
	NORTHWESTSCROLL_ANIMSTATE = 7,
	// Interaccion con ventanas de interfaz
	WINDOWINTERFAZ_ANIMSTATE = 0
  };

  enum {
	// Constantes varias
	CINEMACURTAIN_TOPHEIGHT  = 48, // Altura superior de la cortina de cine
	CINEMACURTAIN_BASEHEIGHT = 96, // Altura inferior de la cortina de cine

	// Identificadores varios
	ID_FLOATINGTEXT = 0X01     // Identificador para el componente de texto flotante
  };

private:  
  // Estructuras   
  struct sGUICursorsInfo {
	// Info asociada a los cursores de GUI  
	// Datos    
	GUIManagerDefs::eGUICursorType ActCursor; // Cursor actual  
	// Cursores disponibles
	CSprite  GUIMovementCursor;				// Cursor de movimiento
	CSprite  GUIEntityInteractCursor;		// Cursor de interaccion
	CSprite  GUIEntityInteractForHitCursor; // Cursor de interaccion para golpear
	CSprite  GUIWaitingCursor;				// Cursor de espera
	CSprite  GUIIsoScrollCursor;			// Cursor de scroll
	CSprite  GUIWindowInteractCursor;		// Cursor de ventana de interfaz
	CSprite* pActCursor;					// Direccion al cursor actual    
	// Datos asociados
	CSprite* pSpriteAttached;      // Grafico asociado al cursor 
	word     uwScrollRightTopPos;  // Pos tope a la dcha. para los cursores de scroll
	word     uwScrollBottomTopPos; // Pos tope inferior para los cursores de scroll  
	word     uwGUICursorsWidth;    // Ancho cursores
	word     uwGUICursorsHeight;   // Alto cursores  	
  };

  struct sCutSceneInfo {
	// Info asociada al efecto de cine para realizar cutscenes
	// Enumerados
	enum {
	  // ID asociados a los comandos 
	  ID_OPENCURTAIN_CMD = 1,
	  ID_CLOSECURTAIN_CMD
	};
	// Info sobre la cortina y comandos de manejo
	CCinemaCurtain         CinemaCurtain;   // Cortinas de cine
	COpenCinemaCurtainCmd  OpenCurtainCmd;  // Comando de apertura de cortinas
	CCloseCinemaCurtainCmd CloseCurtainCmd; // Comando de cierre de cortinas  
	// Info del posible cliente externo
	iCCommandClient*       pClient;      // Cliente a recibir notificacion
	CommandDefs::IDCommand IDCommand;    // Identificador del comando
	dword                  udExtraParam; // Parametro extra
  };

  struct sFloatingTextInfo {
	// Info asociada al texto flotante	
	CGUICSingleText Text;   // Texto flotante
	CFXBack         FXBack; // Texto de fondo	
	// Configuracion base para todos los textos flotantes
	CGUICSingleText::sGUICSingleTextInitData CfgBase;
  }; 


  struct sGameInterfaces {
	// Interfaces de juego, que estaran supeditados a un estado de juego
	// en concreto, no pudiendo existir si no se esta en el estado que corresponda
	// Interfaces
	CGUIWMainMenu				MainMenu;         // GUI de menu principal
	CGUIWCharacterCreator		CharacterCreator; // GUI del creador de caracteres
	CGUIWGameMenu				GameMenu;         // GUI de menu de juego
	CGUIWEntityInteract			EntityInteract;   // GUI para la interaccion con entidades
	CGUIWItemSelectToUse		ItemSelectToUse;  // GUI de seleccion de item para usar
	CGUIWPlayerProfile			PlayerProfile;    // GUI para la hoja del personaje
	CGUIWMainInterfaz			MainInterfaz;     // GUI de interfaz principal
	CGUIWGameLoading			GameLoading;      // GUI indicativa de carga de un juego
	CGUIWConversator			Conversator;      // GUI de conversacion
	CGUIWTrade					Trade;            // GUI de intercamio
	CGUIWCombat					Combat;           // GUI de info. de combate
	CGUIWPresentation           Presentation;     // GUI de presentacion
	CGUIWLoaderSavedFileManager LoaderSavedFile;  // GUI de carga de archivos	
	CGUIWSaverSavedFileManager  SaverSavedFile;   // GUI de guardar / borrar archivos
	// Ventana de interfaz actual  
	GUIManagerDefs::eGUIWindowType GUIWindowState;   
	// Resto de vbles
	bool bInputActive; // ¿Input activado en los interfaces de juego actuales?  
  };

  struct sGeneralInterfaces {
	// Interfaces de caracter general que no estan supeditados a
	// ningun estado de juego en concreto y que podran activarse en casi	
	// Interfaces
	CGUIWAdviceDialog        AdviceDialog;       // Dialogo de aviso
	CGUIWQuestionDialog      QuestionDialog;     // Dialogo de pregunta  
	CGUIWTextReader          TextReader;         // Lector de archivos de texto
	CGUIWTextOptionsSelector TextOptionSelector; // Selector de opciones de texto
  };
  
private:
  // Instancia singlenton
  static CGUIManager* m_pGUIWindowManager; // Unica instancia a la clase

  // Interfaces a otros modulos del motor
  iCGraphicSystem*   m_pGraphSys;     // Subsistema grafico
  iCInputManager*    m_pInputManager; // Manager de entrada
  iCWorld*           m_pWorld;        // Universo de juego
  iCFontSystem*      m_pFontSystem;   // Manejador de fuentes
  iCCommandManager*  m_pCmdManager;   // Manejador de comandos
  
  // Ventanas de interfaz
  sGameInterfaces    m_GameInterfaces;   // Interfaces de juego
  sGeneralInterfaces m_GenInterfaces;    // Interfaces generales
  InterfazList		 m_ActiveInterfaces; // Lista de interfaces activos

  // Trabajo con resto de elementos de GUI
  sGUICursorsInfo	  m_GUICursorsInfo;   // Info asociada a los cursores de GUI
  sCutSceneInfo		  m_CutSceneInfo;     // Info asociado al efecto de cutscene  
  sFloatingTextInfo   m_FloatingTextInfo; // Info asociada al texto flotante
  CEntitySelector     m_MapSelector;   // Selector de entidades
    
  // Resto vbles    
  bool m_bIsInitOk; // ¿Clase inicializada?

protected:
  // Constructor / destructor
  CGUIManager(void): m_pWorld(NULL),
					 m_pGraphSys(NULL),
					 m_pInputManager(NULL),
					 m_pCmdManager(NULL),
					 m_bIsInitOk(false) { }
public:
  ~CGUIManager(void) { End(); }

public:
  // Protocolo de inicio y fin de instancia
  bool Init(void);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }
private:
  // Metodos de apoyo
  bool InitGUICursors(const std::string& szATGUIMovementCursor,
					  const std::string& szATGUIEntityInteractCursor,
					  const std::string& szATGUIEntityInteractForHitCursor,
					  const std::string& szATGUIWaitingCursor,
					  const std::string& szATGUIScrollCursor,
					  const std::string& szATGUIWindowInteractCursor);
  bool InitCutSceneInfo(void);
  void InitFloatingTextCfg(void);
  void EndGUICursors(void);
  void EndCutSceneInfo(void);  

public:
  // Obtencion y destruccion de la instancia singlenton
  static inline CGUIManager* const GetInstance(void) {
    if (NULL == m_pGUIWindowManager) { 
      m_pGUIWindowManager = new CGUIManager; 
      ASSERT(m_pGUIWindowManager)
    }
    return m_pGUIWindowManager;
  }
  static inline void DestroyInstance(void) {
    if (m_pGUIWindowManager) {
      delete m_pGUIWindowManager;
      m_pGUIWindowManager = NULL;
    }
  }

public:
  // Metodos generales de trabajo con interfaces
  GUIManagerDefs::eGUIWindowType GetGameGUIWindowState(void) const {
	ASSERT(IsInitOk());
	// Retorna el estado de interfaz de juego actual
	return m_GameInterfaces.GUIWindowState;
  }

public:
  // Dibujado
  void Draw(void);

public:
  // Trabajo con interfaz de menu principal
  // iCGUIManager
  void SetMainMenuWindow(void);
private:
  void ActiveMainMenu(void);
  void DeactiveMainMenu(void);

public:
  // Trabajo con interfaz de creacion de caracteres
  // iCGUIManager
  void SetCharacterCreatorWindow(void);
private:
  void ActiveCharacterCreator(void);
  void DeactiveCharacterCreator(void);

public:
  // Trabajo con interfaz de menu de juego
  // iCGUIManager
  void SetGameMenuWindow(void);
private:
  void ActiveGameMenu(void);
  void DeactiveGameMenu(void);

public:
  // Trabajo con interfaz de ventana principal
  // iCGUIManager
  void SetMainInterfazWindow(void);
  void MainInterfazSetMode(const WorldDefs::eWorldMode& Mode, 
						   const AreaDefs::EntHandle& hCriatureInTurn = 0) {
	ASSERT(IsInitOk());
	// Establece el modo de funcionamiento del interfaz
	m_GameInterfaces.MainInterfaz.SetInterfazMode(Mode, hCriatureInTurn);
  }
  bool IsKeyPressedInMainInterfaz(const InputDefs::eInputEventCode& KeyPressed) const {
	ASSERT(IsInitOk());
	// Retorna el flag
	return m_GameInterfaces.MainInterfaz.IsKeyPressed(KeyPressed);
  }
public:
  inline bool MainInterfazInGameAreaLoading(const word uwIDArea,
											const AreaDefs::sTilePos& PlayerPos) {
	ASSERT(IsInitOk());
	// Ordena al interfaz que realice el filtrado de la carga de un area
	return m_GameInterfaces.MainInterfaz.InGameAreaLoading(uwIDArea, PlayerPos);
  }
  
private:
  void ActiveMainInterfaz(const bool bDoFadeIn = true);
  void DeactiveMainInterfaz(void);
  
public:
  // Trabajo con interfaz de interaccion con entidades en juego  
  // iCGUIManager
  void SetEntityInteractWindow(const sPosition& InitDrawPos,
							   CCriature* const pEntitySrc,
							   CWorldEntity* const pEntityDest);
private:
  void ActiveEntityInteract(const sPosition& InitDrawPos,
							CCriature* const pEntitySrc,
							CWorldEntity* const pEntityDest,
							const GUIManagerDefs::eGUIWindowType& ActiveContext);
  void DeactiveEntityInteract(void);

public:
  // Trabajo con interfaz de seleccion de item para usar en juego
  // iCGUIManager
  void SetItemSelectToUseWindow(CCriature* const pCriatureSrc,
							    CWorldEntity* const pEntityDest);
private:
  void ActiveItemSelectToUse(CCriature* const pCriatureSrc,
							 CWorldEntity* const pEntityDest);
  void DeactiveItemSelectToUse(void);

public:
  // Trabajo con interfaz de inventario / perfil de jugador
  // iCGUIManager
  void SetPlayerProfileWindow(void);
private:
  void ActivePlayerProfile(const bool bDoFadeIn = false);
  void DeactivePlayerProfile(void);
  
public:
  // Trabajo con interfaz de carga de area
  // iCGUIManager
  void SetGameLoadingWindow(const word uwIDArea,
							CPlayer* const pPlayer = NULL,
							const AreaDefs::sTilePos& PlayerPos = AreaDefs::sTilePos(0, 0));
private:
  void ActiveGameLoading(const word uwIDArea,
						 CPlayer* const pPlayer,
						 const AreaDefs::sTilePos& PlayerPos);  
  void DeactiveGameLoading(void);

public:
  // Trabajo con interfaz para conversaciones entre entidades
  // iCGUIManager
  bool SetConversatorWindow(CWorldEntity* const pMainConv,
							CWorldEntity* const pSecondaryConv);
  bool AddOptionToConversatorWindow(const GUIDefs::GUIIDComponent& IDOption,
								    const std::string& szText);
  void ResetOptionsInConversatorWindow(void);
  bool GetOptionFromConversatorWindow(iCGUIWindowClient* const pClient);
private:
  void ActiveConversatorWindow(CWorldEntity* const pMainConv,
							   CWorldEntity* const pSecondaryConv);
  void DeactiveConversatorWindow(void);

public:
  // Trabajo con la interfaz de intercambio de datos
  // iCGUIWManager
  bool SetTradeWindow(CCriature* const pMainCriature,
					  CWorldEntity* const pEntity,
					  iCGUIWindowClient* const pClient);  
private:
  void ActiveTradeWindow(CCriature* const pMainCriature,
						 CWorldEntity* const pEntity,
						 iCGUIWindowClient* const pClient);
  void DeactiveTradeWindow(void);

public:
  // Trabajo con la interfaz de combate
  // iCGUIManager
  void SetCombatWindow(const AreaDefs::EntHandle& hCriature);
  void CombatSetActionPoints(const RulesDefs::CriatureActionPoints& ActionPoints) {
	ASSERT(IsInitOk());
	ASSERT(m_GameInterfaces.Combat.IsInitOk());
	ASSERT(m_GameInterfaces.Combat.IsActive());
	// Se traslada la peticion al interfaz
	m_GameInterfaces.Combat.SetActionPoints(ActionPoints);
  }
private:
  void ActiveCombatWindow(const AreaDefs::EntHandle& hCriature);
  void DeactiveCombatWindow(void);

public:
  // iCGUIManager / Interfaz de presentacion
  bool SetPresentationWindow(const std::string& szPresentationProfile);
private:
  void ActivePresentation(const std::string& szPresentationProfile,
						  const GUIManagerDefs::eGUIWindowType& PrevInterfaz);
  void DeactivePresentation(void);

public:
  // iCGUIManager / Interfaz de cargar partidas
  void SetLoaderSavedFileWindow(void);
private:
  void ActiveLoaderSavedFile(const GUIManagerDefs::eGUIWindowType& PrevInterfaz);
  void DeactiveLoaderSavedFile(void);

public:
  // iCGUIManager / Interfaz de guardar partidas
  void SetSaverSavedFileWindow(void);
private:
  void ActiveSaverSavedFile(void);
  void DeactiveSaverSavedFile(void);

public:
  // Trabajo con interfaces de caracter general
  // iCGUIManager / Cuadros de dialogo
  bool ActiveAdviceDialog(const std::string& szText,
					      iCGUIWindowClient* const pClient);
  bool ActiveQuestionDialog(const std::string& szText,
						    const bool bCancelPresent,
						    iCGUIWindowClient* const pClient);
  void DeactiveAdviceDialog(void);
  void DeactiveQuestionDialog(void);
  // iCGUIManager / Lector de archivos de texto
  bool ActiveTextReader(const std::string& szTitle,
						const std::string& szTextFileName,
						iCGUIWindowClient* const pClient);
  void DeactiveTextReader(void);  
  // iCGUIManager / Seleccionador de opciones de texto general
  bool ActiveTextOptionsSelector(const std::string& szTitle,
								 const bool bCanUseCancel,
								 iCGUIWindowClient* const pClient);
  void DeactiveTextOptionsSelector(void);
  bool AddOptionToTextOptionsSelector(const GUIDefs::GUIIDComponent& IDOption,
									  const std::string& szText,
									  const bool bCanSelect = true);
  void ReleaseTextOptionsSelector(void);

private:
  // Metodos de apoyo
  bool CanActiveGeneralInterfaz(const GUIManagerDefs::eGUIWindowType& Interfaz) const;

private:
  // Metodos de apoyo para el bloqueo / desbloqueo de la entrada
  void SetGameInterfacesInput(const bool bInput);
  void SetGeneralInterfacesInput(const bool bInput);

public:
  // iCCommandClient / Cliente de comandos
  void EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
					const dword udInstant,
					const dword udExtraParam);  

public:
  // iCGUIManager / Trabajo con los cursores de interfaz
  void SetGUICursor(const GUIManagerDefs::eGUICursorType& Cursor);  
  CSprite* AttachSpriteToGUICursor(CSprite* const pSprite);
  inline GUIManagerDefs::eGUICursorType GetActGUICursor(void) const {
	ASSERT(IsInitOk());
	// Retorna tipo cursor actual
	return m_GUICursorsInfo.ActCursor;
  }
  inline byte GetGUICursorsHeight(void) const {
	ASSERT(IsInitOk());
	// Retorna altura del cursor actual
	return m_GUICursorsInfo.uwGUICursorsHeight;  
  }
  inline byte GetGUICursorsWidth(void) const {
	ASSERT(IsInitOk());
	// Retorna anchura del cursor actual
	return m_GUICursorsInfo.uwGUICursorsWidth;  
  }

public:
  // iCGUIManager / Modo de Cutscene
  void BeginCutScene(iCCommandClient* const pClient = NULL,
					 const CommandDefs::IDCommand& IDCommand = 0,
					 const dword udExtraParam = 0);
  void EndCutScene(iCCommandClient* const pClient = NULL,
				   const CommandDefs::IDCommand& IDCommand = 0,
				   const dword udExtraParam = 0);
  bool IsCutSceneActive(void) const {
	ASSERT(IsInitOk());
	// Retorna flag
	return m_CutSceneInfo.CinemaCurtain.IsOpen();
  }
private:
  // Metodos de apoyo
  void OnOpenCinemaCurtainCmdEnd(void);
  void OnCloseCinemaCurtainCmdEnd(void);

public:
  // iCGUIManager / Trabajo con la consola de interfaz
  void WriteToConsole(const std::string& szText, 
					  const bool bPlaySound = false);
  void ResetConsoleInfo(void);

public:
  // Trabajo con texto flotante
  // iCGUIManager
  void SetFloatingText(const std::string& szText,
					   const GraphDefs::sRGBColor& RGBColor,
					   const GraphDefs::sAlphaValue& Alpha,
					   const GUIManagerDefs::eFloatingTextType& Type);
  void UnsetFloatingText(void) {
	ASSERT(IsInitOk());
	// Quita el texto actual flotante de forma incondicional
	m_FloatingTextInfo.Text.End();
	m_FloatingTextInfo.FXBack.End();
  }
  void UnsetFloatingText(const std::string& szText) {
	ASSERT(IsInitOk());
	// Quita el texto SOLO si el recibido es el mismo que el actual
	if (0 == m_FloatingTextInfo.CfgBase.szText.compare(szText)) {
	  UnsetFloatingText();
	}
  }

private:
  // Metodos de apoyo
  void InsertInterfazInList(CGUIWindow* const pInterfaz);
  bool ExtractInterfazFromList(CGUIWindow* const pInterfaz);

public:
  // iCGUIWindowClient / Notificacion de un determinado suceso en un interfaz
  void GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
					   const dword udParams);

public:
  // Trabajo con el selector de entidades global
  // iCGUIManager
  iCEntitySelector* const GetMapSelector(void) {
	ASSERT(IsInitOk());
	// Retorna el selector de entidades
	return &m_MapSelector;
  }
};

#endif // ~ CGUIManager

