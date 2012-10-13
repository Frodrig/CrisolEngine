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
// CGUIWItemSelectToUse.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWItemSelectToUse.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWItemSelectToUse.h"

#include "SYSEngine.h"
#include "iCLogger.h"
#include "iCInputManager.h"
#include "iCGUIManager.h"
#include "iCGameDataBase.h"
#include "iCWorld.h"
#include "CItem.h"
#include "CCriature.h"
#include "CWorldEntity.h"
#include "CCBTEngineParser.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia.
// Parametros:
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
// - No se permitira reinicializar
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWItemSelectToUse::Init(void)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("CGUIWItemSelectToUse::Init> Inicializando interfaz de selección de item para uso en entidad\n");
  #endif

  // Inicializa clase base
  if (Inherited::Init()) {
	// Carga e inicializa componentes
	if (TLoad()) {
	  // Ajusta resto de vbles de miembro
	  m_EntitiesInfo.pMainEntity = NULL;
	  m_EntitiesInfo.pDestEntity = NULL;

	  // Todo correcto
	  #ifdef ENGINE_TRACE
		SYSEngine::GetLogger()->Write("                          | Ok.\n");
	  #endif
	  return true;
	}
  }

  // Hubo problemas
  End();
  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("                          | Error.\n");
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
CGUIWItemSelectToUse::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CGUIWItemSelectToUse::End> Finalizando interfaz de selección de item para uso en entidad\n");
	#endif

	// Desactiva
	Deactive();

	// Finaliza componentes
	EndComponents();
	
	// Propaga
	Inherited::End();
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("                         | Ok.\n");
	#endif
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la carga e inicializacion de componentes
// Parametros:
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWItemSelectToUse::TLoad(void)
{
  // Obtiene el parser
  CCBTEngineParser* const pParser = SYSEngine::GetGameDataBase()->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
																		       "[InventorySelector]");
  ASSERT(pParser);

  // Carga datos para clases base
  pParser->SetVarPrefix("");
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
// - Carga info asociada al boton de cancelacion.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWItemSelectToUse::TLoadCancelInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Vbles
  CGUICButton::sGUICButtonInitData CfgButton; // Cfg del boton

  // Prefijo
  pParser->SetVarPrefix("Options.");

  // Se cargan datos
  CfgButton.szATButton = pParser->ReadString("CancelButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("CancelButton.");

  // Se completan datos
  CfgButton.ID = CGUIWItemSelectToUse::ID_CANCEL;
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
CGUIWItemSelectToUse::EndComponents(void)
{
  // Finaliza componentes
  // Boton de cancelacion
  m_CancelInfo.Cancel.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la operacion de activacion de la interfaz.
// Parametros:
// - pMainEntity. Criatura (jugador) que inicia la accion
// - pDestEntity. Entidad que recibira la accion de usar el item seleccionado.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWItemSelectToUse::Active(CCriature* const pMainEntity,
							 CWorldEntity* const pDestEntity)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pMainEntity);
  ASSERT(pDestEntity);
  
  // Activa si procede
  if (!Inherited::IsActive()) {
	// Almacena instancias
	m_EntitiesInfo.pMainEntity = pMainEntity;
	m_EntitiesInfo.pDestEntity = pDestEntity;

	// Activa componentes
	ActiveComponents(true);

	// Se cambia a cursor de ventana, quitando cualquier tipo de texto flotante
	iCGUIManager* const pGUIManager = SYSEngine::GetGUIManager();
	ASSERT(pGUIManager);	
	pGUIManager->SetGUICursor(GUIManagerDefs::WINDOWINTERFAZ_CURSOR);
	pGUIManager->UnsetFloatingText();	
	
	// Se instala como observer de CWorld
	SYSEngine::GetWorld()->AddObserver(this);

	// Activa seleccion en entidad
	pDestEntity->SetFadeSelect(true);
	
	// Propaga la llamada
	ASSERT(pMainEntity->GetItemContainer());
	Inherited::Active(pMainEntity->GetItemContainer(), NULL);
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
CGUIWItemSelectToUse::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Desactiva si procede
  if (Inherited::IsActive()) {
	// Desactiva componentes
	ActiveComponents(false);

	// Se desinstala como observer de CWorld
	SYSEngine::GetWorld()->RemoveObserver(this);

	// Activa seleccion en entidad
	m_EntitiesInfo.pDestEntity->SetFadeSelect(false);	

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
CGUIWItemSelectToUse::ActiveComponents(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Activa / desactiva componentes
  // Boton de cancelacion
  m_CancelInfo.Cancel.SetActive(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion de un evento sobre un componente.
// - Se debera de tener en cuenta que si el identificador asociado al
//   evento es menor que el primero de los identificadores para componentes
//   locales, se debera de trasladar la responsabilidad de la llamada a la
//   clase base.
// Parametros:
// - GUICEvent. Evento que se ha producido.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWItemSelectToUse::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿NO pertence el evento a esta instancia?
  if (GUICEvent.IDComponent < CGUIWItemSelectToUse::ID_CANCEL) {
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
// - Logica asociada a la seleccion / deseleccion de un componente
// Parametros:
// - IDComponent. Identificador del componente.
// - bSelect. Flag de seleccion / deseleccion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWItemSelectToUse::OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
							   const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(IDComponent);

  // Se actua sobre el componente que proceda
  switch(IDComponent) {
	case CGUIWItemSelectToUse::ID_CANCEL: {
	  // Boton de cancelacion
	  m_CancelInfo.Cancel.Select(bSelect);
	} break;	
  }; // ~ switch  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada al cliqueo con el boton izq. de un componente
// Parametros:
// - IDComponent. Identificador del componente.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWItemSelectToUse::OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(IDComponent);

  // Se actua sobre el componente que proceda
  switch(IDComponent) {
	case CGUIWItemSelectToUse::ID_CANCEL: {
	  // Se ha pulsado sobre el boton de cancelar
	  // Se cierra la interfaz
	  SYSEngine::GetGUIManager()->SetMainInterfazWindow();
	} break;	
  }; // ~ switch  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se ha escogido el item con handle hItem para usar sobre la entidad
//   destino. 
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWItemSelectToUse::OnItemLeftClick(const AreaDefs::EntHandle hItem,
									  CGUICSpriteSelector& Selector)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(hItem);

  // Se ha pulsado sobre un item, en caso de que dicho item pueda
  // usarse, se producira un evento de uso entre el item seleccionado y la
  // entidad destino
  const CItem* const pItem = SYSEngine::GetWorld()->GetItem(hItem);  
  ASSERT(pItem);
  m_EntitiesInfo.pMainEntity->UseItem(hItem, m_EntitiesInfo.pDestEntity->GetHandle());
  
  // Se cierra la interfaz
  SYSEngine::GetGUIManager()->SetMainInterfazWindow();  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dibuja componentes
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWItemSelectToUse::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Dibuja si procede
  if (Inherited::IsActive()) {
	// Clase base
	Inherited::Draw();

	// Boton de cancelacion
	m_CancelInfo.Cancel.Draw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Captura la pulsacion de la tecla ESC para cancelacion rapida de la
//   interfaz.
// Parametros:
// - InputEvent. Evento.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWItemSelectToUse::DispatchEvent(const InputDefs::sInputEvent& InputEvent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Evalua el evento producido 
  switch(InputEvent.EventType) {
	case InputDefs::BUTTON_EVENT: {
	  // Pulsacion de tecla
	  switch(InputEvent.EventButton.Code) {
		case InputDefs::IE_KEY_ESC: {
		  // Se simula la pulsacion del boton de salir
		  OnSelect(CGUIWItemSelectToUse::ID_CANCEL, true);
		  OnLeftClick(CGUIWItemSelectToUse::ID_CANCEL);
		  return InputDefs::NO_SHARE_INPUT_EVENT;
		} break;
	  }; // ~ switch
	} break;	  
  }; // ~ switch

  // No se capturo evento / Se comparte
  return InputDefs::SHARE_INPUT_EVENT;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se instala como cliente para recibir eventos sobre el global de la
//   ventana de interfaz. En concreto:
//   * Pulsacion ESC -> Simulara pulsar sobre el boton de cancelar.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWItemSelectToUse::InstallClient(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se instala como cliente
  SYSEngine::GetInputManager()->SetInputEvent(InputDefs::IE_KEY_ESC, 
											  this, 
											  InputDefs::BUTTON_PRESSED_DOWN);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se desinstala como cliente para eventos de entrada.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWItemSelectToUse::UnistallClient(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se instala como cliente
  SYSEngine::GetInputManager()->UnsetInputEvent(InputDefs::IE_KEY_ESC, 
												this, 
											    InputDefs::BUTTON_PRESSED_DOWN);
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
CGUIWItemSelectToUse::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
										  const dword udParam)
{
  // SOLO si instancia inicializada y activada
  ASSERT(Inherited::IsInitOk());
  ASSERT(Inherited::IsActive());

  // Comprueba el tipo de notificacion
  switch(NotifyType) {
	case WorldObserverDefs::ENTITY_DESTROY: {
	  // ¿Es la entidad destruida la asociada?
	  if (udParam == m_EntitiesInfo.pDestEntity->GetHandle()) {
		// Si, se cierra de inmediato la interfaz, simulando la 
		// pulsacion del boton ESC
		OnSelect(CGUIWItemSelectToUse::ID_CANCEL, true);
		OnLeftClick(CGUIWItemSelectToUse::ID_CANCEL);
	  }
	} break;
  }; // ~ switch
}