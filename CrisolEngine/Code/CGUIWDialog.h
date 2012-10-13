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
// CGUIWDialog.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUIWDialog 
//    * CGUIWAdviceDialog
//    * CGUIWQuestionDialog
//
// Descripcion:
// - En este archivo se definiran los cuadros de dialogo para trabajar con
//   la muestra de pequeños mensajes de notificacion y de preguntas si/no al
//   usuario.
// - Todos los cuadros de dialogo heredaran de la clase abstracta CGUIWDialog
//   que se encargara de mantener la imagen de fondo y el texto a mostrar.
//
// Notas:
// - En las clases base se debera de implementar el metodo TLoadOptionsInfo
//   cuyo objetivo sera el de la carga de los botones que particularizan
//   a los cuadros de dialogo. Este metodo sera llamado desde CGUIWDialog
//   en el proceso de carga de datos.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWDIALOG_H_
#define _CGUIWDIALOG_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabecera
#ifndef _CGUIWINDOW_H_
#include "CGUIWindow.h"
#endif
#ifndef _CGUICBITMAP_H_
#include "CGUICBitmap.h"
#endif
#ifndef _CGUICSINGLETEXT_H_
#include "CGUICSingleText.h"
#endif
#ifndef _CFXBACK_H_
#include "CFXBack.h"
#endif
#ifndef _CGUICBUTTON_H_
#include "CGUICButton.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif

// Definicion de clases / estructuras / espacios de nombres
class CCBTEngineParser;
struct iCGraphicSystem;

// clase CGUIWDialog
class CGUIWDialog: public CGUIWindow				
{
private:
  // Tipos
  typedef CGUIWindow Inherited; // Acceso a la clase base

private:  
  // Enumerados
  enum {	
	// Identificadores para componentes
	ID_BACKIMG = 0x01, // Imagen de fondo
	ID_TEXT	           // Texto
  };
 
private:
  // Estructuras
  struct sBackImgInfo {
	// Info asociada a la imagen de fondo
	CGUICBitmap BackImg; // Imagen de fondo
  };

  struct sTextInfo {
	// Informacion asociada al texto 
	CGUICSingleText Text;   // Texto
	CFXBack         FXBack; // FXBack
	// Configuracion del FXBack
	GraphDefs::sAlphaValue FXBackAlpha; // Alpha del FXBack
	GraphDefs::sRGBColor   FXBackColor; // Color del FXBack 
  }; 

private:  
  // Info asociada a los componentes
  sBackImgInfo m_BackImg;  // Imagen de fondo
  sTextInfo    m_TextInfo; // Texto de fondo
  
public:
  // Constructor / destructor  
  CGUIWDialog(void) { }
  ~CGUIWDialog(void) { End(); }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(void);
  void End(void);
protected:
  // Metodos de apoyo
  bool TLoad(CCBTEngineParser* const pParser);
private:
  bool TLoadBackImgInfo(CCBTEngineParser* const pParser);
  bool TLoadTextInfo(CCBTEngineParser* const pParser);
protected:
  virtual bool TLoadOptionsInfo(CCBTEngineParser* const pParser) = 0;
private:
  void EndBackImg(void);  
  void EndTextInfo(void);  
  
public:
  // CGUIWindow / Activacion y desactivacion de la ventana de interfaz
  void Active(const std::string& szText,
			  iCGUIWindowClient* const pClient);  
  void Deactive(void);

public:
  // Operacion de dibujado
  void Draw(void);
private:
  // Operacion de dibujado de las opciones
  virtual void DrawOptions(void) = 0;
}; // ~ CGUIWADVICEDialog

// clase CGUIWAdviceDialog
class CGUIWAdviceDialog: public CGUIWDialog
{
private:
  // Tipos
  typedef CGUIWDialog Inherited; // Acceso a la clase base

private:  
  // Enumerados
  enum {	
	// Identificadores para componentes
	ID_ACCEPT = 0X03 // Boton de aceptacion
  };
 
private:
  // Estructuras
  struct sOptionsInfo {
	// Info asociada a las opciones disponibles
	CGUICButton Accept; // Boton de aceptacion	
  };
  
private:  
  // Info asociada a los componentes
  sOptionsInfo m_OptionsInfo; // Opciones
  
public:
  // Constructor / destructor  
  CGUIWAdviceDialog(void) { }
  ~CGUIWAdviceDialog(void) { End(); }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(void);
  void End(void);
private:
  // Metodos de apoyo
  bool TLoad(void);
  bool TLoadOptionsInfo(CCBTEngineParser* const pParser);

public:
  // CGUIWindow / Obtencion del tipo de ventana
  inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el tipo de interfaz
	return GUIManagerDefs::GUIW_ADVICEDIALOG;
  }
  
public:
  // CGUIWindow / Activacion y desactivacion de la ventana de interfaz
  void Active(const std::string& szText,
			  iCGUIWindowClient* const pClient);  
  void Deactive(void);

public:
  // CGUIWindow / Notificacion de eventos
  void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent);

private:
  // Operacion de dibujado de las opciones
  void DrawOptions(void);
};

// clase CGUIWQuestionDialog
class CGUIWQuestionDialog: public CGUIWDialog
{
private:
  // Tipos
  typedef CGUIWDialog Inherited; // Acceso a la clase base

private:  
  // Enumerados
  enum {	
	// Identificadores para componentes
	ID_YES = 0X03, // Boton de aceptacion
	ID_NO,         // Boton de rechazo
	ID_CANCEL      // Boton de cancelacion
  };
 
private:
  // Estructuras
  struct sOptionsInfo {
	// Info asociada a las opciones disponibles
	CGUICButton Yes;    // Boton de aceptacion
	CGUICButton No;     // Boton de rechazo
	CGUICButton Cancel; // Boton de cancelacion
  };
  
private:  
  // Info asociada a los componentes
  sOptionsInfo m_OptionsInfo; // Opciones
  
public:
  // Constructor / destructor  
  CGUIWQuestionDialog(void) { }
  ~CGUIWQuestionDialog(void) { End(); }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(void);
  void End(void);
private:
  // Metodos de apoyo
  bool TLoad(void);
  bool TLoadOptionsInfo(CCBTEngineParser* const pParser);

public:
  // CGUIWindow / Obtencion del tipo de ventana
  inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el tipo de interfaz
	return GUIManagerDefs::GUIW_QUESTIONDIALOG;
  }
  
public:
  // CGUIWindow / Activacion y desactivacion de la ventana de interfaz
  void Active(const std::string& szText,
			  const bool bCancelPresent,
			  iCGUIWindowClient* const pClient);  
  void Deactive(void);

public:
  // CGUIWindow / Notificacion de eventos
  void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent);
private:
  // Metodos de apoyo
  void OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
			    const bool bSelect);
  void OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent);

private:
  // Operacion de dibujado de las opciones
  void DrawOptions(void);
}; // ~ CGUIWQuestionDialog

#endif // ~ ifndef CGUIWDialog