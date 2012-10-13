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
// CGameState.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGameState.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGraphicSystem.h"
#include "CGameState.h"

#include "SYSEngine.h"
#include "CCRISOLEngine.h"
#include "CGUIManager.h"
#include "CWorld.h"
#include "CCombatSystem.h"
#include "CGFXManager.h"
#include "CAudioSystem.h"
#include "CBitmap.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia.
// Parametros:
// - pEngine. Instancia al motor.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGameState::Init(CCRISOLEngine* const pEngine)
{
  // SOLO si parametros validos
  ASSERT(pEngine);

  // ¿Se intenta reinicializar?
  if (IsInitOk()) {
	End();
  }

  // Establece motor
  m_pEngine = pEngine;

  // Todo correcto
  m_bIsInitOk = true;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGameState::End(void)
{
  // Finaliza si procede
  if (IsInitOk()) {
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia / menu principal
// Parametros:
// - pEngine. Instancia al motor.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGSMainMenu::Init(CCRISOLEngine* const pEngine)
{
  // SOLO si parametros validos
  ASSERT(pEngine);
  
  // Inicializa clase base
  if (Inherited::Init(pEngine)) {	
	// Establece GUI asociado a MainMenu
	Inherited::GetEngine()->m_pGUIManager->SetMainMenuWindow();
	
	// Se resetean valores 
	Inherited::GetEngine()->InitLogicValues();				

	// Desactiva muestra de FPS
	SYSEngine::ShowFPS(false);

	// NO permite la ejecucion de scripts
	SYSEngine::GetVirtualMachine()->SetScriptExecution(false);    
  }

  // Retorna resultado
  return Inherited::IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia / menu principal
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGSMainMenu::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {	
	// Propaga finalizancion
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la actualizacion del motor.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGSMainMenu::Execute(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Actualiza logica  
  Inherited::GetEngine()->UpdateLogic();
  // Dibuja GUI  
  Inherited::GetEngine()->m_pGUIManager->Draw();
  // Se dibuja FX / Fades
  Inherited::GetEngine()->m_pGFXManager->DrawFXFade();
  // Render grafico  
  Inherited::GetEngine()->m_pGraphicSystem->Render();
  // Actualiza audio  
  Inherited::GetEngine()->m_pAudioSystem->Update();  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia / creacion de un caracter
// Parametros:
// - pEngine. Instancia al motor.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGSCreateCharacter::Init(CCRISOLEngine* const pEngine)
{
  // SOLO si parametros validos
  ASSERT(pEngine);
  
  // Inicializa clase base
  if (Inherited::Init(pEngine)) {	
	// Establece GUI asociado a MainMenu
	Inherited::GetEngine()->m_pGUIManager->SetCharacterCreatorWindow();	
	
	// Se resetean valores 
	Inherited::GetEngine()->InitLogicValues();			

	// NO permite la ejecucion de scripts
	SYSEngine::GetVirtualMachine()->SetScriptExecution(false);    
	
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CGSCreateCharacter::Init> Establecido estado.\n");
	#endif
  }

  // Retorna resultado
  return Inherited::IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia / menu principal
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGSCreateCharacter::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {	
	// Propaga finalizancion
	Inherited::End();
	#ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("CGSCreateCharacter::Init> Finalizado estado.\n");
	#endif
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la actualizacion del motor.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGSCreateCharacter::Execute(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Actualiza logica  
  Inherited::GetEngine()->UpdateLogic();
  // Dibuja GUI
  Inherited::GetEngine()->m_pGUIManager->Draw();  
  // Se dibuja FX / Fades
  Inherited::GetEngine()->m_pGFXManager->DrawFXFade();
  // Render grafico
  Inherited::GetEngine()->m_pGraphicSystem->Render();
  // Actualiza audio
  Inherited::GetEngine()->m_pAudioSystem->Update();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el estado de juego para cargar una nueva partida al comenzar
//   a jugar desde el principio. En caso de que pPlayer sea NULL, significara
//   que la carga de un nuevo juego se hace desde una partida salvada
// Parametros:
// - pEngine. Instancia al motor.
// - uwIDArea. Identificador del area a cargar.
// - pPlayer. Instancia al jugador.
// - PlayerPos. Posicion del jugador en el area a cargar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGSGameLoading::Init(CCRISOLEngine* const pEngine,
					 const word uwIDArea,			
					 CPlayer* const pPlayer,
					 const AreaDefs::sTilePos& PlayerPos)
{  
  // SOLO si parametros validos
  ASSERT(pEngine);
  
  // Inicializa clase base
  if (Inherited::Init(pEngine)) {	
	// Se establece ventana indicativa de la carga de una area
	Inherited::GetEngine()->m_pGUIManager->SetGameLoadingWindow(uwIDArea,
																pPlayer,
																PlayerPos);

	// NO permite la ejecucion de scripts
	SYSEngine::GetVirtualMachine()->SetScriptExecution(false);    

	// Todo correcto
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CGSGameLoading::Init> Establecido estado.\n");
	#endif
	return true;	
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
///////////////////////////////////////////////////////////////////////////////
void 
CGSGameLoading::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	// Se propaga finalizacion
	Inherited::End();
	#ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("CGSGameLoading::End> Finalizado estado.\n");
	#endif
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la ejecucion del estado.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGSGameLoading::Execute(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Actualiza logica
  Inherited::GetEngine()->UpdateLogic();
  // Dibuja GUI
  Inherited::GetEngine()->m_pGUIManager->Draw();
  // Se dibuja FX / Fades
  Inherited::GetEngine()->m_pGFXManager->DrawFXFade();
  // Render grafico
  Inherited::GetEngine()->m_pGraphicSystem->Render();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el estado de juego.
// Parametros:
// - pEngine. Instancia al motor.
// - szBackImg. Imagen de fondo.
// - szArea. Area a cargar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGSInGame::Init(CCRISOLEngine* const pEngine)
{  
  // SOLO si parametros validos
  ASSERT(pEngine);

  // Inicializa clase base
  if (Inherited::Init(pEngine)) {	
	// Se resetean tiempos para actualizaciones logicas
	Inherited::GetEngine()->InitLogicValues();
	
	// Se activa el MainInterfaz
	Inherited::GetEngine()->m_pGUIManager->SetMainInterfazWindow();	  	

	// SI permite la ejecucion de scripts
	SYSEngine::GetVirtualMachine()->SetScriptExecution(true);    

	// Se avisa a World de que el juego ha comenzado
	Inherited::GetEngine()->m_pWorld->OnStartGame();
	
	// Todo correcto
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CGSInGame::Init> Establecido estado.\n");
	#endif
	return true;	
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
///////////////////////////////////////////////////////////////////////////////
void 
CGSInGame::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	// Se finaliza la sesion de juego actual en el universo	
	Inherited::GetEngine()->m_pWorld->EndSesion();	

	// Se propaga finalizacion
	Inherited::End();	

	#ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("CGSInGame::End> Finalizado estado.\n");
	#endif
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la ejecucion del estado.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGSInGame::Execute(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
    
  // Actualiza logica
  Inherited::GetEngine()->UpdateLogic();

  // Actualiza pantalla
  // Dibuja GUI
  Inherited::GetEngine()->m_pGUIManager->Draw();
  // Dibuja universo de juego
  Inherited::GetEngine()->m_pWorld->Draw();
  // Se dibuja FX / Fades
  Inherited::GetEngine()->m_pGFXManager->DrawFXFade();
  // Render grafico
  Inherited::GetEngine()->m_pGraphicSystem->Render();

  // Actualiza audio
  Inherited::GetEngine()->m_pAudioSystem->Update();  
}
