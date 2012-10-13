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
// CGUIWPlayerProfile.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWPlayerProfile.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWPlayerProfile.h"

#include "SYSEngine.h"
#include "iCGraphicSystem.h"
#include "iCWorld.h"
#include "iCGUIManager.h"
#include "iCInputManager.h"
#include "iCGameDataBase.h"
#include "iCLogger.h"
#include "GUIDefs.h"
#include "ResDefs.h"
#include "CItem.h"
#include "CSprite.h"
#include "CCriature.h"
#include "CPlayer.h"
#include "CCBTEngineParser.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar.
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWPlayerProfile::Init(void)
{
  // ¿Se quiere reinicializar?
  if (Inherited::IsInitOk()) { 
	return false; 
  }

  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("CGUIWPlayerProfile::Init> Inicializando interfaz para hoja del personaje.\n");    
  #endif

  // Inicializa clase base
  if (Inherited::Init()) {
	// Obtiene interfaz a otros subsistemas
	m_pWorld = SYSEngine::GetWorld();
	ASSERT(m_pWorld);  
	m_pInputManager = SYSEngine::GetInputManager();
	ASSERT(m_pInputManager);
	m_pGraphicSystem = SYSEngine::GetGraphicSystem();
	ASSERT(m_pGraphicSystem);
	m_pGUIManager = SYSEngine::GetGUIManager();
	ASSERT(m_pGUIManager);
	m_pGDBase = SYSEngine::GetGameDataBase();
	ASSERT(m_pGDBase);

	// Se cargan datos
	if (TLoad()) {	  	  
	  // Todo correcto
	  #ifdef ENGINE_TRACE
	    SYSEngine::GetLogger()->Write("                        | Ok.\n");    
	  #endif
	  return true;
	}	
  }

  // Hubo problemas
  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("                        | Error.\n");    
  #endif
  End();
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la carga de informacion asociada a componentes asi como su
//   inicializacion.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - De la clase base se cargara la informacion de como sera dispuesto el
//   contenido del inventario del jugador. Sin embargo, de forma local, sera
//   necesario cargar los botones de scroll asociados al inventario
///////////////////////////////////////////////////////////////////////////////
bool
CGUIWPlayerProfile::TLoad(void)
{
  // Obtiene parser
  CCBTEngineParser* const pParser = m_pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
													        "[PlayerProfileWindow]");
  ASSERT(pParser);

  // Procede a cargar componentes de clase base
  pParser->SetVarPrefix("Inventory.");
  if (Inherited::TLoad(pParser, 0, false)) {	
	// Imagen de fondo
	if (!TLoadBackImg(pParser)) { 
	  return false; 
	}
	
	// Slots de inventario
	if (!TLoadInventory(pParser)) { 
	  return false; 
	}

	// Consola
	if (!TLoadConsole(pParser)) { 
	  return false; 
	}

	// Sintomas
	if (!TLoadSymptomsInfo(pParser)) { 
	  return false; 
	}

	// Slots de equipamiento
	if (!TLoadEquipmentSlots(pParser)) { 
	  return false; 
	}

	// Retrato
	if (!TLoadPortraitInfo(pParser)) { 
	  return false; 
	}

	// Habilidades
	if (!TLoadHabilitiesInfo(pParser)) { 
	  return false; 
	}

	// Boton de salida
	if (!TLoadExitInfo(pParser)) { 
	  return false; 
	}

	// Atributos
	if (!TLoadAttribInfo(pParser)) { 
	  return false; 
	}  

	// Valores de texto flotante
	if (!TLoadFloatingTextValues(pParser)) {
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
// - Carga e inicializa imagen de fondo
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWPlayerProfile::TLoadBackImg(CCBTEngineParser* const pParser)
{
  // Vbles
  CGUICBitmap::sGUICBitmapInitData CfgData; // Cfg de inicializacion

  // Se establece prefijo
  pParser->SetVarPrefix("BackImage.");
  
  // Se cargan datos
  CfgData.BitmapInfo.szFileName = pParser->ReadString("FileName");

  // Se completan datos
  CfgData.ID = CGUIWPlayerProfile::ID_BACKIMG;
  CfgData.BitmapInfo.WidthTextureDim = GraphDefs::TEXTURE_DIM_32;
  CfgData.BitmapInfo.HeightTextureDim = GraphDefs::TEXTURE_DIM_32;
  CfgData.BitmapInfo.Bpp = GraphDefs::BPP_16;
  CfgData.BitmapInfo.ABpp = GraphDefs::ALPHA_BPP_0;
  
  // Se inicializa la imagen de fondo
  if (!m_BackImgInfo.BackImg.Init(CfgData)) {
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
// - Carga datos asociados al inventario. Unicamente los botones de scroll
//   porque el resto de datos ya habran sido cargados por la clase base.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWPlayerProfile::TLoadInventory(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Prefijo
  pParser->SetVarPrefix("Inventory.");    

  // Vbles
  CGUICButton::sGUICButtonInitData  CfgButton; // Cfg de botones de scroll

  // Se establece funcion comun en las estructura de inicializacion
  // Botones de scroll
  CfgButton.pGUIWindow = this;
  CfgButton.GUIEvents = GUIDefs::GUIC_SELECT | 
  					    GUIDefs::GUIC_UNSELECT |
					    GUIDefs::GUIC_BUTTONLEFT_PRESSED;

  // Scroll hacia arriba
  // Se cargan datos
  CfgButton.szATButton = pParser->ReadString("Options.UpButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("Options.UpButton.");
  // Se completan y se inicializa
  CfgButton.ID = CGUIWPlayerProfile::ID_INV_BUTTON_UP;
  if (!m_InventoryInfo.UpButton.Init(CfgButton)) {
	return false;
  }

  // Scroll hacia abajo
  // Se cargan datos
  CfgButton.szATButton = pParser->ReadString("Options.DownButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("Options.DownButton.");
  // Se completan y se inicializa
  CfgButton.ID = CGUIWPlayerProfile::ID_INV_BUTTON_DOWN;
  if (!m_InventoryInfo.DownButton.Init(CfgButton)) {
	return false;
  }

  // Todo correcto
  return true;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga e inicializa la consola
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CGUIWPlayerProfile::TLoadConsole(CCBTEngineParser* const pParser)
{  
  // SOLO si parametros validos
  ASSERT(pParser);

  // Vbles  
  CGUICBlockText::sGUICBlockTextInitData CfgBlockText; // Cfg para bloque de texto
  
  // Prefijo
  pParser->SetVarPrefix("Console.");

  // Consola
  CfgBlockText.RGBUnselectColor = pParser->ReadRGBColor("Text.");
  CfgBlockText.RGBSelectColor = CfgBlockText.RGBUnselectColor;
  CfgBlockText.Position = pParser->ReadPosition();
  CfgBlockText.uwWidthJustify = pParser->ReadNumeric("Width");
  CfgBlockText.uwNumLines = pParser->ReadNumeric("NumLines");  
  CfgBlockText.szWAVBipSound = pParser->ReadString("WAVBipFileName", false);
  // Se completan datos e inicializa
  CfgBlockText.szFont = "Arial";  
  CfgBlockText.uwCapacity = 1024;
  CfgBlockText.bCanReutilice = true;
  CfgBlockText.ID = CGUIWPlayerProfile::ID_CONSOLE;
  if (!m_ConsoleInfo.Text.Init(CfgBlockText)) { 
	return false; 
  }

  // FXBack de la consola
  const sPosition FXBackDrawPos(m_ConsoleInfo.Text.GetXPos() - 4,
								m_ConsoleInfo.Text.GetYPos() - 2);
  const word uwFXBackHeight = FontDefs::CHAR_PIXEL_HEIGHT * CfgBlockText.uwNumLines;
  if (!m_ConsoleInfo.FXBack.Init(FXBackDrawPos, 
								 0, 
								 CfgBlockText.uwWidthJustify + 4,
								 uwFXBackHeight,
								 pParser->ReadRGBColor("FXBack."),
								 pParser->ReadAlpha("FXBack."))) {
	return false;
  }

  // Vbles
  CGUICButton::sGUICButtonInitData CfgButton; // Cfg para botones de scroll

  // Se establecen datos comunes
  CfgButton.pGUIWindow = this;
  CfgButton.GUIEvents = GUIDefs::GUIC_SELECT |
						GUIDefs::GUIC_UNSELECT |
						GUIDefs::GUIC_BUTTONRIGHT_PRESSED |
						GUIDefs::GUIC_BUTTONLEFT_PRESSED;

  // Boton subir texto  
  CfgButton.szATButton = pParser->ReadString("Options.UpButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("Options.UpButton.");
  // Se completan datos e inicializa
  CfgButton.ID = CGUIWPlayerProfile::ID_CONSOLE_BUTTON_UP;
  if (!m_ConsoleInfo.ButtonUp.Init(CfgButton)) { 
	return false; 
  }
  
  // Boton bajar texto
  CfgButton.szATButton = pParser->ReadString("Options.DownButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("Options.DownButton.");  
  // Se completan datos e inicializa
  CfgButton.ID = CGUIWPlayerProfile::ID_CONSOLE_BUTTON_DOWN;
  if (!m_ConsoleInfo.ButtonDown.Init(CfgButton)) { 
	return false; 
  }  

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga e inicializa los datos referidos a los sintomas
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CGUIWPlayerProfile::TLoadSymptomsInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Vbles
  CGUICButton::sGUICButtonInitData CfgData; // Cfg para los iconos
  std::string                      szTmp;   // Vble temporal

  // Establecimiento de valores comunes
  CfgData.pGUIWindow = this;
  CfgData.GUIEvents = GUIDefs::GUIC_SELECT | 
  				      GUIDefs::GUIC_UNSELECT;

  // Procede a inicializar sintomas
  pParser->SetVarPrefix("Symptoms.");
  m_SymptomsInfo.uwNumSymptoms = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_SYMPTOM);
  byte ubIt = 0;
  for (; ubIt < m_SymptomsInfo.uwNumSymptoms; ++ubIt) {
	// Se carga informacion
	SYSEngine::PassToString(szTmp, 
							"Symptom[%s].",
							m_pGDBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_SYMPTOM,
																		  ubIt).c_str());

	CfgData.szATButton = pParser->ReadString(szTmp + "AnimTemplate");
	CfgData.Position = pParser->ReadPosition(szTmp);
	// Se completan datos e inicializan
	CfgData.ID = CGUIWPlayerProfile::ID_SYMPTOM_00 + ubIt;
	if (!m_SymptomsInfo.Symptoms[ubIt].Init(CfgData)) {
	  return false;
	}
  }  

  // Se toma el alpha asociado a lo sintomas no presentes si procede
  if (m_SymptomsInfo.uwNumSymptoms) {
	m_SymptomsInfo.NoPresentAlpha = pParser->ReadAlpha("NoPresent.");
  }
  
  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga e inicializa los datos referidos a los slots de equipamiento
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWPlayerProfile::TLoadEquipmentSlots(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Prefijo
  pParser->SetVarPrefix("EquipmentSlots.");

  // Flag de uso de nombres y color si procede
  m_EquipSlotsInfo.bUseSlotsNames = pParser->ReadFlag("DrawFormat.Names.UseFlag");  
  
  // Vbles
  CGUICSpriteSelector::sGUICSpriteSelectorInitData CfgSlot; // Cfg de selector
  CGUICLineText::sGUICLineTextInitData             CfgName; // Cfg para nombres
  std::string                                      szTmp;   // Vble temporal

  // Se inicializan datos basicos
  // Selector de slot  
  CfgSlot.GUIEvents = GUIDefs::GUIC_SELECT |
					  GUIDefs::GUIC_UNSELECT |					
					  GUIDefs::GUIC_BUTTONLEFT_PRESSED |
					  GUIDefs::GUIC_BUTTONRIGHT_PRESSED;
  CfgSlot.pGUIWindow = this;
  CfgSlot.bSelectOnlyWithSprite = false;  
  CfgSlot.RGBUnselect = pParser->ReadRGBColor("DrawFormat.FXBack.Unselect.");
  CfgSlot.AlphaUnselect = pParser->ReadAlpha("DrawFormat.FXBack.Unselect.");
  CfgSlot.RGBSelect = pParser->ReadRGBColor("DrawFormat.FXBack.Select.");
  CfgSlot.AlphaSelect = pParser->ReadAlpha("DrawFormat.FXBack.Select.");
  // Nombre
  CfgName.szFont = "Arial";
  if (m_EquipSlotsInfo.bUseSlotsNames) {
	CfgName.RGBUnselectColor = pParser->ReadRGBColor("DrawFormat.Names.Unselect.");
	CfgName.RGBSelectColor = pParser->ReadRGBColor("DrawFormat.Names.Select.");
  }
  
  // Se inicializan los slots donde se alojaran los items que el jugador
  // lleve equipados, asi como los componentes con los nombres de estos solo
  // si esta levantado el flag apropiado
  byte ubIt = 0;
  const word uwNumSlots = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT);
  for (; ubIt < uwNumSlots; ++ubIt) {
	// Se carga informacion
	SYSEngine::PassToString(szTmp, 
							"Slot[%s].",
							m_pGDBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT,
																		  ubIt).c_str());	
	CfgSlot.Position = pParser->ReadPosition(szTmp);
	// Se completa e inicializa
	CfgSlot.ID = CGUIWPlayerProfile::ID_EQUIPMENTSLOT_00 + ubIt;
	if (!m_EquipSlotsInfo.Slots[ubIt].Init(CfgSlot)) {
	  return false;
	}
	
	// ¿Se desea usar nombres para los slots?
	if (m_EquipSlotsInfo.bUseSlotsNames) {
	  // Se carga informacion
	  CfgName.Position = pParser->ReadPosition(szTmp + "Name.");
	  // Se completa e inicializa
	  CfgName.ID = CGUIWPlayerProfile::ID_EQUIPMENTSLOT_NAME_00 + ubIt;
	  CfgName.szLine = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_EQUIPMENT_SLOT,
												ubIt);
	  if (!m_EquipSlotsInfo.Names[ubIt].Init(CfgName)) {
		return false;
	  }
	}	
  } 
  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga e inicializa los datos referidos al retrato
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CGUIWPlayerProfile::TLoadPortraitInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Vbles
  CGUICSpriteSelector::sGUICSpriteSelectorInitData CfgData; // Cfg para inicializar

  // Se carga informacion
  pParser->SetVarPrefix("PlayerPortrait.");  
  CfgData.Position = pParser->ReadPosition();
  CfgData.RGBUnselect = pParser->ReadRGBColor("FXBack.Unselect.");
  CfgData.AlphaUnselect = pParser->ReadAlpha("FXBack.Unselect.");
  CfgData.RGBSelect = pParser->ReadRGBColor("FXBack.Select.");
  CfgData.AlphaSelect = pParser->ReadAlpha("FXBack.Select.");

  // Se completan datos
  CfgData.ID = CGUIWPlayerProfile::ID_Portrait;
  CfgData.GUIEvents = GUIDefs::GUIC_SELECT |
					  GUIDefs::GUIC_BUTTONLEFT_PRESSED |
					  GUIDefs::GUIC_UNSELECT;
  CfgData.pGUIWindow = this;
    
  // Inicializa y retorna
  m_PortraitInfo.PortraitSlot.Init(CfgData);
  return m_PortraitInfo.PortraitSlot.IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga e inicializa los datos referidos a las habilidades
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - Sera necesario guardar las posiciones en donde se deberan de disponer
//   las habilidades segun estas vayan apareciendo en el jugador. Esto es
//   asi, ya que para disponerlas en pantalla, sera necesario tener la 
//   instancia al jugador y esta solo vendra a traves del metodo active.
//   Inicialmente, la posicion de todos los botones sera de 0.
///////////////////////////////////////////////////////////////////////////////
bool
CGUIWPlayerProfile::TLoadHabilitiesInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Vbles
  CGUICButton::sGUICButtonInitData CfgData; // Datos de configuracion
  std::string                      szTmp;   // Vble temporal
  
  // Se establecen datos de configuracion comunes
  CfgData.pGUIWindow = this;
    
  // Prefijo
  pParser->SetVarPrefix("Habilities.");
  
  // Inicializa los botones asociados con las habilidades
  m_HabilitiesInfo.uwNumHabilities = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY);  
  byte ubIt = 0;
  for (; ubIt < m_HabilitiesInfo.uwNumHabilities; ++ubIt) {	
	// Se cargan datos
	SYSEngine::PassToString(szTmp,
						    "Hability[%s].",
							m_pGDBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_HABILITY,
																		  ubIt).c_str());
	CfgData.szATButton = pParser->ReadString(szTmp + "AnimTemplate");
	CfgData.Position = pParser->ReadPosition(szTmp);
	// Se completan datos e inicializa
	CfgData.ID = CGUIWPlayerProfile::ID_HABILITY_00 + ubIt;
	if (!m_HabilitiesInfo.Habilities[ubIt].Init(CfgData)) {
	  return false;
	}	
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga e inicializa los datos referidos al boton de salida
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWPlayerProfile::TLoadExitInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Vbles
  CGUICButton::sGUICButtonInitData CfgData; // Datos de configuracion

  // Se cargan datos
  pParser->SetVarPrefix("Options.");
  CfgData.szATButton = pParser->ReadString("ExitButton.AnimTemplate");
  CfgData.Position = pParser->ReadPosition("ExitButton.");

  // Se completan datos
  CfgData.ID = CGUIWPlayerProfile::ID_EXIT_BUTTON;
  CfgData.GUIEvents = GUIDefs::GUIC_SELECT |
					  GUIDefs::GUIC_UNSELECT |
					  GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  CfgData.pGUIWindow = this;
  
  // Se inicializa y retorna
  m_ExitInfo.ExitButton.Init(CfgData);
  return m_ExitInfo.ExitButton.IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga e inicializa los datos referidos a los atributos.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CGUIWPlayerProfile::TLoadAttribInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Prefijo
  pParser->SetVarPrefix("Attributes.");

  // Vbles
  CGUICLineText::sGUICLineTextInitData           CfgName;       // Cfg para los nombres
  CGUICLineText::sGUICLineTextInitData			 CfgValue;      // Cfg para los valores
  CGUICPercentageBar::sGUICPercentageBarInitData CfgPercentage; // Cfg de porcentaje
  std::string                                    szTmp;         // Vble temporal

  // Establecimiento de valores generales para los cfgs
  // Nombres  
  CfgName.RGBUnselectColor = pParser->ReadRGBColor("DrawFormat.Names.");
  CfgName.RGBSelectColor = CfgName.RGBUnselectColor;
  CfgName.ID = CGUIWPlayerProfile::ID_ATTRIB_TEXT;
  CfgName.szFont = "Arial";
  // Valores  
  CfgValue.RGBUnselectColor = pParser->ReadRGBColor("DrawFormat.Values.");
  CfgValue.RGBSelectColor = CfgValue.RGBUnselectColor;
  CfgValue.ID = CGUIWPlayerProfile::ID_ATTRIB_NUM_VALUE;
  CfgValue.szFont = "Arial";
  // Porcentaje
  CfgPercentage.ID = CGUIWPlayerProfile::ID_ATTRIB_PERCENTAGE;
  CfgPercentage.bDrawHoriz = pParser->ReadFlag("DrawFormat.PercentageBars.HorizontalDrawFlag");
  CfgPercentage.uwLenght = pParser->ReadNumeric("DrawFormat.PercentageBars.Lenght");
  CfgPercentage.uwWidth = pParser->ReadNumeric("DrawFormat.PercentageBars.Width");
  CfgPercentage.Alpha = pParser->ReadAlpha("DrawFormat.PercentageBars.");

  // Nombre
  CfgName.Position = pParser->ReadPosition("Player.Name.");
  if (!m_AttribInfo.Name.Text.Init(CfgName)) {	
	return false;
  }  
    
  // Genero / Tipo / Clase
  CfgName.Position = pParser->ReadPosition("GenreTypeClass.Name.");
  if (!m_AttribInfo.GenreTypeClass.Text.Init(CfgName)) {
	return false;
  }
    
  // Nivel
  CfgName.szLine = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_LEVEL);
  CfgName.Position = pParser->ReadPosition("Level.Name.");
  if (!m_AttribInfo.Level.Text.Init(CfgName)) {
	return false;
  }  
  CfgValue.Position = pParser->ReadPosition("Level.Value.");
  if (!m_AttribInfo.Level.Numeric.Init(CfgValue)) {
	return false;
  }
  
  // Experiencia
  CfgName.szLine = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_EXPERIENCE);
  CfgName.Position = pParser->ReadPosition("Experience.Name.");
  if (!m_AttribInfo.Experience.Text.Init(CfgName)) {
	return false;
  }  
  CfgValue.Position = pParser->ReadPosition("Experience.Value.");
  if (!m_AttribInfo.Experience.Numeric.Init(CfgValue)) {
	return false;
  }
  
  // Puntos de combate
  CfgName.szLine = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_COMBATPOINTS);
  CfgName.Position = pParser->ReadPosition("CombatPoints.Name.");
  if (!m_AttribInfo.CombatPoints.Text.Init(CfgName)) {
	return false;
  }
  CfgValue.Position = pParser->ReadPosition("CombatPoints.Value.");
  if (!m_AttribInfo.CombatPoints.Numeric.Init(CfgValue)) {
	return false;
  }
      
  // Salud
  CfgName.szLine = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_HEALTH);
  CfgName.Position = pParser->ReadPosition("Health.Name.");
  if (!m_AttribInfo.Health.Text.Init(CfgName)) {
	return false;
  }
  CfgValue.Position = pParser->ReadPosition("Health.Value.");
  if (!m_AttribInfo.Health.Numeric.Init(CfgValue)) {
	return false;
  }
  CfgPercentage.Position = pParser->ReadPosition("Health.PercentageBar.");
  if (!m_AttribInfo.Health.Percentage.Init(CfgPercentage)) {
	return false;
  }
   
  // Atributos extendidos  
  m_AttribInfo.uwNumExtAttribs = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB);
  byte ubIt = 0;
  for (; ubIt < m_AttribInfo.uwNumExtAttribs; ++ubIt) {
	// Se establece nombre vble
	SYSEngine::PassToString(szTmp, 
							"ExtendedAttribute[%s].", 
							 m_pGDBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_EXTATTRIB,
																		   ubIt).c_str());
	
	// Carga datos
	CfgName.szLine = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_EXT_ATTRIB,
											  ubIt);
	CfgName.Position = pParser->ReadPosition(szTmp + "Name.");
	if (!m_AttribInfo.EAttributes[ubIt].Text.Init(CfgName)) {
	  return false;
	}	
	CfgValue.Position = pParser->ReadPosition(szTmp + "Value.");
	if (!m_AttribInfo.EAttributes[ubIt].Numeric.Init(CfgValue)) {
	  return false;
	}	
	CfgPercentage.Position = pParser->ReadPosition(szTmp + "PercentageBar.");
	if (!m_AttribInfo.EAttributes[ubIt].Percentage.Init(CfgPercentage)) {
	  return false;
	}	
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga valores del texto flotante
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWPlayerProfile::TLoadFloatingTextValues(CCBTEngineParser* const pParser)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // Se cargan datos del texto flotante
  pParser->SetVarPrefix("FloatingText.");

  // Color
  m_FloatingTextValues.RGBColor = pParser->ReadRGBColor();

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza inicializacion llamando a los metodos correspondientes a cada
//   uno de los componentes.
// Parametros:
// Modifica:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CGUIWPlayerProfile::End> Finalizando interfaz para hoja del personaje.\n");        
	#endif

	// Desactiva
	Deactive();	

	// Finaliza componentes
	EndComponents();

	// Propaga finalizacion
	Inherited::End();
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("                       | Ok.\n");        
	#endif
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza componentes
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGUIWPlayerProfile::EndComponents(void)
{
  // Finaliza componentes
  // Imagen de fondo
  m_BackImgInfo.BackImg.End();

  // Inventario / botones de scroll
  m_InventoryInfo.UpButton.End();
  m_InventoryInfo.DownButton.End();
    
  // Finaliza consola
  m_ConsoleInfo.ButtonDown.End();
  m_ConsoleInfo.ButtonUp.End();
  m_ConsoleInfo.Text.End();
  m_ConsoleInfo.FXBack.End();

  // Slots de equipamiento
  byte ubIt = 0;
  const word uwNumSlots = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT);
  for (; ubIt < uwNumSlots; ++ubIt) {
	m_EquipSlotsInfo.Slots[ubIt].End();
	// ¿Se usan nombres?
	if (m_EquipSlotsInfo.bUseSlotsNames) {
	  m_EquipSlotsInfo.Names[ubIt].End();
	}	
  } 

  // Finaliza sintomas
  ubIt = 0;
  for (; ubIt < m_SymptomsInfo.uwNumSymptoms; ++ubIt) {
	m_SymptomsInfo.Symptoms[ubIt].End();
  }  
  
  // Finaliza retrato
  m_PortraitInfo.PortraitSlot.End();

  // Finaliza info de atributos
  m_AttribInfo.Name.Text.End();
  m_AttribInfo.GenreTypeClass.Text.End();
  m_AttribInfo.Level.Text.End();
  m_AttribInfo.Level.Numeric.End();
  m_AttribInfo.Experience.Text.End();
  m_AttribInfo.Experience.Numeric.End();
  m_AttribInfo.CombatPoints.Text.End();
  m_AttribInfo.CombatPoints.Numeric.End();    
  m_AttribInfo.Health.Text.End();
  m_AttribInfo.Health.Numeric.End();
  m_AttribInfo.Health.Percentage.End();
  ubIt = 0;
  for (; ubIt < RulesDefs::MAX_CRIATURE_EXATTRIB; ++ubIt) {
	m_AttribInfo.EAttributes[ubIt].Text.End();
	m_AttribInfo.EAttributes[ubIt].Numeric.End();
	m_AttribInfo.EAttributes[ubIt].Percentage.End();
  }  

  // Inicializa los botones asociados con las habilidades
  ubIt = 0;
  for (; ubIt < RulesDefs::MAX_CRIATURE_HABILITIES; ++ubIt) {	
	m_HabilitiesInfo.Habilities[ubIt].End();
  }

  // Finaliza boton de salida
  m_ExitInfo.ExitButton.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el interfaz y todos los componentes embebidos.
