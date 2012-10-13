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
// CGUIWMainInterfaz.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUIWMainInterfaz
//
// Descripcion:
// - Implementa el menu de ventana principal. Este menu comprende las acciones
//   que se realizan sobre la ventana inferior en modo juego y ademas el scroll
//   sobre el motor isometrico.
//
// Notas:
// - Desde el MainInterfaz tambien se filtrara la carga de un area desde
//   el juego activo.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWMAININTERFAZ_H_
#define _CGUIWMAININTERFAZ_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabecera
#ifndef _ICCRIATUREOBSERVER_H_
#include "iCCriatureObserver.h"
#endif
#ifndef _ICWORLDOBSERVER_H_
#include "iCWorldObserver.h"
#endif
#ifndef _CGUIWINDOW_H_
#include "CGUIWindow.h"
#endif
#ifndef _CGUICBITMAP_H_
#include "CGUICBitmap.h"
#endif
#ifndef _CGUICBLOCKTEXT_H_
#include "CGUICBlockText.h"
#endif
#ifndef _CGUICBUTTON_H_
#include "CGUICButton.h"
#endif
#ifndef _CGUICSPRITESELECTOR_H_
#include "CGUICSpriteSelector.h"
#endif
#ifndef _CGUICLINETEXT_H_
#include "CGUICLineText.h"
#endif
#ifndef _CGUICPERCENTAGEBAR_H_
#include "CGUICPercentageBar.h"
#endif
#ifndef _CCYCLICFADECMD_H_
#include "CCyclicFadeCmd.h"
#endif
#ifndef _GUIMANAGERDEFS_H_
#include "GUIManagerDefs.h"
#endif
#ifndef _CFXBACK_H_
#include "CFXBack.h"
#endif
#ifndef _WORLDEFS_H_
#include "WorldDefs.h"
#endif
#ifndef _ICSCRIPTCLIENT_H_
#include "iCScriptClient.h"
#endif
#ifndef _ICEQUIPMENTSLOTS_H_
#include "iCEquipmentSlots.h"
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif
#ifndef _SET_H_
#define _SET_H_
#include <set>
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif

// Defincion de clases / estructuras / espacios de nombres
struct iCWorld;
struct iCGUIManager;
struct iCGraphicSystem;
struct iCAudioSystem;
struct iCGameDataBase;
struct iCFontSystem;
struct iCVirtualMachine;
class CCriature;
class CItem;
class CWorldEntity;
class CCBTEngineParser;

