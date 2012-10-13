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
// CMoveCmd.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CMoveCmd
//
// Descripcion:
// - Comando que coordina el movimiento de una entidad. El movimiento sera un
//   comando fisico que se actualizara por interpolacion.
// - El cambio de tile en el movimiento dependera de la direccion. Es necesario
//   trabajar asi ya que pueden producirse incoherencias graficas. Por ejemplo,
//   si una criatura se desplaza hacia abajo y NO se cambia de tile desde el
//   comienzo, sera "machacado" por el mismo suelo del tile al que se dirige.
//   Pasara justamente lo contrario cuando se mueva hacia el norte.
//
// Notas:
// - En el caso de que el modo combate este activo, el comando debera de 
//   controlar que la accion de movimiento sea posible notificando a los
//   observers de la criatura de tal posibilidad o imposibilidad. Para ello,
//   se utilizara el privilegio de ser clase amiga de CCriature.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CMOVECMD_H_
#define _CMOVECMD_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CCOMMAND_H_
#include "CCommand.h"
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _ISODEFS_H_
#include "IsoDefs.h"
#endif
#ifndef _SYSENGINE_H_
#include "SYSEngine.h"
#endif
#ifndef _ANIMTEMPLATEDEFS_H_
#include "AnimTemplateDefs.h"
#endif
#ifndef _CLINEARINTERPOLATION_H_
#include "CLinearInterpolation.h"
#endif
#ifndef _ICAUDIOSYSTEM_H_
#include "iCAudioSystem.h"
#endif

// Definicion de clases / estructuras / espacios de nombres
class CCriature;
class CPath;

// Clase CMoveCmd
class CMoveCmd: public CCommand
{
private:
  // Tipos
  typedef CCommand Inherited;

private:
  // Estructuras  
  struct sExecuteInfo {
	// Informacion relativa al comando en ejecucion	
	bool                     bXMovComplete; // ¿Movimiento en X completo?
	bool                     bYMovComplete; // ¿Movimiento en Y completo? 
	bool                     bNextMove;     // ¿Se prepara sig. movimiento?
	IsoDefs::eDirectionIndex ActDirection;  // Direccion actual de movimiento	
	ResDefs::WAVSoundHandle  hWAVWalkSound; // Sonido WAV asociado a pasos
  };

  struct sCmdInfo {
	// Informacion relativa al comando		
	CCriature*					pCriature;  // Criatura a mover
	CPath*						pPath;      // Path
	AreaDefs::sTilePos			TileDest;   // Tile de destino
	AnimTemplateDefs::AnimState AnimState;  // Estado de animacion original
	bool                        bGhostMode; // ¿Modo fantasma?
  };

  struct sPendingMove {
	// Informacion relativa a movimiento pendiente	
	AnimTemplateDefs::AnimState AnimState;     // Estado de animacion a usar en el movimiento
	AreaDefs::sTilePos			TileDest;      // Tile de destino
	word						uwDistanceMin; // Distancia minima
	bool                        bGhostMode;    // Flag de mov. en modo fantasma
  };

private:
  // Resto de vbles de miembro
  CLinearInterpolation m_XInterpolation; // Control de la interpolacion en X
  CLinearInterpolation m_YInterpolation; // Control de la interpolacion en Y
  sCmdInfo			   m_CmdInfo;	     // Informacion relativa al comando
  sExecuteInfo         m_ExInfo;		 // Info del comando en ejecucion  
  sPendingMove         m_PendingMove;    // Movimiento pendiente
  bool                 m_bIsPending;     // ¿Hay camino pendiente?
  bool                 m_bStopWalk;      // ¿Orden de detencion?

public:
  // Constructor / destructor
  CMoveCmd(void) { }
  ~CMoveCmd(void) { End(); }

public:
  // Protocolos de inicializacion / fin
  bool Init(CCriature* const pCriature,
			const AreaDefs::sTilePos& TileDest,
			const byte ubAnimState,
			const word uwDistanceMin = 0,
			const bool bGhostMode = false);
  void End(void);  
 
public:
  // CCommand / Ejecucion
  void Execute(const float fDelta);    

private:
  // Metodos de apoyo a la inicializacion
  bool StartCmd(const AreaDefs::sTilePos& TileDest,
				const byte ubAnimState,
				const word uwDistanceMin,
				const bool bGhostMode);

private:
  // Metodos de apoyo al movimiento
  void SetNextMovement(void);
  void UpdateMovement(const float fDelta);
  void MoveRight(const float fDelta);
  void MoveLeft(const float fDelta);
  void MoveUp(const float fDelta);
  void MoveDown(const float fDelta);  
  bool CanDoMovementInTurnMode(void);
  inline MustChangeAtStart(const IsoDefs::eDirectionIndex& Direction) const {
	ASSERT(IsActive());
	// Se comprueba si la direccion es sensible a un cambio al comienzo
	return (Direction == IsoDefs::SOUTH_INDEX ||
		    Direction == IsoDefs::SOUTHEAST_INDEX ||
			Direction == IsoDefs::SOUTHWEST_INDEX);
  }

private:
  // Metodos para el trabajo con la reproduccion del sonido de los pasos
  void ReleaseWAVWalkSound(void);
  void PlayWAVWalkSound(const AreaDefs::sTilePos& TilePos);	  

public:
  // CCommand / Trabajo con el modo pausa
  void SetPause(const bool bPause);

public:
  // Detencion de caminar
  void StopWalk(const bool bNow = false);
};

#endif // ~ CMoveCmd

