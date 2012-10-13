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
// CIsoEngine.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CIsoEngine
//    * CPathFinder
//    * CDrawArea
//       * CDrawAreaFloor
//       * CDrawAreaOnFloor
//
// Descripcion:
// - Modulo que implementa el trabajo con el motor isometrico. 
// - Todos los tiles tendra unas dimensiones de 64x32. 
// - El motor isometrico sera un componente de CWorld y trabajara con objetos
//   de tipos CArea, que contendran toda la informacion relativa al contenido que
//   el motor sera capaz de coordinar para el dibujado.
// - Dentro del motor isometrico se implementara la clase CPathFinder que sera
//   la que se encargue de implementar el sistema de busqueda utilizando el
//   algoritmo A*.
//
// Notas:
// * CIsoEngine
// - Las dimensiones maximas de un area podran oscilar entre 1..255. En el
//   array que represente el area, la fila y columna 0 estaran reseradas para
//   las celdas marco. Cuando se quiera acceder a una posicion de un area,
//   siempre se exijira trabajar con posiciones entre 1 y 255.
// * CPathFinder
// - La Closed list no existira como tal, en lugar de ello, se usara un map
//   general que contendra punteros a nodos. Dichos nodos tendran dos flags
//   indicando en que lista se hallan. Cuando un nodo este en la Open, es seguro
//   que se pueda encontrar en el monticulo que la representa y cuando este
//   en la Closed es seguro que se podra encontrar unicamente en el map
//   general. El map general acelerara el proceso de busqueda.
// - Como apoyo en la eficiencia, se usara un CRecycleNodePool que evitara
//   el estar haciendo sucesivas llamadas New y aprovechara nodos previamente
//   creados.
// - Se tomara al motor isometrico como observer de CWorld para conocer 
//   cuando es destruida una entidad y comprobar si dicha entidad posee
//   la camara asociada. Si esto ocurriera asi, se debera de asociar la
//   camara al jugador.
// * CDrawArea
// - Se implementara una jerarquia de clases muy sencillas para sobrecargar
//   su operador (). Al sobrecargarlo, se incluira en el mismo el metodo
//   de dibujado del area preciso. De esta forma, se implementara un metodo
//   en CIsoEngine que contendra el bucle de dibujado y recibira por parametro
//   una clase derivada de CDrawArea cuyo operador () contendra la llamada a
//   CArea para los metodos que se requieran.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CISOENGINE_H_
#define _CISOENGINE_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _SYSENGINE_H_
#include "SYSEngine.h"
#endif
#ifndef _ICRESOURCEMANAGER_H_
#include "iCResourceManager.h"
#endif
#ifndef _ICCRIATUREOBSERVER_H_
#include "iCCriatureObserver.h"
#endif
#ifndef _ICWORLDOBSERVER_H_
#include "iCWorldObserver.h"
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif
#ifndef _WORLDDEFS_H_
#include "WorldDefs.h"
#endif
#ifndef _CMEMORYPOOL_H_
#include "CMemoryPool.h"
#endif
#ifndef _CSPRITE_H_
#include "CSprite.h"
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _CISOTRAVELLINGCMD_H_
#include "CIsoTravellingCmd.h"
#endif
#ifndef _CFADECMD_H_
#include "CFadeCmd.h"
#endif
#ifndef _CCYCLICFADECMD_H_
#include "CCyclicFadeCmd.h"
#endif
#ifndef _CMOVISOENGINECMD_H_
#include "CMovIsoEngineCmd.h"
#endif
#ifndef _CAREA_H_
#include "CArea.h"
#endif
#ifndef _CCRIATURE_H_
#include "CCriature.h"
#endif
#ifndef _CPLAYER_H_
#include "CPlayer.h"
#endif
#ifndef _CRECYCLENODEPOOL_CPP_
#include "CRecycleNodePool.cpp"
#endif
#ifndef _DEQUE_H_
#include <deque>
#define _DEQUE_H_
#endif
#ifndef _SET_H_
#include <set>
#define _SET_H_
#endif
#ifndef _VECTOR_H_
#define _VECTOR_H_
#include <vector>
#endif
#ifdef _ICMATHUTIL_H_
#include "iCMathUtil.h"
#endif
#ifndef _ICGUIMANAGER_H_
#include "iCGUIManager.h"
#endif
#ifndef _ICENTITYSELECTOR_H_
#include "iCEntitySelector.h"
#endif