// clase CGUIWMainInterfaz
class CGUIWMainInterfaz: public CGUIWindow,
						 public iCCriatureObserver,
						 public iCWorldObserver,
						 public iCScriptClient,
						 public iCEquipmentSlotsObserver
{  
private:
  // Tipos
  typedef CGUIWindow Inherited;  // Acceso a la clase base      
    
private:
  // Enumerados
  enum eIDComponent {
	// ID asociados a los componentes
	ID_BACKGROUND = 0X01,    // Imagen de fondo
	ID_CONSOLE,              // Bloque de texto de consola
	ID_CONSOLE_SCROLL_UP,    // Boton de scroll hacia arriba para consola
	ID_CONSOLE_SCROLL_DOWN,  // Boton de scroll hacia abajo para consola
	ID_SLOT_LEFT,            // Slot de equipamiento a izq.
	ID_SLOT_RIGHT,           // Slot de equipamiento a dcha.
	ID_SLOTNAME_LEFT,        // Nombre del slot de equip. a izq.
	ID_SLOTNAME_RIGHT,       // Nombre del slot de equip. a dcha
	ID_PORTRAIT,             // Retrato
	ID_ATTRIB_NAMES,         // Nombres de atributos
	ID_ATTRIB_NUMERIC,       // Valores numericos asociados a atributos
	ID_ATTRIB_PERCENTAGES,   // Porcentajes asociados a atributos
	ID_NEXTTURNBUTTON,       // Boton de combate / pasar de turno
	ID_FLEECOMBATBUTTON,     // Boton de combate / huir de combate
	ID_SYMPTOM,              // Icono (cualquiera) de sintoma
	ID_TIMECONTROLLER,       // Interfaz de control horario
  };

  enum eIsoScrollCursors {
	// Cursores de scroll isometrico y su localizacion en el array de botones
	ISOSCROLL_NORTH = 0,
	ISOSCROLL_SOUTH,
	ISOSCROLL_EAST,
	ISOSCROLL_WEST,
	ISOSCROLL_NORTHEAST,
	ISOSCROLL_NORTHWEST,
	ISOSCROLL_SOUTHEAST,
	ISOSCROLL_SOUTHWEST,
	ISOSCROLL_NUMCURSORS,
	ISOSCROLL_NO_SCROLL
  };

  enum eInterfazState {
	// Estados asociados con el interfaz
	DESACTIVATED = 0, // Interfaz desactivado
	MOVING_PLAYER,    // Moviendo al jugador
	ENTITY_INTERACT,  // Interaccion con entidades
	WINDOW_INTERACT,  // Interaccon con ventana de interfaz
	INIT              // Estado de inicializacion
  };

  enum {
	// Identificadores de fade	
	ID_ENDFADEIN = 0X01,       // Idenficador del final del FadeIn
	ID_ENDAREALOADING_FADEOUT, // FadeOut asociado a una carga de un area
  };

private:
  // Tipos
  // Conjunto donde localizar los codigos de las teclas pulsadas
  typedef std::set<InputDefs::eInputEventCode> KeySet;
  typedef KeySet::iterator                      KeySetIt;

private:
  // Estructuras
  struct sInterfazInfo {
	// Info asociada a la interfaz general
	// Estados
	eInterfazState State;     // Estado de interfaz actual
	eInterfazState PrevState; // Estado de interfaz previo
	
	// Modo de funcionamiento (real / por turnos)
	WorldDefs::eWorldMode Mode;            // Modo de funcionamiento
	CCriature*            pCriatureInTurn; // Criatura con el turno 
	
	// Posiciones de movimiento
	sPosition Pos;
	
	// Interaccion
	CWorldEntity*      pEntityInteract;   // Entidad de interaccion	
	AreaDefs::sTilePos EntityInteractPos; // Pos. de la entidad de interaccion
  };

  struct sFloatingTextInfo {
	// Info asociada al texto flotante
	// Nota: Los valores Alpha y Type seran preestablecidos
	GraphDefs::sRGBColor              RGBColor; // Color para texto flotante
	GraphDefs::sAlphaValue            Alpha;    // Alpha
	GUIManagerDefs::eFloatingTextType Type;     // Formato de muestra
  };
  
  struct sScrollInfo {  
	// Scroll en el isometrico  
	// Cursores de scroll isometrico
	// Estado de scroll
	GUIManagerDefs::eGUICursorType ScrollCursor; 
	word uwXDrawPos;            // Pos en X donde dibujar
	word uwYDrawPos;            // Pos en Y donde dibujar
	word uwDrawRightPos;        // Pos. de dibujado mas a la dcha
	word uwDrawBottomPos;       // Pos. de dibujado mas abajo
	word uwRightCursorTop;      // Tope a dcha para el cursor
	word uwBottomCursorTop;     // Tope hacia abajo para el cursor
	word uwRightDiagCursorTop;  // Tope en diagonal a dcha para el cursor
	word uwBottomDiagCursorTop; // Tope en diagonal hacia abajo para el cursor
  };

  struct sBackImgInfo {  
	// Informacion relativa a la imagen de fondo
	CGUICBitmap Background; // Imagen de fondo       	
  };

  struct sConsoleInfo {
	// Informacion referida a la consola
	CGUICBlockText Text;       // Consola de texto
	CFXBack        FXBack;     // FXBack
	CGUICButton    ButtonUp;   // Scroll hacia arriba en consola
	CGUICButton    ButtonDown; // Scroll hacia abajo en consola  
  };

  struct sEquipmentSlotsInfo {
	// Estructuras
	struct sSlotInfo {
	  // Info asociada a un slot
	  CGUICSpriteSelector Slot;     // Componente slot
	  CSprite             HandIcon; // Icono a usar cuando no hay item equipado
	  CGUICLineText       SlotName; // Nombre del slot
	};
	// Informacion relativa a los slots con los items
	sSlotInfo LeftSlot;  // Slot izquierdo
	sSlotInfo RightSlot; // Slot derecho
	// Info relativa al arma seleccionada para atacar
	bool                        bIsWeaponSelect; // ¿Hay un slot con arma seleccionado?
	RulesDefs::eIDEquipmentSlot WeaponSlot;      // Slot con arma seleccionado
  };

  struct sPortraitInfo {
	// Informacion relativa a la cara (retrato)
	CGUICSpriteSelector Portrait; // Slot asociado al retrato
  };

  struct sCombatOptionsInfo {
	// Informacion relativa a los botones de combate
	CGUICButton NextTurn;   // Boton de siguiente turno
	CGUICButton FleeCombat; // Boton de huida de combate	
  };

  struct sAttributesInfo {
	// Info relativa a los atributos del personaje
	// Enumerados
    enum {
	  MAX_ATTRIBUTES_TO_SHOW = 2 // Max. numero de atributos a enseñar en ventana
	};
	// Estructuras
	struct sAttrib {
	  // Datos asociados a un atributo
	  // Nota: Si ubID = 0 no se usa el atributo. Si 1 es salud. En otro caso
	  // es atributo extendido
	  byte               ubID;       // ID al atributo que referencia
	  CGUICLineText      Name;       // Nombre del atributo
	  CGUICLineText      Numeric;    // Valor numerico
	  CGUICPercentageBar Percentage; // Barra de porcentaje
	  // Constructor
	  sAttrib(void): ubID(0) { }
	};
	// Datos
	sAttrib Attribs[MAX_ATTRIBUTES_TO_SHOW]; // Atributos a mostrar    
  };

  struct sSymptomsInfo {
	// Info asociada a los sintomas	
	// Estructuras forward
	struct sNSymptomIcon;
	// Tipos
	// Map de componentes CGUICButton con los sintomas
	typedef std::map<RulesDefs::eIDSymptom, sNSymptomIcon*> SymptomIconMap;
	typedef SymptomIconMap::iterator                        SymptomIconMapIt;
	typedef SymptomIconMap::value_type                      SymptomIconMapValType;
	// Lista con los identificadores de los sintomas activos
	typedef std::set<RulesDefs::eIDSymptom> SymptomActiveSet;
	typedef SymptomActiveSet::iterator      SymptomActiveSetIt;
	// Datos
	SymptomIconMap   Icons;          // Iconos de sintomas
	SymptomActiveSet ActiveSymptoms; // Lista de sintomas activos	
  };

  struct sTimeControllerInfo {
	// Info asociada a la interfaz para el control del tiempo
	CGUICButton::sGUICButtonInitData CfgBaseButton; // Cfg base de los botones
	byte                             ubActHour;     // Hora actual que refleja
	CGUICButton                      ActButton;     // Boton actual
	GraphDefs::sAlphaValue           NoSelectAlpha; // Alpha de no seleccion
  };

  struct sInGameAreaLoading {
	// Info asociada a la carga de un area desde juego	
	bool               bIsLoading;       // ¿Se esta cargando?
	word               uwIDArea;         // Identificador del area
	CPlayer*           pPlayer;          // Instancia al jugador
	AreaDefs::sTilePos PlayerPos;        // Posicion del jugador
	std::string        szAreaLoadingMsg; // Mensaje de area cargando
  };

private:
  // Interfaces a otros subsistemas
  iCWorld*          m_pWorld;      // Interfaz al universo de juego
  iCGraphicSystem*  m_pGraphSys;   // Subsistema grafico
  iCGUIManager*     m_pGUIManager; // Manager del GUI
  iCGameDataBase*   m_pGDBase;     // Interfaz a la base de datos
  iCFontSystem*     m_pFontSys;    // Subsistema de fuentes
  iCVirtualMachine* m_pVMachine;   // Maquina virtual

  // Informacion general de interfaz
  sInterfazInfo     m_InterfazInfo;     // Info asociada al estado de interfaz
  sFloatingTextInfo m_FloatingTextInfo; // Datos asociados al texto flotante
  
  // Componentes del interfaz
  sBackImgInfo         m_BackImgInfo;       // Info relativa a la imagen de fondo
  sScrollInfo          m_ScrollInfo;        // Info relativa a los cursores de scroll
  sConsoleInfo         m_ConsoleInfo;       // Info relativa a la consola
  sEquipmentSlotsInfo  m_EquipSlotsInfo;    // Info relativa a los slots
  sPortraitInfo        m_PortraitInfo;      // Info relativa al retrato
  sAttributesInfo      m_AttributesInfo;    // Info relativa a los porcentajes
  sCombatOptionsInfo   m_CombatOptionsInfo; // Info relatia al control de modo de combate
  sSymptomsInfo        m_SymptomsInfo;      // Info relativa a los sintomas activos
  sTimeControllerInfo  m_TimeController;    // Info asociada al controlador de horas
  
  // Resto  
  sInGameAreaLoading m_InGameAreaLoading;  // Info asociada a la carga de un area
  sRect				 m_InterfazBarArea;    // Area asociada a la barra de interfaz	
  sRect				 m_TimeControllerArea; // Area asociada al controlador de tiempo
  KeySet             m_KeyPressedSet;      // Conjunto de teclas pulsadas
    
public:
  // Constructor / destructor  
  CGUIWMainInterfaz(void): m_pGUIManager(NULL),
						   m_pGraphSys(NULL),
						   m_pGDBase(NULL), 
						   m_pWorld(NULL),
						   m_pFontSys(NULL),
						   m_pVMachine(NULL) { }
  ~CGUIWMainInterfaz(void) { End(); }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(void);
  void End(void);  
private:
  // Metodos de apoyo
  void InitIsoCursors(void);  
  void InitInterfazState(void);  
private:
  // Metodos de apoyo para la carga y configuracion de componentes
  bool TLoad(void);
  bool TLoadFloatingText(CCBTEngineParser* const pParser);
  bool TLoadBackImg(CCBTEngineParser* const pParser);
  bool TLoadConsole(CCBTEngineParser* const pParser);
  bool TLoadEquipmentSlots(CCBTEngineParser* const pParser);
  bool TLoadPortraitInfo(CCBTEngineParser* const pParser);
  bool TLoadCombatButton(CCBTEngineParser* const pParser);
  bool TLoadAttribInfo(CCBTEngineParser* const pParser);  
  bool TLoadCombatOptionsInfo(CCBTEngineParser* const pParser);
  bool TLoadSymptomInfo(CCBTEngineParser* const pParser);
  bool TLoadTimeControllerInfo(CCBTEngineParser* const pParser);  
private:
  // Metodos de apoyo para la finalizacion de componentes
  void EndComponents(void);

public:
  // CGUIWindow / Obtencion del tipo de ventana
  inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el tipo de interfaz
	return GUIManagerDefs::GUIW_MAININTERFAZ;
  }

