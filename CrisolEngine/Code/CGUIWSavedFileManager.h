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
// CGUIWSavedFileManager.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUIWBaseSavedFileManager
//   * CGUIWLoaderSavedFileManager
//   * CGUIWSaverSavedFileManager
//
// Descripcion:
// - la clase CGUIWBaseSavedFileManager sera una clase base pura para representar 
//   las interfaces de cargar y salvar  partidas.
//   Esta clase implementara la logica asociada al manejo de la lista de
//   partidas guardadas, dejando que las derivadas recogan los eventos asociados
//   sobre el boton de cargar / salvar, asi como el relativo a abandonar la
//   interfaz o eliminar archivos (aunque la implementacion este en esta clase)
// - La clase CGUIWLoaderSavedFileManager se encargara de gestionar las acciones
//   de carga de ficheros. Desde este interfaz solo se podra seleccionar un
//   fichero para cargar. A este interfaz se accedera tanto desde el menu
//   principal como desde dentro del juego.
// - La clase CGUIWSaverSavedFileManager se encargara de gestionar las acciones
//   de salvar ficheros, con la particularidad de que tambien se dejara borrar
//   ficheros que previamente pudieran haberse guardado, con el fin de tener 
//   unas minimas opciones de gestion de los mismos. Se accedera a este 
//   interfaz SIEMPRE desde dentro del juego.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWSAVEDFILEMANAGER_H_
#define _CGUIWSAVEDFILEMANAGER_H_

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
#ifndef _CFXBACK_H_
#include "CFXBack.h"
#endif
#ifndef _CGUICLINETEXT_H_
#include "CGUICLineText.h"
#endif
#ifndef _CGUICINPUT_H_
#include "CGUICInput.h"
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif

// Definicion de clases / estructuras / espacios de nombres
class CCBTEngineParser;
struct iCGUIManager;
struct iCInputManager;

// clase CGUIWBaseSavedFileManager
class CGUIWBaseSavedFileManager: public CGUIWindow
{
private:
  // Tipos
  typedef CGUIWindow Inherited; // Acceso a la clase base

private:
  // Estructuras forward
  struct sSavedFileInfo;

private:
  // Tipos
  // Lista de ficheros de partidas guardadas
  typedef std::list<sSavedFileInfo*> SavedFileList;
  typedef SavedFileList::iterator    SavedFileListIt;

protected:
  // Enumerados
  enum {
	ID_BACKIMG = 1,        // Id. de la imagen de fondo
	ID_UPBUTTON,           // Id. de boton de navegacion hacia arriba
	ID_DOWNBUTTON,         // Id. de boton de navegacion hacia abajo
	ID_LOADBUTTON,         // Id. del boton de cargar
	ID_SAVEBUTTON,         // Id. del boton de salvar
	ID_REMOVEBUTTON,       // Id. de boton de borrar partida
	ID_EXITBUTTON,         // Id. de boton de salir
	ID_SAVEDFILEINPUT,     // Id. del input para el nombre del archivo
	ID_TEXTINFO,           // Id. de texto informativo general
	ID_BASEFILESAVED = 100 // Id. base para comenzar a asignar a nombres de archivos
  };
  
private:
  // Estructuras
  struct sBackImgInfo {
	// Info relativa a la imagen de fondo
	CGUICAnimBitmap BackImg; // Imagen de fondo
  };

  struct sSavedFileList {
	// Info relativa a la lista de ficheros salvados a mostrar
	SavedFileList   Files;             // Ficheros guardados
	SavedFileListIt FileSelectIt;      // Iterador al fichero seleccionado	
	byte            ubMaxVisibleFiles; // Num. maximo de archivos visibles
	word            uwMaxCharacters;   // Num. maximo de caracteres
	word            uwIDBaseDescIdx;   // Indice para asociar id. a componentes
	// Datos de configuracion de los nombres de archivo
	sPosition                            InitDrawPos; // Pos. inicial
	CGUICLineText::sGUICLineTextInitData BaseDescCfg; // Cfg. textos
	// Info asociada a los botones de scroll
	CGUICButton UpButton;   // Boton de subir en lista
	CGUICButton DownButton; // Boton de bajar en lista		
	// Info relativa al FX asociado al nombre seleccionado
	CFXBack                FXBack;         // FXBack
	word                   uwMaxCharWidth; // Anchura del caracter mas ancho
	GraphDefs::sRGBColor   FXRGBColor;     // Color RGB
	GraphDefs::sAlphaValue FXAlpha;        // Valor alpha asociado	
  };

