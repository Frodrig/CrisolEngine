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
// CGUIWConversator.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWConversator.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWConversator.h"

#include "SYSEngine.h"
#include "iCGUIManager.h"
#include "iCLogger.h"
#include "iCGameDataBase.h"
#include "iCGraphicSystem.h"
#include "iCInputManager.h"
#include "iCWorld.h"
#include "CWorldEntity.h"
#include "CCBTEngineParser.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false. 
// Notas:
// - No se permitira reinicializacion.
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWConversator::Init(void)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  #ifdef ENGINE_TRACE
    SYSEngine::GetLogger()->Write("CGUIWConversator::Init> Inicializando ventana para gestión de conversaciones.\n");
  #endif

  // Inicializa la clase base  
  if (Inherited::Init()) {
	// Se toman interfaces a subsistemas
	m_pGraphSys = SYSEngine::GetGraphicSystem();
	ASSERT(m_pGraphSys);

	// Procede a cargar informacion
	if (TLoad()) {
	  // Todo correto
	  #ifdef ENGINE_TRACE
		SYSEngine::GetLogger()->Write("                       | Ok.\n");
	  #endif
	  return true;
	}
  }

  // Hubo problemas
  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("                       | Error.\n");
  #endif
  End();
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWConversator::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	// Desactiva y finaliza componentes
	Deactive();
	EndComponents();

	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la carga y configuracion de componentes
// Parametros:
// Devuelve:
// - Si todo ha ido bien true en caso contrario false
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWConversator::TLoad(void)
{
  // Se obtiene el parser
  CCBTEngineParser* const pParser = SYSEngine::GetGameDataBase()->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
																		       "[ConversationWindow]");
  ASSERT(pParser);

  // Se procede a cargar e inicializar componentes
  // Imagen de fondo
  if (!TLoadBackImgInfo(pParser)) {
	return false;
  }

  // Opciones y scroll
  if (!Inherited::TLoadTextOptionsInfo(pParser, 0)) {
	return false;
  }
  if (!TLoadScrollInfo(pParser)) {
	return false;
  }
  
  // Retratos
  if (!TLoadPortraitInfo(pParser)) {
	return false;
  }

  // Todo correcto
  return true;
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la imagen de fondo.
// Parametros:
// - pParser. Parser a utilizar
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWConversator::TLoadBackImgInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Vbles
  CGUICBitmap::sGUICBitmapInitData CfgBitmap; // Cfg del bitmap

  // Prefijo
  pParser->SetVarPrefix("BackImage.");

  // Se cargan datos
  CfgBitmap.BitmapInfo.szFileName = pParser->ReadString("FileName");
  // Se completan datos  
  CfgBitmap.Position.swXPos = 0;
  CfgBitmap.Position.swYPos = m_pGraphSys->GetVideoHeight() - 128;  
  CfgBitmap.ID = CGUIWConversator::ID_BACKIMG;

  // Se inicializa y se retorna
  m_BackImg.BackImg.Init(CfgBitmap);
  return m_BackImg.BackImg.IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga datos asociados a los retratos.
