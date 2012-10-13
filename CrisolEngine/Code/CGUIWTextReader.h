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
// CGUIWTextReader.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases
// - CGUIWTextReader
//
// Descripcion:
// - Interfaz encargado de procesar un archivo de texto para mostrarlo en
//   pantalla. La idea es simular mediante este interfaz libros, notas, etc,
//   etc, sensibles de lectura.
// - El interfaz estara compuesto por una imagen de fondo, dos botones de scroll
//   y un boton de aceptacion.
// - La notificacion a un posible cliente se hara cuando haya pulsacion sobre
//   el boton de salida, mandandose un 0 indicativo de que se ha pulsado
//   sobre el boton salir.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWTEXTREADER_H_
#define _CGUIWTEXTREADER_H_

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
#ifndef _CGUICBLOCKTEXT_H_
#include "CGUICBlockText.h"
#endif
#ifndef _CGUICLINETEXT_H_
#include "CGUICLineText.h"
#endif
#ifndef _CFXBACK_H_
#include "CFXBack.h"
#endif

// Definicion de clases / estructuras / espacios de nombres
class CCBTEngineParser;

// clase CGUIWTextReader
class CGUIWTextReader: public CGUIWindow
{  
private:
  // Tipos
  typedef CGUIWindow Inherited; // Acceso a la clase base

private:  
  // Enumerados
  enum {	
	// Identificadores	
	ID_BACKIMG = 0x01, // Imagen de fondo	
	ID_UPBUTTON,       // Boton de scroll hacia arriba
	ID_DOWNBUTTON,     // Boton de scroll hacia abajo
	ID_EXITBUTTON,     // Boton de aceptacion
	ID_TEXT,	       // Texto 
	ID_TITLE           // Titulo
  };

private:
  // Estructuras
  struct sBackImgInfo {
	// Info asociada a la imagen de fondo
	CGUICBitmap BackImg; // Imagen de fondo
  };

  struct sOptionsInfo {
	// Opciones sobre el interfaz
	CGUICButton ExitButton; // Boton de aceptacion
  };

  struct sTextInfo {
	// Texto a leer y botones de scroll para su manejo
	// Configuracion 
	CGUICBlockText::sGUICBlockTextInitData CfgText; // Cfg del bloque de texto	
	// Componentes	 
	CGUICButton    UpButton;   // Scroll hacia arriba
	CGUICButton    DownButton; // Scroll hacia abajo  
	CGUICBlockText Text;       // Texto a leer	
	CFXBack        FXBack;     // Fx de fondo
  };

  struct sTitleInfo {
	// Info asociada al titulo
	bool          bExistTitle; // ¿Hay titulo asociado?
	CGUICLineText Title;       // Texto asociado al titulo
	CFXBack       FXBack;      // FX de fondo para el texto	
	// Datos de configuracion
	GraphDefs::sRGBColor   FXBackColor; // Color asociado al FXBack
	GraphDefs::sAlphaValue FXAlpha;     // Alpha asociado al FXBack
  };

private:
  // Info asociada a los componentes
  sBackImgInfo m_BackImgInfo; // Imagen de fondo  
  sTextInfo    m_TextInfo;    // Texto a leer
  sOptionsInfo m_OptionsInfo; // Opciones
  sTitleInfo   m_TitleInfo;   // Titulo

public:
  // Constructor / destructor  
  CGUIWTextReader(void) { }
  ~CGUIWTextReader(void) { End(); }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(void);
  void End(void);
private:
  // Metodos de apoyo
  bool TLoad(void);  
  bool TLoadBackImgInfo(CCBTEngineParser* const pParser);
  bool TLoadTextInfo(CCBTEngineParser* const pParser);
  bool TLoadOptionsInfo(CCBTEngineParser* const pParser);  
  bool TLoadTitleInfo(CCBTEngineParser* const pParser);
  void EndComponents(void);
  
public:
  // CGUIWindow / Obtencion del tipo de ventana
  inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el tipo de interfaz
	return GUIManagerDefs::GUIW_TEXTREADER;
  }
  
public:
  // CGUIWindow / Activacion y desactivacion de la ventana de interfaz
  void Active(const std::string& szTitle,
			  const std::string& szFileName,
			  iCGUIWindowClient* const pClient);  
  void Deactive(void);
private:
  // Metodos de apoyo
  void ActiveComponents(const bool bActive);
  bool PrepareText(const std::string& szFileName);
  void SetTitle(const std::string& szTitle);

public:
  // iCGUIWindow / Notificacion de eventos
  void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent);
private:
  // Metodos de apoyo  
  void OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
			    const bool bSelect);
  void OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent);

public:
  // Operacion de dibujado
  void Draw(void);
};

#endif // ~ ifndef CGUIWTextReader