  struct sSelectSavedFileDesc {
	// Descripcion general asociada a la partida guardada seleccionada
    CGUICLineText AreaName;     // Componente con el nombre del area
	CGUICLineText PlayerName;   // Componente con el nombre del jugador
	CGUICLineText PlayerHealth; // Componente con la salud del jugador
	CGUICLineText WorldTime;    // Componente con la hora del universo de juego
  };

  struct sSavedFileIt {
	// Estructura para el control de la lista de ficheros guardados
	SavedFileListIt It;   // Iterador a la primera descripcion mostrada
	word            uwIt; // Identificador numerico asociado 
	// Constructores
	sSavedFileIt(void) { }
	sSavedFileIt(const SavedFileListIt& ListIt,
				 const word uwListIt) {
	  It = ListIt;
	  uwIt = uwListIt;
	}
	// Operadores
	const sSavedFileIt& operator=(const sSavedFileIt& SavedFileIt) {
	  It = SavedFileIt.It;
	  uwIt = SavedFileIt.uwIt;
	  return *this;
	}
  };

private:
  // Vbles de miembro
  sBackImgInfo         m_BackImgInfo;         // Info relativa a la imagen de fondo
  sSavedFileList       m_SavedFileList;       // Manejo de la lista de ficheros guardados
  sSelectSavedFileDesc m_SelectSavedFileDesc; // Desc. del fichero seleccionado
  sSavedFileIt         m_SavedFileIt;         // Iterador a la lista de ficheros
  
public:
  // Constructor / destructor
  CGUIWBaseSavedFileManager(void) { }
  ~CGUIWBaseSavedFileManager(void) { End(); }

public:
  // Protocolos de inicio y fin
  bool Init(void);
  void End(void);
protected:
  // Metodos de apoyo
  bool TLoad(const std::string& szSection);
private:
  bool TLoadBackImg(CCBTEngineParser* const pParser);
  bool TLoadSavedFileList(CCBTEngineParser* const pParser);  
  bool TLoadSelectSavedFileDesc(CCBTEngineParser* const pParser);
  bool TLoadOptions(CCBTEngineParser* const pParser);
  void EndComponents(void); 

public:
  // Activacion / desactivacion
  void Active(void);
  void Deactive(void);
private:
  // Metodos de apoyo
  void ActiveComponents(const bool bActive);
  
public:
  // Activacion / desactivacion de la entrada
  void SetInput(const bool bActive);

protected:
  // Metodos para el trabajo con los eventos
  void OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
				const bool bSelect);
  void OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent);
    
protected:
  // Trabajo con la lista de ficheros salvados
  inline word GetNumSavedFiles(void) const {
	ASSERT(Inherited::IsInitOk());
	// Devuelve el numero de ficheros
	return m_SavedFileList.Files.size();
  }
  inline word GetSavedFileBaseDescMaxCharacters(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el numero max. de caracteres para mostrar descripcion base
	return m_SavedFileList.uwMaxCharacters;
  }
  word GetIDSelectSavedFile(void);
  void AddSavedFile(void);
  void RemoveSelectSavedFile(void);
private:
  // Metodos de apoyo
  void ReleaseSavedFilesList(void);  
  word MakeSavedFilesList(void);  
  sSavedFileInfo* MakeSavedFileInfoNode(const word uwIDFile);
  void SetSavedFile(const GUIDefs::GUIIDComponent& IDComponent);
  void SetSavedFileListPos(const sSavedFileIt& SavedFileIt,
						   const bool bSet);
  void MoveUp(void);
  void MoveDown(void);
  SavedFileListIt FindSavedFileIt(const GUIDefs::GUIIDComponent& IDComponent);
  void SetFXBack(void);

public:
  // Dibujado
  void Draw(void);
}; // ~ CGUIWBaseSavedFileManager

// Clase CGUIWLoaderSavedFileManager
class CGUIWLoaderSavedFileManager: public CGUIWBaseSavedFileManager
{
private:
  // Tipos
  typedef CGUIWBaseSavedFileManager Inherited; // Acceso a la clase base

private:
  // Enumerados
  enum {
	// Fades
	ID_ENDLOADFADE = 1,   // Ha finalizado el fade despues de dar a cargar
	ID_ENDTOMAINMENUFADE, // Ha finalizado el fade de salir para volver al main menu
	ID_ENDACTIVEFADE      // Ha finalizado el fade de activacion
  };

private:
  // Estructuras
  struct sOptions {
	// Opciones posibles
	CGUICButton LoadButton; // Boton de cargar
	CGUICButton ExitButton; // Boton de salir
  };