// - Al activarse, se inicializaran diversas vbles asociadas a grupos de
//   componentes.
// - Tambien se instalara aqui a la interfaz como observadora al inventario
//   del jugador y al contenedor con los slots de equipamiento. Estos ultimos
//   tambien seran mapeados a los selectores
// Parametros:
// - bDoFadeIn. Flag indicativo de si hay que hacer, o no, el FadeIn (siempre
//   habra que hacerlo al retornar de una presentacion). Por defecto valdra 
//   false.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::Active(const bool bDoFadeIn)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Activa si procede
  if (!Inherited::IsActive()) {		
	// ¿Hay que realizar FadeIn?
	if (bDoFadeIn) {
	  // Establece el FadeIn
	  Inherited::DoFadeIn(CGUIWPlayerProfile::ID_ENDFADEIN, 196);
	}

	// Establece cursor de ventana
	m_pGUIManager->SetGUICursor(GUIManagerDefs::WINDOWINTERFAZ_CURSOR);	
	
	// Procede a activar componentes    
	ActiveComponents(true);
	
	// Se instala instancia como observer del jugador
	CPlayer* const pPlayer = m_pWorld->GetPlayer();
	ASSERT(pPlayer);		
	pPlayer->AddObserver(this);

	// Se instala a la interfaz como observadora de los contenedores del jugador
	pPlayer->GetItemContainer()->AddObserver(this);	
	pPlayer->GetEquipmentSlots()->AddObserver(this);
	
	// Se indica que no hay ningun item cogido
	m_InventoryInfo.hActItem = 0;
	m_EquipSlotsInfo.hActItem = 0;
	
	// Se mapean slots de equipamiento
	MapEquipmentSlots();
	
	// Asocia como observador de CWorld
	m_pWorld->AddObserver(this);
	
	// Se propaga	
	ASSERT(m_pWorld->GetPlayer()->GetItemContainer());
	Inherited::Active(pPlayer->GetItemContainer(), NULL);			
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva interfaz y componentes
// - Se desinstalara como observador del contenedor del jugador
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Desactiva si procede
  if (Inherited::IsActive()) {
	// Procede a desactivar componentes    
	ActiveComponents(false);

	// Se desinstala como observador de la criatura
	m_pWorld->GetPlayer()->RemoveObserver(this);

	// Se desinstala como observador de los contenedores y slots
	m_pWorld->GetPlayer()->GetItemContainer()->RemoveObserver(this);
	m_pWorld->GetPlayer()->GetEquipmentSlots()->RemoveObserver(this);

	// Desinstala como observador de CWorld
	m_pWorld->RemoveObserver(this);
	
	// Se propaga
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
CGUIWPlayerProfile::ActiveComponents(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Procede a activar / desactivar
  // Imagen de fondo
  m_BackImgInfo.BackImg.SetActive(bActive);

  // Inventario / botones de scroll
  m_InventoryInfo.UpButton.SetActive(bActive);
  m_InventoryInfo.DownButton.SetActive(bActive);
  
  // Slots de equipamiento
  iCEquipmentSlots* const pEquipment = m_pWorld->GetPlayer()->GetEquipmentSlots();
  ASSERT(pEquipment);
  RulesDefs::EquipmentSlot Slot = 0;	
  const word uwNumSlots = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT);
  for (; Slot < uwNumSlots; ++Slot) {
	// Se activa / desactiva slot
	m_EquipSlotsInfo.Slots[Slot].SetActive(bActive);	

	// ¿Se usan nombres?
	if (m_EquipSlotsInfo.bUseSlotsNames) {
	  m_EquipSlotsInfo.Names[Slot].SetActive(bActive);
	}	
	
	// ¿Hay item en el slot Slot?
    const AreaDefs::EntHandle hItem = pEquipment->GetItemEquipedAt(Slot);
    if (hItem) {
	  // Si, se establece visibilidad en pantalla en funcion del flag
  	  CItem* const pItem = m_pWorld->GetItem(hItem);
	  ASSERT(pItem);
	  pItem->VisibleInScreen(bActive);
	}
  } 

  // Consola y botones de scroll
  m_ConsoleInfo.Text.SetActive(bActive);
  m_ConsoleInfo.ButtonUp.SetActive(bActive);
  m_ConsoleInfo.ButtonDown.SetActive(bActive);

  // Sintomas.
  // Los que no posea, se dejara sin seleccionar y con alpha  
  byte ubIt = 0;
  for (; ubIt < m_SymptomsInfo.uwNumSymptoms; ++ubIt) {
	// Se activa
	m_SymptomsInfo.Symptoms[ubIt].SetActive(bActive);
	
	// ¿El sintoma esta presente en el jugador?
	if (m_pWorld->GetPlayer()->IsSymptomActive(RulesDefs::eIDSymptom(0x01 << ubIt))) {	  
	  // Si, se selecciona
	  m_SymptomsInfo.Symptoms[ubIt].Select(bActive);
	} else {
	  // No, se quita seleccion y se asocia valor de transparencia
	  m_SymptomsInfo.Symptoms[ubIt].Select(false);
	  m_SymptomsInfo.Symptoms[ubIt].SetAlpha(m_SymptomsInfo.NoPresentAlpha);
	}
  }  
  
  // Retrato del personaje
  m_PortraitInfo.PortraitSlot.SetActive(bActive);  
  CSprite* const pPortrait = m_pWorld->GetPlayer()->GetPortrait();
  m_PortraitInfo.PortraitSlot.AttachSprite(pPortrait, 0);
  if (bActive) {
	// El retrato no debera de estar pausado
	// Nota: Recordemos que al entrar en el inventario,se propagara una orden
	// de pausa sobre el universo de juego.
	if (pPortrait) {
	  pPortrait->GetAnimTemplate()->ActiveAnimCmd(true);
	}
  }

  // Atributos
  // Instancia al jugador
  CCriature* const pPlayer = m_pWorld->GetPlayer();
  ASSERT(pPlayer);

  // Se establece el nombre del jugador
  m_AttribInfo.Name.Text.ChangeText(pPlayer->GetName());
    
  // Se activan informaciones asociadas a atributos
  
  // Nombre
  m_AttribInfo.Name.Text.SetActive(bActive);
  
  // Genero / Tipo / Clase
  m_AttribInfo.GenreTypeClass.Text.SetActive(bActive);
  // Se toma el texto estatico  
  std::string szText = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_GENRE,
												pPlayer->GetGenre());
  szText += " \\ " + m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_TYPE,
												  pPlayer->GetType());
  szText += " \\ " + m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_CLASS,
												  pPlayer->GetClass());
  m_AttribInfo.GenreTypeClass.Text.ChangeText(szText);
   
  // Nivel
  m_AttribInfo.Level.Text.SetActive(bActive);
  m_AttribInfo.Level.Numeric.SetActive(bActive);
  SetLevelNumericValues();
  
  // Experiencia
  m_AttribInfo.Experience.Text.SetActive(bActive);
  m_AttribInfo.Experience.Numeric.SetActive(bActive);
  SetExperienceNumericValues();
  
  // Puntos de combate
  m_AttribInfo.CombatPoints.Text.SetActive(bActive);
  m_AttribInfo.CombatPoints.Numeric.SetActive(bActive);
  SetCombatPointsNumericValues();
  
  // Salud
  m_AttribInfo.Health.Text.SetActive(bActive);
  m_AttribInfo.Health.Numeric.SetActive(bActive);
  m_AttribInfo.Health.Percentage.SetActive(bActive);
  SetHealthNumericValues();
  
  // Atributos extendidos
  ubIt = 0;
  for (; ubIt < m_AttribInfo.uwNumExtAttribs; ++ubIt) {
	m_AttribInfo.EAttributes[ubIt].Text.SetActive(bActive);
	m_AttribInfo.EAttributes[ubIt].Numeric.SetActive(bActive);
	m_AttribInfo.EAttributes[ubIt].Percentage.SetActive(bActive);
	SetExtAttributesNumericValues(ubIt);
  }  

  // Habilidades
  ubIt = 0;
  for (; ubIt < m_HabilitiesInfo.uwNumHabilities; ++ubIt) {
	// ¿Habilidad activa?
	if (m_pWorld->GetPlayer()->IsHabilityActive(RulesDefs::eIDHability(RulesDefs::HABILITY_01 << ubIt))) {
	  m_HabilitiesInfo.Habilities[ubIt].SetActive(bActive);
	  m_HabilitiesInfo.Habilities[ubIt].Select(bActive);
	}
  }

  // Salida
  m_ExitInfo.ExitButton.SetActive(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se mapean los slots de equipamiento sobre los selectores de interfaz.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::MapEquipmentSlots(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se obtiene interfaz al contenedor de slots de equipamiento del jugador
  iCEquipmentSlots* const pEquipment = m_pWorld->GetPlayer()->GetEquipmentSlots();
  ASSERT(pEquipment);

  // Se mapean slots de equipamiento
  const word uwMaxSlots = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT);
  RulesDefs::EquipmentSlot Slot = 0;	
  for (; Slot < uwMaxSlots; ++Slot) {
	// ¿Hay item en el slot Slot?
    const AreaDefs::EntHandle hItem = pEquipment->GetItemEquipedAt(Slot);
    if (hItem) {
	  // Si, se toma el item, se indica que es visible y se asocia
  	  CItem* const pItem = m_pWorld->GetItem(hItem);
	  ASSERT(pItem);
	  pItem->VisibleInScreen(true);
	  m_EquipSlotsInfo.Slots[Slot].AttachSprite(pItem, pItem->GetHandle());
	} else {
	  // No, no se asocia nada
	  m_EquipSlotsInfo.Slots[Slot].AttachSprite(NULL, 0);
	}
  }		    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa / desactiva a todos los componentes y a la propia ventana de 
//   interfaz en cuanto al procesamiento de la entrada. Este metodo sera llamado
//   cuando se active y Deactive el menu de interaccion con items, para que
//   no colisionen interPortraits.
// Parametros:
// - bActive. Indica si hay que activar o desactivar la entrada
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::SetInput(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Inventario
  // Botones de scroll
  m_InventoryInfo.UpButton.SetInput(bActive);
  m_InventoryInfo.DownButton.SetInput(bActive);

  // Consola
  m_ConsoleInfo.Text.SetInput(bActive);
  
  // Consola / Botones de scroll
  m_ConsoleInfo.ButtonUp.SetInput(bActive);
  m_ConsoleInfo.ButtonDown.SetInput(bActive);
  
  // Sintomas / Iconos
  byte ubIt = 0;
  for (; ubIt < m_SymptomsInfo.uwNumSymptoms; ++ubIt) {
	m_SymptomsInfo.Symptoms[ubIt].SetInput(bActive);
  }
  
  // Equipamiento / Slots & Nombres
  ubIt = 0;
  const word uwNumSlots = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT);
  for (; ubIt < uwNumSlots; ++ubIt) {
	m_EquipSlotsInfo.Slots[ubIt].SetInput(bActive);
	if (m_EquipSlotsInfo.bUseSlotsNames) {
	  m_EquipSlotsInfo.Names[ubIt].SetInput(bActive);
	}
  }  
  
  // Slot de cara
  m_PortraitInfo.PortraitSlot.SetInput(bActive);
  
  // Atributos
  m_AttribInfo.Name.Text.SetInput(bActive);
  m_AttribInfo.GenreTypeClass.Text.SetInput(bActive);
  m_AttribInfo.Level.Text.SetInput(bActive);
  m_AttribInfo.Level.Numeric.SetInput(bActive);
  m_AttribInfo.Experience.Text.SetInput(bActive);
  m_AttribInfo.Experience.Numeric.SetInput(bActive);
  m_AttribInfo.CombatPoints.Text.SetInput(bActive);
  m_AttribInfo.CombatPoints.Numeric.SetInput(bActive);
  m_AttribInfo.Health.Text.SetInput(bActive);
  m_AttribInfo.Health.Numeric.SetInput(bActive);
  m_AttribInfo.Health.Percentage.SetInput(bActive);
  ubIt = 0;
  for (; ubIt < m_AttribInfo.uwNumExtAttribs; ++ubIt) {
	m_AttribInfo.EAttributes[ubIt].Text.SetInput(bActive);
	m_AttribInfo.EAttributes[ubIt].Numeric.SetInput(bActive);
	m_AttribInfo.EAttributes[ubIt].Percentage.SetInput(bActive);
  }

  // Habilidades
  ubIt = 0;
  for (; ubIt < m_HabilitiesInfo.uwNumHabilities; ++ubIt) {	
	m_HabilitiesInfo.Habilities[ubIt].SetInput(bActive);
  }
  
  // Salida / Boton  
  m_ExitInfo.ExitButton.SetInput(bActive);

  // Se propaga evento a la clase base
  Inherited::SetInput(bActive);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece los valores numericos asociados al nivel.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::SetLevelNumericValues(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Vbles
  std::string szValue; // Valor temporal
  
  // Se establecen informaciones numericas
  // Nivel
  SYSEngine::PassToString(szValue, "%d", m_pWorld->GetPlayer()->GetLevel());	    
  m_AttribInfo.Level.Numeric.ChangeText(szValue);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece los valores numericos asociados a la experiencia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::SetExperienceNumericValues(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Vbles
  std::string szValue; // Valor temporal

  // Experiencia
  SYSEngine::PassToString(szValue, "%d", m_pWorld->GetPlayer()->GetExperience());
  m_AttribInfo.Experience.Numeric.ChangeText(szValue);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece los valores numericos asociados a los puntos de combate
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::SetCombatPointsNumericValues(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Vbles
  std::string szValue; // Valor temporal

  // Puntos de combate
  SYSEngine::PassToString(szValue, 
						  "%d", 
						  m_pWorld->GetPlayer()->GetActionPoints());
  m_AttribInfo.CombatPoints.Numeric.ChangeText(szValue);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece los valores numericos asociados a la salud
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::SetHealthNumericValues(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Se toman valores numericos para construir el texto
  const sword swBase = m_pWorld->GetPlayer()->GetHealth(RulesDefs::BASE_VALUE);
  const sword swTemp = m_pWorld->GetPlayer()->GetHealth(RulesDefs::TEMP_VALUE);
  std::string szValue;
  SYSEngine::PassToString(szValue, "%d \\ %d", swTemp, swBase);  

  // Se establecen cambios
  m_AttribInfo.Health.Numeric.ChangeText(szValue);
  m_AttribInfo.Health.Percentage.ChangeBaseAndTempValue(swBase, swTemp);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece los valores numericos asociados a los atributos extendidos
// Parametros:
// - ubExtAttribute. Id del atributo sobre el que establecer la informacion.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::SetExtAttributesNumericValues(const byte ubExtAttribute)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se obtiene instancia al jugador
  CCriature* const pPlayer = m_pWorld->GetPlayer();
  ASSERT(pPlayer);

  // Vbles
  std::string szValue; // Valor temporal

  // Obtiene el valor del atributo extendido
  const RulesDefs::eIDExAttrib ExtAttribIdx = RulesDefs::eIDExAttrib(ubExtAttribute); 
  const sword swBase = pPlayer->GetExAttribute(ExtAttribIdx,
											   RulesDefs::BASE_VALUE);
  const sword swTemp = pPlayer->GetExAttribute(ExtAttribIdx,
											   RulesDefs::TEMP_VALUE);
  SYSEngine::PassToString(szValue, "%d \\ %d", swTemp, swBase);  
  // Establece la informacion
  m_AttribInfo.EAttributes[ubExtAttribute].Numeric.ChangeText(szValue);
  m_AttribInfo.EAttributes[ubExtAttribute].Percentage.ChangeBaseAndTempValue(swBase, swTemp);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este metodo se llamara cuando la clase base detecte que se ha pulsado
//   sobre un selector de items con boton izq. En caso de que hItem sea 0, 
//   significara que se ha pulsado sobre un item vacio y viceversa.
// - Las posibles acciones que podran ocurrir:
//   * Si hItem es distinto de cero
//      * Si hay un item de inventario cogido
//         - Se generara un evento de usar el item cogido sobre el seleccionado
//           devolviendo este a inventario.
//      * Si hay un item de slot de equipamiento cogido
//        - Se devuelve al inventario
//      * Si NO hay un item de inventario cogido NI de slot de equipamiento
//        - Se cogera dicho item, sacandolo del inventario
//   * Si hItem es igual a 0
//     * Si hay un item de inventario cogido
//        - Se devolvera al inventario
// Parametros:
// - hItem. Item sobre el que se ha pulsado o bien 0 indicando que se ha
//   pulsado en una ranura libre de inventario
// - Selector. Selector al que esta asociado hItem.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::OnItemLeftClick(const AreaDefs::EntHandle hItem,
									CGUICSpriteSelector& Selector)
{
  // SOLO si intancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se ejecuta la logica asociada a un cliqueo sobre inv.
  OnInventoryLeftClick(hItem);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este metodo se llamara cuando la clase base detecte que se ha pulsado
//   sobre un selector de items con boton dcho. En caso de que hItem sea 0, 
//   significara que se ha pulsado sobre un item vacio y viceversa.
// - Las posibles acciones que podran ocurrir:
//   * Si hItem NO es cero y ademas no hay ningun item cogido de inv. o slot.
//      - Se activara la interfaz de interaccion.
//        La interfaz de interaccion permitira observar el item, usar
//        una habilidad sobre el mismo o tirarlo
// Parametros:
// - hItem. Item sobre el que se ha pulsado o bien 0 indicando que se ha
//   pulsado en una ranura libre de inventario
// - Selector. Selector al que esta asociado hItem
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::OnItemRightClick(const AreaDefs::EntHandle hItem,
									 CGUICSpriteSelector& Selector)
{
  // SOLO si intancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // ¿Es un item valido y ademas no hay nada cogido?
  if (hItem &&
	  !IsAnyItemAttachedToGUI()) {
	// Se quita la seleccion del slot, simulando la deseleccion
	// Nota: se debe de hacer antes de activar el interfaz de interaccion
	// con items, porque este bloqueara la entrada para el interfaz de inventario
	const GUIDefs::sGUICEvent Event(GUIDefs::GUIC_UNSELECT, Selector.GetID(), 0);
	ComponentNotify(Event);

	// Se ordena al GUIManager que active la interfaz de interaccion
	const sPosition InitDrawPos(Selector.GetXPos() + 64, Selector.GetYPos());
	m_pGUIManager->SetEntityInteractWindow(InitDrawPos,
										   m_pWorld->GetPlayer(),
										   m_pWorld->GetWorldEntity(hItem));	
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a un cliqueo sobre el inventario.
// - Las posibles acciones que podran ocurrir:
//   * Si hItem es distinto de cero
//      * Si hay un item de inventario cogido
//         - Si se pulsa sobre otro item:
//              - Se generara un evento de usar el item cogido sobre el 
//                seleccionado devolviendo este ultimo a inventario.
//         - Si se pulsa sobre slot vacio:
//              - Se colocara el item al final del todo.
//      * Si hay un item de slot de equipamiento cogido
//        - Se devuelve al slot de equipamiento.
//      * Si NO hay un item de inventario cogido NI de slot de equipamiento
//        - Se cogera dicho item, sacandolo del inventario
//   * Si hItem es igual a 0
//     * Si hay un item de inventario cogido
//        - Se devolvera al inventario
//     * Si hay item de equipamiento cogido
//        - Se devolvera al inventario
// Parametros:
// - hItem. Item sobre el que se ha pulsado o bien 0 indicando que se ha
//   pulsado en una ranura libre de inventario
// - Selector. Selector al que esta asociado hItem.
// Devuelve:
// Notas:
// - No se incluira la logica en el metodo OnItemLeftClick, pues esta 
//   sera utilizada desde multiples sitios.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::OnInventoryLeftClick(const AreaDefs::EntHandle hItem)
{
  // SOLO si intancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Se ha pulsado sobre un item valido?
  if (hItem) {
	// ¿Hay item de inventario cogido?
	if (m_InventoryInfo.hActItem) {
	  // Se toma item
	  // Nota: Es necesario guardarlo en otra variable pues se desvinculara
	  // m_InventoryInfo.hActItem para que no existan efectos laterales
	  const AreaDefs::EntHandle hInvItem = m_InventoryInfo.hActItem;
	  
	  // Si, se realizara una accion de USAR, devolviendo el item cogido
	  m_pWorld->GetPlayer()->GetItemContainer()->Insert(hInvItem);	  
	  	  
	  // Se elimina referencia del GUI
	  m_pGUIManager->AttachSpriteToGUICursor(NULL);
  	  m_InventoryInfo.hActItem = 0;

	  // Se usa el item cogido sobre el item de inventario cliqueado
	  m_pWorld->GetPlayer()->UseItem(hInvItem, hItem);	  
	} // ¿Hay item cogido de los slots de equipamiento?
	else if (m_EquipSlotsInfo.hActItem) {	  
	  // Se toma item
	  // Nota: Es necesario guardarlo en otra variable pues se desvinculara
	  // m_EquipSlotsInfo.hActItem para que no existan efectos laterales
	  const AreaDefs::EntHandle hEquipItem = m_EquipSlotsInfo.hActItem;
	  
	  // Se elimina referencia del GUI
	  m_pGUIManager->AttachSpriteToGUICursor(NULL);
	  m_EquipSlotsInfo.hActItem = 0;	  

  	  // Se vuelve a colocar en el inventario usandolo despues sobre
	  // el item pulsado que ya se encontraba en inv.
	  m_pWorld->GetPlayer()->GetItemContainer()->Insert(hEquipItem);
	  m_pWorld->GetPlayer()->UseItem(hEquipItem, hItem);	  
	} else {
	  // No hay item de inventario ni de equipamiento, luego se coge del inv.
	  m_pWorld->GetPlayer()->GetItemContainer()->Extract(hItem);
	  m_InventoryInfo.hActItem = hItem;	  
	  
	  // Se asocia al gui
	  CSprite* const pItemSprite = m_pWorld->GetItem(hItem);
	  ASSERT(pItemSprite);
	  m_pGUIManager->AttachSpriteToGUICursor(pItemSprite);	
	}
  } else {
	// No, se pulso sobre un slot sin item
	// ¿Hay item de inventario cogido?
	if (m_InventoryInfo.hActItem) {
	  // Se elimina referencia del GUI
	  m_pGUIManager->AttachSpriteToGUICursor(NULL);

	  // Se devuelve este a inventario
	  m_pWorld->GetPlayer()->GetItemContainer()->Insert(m_InventoryInfo.hActItem);	  
	  m_InventoryInfo.hActItem = 0;	  
	} else if (m_EquipSlotsInfo.hActItem) {
	  // Se toma item
	  // Nota: Es necesario guardarlo en otra variable pues se desvinculara
	  // m_EquipSlotsInfo.hActItem para que no existan efectos laterales
	  const AreaDefs::EntHandle hEquipItem = m_EquipSlotsInfo.hActItem;
	  
	  // Hay item cogido de los slots de equipamiento
	  // Se elimina referencia del GUI
	  m_EquipSlotsInfo.hActItem = 0;	  
	  m_pGUIManager->AttachSpriteToGUICursor(NULL);	  

	  // Se vuelve a colocar en el inventario
	  m_pWorld->GetPlayer()->GetItemContainer()->Insert(hEquipItem);
	} 
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la seleccion / deseleccion de un boton de scroll de
//   inventario.
// Parametros:
// - IDButton. Identificador del boton.
// - bSelect. Flag de seleccion / deseleccion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::OnInvButtonSelect(const GUIDefs::GUIIDComponent& IDButton,
									  const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Se comprueba el boton y se ejecuta la logica
  switch (IDButton) {
	case CGUIWPlayerProfile::ID_INV_BUTTON_UP: {
	  // Boton de subir inv.
	  m_InventoryInfo.UpButton.Select(bSelect);
	  
	} break;

	case CGUIWPlayerProfile::ID_INV_BUTTON_DOWN: {
	  // Boton de bajar inv.
	  m_InventoryInfo.DownButton.Select(bSelect);
	} break;

	default:
	  ASSERT_MSG(false, "El identificador es incorrecto");
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la pulsacion de un boton de scroll de inventario.
// Parametros:
// - IDButton. Identificador del boton.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::OnInvButtonLeftClick(const GUIDefs::GUIIDComponent& IDButton)
{
  // Se comprueba el boton y se ejecuta la logica
  switch (IDButton) {
	case CGUIWPlayerProfile::ID_INV_BUTTON_UP: {
	  // Boton de subir inv.
	  Inherited::PrevItemFile();
	  
	} break;

	case CGUIWPlayerProfile::ID_INV_BUTTON_DOWN: {
	  // Boton de bajar inv.
	  Inherited::NextItemFile();
	} break;

	default:
	  ASSERT_MSG(false, "El identificador es incorrecto");
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la seleccion / deseleccion de un slot de equipamiento.
// - En el caso de que haya seleccion, esta solo se podra llevar a cabo si
//   se tiene un item cogido y dicho item se puede colocar sobre el slot
//   seleccionado. O bien, si no se tiene ningun item cogido y se ha seleccionado
//   un slot que si tiene un item cogido.
// Parametros:
// - IDSlot. Identificador del slot.
// - bSelect. Flag de seleccion / deseleccion
// Devuelve:
// Notas:
// - La accion de seleccionar el slot se llevara a cabo desde el metodo
//   EquipmentSlotSelect.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::OnEquipmentSlotSelect(const RulesDefs::EquipmentSlot& Slot,
										  const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Se quiere seleccionar?
  if (bSelect) {
	// ¿Hay un item cogido?
	const AreaDefs::EntHandle hActItem = IsAnyItemAttachedToGUI();
	if (hActItem) {
	  // Si, pero ¿El item puede depositarse en el slot señalado?
	  const CItem* const pItem = m_pWorld->GetItem(hActItem);
	  ASSERT(pItem);
	  if (m_pGDBase->GetRulesDataBase()->CanSetItemAtEquipmentSlot(pItem->GetType(), Slot)) {
		// Si, se selecciona
		SelectEquipmentSlot(Slot, bSelect);
	  }
	} else if (m_EquipSlotsInfo.Slots[Slot].GetAttachedSprite()) {
	  // No hay item cogido pero se apunta a un slot con item
	  // Se selecciona
	  SelectEquipmentSlot(Slot, bSelect);
	}		
  } else {
	// No se quire seleccionar, se deselecciona
	SelectEquipmentSlot(Slot, bSelect);
  }
} 

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la pulsacion sobre un slot de equipamiento.
// - Podran ocurrir las siguientes cosas.
//   * Si hay un item cogido
//     * Si el item estaba en un slot de equipamiento
//     * Si el item proviene del inventario
//   * Si NO hay un item cogido
//     - Lo cogemos
// Parametros:
// Devuelve:
// Notas:
// - Al tomar un item equipado necesariamente debemos de desquiparlo primero
//   y despues sacarlo de su inventario. De igual modo, si tenemos un item
//   cogido y lo queremos equipar, como dicho item se saco del inventario,
//   habra que devolverlo al inventario y una vez alli equiparlo.
// - Al estar declarada la interfaz como observer, se realizaran las 
//   pertinentes notificaciones.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::OnEquipmentSlotLeftClick(const RulesDefs::EquipmentSlot& Slot)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Hay algun item cogido?
  if (IsAnyItemAttachedToGUI()) {
	// Se volvera a insertar el item cogido en inventario
	AreaDefs::EntHandle hItem = 0;
	if (m_InventoryInfo.hActItem) {
	  // Se habia cogido del inventario
	  hItem = m_InventoryInfo.hActItem;
	  m_InventoryInfo.hActItem = 0;
	} else {
	  // Se habia cogido de un slot de equipamiento
	  hItem = m_EquipSlotsInfo.hActItem;
	  m_EquipSlotsInfo.hActItem = 0;
	}
	
	// Para poder equiparlo, deberemos de insertar el item que cogimos en el
	// inventario de nuevo, pues lo habiamos sacado.
	ASSERT(hItem);
	m_pWorld->GetPlayer()->GetItemContainer()->Insert(hItem);	

	// Antes de equiparlo, tomamos el posible item que ya lo este para indicar
	// que deja de ser visible, pues vamos a equipar en su lugar otro y va a
	// ser regresado al inventario (el selector de items decidira si seguira
	// entonces visible o no)
	// Se mapean slots de equipamiento
	iCEquipmentSlots* const pEquipment = m_pWorld->GetPlayer()->GetEquipmentSlots();
	ASSERT(pEquipment);
    const AreaDefs::EntHandle hPrevItem = pEquipment->GetItemEquipedAt(Slot);
    if (hPrevItem) {
	  // Si, se toma y se indica que no es visible
  	  CItem* const pItem = m_pWorld->GetItem(hPrevItem);
	  ASSERT(pItem);
	  pItem->VisibleInScreen(false);
	}	
	
	// Lo equipamos
	if (RulesDefs::CAR_FAIL != m_pWorld->GetPlayer()->EquipItem(hItem, Slot)) {
	  // Se notifica que el item equipado es visible
	  CItem* const pItem = m_pWorld->GetItem(hItem);
	  ASSERT(pItem);
	  pItem->VisibleInScreen(true);	 
	} else {
	  // No se pudo realizar la accion, asi que se simulara un evento de 
	  // deseleccion del slot
	  OnEquipmentSlotSelect(Slot, false);
	}
	
	// Se desvinculara representacion desde el GUI
	m_pGUIManager->AttachSpriteToGUICursor(NULL);
  } else {	
	// No hay ningun item cogido	
	// Se intenta desequipar
	// Nota: Si se esta en modo combate, el simple hecho de pulsar sobre
	// un slot de equipamiento y tomar el item, se considerara desequipar	
	const AreaDefs::EntHandle hItem = m_pWorld->GetPlayer()->GetEquipmentSlots()->GetItemEquipedAt(Slot);
	ASSERT(hItem);
	if (m_pWorld->GetPlayer()->UnequipItemAt(Slot)) {
	  // Se extrae del inventario el item desequipado	  
	  m_pWorld->GetPlayer()->GetItemContainer()->Extract(hItem);	

	  // Se notifica que el item cogido NO es visible
	  CItem* const pItem = m_pWorld->GetItem(hItem);
	  ASSERT(pItem);
	  pItem->VisibleInScreen(false);
	  
	  // Se establecen vbles	
	  m_EquipSlotsInfo.ActSlot = Slot;	
	  m_EquipSlotsInfo.hActItem = hItem;
	  
	  // Se asociara al GUI su representacion
	  m_pGUIManager->AttachSpriteToGUICursor(pItem);		  
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la pulsacion con el boton derecho sobre un slot de 
//   equipamiento con item equipado. Lo que se hara sera devolver dicho
//   item equipado al inventario, generandose un evento de DESEQUIPAR.
// Parametros:
// - Slot. Slot sobre el que se realiza la accion.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::OnEquipmentSlotRightClick(const RulesDefs::EquipmentSlot& Slot)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // ¿Hay item asociado a slot?
  iCEquipmentSlots* const pEquipment = m_pWorld->GetPlayer()->GetEquipmentSlots();
  ASSERT(pEquipment);
  const AreaDefs::EntHandle hItem = pEquipment->GetItemEquipedAt(Slot);
  if (hItem) {
	// Si, se toma el item y se indica que NO es visible
  	CItem* const pItem = m_pWorld->GetItem(hItem);
	ASSERT(pItem);
	pItem->VisibleInScreen(false);

	// Se desequipa
	m_pWorld->GetPlayer()->UnequipItemAt(Slot);
	
	// Se simula la deseleccion del slot, pues ya no hay item en el mismo
	// y es seguro que nosotros no tenemos un item cogido
	OnEquipmentSlotSelect(Slot, false);	
  }		
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la pulsacion de un boton de scroll. Realizara el scroll
//   sobre el contenido de la consola.
// Parametros:
// - ubButton. Boton pulsado.
// Devuelve:
// Notas:
// - Si ubButton es 0, correspondera al boton de scroll hacia arriba y si es
//   1 al boton hacia abajo.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::OnConsoleButtonLeftClick(const byte ubButton)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT_MSG((ubButton < 2) != 0, "Solo hay dos botones de scroll de consola.");

  // Realiza el scroll que proceda
  switch (ubButton) {
	case 0: {	  
	  m_ConsoleInfo.Text.MoveUp();
	} break;

	case 1: {
	  m_ConsoleInfo.Text.MoveDown();
	} break;	
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la seleccion de un boton de scroll de consola
// Parametros:
// - ubButton. Boton seleccionado
// - bSelect. Flag de seleccion / deseleccion
// Devuelve:
// Notas:
// - Si ubButton es 0, correspondera al boton de scroll hacia arriba y si es
//   1 al boton hacia abajo.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::OnConsoleButtonSelect(const byte ubButton,
										  const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT_MSG((ubButton < 2) != 0, "Solo hay dos botones de scroll de consola.");

  // Deselecciona el boton que proceda
  if (0 == ubButton) {
	m_ConsoleInfo.ButtonUp.Select(bSelect);  
  } else {
	m_ConsoleInfo.ButtonDown.Select(bSelect);  
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la seleccion / deseleccion del retrato del jugador.
// - En el caso de que querer seleccionar y tener asociado un item que NO se
//   pueda usar sobre el jugador, la seleccion no se llevara a cabo.
// Parametros:
// - bSelect. Flag de seleccion / deseleccion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::OnPortraitSelect(const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Se quiere seleccionar?
  if (bSelect) {	
	// Se toma el posible item tomado de inventario o de equipamiento
	const CItem* pItem = NULL;
	if (m_InventoryInfo.hActItem) {
	  pItem = m_pWorld->GetItem(m_InventoryInfo.hActItem);
	} 

	// ¿Hay item asociado?
	if (pItem) {
	  // Se selecciona
	  m_PortraitInfo.PortraitSlot.Select(bSelect);	 
	}
  } else {
	// Se deselecciona
	m_PortraitInfo.PortraitSlot.Select(bSelect);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la pulsacion con el boton izquierdo sobre la cara.
//   En caso de que existiera algun item seleccionado, se ejecutaria un evento 
//   de USAR ITEM SOBRE PERSONAJE.
// - Notese que solo se podra cliquear cuando se tenga un item usable.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::OnPortraitLeftClick(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se comprobaria si hay un item asociado al cursor, en cuyo caso
  // se ejecutaria el vento de usar item sobre el jugador
  // ¿Hay un item de inventario?
  if (m_InventoryInfo.hActItem) {
	// Si, se realizara una accion de USAR, devolviendo el item cogido
	m_pWorld->GetPlayer()->GetItemContainer()->Insert(m_InventoryInfo.hActItem);	 	

	// Se elimina referencia del GUI
	m_pGUIManager->AttachSpriteToGUICursor(NULL);	

	// Se usa el item cogido sobre el item de inventario cliqueado
	m_pWorld->GetPlayer()->UseItem(m_InventoryInfo.hActItem, 
								   m_pWorld->GetPlayer()->GetHandle());	

	// Desreferencia handle
	m_InventoryInfo.hActItem = 0;	
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Trabaja con el evento de seleccion / deseleccion de un sintoma. Cuando
//   un sintoma seleccione, se establecera el texto flotante indicativo del
//   nombre de dicho sintoma y viceversa. No se podra seleccionar si el
//   sintoma no se posee. En caso de recibir deseleccion, se quitara el
//   texto flotante.
// Parametros:
// - IdxSymptom. Sintoma seleccionado
// - bSelect. Flag de seleccion / deseleccion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::OnSymptomSelect(const word uwIdxSymptom,
									const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿El jugador posee el sintoma?
  if (m_SymptomsInfo.Symptoms[uwIdxSymptom].IsSelect()) {
	// Si, luego se atiende el evento 
	if (bSelect) {
	  // Es seleccion, luego se indica con texto flotante el nombre del sintoma
	  m_pGUIManager->SetFloatingText(m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_SYMPTOMS, uwIdxSymptom),
									 m_FloatingTextValues.RGBColor,
									 m_FloatingTextValues.Alpha,
									 GUIManagerDefs::NORMAL_FLOATINGTEXT);
	} else {
	  // Es deseleccion, se desvincula texto flotante previo
	  m_pGUIManager->UnsetFloatingText(m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_SYMPTOMS, uwIdxSymptom));
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la seleccion / deseleccion de la opcion de salida.
// Parametros:
// - bSelect. Flag de seleccion / deseleccion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::OnExitSelect(const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿No hay ningun item asociado a GUI?
  if (!IsAnyItemAttachedToGUI()) {
	// Se selecciona el boton de salida
	m_ExitInfo.ExitButton.Select(bSelect);
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la pulsacion con el boton izq. del boton de salida.
//   En caso de que no se tenga ningun item cogido, se abandonara el interfaz
//   de pantalla de jugador para regresar al juego.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::OnExitLeftClick(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Antes de abandonar, si hubiera algun tipo de item cogido, se
  // simularia la pulsacion al boton derecho para evitar cualquier
  // vinculacion de un posible objeto cogido.
  OnButtonMouseRight();

  // Se deselecciona boton y se restablece menu de interfaz principal
  OnExitSelect(false);
  m_pGUIManager->SetMainInterfazWindow();  

  // Se reanuadara el universo de juego
  m_pWorld->SetPause(false);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Gestiona la entrega de notificaciones por parte de los componentes que
//   componen la hoja del personaje. Segun sea el tipo de evento recibido,
//   se llamara a un metodo para realizar las acciones de actualizacion.
// - Se debera de tener en cuenta que el componente al que esta asociado
//   el evento podria ser un selector de sprites de la clase base
// Parametros:
// - GUICEvent. Evento recibido.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿El evento pertenece a la clase base?
  if (GUICEvent.IDComponent < CGUIWPlayerProfile::ID_INV_BUTTON_UP) {
	Inherited::ComponentNotify(GUICEvent);
  } else {
	// Se llama al metodo apropiado de actualizacion
	switch(GUICEvent.Event) {
	  case GUIDefs::GUIC_SELECT:
		// Seleccion
		OnSelectNotify(GUICEvent.IDComponent, true);
		break;

	  case GUIDefs::GUIC_UNSELECT:
		// Deseleccion
		OnSelectNotify(GUICEvent.IDComponent, false);
		break;

	  case GUIDefs::GUIC_BUTTONRIGHT_PRESSED:
		// Pulsacion boton derecho
		OnButtonRightPressedNotify(GUICEvent.IDComponent);
		break;

	  case GUIDefs::GUIC_BUTTONLEFT_PRESSED:
		// Pulsacion boton izquierdo
		OnButtonLeftPressedNotify(GUICEvent.IDComponent);
		break;
	};
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Atiende a los eventos de seleccion / deseleccion
// Parametros:
// - IDComponent. Identificador del componente
// - bSelect. Flag de seleccion / deseleccion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::OnSelectNotify(const GUIDefs::GUIIDComponent& IDComponent,
								   const bool bSelect)
{
  // Procede a realizar las actualizaciones pertinentes
  switch(IDComponent) {	
	case CGUIWPlayerProfile::ID_INV_BUTTON_UP: 
	case CGUIWPlayerProfile::ID_INV_BUTTON_DOWN: { 	  
	  OnInvButtonSelect(IDComponent, bSelect);
	} break;

	case CGUIWPlayerProfile::ID_CONSOLE_BUTTON_UP:
	case CGUIWPlayerProfile::ID_CONSOLE_BUTTON_DOWN: {
	  OnConsoleButtonSelect(IDComponent - CGUIWPlayerProfile::ID_CONSOLE_BUTTON_UP,
						    bSelect);
	} break;

	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_00:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_01:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_02:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_03:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_04:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_05:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_06:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_07: {
	  OnEquipmentSlotSelect(IDComponent - CGUIWPlayerProfile::ID_EQUIPMENTSLOT_00, 
						    bSelect);
	} break;

	case CGUIWPlayerProfile::ID_SYMPTOM_00:
	case CGUIWPlayerProfile::ID_SYMPTOM_01:
	case CGUIWPlayerProfile::ID_SYMPTOM_02:
	case CGUIWPlayerProfile::ID_SYMPTOM_03:
	case CGUIWPlayerProfile::ID_SYMPTOM_04:
	case CGUIWPlayerProfile::ID_SYMPTOM_05:
	case CGUIWPlayerProfile::ID_SYMPTOM_06: 
	case CGUIWPlayerProfile::ID_SYMPTOM_07:
	case CGUIWPlayerProfile::ID_SYMPTOM_08:
	case CGUIWPlayerProfile::ID_SYMPTOM_09:
	case CGUIWPlayerProfile::ID_SYMPTOM_10:
	case CGUIWPlayerProfile::ID_SYMPTOM_11:
	case CGUIWPlayerProfile::ID_SYMPTOM_12:
	case CGUIWPlayerProfile::ID_SYMPTOM_13:
	case CGUIWPlayerProfile::ID_SYMPTOM_14:
	case CGUIWPlayerProfile::ID_SYMPTOM_15: {
	  OnSymptomSelect(IDComponent - CGUIWPlayerProfile::ID_SYMPTOM_00,
					  bSelect);
	} break;

	case CGUIWPlayerProfile::ID_Portrait: {	  
	  OnPortraitSelect(bSelect);
	} break;

	case CGUIWPlayerProfile::ID_EXIT_BUTTON: {	  	  
	  OnExitSelect(bSelect);	  
	} break;
  };
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la llamada a metodos con la logica asociada a la pulsacion
//   con el boton derecho sobre componentes.
// Parametros:
// - IDComponent. Identificador del componente
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::OnButtonRightPressedNotify(const GUIDefs::GUIIDComponent& IDComponent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Procede a realizar las actualizaciones pertinentes
  switch(IDComponent) {
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_00:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_01:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_02:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_03:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_04:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_05:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_06:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_07: {		    	  
  	  OnEquipmentSlotRightClick(IDComponent - CGUIWPlayerProfile::ID_EQUIPMENTSLOT_00);	  
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Implementa las acciones a realizar para notificaciones relacionadas con
//   el evento de pulsacion del boton izquierdo del raton.
// Parametros:
// - IDComponent. Identificador del componente
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::OnButtonLeftPressedNotify(const GUIDefs::GUIIDComponent& IDComponent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Procede a realizar las actualizaciones pertinentes
  switch(IDComponent) {
	case CGUIWPlayerProfile::ID_INV_BUTTON_UP:
	case CGUIWPlayerProfile::ID_INV_BUTTON_DOWN: {	  
	  OnInvButtonLeftClick(IDComponent);
	} break;

  	case CGUIWPlayerProfile::ID_CONSOLE_BUTTON_UP:
	case CGUIWPlayerProfile::ID_CONSOLE_BUTTON_DOWN: {
	  OnConsoleButtonLeftClick(IDComponent - CGUIWPlayerProfile::ID_CONSOLE_BUTTON_UP);
	} break;

	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_00:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_01:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_02:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_03:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_04:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_05:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_06:
	case CGUIWPlayerProfile::ID_EQUIPMENTSLOT_07: {		    	  
  	  OnEquipmentSlotLeftClick(IDComponent - CGUIWPlayerProfile::ID_EQUIPMENTSLOT_00);	  
	} break;

	case CGUIWPlayerProfile::ID_Portrait: {
	  // Si hay item seleccionado, evento de USAR SOBRE el personaje, 
	  // en caso contrario, evento de OBSERVAR PERSONAJE.	  
	  OnPortraitLeftClick();	  
	} break;

	case CGUIWPlayerProfile::ID_EXIT_BUTTON: {
	  OnExitLeftClick();
	} break;
  };
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion de que se ha realizado alguna operacion sobre el contenedor
//   o inventario del jugador
// Parametros:
// - NotifyType. Tipo de notificacion.
// - hItem. Handle al item protagonista de la notificacion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::ItemContainerNotify(const ItemContainerDefs::eObserverNotifyType& NotifyType,
										const AreaDefs::EntHandle& hItem)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(hItem);

  // Se comprueba el tipo de notificacion
  switch(NotifyType) {
	case ItemContainerDefs::ITEM_INSERTED: {
	  // Se notifica que el item ha sido devuelto al inventario
	  Inherited::ItemInserted(hItem);
	} break;

  	case ItemContainerDefs::ITEM_REMOVED: {
	  // Se notifica que el item ha sido quitado del inventario
	  Inherited::ItemRemoved(hItem);
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la notificacion de una accion sobre los slots de 
//   equipamiento del jugador.
// Parametros:
// - NotifyType. Tipo de notificacion
// - Slot. Slot en donde se ha realizado la notificacion.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::EquipmentSlotNotify(const EquipmentSlotsDefs::eObserverNotifyType& NotifyType,
									    const RulesDefs::EquipmentSlot& Slot)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT((Slot < RulesDefs::MAX_CRIATURE_EQUIPMENTSLOT) != 0);

  // Se comprueba el tipo de notificacion
  switch(NotifyType) {
	case EquipmentSlotsDefs::ITEM_EQUIPED: {
	  // Un item ha sido equipado.
	  // Se toma el handle al mismo y se asocia al selector
	  const AreaDefs::EntHandle hItem = m_pWorld->GetPlayer()->GetEquipmentSlots()->GetItemEquipedAt(Slot);
	  ASSERT(hItem);
	  CItem* const pItem = m_pWorld->GetItem(hItem);
	  ASSERT(pItem);
	  m_EquipSlotsInfo.Slots[Slot].AttachSprite(pItem, 0);	  
	} break;

  	case EquipmentSlotsDefs::ITEM_UNEQUIPED: {	  
	  // Un item ha sido desequipado.
	  // Se quita el sprite en el slot selector
	  m_EquipSlotsInfo.Slots[Slot].AttachSprite(NULL, 0);	  	  
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion sobre un suceso sobre el jugador.
// Parametros:
// - hCriature. Handle a la criatura.
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
										   const CriatureObserverDefs::eObserverNotifyType& NotifyType,
										   const dword udParam)
{
    // SOLO si intancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Interfaz activa? Y
  // ¿La entidad es el jugador?
  if (Inherited::IsActive()) {
	// Se captura el suceso
	switch (NotifyType) {
	  case CriatureObserverDefs::SYMPTOM_ACTIVE:
	  case CriatureObserverDefs::SYMPTOM_DEACTIVE: {
		// Activacion / desactivacion de un sintoma
		// Se obtiene indice al sintoma
		const byte ubIt = SYSEngine::PassBitValueToIndexValue(udParam);

		// ¿El sintoma estaba seleccionado?
		if (m_SymptomsInfo.Symptoms[ubIt].IsSelect()) {
		  // Si, luego significara que se habra quitado
		  m_SymptomsInfo.Symptoms[ubIt].Select(false);
		  m_SymptomsInfo.Symptoms[ubIt].SetAlpha(m_SymptomsInfo.NoPresentAlpha);	
		} else {
		  // No, luego significara que se habra establecido
		  m_SymptomsInfo.Symptoms[ubIt].Select(true);
		  m_SymptomsInfo.Symptoms[ubIt].SetAlpha(GraphDefs::sAlphaValue(255));	
		}
	  } break;
	  
	  case CriatureObserverDefs::HEALTH_MODIFY: {
		// Salud modificada
		SetHealthNumericValues();
	  } break;

	  case CriatureObserverDefs::IS_DEATH: {
		// La salud puede que este modificada
		SetHealthNumericValues();
	  } break;

	  case CriatureObserverDefs::EXTENDED_ATTRIBUTE_MODIFY: {
		// Atributo extendido modificado		
		SetExtAttributesNumericValues(udParam);
	  } break;

	  case CriatureObserverDefs::HABILITY_SET: 
	  case CriatureObserverDefs::HABILITY_UNSET: {
		// Activacion / desactivacion de una habilidad
		// Se obtiene indice al sintoma
		const byte ubIt = SYSEngine::PassBitValueToIndexValue(udParam);

		// ¿Habilidad activa?
		if (m_HabilitiesInfo.Habilities[ubIt].IsActive()) {
		  // Si, luego hay que desactivar
		  m_HabilitiesInfo.Habilities[ubIt].SetActive(false);
		  m_HabilitiesInfo.Habilities[ubIt].Select(false);
		} else {
		  // No, luego hay que activar
		  m_HabilitiesInfo.Habilities[ubIt].SetActive(true);
		  m_HabilitiesInfo.Habilities[ubIt].Select(true);
		}
	  } break;

	  case CriatureObserverDefs::LEVEL_MODIFY: {
		// Nivel modificado
		SetLevelNumericValues();
	  } break;

	  case CriatureObserverDefs::EXPERIENCE_MODIFY: {
		// Puntos de experiencia modificados
		SetExperienceNumericValues();
	  } break;

	  case CriatureObserverDefs::ACTION_POINTS_MODIFY: {
		// Puntos de combate modificados
		SetCombatPointsNumericValues();
	  } break;
	}; // ~ switch
  }    
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
CGUIWPlayerProfile::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
										const dword udParam)
{
  // SOLO si instancia inicializada y activada
  ASSERT(Inherited::IsInitOk());
  ASSERT(Inherited::IsActive());

  // Comprueba el tipo de notificacion
  switch(NotifyType) {
	case WorldObserverDefs::ENTITY_DESTROY: {
	  // Se comprueba si el item destruido estaba asociado al cursor y era
	  // de inventario o bien fue recogido de los slots de equipamiento
	  if (udParam == m_InventoryInfo.hActItem) {
		// Era de inventario, se desvincula informacion
		m_InventoryInfo.hActItem = 0;	  
		m_pGUIManager->AttachSpriteToGUICursor(NULL);
	  } else if (udParam == m_EquipSlotsInfo.hActItem) {
		// Era de slots de equipamiento, se desvincula informacion
		m_EquipSlotsInfo.hActItem = 0;	  
		m_pGUIManager->AttachSpriteToGUICursor(NULL);
	  }
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado de todos los componentes, mediante llamadas a los
//   metodos apropiados.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::Draw(void)
{
  // SOLO si clase inicializada
  ASSERT(Inherited::IsInitOk());

  // Se dibuja si procede
  if (Inherited::IsActive()) {
	// Fondo / Imagen
	m_BackImgInfo.BackImg.Draw();

	// Inventario, componentes y botones de scroll
	Inherited::Draw();
	m_InventoryInfo.UpButton.Draw();
	m_InventoryInfo.DownButton.Draw();

	// Consola 
	m_ConsoleInfo.FXBack.Draw();
	m_ConsoleInfo.Text.Draw();
	m_ConsoleInfo.ButtonUp.Draw();
	m_ConsoleInfo.ButtonDown.Draw();

	// Sintomas / Iconos
	byte ubIt = 0;
	for (; ubIt < m_SymptomsInfo.uwNumSymptoms; ++ubIt) {
	  m_SymptomsInfo.Symptoms[ubIt].Draw();
	}

	// Equipamiento / Slots & Nombres
	ubIt = 0;
	const word uwNumSlots = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT);
	for (; ubIt < uwNumSlots; ++ubIt) {
	  m_EquipSlotsInfo.Slots[ubIt].Draw();
	  if (m_EquipSlotsInfo.bUseSlotsNames) {
		m_EquipSlotsInfo.Names[ubIt].Draw();
	  }
	}  

	// Retrato
	m_PortraitInfo.PortraitSlot.Draw();

	// Atributos  
	m_AttribInfo.Name.Text.Draw();
	m_AttribInfo.GenreTypeClass.Text.Draw();
	m_AttribInfo.Level.Text.Draw();
	m_AttribInfo.Level.Numeric.Draw();
	m_AttribInfo.Experience.Text.Draw();
	m_AttribInfo.Experience.Numeric.Draw();
	m_AttribInfo.CombatPoints.Text.Draw();
	m_AttribInfo.CombatPoints.Numeric.Draw();
	m_AttribInfo.Health.Percentage.Draw();
	m_AttribInfo.Health.Text.Draw();
	m_AttribInfo.Health.Numeric.Draw();  
	ubIt = 0;
	for (; ubIt < m_AttribInfo.uwNumExtAttribs; ++ubIt) {
	  m_AttribInfo.EAttributes[ubIt].Percentage.Draw();
	  m_AttribInfo.EAttributes[ubIt].Text.Draw();
	  m_AttribInfo.EAttributes[ubIt].Numeric.Draw();	
	}

	// Habilidades
	ubIt = 0;
	for (; ubIt < m_HabilitiesInfo.uwNumHabilities; ++ubIt) {	
	  m_HabilitiesInfo.Habilities[ubIt].Draw();
	}
  
	// Salida
	m_ExitInfo.ExitButton.Draw();
  }  
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
CGUIWPlayerProfile::DispatchEvent(const InputDefs::sInputEvent& aInputEvent)
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
		case InputDefs::IE_KEY_ESC: {
		  // Se simula la pulsacion del boton de salir
		  bShareEvent = InputDefs::NO_SHARE_INPUT_EVENT;  
		  OnExitSelect(false);
		  OnExitLeftClick();
		} break;

		case InputDefs::IE_BUTTON_MOUSE_RIGHT: {		  
		  // Pulsado boton derecho del raton
		  // En caso de que haya un item cogido, se devolvera al lugar
		  // desde donde se cogio.
		  // En caso de que no se de la cincunstancia anterior, se 
		  // desistimara la accion
		  if (!OnButtonMouseRight()) {
			bShareEvent = InputDefs::SHARE_INPUT_EVENT;  
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
// - Simula la pulsacion del boton derecho del raton en general. La logica
//   a ejecutar consistira en comprobar si hay algun item asociado a cursor,
//   para que si tal ocurre, se reponga dicho item a su lugar de origen.
// Parametros:
// Devuelve:
// - En caso de que se haya realizado alguna operacion true, en caso contrario
//   false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWPlayerProfile::OnButtonMouseRight(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Hay item tomado desde inventario?
  if (m_InventoryInfo.hActItem) {
	// Si, se simula pulsacion sobre un slot vacio y retorna
	OnInventoryLeftClick(0);			
	return true;
  } 

  // ¿Hay item tomado desde los slots de equipamiento?
  if (m_EquipSlotsInfo.hActItem) {
	// Si, se simula pulsacion sobre el mismo slot en donde estaba
	OnEquipmentSlotLeftClick(m_EquipSlotsInfo.ActSlot);			
	return true;
  }

  // No se realizo accion alguna
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se instalara como cliente de entrada para capturar los eventos de:
//   * Boton derecho. Si hay item cogido, este se devolvera al inventario.
//   * ESC. Salida si procede.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::InstallClient(void)
{
  // Se instalan eventos
  m_pInputManager->SetInputEvent(InputDefs::IE_BUTTON_MOUSE_RIGHT, 
								 this, 
								 InputDefs::BUTTON_PRESSED_DOWN);    
  m_pInputManager->SetInputEvent(InputDefs::IE_KEY_ESC, 
							     this, 
								 InputDefs::BUTTON_PRESSED_DOWN);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desinstala eventos de entrada para los que es cliente.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPlayerProfile::UnistallClient(void)
{
  // Se desinstalan eventos  
  m_pInputManager->UnsetInputEvent(InputDefs::IE_BUTTON_MOUSE_RIGHT, 
								   this, 
								   InputDefs::BUTTON_PRESSED_DOWN);
  m_pInputManager->UnsetInputEvent(InputDefs::IE_KEY_ESC, 
								   this, 
								   InputDefs::BUTTON_PRESSED_DOWN);  
}
  
