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
// CArea.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CArea
// - CCellIterator
// - CEntitiesIterator
//    * CCriaturesIterator
//    * CSceneObjsIterator
//    * CWallsIterator
//    * CRoofsIterator
//    * CItemsIterator
//
// Descripcion:
// - Representa un area. Desde el area se controlaran todas las celdas
//   que representen el mapa o la zona en la que se halle el jugador.
// - En este mismo fichero se implementa un iterador para recorrer todos los
//   elementos de un tile especifico del area y otro para recorrer el map
//   asociado a un tipo de entidad concreto distinto de CFloor.
// - El area se encargara tambien de gestionar enteramente el trabajo con la
//   informacion de la luz.
// - El area debera de ser un observador para criaturas ya que en el cambio
//   de posicion de las mismas, asi como en el trabajo con items equipados
//   luminosos, debera de ser capaz de recalcular los valores luminosos.
//
// Notas:
// - Cuando una posicion en el mapa de areas sea NULL, querra decir que no
//   existira un tile asociado a dicho area, pero que se contabilizara
//   como si realmente fuera ocupado por un tile. Estos tiles se caracterizaran
//   porque no seran vistos en el mapa. Al no crear instancia se ahorra en
//   en recursos. Por convenio, estos tiles seran infranqueables y no se podra
//   trabajar con ellos.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CAREA_H_
#define _CAREA_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CMEMORYPOOL_H_
#include "CMemoryPool.h"
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _WORLDDEFS_H_
#include "WorldDefs.h"
#endif
#ifndef _CFLOOR_H_
#include "CFloor.h"
#endif
#ifndef _MAP_H_
#include <map>
#define _MAP_H_
#endif
#ifndef _STRING_H_
#include <string>
#define _STRING_H_
#endif
#ifndef _CSCENEOBJ_H
#include "CSceneObj.h"
#endif
#ifndef _CITEM_H_
#include "CItem.h"
#endif
#ifndef _CCRIATURE_H_
#include "CCriature.h"
#endif
#ifndef _CWALL_H_
#include "CWall.h"
#endif
#ifndef _CPLAYER_H_
#include "CPlayer.h"
#endif
#ifndef _CROOF_H_
#include "CRoof.h"
#endif
#ifndef _LIST_H_
#include <list>
#define _LIST_H_
#endif
#ifndef _SYSENGINE_H_
#include "SYSEngine.h"
#endif
#ifndef _ISODEFS_H_
#include "IsoDefs.h"
#endif
#ifndef _ICCRIATUREOBSERVER_H_
#include "iCCriatureObserver.h"
#endif
#ifndef _ICFILESYSTEM_H_
#include "iCFileSystem.h"
#endif
#ifndef _SET_H_
#include <set>
#define _SET_H_
#endif

// Definicion de clases / estructuras / espacios de nombres
struct iCGameDataBase;
struct iCEntitySelector;
struct iCWorld;
class CWorldEntity;
class CSprite;

// Clase CArea
class CArea: public iCCriatureObserver
{
public:
  // Clases amigas
  // Iterador a celda
  friend class CCellIterator;  
  // Iterador a entidades
  friend class CCriatureIterator;
  friend class CItemIterator;
  friend class CWallIterator;
  friend class CSceneObjIterator;
  // Motor isometrico
  friend class CIsoEngine;
  
private:
  // Estructuras forward
  struct sPrevPlayerLightInfo;
  
private:
  // Estructuras 
  struct sNSceneObj {
	// Nodo asociado a un objeto de escenario normal
	CSceneObj SceneObj; // Objeto de escenario
	// Pool de memoria
	static CMemoryPool m_MPool;
	static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
	static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
  };

  struct sNItem {
	// Nodo asociado a un objeto de escenario <normal>
	CItem Item; // Item
	// Pool de memoria
	static CMemoryPool m_MPool;
	static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
	static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
  };

  struct sNCriature {
	// Nodo asociado a una criatura
	CCriature Criature; // Criatura
	// Pool de memoria
	static CMemoryPool m_MPool;
	static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
	static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
  };

  struct sNWall {
	// Nodo asociado a una pared
	CWall Wall; // Pared
	// Pool de memoria  
	static CMemoryPool m_MPool;
	static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
	static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); }   
  };

  struct sNRoof {
	// Nodo asociado a un techo
	CRoof Roof; // Techo
	// Pool de memoria  
	static CMemoryPool m_MPool;
	static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
	static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); }   
  };

private:
  // Tipos
  // Conjunto para el mantenimiento de las celdas asociadas a una habitacion
  typedef std::set<AreaDefs::TileIndex> CellsInRoomSet;
  typedef CellsInRoomSet::iterator      CellsInRoomSetIt;

private:
  // Estructuras
  struct sRoomInfo {
	// Info asociada a un identificador de habitacion
	bool           bIsRoofVisible; // ¿Es el techo de la habitacion visible?
	CellsInRoomSet Cells;          // Celdas que pertenecen a dicha habitacion
	// Constructor
	sRoomInfo(void): bIsRoofVisible(true) { }
  };