// Parametros:
// - pParser. Parser a utilizar
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWConversator::TLoadPortraitInfo(CCBTEngineParser* const pParser)
{
  // SOLO si instancia inicializada
  ASSERT(pParser);

  // Prefijo
  pParser->SetVarPrefix("Speakers.");

  // Vbles
  CGUICLineText::sGUICLineTextInitData CfgName; // Cfg para la linea con el nombre

  // Se establecen valores comunes para la configuracion del nombre
  CfgName.ID = CGUIWConversator::ID_NAMECONVERSATOR;
  CfgName.szFont = "Arial";
  CfgName.szLine = "-";

  // Conversador principal
  // Pos del retrato
  m_MainConv.DrawPos = pParser->ReadPosition("Player.Portrait.");
  // Linea con su nombre
  CfgName.RGBUnselectColor = pParser->ReadRGBColor("Player.Name.");
  CfgName.RGBSelectColor = CfgName.RGBUnselectColor; 
  CfgName.Position = pParser->ReadPosition("Player.Name.");
  if (!m_MainConv.Name.Init(CfgName)) {
	return false;
  }

  // Conversador secundario
  // Pos del retrato
  m_SecondaryConv.DrawPos = pParser->ReadPosition("Rest.Portrait.");
  // Linea con su nombre
  CfgName.RGBUnselectColor = pParser->ReadRGBColor("Rest.Name.");
  CfgName.RGBSelectColor = CfgName.RGBUnselectColor; 
  CfgName.Position = pParser->ReadPosition("Rest.Name.");
  if (!m_SecondaryConv.Name.Init(CfgName)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga informacion relativa a los botones de scroll.
// Parametros:
// - pParser. Parser a utilizar
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWConversator::TLoadScrollInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Vbles
  CGUICButton::sGUICButtonInitData CfgButton; // Configuracion para los botones

  // Establece datos de configuracion comunes
  CfgButton.pGUIWindow = this;
  CfgButton.GUIEvents = GUIDefs::GUIC_SELECT | 
						GUIDefs::GUIC_UNSELECT |
						GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  CfgButton.ubDrawPlane = 2;

  // Prefijo
  pParser->SetVarPrefix("Options.");

  // Carga datos del boton de scroll hacia arriba 
  CfgButton.szATButton = pParser->ReadString("UpButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("UpButton.");
  // Completa datos e inicializa
  CfgButton.ID = CGUIWConversator::ID_UPSCROLL;
  if (!m_ScrollInfo.UpButton.Init(CfgButton)) {
	return false;
  }

  // Carga datos del boton de scroll hacia abajo
  CfgButton.szATButton = pParser->ReadString("DownButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("DownButton.");
  // Completa datos e inicializa
  CfgButton.ID = CGUIWConversator::ID_DOWNSCROLL;
  if (!m_ScrollInfo.DownButton.Init(CfgButton)) {
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
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWConversator::EndComponents(void)
{
  // Finaliza componentes
  // Imagen de fondo
  m_BackImg.BackImg.End();
  
  // Botones de scroll
  m_ScrollInfo.UpButton.End();
  m_ScrollInfo.DownButton.End();

  // Info asociada a retratos
  m_MainConv.Name.End();
  m_MainConv.pEntity = NULL;
  m_SecondaryConv.Name.End();
  m_SecondaryConv.pEntity = NULL;
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa interfaz. Al activarse, se recibira la instancia de las dos 
//   entidades que participan en el dialogo para obtener sus nombres y retrato.
//   Lo normal es que el MainConv sea el jugador pues sera este el que escoga
//   las opciones presentadas.
// - Si la entidad Main estuviera hablando, se callara pues al activarse
//   se entiende que va a hablar.
// Parametros:
// - pMainConv. Instancia a conversador principal
// - pSecondaryConv. Instancia a conversador secundario.
// Devuelve:
// Notas:
// - En caso de que no haya ninguna opcion activa, no se realizara la 
//   activacion.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWConversator::Active(CWorldEntity* const pMainConv,
						 CWorldEntity* const pSecondaryConv)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pMainConv);
  ASSERT(pSecondaryConv);

  // Activa si procede
  if (!Inherited::IsActive()) {
	// Configura informacion relativa a retratos
	// Conversador principal y secundario
	m_MainConv.pEntity = pMainConv;
	m_MainConv.Name.ChangeText(pMainConv->GetName());	
	m_SecondaryConv.pEntity = pSecondaryConv;
	m_SecondaryConv.Name.ChangeText(pSecondaryConv->GetName());
	
	// Activa componentes
	ActiveComponents(true);

	// Se manda callar a la entidad principal
	pMainConv->ShutUp();

	// Se instala como observer
	SYSEngine::GetWorld()->AddObserver(this);
	
	// Propaga
	Inherited::Active();
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva interfaz
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWConversator::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Desactiva si procede
  if (Inherited::IsActive()) {
	// Anula instancias a retratos
	m_MainConv.pEntity = NULL;
	m_SecondaryConv.pEntity = NULL;

	// Desactiva componentes
	ActiveComponents(false);

	// Se desinstala como observer
	SYSEngine::GetWorld()->RemoveObserver(this);

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
///////////////////////////////////////////////////////////////////////////////
void
CGUIWConversator::ActiveComponents(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Establece flag de activacion / desactivacion
  // Imagen de fondo
  m_BackImg.BackImg.SetActive(bActive);

  // Botones de scroll
  m_ScrollInfo.UpButton.SetActive(bActive);
  m_ScrollInfo.DownButton.SetActive(bActive);

  // Nombres asociados a retratos
  m_MainConv.Name.SetActive(bActive);
  m_SecondaryConv.Name.SetActive(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion de un evento sobre componente asociado a esta interfaz.
// Parametros:
// - GUICEvent. Evento notificado.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWConversator::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿El identificador corresponde a una opcion?
  if (GUICEvent.IDComponent <= Inherited::MAX_IDOPTION_VALUE) {
	// Se propaga a la clase base
	Inherited::ComponentNotify(GUICEvent);
  } else {
	// Se trata de un componente particular del interfaz
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
		// Ante una pulsacion se llamara al metodo virtual puro que se espera
		// sea implementado en la clase base
		OnLeftClick(GUICEvent.IDComponent);
	  } break;	  
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la seleccion / deseleccion de los componentes
// Parametros:
// - IDComponent. Identificador del componente.
// - bSelect. Flag para seleccion / deseleccion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWConversator::OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
						   const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(IDComponent);

  // Procede a localizar componente
  switch(IDComponent) {
	case CGUIWConversator::ID_UPSCROLL: {
	  // Scroll de subir opciones
	  m_ScrollInfo.UpButton.Select(bSelect);
	} break;

	case CGUIWConversator::ID_DOWNSCROLL: {
	  // Scroll de bajar opciones
	  m_ScrollInfo.DownButton.Select(bSelect);
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la pulsacion con el boton izquierdo sobre un componente
//   del interfaz.
// Parametros:
// - IDComponent. Identificador del componente.
// Devuelve:
// Notas:
// - La notificacion de la pulsacion del boton cancelar se hara con
//   parametro de 0.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWConversator::OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(IDComponent);

  // Procede a localizar componente
  switch(IDComponent) {
	case CGUIWConversator::ID_UPSCROLL: {
	  // Scroll de subir opciones
	  Inherited::PrevOption();
	} break;

	case CGUIWConversator::ID_DOWNSCROLL: {
	  // Scroll de bajar opciones
	  Inherited::NextOption();
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
CGUIWConversator::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
							          const dword udParam)
{
  // SOLO si instancia inicializada y activada
  ASSERT(Inherited::IsInitOk());
  ASSERT(Inherited::IsActive());

  // Comprueba el tipo de notificacion
  switch(NotifyType) {
	case WorldObserverDefs::ENTITY_DESTROY: {
	  // ¿Es la entidad destruida la asociada?
	  if (udParam == m_SecondaryConv.pEntity->GetHandle()) {
		// Si, se cierra interfaz pasando a estado de MainInterfaz
		// Nota: Sera equivalente a realizar DesactiveConversatorDialog desde
		// CrisolScript. Al desactivar, la clase base notificara la seleccion
		// de una opcion no existente en el cuadro de dialogo, con lo que
		// si hubiera una instruccion destinada a recoger una opcion en 
		// CrisolScript, esta recibira notificacion
		SYSEngine::GetGUIManager()->SetMainInterfazWindow();
	  }
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dibuja las componentes y opciones
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWConversator::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Dibuja si procede
  if (Inherited::IsInitOk()) {
	// Imagen de fondo
	m_BackImg.BackImg.Draw();

	// Opciones y botones de scroll
	Inherited::DrawTextOptions();
	m_ScrollInfo.UpButton.Draw();
	m_ScrollInfo.DownButton.Draw();

	// Retratos y sus nombres
	if (m_MainConv.pEntity->GetPortrait()) {
	  m_pGraphSys->Draw(GraphDefs::DZ_GUI,
					    0,
						m_MainConv.DrawPos.swXPos,
						m_MainConv.DrawPos.swYPos,
						m_MainConv.pEntity->GetPortrait());
	}
	m_MainConv.Name.Draw();
	if (m_SecondaryConv.pEntity->GetPortrait()) {
	  m_pGraphSys->Draw(GraphDefs::DZ_GUI,
					    0,
						m_SecondaryConv.DrawPos.swXPos,
						m_SecondaryConv.DrawPos.swYPos,
						m_SecondaryConv.pEntity->GetPortrait());
	}
	m_SecondaryConv.Name.Draw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se instalara para obtener el evento de:
//   * Pulsacion del boton izq. del raton.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWConversator::InstallClient(void)
{
  // Se instalan eventos
  SYSEngine::GetInputManager()->SetInputEvent(InputDefs::IE_BUTTON_MOUSE_LEFT, 
											  this, 
											  InputDefs::BUTTON_PRESSED_DOWN);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Atiende los eventos de entrada.
// Parametros:
// - aInputEvent. Evento recibido.
// Devuelve:
// - Si se queda con el evento true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWConversator::DispatchEvent(const InputDefs::sInputEvent& aInputEvent)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Vbles
  bool bShareEvent = InputDefs::NO_SHARE_INPUT_EVENT; // ¿Hay que compartir el evento?

  // Evalua el evento producido 
  switch(aInputEvent.EventType) {	
	case InputDefs::BUTTON_EVENT: {	
	  // Evento de pulsacion 	  
	  // Se evaluan subeventos
	  switch (aInputEvent.EventButton.Code) {
		case InputDefs::IE_BUTTON_MOUSE_LEFT: {
		  // ¿Interfaz activa?
		  if (Inherited::IsActive()) {
			// Hace que las criaturas asociadas finalicen de hablar
			ASSERT(m_MainConv.pEntity);
			m_MainConv.pEntity->ShutUp();		
			ASSERT(m_SecondaryConv.pEntity);
			m_SecondaryConv.pEntity->ShutUp();
		  }
		} break;
	  }; // ~ switch
	} break;	
  }; // ~ switch

  // Se devuelve resultado
  return bShareEvent;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desinstala eventos de entrada para los que es cliente.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWConversator::UnistallClient(void)
{
  // Se desinstalan eventos  
  SYSEngine::GetInputManager()->UnsetInputEvent(InputDefs::IE_BUTTON_MOUSE_LEFT, 
												this, 
												InputDefs::BUTTON_PRESSED_DOWN);
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Bloquea / desbloquea la ventana de interfaz de cara al uso de entrada.
// Parametros:
// - bActive. Si vale true, recibira entrada si vale false dejara de recibir
//   actualizaciones de entrada.
// Modifica:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWConversator::SetInput(const bool bActive)
{
  // Solo si instancia inicializada
  ASSERT(IsInitOk());

  // Actua sobre componentes  
  // Imagen de fondo
  m_BackImg.BackImg.SetInput(bActive);

  // Botones de scroll
  m_ScrollInfo.UpButton.SetInput(bActive);
  m_ScrollInfo.DownButton.SetInput(bActive);

  // Nombres asociados a retratos
  m_MainConv.Name.SetInput(bActive);
  m_SecondaryConv.Name.SetInput(bActive);

  // Propaga
  Inherited::SetInput(bActive);
}
