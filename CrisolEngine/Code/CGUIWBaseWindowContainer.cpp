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
// CGUIWBaseItemSelector.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWBaseItemSelector.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWBaseWindowContainer.h"

#include "SYSEngine.h"
#include "CCBTEngineParser.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia
// Parametros:
// Devuelve:
// - Si todo ha sido correcto true.
// Notas:
// - No se dejara reinicializar
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseWindowContainer::Init(void)
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
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseWindowContainer::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	// Desactiva
	Deactive();
	
	// Finaliza componentes
	m_BackImgInfo.BackImg.End();
	m_ScrollInfo.UpButton.End();
	m_ScrollInfo.DownButton.End();

	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la carga de componentes.
// Parametros:
// - pParser. Parser a utilizar
// - ubDrawPlane. Plano en donde dibujar los componentes.
// - bReadBackImgAlpha. ¿Se debe de leer el componente alpha en la imagen 
//   de fondo?
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
// - En los metodos de carga no se establecera prefijo, se usara el que se 
//   haya puesto en la clase derivada
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseWindowContainer::TLoad(CCBTEngineParser* const pParser, 
								const byte ubDrawPlane,
								const bool bReadBackImgAlpha)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(pParser);

  // Procede a cargar componentes de clase base
  if (Inherited::TLoad(pParser, ubDrawPlane)) {
	// Se cargan componentes locales
	// Imagen de fondo  
	if (!TLoadBackImgInfo(pParser, ubDrawPlane, bReadBackImgAlpha)) {
	  return false;
	}

	// Botones de scroll	
	if (!TLoadScrollInfo(pParser, ubDrawPlane)) {
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
// - Carga imagen de fondo
// Parametros:
// - pParser. Parser a utilizar.
// - ubDrawPlane. Plano de dibujado
// - bReadBackImgAlpha. ¿Se debe de leer el componente alpha en la imagen 
//   de fondo?
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseWindowContainer::TLoadBackImgInfo(CCBTEngineParser* const pParser,
										   const byte ubDrawPlane,
										   const bool bReadBackImgAlpha)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Vbles
  CGUICBitmap::sGUICBitmapInitData CfgBitmap; // Datos de configuracion

  // Se cargan datos
  CfgBitmap.BitmapInfo.szFileName = pParser->ReadString("BackImage.FileName");
  CfgBitmap.Position = pParser->ReadPosition("BackImage.");
  if (bReadBackImgAlpha) {
	CfgBitmap.BitmapInfo.Alpha = pParser->ReadAlpha("BackImage.");
  }
  
  // Se completan datos
  CfgBitmap.ubDrawPlane = ubDrawPlane;
  CfgBitmap.ID = CGUIWBaseWindowContainer::ID_BACKIMG;

  // Se inicializa y se retorna
  return m_BackImgInfo.BackImg.Init(CfgBitmap);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga informacion relativa a los botones de scroll.
// Parametros:
// - pParser. Parser a utilizar
// - ubPlane. Plano de dibujado
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseWindowContainer::TLoadScrollInfo(CCBTEngineParser* const pParser,
										  const byte ubDrawPlane)
{
  // SOLO si parametros validos
  ASSERT(pParser);
  
  // Vbles
  CGUICButton::sGUICButtonInitData CfgButton; // Cfg del boton

  // Se establecen datos comunes a los botones de scroll
  CfgButton.GUIEvents = GUIDefs::GUIC_SELECT | 
						GUIDefs::GUIC_UNSELECT |
						GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  CfgButton.pGUIWindow = this;
  CfgButton.ubDrawPlane = ubDrawPlane;
  
  // Scroll hacia arriba
  // Se cargan datos
  CfgButton.szATButton = pParser->ReadStringID("Options.UpButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("Options.UpButton.");
  // Se completan e inicializa  
  CfgButton.ID = CGUIWBaseWindowContainer::ID_UPSCROLL;
  if (!m_ScrollInfo.UpButton.Init(CfgButton)) {
	return false;
  }
  
  // Scroll hacia abajo
  // Se cargan datos
  CfgButton.szATButton = pParser->ReadStringID("Options.DownButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("Options.DownButton.");
  // Se completan e inicializa
  CfgButton.ID = CGUIWBaseWindowContainer::ID_DOWNSCROLL;
  if (!m_ScrollInfo.DownButton.Init(CfgButton)) {
	return false;
  }  
  
  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa interfaz y componentes y propaga llamada.
// Parametros:
// - pItemContainer. Direccion del contenedor a representar.
// - pClient. Direccion del posible cliente asociado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseWindowContainer::Active(iCItemContainer* const pItemContainer,
								 iCGUIWindowClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(pItemContainer);

  // Activa si procede    
  if (!Inherited::IsActive()) {
	// Activa componentes
	m_ScrollInfo.UpButton.SetActive(true);
	m_ScrollInfo.DownButton.SetActive(true);
	m_BackImgInfo.BackImg.SetActive(true); 	
	
	// Propaga 
	Inherited::Active(pItemContainer, pClient);		
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva interfaz y componentes
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGUIWBaseWindowContainer::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Desactiva si procede
  if (Inherited::IsActive()) {
	// Desactiva componentes
	m_ScrollInfo.UpButton.SetActive(false);
	m_ScrollInfo.DownButton.SetActive(false);
	m_BackImgInfo.BackImg.SetActive(false); 

	// Propaga
	Inherited::Deactive();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa / desactiva entrada
// Parametros:
// - bActive. Flag de activacion / desactivacion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseWindowContainer::SetInput(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Procede a activar / desactivar
  // Imagen de fondo
  m_BackImgInfo.BackImg.SetInput(bActive);

  // Botones de scroll
  m_ScrollInfo.UpButton.SetInput(bActive);
  m_ScrollInfo.DownButton.SetInput(bActive);

  // Propaga
  Inherited::SetInput(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion de un evento sobre componente
// Parametros:
// Devuelve:
// Notas:
// - El metodo debera de propagar la llamada si el identificador corresponde
//   a un selector.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseWindowContainer::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(GUICEvent.IDComponent);

  // ¿No es un identificador de los componentes mantenidos?  
  if (GUICEvent.IDComponent < CGUIWBaseWindowContainer::ID_BACKIMG) {
	// Propaga
	Inherited::ComponentNotify(GUICEvent);
  } else {
	// Comprueba el tipo de evento
	switch(GUICEvent.Event) {
	  case GUIDefs::GUIC_SELECT: {	
		// Seleccion
		OnBaseWindowSelect(GUICEvent.IDComponent, true);	
	  } break;

  	  case GUIDefs::GUIC_UNSELECT: {	
		// Deseleccion
		OnBaseWindowSelect(GUICEvent.IDComponent, false);	  
	  } break;
	  
	  case GUIDefs::GUIC_BUTTONLEFT_PRESSED: {	  	  
		// Pulsacion
		OnBaseWindowLeftClick(GUICEvent.IDComponent);
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
CGUIWBaseWindowContainer::OnBaseWindowSelect(const GUIDefs::GUIIDComponent& IDComponent,
											 const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(IDComponent);

  // Se actua sobre el componente que proceda
  switch(IDComponent) {
	case CGUIWBaseWindowContainer::ID_UPSCROLL: {
	  // Boton de scroll hacia arriba
	  m_ScrollInfo.UpButton.Select(bSelect);
	} break;
	
	case CGUIWBaseWindowContainer::ID_DOWNSCROLL: {
	  // Boton de scroll hacia abajo
	  m_ScrollInfo.DownButton.Select(bSelect);
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
CGUIWBaseWindowContainer::OnBaseWindowLeftClick(const GUIDefs::GUIIDComponent& IDComponent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(IDComponent);

  // Se actua sobre el componente que proceda
  switch(IDComponent) {
	case CGUIWBaseWindowContainer::ID_UPSCROLL: {
	  // Boton de scroll hacia arriba
	  // Se retrocedera en una fila los items visualizables
	  Inherited::PrevItemFile();
	} break;
	
	case CGUIWBaseWindowContainer::ID_DOWNSCROLL: {
	  // Boton de scroll hacia abajo
	  // Se avanzara en una fila los items visualizables
	  Inherited::NextItemFile();
	} break;
  }; // ~ switch  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dibuja componentes.
// Parametros:
// Devuelve:
// Notas:
// - La responsabilidad de poder dibujar quedara en clases derivadas
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseWindowContainer::Draw(void)
{
  // SOLO si instancia incializada
  ASSERT(Inherited::IsInitOk());

  // Imagen de fondo
  m_BackImgInfo.BackImg.Draw();

  // Dibuja botones de scroll
  m_ScrollInfo.UpButton.Draw();
  m_ScrollInfo.DownButton.Draw(); 

  // Se dibuja clase base con los componentes
  Inherited::Draw(); 
}