private:
  // Tipos de uso durante el trabajo con el area
  // Objetos de escenario  
  typedef std::map<AreaDefs::EntHandle, sNSceneObj*> SObjsMap;
  typedef SObjsMap::value_type                       SObjsValType;
  typedef SObjsMap::iterator                         SObjsIt;
  // Items
  typedef std::map<AreaDefs::EntHandle, sNItem*> ItemsMap;
  typedef ItemsMap::value_type                   ItemsValType;
  typedef ItemsMap::iterator                     ItemsIt;
  // Criaturas  
  typedef std::map<AreaDefs::EntHandle, sNCriature*> CriaturesMap;
  typedef CriaturesMap::value_type                   CriaturesValType;
  typedef CriaturesMap::iterator                     CriaturesIt;
  // Paredes  
  typedef std::map<AreaDefs::EntHandle, sNWall*> WallsMap;
  typedef WallsMap::value_type                   WallsValType;
  typedef WallsMap::iterator                     WallsIt;    
  // Techos
  typedef std::map<AreaDefs::EntHandle, sNRoof*> RoofsMap;
  typedef RoofsMap::iterator                     RoofsMapIt;
  typedef RoofsMap::value_type                   RoofsMapValType;
  // Entidades asociadas a una celda
  typedef std::list<AreaDefs::EntHandle> CellEntitiesList;
  typedef CellEntitiesList::iterator     CellEntitiesListIt;
  // Mapeado de las mascaras de acceso
  typedef std::map<AreaDefs::TileIndex, AreaDefs::MaskTileAccess> MaskTileAccessMap;
  typedef MaskTileAccessMap::iterator                             MaskTileAccessMapIt;
  typedef MaskTileAccessMap::value_type                           MaskTileAccessMapValType;
  // Mapeado de la informacion referida a habitaciones
  typedef std::map<AreaDefs::RoomID, sRoomInfo> RoomInfoMap;
  typedef RoomInfoMap::iterator                 RoomInfoMapIt;
  typedef RoomInfoMap::value_type               RoomInfoMapValType;
  // Mapeado de Roofs (techos) que MUESTRAN lo que se halla por debajo de los
  // mismos siempre que la entidad con la camara NO SE HALLE en su habitacion
  typedef std::set<AreaDefs::EntHandle> ShowUnderRoofsSet;
  typedef ShowUnderRoofsSet::iterator   ShowUnderRoofsSetIt;

private:
  // Estructuras
  struct sNCell {
	// Representa a un tile
	CFloor		        Floor;    // Floor del tile 
	CellEntitiesList    Entities; // Entidades del universo & elementos dibujables SOBRE el floor	
	AreaDefs::EntHandle hRoof;    // Handle al posible techo
	// Constructor por defecto
	sNCell(void): hRoof(0) { }
	// Pool de memoria
	static CMemoryPool m_MPool;
	static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
	static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
  };

  struct sLightFocusInfo {
	// Informacion asociada a un foco de luz
	GraphDefs::Light   LightIntensity; // Intensidad de la luz
	AreaDefs::sTilePos TilePos;        // Pos. del foco en tile
	// Constructor
	sLightFocusInfo(const sLightFocusInfo& LightFocusInfo): LightIntensity(LightFocusInfo.LightIntensity),
															TilePos(LightFocusInfo.TilePos) { }
						
	sLightFocusInfo(const GraphDefs::Light& aLightIntensity,
					const AreaDefs::sTilePos& aTilePos): LightIntensity(aLightIntensity),
														 TilePos(aTilePos) { }
  };

private:
  // Tipos
  // Map para el mantenimiento de la info asociada a los focos
  typedef std::map<AreaDefs::EntHandle, sLightFocusInfo> LightFocusInfoMap;
  typedef LightFocusInfoMap::iterator                    LightFocusInfoMapIt;
  typedef LightFocusInfoMap::value_type                  LightFocusInfoMapValType;
  // Map que representa un vertice de un tile con todos las intensidades de luz
  // que inciden en el mismo, asociadas a los dueños (focos) de las mismas
  typedef std::map<AreaDefs::EntHandle, GraphDefs::Light> VertexTileLightsMap;
  typedef VertexTileLightsMap::iterator                   VertexTileLightMapIt;
  typedef VertexTileLightsMap::value_type                 VertextTileLightMapValType;

private:
  // Estructuras
  struct sLightInTilePosInfo {
	// Informacion referida a la luz reflejada sobre una posicion
	VertexTileLightsMap LightsInVertex[4]; // Luz asociada a cada vertice
	GraphDefs::sLight   TileLight;         // Luz actual de la posicion
	sPosition           WorldPos;          // Posicion universal del tile
	// Constructores
	sLightInTilePosInfo(const sPosition& aWorldPos): WorldPos(aWorldPos) { }
	sLightInTilePosInfo(const sLightInTilePosInfo& LightInTilePosInfo): TileLight(LightInTilePosInfo.TileLight),
																		WorldPos(LightInTilePosInfo.WorldPos) {
	  LightsInVertex[0] = LightInTilePosInfo.LightsInVertex[0];
	  LightsInVertex[1] = LightInTilePosInfo.LightsInVertex[1];
	  LightsInVertex[2] = LightInTilePosInfo.LightsInVertex[2];
	  LightsInVertex[3] = LightInTilePosInfo.LightsInVertex[3];
	}
  };

private:
  // Tipos
  // Map asociado a los tags del area actual
  typedef std::map<std::string, AreaDefs::EntHandle> TagMap;
  typedef TagMap::value_type                         TagMapValType;
  typedef TagMap::iterator                           TagMapIt;
  // Map con todas las posiciones del universo de juego que estan afectadas por luz
  typedef std::map<AreaDefs::TileIndex, sLightInTilePosInfo> LightInTilePosInfoMap;
  typedef LightInTilePosInfoMap::iterator                    LightInTilePosInfoMapIt;
  typedef LightInTilePosInfoMap::value_type					 LightInTilePosInfoMapValType;
  
