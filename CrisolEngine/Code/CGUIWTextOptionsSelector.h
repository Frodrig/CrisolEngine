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
// CGUIWTextOptionsSelector.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases
// - CGUIWTextOptionsSelector
//
// Descripcion:
// - Representa la interfaz que se encarga de trabajar con la seleccion
//   de opciones de texto de forma general. 
//
// Notas:
// - Los identificadores de los componentes debera de tener un valor 
//   superior a Inherited::MAX_IDOPTION_VALUE para que no existan
//   conflictos con los identificadores.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWTEXTOPTIONSELECTOR_H_
#define _CGUIWTEXTOPTIONSELECTOR_H_

// Cabecera
#ifndef _CGUIWBASETEXTSELECTOR_H_
#include "CGUIWBaseTextSelector.h"
#endif
#ifndef _CGUICBITMAP_H_
#include "CGUICBitmap.h"
#endif
#ifndef _CGUICLINETEXT_H_
#include "CGUICLineText.h"
#endif
#ifndef _CGUICBUTTON_H_
#include "CGUICButton.h"
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif

// Definicion de clases / estructuras / espacios de nombres
class CCBTEngineParser;

// clase CGUIWTextOptionsSelector
class CGUIWTextOptionsSelector: public CGUIWBaseTextSelector								
{  
private:
  // Tipos
  typedef CGUIWBaseTextSelector Inherited; // Acceso a la clase base

private:
  // Enumerados
  enum {
	// Identificadores para los componentes
	ID_BACKIMG = Inherited::MAX_IDOPTION_VALUE + 1,
	ID_TITLE,
	ID_UPSCROLL,
	ID_DOWNSCROLL,
	ID_CANCEL,
  };

private:
  // Estructuras
  struct sBackImgInfo {
	// Info asociada a la imagen de fondo
	CGUICBitmap BackImg; // Imagen de fondo
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

  struct sCancelInfo {
	// Info asocada al boton de cancelacion
	CGUICButton CancelButton; // Boton de cancelacion
	bool        bCanCancel;   // ¿Se puede usar el boton de cancelacion?
  };

  struct sScrollInfo {
	// Info asociada a los botones de scroll
	CGUICButton UpButton;   // Boton de scroll hacia arriba
	CGUICButton DownButton; // Boton de scroll hacia abajo
  };

private:
  // Componentes
  sBackImgInfo m_BackImgInfo; // Imagen de fondo
  sTitleInfo   m_TitleInfo;   // Titulo
  sCancelInfo  m_CancelInfo;  // Boton de cancelacion
  sScrollInfo  m_ScrollInfo;  // Botones de scroll  

  // Resto
  //OptionsMap m_OptionsToAdd; // Opciones a añadir

public:
  // Constructor / destructor  
  CGUIWTextOptionsSelector(void) { }
  ~CGUIWTextOptionsSelector(void) { End(); }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(void);
  void End(void);
private:
  // Metodos de apoyo en la carga (e inicializacion) y finalizacion
  bool TLoad(void);
  bool TLoadBackImgInfo(CCBTEngineParser* const pParser);
  bool TLoadTitleInfo(CCBTEngineParser* const pParser);
  bool TLoadCancelInfo(CCBTEngineParser* const pParser);
  bool TLoadScrollInfo(CCBTEngineParser* const pParser);
  void EndComponents(void);

public:
  // CGUIWindow / Obtencion del tipo de ventana
  inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el tipo de interfaz
	return GUIManagerDefs::GUIW_GENERICTEXTSELECTOR;
  }

public:
  // CGUIWindow / Activacion y desactivacion de la ventana de interfaz
  void Active(const std::string& szTitle,
			  const bool bCanCancel);  
  void Deactive(void);
private:
  // Metodos de apoyo
  void SetTitle(const std::string& szTitle);
  void ActiveComponents(const bool bActive);

public:
  // Activacion / desactivacion de la entrada
  void SetInput(const bool bInput);

public:
  // iCGUIWindow / Notificacion de eventos
  void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent);
private:
  // Metodos de apoyo  
  void OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
			    const bool bSelect);
  void OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent);
protected:
  // CGUIWBaseTextSelector
  void OnTextOptionLeftClick(const GUIDefs::GUIIDComponent& IDOption);

public:
  // Operacion de dibujado
  void Draw(void);
};

#endif // ~ ifndef CGUIWTextOptionsSelector