// Defincion de clases / estructuras / espacios de nombres
class CPath;
class CWorldEntity;
struct iCCommandManager;
struct iCGraphicSystem;
struct iCResourceManager;
struct iCMathUtil;

// Clase CIsoEngine
class CIsoEngine: public iCCriatureObserver,
				  public iCWorldObserver
{
private:
  // Clase para la busqueda de caminos
  class CPathFinder 
  {
  private:
	// Estructuras
	struct sNodeSearch {
	  // Nodo de busqueda
	  // Enumerados
	  enum eNodeAlloc {
		// Posibles lugares de alojamiento para un nodo
		OPEN, CLOSED, NO_ALLOC
	  };
	  // Datos
	  sNodeSearch*       pParentState; // Estado padre
	  AreaDefs::sTilePos TilePos;      // Posicion del tile al que hace ref.
	  float              fCostToThis;  // Coste para llegar a este nodo
	  float              fCostToDest;  // fCostToThis + Heuristic
	  eNodeAlloc         NodeAlloc;    // Lugar de alojamiento del nodo
	  // Constructor
	  sNodeSearch(void): pParentState(NULL),
						 fCostToThis(0.0f),
						 fCostToDest(0.0f),
						 NodeAlloc(sNodeSearch::NO_ALLOC) { }
	  // Pool de memoria
	  static CMemoryPool MPool;
	  static void* operator new(const size_t size) { return MPool.AllocMem(size); }
	  static void operator delete(void* pItem) { MPool.FreeMem(pItem); } 
	};

  private:
	// Tipos
	// Define el map para la relacion posicion - nodo estado
	typedef std::map<AreaDefs::TileIndex, sNodeSearch*> MainNodeIndex;
	typedef MainNodeIndex::iterator                     MainNodeIndexIt;
	typedef MainNodeIndex::value_type                   MainNodeIndexValType;
	// Define un monticulo a partir de un vector para representar la Open
	typedef std::vector<sNodeSearch*> Heap;
	typedef Heap::iterator            HeapIt;	

  private:
	// Predicado para el trabajo con el monticulo de la Open
	// a la hora de comparar nodos entre si
	class CHeapComp 
	{
	public:
	  bool operator()(sNodeSearch* const pFirst, 
					  sNodeSearch* const pSecond) const {
		return (pFirst->fCostToDest > pSecond->fCostToDest);
	  }
	}; // ~ CHeapComp 

  private:
	// Alojamiento de los nodos	
	CRecycleNodePool<sNodeSearch> 
	               m_RecycleNodePool; // Banco de nodos
	Heap           m_Open;            // Monticulo con los nodos en la Open
	MainNodeIndex  m_MainNodeIndex;   // Map principal (ver notas)

	// Area actual
	CArea* m_pActArea; // Area actual

	// Datos precalculados	
	// Direc. a visitar
	IsoDefs::eDirectionIndex m_DirsToVisit[IsoDefs::MAX_DIRECTIONS]; 
	// Flags asociados a las direcciones a visitar que hay que chequear en los adj.
	IsoDefs::eDirectionFlag  m_MaskFlagToCheck[IsoDefs::MAX_DIRECTIONS];

	// Resto de vbles
	bool m_bIsInitOk; // ¿Instancia inicializada?

  public:
	// Constructor / destructor
	CPathFinder(void): m_bIsInitOk(false),
				       m_pActArea(NULL) { }
	~CPathFinder(void) { End(); }

  public:
	// Protocolo de inicio y fin de instancia
	bool Init(void);
	void End(void);
	inline bool IsInitOk(void) const { return m_bIsInitOk; }

  public:
	// Trabajo con el establecimiento del area
	inline void SetArea(CArea* const pArea) {
	  ASSERT(IsInitOk());
	  // Asocia / desasocia area
	  m_pActArea = pArea;
	}
	inline bool IsAreaAttached(void) const {
	  ASSERT(IsInitOk());
	  // Retorna flag
	  return (m_pActArea != NULL);
	}

  public:
	// Trabajo para la obtencion del camino de busqueda o info derivada
	CPath* const FindPath(const AreaDefs::sTilePos& TileSrc,
						  const AreaDefs::sTilePos& TileDest,
						  const word uwDistanceMin = 0,
						  const bool bGhostMode = false);	
	sword CalculePathLenght(const AreaDefs::sTilePos& TileSrc,
						    const AreaDefs::sTilePos& TileDest);	
	sword CalculeAbsoluteDistance(const AreaDefs::sTilePos& TileSrc,
								  const AreaDefs::sTilePos& TileDest);
  private:
	// Metodos de a apoyo
	bool AccessValidInAdjacentsOfTileDest(const AreaDefs::sTilePos& TileSrc,
										  const IsoDefs::eDirectionIndex& TileDestDir);

  private:
	// Generacion del camino de busqueda una vez encontrado
	void _CreatePath(sNodeSearch* const pNodeSearch,
					 CPath*& pPath);
  
  private:
	// Metodos de trabajo con el MainNodeIndex	
	void CleanMainNodeIndex(void);
	
  private:
	// Metodos para el trabajo con la Open
	void OpenPop(sNodeSearch*& pNodeSearch);
	void OpenPush(sNodeSearch* const pNode);
	void OpenUpdateNodePriority(sNodeSearch* const pNodeUpdated);
	void CleanOpen(void);

  private:
	// Obtencion del valor heuristico
	inline float GetHeuristicValue(const AreaDefs::sTilePos& TileSrc, 						 
								   const AreaDefs::sTilePos& TileDest) {	  
	  ASSERT(IsInitOk());  
	  // Calcula el valor heuristico y lo devuelve
	  const word uwXDist = TileSrc.XTile > TileDest.XTile ? 
						   TileSrc.XTile - TileDest.XTile : 
						   TileDest.XTile - TileSrc.XTile;
	  const word uwYDist = TileSrc.YTile > TileDest.YTile ? 
						   TileSrc.YTile - TileDest.YTile : 
						   TileDest.YTile - TileSrc.YTile;
	  return (uwXDist > uwYDist) ? uwXDist : uwYDist;
	}  
  }; // ~ CPathFinder

  class CDrawArea {
	public:
	  // Operador de dibujado	
	  virtual operator()(CArea*& pArea,
						 const AreaDefs::sTilePos& TilePos,
						 const sword& swXScreen, 
						 const sword& swYScreen) = 0;
  }; // ~ CDrawArea

  class CDrawAreaFloor: public CDrawArea {
	public:	
	  // Operador de dibujado 
	  operator()(CArea*& pArea,
				 const AreaDefs::sTilePos& TilePos,
				 const sword& swXScreen, 
				 const sword& swYScreen) {
		// Realiza la operacion de dibujado
		pArea->DrawFloor(TilePos, swXScreen, swYScreen);	 
	  }
  }; // ~ CDrawAreaFloor

  class CDrawAreaOnFloor: public CDrawArea {
	public:	
	  // Operador de dibujado
	  operator()(CArea*& pArea,
				 const AreaDefs::sTilePos& TilePos,
				 const sword& swXScreen, 
				 const sword& swYScreen) {
		// Realiza la operacion de dibujado
		pArea->DrawOnFloor(TilePos, swXScreen, swYScreen);	 
	  }
  }; // ~ CDrawAreaOnFloor

private:
  // Tipos
  // Conjutos para el control de entidades hechas transparentes
  typedef std::set<AreaDefs::EntHandle> TranspEntitiesSet;
  typedef TranspEntitiesSet::iterator   TranspEntitiesSetIt;
  // Mascara para grupos de entidades
  typedef word GroupMask;

private:
  // Enumerados
  enum {
	// Grupos de entidades catalogadas
	CRIATURE_GROUP    = 0X01,
	SCENE_OBJ_GROUP   = 0X02,
	WALL_GROUP        = 0X04,
  };

private:
  // Estructuras  
  struct sIsoMap {
	// Sistema de mantenimiento del mapa isometrico
	// Datos asociados al area
	CArea* pArea; // Area actual
	// Datos asociados a la pos. del mapa para dibujado
	sRect              rWindow;             // Ventana de dibujado
	sRect              rTileViewArea;       // Area de dibujado relativa a tiles
	AreaDefs::sTilePos TilePos;             // Posicion actual del area en X	
	sbyte              sbYScrInit;          // Posicion de inicio de dibujado en pantalla
	sbyte              sbXScrEvenInit;      // Posicion par de inicio de dibujado en pantalla
	sbyte              sbXScrOddInit;       // Posicion impar de inicio de dibujado en pantalla
	bool               bDraw;               // ¿Hay que dibujar?
	sword              swMaxXScreenDrawPos; // Pos. max. de dibujado en parte dcha de pantalla
	sword              swMaxYScreenDrawPos; // Pos. max. de dibujado en parte inf de pantalla
	// Flags posibles de dibujado
	byte ubNormalDrawFlags;      // Flags normales de dibujado
	byte ubEntityMovExDrawFlags; // Flags para el dibujado de entidades en extremos
	byte ubExLayersDrawFlags;    // Flags para el dibujado de capas inferiores
	// Datos precalculados
	byte ubNumTilesAtHoriz; // Numero de tiles en horizontal
	byte ubNumTilesAtVert;  // Numero de tiles en vertical.	
  };

  struct sScrollInfo {
	// Informacion referida al scroll / desplazamiento del mapa	
	// Control del scroll
	word  uwSpeed;    // Velocidad del scroll en pixels	
	float fXWorld;    // Posicion en el universo en X
	float fYWorld;    // Posicion en el universo en Y
	float fXWorldMax; // Posicion maxima en el universo en X
	float fYWorldMax; // Posicion maxima en el universo en Y
	byte  ubXOffset;  // Valor actual de offset en X
	byte  ubYOffset;  // Valor actual de offset en Y	
	// Ejecucion de scroll
	CMovIsoEngineCmd ScrollCmd;  // Comando de mov. del motor isometrico
  };

  struct sCameraInfo {
	// Informacion asociada a la camara	
	bool                bAttached;         // ¿Asociada camara a entidad o posicion?
	bool                bAttachedToPlayer; // ¿Asociada al jugador?
	AreaDefs::EntHandle hEntity;           // Handle a la entidad
	AreaDefs::RoomID    Room;              // Id a la POSIBLE habitacion asociada
	CIsoTravellingCmd   TravellingCmd;     // Comando de travelling
	CMovIsoEngineCmd    MovIsoCmd;         // Comando de movimiento del motor
	float               fXScrollMax;       // Max. scroll en x para entidad con camara
	float               fYScrollMax;       // Max. scroll en y para entidad con camara
	float               fXScrollMin;       // Min. scroll en x para entidad con camara
	float               fYScrollMin;       // Min. scroll en y para entidad con camara
  };

  struct sTransparentSys {
	// Datos asociados al sistema para el trabajo con el proceso de
	// hacer transparente a todas aquellas entidades (paredes y objetos de
	// escenario) que puedan tapar al jugador.
	GraphDefs::sAlphaValue Alpha;          // Valor alpha de transparencia.
	TranspEntitiesSet      TranspEntities; // Set de entidades transparentes
  };

  struct sSelectCellSys {
	// Trabajo con la seleccion de entidades
	// Datos visuales
	AreaDefs::sTilePos  TilePos; // Posicion donde localizar la seleccion
	AreaDefs::TileIndex IdxPos;  // Posicion absoluta
	bool                bDraw;   // Flag de dibujado
	// Seleccion de celda
	byte   ubWidthPow;              // 2^ubWidthPow = TILE_WIDTH
	byte   ubHeightPow;             // 2^ubHeightPow = TILE_HEIGHT			
	sDelta Mask[IsoDefs::TILE_DIM]; // Valores deltas de la mascara			
  };

private:
  // Interfaces a otros subsistemas
  iCCommandManager*  m_pCmdManager;  // Manager de comandos
  iCGraphicSystem*   m_pGraphSystem; // Subsistema grafico
  iCResourceManager* m_pResManager;  // Servidor de recursos
  iCMathUtil*        m_pMathUtil;    // Utilidades matematicas
  
  // Sistemas de informacion 
  sIsoMap         m_IsoMap;        // Info. sobre el area vinculada  
  CPathFinder     m_PathFinder;    // Localizador de caminos
  sScrollInfo     m_ScrollInfo;    // Info. sobre el trabajo con el scroll  
  sCameraInfo     m_CameraInfo;    // Informacion asociada a la camara 
  sTransparentSys m_TranspSys;     // Subsistema de info para transparencias sobre entidades
  sSelectCellSys  m_SelectCellSys; // Subsistema de info para seleccion de celdas
  
  // Resto vbles  
  bool m_bIsInitOk; // ¿Clase inicializada?

public:
  // Constructor / destructor
  CIsoEngine(void):	m_pResManager(NULL),
					m_pCmdManager(NULL),
					m_pGraphSystem(NULL),
					m_pMathUtil(NULL),
					m_bIsInitOk(false) { }
  ~CIsoEngine(void) { End(); }

public:
  // Protocolo de inicio y fin de instancia
  bool Init(void);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }
  