private:
  // Estructuras
  struct sMapInfo {	
	// Informacion referida al mapa del area
	// Datos
	std::string      szName;       // Nombre del area
	word             uwID;         // ID del area
	GraphDefs::Light AmbientLight; // Valor ambiental de luz
	word			 uwWidth;      // Num. tiles a lo ancho
	word			 uwHeight;     // Num. tiles a lo alto	
	bool             bTmpArea;     // ¿Area temporal?
	sNCell**		 pMap;         // Mapa del area			
	// Map con mascaras de accesos para tiles
	MaskTileAccessMap IndexOfMaskTileAccess; 
	// Entidades
	SObjsMap     SceneObjs;  // Map con los objetos de escenario
	ItemsMap     Items;      // Map con los items
	WallsMap     Walls;      // Map de paredes
	CriaturesMap Criatures;  // Map de criaturas		
	RoofsMap     Roofs;      // Techos
	// Informacion referida a las habitaciones del area / particularidades techos
	RoomInfoMap       RoomInfo;       // Relacion habitaciones / celdas
	ShowUnderRoofsSet ShowUnderRoofs; // Techos que muestran lo que tienen debajo
	// Constructor por defecto
	sMapInfo(void): pMap(NULL), AmbientLight(0) { }
  };

  struct sDinamicLightInfo {
	// Info asociada al trabajo con el sistema de luz dinamica
	LightFocusInfoMap     LightFocus;     // Focos de luz
	LightInTilePosInfoMap LightInTilePos; // Luz asociada a posiciones
	GraphDefs::sLight     CommonLight;    // Luz comun a todos los tiles (0)
  };

  struct sPlayerInfo {
	// Informacion referida al jugador
	CPlayer* pPlayer; // Instancia al jugador	
  };

  struct sFloorSelectorInfo {
	// Info del posible selector asociado a Floor
	iCEntitySelector*  pSelector; // Selector
	AreaDefs::sTilePos TilePos;   // Posicion asociada
  };

private:
  // Instancias a otros modulos
  iCFileSystem*    m_pFileSys;  // Subsistema de ficheros
  iCGameDataBase*  m_pGDBase;   // Base de datos del juego
  iCGraphicSystem* m_pGraphSys; // Subsistema grafico
  iCWorld*         m_pWorld;    // Universo de juego

  // Arrays con datos precalculados para hallar posiciones adyacentes
  sDelta m_EvenDeltas[IsoDefs::MAX_DIRECTIONS]; // Deltas para celda con Y par
  sDelta m_OddDeltas[IsoDefs::MAX_DIRECTIONS];  // Deltas para celda con Y impar

  // Resto de vbles
  sMapInfo			 m_Map;              // Info sobre el mapa asociado al area 
  TagMap             m_EntityTags;       // Tags asociados a las entidades
  sDinamicLightInfo  m_DinamicLightInfo; // Info sobre el sistema dinamico de luz
  sPlayerInfo		 m_PlayerInfo;		 // Info referida al jugador  
  sFloorSelectorInfo m_FloorSelector;    // Datos sobre el selector actual asociado
  bool				 m_bIsInitOk;		 // ¿Clase correctamente inicializada?  
  bool				 m_bIsAreaLoading;	 // ¿Se esta cargando un area?
  bool               m_bIsAreaLoaded;    // ¿Area cargada?
  bool               m_bInFreeAreaMode;  // ¿Se esta liberando el area?
      
public:
  // Constructor / destructor
  CArea(void): m_pGDBase(NULL),
			   m_pGraphSys(NULL),
			   m_pWorld(NULL),
			   m_pFileSys(NULL),
			   m_bIsInitOk(false),
			   m_bIsAreaLoading(false),
			   m_bIsAreaLoaded(false),
			   m_bInFreeAreaMode(false) { }
  ~CArea(void) { 
	End(); 
  }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(void);  
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }  
private:
  // Metodos de apoyo a la inicializacion
  void InitDeltasValues(void);

public:
  // Carga de un area
  bool SetInitArea(const word uwIDArea,
				   CPlayer* const pPlayer,
				   const AreaDefs::sTilePos& PlayerPos);
  bool ChangeArea(const word uwIDArea,
				  const AreaDefs::sTilePos& PlayerPos);
  inline bool IsAreaLoading(void) const { return m_bIsAreaLoading; }
  inline bool IsAreaLoaded(void) const { return m_bIsAreaLoaded; }
private:
  // Metodos de apoyo para la carga de datos
  void FreeAreaInfo(const bool bRemovePlayerInfo);
  void SaveAreaInfo(const bool bIsChangingArea);
  bool SaveEntitiesInfo(const FileDefs::FileHandle& hFile,
					    dword& udOffset,
						CellEntitiesList& Entities,
						const bool bIsChangingArea);
  bool LoadArea(const word uwIDArea);
  void CheckAreaFile(const FileDefs::FileHandle& hAreaFile,
					 dword& udAreaOffset,
					 const byte ubAreaFileType,
					 const word uwIDArea,
					 const std::string& szArea);
  void LoadAreaInfo(const FileDefs::FileHandle& hAreaFile,
					dword& udAreaOffset);	
  void LoadCells(const FileDefs::FileHandle& hAreaFile,
				 dword& udAreaOffset,
			     const AreaDefs::sTilePos& TilePos,
				 const AreaDefs::TileIndex& TileIndex,
				 const bool bTmpAreaFile);
  AreaDefs::MaskTileAccess LoadFloorMaskAccess(const FileDefs::FileHandle& hFile,
											   dword& udOffset);
  void LoadRoofs(const FileDefs::FileHandle& hAreaFile,
				 dword& udAreaOffset,
				 const AreaDefs::TileIndex& TileIndex,
				 const bool bTmpAreaFile);
  void LoadSceneObjs(const FileDefs::FileHandle& hAreaFile,
					 dword& udAreaOffset,
					 const AreaDefs::sTilePos& TilePos,
					 const AreaDefs::TileIndex& TileIndex,
					 const bool bTmpAreaFile);	
  void LoadCriatures(const FileDefs::FileHandle& hAreaFile,
					 dword& udAreaOffset,
					 const AreaDefs::sTilePos& TilePos,
					 const AreaDefs::TileIndex& TileIndex,
					 const bool bTmpAreaFile);
  void LoadWalls(const FileDefs::FileHandle& hAreaFile,
			 	 dword& udAreaOffset,
				 const AreaDefs::sTilePos& TilePos,
				 const AreaDefs::TileIndex& TileIndex,
				 const bool bTmpAreaFile);	  
  void LoadItems(const FileDefs::FileHandle& hAreaFile,
				 dword& udAreaOffset,
				 iCItemContainer* const pContainer,
				 const RulesDefs::eEntityType& EntityType,
				 const AreaDefs::sTilePos& TilePos,
				 const bool bTmpAreaFile);
  void LoadEquippedItems(const FileDefs::FileHandle& hAreaFile,
						 dword& udAreaOffset,
						 CCriature* const pCriature,
						 const bool bTmpAreaFile);
  AreaDefs::EntHandle CreateItem(const std::string& szItemProfile);  
  void LoadEntityTag(const FileDefs::FileHandle& hAreaFile,
					 dword& udAreaOffset,					  
					 const AreaDefs::EntHandle& hEntity);
  void LoadLightValue(const FileDefs::FileHandle& hAreaFile,
					  dword& udAreaOffset,					  
					  const AreaDefs::EntHandle& hEntity);
  void LoadPlayer(const FileDefs::FileHandle& hFile,
				  dword& udOffset);

