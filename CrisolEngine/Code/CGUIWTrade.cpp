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
// CGUIWTrade.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWTrade.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWTrade.h"

#include "SYSEngine.h"
#include "iCGraphicSystem.h"
#include "iCGameDataBase.h"
#include "iCGUIManager.h"
#include "iCLogger.h"
#include "iCWorld.h"
#include "iCVirtualMachine.h"
#include "CCBTEngineParser.h"
#include "CCriature.h"
#include "CWorldEntity.h"
#include "CSprite.h"
#include "CPlayer.h"
#include "CItem.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar.
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWTrade::Init(void)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("CGUIWTrade::Init> Inicializando interfaz de intercambio de items entre jugador y un contenedor.\n");
  #endif

  // Inicializa clase base
  if (Inherited::Init()) {
	// Se inicializa interfaz de intercambio entre entidad y jugador
	if (m_EntityToPlayer.Init()) {
	  // Se inicializa interfaz de intercambio entre jugador y entidad
	  if (m_PlayerToEntity.Init()) {
		// Se inicializan resto de vbles
		m_pMainCriature = NULL;
		m_pEntity = NULL;

		// Todo correcto
		#ifdef ENGINE_TRACE
		  SYSEngine::GetLogger()->Write("                | Ok.\n");
		#endif
		return true;	
	  }
	}	
  }

  // Hubo problemas
  End();
  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("                | Error.\n");
  #endif
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CGUIWTrade::End> Finalizando interfaz de intercambio de items entre jugador y un contenedor.\n");
	#endif

	// Desactiva
	Deactive();

	// Finaliza ventanas de interfaz
	m_EntityToPlayer.End();
	m_PlayerToEntity.End();

	// Finaliza posibles scripts asociados
	SYSEngine::GetVirtualMachine()->ReleaseScripts(this);

	// Propaga
	Inherited::End();
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("               | Ok.\n");
	#endif  
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa la interfaz.
// Parametros:
// - pMainCriature. Criatura principal, la que inicia el intercambio.
// - pEntity. Criatura secundaria, sobre la que se inicia el intercambio.
// - pClient. Posible cliente asociado al cierre de las ventanas de
//   intercambio.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::Active(CCriature* const pMainCriature,
				   CWorldEntity* const pEntity,				   
				   iCGUIWindowClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(pMainCriature);
  ASSERT(pEntity);

  // Se activa si procede
  // Nota: Las entidades deberan de ser distintas y la entidad destino
  // debera de ser un contenedor
  if (!Inherited::IsActive() &&
	  pEntity->GetHandle() != pMainCriature->GetHandle() &&
	  pEntity->GetItemContainer()) {
	// Se establece cursor de la ventana, SERA MUY IMPORTANTE para poder
	// establecer luego los textos flotantes
	SYSEngine::GetGUIManager()->SetGUICursor(GUIManagerDefs::WINDOWINTERFAZ_CURSOR);
	
	// Se activa interfaz asociado a criatura principal
	m_EntityToPlayer.Active(pEntity, this);
	ASSERT(m_EntityToPlayer.IsActive());
	m_PlayerToEntity.Active(pMainCriature, this);
	ASSERT(m_PlayerToEntity.IsActive());	

	// Se instalan las interfaces como observadoras
	// -> Nota: ¿Porque no trasladar esto a las clases interesadas? <-
	pMainCriature->GetItemContainer()->AddObserver(&m_PlayerToEntity);
	pEntity->GetItemContainer()->AddObserver(&m_EntityToPlayer);

	// Se guardan las instancias a las entidades
	m_pMainCriature = pMainCriature;
	m_pEntity = pEntity;

	// Se instala como observer de CWorld
	SYSEngine::GetWorld()->AddObserver(this);
	
	// Se propaga
	Inherited::Active(pClient);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva interfaz
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Desactiva si procede
  if (Inherited::IsActive()) {
	// Desactiva interfaces
	m_PlayerToEntity.Deactive();
	m_EntityToPlayer.Deactive();

	// Se desinstalan las interfaces como observadoras
	m_pMainCriature->GetItemContainer()->RemoveObserver(&m_PlayerToEntity);
	m_pEntity->GetItemContainer()->RemoveObserver(&m_EntityToPlayer);

	// Se desinstala como observer de CWorld
	SYSEngine::GetWorld()->RemoveObserver(this);

	// Desvincula instancias actuales
	m_pMainCriature = NULL;
	m_pEntity = NULL;

	// Propaga
	Inherited::Deactive();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe la notificacion de alguna de las dos ventanas de interfaz y 
//   realiza las acciones pertinentes, que no sera otra que la de comunicar
//   al sistema script un evento que sera:
//   * Si IDGUIWindow es la ventana de la entidad -> Coger
//   * Si IDGUIWindow es la ventana de la criatura principal (jugador) -> Soltar.
// - En udParams siempre vendra el handle al hItem salvo que todo el sea 0 que
//   significara que lo que se ha pulsado ha sido a la opcion de salir en la
//   ventana de la criatura principal
// Parametros:
// - IDGUIWindow. Identificador de la ventana desde la que se recibe la 
//   notificacion
// - udParams. Parametros
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
							const dword udParams)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Se comprueba de que ventana se recibe el evento
  switch(IDGUIWindow) {
	case GUIManagerDefs::GUIW_ENTITYTOPLAYERTRADE: {
	  // Se comunica al script un evento de TOMAR por parte del jugador, un
	  // item de la otra entidad contenedora
	  CItem* const pItem = SYSEngine::GetWorld()->GetItem(udParams);
	  ASSERT(pItem);
	  SYSEngine::GetVirtualMachine()->OnTradeItem(this,
												  pItem->GetScriptEvent(RulesDefs::SE_ONTRADEITEM),
		                                          pItem->GetHandle(),
												  m_pEntity->GetHandle(),
												  m_pMainCriature->GetHandle());
	} break;

	case GUIManagerDefs::GUIW_PLAYERTOENTITYTRADE: {
	  // ¿Se ha pulsado sobre la opcion de cancelar / salir?
	  if (!udParams) {
		// Si, se pasa al estado de interfaz principal, notificando antes
		Inherited::GUIWindowNotify(0);	  
		SYSEngine::GetGUIManager()->SetMainInterfazWindow();
	  } else {
		// Se comunica al script un evento de PASAR un item desde la
		// criatura jugador a la otra entidad
		CItem* const pItem = SYSEngine::GetWorld()->GetItem(udParams);
		ASSERT(pItem);	  
		SYSEngine::GetVirtualMachine()->OnTradeItem(this,
		                                            pItem->GetScriptEvent(RulesDefs::SE_ONTRADEITEM),
													pItem->GetHandle(),
												    m_pMainCriature->GetHandle(),
												    m_pEntity->GetHandle());
	  }
	} break;	
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la notificacion de un suceso en CWorld.
// - Seran de interes las notificaciones:
//    * Destruccion de una entidad.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
							    const dword udParam)
{
  // SOLO si instancia inicializada y activada
  ASSERT(Inherited::IsInitOk());
  ASSERT(Inherited::IsActive());

  // Comprueba el tipo de notificacion
  switch(NotifyType) {
	case WorldObserverDefs::ENTITY_DESTROY: {
	  // ¿Es la entidad destruida la asociada?
	  if (udParam == m_pEntity->GetHandle()) {
		// Si, se pasa al estado de interfaz principal, notificando antes
		Inherited::GUIWindowNotify(0);	  
		SYSEngine::GetGUIManager()->SetMainInterfazWindow();
	  }
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dibuja componentes
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWTrade::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Dibuja interfaces si procede
  if (Inherited::IsActive()) {	
    m_EntityToPlayer.Draw();
    m_PlayerToEntity.Draw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion de la accion de intercambio de items
// Parametros:
// - Notify. Tipo de notificacion.
// - ScriptEvent. Evento que lo lanza.
// - udParams. Parametros asociados.
// Devuelve:
// Notas:
// - El formato del parametro sera:
//   * WORD superior el handle al item
//   * WORD inferior:
//       - Primer byte, si 0 de Player a Entity y si 1 de Entity a Player.
//       - Segundo byte, si != 0 accion por defecto y viceversa
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::ScriptNotify(const RulesDefs::eScriptEvents& ScriptEvent,
						 const ScriptClientDefs::eScriptNotify& Notify,
						 const dword udParams)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  switch (ScriptEvent) {
	case RulesDefs::SE_ONTRADEITEM: {
  	  // ¿NO hubo ERRORES ejecutando el script?
	  if (ScriptClientDefs::SN_SCRIPT_ERROR != Notify) {
		// ¿Se puede realizar la accion por defecto?
		if ((udParams & 0x000000FF) > 0) {
		  // ¿Es un intercambio desde la ventana de jugador a otra?
		  const AreaDefs::EntHandle hItem = ((udParams & 0xFFFF0000) >> 16);
		  if ((udParams & 0x0000FF00) == 0) {
			// Si, el jugador perdera una entidad
			m_pMainCriature->GetItemContainer()->Extract(hItem);
			m_pEntity->GetItemContainer()->Insert(hItem);
		  } else {
			// No, el jugador ganara una entidad
			m_pEntity->GetItemContainer()->Extract(hItem);
			m_pMainCriature->GetItemContainer()->Insert(hItem);	
		  }
		}
	  }
	} break;	

	default:
	  ASSERT(false);
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia
// Parametros:
// Devuelve:
// - Si todo ha sido correcto true. En caso contrario false.
// Notas:
// - No se permitira reinicializar
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWTrade::CGUIWEntityToPlayer::Init(void)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  // Se inicializa clase base
  if (Inherited::Init()) {
	// Se toman interfaces a otros subsistemas
	m_pGraphSys = SYSEngine::GetGraphicSystem();
	ASSERT(m_pGraphSys);

	// Se cargan datos
	if (TLoad()) {	  	  
	  // Todo correcto
	  return true;
	}
  }

  // Hubo problemas
  End();
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWEntityToPlayer::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	// Desactiva
	Deactive();

	// Finaliza componentes
	EndComponents();

	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la carga de datos y la incializacion de componentes.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWTrade::CGUIWEntityToPlayer::TLoad(void)
{
  // Obtiene el parser
  CCBTEngineParser* const pParser = SYSEngine::GetGameDataBase()->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
																		       "[TradeItemsManager]");
  ASSERT(pParser);

  // Carga datos para clases base
  pParser->SetVarPrefix("RestWindow.");
  if (Inherited::TLoad(pParser, 2, true)) {
	// Procede a cargar datos de componentes locales
	// Retrato
	if (!TLoadPortraitInfo(pParser)) {
	  return false;
	}

	// Todo correcto
	return true;
  }

  // Hubo problemas
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga informacion relativa al retrato
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - Los valores correctos sobre el retrato no se estableceran hasta la
//   activacion
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWTrade::CGUIWEntityToPlayer::TLoadPortraitInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Vbles
  CGUICLineText::sGUICLineTextInitData CfgLine; // Cfg de la linea
  
  // Se cargan datos
  CfgLine.Position = pParser->ReadPosition("Portrait.Name.");
  CfgLine.RGBUnselectColor = pParser->ReadRGBColor("Portrait.Name.");
  CfgLine.RGBSelectColor = CfgLine.RGBUnselectColor;

  // Se completan datos y se inicializa componente
  CfgLine.szFont = "Arial";
  CfgLine.szLine = "-";
  CfgLine.ID = CGUIWEntityToPlayer::ID_PORTRAITNAME;
  CfgLine.ubDrawPlane = 2;
  if (!m_PortraitInfo.Name.Init(CfgLine)) {
	return false;
  }
  
  // Pos. de dibujado del retrato
  m_PortraitInfo.PortraitDrawPos = pParser->ReadPosition("Portrait.");

  // La instancia al sprite con el retrato no se establecera hasta activacion  
  m_PortraitInfo.pPortrait = NULL;

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza componentes
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWEntityToPlayer::EndComponents(void)
{
  // Finaliza componentes
  // Nombre y desvinculacion del retrato
  m_PortraitInfo.Name.End();
  m_PortraitInfo.pPortrait = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa instancia
// Parametros:
// - pEntity. Entidad con la que trabajar
// - pClient. Cliente de la ventana interfaz. Necesariamente distinto de NULL.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWEntityToPlayer::Active(CWorldEntity* const pEntity,
										iCGUIWindowClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pEntity);
  ASSERT(pClient);

  // Activa si procede
  if (!Inherited::IsActive()) {
	// Establece el nombre de la entidad y retrato
	m_PortraitInfo.Name.ChangeText(pEntity->GetName());
	m_PortraitInfo.pPortrait = pEntity->GetPortrait();

	// Activa componentes
	ActiveComponents(true);

	// Propaga
	ASSERT(pEntity->GetItemContainer());
	Inherited::Active(pEntity->GetItemContainer(), pClient);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWEntityToPlayer::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Desactiva si procede
  if (Inherited::IsActive()) {
	// Desactiva componentes
	ActiveComponents(false);

	// Desasocia retrato actual
	m_PortraitInfo.pPortrait = NULL;

	// Propaga
	Inherited::Deactive();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa / desactiva componentes
// Parametros:
// - bActive. Flag de activacion / desactivacion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWEntityToPlayer::ActiveComponents(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Desactiva componentes
  // Nombre retrato
  m_PortraitInfo.Name.SetActive(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa / desactiva la entrada
// Parametros:
// - bActive. Flag de activacion / desactivacion de la entrada.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWEntityToPlayer::SetInput(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se activa / desactiva entrada
  // Nombre retrato
  m_PortraitInfo.Name.SetInput(bActive);

  // Propaga
  Inherited::SetInput(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion enviada por el contenedor al que esta clase es observador
//   de que un item ha podido ser extraido / insertado.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - hItem. Handle del iteem que interviene en el suceso
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWEntityToPlayer::ItemContainerNotify(const ItemContainerDefs::eObserverNotifyType& NotifyType,
													 const AreaDefs::EntHandle& hItem)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos 
  ASSERT(hItem);

  // Se comprueba el tipo de notificacion y segun sea este se realizara
  // una notificacion a la clase base para que actualice su logica interna
  switch(NotifyType) {
	case ItemContainerDefs::ITEM_INSERTED: {
	  Inherited::ItemInserted(hItem);
	} break;

	case ItemContainerDefs::ITEM_REMOVED: {
	  Inherited::ItemRemoved(hItem);
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este metodo se llamara cuando la clase base detecte que se ha pulsado
//   sobre un selector de items con boton dcho. Cuando esto ocurra, se
//   activara el interfaz de interaccion con entidades para que se pueda
//   actuar sobre dicho item. Como el item esta en el interfaz asociado al
//   contenedor que estamos examinando, la unica accion que se llevara a
//   cabo sera la de observar. De esto se encargara la propia interfaz
//   EntityInteract.
// Parametros:
// - hItem. Item sobre el que se ha pulsado o bien 0 indicando que se ha
//   pulsado en una ranura libre de inventario
// - Selector. Selector al que esta asociado hItem
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWEntityToPlayer::OnItemRightClick(const AreaDefs::EntHandle hItem,
												  CGUICSpriteSelector& Selector)
{
  // SOLO si intancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(hItem);

  // Se quita la seleccion del slot, simulando la deseleccion
  // Nota: se debe de hacer antes de activar el interfaz de interaccion
  // con items, porque este bloqueara la entrada para el interfaz de inventario
  const GUIDefs::sGUICEvent Event(GUIDefs::GUIC_UNSELECT, Selector.GetID(), 0);
  ComponentNotify(Event);

  // Se ordena al GUIManager que active la interfaz de interaccion
  const sPosition InitDrawPos(Selector.GetXPos() + 64, Selector.GetYPos());
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  SYSEngine::GetGUIManager()->SetEntityInteractWindow(InitDrawPos,
  												      pWorld->GetPlayer(),
													  pWorld->GetWorldEntity(hItem));	  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la operacion de dibujado
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWEntityToPlayer::Draw(void)
{
  // SOLO si intancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Dibuja si procede
  if (Inherited::IsActive()) {
	// Dibuja elementos clase base
	Inherited::Draw();

	// Dibuja retrato y nombre
	if (m_PortraitInfo.pPortrait) {
	  m_pGraphSys->Draw(GraphDefs::DZ_GUI,
						2,
						m_PortraitInfo.PortraitDrawPos.swXPos,
						m_PortraitInfo.PortraitDrawPos.swYPos,
						m_PortraitInfo.pPortrait);
	}	
	m_PortraitInfo.Name.Draw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWTrade::CGUIWPlayerToEntity::Init(void)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  // Inicializa clase base
  if (Inherited::Init()) {
	// Carga datos
	if (TLoad()) {
	  // Todo correcto
	  return true;
	}
  }

  // Hubo problemas
  End();
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWPlayerToEntity::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	// Desactiva
	Deactive();

	// Finaliza componentes
	EndComponents();

	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la carga de componentes y su inicializacion
// Parametros:
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWTrade::CGUIWPlayerToEntity::TLoad(void)
{
  // Obtiene el parser
  CCBTEngineParser* const pParser = SYSEngine::GetGameDataBase()->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
																		       "[TradeItemsManager]");
  ASSERT(pParser);

  // Carga datos para clases base
  pParser->SetVarPrefix("PlayerWindow.");
  if (Inherited::TLoad(pParser, 2, true)) {
	// Procede a cargar datos de componentes locales
	// Boton de cancelacion
	if (!TLoadCancelInfo(pParser)) {
	  return false;
	}

	// Todo correcto
	return true;
  }

  // Hubo problemas
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga datos asociados al boton de cancelacion.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWTrade::CGUIWPlayerToEntity::TLoadCancelInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

    // SOLO si parametros correctos
  ASSERT(pParser);

  // Vbles
  CGUICButton::sGUICButtonInitData CfgButton; // Cfg del boton

  // Se cargan datos
  CfgButton.szATButton = pParser->ReadString("Options.CancelButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("Options.CancelButton.");

  // Se completan datos
  CfgButton.ID = CGUIWPlayerToEntity::ID_CANCEL;
  CfgButton.pGUIWindow = this;
  CfgButton.ubDrawPlane = 2;
  CfgButton.GUIEvents = GUIDefs::GUIC_SELECT | 
						GUIDefs::GUIC_UNSELECT |
						GUIDefs::GUIC_BUTTONLEFT_PRESSED;

  // Se inicializa
  if (!m_CancelInfo.Cancel.Init(CfgButton)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza componentes
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWPlayerToEntity::EndComponents(void)
{
  // Finaliza componentes
  // Cancelacion
  m_CancelInfo.Cancel.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa interfaz.
// Parametros:
// - pMainInterfaz. Instancia a la criatura que va a representar esta interfaz.
//   La criatura sera el jugador por norma general.
// - pClient. Cliente que debera de ser obligatoriamente distinto de NULL y 
//   se correspondera con la ventana coordinadora CGUIWTrade.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWPlayerToEntity::Active(CCriature* const pMainEntity,
										iCGUIWindowClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pMainEntity);
  ASSERT(pClient);

  // Activa si procede
  if (!Inherited::IsActive()) {
	// Activa componentes
	ActiveComponents(true);

	// Propaga
	Inherited::Active(pMainEntity->GetItemContainer(), pClient);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWPlayerToEntity::Deactive(void)
{
  // SOLO si intancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Desactiva
  if (Inherited::IsActive()) {
	// Desactiva componentes
	ActiveComponents(false);

	// Propaga
	Inherited::Deactive();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa / desactiva componentes asociados
// Parametros:
// - bActive. Flag de activacion / desactivacion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWPlayerToEntity::ActiveComponents(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Procede a activar / desactivar componentes
  // Boton de cancelacion
  m_CancelInfo.Cancel.SetActive(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa / desactiva la entrada
// Parametros:
// - bActive. Flag de activacion / desactivacion de la entrada.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWPlayerToEntity::SetInput(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se activa / desactiva entrada
  // Boton de cancelacion
  m_CancelInfo.Cancel.SetInput(bActive);

  // Propaga
  Inherited::SetInput(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe notificacion sobre un componente. Debera de tener en cuenta que
//   si el identificador no corresponde con ningun componente, correspondera
//   la responsabilidad a la clase base.
// Parametros:
// - CGUICEvent. Evento recibido
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWPlayerToEntity::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿NO pertence el evento a esta instancia?
  if (GUICEvent.IDComponent < CGUIWPlayerToEntity::ID_CANCEL) {
	Inherited::ComponentNotify(GUICEvent);
  } else {
	// Se comprueba el tipo de evento
		// Comprueba el tipo de evento
	switch(GUICEvent.Event) {
	  case GUIDefs::GUIC_SELECT: {	
		// Seleccion
		OnSelect(GUICEvent.IDComponent, true);	
	  } break;

  	  case GUIDefs::GUIC_UNSELECT: {	
		// Deseleccion
		OnSelect(GUICEvent.IDComponent, false);	  
	  } break;
	  
	  case GUIDefs::GUIC_BUTTONLEFT_PRESSED: {	  	  
		// Pulsacion
		OnLeftClick(GUICEvent.IDComponent);
	  } break;	  
	}; // ~ switch	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Selecciona / deselecciona componente.
// Parametros:
// - IDComponent. Identificador del componente.
// - bSelect. Flag de seleccion / deseleccion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWPlayerToEntity::OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
										  const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(IDComponent);

  // Se actua sobre el componente que proceda
  switch(IDComponent) {
	case CGUIWPlayerToEntity::ID_CANCEL: {
	  // Boton de cancelacion
	  m_CancelInfo.Cancel.Select(bSelect);
	} break;	
  }; // ~ switch  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Evento de pulsacion con el boton izq. sobre un componente.
// Parametros:
// - IDComponent. Identificador del componente
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWPlayerToEntity::OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent)
{
    // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(IDComponent);

  // Se actua sobre el componente que proceda
  switch(IDComponent) {
	case CGUIWPlayerToEntity::ID_CANCEL: {
	  // Al pulsarse sobre el boton de cancelacion, se comunicara
	  // a la ventana cliente (que no sera otra que CGUIWTrade) que se
	  // desea salir de la interfaz de intercambio
	  Inherited::GUIWindowNotify(0);	  
	} break;	
  }; // ~ switch  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion enviada por el contenedor al que esta clase es observador
//   de que un item ha podido ser extraido / insertado.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - hItem. Handle del iteem que interviene en el suceso
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWPlayerToEntity::ItemContainerNotify(const ItemContainerDefs::eObserverNotifyType& NotifyType,
													 const AreaDefs::EntHandle& hItem)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos 
  ASSERT(hItem);

  // Se comprueba el tipo de notificacion y segun sea este se realizara
  // una notificacion a la clase base para que actualice su logica interna
  switch(NotifyType) {
	case ItemContainerDefs::ITEM_INSERTED: {
	  Inherited::ItemInserted(hItem);
	} break;

	case ItemContainerDefs::ITEM_REMOVED: {
	  Inherited::ItemRemoved(hItem);
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este metodo se llamara cuando la clase base detecte que se ha pulsado
//   sobre un selector de items con boton dcho. Cuando esto ocurra, se
//   activara el interfaz de interaccion con entidades para que se pueda
//   actuar sobre dicho item, ya sea para tirarlo del inventario, observarlo
//   o bien usar una habilidad sobre el mismo.
// Parametros:
// - hItem. Item sobre el que se ha pulsado o bien 0 indicando que se ha
//   pulsado en una ranura libre de inventario
// - Selector. Selector al que esta asociado hItem
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWPlayerToEntity::OnItemRightClick(const AreaDefs::EntHandle hItem,
												  CGUICSpriteSelector& Selector)
{
  // SOLO si intancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(hItem);
    
  // Se quita la seleccion del slot, simulando la deseleccion
  // Nota: se debe de hacer antes de activar el interfaz de interaccion
  // con items, porque este bloqueara la entrada para el interfaz de inventario
  const GUIDefs::sGUICEvent Event(GUIDefs::GUIC_UNSELECT, Selector.GetID(), 0);
  ComponentNotify(Event);

  // Se ordena al GUIManager que active la interfaz de interaccion
  const sPosition InitDrawPos(Selector.GetXPos() + 64, Selector.GetYPos());
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  SYSEngine::GetGUIManager()->SetEntityInteractWindow(InitDrawPos,
  												      pWorld->GetPlayer(),
													  pWorld->GetWorldEntity(hItem));	  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la operacion de dibujado
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTrade::CGUIWPlayerToEntity::Draw(void)
{
  // SOLO si intancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Dibuja si procede
  if (Inherited::IsActive()) {
	// Base
	Inherited::Draw();

	// Dibuja boton de cancelacion
	m_CancelInfo.Cancel.Draw();
  }
}