private:
  // Metodos de apoyo al Init  
  bool InitIsoMap(void);
  void InitMaskDeltas(const std::string& szMaskFileName);
  bool InitIsoInterfaz(void);
  bool InitIsoScroll(void);
  bool InitIsoCamera(void);
  bool InitTranspSys(void);
  bool InitSelectCellSys(void);

public:
  // Carga y configuracion de areas
  void SetArea(CArea* const pArea);
  inline bool IsAreaSet(void) const { 
	ASSERT(IsInitOk());
	// Comprueba si hay area vinculada
	return (NULL != m_IsoMap.pArea); 
  }
  
public:
  // Dibujado del mapa
  void Draw(void);  
private:
  // Metodos de apoyo
  void DrawArea(CDrawArea& Draw);

public:
  // Manipulacion del area de vision
  void SetDrawWindow(const word uwXTop, 
					 const word uwYTop);
private:
  // Metodos de apoyo
  void SetWorldView(const float fXWorldPos, 
					const float fYWorldPos);

private:
  // Trabajo con el area de vision relativa a tiles
  inline void SetTileViewArea(void) {	
	ASSERT(IsInitOk());
	// Este metodo se llamara siempre que exista una variacion en el comienzo
	// del tile visualizable, tanto en x como en y, para establecer el area
	// de vision referido al mismo. Con dicho area, se podra averiguar rapidamente
	// si una entidad es o no visible en pantalla.
	m_IsoMap.rTileViewArea.swLeft = m_IsoMap.TilePos.XTile;
	m_IsoMap.rTileViewArea.swRight = m_IsoMap.TilePos.XTile + m_IsoMap.ubNumTilesAtHoriz;
	m_IsoMap.rTileViewArea.swTop = m_IsoMap.TilePos.YTile;
	m_IsoMap.rTileViewArea.swBottom = m_IsoMap.TilePos.YTile + m_IsoMap.ubNumTilesAtVert;
	// Clipping
	ASSERT(m_IsoMap.pArea);
	if (m_IsoMap.rTileViewArea.swRight > m_IsoMap.pArea->GetWidth()) { 
	  m_IsoMap.rTileViewArea.swRight = m_IsoMap.pArea->GetWidth();
	}
	if (m_IsoMap.rTileViewArea.swBottom > m_IsoMap.pArea->GetHeight()) {
	  m_IsoMap.rTileViewArea.swBottom = m_IsoMap.pArea->GetHeight();
	}
  }  

