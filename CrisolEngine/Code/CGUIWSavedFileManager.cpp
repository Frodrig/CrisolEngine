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
// CGUIWBaseSavedFileManager.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWBaseSavedFileManager para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWSavedFileManager.h"

#include "SYSEngine.h"
#include "iCLogger.h"
#include "iCFileSystem.h"
#include "iCGUIManager.h"
#include "iCInputManager.h"
#include "iCWorld.h"
#include "iCFont.h"
#include "iCFontSystem.h"
#include "iCGameDataBase.h"
#include "CCBTEngineParser.h"
#include "CMemoryPool.h"
#include "FontDefs.h"

// Declaracion de estructuras forward
struct CGUIWBaseSavedFileManager::sSavedFileInfo {
  // Info asociada a un archivo guardado
  std::string   szFileName;     // Nombre del archivo
  CGUICLineText BaseDesc;       // Componente con la BaseDescripcion base asociada 
  std::string   szAreaName;     // Nombre del area del fichero seleccionado
  std::string   szPlayerName;   // Nombre del jugador
  std::string   szPlayerHealth; // Salud del jugador
  std::string   szWorldTime;    // Horario
  // Manejador de memoria
  static CMemoryPool m_MPool;
  static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
  static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); }   
};

// Manejador de memoria
CMemoryPool 
CGUIWBaseSavedFileManager::sSavedFileInfo::m_MPool(8, 
												   sizeof(CGUIWBaseSavedFileManager::sSavedFileInfo), 
												   true);

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
CGUIWBaseSavedFileManager::Init(void)
{
  // Se inicializa clase base
  Inherited::Init();

  // Se retorna resultado
  return Inherited::IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseSavedFileManager::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {
	// Finaliza componentes
	EndComponents();

	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina toda la carga de datos en los componentes
// Parametros:
// - szSection. Seccion donde localizar los datos.
// Devuelve:
// Notas:
// - Este metodo se esperara que sea llamado por las clases base.
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseSavedFileManager::TLoad(const std::string& szSection)
{
  // SOLO si parametros validos
  ASSERT(!szSection.empty());

  // Establece parser
  CCBTEngineParser * const pParser = SYSEngine::GetGameDataBase()->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
																		        szSection);
  
  // Carga imagen de fondo
  if (!TLoadBackImg(pParser)) {
	return false;
  }
  
  // Carga de datos asociados a la gestion de los archivos de partidas guardadas
  if (!TLoadSavedFileList(pParser)) {
	return false;
  }

  // Carga componentes asociados a la info general del fichero seleccionado  
  if (!TLoadSelectSavedFileDesc(pParser)) {
	return false;
  }

  // Todo correcto 
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la imagen de fondo, que podra ser animada.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseSavedFileManager::TLoadBackImg(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);
  
  // Se establecen datos de configuracion e inicializa
  CGUICBitmap::sGUICBitmapInitData BackImgCfg;
  BackImgCfg.ID = CGUIWBaseSavedFileManager::ID_BACKIMG;  
  pParser->SetVarPrefix("BackImage.");      
  BackImgCfg.BitmapInfo.szFileName = pParser->ReadString("FileName");  
  if (!m_BackImgInfo.BackImg.Init(BackImgCfg)) {
	return false;
  }

  // Se añaden las posibles animaciones asociadas
  word uwIt = 0;
  for (; uwIt < CGUICAnimBitmap::MAX_ANIMS; ++uwIt) {
	// Se lee la posible plantilla de animacion asociada a animacion
	std::string szVar;
	SYSEngine::PassToString(szVar, "Anim[%u].", uwIt);
	const std::string szAnim(pParser->ReadString(szVar + "AnimTemplate", false));
	
	// ¿Se leyo plantilla de animacion?
	if (pParser->WasLastParseOk()) {
	  // Si, se lee posicion y alpha y se añade a la imagen
	  // Nota: La posicion sera obligatoria, el valor alpha no.
	  m_BackImgInfo.BackImg.AddAnim(szAnim, 
								    pParser->ReadPosition(szVar), 
									pParser->ReadAlpha(szVar, false));
	} else {
	  // No, se abandona
	  break;
	}
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la informacion relativa a la navegacion por la lista de los archivos
//   de partidas guardadas.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseSavedFileManager::TLoadSavedFileList(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Establece prefijo
  pParser->SetVarPrefix("SavedFilesList.");

  // Carga datos asociados a la muestra de los ficheros
  // Posicion  
  m_SavedFileList.InitDrawPos = pParser->ReadPosition();

  // Numero maximo de archivos visibles
  m_SavedFileList.ubMaxVisibleFiles = pParser->ReadNumeric("NumberOfVisibleFiles");
  ASSERT(m_SavedFileList.ubMaxVisibleFiles);
  
  // Numero maximo de caracteres
  // Nota: Para leer el numero maximo de caracteres, se debera de acudir a la
  // seccion [SaverSavedFileManager]
  const std::string szActSection(pParser->GetSectionName());
  const std::string szVarPrefix(pParser->GetPrefixName());
  pParser->SetSection("[SaverSavedFileManager]");  
  ASSERT(pParser->IsSectionActive());
  pParser->SetVarPrefix("");  
  m_SavedFileList.uwMaxCharacters = pParser->ReadNumeric("SavedFileDesc.Input.MaxCharacters");
  ASSERT(m_SavedFileList.uwMaxCharacters);
  pParser->SetSection(szActSection.c_str());  
  pParser->SetVarPrefix(szVarPrefix.c_str());  

  // Se cargan los datos de color del FX
  m_SavedFileList.FXRGBColor = pParser->ReadRGBColor("File.FXBack.Select.");
  m_SavedFileList.FXAlpha = pParser->ReadAlpha("File.FXBack.Select.");
  
  // Configuracion base del texto de seleccion
  m_SavedFileList.BaseDescCfg.pGUIWindow = this;
  m_SavedFileList.BaseDescCfg.GUIEvents = GUIDefs::GUIC_SELECT |
										  GUIDefs::GUIC_UNSELECT |
										  GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  m_SavedFileList.BaseDescCfg.szFont = "Arial";
  m_SavedFileList.BaseDescCfg.ubDrawPlane = 0; 
  m_SavedFileList.BaseDescCfg.RGBUnselectColor = pParser->ReadRGBColor("File.Name.Unselect.");
  m_SavedFileList.BaseDescCfg.RGBSelectColor = pParser->ReadRGBColor("File.Name.Select.");
  
  // Se establecen datos comunes para los botones de navegacion
  CGUICButton::sGUICButtonInitData CfgButton;
  CfgButton.pGUIWindow = this;
  CfgButton.GUIEvents = GUIDefs::GUIC_SELECT |
						GUIDefs::GUIC_UNSELECT |
						GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  CfgButton.ubDrawPlane = 0;

  // Anchura del caracter mas ancho
  // Nota: Se considerara el caracter mas ancho la 'W' mayuscula
  iCFont* const pFont = SYSEngine::GetFontSystem()->GetFont(m_SavedFileList.BaseDescCfg.szFont);
  ASSERT(pFont);
  m_SavedFileList.uwMaxCharWidth = pFont->GetCharWidth('W');

  // Carga e inicializa UpButton
  pParser->SetVarPrefix("Options.");
  CfgButton.ID = CGUIWBaseSavedFileManager::ID_UPBUTTON;
  CfgButton.szATButton = pParser->ReadString("UpButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("UpButton.");
  if (!m_SavedFileList.UpButton.Init(CfgButton)) {
	return false;
  }

  // Carga e inicializa DownButton
  CfgButton.ID = CGUIWBaseSavedFileManager::ID_DOWNBUTTON;
  CfgButton.szATButton = pParser->ReadString("DownButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("DownButton.");
  if (!m_SavedFileList.DownButton.Init(CfgButton)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se obtiene la informacion general de los componentes que serviran para
//   mostrar la descripcion asociada al archivo 
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseSavedFileManager::TLoadSelectSavedFileDesc(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Establece prefijo
  pParser->SetVarPrefix("SelectSavedFileInfo.");

  // Establece informacion basica de la estructura de inicializacion
  CGUICLineText::sGUICLineTextInitData TextCfg;
  TextCfg.ID = CGUIWBaseSavedFileManager::ID_TEXTINFO;
  TextCfg.pGUIWindow = this;
  TextCfg.GUIEvents = GUIDefs::GUIC_NO_EVENT;
  TextCfg.szLine = "-";
  TextCfg.szFont = "Arial";
  TextCfg.ubDrawPlane = 0; 
  
  // Nombre del area
  TextCfg.RGBUnselectColor = pParser->ReadRGBColor("AreaName.");
  TextCfg.RGBSelectColor = TextCfg.RGBUnselectColor;
  TextCfg.Position = pParser->ReadPosition("AreaName.");
  if (!m_SelectSavedFileDesc.AreaName.Init(TextCfg)) {
	return false;
  }

  // Nombre del jugador
  TextCfg.RGBUnselectColor = pParser->ReadRGBColor("PlayerName.");
  TextCfg.RGBSelectColor = TextCfg.RGBUnselectColor;
  TextCfg.Position = pParser->ReadPosition("PlayerName.");
  if (!m_SelectSavedFileDesc.PlayerName.Init(TextCfg)) {
	return false;
  }

  // Salud del jugador
  TextCfg.RGBUnselectColor = pParser->ReadRGBColor("PlayerHealth.");
  TextCfg.RGBSelectColor = TextCfg.RGBUnselectColor;
  TextCfg.Position = pParser->ReadPosition("PlayerHealth.");
  if (!m_SelectSavedFileDesc.PlayerHealth.Init(TextCfg)) {
	return false;
  }

  // Hora en el universo de juego
  TextCfg.RGBUnselectColor = pParser->ReadRGBColor("WorldTime.");
  TextCfg.RGBSelectColor = TextCfg.RGBUnselectColor;
  TextCfg.Position = pParser->ReadPosition("WorldTime.");
  if (!m_SelectSavedFileDesc.WorldTime.Init(TextCfg)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza componentes.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseSavedFileManager::EndComponents(void)
{
  // Desactiva
  Deactive();

  // Libera lista
  ReleaseSavedFilesList();
  m_SavedFileList.FXBack.End();

  // Finaliza botones
  m_SavedFileList.UpButton.End();  
  m_SavedFileList.DownButton.End();  

  // Descripciones generales.
  m_SelectSavedFileDesc.AreaName.End();
  m_SelectSavedFileDesc.PlayerName.End();
  m_SelectSavedFileDesc.PlayerHealth.End();
  m_SelectSavedFileDesc.WorldTime.End();  

  // Finaliza imagen de fondo
  m_BackImgInfo.BackImg.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa componentes
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseSavedFileManager::Active(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Instancia no activa?
  if (!Inherited::IsActive()) {
	// Crea la lista de ficheros de partidas guardadas 
	// Nota: Los primeros componentes visibles se activaran en la llamada
	MakeSavedFilesList();
	
	// ¿Hay ficheros?
	if (!m_SavedFileList.Files.empty()) {
	  // Si, se activan desde la primera posicion y se selecciona el primero
	  SetSavedFileListPos(sSavedFileIt(m_SavedFileList.Files.begin(), 0), true);  
	  SetSavedFile((*m_SavedFileList.Files.begin())->BaseDesc.GetID());
	}
	
	// Activa resto componentes
	ActiveComponents(true);
	
	// Propaga
	Inherited::Active();
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
CGUIWBaseSavedFileManager::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Desactiva si procede
  if (Inherited::IsActive()) {
	// Libera la lista de ficheros de partidas guardadas
	// Nota: Los componentes se desactivaran en la llamada
	ReleaseSavedFilesList();

	// Desactiva componentes
	ActiveComponents(false);

	// Quita cursor
	SYSEngine::GetGUIManager()->SetGUICursor(GUIManagerDefs::NO_CURSOR);  

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
CGUIWBaseSavedFileManager::ActiveComponents(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Activa / desactiva componentes 
  m_BackImgInfo.BackImg.SetActive(bActive);
  m_SavedFileList.UpButton.SetActive(bActive);
  m_SavedFileList.DownButton.SetActive(bActive); 
  m_SelectSavedFileDesc.AreaName.SetActive(bActive); 
  m_SelectSavedFileDesc.PlayerName.SetActive(bActive); 
  m_SelectSavedFileDesc.PlayerHealth.SetActive(bActive); 
  m_SelectSavedFileDesc.WorldTime.SetActive(bActive); 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece entrada en componentes
// Parametros:
// - bActive. Flag de activacion / desactivacion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseSavedFileManager::SetInput(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Actua sobre componentes
  m_BackImgInfo.BackImg.SetInput(bActive);
  m_SavedFileList.UpButton.SetInput(bActive);
  m_SavedFileList.DownButton.SetInput(bActive);
  m_SelectSavedFileDesc.AreaName.SetInput(bActive);
  m_SelectSavedFileDesc.PlayerName.SetInput(bActive);
  m_SelectSavedFileDesc.PlayerHealth.SetInput(bActive);
  m_SelectSavedFileDesc.WorldTime.SetInput(bActive);

  // Propaga
  Inherited::SetInput(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo que controla la seleccion / deseleccion de un componente
// Parametros:
// - IDComponent. Identificador del componente.
// - bSelect. Flag de seleccion.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWBaseSavedFileManager::OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
									const bool bSelect)
{
  // Se comprueba el tipo de componente
  switch(IDComponent) {
	case CGUIWBaseSavedFileManager::ID_UPBUTTON: {
	  // Boton para subir
	  m_SavedFileList.UpButton.Select(bSelect);
	} break;

	case CGUIWBaseSavedFileManager::ID_DOWNBUTTON: {
	  // Boton para bajar
	  m_SavedFileList.DownButton.Select(bSelect);
	} break;
	
	default: {
	  // Se trata de un identificador de fichero
	  // Se obtiene iterador a nodo
	  SavedFileListIt It(FindSavedFileIt(IDComponent));
	  ASSERT((It != m_SavedFileList.Files.end()) != 0);
	  // ¿Se desea deseleccionar?
	  if (!bSelect) {
		// Si, solo se deseleccionara si el componente no es el establecido
		if (It != m_SavedFileList.FileSelectIt) {
		  (*It)->BaseDesc.Select(bSelect);	  
		}
	  } else {
		// No, se selecciona
		(*It)->BaseDesc.Select(bSelect);	  
	  }	  
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la pulsacion de un componente.
// Parametros:
// - IDComponent. Identificador del componente
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWBaseSavedFileManager::OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent)
{
  // Se comprueba el tipo de componente
  switch(IDComponent) {
	case CGUIWBaseSavedFileManager::ID_UPBUTTON: {
	  // Boton para subir
	  MoveUp();
	} break;

	case CGUIWBaseSavedFileManager::ID_DOWNBUTTON: {
	  // Boton para bajar
	  MoveDown();
	} break;
	
	default: {
	  // Se trata de un identificador de fichero
	  // Si, se establece el seleccionado
	  SetSavedFile(IDComponent);	  
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre en la carpeta save a traves de todos los archivos que sean de
//   partidas guardadas. Por cada uno de ellos, ira creando una entrada en la
//   lista.
// - Este metodo NO activara los componentes ni tampoco les asignara posiciones
//   validas. Esa tarea se realizara en el metodo Active y en los relacionados
//   con la navegacion para corregir posiciones.
// Parametros:
// Devuelve:
// - El numero de partidas guardadas que se encontraron.
// Notas:
//////////////////////////////////////////////////////////////////////////////
word 
CGUIWBaseSavedFileManager::MakeSavedFilesList(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se inicializa indice a componentes para descripciones base
  m_SavedFileList.uwIDBaseDescIdx = 0;  

  // Procede a recorrer en la carpeta hasta encontrar que no hay archivos
  word uwIt = 0;
  for (; uwIt < 0xFF; ++uwIt) {
	// Se crea nodo e inserta en lista si procede
	sSavedFileInfo* const pNode = MakeSavedFileInfoNode(uwIt);
	if (pNode) {	  
	  m_SavedFileList.Files.push_back(pNode);
	} else {
	  // No se hallo info, luego rompe el bucle
	  break;
	}
  }

  // Inicializa informaciones de iteradores
  m_SavedFileList.FileSelectIt = m_SavedFileList.Files.end();
  m_SavedFileIt.It = m_SavedFileList.Files.begin();
  m_SavedFileIt.uwIt = 0;

  // Retorna el numero de ficheros
  return m_SavedFileList.Files.size();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea el nodo de informacion asociado a un fichero de partida guardada
//   y lo retorna.
// Parametros:
// - uwIDFile. Identificador del fichero.
// Devuelve:
// - El nodo de informacion creado o NULL si no se hallo el fichero.
// Notas:
//////////////////////////////////////////////////////////////////////////////
CGUIWBaseSavedFileManager::sSavedFileInfo*
CGUIWBaseSavedFileManager::MakeSavedFileInfoNode(const word uwIDFile)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Existe sig. fichero de partida guardada?
  std::string szFileName;
  SYSEngine::PassToString(szFileName, "save\\save_%u.sav", uwIDFile);	
  iCFileSystem* const pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys);
  const FileDefs::FileHandle hFile = pFileSys->Open(szFileName);
  if (hFile) {
    // Si existe, luego se crea instancia a estructura
    sSavedFileInfo* const pNode = new sSavedFileInfo;
    ASSERT(pNode);  
	
    // Guarda nombre archivo
    pNode->szFileName = szFileName;	  
	
    // Si, obtiene descripcion base e inicializa componente
    dword udOffset = sizeof(byte) * 3;	  
	udOffset += pFileSys->ReadStringFromBinary(hFile, udOffset, m_SavedFileList.BaseDescCfg.szLine);
    m_SavedFileList.BaseDescCfg.ID = CGUIWBaseSavedFileManager::ID_BASEFILESAVED + m_SavedFileList.uwIDBaseDescIdx++;
    pNode->BaseDesc.Init(m_SavedFileList.BaseDescCfg);
    ASSERT(pNode->BaseDesc.IsInitOk());	  
	
    // Se procede a leer la descripcion general
    // Se lee el offset en donde comenzar a leer esta informacion
	dword udSavedFileGenDescOffset;
    pFileSys->Read(hFile, 
				   (sbyte *)(&udSavedFileGenDescOffset),
				   sizeof(dword),
				   udOffset);  
	udOffset = udSavedFileGenDescOffset;
	
	// Nombre del area
	udOffset += pFileSys->ReadStringFromBinary(hFile, udOffset, pNode->szAreaName);	  
	
	// Nombre del jugador
	udOffset += pFileSys->ReadStringFromBinary(hFile, udOffset, pNode->szPlayerName);	  	
	
	// Salud del jugador
	RulesDefs::sPairValue Health;  
	udOffset += pFileSys->Read(hFile, 
				               (sbyte *)(&Health),
					           sizeof(RulesDefs::sPairValue),
					           udOffset);  
	SYSEngine::PassToString(pNode->szPlayerHealth, "%d // %d", Health.swTemp, Health.swBase);	  
	
	// Valor horario
	byte ubHour;
	udOffset += pFileSys->Read(hFile, 
				               (sbyte *)(&ubHour),
					           sizeof(byte),
					           udOffset);  
	byte ubMinute;
	udOffset += pFileSys->Read(hFile, 
				               (sbyte *)(&ubMinute),
					           sizeof(byte),
					           udOffset);  
	SYSEngine::PassToString(pNode->szWorldTime, "%u::%uh", ubHour, ubMinute);	  
	
	// Cierra fichero y retorna el nodo
	pFileSys->Close(hFile);	 
	return pNode;
  }

  // No se hallo fichero
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera toda la informacion relativa a los nodos en la lista de 
//   ficheros de partidas guardadas
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseSavedFileManager::ReleaseSavedFilesList(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se desactivan componentes a partir de posicion actual
  SetSavedFileListPos(m_SavedFileIt, false);
 
  // Recorre la lista de ficheros borrando informacion
  SavedFileListIt It(m_SavedFileList.Files.begin());
  while (It != m_SavedFileList.Files.end()) {
	// Se borra nodo y se pasa a sig. iterador
	delete *It;
	It = m_SavedFileList.Files.erase(It);
  }

  // Se desvincula iterador a slot actual
  m_SavedFileList.FileSelectIt = m_SavedFileList.Files.end();
  m_SavedFileIt.It = m_SavedFileList.Files.end();
  m_SavedFileIt.uwIt = 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece la posicion de los componentes con las descripciones base de 
//   cada uno de los archivos que forman la lista de partidas guardadas. A la
//   vez que se establecen esas posiciones, se activiran los componentes.
//   Para establecer las posiciones, se tendran en cuenta la posicion en donde
//   se debera de comenzar a hacer, recibiendose un iterador a dicha posicion.
// Parametros:
// - SavedFileIt. Estructura con el par de iteradores a partir del cual
//   comenzar a establecer las posiciones en la lista de archivos.
// - bSet. Flag para saber si hay que activar o desactivar.
// Devuelve:
// Notas:
// - El calculo de las posiciones solo se llevara a cabo si hay activar.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseSavedFileManager::SetSavedFileListPos(const sSavedFileIt& SavedFileIt,
											   const bool bSet)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se procede a iterar, colocando y activando componentes
  sPosition DrawPos = m_SavedFileList.InitDrawPos;
  word uwIt = 0;
  SavedFileListIt It(SavedFileIt.It);
  while (It != m_SavedFileList.Files.end() && 
		 uwIt < m_SavedFileList.ubMaxVisibleFiles) {
	// ¿Se desea activar?
	if (bSet) {
	  // Si, luego se configuran posiciones con la descripcion base
	  (*It)->BaseDesc.SetXPos(DrawPos.swXPos);
	  (*It)->BaseDesc.SetYPos(DrawPos.swYPos);
	  DrawPos.swYPos += FontDefs::CHAR_PIXEL_HEIGHT;
	}

	// Se activan / desactivan componentes
	(*It)->BaseDesc.SetActive(bSet);

	// Sig. iteracion
	++It;
	++uwIt;
  }

  // Se guarda iterador al primer componente
  m_SavedFileIt = SavedFileIt;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Localiza el identificador asociado al archivo de la partida establecida
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
word 
CGUIWBaseSavedFileManager::GetIDSelectSavedFile(void)
{
  // SOLO si instancia inicializada y activada
  ASSERT(Inherited::IsInitOk());
  ASSERT(Inherited::IsActive());

  // Procede a buscar desde el comienzo
  word uwIt = 0;
  SavedFileListIt It(m_SavedFileList.Files.begin());
  for (; It != m_SavedFileList.Files.end(); ++It, ++uwIt) {
	// ¿Es la actual?
	if (It == m_SavedFileList.FileSelectIt) {
	  // Si, abandona bucle
	  break;
	}
  }

  // Retorna resultado
  ASSERT((It != m_SavedFileList.Files.end()) != 0);
  return uwIt;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se mueve hacia arriba en la lista de partidas guardadas
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseSavedFileManager::MoveUp(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Mueve hacia arriba si procede
  // Nota: Se podra si hay elementos en lista y no se esta en la primera pos.
  if (m_SavedFileIt.It != m_SavedFileList.Files.end() &&
	  m_SavedFileIt.It != m_SavedFileList.Files.begin()) {
	// Se desactivan componentes desde la pos. actual
	SetSavedFileListPos(m_SavedFileIt, false);

	// Se establece nueva posicion para iterador y se activa desde esa pos.
	--m_SavedFileIt.It;
	--m_SavedFileIt.uwIt;
	SetSavedFileListPos(m_SavedFileIt, true);

	// Establece el FXBack
	SetFXBack();		
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se mueve hacia abajo en la lista de partidas guardadas
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseSavedFileManager::MoveDown(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Mueve hacia abajo si procede
  // Nota: Se podra si hay elementos en lista y no se esta en la ultima pos.
  if (m_SavedFileIt.It != m_SavedFileList.Files.end() &&
	  m_SavedFileList.Files.size() - m_SavedFileIt.uwIt > m_SavedFileList.ubMaxVisibleFiles) {
	// Se desactivan componentes desde la pos. actual
	SetSavedFileListPos(m_SavedFileIt, false);

	// Se establece nueva posicion para iterador y se activa desde esa pos.
	++m_SavedFileIt.It;
	++m_SavedFileIt.uwIt;
	SetSavedFileListPos(m_SavedFileIt, true);

	// Establece el FXBack
	SetFXBack();	
  }
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Añade un nuevo fichero, teniendo en cuenta cual es el ultimo fichero
//   que se añadio.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWBaseSavedFileManager::AddSavedFile(void)
{
  // SOLO si instancia inicializada y activada
  ASSERT(Inherited::IsInitOk());
  ASSERT(Inherited::IsActive());

  // Se intenta crear nodo asociado al fichero
  sSavedFileInfo* const pNode = MakeSavedFileInfoNode(m_SavedFileList.Files.size());
  if (pNode) {
	// Nodo creado, se añade a la lista
	m_SavedFileList.Files.push_back(pNode);

	// ¿Es el unico nodo en lista?
	if (1 == m_SavedFileList.Files.size()) {
	  // Si, luego se activan elementos desde el comienzo y se selecciona el primero
	  SetSavedFileListPos(m_SavedFileIt, false);
	  SetSavedFileListPos(sSavedFileIt(m_SavedFileList.Files.begin(), 0), true);  
	  SetSavedFile((*m_SavedFileList.Files.begin())->BaseDesc.GetID());
	} else {
	  // No, luego se activan elementos desde posicion actual
	  SetSavedFileListPos(m_SavedFileIt, true);
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina el archivo en relacion con el seleccionado en la lista. Al
//   eliminar se volvera a situar todo al comienzo, seleccionando al primero
//   de todos los elementos.
// - Una vez que se borre un archivo, se debera de recorrer toda la lista de
//   los mismos, renombrando fisicamente si los nuevos nombres, en relacion al
//   numero de archivos, no coincide. Esto sera necesario para evitar 
//   incoherencias.
// Parametros:
// Devuelve:
// Notas:
// - Borrar un nodo hace que se vuelva al comienzo de la lista.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseSavedFileManager::RemoveSelectSavedFile(void)
{
  // SOLO si instancia inicializada y activa
  ASSERT(Inherited::IsInitOk());
  ASSERT(Inherited::IsActive());

  // ¿Hay algun nodo seleccionado?
  if (m_SavedFileList.FileSelectIt != m_SavedFileList.Files.end()) {
	// Si, se obtiene el nombre del fichero
	const std::string szRemoveFileName = (*m_SavedFileList.FileSelectIt)->szFileName;
	ASSERT(!szRemoveFileName.empty());

	// Se desactiva lista de ficheros y se borra el nodo
	SetSavedFileListPos(m_SavedFileIt, false);
	delete *m_SavedFileList.FileSelectIt;	
	m_SavedFileList.Files.erase(m_SavedFileList.FileSelectIt);
	m_SavedFileList.FileSelectIt = m_SavedFileList.Files.end();
	
	// Se borra fisicamente el fichero
	remove(szRemoveFileName.c_str());

	// Se recorre la lista entera renombrando los archivos si procede
	word uwIt = 0;
	SavedFileListIt It(m_SavedFileList.Files.begin());	
	for (; It != m_SavedFileList.Files.end(); ++It, ++uwIt) {
	  // Se forma el nombre
	  std::string szNewName;
  	  SYSEngine::PassToString(szNewName, "save\\save_%u.sav", uwIt);

	  // ¿NO coinciden?
	  if (0 != szNewName.compare((*It)->szFileName)) {
		// Se renombra fisicamente y se guarda
		rename((*It)->szFileName.c_str(), szNewName.c_str());
		(*It)->szFileName = szNewName;
	  }
	}

	// ¿Hay ficheros?
	if (!m_SavedFileList.Files.empty()) {
	  // Si, se activan desde la primera posicion y se selecciona el primero
	  SetSavedFileListPos(sSavedFileIt(m_SavedFileList.Files.begin(), 0), true);  
	  SetSavedFile((*m_SavedFileList.Files.begin())->BaseDesc.GetID());
	} else {
	  // No, se desvinculan interadores
	  m_SavedFileList.FileSelectIt = m_SavedFileList.Files.end();
	  m_SavedFileIt.It = m_SavedFileList.Files.end();
	  m_SavedFileIt.uwIt = 0;
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece la partida guardada cuyo identificador sea uwIDSavedFile. A la
//   hora de establecerla, se asociara el iterador de partida guardada actual
//   y su descripcion en pantalla. Todas las operaciones que se realicen
//   apuntaran entonces a dicho archivo.
// Parametros:
// - IDComponent. Identificador del componente.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseSavedFileManager::SetSavedFile(const GUIDefs::GUIIDComponent& IDComponent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT((IDComponent >= CGUIWBaseSavedFileManager::ID_BASEFILESAVED) != 0);
  
  // Se localiza al nodo con dicho identificador
  const SavedFileListIt It(FindSavedFileIt(IDComponent));
  ASSERT((It != m_SavedFileList.Files.end()) != 0);

  // ¿Es distinto que el actual?
  if (It != m_SavedFileList.FileSelectIt) {	
	// Si, se deselecciona el previo si lo hubiera y se guarda el nuevo
	if (m_SavedFileList.Files.end() != m_SavedFileList.FileSelectIt) {
	  (*m_SavedFileList.FileSelectIt)->BaseDesc.Select(false);
	}
	m_SavedFileList.FileSelectIt = It;
		
	// Se procede a situar la descripcion general del archivo seleccionado  
	m_SelectSavedFileDesc.AreaName.ChangeText((*It)->szAreaName);
	m_SelectSavedFileDesc.PlayerName.ChangeText((*It)->szPlayerName);
	m_SelectSavedFileDesc.PlayerHealth.ChangeText((*It)->szPlayerHealth);
	m_SelectSavedFileDesc.WorldTime.ChangeText((*It)->szWorldTime);
  
	// Si el componente establecido no estuviera seleccionado, se seleccionara
	if (!(*m_SavedFileList.FileSelectIt)->BaseDesc.IsSelect()) {
	  (*m_SavedFileList.FileSelectIt)->BaseDesc.Select(true);
	}
  }
  
  // Se establece FXBack
  SetFXBack();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia el FXBack al archivo de partida guardada establecido SOLO si 
//   dicho archivo esta viendose en pantalla. En caso contrario, finalizara
//   eL FXBack si no lo estuviera.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWBaseSavedFileManager::SetFXBack(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Hay algun archivo de partida guardada establecido?
  if (m_SavedFileList.FileSelectIt != m_SavedFileList.Files.end()) {
	// ¿Es la partida guardada visible?
	if ((*m_SavedFileList.FileSelectIt)->BaseDesc.IsActive()) {
	  // Se (re)inicializa el FXBack
	  sPosition DrawPos((*m_SavedFileList.FileSelectIt)->BaseDesc.GetXPos() - 2, 
						(*m_SavedFileList.FileSelectIt)->BaseDesc.GetYPos() - 2);
	  m_SavedFileList.FXBack.Init(DrawPos,
								  0,
								  m_SavedFileList.uwMaxCharacters * m_SavedFileList.uwMaxCharWidth + 6,
								  (*m_SavedFileList.FileSelectIt)->BaseDesc.GetHeight() + 2,
								  m_SavedFileList.FXRGBColor,
								  m_SavedFileList.FXAlpha);
	  ASSERT(m_SavedFileList.FXBack.IsInitOk());
	} else {
	  // No, se finaliza el FXBack si estuviera activo
	  m_SavedFileList.FXBack.End();
	}
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Localiza el iterador asociado al nodo de ficheros salvados, cuya 
//   descripcion base tiene por identificador IDComponent.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
CGUIWBaseSavedFileManager::SavedFileListIt 
CGUIWBaseSavedFileManager::FindSavedFileIt(const GUIDefs::GUIIDComponent& IDComponent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Se localiza al nodo con dicho identificador
  SavedFileListIt It(m_SavedFileList.Files.begin());
  for (; It != m_SavedFileList.Files.end(); ++It) {
	// ¿Se hallo identificador?
	if ((*It)->BaseDesc.GetID() == IDComponent) {
	  // Si, abandona
	  break;
	}
  }
  ASSERT((It != m_SavedFileList.Files.end()) != 0);
  
  // Retorna
  return It;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dibujado de los componentes
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseSavedFileManager::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  ASSERT(Inherited::IsActive());

  // Dibuja imagen de fondo
  m_BackImgInfo.BackImg.Draw();

  // ¿Hay elementos en lista?
  if (!m_SavedFileList.Files.empty()) {
    // Si, se dibuja FXBack si procede
	if (m_SavedFileList.FXBack.IsInitOk()) {
	  m_SavedFileList.FXBack.Draw();
	}    

    // Se dibuja descripcion base de los archivos visibles
    SavedFileListIt It(m_SavedFileIt.It);	
    for (; It != m_SavedFileList.Files.end(); ++It) { 	  
	  // ¿Componente activo?
	  if ((*It)->BaseDesc.IsActive()) {
		// Si, se dibuja
		(*It)->BaseDesc.Draw();
	  } else {
		// No, abandona el bucle
		break;
	  }
    }

	// Se dibuja descripcion general del archivo establecido
	m_SelectSavedFileDesc.AreaName.Draw();
	m_SelectSavedFileDesc.PlayerName.Draw();
	m_SelectSavedFileDesc.PlayerHealth.Draw();
	m_SelectSavedFileDesc.WorldTime.Draw();
  }

  // Botones de navegacion por archivos
  m_SavedFileList.UpButton.Draw();
  m_SavedFileList.DownButton.Draw();  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia
// Parametros:
// - PrevInterfaz. Interfaz origen desde donde se activa este.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar
// - Solo se podra activar la interfaz desde el menu principal o desde el
//   menu de juego
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWLoaderSavedFileManager::Init(const GUIManagerDefs::eGUIWindowType& PrevInterfaz)
{
  // SOLO si parametros correctos
  ASSERT((PrevInterfaz == GUIManagerDefs::GUIW_MAINMENU ||
	      PrevInterfaz == GUIManagerDefs::GUIW_GAMEMENU) != 0);

  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("CGUIWLoadSavedFileManager::Init> Inicializando interfaz de carga de partidas.\n");
  #endif

  // Se toman interfaces
  m_pGUIManager = SYSEngine::GetGUIManager();  
  ASSERT(m_pGUIManager);

  // Se inicializa clase base
  if (Inherited::Init()) {	
	// Se cargan componentes
	if (TLoad()) {
	  // Se guarda interfaz origen
	  m_PrevInterfaz = PrevInterfaz;

	  // Todo correcto
	  #ifdef ENGINE_TRACE
		SYSEngine::GetLogger()->Write("                               | Ok.\n");
	  #endif
	  return true;
	}
  } 
  
  // Problemas
  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("                               | Error.\n");
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
CGUIWLoaderSavedFileManager::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CGUIWLoaderSavedFileManager::End> Finalizando interfaz de cargar partidas.\n");
	#endif

	// Finaliza componentes
	EndComponents();

	// Desvincula
	m_pGUIManager = NULL;

	// Propaga
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("                                | Ok.\n");
	#endif
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Procede a cargar datos asociados a los componentes
// Parametros:
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWLoaderSavedFileManager::TLoad(void)
{
  // Se cargan componentes de la clase base
  if (!Inherited::TLoad("[LoaderSavedFileManager]")) {
	return false;
  }

  // Establece parser
  CCBTEngineParser * const pParser = SYSEngine::GetGameDataBase()->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
																		        "[LoaderSavedFileManager]");
  ASSERT(pParser);
  
  // Se cargan opciones
  if (!TLoadOptions(pParser)) {
	return false;
  }

  // Se carga info de fade
  if (!TLoadFadeInfo(pParser)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se cargan opciones relativas a cargar y abandonar
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWLoaderSavedFileManager::TLoadOptions(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Establece prefijo
  pParser->SetVarPrefix("Options.");

  // Establece valores base para la configuracion de todos los botones
  CGUICButton::sGUICButtonInitData CfgButton;
  CfgButton.pGUIWindow = this;
  CfgButton.GUIEvents = GUIDefs::GUIC_SELECT |
						GUIDefs::GUIC_UNSELECT |
						GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  CfgButton.ubDrawPlane = 0;

  // Boton de cargar partida
  CfgButton.ID = CGUIWBaseSavedFileManager::ID_LOADBUTTON;
  CfgButton.szATButton = pParser->ReadString("LoadButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("LoadButton.");
  if (!m_Options.LoadButton.Init(CfgButton)) {
	return false;
  }

  // Boton de abandonar
  CfgButton.ID = CGUIWBaseSavedFileManager::ID_EXITBUTTON;
  CfgButton.szATButton = pParser->ReadString("ExitButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("ExitButton.");
  if (!m_Options.ExitButton.Init(CfgButton)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se carga la informacion relativa al fade
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWLoaderSavedFileManager::TLoadFadeInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Carga datos del FadeOut
  pParser->SetVarPrefix("Fade.");
  m_FadeInfo.RGBFadeOut = pParser->ReadRGBColor("Out.");
  m_FadeInfo.uwFadeOutSpeed = pParser->ReadNumeric("Out.Speed");
  ASSERT(m_FadeInfo.uwFadeOutSpeed);
  m_FadeInfo.uwFadeInSpeed = pParser->ReadNumeric("In.Speed");
  ASSERT(m_FadeInfo.uwFadeInSpeed);
  
  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza componentes.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWLoaderSavedFileManager::EndComponents(void)
{
  // Desactiva
  Deactive();

  // Finaliza componentes
  m_Options.LoadButton.End();
  m_Options.ExitButton.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa componentes. A la hora de activar, se realizara siempre un
//   FadeIn.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWLoaderSavedFileManager::Active(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Instancia no activa?
  if (!Inherited::IsActive()) {	
	// Se bloquea input y esconde cursor hasta final del fade
	// Nota: Se realizara ANTES de activar componentes para evitar que
	// llegue evento alguno
	SetInput(false);  
	m_pGUIManager->SetGUICursor(GUIManagerDefs::NO_CURSOR);  	  

	// Activa componentes
	ActiveComponents(true);

	// Se realiza el fade in
	Inherited::DoFadeIn(CGUIWLoaderSavedFileManager::ID_ENDACTIVEFADE,
						 m_FadeInfo.uwFadeInSpeed);

	// Propaga
	Inherited::Active();
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
CGUIWLoaderSavedFileManager::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Desactiva si procede
  if (Inherited::IsActive()) {
	// Desactiva componentes
	ActiveComponents(false);

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
CGUIWLoaderSavedFileManager::ActiveComponents(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Activa / desactiva componentes 
  m_Options.LoadButton.SetActive(bActive);
  m_Options.ExitButton.SetActive(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece entrada en componentes
// Parametros:
// - bActive. Flag de activacion / desactivacion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWLoaderSavedFileManager::SetInput(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Actua sobre componentes
  m_Options.ExitButton.SetInput(bActive);
  m_Options.LoadButton.SetInput(bActive);  

  // Propaga
  Inherited::SetInput(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe los eventos producidos en los componentes.
// Parametros:
// - CGUICEvent. Estructura con el evento producido.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWLoaderSavedFileManager::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se comprueba el tipo de evento
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
// - Metodo que controla la seleccion / deseleccion de un componente
// Parametros:
// - IDComponent. Identificador del componente.
// - bSelect. Flag de seleccion.
// Devuelve:
// Notas:
// - En caso de no capturar el identificador concreto, se propagara
//////////////////////////////////////////////////////////////////////////////
void
CGUIWLoaderSavedFileManager::OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
									  const bool bSelect)
{
  // Se comprueba el tipo de componente
  switch(IDComponent) {
	case Inherited::ID_LOADBUTTON: {
	  // Boton para cargar	  
	  // Nota: Solo se permitira seleccionarlo si hay algun de fichero seleccionado
	  if (bSelect) {
		if (Inherited::GetNumSavedFiles()) {
		  m_Options.LoadButton.Select(bSelect);
		}
	  } else {
		m_Options.LoadButton.Select(bSelect);
	  }
	} break;

	case Inherited::ID_EXITBUTTON: {
	  // Boton para cargar	  
	  m_Options.ExitButton.Select(bSelect);
	} break;

	default:
	  // Se propaga
	  Inherited::OnSelect(IDComponent, bSelect);	
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la pulsacion de un componente.
// Parametros:
// - IDComponent. Identificador del componente
// Devuelve:
// Notas:
// - En caso de capturar el identificador concreto, se propagara
//////////////////////////////////////////////////////////////////////////////
void
CGUIWLoaderSavedFileManager::OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent)
{
  // Se comprueba el tipo de componente
  switch(IDComponent) {
	case Inherited::ID_LOADBUTTON: {
	  // Boton para cargar
	  // Se realiza el fade out, se bloquea entrada y se esconde cursor
	  Inherited::DoFadeOut(CGUIWLoaderSavedFileManager::ID_ENDLOADFADE,
						   m_FadeInfo.RGBFadeOut,
						   m_FadeInfo.uwFadeOutSpeed);
	  SetInput(false);  
	  m_pGUIManager->SetGUICursor(GUIManagerDefs::NO_CURSOR);  	  	  
	} break;

	case Inherited::ID_EXITBUTTON: {
	  // Boton para salir
	  switch(m_PrevInterfaz) {
		case GUIManagerDefs::GUIW_GAMEMENU: {
		  // Se retorna a juego, retorno directo
		  m_pGUIManager->SetGameMenuWindow();
		} break;

		case GUIManagerDefs::GUIW_MAINMENU: {
		  // Se retorna al menu principal, realizando un fade
		  Inherited::DoFadeOut(CGUIWLoaderSavedFileManager::ID_ENDTOMAINMENUFADE,
						   m_FadeInfo.RGBFadeOut,
						   m_FadeInfo.uwFadeOutSpeed);
		  SetInput(false);  
		  m_pGUIManager->SetGUICursor(GUIManagerDefs::NO_CURSOR);  	  	  		  
		} break;
	  }; // ~ switch
	} break;

	default:
	  // Se propaga
	  Inherited::OnLeftClick(IDComponent);	
  }; // ~ switch
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notifica finalizacion de comando.
// Parametros:
// - IDCommand. Identificador del comando.
// - udInstant. Instante en se produce.
// - udExtraParam. Posible parametro extra.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWLoaderSavedFileManager::EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
										  const dword udInstant,
										  const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se comprueba codigo de fade recibido
  switch(IDCommand) {
	case CGUIWLoaderSavedFileManager::ID_ENDACTIVEFADE: {
	  // Finalizo el fade in que sigue a la activacion de la interfaz
	  // Se desbloquea la entrada y muestra el cursor
	  SetInput(true);  
  	  m_pGUIManager->SetGUICursor(GUIManagerDefs::WINDOWINTERFAZ_CURSOR);  	  
	} break;

	case CGUIWLoaderSavedFileManager::ID_ENDLOADFADE: {
	  // Finalizo el fade que sigue a una peticion de carga de partida
	  // Se cambia de estado al de carga de juego
	  // Establece cursores de GUI
	  SYSEngine::SetGameLoadingState(Inherited::GetIDSelectSavedFile());
	} break;

	case CGUIWLoaderSavedFileManager::ID_ENDTOMAINMENUFADE: {
	  // Finalizo el fade que sigue a la peticion de salida, pero para volver
	  // al menu principal
	  // Se establece menu principal
	  m_pGUIManager->SetMainMenuWindow();
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWLoaderSavedFileManager::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Dibuja si procede
  if (Inherited::IsActive()) {
	// Clase base
	Inherited::Draw();

	// Opciones
	m_Options.LoadButton.Draw();
	m_Options.ExitButton.Draw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar
// - Solo se podra activar la interfaz desde el menu de juego
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWSaverSavedFileManager::Init(void)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("CGUIWSaverSavedFileManager::Init> Inicializando interfaz de guardar partidas.\n");
  #endif

  // Se toman interfaces
  m_pGUIManager = SYSEngine::GetGUIManager();  
  ASSERT(m_pGUIManager);
  m_pInputManager = SYSEngine::GetInputManager();
  ASSERT(m_pInputManager);

  // Se inicializa clase base
  if (Inherited::Init()) {	
	// Se cargan componentes
	if (TLoad()) {
	  // Todo correcto
	  #ifdef ENGINE_TRACE
		SYSEngine::GetLogger()->Write("                                | Ok.\n");
	  #endif
	  return true;
	}
  } 
  
  // Problemas
  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("                                | Error.\n");
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
CGUIWSaverSavedFileManager::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CGUIWSaverSavedFileManager::End> Finalizando interfaz de guardar partidas.\n");
	#endif

	// Finaliza componentes
	EndComponents();

	// Desvincula
	m_pGUIManager = NULL;
	m_pInputManager = NULL;

	// Propaga
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("                               | Ok.\n");
	#endif
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Procede a cargar datos asociados a los componentes
// Parametros:
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWSaverSavedFileManager::TLoad(void)
{
  // Se cargan componentes de la clase base
  if (!Inherited::TLoad("[SaverSavedFileManager]")) {
	return false;
  }

  // Establece parser
  CCBTEngineParser * const pParser = SYSEngine::GetGameDataBase()->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
																		        "[SaverSavedFileManager]");

  // Se cargan opciones
  if (!TLoadOptions(pParser)) {
	return false;
  }

  // Se cargan datos asociados al input
  if (!TLoadDescInput(pParser)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se cargan opciones relativas a cargar y abandonar
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWSaverSavedFileManager::TLoadOptions(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Establece prefijo
  pParser->SetVarPrefix("Options.");

  // Establece valores base para la configuracion de todos los botones
  CGUICButton::sGUICButtonInitData CfgButton;
  CfgButton.pGUIWindow = this;
  CfgButton.GUIEvents = GUIDefs::GUIC_SELECT |
						GUIDefs::GUIC_UNSELECT |
						GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  CfgButton.ubDrawPlane = 0;

  // Boton de cargar partida
  CfgButton.ID = CGUIWBaseSavedFileManager::ID_SAVEBUTTON;
  CfgButton.szATButton = pParser->ReadString("SaveButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("SaveButton.");
  if (!m_Options.SaveButton.Init(CfgButton)) {
	return false;
  }

  // Boton de borrar partida
  CfgButton.ID = CGUIWBaseSavedFileManager::ID_REMOVEBUTTON;
  CfgButton.szATButton = pParser->ReadString("RemoveButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("RemoveButton.");
  if (!m_Options.RemoveButton.Init(CfgButton)) {
	return false;
  }

  // Boton de abandonar
  CfgButton.ID = CGUIWBaseSavedFileManager::ID_EXITBUTTON;
  CfgButton.szATButton = pParser->ReadString("ExitButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("ExitButton.");
  if (!m_Options.ExitButton.Init(CfgButton)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la informacion asociada al input de entrada
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWSaverSavedFileManager::TLoadDescInput(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Establece prefijo
  pParser->SetVarPrefix("SavedFileDesc.");

  // Carga datos asociados al texto de ayuda
  CGUICLineText::sGUICLineTextInitData TextCfg;
  TextCfg.ID = CGUIWSaverSavedFileManager::ID_TEXTINFO;
  TextCfg.pGUIWindow = this;
  TextCfg.GUIEvents = GUIDefs::GUIC_NO_EVENT;
  TextCfg.szFont = "Arial";
  TextCfg.ubDrawPlane = 0; 
  TextCfg.szLine = SYSEngine::GetGameDataBase()->GetStaticText(GameDataBaseDefs::ST_INTERFAZ_SAVEDFILEMANAGER_SAVEDFILEDESC);
  TextCfg.RGBUnselectColor = pParser->ReadRGBColor("InfoText.");
  TextCfg.RGBSelectColor = TextCfg.RGBUnselectColor;
  TextCfg.Position = pParser->ReadPosition("InfoText.");
  if (!m_DescInput.HelpText.Init(TextCfg)) {
	return false;
  }

  // Se cargan los datos del input
  CGUICInput::sGUICInputInitData InputCfg; 
  InputCfg.szFont = "Arial";
  InputCfg.ID = Inherited::ID_SAVEDFILEINPUT;
  InputCfg.pGUIWindow = this;
  InputCfg.GUIEvents = GUIDefs::GUIC_INPUT_ACCEPT_ACQUIRING |
					   GUIDefs::GUIC_INPUT_NOACCEPT_ACQUIRING;
  InputCfg.ubDrawPlane = 0;
  InputCfg.uwMaxCharacters = Inherited::GetSavedFileBaseDescMaxCharacters();
  InputCfg.RGBTextSelect = pParser->ReadRGBColor("Input.");
  InputCfg.RGBTextUnselect = InputCfg.RGBTextSelect;  
  InputCfg.Position = pParser->ReadPosition("Input.");  
  if (!m_DescInput.Input.Init(InputCfg)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza componentes.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWSaverSavedFileManager::EndComponents(void)
{
  // Desactiva
  Deactive();

  // Finaliza componentes
  m_Options.SaveButton.End();
  m_Options.RemoveButton.End();
  m_Options.ExitButton.End();
  m_DescInput.HelpText.End();
  m_DescInput.Input.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa componentes. 
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWSaverSavedFileManager::Active(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Instancia no activa?
  if (!Inherited::IsActive()) {	
	// Activa componentes
	ActiveComponents(true);

	// Se establece cursor
	m_pGUIManager->SetGUICursor(GUIManagerDefs::WINDOWINTERFAZ_CURSOR);  	  
	
	// Propaga
	Inherited::Active();
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
CGUIWSaverSavedFileManager::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Desactiva si procede
  if (Inherited::IsActive()) {
	// Desactiva componentes
	ActiveComponents(false);

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
CGUIWSaverSavedFileManager::ActiveComponents(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Activa / desactiva componentes 
  m_Options.SaveButton.SetActive(bActive);
  m_Options.RemoveButton.SetActive(bActive);
  m_Options.ExitButton.SetActive(bActive);

  // Los componentes de entrada NO se activaran desde este metodo, ya que lo
  // deberan de hacer SOLO cuando se vaya a grabar, sin embargo, si se dejara
  // desactivarlos desde aqui
  if (!bActive) {
	m_DescInput.HelpText.SetActive(bActive);
	m_DescInput.Input.SetActive(bActive);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece entrada en componentes
// Parametros:
// - bActive. Flag de activacion / desactivacion
// Devuelve:
// Notas:
// - No se trabajara con los componentes de entrada, ya que estos deberan de
//   activarse cuando el resto no lo haga, asi que se dejara que se traten
//   en el evento de pulsacion de grabar partida.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWSaverSavedFileManager::SetInput(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Actua sobre componentes
  m_Options.SaveButton.SetInput(bActive);
  m_Options.RemoveButton.SetInput(bActive);
  m_Options.ExitButton.SetInput(bActive);  
  
  // Propaga
  Inherited::SetInput(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe los eventos producidos en los componentes.
// Parametros:
// - CGUICEvent. Estructura con el evento producido.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWSaverSavedFileManager::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se comprueba el tipo de evento
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

	case GUIDefs::GUIC_INPUT_ACCEPT_ACQUIRING: {
	  // Se introdujo entrada
	  OnEndInput(true);
	} break;

	case GUIDefs::GUIC_INPUT_NOACCEPT_ACQUIRING: {
	  // NO se introdujo entrada
	  OnEndInput(false);
	} break;
  }; // ~ switch	  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo que controla la seleccion / deseleccion de un componente
// Parametros:
// - IDComponent. Identificador del componente.
// - bSelect. Flag de seleccion.
// Devuelve:
// Notas:
// - En caso de no capturar el identificador concreto, se propagara
//////////////////////////////////////////////////////////////////////////////
void
CGUIWSaverSavedFileManager::OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
									 const bool bSelect)
{
  // Se comprueba el tipo de componente
  switch(IDComponent) {
	case Inherited::ID_SAVEBUTTON: {
	  // Boton para salvar	  
	  m_Options.SaveButton.Select(bSelect);
	} break;

	case Inherited::ID_REMOVEBUTTON: {
	  // Boton para borrar
	  // Nota: La seleccion solo se permitira si existe un archivo seleccionado
	  if (bSelect) {
		if (Inherited::GetNumSavedFiles()) {
		  m_Options.RemoveButton.Select(bSelect);
		}
	  } else {
		m_Options.RemoveButton.Select(bSelect);
	  }
	} break;

	case Inherited::ID_EXITBUTTON: {
	  // Boton para cargar	  
	  m_Options.ExitButton.Select(bSelect);
	} break;

	default:
	  // Se propaga
	  Inherited::OnSelect(IDComponent, bSelect);	
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la pulsacion de un componente.
// Parametros:
// - IDComponent. Identificador del componente
// Devuelve:
// Notas:
// - En caso de capturar el identificador concreto, se propagara
//////////////////////////////////////////////////////////////////////////////
void
CGUIWSaverSavedFileManager::OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent)
{
  // Se comprueba el tipo de componente
  switch(IDComponent) {
	case Inherited::ID_SAVEBUTTON: {
	  // Boton para guardar	
	  // Se bloqueara la entrada y ocultara el cursor
	  SetInput(false);
	  ASSERT(m_pGUIManager);
	  m_pGUIManager->SetGUICursor(GUIManagerDefs::NO_CURSOR);

	  // Se habilitara el input (activacion, seleccion y entrada) y el texto descriptivo
	  m_DescInput.HelpText.SetActive(true);
	  m_DescInput.Input.SetActive(true);
	  m_DescInput.Input.SetInput(true);
	  m_DescInput.Input.Select(true);
	  m_DescInput.Input.AcquireInput(true);	  
	  ASSERT(m_DescInput.Input.IsAcquiringInput());	  
	  
	  // Se toma la posicion del raton
	  m_DescInput.LastMousePos = m_pInputManager->GetLastMousePos();
	} break;

	case Inherited::ID_REMOVEBUTTON: {
	  // Boton para borrar	
	  // Se borra el que este seleccionado
	  Inherited::RemoveSelectSavedFile();
	} break;

	case Inherited::ID_EXITBUTTON: {
	  // Boton para salir
	  // Se retorna a juego, retorno directo
	  m_pGUIManager->SetGameMenuWindow();	
	} break;

	default:
	  // Se propaga
	  Inherited::OnLeftClick(IDComponent);	
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se notifica cuando ha terminado la entrada de la descripcion para
//   guardar el archivo.
// Parametros:
// - bInputOk. ¿Se introdujo correctamente el input?
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWSaverSavedFileManager::OnEndInput(const bool bInputOk)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se bloquea la entrada en el componente de entrada
  m_DescInput.Input.SetInput(false);

  // Se desbloqueara entrada para todos los componentes salvo input
  SetInput(true);
  m_DescInput.Input.SetInput(false);
  m_DescInput.Input.Select(false);	  
  
  // Se mostrara cursor de GUI y se restaurara la anterior pos. del raton
  m_pGUIManager->SetGUICursor(GUIManagerDefs::WINDOWINTERFAZ_CURSOR);  
  m_pInputManager->SetMousePos(m_DescInput.LastMousePos);

  // ¿Se introdujo descripcion?
  if (bInputOk) {
	// Si, se graba archivo y se incorpora a la lista de ficheros
	SYSEngine::GetWorld()->SaveGame(Inherited::GetNumSavedFiles(), 
								    m_DescInput.Input.GetActInput());
	Inherited::AddSavedFile();

	// Se elimina valor de entrada
	m_DescInput.Input.SetActInput("");
  }  

  // Se desactivan componentes de entrada
  m_DescInput.Input.SetActive(false);
  m_DescInput.HelpText.SetActive(false);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWSaverSavedFileManager::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Dibuja si procede
  if (Inherited::IsActive()) {
	// Clase base
	Inherited::Draw();

	// Opciones
	m_Options.SaveButton.Draw();
	m_Options.RemoveButton.Draw();
	m_Options.ExitButton.Draw();

	// Input	
	m_DescInput.Input.Draw();
	m_DescInput.HelpText.Draw();
  }
}
