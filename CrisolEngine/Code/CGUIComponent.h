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
// CGUIComponent.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUIComponent
//
// Descripcion:
// - Define la clase base para todos los componentes de interfaz.
//
// Notas:
// - Clase abstracta. 
// - Se heredara de iCInputClient para permitir que las clases que a
//   su vez hereden de esta, puedan recibir entrada. Desde esta clase solo
//   se instalaran los eventos de entrada oportunos con InstallClient y
//   UnistallClient.
// - Las dimensiones se mediran en pixels, luego una dimension sera [0..63]
//   y no [0..64), luego tendra una dimension de 63-0+1=64 pixels.
// - Cuando se hable de clase activa nos referiremos unicamente a si la
//   clase debe de responder eventos y/o dibujarse, independientemente de que
//   se halle inicializada.
// - No se permitiran ID's con valor 0.
// - Todas las clases que hereden de esta usaran el prefio GUIC, donde la 
//   C indicara Component.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUICOMPONENT_H_
#define _CGUICOMPONENT_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _GUIDEFS_H_
#include "GUIDefs.h"
#endif
#ifndef _SYSENGINE_H_
#include "SYSEngine.h"
#endif
#ifndef _FONTDEFS_H_
#include "FontDefs.h"
#endif
#ifndef _ICINPUTCLIENT_H_
#include "iCInputClient.h"
#endif
#ifndef _ICGUIWINDOW_H_
#include "iCGUIWindow.h"
#endif
#ifndef _ICINPUTMANAGER_H_
#include "iCInputManager.h"
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CGUIComponent
class CGUIComponent: public iCInputClient 
{
public:
  // Estructuras
  struct sGUIComponentInitData {
	// Inicializacion de CGUIComponent
	// Datos
	GUIDefs::GUIIDComponent ID; 	          // ID de componente
	byte                    ubDrawPlane;      // Plano de dibujado
	sPosition			    Position;         // Posicion de dibujado
	iCGUIWindow*            pGUIWindow;       // Ventana asociada
	GUIDefs::GUIEvents      GUIEvents;        // Eventos
	bool                    bClickWithSelect; // ¿Los cliqueos SOLO con seleccion?
	// Constructor por defecto
	sGUIComponentInitData(void): ubDrawPlane(0), 
								 ID(0), 
								 pGUIWindow(NULL), 
								 GUIEvents(GUIDefs::GUIC_NO_EVENT),
								 bClickWithSelect(true) { }
  };

private:
  // Estructuras
  struct sComponentInfo: public sGUIComponentInitData {
	// Info asociada al componente	
	bool  bIsCursorInArea; // ¿Cursor en area del componente?
	bool  bIsActive;       // ¿Componente activo?	
	dword udInitClickTime; // Tiempo del primer click de raton
  };

private:
  // Vbles de miembro
  sComponentInfo m_ComponentInfo; // Info asociada al componente
  bool           m_bIsInitOk;     // ¿Inicializacion correcta?

public:
  // Constructor / destructor
  CGUIComponent(void): m_bIsInitOk(false) { }	
  ~CGUIComponent(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const sGUIComponentInitData& InitData);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // Establecimiento de componente activo / inactivo
  virtual void SetActive(const bool bActive); 
  inline bool IsActive(void) const {	
	ASSERT(IsInitOk());
	// ¿Componente activo?
	return m_ComponentInfo.bIsActive;
  }

public:
  // Refresco sobre la entrada
  void RefreshInput(void);

public:
  // Activacion / desactivacion de la entrada
  virtual void SetInput(const bool bInput);
  
public:
  // iCInputClient / Trabajo con los eventos de entrada
  bool DispatchEvent(const InputDefs::sInputEvent& aInputEvent);
private:
  // iCInputClient / Metodos de apoyo
  void InstallClient(void);    
  void UnistallClient(void);
private:
  // Metodos de apoyo para la gestion de los eventos
  void OnButtonMouseClick(const GUIDefs::eGUICEventType& Button);
  void OnMouseMovement(const sPosition& MousePos);
  
public:
  // Operaciones de seleccion
  virtual void Select(const bool bSelect) = 0;
  virtual inline bool IsSelect(void) const = 0; 

public:
  // Dibujado
  virtual void Draw(void) = 0;  

public:
  // Trabajo con los eventos asociados
  inline bool IsEvent(const GUIDefs::eGUICEventType& Event) const { 
	// SOLO si instancia activa
	ASSERT(IsInitOk());
	return (m_ComponentInfo.GUIEvents & Event) ? true : false;
  }

protected:
  // Envio de los eventos producidos
  inline void SendEventToGUIWindow(const GUIDefs::eGUICEventType& Event,
								   const GUIDefs::GUIEventParam& Param = 0) {
	// SOLO si instancia activa
	ASSERT(IsInitOk());
	// Se envia el evento SOLO si componente no esta bloqueado
	if (!SYSEngine::GetInputManager()->IsInputClientBlocked(this)) {
	  m_ComponentInfo.pGUIWindow->ComponentNotify(GUIDefs::sGUICEvent(Event, m_ComponentInfo.ID, Param));
	}
  }	

public:
  // Trabajo con el area del componente
  virtual sword GetXPos(void) {
	ASSERT(IsInitOk());
	// Retorna la posicion en X
	return m_ComponentInfo.Position.swXPos;
  }  
  virtual sword GetYPos(void) {
	ASSERT(IsInitOk());
	// Retorna la posicion en X
	return m_ComponentInfo.Position.swYPos;
  }
  inline void SetXPos(const sword& swXPos) {
	ASSERT(IsInitOk());
	// Varia la posicion en X del area
	m_ComponentInfo.Position.swXPos = swXPos;
  }
  inline void SetYPos(const sword& swYPos) {
	ASSERT(IsInitOk());
	// Varia la posicion en Y del area
	m_ComponentInfo.Position.swYPos = swYPos;	
  }
  virtual word GetWidth(void) = 0;
  virtual word GetHeight(void) = 0;  
  
public:
  // Obtencion de informacion
  inline GUIDefs::GUIIDComponent GetID(void) const { 	
	ASSERT(IsInitOk());
	// Retorna ID del componente
	return m_ComponentInfo.ID; 
  }
  inline byte GetDrawPlane(void) const {	
	ASSERT(IsInitOk());
	// Retorna plano de dibujado
	return m_ComponentInfo.ubDrawPlane;
  }
};

#endif
