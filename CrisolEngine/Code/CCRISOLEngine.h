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

//////////////////////////////////////////////////////////////////////////////
// CCRISOLEngine.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CCRISOLEngine
//
// Descripcion:
// - Este sera el modulo principal del engine, pues actuara de contenedor de
//   todas los modulos del motor. Tambien, servira de interfaz para el manejo
//   del mismo desde el exterior.
//
// Notas:
// - La clase sera definida como un singlenton.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CCRISOLENGINE_H_
#define _CCRISOLENGINE_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"             
#endif
#ifndef _SYSENGINE_H_
#include "SYSEngine.h"
#endif
#ifndef _RESDEFS_H_
#include "ResDefs.h"
#endif
#ifndef _CGAMESTATE_H_
#include "CGameState.h"
#endif
#ifndef _CBITMAP_H_
#include "CBitmap.h"
#endif
#ifndef _STRING_H_
#include <string> 
#define _STRING_H_
#endif

// Defincion de clases / estructuras / espacios de nombres
class CFileSystem;
class CInputManager;
class CGraphicSystem;
class CAudioSystem;
class CResourceManager;
class CGraphicLoader;
class CWorld;
class CAlarmManager;
class CCommandManager;
class CGUIManager;
class CFontSystem;  
class CGameDataBase;
class CCombatSystem;
class CGFXManager;
class CVirtualMachine;
class CLogger;
class CTimer;
class CMathUtil;
class CPlayer;

// Clase CCRISOLEngine
class CCRISOLEngine
{  
private:
  // Clases amigas
  // Estados del motor / juego
  friend class CGameState;  
  friend class CGSNull;
  friend class CGSMainMenu;  
  friend class CGSCreateCharacter;
  friend class CGSGameLoading;
  friend class CGSInGame;

private:
  // Estructuras
  struct SysVarInfo {
	// Vbles del sistema
	// Windows
	HINSTANCE hInstance; // Handle HINSTANCE de Windows
    HWND      hWnd;      // Handle HWND de Windows
	
	// Nombres de archivos con datos de contenido de texto
	std::string szIDAnimTemplatesText; // Nombre del archivo

	// Ticks de AI
	float fIAMSTickTime;  // Tiempo, en ms, en que debe de actualizarse la IA
    float fIASECTickTime; // Tiempo, en sec, en que debe de actualizarse la IA
	float fTimeElapsed;   // Controla el tiempo pasado entre updates
    dword udIAMaxLoops;   // Numero maximo de loops posibles en el bucle de IA    
	
	// Control del tiempos para actualizacion de la logica
	sqword sqLastTime; // Ultimo instante de tiempo
	sqword sqActTime;  // Instante de tiempo actual
	word   udIALoops;  // Numero de loops de IA realizados
	float  fActDelta;  // Delta actual
	float  fLastDelta; // Ultimo delta	
  };

  struct sFPSInfo {
	// Control de FPS
	dword udTimeStart; // Tiempo de inicio
	dword udNumFrames; // Numero de frames realizados
	dword udTicks;     // Numero de ticks
	dword udFPS;       // Frames por segundo
  };

  struct sGameStates {
	// Instancias a los estados de juego posible
	// Estructuras
	struct sAreaLoadingParams {
	  // Parametros en la carga del area
	  word               uwIDArea;  // Identificador del area
	  CPlayer*           pPlayer;   // Instancia al jugador
	  AreaDefs::sTilePos PlayerPos; // Pos. del jugador en el area	  
	};
	
	// Datos
	CGSNull            GSNull;            // Estado nulo
	CGSMainMenu        GSMainMenu;        // Menu principal
	CGSCreateCharacter GSCreateCharacter; // Creacion de caracter para jugar
	CGSGameLoading     GSAreaLoading;     // Carga de un area
	CGSInGame          GSInGame;          // En juego
	
	// Parametros		
	sAreaLoadingParams AreaLoadingParams; // Para CGSGameLoading
  };

  // Vbles de miembro
private:
  // Instancia Singlenton
  static CCRISOLEngine* m_pCrisolEngine; // Unica instancia a la clase