public:
  // Trabajo con el scroll  
  bool MoveRight(const float fOffset);
  bool MoveLeft(const float fOffset);
  bool MoveNorth(const float fOffset);
  bool MoveSouth(const float fOffset);  
  void MoveTo(const IsoDefs::eDirectionIndex& Direction);
  inline void SetScrollSpeed(const word uwSpeed) { m_ScrollInfo.uwSpeed = uwSpeed; }
  inline word GetScrollSpeed(void) const { return m_ScrollInfo.uwSpeed; }
  
public:
  // Trabajo con la camara
  void AttachCamera(const AreaDefs::EntHandle& hEntity, 
					const word uwSpeed = 0,
					iCCommandClient* const pClient = NULL,
					const CommandDefs::IDCommand& IDCommand = 0,
					const dword udExtraParam = 0);
  void AttachCamera(const AreaDefs::sTilePos& TilePos,
				    const word uwSpeed = 0,
					iCCommandClient* const pClient = NULL,
					const CommandDefs::IDCommand& IDCommand = 0,
					const dword udExtraParam = 0);  
  void AttachCamera(const float fXWorldPos, 
					const float fYWorldPos,
				    const word uwSpeed = 0,
					iCCommandClient* const pClient = NULL,
					const CommandDefs::IDCommand& IDCommand = 0,
					const dword udExtraParam = 0);    
  void CenterCamera(void);
  void QuitCamera(void);
  inline AreaDefs::EntHandle GetEntityWithCamera(void) const { 
	ASSERT(IsInitOk());
	// Se retorna handle
	return m_CameraInfo.hEntity; 
  }
  inline bool IsCameraAttached(void) const { 
	ASSERT(IsInitOk());
	// Se retorna flag
	return m_CameraInfo.bAttached; 
  }  