public:
  // CGUIWindow / Activacion y desactivacion de la ventana de interfaz
  void Active(const bool bDoFadeIn);  
  void Deactive(void);
private:
  // Metodos de apoyo
  void SetTimeController(const byte ubHour, const bool bActive = true);
  void ActiveComponents(const bool bActive);
  void SetActiveSymptoms(void);
  void SetAttributesInfo(void);  
  void RemovePlayerInfo(void);
  void AttachPlayerInfo(void);

private:
  // Trabajo con los items en los slots principales  
  void SetMainEquipmentSlots(sEquipmentSlotsInfo::sSlotInfo& SlotInfo,
							 const RulesDefs::eIDEquipmentSlot& IdSlot,
							 const bool bAttach);

private:
  // Trabajo con el estado de pausa
  void SetPauseInEquipementSlot(const bool bPause,
								const RulesDefs::eIDEquipmentSlot& IdSlot);
  void SetPauseInComponents(const bool bPause);

public:
  // Establecimiento del modo de funcionamiento
  void SetInterfazMode(const WorldDefs::eWorldMode& Mode, 
					   const AreaDefs::EntHandle& hCriatureInTurn = 0);
  
public:
  // iCCriatureObserver / Operacion de notificacion
  void CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
							  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
							  const dword udParam = 0);