  // Instancias a los subsistemas de Engine
  CFileSystem*      m_pFileSystem;      // Subsistema de ficheros
  CInputManager*    m_pInputManager;    // Subsistema de entrada
  CGraphicSystem*   m_pGraphicSystem;   // Subsistema grafico
  CAudioSystem*     m_pAudioSystem;     // Subsistema de audio
  CResourceManager* m_pResourceManager; // Subsistema manejador de recursos  
  CGraphicLoader*   m_pGraphicLoader;   // Subsistema cargador de fich. graficos
  CWorld*           m_pWorld;           // Universo de juego
  CAlarmManager*    m_pAlarmManager;    // Subsis. para la gestion de alarmas
  CCommandManager*  m_pCommandManager;  // Subsis. para gestion de comandos
  CGUIManager*      m_pGUIManager;      // Manager de ventanas de interfaz  
  CFontSystem*      m_pFontSystem;      // Subsistema de fuentes.
  CGameDataBase*    m_pGameDataBase;    // Base de datos del juego
  CCombatSystem*    m_pCombatSystem;    // Subsistema de combate
  CGFXManager*      m_pGFXManager;      // Subsistema manager de GFX
  CVirtualMachine*  m_pVirtualMachine;  // Maquina virtual de CrisolScript
  
  // Instancias a modulos de apoyo del Engine
  CLogger*   m_pLogger;   // Logger
  CTimer*    m_pTimer;    // Timer  
  CMathUtil* m_pMathUtil; // Utilidades matematicas

  // Estados del motor
  sGameStates			    m_GameStates; // Estados del motor
  CGameState*			    m_pActState;  // Estado actual
  GameStateDefs::eGameState m_NextState;  // Codigo del sig. estado
  
  // Resto  
  SysVarInfo m_SysVarInfo; // Variables de sistema generales
  sFPSInfo   m_FPSInfo;    // Vbles para el control de los FPS
  bool       m_bShowFPS;   // ¿Se desea mostrar los FPS?
  bool		 m_bInitOk;    // ¿Clase inicializada correctamente?
  bool		 m_bExit;      // ¿Hay que abandonar en el sig. update?

protected:
  // Constructor / Destructor
  CCRISOLEngine(void): m_bInitOk(false),
					   m_bExit(true),
                       m_pFileSystem(NULL),
                       m_pInputManager(NULL),
                       m_pGraphicSystem(NULL),
                       m_pAudioSystem(NULL),
                       m_pResourceManager(NULL),
                       m_pGraphicLoader(NULL),
					   m_pWorld(NULL),
                       m_pAlarmManager(NULL),
                       m_pCommandManager(NULL),
					   m_pGUIManager(NULL),
					   m_pFontSystem(NULL),
					   m_pGameDataBase(NULL),
					   m_pCombatSystem(NULL),
					   m_pGFXManager(NULL),
					   m_pVirtualMachine(NULL),
					   m_pLogger(NULL),
					   m_pMathUtil(NULL),
					   m_pTimer(NULL),
					   m_pActState(NULL) { }
public:
  ~CCRISOLEngine(void) { 
    End(); 
  }

public:
  // Protocolo de inicio y fin de instancia
  bool Init(const std::string& szConfigFileName, 
            const HINSTANCE& hInstance, 
			const HWND& hWnd);
  void End(void);
  inline bool IsInitOk(void) const { return m_bInitOk; }
private:
  // Metodos de apoyo
  void InitSysVar(const HINSTANCE& hInstance, 
				  const HWND& hWnd);  
  bool InitLogger(void);
  bool InitTimer(void);
  bool InitMathUtil(void);
  bool InitFileSystem(void);
  bool InitAlarmManager(void);
  bool InitResourceManager(void);
  bool InitGraphicLoader(void);
  bool InitFontSystem(void);
  bool InitGraphicSystem(void);
  bool InitAudioSystem(void);
  bool InitInputManager(void);
  bool InitCommandManager(void);
  bool InitWorld(void);
  bool InitGUIManager(void);
  bool InitGameDataBase(const std::string& szInitFileName);
  bool InitCombatSystem(void);
  bool InitGFXManager(void);
  bool InitVirtualMachine(void);
  bool InitGameStateManager(void);
  void EndLogger(void);
  void EndTimer(void);
  void EndMathUtil(void);
  void EndFileSystem(void);
  void EndAlarmManager(void);
  void EndResourceManager(void);
  void EndGraphicLoader(void);
  void EndFontSystem(void);
  void EndGraphicSystem(void);
  void EndAudioSystem(void);
  void EndInputManager(void);
  void EndCommandManager(void);
  void EndWorld(void);
  void EndGUIManager(void);
  void EndGameDataBase(void);
  void EndGameStateManager(void);
  void EndCombatSystem(void);
  void EndGFXManager(void);
  void EndVirtualMachine(void);

public:
  // Obtencion y destruccion de la instancia singlenton
  static inline CCRISOLEngine* const GetInstance(void) {
    if (NULL == m_pCrisolEngine) { 
      m_pCrisolEngine = new CCRISOLEngine; 
    }
    return m_pCrisolEngine;
  }
  static inline void DestroyInstance(void) {
    if (m_pCrisolEngine) {
      delete m_pCrisolEngine;
      m_pCrisolEngine = NULL;
    }
  }

public:
  // Actualizacion interna ante la insercion de un caracter
  void InputCharEvent(const sbyte sbChar) const;

public:
  // Actualizacion de frame
  bool UpdateFrame(void);
private:
  // Metodos de apoyo
  void InitLogicValues(void);
  void UpdateLogic(void);
  bool CalculeFPS(void);

private:
  // Metodos de apoyo para el cambio de estado
  void ChangeToNextState(void);

public:
  // Restauracion de recursos
  void Restore(void);
  
private:
  // Metodos de apoyo
  template<class Type> inline Type Min(const Type& Value0, const Type& Value1) const {	
	return (Value0 < Value1) ? Value0 : Value1;
  }
  
private:
  // Funciones de SYSEngine, de acceso en cualquier parte del motor
  // Acceso a subsistemas
  friend iCFileSystem* SYSEngine::GetFileSystem(void);
  friend iCInputManager* SYSEngine::GetInputManager(void);
  friend iCGraphicSystem* SYSEngine::GetGraphicSystem(void);
  friend iCAudioSystem* SYSEngine::GetAudioSystem(void);
  friend iCResourceManager* SYSEngine::GetResourceManager(void);  
  friend iCGraphicLoader* SYSEngine::GetGraphicLoader(void);
  friend iCAlarmManager* SYSEngine::GetAlarmManager(void);
  friend iCCommandManager* SYSEngine::GetCommandManager(void);  
  friend iCWorld* SYSEngine::GetWorld(void);
  friend iCGUIManager* SYSEngine::GetGUIManager(void);
  friend iCFontSystem* SYSEngine::GetFontSystem(void);
  friend iCGameDataBase* SYSEngine::GetGameDataBase(void);
  friend iCCombatSystem* SYSEngine::GetCombatSystem(void);
  friend iCVirtualMachine* SYSEngine::GetVirtualMachine(void);
  friend iCGFXManager* SYSEngine::GetGFXManager(void);
    