public:
  // Guarda / carga una partida
  void SaveGame(const FileDefs::FileHandle& hFile,
				dword& udOffset);
  void LoadGame(const FileDefs::FileHandle& hFile,
				dword& udOffset);

public:
  // iCCriatureObserver / Operacion de notificacion
  void CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
							  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
						      const dword udParam = 0);

public:
  // Obtencion de informacion basica del area
  inline word GetIDArea(void) const {
	ASSERT(IsInitOk());
	// Retorna identificador de area
	return m_Map.uwID;
  }
  inline std::string GetAreaName(void) const {
	ASSERT(IsInitOk() && IsAreaLoaded());
	// Retorna el nombre del area
	return m_Map.szName;
  }
  inline GraphDefs::Light GetAmbientLight(void) const {
	ASSERT(IsInitOk() && IsAreaLoaded());
	// Se retorna la luz ambiente
	return m_Map.AmbientLight;
  }
  inline bool IsTempArea(void) const {
	ASSERT(IsInitOk() && IsAreaLoaded());
	// Retorna el flag de area temporal
	return m_Map.bTmpArea;
  }

public:
  // Operaciones de dibujado
  void DrawFloor(const AreaDefs::sTilePos& TilePos,
				 const sword& swXScreen, 
				 const sword& swYScreen);
  void DrawOnFloor(const AreaDefs::sTilePos& TilePos,
				   const sword& swXScreen, 
				   const sword& swYScreen);
private:  
  bool CanDrawWorldEntity(const AreaDefs::EntHandle& hHandle, const byte ubDrawFlags);
  byte GetDrawZone(const AreaDefs::EntHandle& hHandle);

public:
  // Trabajo con la obtencion del tag / handle
  inline std::string GetTagFromHandle(const AreaDefs::EntHandle& hEntity) {
	ASSERT(IsInitOk());
	ASSERT(IsAreaLoaded());  
	ASSERT(hEntity);
	// Obtiene el iterador y retorna tag si lo localiza
	const TagMapIt It(GetMapTagIt(hEntity));
	return (It != m_EntityTags.end()) ? It->first : "";
  }
  inline AreaDefs::EntHandle GetHandleFromTag(const std::string& szTag) {
	ASSERT(IsInitOk());
	ASSERT(IsAreaLoaded());
	// Retorna handle si encuentra tag
	std::string szTagLower(szTag);
	SYSEngine::MakeLowercase(szTagLower);
	const TagMapIt It(m_EntityTags.find(szTagLower));
	return (It != m_EntityTags.end()) ? It->second : 0;
  }
private:
  TagMapIt GetMapTagIt(const AreaDefs::EntHandle& hEntity);
  void InsertTag(const AreaDefs::EntHandle& hEntity,
				 const std::string& szTag);
  inline void RemoveTag(const AreaDefs::EntHandle& hEntity) {
	ASSERT(IsInitOk());
	ASSERT(IsAreaLoaded());  
	ASSERT(hEntity);
	// Obtiene el iterador y lo elimina del map si procede	
	const TagMapIt It(GetMapTagIt(hEntity));
	if (It != m_EntityTags.end()) {
	  m_EntityTags.erase(It);
	}
  }  

public:
  // Trabajo con el sistema dinamico del luz
  void SetLight(const AreaDefs::EntHandle& hEntity,
				const GraphDefs::Light& LightIntensity);
  GraphDefs::Light GetLightAt(const AreaDefs::sTilePos& TilePos); 
  inline GraphDefs::Light GetLight(const AreaDefs::EntHandle& hEntity) {
	ASSERT(IsInitOk() && IsAreaLoaded());
	// Retorna la intensidad de luz asociado a la entidad hEntity
	LightFocusInfoMapIt LightFocusIt(m_DinamicLightInfo.LightFocus.find(hEntity));
	return (LightFocusIt != m_DinamicLightInfo.LightFocus.end()) ? 
		    LightFocusIt->second.LightIntensity : 0;
  }  
