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
// iCWorld.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCWorld
//
// Descripcion:
// - Clase interfaz para CWorld.
// - Desde esta interfaz se podra acceder al motor isometrico y a los elementos
//   asociados al area de juego actual. CWorl actuara de "facade" con respecto
//   a esos dos elementos.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICWORLD_H_
#define _ICWORLD_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _WORLDDEFS_H_
#include "WorldDefs.h"
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _ISODEFS_H_
#include "IsoDefs.h"
#endif
#ifndef _RULESDEFS_H_
#include "RulesDefs.h"
#endif
#ifndef _COMMANDDEFS_H_
#include "CommandDefs.h"
#endif
#ifndef _SET_H_
#define _SET_H_
#include <set>
#endif

// Defincion de clases / estructuras / espacios de nombres
struct iCIsoEngine;
struct iCWorldObserver;
class CWorldEntity;
class CSceneObj;
class CItem;
class CCriature;
class CPlayer;
class CWall;
class CFloor;
class CPath;

// Defincion de clases / estructuras / espacios de nombres
struct iCCommandClient;

// Clase iCWorld
struct iCWorld
{
private:
  // Clases amigas
  friend class CGUIWGameLoading;
  friend class CGUIWMainInterfaz;

public:
  // Trabajo con el establecimiento de modos temporales en el universo de juego
  virtual void SetMode(const WorldDefs::eWorldMode& Mode) = 0;
  virtual WorldDefs::eWorldMode GetMode(void) const = 0;

private:
  // Inicio del universo de juego / carga de areas
  // Nota: estos metodos estaran reservados a las clases amigas
  virtual bool StartWorld(const word uwIDArea,
						  CPlayer* const pPlayer,
						  const AreaDefs::sTilePos& PlayerPos) = 0;
  virtual bool ChangeArea(const word uwIDArea,
						  const AreaDefs::sTilePos& PlayerPos) = 0;

public:
  // Guarda / carga una partida
  virtual void SaveGame(const word uwIDSaveSlot,
						const std::string& szSaveDesc) = 0;
  virtual void LoadGame(const word uwIDLoadSlot) = 0;
  virtual bool IsLoadingASavedGame(void) const = 0;

public:
  // Trabajo con el establecimiento de los distintos eventos
  virtual void SetScriptEvent(const RulesDefs::eScriptEvents& Event,
					          const std::string& szFileName) = 0;
  virtual std::string GetScriptEventFile(const RulesDefs::eScriptEvents& Event) = 0;

public:
  // Operaciones sobre el sonido ambiente
  virtual bool SetAmbientWAVSound(const std::string& szSound) = 0;
  virtual bool IsAmbientWAVSoundActive(void) const = 0;
  virtual std::string GetAmbientWAVSound(void) const = 0;

public:
  // Operaciones sobre los sonidos anonimos
  virtual bool PlayWAVSound(const std::string& szSound) = 0;

public:
  // Flag de permiso de guardar partidas
  virtual void SetSavePermission(const bool bPermission) = 0;
  virtual bool GetSavePermission(void) const = 0;
  virtual bool CanSaveGame(void) = 0;

public:
  // Trabajo con el jugador
  virtual void MovPlayerToSelectCell(void) const = 0;
  virtual CPlayer* const GetPlayer(void) = 0;

public:
  // Trabajo con los valores horarios
  virtual void SetWorldTimePause(const bool bPause) = 0;
  virtual bool IsWorldTimeInPause(void) = 0;
  virtual byte GetHour(void) const = 0;
  virtual byte GetMinute(void) const = 0;
  virtual void SetHour(byte ubHour) = 0;
  virtual void SetMinute(byte ubMinute) = 0;

public:
  // Trabajo con los observadores
  virtual void AddObserver(iCWorldObserver* const pObserver) = 0;
  virtual void RemoveObserver(iCWorldObserver* const pObserver) = 0;

public:
  // Pausa
  virtual void SetPause(const bool bPause) = 0;
  virtual bool IsInPause(void) const = 0;

public:
  // Trabajo con el evento idle
  virtual void SetIdleEventTime(const float fTime) = 0;

public:
  // Creacion y destruccion dinamica de entidades
  virtual AreaDefs::EntHandle CreateCriature(const std::string& szProfile,
											 const AreaDefs::sTilePos& TilePos,
											 const std::string& szTag,											 
											 const RulesDefs::Elevation& Elevation,
											 const GraphDefs::Light& Light,
											 const bool bTempCriature) = 0;
  virtual AreaDefs::EntHandle CreateItem(const std::string& szProfile,
	                                     const AreaDefs::sTilePos& TilePos,
										 const std::string& szTag,										 								
										 const GraphDefs::Light& Light) = 0;
  virtual AreaDefs::EntHandle CreateItem(const std::string& szProfile,
	                                     const AreaDefs::EntHandle& hOwner,
										 const std::string& szTag,										 								
										 const GraphDefs::Light& Light) = 0;
  virtual AreaDefs::EntHandle CreateWall(const std::string& szProfile,
	                                     const AreaDefs::sTilePos& TilePos,
										 const std::string& szTag,										 
										 const RulesDefs::Elevation& Elevation,
										 const GraphDefs::Light& Light,
										 const bool bBlockAccessFlag) = 0;
  virtual AreaDefs::EntHandle CreateSceneObj(const std::string& szProfile,
	                                         const AreaDefs::sTilePos& TilePos,
											 const std::string& szTag,											 
											 const RulesDefs::Elevation& Elevation,
											 const GraphDefs::Light& Light) = 0;  
  virtual bool DestroyEntity(const AreaDefs::EntHandle& hEntity) = 0;

public:
  // Trabajo con el area de juego
  virtual std::string GetAreaName(void) const = 0;
  virtual word GetIDArea(void) const = 0;
  virtual word GetAreaWidth(void) const = 0;
  virtual word GetAreaHeight(void) const = 0;
  virtual RulesDefs::eEntityType GetEntityType(const AreaDefs::EntHandle& hEntity) const = 0;
  virtual AreaDefs::EntHandle GetHandleFromTag(const std::string& szTag) = 0;
  virtual std::string GetTagFromHandle(const AreaDefs::EntHandle& hEntity) = 0;
  virtual CWorldEntity* const GetWorldEntity(const AreaDefs::EntHandle& hEntity) = 0;
  virtual CSceneObj* const GetSceneObj(const AreaDefs::EntHandle& hItem) = 0;
  virtual CItem* const GetItem(const AreaDefs::EntHandle& hItem) = 0;
  virtual CCriature* const GetCriature(const AreaDefs::EntHandle& hCriature) = 0;
  virtual CWall* const GetWall(const AreaDefs::EntHandle& hWall) = 0;  
  virtual CFloor* const GetFloor(const AreaDefs::sTilePos& TilePos) = 0;
  virtual IsoDefs::eDirectionIndex CalculeDirection(const AreaDefs::sTilePos& TilePosSrc,
												    const AreaDefs::sTilePos& TilePosDest) = 0;
  virtual void SetLight(const AreaDefs::EntHandle& hEntity,
						const GraphDefs::Light& LightIntensity) = 0;
  virtual GraphDefs::Light GetLight(const AreaDefs::EntHandle& hEntity) = 0;
  virtual GraphDefs::Light GetLightAt(const AreaDefs::sTilePos& TilePos) = 0;
  virtual bool IsFloorValid(const AreaDefs::sTilePos& TilePos) const = 0;
  virtual word GetNumItemsAt(const AreaDefs::sTilePos& TilePos) = 0;
  virtual AreaDefs::EntHandle GetItemAt(const AreaDefs::sTilePos& TilePos,
									    const word uwLocation) = 0;
  virtual GraphDefs::Light GetAmbientLight(void) const = 0;
  virtual AreaDefs::MaskTileAccess GetMaskTileAccess(const AreaDefs::sTilePos& TilePos) = 0;
  virtual void FindCriaturesInRangeAt(const AreaDefs::sTilePos& Pos,
									  const RulesDefs::CriatureRange& Range,
									  std::set<AreaDefs::EntHandle>& InRangeSet) = 0;
  virtual bool IsCriatureInRangeAt(const AreaDefs::EntHandle& hCriature,
								   const AreaDefs::sTilePos& Pos,
								   const RulesDefs::CriatureRange& Range) = 0;
  virtual void FindCriaturesContainingCriatureInRange(const AreaDefs::EntHandle& hCriature,
													  std::set<AreaDefs::EntHandle>& InRangeSet) = 0;

public:
  // Trabajo con motor isometrico
  virtual void AttachCamera(const AreaDefs::EntHandle& hEntity, 
							const word uwSpeed = 0,
							iCCommandClient* const pClient = NULL,
							const CommandDefs::IDCommand& IDCommand = 0,
							const dword udExtraParam = 0) = 0;
  virtual void AttachCamera(const AreaDefs::sTilePos& TilePos,
							const word uwSpeed = 0,
							iCCommandClient* const pClient = NULL,
							const CommandDefs::IDCommand& IDCommand = 0,
							const dword udExtraParam = 0) = 0;    
  virtual void AttachCamera(const float fXWorldPos, 
							const float fYWorldPos,
						    const word uwSpeed = 0,
							iCCommandClient* const pClient = NULL,
							const CommandDefs::IDCommand& IDCommand = 0,
							const dword udExtraParam = 0) = 0;
  virtual void CenterCamera(void) = 0;
  virtual IsoDefs::eSelectCellType SelectCell(const sword& swXScreen,
											  const sword& swYScreen) = 0;
  virtual void SetDrawSelectCell(const bool bDraw) = 0;
  virtual AreaDefs::sTilePos GetSelectCellPos(void) const = 0;
  virtual CWorldEntity* const GetEntityAt(const sword swXScreen,
									      const sword swYScreen) = 0;
  virtual sRect GetVisibleWorldArea(void) const = 0;  
  virtual bool IsDrawAreaActive(void) const = 0;
  virtual void SetDrawArea(const bool bDraw) = 0;
  virtual bool IsTilePosVisible(const AreaDefs::sTilePos& TilePos) const = 0;
  virtual bool GetEntityScreenPos(CWorldEntity* const pEntity, 
								  sword& swXScreen, 
								  sword& swYScreen) = 0;
  virtual void SetDrawWindow(const word uwXTop, 
							 const word uwYTop) = 0;
  virtual float GetXWorldPos(void) const = 0;
  virtual float GetYWorldPos(void) const = 0;
  virtual bool MoveRight(const float fOffset) = 0;
  virtual bool MoveLeft(const float fOffset) = 0;
  virtual bool MoveNorth(const float fOffset) = 0;
  virtual bool MoveSouth(const float fOffset) = 0;  
  virtual void MoveTo(const IsoDefs::eDirectionIndex& Direction) = 0;
  virtual bool MoveEntity(const AreaDefs::EntHandle& hEntity,
						  const IsoDefs::eDirectionIndex& Dir, 
						  const bool bForce) = 0;
  virtual CPath* FindPath(const AreaDefs::sTilePos& TileSrc,
						  const AreaDefs::sTilePos& TileDest,
						  const word uwDistanceMin = 0,
						  const bool bGhostMode = false) = 0;  
  virtual sword CalculePathLenght(const AreaDefs::sTilePos& TileSrc,
								  const AreaDefs::sTilePos& TileDest) = 0;
  virtual sword CalculeAdjacentPosInDestination(const AreaDefs::sTilePos& TileSrc,
											    const AreaDefs::sTilePos& TileDest,
												AreaDefs::sTilePos& AdjacentTilePos) = 0;
  virtual sword CalculeAbsoluteDistance(const AreaDefs::sTilePos& TileSrc,
									    const AreaDefs::sTilePos& TileDest) = 0;
  virtual bool IsAdjacentTo(const AreaDefs::sTilePos& TileSrc,
						    const AreaDefs::sTilePos& TileDest) = 0;  
  virtual sPosition PassTilePosToWorldPos(const AreaDefs::sTilePos& TilePos) = 0;
  virtual bool ChangeLocation(const AreaDefs::EntHandle& hEntity,
							  const AreaDefs::sTilePos& NewPos) = 0;
  virtual bool ChangeLocation(const AreaDefs::EntHandle& hItem,
							  const AreaDefs::EntHandle& hEntity) = 0;
  virtual void ChangeLocationForVisuals(const AreaDefs::EntHandle& hEntity,
										const AreaDefs::sTilePos& OriginalPos,
										const AreaDefs::sTilePos& VisualPos) = 0;
};

#endif // ~ #ifdef _ICWORLD_H_