  // Accesos a clases de utilidad
  friend iCLogger* SYSEngine::GetLogger(void);
  friend iCTimer* SYSEngine::GetTimer(void);  
  friend iCMathUtil* SYSEngine::GetMathUtil(void);

  // Establecimiento de estados del motor / juego
  friend void SYSEngine::SetMainMenuState(void);
  friend void SYSEngine::SetCreateCharacterState(void);
  friend void SYSEngine::SetStartNewGameState(const word uwIDArea,
											  CPlayer* const pPlayer,
											  const AreaDefs::sTilePos& PlayerPos);
  friend void SYSEngine::SetAreaLoadingState(const word uwIDArea,
											 CPlayer* const pPlayer,
											 const AreaDefs::sTilePos& PlayerPos);
  friend bool SYSEngine::SetAreaLoadingState(const word uwIDArea,
											 const AreaDefs::sTilePos& PlayerPos);
  friend void SYSEngine::SetGameLoadingState(const word uwIDArea);
  friend void SYSEngine::SetInGameState(void);

  // Trabajo con los frames por segundo
  friend void SYSEngine::ShowFPS(const bool bShow);  
  friend bool SYSEngine::IsFPSShowActive(void);
  
  // Operaciones de salida
  friend void SYSEngine::FatalError(char *szMsg, ...);
  friend void SYSEngine::SetExit(const bool bExit);
  
  // Obtencion de vbles de entorno
  // Windows
  friend HINSTANCE SYSEngine::GethInstance(void);
  friend HWND SYSEngine::GethWnd(void);
  
  // Ticks de AI
  friend float SYSEngine::GetIAMSTickTime(void);  
  friend float SYSEngine::GetIASECTickTime(void);  
  friend float SYSEngine::GetTimeElapsed(void);  
  
  // Operaciones de apoyo general
  friend dword PassBitValueToIndexValue(const dword udBitValue);
  friend void SYSEngine::MakeLowercase(std::string& szText);
  friend word SYSEngine::GetLine(const sbyte* const psbSrcBuff,
								 std::string& pszDest,
								 const word uwSrcBuffSize,
								 const word uwSrcBuffPos,
								 const sbyte sbDelim);  
  friend void SYSEngine::PassToString(std::string& szString, 
									  char *szText, ...);
}; // ~ CCRISOLEngine

#endif // ~ CRISOLEngine