private:
  // Metodos de apoyo
  void CArea::RemoveLight(const AreaDefs::EntHandle& hEntity,
						  const bool bUpdatePos);
  void SetLightFocusInArea(const AreaDefs::EntHandle& hEntity,
						   const GraphDefs::Light& LightIntensity,
						   const bool bUpdatePos,
						   const bool bNewFocus);
  bool CanEntityFocusAffectInMap(const AreaDefs::EntHandle& hEntity);
  void CalculeLightFocusRegion(const AreaDefs::sTilePos& FocusTilePos,
							   const GraphDefs::Light& LightIntensity, 
							   AreaDefs::sTilePos& InitRegionTilePos, 
							   AreaDefs::sTilePos& EndRegionTilePos);
  void CalculeAndSetMaxLightInTilePos(const LightInTilePosInfoMapIt& LightInfoIt, 
									  const byte ubVertex);
  inline void CalculeWorldVertexOfTilePos(const LightInTilePosInfoMapIt& LightInfoIt,
										  sPosition* pVertexPos) {
	ASSERT(IsInitOk() && IsAreaLoaded());
	ASSERT((LightInfoIt != m_DinamicLightInfo.LightInTilePos.end()) != 0);
	ASSERT(pVertexPos);
	// Se halla la posicion de cada uno de los vertices del tile en
    // terminos de posicion universal, a partir de la posicion universal del 
	// vertice 0, que se conoce al recibir el iterador al nodo de informacion
	// luminosa de la posicion
	pVertexPos[0] = LightInfoIt->second.WorldPos;
	pVertexPos[1].swXPos = LightInfoIt->second.WorldPos.swXPos + IsoDefs::TILE_WIDTH;
	pVertexPos[1].swYPos = LightInfoIt->second.WorldPos.swYPos;
	pVertexPos[2].swXPos = LightInfoIt->second.WorldPos.swXPos;
	pVertexPos[2].swYPos = LightInfoIt->second.WorldPos.swYPos + IsoDefs::TILE_HEIGHT;
	pVertexPos[3].swXPos = pVertexPos[1].swXPos;
	pVertexPos[3].swYPos = pVertexPos[2].swYPos;
  }
  
public:
  // Trabajo con el selector de Floor
  inline void SetFloorSelector(const AreaDefs::sTilePos& TilePos,
							   iCEntitySelector* const pSelector) {
	ASSERT(IsInitOk()&& IsAreaLoaded());
	ASSERT(pSelector);
	if (IsCellValid(TilePos) &&
	    IsCellWithContent(TilePos)) {	  
	  // Se asocia el selector y su posicion
	  m_FloorSelector.pSelector = pSelector;
	  m_FloorSelector.TilePos = TilePos;
	}
  }
  inline void UnsetFloorSelector(void) {
	ASSERT(IsInitOk() && IsAreaLoaded());
	// Desvincula el selector
	m_FloorSelector.pSelector = NULL;
  }

public:
  // Control de las entidades visibles en un determinado rango
  void FindCriaturesInRangeAt(const AreaDefs::sTilePos& Pos,
							  const RulesDefs::CriatureRange& Range,
							  std::set<AreaDefs::EntHandle>& InRangeSet);
  bool IsCriatureInRangeAt(const AreaDefs::EntHandle& hCriature,
						   const AreaDefs::sTilePos& Pos,
						   const RulesDefs::CriatureRange& Range);
  void FindCriaturesContainingCriatureInRange(const AreaDefs::EntHandle& hCriature,
									          std::set<AreaDefs::EntHandle>& InRangeSet);

private:
  // Operaciones de localizacion de entidades en tiles  
  bool ChangeLocation(const AreaDefs::EntHandle& hEntity,
					  const AreaDefs::sTilePos& NewPos);
  bool ChangeLocation(const AreaDefs::EntHandle& hItem,
					  const AreaDefs::EntHandle& hEntity);
  bool ChangeLocationForVisuals(const AreaDefs::EntHandle& hEntity,
								const AreaDefs::sTilePos& OriginalPos,
								const AreaDefs::sTilePos& VisualPos);
private:
  // Metodos de apoyo  
  void InsertWorldEntityInTile(const AreaDefs::EntHandle& hEntity,
						       const AreaDefs::sTilePos& NewPos);
  void RemoveWorldEntityFromTile(const AreaDefs::EntHandle& hEntity);
  
public:
  // Creacion y destruccion dinamica de entidades
  AreaDefs::EntHandle CreateCriature(const std::string& szProfile,
									 const AreaDefs::sTilePos& TilePos,
									 const std::string& szTag,									 
									 const RulesDefs::Elevation& Elevation,
									 const GraphDefs::Light& Light,
									 const bool bTempCriature);
  AreaDefs::EntHandle CreateItem(const std::string& szProfile,
								 const AreaDefs::sTilePos& TilePos,
								 const std::string& szTag,								 								
								 const GraphDefs::Light& Light);
  AreaDefs::EntHandle CreateItem(const std::string& szProfile,
								 const AreaDefs::EntHandle& hOwner,
								 const std::string& szTag,								 								
								 const GraphDefs::Light& Light);
  AreaDefs::EntHandle CreateWall(const std::string& szProfile,
								 const AreaDefs::sTilePos& TilePos,
								 const std::string& szTag,								 
								 const RulesDefs::Elevation& Elevation,
								 const GraphDefs::Light& Light,
								 const bool bBlockAccessFlag);
  AreaDefs::EntHandle CreateSceneObj(const std::string& szProfile,
									 const AreaDefs::sTilePos& TilePos,
									 const std::string& szTag,									 
									 const RulesDefs::Elevation& Elevation,
									 const GraphDefs::Light& Light);
  void DestroyEntity(const AreaDefs::EntHandle& hEntity);
private:
  // Metodos de apoyo
  CItem* const _CreateItem(const std::string& szProfile);  

