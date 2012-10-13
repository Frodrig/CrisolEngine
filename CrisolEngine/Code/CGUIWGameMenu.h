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
// CGUIWGameMenu.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUIWGameMenu
//
// Descripcion:
// - Implementa el menu de juego. El menu de juego aparecera cuando se pulse
//   a la tecla ESC durante el juego y sin tener ningun interfaz de interaccion
//   activo. Desde este menu se podra CARGAR, SALVAR, CONFIGURAR, PROSEGUIR y
//   SALIR DEL JUEGO.
// - Cuando desde este menu se tenga que mostrar un cuadro de dialogo (caso
//   del de pregunta para confirmar salida y el de aviso para informar de que
//   no es posible guardar) se utilizara un flag para que no se dibuje el
//   menu temporalmente.
//
// Notas:
// - Al pulsar sobre salir de la sesion de juego actual, se lanzara un
//   cuadro de dialogo advirtiendo.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWGAMEMENU_H_
#define _CGUIWGAMEMENU_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabecera
#ifndef _CGUIWINDOW_H_
#include "CGUIWindow.h"
#endif
#ifndef _CGUICBITMAP_H_
#include "CGUICBitmap.h"
#endif
#ifndef _CGUICBUTTON_H_
#include "CGUICButton.h"
#endif
#ifndef _ICGUIWINDOWCLIENT_H_
#include "iCGUIWindowClient.h"
#endif

// Definicion de clases / estructuras / espacios de nombres
class CCBTEngineParser;
struct iCGraphicSystem;
struct iCGUIManager;

// clase CGUIWGameMenu
class CGUIWGameMenu: public CGUIWindow,
                     public iCGUIWindowClient
{
private:
  // Tipos
  typedef CGUIWindow Inherited; // Acceso a la clase base

private:  
  // Enumerados
  enum {	
	// Identificadores para componentes
	ID_SAVEGAME = 0x01,
	ID_LOADGAME,
	ID_RESUMEGAME,
	ID_EXITGAME,
	ID_BACKIMG
  };

  enum {
	// Identificadores para los posibles fades
	ID_ENDEXITFADE = 0x01,
  };
  
private:
  // Estructuras
  struct sBackgroundInfo {
	// Info asociado a la imagen de fondo del menu
	CGUICBitmap Background; // Imagen de fondo       	
  };

  struct sMenuOptionsInfo {
	// Info asociada a los botones del menu
	// Enumerados
	enum {
	  MAX_MENU_BUTTONS = 4
	};

	// Botones de menu
	CGUICButton MenuButtons[MAX_MENU_BUTTONS];
  };

  struct sFadeInfo {
	// Fade de FX para el fondo
	GraphDefs::sRGBColor RGBColor; // Color
	word                 uwSpeed;  // Velocidad
  };

private:
  // Interfaces a otros subsistemas
  iCGraphicSystem* m_pGraphSys;   // Subsistema grafico
  iCGUIManager*    m_pGUIManager; // Manejador de interfaces

  // Componentes
  sBackgroundInfo  m_BackImgInfo; // Imagen de fondo
  sMenuOptionsInfo m_MenuOptions; // Menu de botones
  sFadeInfo        m_FadeInfo;    // Info de FX

  // Resto
  bool m_bIsDialogActive; // ¿Esta activo algun cuadro de dialogo?

public:
  // Constructor / destructor  
  CGUIWGameMenu(void): m_pGraphSys(NULL),
					   m_pGUIManager(NULL),
					   m_bIsDialogActive(false) { }
  ~CGUIWGameMenu(void) { End(); }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(void);
  void End(void);
private:
  // Metodos de apoyo
  bool TLoad(void);
  bool TLoadBackImage(CCBTEngineParser* const pParser);
  bool TLoadMenuOptions(CCBTEngineParser* const pParser);
  bool TLoadFadeInfo(CCBTEngineParser* const pParser);
  void TLoadButton(CGUICButton::sGUICButtonInitData& CfgData,
				   const std::string& szButtonName,
				   CCBTEngineParser* const pParser);
  void EndBackImage(void);
  void EndMenuOptions(void);  

public:
  // CGUIWindow / Obtencion del tipo de ventana
  inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el tipo de interfaz
	return GUIManagerDefs::GUIW_GAMEMENU;
  }

private:
  // Refresco de la entrada
  void RefreshInput(void);

public:
  // Activacion / desactivacion de la entrada
  void SetInput(const bool bInput);

public:
  // CGUIWindow / Activacion y desactivacion de la ventana de interfaz
  void Active(void);  
  void Deactive(void);
private:
  // Metodos de apoyo
  void ActiveComponents(const bool bActive);
  
public:
  // iCGUIWindow / Notificacion de eventos
  void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent);
private:
  // Metodos de apoyo
  void OnButtonSelect(const GUIDefs::GUIIDComponent& IDButton,
					  const bool bSelect);
  void OnButtonLeftClick(const GUIDefs::GUIIDComponent& IDButton);

public:
  // iCGUIWindow / Notificacion para la finalizacion de comandos (fade)
  void EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
					const dword udInstant,					
					const dword udExtraParam);

public:
  // iCGUIWindow / Manejo de los eventos de entrada  
  bool DispatchEvent(const InputDefs::sInputEvent& aInputEvent);
private:
  void InstallClient(void);
  void UnistallClient(void);

public:
  // iCGUIWindowClient / Notificacion de un determinado suceso en un interfaz
  void GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
					   const dword udParams);

public:
  // Operacion de dibujado
  void Draw(void);
};

#endif // ~ ifndef CGUIWGameMenu