private:
  // Metodos de apoyo
  void FindCenterPosition(const float fXWorldSrc, 
						  const float fYWorldSrc,
						  float& fXWorldDest, 
						  float& fYWorldDest);  
  void CalculeCameraScrollValues(void);
  inline bool IsCameraAttachedToPlayer(void) const {
	ASSERT(IsInitOk());
	// Se retorna flag
	return m_CameraInfo.bAttachedToPlayer;
  }  

public:
  // Filtrado para el cambio de posicion de entidades en el universo de juego
  bool ChangeLocation(const AreaDefs::EntHandle& hEntity,
					  const AreaDefs::sTilePos& NewPos);
  bool ChangeLocation(const AreaDefs::EntHandle& hItem,
					  const AreaDefs::EntHandle& hEntity);
  void ChangeLocationForVisuals(const AreaDefs::EntHandle& hEntity,
								const AreaDefs::sTilePos& OriginalPos,
								const AreaDefs::sTilePos& VisualPos);

private:
  // Notificacion de la presencia de entidades en pantalla
  void EntitiesVisiblesInScreenNotify(AreaDefs::sTilePos InitPos,
									  AreaDefs::sTilePos EndTilePos,
									  const bool bVisibles);
  inline void ClipTilePos(AreaDefs::sTilePos& TilePos) {
	ASSERT(IsInitOk());
	// Acota una posicion recibida
    if (TilePos.XTile < 0) {
	  TilePos.XTile = 0;
	} else if (TilePos.XTile > m_IsoMap.pArea->GetWidth()) {
	  TilePos.XTile = m_IsoMap.pArea->GetWidth();
	}
  }

