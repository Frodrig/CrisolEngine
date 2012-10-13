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
// CGUIWMainMenu.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWMainMenu.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWMainMenu.h"

#include "SYSEngine.h"
#include "iCLogger.h"
#include "iCInputManager.h"
#include "iCGUIManager.h"
#include "iCAudioSystem.h"
#include "iCGameDataBase.h"
#include "GraphDefs.h"
#include "CCBTEngineParser.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false
// Notas:
// - No se dejara reinicializar
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWMainMenu::Init(void)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) { 
	return false; 
  }
  
  #ifdef ENGINE_TRACE
     SYSEngine::GetLogger()->Write("CGUIWMainMenu::Init> Inicializando interfaz de menu principal.\n");
  #endif
    
  // Se inicializa clase base
  if (Inherited::Init()) { 
	// Se toman interfaces a otros subsistemas
	m_pGUIManager = SYSEngine::GetGUIManager();
	ASSERT(m_pGUIManager);

	// Se procede a cargar componentes
	if (TLoad()) {
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
CGUIWMainMenu::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	#ifdef ENGINE_TRACE
     SYSEngine::GetLogger()->Write("CGUIWMainMenu::End> Finalizando interfaz de menu principal.\n");
	#endif

	// Desactiva
	Deactive();

	// Se finalizan componentes
	EndComponents();
	 
	// Finaliza clase base
	Inherited::End();
	#ifdef ENGINE_TRACE
      SYSEngine::GetLogger()->Write("                   | Ok.\n");
	#endif
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la carga de datos
// Parametros:
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool
CGUIWMainMenu::TLoad(void)
{
  // Se obtiene el parser
  CCBTEngineParser* const pParser = SYSEngine::GetGameDataBase()->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
																			   "[MainMenu]");
  ASSERT(pParser);

  // Procede a cargar
  // Imagen de fondo
  if (!TLoadBackImgInfo(pParser)) {
	return false;
  }

  // Botones de opciones
  if (!TLoadOptionButton(pParser)) {
	return false;
  }
  
  // Posible texto informativo asociado
  if (!TLoadVersionInfo(pParser)) {
	return false;
  }
  
  // Musica de fondo
  if (!TLoadBackMIDIMusic(pParser)) {
	return false;
  }
  
  // Datos asociados a los fades
  if (!TLoadFadeValues(pParser)) {
	return false;
  }

  // Todo correcto
  return true;	
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene los datos de configuracion asociados a la imagen de fondo
//   y llama al metodo de inicializacion de dicho componente a traves
//   de la instancia al interfaz.
// Parametros:
// - pParser-> Parser a utilizar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CGUIWMainMenu::TLoadBackImgInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Vbles
  CGUICBitmap::sGUICBitmapInitData BackImgData; // Datos de configuracion
  
  // Obtiene datos
  // Nombre del fichero
  pParser->SetVarPrefix("BackImage.");
  BackImgData.BitmapInfo.szFileName = pParser->ReadString("FileName");
  
  // Se completan datos
  BackImgData.ID = CGUIWMainMenu::ID_BACKIMG;
  BackImgData.ubDrawPlane = 1;
  BackImgData.BitmapInfo.WidthTextureDim = GraphDefs::TEXTURE_DIM_32;
  BackImgData.BitmapInfo.HeightTextureDim = GraphDefs::TEXTURE_DIM_32;
  BackImgData.BitmapInfo.Bpp = GraphDefs::BPP_16;
  BackImgData.BitmapInfo.ABpp = GraphDefs::ALPHA_BPP_1;  
  
  // Se realiza la inicializacion
  if (!m_BackImg.BackImg.Init(BackImgData)) {
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
	  m_BackImg.BackImg.AddAnim(szAnim, 
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

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga los datos asociados a un boton de opciones y realiza su 
//   inicializacion.
// Parametros:
// - pParser-> Parser a utilizar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CGUIWMainMenu::TLoadOptionButton(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Nombres de las opciones posibles
  const std::string szOptions[] = {
	"StartNewGameButton.", "LoadGameButton.", "CreditsButton.", "ExitButton."
  };

  // Se procede a cargar opciones
  pParser->SetVarPrefix("Options.");
  byte ubIt = 0;
  for (; ubIt < CGUIWMainMenu::sOptionButtonsInfo::MAX_OPTIONS; ++ubIt) {
	// Se cargan datos
    CGUICButton::sGUICButtonInitData ButtonData; // Datos de configuracion	
	ButtonData.szATButton = pParser->ReadString(szOptions[ubIt] + "AnimTemplate");  
	ButtonData.Position = pParser->ReadPosition(szOptions[ubIt]);
	
	// Se completan datos
	ButtonData.ID = CGUIWMainMenu::ID_STARTNEWGAME + ubIt;
	ButtonData.ubDrawPlane = 1;
	ButtonData.pGUIWindow = this;
	ButtonData.GUIEvents = GUIDefs::GUIC_SELECT |
						   GUIDefs::GUIC_UNSELECT |									
						   GUIDefs::GUIC_BUTTONLEFT_PRESSED;
	// Se inicializa
	if (!m_OptionsInfo.Options[ubIt].Init(ButtonData)) {
	  return false;
	}
  }

  // Se carga la informacion asociada al perfil de presentacion para la
  // opcion de ver creditos    
  m_szCreditPresentProfile = pParser->ReadString(szOptions[2] + "PresentationProfile");

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga los datos asociados al texto informativo y realiza su inicializacion.
// Parametros:
// - pParser-> Parser a utilizar.
// Devuelve:
// Notas:
// - El texto informativo no sera obligatorio.
///////////////////////////////////////////////////////////////////////////////
bool
CGUIWMainMenu::TLoadVersionInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Vbles
  
  // Se establece prefijo
  pParser->SetVarPrefix("VersionInfo.");

  // Texto asociado  
  const std::string szVersionInfo(SYSEngine::GetGameDataBase()->GetStaticText(GameDataBaseDefs::ST_INTERFAZ_MAINMENU_VERSIONINFO));
  if (!szVersionInfo.empty()) {
	// Hay informacion, se carga resto de datos
	CGUICLineText::sGUICLineTextInitData CfgData; 
	CfgData.szLine = szVersionInfo;
	CfgData.Position = pParser->ReadPosition();  
	CfgData.RGBUnselectColor = pParser->ReadRGBColor();
	CfgData.RGBSelectColor = CfgData.RGBUnselectColor;
	// Se completan datos
	CfgData.szFont = "Arial";	
	CfgData.ID = CGUIWMainMenu::ID_INFOTEXT;
	CfgData.ubDrawPlane = 1;
  
	// Se realiza la inicializacion y se retorna resultado
	m_VersionInfo.VersionInfo.Init(CfgData);  
	return m_VersionInfo.VersionInfo.IsInitOk();
  }

  // Todo correcto
  return true;
  
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga los datos asociados a la musica de fondo. 
// Parametros:
// - pParser-> Parser a utilizar.
// Devuelve:
// Notas:
// - La musica de fondo no sera obligatoria
///////////////////////////////////////////////////////////////////////////////
bool
CGUIWMainMenu::TLoadBackMIDIMusic(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);
  
  // Prefijo
  pParser->SetVarPrefix("MIDIMusic.");

  // Nombre del fichero MIDI
  m_MusicInfo.szFileName = pParser->ReadString("FileName", false);
  if (!pParser->WasLastParseOk()) { 
	return true; 
  }  
  
  // Flag de reproduccion ciclica  
  m_MusicInfo.Mode = pParser->ReadFlag("LoopFlag", false) ? 
					 AudioDefs::MIDI_PLAY_LOOP : 
					 AudioDefs::MIDI_PLAY_NORMAL;  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga los valores asociados a los fades de la ventana cuando esta
//   se active y Deactive. No sera obligatorio especificar valores. En caso
//   de no hacerlo se tomaran valores de 0. En lo referente a la velocidad,
//   se tomara un valor de 256 si no esta presente.
// Parametros:
// - pParser-> Parser a utilizar.
// Devuelve:
// Notas:
// - La musica de fondo no sera obligatoria
///////////////////////////////////////////////////////////////////////////////
bool
CGUIWMainMenu::TLoadFadeValues(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);
  
  // Se establece prefijo
  pParser->SetVarPrefix("Fade.");

  // Fade inicial (de ese color a la imagen)
  m_FadeInfo.RGBStartFade = pParser->ReadRGBColor("Start.");
  
  // Fade in
  m_FadeInfo.uwStartFadeSpeed = pParser->ReadNumeric("In.Speed");

  // Fade out
  m_FadeInfo.RGBEndFade = pParser->ReadRGBColor("Out.");
  m_FadeInfo.uwEndFadeSpeed = pParser->ReadNumeric("Out.Speed");

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza componentes
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainMenu::EndComponents(void)
{
  // Finaliza componentes
  // Imagen de fondo
  m_BackImg.BackImg.End();

  // Opciones
  byte ubIt = 0;
  for(; ubIt < CGUIWMainMenu::sOptionButtonsInfo::MAX_OPTIONS; ++ubIt) {
	m_OptionsInfo.Options[ubIt].End();
  }

  // Version
  m_VersionInfo.VersionInfo.End();  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el interfaz y a todos sus componentes.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainMenu::Active(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se activa si procede
  if (!Inherited::IsActive()) {
	// Se bloquea el input y se esconden cursores
	// Nota: Se bloquea ANTES de activar para evitar cualquier evento
	SetInput(false);
	m_pGUIManager->SetGUICursor(GUIManagerDefs::NO_CURSOR);  	

	// Se activan los componentes
	ActiveComponents(true);
	
	// Realiza fade in, poniendo antes el FadeOut inmediato
	Inherited::DoFadeOut(CGUIWMainMenu::ID_ENDSETFADEOUT,
						 m_FadeInfo.RGBStartFade,
						 0);	
	Inherited::DoFadeIn(CGUIWMainMenu::ID_ENDSTARTFADE,
						m_FadeInfo.uwStartFadeSpeed);

	// Se propaga activacion
	Inherited::Active();	
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva los componentes.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainMenu::Deactive(void)
{   
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se desactiva
  if (Inherited::IsActive()) {
	// Desactiva componentes
	ActiveComponents(false);

	// Se propaga desactivacion
	Inherited::Deactive();
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa / desactiva componentes
// Parametros:
// - bActive. Flag de activacion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainMenu::ActiveComponents(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Actual
  // Imagen de fondo
  m_BackImg.BackImg.SetActive(bActive);

  // Botones de opciones
  byte ubIt = 0;
  for(; ubIt < CGUIWMainMenu::sOptionButtonsInfo::MAX_OPTIONS; ++ubIt) {
	m_OptionsInfo.Options[ubIt].SetActive(bActive);
  }

  // Version
  if (m_VersionInfo.VersionInfo.IsInitOk()) {
	m_VersionInfo.VersionInfo.SetActive(bActive);
  }

  // Musica de fondo
  if (m_MusicInfo.szFileName.size()) {
	if (bActive) {
	  SYSEngine::GetAudioSystem()->PlayMIDIMusic(m_MusicInfo.szFileName, 
											     m_MusicInfo.Mode);
	} else {
	  SYSEngine::GetAudioSystem()->StopMIDIMusic();
	}
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe la notificacion de un evento asociado a un componente y lo 
//   distribuye al metodo adecuado para su tratamiento.
// Parametros:
// - GUICEvent. Evento recibido
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainMenu::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
{
  // SOLO si instancia inicialiada
  ASSERT(Inherited::IsInitOk());
  
  // Se comprueba el evento recibido
  switch (GUICEvent.Event) {
	case GUIDefs::GUIC_SELECT: {	
	  OnSetSelect(true, GUICEvent.IDComponent);
	} break;

  	case GUIDefs::GUIC_UNSELECT: {	
	  OnSetSelect(false, GUICEvent.IDComponent);
	} break;
	
	case GUIDefs::GUIC_BUTTONLEFT_PRESSED: {	  	  
	  OnLeftClick(GUICEvent.IDComponent);
	} break;	  
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la seleccion / deseleccion de componentes.
// Parametros:
// - bSelect. Flag de seleccion / deseleccion
// - IDComponent. Identificador del componente.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainMenu::OnSetSelect(const bool bSelect,
						   const GUIDefs::GUIIDComponent& IDComponent)
{
  // SOLO si instancia inicialiada
  ASSERT(Inherited::IsInitOk());

  // Se actual en el componente que proceda
  switch(IDComponent) {	
	case CGUIWMainMenu::ID_STARTNEWGAME:
	case CGUIWMainMenu::ID_CONTINUEGAME: 
	case CGUIWMainMenu::ID_CREDITS: 
	case CGUIWMainMenu::ID_EXIT: {	  
	  // Se selecciona opcion que proceda	  
	  m_OptionsInfo.Options[GetIDOptionPos(IDComponent)].Select(bSelect);	  
	} break;
  }; // ~ switch  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la pulsacion del boton izquierdo sobre un componente.
//   Antes de realizar la logica, se realizara un FadeOut y se bloqueara la
//   entrada. Una vez que se termine el fade, se realizara la logica asociada
//   a la accion escogida.
// Parametros:
// - IDComponent. Identificador del componente.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainMenu::OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent)
{  	
  // SOLO si instancia inicialiada
  ASSERT(Inherited::IsInitOk());

  // Vbles
  CommandDefs::IDCommand IDCmd; // Identificador del comando despues de fade

  // Se actual en el componente que proceda
  switch(IDComponent) {		
	case CGUIWMainMenu::ID_CONTINUEGAME: {
	  // Opcion de cargar una partida para continuarla
	  IDCmd = CGUIWMainMenu::ID_ENDCONTINUEGAMEFADE;	  
  	} break;	
	
	case CGUIWMainMenu::ID_STARTNEWGAME: {
	  // Opcion de comenzar nuevo juego	  
	  IDCmd = CGUIWMainMenu::ID_ENDSTARTNEWGAMEFADE;
	} break;

	case CGUIWMainMenu::ID_CREDITS: {
	  // Opcion de ver creditos
	  IDCmd = CGUIWMainMenu::ID_ENDCREDITSFADE;	  	  
	} break;	   

	case CGUIWMainMenu::ID_EXIT: {	  
	  // Opcion de salir
	  IDCmd = CGUIWMainMenu::ID_ENDEXITFADE;
	} break;	  
  }; // ~ switch  

  // Se inicia fade in, se bloquea entrada y se esconde cursores
  SetInput(false);    
  m_pGUIManager->SetGUICursor(GUIManagerDefs::NO_CURSOR);  
  Inherited::DoFadeOut(IDCmd, m_FadeInfo.RGBEndFade, m_FadeInfo.uwEndFadeSpeed);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Bloquea / desbloquea la entrada en los componentes y la propia ventana.
// Parametros:
// - bActive. Flag indicando la accion a realizar
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainMenu::SetInput(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se actua sobre los botones de opciones
  byte ubIt = 0;
  for (; ubIt < CGUIWMainMenu::sOptionButtonsInfo::MAX_OPTIONS; ++ubIt) {
	m_OptionsInfo.Options[ubIt].SetInput(bActive);
  }

  // Se propaga la orden
  Inherited::SetInput(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Despacha los eventos que se sucedan sobre la ventana.
// Parametros:
// - InputEvent. Evento.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWMainMenu::DispatchEvent(const InputDefs::sInputEvent& InputEvent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Evalua el evento producido 
  switch(InputEvent.EventType) {
	case InputDefs::BUTTON_EVENT: {
	  // Pulsacion de tecla
	  switch(InputEvent.EventButton.Code) {
		case InputDefs::IE_KEY_ESC: {
		  // Se simula la pulsacion del boton de salir
		  // ¿NO esta seleccionada ya la opcion de salir?
		  if (!m_OptionsInfo.Options[GetIDOptionPos(CGUIWMainMenu::ID_EXIT)].IsSelect()) {
			// Se deselecciona cualquier otra posible opcion
			OnSetSelect(false, CGUIWMainMenu::ID_STARTNEWGAME);
			OnSetSelect(false, CGUIWMainMenu::ID_CONTINUEGAME);
			OnSetSelect(false, CGUIWMainMenu::ID_CREDITS);

			// Se simula la seleccion de la opcion de salida
			OnSetSelect(true, CGUIWMainMenu::ID_EXIT);
		  }

		  // Se simula el cliqueo cliqueo de la opcion de salida		  
		  OnLeftClick(CGUIWMainMenu::ID_EXIT);
		  return InputDefs::NO_SHARE_INPUT_EVENT;
		} break;
	  }; // ~ switch
	} break;	  
  }; // ~ switch

  // No se capturo evento / Se comparte
  return InputDefs::SHARE_INPUT_EVENT;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se instala como cliente para recibir eventos sobre el global de la
//   ventana de interfaz. En concreto:
//   * Pulsacion ESC -> Simulara pulsar sobre el boton de cancelar.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainMenu::InstallClient(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se instala como cliente
  SYSEngine::GetInputManager()->SetInputEvent(InputDefs::IE_KEY_ESC, 
											  this, 
											  InputDefs::BUTTON_PRESSED_DOWN);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se desinstala como cliente para eventos de entrada.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainMenu::UnistallClient(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se instala como cliente
  SYSEngine::GetInputManager()->UnsetInputEvent(InputDefs::IE_KEY_ESC, 
												this, 
											    InputDefs::BUTTON_PRESSED_DOWN);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado de todos los componentes.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainMenu::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Imagen de fondo
  m_BackImg.BackImg.Draw();

  // Procede a activar botones de opciones
  byte ubIt = 0;
  for(; ubIt < CGUIWMainMenu::sOptionButtonsInfo::MAX_OPTIONS; ++ubIt) {	
	m_OptionsInfo.Options[ubIt].Draw();
  }

  // Texto informativo (si procede)
  if (m_VersionInfo.VersionInfo.IsInitOk()) {
	m_VersionInfo.VersionInfo.Draw();
  }  
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion de la finalizacion de un comando.
// - Se utilizara para conocer cuando ha finalizado el fade de finalizacion y
//   asi realizar la accion que sea pertinente. Segun la accion que se pulsara,
//   asi se asignara el comando de fade.
// Parametros:
// - IDCommand. ID del comando.
// - udInstant. Instante de finalizacion.
// - udExtraParam. Parametro extra.
// Devuelve:
// - Si todo bien true. En caso contrario false.
// Notas:
// - Al finalizar el fade asociado a una opcion, se deseleccionara la misma
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainMenu::EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
							const dword udInstant,							
							const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se comprueba codigo de fade recibido
  switch(IDCommand) {
	case CGUIWMainMenu::ID_ENDSTARTFADE: {
	  // Fin de fade de inicio
	  // Se establece cursor de gui y desbloquean componentes  
	  m_pGUIManager->SetGUICursor(GUIManagerDefs::WINDOWINTERFAZ_CURSOR);		  
	  SetInput(true);	  
	} break;

	case CGUIWMainMenu::ID_ENDSTARTNEWGAMEFADE: {
	  // Fin de fade para comenzar nuevo juego
	  // Se cambia de estado
	  OnSetSelect(false, CGUIWMainMenu::ID_STARTNEWGAME);	  
	  SYSEngine::SetCreateCharacterState();
	} break;

	case CGUIWMainMenu::ID_ENDCONTINUEGAMEFADE: {
	  // Fin de fade para continuar con una partida
	  OnSetSelect(false, CGUIWMainMenu::ID_CONTINUEGAME);	  
	  m_pGUIManager->SetLoaderSavedFileWindow();
	} break;

	case CGUIWMainMenu::ID_ENDCREDITSFADE: {
	  // Fin de fade para ver creditos
	  // Se establece la ventana de presentacion
	  OnSetSelect(false, CGUIWMainMenu::ID_CREDITS);	  
	  m_pGUIManager->SetPresentationWindow(m_szCreditPresentProfile);
	} break;

	case CGUIWMainMenu::ID_ENDEXITFADE: {
	  // Se ordena el abandono del motor
	  OnSetSelect(false, CGUIWMainMenu::ID_EXIT);
	  SYSEngine::SetExit(true);
	} break;
  }; // ~ switch
}
