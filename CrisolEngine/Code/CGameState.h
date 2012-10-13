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
// CCRISOLEngine.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGameState
//   - CGSNull
//   - CGSMainMenu
//   - CGSCreateCharacter
//   - CGSGameLoading
//   - CGSInGame
//
// Descripcion:
// - Aqui se definen todos los estados posibles en los que se podra hallar
//   el motor. Estos estado seran mantenidos en el modulo CCRISOLEngine.
//
// Notas:
// - Los estados de juego actuaran directamente sobre el motor, por lo que
//   no tomaran interfaces a los componentes de estos. En el archivo
//   CGameState.cpp se incluiran las clases de los distintos subsistemas sin
//   tener en cuenta a los interfaces.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGAMESTATE_H_
#define _CGAMESTATE_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"             
#endif
#ifndef _GAMESTATEDEFS_H_
#include "GameStateDefs.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif

// Defincion de clases / estructuras / espacios de nombres
class CCRISOLEngine;
class CPlayer;

// Clase CGameState
class CGameState 
{
private:
  // Vbles de miembro
  CCRISOLEngine* m_pEngine;   // Instancia al motor
  bool           m_bIsInitOk; // ¿Instancia inicializada?

public:
  // Constructor / Destructor
  CGameState(void): m_bIsInitOk(false),
					m_pEngine(NULL) { }
  ~CGameState(void) { End(); }

public:
  // Protocolo de inicio y fin de instancia
  virtual bool Init(CCRISOLEngine* const pEngine);
  virtual void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // Obtencion de la instancia a CRISOL
  inline CCRISOLEngine* const GetEngine(void) const {
	ASSERT(IsInitOk());
	// Retorna instancia al motor
	return m_pEngine;
  }
	
public:
  // Ejecucion del estado
  virtual void Execute(void) = 0;

public:
  // Obtencion de informacion
  virtual inline GameStateDefs::eGameState GetState(void) const = 0;
}; // ~ CGameState

// Clase CGSNull
class CGSNull: public CGameState
{
private:
  // Tipos
  // Clase base
  typedef CGameState Inherited;
  
public:
  // Constructor / Destructor
  CGSNull(void) { }
  ~CGSNull(void) { End(); }

public:
  // Protocolo de inicio y fin de instancia
  bool Init(CCRISOLEngine* const pEngine) {
	return Inherited::Init(pEngine);
  }

public:
  // Ejecucion del estado
  void Execute(void) { }

public:
  // Obtencion de informacion
  inline GameStateDefs::eGameState GetState(void) const {
    ASSERT(Inherited::IsInitOk());
    return GameStateDefs::GS_NULL;
  }
}; // ~ CGameState

// Clase CGSMainMenu
class CGSMainMenu: public CGameState 
{
private:
  // Tipos
  // Clase base
  typedef CGameState Inherited;
  
public:
  // Constructor / Destructor
  CGSMainMenu(void) { }
  ~CGSMainMenu(void) { End(); }

public:
  // Protocolo de inicio y fin de instancia
  bool Init(CCRISOLEngine* const pEngine);
  void End(void);

public:
  // Ejecucion del estado
  void Execute(void);

public:
  // Obtencion de informacion
  inline GameStateDefs::eGameState GetState(void) const {
    ASSERT(Inherited::IsInitOk());
    return GameStateDefs::GS_MAIN_MENU;
  }
}; // ~ CGSMainMenu

// Clase CGSCreateCharacter
class CGSCreateCharacter: public CGameState 
{
private:
  // Tipos
  // Clase base
  typedef CGameState Inherited;
  
public:
  // Constructor / Destructor
  CGSCreateCharacter(void) { }
  ~CGSCreateCharacter(void) { End(); }

public:
  // Protocolo de inicio y fin de instancia
  bool Init(CCRISOLEngine* const pEngine);
  void End(void);

public:
  // Ejecucion del estado
  void Execute(void);

public:
  // Obtencion de informacion
  inline GameStateDefs::eGameState GetState(void) const {
    ASSERT(Inherited::IsInitOk());
    return GameStateDefs::GS_CREATE_CHARACTER;
  }
}; // ~ CGSCreateCharacter

// Clase CGSGameLoading
class CGSGameLoading: public CGameState 
{
private:
  // Tipos
  // Clase base
  typedef CGameState Inherited;

public:
  // Constructor / Destructor
  CGSGameLoading(void) { }
  ~CGSGameLoading(void) { End(); }

public:
  // Protocolo de inicio y fin de instancia
  bool Init(CCRISOLEngine* const pEngine,
			const word uwIDArea,			
			CPlayer* const pPlayer,
			const AreaDefs::sTilePos& PlayerPos);  
  void End(void);

public:
  // Ejecucion del estado
  void Execute(void);

public:
  // Obtencion de informacion
  inline GameStateDefs::eGameState GetState(void) const {
    ASSERT(Inherited::IsInitOk());
    return GameStateDefs::GS_AREA_LOADING;
  }
}; // ~ CGSGameLoading

// Clase CGSInGame
class CGSInGame: public CGameState 
{
private:
  // Tipos
  // Clase base
  typedef CGameState Inherited;
  
public:
  // Constructor / Destructor
  CGSInGame(void) { }
  ~CGSInGame(void) { End(); }

public:
  // Protocolo de inicio y fin de instancia
  bool Init(CCRISOLEngine* const pEngine);
  void End(void);

public:
  // Ejecucion del estado
  void Execute(void);

public:
  // Obtencion de informacion
  inline GameStateDefs::eGameState GetState(void) const {
    ASSERT(Inherited::IsInitOk());
    return GameStateDefs::GS_INGAME;
  }
}; // ~ CGSInGame

#endif