public:
  // Operaciones de obtencion de entidades 
  CWorldEntity* const GetWorldEntity(const AreaDefs::EntHandle& hEntity);
  CCriature* const GetCriature(const AreaDefs::EntHandle& hCriature);  
  inline CSceneObj* const GetSceneObj(const AreaDefs::EntHandle& hSceneObj) {	  
	ASSERT(IsInitOk() && IsAreaLoaded());  
	// Localiza y retorna
	const SObjsIt It(m_Map.SceneObjs.find(hSceneObj));  
	return (It != m_Map.SceneObjs.end()) ? &It->second->SceneObj : NULL;
  }
  inline CItem* const GetItem(const AreaDefs::EntHandle& hItem) {	  
	ASSERT(IsInitOk() && IsAreaLoaded());  
	// Se localiza y retorna
	const ItemsIt It(m_Map.Items.find(hItem));  
	return (It != m_Map.Items.end()) ? &It->second->Item : NULL;
  }  
  inline CWall* const GetWall(const AreaDefs::EntHandle& hWall) {
	ASSERT(IsInitOk() && IsAreaLoaded());  
	// Se localiza y retorna
	const WallsIt It(m_Map.Walls.find(hWall));
	return (It != m_Map.Walls.end()) ? &It->second->Wall : NULL;
  }
  inline CFloor* const GetFloor(const AreaDefs::sTilePos& TilePos) {
	ASSERT(IsInitOk() && IsAreaLoaded());
	ASSERT(IsCellValid(TilePos));
	// Localiza y retorna
	const AreaDefs::TileIndex TileIdx = GetTileIdx(TilePos);
	return &m_Map.pMap[TileIdx]->Floor;
  }
  inline CRoof* const GetRoof(const AreaDefs::EntHandle& hRoof) {
	ASSERT(IsInitOk() && IsAreaLoaded());
	// Se localiza y retorna
	const RoofsMapIt It(m_Map.Roofs.find(hRoof));
	return (It != m_Map.Roofs.end()) ? &It->second->Roof : NULL;
  }
  inline CRoof* const GetRoof(const AreaDefs::sTilePos& TilePos) {
	ASSERT(IsInitOk() && IsAreaLoaded());
	ASSERT(IsCellValid(TilePos));
	// Localiza y retorna
	sNCell* const pCell = m_Map.pMap[GetTileIdx(TilePos)];
	if (pCell) {	  
	  if (pCell->hRoof) {
		return GetRoof(pCell->hRoof);
	  }
	} 
	
	// No se encontro
	return NULL;	
  }

public:
  // Operaciones para la obtencion de tipos
  inline RulesDefs::eEntityType GetEntityType(const AreaDefs::EntHandle& hHandle) const {
	ASSERT(IsInitOk() && IsAreaLoaded());
	// El tipo de entidad se halla en los 4 bits superiores	
	return RulesDefs::eEntityType(hHandle >> 12);
  }

private:
  // Creacion de handles
  AreaDefs::EntHandle CreateHandle(const RulesDefs::eEntityType& EntityType);
  
public:
  // Trabajo con la instancia al jugador
  inline CPlayer* const GetPlayer(void) const { 	
	// Retorna instancia
	ASSERT(IsInitOk() && IsAreaLoaded());
	return m_PlayerInfo.pPlayer; 
  }
private:
  // Metodos de apoyo  
  bool SetPlayer(CPlayer* const pPlayer,
				 const AreaDefs::sTilePos& TileDest);
  void UnsetPlayer(void);
  

public:
  // Trabajo con las habitaciones del area
  AreaDefs::RoomID GetRoomAt(const AreaDefs::sTilePos& TilePos);
  void SetRoomRoofVisible(const AreaDefs::RoomID& RoomID,
						  const bool bVisible);
  inline bool IsRoofInRoomVisible(const AreaDefs::RoomID& RoomID) {
	ASSERT(IsInitOk() && IsAreaLoaded());
	// Comprueba si la habitacion con identificador RoomID 
	// tiene, o no, los techos visibles
	const RoomInfoMapIt RoomIt(m_Map.RoomInfo.find(RoomID));
	ASSERT((RoomIt != m_Map.RoomInfo.end()) != 0);
	return RoomIt->second.bIsRoofVisible;
  }

public:
  // Trabajo con la mascara de acceso
  AreaDefs::MaskTileAccess GetMaskTileAccess(const AreaDefs::sTilePos& TilePos,											 
											 const dword udNoEntitiesToCheck = RulesDefs::NO_ENTITY);
  void SetFloorAccess(const AreaDefs::sTilePos& TilePos,
					  const IsoDefs::eDirectionFlag& Orientation,
					  const bool bCanAccess);
private:
  // Metodos de apoyo
  void SetMaskFloorAccess(const AreaDefs::sTilePos& TilePos,
					      const AreaDefs::MaskTileAccess& MaskTileAccess);
  inline AreaDefs::MaskTileAccess GetMaskFloorAccess(const AreaDefs::sTilePos& TilePos) {
	ASSERT(IsInitOk() && IsAreaLoaded());
	// Se retorna la mascara	
	const MaskTileAccessMapIt It(m_Map.IndexOfMaskTileAccess.find(GetTileIdx(TilePos)));  
	return (It != m_Map.IndexOfMaskTileAccess.end()) ? 
		   It->second : AreaDefs::ALL_TILE_ACCESS;
  }
  bool IsAdjWallBlocked(const AreaDefs::sTilePos& TilePosSrc,
					    const IsoDefs::eDirectionIndex& AdjTileDirection,
						const RulesDefs::eWallOrientation& WallOrientation);  