public:
  // Control de la pausa
  void SetPause(const bool bPause);

public:
  // Obtencion de valores de localizacion
  inline sPosition PassTilePosToWorldPos(const AreaDefs::sTilePos& TilePos) {
	ASSERT(IsInitOk());
	// Pasa una posicion referida a un tile a una posicion universal y la
	// retorna ajustando antes el resultado.
	sPosition WorldPos(TilePos.XTile * IsoDefs::TILE_WIDTH, 
					   TilePos.YTile * IsoDefs::TILE_HEIGHT_DIV);
	WorldPos.swYPos += m_IsoMap.sbYScrInit;
	if (m_pMathUtil->IsEven(TilePos.YTile)) { 
	  WorldPos.swXPos += m_IsoMap.sbXScrEvenInit; 
	}
	return WorldPos;
  }
  inline float GetXWorldPos(void) const {
	ASSERT(IsInitOk());
	// Retorna posicion en X en el universo de juego
	return m_ScrollInfo.fXWorld; 
  }
  inline float GetYWorldPos(void) const  { 
	ASSERT(IsInitOk());
	// Retorna posicion en Y en el universo de juego
	return m_ScrollInfo.fYWorld; 
  }
  inline sRect GetViewWindow(void) const { 
	ASSERT(IsInitOk());
	// Retorna area de la ventana
	return m_IsoMap.rWindow; 
  }  

