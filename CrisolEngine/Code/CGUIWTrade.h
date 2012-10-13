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
// CGUIWTrade.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUIWTrade
// - CGUIWEntityToPlayer (definida en CGUIWTrade).
// - CGUIWPlayerToEntity (definida en CGUIWTrade).
//
// Descripcion:
// - Clase base para representar el interfaz de intercambio de items entre
//   dos contenedores.
//
// Notas:
// - La ventana de interfaz debera de ser un observer de CWorld para poder
//   conocer si la criatura con la que el jugador esta intercambiando items
//   es destruida, en cuyo caso la interfaz se cerrara de inmediato.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWTRADE_H_
#define _CGUIWTRADE_H_

// Cabecera
#ifndef _CGUIWBASEWINDOWCONTAINER_H_
#include "CGUIWBaseWindowContainer.h"
#endif
#ifndef _CGUICLINETEXT_H_
#include "CGUICLineText.h"
#endif
#ifndef _ICITEMCONTAINER_H_
#include "iCItemContainer.h"
#endif
#ifndef _CGUIWINDOW_H_
#include "CGUIWindow.h"
#endif
#ifndef _ICSCRIPTCLIENT_H_
#include "iCScriptClient.h"
#endif
#ifndef _ICWORLDOBSERVER_H_
#include "iCWorldObserver.h"
#endif

// Definicion de clases / estructuras / espacios de nombres
struct iCGraphicSystem;
class CSprite;
class CCriature;
class CWorldEntity;
class CCBTEngineParser;

// clase CGUIWTrade
class CGUIWTrade: public CGUIWindow,
				  public iCGUIWindowClient,
				  public iCScriptClient,
				  public iCWorldObserver
{
private:
  // Tipos
  typedef CGUIWindow Inherited; // Acceso a la clase base

private:
  // Clase CGUIWEntityToPlayer
  class CGUIWEntityToPlayer: public CGUIWBaseWindowContainer,
	                         public iCItemContainerObserver
  {
  private:
	// Tipos
	typedef CGUIWBaseWindowContainer Inherited; // Acceso a la clase base

  private:  
	// Enumerados
	enum {	
	  ID_PORTRAITNAME = Inherited::ID_DOWNSCROLL + 1	
	};

  private:
	// Estructuras  
	struct sPortraitInfo {
	  // Info asociada al retrato
	  sPosition     PortraitDrawPos; // Pos de dibujado del retrato
	  CSprite*      pPortrait;       // Retrato
	  CGUICLineText Name;            // Nombre de la entidad asociada al retrato
	};

  private:
	// Interfaces a subsistemas
	iCGraphicSystem* m_pGraphSys; // Subsistema grafico

	// Info asociada a los componentes
	sPortraitInfo m_PortraitInfo;
	
  public:
	// Constructor / destructor  
	CGUIWEntityToPlayer(void): m_pGraphSys(NULL) { }
	~CGUIWEntityToPlayer(void) { End(); }

  public:
	// Protocolo de inicializacion / finalizacion
	bool Init(void);
	void End(void);
  private:
	// Metodos de apoyo
	bool TLoad(void);
	bool TLoadPortraitInfo(CCBTEngineParser* const pParser);
	void EndComponents(void); 

  public:
	// CGUIWindow / Obtencion del tipo de ventana
	inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	  ASSERT(Inherited::IsInitOk());
	  // Se retorna el tipo de interfaz
	  return GUIManagerDefs::GUIW_ENTITYTOPLAYERTRADE;
	}

  public:
	// CGUIWindow / Activacion y desactivacion de la ventana de interfaz
	void Active(CWorldEntity* const pEntity,
				iCGUIWindowClient* const pClient);  
	void Deactive(void);
  private:
	// Metodos de apoyo
	void ActiveComponents(const bool bActive);

  public:
  // Activacion / desactivacion de la entrada
  void SetInput(const bool bActive);

  protected:
	// Notificacion de que un item ha sido cliqueado
	void OnItemLeftClick(const AreaDefs::EntHandle hItem,
					     CGUICSpriteSelector& Selector) {
	  ASSERT(Inherited::IsInitOk());
	  // Recibe la notificacion de que un item ha sido cliqueado. En este caso,
	  // se notificara a la clase coordinadora, que es cliente de esta, sobre
	  // el suceso.
	  Inherited::GUIWindowNotify(hItem);
	}
	void OnItemRightClick(const AreaDefs::EntHandle hItem,
						  CGUICSpriteSelector& Selector);

  public:
  // iCGUIWItemContainerObserver / Operacion de notificacion 
  void ItemContainerNotify(const ItemContainerDefs::eObserverNotifyType& NotifyType,
						   const AreaDefs::EntHandle& hItem);

  public:
	// Operacion de dibujado
	void Draw(void);  
  }; // ~ CGUIWEntityToPlayer

