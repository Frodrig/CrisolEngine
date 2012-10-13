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
// CGUIWindow.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUIWindow
//
// Descripcion:
// - Clase base que define un componente de tipo ventana.
//
// Notas:
// - La clase hereda de su interfaz para que los CGUIComponent puedan 
//   notificarle el suceso de eventos. Tambien herda de iCInputClient para que
//   pueda contestar a los eventos de entrada
// - La clase sera definida como pura a traves de su metodo 
// - Incorpora el comportamiento por defeco para el manejo de los eventos de
//   entrada y notificacion de eventos CGUIComponent. Estos comportamientos
//   sera el no hacer nada.
// - Gestiona el activar / desactivar una ventana de interfaz mediante los
//   metodos Active y Deactive. Ambos metodos instalan y desinstalan los 
//   clientes de interfaz respectivamente.
// - La activacion recibira siempre un posible cliente para la notificacion
//   de eventos sobre una ventana. Por defecto valdra NULL. La notificacion
//   a un cliente siempre se hara desde la ventana particular, nunca desde
//   esta clase y no todas las ventanas particulares tendran porque notificar
//   eventos.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWINDOW_H_
#define _CGUIWINDOW_H_

#ifndef _ICGUIWINDOW_H_
#include "iCGUIWindow.h"
#endif
#ifndef _ICINPUTCLIENT_H_
#include "iCInputClient.h"
#endif
#ifndef _ICCOMMANDCLIENT_H_
#include "iCCommandClient.h"
#endif
#ifndef _ICGUIWINDOWCLIENT_H_
#include "iCGUIWindowClient.h"
#endif

// clase CGUIWindow
class CGUIWindow: public iCGUIWindow, 
				  public iCInputClient,
				  public iCCommandClient
{
protected:
  // Enumerados
  enum {
	// Velocidades maximas y minimas de fade
	MAX_FADE_SPEED = 512,
	MIN_FADE_SPEED = 18
  };

private:
  // Vbles de miembro
  iCGUIWindowClient* m_pGUIWClient; // Cliente asociado a la ventana
  bool               m_bIsInput;    // ¿Hay entrada?
  bool				 m_bIsActive;   // ¿Ventana de interfaz activa?
  bool				 m_bIsInitOk;   // ¿Clase inicializada?  

public:
  // Constructor / destructor
  CGUIWindow(void): m_pGUIWClient(NULL),
					m_bIsInitOk(false),
					m_bIsActive(false),
					m_bIsInput(false) { }
  ~CGUIWindow(void) { End(); }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(void);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // Obtencion del tipo de ventana
  virtual inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const = 0;

public:
  // iCGUIWindow / Notificacion de eventos a componentes de la interfaz
  void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent);

public:
  // Activacion / desactivacion de la ventana de interfaz
  virtual void Active(iCGUIWindowClient* const pGUIWClient = NULL);
  virtual void Deactive(void);
  inline bool IsActive(void) const { return m_bIsActive; }

protected:
  // Vinculacion de un cliente a la ventana de interfaz
  void SetGUIWindowClient(iCGUIWindowClient* const pClient) {
	ASSERT(IsInitOk());
	// Establece / quita un cliente de ventana asociado
	m_pGUIWClient = pClient;
  }

protected:
  // Notificacion de un evento a un posible cliente
  inline void GUIWindowNotify(const dword udParams) const {
	ASSERT(IsInitOk());
	// Notifica el evento al POSIBLE cliente asociado
	if (m_pGUIWClient) {
	  m_pGUIWClient->GUIWindowNotify(GetGUIWindowType(), udParams);
	}
  }

public:
  // Activacion / desactivacion de la entrada
  virtual void SetInput(const bool bInput);
  bool IsInput(void);

public:
  // iCInputClient / Manejo de los eventos de entrada
  virtual bool DispatchEvent(const InputDefs::sInputEvent& aInputEvent);
private:
  virtual void InstallClient(void);
  virtual void UnistallClient(void);

public:
  // Trabajo con los fades
  void DoFadeIn(const CommandDefs::IDCommand& IDCmd,
				const word uwSpeed,
				const GraphDefs::eDrawZone DrawZone = GraphDefs::DZ_CURSOR,
				const byte ubPlane = 0);
  void DoFadeOut(const CommandDefs::IDCommand& IDCmd,	
				 const GraphDefs::sRGBColor& RGBColor,
				 const word uwSpeed,
				 const GraphDefs::eDrawZone DrawZone = GraphDefs::DZ_CURSOR,
				 const byte ubPlane = 0);
private:
  // Metodos de apoyo
  inline word CheckFadeSpeed(const word uwSpeed) {
	ASSERT(IsInitOk());
	// Dada la velocidad de Fade recibida, retornara una que se ajuste a
	// un minimo de MIN_FADE_SPEED unidades y MAX_FADE_SPEED.
	if (uwSpeed < MIN_FADE_SPEED) { 
	  return MIN_FADE_SPEED;
	} else if (uwSpeed > MAX_FADE_SPEED) {
	  return MAX_FADE_SPEED;
	} else {
	  return uwSpeed;
	}
  }

public:
  // iCCommandClient / Notificacion para la finalizacion de comandos (fade)
  void EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
					const dword udInstant,
					const dword udExtraParam) { }

public:
  // Operacion de dibujado
  virtual void Draw(void) = 0;
};

#endif // ~ ifndef _CGUIWINDOW_H_