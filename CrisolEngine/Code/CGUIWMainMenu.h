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
// CGUIWMainMenu.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUIWMainMenu
//
// Descripcion:
// - Esta es la ventana de interfaz primaria, que es la que sale nada mas que
//   se arranca el juego, ofreciendo todas las posibles opciones.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWMAINMENU_H_
#define _CGUIWMAINMENU_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabecera
#ifndef _CGUIWINDOW_H_
#include "CGUIWindow.h"
#endif
#ifndef _CGUICANIMBITMAP_H_
#include "CGUICAnimBitmap.h"
#endif
#ifndef _CGUICBUTTON_H_
#include "CGUICButton.h"
#endif
#ifndef _CGUICLINETEXT_H_
#include "CGUICLineText.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif
#ifndef _AUDIODEFS_H_
#include "AudioDefs.h"
#endif

// Definicion de clases / estructuras / espacios de nombres
class CCBTEngineParser;
struct iCGUIManager;

// clase CGUIWMainMenu
class CGUIWMainMenu: public CGUIWindow
{  
private:
  // Tipos
  typedef CGUIWindow Inherited; // Acceso a la clase base

private:  
  // Enumerados
  enum {
	// Identificadores para fade	
	ID_ENDSTARTFADE = 0x01, // Fin del fade de inicio
	ID_ENDSETFADEOUT,       // Fin del establecimiento del color FadeOut
	ID_ENDSTARTNEWGAMEFADE, // Fin de fade asociado al inicio de nueva partida
	ID_ENDCONTINUEGAMEFADE, // Fin de fade asociado a cargar una partida
	ID_ENDCREDITSFADE,      // Fin de fade asociado a ver creditos
	ID_ENDEXITFADE,         // Fin de fade asociado al boton salir
  };

  enum {	
	// Identificadores para componentes
	// Imagen de fondo
	ID_BACKIMG = 1,
	// Botones de opciones
	ID_STARTNEWGAME,
	ID_CONTINUEGAME,
	ID_CREDITS,
	ID_EXIT,
	// Texto informativo	
	ID_INFOTEXT,
  };

private:
  // Estructuras
  struct sBackImgInfo {
	// Info asociada a la imagen de fondo
	CGUICAnimBitmap BackImg; // Imagen de fondo
  };

  struct sOptionButtonsInfo { 
	// Info asociada a los botones de opciones
	// Enumerados
	enum { 	  
	  MAX_OPTIONS = 4
	};
	// Info asociada a los botones de opciones
	CGUICButton Options[MAX_OPTIONS]; // Num. maximo de opciones	
  };
  
  struct sVersionInfo {
	// Info asociada al texto informativo
	CGUICLineText VersionInfo; // Texto informativo
  };

  struct sBackMIDIMusicInfo {
	// Info asociada a la musica de fondo
	std::string              szFileName; // Nombre del fichero
	AudioDefs::eMIDIPlayMode Mode;       // Modo de reproduccion
  };

  struct sFadeInfo {
	// Valores asociados a los fades
	GraphDefs::sRGBColor RGBStartFade;     // Fade inicio
	GraphDefs::sRGBColor RGBEndFade;       // Fade final
	word                 uwStartFadeSpeed; // Velocidad del fade de inicio
	word                 uwEndFadeSpeed;   // Velocidad del fade de fin
  };

private:
  // Interfaces a otro subsistemas
  iCGUIManager* m_pGUIManager; // Interfaz a manager de GUIs

  // Info asociada a los componentes
  sBackImgInfo       m_BackImg;                // Imagen de fondo
  sOptionButtonsInfo m_OptionsInfo;            // Botones de opciones
  sVersionInfo       m_VersionInfo;            // Texto informativo 
  sBackMIDIMusicInfo m_MusicInfo;			   // Info asociada a la musica MIDI de fondo
  sFadeInfo          m_FadeInfo;               // Info asociada al fade
  std::string        m_szCreditPresentProfile; // Perfil de presentacion para creditos
  
public:
  // Constructor / destructor  
  CGUIWMainMenu(void): m_pGUIManager(NULL) { }				
  ~CGUIWMainMenu(void) { End(); }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(void);
  void End(void);
private:
  // Metodos de apoyo
  bool TLoad(void);
  bool TLoadBackImgInfo(CCBTEngineParser* const pParser);
  bool TLoadOptionButton(CCBTEngineParser* const pParser);
  bool TLoadVersionInfo(CCBTEngineParser* const pParser);
  bool TLoadBackMIDIMusic(CCBTEngineParser* const pParser);
  bool TLoadFadeValues(CCBTEngineParser* const pParser);
  void EndComponents(void);
  
public:
  // CGUIWindow / Obtencion del tipo de ventana
  inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el tipo de interfaz
	return GUIManagerDefs::GUIW_MAINMENU;
  }

public:
  // CGUIWindow / Activacion y desactivacion de la ventana de interfaz
  void Active(void);  
  void Deactive(void);
private:
  // Metodos de apoyo
  void ActiveComponents(const bool bActive);
  
public:
  // CGUIWindow / Activacion / desactivacion de la entrada
  void SetInput(const bool bActive);

public:
  // iCCommandClient / Notificacion para la finalizacion del comando Fade
  void EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
					const dword udInstant,
					const dword udExtraParam);  

public:
  // iCGUIWindow / Notificacion de eventos
  void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent);
private:
  // Metodos de apoyo
  void OnSetSelect(const bool bSelect,
				   const GUIDefs::GUIIDComponent& IDComponent);
  void OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent);
  byte GetIDOptionPos(const GUIDefs::GUIIDComponent& IDComponent) const {
	ASSERT(IsInitOk());
	ASSERT((IDComponent >= CGUIWMainMenu::ID_STARTNEWGAME) != 0);
	ASSERT((IDComponent <= CGUIWMainMenu::ID_EXIT) != 0);
	// Retorna indice en el array de posiciones a traves de su ID
	return IDComponent - CGUIWMainMenu::ID_STARTNEWGAME;
  }

public:
  // iCGUIWindow / Manejo de los eventos de entrada  
  bool DispatchEvent(const InputDefs::sInputEvent& InputEvent);
private:
  void InstallClient(void);
  void UnistallClient(void);

public:
  // Operacion de dibujado
  void Draw(void);
};

#endif // ~ ifndef CGUIWMainMenu