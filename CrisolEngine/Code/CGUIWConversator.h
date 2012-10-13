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
// CGUIWConversator.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUIWConversator 
//
// Descripcion:
// - Ventana que se activara durante una conversacion entre el jugador y otra
//   entidad. Heredara de CGUIWBaseTextSelector, por lo que esta ultima se
//   encargara de gestionar todas las opciones de texto que se puedan
//   activar.
// - El proceso de funcionamiento sera el siguiente:
//   * Activacion
//   * Asociacion de opciones
//   * Pregunta al jugador por la opcion que este desee
//   Los pasos anteriores es repetiran hasta que se decida desactivar la
//   interfaz. 
//
// Notas:
// - La interfaz sera observer de CWorld para poder conocer si la entidad
//   con la que esta dialogando el jugador es eliminada. En tal caso, el 
//   cierre de la interfaz sera inmediato. 
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWCONVERSATOR_H_
#define _CGUIWCONVERSATOR_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabecera
#ifndef _ICWORLDOBSERVER_H_
#include "iCWorldObserver.h"
#endif
#ifndef _CGUICBITMAP_H_
#include "CGUICBitmap.h"
#endif
#ifndef _CGUICBUTTON_H_
#include "CGUICButton.h"
#endif
#ifndef _CGUICLINETEXT_H_
#include "CGUICLineText.h"
#endif
#ifndef _CGUIWBASETEXTSELECTOR_H_
#include "CGUIWBaseTextSelector.h"
#endif

// Definicion de clases / estructuras / espacios de nombres
class CCBTEngineParser;
class CWorldEntity;
struct iCGraphicSystem;

// clase CGUIWConversator
class CGUIWConversator: public CGUIWBaseTextSelector,
						public iCWorldObserver
{
private:
  // Tipos
  typedef CGUIWBaseTextSelector Inherited; // Acceso a la clase base

private:  
  // Enumerados
  enum {
	// Identificadores para los componentes
	ID_BACKIMG = Inherited::MAX_IDOPTION_VALUE + 1,	
	ID_UPSCROLL,
	ID_DOWNSCROLL,
	ID_NAMECONVERSATOR,
  };

private:
  // Estructuras
  struct sBackImgInfo {
	// Info asociada a la imagen de fondo
	CGUICBitmap BackImg; // Imagen de fondo
  };

  struct sScrollInfo {
	// Info asociada a los botones de scroll
	CGUICButton UpButton;   // Bajar
	CGUICButton DownButton; // Subir
  };  
  
  struct sPortraitInfo {
	// Info asociada a un retrato
	sPosition     DrawPos; // Posicion de dibujado del retrato
	CWorldEntity* pEntity; // Direccion del retrato 
	CGUICLineText Name;    // Nombre
  };

private:
  // Interfaces a otros subsistemas
  iCGraphicSystem* m_pGraphSys; // Subsistema grafico

  // Info asociada a los componentes
  sBackImgInfo  m_BackImg;       // Imagen de fondo
  sScrollInfo   m_ScrollInfo;    // Info asociada a los botones
  sPortraitInfo m_MainConv;      // Info asociada al conversante principal
  sPortraitInfo m_SecondaryConv; // Info asociada al conversante secundario

public:
  // Constructor / destructor  
  CGUIWConversator(void): m_pGraphSys(NULL) { }
  ~CGUIWConversator(void) { End(); }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(void);
  void End(void);
private:
  // Metodos de apoyo
  bool TLoad(void);
  bool TLoadBackImgInfo(CCBTEngineParser* const pParser);  
  bool TLoadScrollInfo(CCBTEngineParser* const pParser);
  bool TLoadPortraitInfo(CCBTEngineParser* const pParser);
  void EndComponents(void);

public:
  // CGUIWindow / Obtencion del tipo de ventana
  inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el tipo de interfaz
	return GUIManagerDefs::GUIW_CONVERSATOR;
  }
  
public:
  // CGUIWindow / Activacion y desactivacion de la ventana de interfaz
  void Active(CWorldEntity* const pMainConv,
			  CWorldEntity* const pSecondaryConv);  
  void Deactive(void);
private:
  // Metodos de apoyo
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

public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);

public:
  // iCGUIWindow / Manejo de los eventos de entrada  
  bool DispatchEvent(const InputDefs::sInputEvent& aInputEvent);
private:
  void InstallClient(void);
  void UnistallClient(void);

public:
  // Operacion de dibujado
  void Draw(void);
};

#endif // ~ ifndef CGUIWConversator