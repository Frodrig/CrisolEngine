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
// CGUIWMainInterfaz.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWMainInterfaz.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWMainInterfaz.h"

#include "SYSEngine.h"
#include "iCGUIManager.h"
#include "iCWorld.h"
#include "iCGUIManager.h"
#include "iCLogger.h"
#include "iCInputManager.h"
#include "iCGameDataBase.h"
#include "iCFontSystem.h"
#include "iCVirtualMachine.h"
#include "AreaDefs.h"
#include "CWorldEntity.h"
#include "CCriature.h"
#include "CPlayer.h"
#include "CItem.h"
#include "CSprite.h"
#include "CCBTEngineParser.h"
#include "CMemoryPool.h"

// Definicion de estructuras forward
struct CGUIWMainInterfaz::sSymptomsInfo::sNSymptomIcon {
  // Nodo asociado a un icono que represente un sintoma activo
  CGUICButton Icon; // Boton para representar el sintoma.
  // Pool de memoria
  static CMemoryPool MPool;
  static void* operator new(const size_t size) { return MPool.AllocMem(size); }
  static void operator delete(void* pItem) { MPool.FreeMem(pItem); } 
};

// Inicializacion del pool de memoria
CMemoryPool 
CGUIWMainInterfaz::sSymptomsInfo::sNSymptomIcon::MPool(8,
													   sizeof(CGUIWMainInterfaz::sSymptomsInfo::sNSymptomIcon), 
													   true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar.
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWMainInterfaz::Init(void)
{
  // ¿Se quiere reinicializar?
  if (Inherited::IsInitOk()) { 
	return false; 
  }

  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("CGUIWMainInterfaz::Init> Inicializando interfaz de ventana principal.\n");        
  #endif

  // Se proceden a inicializar instancias
  if (Inherited::Init()) { 
	// Se toman interfaces a otros modulos
	m_pWorld = SYSEngine::GetWorld();
	ASSERT(m_pWorld);  
	m_pGUIManager = SYSEngine::GetGUIManager();
	ASSERT(m_pGUIManager);
	m_pGraphSys = SYSEngine::GetGraphicSystem();
	ASSERT(m_pGraphSys);
	m_pGDBase = SYSEngine::GetGameDataBase();
	ASSERT(m_pGDBase);
	m_pFontSys = SYSEngine::GetFontSystem();
	ASSERT(m_pFontSys);
	m_pVMachine = SYSEngine::GetVirtualMachine();
	ASSERT(m_pVMachine);
	
	// Se inicializan cursores de scroll 
	InitIsoCursors();

	// Se iniciallizan estados de interfaz
	InitInterfazState();

	// Procede a cargar datos de configuracion de componentes
	if (TLoad()) {
	  // Todo correcto
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
// - Finaliza instancia, liberando todos los componentes asociados.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::End(void)
{
  // Finaliza instacia si procede
  if (Inherited::IsInitOk()) {
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CGUIWMainInterfaz::End> Finalizando interfaz de ventana principal.\n");        
	#endif

	// Desactiva
	Deactive();
	
	// Finaliza componentes
	EndComponents();	

	// Finaliza posibles scripts asociados
	m_pVMachine->ReleaseScripts(this);
	
	// Propaga
	Inherited::End();
	
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("                      | Ok.\n");        
	#endif
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa estados de interfaz
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::InitInterfazState(void)
{
  // Se estblecen valores en estado de interfaz
  m_InterfazInfo.State = CGUIWMainInterfaz::INIT;
  m_InterfazInfo.PrevState = CGUIWMainInterfaz::MOVING_PLAYER;
  m_InterfazInfo.Pos = sPosition(0, 0);
  m_InterfazInfo.pEntityInteract = NULL;			  
  m_InterfazInfo.Mode = WorldDefs::REAL_MODE;
  m_InterfazInfo.pCriatureInTurn = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa los cursores de scroll
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWMainInterfaz::InitIsoCursors(void)
{
  // Se establece estado de scroll
  m_ScrollInfo.ScrollCursor = GUIManagerDefs::NO_CURSOR;

  // Se toma tope al que puede llegar el cursor a dcha y parte inferior
  m_ScrollInfo.uwRightCursorTop = m_pGraphSys->GetVideoWidth();
  m_ScrollInfo.uwBottomCursorTop = m_pGraphSys->GetVideoHeight();

  // Se toman dimensiones de los cursores de GUI
  const byte ubGUICurWidth = m_pGUIManager->GetGUICursorsWidth();
  const byte ubGUICurHeight = m_pGUIManager->GetGUICursorsHeight();
  
  // Realiza las mismas operaciones para para las posiciones diagonales
  m_ScrollInfo.uwRightDiagCursorTop = m_ScrollInfo.uwRightCursorTop - (ubGUICurWidth / 2);
  m_ScrollInfo.uwBottomDiagCursorTop = m_ScrollInfo.uwBottomCursorTop - (ubGUICurHeight / 2);

  // Se guarda posicion de dibujado para el cursor para los topes
  m_ScrollInfo.uwDrawRightPos = m_ScrollInfo.uwRightCursorTop - ubGUICurWidth;
  m_ScrollInfo.uwDrawBottomPos = m_ScrollInfo.uwBottomCursorTop - ubGUICurHeight;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la carga e inicializacion de componentes desde archivos de 
//   configuracion de texto.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWMainInterfaz::TLoad(void)
{
  // Se obtiene parser y se situa
  CCBTEngineParser* const pParser = m_pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
															"[MainGameInterfaz]");
  ASSERT(pParser);

  // Se procede a cargar e inicializar componentes y valores generales
  // Texto flotante
  if (!TLoadFloatingText(pParser)) {
	return false;
  }

  // Imagen de fondo
  if (!TLoadBackImg(pParser)) { 
	return false;
  }

  // Consola
  if (!TLoadConsole(pParser)) { 
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

  // Sintomas
  if (!TLoadSymptomInfo(pParser)) { 
	return false;
  }

  // Info de atributos
  if (!TLoadAttribInfo(pParser)) { 
	return false;
  }

  // Botones de combate
  if (!TLoadCombatOptionsInfo(pParser)) {
	return false;
  }

  // Info del controlador de tiempo
  if (!TLoadTimeControllerInfo(pParser)) {
	return false;
  }
  
  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga datos asociados al texto flotante.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo va bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWMainInterfaz::TLoadFloatingText(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Carga color
  pParser->SetVarPrefix("FloatingText.");
  m_FloatingTextInfo.RGBColor = pParser->ReadRGBColor();

  // Se establece el valor alpha solido y formato de texto normal
  m_FloatingTextInfo.Alpha = GraphDefs::sAlphaValue(255);
  m_FloatingTextInfo.Type = GUIManagerDefs::NORMAL_FLOATINGTEXT;

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga e inicializa imagen de fondo usando ficheros de texto.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo va bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWMainInterfaz::TLoadBackImg(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Vbles
  CGUICBitmap::sGUICBitmapInitData CfgInit; // Datos de configuracion

  // Se establece prefijo
  pParser->SetVarPrefix("BackImage.");
  
  // Se carga nombre imagen
  CfgInit.BitmapInfo.szFileName = pParser->ReadString("FileName");
  // Se completan datos  
  CfgInit.ID = CGUIWMainInterfaz::ID_BACKGROUND;
  CfgInit.Position.swXPos = 0;
  CfgInit.Position.swYPos = m_pGraphSys->GetVideoHeight() - 96;
  
  // Se inicializa y retorna resultado
  m_BackImgInfo.Background.Init(CfgInit);
  return m_BackImgInfo.Background.IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga e inicializa consola usando ficheros de texto
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo va bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWMainInterfaz::TLoadConsole(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Vbles  
  CGUICBlockText::sGUICBlockTextInitData CfgBlockText; // Cfg para bloque de texto
  CGUICButton::sGUICButtonInitData       CfgButton;    // Cfg para boton

  // Prefijo
  pParser->SetVarPrefix("Console.");

  // Consola
  CfgBlockText.RGBUnselectColor = pParser->ReadRGBColor("Text.");
  CfgBlockText.RGBSelectColor = CfgBlockText.RGBUnselectColor;
  CfgBlockText.Position = pParser->ReadPosition();
  CfgBlockText.uwWidthJustify = pParser->ReadNumeric("Width");
  CfgBlockText.szWAVBipSound = pParser->ReadString("WAVBipFileName", false);
  // Se completan datos e inicializa
  CfgBlockText.szFont = "Arial";
  CfgBlockText.uwNumLines = 5;
  CfgBlockText.uwCapacity = 1024;
  CfgBlockText.bCanReutilice = true;
  CfgBlockText.ID = CGUIWMainInterfaz::ID_CONSOLE;
  CfgBlockText.ubDrawPlane = 0;
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

  // Boton subir texto
  CfgButton.szATButton = pParser->ReadString("Options.UpButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("Options.UpButton.");  

  // Se completan datos e inicializa
  CfgButton.ID = CGUIWMainInterfaz::ID_CONSOLE_SCROLL_UP;
  CfgButton.pGUIWindow = this;
  CfgButton.GUIEvents = GUIDefs::GUIC_SELECT |
						GUIDefs::GUIC_UNSELECT |
						GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  CfgButton.ubDrawPlane = 0;
  if (!m_ConsoleInfo.ButtonUp.Init(CfgButton)) { 
	return false; 
  }
  
  // Boton bajar texto
  CfgButton.szATButton = pParser->ReadString("Options.DownButton.AnimTemplate");
  CfgButton.Position = pParser->ReadPosition("Options.DownButton.");  
  // Se completan datos e inicializa
  CfgButton.ID = CGUIWMainInterfaz::ID_CONSOLE_SCROLL_DOWN;
  if (!m_ConsoleInfo.ButtonDown.Init(CfgButton)) { 
	return false; 
  }  

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga e inicializa slots de equipamiento usando ficheros de texto
// - En lo referido a los slots de equipamiento, se permitira recibir eventos
//   de cliqueo aunque estos no se hallen seleccionados.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo va bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWMainInterfaz::TLoadEquipmentSlots(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Vbles
  CGUICSpriteSelector::sGUICSpriteSelectorInitData CfgSlot; // Cfg selector
  CGUICLineText::sGUICLineTextInitData             CfgName; // Cfg del nombre
  std::string                                      szTmp;   // Var temporal
  
  // Se establece info comun
  // Slot 
  CfgSlot.pGUIWindow = this;
  CfgSlot.GUIEvents = GUIDefs::GUIC_SELECT | 
					  GUIDefs::GUIC_UNSELECT |
					  GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  CfgSlot.bSelectOnlyWithSprite = false;
  CfgSlot.bClickWithSelect = false;
  // Nombre
  CfgName.szFont = "Arial";
  CfgName.szLine = "-";
  
  // Se carga info del slot
  pParser->SetVarPrefix("Equipment.");  
  CfgSlot.RGBSelect = pParser->ReadRGBColor("DrawSlotsFormat.FXBack.Select.");
  CfgSlot.AlphaSelect = pParser->ReadAlpha("DrawSlotsFormat.FXBack.Select.");
  CfgSlot.RGBUnselect = pParser->ReadRGBColor("DrawSlotsFormat.FXBack.Unselect.");
  CfgSlot.AlphaUnselect = pParser->ReadAlpha("DrawSlotsFormat.FXBack.Unselect.");

  // Carga datos slot mano izq
  SYSEngine::PassToString(szTmp, 
						  "Slot[%s].",
						  m_pGDBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT,
																		0).c_str());
  CfgSlot.Position = pParser->ReadPosition(szTmp);  
  // Se completan datos e inicializa
  CfgSlot.ID = CGUIWMainInterfaz::ID_SLOT_LEFT;  
  if (!m_EquipSlotsInfo.LeftSlot.Slot.Init(CfgSlot)) { 
	return false; 
  }

  // Se carga la info relativa al nombre del slot
  CfgName.Position = pParser->ReadPosition(szTmp + "Name.");
  CfgName.RGBSelectColor = pParser->ReadRGBColor(szTmp + "Name.Select.");
  CfgName.RGBUnselectColor = pParser->ReadRGBColor(szTmp + "Name.Unselect.");  
  // Se completa info y se inicializa
  CfgName.ID = CGUIWMainInterfaz::ID_SLOTNAME_LEFT;
  if (!m_EquipSlotsInfo.LeftSlot.SlotName.Init(CfgName)) {
	return false;
  }

  // Se obtiene nombre de la plantilla a icono de mano por defecto
  if (!m_EquipSlotsInfo.LeftSlot.HandIcon.Init(pParser->ReadString(szTmp + "DefaultAnimTemplate"))) {
	return false;
  }

  // Carga datos slot mano dcha
  SYSEngine::PassToString(szTmp, 
						  "Slot[%s].",
						  m_pGDBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT,
																		1).c_str());
  CfgSlot.Position = pParser->ReadPosition(szTmp);
  // Se completan datos e inicializa
  CfgSlot.ID = CGUIWMainInterfaz::ID_SLOT_RIGHT;
  if (!m_EquipSlotsInfo.RightSlot.Slot.Init(CfgSlot)) { 
	return false; 
  }

  // Se carga la info relativa al nombre del slot
  CfgName.Position = pParser->ReadPosition(szTmp + "Name.");
  CfgName.RGBSelectColor = pParser->ReadRGBColor(szTmp + "Name.Select.");
  CfgName.RGBUnselectColor = pParser->ReadRGBColor(szTmp + "Name.Unselect.");  
  // Se completa info y se inicializa
  CfgName.ID = CGUIWMainInterfaz::ID_SLOTNAME_RIGHT;
  if (!m_EquipSlotsInfo.RightSlot.SlotName.Init(CfgName)) {
	return false;
  }
  
  // Se obtiene nombre de la plantilla a icono de mano por defecto
  if (!m_EquipSlotsInfo.RightSlot.HandIcon.Init(pParser->ReadString(szTmp + "DefaultAnimTemplate"))) {
	return false;
  }  

  // Inicialmente no habra ningun slot seleccionado
  m_EquipSlotsInfo.bIsWeaponSelect = false;

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga e inicializa retratos usando ficheros de texto
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo va bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWMainInterfaz::TLoadPortraitInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Vbles
  CGUICSpriteSelector::sGUICSpriteSelectorInitData CfgSlot; // Cfg selector

  // Se carga info del slot
  pParser->SetVarPrefix("PlayerPortrait.");  
  CfgSlot.Position = pParser->ReadPosition();
  CfgSlot.RGBSelect = pParser->ReadRGBColor("FXBack.Select.");
  CfgSlot.AlphaSelect = pParser->ReadAlpha("FXBack.Select.");
  CfgSlot.RGBUnselect = pParser->ReadRGBColor("FXBack.Unselect.");
  CfgSlot.AlphaUnselect = pParser->ReadAlpha("FXBack.Unselect.");
  	
  // Se completan datos e inicializa
  CfgSlot.ID = CGUIWMainInterfaz::ID_PORTRAIT;
  CfgSlot.GUIEvents = GUIDefs::GUIC_SELECT |
					  GUIDefs::GUIC_UNSELECT |
					  GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  CfgSlot.pGUIWindow = this;
  CfgSlot.ubDrawPlane = 0;
  CfgSlot.bSelectOnlyWithSprite = true;  
  if (!m_PortraitInfo.Portrait.Init(CfgSlot)) { 
	return false; 
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga los botones asociados al modo combate.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWMainInterfaz::TLoadCombatOptionsInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Vbles
  CGUICButton::sGUICButtonInitData CfgButton; // Cfg para botones

  // Se establecen valores por defecto
  CfgButton.GUIEvents = GUIDefs::GUIC_SELECT |
						GUIDefs::GUIC_UNSELECT |
						GUIDefs::GUIC_BUTTONLEFT_PRESSED;
  CfgButton.pGUIWindow = this;
  CfgButton.ubDrawPlane = 0;  

  // Prefijo
  pParser->SetVarPrefix("Combat.");

  // Se inicializa boton de sig. turno
  CfgButton.Position = pParser->ReadPosition("Options.NextTurnButton.");
  CfgButton.szATButton = pParser->ReadString("Options.NextTurnButton.AnimTemplate");
  CfgButton.ID = CGUIWMainInterfaz::ID_NEXTTURNBUTTON;
  if (!m_CombatOptionsInfo.NextTurn.Init(CfgButton)) {
	return false;
  }

  // Se inicializa boton de huida en combate
  CfgButton.Position = pParser->ReadPosition("Options.FleeButton.");
  CfgButton.szATButton = pParser->ReadString("Options.FleeButton.AnimTemplate");
  CfgButton.ID = CGUIWMainInterfaz::ID_FLEECOMBATBUTTON;
  if (!m_CombatOptionsInfo.FleeCombat.Init(CfgButton)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga todos los botones para representar a los iconos de sintomas y los
//   mapea en el map donde estos se alojaran.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo va bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool
CGUIWMainInterfaz::TLoadSymptomInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Se lee componente alpha
  pParser->SetVarPrefix("Symptoms.");
  const GraphDefs::sAlphaValue Alpha = pParser->ReadAlpha();

  // Vbles
  CGUICButton::sGUICButtonInitData CfgIcon; // Cfg para el icono de boton
  std::string                      szTmp;   // String temporal
  
  // Se establecen valores comunes  
  CfgIcon.ubDrawPlane = 1;
  CfgIcon.ID = CGUIWMainInterfaz::ID_SYMPTOM;

  // Procede a inicializar sintomas
  const word uwNumSymptoms = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_SYMPTOM);
  byte ubIt = 0;
  for (; ubIt < uwNumSymptoms; ++ubIt) {
	// Se carga informacion
	SYSEngine::PassToString(szTmp, 
							"Icon[%s].",
							m_pGDBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_SYMPTOM,
																		  ubIt).c_str());
	
	CfgIcon.szATButton = pParser->ReadString(szTmp + "AnimTemplate");
	CfgIcon.Position = pParser->ReadPosition(szTmp);    	
	// Se crea icono
	sSymptomsInfo::sNSymptomIcon* const pIcon = new sSymptomsInfo::sNSymptomIcon;
	ASSERT(pIcon);
	if (!pIcon->Icon.Init(CfgIcon)) {
	  delete pIcon;
	  return false;
	}

	// Se asocia el componente alpha
	pIcon->Icon.SetAlpha(Alpha);

	// Se mapea
	const RulesDefs::eIDSymptom IDSymp = RulesDefs::eIDSymptom(RulesDefs::SYMPTOM_01 << ubIt);
	m_SymptomsInfo.Icons.insert(sSymptomsInfo::SymptomIconMapValType(IDSymp, pIcon));
  }  

  // Todo correcto
  return true;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga e inicializa info asociada a atributos usando ficheros de texto
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo va bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWMainInterfaz::TLoadAttribInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Num maximo de atributos mostrables
  const word uwMaxAttribs = 1 + m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB);

  // Se establece el nombre de los atributos posibles en el array pszAttribs
  // teniendo en cuenta que el primer atributo mostrable sera el de salud
  std::string* const pszAttribs = new std::string[uwMaxAttribs]; 
  ASSERT(pszAttribs);  
  byte ubIt = 0;
  pszAttribs[ubIt++] = "Health";
  for (; ubIt < uwMaxAttribs; ++ubIt) {
	pszAttribs[ubIt] = m_pGDBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_EXTATTRIB,
																	 ubIt - 1);
  }
  
  // Nombres
  CGUICLineText::sGUICLineTextInitData CfgName;
  CfgName.ID = CGUIWMainInterfaz::ID_ATTRIB_NAMES;
  CfgName.ubDrawPlane = 1;	
  CfgName.szFont = "Arial";
  CfgName.szLine = "-";  
  // Valores
  CGUICLineText::sGUICLineTextInitData CfgValue;
  CfgValue.ID = CGUIWMainInterfaz::ID_ATTRIB_NUMERIC;
  CfgValue.ubDrawPlane = 1;	
  CfgValue.szFont = "Arial";
  CfgValue.szLine = "-";
  // Porcentaje                                                
  CGUICPercentageBar::sGUICPercentageBarInitData CfgPercentage;
  CfgPercentage.ID = CGUIWMainInterfaz::ID_ATTRIB_PERCENTAGES;

  // Carga de datos concretos para atributos
  // Se podran mostrar hasta dos atributos simultaneamente que
  // seran el de salud y cualquiera de los atributos extendidos      
  byte ubNumAttribsFound = 0; 
  bool bResult = true;
  pParser->SetVarPrefix("Attributes.");  
  ubIt = 0;  
  while (ubIt < uwMaxAttribs && 
		 ubNumAttribsFound < sAttributesInfo::MAX_ATTRIBUTES_TO_SHOW) {
	// Nombre
	std::string szTmp;  
	SYSEngine::PassToString(szTmp, "Attribute[%s].", pszAttribs[ubIt].c_str());	
	CfgName.Position = pParser->ReadPosition(szTmp + "Name.", false);	
	
	// ¿SI se leyo?
	if (pParser->WasLastParseOk()) { 	  
	  // Resto de datos de Nombre e inicializacion
	  CfgName.RGBUnselectColor = pParser->ReadRGBColor(szTmp + "Name.");
      CfgName.RGBSelectColor = CfgName.RGBUnselectColor;  
	  if (!m_AttributesInfo.Attribs[ubNumAttribsFound].Name.Init(CfgName)) {	  
		bResult = false;
		break;
	  }

	  // Valor, carga de datos e inicializacion
	  CfgValue.Position = pParser->ReadPosition(szTmp + "Value.");	
	  CfgValue.RGBUnselectColor = pParser->ReadRGBColor(szTmp + "Value.");
      CfgValue.RGBSelectColor = CfgValue.RGBUnselectColor;  
	  if (!m_AttributesInfo.Attribs[ubNumAttribsFound].Numeric.Init(CfgValue)) {	
		bResult = false;
		break;
	  }

	  // Porcentaje, resto de datos e inicializacion
	  CfgPercentage.Position = pParser->ReadPosition(szTmp + "PercentageBar.");	
	  CfgPercentage.bDrawHoriz = pParser->ReadFlag(szTmp + "PercentageBar.HorizontalDrawFlag");
	  CfgPercentage.uwLenght = pParser->ReadNumeric(szTmp + "PercentageBar.Lenght");
	  CfgPercentage.uwWidth = pParser->ReadNumeric(szTmp + "PercentageBar.Width");
	  CfgPercentage.Alpha = pParser->ReadAlpha(szTmp + "PercentageBar.");  
	  if (!m_AttributesInfo.Attribs[ubNumAttribsFound].Percentage.Init(CfgPercentage)) {	 
		bResult = false;
		break;
	  }

	  // Se almacena el ID del atributo leido e incrementar num. leidos
	  // El identificador sera siempre distinto de cero. Esto servira para
	  // decidir si un atributo es mostrable o no (por defecto valdran 0)
	  m_AttributesInfo.Attribs[ubNumAttribsFound].ubID = ubIt + 1;
	  ++ubNumAttribsFound;
	}
	
	// Sig it.
	++ubIt;
  }
  
  // Se retorna resultado
  delete[uwMaxAttribs] pszAttribs;
  return bResult;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la informacion base referida al interfaz para el control horario.
// - Interiormente se llamara al metodo para situar la hora en el valor por
//   defecto de 0.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo fue correcto true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool
CGUIWMainInterfaz::TLoadTimeControllerInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Procede a establecer la informacion base para el componente horario
  m_TimeController.CfgBaseButton.ID = CGUIWMainInterfaz::ID_TIMECONTROLLER;
  m_TimeController.CfgBaseButton.pGUIWindow = this;
  m_TimeController.CfgBaseButton.GUIEvents = GUIDefs::GUIC_SELECT |
											 GUIDefs::GUIC_UNSELECT |
											 GUIDefs::GUIC_BUTTONLEFT_PRESSED;;
  m_TimeController.CfgBaseButton.ubDrawPlane = 1;
  
  // Se cargan datos asociados al interfaz
  pParser->SetVarPrefix("TimeController.");
  m_TimeController.CfgBaseButton.Position = pParser->ReadPosition();
  m_TimeController.NoSelectAlpha = pParser->ReadAlpha("Unselect.");

  // Se establece la hora por defecto a 0
  // Nota: se establecera un valor de hora actual no valido para que
  // el metodo SetTimeController lo encuentre diferente a la hora 0 y
  // cargue el boton interesado
  m_TimeController.ubActHour = 100;
  SetTimeController(0, false);
  
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
CGUIWMainInterfaz::EndComponents(void)
{
  // Finaliza components
  // Imagen de fondo
  m_BackImgInfo.Background.End();  
  
  // Consola
  m_ConsoleInfo.Text.End();
  m_ConsoleInfo.ButtonUp.End();
  m_ConsoleInfo.ButtonDown.End();

  // Finaliza slots de equipamiento principales
  m_EquipSlotsInfo.LeftSlot.Slot.End();  
  m_EquipSlotsInfo.RightSlot.Slot.End();
  m_EquipSlotsInfo.LeftSlot.SlotName.End();
  m_EquipSlotsInfo.RightSlot.SlotName.End();
  m_EquipSlotsInfo.LeftSlot.HandIcon.End();
  m_EquipSlotsInfo.RightSlot.HandIcon.End();

  // Retrato
  m_PortraitInfo.Portrait.End();

  // Sintomas
  // Libera lista de sintomas activos
  m_SymptomsInfo.ActiveSymptoms.clear();
  // Libera componentes en memoria 
  sSymptomsInfo::SymptomIconMapIt It(m_SymptomsInfo.Icons.begin());
  while (It != m_SymptomsInfo.Icons.end()) {
	delete It->second;
	It = m_SymptomsInfo.Icons.erase(It);
  }

  // Opciones de combate
  m_CombatOptionsInfo.NextTurn.End();
  m_CombatOptionsInfo.FleeCombat.End();

  // Atributos
  byte ubIt = 0;
  for (; ubIt < CGUIWMainInterfaz::sAttributesInfo::MAX_ATTRIBUTES_TO_SHOW; ++ubIt) {
	m_AttributesInfo.Attribs[ubIt].Name.End();
	m_AttributesInfo.Attribs[ubIt].Percentage.End();
	m_AttributesInfo.Attribs[ubIt].Numeric.End();
  }

  // Controlador horario
  m_TimeController.ActButton.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Bloquea / desbloquea la entrada para la ventana de interfaz y todos sus
//   componentes.
// Parametros:
// - bActive. Flag de bloqueo / desbloqueo.
// Devuelve:
// Notas:
// - Tambien establece de forma correcta los elementos de interfaz asociados
//   a la posicion del cursor, mediante la realizacion de una llamada a 
//   OnMouseMoved simulada.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::SetInput(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
	
  // Se modifica el estado si procede
  if (bActive != Inherited::IsInput()) {
	// Imagen de fondo	
	m_BackImgInfo.Background.SetInput(bActive);
  
	// Consola
	m_ConsoleInfo.Text.SetInput(bActive);
    
	// Consola / botones
	m_ConsoleInfo.ButtonUp.SetInput(bActive);
	m_ConsoleInfo.ButtonDown.SetInput(bActive);
	
	// Cara
	m_PortraitInfo.Portrait.SetInput(bActive);
	
	// Slots
	m_EquipSlotsInfo.LeftSlot.Slot.SetInput(bActive);
	m_EquipSlotsInfo.RightSlot.Slot.SetInput(bActive);
	m_EquipSlotsInfo.LeftSlot.SlotName.SetInput(bActive);
	m_EquipSlotsInfo.RightSlot.SlotName.SetInput(bActive);
  
	// Opciones de combate
	m_CombatOptionsInfo.NextTurn.SetInput(bActive);
	m_CombatOptionsInfo.FleeCombat.SetInput(bActive);

	// Sintomas
	// Recorre la lista de sintomas activos actuando sobre ellos
	sSymptomsInfo::SymptomActiveSetIt It(m_SymptomsInfo.ActiveSymptoms.begin());
	for (; It != m_SymptomsInfo.ActiveSymptoms.end(); ++It) {
	  // Se toma el nodo con el icono del sintoma
	  const sSymptomsInfo::SymptomIconMapIt IconIt(m_SymptomsInfo.Icons.find(*It));
	  ASSERT((IconIt != m_SymptomsInfo.Icons.end()) != 0);
	  IconIt->second->Icon.SetInput(bActive);
	}
	
	// Se ajustan valores de interfaz
	// ¿Se desea restaurar la entrada?
	if (bActive) {
	  // Se desvincula cualquier info de interaccion y se cambia el estado
	  ReleaseEntityInteractInfo();
	  ActiveInterfazState(m_InterfazInfo.State,
						  m_InterfazInfo.PrevState);
	} else {
	  // Se ocultara cursor y selector de movimiento
	  m_pGUIManager->SetGUICursor(GUIManagerDefs::NO_CURSOR);
	  m_pWorld->SetDrawSelectCell(false);

	  // En caso de querer deshabilitar la entrada y estar el panel de control
	  // activo, este se deseleccionara mediante simulacion
	  // Nota: La razon de esta accion, es la de que a la hora de 
	  // devolver la entrada, el estado de seleccion / no seleccion en
	  // el panel sea el correcto.
	  if (m_TimeController.ActButton.IsSelect()) {
		OnSelectNotify(CGUIWMainInterfaz::ID_TIMECONTROLLER, false);
	  }

	  // Por ultimo, se deseleccionaran los componentes para evitar
	  // que sigan activos pese a haber establecido otra interfaz
	  // Nota: Se realizara simulando deseleccion
	  OnSelectNotify(CGUIWMainInterfaz::ID_CONSOLE_SCROLL_UP, false);
	  OnSelectNotify(CGUIWMainInterfaz::ID_CONSOLE_SCROLL_DOWN, false);
	  OnSelectNotify(CGUIWMainInterfaz::ID_PORTRAIT, false);
	  OnSelectNotify(CGUIWMainInterfaz::ID_SLOT_LEFT, false);	  
	  OnSelectNotify(CGUIWMainInterfaz::ID_SLOT_RIGHT, false);	  
	  OnSelectNotify(CGUIWMainInterfaz::ID_NEXTTURNBUTTON, false);
	  OnSelectNotify(CGUIWMainInterfaz::ID_FLEECOMBATBUTTON, false);
	  OnSelectNotify(CGUIWMainInterfaz::ID_TIMECONTROLLER, false);
	}	

    // Interfaz de control de tiempo
    m_TimeController.ActButton.SetInput(bActive);
	
	// Se propaga a clase base
	Inherited::SetInput(bActive);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coloca el estado de interfaz apropiado basandose en lo recibido.
// Parametros:
// - InitState. Estado con el que inicializar el interfaz.
// - InitPrevState. Valor para el estado que sera previo
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::ActiveInterfazState(const CGUIWMainInterfaz::eInterfazState InitState,
									   const CGUIWMainInterfaz::eInterfazState InitPrevState)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Establece el estado de interfaz
  m_InterfazInfo.State = InitState;
  m_InterfazInfo.PrevState = InitPrevState;
  	
  // Se obtiene la posicion en x,y actual del raton y se realiza un
  // simulacro de evento de movimiento, para recolocar la interfaz, pues 
  // durante el tiempo que esta ha estado deshabilitada, la posicion del
  // cursor habra podido variar. 
  // Nota: En caso de que el estado sea INIT, se recolocara la interfaz en la
  // posicion central de pantalla.
  iCInputManager* const pInputManager = SYSEngine::GetInputManager();
  ASSERT(pInputManager);    
  if (CGUIWMainInterfaz::INIT == InitState) {
	m_InterfazInfo.Pos.swXPos = m_pGraphSys->GetVideoWidth() >> 1;
	m_InterfazInfo.Pos.swYPos = m_pGraphSys->GetVideoHeight() >> 1;
	pInputManager->SetMousePos(m_InterfazInfo.Pos);
  } else {  
	m_InterfazInfo.Pos = pInputManager->GetLastMousePos();	
  }   
  OnMouseMoved();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve el cursor de interaccion que proceda. El cursor sera distinto
//   si esta seleccionado un slot de armamento que si no lo esta.
// Parametros:
// - bInteractPossible. Flag para indicar la clase concreta de cursor de
//   interaccion elegido.
// Devuelve:
// - El identificador del cursor adecuado.
// Notas:
//////////////////////////////////////////////////////////////////////////////
GUIManagerDefs::eGUICursorType 
CGUIWMainInterfaz::GetEntityInteractCursor(const bool bInteractPossible) const
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Esta seleccionado algun slot de armamento?
  if (m_EquipSlotsInfo.bIsWeaponSelect) {	  
	// Si, se devuelve el cursor de interaccion para golpear
	return (bInteractPossible) ? 
  			GUIManagerDefs::ENTITYINTERACTFORHIT_CURSOR : 
			GUIManagerDefs::NOPOSSIBLE_ENTITYINTERACTFORHIT_CURSOR;	  	
  } else {	  
	// No, se devuelve el cursor de interaccion normal
	return (bInteractPossible) ? 
	  		GUIManagerDefs::ENTITYINTERACT_CURSOR : 
			GUIManagerDefs::NOPOSSIBLE_ENTITYINTERACT_CURSOR;	      
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se activa la ventana de interfaz activando todos los componentes que
//   tenga asociados.
// Parametros:
// - bDoFadeIn. ¿Activacion realizando un FadeIn?. Por defecto es true
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::Active(const bool bDoFadeIn)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se activa si procede
  if (!Inherited::IsActive()) {		
	// ¿Hay que realizar FadeIn?
	if (bDoFadeIn) {
	  // Establece el FadeIn
	  Inherited::DoFadeIn(CGUIWMainInterfaz::ID_ENDFADEIN, 196);	  	
	}

	// Se activan componentes 	
	ActiveComponents(true);

	// Se inicializa la posicion de los cursores de interfaz
	// estableciendo como estado previo el de movimiento de entidad
	ActiveInterfazState(CGUIWMainInterfaz::INIT,
						CGUIWMainInterfaz::MOVING_PLAYER);

	// Se asocia informacion relativa al jugador
	AttachPlayerInfo();

    // Se establece la interfaz de control horario
	SetTimeController(m_pWorld->GetHour());
	
	// Se asocia la interfaz como observadora del universo de juego
	m_pWorld->AddObserver(this);

	// Se hallan las areas asociadas a interfaces
	// Nota: Se añadira una unidad a los extremos, porque el metodo que controla
	// la interseccion de un punto sobre un rectagulo, no los considerara y a la
	// hora de trabajar con la entrada, se podra recibir los extremos del rectangulo
	// (es decir, para el metodo no es 640, sino 639, pero para el sistema de
	// entrada, se contabiliza 640).
	// Barra de interfaz
	m_InterfazBarArea.swTop = m_pGraphSys->GetVideoHeight() - 
						      m_BackImgInfo.Background.GetHeight();
	m_InterfazBarArea.swLeft = 0;
	m_InterfazBarArea.swBottom = m_pGraphSys->GetVideoHeight() + 1;
	m_InterfazBarArea.swRight = m_BackImgInfo.Background.GetWidth() + 1;
	
	// Controlador de tiempo
	m_TimeControllerArea.swTop = m_TimeController.ActButton.GetYPos();
	m_TimeControllerArea.swLeft = m_TimeController.ActButton.GetXPos();
	m_TimeControllerArea.swBottom = m_TimeControllerArea.swTop +
									m_TimeController.ActButton.GetHeight() + 1;
	m_TimeControllerArea.swRight = m_TimeControllerArea.swLeft + 
								   m_TimeController.ActButton.GetWidth() + 1;

	// Vacia conjunto de teclas pulsadas
	m_KeyPressedSet.clear();

	// Se comunica a la clase base que se active
	Inherited::Active();  	
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva todos los componentes asociados a la ventana de interfaz.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se desactiva si procede
  if (Inherited::IsActive()) { 			
	// Se desactivan distintos componentes
	ActiveComponents(false);
	
	// Se establece estado de desactivacion en el interfaz
	SetInterfazState(CGUIWMainInterfaz::DESACTIVATED);

	// Se desvincula la info de seleccion
	ReleaseEntityInteractInfo();	

	// Se elimina cualquier informacion del jugador
	RemovePlayerInfo();
	
	// Se asocia la interfaz como observadora del universo de juego
	m_pWorld->RemoveObserver(this);	
	 
	// Se comunica a la clase base que se Deactive  
	Inherited::Deactive();  
  }    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desvincula cualquier tipo de informacion relativa al jugador
// Parametros:
// Devuelve:
// Notas:
// - Este metodo se llamara siempre que se Deactive el interfaz o bien 
//   cuando se reciba un evento de que se va a finalizar una sesion de juego
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::RemovePlayerInfo(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se desasocia retrato al selector para acceso a inventario
  m_PortraitInfo.Portrait.AttachSprite(NULL, 0);
	
  // Se desasocian posibles items equipados		
  SetMainEquipmentSlots(m_EquipSlotsInfo.LeftSlot, 
					    RulesDefs::EQUIPMENT_SLOT_0, 
						false);	
  SetMainEquipmentSlots(m_EquipSlotsInfo.RightSlot, 
					    RulesDefs::EQUIPMENT_SLOT_1, 
						false);	
	
  // Se libera la lista de sintomas activos
  m_SymptomsInfo.ActiveSymptoms.clear();

  // Se desasocia la interfaz como observadora del jugador si procede
  // Nota: Lo normal es que la instancia no este activa porque al finalizar
  // esta interfaz se habra finalizado la sesion de juego (ver CGSInGame::End)
  // por lo que si se llamara, habria un error via ASSERT al detectarse que se
  // quiere hacer una operacion sobre el jugador estando este finalizado
  CPlayer* const pPlayer = m_pWorld->GetPlayer();
  if (pPlayer && pPlayer->IsInitOk()) {
    pPlayer->RemoveObserver(this);  
	pPlayer->GetEquipmentSlots()->RemoveObserver(this);
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Vincula cualquier tipo de informacion asociada al jugador
// Parametros:
// Devuelve:
// Notas:
// - Este metodo se llamara siempre que se active el interfaz o bien 
//   cuando se reciba un evento de que se va a finalizar una sesion de juego
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::AttachPlayerInfo(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se asocia retrato al selector para acceso a inventario
  m_PortraitInfo.Portrait.AttachSprite(m_pWorld->GetPlayer()->GetPortrait(), 0);
	
  // Se asocian posibles items equipados
  SetMainEquipmentSlots(m_EquipSlotsInfo.LeftSlot, 
						RulesDefs::EQUIPMENT_SLOT_0, 
						true);	
  SetMainEquipmentSlots(m_EquipSlotsInfo.RightSlot, 
						RulesDefs::EQUIPMENT_SLOT_1, 
						true);	
	
  // Se establece info asociada a atributos
  SetAttributesInfo();
	
  // Se establece sintomas que pudieran estar activos
  SetActiveSymptoms();
	
  // Se asocia la interfaz como observadora del jugador y su slots de equip.
  m_pWorld->GetPlayer()->AddObserver(this);
  m_pWorld->GetPlayer()->GetEquipmentSlots()->AddObserver(this);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el interfaz controlador de tiempo deacuerdo a la hora ubHour.
// Parametros:
// - ubHour. Hora que debe de representar el controlador.
// - bActive. Flag indicador de si hay que activar o no el componente.
//   Por defecto valdra true.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::SetTimeController(const byte ubHour,
									 const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT((ubHour < 24) != 0);

  // ¿Se quiere establecer una hora distinta a la actual?
  if (ubHour != m_TimeController.ubActHour) {	
	// Se obtiene parser y se situa
	CCBTEngineParser* const pParser = m_pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
														      "[MainGameInterfaz]");
	ASSERT(pParser);

	// Se crea vble y se lee plantilla de animacion
	pParser->SetVarPrefix("TimeController.");
	std::string szVar;
	SYSEngine::PassToString(szVar, "TimeButton[%u].AnimTemplate", ubHour);
	m_TimeController.CfgBaseButton.szATButton = pParser->ReadString(szVar);

	// Se reinicializa componente  
	m_TimeController.ActButton.End();
	m_TimeController.ActButton.Init(m_TimeController.CfgBaseButton);
	ASSERT(m_TimeController.ActButton.IsInitOk());
	m_TimeController.ActButton.SetActive(bActive);

	// Se guarda la hora establecida
	m_TimeController.ubActHour = ubHour;
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa / desactiva componentes
// - A la hora de activar comopnentes, en el caso concreto de sintomas, estos
//   se trataran aunque no esten visualizandose, pues este metodo alcanza a 
//   todos los sintomas.
// Parametros:
// - bActive. Flag de activacion / desactivacion.
// Devuelve:
// Notas:
// - En caso de activiar y hallarse el universo de juego en pausa, se
//   actuara sobre diversos componentes
//////////////////////////////////////////////////////////////////////////////
void
CGUIWMainInterfaz::ActiveComponents(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se activan / desactivan componentes
  // Imagen de fondo
  m_BackImgInfo.Background.SetActive(bActive);  

  // Consola
  m_ConsoleInfo.Text.SetActive(bActive);
  m_ConsoleInfo.ButtonUp.SetActive(bActive);
  m_ConsoleInfo.ButtonDown.SetActive(bActive);

  // Retrato    
  m_PortraitInfo.Portrait.SetActive(bActive);  

  // Slots de equipamiento
  m_EquipSlotsInfo.LeftSlot.Slot.SetActive(bActive);
  m_EquipSlotsInfo.RightSlot.Slot.SetActive(bActive);
  m_EquipSlotsInfo.LeftSlot.SlotName.SetActive(bActive);
  m_EquipSlotsInfo.RightSlot.SlotName.SetActive(bActive);

  // En el caso de los slots, al activarlos pudiera ser que se regresara del 
  // inventario, en ese caso, si se estaba en modo combate al haber
  // seleccionado un slot este se debera de volver a seleccionar
  // ¿Habia algun slot seleccionado?
  if (bActive &&
	  m_EquipSlotsInfo.bIsWeaponSelect) {
    // Si, se volvera a seleccionar
    switch (m_EquipSlotsInfo.WeaponSlot) {
  	  case RulesDefs::EQUIPMENT_SLOT_0: {
	    m_EquipSlotsInfo.LeftSlot.Slot.Select(bActive);
	  } break;

	  case RulesDefs::EQUIPMENT_SLOT_1: {
	    m_EquipSlotsInfo.RightSlot.Slot.Select(bActive);
	  } break;
	}; // ~ switch
  }

  // Sintomas
  sSymptomsInfo::SymptomIconMapIt IconIt(m_SymptomsInfo.Icons.begin());
  for (; IconIt != m_SymptomsInfo.Icons.end(); ++IconIt) {
	IconIt->second->Icon.SetActive(bActive);	
  }

  // Opciones de combate
  m_CombatOptionsInfo.NextTurn.SetActive(bActive);
  m_CombatOptionsInfo.FleeCombat.SetActive(bActive);

  // Atributos
  byte ubIt = 0;
  for (; ubIt < CGUIWMainInterfaz::sAttributesInfo::MAX_ATTRIBUTES_TO_SHOW; ++ubIt) {		
	// ¿Se desea mostrar atributo?	
	if (m_AttributesInfo.Attribs[ubIt].ubID) { 	
	  // Nombre / Porcentaje / Info numerica
	  m_AttributesInfo.Attribs[ubIt].Name.SetActive(bActive);		
	  m_AttributesInfo.Attribs[ubIt].Percentage.SetActive(bActive);		
	  m_AttributesInfo.Attribs[ubIt].Numeric.SetActive(bActive);
	}
  }  
  
  // Controlador de tiempo SOLO si no se esta en modo turno
  if (m_InterfazInfo.Mode != WorldDefs::TURN_MODE) {
	m_TimeController.ActButton.SetActive(bActive);  
  }

  // En caso de estar en universo de juego en pausa y activar, se debera
  // de actuar sobre diversos componentes con posible animacion
  if (m_pWorld->IsInPause() &&
	  bActive) {
	SetPauseInComponents(true);
  }	    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Localiza los sintomas que esten activos en el jugador y los asocia a la
//   lista de sintomas activos para que estos sean dibujados.
// - Este metodo sera llamado desde Active una vez que se hayan activado los
//   componentes que representan a los sintomas.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::SetActiveSymptoms(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se procede a comprobar que sintomas estan activos en el jugador
  // y a insertarlos en la lista de sintomas activos
  const word uwMaxSymptoms = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_SYMPTOM);
  byte ubIt = 0;
  for (; ubIt < uwMaxSymptoms; ++ubIt) {	
	// ¿Esta el sintoma activo?
	const RulesDefs::eIDSymptom Symp = RulesDefs::eIDSymptom(RulesDefs::SYMPTOM_01 << ubIt);
	if (m_pWorld->GetPlayer()->IsSymptomActive(Symp)) {
	  // Si, se añade a la lista y se selecciona
	  m_SymptomsInfo.ActiveSymptoms.insert(Symp);
	  SelectSymptomIcon(Symp, true);	  
	}
  }    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Localiza el sintoma Symptom y lo selecciona o lo deselecciona segun sea
//   el flag bSelect.
// - Los sintomas solo se podran seleccionar o deseleccionar cuando estos dejen
//   de estar activos o pasen a estarlos. Este metodo NO SE LLAMARA desde 
//   el gestor de entrada para componentes ya que los iconos de sintomas no
//   podran recibir entrada, seran solo de caracter informativo.
// Parametros:
// - Symptom. Sintoma sobre el que actuar.
// - bSelect. Flag de seleccion.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWMainInterfaz::SelectSymptomIcon(const RulesDefs::eIDSymptom& Symptom,
									 const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se obtiene el sitoma
  const sSymptomsInfo::SymptomIconMapIt It = m_SymptomsInfo.Icons.find(Symptom);
  ASSERT((It != m_SymptomsInfo.Icons.end()) != 0);
  It->second->Icon.Select(bSelect);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia / desasocia los items que pudieran estar equipados en los slots 
//   principales.
// Parametros:
// - SlotInfo. Estructura con la informacion del slot sobre el que trabajar.
// - IdSlot. Identificador del slot.
// - bAttach. Flag de asociacion / desasocion
// Devuelve:
// Notas:
// - Este metodo sera llamado desde Active y Deactive.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::SetMainEquipmentSlots(sEquipmentSlotsInfo::sSlotInfo& SlotInfo,
										 const RulesDefs::eIDEquipmentSlot& IdSlot,
										 const bool bAttach)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Jugador existe y activo?
  CPlayer* const pPlayer = m_pWorld->GetPlayer();
  if (pPlayer && 
	  pPlayer->IsInitOk()) {
	// Se obtienen los items que posee el jugador en las manos
	iCEquipmentSlots* const pEquipSlots = pPlayer->GetEquipmentSlots();
	ASSERT(pEquipSlots);
  
	// ¿Hay item equipado en el slot IdSlot?
	const AreaDefs::EntHandle hItem = pEquipSlots->GetItemEquipedAt(IdSlot);    
	if (hItem) {		
	  // Si, se toma su instancia y se establece el nombre
	  CItem* const pItem = m_pWorld->GetItem(hItem);
	  ASSERT(pItem);
	  // ¿Se desan asociar?
	  if (bAttach) {
		// Si, se asociara el item que haya puesto
		SlotInfo.SlotName.ChangeText(pItem->GetName());
	
		// ¿El slot estaba previamente seleccionado? y
		// ¿NO se ha asociado un arma ofensiva?
		if (SlotInfo.Slot.IsSelect() &&
			0 == m_pGDBase->GetRulesDataBase()->GetUseWeaponItemCost(pItem->GetType())) {
		  // Se deselecciona
		  SlotInfo.Slot.Select(false);	  
		}
		
		// Se quita pausa si no se esta en estado de pausa y se asocia
		if (!m_pWorld->IsInPause()) {
		  pItem->SetPause(false);
		}
		SlotInfo.Slot.AttachSprite(pItem, pItem->GetHandle());	
	  } else {
		// No, se pausara y se desasociara
  		pItem->SetPause(true);	  
		SlotInfo.Slot.AttachSprite(NULL, 0);
	  }	
	} else {
	  // No hay nada asociado
	  // ¿Se desean asociar?
	  if (bAttach) {
		// Si, se mostrara la mano
		SlotInfo.SlotName.ChangeText(m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_EQUIPMENT_SLOT,
															  IdSlot));
		SlotInfo.Slot.AttachSprite(&SlotInfo.HandIcon, 0);
	  } else {
		// No, se desasociara
		SlotInfo.Slot.AttachSprite(NULL, 0);
	  }	
	}   
  } else {
	// No se hallo, ¿se desea desasociar?
	if (!bAttach) {
	  // Se hace de forma directa, ya que es seguro que se habra terminado 
	  // una sesion de juego y las instancias hItem no sean validas
	  SlotInfo.Slot.AttachSprite(NULL, 0);
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se establece la pausa en los posibles items que se hallan en los
//   slots principales (estos items, reciben el evento de que son visibles
//   a traves de este interfaz, y desde aqui se les debera tambien de pausar).
// Parametros:
// - bPause. Flag a asociar al estado de pausa.
// - IDSlot. Slot sobre el que trabajar.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::SetPauseInEquipementSlot(const bool bPause,
											const RulesDefs::eIDEquipmentSlot& IdSlot)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // ¿Jugador existe y activo?
  CPlayer* const pPlayer = m_pWorld->GetPlayer();
  if (pPlayer && 
	  pPlayer->IsInitOk()) {
	// Se obtienen los items que posee el jugador en las manos
	iCEquipmentSlots* const pEquipSlots = pPlayer->GetEquipmentSlots();
	ASSERT(pEquipSlots);
  
	// ¿Hay item equipado en el slot IdSlot?
	const AreaDefs::EntHandle hItem = pEquipSlots->GetItemEquipedAt(IdSlot);    
	if (hItem) {		
	  // Si, se toma su instancia y se pausa
	  CItem* const pItem = m_pWorld->GetItem(hItem);
	  ASSERT(pItem);
	  pItem->SetPause(bPause);
	} 
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Actua sobre los componentes, estableciendoles la pausa que no sera otra
//   opcion que establecer el flag de activacion / desactivacion de animacion
// - Solo se actuara sobre los componentes preciosos, ya que otros como el
//   de retrato seran controlados directamente por el jugador
// Parametros:
// - bPause. Flag de pausa
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWMainInterfaz::SetPauseInComponents(const bool bPause)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Consola / botones
  m_ConsoleInfo.ButtonUp.StopAnim(bPause);
  m_ConsoleInfo.ButtonDown.StopAnim(bPause);

  // Opciones de combate
  m_CombatOptionsInfo.NextTurn.StopAnim(bPause);
  m_CombatOptionsInfo.FleeCombat.StopAnim(bPause);
  
  // Controlador de tiempo SOLO si no se esta en modo turno
  if (m_InterfazInfo.Mode != WorldDefs::TURN_MODE) {
	m_TimeController.ActButton.StopAnim(bPause);
  }

  // Se recorren los sintomas
  sSymptomsInfo::SymptomIconMapIt IconIt(m_SymptomsInfo.Icons.begin());
  for (; IconIt != m_SymptomsInfo.Icons.end(); ++IconIt) {
	IconIt->second->Icon.StopAnim(bPause);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece por primera vez la informacion referida a los atributos del
//   jugador que deberan de ser mostradas en pantalla.
// - Este metodo sera de apyo a Active para realizar dicha operacion nada
//   mas activar la interfaz.
// Parametros:
// Devuelve:
// Notas:
// - Al cargar la informacion referida a que atributos a mostrar, se 
//   almaceno un identificador entre 1 y el numero maximo de atributos
//   extendidos existentes. Si el identificador es 1, querra decir que se
//   quiere mostrar la salud y si es entre 2 y el numero maximo de
//   atributos extendidos existentes menos uno, es un atributo extendido.
//   En caso de que el identificador fuera 0, significaria que ese atributo
//   no se desea visualizar.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::SetAttributesInfo(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
 
  // Se activan componentes
  byte ubIt = 0;
  for (; ubIt < CGUIWMainInterfaz::sAttributesInfo::MAX_ATTRIBUTES_TO_SHOW; ++ubIt) {
	// ¿Atributo NO visualizable?
	if (!m_AttributesInfo.Attribs[ubIt].ubID) {
	  continue;
	}

	// Vbles
	std::string szText; // Texto asociado al atributo
	sword swBase;       // Valor base
	sword swTemp;       // Valor temporal
	
	// Barra de porcentaje y texto asociado    
	switch(m_AttributesInfo.Attribs[ubIt].ubID) {
	  case 1: {
		swBase = m_pWorld->GetPlayer()->GetHealth(RulesDefs::BASE_VALUE);
		swTemp = m_pWorld->GetPlayer()->GetHealth(RulesDefs::TEMP_VALUE);		  
		szText = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_HEALTH);		
	  } break;

	  default: {
		// ID al atributo extendido sobre el que solicitar info
		// Vbles
		RulesDefs::eIDExAttrib IDExAttr = RulesDefs::eIDExAttrib(RulesDefs::EX_ATTRIB_0 << (m_AttributesInfo.Attribs[ubIt].ubID - 2));
		szText = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_EXT_ATTRIB,
										  m_AttributesInfo.Attribs[ubIt].ubID - 2);
		swBase = m_pWorld->GetPlayer()->GetExAttribute(IDExAttr, RulesDefs::BASE_VALUE);
		swTemp = m_pWorld->GetPlayer()->GetExAttribute(IDExAttr, RulesDefs::TEMP_VALUE);
	  } break;		
	}; // ~ switch

	// Se establece texto
	m_AttributesInfo.Attribs[ubIt].Name.ChangeText(szText);
	// Se establecen los valores en la barra de porcentaje y se activa
	m_AttributesInfo.Attribs[ubIt].Percentage.ChangeBaseAndTempValue(swBase, swTemp);

	// Se establece el valor numerico y se activa
	std::string szNumericValue;
	SYSEngine::PassToString(szNumericValue, "%d \\ %d", swTemp, swBase);
	m_AttributesInfo.Attribs[ubIt].Numeric.ChangeText(szNumericValue);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el modo de funcionamiento del interfaz que podra ser en base
//   a funcionamiento por turnos o bien en base a funcionamiento en tiempo
//   real.
// Parametros:
// - Mode. Modo de funcionamiento.
// - hCriatureInTurn. Handle a la criatura con el turno de funcionamiento.
//   Esta criatura solo se utilizara si se establece el modo por turnos y su
//   valor por defecto podra ser 0.
// Devuelve:
// Notas:
// - En modo de funcionamiento por turnos, la criatura tambien podra tener
//   asociada valor 0 indicando que no hay criatura con turno.
// - Cuando se pase a modo turno y la entidad con el turno sea nula, a la 
//   hora de centrar el motor isometrico, se usara una posicion que sera
//   el jugador si antes estaba el modo real o un npc si ya se estaba en modo
//   turno y el handle asociado no fuera nulo. En caso de que fuera nulo, no
//   se hara nada porque se supondra que el isometrico ya estara bien 
//   centrado.
// - Al centrarse la camara se hara siempre SIN travelling.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::SetInterfazMode(const WorldDefs::eWorldMode& Mode, 
								   const AreaDefs::EntHandle& hCriatureInTurn)
{
  
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Se desea poner en modo real?
  if (Mode == WorldDefs::REAL_MODE) {
	// Si, ¿Esta establecido el modo por turnos?
	if (m_InterfazInfo.Mode == WorldDefs::TURN_MODE) {
	  // Desvincula cualquier info de interaccion
	  ReleaseEntityInteractInfo();

	  // Se desbloquea entrada, dejando como estado primario el mov.
	  m_InterfazInfo.State = CGUIWMainInterfaz::INIT;
	  m_InterfazInfo.PrevState = CGUIWMainInterfaz::MOVING_PLAYER;
	  SetInput(true);
	  
	  // Se activa el interfaz de control horario
	  m_TimeController.ActButton.SetActive(true);
	  
	  // Se establece la interaccion normal para el jugador y se asocia camara
	  ActiveInterfazState(CGUIWMainInterfaz::INIT,
						  CGUIWMainInterfaz::MOVING_PLAYER);	   
	  m_pWorld->AttachCamera(m_pWorld->GetPlayer()->GetHandle(), false);	  

	  // Se establece el modo
	  m_InterfazInfo.Mode = Mode;
	  ASSERT((m_InterfazInfo.Mode == m_pWorld->GetMode()) != 0);
	}
  } else {
	// No, se desea poner en modo por turnos
	// Desvincula cualquier info de interaccion
	ReleaseEntityInteractInfo();

	// ¿Entidad nula?
	if (!hCriatureInTurn) {
	  // Se quita cursor y se bloquea interfaz
	  m_pGUIManager->SetGUICursor(GUIManagerDefs::NO_CURSOR);
	  SetInput(false);
	  
	  // Se centra la vista del isometrico si procede (consultar Notas)
	  CCriature* const pCriature = (m_InterfazInfo.Mode == WorldDefs::REAL_MODE) ?
									m_pWorld->GetPlayer() : m_InterfazInfo.pCriatureInTurn;
	  if (pCriature) {
		// Se obtiene la posicion universal y se centra en ella
		const sPosition WorldPos(m_pWorld->PassTilePosToWorldPos(pCriature->GetTilePos()));
	  }

	  // Se desvincula criatura con turno
	  m_InterfazInfo.pCriatureInTurn = NULL;
	} else {
	  // Entidad no nula
	  // ¿La entidad es un NPC?
	  if (hCriatureInTurn != m_pWorld->GetPlayer()->GetHandle()) {
		// Se bloquea entrada y se asocia cursor de espera
		SetInput(false);
		m_pGUIManager->SetGUICursor(GUIManagerDefs::WAITING_CURSOR);
	  } else {
		// Se desbloquea entrada, dejando como estado primario el mov.
		m_InterfazInfo.State = CGUIWMainInterfaz::INIT;
		m_InterfazInfo.PrevState = CGUIWMainInterfaz::MOVING_PLAYER;	  
		SetInput(true);		
	  }

	  // Se centra la vista en la entidad y se guarda su instancia
	  m_InterfazInfo.pCriatureInTurn = m_pWorld->GetCriature(hCriatureInTurn);	  
	  ASSERT(m_InterfazInfo.pCriatureInTurn);
	}	

	// ¿El modo actual no es por turnos?
	if (m_InterfazInfo.Mode != WorldDefs::TURN_MODE) {
	  // Se establece y se desvincula interfaz de control horario
	  m_InterfazInfo.Mode = Mode;
	  ASSERT((m_InterfazInfo.Mode == m_pWorld->GetMode()) != 0);
	  m_TimeController.ActButton.SetActive(false);	
	}	
  }    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion sobre un suceso sobre el jugador o la entidad seleccionada
//   para la interaccion.
// Parametros:
// - hCriature. Handle a la criatura.
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
										  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
										  const dword udParam)
{
  // SOLO si intancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Interfaz activa?
  if (Inherited::IsActive()) {
	// ¿La entidad es el jugador?
	if (m_pWorld->GetPlayer()->GetHandle() == hCriature) {
	  // Si, se captura el suceso
	  switch (NotifyType) {
		case CriatureObserverDefs::SYMPTOM_ACTIVE: {
		  // Activacion de un sintoma		  
		  // Se añade a la lista de sintomas activos
		  const RulesDefs::eIDSymptom Symp = RulesDefs::eIDSymptom(udParam);
		  m_SymptomsInfo.ActiveSymptoms.insert(Symp);

		  // Se selecciona
		  SelectSymptomIcon(Symp, true);
		} break;

		case CriatureObserverDefs::SYMPTOM_DEACTIVE: {
		  // Desactivacion de un sintoma
		  // Se quita de la lista de sintomas activos 
		  const RulesDefs::eIDSymptom Symp = RulesDefs::eIDSymptom(udParam);
		  sSymptomsInfo::SymptomActiveSetIt It(std::find(m_SymptomsInfo.ActiveSymptoms.begin(),
														 m_SymptomsInfo.ActiveSymptoms.end(),
														 Symp));
		  ASSERT((It != m_SymptomsInfo.ActiveSymptoms.end()) != 0);
		  m_SymptomsInfo.ActiveSymptoms.erase(It);		

		  // Se deselecciona
		  SelectSymptomIcon(Symp, false);
		} break;

		case CriatureObserverDefs::HEALTH_MODIFY:
		case CriatureObserverDefs::EXTENDED_ATTRIBUTE_MODIFY: {
		  // La salud o un atributo extendido se modifico
		  // Se reflejan cambios
		  SetAttributesInfo();
		} break;

		case CriatureObserverDefs::IS_DEATH: {
		  // La criatura ha muerto, se reflejan atributos
		  SetAttributesInfo();
		} break;		

   		case CriatureObserverDefs::ACTION_REALICE: {
		  // Se ha realizado una accion
		  // Se comprueba la accion realizada
		  const RulesDefs::eCriatureAction Action = RulesDefs::eCriatureAction(udParam & 0x0000FFFF);
		  switch(Action) {
			case RulesDefs::CA_HIT: {	
			  // Se ha golpeado, por lo que habra que actualizar la
			  // informacion asociada a la posible criatura golpeada
			  std::string szFloatingText;	
			  if (m_EquipSlotsInfo.bIsWeaponSelect &&
				  m_InterfazInfo.pEntityInteract &&
				  m_InterfazInfo.pEntityInteract->GetEntityType() == RulesDefs::CRIATURE) {
				// Si, Obtiene el texto informativo y se actualiza
				szFloatingText = GetAtackInfoSelect(m_pWorld->GetCriature(m_InterfazInfo.pEntityInteract->GetHandle()));
				m_pGUIManager->SetFloatingText(szFloatingText,
											   m_FloatingTextInfo.RGBColor,
											   m_FloatingTextInfo.Alpha,
											   m_FloatingTextInfo.Type);
			  }
			} break;

			case RulesDefs::CA_WALK: {
			  // Si resulta que la entidad es tambien la que representa al
			  // jugador y ha cambiado de posicion, se desvinculara cualquier
			  // informacion de interaccion
			  if (m_InterfazInfo.pEntityInteract &&
				  m_InterfazInfo.pEntityInteract->GetHandle() == hCriature) {
				// Desvincula informacion de interaccion si ha habido cambio de pos.
				if (m_InterfazInfo.EntityInteractPos != m_pWorld->GetPlayer()->GetTilePos()) {
				  ReleaseEntityInteractInfo();
				  m_pGUIManager->SetGUICursor(GetEntityInteractCursor(false));		  
				}
			  }			  
			} break;			
		  }; // ~ switch
		}
	  }; // ~ switch
	} else {
	  // No, es una criatura NO jugador y ademas es SEGURO que
	  // se trata de la que se halla seleccionada para interaccion
	  // Se comprueba la notificacion	  
	  ASSERT(m_InterfazInfo.pEntityInteract);
	  ASSERT((m_InterfazInfo.pEntityInteract->GetHandle() == hCriature) != 0);	  	  
	  switch (NotifyType) {
		case CriatureObserverDefs::IS_DEATH: {
		  // Ha muerto, se desvincula cualquier tipo de informacion y
		  // se simula pulsacion al boton dcho. para vuelta a estado de mov.
		  ReleaseEntityInteractInfo();		  
		  OnRightMouseClick();		  
		} break;

		case CriatureObserverDefs::ACTION_REALICE: {
		  // Se ha realizado una accion
		  // Se comprueba la accion realizada
		  const RulesDefs::eCriatureAction Action = RulesDefs::eCriatureAction(udParam & 0x0000FFFF);
		  switch(Action) {
			case RulesDefs::CA_WALK: {
			  // Si la nueva posicion es distinta que la original, 
			  // se desvinculara la info. de interaccion.
			 if (m_InterfazInfo.EntityInteractPos != m_pWorld->GetCriature(hCriature)->GetTilePos()) {
			   ReleaseEntityInteractInfo();			   
			   m_pGUIManager->SetGUICursor(GetEntityInteractCursor(false));
			 }									  
			} break;			
		  }; // ~ switch		  
		} break;		
	  }; // ~switch	  
	}
  }  
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
CGUIWMainInterfaz::EquipmentSlotNotify(const EquipmentSlotsDefs::eObserverNotifyType& NotifyType,
									   const RulesDefs::EquipmentSlot& Slot)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT((Slot < RulesDefs::MAX_CRIATURE_EQUIPMENTSLOT) != 0);

  // Se comprueba el tipo de notificacion
  switch(NotifyType) {
	case EquipmentSlotsDefs::ITEM_EQUIPED: 
	case EquipmentSlotsDefs::ITEM_UNEQUIPED: {
	  // ¿Interfaz activa?
	  if (Inherited::IsActive()) {
		// Un item ha sido equipado o desequipado, se actualizan slots
		// Se asocian posibles items equipados
		SetMainEquipmentSlots(m_EquipSlotsInfo.LeftSlot, 
							  RulesDefs::EQUIPMENT_SLOT_0, 
							  true);	
		SetMainEquipmentSlots(m_EquipSlotsInfo.RightSlot, 
							  RulesDefs::EQUIPMENT_SLOT_1, 
							  true);	  
	  }
	} break;
  } // ~ switch
}


///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recoge las notificaciones que manda CWorld cuando ocurre un hecho que
//   debe de ser conocido por el observador.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro de acompañamiento.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
									   const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Comprueba el tipo de notificacion
  switch (NotifyType) {
    case WorldObserverDefs::NEW_HOUR: {
  	  // Se ha pasado a una nueva hora
	  // Se establece la interfaz asociada a esa nueva hora
	  SetTimeController(udParam);
	} break;

	case WorldObserverDefs::END_SESION: {
	  // Se va a finalizar una sesion de juego
	  // Se elimina cualquier informacion del jugador
	  RemovePlayerInfo();
	} break;

	case WorldObserverDefs::NEW_SESION: {
	  // Comienza una nueva sesion de juego
	  // Se vincula informacion relativa al jugador actual
	  AttachPlayerInfo();
	} break;

	case WorldObserverDefs::PAUSE_ON: {
	  // Se acaba de activar la pausa en el universo de juego		
	  // Se pausan posibles items en los slots principales		
	  SetPauseInEquipementSlot(true, RulesDefs::EQUIPMENT_SLOT_0);
	  SetPauseInEquipementSlot(true, RulesDefs::EQUIPMENT_SLOT_1);

	  // Se pausan iconos de sintomas activos
	  SetPauseInComponents(true);
	} break;

	case WorldObserverDefs::PAUSE_OFF: {
	  // Se acaba de desactivar la pausa
	  // Se quita la pausa de los posibles items en los slots principales		
	  SetPauseInEquipementSlot(false, RulesDefs::EQUIPMENT_SLOT_0);
	  SetPauseInEquipementSlot(false, RulesDefs::EQUIPMENT_SLOT_1);
	  
	  // Se quita la pausa de los iconos de sintomas activos
	  SetPauseInComponents(false);
	} break;

	case WorldObserverDefs::ENTITY_DESTROY: {
	  // ¿Interfaz activa?
	  // ¿Se esta en estado de interaccion ? Y
	  // ¿La entidad seleccionada es la destruida?	  
	  if (Inherited::IsActive() &&
		  m_InterfazInfo.pEntityInteract &&
	      udParam == m_InterfazInfo.pEntityInteract->GetHandle()) {
	    // Si, se hace como si se pulsara al boton derecho para pasar
	    // a estado de movimiento y perder la info de la entidad seleccionada
	    OnRightMouseClick();	  
	  }
	} break;

	case WorldObserverDefs::ENTITY_CREATE: {
	  // ¿Interfaz activa? y
	  // ¿Sin interaccion?
	  if (Inherited::IsActive() &&
		  !m_InterfazInfo.pEntityInteract) {
		// Se simula un movimiento del raton para comprobar si al crearse
		// una nueva entidad, esta puede haberse localizado debajo del cursor
		OnMouseMoved();		
	  }
	} break;

	case WorldObserverDefs::VISIBLE_WORLD_UPDATE: {
	  // El area visible del universo de juego ha sido actualizado,
	  // se simulara movimiento del raton si la interfaz posee la entrada
	  // para reestablecer la coherencia visual.
	  if (Inherited::IsActive() && 
		  Inherited::IsInput()) {
		OnMouseMoved();		
	  }		  
	} break;
  }; // ~ switch   
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de comprobar si un item equipado en un slot PRINCIPAL es
//   usable o no como arma. 
// Parametros:
// - Slot. Slot de equipamiento principal
// Devuelve:
// - Si es usable como arma true. En caso contrario false.
// Notas:
// - Podra no existir un item equipado, en ese caso se trabajara con las
//   manos (puños).
//////////////////////////////////////////////////////////////////////////////
bool
CGUIWMainInterfaz::CanUseWeaponInSlot(const RulesDefs::eIDEquipmentSlot& Slot) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT((Slot == RulesDefs::EQUIPMENT_SLOT_0 || 
	      Slot == RulesDefs::EQUIPMENT_SLOT_1) != 0);

  // Se obtiene contenedor de items
  iCEquipmentSlots* const pEquipSlots = m_pWorld->GetPlayer()->GetEquipmentSlots();
  ASSERT(pEquipSlots);
  
  // Se obtiene item equipado en Slot 
  const AreaDefs::EntHandle hItem = pEquipSlots->GetItemEquipedAt(Slot);

  // En caso de que no haya item equipado o 
  // el item equipado sea un arma ofensiva (al tener un coste de uso superior a 0)
  // se podra usar como arma
  if ((0 == hItem) ||
	  (m_pGDBase->GetRulesDataBase()->GetUseWeaponItemCost(m_pWorld->GetItem(hItem)->GetType()) > 0)) {
	return true;
  } 

  // No se puede usar como arma
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada al cliqueo sobre uno de los slots de equipamiento.
// Parametros:
// - EquipSlot. Slot de equipamiento cliqueado.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::ClickOnEquipmentSlot(const RulesDefs::eIDEquipmentSlot& EquipSlot)

{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT((EquipSlot == RulesDefs::EQUIPMENT_SLOT_0 ||
		  EquipSlot == RulesDefs::EQUIPMENT_SLOT_1) != 0);  

  // Se establece el slot cliqueado y el secundario
  sEquipmentSlotsInfo::sSlotInfo& ClickedSlot = (EquipSlot == RulesDefs::EQUIPMENT_SLOT_0) ?
												m_EquipSlotsInfo.LeftSlot : m_EquipSlotsInfo.RightSlot;
  sEquipmentSlotsInfo::sSlotInfo& SecondSlot = (EquipSlot == RulesDefs::EQUIPMENT_SLOT_0) ?
											   m_EquipSlotsInfo.RightSlot : m_EquipSlotsInfo.LeftSlot;

  // ¿El nombre del slot cliqueado esta seleccionado?
  if (ClickedSlot.SlotName.IsSelect()) {
	// ¿Esta seleccionado el slot?
	if (ClickedSlot.Slot.IsSelect()) {
	  // Si, se deselecciona
	  ClickedSlot.Slot.Select(false);
	  
	  // Se baja el flag de arma seleccionada
	  m_EquipSlotsInfo.bIsWeaponSelect = false;	  
	} else {
	  // Se selecciona, deseleccionado el slot secundario
	  ClickedSlot.Slot.Select(true);
	  SecondSlot.Slot.Select(false);
	 
	  // Se sube el flag de arma seleccionada y se asocia slot
	  m_EquipSlotsInfo.bIsWeaponSelect = true;
	  m_EquipSlotsInfo.WeaponSlot = EquipSlot;	
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Responde a las notificaciones de los componentes embebidos en el 
//   interfaz.
// Parametros:
// - GUICEvent. Informacion sobre el componente que ha realizado la notificacion
//   al interfaz.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
{
  // SOLO si instancia inicialiada
  ASSERT(Inherited::IsInitOk());
  
  // Se comprueba el evento recibido
  switch (GUICEvent.Event) {
	case GUIDefs::GUIC_SELECT: {
	  // Seleccion
	  OnSelectNotify(GUICEvent.IDComponent, true);
	} break;

  	case GUIDefs::GUIC_UNSELECT: {
	  // No seleccion
	  OnSelectNotify(GUICEvent.IDComponent, false);
	} break;
	
	case GUIDefs::GUIC_BUTTONLEFT_PRESSED: {	  
	  // Pulsacion boton izquierdo
	  OnLeftClickNotify(GUICEvent.IDComponent);
	} break;	  

	default:
	  ASSERT_MSG(false, "No estoy preparado para recibir otro evento");
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion asociada a la seleccion / deseleccion de un componente.
// Parametros:
// - IDComponent. Identificador del componente.
// - bSelect. Flag de seleccion / deseleccion.
// Devuelve:
// Notas:
// - En el caso de los botones asociados al combate, estos solo seran 
//   seleccionables si el interfaz esta en modo turno y ademas el modo
//   turno lo posee el jugador.
// - En el caso de los slots con las armas equipadas, estas solo seran
//   usables si estan asociadas a ellas las manos (es decir, no hay ningun
//   item equipado) o bien un arma ofensiva. Seran armas ofensivas aquellas
//   cuyo coste de uso sea 0. Esto se reflejara cuando se reciba un evento
//   de seleccion. 
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::OnSelectNotify(const GUIDefs::GUIIDComponent& IDComponent,
								  const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se comprueba el componente
  switch(IDComponent) {
	case CGUIWMainInterfaz::ID_CONSOLE_SCROLL_UP: {
	  // Boton de scroll arriba para consola
	  m_ConsoleInfo.ButtonUp.Select(bSelect);
	} break;
	
	case CGUIWMainInterfaz::ID_CONSOLE_SCROLL_DOWN: {	 
	  // Boton de scroll abajo para consola
	  m_ConsoleInfo.ButtonDown.Select(bSelect);
	} break;

	case CGUIWMainInterfaz::ID_SLOT_LEFT: {
	  // Slot de equipamiento izquierdo	  	  
	  if (bSelect && CanUseWeaponInSlot(RulesDefs::EQUIPMENT_SLOT_0)) {
		m_EquipSlotsInfo.LeftSlot.SlotName.Select(bSelect);			  
	  } else if (!bSelect) {
		m_EquipSlotsInfo.LeftSlot.SlotName.Select(bSelect);
	  }
	} break;

	case CGUIWMainInterfaz::ID_SLOT_RIGHT: {		  	
	  // Slot de equipamiento dcho
	  if (bSelect && CanUseWeaponInSlot(RulesDefs::EQUIPMENT_SLOT_1)) {
		m_EquipSlotsInfo.RightSlot.SlotName.Select(bSelect);			  
	  } else if (!bSelect) {
		m_EquipSlotsInfo.RightSlot.SlotName.Select(bSelect);
	  }
	} break;
	
	case CGUIWMainInterfaz::ID_PORTRAIT: {		  	 
	  // Retrato
	  m_PortraitInfo.Portrait.Select(bSelect);
	} break;

	case CGUIWMainInterfaz::ID_NEXTTURNBUTTON: {
	  // Boton de sig. turno, se selecciona si procede (ver Notas)  
	  if (WorldDefs::TURN_MODE == m_InterfazInfo.Mode &&
		m_InterfazInfo.pCriatureInTurn == m_pWorld->GetPlayer()) {
		m_CombatOptionsInfo.NextTurn.Select(bSelect);
	  }	  
	} break;

	case CGUIWMainInterfaz::ID_FLEECOMBATBUTTON: {
	  // Boton de huir de combate, se selecciona si procede (ver Notas)  
	  if (WorldDefs::TURN_MODE == m_InterfazInfo.Mode &&
		  m_InterfazInfo.pCriatureInTurn == m_pWorld->GetPlayer()) {
		m_CombatOptionsInfo.FleeCombat.Select(bSelect);
	  }
	} break;
	
	case CGUIWMainInterfaz::ID_TIMECONTROLLER: {
	  // Controlador de tiempo
	  // En caso de seleccion, se le asociara un valor alpha solido y
	  // en caso contrario, el valor alpha establecido en la configuracion
	  if (bSelect) {
		m_TimeController.ActButton.SetAlpha(GraphDefs::sAlphaValue(255));		
	  } else {
		m_TimeController.ActButton.SetAlpha(m_TimeController.NoSelectAlpha);
	  }
	  m_TimeController.ActButton.Select(bSelect);
	} break;
  }; // ~ switch  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la pulsacion del boton izquierdo sobre un componente.
// Parametros:
// - IDComponent. Identificador del componente que ha recibido la notificacion.
// Devuelve:
// Notas:
// - En el caso de los botones de combate, solo se ejecutara la logica de
//   cliqueo si el boton ha sido seleccionado previamente, por lo que no 
//   hara falta realizar la comprobacion de que el interfaz este en modo
//   turno y que ademas el jugador posea el turno.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::OnLeftClickNotify(const GUIDefs::GUIIDComponent& IDComponent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se comprueba el componente
  switch(IDComponent) {
	case CGUIWMainInterfaz::ID_CONSOLE_SCROLL_UP: {
	  // Boton de scroll arriba para consola
	  // Se sube el texto
	  m_ConsoleInfo.Text.MoveUp();
	} break;
	
	case CGUIWMainInterfaz::ID_CONSOLE_SCROLL_DOWN: {	 
	  // Boton de scroll abajo para consola
	  // Se baja el texto
	  m_ConsoleInfo.Text.MoveDown();
	} break;

	case CGUIWMainInterfaz::ID_SLOT_LEFT: {
	  // Slot de equipamiento izquierdo pulsado
	  ClickOnEquipmentSlot(RulesDefs::EQUIPMENT_SLOT_0);	  
	} break;

	case CGUIWMainInterfaz::ID_SLOT_RIGHT: {		  	
	  // Slot de equipamiento dcho pulsado
	  ClickOnEquipmentSlot(RulesDefs::EQUIPMENT_SLOT_1);	  
	} break;
	
	case CGUIWMainInterfaz::ID_PORTRAIT: {		  	 
	  // Retrato
	  // Se pausa el universo de juego
	  // Antes se detendra incondicionalmente al jugador, pues en caso de
	  // no hacerlo, se mantendra una orden y no se podran dar desde
	  // el inventario
	  m_pWorld->GetPlayer()->StopWalk(true);
	  m_pWorld->SetPause(true);  

	  // Activara el interfaz de hoja de personaje, antes se deseleccionara
	  OnSelectNotify(IDComponent, false);
	  SYSEngine::GetGUIManager()->SetPlayerProfileWindow();	  
	} break;

	case CGUIWMainInterfaz::ID_NEXTTURNBUTTON: {
	  // Boton de sig. turno	  
	  SYSEngine::GetCombatSystem()->NextTurn(m_InterfazInfo.pCriatureInTurn->GetHandle());
	} break;

	case CGUIWMainInterfaz::ID_FLEECOMBATBUTTON: {
	  // Boton de huir de combate, se genera el evento relaccionado. 
	  // El comportamiento por defecto sera finalizar el combate
	  m_pVMachine->OnFleeCombatEvent(this, 
								     m_pWorld->GetScriptEventFile(RulesDefs::SE_ONFLEECOMBAT));	  
	} break;	

	case CGUIWMainInterfaz::ID_TIMECONTROLLER: {
	  // Boton controlador de tiempo
	  // Se genera el evento relacionado con la pulsacion sobre el panel	  
	  m_pVMachine->OnClickHourPanelEvent(this, 
										 m_pWorld->GetScriptEventFile(RulesDefs::SE_ONCLICKHOURPANEL));										 
	} break;
  }; // ~ switch  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Atendera los eventos generales de entrada que caigan sobre la barra
//   de interfaz. Estos seran relativos al movimiento del cursor, para poder
//   realizar el scroll sobre el motor isometrico y cambiar el cursor al que
//   proceda.
// - Tambien se encargara de comprobar si se ha pulsado a ESC y activar el 
//   menu de juego.
// - El resto de eventos que se recogeran seran para comunicarlos via script.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::InstallClient(void)
{
  // Se establecen eventos de entrada
  // Eventos utilizados por la interfaz
  iCInputManager* const pInputManager = SYSEngine::GetInputManager();
  ASSERT(pInputManager);
  pInputManager->SetInputEvent(InputDefs::IE_MOVEMENT_MOUSE, 
							   this, 
							   InputDefs::MOVEMENT_MOVED);
  pInputManager->SetInputEvent(InputDefs::IE_MOVEMENT_MOUSE, 
							   this, 
							   InputDefs::MOVEMENT_UNMOVED);
  pInputManager->SetInputEvent(InputDefs::IE_BUTTON_MOUSE_LEFT,
						       this,
							   InputDefs::BUTTON_PRESSED_DOWN);
  pInputManager->SetInputEvent(InputDefs::IE_BUTTON_MOUSE_RIGHT,
						       this,
							   InputDefs::BUTTON_PRESSED_DOWN);
  pInputManager->SetInputEvent(InputDefs::IE_KEY_ESC, 
							   this, 
							   InputDefs::BUTTON_PRESSED_DOWN);
  
  // Eventos a propagar por el script
  // Iran justo despues de la tecla ESC
  sword swIDKeyEvent = InputDefs::IE_KEY_F1;
  for (; swIDKeyEvent != InputDefs::IE_KEY_LEFT; ++swIDKeyEvent) {
	// Se establece
	pInputManager->SetInputEvent(InputDefs::eInputEventCode(swIDKeyEvent),
							     this, 
							     InputDefs::BUTTON_PRESSED_DOWN);  
	pInputManager->SetInputEvent(InputDefs::eInputEventCode(swIDKeyEvent),
							     this, 
							     InputDefs::BUTTON_RELEASED);  
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desinstala los eventos de entrada.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::UnistallClient(void)
{ 
  // Se quitan eventos de entrada
  // Eventos utilizados por la interfaz
  iCInputManager* const pInputManager = SYSEngine::GetInputManager();
  ASSERT(pInputManager);  
  pInputManager->UnsetInputEvent(InputDefs::IE_MOVEMENT_MOUSE, 
								 this, 
								 InputDefs::MOVEMENT_MOVED);
  pInputManager->UnsetInputEvent(InputDefs::IE_MOVEMENT_MOUSE, 
								 this, 
								 InputDefs::MOVEMENT_UNMOVED);
  pInputManager->UnsetInputEvent(InputDefs::IE_KEY_ESC, 
								 this,
								 InputDefs::BUTTON_PRESSED_DOWN);
  pInputManager->UnsetInputEvent(InputDefs::IE_BUTTON_MOUSE_LEFT, 
								 this,
								 InputDefs::BUTTON_PRESSED_DOWN);
  pInputManager->UnsetInputEvent(InputDefs::IE_BUTTON_MOUSE_RIGHT, 
								 this,
								 InputDefs::BUTTON_PRESSED_DOWN);  

  // Eventos a propagar por el script
  // Iran justo despues de la tecla ESC
  sword swIDKeyEvent = InputDefs::IE_KEY_F1;
  for (; swIDKeyEvent != InputDefs::IE_KEY_LEFT; ++swIDKeyEvent) {
	// Se quita
	pInputManager->UnsetInputEvent(InputDefs::eInputEventCode(swIDKeyEvent), 
								   this, 
								   InputDefs::BUTTON_PRESSED_DOWN);  
	pInputManager->UnsetInputEvent(InputDefs::eInputEventCode(swIDKeyEvent), 
								   this, 
								   InputDefs::BUTTON_RELEASED);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Despacha el evento de entrada referido al movimiento del cusor, 
//   comprobando si se esta en la zona de interfaz y tiene que cambiar el cursor
//   o bien si esta en la zona de scroll y ha de ordenar al isometrico que
//   realice un scroll.
// Parametros:
// - aInputEvent. Informacion sobre el evento de entrada.
// Devuelve:
// - Flag para indicar si el evento debe de compartirse o no.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWMainInterfaz::DispatchEvent(const InputDefs::sInputEvent& aInputEvent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se procede a trabajar con el interfaz solo si este NO esta desactivado
  if (CGUIWMainInterfaz::DESACTIVATED != m_InterfazInfo.State) {  
	// Se establece flag de compartir evento
	// El evento NO se comparte siempre que, antes de ejecutar toda la logica, 
	// se estuviera en ventana de interfaz y a la vuelta en area de juego
	const bool bShareEvent = (CGUIWMainInterfaz::WINDOW_INTERACT == m_InterfazInfo.State) ? 
							 InputDefs::SHARE_INPUT_EVENT : InputDefs::NO_SHARE_INPUT_EVENT; 

	// Evalua el evento producido 
	switch(aInputEvent.EventType) {
	  case InputDefs::BUTTON_EVENT: {
		// Pulsacion de tecla
		switch(aInputEvent.EventButton.Code) {
		  // Eventos de pulsacion asociados a la interfaz		  
		  case InputDefs::IE_BUTTON_MOUSE_LEFT: {
			// Boton izquierdo del raton
			OnLeftMouseClick();
		  } break;

		  case InputDefs::IE_BUTTON_MOUSE_RIGHT: {
			// Boton derecho del raton
			OnRightMouseClick();
		  } break;

  		  case InputDefs::IE_KEY_ESC: {		  
			// Tecla ESC
			OnKeyEscPressed();			
		  } break;

		  // Eventos de pulsacion a comunicar via script
		  default: {
			// ¿Pulsacion?
			if (aInputEvent.EventButton.State == InputDefs::BUTTON_PRESSED_DOWN) {
			  // Si, se registra codigo y se propaga evento
			  m_KeyPressedSet.insert(aInputEvent.EventButton.Code);
			  m_pVMachine->OnKeyPressedEvent(this, 
											 m_pWorld->GetScriptEventFile(RulesDefs::SE_ONKEYPRESSED));
			} else {
			  // No, se libera codigo
			  m_KeyPressedSet.erase(aInputEvent.EventButton.Code);
			}			
		  } break;
		}; // ~ switch
	  } break;
	  
	  case InputDefs::MOVEMENT_EVENT:	{
		// Evento de movimiento del raton.
		// Se evaluan subeventos
		switch(aInputEvent.EventMovements.State) {		
		  case InputDefs::MOVEMENT_MOVED: {		  
			// Movimiento del raton
			// Se almacenan posiciones
			m_InterfazInfo.Pos.swXPos = aInputEvent.EventMovements.udXAct;
			m_InterfazInfo.Pos.swYPos = aInputEvent.EventMovements.udYAct;
	
			// Se realizan las acciones pertinentes  
			OnMouseMoved();		  		  
		  } break;

		   case InputDefs::MOVEMENT_UNMOVED: {			 
			 OnMouseUnmove();
		  } break;
		}; // ~ switch

	  } break;	 
	}; // ~ switch    

	// Se retorna flag de comparticion
	return bShareEvent;
  }

  // En caso de que la ventana este desactivada, el evento se compartira siempre
  return InputDefs::SHARE_INPUT_EVENT;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada al movimiento del raton. Se encargara de coordinar los
//   metodos asociados al estado de interfaz actual.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void
CGUIWMainInterfaz::OnMouseMoved(void)
{
  // Se establece el flag indicativo de si hay cursor de scroll
  const bool bWasScrollCursor = (m_ScrollInfo.ScrollCursor != GUIManagerDefs::NO_CURSOR);

  // ¿El cursor se ha colocado en zona de scroll de mapa?
  if (CheckForScroll(m_InterfazInfo.Pos.swXPos, m_InterfazInfo.Pos.swYPos)) { 
	// Si, cursor de scroll establecido y todo realizado
	return; 
  }

  // ¿Esta el cursor en el area de la ventana de interfaz? o
  // ¿Esta el cursor en el area de la ventana de controlador de tiempo?
  // Nota: Para considerar que el cursor se halle en la ventana controladora
  // de tiempo (para permitir la seleccion de esta) debera de existir un
  // script asociado, en caso contrario no sera tendio en cuenta
  iCMathUtil* const pMathUtil = SYSEngine::GetMathUtil();
  ASSERT(pMathUtil);
  const sPosition MousePos(m_InterfazInfo.Pos);  
  if (pMathUtil->IsPointInRect(MousePos, m_InterfazBarArea) ||
	  (pMathUtil->IsPointInRect(MousePos, m_TimeControllerArea) && 
	   !m_pWorld->GetScriptEventFile(RulesDefs::SE_ONCLICKHOURPANEL).empty() &&
	   m_TimeController.ActButton.IsActive())) {
	// ¿NO esta configurado como estado actual la interaccion con ventana? o
	// ¿Estaba activo el estado de scroll?
	if (CGUIWMainInterfaz::WINDOW_INTERACT != m_InterfazInfo.State ||
	    bWasScrollCursor) {	  
	  // Se establece cursor asociado a estado	  
	  m_pGUIManager->SetGUICursor(GUIManagerDefs::WINDOWINTERFAZ_CURSOR);	
	  
	  // Si NO se venia de scroll, se completan el resto de accioenes
	  if (!bWasScrollCursor) {
		// Se establece estado de interfaz
		SetInterfazState(CGUIWMainInterfaz::WINDOW_INTERACT);	  
		
		// Se desactivan elementos del interfaz anterior, segun proceda
		if (CGUIWMainInterfaz::MOVING_PLAYER == m_InterfazInfo.PrevState) {
		  // Se desactiva dibujado de celda de seleccion
		  m_pWorld->SetDrawSelectCell(false);
		} else {
		  // Se desactiva posible entidad seleccionada con fade
		  ReleaseEntityInteractInfo();
		}
	  }  
	}	
  } else {	
	// Cursor en area isometrica
	// ¿El estado era de interaccion con ventana principal?
	if (CGUIWMainInterfaz::WINDOW_INTERACT == m_InterfazInfo.State) {
	  // Se establece el estado previo, que seguro sera de movimiento o interaccion
	  SetInterfazState(m_InterfazInfo.PrevState);
	}
  }
  
  // Se ejecuta la logica que proceda
  switch(m_InterfazInfo.State) {
	case CGUIWMainInterfaz::INIT: {
	  // Estado de inicializacion, que se activara cuando el cursor caiga
	  // en el area isometrica. Se tomara el estado de movimiento por defecto
	  SetInterfazState(CGUIWMainInterfaz::MOVING_PLAYER);
	  
	  // Logica asociada
	  OnPlayerMovementSelectCell();	  
	} break;

	case CGUIWMainInterfaz::MOVING_PLAYER: {
	  // Seleccion de una celda para el movimiento
	  // Logica asociada	  
	  OnPlayerMovementSelectCell();
	} break;

	case CGUIWMainInterfaz::ENTITY_INTERACT: {
	  // Seleccion de una entidad para interaccion
	  // Logica asociada	  
	  OnEntityInteractSelectEntity();
	} break;

	case CGUIWMainInterfaz::WINDOW_INTERACT: {
	  // Interaccion con ventana principal o controlador de tiempo
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada al NO movimiento del raton. Se utilizara para ajustar
//   valores de scroll.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::OnMouseUnmove(void)
{
  
  // Forma de mantener la continuidad del scroll.
  // Lo ideal seria instalar esta interfaz como cliente del
  // comando de notificacion de movimiento del motor isometrico.
  // Cuando acabara, bastaria con comprobar el estado de scroll
  // y actuar en consecuencia. Esto obligaria a sacar del isometrico
  // el comando.
  if (m_ScrollInfo.ScrollCursor != GUIManagerDefs::NO_CURSOR) {
	CheckForScroll(m_InterfazInfo.Pos.swXPos, m_InterfazInfo.Pos.swYPos);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina el evento relacionado con la pulsacion del boton izquierdo
//   del raton.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::OnLeftMouseClick(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Comprueba el estado actual de interfaz y actua en consecuencia
  switch(m_InterfazInfo.State) {
	case CGUIWMainInterfaz::MOVING_PLAYER: {
	  // Moviendo al jugador
	  // ¿Celda actualmente seleccionada es IGUAL a donde esta el jugador? y
	  // ¿El jugador NO esta moviendose?	  
	  if (m_pWorld->GetSelectCellPos() == m_pWorld->GetPlayer()->GetTilePos() &&
		  !m_pWorld->GetPlayer()->IsWalking()) {
		// Se gira al jugador
		m_pWorld->GetPlayer()->GetAnimTemplate()->NextOrientation();
	  } else {
	   	// Se intenta mover al jugador
		// Nota: Se cumplira que en caso de no estar en combate,
		// movimientos previos seran cancelados y sustituidos por los nuevos
		OnPlayerMovementDoMovement();
	  }
	} break;

	case CGUIWMainInterfaz::ENTITY_INTERACT: {
	  // Interactuando con entidad seleccionada si procede
  	  OnEntityInteractDoInteract();
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la pulsacion del boton derecho del raton.
// Parametros:
// Devuelve:
// Notas:
// - Pulsar el boton derecho en estado de movimiento de personaje / 
//   interaccion con una entidad, supondra cambiar a intearccion con una
//   entidad / movimiento de personaje respectivamente y de forma circular.
//   En caso de estar en estado de interaccion usando un objeto del slot
//   principal, este slot desaparecera y se habilitara el estado asociado a
//   la posicion actual del cursor.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::OnRightMouseClick(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Comprueba el estado actual de interfaz
  // Iremos siempre al estado OPUESTO al estado actual de interfaz
  switch(m_InterfazInfo.State) {
	case CGUIWMainInterfaz::MOVING_PLAYER: {
	  // Se desactiva seleccion de celdas y se establece estado de interaccion
	  m_pWorld->SetDrawSelectCell(false);
   	  SetInterfazState(CGUIWMainInterfaz::ENTITY_INTERACT);
	  
	  // Si el jugador se esta moviendo, se detendra
	  m_pWorld->GetPlayer()->StopWalk();
	  
	  // Se simula evento de seleccion
	  OnEntityInteractSelectEntity();
	} break;

	case CGUIWMainInterfaz::ENTITY_INTERACT: {
	  // Si el jugador se encuentra en movimiento, significara que esta
	  // realizando una orden. En tal caso, simplemente se detendra anulando
	  // la accion que estuviera haciendo
	  if (m_pWorld->GetPlayer()->IsWalking()) {
		m_pWorld->GetPlayer()->StopWalk();		
	  } else {
		// No esta acatando ninguna opcion, luego se pasa a estado de mov.
		// Si se estuviera en estado de interaccion con una entidad seleccionada
		// se deseleccionaria y ademas se desvincularia el posible texto flotante
		ReleaseEntityInteractInfo();
		
		// Se activa seleccion de celdas y estado
		SetInterfazState(CGUIWMainInterfaz::MOVING_PLAYER);		
		
		// Se simula evento de movimiento
		OnPlayerMovementSelectCell();
	  }		  
	} break;
  }; // ~ switch  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la seleccion de una celda cuando el interfaz se halla en
//   estado de movimiento.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::OnPlayerMovementSelectCell(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se ordena seleccionar y se resuelve resultado
  switch(m_pWorld->SelectCell(m_InterfazInfo.Pos.swXPos, 
							  m_InterfazInfo.Pos.swYPos)) {
	case IsoDefs::SCT_NOVALIDCELL: {
	  // Celda no valida
	  // Se establece cursor y se elimina seleccion de tile
	  m_pGUIManager->SetGUICursor(GUIManagerDefs::NOPOSSIBLE_MOVEMENT_CURSOR);
	  m_pWorld->SetDrawSelectCell(false);
	} break;

	case IsoDefs::SCT_NOACCESIBLECELL: {
	  // Celda valida pero NO accesible
	  // Se establece cursor y selector
	  m_pGUIManager->SetGUICursor(GUIManagerDefs::NOPOSSIBLE_MOVEMENT_CURSOR);
	  m_pWorld->SetDrawSelectCell(true);
	} break;

	case IsoDefs::SCT_ACCESIBLECELL: {
	  // Celda valida y accesible
	  // Se establece cursor y selector
	  m_pGUIManager->SetGUICursor(GUIManagerDefs::MOVEMENT_CURSOR);
	  m_pWorld->SetDrawSelectCell(true);
	} break;
  }; // ~ switch  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la pulsacion del boton izquierdo del raton para mover
//   al jugador a la celda previamente seleccionada.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::OnPlayerMovementDoMovement(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si estado apropiado
  ASSERT((m_InterfazInfo.State == CGUIWMainInterfaz::MOVING_PLAYER) != 0);

  // ¿Activo el cursor de movimiento?
  if (m_pGUIManager->GetActGUICursor() == GUIManagerDefs::MOVEMENT_CURSOR) {	
	// Si, ¿Se esta en combate?
	if (SYSEngine::GetCombatSystem()->IsCombatActive()) {
	  // Si, ¿el jugador se halla en movimiento?
	  if (m_pWorld->GetPlayer()->IsWalking()) {
		// Si, entonces la accion sera la detencion inmediata
		m_pWorld->GetPlayer()->StopWalk();
	  } else {
		// No, se ordenara que cambie de direccion dinamicamente
		m_pWorld->MovPlayerToSelectCell();
	  }
	} else {
	  // No, se mueve directamente
	  m_pWorld->MovPlayerToSelectCell();
	}	
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encargara de comprobar si en la ultima posicion obtenida de un 
//   evento de movimiento, hay alguna entidad que sea sensible de ser
//   seleccionada para realizar una operacion de interaccion. 
//   En dicho caso, almacenara la instancia a dicha entidad.
// - Cada vez que se seleccione una entidad, se mostrara su nombre utilizando
//   el texto flotante pero de tal forma que no sea con efecto de disolucion,
//   ya que no se pretende llamar la atencion de forma especial, solo indicar
//   una informacion.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::OnEntityInteractSelectEntity(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si estado apropiado
  ASSERT((m_InterfazInfo.State == CGUIWMainInterfaz::ENTITY_INTERACT) != 0);

  // Obtiene posible entidad con la que interactuar
  CWorldEntity* const pEntityInteract = m_pWorld->GetEntityAt(m_InterfazInfo.Pos.swXPos,
														      m_InterfazInfo.Pos.swYPos);
  
  // ¿Entidad es distinta a la entidad previa y la entidad previa es valida?
  bool bPost = true;
  if (m_InterfazInfo.pEntityInteract != pEntityInteract) {
	// Se desactiva cualquier info de interaccion si procede
	if (m_InterfazInfo.pEntityInteract) {
	  // Se indica que no se ponga en la cola de comandos porque ya se encuentra
	  ReleaseEntityInteractInfo();	  	  
	  bPost = false;
	}
  }      
  
  // ¿Hay entidad a la que seleccionar?
  if (pEntityInteract) {	
	// ¿Se esta en modo de interaccion para ataque?
	if (m_EquipSlotsInfo.bIsWeaponSelect) {	   
	  // ¿La entidad seleccionada es un item?
	  // ¿el jugador? o
	  // ¿Un NPC muerto (salud temporal de 0)? o
	  // ¿NO tiene evento OnHit asociado?
	  const EntityType = pEntityInteract->GetEntityType();
	  if (EntityType == RulesDefs::ITEM ||
		  EntityType == RulesDefs::PLAYER ||
		  (EntityType == RulesDefs::CRIATURE &&
		   m_pWorld->GetCriature(pEntityInteract->GetHandle())->GetHealth(RulesDefs::TEMP_VALUE) < 1) ||
		   pEntityInteract->GetScriptEvent(RulesDefs::SE_ONHIT) == "") {
		// Se abandona ya que no se permite atacar a items ni al jugador
		return;
	  }
	}	

    // Se establece el cursor se procede	
    if (m_pGUIManager->GetActGUICursor() != GetEntityInteractCursor(true)) {
  	  m_pGUIManager->SetGUICursor(GetEntityInteractCursor(true));
	}
	
	// Se activa fade de seleccion si procede
	if (pEntityInteract != m_InterfazInfo.pEntityInteract) {
	  // Si la entidad es una criatura, se instalara el interfaz como
	  // observador para que si esta muere y esta seleccionada, se 
	  // desvincule cualquer tipo de informacion.
	  if (RulesDefs::CRIATURE == pEntityInteract->GetEntityType()) {
		// Es una criatura, se instala como observador
		CCriature* const pCriature = m_pWorld->GetCriature(pEntityInteract->GetHandle());
		ASSERT(pCriature);
		pCriature->AddObserver(this);
	  }

	  // Se guarda entidad, posicion y se activa el fade de seleccion
	  m_InterfazInfo.pEntityInteract = pEntityInteract;
	  m_InterfazInfo.EntityInteractPos = pEntityInteract->GetTilePos();
	  m_InterfazInfo.pEntityInteract->SetFadeSelect(true);	  
	  
	  // Se establece texto flotante
	  // ¿Hay seleccionada un arma?	y
	  // ¿La entidad seleccionada es una criatura?
	  std::string szFloatingText;	
	  if (m_EquipSlotsInfo.bIsWeaponSelect &&
		  m_InterfazInfo.pEntityInteract->GetEntityType() == RulesDefs::CRIATURE) {
		// Si, Obtiene el texto informativo
		szFloatingText = GetAtackInfoSelect(m_pWorld->GetCriature(m_InterfazInfo.pEntityInteract->GetHandle()));	
	  } else {
		// No, se pone simplemente el nombre de la entidad
		szFloatingText = m_InterfazInfo.pEntityInteract->GetName();
	  }
	  
	  // Finalmente se asocia el texto flotante
	  m_pGUIManager->SetFloatingText(szFloatingText,
									 m_FloatingTextInfo.RGBColor,
									 m_FloatingTextInfo.Alpha,
									 m_FloatingTextInfo.Type);
	}		
  } else {
    // No hay entidad a quien seleccionar
	// Se establece el cursor si procede
    if (m_pGUIManager->GetActGUICursor() != GetEntityInteractCursor(false)) {
	  m_pGUIManager->SetGUICursor(GetEntityInteractCursor(false));	  
	}	
  }    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la informacion relativa a una criatura seleccionada para ser
//   golpeada.
// Parametros:
// - pCriature. Instancia a la criatura.
// Devuelve:
// - El texto a asociar
// Notas:
//////////////////////////////////////////////////////////////////////////////
std::string
CGUIWMainInterfaz::GetAtackInfoSelect(CCriature* const pCriature)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(pCriature);

  // Nombre
  std::string szFloatingText(pCriature->GetName() + "\n");

  // Tipo y clase
  szFloatingText += m_pGDBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_TYPE,
																  pCriature->GetType()) + " \\ ";		
  szFloatingText += m_pGDBase->GetRulesDataBase()->GetEntityIDStr(RulesDefs::ID_CRIATURE_CLASS,
																  pCriature->GetType(),
																  pCriature->GetClass()) + "\n";		
  // Nivel
  std::string szTmp;
  SYSEngine::PassToString(szTmp, ": %u\n", pCriature->GetLevel());
  szFloatingText += m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_LEVEL) + szTmp;

  // Salud
  SYSEngine::PassToString(szTmp, 
						  ": %d \\ %d", 
						  pCriature->GetHealth(RulesDefs::TEMP_VALUE),
						  pCriature->GetHealth(RulesDefs::BASE_VALUE));
  szFloatingText += m_pGDBase->GetStaticText(GameDataBaseDefs::ST_CRIATURE_HEALTH) + 
				    szTmp;

  // Retorna
  return szFloatingText;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desvincula culaquier informacion de interaccion.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::ReleaseEntityInteractInfo(void) 
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Hay entidad asociada?
  if (m_InterfazInfo.pEntityInteract) {
	// Si, se desvincula informacion
	// ¿Es una criatura?
	if (RulesDefs::CRIATURE == m_InterfazInfo.pEntityInteract->GetEntityType()) {
	  // Si, se desvincula la interfaz como observadora
	  CCriature* const pCriature = m_pWorld->GetCriature(m_InterfazInfo.pEntityInteract->GetHandle());
	  ASSERT(pCriature);
	  pCriature->RemoveObserver(this);
	}	

	// Se desvincula info de seleccion
	m_InterfazInfo.pEntityInteract->SetFadeSelect(false);
	m_InterfazInfo.pEntityInteract = NULL;
	m_pGUIManager->UnsetFloatingText();
  }	 	
}  

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la activacion de la interaccion por parte del jugador
//   con la entidad seleccionada sobre el universo de juego. 
// - En caso de que se este con un arma seleccionada, la accion de
//   interactuar ser transformara en una llamada al metodo Hit para indicar
//   a la criatura que golpee a una entidad destino. En caso de que no haya
//   ningun arma seleccionada, se entendera que se querra realizar una
//   interaccion normal con la entidad destino.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::OnEntityInteractDoInteract(void)
{
  // SOLO si instancia valida
  ASSERT(Inherited::IsInitOk());

  // ¿Hay una entidad seleccionada?
  if (m_InterfazInfo.pEntityInteract) {
	// Se detiene al jugador de la realizacion de cualquier accion
	m_pWorld->GetPlayer()->StopWalk();

	// Establece las dimensiones del area en donde se visualizara el interfaz
	const sRect rViewWindow(m_pWorld->GetVisibleWorldArea());

	// ¿Hay arma seleccionada?
	if (m_EquipSlotsInfo.bIsWeaponSelect) {
	  // Si, luego se realizara una accion de golpear
	  ASSERT(m_InterfazInfo.pEntityInteract);
	  m_pWorld->GetPlayer()->Hit(m_InterfazInfo.pEntityInteract->GetHandle(),
								 m_EquipSlotsInfo.WeaponSlot);
	} else {
	  // Se desactiva info de la interfaz de seleccion
	  CWorldEntity* const pEntity = m_InterfazInfo.pEntityInteract;
	  ASSERT(pEntity);		  
	  ReleaseEntityInteractInfo();

	  // No, se activara el interfaz de interaccion
	  // Nota: El texto flotante que pudiera estar asociado sera deseleccionado
	  // desde el interfaz de interaccion con entidades
	  const sPosition InitDrawPos(m_InterfazInfo.Pos);
	  m_pGUIManager->SetEntityInteractWindow(InitDrawPos,
											 m_pWorld->GetPlayer(),
										     pEntity);
	}	
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la pulsacion de la tecla ESC.
//   * Activara el menu de juego
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::OnKeyEscPressed(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se desvincula informacion de interaccion
  ReleaseEntityInteractInfo();

  // Se desactiva el dibujado de la celda de seleccion
  m_pWorld->SetDrawSelectCell(false);

  // Se activa el interfaz de menu de juego
  SYSEngine::GetGUIManager()->SetGameMenuWindow();

  // Se pausa el universo de juego
  m_pWorld->SetPause(true);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada al cambio de estado de desplazamiento en el jugador. El
//   cambio sera circular, es decir, en el caso de que se este andando y se
//   pulse, se pasara a estado de correr y viceversa.
// Parametros:
// Devuelve:
// Notas:
// - Este metodo no debera de llamarse en modo combate, pues en modo combate
//   el estado siempre sera el de andar. Se dejara la responsabilidad al
//   exterior.
//////////////////////////////////////////////////////////////////////////////
void
CGUIWMainInterfaz::OnKeyWalkModePressed(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se establecera el estado contrario al actual
  m_pWorld->GetPlayer()->SetRunMode(!m_pWorld->GetPlayer()->IsInRunMode());
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si la posicion en uwXPos, uwYPos puede producir algun tipo de
//   scroll, en cuyo caso se cambiara el icono del cursor asociado al scroll
//   (si procede) y se efectuara, devuelviendo true al exterior.
// Parametros:
// - uwXPos, uwYPos. Posicion en X,Y donde se halla el cursor.
// Devuelve:
// - Si se ha producido scroll true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWMainInterfaz::CheckForScroll(const word uwXPos, 
								  const word uwYPos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se comprueba si hay scroll y que tipo
  GUIManagerDefs::eGUICursorType NewCursorType;
  bool bResult = true;
  if (0 == uwXPos && 0 == uwYPos) {
	// Noroeste	
	NewCursorType = GUIManagerDefs::SCROLL_NORTHWEST_CURSOR;
	m_pWorld->MoveTo(IsoDefs::NORTHWEST_INDEX);
  } else if (m_ScrollInfo.uwRightDiagCursorTop <= uwXPos && 
			 0 == uwYPos) {
	// Noreste
	NewCursorType = GUIManagerDefs::SCROLL_NORTHEAST_CURSOR;	
	m_pWorld->MoveTo(IsoDefs::NORTHEAST_INDEX);
  } else if (0 == uwXPos && 
			 m_ScrollInfo.uwBottomDiagCursorTop <= uwYPos) {
	// Suroeste
	NewCursorType = GUIManagerDefs::SCROLL_SOUTHWEST_CURSOR;	
	m_pWorld->MoveTo(IsoDefs::SOUTHWEST_INDEX);
  } else if (m_ScrollInfo.uwRightDiagCursorTop <= uwXPos && 
			 m_ScrollInfo.uwBottomDiagCursorTop <= uwYPos) {
	// Sureste
	NewCursorType = GUIManagerDefs::SCROLL_SOUTHEAST_CURSOR;
	m_pWorld->MoveTo(IsoDefs::SOUTHEAST_INDEX);
  } else if (0 == uwYPos) {
	// Norte
	NewCursorType = GUIManagerDefs::SCROLL_NORTH_CURSOR;		
	m_pWorld->MoveTo(IsoDefs::NORTH_INDEX);
  } else if (m_ScrollInfo.uwBottomCursorTop == uwYPos) {
	// Sur
	NewCursorType = GUIManagerDefs::SCROLL_SOUTH_CURSOR;
	m_pWorld->MoveTo(IsoDefs::SOUTH_INDEX);
  } else if (0 == uwXPos) {
	// Oeste
	NewCursorType = GUIManagerDefs::SCROLL_WEST_CURSOR;	
	m_pWorld->MoveTo(IsoDefs::WEST_INDEX);
  } else if (m_ScrollInfo.uwRightCursorTop == uwXPos) {
	// Este
	NewCursorType = GUIManagerDefs::SCROLL_EAST_CURSOR;			
	m_pWorld->MoveTo(IsoDefs::EAST_INDEX);
  } else {
	// No hay scroll
	bResult = false;	
  }
  
  // Se cambia y guarda el cursor de scroll si procede
  if (bResult && 
	  NewCursorType != m_pGUIManager->GetActGUICursor()) {
	// Se establece la existencia de cursor
	m_pGUIManager->SetGUICursor(NewCursorType);
	m_ScrollInfo.ScrollCursor = NewCursorType;

	// Se baja flag de dibujado en celda de seleccion, si procede	
	if (CGUIWMainInterfaz::MOVING_PLAYER == m_InterfazInfo.State) {
	  m_pWorld->SetDrawSelectCell(false);
	}	
  } else if (!bResult) {
	// Se establece la no existencia de cursor de scroll
	m_ScrollInfo.ScrollCursor = GUIManagerDefs::NO_CURSOR;
  }

  // Se retorna true si hay scroll
  return bResult;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de filtrar la carga desde dentro del juego. Para que se
//   acepte la carga, el interfaz ha de estar activo y seleccionado como
//   interfaz actual.
// Parametros:
// - uwIDArea. Identificador del area.
// - PlayerPos. Posicion del jugador en el area.
// Devuelve:
// - Si se va a poder cargar true. En caso contrario false.
// Notas:
// - El proceso de carga supondra, si es aceptado, hacer un FadeOut indicando
//   al mismo tiempo que se esta cargando un area. Tambien se bloqueara la
//   entrada. Una vez realizado el FadeOut, se cargara el area y se hara
//   un FadeIn, desbloqueandose el interfaz.
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWMainInterfaz::InGameAreaLoading(const word uwIDArea,
									 const AreaDefs::sTilePos& PlayerPos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Esta el interfaz activo? y
  // ¿Es el interfaz actual?
  if (Inherited::IsActive() &&
	  m_pGUIManager->GetGameGUIWindowState() == GUIManagerDefs::GUIW_MAININTERFAZ) {
	// Si, se bloquea el interfaz	
	SetInput(false);

	// Se guardan valores
	m_InGameAreaLoading.uwIDArea = uwIDArea;
	m_InGameAreaLoading.PlayerPos = PlayerPos;
	m_InGameAreaLoading.bIsLoading = true;

	// Se establece el mensaje de carga de area
	m_InGameAreaLoading.szAreaLoadingMsg = m_pGDBase->GetStaticText(GameDataBaseDefs::ST_GENERAL_AREA_LOADING);
	
	// Se ordena que se detenga al jugador si este estaba en mov.
	m_pWorld->GetPlayer()->StopWalk();
	
	// Se realiza un FadeOut y se retorna
	Inherited::DoFadeOut(CGUIWMainInterfaz::ID_ENDAREALOADING_FADEOUT,
						 GraphDefs::sRGBColor(0, 0, 0),
						 196,
						 GraphDefs::DZ_GUI,
						 1);
	return true;
  }
  
  // No se pudo realizar
  return false;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado de todos los componentes. En caso de que la ventana
//   de interfaz no este activa, no se realizara el dibujado.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se dibuja si procede
  if (Inherited::IsActive()) {	  
	// Imagen de fondo	
	m_BackImgInfo.Background.Draw();
  
	// Consola
	m_ConsoleInfo.FXBack.Draw();
	m_ConsoleInfo.Text.Draw();  
	m_ConsoleInfo.ButtonUp.Draw();
	m_ConsoleInfo.ButtonDown.Draw();
  
	// Cara
	m_PortraitInfo.Portrait.Draw();  
  
	// Slots
	m_EquipSlotsInfo.LeftSlot.Slot.Draw();
	m_EquipSlotsInfo.RightSlot.Slot.Draw();
	m_EquipSlotsInfo.LeftSlot.SlotName.Draw();
	m_EquipSlotsInfo.RightSlot.SlotName.Draw();
  
	// Atributos
	byte ubIt = 0;
	for (; ubIt < CGUIWMainInterfaz::sAttributesInfo::MAX_ATTRIBUTES_TO_SHOW; ++ubIt) {		
	  // ¿Se desea mostrar atributo?	
	  if (!m_AttributesInfo.Attribs[ubIt].ubID) { continue; }	  
	  
	  // Nombre / Porcentaje / Info numerica
	  m_AttributesInfo.Attribs[ubIt].Name.Draw();		
	  m_AttributesInfo.Attribs[ubIt].Percentage.Draw();		
	  m_AttributesInfo.Attribs[ubIt].Numeric.Draw();
	}

	// Opciones de combate
	m_CombatOptionsInfo.NextTurn.Draw();
	m_CombatOptionsInfo.FleeCombat.Draw();

	// Sintomas
	sSymptomsInfo::SymptomActiveSetIt It(m_SymptomsInfo.ActiveSymptoms.begin());
	for (; It != m_SymptomsInfo.ActiveSymptoms.end(); ++It) {
	  // Se obtiene componente al sintoma activo y se dibuja
	  const sSymptomsInfo::SymptomIconMapIt IconIt = m_SymptomsInfo.Icons.find(*It);
	  ASSERT((IconIt != m_SymptomsInfo.Icons.end()) != 0);
	  IconIt->second->Icon.Draw();
	}

	// Realiza el dibujado de la interfaz de control de tiempo
	m_TimeController.ActButton.Draw();

	// ¿Se esta cargando el area?
	if (m_InGameAreaLoading.bIsLoading) {
	  // Si, se escribe la frase de AreaLoading
	  m_pFontSys->Write(5, 
						m_pGraphSys->GetVideoHeight() - 16,
						2,
						m_InGameAreaLoading.szAreaLoadingMsg);	  
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - En este metodo se recogera en exclusiva la finalizacion del FadeIn que
//   se activara SIEMPRE que se active la propia interfaz. Mientras dure el
//   FadeIn, la entrada permanecera activada, por lo que este metodo 
//   unicamente la activara.
// Parametros:
// - IDCommand. Identificador del comando.
// - udInstant. Instante de la notificacion.
// - udExtraParam. Parametro extra.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
								const dword udInstant,
								const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Se comprueba codigo de fade recibido
  switch(IDCommand) {
	case CGUIWMainInterfaz::ID_ENDFADEIN: {
	  // Fin de un FadeIn
	} break;

	case CGUIWMainInterfaz::ID_ENDAREALOADING_FADEOUT: {
	  // Fin del FadeOut de carga	  
	  // Nota: La activacion y desactivacion de interfaz devolvera la 
	  // coherencia al estado de interfaz
	  // Se desactiva interfaz
	  Deactive();	  
	  
	  // Se cambia de area
	  m_pWorld->ChangeArea(m_InGameAreaLoading.uwIDArea,
						   m_InGameAreaLoading.PlayerPos);	  
	  
	  // Se activa interfaz con FadeIn
	  Active(true);
	  
	  // Se baja el flag de area cargando
	  m_InGameAreaLoading.bIsLoading = false;	  	  

	  // Se realiza el FadeOut
	  Inherited::DoFadeIn(CGUIWMainInterfaz::ID_ENDFADEIN, 196);
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion por parte de los scripts lanzados desde esta clase.
// Parametros:
// - Notify. Tipo de notificacion.
// - ScriptEvent. Evento que lo lanza.
// - udParams. Parametros asociados.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWMainInterfaz::ScriptNotify(const RulesDefs::eScriptEvents& ScriptEvent,
								const ScriptClientDefs::eScriptNotify& Notify,								
								const dword udParams)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Filtrado de evento que manda notificacion
  switch(ScriptEvent) {
	case RulesDefs::SE_ONCLICKHOURPANEL: {
	  // Sin comportamiento por defecto
	} break;

	case RulesDefs::SE_ONKEYPRESSED: {
	  // Sin comportamiento por defecto
	} break;
	
	case RulesDefs::SE_ONFLEECOMBAT: {
	  // ¿No hubo errores?
	  if (ScriptClientDefs::SN_SCRIPT_ERROR != Notify) {
		// El script se ejecuto correctamente o no se encontro
		// ¿Comportamiento por defecto?
		if (udParams) {
		  // Si, se solicita huida de combate finalizando combate
		  SYSEngine::GetCombatSystem()->EndCombat();
		} 
	  }
	} break;	

	default:
	  ASSERT(false);
  }; // ~ switch
}