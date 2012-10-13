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
// CCRISOLEngine.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CCRISOLEngine.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGraphicSystem.h"       
#include "CCRISOLEngine.h"

#include "CInputManager.h"        
#include "CFileSystem.h"          
#include "CResourceManager.h"     
#include "CGraphicLoader.h"       
#include "CWorld.h"
#include "CAudioSystem.h"
#include "CAlarmManager.h"        
#include "CCommandManager.h"      
#include "CGUIManager.h"
#include "CFontSystem.h"
#include "CGameDataBase.h"
#include "CCombatSystem.h"
#include "CGFXManager.h"
#include "CVirtualMachine.h"
#include "CTimer.h"              
#include "CLogger.h"
#include "CMathUtil.h"
#include "CCBTEngineParser.h"
#include "time.h"
#include <fstream>

// Se inicializan los miembros static
CCRISOLEngine* CCRISOLEngine::m_pCrisolEngine = NULL;

///////////////////////////////////////////////////////////////////////////////
// IMPLEMENTACION DE SYSEngine
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Se define un espacio de nombres anonimo (solo sera accesible desde este
// fichero), en el que se guardara una copia de la unica instancia a la
// clase CCRISOLEngine. Dicha instancia se utilizara para que las funciones
// friend puedan acceder a los servicios de CCRISOLEngine directamente
///////////////////////////////////////////////////////////////////////////////
namespace {
  CCRISOLEngine* pCRISOLEngine = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// A continuacion se implementan las funciones de SYSEngine que estaran
// declaradas como friends de CCRISOLEngine. Accederan a los elementos 
// necesarios de CCRISOLEngine a traves de pCRISOLEngine, que es inicializado
// en CCRISOLEngine::Init
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Manda un mensaje de alarma al exterior, usando un sistema equivalente
//   al de printf. Despues finaliza la instancia y realiza la terminacion
//   de la ejecucion.
// Parametros:
// - szMsg: Mensaje con el error.
// - ...: Resto de parametros asociados al mensaje.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SYSEngine::FatalError(char *szMsg, ...)
{
  // Comprobaciones de validez de operacion
  ASSERT(pCRISOLEngine);
  
  // Vbles
  char    szTextBuffer[1024]; // Buffer con el texto a mostrar
  va_list args;               // Lista de argumentos

  // Se crea una cadena con el texto recibido	  
  va_start(args, szMsg);
  vsprintf(szTextBuffer, szMsg, args);
  va_end(args);

  // Se escribe en un fichero a parte el mensaje
  std::ofstream File;
  File.open("FatalError.txt");
  if (File.is_open()) {
	File << "Se ha producido un error fatal en el motor\n";
	File << szTextBuffer << "\n";
	File.flush();
	OutputDebugString("Se ha producido un error fatal en el motor\n");
	OutputDebugString(szTextBuffer);
  }

  // Se abandona el motor
  if (pCRISOLEngine->IsInitOk()) {
	pCRISOLEngine->End();
	exit(1); 
  }

  // Se abandona con codigo de error
  //exit(1); 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el flag de abandono del engine. Cuando se ponga el flag de 
//   abandono a true, el metodo Update devolvera false.
// Parametros:
// - bExit: Nuevo valor para el flag de abandono.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SYSEngine::SetExit(const bool bExit)
{
  // Comprobaciones de validez de operacion
  ASSERT(pCRISOLEngine);
  ASSERT(pCRISOLEngine->IsInitOk());

  // Se establece el flag
  pCRISOLEngine->m_bExit = bExit;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve interfaz global al CFileSystem
// Parametros:
// Modifica:
// Devuelve:
// - Interfaz iCFileSystem
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCFileSystem* 
SYSEngine::GetFileSystem(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);
  return pCRISOLEngine->m_pFileSystem; 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve interfaz global al CInputManager
// Parametros:
// Devuelve:
// - Interfaz iCInputManager
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCInputManager* 
SYSEngine::GetInputManager(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);
  return pCRISOLEngine->m_pInputManager; 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve interfaz global al CCGraphicSystem
// Parametros:
// Modifica:
// Devuelve:
// - Interfaz iCGraphicSystem
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCGraphicSystem* 
SYSEngine::GetGraphicSystem(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);
  return pCRISOLEngine->m_pGraphicSystem;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve interfaz global al CAudioSystem
// Parametros:
// Devuelve:
// - Interfaz iCAudioSystem
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCAudioSystem* 
SYSEngine::GetAudioSystem(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);
  return pCRISOLEngine->m_pAudioSystem;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve interfaz global al CResourceManager
// Parametros:
// Modifica:
// Devuelve:
// - Interfaz iCResourceManager
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCResourceManager* 
SYSEngine::GetResourceManager(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);
  return pCRISOLEngine->m_pResourceManager;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve interfaz global al CGraphicLoader
// Parametros:
// Devuelve:
// - Interfaz iCGraphicLoader
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCGraphicLoader* 
SYSEngine::GetGraphicLoader(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);  
  return pCRISOLEngine->m_pGraphicLoader;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve interfaz global al CAlarmManager
// Parametros:
// Modifica:
// Devuelve:
// - Interfaz iCAlarmManager
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCAlarmManager* 
SYSEngine::GetAlarmManager(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);  
  return pCRISOLEngine->m_pAlarmManager;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve interfaz global al CWorld
// Parametros:
// Devuelve:
// - Interfaz iCWorld
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCWorld* 
SYSEngine::GetWorld(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);  
  return pCRISOLEngine->m_pWorld;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve interfaz global al CCommandManager
// Parametros:
// Modifica:
// Devuelve:
// - Interfaz iCCommandManager
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCCommandManager* 
SYSEngine::GetCommandManager(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);  
  return pCRISOLEngine->m_pCommandManager;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve interfaz al manager de interfaces
// Parametros:
// Devuelve:
// - Interfaz iCGUIWindowManager
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCGUIManager* 
SYSEngine::GetGUIManager(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);  
  return pCRISOLEngine->m_pGUIManager;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve interfaz a la base de datos
// Parametros:
// Devuelve:
// - Interfaz iCGameDataBase
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCGameDataBase* 
SYSEngine::GetGameDataBase(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);  
  return pCRISOLEngine->m_pGameDataBase;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve interfaz al subsistema de manejo de fuentes
// Parametros:
// Devuelve:
// - Interfaz iCFontSystem
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCFontSystem*
SYSEngine::GetFontSystem(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);  
  return pCRISOLEngine->m_pFontSystem;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve interfaz al subsistema de manejo GFX (graficos de fx)
// Parametros:
// Devuelve:
// - Interfaz iCGFXManager
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCGFXManager*
SYSEngine::GetGFXManager(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);  
  return pCRISOLEngine->m_pGFXManager;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve interfaz global al CLogger.
// Parametros:
// Devuelve:
// - Interfaz iCLogger.
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCLogger* 
SYSEngine::GetLogger(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);  
  return pCRISOLEngine->m_pLogger; 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve el interfaz a CCombatSystem
// Parametros:
// Devuelve:
// - Interfaz a iCCombatSystem.
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCCombatSystem*
SYSEngine::GetCombatSystem(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);
  return pCRISOLEngine->m_pCombatSystem;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve el interfaz a CVirtualMachine
// Parametros:
// Devuelve:
// - Interfaz a iCVirtualMachine
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCVirtualMachine*
SYSEngine::GetVirtualMachine(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);
  return pCRISOLEngine->m_pVirtualMachine;
}


///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve interfaz global al CTimer.
// Parametros:
// Devuelve:
// - Interfaz iCTimer.
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCTimer* 
SYSEngine::GetTimer(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);
  return pCRISOLEngine->m_pTimer; 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve interfaz global a la clase de utilidades
// Parametros:
// Devuelve:
// - Interfaz iCMathUtil
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCMathUtil* 
SYSEngine::GetMathUtil(void)
{
  // SOLO si el engine esta correctamente inicializado
  ASSERT(pCRISOLEngine);
  return pCRISOLEngine->m_pMathUtil;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece como estado siguiente, el estado asociado a la pantalla 
//   de menu principal si es que es posible.
// Parametros:
// Devuelve:
// Notas:
// - Se podra llegar al menu principal desde:
//   * Estado nulo
//   * Creacion de un personaje (por cancelacion)
//   * Juego (a traves del menu de juego)
///////////////////////////////////////////////////////////////////////////////
void 
SYSEngine::SetMainMenuState(void)
{
  // SOLO si instancia al engine
  ASSERT(pCRISOLEngine);  

  // Se obtiene estado actual y se realiza el cambio si procede
  ASSERT(pCRISOLEngine->m_pActState);
  switch(pCRISOLEngine->m_pActState->GetState()) {
	case GameStateDefs::GS_NULL:
	case GameStateDefs::GS_CREATE_CHARACTER:	
	case GameStateDefs::GS_INGAME: {	  
	  pCRISOLEngine->m_NextState = GameStateDefs::GS_MAIN_MENU;
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece estado de creacion de caracter.
// Parametros:
// Devuelve:
// Notas:
// - Se podra llegar a la creacion de personajes desde
//   * MainMenu (pulsando NewGame)
///////////////////////////////////////////////////////////////////////////////
void 
SYSEngine::SetCreateCharacterState(void)
{
  // SOLO si instancia al engine
  ASSERT(pCRISOLEngine);  

  // Se obtiene estado actual y se realiza el cambio si procede
  ASSERT(pCRISOLEngine->m_pActState);
  switch(pCRISOLEngine->m_pActState->GetState()) {	
	case GameStateDefs::GS_MAIN_MENU: {	  
	  pCRISOLEngine->m_NextState = GameStateDefs::GS_CREATE_CHARACTER;
	} break;
  }; // ~ swtich
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece un estado de transicion (realmente no existira como tal) en el
//   que el motor se prepara para comenzar un nuevo juego. Por parametro se
//   recibiran los valores del personaje jugador que se creo para establecerlo
//   en el universo de juego.
// - Una vez establecido, lo normal sera generar un evento de comienzo de nuevo
//   juego para que el usuario cargue el area que corresponda.
// Parametros:
// - uwIDArea. Identificador del area a cargar.
// - pPlayer. Instancia al jugador creado
// - PlayerPos. Posicion del jugador en el area a cargar.
// Devuelve:
// Notas:
// - Se podra llegar a la creacion de personajes desde
//   * CreateCharacter
///////////////////////////////////////////////////////////////////////////////
void 
SYSEngine::SetStartNewGameState(const word uwIDArea,
								CPlayer* const pPlayer,
								const AreaDefs::sTilePos& PlayerPos)
{
  // SOLO si instancia al engine
  ASSERT(pCRISOLEngine);  
  // SOLO si parametros validos
  ASSERT(pPlayer);

  // Se obtiene estado actual y se realiza el cambio si procede
  ASSERT(pCRISOLEngine->m_pActState);
  switch(pCRISOLEngine->m_pActState->GetState()) {	
	case GameStateDefs::GS_CREATE_CHARACTER: {	  
	  // Se carga directamente el area	  
	  SYSEngine::SetAreaLoadingState(uwIDArea, pPlayer, PlayerPos);
	} break;
  }; // ~ switch  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece estado de carga de area cuando esta se realiza desde un
//   comienzo o carga de partida independiente.
// Parametros:
// - uwIDArea. Identificador del area.
// - pPlayer. Instancia al jugador.
// - PlayerPos. Posicion donde se situara al jugador.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SYSEngine::SetAreaLoadingState(const word uwIDArea,
							   CPlayer* const pPlayer,
							   const AreaDefs::sTilePos& PlayerPos)
{
  // SOLO si instancia al engine
  ASSERT(pCRISOLEngine);  
  // SOLO si parametros validos
  ASSERT(pPlayer);

  // Se obtiene estado actual y se realiza el cambio si procede
  ASSERT(pCRISOLEngine->m_pActState);
  switch(pCRISOLEngine->m_pActState->GetState()) {	
	case GameStateDefs::GS_CREATE_CHARACTER: {
	  // Desde estado de creacion de personaje
	  pCRISOLEngine->m_GameStates.AreaLoadingParams.uwIDArea = uwIDArea;
	  pCRISOLEngine->m_GameStates.AreaLoadingParams.pPlayer = pPlayer;
	  pCRISOLEngine->m_GameStates.AreaLoadingParams.PlayerPos = PlayerPos;
	  pCRISOLEngine->m_NextState = GameStateDefs::GS_AREA_LOADING;
	} break;
  }; // ~ switch
} 

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece estado de carga de un nuevo juego. En concreto, esta funcion
//   se llamara cuando se cargue un juego a partir de una partida salvada.
// Parametros:
// - uwIDArea. Identificador del area.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SYSEngine::SetGameLoadingState(const word uwIDArea)
{
  // SOLO si instancia al engine
  ASSERT(pCRISOLEngine);  

  // Se obtiene estado actual y se realiza el cambio si procede
  ASSERT(pCRISOLEngine->m_pActState);
  switch(pCRISOLEngine->m_pActState->GetState()) {	
	case GameStateDefs::GS_INGAME:
	case GameStateDefs::GS_MAIN_MENU: {
	  // Desde estado de juego o bien desde el menu principal
	  pCRISOLEngine->m_GameStates.AreaLoadingParams.uwIDArea = uwIDArea;
  	  pCRISOLEngine->m_GameStates.AreaLoadingParams.pPlayer = NULL;
	  pCRISOLEngine->m_NextState = GameStateDefs::GS_AREA_LOADING;
	} break;
  }; // ~ switch
} 

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Simula un cambio de estado para la carga de un area, con la diferencia
//   de que se debera de estar en juego y que lo que hara unicamente sera
//   llamar a MainInterfaz para que realice el trabajo de carga de un area.
// Parametros:
// - uwIDArea. Identificador del area a cargar.
// - PlayerPos. Posicion donde se situara al jugador.
// Devuelve:
// - Si se pudo cargar true, en caso contrario false.
// Notas:
// - Notese que no hay un cambio real de estado, solo se simula.
///////////////////////////////////////////////////////////////////////////////
bool
SYSEngine::SetAreaLoadingState(const word uwIDArea,
							   const AreaDefs::sTilePos& PlayerPos)
{
  // SOLO si instancia al engine
  ASSERT(pCRISOLEngine);  

  // Se obtiene estado actual y se realiza el cambio si procede
  ASSERT(pCRISOLEngine->m_pActState);
  switch(pCRISOLEngine->m_pActState->GetState()) {	
	case GameStateDefs::GS_INGAME: {
	  // ¿Esta activo el MainInterfaz?
	  if (pCRISOLEngine->m_pGUIManager->GetGameGUIWindowState() == GUIManagerDefs::GUIW_MAININTERFAZ) {
		// Se intenta cargar
		return pCRISOLEngine->m_pGUIManager->MainInterfazInGameAreaLoading(uwIDArea, 
																		   PlayerPos);
	  } else {
		// No se esta en estado valido
		return false;
	  }
	} break;

	default:
	  ASSERT(false);
	  return false;
  }; // ~ switch
} 
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece como estado siguiente, el estado asociado al juego en ejecucion,
//   si es que es posible.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SYSEngine::SetInGameState(void)
{
  // SOLO si instancia al engine
  ASSERT(pCRISOLEngine);  

  // Se obtiene estado actual y se realiza el cambio si procede
  ASSERT(pCRISOLEngine->m_pActState);
  switch(pCRISOLEngine->m_pActState->GetState()) {
	case GameStateDefs::GS_AREA_LOADING: {
	  // Desde estado nulo	  
	  pCRISOLEngine->m_NextState = GameStateDefs::GS_INGAME;
	} break;
  }; // ~ switch
} 

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el flag asociado a la muestra / no muestra de los frames por
//   segundo.
// Parametros:
// - bShow. Flag de muestra.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SYSEngine::ShowFPS(const bool bShow)
{
  // SOLO si instancia al engine
  ASSERT(pCRISOLEngine);  

  // Se toma flag
  pCRISOLEngine->m_bShowFPS = bShow;

  // ¿Se desea establecer?
  if (bShow) {
	// Se inicializan valores de FPS
	pCRISOLEngine->m_FPSInfo.udTimeStart = pCRISOLEngine->m_pTimer->GetTime();
	pCRISOLEngine->m_FPSInfo.udNumFrames = 0;
	pCRISOLEngine->m_FPSInfo.udTicks = pCRISOLEngine->m_pTimer->GetTime() - 
									   pCRISOLEngine->m_FPSInfo.udTimeStart;
	pCRISOLEngine->m_FPSInfo.udFPS = 0;    
  }
} 

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve el flag de muestra de FPS
// Parametros:
// Devuelve:
// - El flag de muestra de FPS
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
SYSEngine::IsFPSShowActive(void)
{
  // SOLO si instancia al engine
  ASSERT(pCRISOLEngine);

  // Retorna flag
  return pCRISOLEngine->m_bShowFPS;
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve el handle HINSTANCE de Windows
// Parametros:
// Devuelve:
// - El handle HINSTANCE de Windows
// Notas:
///////////////////////////////////////////////////////////////////////////////
HINSTANCE 
SYSEngine::GethInstance(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);  
  return pCRISOLEngine->m_SysVarInfo.hInstance;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve el handle HWND de Windows
// Parametros:
// Modifica:
// Devuelve:
// - Handle HWND de Windows
// Notas:
///////////////////////////////////////////////////////////////////////////////
HWND 
SYSEngine::GethWnd(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);  
  return pCRISOLEngine->m_SysVarInfo.hWnd;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve los ticks asociados a la IA. Los ticks seran relativos a los
//   milisegundos en los que se ejectan las actividades logicas (de IA)
// Parametros:
// Devuelve:
// - Ticks de actualizacion logica en ms.
// Notas:
//////////////////////////////////////////////////////////////////////////////
float 
SYSEngine::GetIAMSTickTime(void)
{
   // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);
  return pCRISOLEngine->m_SysVarInfo.fIAMSTickTime;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve los ticks asociados a la IA. Los ticks seran relativos a los
//   segundos en los que se ejectan las actividades logicas (de IA)
// Parametros:
// Devuelve:
// - Ticks de actualizacion logica en segundos
// Notas:
//////////////////////////////////////////////////////////////////////////////
float 
SYSEngine::GetIASECTickTime(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);
  return pCRISOLEngine->m_SysVarInfo.fIASECTickTime;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve el tiempo pasado entre dos actualizaciones
// Parametros:
// Modifica:
// Devuelve:
// - Tiempo pasado entre dos actualizaciones
// Notas:
//////////////////////////////////////////////////////////////////////////////
float
SYSEngine::GetTimeElapsed(void)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);
  return (pCRISOLEngine->m_SysVarInfo.fTimeElapsed / 1000.0f);
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Pasa un valor asociado a bit a un valor asociado a indice. Por ejemplo,
//   el valor 0x0001 se correspondera con el valor de indice 0.
// Parametros:
// - udBitValue. El valor en formato bit.
// Devuelve:
// - El valor en formato index.
// Notas:
///////////////////////////////////////////////////////////////////////////////
dword 
SYSEngine::PassBitValueToIndexValue(const dword udBitValue)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);

  // Inicializa el iterador a bit
  dword udBitIt = udBitValue;
  if (udBitIt) {
	udBitIt >>= 1;
  }

  // Procede a realizar desplazamiento bit a bit
  dword udIdxValue = 0;
  for (; udBitIt; udBitIt >>= 1) {
	++udIdxValue;
  }

  // retorna
  return udIdxValue;  
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Pasa el string recibido a minusculas.
// Parametros:
// - szText. Texto a pasar a minusculas.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
SYSEngine::MakeLowercase(std::string& szText)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);
  
  // Realiza el paso a minusculas
  word uwIt = 0;
  while (uwIt < szText.length()) {
	szText[uwIt] = tolower(szText[uwIt]);
	++uwIt;
  }
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Extrae del buffer pszSrcBuff una linea que comienza en la posicion 
//   udSrcBuffPos y acaba en un caracter igual a sbDelim o bien contiene el
//   ultimo caracter del buffer. La linea se depositara en el string
//   pszDet. 
// Parametros:
// - psbSrcBuff. Buffer donde estan los caracteres.
// - szDest. String donde depositiar la linea.
// - uwSrcBuffSize. Tamaño del buffer.
// - uwSrcBuffPos. Posicion desde donde comenzar a leer. 
// - sbDelim. Delimitador. 
// Devuelve:
// - La longitud del string.
// Notas:
// - El string que se extraiga tendra por terminacion el '\0'
///////////////////////////////////////////////////////////////////////////////
word 
SYSEngine::GetLine(const sbyte* const psbSrcBuff,
				   std::string& szDest,
				   const word uwSrcBuffSize,
				   const word uwSrcBuffPos,
				   const sbyte sbDelim)
{
  // SOLO si el engine esta correctamente inicializado.
  ASSERT(pCRISOLEngine);
  // SOLO si parametros valido
  ASSERT(psbSrcBuff);
  ASSERT((uwSrcBuffPos < uwSrcBuffSize) != 0);

  // Procede a encontrar el delimitador  
  word uwIt = uwSrcBuffPos;
  while (uwIt < uwSrcBuffSize && 
		 sbDelim != psbSrcBuff[uwIt] &&
		 '\0' != psbSrcBuff[uwIt]) {
	// Sig caracter
	++uwIt;
  }

  // Se copia el buffer en el string
  szDest.erase();
  const word uwSize = uwIt - uwSrcBuffPos;
  szDest.replace(0, uwSize, &psbSrcBuff[uwSrcBuffPos], uwSize);
  
  // Se retorna la cantidad de ellos copiados
  return uwSize;
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Convierte una entrada en formato printf a un string y se devuelve por
//   parametro.
// Parametros:
// - szString. Direccion del string donde alojar la conversion.
// - szText. Cadena en formato printf.
// - ... Argumentos
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
SYSEngine::PassToString(std::string& szString, char *szText, ...)
{
  // Vbles
  static char szTextBuffer[1024]; // Buffer con el texto a mostrar
  va_list     args;               // Lista de argumentos

  // Se crea una cadena con el texto recibido	  
  va_start(args, szText);
  vsprintf(szTextBuffer, szText, args);
  va_end(args);

  // Se copia al string
  szString = szTextBuffer;
}

//////////////////////////////////////////////////////////////////////////////
// IMPLEMENTACION DE CCRISOLEngine
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Esta es la clase principal del engine. Desde aqui se crearan las 
//   instancias a todos los modulos y clases de uso comun del engine. Tambien
//   se realizara la configurancion de los mismos usando los datos que se
//   puedan leer del fichero de nombre szConfigFileName.
//   Tambien recibira instancias de Windows que puedan ser necesarias para 
//   otros modulos, en concreto, los que encapuslen algun wrapper de DX.
// - Se inicializara COM.
// Parametros:
// - szConfigFileName: Nombre del fichero con la configuracion de inicio del
//   motor.
// - HInstance: valor del HANDLE HInstance de Windows
// - HWnd: valor del HANDLE HWND de Windows
// Devuelve:
// - El valor de m_bInitOk.
// Notas:
// - En primer lugar se inicializaran aquellos pequeños modulos que puedan ser
//   necesarios para ciertas tareas de los grandes subsistemas del engine.
//   Despues se procedera con estos ultimos. De no ser asi, podrian existir
//   problemas.
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::Init(const std::string& szConfigFileName,
					const HINSTANCE& hInstance, 
					const HWND& hWnd)
{
  // No se deja reicinializar
  if (IsInitOk()) { 
	return false; 
  }

  // Se inicializa el logger
  if (InitLogger()) { 
	// Se hacen accesibles las operaciones de SYSEngine cuanto antes
	pCRISOLEngine = this;

	// Se levanta el flag de inicializacion para permitir llamadas a End
	// en caso de error desde SYSEngine::FatalError
	m_bInitOk = true;

	// Inicializacion COM
	CoInitialize(NULL);

	#ifdef ENGINE_TRACE
	  m_pLogger->Write("CCRISOLEngine::Init> Inicializando CRISOLEngine.\n");    
	#endif

	// Se inicializa clase de utilidades matematicas
	if (InitMathUtil()) { 
	  // Se inicializan subsistemas basicos para el trabajo con ficheros
	  // es necesario inicializarlos aqui para que el resto de subsistemas puedan
	  // obtener instancias al parser.
	  if (InitFileSystem() &&
		  m_pFileSystem->AddCPAKFile("GameData.pak") &&
		  InitGameDataBase("CrisolEngine.ini")) { 
		// Se inicializan vbles del sistema
		InitSysVar(hInstance, hWnd);

		// Se inicializa timer
		if (InitTimer()) {   
		  // Se inicializan resto de subsistemas
		  if (InitCommandManager() &&
			  InitGraphicLoader() &&
			  InitResourceManager() &&
			  InitAlarmManager() &&
			  InitGraphicSystem() &&
			  InitInputManager() &&
			  InitFontSystem() &&
			  InitAudioSystem() &&
			  InitVirtualMachine() &&
			  InitWorld() &&
			  InitGUIManager() &&
			  InitCombatSystem() &&
			  InitGFXManager()) {  
			// Se establece como estado actual el MainMenu
			m_GameStates.GSMainMenu.Init(this);
			m_pActState = &m_GameStates.GSMainMenu;
			ASSERT(m_pActState);
			m_NextState = GameStateDefs::GS_NULL;
  
			// Se establece el trabajo con los FPS
			SYSEngine::ShowFPS(false);
  
			// Se baja flag de abandono
			m_bExit = false;  
  
			// Todo correcto 
			return true;
		  }
		}
	  }
	}
  }
	
  // Hubo problemas
  #ifdef ENGINE_TRACE
	if (m_pLogger) {
	  m_pLogger->Write("                   | Error.\n");    
	}
  #endif  
  End();  
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia actual a CCRISOLEngine. Al finalizarla, libera todos
//   los recursos mantenidos por la misma.
// - Finalizacion COM.
// Parametros:
// Devuelve:
// Notas:
// - La liberacion y terminacion de los modulos, siempre ha de hacerse
//   en orden inverso al que fueron creados e inicializados.
///////////////////////////////////////////////////////////////////////////////
void 
CCRISOLEngine::End(void)
{
  // Finaliza instancia
  if (IsInitOk()) {
	#ifdef ENGINE_TRACE    
	  m_pLogger->Write("CCRISOLEngine::End> Finalizando CRISOLEngine.\n");        
	#endif  

	// Si hay estado funcionando, se termina
	if (m_pActState) {
	  m_pActState->End();
	  m_pActState = NULL;
	}

	// Finalizacion de las instancias a los subsistemas        	        	 
	EndGFXManager();
	EndCombatSystem();
	EndGUIManager();
	EndWorld();
	EndVirtualMachine();	
	EndAudioSystem();
	EndFontSystem();
	EndGraphicSystem();	
	EndGraphicLoader();
	EndAlarmManager();	
	EndResourceManager();		
	EndInputManager();		
	EndCommandManager();
	EndGameDataBase();
	EndFileSystem();
		
    // Finalizacion de las instancias a los modulos de apoyo
	EndMathUtil();
	EndTimer();
	
	#ifdef ENGINE_TRACE
	  m_pLogger->Write("                  | Ok.\n");        
	#endif  

    // El logger se finaliza en ultimo lugar    
    EndLogger();
    
    // Se imposibilita el acceso a CCRISOLEngine desde SYSEngine    
    pCRISOLEngine = NULL;

    // Finalizacion COM
    CoUninitialize();
    
    // Se baja el flag de instancia inicializada
    m_bInitOk = false;    
	m_bExit = true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece vbles del sistema.
// Parametros:
// - HInstance: valor del HANDLE HInstance de Windows
// - HWnd: valor del HANDLE HWND de Windows
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::InitSysVar(const HINSTANCE& hInstance, 
						  const HWND& hWnd)
{
  // Se obtiene el parser
  CCBTEngineParser* const pParser = m_pGameDataBase->GetCBTParser(GameDataBaseDefs::CBTF_CRISOLENGINE_INI,
														          "[SysVar]");
  ASSERT(pParser);

  // Se establecen vbles del sistema  
  pParser->SetVarPrefix("");
  m_SysVarInfo.hInstance = hInstance;
  m_SysVarInfo.hWnd = hWnd;  
  m_SysVarInfo.fIAMSTickTime = 1000.0f / pParser->ReadNumeric("AITicksPerSecond");
  m_SysVarInfo.fIASECTickTime = m_SysVarInfo.fIAMSTickTime / 1000.0f;  
  m_SysVarInfo.udIAMaxLoops = pParser->ReadNumeric("NumMaxLoops");
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el Logger
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::InitLogger(void)
{   
  // Se crea instancia
  ASSERT(!m_pLogger);
  m_pLogger = new CLogger;
  ASSERT(m_pLogger);
  
  // Se inicializa
  if (!m_pLogger->Init("logger.txt", false)) { 
	return false; 
  } 

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el timer
// Parametros:
// - InitData. Parametros de inicializacion
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::InitTimer(void)
{
  // Se crea instancia
  ASSERT(!m_pTimer);
  m_pTimer = new CTimer;
  ASSERT(m_pTimer);

  // Se obtiene el parser
  CCBTEngineParser* const pParser = m_pGameDataBase->GetCBTParser(GameDataBaseDefs::CBTF_CRISOLENGINE_INI,
														          "[Timer]");
  ASSERT(pParser);

  // Se inicializa
  pParser->SetVarPrefix("");
  if (!m_pTimer->Init(pParser->ReadFlag("PerformanceOnlyFlag"))) { 
	return false; 
  }  

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la clase de utilidades matematicas
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::InitMathUtil(void)
{
  // Se crea instancia
  ASSERT(!m_pMathUtil);
  m_pMathUtil = new CMathUtil;
  ASSERT(m_pMathUtil);

  // Se inicializa
  if (!m_pMathUtil->Init()) {
	return false; 
  }  

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa subistema de ficheros.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::InitFileSystem(void)
{
  // Se crea instancia
  ASSERT(!m_pFileSystem);
  m_pFileSystem = CFileSystem::GetInstance();
  ASSERT(m_pFileSystem);

  // Se inicializa
  if (!m_pFileSystem->Init()) { 
	return false; 
  }

  // Todo correcto
  return true;
}  
   
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa subistema de alarmas
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::InitAlarmManager(void)
{
  // Se crea instancia
  ASSERT(!m_pAlarmManager);
  m_pAlarmManager = CAlarmManager::GetInstance();
  ASSERT(m_pAlarmManager);

  // Se inicializa
  if (!m_pAlarmManager->Init()) { 
	return false; 
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el servidor de recursos
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::InitResourceManager(void)
{
  // Se crea instancia
  ASSERT(!m_pResourceManager);
  m_pResourceManager = CResourceManager::GetInstance();
  ASSERT(m_pResourceManager);

  // Se inicializa
  if (!m_pResourceManager->Init()) { 
	return false; 
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el cargador grafico.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::InitGraphicLoader(void)
{
  // Se crea instancia
  ASSERT(!m_pGraphicLoader);
  m_pGraphicLoader = CGraphicLoader::GetInstance();
  ASSERT(m_pGraphicLoader);

  // Se inicializa
  if (!m_pGraphicLoader->Init(0)) { 
	return false; 
  }

  // Todo correcto
  return true;
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el subsistema de fuentes
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CCRISOLEngine::InitFontSystem(void)
{
  // Se crea instancia
  ASSERT(!m_pFontSystem);
  m_pFontSystem = CFontSystem::GetInstance();
  ASSERT(m_pFontSystem);

  // Se obtiene el parser
  CCBTEngineParser* const pParser = m_pGameDataBase->GetCBTParser(GameDataBaseDefs::CBTF_CRISOLENGINE_INI,
														          "[FontSystem]");
  ASSERT(pParser);

  // Se inicializa
  pParser->SetVarPrefix("");
  if (!m_pFontSystem->Init(pParser->ReadString("DefaultFont"))) { 
	return false; 
  }

  // Una vez añadida la fuente por defecto, se añadiran todas las opcionales
  std::string szTmp;
  word uwIt = 0;
  for (; uwIt < 256; ++uwIt) {	
	SYSEngine::PassToString(szTmp, "OptionalFont[%u]", uwIt);
	szTmp = pParser->ReadString(szTmp, false);
	if (pParser->WasLastParseOk()) {
	  m_pFontSystem->AddFont(szTmp);
	} else {
	  break;
	}
  }
  
  // Todo correto
  return true;
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el subsistema grafico
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::InitGraphicSystem(void)
{
  // Se crea instancia
  ASSERT(!m_pGraphicSystem);
  m_pGraphicSystem = CGraphicSystem::GetInstance();
  ASSERT(m_pGraphicSystem);

  // Se obtiene el parser
  CCBTEngineParser* const pParser = m_pGameDataBase->GetCBTParser(GameDataBaseDefs::CBTF_CRISOLENGINE_INI,
														          "[GraphicSystem]");
  ASSERT(pParser);

  // Se inicializa
  pParser->SetVarPrefix("");
  if (!m_pGraphicSystem->Init(pParser->ReadNumeric("GraphicDeviceID"),
							  pParser->ReadNumeric("3DDeviceType"),
							  pParser->ReadNumeric("VideoWidth"),
							  pParser->ReadNumeric("VideoHeight"),
							  pParser->ReadNumeric("VideoBpp"),
							  pParser->ReadNumeric("NumBackBuffers"),
							  pParser->ReadFlag("VSyncFlipFlag"))) { 
	return false; 
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el subsistema de audio.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::InitAudioSystem(void)
{
  // Se crea instancia
  ASSERT(!m_pAudioSystem);
  m_pAudioSystem = CAudioSystem::GetInstance();
  ASSERT(m_pAudioSystem);

  // Se obtiene el parser
  CCBTEngineParser* const pParser = m_pGameDataBase->GetCBTParser(GameDataBaseDefs::CBTF_CRISOLENGINE_INI,
														          "[AudioSystem]");
  ASSERT(pParser);

  // Se inicializa subsistema
  pParser->SetVarPrefix("");
  if (!m_pAudioSystem->Init(pParser->ReadNumeric("Frequency"),
							pParser->ReadNumeric("NumChannels"),
							pParser->ReadNumeric("Bpm"))) {
	return false;
  }

  // Todo correcto
  return true; 
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el subsistema de entrada.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::InitInputManager(void)
{
  // Se crea instancia
  ASSERT(!m_pInputManager);
  m_pInputManager = CInputManager::GetInstance();
  ASSERT(m_pInputManager);
  
  // Area de mov. del raton
  ASSERT(m_pGraphicSystem);
  const sRect rMouseArea(0, m_pGraphicSystem->GetVideoWidth(),
						 0, m_pGraphicSystem->GetVideoHeight());

  // Inicializa el subsistema
  if (!m_pInputManager->Init(rMouseArea)) { 
	return false; 
  }

  // Todo correcto
  return true;
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el subsistema de comandos
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::InitCommandManager(void)
{
  // Se crea instancia
  ASSERT(!m_pCommandManager);
  m_pCommandManager = CCommandManager::GetInstance();
  ASSERT(m_pCommandManager);

  // Se inicializa
  if (!m_pCommandManager->Init()) { 
	return false; 
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la base de datos del juego.
// Parametros:
// - szInitFileName. Nombre del fichero de inicializacion.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - Este sera el unico subsistema que se inicializara sin que desde el
//   exterior se lea la informacion alojada en el fichero .ini, pues es
//   precisamente este modulo el encargado de mantener el parcher a dicho 
//   fichero .ini.
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::InitGameDataBase(const std::string& szInitFileName)
{
  // Se crea instancia
  ASSERT(!m_pGameDataBase);
  m_pGameDataBase = CGameDataBase::GetInstance();
  ASSERT(m_pGameDataBase);

  // Se inicializa
  if (!m_pGameDataBase->Init(szInitFileName)) { 
	return false; 
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el manager del universo de juego.
// Parametros
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::InitWorld(void)
{
  // Se crea instancia
  ASSERT(!m_pWorld);
  m_pWorld = CWorld::GetInstance();
  ASSERT(m_pWorld);

  // Se inicializa 
  if (!m_pWorld->Init()) { 
	return false; 
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el subsistema de interfaz
// Parametros
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::InitGUIManager(void)
{
  // Se crea instancia
  ASSERT(!m_pGUIManager);
  m_pGUIManager = CGUIManager::GetInstance();
  ASSERT(m_pGUIManager);

  // Se inicializa 
  if (!m_pGUIManager->Init()) { 
	return false; 
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el subsistema de combate.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::InitCombatSystem(void)
{
  // Se crea instancia
  ASSERT(!m_pCombatSystem);
  m_pCombatSystem = CCombatSystem::GetInstance();
  ASSERT(m_pCombatSystem);

  // Se inicializa
  if (!m_pCombatSystem->Init()) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el subsistema de GFX.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::InitGFXManager(void)
{
  // Se crea instancia
  ASSERT(!m_pGFXManager);
  m_pGFXManager = CGFXManager::GetInstance();
  ASSERT(m_pGFXManager);

  // Se inicializa
  if (!m_pGFXManager->Init()) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la maquina virtual
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::InitVirtualMachine(void)
{
  // Se crea instancia
  ASSERT(!m_pVirtualMachine);
  m_pVirtualMachine = CVirtualMachine::GetInstance();
  ASSERT(m_pVirtualMachine);

  // Se inicializa
  if (!m_pVirtualMachine->Init()) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza el Logger
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::EndLogger(void)
{   
  // Se destruye instancia
  if (m_pLogger) {
	delete m_pLogger;
	m_pLogger = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza timer
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCRISOLEngine::EndTimer(void)
{
  // Destruye instancia
  if (m_pTimer) {
	delete m_pTimer;
	m_pTimer = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza clase de utilidades matematicas
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::EndMathUtil(void)
{
  // Se destruye instancia
  if (m_pMathUtil) {
	delete m_pMathUtil;
	m_pMathUtil = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza subsistema de ficheros
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::EndFileSystem(void)
{
  // Se destruye instancia
  if (m_pFileSystem) {
	CFileSystem::DestroyInstance();
	m_pFileSystem = NULL;
  }
}  
   
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza subistema de alarmas
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::EndAlarmManager(void)
{  
  // Se destruye instancia
  if (m_pAlarmManager) {
	CAlarmManager::DestroyInstance();
	m_pAlarmManager = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza el servidor de recursos
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::EndResourceManager(void)
{
  // Se destruye instancia
  if (m_pResourceManager) {
	CResourceManager::DestroyInstance();
	m_pResourceManager = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza el cargador grafico.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::EndGraphicLoader(void)
{
  // Se destruye instancia
  if (m_pGraphicLoader) {
	CGraphicLoader::DestroyInstance();
	m_pGraphicLoader = NULL;
  }
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza el subsistema de fuentes
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::EndFontSystem(void)
{
  // Se destruye instancia
  if (m_pFontSystem) {
	CFontSystem::DestroyInstance();
	m_pFontSystem = NULL;
  }
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza el subsistema grafico
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::EndGraphicSystem(void)
{
  // Se destruye instancia
  if (m_pGraphicSystem) {
	CGraphicSystem::DestroyInstance();
	m_pGraphicSystem = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza el subsistema de audio.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::EndAudioSystem(void)
{
  // Se destruye instancia
  if (m_pAudioSystem) {
	CAudioSystem::DestroyInstance();
	m_pAudioSystem = NULL;
  }
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza el subsistema de entrada.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::EndInputManager(void)
{
  // Se destruye instancia
  if (m_pInputManager) {
	CInputManager::DestroyInstance();
	m_pInputManager = NULL;
  }
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza el subsistema de comandos
// Parametros
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::EndCommandManager(void)
{
  // Se destruye instancia
  if (m_pCommandManager) {
	CCommandManager::DestroyInstance();
	m_pCommandManager = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destruye la base de datos del juego
// Parametros
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::EndGameDataBase(void)
{
  // Se destruye instancia
  if (m_pGameDataBase) {
	CGameDataBase::DestroyInstance();
	m_pGameDataBase = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destruye el manager del universo de juego.
// Parametros
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::EndWorld(void)
{
  // Se destruye instancia
  if (m_pWorld) {
	CWorld::DestroyInstance();
	m_pWorld = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza el subsistema de interfaz
// Parametros
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::EndGUIManager(void)
{
  // Se destruye instancia
  if (m_pGUIManager) {
	CGUIManager::DestroyInstance();
	m_pGUIManager = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza el subsistema de combate
// Parametros
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::EndCombatSystem(void)
{
  // Se destruye instancia
  if (m_pCombatSystem) {
	CCombatSystem::DestroyInstance();
	m_pCombatSystem = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza el subsistema de GFX
// Parametros
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::EndGFXManager(void)
{
  // Se destruye instancia
  if (m_pGFXManager) {
	CGFXManager::DestroyInstance();
	m_pGFXManager = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la maquina virtual
// Parametros
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::EndVirtualMachine(void)
{
  // Se destruye instancia
  if (m_pVirtualMachine) {
	CVirtualMachine::DestroyInstance();
	m_pVirtualMachine = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este metodos se llamara desde el exterior y en el se realizara la 
//   coordinacion completa de la actualizacion. Los estados seran encapsulados
//   en clases.
// Parametros:
// Devuelve:
// - Si el motor sigue funcinando con normalidad true. En caso de que se este
//   esperando un abandono, false.
// Notas:
// - Para controlar cuando hay una peticion de abandono, se comprobara la vble
//   m_bExit, que sera activada / desactivada con SetExit
///////////////////////////////////////////////////////////////////////////////
bool 
CCRISOLEngine::UpdateFrame(void)
{
  // SOLO si instancia correctamente inicializada
  ASSERT(IsInitOk());

  // ¿Hay que abandonar?
  if (m_bExit) { 
	return false; 
  }  

  // ¿Hay que poner los FPS?
  if (m_bShowFPS) {
	// Se calculan los frames por segundo
	CalculeFPS();
	
	// Paso un minuto, se establece el nuevo texto
	std::string szText;
	SYSEngine::PassToString(szText, "FPS: %u", m_FPSInfo.udFPS);	
	m_pFontSystem->Write(10, 10, 2, szText, GraphDefs::sRGBColor(255, 0, 0));	
  }
  
  // ¿Hay estado pendiente?  
  if (m_NextState) {
	// Se cambia a ese estado
	ChangeToNextState();
  }

  // Se ejecuta estado actual
  ASSERT(m_pActState);
  m_pActState->Execute();

  // El motor sigue funcionando
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba el valor del estado siguiente y realiza el cambio al estado
//   que proceda.
// Parametros
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCRISOLEngine::ChangeToNextState(void)
{
  // Se finaliza estado actual
  m_pActState->End();

  // Se inicializa el estado
  switch(m_NextState) {
    case GameStateDefs::GS_MAIN_MENU: {			  
  	  // Menu principal
	  m_GameStates.GSMainMenu.Init(this);
	  m_pActState = &m_GameStates.GSMainMenu;	  
	} break;   	  

	case GameStateDefs::GS_CREATE_CHARACTER: {			  
  	  // Creacion de caracter para comenzar a jugar
	  m_GameStates.GSCreateCharacter.Init(this);
	  m_pActState = &m_GameStates.GSCreateCharacter;
	} break;   	  

  	case GameStateDefs::GS_AREA_LOADING: {
	  m_GameStates.GSAreaLoading.Init(this,	
									  m_GameStates.AreaLoadingParams.uwIDArea,
									  m_GameStates.AreaLoadingParams.pPlayer,									  
									  m_GameStates.AreaLoadingParams.PlayerPos);
	  m_pActState = &m_GameStates.GSAreaLoading;	  
    } break;
	  
	case GameStateDefs::GS_INGAME: {
	  // En juego
	  m_GameStates.GSInGame.Init(this);
	  m_pActState = &m_GameStates.GSInGame;	  
    } break;

	default:
	  // Estado no conocido
	  ASSERT(false);
  }; // ~ switch	

  // Se desvincula estado siguiente
  m_NextState = GameStateDefs::GS_NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa los valores asociados a la logica. Estos valores estaran 
//   principalmente relacionados con la actualizacion de tiempos.
// Parametros
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCRISOLEngine::InitLogicValues(void)
{
  // Se inicializan vbles 
  m_SysVarInfo.sqLastTime = m_pTimer->GetTime();
  m_SysVarInfo.sqActTime = m_SysVarInfo.sqLastTime;  
  m_SysVarInfo.udIALoops = 0;
  m_SysVarInfo.fActDelta = 0;
  m_SysVarInfo.fLastDelta = 0;  
  m_SysVarInfo.fTimeElapsed = 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Actualiza la logica. La logica comprendera la obtencion de los valores
//   de tiempo actuales, la ejecucion de los comandos en la cola de comandos
//   en ejecucion y la obtencion de la entrada.
// Parametros
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCRISOLEngine::UpdateLogic(void)
{
  // SOLO si instancia correctamente inicializada
  ASSERT(IsInitOk());

  // Se toman tiempos
  sqword sqTimeTmp = m_pTimer->GetTime();
  m_SysVarInfo.fTimeElapsed = float(sqTimeTmp - m_SysVarInfo.sqActTime);
  m_SysVarInfo.sqActTime = sqTimeTmp;   
  m_SysVarInfo.udIALoops = 0;      
  
  // Se halla el delta actual
  m_SysVarInfo.fActDelta = Min(1.0f, 
							   (m_SysVarInfo.sqActTime - m_SysVarInfo.sqLastTime) / 
							    m_SysVarInfo.fIAMSTickTime); 
  
  // Se ejecutan comandos
  m_pCommandManager->ExecuteCommands(m_SysVarInfo.sqActTime, 
                                     (m_SysVarInfo.fActDelta - m_SysVarInfo.fLastDelta) * m_SysVarInfo.fIASECTickTime);
  
  // Se actualizan alarmas pasando el tiempo transcurrido en segundos
  m_pAlarmManager->Update(m_SysVarInfo.fTimeElapsed / 1000.0f);
    
  // Se ajusta el delta
  m_SysVarInfo.fLastDelta = (1.0f == m_SysVarInfo.fActDelta) ? 0.0f : m_SysVarInfo.fActDelta;
    
  // Bucle de ejecucion de IA  
  while (m_SysVarInfo.sqActTime - m_SysVarInfo.sqLastTime > m_SysVarInfo.fIAMSTickTime && 
         m_SysVarInfo.udIALoops < m_SysVarInfo.udIAMaxLoops) {  
	// Se actualiza la entrada
	m_pInputManager->Update();
	
	// Actualiza subsistema de combate
	m_pCombatSystem->Update();
	
	// Se actualiza IA
	m_pVirtualMachine->Update();

    // Se actualizan tiempos y loops        	
	m_SysVarInfo.sqLastTime += m_SysVarInfo.fIAMSTickTime;	
	++m_SysVarInfo.udIALoops;
  } 

  // Si se ha abandonado el bucle por exceso de loops, se elimina tiempo pendiente  
  if (m_SysVarInfo.udIALoops >= m_SysVarInfo.udIAMaxLoops) {
    m_SysVarInfo.sqLastTime = m_SysVarInfo.sqActTime - m_SysVarInfo.fIAMSTickTime;	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se calculan los frames por segundo.
// Parametros:
// Devuelve:
// - Si ha pasado un minuto true. En caso contrario false.
// Notas:
// - La comprobacion del flag asociado al deseo de mostrar los frames por
//   segundo se debera de evaluar desde el exterior.
///////////////////////////////////////////////////////////////////////////////
bool
CCRISOLEngine::CalculeFPS(void)
{
  // Se hallan los ticks actuales
  m_FPSInfo.udTicks = m_pTimer->GetTime() - m_FPSInfo.udTimeStart;  
  
  // ¿Ha pasado 1 segundo (1000ms)?
  if (m_FPSInfo.udTicks >= 1000) {
    // Si, se halla la relacion de numero de frames en un segundo
    m_FPSInfo.udFPS = m_FPSInfo.udNumFrames * 1000 / m_FPSInfo.udTicks;       
	
	// Se preparan vbles para sig. actualizacion
	m_FPSInfo.udTimeStart = m_pTimer->GetTime(); 
	m_FPSInfo.udNumFrames = 0;

	// Se retorna
	return true;
  } else { 
    // No, se incrementa el numero de frames y retorna
    ++m_FPSInfo.udNumFrames;
	return false;
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe un evento de que se ha pulsado una tecla desde el exterior y 
//   propaga el suceso al InputManager.
// Parametros:
// - sbChar. Codigo de la tecla pulsada
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCRISOLEngine::InputCharEvent(const sbyte sbChar) const
{
  ASSERT(IsInitOk());
  // Traslada la responsabilidad al InputManager
  ASSERT(m_pInputManager);
  m_pInputManager->InputCharEvent(sbChar);
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la operacion de coordinacion en la restauracion de recursos. Este
//   metodo sera llamado desde el exterior cuando haya un cambio de aplicacion
//   se regrese al motor.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCRISOLEngine::Restore(void) 
{
  // Procede a restaurar dispositivos
  m_pInputManager->Restore();             // Restaura dispositivos de entrada  
  m_pGraphicSystem->RestoreSurfaces();    // Se restauran las superficies  
  m_pResourceManager->RestoreResources(); // Se restauran los recursos en sup.  
  m_pFontSystem->RestoreFonts();          // Se reescriben las fuentes
}