public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);
public:
  // iCGUIWindow / Manejo de los eventos de entrada  
  bool DispatchEvent(const InputDefs::sInputEvent& aInputEvent);  
  inline bool IsKeyPressed(const InputDefs::eInputEventCode& KeyPressed) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el flag de pulsacion
	return (m_KeyPressedSet.find(KeyPressed) != m_KeyPressedSet.end());
  }

private:
  // Trabajo con iconos de sintomas
  void SelectSymptomIcon(const RulesDefs::eIDSymptom& Symptom,
						 const bool bSelect);
private:
  // Metodos de apoyo para la gestion de la entrada global
  void InstallClient(void);
  void UnistallClient(void);  
  void OnMouseMoved(void);
  void OnMouseUnmove(void);
  void OnLeftMouseClick(void);
  void OnRightMouseClick(void);
  void OnKeyEscPressed(void); 
  void OnKeyWalkModePressed(void);
private:
  // Gestion del trabajo con el jugador
  void OnPlayerMovementSelectCell(void);
  void OnPlayerMovementDoMovement(void);  
  void OnEntityInteractSelectEntity(void);
  void OnEntityInteractDoInteract(void);  
  std::string GetAtackInfoSelect(CCriature* const pCriature);
  void ReleaseEntityInteractInfo(void);