private:
  // Clase CGUIWPlayerToEntity
  class CGUIWPlayerToEntity: public CGUIWBaseWindowContainer,
		                     public iCItemContainerObserver
  {
  private:
	// Tipos
	typedef CGUIWBaseWindowContainer Inherited; // Acceso a la clase base

  private:  
	// Enumerados
	enum {	
	  ID_CANCEL = Inherited::ID_DOWNSCROLL + 1	
	};

  private:
	// Estructuras  
	struct sCancelInfo {
	  // Info asociada al retrato	
	  CGUICButton Cancel; // Boton de cancelacion
	};

  private:
	// Info asociada a los componentes
	sCancelInfo m_CancelInfo; // Info asociada a la cancelacion

  public:
	// Constructor / destructor  
	CGUIWPlayerToEntity(void) { }
	~CGUIWPlayerToEntity(void) { End(); }

  public:
	// Protocolo de inicializacion / finalizacion
	bool Init(void);
	void End(void);
  private:
	// Metodos de apoyo
	bool TLoad(void);
	bool TLoadCancelInfo(CCBTEngineParser* const pParser);
	void EndComponents(void); 

  public:
	// CGUIWindow / Obtencion del tipo de ventana
	inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	  ASSERT(Inherited::IsInitOk());
	  // Se retorna el tipo de interfaz
	  return GUIManagerDefs::GUIW_PLAYERTOENTITYTRADE;
	}

  public:
	// CGUIWindow / Activacion y desactivacion de la ventana de interfaz
	void Active(CCriature* const pMainEntity,
				iCGUIWindowClient* const pClient);  
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
  private:
	// Metodos de apoyo
	void OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
				  const bool bSelect);
	void OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent);
  protected:
	// Notificacion de que un item ha sido cliqueado
	void OnItemLeftClick(const AreaDefs::EntHandle hItem,
					     CGUICSpriteSelector& Selector) {
	  ASSERT(Inherited::IsInitOk());
	  // Recibe la notificacion de que un item ha sido cliqueado. En este caso,
	  // se notificara a la clase coordinadora, que es cliente de esta, sobre
	  // el suceso.
	  Inherited::GUIWindowNotify(hItem);
	}
	void OnItemRightClick(const AreaDefs::EntHandle hItem,
						  CGUICSpriteSelector& Selector);

  public:
	// iCGUIWItemContainerObserver / Operacion de notificacion 
	void ItemContainerNotify(const ItemContainerDefs::eObserverNotifyType& NotifyType,
							 const AreaDefs::EntHandle& hItem);

  public:
	// Operacion de dibujado
	void Draw(void);
  }; // ~ CGUWPlayerToEntity

private:
  // Ventanas de interfaz para controlar el intercambio
  CGUIWEntityToPlayer m_EntityToPlayer; // Contenido de la entidad
  CGUIWPlayerToEntity m_PlayerToEntity; // Contentido del jugador

  // Instancias a las entidades
  CCriature*    m_pMainCriature; // Criatura principal que inicia el intercambio
  CWorldEntity* m_pEntity;       // Entitad que recibe la accion

public:
  // Constructor / destructor
  CGUIWTrade(void): m_pMainCriature(NULL),
					m_pEntity(NULL) { }
  ~CGUIWTrade(void) { End(); }

public:
  // Protocolos de inicio y fin
  bool Init(void);
  void End(void);

public:
  // CGUIWindow / Obtencion del tipo de ventana
  inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
    ASSERT(Inherited::IsInitOk());
    // Se retorna el tipo de interfaz
    return GUIManagerDefs::GUIW_TRADE;
  }

public:
  // Activacion / desactivacion
  void Active(CCriature* const pMainCriature,
	          CWorldEntity* const pEntity,			  
			  iCGUIWindowClient* const pClient);
  void Deactive(void);

public:
  // Activacion / desactivacion de la entrada
  void SetInput(const bool bActive) {
	ASSERT(Inherited::IsInitOk());
	// Se traslada la responsabilidad a las intefaces
	m_EntityToPlayer.SetInput(bActive);
	m_PlayerToEntity.SetInput(bActive);
	// Se propaga
	Inherited::SetInput(bActive);
  }

public:
  // iCGUIWindow / Notificacion de eventos
  void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent) { }

public:
  // iCGUIWindowClient
  void GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
					   const dword udParams);

public:
  // Dibujado
  void Draw(void);

public:
  // iCScriptClient / Operacion de notificacion, para cuando acabe un comando
  void ScriptNotify(const RulesDefs::eScriptEvents& ScriptEvent,
					const ScriptClientDefs::eScriptNotify& Notify,
					const dword udParams);  

public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);
}; // ~ CGUIWTrade

#endif // ~ ifndef CGUIWTrade