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
// iCGUIManager.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - iCGUIManager
//
// Descripcion:
// - Interfaz para CGUIWindowManager.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICGUIMANAGER_H_
#define _ICGUIMANAGER_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _GUIMANAGERDEFS_H_
#include "GUIManagerDefs.h"
#endif
#ifndef _RESDEFS_H_
#include "ResDefs.h"
#endif
#ifndef _GUIDEFS_H_
#include "GUIDefs.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif
#ifndef _WORLDEFS_H_
#include "WorldDefs.h"
#endif
#ifndef _RULESDEFS_H_
#include "RulesDefs.h"
#endif
#ifndef _COMMANDDEFS_H_
#include "CommandDefs.h"
#endif
#ifndef _INPUTDEFS_H_
#include "InputDefs.h"
#endif

// Defincion de clases / estructuras / espacios de nombres
class CPlayer;
class CWorldEntity;
class CGUIWindow;
class CCriature;
class CSprite;
struct iCEntitySelector;
struct iCGUIWindowClient;
struct iCCommandClient;

// Interfaz iCGUIManager
struct iCGUIManager
{
public:
  // Metodos generales de trabajo con interfaces
  virtual GUIManagerDefs::eGUIWindowType GetGameGUIWindowState(void) const = 0;

public:
  // Trabajo con interfaces asociados a un estado del motor.
  // Menu principal
  virtual void SetMainMenuWindow(void) = 0;
  // Generador de caracteres
  virtual void SetCharacterCreatorWindow(void) = 0;
  // Menu de juego
  virtual void SetGameMenuWindow(void) = 0;
  // Main interfaz
  virtual void SetMainInterfazWindow(void) = 0;
  virtual void MainInterfazSetMode(const WorldDefs::eWorldMode& Mode, 
								   const AreaDefs::EntHandle& hCriatureInTurn = 0) = 0;
  virtual bool IsKeyPressedInMainInterfaz(const InputDefs::eInputEventCode& KeyPressed) const = 0;

  // Interaccion con entidades
  virtual void SetEntityInteractWindow(const sPosition& InitDrawPos,
									   CCriature* const pEntitySrc,
									   CWorldEntity* const pEntityDest) = 0;
  // Seleccion de un item del inventario para usar
  virtual void SetItemSelectToUseWindow(CCriature* const pCriatureSrc,
										CWorldEntity* const pEntityDest) = 0;
  // Inventario
  virtual void SetPlayerProfileWindow(void) = 0;

  // Cargando un juego nuevo
  virtual void SetGameLoadingWindow(const word uwIDArea,
									CPlayer* const pPlayer = NULL,
									const AreaDefs::sTilePos& PlayerPos = AreaDefs::sTilePos(0, 0)) = 0;

  // Interfaz de conversacion
  virtual bool SetConversatorWindow(CWorldEntity* const pMainConv,
									CWorldEntity* const pSecondaryConv) = 0;
  virtual bool AddOptionToConversatorWindow(const GUIDefs::GUIIDComponent& IDOption,
										    const std::string& szText) = 0;
  virtual void ResetOptionsInConversatorWindow(void) = 0;
  virtual bool GetOptionFromConversatorWindow(iCGUIWindowClient* const pClient) = 0;

  // Interfaz de intercambio de datos entre entre
  virtual bool SetTradeWindow(CCriature* const pMainCriature,
							  CWorldEntity* const pEntity,
							  iCGUIWindowClient* const pClient) = 0;  
  
  // Interfaz de combate
  virtual void SetCombatWindow(const AreaDefs::EntHandle& hCriature) = 0;
  virtual void CombatSetActionPoints(const RulesDefs::CriatureActionPoints& ActionPoints) = 0;

  // Interfaz de presentacion
  virtual bool SetPresentationWindow(const std::string& szPresentationProfile) = 0;

  // Interfaz de cargar partidas
  virtual void SetLoaderSavedFileWindow(void) = 0;

  // Interfaz de guardar partidas
  virtual void SetSaverSavedFileWindow(void) = 0;

public:
  // Trabajo con interfaces generales
  // Cuadros de dialogo
  virtual bool ActiveAdviceDialog(const std::string& szText,
								  iCGUIWindowClient* const pClient) = 0;
  virtual bool ActiveQuestionDialog(const std::string& szText,
								    const bool bCancelPresent,
								    iCGUIWindowClient* const pClient) = 0;
  virtual void DeactiveAdviceDialog(void) = 0;
  virtual void DeactiveQuestionDialog(void) = 0;
  // Lector de archivos de texto
  virtual bool ActiveTextReader(const std::string& szTitle,
								const std::string& szTextFileName,
								iCGUIWindowClient* const pClient) = 0;
  virtual void DeactiveTextReader(void) = 0;
  // Seleccionador de opciones de texto general
  virtual bool ActiveTextOptionsSelector(const std::string& szTitle,
										 const bool bCanUseCancel,
										 iCGUIWindowClient* const pClient) = 0;
  virtual void DeactiveTextOptionsSelector(void) = 0;
  virtual bool AddOptionToTextOptionsSelector(const GUIDefs::GUIIDComponent& IDOption,
											  const std::string& szText,
											  const bool bCanSelect = true) = 0;
  virtual void ReleaseTextOptionsSelector(void) = 0;

public:
  // Trabajo con los cursores de interfaz
  virtual void SetGUICursor(const GUIManagerDefs::eGUICursorType& Cursor) = 0;
  virtual CSprite* AttachSpriteToGUICursor(CSprite* const pSprite) = 0;
  virtual inline GUIManagerDefs::eGUICursorType GetActGUICursor(void) const = 0;
  virtual inline byte GetGUICursorsHeight(void) const = 0;
  virtual inline byte GetGUICursorsWidth(void) const = 0;

public:
  // Modo de Cutscene
  virtual void BeginCutScene(iCCommandClient* const pClient = NULL,
							 const CommandDefs::IDCommand& IDCommand = 0,
							 const dword udExtraParam = 0) = 0;
  virtual void EndCutScene(iCCommandClient* const pClient = NULL,
						   const CommandDefs::IDCommand& IDCommand = 0,
						   const dword udExtraParam = 0) = 0;
  virtual bool IsCutSceneActive(void) const = 0;

public:
  // Trabajo con la consola de interfaz
  virtual void WriteToConsole(const std::string& szText, 
							  const bool bPlaySound) = 0;
  virtual void ResetConsoleInfo(void) = 0;

public:
  // Trabajo con texto flotante
  virtual void SetFloatingText(const std::string& szText,
							   const GraphDefs::sRGBColor& RGBColor,
							   const GraphDefs::sAlphaValue& Alpha,
							   const GUIManagerDefs::eFloatingTextType& Type) = 0;
  virtual void UnsetFloatingText(void) = 0;
  virtual void UnsetFloatingText(const std::string& szText) = 0;

public:
  // Trabajo con el selector de entidades global
  virtual iCEntitySelector* const GetMapSelector(void) = 0;
};

#endif // ~ #ifdef _ICGUIMANAGER_H_