private:
  // Trabajo con la visibilidad de los techos
  void DetermineRoofVisibility(const AreaDefs::RoomID& PrevRoom,
							   const AreaDefs::RoomID& NewRoom);
  
public:
  // Trabajo con la seleccion de celdas
  IsoDefs::eSelectCellType SelectCell(const sword& swXScreen, 
									  const sword& swYScreen);
  inline void SetDrawSelectCell(const bool bDraw) {	
	ASSERT(IsInitOk());
	// Accion distinta
	if (m_SelectCellSys.bDraw != bDraw) {
	  // Establece el flag de dibujado
	  iCEntitySelector* pSelector = SYSEngine::GetGUIManager()->GetMapSelector();
	  if (bDraw) {
		m_IsoMap.pArea->SetFloorSelector(m_SelectCellSys.TilePos, pSelector);
		pSelector->SelectWithAlphaFade(true);
	  } else {	  
		m_IsoMap.pArea->UnsetFloorSelector();
		pSelector->SelectWithAlphaFade(false);
	  }	
	  m_SelectCellSys.bDraw = bDraw;
	}	
  }
  inline AreaDefs::sTilePos GetSelectCellPos(void) const {
	ASSERT(IsInitOk());
	// Devuelve la posicion donde esta seleccionada la celda
	return m_SelectCellSys.TilePos;
  }
private:
  // Metodos de apoyo
  void WorldToTile(const float fXWorld, 
				   const float fYWorld, 
				   AreaDefs::sTilePos& TilePos);  
  void ScreenToTile(const sword& swXScreen, 
					const sword& swYScreen,					
					AreaDefs::sTilePos& TilePos);

public:
  // Trabajo con el jugador
  inline void MovPlayerToSelectCell(void) const {
	ASSERT(IsInitOk());
	ASSERT(m_IsoMap.pArea);
	// Ordena el movimiento 
	// NOTA: El ultimo parametro es el estado de animacion. Habra que sacar
	// fuera dicho parametro, estableciendo el estado de animacion de algun
	// otro modo.
	// En caso de que se este andando, antes se ordenara su detencion pues
	// no sera posible ordenar cuando hay ejecutandose ya una orden
	if (m_IsoMap.pArea->GetPlayer()->IsWalking()) {
	  m_IsoMap.pArea->GetPlayer()->StopWalk();
	}
	m_IsoMap.pArea->GetPlayer()->Walk(m_SelectCellSys.TilePos);
  }
  
public:
  // Movimiento de entidades
  bool MoveEntity(const AreaDefs::EntHandle& hEntity,
				  const IsoDefs::eDirectionIndex& Dir, 
				  const bool bForce);

public:
  // Localizacion de entidades
  CWorldEntity* const GetEntityAt(const word uwXScreen, 
								  const word uwYScreen);
  CWorldEntity* const GetEntityAt(const AreaDefs::sTilePos& TilePos,
								  const word uwXScreen,
								  const word uwYScreen);

public:
  // Trabajo con el dibujado
  inline void SetDrawFlag(const bool bDraw) {
	ASSERT(IsInitOk());
	// Establece el flag de dibujado
	m_IsoMap.bDraw = bDraw;
  }
  inline bool IsDrawFlagActive(void) const {
	ASSERT(IsInitOk());
	// Se retorna el flag de dibujado
	return m_IsoMap.bDraw;
  }

