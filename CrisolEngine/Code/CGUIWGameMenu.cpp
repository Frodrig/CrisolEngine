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
// CGUIComponent.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIComponent.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWGameMenu.h"

#include "SYSEngine.h"
#include "iCGraphicSystem.h"
#include "iCLogger.h"
#include "iCInputManager.h"
#include "iCGUIManager.h"
#include "iCWorld.h"
#include "iCGameDataBase.h"
#include "iCVirtualMachine.h"
#include "GraphDefs.h"
#include "CCBTEngineParser.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se dejara reinicializar
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWGameMenu::Init(void)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) { return false; }
  
  #ifdef ENGINE_TRACE
     SYSEngine::GetLogger()->Write("CGUIWGameMenu::Init> Inicializando menu de juego.\n");
  #endif
    
  // Se inicializa clase base
  if (Inherited::Init()) { 
	// Se obtienen interfaces a otro subsistemas
	m_pGraphSys = SYSEngine::GetGraphicSystem();
	ASSERT(m_pGraphSys);
	m_pGUIManager = SYSEngine::GetGUIManager();
	ASSERT(m_pGUIManager);

	// Se cargan los datos inicializando componentes
	if (TLoad()) {
	  // Se inicializan resto de vbles de miembro
	  m_bIsDialogActive = false;

	  // Todo correcto
	  #ifdef ENGINE_TRACE
		SYSEngine::GetLogger()->Write("                   | Ok.\n");
	  #endif
	  return true;
	}
  }

  // Finaliza inicializacion con problemas  
  #ifdef ENGINE_TRACE
     SYSEngine::GetLogger()->Write("                   | Error.\n");
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
CGUIWGameMenu::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	// Finaliza componentes
	EndBackImage();
	EndMenuOptions();

	// Finaliza clase base
	Inherited::End();
  }
}
//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la carga y configuracion de componentes desde disco para 
//   ficheros de texto
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CGUIWGameMenu::TLoad(void)
{
  // Obtiene el parser
  iCGameDataBase* const pGDBase = SYSEngine::GetGameDataBase();
  ASSERT(pGDBase);
  CCBTEngineParser* const pParser = pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
													"[GameMenu]");

  // Procede a cargar e inicializar elementos
  if (!TLoadBackImage(pParser)) { return false; }
  if (!TLoadMenuOptions(pParser)) { return false; }
  if (!TLoadFadeInfo(pParser)) { return false; }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga e inicializa la imagen de fondo.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWGameMenu::TLoadBackImage(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Vbles
  CGUICBitmap::sGUICBitmapInitData CfgBitmap; // Cfg del bitmap

  // Prefijo
  pParser->SetVarPrefix("BackImage.");

  // Se cargan datos
  // Imagen de fondo
  CfgBitmap.BitmapInfo.szFileName = pParser->ReadString("FileName");
  CfgBitmap.Position = pParser->ReadPosition();
  CfgBitmap.BitmapInfo.Alpha = pParser->ReadAlpha();   
  
  // Se completan datos
  CfgBitmap.ID = CGUIWGameMenu::ID_BACKIMG;
  CfgBitmap.ubDrawPlane = 2;
  CfgBitmap.BitmapInfo.WidthTextureDim = GraphDefs::TEXTURE_DIM_256;
  CfgBitmap.BitmapInfo.HeightTextureDim = GraphDefs::TEXTURE_DIM_256;
  CfgBitmap.BitmapInfo.Bpp = GraphDefs::BPP_16;
  CfgBitmap.BitmapInfo.ABpp = GraphDefs::ALPHA_BPP_0;
  
  // Se inicializa componente
  if (!m_BackImgInfo.Background.Init(CfgBitmap)) { 
	return false; 
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga e inicializa los botones que apareceran en el menu principal. 
//   Seran los de SALVAR, CARGAR, CONFIGURAR, VOLVER y SALIR respectivamente.
//   La funcionalidad del boton SALIR tambien estara presente a traves de
//   la pulsacion de la tecla ESC.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha sido correcto true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWGameMenu::TLoadMenuOptions(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Nombres de los botones con las opciones
  const std::string szButtons[] = {
	"SaveButton.", "LoadButton.", "ReturnButton.", "ExitButton."
  };

  // Vbles
  CGUICButton::sGUICButtonInitData CfgButton; // Inicializacion botones

  // Establece datos de inicializacion comunes
  CfgButton.GUIEvents = GUIDefs::GUIC_SELECT |
					    GUIDefs::GUIC_UNSELECT |
					    GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  CfgButton.pGUIWindow = this;
  CfgButton.ubDrawPlane = 2;

  // Prefijo
  pParser->SetVarPrefix("Options.");
  
  // Procede a cargar e inicializar
  byte ubIt = 0;
  for (; ubIt < CGUIWGameMenu::sMenuOptionsInfo::MAX_MENU_BUTTONS; ++ubIt) {
	// Carga y completa datos
	TLoadButton(CfgButton, szButtons[ubIt], pParser);
	CfgButton.ID = GUIDefs::GUIIDComponent(ubIt + 1);
	// Se inicializa
	if (!m_MenuOptions.MenuButtons[ubIt].Init(CfgButton)) { 	  
	  return false; 
	}
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga datos asociados al efecto de fade que se producira cuando se
//   pulse al boton de salir de juego.
// Parametros:
// - pParser. Parser a utilizar
// Devuelve:
// - Si todo ha sido correcto true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWGameMenu::TLoadFadeInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Prefijo
  pParser->SetVarPrefix("Fade.");

  // Carga informacion
  m_FadeInfo.RGBColor = pParser->ReadRGBColor("Out.");
  m_FadeInfo.uwSpeed = pParser->ReadNumeric("Out.Speed");

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la operacion de la carga desde disco de informacion relativa a
//   un boton, dejando la informacion depositada en la estructura CfgData.
// Parametros:
// - CfgData. Estructura donde dejar la informacion cargada.
// - szButtonName. Nombre del boton.
// - pParser. Parser a utilizar. El parser debera de estar situado en la
//   seccion correcta y con el prefijo correcto.
// Devuelve:
// Notas:
// - Para todos los botones, se leera en este metodo la posicion y la
//   plantilla de animacion del mismo
//////////////////////////////////////////////////////////////////////////////
void
CGUIWGameMenu::TLoadButton(CGUICButton::sGUICButtonInitData& CfgData,
						   const std::string& szButtonName,
						   CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Procede a cargar la informacion
  CfgData.szATButton = pParser->ReadString(szButtonName + "AnimTemplate");
  CfgData.Position = pParser->ReadPosition(szButtonName);
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza imagen de fondo
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameMenu::EndBackImage(void)
{
  // Finaliza
  m_BackImgInfo.Background.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza botones con las opciones
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameMenu::EndMenuOptions(void)
{
  // Finaliza
  byte ubIt = 0;
  for (; ubIt < CGUIWGameMenu::sMenuOptionsInfo::MAX_MENU_BUTTONS; ++ubIt) {
	m_MenuOptions.MenuButtons[ubIt].End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece / bloquea la entrada
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameMenu::SetInput(const bool bInput)
{
  // SOLO si instanca inicializada
  ASSERT(IsInitOk());

  // Imagen
  m_BackImgInfo.Background.SetInput(bInput);

  // Botones
  byte ubIt = 0;
  for (; ubIt < CGUIWGameMenu::sMenuOptionsInfo::MAX_MENU_BUTTONS; ++ubIt) {
    m_MenuOptions.MenuButtons[ubIt].SetInput(bInput);
  }  
  
  // Propaga
  Inherited::SetInput(bInput);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el menu realizando llamadas a los metodos que actuan sobre los
//   componentes que forman parte del menu.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameMenu::Active(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se activa si procede
  if (!Inherited::IsActive()) {
	// Activacion de componentes  
	ActiveComponents(true); 

	// Establece cursor de interaccion con interfaces
	m_pGUIManager->SetGUICursor(GUIManagerDefs::WINDOWINTERFAZ_CURSOR);

	// NO permite la ejecucion de scripts
	SYSEngine::GetVirtualMachine()->SetScriptExecution(false);  
  
	// Se propaga a la clase base
	Inherited::Active();
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva el menu llamando a los metodos que actuan sobre los componentes
//   que forman parte del mismo.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameMenu::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Desactiva si procede
  if (Inherited::IsActive()) {
	// Desactiva componentes
	ActiveComponents(false);

	// Permite la ejecucion de scripts
	SYSEngine::GetVirtualMachine()->SetScriptExecution(true);
  
	// Se propaga a clase base
	Inherited::Deactive();	
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa / desactiva componentes
// Parametros:
// - bActive. Flag de activacion.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameMenu::ActiveComponents(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Procede a activar la imagen de fondo
  m_BackImgInfo.Background.SetActive(bActive);

  // Procede a activar todos los botones asociados al menu
  byte ubIt = 0;
  for (; ubIt < CGUIWGameMenu::sMenuOptionsInfo::MAX_MENU_BUTTONS; ++ubIt) {
	m_MenuOptions.MenuButtons[ubIt].SetActive(bActive);
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Responde a las notificaciones de los componentes embebidos en el 
//   interfaz.
// Parametros:
// - GUICEvent. Informacion sobre el componente que ha realizado la notificacion
//   al interfaz.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameMenu::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
{
  // SOLO si instancia inicialiada
  ASSERT(Inherited::IsInitOk());
  
  // Se comprueba el evento recibido
  switch (GUICEvent.Event) {
	case GUIDefs::GUIC_SELECT: {	
	  OnButtonSelect(GUICEvent.IDComponent, true);
	} break;

  	case GUIDefs::GUIC_UNSELECT: {	
	  OnButtonSelect(GUICEvent.IDComponent, false);
	} break;
	
	case GUIDefs::GUIC_BUTTONLEFT_PRESSED: {	  	  
	  OnButtonLeftClick(GUICEvent.IDComponent);
	} break;	  
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la seleccion / deseleccion de un boton de menu.
// Parametros:
// - IDButton. ID del boton que se ha seleccionado.
// - bSelect. Flag de seleccion.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameMenu::OnButtonSelect(const GUIDefs::GUIIDComponent& IDButton,
							  const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Selecciona el boton
  m_MenuOptions.MenuButtons[IDButton - 1].Select(bSelect);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Contiene la logica asociada a la pulsacion de un boton del menu de juego.
// Parametros:
// - IDButton. ID del boton que se ha seleccionado.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameMenu::OnButtonLeftClick(const GUIDefs::GUIIDComponent& IDButton)
{ 
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Obtiene la posicion donde localizar el boton
  const byte ubButtonPos = IDButton - 1;

  // Realiza la accion segun sea el boton presionado
  switch(IDButton) {
	case CGUIWGameMenu::ID_SAVEGAME: {
	  // Guarda partidas
	  // ¿Se puede guardar?
	  if (SYSEngine::GetWorld()->CanSaveGame()) {
		// Si, se guarda
		m_pGUIManager->SetSaverSavedFileWindow();	   	  
	  } else {
		// No, se bloquea entrada y se activa cuadro de dialogo de aviso
		m_bIsDialogActive = true;
		OnButtonSelect(IDButton, false);
		m_pGUIManager->ActiveAdviceDialog(SYSEngine::GetGameDataBase()->GetStaticText(GameDataBaseDefs::ST_GENERAL_NO_SAVEGAME_PERMISSION), 
										  this);  		
	  }
	} break;

	case CGUIWGameMenu::ID_LOADGAME: {
	  // Carga partidas
	  m_pGUIManager->SetLoaderSavedFileWindow();
	} break;
	
	case CGUIWGameMenu::ID_RESUMEGAME: {
	  // Devuelve el control al jugador, activando el MainInterfaz
	  m_pGUIManager->SetMainInterfazWindow();	  

	  // Se quita la pausa del universo de juego
	  ASSERT(SYSEngine::GetWorld()->IsInPause());
	  SYSEngine::GetWorld()->SetPause(false);
	} break;

	case CGUIWGameMenu::ID_EXITGAME: {	 
	  // Se pregunta por confirmacion y se levanta el flag de cuadro de dialogo	  
	  m_bIsDialogActive = true;
	  OnButtonSelect(IDButton, false);
	  m_pGUIManager->ActiveQuestionDialog(SYSEngine::GetGameDataBase()->GetStaticText(GameDataBaseDefs::ST_GENERAL_EXIT_GAME_CONFIRM),
										  false, 
										  this);
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe la notificacion del cuadro de dialogo que se lanza tras pulsar 
//   salir de juego. En caso de que el parametro sea 1, se verificara el deseo
//   de salir, en caso contrario sera 0 y se deseara permanecer.
// Parametros:
// - IDGUIWindow. Identificador de la ventana de interfaz sobre la que se
//   recibe notifiacion (cuadro de dialogo)
// - udParams. Parametros
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameMenu::GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
							   const dword udParams)
{
  // SOLO si intancia incializada
  ASSERT(Inherited::IsInitOk());

  // Se comprueba de donde viene la notificacion
  switch (IDGUIWindow) {
	case GUIManagerDefs::GUIW_QUESTIONDIALOG: {
	  // Cuadro de dialogo de cuestion
	  // Se comprueba opcion escogida
	  switch(udParams) {
		case 1: {
		  // Pregunta aceptada / se desea salir
		  // Se bloquea entrada y se oculta el cursor
		  SetInput(false);
		  m_pGUIManager->SetGUICursor(GUIManagerDefs::NO_CURSOR);

		  // Se ejecuta el fade, al terminar se abandonara el juego
		  Inherited::DoFadeOut(CGUIWGameMenu::ID_ENDEXITFADE, 
						       m_FadeInfo.RGBColor,
							   m_FadeInfo.uwSpeed);  	  
		} break;		  
		
		case 2: {
		  // No se desea salir
		  // Se baja el flag de cuadro de dialogo activo y refrescan opciones
		  m_bIsDialogActive = false;
		  RefreshInput();
		} break;
	  }; // ~ switch		  
	} break;

	case GUIManagerDefs::GUIW_ADVICEDIALOG: {
	  // Cuadro de dialogo de aviso
	  // Se baja el flag de cuadro de dialogo activo y refresca opciones
	  m_bIsDialogActive = false;
	  RefreshInput();
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Actualiza la entrada sobre los componentes de opciones
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameMenu::RefreshInput(void)
{
  // SOLO si instancia inicializada y activa
  ASSERT(Inherited::IsInitOk());
  ASSERT(Inherited::IsActive());

  // Se refresca la entrada en las opciones
  byte ubIt = 0;
  for (; ubIt < CGUIWGameMenu::sMenuOptionsInfo::MAX_MENU_BUTTONS; ++ubIt) {
	m_MenuOptions.MenuButtons[ubIt].RefreshInput();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo que recibira la notificacion de la finalizacion de un comando. En
//   este caso en concreto, interesara la finalizacion del comando de
//   fade que se produce cuando se pulsa salir de juego.
// Parametros:
// - IDCommand. Id. del comando.
// - udInstant. Intante en que se produce.
// - udExtraParam. Parametro extra.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameMenu::EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
						    const dword udInstant,
							const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(IDCommand);
  ASSERT(udInstant);

  // Comprueba el identificador del comando
  switch (IDCommand) {
	case CGUIWGameMenu::ID_ENDEXITFADE: {
	  // Se cambia de estado a menu principal	  
	  SYSEngine::SetMainMenuState();	  
	} break;

	default: {
	  ASSERT(false);
	} break;
  }; // ~ switch
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
CGUIWGameMenu::InstallClient(void)
{
  // Se instala como cliente
  SYSEngine::GetInputManager()->SetInputEvent(InputDefs::IE_KEY_ESC, 
											  this, 
											  InputDefs::BUTTON_PRESSED_DOWN);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de saber si se ha pulsado ESC y se debe de abandonar el menu
// Parametros:
// - aInputEvent. Informacion sobre el evento de entrada.
// Devuelve:
// - Flag para indicar si el evento debe de compartirse o no.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWGameMenu::DispatchEvent(const InputDefs::sInputEvent& aInputEvent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Vbles
  bool bShareEvent; // ¿Hay que compartir el evento?

  // Inicializaciones
  bShareEvent = InputDefs::NO_SHARE_INPUT_EVENT;  
  
  // Evalua el evento producido 
  switch(aInputEvent.EventType) {
	case InputDefs::BUTTON_EVENT: {
	  // Pulsacion de tecla
	  switch(aInputEvent.EventButton.Code) {
		case InputDefs::IE_KEY_ESC: {
		  // Se simula la pulsacion del boton de salir
		  OnButtonSelect(CGUIWGameMenu::ID_RESUMEGAME, true);
		  OnButtonLeftClick(CGUIWGameMenu::ID_RESUMEGAME);
		} break;
	  }; // ~ switch
	} break;	  
  }; // ~ switch

  // No se ha capturado ningun evento
  return bShareEvent;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desinstala los eventos de entrada.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameMenu::UnistallClient(void)
{ 
  // Se quitan eventos de entrada  
  SYSEngine::GetInputManager()->UnsetInputEvent(InputDefs::IE_KEY_ESC, 
												this, 
												InputDefs::BUTTON_PRESSED_DOWN);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado de los componentes que forman parte del menu.
// Parametros:
// Devuelve:
// Notas:
// - En caso de que el cuadro de dialogo que pregunta sobre si de verdad se
//   de sea abandonar se halla activo (m_bIsQDialogActive), no se dibujara
//   la ventana de interfaz.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameMenu::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Dibuja si procede
  if (Inherited::IsActive() && 
	  !m_bIsDialogActive) {
	// Imagen de fondo
	m_BackImgInfo.Background.Draw();    

	// Botones
	byte ubIt = 0;
	for (; ubIt < CGUIWGameMenu::sMenuOptionsInfo::MAX_MENU_BUTTONS; ++ubIt) {
	  m_MenuOptions.MenuButtons[ubIt].Draw();
	}  
  }  
}
