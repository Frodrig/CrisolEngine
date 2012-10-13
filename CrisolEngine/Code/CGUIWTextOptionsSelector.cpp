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
// CGUIWTextOptionsSelector.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWTextOptionsSelector.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWTextOptionsSelector.h"

#include "SYSEngine.h"
#include "iCGameDataBase.h"
#include "iCGUIManager.h"
#include "CCBTEngineParser.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No permitira reinicializacion
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWTextOptionsSelector::Init(void)
{  
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  // Se inicializa clase base
  if (Inherited::Init()) {
	// Se cargan componentes
	if (TLoad()) {
	  // Todo correcto
	  return true;
	}
  }

  // Hubo problemas
  return false;	
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se finaliza instancia
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTextOptionsSelector::End(void)
{
  // Se finaliza si procede
  if (Inherited::IsInitOk()) {
	// Se desactivan componentes
	Deactive();

	// Finaliza componentes
	EndComponents();
	
	// Se propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la carga de componentes e inicializacion desde el archivo de
//   configuracion de texto.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWTextOptionsSelector::TLoad(void)
{
  // Se obtiene el parser
  CCBTEngineParser* const pParser = SYSEngine::GetGameDataBase()->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
																		 "[TextOptionsSelector]");
  ASSERT(pParser);

  // Se procede a cargar e inicializar componentes
  // Imagen de fondo
  if (!TLoadBackImgInfo(pParser)) {
	return false;
  }

  // Opciones y scroll
  if (!Inherited::TLoadTextOptionsInfo(pParser, 2)) {
	return false;
  }
  if (!TLoadScrollInfo(pParser)) {
	return false;
  }

  // Titulo
  if (!TLoadTitleInfo(pParser)) {
	return false;
  }

  // Boton de cancelacion
  if (!TLoadCancelInfo(pParser)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se carga la imagen de fondo
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWTextOptionsSelector::TLoadBackImgInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Vbles
  CGUICBitmap::sGUICBitmapInitData CfgBitmap; // Cfg del bitmap

  // Prefijo
  pParser->SetVarPrefix("BackImage.");

  // Se cargan datos
  CfgBitmap.BitmapInfo.szFileName = pParser->ReadString("FileName");
  CfgBitmap.Position = pParser->ReadPosition();
  CfgBitmap.BitmapInfo.Alpha = pParser->ReadAlpha();
  // Se completan datos
  CfgBitmap.ubDrawPlane = 2;
  CfgBitmap.ID = CGUIWTextOptionsSelector::ID_BACKIMG;

  // Se inicializa y se retorna
  m_BackImgInfo.BackImg.Init(CfgBitmap);
  return m_BackImgInfo.BackImg.IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la configuracion del titulo de fondo, inicializando el componente
//   que lo representa y el FXBack asociado.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - El texto asociado al titulo se establecera al activar el interfaz.
// - La inicializacion del FXBack se llevara a cabo en la activacion si 
//   procede.
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWTextOptionsSelector::TLoadTitleInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Vbles
  CGUICLineText::sGUICLineTextInitData CfgLine; // Config para la linea

  // Prefijo
  pParser->SetVarPrefix("TitleFormat.");

  // Carga la informacion
  CfgLine.Position = pParser->ReadPosition();
  CfgLine.RGBUnselectColor = pParser->ReadRGBColor();
  CfgLine.RGBSelectColor = CfgLine.RGBUnselectColor;
  // Se completan los datos y se inicializa
  CfgLine.ubDrawPlane = 2;
  CfgLine.ID = CGUIWTextOptionsSelector::ID_TITLE;
  CfgLine.szFont = "Arial";
  CfgLine.szLine = "-";
  if (!m_TitleInfo.Title.Init(CfgLine)) {
	return false;
  }

  // Se carga y guarda el color y valor alpha del FXBack
  m_TitleInfo.FXBackColor = pParser->ReadRGBColor("FXBack.");
  m_TitleInfo.FXAlpha = pParser->ReadAlpha("FXBack.");

  // Todo correcto
  return true;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la informacion relativa al boton de cancelacion
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWTextOptionsSelector::TLoadCancelInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Vbles
  CGUICButton::sGUICButtonInitData CfgButton; // Cfg del boton

  // Prefijo
  pParser->SetVarPrefix("Options.");

  // Procede a cargar informacion
  CfgButton.szATButton = pParser->ReadString("CancelButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("CancelButton.");
  // Completa datos e inicializa
  CfgButton.ID = CGUIWTextOptionsSelector::ID_CANCEL;
  CfgButton.pGUIWindow = this;
  CfgButton.ubDrawPlane = 2;
  CfgButton.GUIEvents = GUIDefs::GUIC_SELECT | 
						GUIDefs::GUIC_UNSELECT |
						GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  if (!m_CancelInfo.CancelButton.Init(CfgButton)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la informacion relativa a los botones de scroll
// Parametros:
// - pParser. Parser a utilizar
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWTextOptionsSelector::TLoadScrollInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
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
  CfgButton.ID = CGUIWTextOptionsSelector::ID_UPSCROLL;
  if (!m_ScrollInfo.UpButton.Init(CfgButton)) {
	return false;
  }

  // Carga datos del boton de scroll hacia abajo
  CfgButton.szATButton = pParser->ReadString("DownButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("DownButton.");
  // Completa datos e inicializa
  CfgButton.ID = CGUIWTextOptionsSelector::ID_DOWNSCROLL;
  if (!m_ScrollInfo.DownButton.Init(CfgButton)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza los componentes
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTextOptionsSelector::EndComponents(void)
{
  // Finaliza imagen de fondo
  m_BackImgInfo.BackImg.End();

  // Finaliza titulo
  m_TitleInfo.Title.End();
  m_TitleInfo.FXBack.End();
  
  // Finaliza boton de cancelacion
  m_CancelInfo.CancelButton.End();
  
  // Finaliza botones de scroll
  m_ScrollInfo.UpButton.End();
  m_ScrollInfo.DownButton.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa componentes y pregunta por la opcion a escoger. En caso de que
//   no tenga exito la orden de obtener una opcion, significara que desde el
//   exterior no se asocio ninguna y el proceso de activacion fallara.
// Parametros:
// - szTitle. Titulo de la interfaz.
// - bCanCancel. Flag para poder cancelar / no cancelar la interfaz.
// Devuelve:
// Notas:
// - Se podra pasar un titulo vacio, en cuyo caso simplemente no se mostrara
//   nada.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTextOptionsSelector::Active(const std::string& szTitle,
								 const bool bCanCancel)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Activa si procede
  if (!Inherited::IsActive() && 
	  Inherited::GetNumOptions() > 0) {
	// Establece titulo
	SetTitle(szTitle);
	
	// Establece flag de uso de cancel
	m_CancelInfo.bCanCancel = bCanCancel;
	
	// Activa componentes
	ActiveComponents(true);
	
	// Propaga
	Inherited::Active();
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva interfaz y componentes
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTextOptionsSelector::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Desactiva si procede
  if (Inherited::IsActive()) {
	// Componentes
	ActiveComponents(false);

	// Propaga
	Inherited::Deactive();
  }
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
CGUIWTextOptionsSelector::SetInput(const bool bActive)
{
  // Solo si instancia inicializada
  ASSERT(IsInitOk());

  // Actua sobre componentes
  // Imagen de fondo
  m_BackImgInfo.BackImg.SetInput(bActive);
  
  // Titulo
  if (m_TitleInfo.bExistTitle) {
	m_TitleInfo.Title.SetInput(bActive);
  }
  
  // Scroll
  m_ScrollInfo.UpButton.SetInput(bActive);
  m_ScrollInfo.DownButton.SetInput(bActive);
  
  // Cancelacion
  m_CancelInfo.CancelButton.SetInput(bActive);  

  // Propaga
  Inherited::SetInput(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece titulo siempre y cuando se reciba por parametro. En caso 
//   contrario no asociara ninguno.
// - Al establecer titulo, se inicializara el FXBack asociado, que ocupara
//   toda la zona inferior en donde se escriba el titulo.
// Parametros:
// - szTitle. Titulo a establecer.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTextOptionsSelector::SetTitle(const std::string& szTitle)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿NO se desea establecer titulo?
  if (!szTitle.size()) {
	m_TitleInfo.bExistTitle = false;
  } else {
	// Si, se asocia al componente linea
	m_TitleInfo.Title.ChangeText(szTitle);
	m_TitleInfo.bExistTitle = true;

	// Se inicializa el FXBack
	const sPosition DrawPos(m_TitleInfo.Title.GetXPos() - 4,
							m_TitleInfo.Title.GetYPos() - 2);
	m_TitleInfo.FXBack.Init(DrawPos,
							2,
							m_TitleInfo.Title.GetWidth() + 6,
							m_TitleInfo.Title.GetHeight() + 2,
							m_TitleInfo.FXBackColor,
							m_TitleInfo.FXAlpha);	  
	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa / desactiva (segun flag) los componentes asociados al interfaz
// Parametros:
// - bActive. Flag de activacion
// Devuelve:
// Notas:
// - El metodo sera de apoyo a Active y Deactive.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTextOptionsSelector::ActiveComponents(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Procede a activar / desactivar componentes
  // Imagen de fondo
  m_BackImgInfo.BackImg.SetActive(bActive);
  
  // Titulo
  if (m_TitleInfo.bExistTitle) {
	m_TitleInfo.Title.SetActive(bActive);
  }
  
  // Scroll
  m_ScrollInfo.UpButton.SetActive(bActive);
  m_ScrollInfo.DownButton.SetActive(bActive);
  
  // Cancelacion
  m_CancelInfo.CancelButton.SetActive(bActive);  
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
CGUIWTextOptionsSelector::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
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
CGUIWTextOptionsSelector::OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
								   const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(IDComponent);

  // Procede a localizar componente
  switch(IDComponent) {
	case CGUIWTextOptionsSelector::ID_UPSCROLL: {
	  // Scroll de subir opciones
	  m_ScrollInfo.UpButton.Select(bSelect);
	} break;

	case CGUIWTextOptionsSelector::ID_DOWNSCROLL: {
	  // Scroll de bajar opciones
	  m_ScrollInfo.DownButton.Select(bSelect);
	} break;

	case CGUIWTextOptionsSelector::ID_CANCEL: {
	  // Opcion de cancelar, que se seleccionara solo si es posible
	  if (m_CancelInfo.bCanCancel) {
		m_CancelInfo.CancelButton.Select(bSelect);
	  }
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
CGUIWTextOptionsSelector::OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(IDComponent);

  // Procede a localizar componente
  switch(IDComponent) {
	case CGUIWTextOptionsSelector::ID_UPSCROLL: {
	  // Scroll de subir opciones
	  Inherited::PrevOption();
	} break;

	case CGUIWTextOptionsSelector::ID_DOWNSCROLL: {
	  // Scroll de bajar opciones
	  Inherited::NextOption();
	} break;

	case CGUIWTextOptionsSelector::ID_CANCEL: {
	  // Opcion de cancelar, que se seleccionara solo si es posible
	  if (m_CancelInfo.bCanCancel) {
		// Se notifica al posible cliente y se desactiva interfaz
		Inherited::GUIWindowNotify(0);
		SYSEngine::GetGUIManager()->DeactiveTextOptionsSelector();
	  }
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion de la opcion pulsada.
// Parametros:
// - IDComponent. Identificador de la opcion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTextOptionsSelector::OnTextOptionLeftClick(const GUIDefs::GUIIDComponent& IDOption)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(IDOption);

  // Se propaga a la clase base
  Inherited::OnTextOptionLeftClick(IDOption);

  // Se desactiva interfaz
  SYSEngine::GetGUIManager()->DeactiveTextOptionsSelector();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Procede a dibujar componentes.
// - El dibujado de las opciones visibles se hara realizando una llamada al
//   metodo apropiado en la clase base.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTextOptionsSelector::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Dibuja si procede
  if (Inherited::IsActive()) {
	// Imagen de fondo
	m_BackImgInfo.BackImg.Draw();

	// Opciones y scroll
	Inherited::DrawTextOptions();
	m_ScrollInfo.UpButton.Draw();
	m_ScrollInfo.DownButton.Draw();

	// Boton de cancelacion
	m_CancelInfo.CancelButton.Draw();

	// Titulo si procede
	if (m_TitleInfo.bExistTitle) {
	  m_TitleInfo.FXBack.Draw();
	  m_TitleInfo.Title.Draw();
	}
  }
}

  