private:
  // Gestion del scroll
  bool CheckForScroll(const word uwXPos, const word uwYPos);  

private:
  // Trabajo con los estados de interfaz
  void ActiveInterfazState(const CGUIWMainInterfaz::eInterfazState InitState,
						   const CGUIWMainInterfaz::eInterfazState InitPrevState);
  inline void SetInterfazState(const CGUIWMainInterfaz::eInterfazState State) {
	ASSERT(IsInitOk());	
	// Se establece el estado indicado, guardando el previo
	// Nota: NO usar referencias en el parametro
	m_InterfazInfo.PrevState = m_InterfazInfo.State;
	m_InterfazInfo.State = State;
  }
  GUIManagerDefs::eGUICursorType GetEntityInteractCursor(const bool bInteractPossible) const;

public:
  // Activacion / desactivacion de la entrada
  void SetInput(const bool bActive);

public:
  // iCGUIWindow / Notificacion de eventos
  void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent);
private:
  // Metodos de apoyo
  void OnSelectNotify(const GUIDefs::GUIIDComponent& IDComponent,
					  const bool bSelect);
  void OnLeftClickNotify(const GUIDefs::GUIIDComponent& IDComponent);

private:
  // Metodos relacionados con los slots de equipamiento de armas
  bool CanUseWeaponInSlot(const RulesDefs::eIDEquipmentSlot& Slot);
  void ClickOnEquipmentSlot(const RulesDefs::eIDEquipmentSlot& EquipSlot);  

public:
  // Trabajo con la consola
  inline void WriteToConsole(const std::string& szText, const bool bPlaySound) {
	ASSERT(Inherited::IsInitOk());
	// Se escribe en consola  
	m_ConsoleInfo.Text.InsertText(szText, bPlaySound);  
  }
  inline void ResetConsoleInfo(void) {
	ASSERT(Inherited::IsInitOk());
	// Quita informacion
	m_ConsoleInfo.Text.Reset();
  }

public:
  // Filtrado de carga de un area desde juego
  bool InGameAreaLoading(const word uwIDArea,
						 const AreaDefs::sTilePos& PlayerPos);

public:
  // Operacion de dibujado
  void Draw(void);

public:
  // iCCommandClient / Notificacion para la finalizacion de comandos (fade)
  void EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
					const dword udInstant,
					const dword udExtraParam);

public:
  // iCScriptClient / Operacion de notificacion, para cuando acabe un comando
  void ScriptNotify(const RulesDefs::eScriptEvents& ScriptEvent,
				    const ScriptClientDefs::eScriptNotify& Notify,					
					const dword udParams);

public:
  // Operacion de notificacion
  void EquipmentSlotNotify(const EquipmentSlotsDefs::eObserverNotifyType& NotifyType,
						   const RulesDefs::EquipmentSlot& Slot);
};

#endif // ~ ifndef CGUIWMainInterfaz