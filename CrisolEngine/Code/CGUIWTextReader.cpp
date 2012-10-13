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
// CGUIWTextReader.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWTextReader.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWTextReader.h"

#include "SYSEngine.h"
#include "iCGUIManager.h"
#include "iCGameDataBase.h"
#include "iCFileSystem.h"
#include "CCBTEngineParser.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar.
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWTextReader::Init(void)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  // Se inicializa clase base
  if (Inherited::Init()) {
	// Carga los componentes
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
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTextReader::End(void)
{
  // Finaliza
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
// - Se encarga de cargar toda la informacion relativa a los componentes,
//   coordinando el proceso.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWTextReader::TLoad(void)
{
  // Obtiene el parser
  CCBTEngineParser* const pParser = SYSEngine::GetGameDataBase()->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
																		       "[TextFileReader]");
  ASSERT(pParser);

  // Procede a cargar e inicializar componentes
  // Imagen de fondo
  if (!TLoadBackImgInfo(pParser)) {
	return false;
  }

  // Texto
  if (!TLoadTextInfo(pParser)) {
	return false;
  }

  // Carga de botones de opciones
  if (!TLoadOptionsInfo(pParser)) {
	return false;
  }

  // Carga titulo
  if (!TLoadTitleInfo(pParser)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la imagen de fondo
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWTextReader::TLoadBackImgInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
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
  CfgBitmap.ID = CGUIWTextReader::ID_BACKIMG;

  // Se inicializa y se retorna
  m_BackImgInfo.BackImg.Init(CfgBitmap);
  return m_BackImgInfo.BackImg.IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Informacion asociada al texto
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
// - El bloque de texto se debera de configurar con cada activacion, por lo
//   que unicamente se guaradan los datos de configuracion leidos desde disco.
// - La capacidad se establecera a valor cero pues no se podra conocer el
//   numero maximo de lineas que seran necesarias.
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWTextReader::TLoadTextInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Prefijo
  pParser->SetVarPrefix("TextFileFormat.");

  // Se cargan y se guardan los datos de configuracion del bloque de texto
  m_TextInfo.CfgText.Position = pParser->ReadPosition();
  m_TextInfo.CfgText.uwWidthJustify = pParser->ReadNumeric("AreaWidth");
  m_TextInfo.CfgText.uwNumLines = pParser->ReadNumeric("MaxLines");
  m_TextInfo.CfgText.RGBUnselectColor = pParser->ReadRGBColor();
  m_TextInfo.CfgText.RGBSelectColor = m_TextInfo.CfgText.RGBUnselectColor;
  // Se completan datos
  m_TextInfo.CfgText.uwCapacity = 0;
  m_TextInfo.CfgText.bViewBottomUp = false;
  m_TextInfo.CfgText.ubDrawPlane = 2;
  m_TextInfo.CfgText.ID = CGUIWTextReader::ID_TEXT;
  m_TextInfo.CfgText.szFont = "Arial";
  m_TextInfo.CfgText.GUIEvents = GUIDefs::GUIC_SELECT | 
								 GUIDefs::GUIC_UNSELECT |
								 GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  m_TextInfo.CfgText.pGUIWindow = this;

  // Se lee la configuracion para el FX de fondo
  // Se ajustan parametros para el FX
  const sPosition FXDrawPos(m_TextInfo.CfgText.Position.swXPos - 4,
							m_TextInfo.CfgText.Position.swYPos - 2);
  const word uwHeight = FontDefs::CHAR_PIXEL_HEIGHT * m_TextInfo.CfgText.uwNumLines + 2;
  if (!m_TextInfo.FXBack.Init(FXDrawPos, 
							  2,
							  m_TextInfo.CfgText.uwWidthJustify + 4,
							  uwHeight,
							  pParser->ReadRGBColor("FXBack."),
							  pParser->ReadAlpha("FXBack."))) {
	return false;
  }

  // Se cargan datos asociados a los botones de scroll para el texto
  pParser->SetVarPrefix("Options.");

  // Se establecen datos comunes a los botones de scroll
  CGUICButton::sGUICButtonInitData CfgButton;  
  CfgButton.GUIEvents = GUIDefs::GUIC_SELECT | 
						GUIDefs::GUIC_UNSELECT |
						GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  CfgButton.pGUIWindow = this;
  CfgButton.ubDrawPlane = 2;

  // Scroll hacia arriba
  // Se cargan datos
  CfgButton.szATButton = pParser->ReadString("UpButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("UpButton.");
  // Se completan e inicializa  
  CfgButton.ID = CGUIWTextReader::ID_UPBUTTON;
  if (!m_TextInfo.UpButton.Init(CfgButton)) {
	return false;
  }

  // Scroll hacia abajo
  // Se cargan datos
  CfgButton.szATButton = pParser->ReadString("DownButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("DownButton.");
  // Se completan e inicializa
  CfgButton.ID = CGUIWTextReader::ID_DOWNBUTTON;
  if (!m_TextInfo.DownButton.Init(CfgButton)) {
	return false;
  }  

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se cargan todos los componentes asociados a botones de opciones. En este
//   caso, unicamente existira un boton de salida.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWTextReader::TLoadOptionsInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Vbles
  CGUICButton::sGUICButtonInitData CfgButton; // Cfg para el botones

  // Prefijo
  pParser->SetVarPrefix("Options.");

  // Se cargan datos
  CfgButton.szATButton = pParser->ReadString("ExitButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("ExitButton.");

  // Se completan datos
  CfgButton.ubDrawPlane = 2;
  CfgButton.ID = CGUIWTextReader::ID_EXITBUTTON;
  CfgButton.GUIEvents = GUIDefs::GUIC_SELECT | 
						GUIDefs::GUIC_UNSELECT |
						GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  CfgButton.pGUIWindow = this;

  // Se inicializa y retorna
  m_OptionsInfo.ExitButton.Init(CfgButton);
  return m_OptionsInfo.ExitButton.IsInitOk();
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
CGUIWTextReader::TLoadTitleInfo(CCBTEngineParser* const pParser)
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
  CfgLine.ID = CGUIWTextReader::ID_TITLE;
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
// - Finaliza componentes
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTextReader::EndComponents(void)
{
  // Finaliza imagen de fondo
  m_BackImgInfo.BackImg.End();

  // Finaliza titulo
  m_TitleInfo.Title.End();
  m_TitleInfo.FXBack.End();

  // Finaliza texto
  m_TextInfo.Text.End();
  m_TextInfo.FXBack.End();
  m_TextInfo.UpButton.End();
  m_TextInfo.DownButton.End();

  // Finaliza boton de salida
  m_OptionsInfo.ExitButton.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa la instancia cargando en el bloque de texto el archivo pasado por
//   parametro. En caso de que no se pueda localizar el archivo, no se
//   producira la activacion. Este hecho debera de comprobarse desde el 
//   exterior consultando dicho flag.
// Parametros:
// - szTitle. Titulo de la interfaz. No sera obligatorio.
// - szFileName. Nombre del archivo
// - pClient. Direccion del posible cliente al que se le notificara cuando
//   se abandone el interfaz.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTextReader::Active(const std::string& szTitle,
						const std::string& szFileName,
						iCGUIWindowClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(szFileName.size());

  // Si se logra preparar el texto y no se halla activa se procede a activar
  if (!Inherited::IsActive() && 
	  PrepareText(szFileName)) {
	// Establece titulo
	SetTitle(szTitle);
	
	// Se activan componentes
	ActiveComponents(true);
	
	// Se muetra cursor
	SYSEngine::GetGUIManager()->SetGUICursor(GUIManagerDefs::WINDOWINTERFAZ_CURSOR);

	// Se propaga
	Inherited::Active(pClient);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se desactiva
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTextReader::Deactive(void)
{
  // SOLO si instancia activa
  ASSERT(Inherited::IsInitOk());

  // Se desactiva si procede
  if (Inherited::IsActive()) {
	// Se desactivan componentes
	ActiveComponents(false);

	// Se propaga
	Inherited::Deactive();
  }
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
CGUIWTextReader::SetTitle(const std::string& szTitle)
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
// - Activa / desactiva componentes.
// Parametros:
// - bActive. Flag de activacion / desactivacion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTextReader::ActiveComponents(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Activa / desactiva componentes
  // Imagen de fondo
  m_BackImgInfo.BackImg.SetActive(bActive);
	
  // Texto
  m_TextInfo.Text.SetActive(bActive);
  m_TextInfo.UpButton.SetActive(bActive);
  m_TextInfo.DownButton.SetActive(bActive);

  // Opciones
  m_OptionsInfo.ExitButton.SetActive(bActive);

  // Titulo
  if (m_TitleInfo.bExistTitle) {
	m_TitleInfo.Title.SetActive(bActive);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encargara, dado el nombre del archivo szFileName, de obtener el handle
//   al mismo, insertar todos sus datos en el bloque de texto e inicializar el
//   bloque de texto. En caso de que no se pudiera hallar el archivo o este
//   rebasara el tamaño permitido (64ks) no se dejaria activar el interfaz.
// Parametros:
// - szFileName. Nombre del archivo.
// Devuelve:
// - Si se pudo preparar convenientemente true. En caso contrario false.
// Notas:
// - El metodo inicializara correcamente el bloque de texto pero NO lo 
//   activara.
//////////////////////////////////////////////////////////////////////////////
bool
CGUIWTextReader::PrepareText(const std::string& szFileName)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(szFileName.size());

  // Se toma instancia al manejador de ficheros
  iCFileSystem* const pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys);

  // Se obtiene handle al fichero, saliendo si no se pudiera
  const FileDefs::FileHandle hFile = pFileSys->Open(szFileName);
  if (!hFile) {
	return false;
  }

  // Se obtiene el tamaño del fichero, saliendo si tamaño no valido
  const dword udFileSize = pFileSys->GetFileSize(hFile);
  if (!udFileSize || udFileSize > 0xFFFF) {
	pFileSys->Close(hFile);
	return false;
  }

  // Se inicializa el componente de texto
  if (!m_TextInfo.Text.Init(m_TextInfo.CfgText)) {
	pFileSys->Close(hFile);
	return false;
  }

  // Se prepara buffer de lectura y se leen datos del fichero
  sbyte* psbReadBuff = new sbyte[udFileSize + 1];
  ASSERT(psbReadBuff);
  pFileSys->Read(hFile, psbReadBuff, udFileSize);
  psbReadBuff[udFileSize] = '\0';
  
  // Se procede a leer el archivo linea a linea y a insertarlo en el bloque
  dword       udOffset = 0; // Offset de lectura
  std::string szTextLine;   // Linea de texto a insertar
  while (udOffset < udFileSize) {
	// Se obtiene nueva linea y se inserta
	udOffset += 1 + SYSEngine::GetLine(psbReadBuff, 
									   szTextLine, 
									   udFileSize, 
									   udOffset, 
									   '\n');
	m_TextInfo.Text.InsertText(szTextLine, false);
  }
    
  // Lectura completa, se libera el buffer y handle al archivo
  delete[udFileSize + 1] psbReadBuff;
  pFileSys->Close(hFile);

  // Se retorna
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion de un evento producido sobre un componente
// Parametros:
// - GUICEvent. Evento
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTextReader::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Comprueba el tipo de evento y lo distribuye
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

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Gestiona el evento de seleccion / deseleccion de un componente
// Parametros:
// - IDComponent. Identificador del componente.
// - bSelect. Flag de seleccion / deseleccion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTextReader::OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
						  const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Selecciona / deselecciona el componente adecuado
  switch(IDComponent) {
	case CGUIWTextReader::ID_UPBUTTON: {
	  // Boton de scroll hacia arriba
	  m_TextInfo.UpButton.Select(bSelect);
	} break;

	case CGUIWTextReader::ID_DOWNBUTTON: {
	  // Boton de scroll hacia abajo
	  m_TextInfo.DownButton.Select(bSelect);
	} break;

	case CGUIWTextReader::ID_EXITBUTTON: {
	  // Boton de salida
	  m_OptionsInfo.ExitButton.Select(bSelect);
	} break;
  }; // ~ switch  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Gestiona la pulsacion sobre uno de los componentes.
// Parametros:
// - IDComponent. Identificador del componente.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTextReader::OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Logica asociada a la pulsacion
  switch(IDComponent) {
	case CGUIWTextReader::ID_UPBUTTON: {
	  // Boton de scroll hacia arriba
	  m_TextInfo.Text.MoveUp();
	} break;

	case CGUIWTextReader::ID_DOWNBUTTON: {
	  // Boton de scroll hacia abajo
	  m_TextInfo.Text.MoveDown();
	} break;

	case CGUIWTextReader::ID_EXITBUTTON: {
	  // Pulsacion de salida
  	  // Se notificara el evento al posible cliente asociado
	  Inherited::GUIWindowNotify(0);
	  // Se notifica al GUIManager que se Deactive el cuadro de dialogo
	  SYSEngine::GetGUIManager()->DeactiveTextReader();
	} break;
  }; // ~ switch  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dibuja los componentes
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWTextReader::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Dibuja si procede
  if (Inherited::IsActive()) {
	// Imagen de fondo
	m_BackImgInfo.BackImg.Draw();

	// Texto
	m_TextInfo.FXBack.Draw();
	m_TextInfo.Text.Draw();
	m_TextInfo.UpButton.Draw();
	m_TextInfo.DownButton.Draw();

	// Opciones
	m_OptionsInfo.ExitButton.Draw();

	// Titulo si procede
	if (m_TitleInfo.bExistTitle) {
	  m_TitleInfo.FXBack.Draw();
	  m_TitleInfo.Title.Draw();
	}  
  }
}