public:
  // Manipulacion de posicion en tiles
  inline bool IsCellValid(const AreaDefs::sTilePos& TilePos) const {		
	ASSERT(IsInitOk());
	// Se retorna flag de validez
	return ((TilePos.XTile < 0 || TilePos.XTile >= m_Map.uwWidth) ||
		   (TilePos.YTile < 0 || TilePos.YTile >= m_Map.uwHeight)) ? 
		   false: true;
  }
  inline bool IsCellWithContent(const AreaDefs::sTilePos& TilePos) const {
	ASSERT(IsInitOk());
	ASSERT(IsCellValid(TilePos));
	// Comprueba si la celda en TilePos tiene contenido asociado o no
	return (NULL != m_Map.pMap[GetTileIdx(TilePos)]);
  }
  inline AreaDefs::TileIndex GetTileIdx(const AreaDefs::sTilePos& TilePos) const {	
	ASSERT((TilePos.XTile >= 0 || TilePos.XTile < m_Map.uwWidth) != 0);
	ASSERT((TilePos.YTile >= 0 || TilePos.YTile < m_Map.uwHeight) != 0);
	// Se retorna posicion
	return (TilePos.YTile * m_Map.uwWidth + TilePos.XTile);
  }
  bool GetAdjacentTilePos(const AreaDefs::sTilePos& TilePosSrc,
						  const IsoDefs::eDirectionIndex& AdjTileDirection,
						  AreaDefs::sTilePos& TilePosDest);
  IsoDefs::eDirectionIndex CalculeDirection(const AreaDefs::sTilePos& TilePosSrc,
											const AreaDefs::sTilePos& TilePosDest);

public:
  // Obtencion del array de deltas pertinentes seguna la poscion del tile
  inline sDelta* const GetDeltasValues(const AreaDefs::sTilePos& TilePos) {
	ASSERT(IsInitOk() && IsAreaLoaded());
	// Retorna el array segun si la posicion es par o impar
	return SYSEngine::GetMathUtil()->IsEven(TilePos.YTile) ? m_EvenDeltas : m_OddDeltas;
  }

public:
  // Obtencion de info general sobre el mapa
  inline word GetWidth(void) const { 
	ASSERT(IsInitOk() && IsAreaLoaded());
	// Se retorna ancho
	return m_Map.uwWidth; 
  }
  inline word GetHeight(void) const { 
	ASSERT(IsInitOk() && IsAreaLoaded());
	// Se retorna alto
	return m_Map.uwHeight; 
  }  
};

// Clase CCellIterator
class CCellIterator
{
private:
  // Tipos
  // Lista de entidades
  typedef std::list<AreaDefs::EntHandle> EntityList; 
  typedef EntityList::iterator           ItEntityList;

private:
  // Vbles de miembro 
  EntityList*  m_pList;     // Lista a los handles del tile concreto
  ItEntityList m_It;        // Iterador a la lista de handles  
  CArea*       m_pArea;     // Area a la que esta asociado
  word         m_uwPos;     // Controla la posicion y al iterador	  
  bool         m_bIsInitOk; // ¿Iterador inicializado bien?
	  
public:
  // Constructor / destructor
  CCellIterator(void): m_bIsInitOk(false),
					   m_pArea(NULL),
					   m_pList(NULL),
					   m_uwPos(0) { }
  ~CCellIterator(void) { End(); }
	  
public:
  // Protocolos de inicializacion / finalizacion
  bool Init(CArea* const pArea,
			const AreaDefs::sTilePos& TilePos);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }
	  
public:
  // Localizacion del iterador
  inline void SetAtFront(void) { 
	ASSERT(IsInitOk());
	// Establece iterador al frente
	m_It = m_pList->begin(); 
	if (m_It == m_pList->end()) { m_uwPos = 0; }
	else { m_uwPos = 1; }
  }
  
  inline void SetAtBack(void) {  
	ASSERT(IsInitOk());
	// Establece iterador al final
	m_uwPos = m_pList->size();
	if (m_uwPos) { 
	  m_It = m_pList->end();
	  --m_It; 		  
	} else { 
	  ++m_uwPos; 
	}
  }
	  
public:
  // Navegacion
  inline void Next(void) { 
	ASSERT(IsInitOk());
	// Sig elemento
	if (m_uwPos <= m_pList->size()) {
	  m_uwPos++;
	  ++m_It;
	}
  }  
  inline void Prev(void) { 
	ASSERT(IsInitOk());
	// Elemento anterior
	if (m_uwPos >= 1) {
	  --m_uwPos;
	  --m_It;
	}
  }
	  
public:
  // Obtencion de informacion
  inline AreaDefs::EntHandle GetWorldEntity(void) const {	
	ASSERT(IsInitOk());
	// Devuelve entidad del universo de juego
	return IsItValid() ? *m_It : 0;
  }
  inline word Size(void) const { 
	ASSERT(IsInitOk());
	// Obtiene tamaño
	return m_pList->size(); 
  }
  inline bool IsItValid(void) const { 
	return (0 == m_uwPos || m_uwPos > m_pList->size()) ? false: true;
  }
}; // ~ CTileIterator

// Clase CEntitiesIterator
class CEntitiesIterator
{
private:
  // Vbles de miembro 
  CArea* m_pArea;     // Area a la que esta asociado
  bool   m_bIsInitOk; // ¿Iterador inicializado bien?
	  
public:
  // Constructor / destructor
  CEntitiesIterator(void): m_bIsInitOk(false),
					       m_pArea(NULL) { }
  virtual ~CEntitiesIterator(void) { End(); }
	  
public:
  // Protocolos de inicializacion / finalizacion
  bool Init(CArea* const pArea);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }

protected:
  // Desvincula puntero
  virtual void ReleaseIt(void) = 0;
	  
public:
  // Localizacion del iterador
  virtual void SetAtFront(void) = 0;
  	  
public:
  // Navegacion
  virtual void Next(void) = 0;
	  
public:
  // Obtencion de informacion
  virtual bool IsItValid(void) const = 0;
   
protected:
  // Obtencion de instancia al area
  inline CArea* const GetArea(void) const {
	ASSERT(IsInitOk());
	// Retorna instancia
	return m_pArea;
  }
}; // ~ CEntitiesIterator

