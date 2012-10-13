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
// CGUIWDialog.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWDialog.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWDialog.h"

#include "SYSEngine.h"
#include "iCGUIManager.h"
#include "iCGameDataBase.h"
#include "CCBTEngineParser.h"

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
CGUIWDialog::Init(void)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }
  
  // Se inicializa clase base
  if (Inherited::Init()) {
	// Todo correcto
	return true;  
  }

  // Hubo problemas
  End();
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWDialog::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {
	// Desactiva
	Deactive();

	// Finaliza componentes
	EndBackImg();
	EndTextInfo();
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la carga de los datos comunes a cualquier cuadro de dialogo, esto
//   es, la imagen de fondo y la configuracion de cómo aparecera el texto en
//   pantalla.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - El metodo sera llamado desde las clases base
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWDialog::TLoad(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Procede a cargar componentes
  // Imagen de fondo
  if (!TLoadBackImgInfo(pParser)) {
	return false;
  }

  // Info asociada al texto
  if (!TLoadTextInfo(pParser)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la informacion asociada a la imagen de fondo.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWDialog::TLoadBackImgInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Vbles
  CGUICBitmap::sGUICBitmapInitData CfgBitmap; // Datos de configuracion

  // Prefijo
  pParser->SetVarPrefix("BackImage.");

  // Se cargan datos
  CfgBitmap.BitmapInfo.szFileName = pParser->ReadString("FileName");
  CfgBitmap.Position = pParser->ReadPosition();
  CfgBitmap.BitmapInfo.Alpha = pParser->ReadAlpha();
  // Se completan datos
  CfgBitmap.ubDrawPlane = 2;
  CfgBitmap.ID = CGUIWDialog::ID_BACKIMG;

  // Se inicializa y se retorna
  m_BackImg.BackImg.Init(CfgBitmap);
  return m_BackImg.BackImg.IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se carga la informacion relativa al texto y al FX de fondo. No se
//   alojara texto alguno pues este se pasara unicamente al activar el dialogo.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - La inicializacion del quad no sera efectiva hasta que se active el
//   cuadro de dialogo, pues sera necesario conocer el area exacta ocupada
//   por el texto a mostrar.
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWDialog::TLoadTextInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Vble donde cargar la configuracion del texto
  CGUICSingleText::sGUICSingleTextInitData CfgText; // Datos de configuracion

  // Se leen datos asociados al texto
  pParser->SetVarPrefix("TextFormat.");
  CfgText.Position = pParser->ReadPosition();
  CfgText.uwWidthJustify = pParser->ReadNumeric("AreaWidth");
  CfgText.RGBSelectColor = pParser->ReadRGBColor();
  CfgText.RGBUnselectColor = CfgText.RGBSelectColor;  

  // Se completan datos
  CfgText.ubDrawPlane = 2;
  CfgText.ID = CGUIWDialog::ID_TEXT;
  CfgText.szFont = "Arial";
  CfgText.szText = "*";

  // Se inicializa
  if (!m_TextInfo.Text.Init(CfgText)) {
	return false;
  }  
  
  // Se lee configuracion del FXBack para cuando llegue la activacion
  m_TextInfo.FXBackColor = pParser->ReadRGBColor("FXBack.");
  m_TextInfo.FXBackAlpha = pParser->ReadAlpha("FXBack.");

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza imagen de fondo
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWDialog::EndBackImg(void)
{
  // Finaliza
  m_BackImg.BackImg.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza texto de fondo.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWDialog::EndTextInfo(void)
{
  // Finaliza    
  m_TextInfo.Text.End();
  m_TextInfo.FXBack.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el cuadro de dialogo.
// Parametros:
// - szText. Texto a establecer en el cuadro de dialogo.
// - pClient. Cliente asociado al cuadro de dialogo que deseamos reciba la
//   notificacion cuando se pulse la opcion que proceda. Ver clases derivadas
//   de esta.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWDialog::Active(const std::string& szText,
					iCGUIWindowClient* const pClient)
{
  // SOLO si parametros correctos
  ASSERT(szText.size());

  // Activa si procede
  if (!Inherited::IsActive()) {
	// Se activan componentes.
	m_BackImg.BackImg.SetActive(true);
	m_TextInfo.Text.SetActive(true);
  
	// Se asocia el texto recibido y se justifica al centro siempre
	m_TextInfo.Text.ChangeText(szText);
	m_TextInfo.Text.DrawJustifyTo(CGUICSingleText::JUSTIFY_CENTER);
  
	// Se inicializa el FXBack
	const sPosition FXBackDrawPos(m_TextInfo.Text.GetXPos() - 4,
								  m_TextInfo.Text.GetYPos() - 2);
	const word uwFXBackHeight = FontDefs::CHAR_PIXEL_HEIGHT * m_TextInfo.Text.GetVisibleTextLines();
	m_TextInfo.FXBack.Init(FXBackDrawPos, 
						   2, 
						   m_TextInfo.Text.GetWidthJustify() + 4,
						   uwFXBackHeight,
						   m_TextInfo.FXBackColor,
						   m_TextInfo.FXBackAlpha);
	ASSERT(m_TextInfo.FXBack.IsInitOk());

	// Se establece la posicion del FXBack
	m_TextInfo.FXBack.SetPosition(sPosition(m_TextInfo.Text.GetXPos() - 4,
											m_TextInfo.Text.GetYPos() - 2));

	// En teoria el cursor deberia de interfaz deberia de estar activo, pero
	// se volvera activar para que no existan efectos laterales
	SYSEngine::GetGUIManager()->SetGUICursor(GUIManagerDefs::WINDOWINTERFAZ_CURSOR);

	// Se propaga inicializacion
	Inherited::Active(pClient);
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva la ventana de interfaz.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWDialog::Deactive(void)
{
  // SOLO si instancia inicializada 
  ASSERT(Inherited::IsInitOk());

  // Desactiva si procede
  if (Inherited::IsActive()) {
	// Desactiva componentes y finaliza quad de FX
	m_BackImg.BackImg.SetActive(false);
	m_TextInfo.Text.SetActive(false);
	m_TextInfo.FXBack.End();

	// Propaga desactivacion
	Inherited::Deactive(); 
  }   
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Operacion de dibujado del cuadro de dialogo
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWDialog::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Dibuja si procede
  if (Inherited::IsActive()) {
	// Imagen de fondo
	m_BackImg.BackImg.Draw();

	// Texto y fx
	m_TextInfo.FXBack.Draw();
	m_TextInfo.Text.Draw();	

	// Opciones
	DrawOptions();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWAdviceDialog::Init(void)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  // Se inicializa clase base
  if (Inherited::Init()) {
	// Procede a cargar datos de disco
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
// - Finaliza instancia.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWAdviceDialog::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {
	// Se desactivan componentes
	Deactive();

	// Se finalizan componentes
	m_OptionsInfo.Accept.End();
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la carga de datos y configuracion de componentes.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWAdviceDialog::TLoad(void)
{
  // Obtiene parser a utilizar
  CCBTEngineParser* const pParser = SYSEngine::GetGameDataBase()->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
																		       "[AdviceDialog]");
  ASSERT(pParser);

  // Carga datos comunes de un cuadro de dialogo
  if (!Inherited::TLoad(pParser)) {
	return false;
  }

  // Carga datos particulares
  if (!TLoadOptionsInfo(pParser)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la informacion relativa al boton de aceptacion.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - Este metodo sera llamado desde CGUIWDialog.
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWAdviceDialog::TLoadOptionsInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Vbles
  CGUICButton::sGUICButtonInitData CfgButton; // Datos de configuracion

  // Prefijo
  pParser->SetVarPrefix("Options.");

  // Se cargan datos
  CfgButton.szATButton = pParser->ReadString("AcceptButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("AcceptButton.");
  // Se completan datos
  CfgButton.ubDrawPlane = 2;
  CfgButton.ID = CGUIWAdviceDialog::ID_ACCEPT;
  CfgButton.pGUIWindow = this;
  CfgButton.GUIEvents = GUIDefs::GUIC_SELECT | 
					    GUIDefs::GUIC_UNSELECT |
						GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  // Se inicializa
  if (!m_OptionsInfo.Accept.Init(CfgButton)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa boton de aceptacion y propaga llamada.
// Parametros:
// - szText. Texto a establecer en el cuadro de dialogo.
// - pClient. Cliente dispuesto a recibir los datos.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWAdviceDialog::Active(const std::string& szText,
						  iCGUIWindowClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(szText.size());

  // Se activa si procede
  if (!Inherited::IsActive()) {
	// Activa boton de aceptacion
	m_OptionsInfo.Accept.SetActive(true);

	// Propaga
	Inherited::Active(szText, pClient);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva componentes.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWAdviceDialog::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Desactiva si procede
  if (Inherited::IsActive()) {
	// Desactiva boton de aceptacion
	m_OptionsInfo.Accept.SetActive(false);
	
	// Propaga
	Inherited::Deactive();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion de eventos sobre componentes. En concreto, se trabajara
//   unicamente sobre el componente boton de aceptacion.
// Parametros:
// - GUICEvent. Evento producido.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWAdviceDialog::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Comprueba tipo de evento
  switch (GUICEvent.Event) {
	case GUIDefs::GUIC_SELECT: {	
	  // Seleccion
	  m_OptionsInfo.Accept.Select(true);
	} break;

  	case GUIDefs::GUIC_UNSELECT: {	
	  // Deseleccion
	  m_OptionsInfo.Accept.Select(false);
	} break;
	
	case GUIDefs::GUIC_BUTTONLEFT_PRESSED: {	  	  
	  // Pulsacion
	  // Se notificara el evento al posible cliente asociado
	  Inherited::GUIWindowNotify(1);

	  // Se notifica al GUIManager que se Deactive el cuadro de dialogo
	  SYSEngine::GetGUIManager()->DeactiveAdviceDialog();
	} break;	  
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dibuja las opciones
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWAdviceDialog::DrawOptions(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Dibuja las opciones
  m_OptionsInfo.Accept.Draw();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWQuestionDialog::Init(void)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  // Se inicializa clase base
  if (Inherited::Init()) {
	// Procede a cargar datos
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
CGUIWQuestionDialog::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {
	// Se desactiva o habra errores al llamar CGUIWindow a Deactive
	Deactive();
	
	// Finaliza botones de opciones
	m_OptionsInfo.Yes.End();
	m_OptionsInfo.No.End();
	m_OptionsInfo.Cancel.End();

	// Propaga
	Inherited::End();	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la carga de datos y configuracion de componentes.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWQuestionDialog::TLoad(void)
{
  // Obtiene parser a utilizar
  CCBTEngineParser* const pParser = SYSEngine::GetGameDataBase()->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
																		  "[QuestionDialog]");
  ASSERT(pParser);

  // Carga datos comunes de un cuadro de dialogo
  if (!Inherited::TLoad(pParser)) {
	return false;
  }

  // Carga datos particulares
  if (!TLoadOptionsInfo(pParser)) {
	return false;
  }

  // Todo correcto
  return true;
}
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga los datos asociados a los botones de opciones
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido correctamente true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWQuestionDialog::TLoadOptionsInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Vbles
  CGUICButton::sGUICButtonInitData CfgButton; // Cfg del boton

  // Prefijo
  pParser->SetVarPrefix("Options.");

  // Se establecen datos comunes a cualquier boton
  //CfgButton.
  CfgButton.pGUIWindow = this;
  CfgButton.GUIEvents = GUIDefs::GUIC_SELECT | 
					    GUIDefs::GUIC_UNSELECT |
						GUIDefs::GUIC_BUTTONLEFT_PRESSED;

  // Boton de aceptacion
  // Se cargan datos
  CfgButton.szATButton = pParser->ReadString("YesButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("YesButton.");
  // Se completan datos y se inicializa
  CfgButton.ubDrawPlane = 2;
  CfgButton.ID = CGUIWQuestionDialog::ID_YES;
  if (!m_OptionsInfo.Yes.Init(CfgButton)) {
	return false;
  }
  
  // Boton de rechazo
  // Se cargan datos
  CfgButton.szATButton = pParser->ReadString("NoButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("NoButton.");
  // Se completan datos y se inicializa
  CfgButton.ubDrawPlane = 2;
  CfgButton.ID = CGUIWQuestionDialog::ID_NO;
  if (!m_OptionsInfo.No.Init(CfgButton)) {
	return false;
  }

  // Boton de cancelacion
  // Se cargan datos
  CfgButton.szATButton = pParser->ReadString("CancelButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("CancelButton.");
  // Se completan datos y se inicializa
  CfgButton.ubDrawPlane = 2;
  CfgButton.ID = CGUIWQuestionDialog::ID_CANCEL;
  if (!m_OptionsInfo.Cancel.Init(CfgButton)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa instancia.
// Parametros:
// - szText. Texto a mostrar.
// - bCancelPresent. En caso de que este flag valga true, la opcion de cancelar
//   se mostrara y en caso contrario no se motrara. Las opciones de aceptacion
//   y rechazo siempre estaran presentes.
// - pClient. Cliente al que notificar el boton seleccionado.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWQuestionDialog::Active(const std::string& szText,
							const bool bCancelPresent,
							iCGUIWindowClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(szText.size());

  // Se activa si procede
  if (!Inherited::IsActive()) {
	// Se activan los botones de aceptacion y rechazo
	m_OptionsInfo.Yes.SetActive(true);
	m_OptionsInfo.No.SetActive(true);

	// Se activa el de cancelacion si procede
	if (bCancelPresent) {
	  m_OptionsInfo.Cancel.SetActive(true);
	}

	// Se Propaga activacion
	Inherited::Active(szText, pClient);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva componentes
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWQuestionDialog::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
    
  // Desactiva componentes si procede
  if (Inherited::IsActive()) {
	m_OptionsInfo.Yes.SetActive(false);
	m_OptionsInfo.No.SetActive(false);
	m_OptionsInfo.Cancel.SetActive(false);  
    
	// Propaga
	Inherited::Deactive();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notifica un evento producido sobre un boton de opciones.
// Parametros:
// - GUICEvent. Evento producido.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWQuestionDialog::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Comprueba tipo de evento
  switch (GUICEvent.Event) {
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
	  // Se notifica al GUIManager que se Deactive el cuadro de dialogo
	  SYSEngine::GetGUIManager()->DeactiveAdviceDialog();	  
	} break;	  
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la seleccion / deseleccion de un componente.
// Parametros:
// - IDComponent. Id del componente.
// - bSelect. Flag de seleccion / deseleccion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWQuestionDialog::OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
							  const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se selecciona / deselecciona el componente que proceda
  switch(IDComponent) {
	case CGUIWQuestionDialog::ID_YES: {
	  m_OptionsInfo.Yes.Select(bSelect);
	} break;

	case CGUIWQuestionDialog::ID_NO: {
	  m_OptionsInfo.No.Select(bSelect);
	} break;

	case CGUIWQuestionDialog::ID_CANCEL: {
	  m_OptionsInfo.Cancel.Select(bSelect);
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la pulsacion sobre un componente seleccionado. Los
//   parametros que se mandaran segun sea el componente seran: 
//   * Aceptacion. 1.
//   * Rechazo 2.
//   * Cancelacion 0.
// Parametros:
// - IDComponent. Id del componente.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWQuestionDialog::OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Se selecciona / deselecciona el componente que proceda
  switch(IDComponent) {
	case CGUIWQuestionDialog::ID_YES: {
	  Inherited::GUIWindowNotify(1);	 
	} break;

	case CGUIWQuestionDialog::ID_NO: {
	  Inherited::GUIWindowNotify(2);	  
	} break;

	case CGUIWQuestionDialog::ID_CANCEL: {
	  Inherited::GUIWindowNotify(0);	  
	} break;

	default:
	  ASSERT(false);
  }; // ~ switch

  // Se notifica al GUIManager que se Deactive el cuadro de dialogo
  SYSEngine::GetGUIManager()->DeactiveQuestionDialog();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dibujado de las opciones
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWQuestionDialog::DrawOptions(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se dibujan las opciones
  m_OptionsInfo.Yes.Draw();
  m_OptionsInfo.No.Draw();
  m_OptionsInfo.Cancel.Draw();  
}