  struct sFadeInfo {
	// Info asociada a los fades
	GraphDefs::sRGBColor RGBFadeOut;     // Color fadeout
	word                 uwFadeOutSpeed; // Velocidad fadeout
	word                 uwFadeInSpeed;  // Velocidad fadein
  };

private:
  // Interfaces a otros subsistemas
  iCGUIManager* m_pGUIManager; // Interfaz al manager de interfaces

  // Resto de vbles de miembro
  sOptions                       m_Options;      // Opciones
  sFadeInfo                      m_FadeInfo;     // Info asociada al fade
  GUIManagerDefs::eGUIWindowType m_PrevInterfaz; // Interfaz previo
  
public:
  // Constructor / destructor
  CGUIWLoaderSavedFileManager(void): m_pGUIManager(NULL) { }
  ~CGUIWLoaderSavedFileManager(void) { End(); }

public:
  // Protocolos de inicio y fin
  bool Init(const GUIManagerDefs::eGUIWindowType& PrevInterfaz);
  void End(void);
private:
  // Metodos de apoyo
  bool TLoad(void);
  bool TLoadOptions(CCBTEngineParser* const pParser);
  bool TLoadFadeInfo(CCBTEngineParser* const pParser);
  void EndComponents(void); 
  
public:
  // CGUIWindow / Obtencion del tipo de ventana
  inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el tipo de interfaz
	return GUIManagerDefs::GUIW_LOADERSAVEDFILEMANAGER;
  } 

public:
  // Activacion / desactivacion
  void Active(void);
  void Deactive(void);
private:
  // Metodos de apoyo
  void ActiveComponents(const bool bActive);
  
public:
  // Activacion / desactivacion de la entrada
  void SetInput(const bool bActive);

public:
  // iCCommandClient / Notificacion para la finalizacion del comando Fade
  void EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
					const dword udInstant,					
					const dword udExtraParam);  

public:
  // iCGUIWindow / Notificacion de eventos
  void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent);

protected:
  // Metodos para el trabajo con los eventos
  void OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
				const bool bSelect);
  void OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent);

public:
  // Dibujado
  void Draw(void);
}; // ~ CGUIWLoaderSavedFileManager

// Clase CGUIWSaverSavedFileManager
class CGUIWSaverSavedFileManager: public CGUIWBaseSavedFileManager
{
private:
  // Tipos
  typedef CGUIWBaseSavedFileManager Inherited; // Acceso a la clase base

private:
  // Estructuras
  struct sOptions {
	// Opciones posibles
	CGUICButton SaveButton;   // Boton de guardar
	CGUICButton RemoveButton; // Boton de borrar
	CGUICButton ExitButton;   // Boton de salir
  };

  struct sDescInput {
	// Info asociada a la entrada de la descripcion
	CGUICLineText HelpText;     // Identificativo del texto de ayuda descriptivo
	CGUICInput    Input;        // Componente para la introduccion del nombre	
	sPosition     LastMousePos; // Ultima posicion del raton
  };

private:
  // Interfaces a otros subsistemas
  iCGUIManager*   m_pGUIManager;   // Interfaz al manager de interfaces
  iCInputManager* m_pInputManager; // Interfaz al manager de entrada

  // Resto de vbles de miembro
  sOptions   m_Options;       // Opciones
  sDescInput m_DescInput; // Descripcion del archivo a guardar
  
public:
  // Constructor / destructor
  CGUIWSaverSavedFileManager(void): m_pGUIManager(NULL),
									m_pInputManager(NULL) { }
  ~CGUIWSaverSavedFileManager(void) { End(); }

public:
  // Protocolos de inicio y fin
  bool Init(void);
  void End(void);
private:
  // Metodos de apoyo
  bool TLoad(void);
  bool TLoadOptions(CCBTEngineParser* const pParser);
  bool TLoadDescInput(CCBTEngineParser* const pParser);
  void EndComponents(void); 
  
public:
  // CGUIWindow / Obtencion del tipo de ventana
  inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el tipo de interfaz
	return GUIManagerDefs::GUIW_SAVERSAVEDFILEMANAGER;
  } 

public:
  // Activacion / desactivacion
  void Active(void);
  void Deactive(void);
private:
  // Metodos de apoyo
  void ActiveComponents(const bool bActive);
  
public:
  // Activacion / desactivacion de la entrada
  void SetInput(const bool bActive);

public:
  // iCGUIWindow / Notificacion de eventos
  void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent);

protected:
  // Metodos para el trabajo con los eventos
  void OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
				const bool bSelect);
  void OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent);
  void OnEndInput(const bool bInputOk);

public:
  // Dibujado
  void Draw(void);
}; // ~ CGUIWSaverSavedFileManager

#endif // ~ ifndef CGUIWSavedFileManager