// Clase CCriatureIterator
class CCriatureIterator: public CEntitiesIterator
{
private:
  // Vbles de miembro 
  CArea::CriaturesIt m_It; // Iterador
  
public:
  // Constructor / destructor
  CCriatureIterator(void) { }
  ~CCriatureIterator(void) { End(); }
	  
protected:
  // Desvincula puntero
  void ReleaseIt(void) {
	ASSERT(CEntitiesIterator::IsInitOk());
	m_It = CEntitiesIterator::GetArea()->m_Map.Criatures.end();
  }    
	  
public:
  // Localizacion del iterador
  void SetAtFront(void) {	
	ASSERT(CEntitiesIterator::IsInitOk());
	// Establece iterador
	m_It = CEntitiesIterator::GetArea()->m_Map.Criatures.begin();
  }
  	  
public:
  // Navegacion
   void Next(void) {
	 ASSERT(CEntitiesIterator::IsInitOk());
	 // Avanza
	 ++m_It;
   }
	  
public:
  // Obtencion de informacion
  CCriature* const GetEntity(void) const {
	ASSERT(CEntitiesIterator::IsInitOk());
	// Retorna instancia
	return IsItValid() ? &m_It->second->Criature : NULL;	
  }
  bool IsItValid(void) const {
	ASSERT(CEntitiesIterator::IsInitOk());
	// Retorna flag
	return (m_It != CEntitiesIterator::GetArea()->m_Map.Criatures.end());
  }
}; // ~ CCriatureIterator

// Clase CItemIterator
class CItemIterator: public CEntitiesIterator
{
private:
  // Vbles de miembro 
  CArea::ItemsIt m_It; // Iterador
  
public:
  // Constructor / destructor
  CItemIterator(void) { }
  ~CItemIterator(void) { End(); }
	  
protected:
  // Desvincula puntero
  void ReleaseIt(void) {
	ASSERT(CEntitiesIterator::IsInitOk());
	m_It = CEntitiesIterator::GetArea()->m_Map.Items.end();
  }    
	  
public:
  // Localizacion del iterador
  void SetAtFront(void) {	
	ASSERT(CEntitiesIterator::IsInitOk());
	// Establece iterador
	m_It = CEntitiesIterator::GetArea()->m_Map.Items.begin();
  }
  	  
public:
  // Navegacion
   void Next(void) {
	 ASSERT(CEntitiesIterator::IsInitOk());
	 // Avanza
	 ++m_It;
   }
	  
public:
  // Obtencion de informacion
  CItem* const GetEntity(void) const {
	ASSERT(CEntitiesIterator::IsInitOk());
	// Retorna instancia
	return IsItValid() ? &m_It->second->Item : NULL;	
  }
  bool IsItValid(void) const {
	ASSERT(CEntitiesIterator::IsInitOk());
	// Retorna flag
	return (m_It != CEntitiesIterator::GetArea()->m_Map.Items.end());
  }
}; // ~ CItemIterator

// Clase CWallIterator
class CWallIterator: public CEntitiesIterator
{
private:
  // Vbles de miembro 
  CArea::WallsIt m_It; // Iterador
  
public:
  // Constructor / destructor
  CWallIterator(void) { }
  ~CWallIterator(void) { End(); }
	  
protected:
  // Desvincula puntero
  void ReleaseIt(void) {
	ASSERT(CEntitiesIterator::IsInitOk());
	m_It = CEntitiesIterator::GetArea()->m_Map.Walls.end();
  }    
	  
public:
  // Localizacion del iterador
  void SetAtFront(void) {	
	ASSERT(CEntitiesIterator::IsInitOk());
	// Establece iterador
	m_It = CEntitiesIterator::GetArea()->m_Map.Walls.begin();
  }
  	  
public:
  // Navegacion
   void Next(void) {
	 ASSERT(CEntitiesIterator::IsInitOk());
	 // Avanza
	 ++m_It;
   }
	  
public:
  // Obtencion de informacion
  CWall* const GetEntity(void) const {
	ASSERT(CEntitiesIterator::IsInitOk());
	// Retorna instancia
	return IsItValid() ? &m_It->second->Wall : NULL;	
  }
  bool IsItValid(void) const {
	ASSERT(CEntitiesIterator::IsInitOk());
	// Retorna flag
	return (m_It != CEntitiesIterator::GetArea()->m_Map.Walls.end());
  }
}; // ~ CWallIterator

// Clase CSceneObjIterator
class CSceneObjIterator: public CEntitiesIterator
{
private:
  // Vbles de miembro 
  CArea::SObjsIt m_It; // Iterador
  
public:
  // Constructor / destructor
  CSceneObjIterator(void) { }
  ~CSceneObjIterator(void) { End(); }
	  
protected:
  // Desvincula puntero
  void ReleaseIt(void) {
	ASSERT(CEntitiesIterator::IsInitOk());
	m_It = CEntitiesIterator::GetArea()->m_Map.SceneObjs.end();
  }    
	  
public:
  // Localizacion del iterador
  void SetAtFront(void) {	
	ASSERT(CEntitiesIterator::IsInitOk());
	// Establece iterador
	m_It = CEntitiesIterator::GetArea()->m_Map.SceneObjs.begin();
  }
  	  
public:
  // Navegacion
   void Next(void) {
	 ASSERT(CEntitiesIterator::IsInitOk());
	 // Avanza
	 ++m_It;
   }
	  
public:
  // Obtencion de informacion
  CSceneObj* const GetEntity(void) const {
	ASSERT(CEntitiesIterator::IsInitOk());
	// Retorna instancia
	return IsItValid() ? &m_It->second->SceneObj : NULL;	
  }
  bool IsItValid(void) const {
	ASSERT(CEntitiesIterator::IsInitOk());
	// Retorna flag
	return (m_It != CEntitiesIterator::GetArea()->m_Map.SceneObjs.end());
  }
}; // ~ CSceneObjIterator

#endif // ~ #ifdef _CAREA_H_

