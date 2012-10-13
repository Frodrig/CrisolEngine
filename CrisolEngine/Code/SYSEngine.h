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
// SYSEngine.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Aqui se definira el espacio de nombres que contendra todas aquellas
//   funciones de uso global a lo largo de los distintos modulos del engine.
//   Cualquier modulo y/o clase que incluya esta cabecera podra tener acceso 
//   a las mismas.
//
// Notas:
// - Las funciones que se declaren en este espacio de nombres, seran declaradas
//   a su vez como funciones amigas del modulo principal, CCRISOLEngine, de
//   tal forma que serviran de interfaz para acceder a los distintos servicios
//   e instancias del controlador principal del engine.
// - Todas las instancias a clases en CCRISOLEngine siempre seran devueltas en
//   terminos de su interfaz.
// - La implementacion de tales funciones se realizara en CCRISOLEngine.cpp
///////////////////////////////////////////////////////////////////////////////
#ifndef _SYSENGINE_H_
#define _SYSENGINE_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"            
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"          
#endif
#ifndef _RULESDEFS_H_
#include "RulesDefs.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif

// Declaracion de clases 
class CPlayer;

// Declaracion de las interfaces a devolver
struct iCFileSystem;          
struct iCInputManager;        
struct iCGraphicSystem;
struct iCAudioSystem; 
struct iCResourceManager;     
struct iCGraphicLoader;       
struct iCAlarmManager;       
struct iCCommandManager;
struct iCGUIManager;
struct iCWorld;
struct iCInfoSysDataLoader;
struct iCFontSystem;
struct iCGameDataBase;
struct iCCombatSystem;
struct iCGFXManager;
struct iCVirtualMachine;
struct iCLogger;             
struct iCTimer;              
struct iCMathUtil;

// Se define el espacio de nombres SYSEngine
namespace SYSEngine
{   
  // Obtencion de subsistemas
  iCFileSystem* GetFileSystem(void);
  iCInputManager* GetInputManager(void);
  iCGraphicSystem* GetGraphicSystem(void);
  iCAudioSystem* GetAudioSystem(void);
  iCResourceManager* GetResourceManager(void);
  iCGraphicLoader* GetGraphicLoader(void);    
  iCAlarmManager* GetAlarmManager(void);
  iCCommandManager* GetCommandManager(void);
  iCWorld* GetWorld(void);
  iCGUIManager* GetGUIManager(void);
  iCInfoSysDataLoader* GetInfoSysDataLoader(void);
  iCFontSystem* GetFontSystem(void);
  iCGameDataBase* GetGameDataBase(void);  
  iCCombatSystem* GetCombatSystem(void);
  iCGFXManager* GetGFXManager(void);
  iCVirtualMachine* GetVirtualMachine(void);
  
  // Obtencion de instancias a clases de utilidad
  iCLogger* GetLogger(void);
  iCTimer* GetTimer(void);
  iCMathUtil* GetMathUtil(void);

  // Establecimiento de estados del motor / juego
  void SetMainMenuState(void);
  void SetCreateCharacterState(void);
  void SetStartNewGameState(const word uwIDArea,
							CPlayer* const pPlayer,
							const AreaDefs::sTilePos& PlayerPos);
  bool SetAreaLoadingState(const word uwIDArea,
						   const AreaDefs::sTilePos& PlayerPos);
  void SetAreaLoadingState(const word uwIDArea,
						   CPlayer* const pPlayer,
						   const AreaDefs::sTilePos& PlayerPos);
  void SetGameLoadingState(const word uwIDArea);
  void SetInGameState(void);

  // Trabajo con los frames por segundo
  void ShowFPS(const bool bShow);  
  bool IsFPSShowActive(void);
  
  // Operaciones directas
  void FatalError(char *szMsg, ...);
  void SetExit(const bool bExit);
  
  // Obtencion de vbles de entorno
  // Windows
  HINSTANCE GethInstance(void);
  HWND GethWnd(void);
  
  // Ticks de AI
  float GetIAMSTickTime(void);  
  float GetIASECTickTime(void);  
  float GetTimeElapsed(void);  
  
  // Metodos de apoyo generales
  dword PassBitValueToIndexValue(const dword udBitValue);
  void MakeLowercase(std::string& szText);  
  word GetLine(const sbyte* const psbSrcBuff,
			   std::string& szDest,
			   const word uwSrcBuffSize,
			   const word uwSrcBuffPos,
			   const sbyte sbDelim);
  void PassToString(std::string& szString, 
					char *szText, ...);
}

#endif // SYSEngine                        