public:  
  inline bool IsTilePosVisible(const AreaDefs::sTilePos& TilePos) const {
	ASSERT(IsInitOk());
	// Comprueba si una posicion es visible o no. Para que sea visible, se ha
	// de dar el caso de ser una posicion que este dentro del area actual de 
	// vision. El area actual de vision, sera medido en tiles no en pixels 
	// y su valor se hallara en m_IsoMap.rTileViewArea.
	return m_pMathUtil->IsPointInRect(sPosition(TilePos.XTile, TilePos.YTile), 
									  m_IsoMap.rTileViewArea);
  }
  bool GetEntityScreenPos(CWorldEntity* const pEntity, 
						  sword& swXScreen, 
						  sword& swYScreen);
public:
  // Metodos para el trabajo con el algoritmo de busqueda de caminos y 
  // proximidad entre tiles
  inline CPath* FindPath(const AreaDefs::sTilePos& TileSrc, 				  
						 const AreaDefs::sTilePos& TileDest,
						 const word uwDistanceMin = 0,
						 const bool bGhostMode = false) {
	ASSERT(IsInitOk());
	// Traslada la responsabilidad al localizador de caminos
	return m_PathFinder.FindPath(TileSrc, TileDest, uwDistanceMin, bGhostMode);
  }
  inline sword CalculePathLenght(const AreaDefs::sTilePos& TileSrc, 				  
						         const AreaDefs::sTilePos& TileDest) {
	ASSERT(IsInitOk());
	// Traslada la responsabilidad al localizador de caminos
	return m_PathFinder.CalculePathLenght(TileSrc, TileDest);
  }
  sword CalculeAdjacentPosInDestination(const AreaDefs::sTilePos& TileSrc, 				  
									    const AreaDefs::sTilePos& TileDest,
									    AreaDefs::sTilePos& AdjacentTilePos);
  inline sword CalculeAbsoluteDistance(const AreaDefs::sTilePos& TileSrc,
									   const AreaDefs::sTilePos& TileDest) {
	ASSERT(IsInitOk());
	// Translada responsibilidad al PathFinder
	// Nota: En caso de ser posiciones NO validas en cuanto a no hallarse
	// dentro de los limites del escenario, se retornara -1
	return m_PathFinder.CalculeAbsoluteDistance(TileSrc, TileDest);
  }
  bool IsAdjacentTo(const AreaDefs::sTilePos& TileSrc,
					const AreaDefs::sTilePos& TileDest);	

private:
  // Control del susbistema para la realizacion de transparencias en entidades
  void UpdateTransparentSys(const AreaDefs::sTilePos& TilePos,
							const GroupMask& EntitiesGroupToCheck);
  void MakeTranspEntitiesOver(const AreaDefs::sTilePos& TilePos,
							  const sRect& PlayerRect,
							  TranspEntitiesSet& TempSet,
							  const GroupMask& EntitiesGroupToCheck);
  void SetEntityTransparent(CWorldEntity* const pEntity, 
							const bool bTransp);

private:
  // Metodos de apoyo generales
  bool IsEntityInGroup(const AreaDefs::EntHandle& hEntity,
  					   const GroupMask& Group);
  void MakeEntityRect(CWorldEntity* const pEntity, 
					  CFloor* const pFloor, 
					  sRect& EntityRect);
  IsoDefs::eDirectionFlag GetDirectionFlag(const IsoDefs::eDirectionIndex& aubDir);
  inline bool InViewWindow(const sword& swXPos, const sword& swYPos) const {
	return (m_IsoMap.rWindow.swLeft > swXPos ||
			m_IsoMap.rWindow.swRight < swXPos ||
			m_IsoMap.rWindow.swTop > swYPos ||
			m_IsoMap.rWindow.swBottom < swYPos) ? false : true;
  }

public:
  // iCCriatureObserver / Operacion de notificacion
  void CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
							  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
						      const dword udParam = 0);

public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);
};

#endif // ~ CIsoEngine

