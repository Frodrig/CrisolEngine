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
// CArea.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CArea.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

#include "CArea.h"

#include "iCGraphicSystem.h"
#include "iCLogger.h"
#include "iCWorld.h"
#include "iCGameDataBase.h"
#include "iCEntitySelector.h"
#include "iCMathUtil.h"
#include "CrisolBuilder\\CBDefs.h"
#include "CEntity.h"
#include "CItemContainerIt.h"
#include <algorithm>

// Estructura donde alojar la info luminosa de forma temporal
struct sTempPlayerLightInfo {
  AreaDefs::EntHandle hEntity;        // Entidad (jugador o item)
  GraphDefs::Light    LightIntensity; // Luz
  // Constructores
  sTempPlayerLightInfo(const AreaDefs::EntHandle& ahEntity,
					   const GraphDefs::Light& aLightIntensity): hEntity(ahEntity),
																 LightIntensity(aLightIntensity) { }
  sTempPlayerLightInfo(const sTempPlayerLightInfo& TempPlayerLightInfo) {
	hEntity = TempPlayerLightInfo.hEntity;
	LightIntensity = TempPlayerLightInfo.LightIntensity;
  }	  
};

// Tipos para el control de listas con la info luminosa
typedef std::list<sTempPlayerLightInfo> PlayerLightInfoList;
typedef PlayerLightInfoList::iterator   PlayerLightInfoListIt;

// Inicializacion de los manejadores de memoria
CMemoryPool 
CArea::sNCell::m_MPool(256, sizeof(CArea::sNCell), true);
CMemoryPool 
CArea::sNSceneObj::m_MPool(16, sizeof(CArea::sNSceneObj), true);
CMemoryPool 
CArea::sNItem::m_MPool(16, sizeof(CArea::sNItem), true);
CMemoryPool 
CArea::sNCriature::m_MPool(16, sizeof(CArea::sNCriature), true);
CMemoryPool 
CArea::sNWall::m_MPool(64, sizeof(CArea::sNWall), true);
CMemoryPool 
CArea::sNRoof::m_MPool(64, sizeof(CArea::sNRoof), true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se dejara reinicializar.
///////////////////////////////////////////////////////////////////////////////
bool CArea::Init(void)
{
  // ¿Se intenta reinicializar?
  if (IsInitOk()) { 
	return false; 
  }

  #ifdef ENGINE_TRACE    
     SYSEngine::GetLogger()->Write("CArea::Init> Inicializando Area.\n");
  #endif 

  // Se obtienen instancias a otros modulos
  m_pFileSys = SYSEngine::GetFileSystem();
  ASSERT(m_pFileSys);
  m_pGDBase = SYSEngine::GetGameDataBase();
  ASSERT(m_pGDBase);
  m_pGraphSys = SYSEngine::GetGraphicSystem();
  ASSERT(m_pGraphSys);
  m_pWorld = SYSEngine::GetWorld();
  ASSERT(m_pWorld);

  // Se inicializan tablas para el calculo rapido
  InitDeltasValues();  

  // Se inicializan resto de vbles de miembro
  m_PlayerInfo.pPlayer = NULL; 
  m_FloorSelector.pSelector = NULL;

  #ifdef ENGINE_TRACE    
     SYSEngine::GetLogger()->Write("           | Ok.\n");
  #endif 

  // Todo correcto
  m_bIsInitOk = true;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa tablas de valores deltas precalculados, para ayudar en la 
//   loalizacion de posiciones de tiles adyacentes.
// Parametros:
// Devuelve:
// Notas:
// - Se inicializan las tablas que contienen los deltas que para una determinada
//   celda, nos dan los valores a sumar a su posicion y asi obtener las celdas
//   adyacentes. Dependiendo de si la celda tiene y par(even) / impar(odd)
//   la tabla sera distinta.
//   La posicion 0 sera el norte y asi en el sentido de las agujas del reloj
//   hasta la posicion noroeste
///////////////////////////////////////////////////////////////////////////////
void
CArea::InitDeltasValues(void)
{
  // Para Y par
  m_EvenDeltas[IsoDefs::NORTH_INDEX].sbYDelta = -2;
  m_EvenDeltas[IsoDefs::NORTH_INDEX].sbXDelta = 0;
  m_EvenDeltas[IsoDefs::NORTHEAST_INDEX].sbYDelta = -1;
  m_EvenDeltas[IsoDefs::NORTHEAST_INDEX].sbXDelta = 0;
  m_EvenDeltas[IsoDefs::EAST_INDEX].sbYDelta = 0;
  m_EvenDeltas[IsoDefs::EAST_INDEX].sbXDelta = 1;
  m_EvenDeltas[IsoDefs::SOUTHEAST_INDEX].sbYDelta = 1;
  m_EvenDeltas[IsoDefs::SOUTHEAST_INDEX].sbXDelta = 0;
  m_EvenDeltas[IsoDefs::SOUTH_INDEX].sbYDelta = 2;
  m_EvenDeltas[IsoDefs::SOUTH_INDEX].sbXDelta = 0;
  m_EvenDeltas[IsoDefs::SOUTHWEST_INDEX].sbYDelta = 1;
  m_EvenDeltas[IsoDefs::SOUTHWEST_INDEX].sbXDelta = -1;
  m_EvenDeltas[IsoDefs::WEST_INDEX].sbYDelta = 0;
  m_EvenDeltas[IsoDefs::WEST_INDEX].sbXDelta = -1;
  m_EvenDeltas[IsoDefs::NORTHWEST_INDEX].sbYDelta = -1;
  m_EvenDeltas[IsoDefs::NORTHWEST_INDEX].sbXDelta = -1;
  
  // Para celdas con Y impar
  m_OddDeltas[IsoDefs::NORTH_INDEX].sbYDelta = -2;
  m_OddDeltas[IsoDefs::NORTH_INDEX].sbXDelta = 0;
  m_OddDeltas[IsoDefs::NORTHEAST_INDEX].sbYDelta = -1;
  m_OddDeltas[IsoDefs::NORTHEAST_INDEX].sbXDelta = 1;
  m_OddDeltas[IsoDefs::EAST_INDEX].sbYDelta = 0;
  m_OddDeltas[IsoDefs::EAST_INDEX].sbXDelta = 1;
  m_OddDeltas[IsoDefs::SOUTHEAST_INDEX].sbYDelta = 1;
  m_OddDeltas[IsoDefs::SOUTHEAST_INDEX].sbXDelta = 1;
  m_OddDeltas[IsoDefs::SOUTH_INDEX].sbYDelta = 2;
  m_OddDeltas[IsoDefs::SOUTH_INDEX].sbXDelta = 0;
  m_OddDeltas[IsoDefs::SOUTHWEST_INDEX].sbYDelta = 1;
  m_OddDeltas[IsoDefs::SOUTHWEST_INDEX].sbXDelta = 0;
  m_OddDeltas[IsoDefs::WEST_INDEX].sbYDelta = 0;
  m_OddDeltas[IsoDefs::WEST_INDEX].sbXDelta = -1;
  m_OddDeltas[IsoDefs::NORTHWEST_INDEX].sbYDelta = -1;
  m_OddDeltas[IsoDefs::NORTHWEST_INDEX].sbXDelta = 0;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia liberando todos los recursos.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CArea::End(void)
{
  // ¿Area inicializada?
  if (IsInitOk()) {	
	// Si, ¿Area cargada?
	if (IsAreaLoaded()) {
	  // Si, se libera info de jugador y area
	  UnsetPlayer();
	  FreeAreaInfo(true);  	  	  
	}
	
	// Se bajan flags
	m_bIsInitOk = false;
  }
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe la notificacion de las criaturas. Sera necesario este metodo para
//   poder mantener bien calculado el efecto de luz dinamica ante el cambio
//   de posicion de una criatura o el equipamiento de un item, etc.
// Parametros:
// - hCriature. Criatura.
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CArea::CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
							  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
						      const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());
  // SOLO si parametros correctos
  ASSERT(hCriature);

    // Se comprueba el tipo de notificacion recibida
  switch (NotifyType) {
	case CriatureObserverDefs::ACTION_REALICE: {
	  // Se comprueba la accion realizada
	  const RulesDefs::eCriatureAction Action = RulesDefs::eCriatureAction(udParam & 0x0000FFFF);
	  switch(Action) {
	    case RulesDefs::CA_WALK: {	
		  // Cambio de posicion, ¿La entidad tiene luz asociada?
		  const GraphDefs::Light LightValue = GetLight(hCriature);
		  if (LightValue > 0) {
			// Si, se notifica una actualizacion al cambiar de posicion
			SetLightFocusInArea(hCriature, LightValue, true, false);			
		  }

		  // Se recorre cada uno de los slots de equipamiento, si existe
		  // un item equipado con luz asociada, se actualizara su valor
		  iCEquipmentSlots* const pEquipSlots = GetCriature(hCriature)->GetEquipmentSlots();
		  ASSERT(pEquipSlots);
		  const word uwNumEquipSlots = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT);
		  word uwIt = 0;
		  for (; uwIt < uwNumEquipSlots; ++uwIt) {
			// Se halla el item equipado en el slot que proceda
			const AreaDefs::EntHandle hItem = pEquipSlots->GetItemEquipedAt(RulesDefs::eIDEquipmentSlot(uwIt));
			if (hItem) {
			  // Hay item equipado, si tiene luz asociada, se actualiza su posicion
			  const GraphDefs::Light LightValue = GetLight(hItem);
			  if (LightValue > 0) {
				// Si, se notifica una actualizacion al cambiar de posicion
				SetLightFocusInArea(hItem, LightValue, true, false);
			  }
			}
		  }
		} break;  
	
		case RulesDefs::CA_GETITEM: {
		  // Se ha recogido un item:
		  // Se toma la instancia del item
		  const AreaDefs::EntHandle hItem = udParam >> 16;
		  // Si el item tenia luz asociada, se desvinculara del escenario al
		  // ser recogido por la criatura.
		  const GraphDefs::Light Light = GetLight(hItem);
		  if (Light > 0) {
			RemoveLight(hItem, true);
		  }
		} break;

		case RulesDefs::CA_UNEQUIP:{
		  // Se ha desequipado un item
		  // Se toma la instancia del item
		  const AreaDefs::EntHandle hItem = udParam >> 16;
		  // Si el item tenia luz asociada, se desvinculara del escenario al
		  // ser recogido por la criatura.
		  const GraphDefs::Light Light = GetLight(hItem);
		  if (Light > 0) {
			// Antes de poder remover la informacion luminosa, debemos de
			// actualizar sus datos, pues su posicion original sera SEGURO la
			// posicion que tenga su dueño.
			LightFocusInfoMapIt FocusIt(m_DinamicLightInfo.LightFocus.find(hItem));
			CItem* const pItem = GetItem(hItem);
			ASSERT(pItem);
			FocusIt->second.TilePos = pItem->GetTilePos();
			RemoveLight(hItem, true);
		  }
		} break;

		case RulesDefs::CA_DROPITEM: 
		case RulesDefs::CA_EQUIP: {
		  // Se ha tirado un item o se ha equipado:
		  // Se toma instancia al mismo
		  const AreaDefs::EntHandle hItem = udParam >> 16;
		  // Si el item tenia luz asociada, recalculara su influencia en el mapa
		  const GraphDefs::Light Light = GetLight(hItem);
		  if (Light > 0) {
			SetLightFocusInArea(hItem, Light, true, false);
		  }
		} break;		
	  }; // ~ switch	
	} break;
  }; // ~ switch    
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga un area que se considerara inicial. Este area liberara cualquier
//   tipo de informacion anterior sin que esta informacion anterior pase a
//   guardarse a los archivos temporales. 
// Parametros:
// - uwIDArea. Identificador del area a cargar.
// - pPlayer. Instancia al jugador
// - PlayerPos. Posicion del jugador en el area.
// Devuelve:
// - Si todo correcto true. En caso de existir algun problema false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CArea::SetInitArea(const word uwIDArea,
				   CPlayer* const pPlayer,
				   const AreaDefs::sTilePos& PlayerPos)
{
  // SOLO si parametros validos
  ASSERT(pPlayer);

  // ¿Hay area cargada?
  if (IsAreaLoaded()) {
	// Libera la info de area sin guardar datos temporales y desvincula jugador
	FreeAreaInfo(true);
	UnsetPlayer();
  }

  // Intenta cargar area
  if (LoadArea(uwIDArea)) {
	// Se asocia jugador
	if (SetPlayer(pPlayer, PlayerPos)) {
	  // Se inicializan resto de vbles de miembro
	  m_FloorSelector.pSelector = NULL;

	  // Todo correcto
	  return true;
	}
  }
  
  // Hubo problemas
  return false;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera los datos asociados al area actual.
// Parametros:
// - bRemovePlayerInfo. Flag indicativo para saber si hay que eliminar tambien
//   la informacion relativa al jugador.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CArea::FreeAreaInfo(const bool bRemovePlayerInfo)
{
  // SOLO si instancia inicializada y cargada
  ASSERT(IsInitOk());
  ASSERT(IsAreaLoaded());

  // Se levanta el flag que evite efectos laterales
  m_bInFreeAreaMode = true;
  
  // Libera informacion sobre tags
  m_EntityTags.clear();

  // Se libera informacion sobre habitaciones
  m_Map.RoomInfo.clear();

  // Se libera informacion sobre techos que muestran lo que tienen por debajo
  m_Map.ShowUnderRoofs.clear();

  // Se libera informacion sobre mascaras de acceso
  m_Map.IndexOfMaskTileAccess.clear();

  // Se liberan entidades / criaturas		
  // Nota: en la liberacion, se debera de desvincular el area como observer
  CriaturesIt ItCriature(m_Map.Criatures.begin());
  for (; ItCriature != m_Map.Criatures.end(); ItCriature = m_Map.Criatures.erase(ItCriature)) {
    ItCriature->second->Criature.RemoveObserver(this);
    delete ItCriature->second;
  }	

  // Se liberan entidades / objetos de escenario
  SObjsIt ItSObj(m_Map.SceneObjs.begin());
  for (; ItSObj != m_Map.SceneObjs.end(); ItSObj = m_Map.SceneObjs.erase(ItSObj)) {
    delete ItSObj->second;	  
  }	

  // Se liberan entidades / paredes
  WallsIt ItWall(m_Map.Walls.begin());
  for (; ItWall != m_Map.Walls.end(); ItWall = m_Map.Walls.erase(ItWall)) { 
    delete ItWall->second; 	  
  }

  // Se liberan entidades / items
  // Nota: En caso de que se descargue para cambio, se mantendran en
  // memoria los items mantenidos por el jugador
  #ifdef _SYSASSERT
	if (!bRemovePlayerInfo) {
	  ASSERT(m_PlayerInfo.pPlayer);
	}
  #endif
  ItemsIt ItItem(m_Map.Items.begin());
  while (ItItem != m_Map.Items.end()) {
    // ¿NO se desea eliminar la info asociada al jugador?
    if (!bRemovePlayerInfo) {
	  // Si, ¿Item en inventario o equipado?
	  if (m_PlayerInfo.pPlayer->GetItemContainer()->IsItemIn(ItItem->first) ||
		  m_PlayerInfo.pPlayer->GetEquipmentSlots()->IsItemEquiped(ItItem->first)) {
  		// Si, luego se pasa a sig. item
		++ItItem;
		continue;
	  }
	}
	
    // Se desea eliminar info del jugador o bien el item no se halla
	// ni en el inventario ni en los slots de equipamiento del jugador
    delete ItItem->second;
    ItItem = m_Map.Items.erase(ItItem);			  	  
  }	

  // Se liberan entidades / techos
  RoofsMapIt RoofsIt(m_Map.Roofs.begin());
  for (; RoofsIt != m_Map.Roofs.end(); RoofsIt = m_Map.Roofs.erase(RoofsIt)) {
    delete RoofsIt->second;
  }

  // Se libera el mapa de celdas
  const dword udSize = m_Map.uwWidth * m_Map.uwHeight;	
  AreaDefs::TileIndex Index = 0;
  for (; Index < udSize; Index++) {	
    if (m_Map.pMap[Index]) {
  	  delete m_Map.pMap[Index];
  	  m_Map.pMap[Index] = NULL;
	}	  
  }  

  // Se borra el mapa base
  delete[udSize] m_Map.pMap;
  m_Map.pMap = NULL;

  // Se bajan flags
  m_bIsAreaLoaded = false;   
  m_bInFreeAreaMode = false;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Guarda los datos del area actual a su archivo tmp correspondiente.
// Parametros:
// - bIsChangingArea. Indica si se esta guardando el area por un cambio
//   o no (por una simple accion de guardar).
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CArea::SaveAreaInfo(const bool bIsChangingArea)
{
  // SOLO si hay area cargada
  ASSERT(IsAreaLoaded());
  
  // Se establece el nombre del archivo temporal y se obtiene handle al mismo
  FileDefs::FileHandle hFile = 0;
  dword udOffset = 0;  
  std::string szFileName;
  SYSEngine::PassToString(szFileName, "tmp\\Area%utmp.cbb", m_Map.uwID);
  hFile = m_pFileSys->Open(szFileName, true);
  ASSERT(hFile);

  // Se guardan los datos de cabecera: tipo de fichero / version / codigo de area
  udOffset += m_pFileSys->Write(hFile, udOffset, (sbyte *)(&CBDefs::CBBAreaTmpFile), sizeof(byte));
  udOffset += m_pFileSys->Write(hFile, udOffset, (sbyte *)(&CBDefs::HVersion), sizeof(byte));
  udOffset += m_pFileSys->Write(hFile, udOffset, (sbyte *)(&CBDefs::LVersion), sizeof(byte));
  udOffset += m_pFileSys->Write(hFile, udOffset, (sbyte *)(&m_Map.uwID), sizeof(word));  

  // Se libera informacion en el mismo sentido en que esta es cargada
  AreaDefs::sTilePos TileIt;
  TileIt.YTile = 0;
  for (; TileIt.YTile < m_Map.uwHeight; ++TileIt.YTile) {
	TileIt.XTile = 0;
	for (; TileIt.XTile < m_Map.uwWidth; ++TileIt.XTile) {	
	  // Array de flags
	  bool bInfoFlags[] = {
		false, // Floor
		false, // Roofs
		false, // Scene objs.
		false, // Criatures
		false  // Walls
	  };

	  // Obtenemos el tile index
	  const AreaDefs::TileIndex TileIndex = GetTileIdx(TileIt); 

	  // Guardamos offset actual y escribimos flags de info temporales
	  const dword udFlagsOffset = udOffset;
	  udOffset += m_pFileSys->Write(hFile, udOffset, (sbyte *)bInfoFlags, sizeof(bool) * 5);

	  // ¿Existe floor asociado?
	  if (m_Map.pMap[TileIndex]) {
		// Se establece el flag
		bInfoFlags[0] = true;

		// Se recorre la lista de entidades y se guarda en otras su contenido 
		// segun tipo: Item / Scene obj / criature / wall
		CellEntitiesList Entities[4];
		CellEntitiesListIt It(m_Map.pMap[TileIndex]->Entities.begin());		
		for (; It != m_Map.pMap[TileIndex]->Entities.end(); ++It) {
		  switch(GetEntityType(*It)) {
			case RulesDefs::ITEM: {
			  Entities[0].push_back(*It);
			} break;

			case RulesDefs::SCENE_OBJ: {
			  Entities[1].push_back(*It);
			} break;

			case RulesDefs::CRIATURE: {
			  Entities[2].push_back(*It);
			} break;

			case RulesDefs::WALL: {
			  Entities[3].push_back(*It);
			} break;
		  }; // ~ switch
		}		

		// Almacena datos de CFloor
		// Nota: En CFloor es donde mas almacenamientos no implementados en
		// la instancia insteresada existiran.
		// Datos basicos SIN mascara
		m_Map.pMap[TileIndex]->Floor.Save(hFile, udOffset);
		
		// Guarda la mascara de acceso al CFloor
		bool bAccessFlags[8];
		const AreaDefs::MaskTileAccess Mask = GetMaskFloorAccess(TileIt);
		byte ubIt = 0;
		for (; ubIt < IsoDefs::MAX_DIRECTIONS; ++ubIt) {
		  // Se establece flag de obstaculizacion segun orientaciona actual
		  // Nota: Las orientaciones Norte = 0, ... , Noroeste = 7
		  bAccessFlags[ubIt] = (Mask & (IsoDefs::NORTH_FLAG << ubIt)) ? true : false;
		} 
		udOffset += m_pFileSys->Write(hFile, udOffset, (sbyte *)(bAccessFlags), sizeof(bool) * 8);
		// Elevacion
		const RulesDefs::Elevation Elevation = m_Map.pMap[TileIndex]->Floor.GetElevation();
		udOffset += m_pFileSys->Write(hFile, udOffset, (sbyte *)(&Elevation), sizeof(RulesDefs::Elevation));
		// Numero de items sobre el floor
		if (!SaveEntitiesInfo(hFile, udOffset, Entities[0], bIsChangingArea)) {
		  // En el caso de no hallarse entidades se indicara que hay 0 entidades
		  // pues el metodo SaveEntitiesInfo no guarda informacion si no la hay
		  const word uwNumItems = 0;
		  udOffset += m_pFileSys->Write(hFile, udOffset, (sbyte *)(&uwNumItems), sizeof(word));
		}

		// Se guarda el identificador de habitacion
		AreaDefs::RoomID Room = 0;	  
		RoomInfoMapIt RoomInfoIt(m_Map.RoomInfo.begin());
		for (; RoomInfoIt != m_Map.RoomInfo.end(); ++RoomInfoIt) {
		  const CellsInRoomSetIt CellIt(RoomInfoIt->second.Cells.find(TileIndex));
		  if (CellIt != RoomInfoIt->second.Cells.end()) {
			Room = RoomInfoIt->first;
			break;
		  }
		}		
		udOffset += m_pFileSys->Write(hFile, udOffset, (sbyte *)(&Room), sizeof(AreaDefs::RoomID));

		// ¿Existe info asociada a techo?
		if (m_Map.pMap[TileIndex]->hRoof) {		  
		  // Si, se guarda su informacion
		  CRoof* const pRoof = GetRoof(m_Map.pMap[TileIndex]->hRoof);
		  pRoof->Save(hFile, udOffset);

		  // Se guarda flag indicativo de muestra de lo que hay por debajo
		  const ShowUnderRoofsSetIt UnderRoofIt(m_Map.ShowUnderRoofs.find(m_Map.pMap[TileIndex]->hRoof));
		  bool bFlag = (UnderRoofIt != m_Map.ShowUnderRoofs.end()) ? true : false;
		  udOffset += m_pFileSys->Write(hFile, udOffset, (sbyte *)(&bFlag), sizeof(bool));

		  // Se establece el flag
		  bInfoFlags[1] = true;
		}

		// Objetos de escenario
		bInfoFlags[2] = SaveEntitiesInfo(hFile, udOffset, Entities[1], bIsChangingArea);
		
		// Criaturas
		bInfoFlags[3] = SaveEntitiesInfo(hFile, udOffset, Entities[2], bIsChangingArea);
		
		// Paredes
		bInfoFlags[4] = SaveEntitiesInfo(hFile, udOffset, Entities[3], bIsChangingArea);
		
		// Se guardan datos relativos al array de offsets
		m_pFileSys->Write(hFile, udFlagsOffset, (sbyte *)(bInfoFlags), sizeof(bool) * 5);
	  }	  
	}
  }

  // Se cierra el archivo
  m_pFileSys->Close(hFile);
} 

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se recibira una lista de entidades y el metodo guardara la informacion
//   relativa a las mismas siguiendo el formato:
//    * Numero de entidades
//    * Datos de cada entidad
// - Cuando se este cambiando el area, las criaturas temporales NO se
//   guardaran a disco.
// Parametros:
// - hFile. Handle al archivo donde guardar los datos.
// - udOffset. Offset donde depositar los datos.
// - Entities. Lista de entidades.
// - bIsChangingArea. Indica si estaba cambiandose el area.
// Devuelve:
// - Si se guardo algo de informacion true. En caso contrario false.
// Notas:
// - Las entidades 
///////////////////////////////////////////////////////////////////////////////
bool
CArea::SaveEntitiesInfo(const FileDefs::FileHandle& hFile,
					    dword& udOffset,
						CellEntitiesList& Entities,
						const bool bIsChangingArea)
{
  // ¿Hay informacion?
  if (!Entities.empty()) {
	// Guarda la cantidad de entidades
	// Nota: Se guardara el offset ya que si hay criaturas temporales
	// estas no seran guardadas
	const dword udNumEntitiesOffset = udOffset;
	word uwNumEntities = 0;
	udOffset += m_pFileSys->Write(hFile, 
								  udOffset, 
								  (sbyte *)(&uwNumEntities), 
								  sizeof(word));		

	// Procede a recorrer la lista de entidades, ordenando el guardar la info
	// Nota: Si la entidad es contenedor, guardara automaticamente los items
	CellEntitiesListIt It(Entities.begin());
	for (; It != Entities.end(); ++It) {
	  // Obtiene entidad y guarda datos
	  // Nota: Save es metodo virtual
	  CWorldEntity* const pEntity = GetWorldEntity(*It);
	  ASSERT(pEntity);

	  // ¿Se esta cambiando el area?
	  bool bCanSave = true;
	  if (bIsChangingArea) {
		// Si, ¿se esta ante criatura temporal?
		if (GetEntityType(*It) == RulesDefs::CRIATURE &&
		    GetCriature(*It)->IsTmpCriature()) {
		  // Si, no se podra guardar
		  bCanSave = false;
		}		
	  } 

	  // ¿Se puede guardar?
	  if (bCanSave) {
		// Si, se graba
		pEntity->Save(hFile, udOffset);			
		++uwNumEntities;
	  }
	} 

	// Se escribie el numero de entidades guardadas
	m_pFileSys->Write(hFile, 
					  udNumEntitiesOffset, 
					  (sbyte *)(&uwNumEntities), 
					  sizeof(word));		

	// Se guardo informacion
	return true;
  } 
  
  // No se guardo informacion
  return false;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga un area a partir de otra. Este metodo no se podra utilizar si
//   previamente no hay ninguna area cargada.
// - Se conservaran los items del jugador y las luces asociados a estos y
//   al propio jugador.
// Parametros:
// - uwIDArea. Identificador del area a cambiar
// - PlayerPos. Nueva posicion del jugador en el area a cambiar.
// Devuelve:
// - Si todo ha ido correctamente true. En caso contrario false.
// Notas:
// - Sera necesario que exista un area previamente cargada y el jugador
//   se encuentre vinculado.
///////////////////////////////////////////////////////////////////////////////
bool 
CArea::ChangeArea(const word uwIDArea,
				  const AreaDefs::sTilePos& PlayerPos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si hay un area previamente cargada
  ASSERT(IsAreaLoaded());

  // ¿El area a establecer es DISTINTA al area actual?
  if (uwIDArea != m_Map.uwID) {	
  	// Info luminosa del jugador
	PlayerLightInfoList TempPlayerLightInfo;  
	TempPlayerLightInfo.push_back(sTempPlayerLightInfo(m_PlayerInfo.pPlayer->GetHandle(),
													   GetLight(m_PlayerInfo.pPlayer->GetHandle())));
	SetLight(m_PlayerInfo.pPlayer->GetHandle(), 0);

	// Items en inventario
	CItemContainerIt ContainerIt(*m_PlayerInfo.pPlayer->GetItemContainer());
	for (; ContainerIt.IsValid(); ContainerIt.Next()) {
	  ASSERT(ContainerIt.GetItem());
	  TempPlayerLightInfo.push_back(sTempPlayerLightInfo(ContainerIt.GetItem(),
														 GetLight(ContainerIt.GetItem())));
	  SetLight(ContainerIt.GetItem(), 0);
	}

	// Items equipados
	iCEquipmentSlots* const pEquipSlots = m_PlayerInfo.pPlayer->GetEquipmentSlots();
	ASSERT(pEquipSlots);
	const word uwMaxSlots = m_pGDBase->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT);
	word uwIt = 0;
	for (; uwIt < uwMaxSlots; ++uwIt) {
	  const AreaDefs::EntHandle hItem = pEquipSlots->GetItemEquipedAt(RulesDefs::EquipmentSlot(uwIt));
	  if (hItem) {
		TempPlayerLightInfo.push_back(sTempPlayerLightInfo(hItem, GetLight(hItem)));
		SetLight(hItem, 0);
	  }
	}
	
	// Se desvincula al jugador del area actual
	RemoveWorldEntityFromTile(m_PlayerInfo.pPlayer->GetHandle());
	
	// Se guardan datos del area actual y se liberan datos del area actual
	// salvo los relativos al jugador, que se mantendran	
	SaveAreaInfo(true);	
	FreeAreaInfo(false);	

	// Intenta cargar area
	if (LoadArea(uwIDArea)) {	  
      // Se inserta de nuevo el jugador en el area de juego
	  // Si la posicion donde situar al jugador no existe, se intentara
	  // encontrar una valida en cuanto a que exista como celda, sin tener en
	  // cuenta que sobre ella pueda existir una criatura u objeto de escenario.
	  if (IsCellValid(PlayerPos)) {
		InsertWorldEntityInTile(m_PlayerInfo.pPlayer->GetHandle(), PlayerPos);
	  } else {
		iCMathUtil* const pMathUtil = SYSEngine::GetMathUtil();
		ASSERT(pMathUtil);
		pMathUtil->GenSeed();				
		AreaDefs::sTilePos TilePos;
		do {
		  TilePos.XTile = pMathUtil->GetRandValue(0, m_Map.uwWidth - 1);
		  TilePos.YTile = pMathUtil->GetRandValue(0, m_Map.uwHeight - 1);
		} while (!IsCellValid(TilePos) || !IsCellWithContent(TilePos));
		InsertWorldEntityInTile(m_PlayerInfo.pPlayer->GetHandle(), TilePos);
		#ifdef ENGINE_TRACE
  		  SYSEngine::GetLogger()->Write("CArea::ChangeArea> Area %u cargada sin localizar al jugador en <%u, %u>, localizado en <%u, %u>.\n", 
										uwIDArea, PlayerPos.XTile, PlayerPos.YTile, TilePos.XTile, TilePos.YTile);
		#endif
	  }	  
	  
	  // Se repone la informacion luminosa, si procede
	  PlayerLightInfoListIt ListIt(TempPlayerLightInfo.begin());	  
	  for (; ListIt != TempPlayerLightInfo.end(); ++ListIt) {		
		SetLight(ListIt->hEntity, ListIt->LightIntensity);
	  }
	  
	  // Se inicializan resto de vbles de miembro
	  m_FloorSelector.pSelector = NULL; 
	
	  // Todo correcto
	  return true;
	} 

	// Hubo problemas
	return false;
  }
  
  // El area ya esta cargada
  return true;  
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga datos asociados a un fichero de area base. 
// - Primeramente, se obtendran los datos base del area a partir del fichero
//   de area base y seguidamente, se comprobara si existe fichero temporal
//   asociado. Si esto fuera asi (y dicho fichero tempral fuera valido) el
//   resto de la carga seria desde el fichero temporal en lugar de usar el
//   fichero de area base.
// Parametros:
// - uwIDArea. Identificador del fichero de area.
// Devuelve:
// Notas:
// - Para saber el formato del archivo consultar CrisolBuilder
///////////////////////////////////////////////////////////////////////////////
bool 
CArea::LoadArea(const word uwIDArea)
{
  // Se forma el nombre del fichero de area base
  std::string szAreaFileName;
  SYSEngine::PassToString(szAreaFileName, 
						  "%sArea%u.cbb", 
						  m_pGDBase->GetAreaFilesPath().c_str(), uwIDArea);

  // Se abre el fichero asociado al area y se establece offset
  FileDefs::FileHandle hAreaFile = m_pFileSys->Open(szAreaFileName);
  ASSERT(hAreaFile);
  dword udAreaOffset = 0;

  // Valida fichero
  CheckAreaFile(hAreaFile, 
				udAreaOffset, 
				CBDefs::CBBAreaBaseFile, 
				uwIDArea, 
				szAreaFileName);
	  
  // Carga datos basicos del area
  LoadAreaInfo(hAreaFile, udAreaOffset);

  // Establece identificador del nuevo area
  m_Map.uwID = uwIDArea;

  // ¿Existe fichero temporal asociado?
  bool bTmpAreaFile = false;
  SYSEngine::PassToString(szAreaFileName, "tmp\\Area%uTmp.cbb", uwIDArea);
  const FileDefs::FileHandle hAreaTmpFile = m_pFileSys->Open(szAreaFileName);
  if (hAreaTmpFile) {
	// Si, se valida el fichero
	dword udAreaBaseOffset = 0;
	CheckAreaFile(hAreaTmpFile, 
				  udAreaBaseOffset, 
				  CBDefs::CBBAreaTmpFile, 
				  uwIDArea, 
				  szAreaFileName);	

	// Se cierra el fichero de area base y se sustituyen valores por los
	// relativos al fichero de datos temporales
	m_pFileSys->Close(hAreaFile);
	hAreaFile = hAreaTmpFile;
	udAreaOffset = udAreaBaseOffset;
	bTmpAreaFile = true;	
  }  
  
  // Procede a cargar las celdas, estableciendo flag de carga
  m_bIsAreaLoading = true;
  AreaDefs::sTilePos TilePos;
  TilePos.YTile = 0;
  for (; TilePos.YTile < m_Map.uwHeight; ++TilePos.YTile) {
	TilePos.XTile = 0;
	for (; TilePos.XTile < m_Map.uwWidth; ++TilePos.XTile) {	
	  // Establece indice y carga flags de informacion	  	  
	  const AreaDefs::TileIndex TileIndex = GetTileIdx(TilePos);  
	  bool bSectionFlags[5];
	  udAreaOffset += m_pFileSys->Read(hAreaFile,
									   (sbyte *)bSectionFlags,
									   sizeof(bool) * 5,
									   udAreaOffset);	  

	  // ¿NO hay informacion referida a celdas en esta seccion?
	  if (!bSectionFlags[0]) {
		// No, luego se pasa a sig. iteraccion
		ASSERT((m_Map.pMap[TileIndex] == NULL) != 0);
		continue;
	  }
	  
	  // Carga datos basicos de la celda      
	  LoadCells(hAreaFile, udAreaOffset, TilePos, TileIndex, bTmpAreaFile);	  

	  // Carga info de roof
	  if (bSectionFlags[1]) {
		LoadRoofs(hAreaFile, udAreaOffset, TileIndex, bTmpAreaFile);
	  } else {		
		m_Map.pMap[TileIndex]->hRoof = 0;
	  }

	  // Objetos de escenario	  
	  if (bSectionFlags[2]) {
		LoadSceneObjs(hAreaFile, udAreaOffset, TilePos, TileIndex, bTmpAreaFile);
	  }

	  // Criatura
	  if (bSectionFlags[3]) {
		LoadCriatures(hAreaFile, udAreaOffset, TilePos, TileIndex, bTmpAreaFile);
	  }

	  // Paredes
	  if (bSectionFlags[4]) {
		LoadWalls(hAreaFile, udAreaOffset, TilePos, TileIndex, bTmpAreaFile);
	  }	  
	}
  }

  // Se cierra fichero
  m_pFileSys->Close(hAreaFile);	

  // Todo correcto, establece vbles y retorna  
  m_Map.bTmpArea = bTmpAreaFile;
  m_bIsAreaLoading = false;
  m_bIsAreaLoaded = true;
  return true;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Valida un fichero de area. En caso de que no sea valido se
//   producira un error fatal.
// Parametros:
// - hAreaFile. Handle al fichero
// - udAreaOffset. Offset del archivo.
// - ubAreaFileType. Tipo del fichero de area a comprobar.
// - uwIDArea. Identificador del fichero de area
// - szArea. Nombre del fichero de area.
// Devuelve:
// Notas:
// - Para mas informacion sobre el formato, consultar CrisolBuilder.
///////////////////////////////////////////////////////////////////////////////
void 
CArea::CheckAreaFile(const FileDefs::FileHandle& hAreaFile,
					 dword& udAreaOffset,
					 const byte ubAreaFileType,
					 const word uwIDArea,
					 const std::string& szArea)
{
  // SOLO si parametros validos
  ASSERT(hAreaFile);
  ASSERT(!szArea.empty());
  
  // Comprueba el tipo del fichero
  byte ubFileType;
  udAreaOffset += m_pFileSys->Read(hAreaFile, 
								   (sbyte *)(&ubFileType), 
								   sizeof(byte), 
								   udAreaOffset);
  if (ubFileType != ubAreaFileType) {
	SYSEngine::FatalError("Error> Fichero de área %s no válido\n", szArea);
  }
  
  // Salta version
  udAreaOffset += (sizeof(byte) * 2);

  // Comprueba que el codigo del area que se pretende cargar coincide con el que
  // existe guardado en el archivo
  word uwValue;
  udAreaOffset += m_pFileSys->Read(hAreaFile, 
								   (sbyte *)(&uwValue), 
								   sizeof(word), 
								   udAreaOffset);
  if (uwIDArea != uwValue) {
	SYSEngine::FatalError("Fichero de área %s tiene código %u y se pretende cargar como %u.\n", 
						  szArea, uwValue, uwIDArea);
  }  
}
  
//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga datos base de informacion del area y crea el array principal. 
// - Tambien preparara la estructura encargada de alojar la informacion de las
//   habitaciones.
// Parametros:
// - hAreaFile. Handle al fichero
// - udAreaOffset. Offset del archivo.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CArea::LoadAreaInfo(const FileDefs::FileHandle& hAreaFile,
					dword& udAreaOffset)
{
  // SOLO si parametros validos
  ASSERT(hAreaFile);

  // Obtiene nombre del area  
  udAreaOffset += m_pFileSys->ReadStringFromBinary(hAreaFile, 
												   udAreaOffset, 
												   m_Map.szName);
  
  // Obtiene dimensiones y crea el array de elementos
  udAreaOffset += m_pFileSys->Read(hAreaFile, 
								   (sbyte*)(&m_Map.uwWidth), 
								   sizeof(word), 
								   udAreaOffset);
  udAreaOffset += m_pFileSys->Read(hAreaFile, 
								   (sbyte*)(&m_Map.uwHeight), 
								   sizeof(word), 
								   udAreaOffset);  
  ASSERT(!m_Map.pMap);
  const dword udSize = m_Map.uwWidth * m_Map.uwHeight;
  m_Map.pMap = new sNCell*[udSize];
  ASSERT(m_Map.pMap);
  memset(m_Map.pMap, NULL, sizeof(sNCell*) * udSize);  

  // Se lee el valor de iluminacion ambiental
  udAreaOffset += m_pFileSys->Read(hAreaFile, 
								   (sbyte *)(&m_Map.AmbientLight), 
								   sizeof(GraphDefs::Light), 
								   udAreaOffset);

  // Se construye el map de informacion de habitaciones segun el numero de estas
  word uwValue;
  udAreaOffset += m_pFileSys->Read(hAreaFile, 
								   (sbyte *)(&uwValue), 
								   sizeof(word), 
								   udAreaOffset);
  if (uwValue) {
	// Como existe al menos una habitacion, se construye el map
	sRoomInfo RoomInfo;
	AreaDefs::RoomID RoomIt = 1;
	for (; RoomIt <= uwValue; ++RoomIt) {
	  m_Map.RoomInfo.insert(RoomInfoMapValType(RoomIt, RoomInfo));
	}
  }
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga datos asociados a una celda del area
// Parametros:
// - hAreaFile. Fichero de datos del area.
// - udAreaOffset. Offset al fichero de datos del area.
// - TilePos. Posicion del tile.
// - TileIndex. Indice donde localizar la informacion en el mapa.
// - bTmpAreaFile. Indica si se esta cargando, o no, desde un archivo temporal.
// Devuelve:
// Notas:
// - Los CFloor siempre estaran activos.
///////////////////////////////////////////////////////////////////////////////
void
CArea::LoadCells(const FileDefs::FileHandle& hAreaFile,
				 dword& udAreaOffset,
			     const AreaDefs::sTilePos& TilePos,
				 const AreaDefs::TileIndex& TileIndex,
				 const bool bTmpAreaFile)
{
  // SOLO si parametros validos
  ASSERT(hAreaFile);

  // Crea instancia
  m_Map.pMap[TileIndex] = new sNCell;  
  ASSERT(m_Map.pMap[TileIndex]);  
  
  // Se inicializa instancia segun proceda, leyendo tambien la mascara de acceso
  AreaDefs::MaskTileAccess FloorMaskAccess;
  if (bTmpAreaFile) {	
	m_Map.pMap[TileIndex]->Floor.Init(hAreaFile, udAreaOffset);
	FloorMaskAccess = LoadFloorMaskAccess(hAreaFile, udAreaOffset);	  
  } else {	
	std::string szSection;
	udAreaOffset += m_pFileSys->ReadStringFromBinary(hAreaFile, 
													 udAreaOffset,
													 szSection);
	const FileDefs::FileHandle hFile = m_pGDBase->GetCBBFileHandle(GameDataBaseDefs::CBBF_FLOORPROFILES);
	dword udOffset = m_pGDBase->GetCBBFileOffset(GameDataBaseDefs::CBBF_FLOORPROFILES,
												 szSection);  
	m_Map.pMap[TileIndex]->Floor.Init(hFile, udOffset);		
	FloorMaskAccess = LoadFloorMaskAccess(hFile, udOffset);	  
  }
  ASSERT_MSG(m_Map.pMap[TileIndex]->Floor.IsInitOk(), "Problemas creando Floor");

  // Se establece mascara de acceso
  SetMaskFloorAccess(TilePos, FloorMaskAccess); 
  
  // Se lee elevacion
  RulesDefs::Elevation Elevation;
  udAreaOffset += m_pFileSys->Read(hAreaFile,
								   (sbyte *)(&Elevation),
								   sizeof(RulesDefs::Elevation),
								   udAreaOffset);
  m_Map.pMap[TileIndex]->Floor.SetElevation(Elevation);

  // Items sobre el terreno
  LoadItems(hAreaFile, 
			udAreaOffset, 
			NULL,
			m_Map.pMap[TileIndex]->Floor.GetEntityType(),
			TilePos,
			bTmpAreaFile);		
  
  // Identificador de habitacion  
  AreaDefs::RoomID Room;
  udAreaOffset += m_pFileSys->Read(hAreaFile,
								   (sbyte *)(&Room),
								   sizeof(AreaDefs::RoomID),
								   udAreaOffset);
  // ¿Se hallo identificador?
  if (Room) {	
	// Si, luego se registra a la celda como perteneciente a habitacion
	const RoomInfoMapIt RoomInfoIt(m_Map.RoomInfo.find(Room));
	ASSERT((RoomInfoIt != m_Map.RoomInfo.end()) != 0);
	RoomInfoIt->second.Cells.insert(TileIndex);
  } 
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este metodo servira para cargar el posible techo asociado a una celda
//   que posee un identificador de habitacion.
// Parametros:
// - hAreaFile. Fichero de datos del area.
// - udAreaOffset. Offset al fichero de datos del area.
// - TileIndex. Index para localizar la celda donde insertar el roof
// - bTmpAreaFile. Indica si se esta cargando, o no, desde un archivo temporal.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CArea::LoadRoofs(const FileDefs::FileHandle& hAreaFile,
				 dword& udAreaOffset,				
				 const AreaDefs::TileIndex& TileIndex,
				 const bool bTmpAreaFile)
{
  // SOLO si parametros correctos
  ASSERT(hAreaFile);

  // Se crea instancia y se registra
  sNRoof* const pNRoof = new sNRoof;
  ASSERT(pNRoof);  
  const AreaDefs::EntHandle hEntity = CreateHandle(RulesDefs::ROOF);
  ASSERT(hEntity);
  ASSERT((GetEntityType(hEntity) == RulesDefs::ROOF) != 0);
  m_Map.Roofs.insert(RoofsMapValType(hEntity, pNRoof));
  
  // Se inicializa segun proceda
  if (bTmpAreaFile) {	
	pNRoof->Roof.Init(hAreaFile, udAreaOffset, hEntity);
  } else {
	std::string szSection;
	udAreaOffset += m_pFileSys->ReadStringFromBinary(hAreaFile, 
													 udAreaOffset,
											         szSection);
	dword udOffset = m_pGDBase->GetCBBFileOffset(GameDataBaseDefs::CBBF_ROOFPROFILES,
												 szSection);  
	pNRoof->Roof.Init(m_pGDBase->GetCBBFileHandle(GameDataBaseDefs::CBBF_ROOFPROFILES), 
					  udOffset, 
					  hEntity);
  }
  ASSERT_MSG(pNRoof->Roof.IsInitOk(), "Problemas creando Roof");	

  // Se lee elevacion
  RulesDefs::Elevation Elevation;
  udAreaOffset += m_pFileSys->Read(hAreaFile,
								   (sbyte *)(&Elevation),
								   sizeof(RulesDefs::Elevation),
								   udAreaOffset);
  pNRoof->Roof.SetElevation(Elevation);

  // ¿Muestra el techo lo que tiene por debajo?
  bool bFlag;
  udAreaOffset += m_pFileSys->Read(hAreaFile,
								   (sbyte *)(&bFlag),
								   sizeof(bool),
								   udAreaOffset);
  if (bFlag) {
	m_Map.ShowUnderRoofs.insert(hEntity);	
  }
  
  // Se asocia el handle a la celda
  m_Map.pMap[TileIndex]->hRoof = hEntity;    
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de cargar desde el archivo de perfiles de floors, la mascara
//   de acceso asociada a un perfil determinado y luego devolverla.
// Parametros:
// - hFile. Handle al fichero.
// - udOffset. Offset donde comenzar a cargar los datos.
// Devuelve:
// - Mascara de acceso asociada al perfil.
// Notas:
// - Cuando falte algun flag asociado a esa mascara de acceso, se entendera
//   que el acceso en esa direccion SI ES posible.
///////////////////////////////////////////////////////////////////////////////
AreaDefs::MaskTileAccess 
CArea::LoadFloorMaskAccess(const FileDefs::FileHandle& hFile,
						   dword& udOffset)
{
  // SOLO si parametros correctos
  ASSERT(hFile);

  // Se leen todos los flags de acceso
  bool bNoAccessFlags[8];
  udOffset += m_pFileSys->Read(hFile, 
							   (sbyte *)(bNoAccessFlags), 
							   sizeof(bool) * 8, 
							   udOffset);  
  
  // Si falta algun flag, significara que se podra pasar.  
  AreaDefs::MaskTileAccess FloorMask = AreaDefs::ALL_TILE_ACCESS;  
  byte ubIt = 0;
  for (; ubIt < IsoDefs::MAX_DIRECTIONS; ++ubIt) {
	// Se establece flag de obstaculizacion segun orientaciona actual
	// Nota: Las orientaciones Norte = 0, ... , Noroeste = 7
	FloorMask |= bNoAccessFlags[ubIt] ? (IsoDefs::NORTH_FLAG << ubIt) : 0;
  } 

  // Retorna la mascara
  return FloorMask;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga los datos asociados a un objeto de escenario.
// Parametros:
// - hAreaFile. Fichero de datos del area.
// - udAreaOffset. Offset al fichero de datos del area.
// - TilePos. Posicion del tile.
// - TileIndex. Indice donde localizar la informacion en el mapa.
// - bTmpAreaFile. Indica si se esta cargando, o no, desde un archivo temporal.
// Devuelve:
// - Si todo esta bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CArea::LoadSceneObjs(const FileDefs::FileHandle& hAreaFile,
					 dword& udAreaOffset,
				     const AreaDefs::sTilePos& TilePos,
				     const AreaDefs::TileIndex& TileIndex,
					 const bool bTmpAreaFile)
{
  // SOLO si parametros validos
  ASSERT(hAreaFile);

  // Se carga el numero de objetos
  word uwNumSceneObj;
  udAreaOffset += m_pFileSys->Read(hAreaFile,
						           (sbyte *)(&uwNumSceneObj),
								   sizeof(word),
								   udAreaOffset);

  // Se itera 
  for (; uwNumSceneObj > 0; uwNumSceneObj--) {
	// Se crea instancia y se obtiene handle
	sNSceneObj* const pNSceneObj = new sNSceneObj;
	ASSERT(pNSceneObj);	
	const AreaDefs::EntHandle hEntity = CreateHandle(RulesDefs::SCENE_OBJ);  
	ASSERT(hEntity);
	ASSERT((GetEntityType(hEntity) == RulesDefs::SCENE_OBJ) != 0);
	
	// Se inicializa segun proceda
	if (bTmpAreaFile) {
  	  pNSceneObj->SceneObj.Init(hAreaFile, udAreaOffset, hEntity);	  
	} else {
	  std::string szSection;
	  udAreaOffset += m_pFileSys->ReadStringFromBinary(hAreaFile, 
													   udAreaOffset,
													   szSection);
	  dword udOffset = m_pGDBase->GetCBBFileOffset(GameDataBaseDefs::CBBF_SCENEOBJPROFILES,
											       szSection);  
	  pNSceneObj->SceneObj.Init(m_pGDBase->GetCBBFileHandle(GameDataBaseDefs::CBBF_SCENEOBJPROFILES), 
								udOffset, 
								hEntity);
	}
	ASSERT_MSG(pNSceneObj->SceneObj.IsInitOk(), "Problemas creando SceneObj");

	// Se inserta en el map del area
	m_Map.SceneObjs.insert(SObjsValType(hEntity, pNSceneObj));

	// Lee posible tag asociado
	LoadEntityTag(hAreaFile, udAreaOffset, hEntity);

    // Se lee elevacion
	RulesDefs::Elevation Elevation;
	udAreaOffset += m_pFileSys->Read(hAreaFile,
									 (sbyte *)(&Elevation),
									 sizeof(RulesDefs::Elevation),
									 udAreaOffset);
	pNSceneObj->SceneObj.SetElevation(Elevation);

	// Se inserta en el escenario
    InsertWorldEntityInTile(hEntity, TilePos);

	// Luz asociada
	LoadLightValue(hAreaFile, udAreaOffset, hEntity);  

	// Se cargan posibles items en contenedor
	// Nota: Si el objeto de escenario no fuera contenedor, no se le cargara nada	
	LoadItems(hAreaFile, 
			  udAreaOffset, 
			  pNSceneObj->SceneObj.GetItemContainer(), 
			  pNSceneObj->SceneObj.GetEntityType(),
			  TilePos,
			  bTmpAreaFile);			
  } 
} 

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga las paredes que se hallen en una celda desde el archivo base de
//   area.
// Parametros:
// - hAreaFile. Fichero de datos del area.
// - udAreaOffset. Offset al fichero de datos del area.
// - TilePos. Posicion del tile.
// - TileIndex. Indice donde localizar la informacion en el mapa.
// - bTmpAreaFile. Indica si se esta cargando, o no, desde un archivo temporal.
// Devuelve:
// - Si para la orientacion recibida habia perfil true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CArea::LoadWalls(const FileDefs::FileHandle& hAreaFile,
			 	 dword& udAreaOffset,
				 const AreaDefs::sTilePos& TilePos,
				 const AreaDefs::TileIndex& TileIndex,
				 const bool bTmpAreaFile)
{
   // SOLO si parametros validos
  ASSERT(hAreaFile);

  // Se carga el numero de paredes
  word uwNumWalls;
  udAreaOffset += m_pFileSys->Read(hAreaFile,
						           (sbyte *)(&uwNumWalls),
								   sizeof(word),
								   udAreaOffset);

  // Se itera 
  for (; uwNumWalls > 0; uwNumWalls--) {
	// Se obtiene handle y crea instancia
	const AreaDefs::EntHandle hEntity = CreateHandle(RulesDefs::WALL);	
	ASSERT(hEntity);
	ASSERT((GetEntityType(hEntity) == RulesDefs::WALL) != 0);
	sNWall* pNWall = new sNWall;
	ASSERT(pNWall);

	// Se inicializa segun proceda
	if (bTmpAreaFile) {
	  pNWall->Wall.Init(hAreaFile, udAreaOffset, hEntity);
	} else {
	  std::string szSection;
	  udAreaOffset += m_pFileSys->ReadStringFromBinary(hAreaFile, 
													   udAreaOffset,
													   szSection);
	  dword udOffset = m_pGDBase->GetCBBFileOffset(GameDataBaseDefs::CBBF_WALLPROFILES,
												   szSection);  	
	  pNWall->Wall.Init(m_pGDBase->GetCBBFileHandle(GameDataBaseDefs::CBBF_WALLPROFILES), 
						udOffset, 
						hEntity);
	}
	ASSERT_MSG(pNWall->Wall.IsInitOk(), "Problemas creando pared");

	// Se inserta en el map del area
	m_Map.Walls.insert(WallsValType(hEntity, pNWall));

	// Lee posible tag asociado
	LoadEntityTag(hAreaFile, udAreaOffset, hEntity);

    // Se lee elevacion
	RulesDefs::Elevation Elevation;
	udAreaOffset += m_pFileSys->Read(hAreaFile,
									 (sbyte *)(&Elevation),
									 sizeof(RulesDefs::Elevation),
									 udAreaOffset);
	pNWall->Wall.SetElevation(Elevation);

	// Se inserta en el escenario
    InsertWorldEntityInTile(hEntity, TilePos);

	// Luz asociada
	LoadLightValue(hAreaFile, udAreaOffset, hEntity);  

	// Flag de bloqueo
	// Nota: Por defecto una pared se hallara bloqueada
	bool bFlag;
	udAreaOffset += m_pFileSys->Read(hAreaFile,
									 (sbyte *)(&bFlag),
									 sizeof(bool),
									 udAreaOffset);
	if (!bFlag) {
	  pNWall->Wall.UnblockAccess();	
	} 
  }
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga los datos asociados a criaturas
// Parametros:
// - hAreaFile. Fichero de datos del area.
// - udAreaOffset. Offset al fichero de datos del area.
// - TilePos. Posicion del tile.
// - TileIndex. Indice donde localizar la informacion en el mapa.
// - bTmpAreaFile. Indica si se esta cargando, o no, desde un archivo temporal.
// Devuelve:
// - Si se hallo datos asociados a una criatura y estos han sido cargados
//   true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CArea::LoadCriatures(const FileDefs::FileHandle& hAreaFile,
					 dword& udAreaOffset,
					 const AreaDefs::sTilePos& TilePos,
					 const AreaDefs::TileIndex& TileIndex,
					 const bool bTmpAreaFile)
{
  // SOLO si parametros validos
  ASSERT(hAreaFile);

  // Se carga el numero de criaturas
  word uwNumCriatures;
  udAreaOffset += m_pFileSys->Read(hAreaFile,
						           (sbyte *)(&uwNumCriatures),
								   sizeof(word),
								   udAreaOffset);
  // Se itera
  for (; uwNumCriatures > 0; uwNumCriatures--) {	
	// Se crea instancia y obtiene handle
	// Nota: Segun sea el flag leido, se creara criatura temporal o local
	sNCriature* pNCriature = new sNCriature;
	ASSERT(pNCriature);
	const AreaDefs::EntHandle hEntity = CreateHandle(RulesDefs::CRIATURE);
	ASSERT(hEntity);
	ASSERT((GetEntityType(hEntity) == RulesDefs::CRIATURE) != 0);
    // Se inicializa segun proceda
	if (bTmpAreaFile) {	  
	  pNCriature->Criature.Init(hAreaFile, udAreaOffset, hEntity);
	} else {
	  std::string szSection;
	  udAreaOffset += m_pFileSys->ReadStringFromBinary(hAreaFile, 
													   udAreaOffset,
													   szSection);
	  dword udOffset = m_pGDBase->GetCBBFileOffset(GameDataBaseDefs::CBBF_CRIATUREPROFILES,
												   szSection);  		  
	  pNCriature->Criature.Init(m_pGDBase->GetCBBFileHandle(GameDataBaseDefs::CBBF_CRIATUREPROFILES), 
								udOffset, 
								hEntity);	  
	}	
	ASSERT_MSG(pNCriature->Criature.IsInitOk(), "Problemas creando Criature");
	
	// Se inserta en el map del area
	m_Map.Criatures.insert(CriaturesValType(hEntity, pNCriature));
	
	// Se lee flag de criatura temporal
	bool bTmpCriature;
	udAreaOffset += m_pFileSys->Read(hAreaFile, 
									 (sbyte *)(&bTmpCriature), 
									 sizeof(bool), 
									 udAreaOffset);		
	pNCriature->Criature.SetTempCriature(bTmpCriature);
	
	// Lee posible tag asociado
	LoadEntityTag(hAreaFile, udAreaOffset, hEntity);
    
	// Se lee elevacion
	RulesDefs::Elevation Elevation;
	udAreaOffset += m_pFileSys->Read(hAreaFile,
									 (sbyte *)(&Elevation),
									 sizeof(RulesDefs::Elevation),
									 udAreaOffset);
	pNCriature->Criature.SetElevation(Elevation);
	
	// Se inserta en el escenario
	InsertWorldEntityInTile(hEntity, TilePos);
	
	// Luz asociada
	LoadLightValue(hAreaFile, udAreaOffset, hEntity);  
	
	// Se cargan posibles items en inventario
	LoadItems(hAreaFile, 
			  udAreaOffset, 
			  pNCriature->Criature.GetItemContainer(), 
			  pNCriature->Criature.GetEntityType(),
			  TilePos,
			  bTmpAreaFile);
	
	// Se cargan items equipados
	LoadEquippedItems(hAreaFile,
					  udAreaOffset,
					  &pNCriature->Criature,
					  bTmpAreaFile);
	
	// Se asocia area como observer de la criatura
	pNCriature->Criature.AddObserver(this);	
  }  
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un item a partir del perfil recibido. Al crearlo, lo mapea en el 
//   map correspondiente y devuelve el handle asociado.
// Parametros:
// - szItemProfile. Perfil del item a registrar.
// Devuelve:
// - Handle al item que ha sido creado. 0 si perfil no valido.
// Notas:
// - Este metodo tambien sera llamado cuando se carge un partida, para
//   recuperar informacion del jugador. En tal caso se simulara que se carga
//   desde un area.
///////////////////////////////////////////////////////////////////////////////
void
CArea::LoadItems(const FileDefs::FileHandle& hAreaFile,
				 dword& udAreaOffset,
				 iCItemContainer* const pContainer,
				 const RulesDefs::eEntityType& EntityType,
				 const AreaDefs::sTilePos& TilePos,
				 const bool bTmpAreaFile)
{  
  // SOLO si parametros validos
  ASSERT(hAreaFile);

  // Se lee el numero de items en inventario
  word uwNumItems;
  udAreaOffset += m_pFileSys->Read(hAreaFile,
								   (sbyte *)(&uwNumItems),
								   sizeof(word),
								   udAreaOffset);	
  
  // Se itera
  for (; uwNumItems > 0; uwNumItems--) {
	// Lee el nombre del perfil SOLO si NO es archivo temporal
	std::string szProfile;	
	if (!bTmpAreaFile) {
	  udAreaOffset += m_pFileSys->ReadStringFromBinary(hAreaFile, 
													   udAreaOffset,
													   szProfile);
	
	  // ¿Se esta trabajando con un obj. de escenario NO contenedor?
	  // Nota: Sera posible que se hayan asociado items a obj. no contenedores
	  if (RulesDefs::SCENE_OBJ == EntityType && 
		  !pContainer) {
		// No, se lee info de tag y luz y se continua en sig. iteracion
		std::string szTagTmp;
		udAreaOffset += m_pFileSys->ReadStringFromBinary(hAreaFile,										 
														 udAreaOffset,
														 szTagTmp);		
		GraphDefs::Light LightTmp;
		udAreaOffset += m_pFileSys->Read(hAreaFile,
										 (sbyte *)(&LightTmp),
										 sizeof(GraphDefs::Light),
										 udAreaOffset);		
		continue;
	  }	
	}	

	// Establece el nuevo handle
	const AreaDefs::EntHandle hItem = CreateHandle(RulesDefs::ITEM);	
	ASSERT(hItem);
	ASSERT((GetEntityType(hItem) == RulesDefs::ITEM) != 0);

	// Se crea instancia e inicializa segun proceda
	sNItem* const pNewItem = new sNItem;
	ASSERT(pNewItem);
	if (bTmpAreaFile) {
	  pNewItem->Item.Init(hAreaFile, udAreaOffset, hItem);
	} else {
	  dword udOffset = m_pGDBase->GetCBBFileOffset(GameDataBaseDefs::CBBF_ITEMPROFILES,
												   szProfile);
	  pNewItem->Item.Init(m_pGDBase->GetCBBFileHandle(GameDataBaseDefs::CBBF_ITEMPROFILES), 
						  udOffset, 
						  hItem);
	}
    ASSERT(pNewItem->Item.IsInitOk());	  
  	
	// Se establecen valores basicos y se inserta entidad en el map
	pNewItem->Item.SetOwner(0);
	pNewItem->Item.SetElevation(0);  	  
	m_Map.Items.insert(ItemsValType(hItem, pNewItem));      

	// ¿Se esta trabajando con un floor?
	if (EntityType == RulesDefs::FLOOR) {
	  // Si, luego se situa sobre el universo
  	  InsertWorldEntityInTile(hItem, TilePos);
	} else {
	  // No, luego se inserta en el contenedor
	  ASSERT(pContainer);
	  pContainer->Insert(hItem);	
	}  

	// Se carga posible tag y luz asociada	
	LoadEntityTag(hAreaFile, udAreaOffset, hItem);
	LoadLightValue(hAreaFile, udAreaOffset, hItem);  
  }   
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga los items equipados en una critura y los equipa en esta.
// Parametros:
// - hAreaFile. Handle al fichero de area.
// - udAreaOffset. Offset en donde comenzar a leer los datos
// - pCriature. Instancia a la criatura
// - bTmpAreaFile. Flag que indica si se estan cargando los datos desde un
//   fichero temporal o no.
// Devuelve:
// Notas:
// - Este metodo tambien sera llamado cuando se carge un partida, para
//   recuperar informacion del jugador. En tal caso se simulara que se carga
//   desde un area.
///////////////////////////////////////////////////////////////////////////////
void 
CArea::LoadEquippedItems(const FileDefs::FileHandle& hAreaFile,
						 dword& udAreaOffset,
						 CCriature* const pCriature,
						 const bool bTmpAreaFile)
{
  // SOLO si parametros validos
  ASSERT(hAreaFile);
  ASSERT(pCriature);

  // Se lee el numero de items equipados
  word uwNumItemEquipped;
  udAreaOffset += m_pFileSys->Read(hAreaFile,
								   (sbyte *)(&uwNumItemEquipped),
								   sizeof(word),
								   udAreaOffset);

  // Se proceden a cargar
  for (; uwNumItemEquipped > 0 ; uwNumItemEquipped--) {
    // Se lee identificador del slot
    RulesDefs::eIDEquipmentSlot Slot;
    udAreaOffset += m_pFileSys->Read(hAreaFile,
									 (sbyte *)(&Slot),
									 sizeof(RulesDefs::eIDEquipmentSlot),
									 udAreaOffset);

	// Lee el nombre del perfil SOLO si NO es archivo temporal
	if (!bTmpAreaFile) {
	  std::string szProfile;
	  udAreaOffset += m_pFileSys->ReadStringFromBinary(hAreaFile, 
													   udAreaOffset,
													   szProfile);
	}	

	// Establece el nuevo handle
	const AreaDefs::EntHandle hItem = CreateHandle(RulesDefs::ITEM);	
	ASSERT(hItem);
	ASSERT((GetEntityType(hItem) == RulesDefs::ITEM) != 0);

	// Se crea instancia e inicializa segun proceda
	sNItem* const pNewItem = new sNItem;
	ASSERT(pNewItem);
	if (bTmpAreaFile) {
	  pNewItem->Item.Init(hAreaFile, udAreaOffset, hItem);
	} else {
	  std::string szSection;
	  udAreaOffset += m_pFileSys->ReadStringFromBinary(hAreaFile, 
													   udAreaOffset,
													   szSection);
	  dword udOffset = m_pGDBase->GetCBBFileOffset(GameDataBaseDefs::CBBF_ITEMPROFILES,
												   szSection);
	  pNewItem->Item.Init(m_pGDBase->GetCBBFileHandle(GameDataBaseDefs::CBBF_ITEMPROFILES), 
						  udOffset, 
						  hItem);
	}	
	ASSERT(pNewItem->Item.IsInitOk());	  

	// Se establecen valores basicos y se inserta entidad en el map
	pNewItem->Item.SetOwner(0);
	pNewItem->Item.SetElevation(0);  	  
	m_Map.Items.insert(ItemsValType(hItem, pNewItem));      

	// Se inserta el item en el slot y se equipa
	pCriature->InitEquipmentSlots(hItem, Slot); 	

  	// Se carga posible tag y luz asociada
	LoadEntityTag(hAreaFile, udAreaOffset, hItem);
	LoadLightValue(hAreaFile, udAreaOffset, hItem);  
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo que se encarga de crear handles a partir del tipo recibido. En la
//   creacion de handles se localizaran posibles valores de forma aleatoria que
//   no debera de existir previamente.
// Parametros:
// - EntityType. Tipo de entidad.
// Devuelve:
// - Handle creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
AreaDefs::EntHandle 
CArea::CreateHandle(const RulesDefs::eEntityType& EntityType)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Segun sea el tipo, se debera de trabajar con un map
  const AreaDefs::EntHandle hEntity = (EntityType << 12);
  word uwBaseValue = 0;
  iCMathUtil* const pMathUtil = SYSEngine::GetMathUtil();
  ASSERT(pMathUtil);	  	  
  pMathUtil->GenSeed();
  const word uwMinValue = 0;
  const word uwMaxValue = 0x0FFF - 1;
  switch(EntityType) {
	case RulesDefs::SCENE_OBJ: {
	  // Objetos de escenario
	  do {
		uwBaseValue = pMathUtil->GetRandValue(uwMinValue, uwMaxValue);
	  } while (m_Map.SceneObjs.find(hEntity + uwBaseValue) != m_Map.SceneObjs.end());
	} break;

	case RulesDefs::CRIATURE: {
	  // Npcs
	  do {
		uwBaseValue = pMathUtil->GetRandValue(uwMinValue, uwMaxValue);
	  } while (m_Map.Criatures.find(hEntity + uwBaseValue) != m_Map.Criatures.end());
	} break;

	case RulesDefs::WALL: {
	  // Paredes
	  do {
		uwBaseValue = pMathUtil->GetRandValue(uwMinValue, uwMaxValue);
	  } while (m_Map.Walls.find(hEntity + uwBaseValue) != m_Map.Walls.end());
	} break;

	case RulesDefs::ITEM: {
	  // Items
	  do {
		uwBaseValue = pMathUtil->GetRandValue(uwMinValue, uwMaxValue);
	  } while (m_Map.Items.find(hEntity + uwBaseValue) != m_Map.Items.end());
	} break;

	case RulesDefs::ROOF: {
	  // Techos
	  do {
		uwBaseValue = pMathUtil->GetRandValue(uwMinValue, uwMaxValue);
	  } while (m_Map.Roofs.find(hEntity + uwBaseValue) != m_Map.Roofs.end());
	} break;

	default:
	  ASSERT(0);
  }; // ~ switch

  // Se retorna valor
  return (hEntity + uwBaseValue); 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Leera el tag asociado a una entidad y lo incorporara al map de tags.
//   Siempre se cumplira que si el tag es un string vacio no se insertara
//   en el map y que si no lo es, es un tag que no se haya asociado a una
//   entidad. 
// Parametros:
// - hAreaFile. Handle al fichero.
// - udAreaOffset. Offset al fichero.
// - hEntity. Entidad a la que asociar el tag
// Devuelve:
// Notas:
// - El prefijo del parser estara configurado con el necesario para leer los
//   datos de la entidad en concreto.
// - Los tags siempre se leeran desde disco en minusculas.
///////////////////////////////////////////////////////////////////////////////
void 
CArea::LoadEntityTag(const FileDefs::FileHandle& hAreaFile,
					 dword& udAreaOffset,					  
					 const AreaDefs::EntHandle& hEntity)
{
  // SOLO si parametros validos
  ASSERT(hAreaFile);
  ASSERT(hEntity);

  // Se carga el posible tag
  std::string szTag;
  udAreaOffset += m_pFileSys->ReadStringFromBinary(hAreaFile, 
												   udAreaOffset, 
												   szTag);

  // Se inserta
  InsertTag(hEntity, szTag);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inserta un tag asociado a una entidad. El tag si ya existe sera 
//   sobreescrito.
// Parametros:
// - hEntity. Handle asociado a la entidad con el tag.
// - szTag. Tag a insertar.
// Devuelve:
// Notas:
// - Si el tag esta vacio, no se insertara nada.
///////////////////////////////////////////////////////////////////////////////
void 
CArea::InsertTag(const AreaDefs::EntHandle& hEntity,
				 const std::string& szTag) 
{
  ASSERT(hEntity);	

  // ¿Se paso tag con contenido?
  if (!szTag.empty()) {
	// Si, se pasa a minusculas
    std::string szTagLower(szTag);
    SYSEngine::MakeLowercase(szTagLower);

	// ¿Ya existia el tag?
    const TagMapIt It(m_EntityTags.find(szTagLower));
	if (It != m_EntityTags.end()) {
	  // Si, se cambia handle asociado
	  It->second = hEntity;
	} else {
	  // No, se inserta por primera vez
	  m_EntityTags.insert(TagMapValType(szTagLower, hEntity));
	}
  }
}  

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga el posible valor luminoso asociado a una entidad desde el archivo
//   basico de areas.
// Parametros:
// - hAreaFile. Handle al fichero.
// - udAreaOffset. Offset al fichero.
// - hEntity. Entidad a la que asociar luz.
// Devuelve:
// Notas:
// - El prefijo del parser estara configurado con el necesario para leer los
//   datos de la entidad en concreto.
///////////////////////////////////////////////////////////////////////////////
void
CArea::LoadLightValue(const FileDefs::FileHandle& hAreaFile,
					  dword& udAreaOffset,
					  const AreaDefs::EntHandle& hEntity)
{
  // SOLO si parametros validos
  ASSERT(hAreaFile);
  ASSERT(hEntity);

  // Se carga luz asociada y se establece en la entidad recibida
  GraphDefs::Light Light;
  udAreaOffset += m_pFileSys->Read(hAreaFile, 
								   (sbyte *)(&Light), 
								   sizeof(GraphDefs::Light), 
								   udAreaOffset);
  SetLight(hEntity, Light);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Completa los datos de un archivo de partida salvada, almacenando toda 
//   la informacion realativa al jugador y a las areas temporales.
// Parametros:
// - hFile. Handle al fichero que hace de partida guardada.
// - udOffset. Offset donde comenzar a depositar los datos.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CArea::SaveGame(const FileDefs::FileHandle& hFile,
				dword& udOffset)
{
  // SOLO si instancia inicializada y cargada
  ASSERT(IsInitOk());
  ASSERT(IsAreaLoaded());
  // SOLO si parametros validos
  ASSERT(hFile);

  // Identificador del area actual
  udOffset += m_pFileSys->Write(hFile, 
								udOffset, 
								(sbyte *)(&m_Map.uwID), 
								sizeof(word));
    
  // Offset temporal con el numero de ficheros de areas temporal
  const dword udNumTmpFileAreasOffset = udOffset;
  word uwNumTmpFileAreas = 0;
  udOffset += m_pFileSys->Write(hFile, 
								udOffset, 
								(sbyte *)(&uwNumTmpFileAreas), 
								sizeof(word));
 
  // Offset temporal con el tamaño del fichero mas grande
  const dword udFileBiggerOffset = udOffset;
  dword udBuffSize = 0;
  udOffset += m_pFileSys->Write(hFile, 
								udOffset, 
								(sbyte *)(&udBuffSize), 
								sizeof(dword));

  // Antes de proceder a copiar ficheros, se guardara el area actual
  SaveAreaInfo(false);

  // Se proceden a copiar los ficheros
  sbyte* pBuff = NULL;
  word uwIt = 0;
  word uwNumAreaTmpFiles = 0;
  for (; uwIt < 0xFF; ++uwIt) {
	// ¿Existe fichero de area base?
	std::string szFileName;
	SYSEngine::PassToString(szFileName, 
							"%sArea%u.cbb", 
							m_pGDBase->GetAreaFilesPath().c_str(), uwIt);
	FileDefs::FileHandle hAreaFile = m_pFileSys->Open(szFileName);
	if (hAreaFile) {
	  // Si, se cierra fichero de area base
	  m_pFileSys->Close(hAreaFile);

	  // ¿Existe fichero temporal asociado?
	  SYSEngine::PassToString(szFileName, "tmp\\Area%utmp.cbb", uwIt);
	  hAreaFile = m_pFileSys->Open(szFileName);
	  if (hAreaFile) {		
		// Si, luego se copia al fichero de partida guardada
		// Identificador
		udOffset += m_pFileSys->Write(hFile, udOffset, (sbyte *)(&uwIt), sizeof(word));
		
		// Tamaño fichero
		const dword udFileSize = m_pFileSys->GetFileSize(hAreaFile); 
		udOffset += m_pFileSys->Write(hFile, udOffset, (sbyte *)(&udFileSize), sizeof(dword));
		
		// Datos
		// Se aloja fichero temporal en un buffer y luego se vuelca
		if (udBuffSize < udFileSize) {
		  delete[udBuffSize] pBuff;
		  pBuff = new sbyte[udFileSize];
		  ASSERT(pBuff);
		  udBuffSize = udFileSize;
		}
		m_pFileSys->Read(hAreaFile, pBuff, udFileSize, 0);		
		udOffset += m_pFileSys->Write(hFile, udOffset, pBuff, udFileSize);

		// Se cierra fichero temporal y se incrementa contador
		m_pFileSys->Close(hAreaFile);
		++uwNumAreaTmpFiles;
	  }	  
	} else {
	  // No, se abandona bucle
	  break;
	}
  }  

  // Se libera buffer de lectura utilizado
  delete[udBuffSize] pBuff;

  // Se guarda el numero de ficheros
  m_pFileSys->Write(hFile, 
					udNumTmpFileAreasOffset, 
					(sbyte *)(&uwNumAreaTmpFiles), 
					sizeof(word));

  // Se guarda el tamaño del fichero mas grande
  m_pFileSys->Write(hFile, 
					udFileBiggerOffset, 
					(sbyte *)(&udBuffSize), 
					sizeof(dword));
    
  // Almacena los datos relativos al jugador
  // Posicion en area actual
  udOffset += m_pFileSys->Write(hFile, 
								udOffset, 
								(sbyte *)(&m_PlayerInfo.pPlayer->GetTilePos()), 
								sizeof(AreaDefs::sTilePos));
  // Datos del jugador
  ASSERT(m_PlayerInfo.pPlayer);
  m_PlayerInfo.pPlayer->Save(hFile, udOffset);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga los datos desde disco.
// Parametros:
// - hFile. Handle al fichero que hace de partida cargada
// - udOffset. Offset donde comenzar a cargar los datos.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CArea::LoadGame(const FileDefs::FileHandle& hFile,
				dword& udOffset)
{
  // SOLO si instancia inicializada y cargada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hFile);

  // ¿Hay area cargada?
  if (IsAreaLoaded()) {
	// Desvincula jugador y libera informacion de la misma
    UnsetPlayer();
    FreeAreaInfo(true);  	  
  }
  
  // Carga el identificador de area actual
  word uwIDActArea;
  udOffset += m_pFileSys->Read(hFile, 
	                           (sbyte *)(&uwIDActArea),
							   sizeof(word),
							   udOffset);  

  // Carga el numero de ficheros de area temporal
  word uwNumTmpFileAreas = 0;
  udOffset += m_pFileSys->Read(hFile, 
	                           (sbyte *)(&uwNumTmpFileAreas),
							   sizeof(word),
							   udOffset);  

  // Carga el tamaño del fichero mas grande
  dword udBuffSize = 0;
  udOffset += m_pFileSys->Read(hFile, 
	                           (sbyte *)(&udBuffSize),
							   sizeof(dword),
							   udOffset);  

  // Crea el buffer para la carga de ficheros
  sbyte* pBuff = new sbyte[udBuffSize];
  ASSERT(pBuff);

  // Procede a volcar los ficheros temporales  
  for (; uwNumTmpFileAreas > 0 ; uwNumTmpFileAreas--) {
	// Lee identificador fichero y crea el nombre
	word uwIDTmpAreaFile;
	udOffset += m_pFileSys->Read(hFile, 
	                           (sbyte *)(&uwIDTmpAreaFile),
							   sizeof(word),
							   udOffset);  
	std::string szFileName;
	SYSEngine::PassToString(szFileName, "tmp\\area%utmp.cbb", uwIDTmpAreaFile);

	// Lee tamaño del fichero y prepara buffer
	dword udTmpAreaFileSize;
	udOffset += m_pFileSys->Read(hFile, 
	                           (sbyte *)(&udTmpAreaFileSize),
							   sizeof(dword),
							   udOffset);
	ASSERT((udBuffSize >= udTmpAreaFileSize) != 0);

	// Lee el contenido del fichero y lo vuelca en uno nuevo
	udOffset += m_pFileSys->Read(hFile, 
	                             pBuff,
							     udTmpAreaFileSize,
							     udOffset);  
	const FileDefs::FileHandle hTmpFile = m_pFileSys->Open(szFileName, true);
	ASSERT(hTmpFile);
	m_pFileSys->Write(hTmpFile, 0, pBuff, udTmpAreaFileSize);
	m_pFileSys->Close(hTmpFile);
  }

  // Libera buffer temporal
  delete[udBuffSize] pBuff;
  pBuff = NULL;

  // Carga el area leido como actual 
  LoadArea(uwIDActArea);

  // Carga los datos del jugador
  LoadPlayer(hFile, udOffset);

  // Se inicializan resto de vbles de miembro
  m_FloorSelector.pSelector = NULL;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga toda la informacion referida al jugador, creando la instancia al
//   mismo y localizandolo en el juego.
// Parametros:
// - hAreaFile. Handle al fichero del area.
// - udAreaOffset. Offset desde donde comenzar a leer.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CArea::LoadPlayer(const FileDefs::FileHandle& hFile,
				  dword& udOffset)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hFile);

  // Carga pos. jugador en area actual 
  AreaDefs::sTilePos PlayerPos;
  udOffset += m_pFileSys->Read(hFile, 
	                           (sbyte *)(&PlayerPos), 
								sizeof(AreaDefs::sTilePos),
								udOffset);

  // Crea instancia al jugador, carga datos basicos
  CPlayer* const pPlayer = new CPlayer;
  ASSERT(pPlayer);
  pPlayer->Init(hFile, udOffset);
  ASSERT(pPlayer->IsInitOk());

  // Se asocia jugador
  SetPlayer(pPlayer, PlayerPos);      

  // Se carga el flag de tipo de criatura, que seguro es NO temporal
  bool bTmpCriature;
  udOffset += m_pFileSys->Read(hFile,
							   (sbyte *)(&bTmpCriature),
							   sizeof(bool),
							   udOffset);
  pPlayer->SetTempCriature(false);

  // Lee posible tag asociado. Es seguro que NO existira 
  LoadEntityTag(hFile, udOffset, pPlayer->GetHandle());

  // Elevacion
  RulesDefs::Elevation Elevation;
  udOffset += m_pFileSys->Read(hFile,
							   (sbyte *)(&Elevation),
							   sizeof(RulesDefs::Elevation),
							   udOffset);
  pPlayer->SetElevation(Elevation);

  // Luz asociada
  // Nota: La luz se leera ahora pero se establecera cuando el jugador se asocie
  // al area. De no hacerlo asi, habra errores.
  GraphDefs::Light Light;
  udOffset += m_pFileSys->Read(hFile, 
							   (sbyte *)(&Light), 
							    sizeof(GraphDefs::Light), 
							    udOffset);    

  // Se cargan posibles items en inventario
  // Nota: Se simulara una carga desde archivo temporal
  LoadItems(hFile, udOffset, pPlayer->GetItemContainer(), RulesDefs::PLAYER, PlayerPos,	true);
  
  // Se cargan items equipados
  // Nota: Se simulara una carga desde archivo temporal
  LoadEquippedItems(hFile, udOffset, pPlayer, true);
  
  // Se establece valor luminoso
  SetLight(pPlayer->GetHandle(), Light);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece luz dinamica en una entidad o la quita si el valor recibido
//   es de 0.
// Parametros:
// - hEntity. Entidad a la que asociar luz.
// - LightIntensity. Intensidad de la luz. 
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CArea::SetLight(const AreaDefs::EntHandle& hEntity,
				const GraphDefs::Light& LightIntensity)
{
  // SOLO si mapa cargado o cargando
  ASSERT((IsAreaLoaded() || IsAreaLoading()) != 0);
  // SOLO si parametros correctos
  ASSERT(hEntity);

  // ¿Se desea desvincular luz?
  if (0 == LightIntensity) {
	// Si, ¿Existe entrada para el foco recibido?
	if (m_DinamicLightInfo.LightFocus.find(hEntity) != m_DinamicLightInfo.LightFocus.end()) {
	  RemoveLight(hEntity, false);
	}
  } else {
	// No, ¿Ya existe la entidad como foco de luz?
	if (m_DinamicLightInfo.LightFocus.find(hEntity) != m_DinamicLightInfo.LightFocus.end()) {
	  // Si, luego se querra MODIFICAR su valor, ANTES se anulara
	  SetLight(hEntity, 0);
	}
	
	// Se creara como foco de luz
	SetLightFocusInArea(hEntity, LightIntensity, false, true);	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina la informacion asociada a un foco de luz. Al eliminar un foco de
//   luz, se debera de recorrer los tiles a los que afecte y recalcular posibles
//   incidencias de luz sobre ellos.
// Parametros:
// - hEntity. Entidad a la que asociar luz.
// - bUpdatePos. Indica si se esta actualizando o no una posicion.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CArea::RemoveLight(const AreaDefs::EntHandle& hEntity,
				   const bool bUpdatePos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());
  // SOLO si parametros correctos
  ASSERT(hEntity);

  // Se toma entrada asociada al foco
  const LightFocusInfoMapIt FocusIt(m_DinamicLightInfo.LightFocus.find(hEntity));
  ASSERT((FocusIt != m_DinamicLightInfo.LightFocus.end()) != 0);
  
  // Se calcula la region que abarca el foco
  AreaDefs::sTilePos InitTilePos, EndTilePos;
  CalculeLightFocusRegion(FocusIt->second.TilePos, 
						  FocusIt->second.LightIntensity,
						  InitTilePos,
						  EndTilePos);

  // Estructura de luz vacia para comparar
  const GraphDefs::sLight CmpLightValue;

  // Se recorre la region que abarca el foco y para cada posicion, se obtendra
  // la posible entrada asociada con informacion de iluminacion.
  AreaDefs::sTilePos TilePosIt(InitTilePos);
  for (; TilePosIt.YTile < EndTilePos.YTile; ++TilePosIt.YTile) {
	TilePosIt.XTile = InitTilePos.XTile;
	for (; TilePosIt.XTile <= EndTilePos.XTile; ++TilePosIt.XTile) {
	  // Se crea indice asociado a la posicion
	  const AreaDefs::TileIndex TileIdx = GetTileIdx(TilePosIt);
	  // ¿Hay nodo de info. luminosa asociada a la posicion?
	  const LightInTilePosInfoMapIt LightInfoIt(m_DinamicLightInfo.LightInTilePos.find(TileIdx));
	  if (LightInfoIt != m_DinamicLightInfo.LightInTilePos.end()) {
		// Si, para cada vertice, se hallara la entrada del foco y se eliminara
		// recalculando el valor maximo de luz en el vertice si fuera necesario
		byte ubVertex = 0;
		for (; ubVertex < 4; ++ubVertex) {
		  // ¿Hay informacion del foco asociada?
		  const VertexTileLightMapIt FocusLightIt(LightInfoIt->second.LightsInVertex[ubVertex].find(hEntity));
		  if (FocusLightIt != LightInfoIt->second.LightsInVertex[ubVertex].end()) {
			// Se borra nodo de informacion, guardando su valor de luz
			const GraphDefs::Light FocusLight = FocusLightIt->second;
			LightInfoIt->second.LightsInVertex[ubVertex].erase(FocusLightIt);
			// ¿Hay que recalcular el valor maximo de luz?
		    // Nota: lo sera si el valor del luz es igual a la intensidad del foco.
		    // (no podra ser mayor, ya que el valor mayor siempre ser el valor
		    // de luz asociado al vertice).			
		    if (LightInfoIt->second.TileLight.VertexLight[ubVertex] == FocusLight) {
			  // Si, se calcula cual es la intensidad de luz asociado al vertice
			  CalculeAndSetMaxLightInTilePos(LightInfoIt, ubVertex);
			}		  			
		  }		  
		} // ~ for

		// ¿La luz que incide sobre la posicion es 0 en todos los vertices?
		if (LightInfoIt->second.TileLight == CmpLightValue) {
		  // Si, luego ya no hay informacion y se borrara el nodo
		  m_DinamicLightInfo.LightInTilePos.erase(LightInfoIt);
		}
	  }
	}	
  }

  // ¿NO se estaba actualizando una posicion?
  if (!bUpdatePos) {
	// No se estaba, luego se borrara la entrada del foco luminoso
	m_DinamicLightInfo.LightFocus.erase(FocusIt);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece un foco luminoso recorriendo la region en donde se establece 
//   para ajustar valores. En caso de que se reciba el flag bNewFocus levantado,
//   significara que el foco es nuevo y se debera de crear una entrada para
//   el mismo. En caso contrario, que se trata de una modificacion de intensidad.
// - Cuando el establecimiento se deba a una actualizacion, se debera de recorrer
//   el rango que comprende la posicion original de la entidad y la nueva, 
//   realizando identicas operaciones.
// Parametros:
// - hEntity. Entidad a la que asociar luz.
// - LightIntensity. Intensidad de la luz. 
// - bUpdatePos. Flag que indica si el establecimiento de luz se debe a una
//   actualizacion de posicion. Cuando esto ocurra, el valor bNewFocus DEBE
//   de valer false.
// - bNewFocus. Flag de nuevo foco.
// Devuelve:
// Notas:
// - 
///////////////////////////////////////////////////////////////////////////////
void 
CArea::SetLightFocusInArea(const AreaDefs::EntHandle& hEntity,
						   const GraphDefs::Light& LightIntensity, 
						   const bool bUpdatePos,
						   const bool bNewFocus)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());
  // SOLO si parametros validos
  ASSERT(hEntity);
  ASSERT(LightIntensity);
#ifdef _SYSASSERT
  if (bUpdatePos) {
	ASSERT(!bNewFocus);
  }
#endif

  // Se obtiene instancia al foco
  CWorldEntity* const pEntity = GetWorldEntity(hEntity);
  ASSERT(pEntity);

  // ¿El foco luminoso es nuevo?
  if (bNewFocus) {
	const sLightFocusInfo LightFocusInfo(LightIntensity, pEntity->GetTilePos());
	m_DinamicLightInfo.LightFocus.insert(LightFocusInfoMapValType(hEntity, LightFocusInfo));
  }

  // ¿El foco NO afecta al area?
  if (!CanEntityFocusAffectInMap(hEntity)) {
	return;
  }

  // Estructura de luz vacia para comparar
  const GraphDefs::sLight CmpLightValue;

  // Se halla la posicion universal del foco
  sPosition WorldFocusPos = m_pWorld->PassTilePosToWorldPos(pEntity->GetTilePos());
  WorldFocusPos.swXPos += IsoDefs::TILE_WIDTH_DIV;
  WorldFocusPos.swYPos += IsoDefs::TILE_HEIGHT_DIV;

  // Se calcula la region que abarca el foco
  AreaDefs::sTilePos InitTilePos, EndTilePos;
  CalculeLightFocusRegion(pEntity->GetTilePos(),
						  LightIntensity,
						  InitTilePos,
						  EndTilePos);

  // ¿Es una actualizacion de posicion?
  if (bUpdatePos) {
	// Se tiene en cuenta las posiciones originales del foco
	const LightFocusInfoMapIt FocusIt(m_DinamicLightInfo.LightFocus.find(hEntity));
	ASSERT((FocusIt != m_DinamicLightInfo.LightFocus.end()) != 0);
	
	// Se halla rango en posiciones previas y se recalculan posiciones del actual
    AreaDefs::sTilePos TmpInitTilePos, TmpEndTilePos;
	CalculeLightFocusRegion(FocusIt->second.TilePos,
							LightIntensity,
							TmpInitTilePos,
							TmpEndTilePos);
	if (InitTilePos.XTile > TmpInitTilePos.XTile) {
	  InitTilePos.XTile = TmpInitTilePos.XTile;
	}
	if (InitTilePos.YTile > TmpInitTilePos.YTile) {
	  InitTilePos.YTile = TmpInitTilePos.YTile;
	}
	if (EndTilePos.XTile < TmpEndTilePos.XTile) {
	  EndTilePos.XTile = TmpEndTilePos.XTile;
	}
	if (EndTilePos.YTile < TmpEndTilePos.YTile) {
	  EndTilePos.YTile = TmpEndTilePos.YTile;
	}  	

	// Se actualiza la info de posicion en el nodo de informacion del foco luminoso
	FocusIt->second.TilePos = pEntity->GetTilePos();	
  }

  // Se recorre la region que abarca el foco y para cada posicion, se obtendra
  // la posible entrada asociada con informacion de iluminacion.
  AreaDefs::sTilePos TilePosIt(InitTilePos);
  for (; TilePosIt.YTile < EndTilePos.YTile; ++TilePosIt.YTile) {
	TilePosIt.XTile = InitTilePos.XTile;
	for (; TilePosIt.XTile <= EndTilePos.XTile; ++TilePosIt.XTile) {
	  // Se crea indice asociado a la posicion
	  const AreaDefs::TileIndex TileIdx = GetTileIdx(TilePosIt);
	  
	  // Se toma iterador a nodo de informacion luminosa asociada a la posicion
	  LightInTilePosInfoMapIt LightInfoIt(m_DinamicLightInfo.LightInTilePos.find(TileIdx));
	  
	  // ¿NO hay info. luminosa?
	  if (LightInfoIt == m_DinamicLightInfo.LightInTilePos.end()) {	
		// Se crea, se inserta en el map y se asocia iterador
		sLightInTilePosInfo LightInTilePosInfo(m_pWorld->PassTilePosToWorldPos(TilePosIt));
		m_DinamicLightInfo.LightInTilePos.insert(LightInTilePosInfoMapValType(TileIdx, LightInTilePosInfo));
		LightInfoIt = m_DinamicLightInfo.LightInTilePos.find(TileIdx);
	  }

	  // Se halla la posicion de cada uno de los vertices del tile en
	  // terminos de posicion universal.	  
	  sPosition VertexPos[4];
	  ASSERT((LightInfoIt != m_DinamicLightInfo.LightInTilePos.end()) != 0);
	  CalculeWorldVertexOfTilePos(LightInfoIt, VertexPos);

	  // Se halla la intensidad de luz proviniente del foco que alcanza
	  // a cada uno de los vertices de la posicion sobre la que trabajar
	  iCMathUtil* const pMathUtil = SYSEngine::GetMathUtil();
	  ASSERT(pMathUtil);
	  byte ubVertex = 0;
	  for (; ubVertex < 4; ++ubVertex) {
		// Se halla distancia del foco al vertice
		const sword swDistance = pMathUtil->GetEuclideanDistance(VertexPos[ubVertex],
																 WorldFocusPos);
		
		// ¿Es mayor la intensidad luminosa del foco que la distancia?
		GraphDefs::Light NewVertexLight;
		if (swDistance < LightIntensity) {
		  // Si, se halla la diferencia entre la distancia y la intensidad
		  NewVertexLight = LightIntensity - swDistance;		  
		} else {
		  // La luz no llega con la minima intensidad al vertice
		  NewVertexLight = 0;		  
		}

		// Se obtiene iterador al nodo con info asociada al foco
		const VertexTileLightMapIt FocusIt(LightInfoIt->second.LightsInVertex[ubVertex].find(hEntity));

		// ¿Hay informacion luminosa para ese vertice?
		if (NewVertexLight > 0) {
		  // ¿Habia nodo de informacion asociado al foco?
		  if (FocusIt != LightInfoIt->second.LightsInVertex[ubVertex].end()) {
			// Se reemplaza el por el nuevo valor, guardando el previo
			const GraphDefs::Light PrevLight = FocusIt->second;
			FocusIt->second = NewVertexLight;
			// ¿El valor asociado era igual al previo? Y
			// ¿El valor nuevo es MAYOR que el previo?
			if (PrevLight == LightInfoIt->second.TileLight.VertexLight[ubVertex] &&
				PrevLight < NewVertexLight) {
			  // Se sustituye el valor de luz por el nuevo
			  LightInfoIt->second.TileLight.VertexLight[ubVertex] = NewVertexLight;
			} else {
			  // Se debe de recalcular el valor maximo
			  CalculeAndSetMaxLightInTilePos(LightInfoIt, ubVertex);
			} 			
		  } else {
			// No habia nodo de informacion:
			// Se inserta un nuevo nodo con la informacion
			LightInfoIt->second.LightsInVertex[ubVertex].insert(VertextTileLightMapValType(hEntity, NewVertexLight));
			// ¿El valor es mayor que el actualmente configurado en la posicion?
			if (NewVertexLight > LightInfoIt->second.TileLight.VertexLight[ubVertex]) {
			  // Si, luego se reemplaza
			  LightInfoIt->second.TileLight.VertexLight[ubVertex] = NewVertexLight;
			}
		  }		  
		} else {		  
		  // No hay informacion luminosa:
		  // ¿Habia nodo de informacion asociado al foco?
		  if (FocusIt != LightInfoIt->second.LightsInVertex[ubVertex].end()) {
			// Se borra nodo de informacion, guardando el valor previo de luz
			const GraphDefs::Light PrevLight = FocusIt->second;
			LightInfoIt->second.LightsInVertex[ubVertex].erase(FocusIt);
			// ¿El valor del foco ERA el mismo que el que esta establecido?
			if (LightInfoIt->second.TileLight.VertexLight[ubVertex] == PrevLight) {
			  // Se debe de recalcular el valor maximo
			  CalculeAndSetMaxLightInTilePos(LightInfoIt, ubVertex);
			}
		  }	
		}
	  } // ~ for
	  
      // ¿No hay luz que incida en la posicion?
	  if (LightInfoIt->second.TileLight == CmpLightValue) {
		// No, luego ya no hay informacion luminosa y se borrara el nodo
		m_DinamicLightInfo.LightInTilePos.erase(LightInfoIt);
	  }
	}	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de comprobar si la entidad recibida, que es un foco, es o no
//   afectable en el mapa del area. Principalmente sera la realizacion de la
//   comprobacion de si la entidad es un item o no. En caso de que lo sea,
//   solo afectara si no tiene dueño o si lo tiene y es criatura con el 
//   propio item equipado.
// Parametros:
// - hEntity. Foco de luminosidad.
// Devuelve:
// - Si la entidad afecta al mapa true. En caso contrario false.
// Notas:
// - Metodo de apoyo a SetLightFocusInArea.
///////////////////////////////////////////////////////////////////////////////
bool
CArea::CanEntityFocusAffectInMap(const AreaDefs::EntHandle& hEntity)
{
  // ¿El foco es un item?
  if (GetEntityType(hEntity) == RulesDefs::ITEM) {
	// Si, en caso de que el dueño sea un objeto de escenario o bien
	// sea una criatura y no lo tenga equipado, no se calculara la region
	// de luz asociada al mismo
	CItem* const pItem = GetItem(hEntity);
	ASSERT(pItem);
	const AreaDefs::EntHandle hEntityOwner = pItem->GetOwner();
	if (hEntityOwner) {
	  switch (GetEntityType(hEntityOwner)) {
		case RulesDefs::SCENE_OBJ: {
		  // Es un objeto de escenario
		  return false;
		} break;

  		case RulesDefs::PLAYER: 
		case RulesDefs::CRIATURE: {
		  CCriature* const pCriature = GetCriature(hEntityOwner);
		  ASSERT(pCriature);
		  if (!pCriature->GetEquipmentSlots()->IsItemEquiped(hEntity)) {
			// Es una criatura que no lo tiene equipado
			return false;
		  }
		} break;
	  }; // ~ switch
	}
  }

  // Si que afecta
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dado el nodo de informacion luminosa asociada a un tile, calcula el valor
//   maximo de luz asociado al vertice ubVertex, de entre el map de focos
//   luminosos asociado al vertice. En caso de no hallar informacion en el
//   vertice, establecera el valor 0 en el mismo.
// Parametros:
// - LightInfoIt. Iterador al nodo de informacion luminosa del vertice.
// - ubVertex. Identificador del vertice con el que trabajar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CArea::CalculeAndSetMaxLightInTilePos(const LightInTilePosInfoMapIt& LightInfoIt, 
									  const byte ubVertex)
{
  // SOLO si insancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());
  // SOLO si parametros validos
  ASSERT((LightInfoIt != m_DinamicLightInfo.LightInTilePos.end()) != 0);
  ASSERT((ubVertex < 4) != 0);

  // Busca entre el map asociado al vertice ubVertex el valor maximo de luz
  GraphDefs::Light MaxLightIntensity = 0;
  VertexTileLightMapIt VertexIt(LightInfoIt->second.LightsInVertex[ubVertex].begin());
  for (; VertexIt != LightInfoIt->second.LightsInVertex[ubVertex].end(); ++VertexIt) {
	if (VertexIt->second > MaxLightIntensity) {
	  MaxLightIntensity = VertexIt->second;
	}
  }

  // Establece el valor conseguido
  LightInfoIt->second.TileLight.VertexLight[ubVertex] = MaxLightIntensity;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Calcula la region que abarca el foco de luz y la deposita en una posicion
//   de inicio (esquina superior izquierda de la region) y en una posicion de
//   final (esquina inferior derecha de la region).
// Parametros:
// - FocusTilePos. Posicion del foco.
// - LightIntensity. Intensidad de la luz.
// - InitRegionTilePos. Posicion inicial de la region.
// - EndRegionTilePos. Posicion final de la region.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CArea::CalculeLightFocusRegion(const AreaDefs::sTilePos& FocusTilePos,
							   const GraphDefs::Light& LightIntensity, 
							   AreaDefs::sTilePos& InitRegionTilePos, 
							   AreaDefs::sTilePos& EndRegionTilePos)
{ 
  // SOLO si instancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());

  // Se hallan deltas
  const float fExtraPos = 2.5f;
  const word uwXDelta = (float(LightIntensity) / float(IsoDefs::TILE_WIDTH)) + fExtraPos;
  const word uwYDelta = (float(LightIntensity) / float(IsoDefs::TILE_HEIGHT_DIV)) + fExtraPos;

  // Se halla el tile inicial   
  sword swTmpPos = FocusTilePos.XTile - uwXDelta;
  InitRegionTilePos.XTile = (swTmpPos < 0) ? 0 : swTmpPos;  
  swTmpPos = FocusTilePos.YTile - uwYDelta;
  InitRegionTilePos.YTile = (swTmpPos < 0) ? 0 : swTmpPos;

  // Se halla el tile final
  swTmpPos = FocusTilePos.XTile + uwXDelta;
  EndRegionTilePos.XTile = (swTmpPos >= GetWidth()) ? GetWidth() - 1 : swTmpPos;
  swTmpPos = FocusTilePos.YTile + uwYDelta;
  EndRegionTilePos.YTile = (swTmpPos >= GetHeight()) ? GetHeight() - 1 : swTmpPos;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el valor maximo de luz que incide sobre una determinada posicion.
//   Debemos de tener en cuenta que las posiciones no podran tener luces
//   asociadas via SetLight pero que si podran recibir incidencia de la misma.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
GraphDefs::Light 
CArea::GetLightAt(const AreaDefs::sTilePos& TilePos) 
{
  // SOLO si instancia inicializada 
  ASSERT(IsInitOk() && IsAreaLoaded());
  // SOLO si parametros validos
  ASSERT(IsCellValid(TilePos));
  ASSERT(IsCellWithContent(TilePos));

  // ¿Llega alguna luz a la posicion?
  GraphDefs::Light Light = 0;	
  const LightInTilePosInfoMapIt LightIt(m_DinamicLightInfo.LightInTilePos.find(GetTileIdx(TilePos)));
  if (LightIt != m_DinamicLightInfo.LightInTilePos.end()) {
	// Si, se obtiene el valor maximo para cada uno de los vertices
	byte ubIt = 0;
	for (; ubIt < 4; ++ubIt) {
	  if (Light < LightIt->second.TileLight.VertexLight[ubIt]) {
		Light = LightIt->second.TileLight.VertexLight[ubIt];
	  }
	}
  }

  // Retorna valor
  return Light;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado del suelo del area unicamente.
// Parametros:
// - TilePos. Posicion del tile.
// - swXScreen, swYScreen. Posicion de la pantalla en donde dibujar.
// - ubDrawFlags. Contiene los flags con los elementos que se han de dibujar
//   en pantalla.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CArea::DrawFloor(const AreaDefs::sTilePos& TilePos,
				 const sword& swXScreen, 
				 const sword& swYScreen)
{
  // SOLO si area inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());
  // SOLO si parametros validos
  ASSERT(IsCellValid(TilePos));
  	
  // Toma indice de posicion asociada al tile
  const AreaDefs::TileIndex TileIdx = GetTileIdx(TilePos);
  sNCell* const pCell = m_Map.pMap[TileIdx];

  // ¿Celda CON datos?
  if (pCell) { 
	// Se toma instancia a posible techo
	CRoof* const pRoof = pCell->hRoof ? GetRoof(pCell->hRoof) : NULL;

	// ¿NO hay techo? o
	// ¿El techo es NO visible? o
	// ¿El techo muestra lo que tiene por debajo?
	if (!pRoof ||
	    !pRoof->IsVisible() ||
		(m_Map.ShowUnderRoofs.find(pCell->hRoof) != m_Map.ShowUnderRoofs.end())) {
	  // Si, luego se dibuja Floor
	  // Se establece info luminosa para el tile
	  const LightInTilePosInfoMapIt LightIt(m_DinamicLightInfo.LightInTilePos.find(TileIdx));
	  GraphDefs::sLight& Light = (LightIt != m_DinamicLightInfo.LightInTilePos.end()) ?	
								  LightIt->second.TileLight :
								  m_DinamicLightInfo.CommonLight;
	  
	  // Se dibuja el floor
	  pCell->Floor.Draw(swXScreen, swYScreen, Light);

	  // ¿Hay que dibujar tile de seleccion?
	  if (m_FloorSelector.pSelector &&
		  TilePos == m_FloorSelector.TilePos) {
		// Si, se dibuja en la misma posicion y elevacion que el floor
		m_FloorSelector.pSelector->Draw(sPosition(swXScreen, swYScreen));		
	  }
	}	
  }    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado de las entidades que van por encima de una
//   determinada posicion del suelo.
// Parametros:
// - TilePos. Posicion del tile.
// - swXScreen, swYScreen. Posicion de la pantalla en donde dibujar.
// - ubDrawFlags. Contiene los flags con los elementos que se han de dibujar
//   en pantalla.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CArea::DrawOnFloor(const AreaDefs::sTilePos& TilePos,
				   const sword& swXScreen, 
				   const sword& swYScreen)
{
  // SOLO si area inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());
  // SOLO si parametros validos
  ASSERT(IsCellValid(TilePos));
  	
  // Toma indice de posicion asociada al tile
  const AreaDefs::TileIndex TileIdx = GetTileIdx(TilePos);
  sNCell* const pCell = m_Map.pMap[GetTileIdx(TilePos)];

  // ¿Celda CON datos?
  if (pCell) { 
	// Se toma instancia a posible techo
	CRoof* const pRoof = pCell->hRoof ? GetRoof(pCell->hRoof) : NULL;

	// ¿NO hay techo? o
	// ¿El techo es NO visible? o
	// ¿El techo muestra lo que tiene por debajo?
	if (!pRoof ||
	    !pRoof->IsVisible() ||
		(m_Map.ShowUnderRoofs.find(pCell->hRoof) != m_Map.ShowUnderRoofs.end())) {	  	  
	  // Se obtiene la elevacion del terreno
	  const sword swYElevationScreen = swYScreen - pCell->Floor.GetElevation();

	  // ¿Hay al menos una entidad?
	  if (!pCell->Entities.empty()) {
		// Si, se procederan a dibujar
		// Se establece info luminosa para el tile
		LightInTilePosInfoMapIt LightIt(m_DinamicLightInfo.LightInTilePos.find(TileIdx));
		GraphDefs::sLight& Light = (LightIt != m_DinamicLightInfo.LightInTilePos.end()) ?	
									LightIt->second.TileLight :
									m_DinamicLightInfo.CommonLight;

		// Obtiene los handles asociados al tile y los dibuja    
		CellEntitiesListIt It(pCell->Entities.begin());  
		for(; It != pCell->Entities.end(); ++It) {
		  // ¿HAY permiso de dibujado?	
		  // if (CanDrawWorldEntity(*It, ubDrawFlags)) { 
		  // Se obtiene la entidad y se dibuja	
		  GetWorldEntity(*It)->Draw(swXScreen, swYElevationScreen, Light);
		  //}			  
		}
	  }	
	}	

	// Se dibuja el posible techo
	if (pRoof) {
	  // Nota: El techo NO tendran en cuenta la elevacion del terreno ni la luz
	  pRoof->Draw(swXScreen, swYScreen, m_DinamicLightInfo.CommonLight);
	}
  }    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si una entidad puede dibujarse a partir de los flags de dibujado
//   recibidos. El tipo de entidad se reconocera a traves de su handle.
// Parametros:
// - hEntity. Handle de la entidad.
// - ubDrawFlags. Flags de dibujado.
// Devuelve:
// - Si la entidad puede dibujarse true. En caso contrario false.
// Notas:
// - Se considerara que los tiles base siempre se dibujan
// - Este metodo se llamara desde el de dibujado.
///////////////////////////////////////////////////////////////////////////////
bool CArea::CanDrawWorldEntity(const AreaDefs::EntHandle& hEntity, 
							   const byte ubDrawFlags)
{
  // SOLO si parametros validos
  ASSERT(hEntity);

  switch(GetEntityType(hEntity)) {
	case RulesDefs::SCENE_OBJ:
	  return (ubDrawFlags & AreaDefs::DRAW_SCENE_OBJ) ? true : false;
	  break;

	case RulesDefs::ITEM:
	  return (ubDrawFlags & AreaDefs::DRAW_ITEM) ? true : false;
	  break;

	case RulesDefs::PLAYER:
	case RulesDefs::CRIATURE:
	  return (ubDrawFlags & AreaDefs::DRAW_CRIATURE) ? true : false;
	  break;
	
	case RulesDefs::WALL:
	  return (ubDrawFlags & AreaDefs::DRAW_WALL) ? true : false;
	  break;

	case RulesDefs::ROOF:
	  return (ubDrawFlags & AreaDefs::DRAW_ROOF) ? true : false;
	  break;
  };

  // No se puede
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Creara de forma dinamica una entidad de tipo criatura. Para que se pueda 
//   crear, la posicion pasada debe de ser valida y el valor del perfil debe 
//   de estar disponible.
// - A la hora de crear la criatura, se podra utilizar la creacion temporal
//   o la creacion local de la misma con respecto al area.
// Parametros:
// - TilePos. Posicion donde crear la entidad.
// - szTag. Tag asociado a la entidad.
// - szProfile. Perfil a partir del cual crear la entidad.
// - Elevation. Elevacion asociada.
// - Light. Posible luz asociada.
// - bTempCriature. Flag de creacion de la criatura como temporal.
// Devuelve:
// - Si se pudo crear, el handle a la entidad. En caso contrario handle nulo.
// Notas:
///////////////////////////////////////////////////////////////////////////////
AreaDefs::EntHandle 
CArea::CreateCriature(const std::string& szProfile,
				      const AreaDefs::sTilePos& TilePos,
					  const std::string& szTag,					  
					  const RulesDefs::Elevation& Elevation,
					  const GraphDefs::Light& Light,
					  const bool bTempCriature)
{
  // SOLO si instancia inicializada y cargada
  ASSERT(IsInitOk());
  ASSERT(IsAreaLoaded());

  // ¿Es posicion de celda NO valida? o 
  // ¿celda SIN contenido? o
  // ¿El perfil recibido es NO es valida?
  iCGameDataBase* const pGDBase = SYSEngine::GetGameDataBase();
  ASSERT(pGDBase);
  if (!IsCellValid(TilePos) ||
	  !IsCellWithContent(TilePos) ||
	  !pGDBase->IsValidCBBFileSection(GameDataBaseDefs::CBBF_CRIATUREPROFILES, 
									  szProfile)) {
	// Si, se abandona accion
	return 0;
  }

  // Se crea instancia y obtiene handle
  sNCriature* pNCriature = new sNCriature;
  ASSERT(pNCriature);
  const AreaDefs::EntHandle hEntity = CreateHandle(RulesDefs::CRIATURE);
  ASSERT(hEntity);
  ASSERT((GetEntityType(hEntity) == RulesDefs::CRIATURE) != 0);

  // Se cargan datos del archivo de perfil
  dword udOffset = m_pGDBase->GetCBBFileOffset(GameDataBaseDefs::CBBF_CRIATUREPROFILES, szProfile);
  pNCriature->Criature.Init(m_pGDBase->GetCBBFileHandle(GameDataBaseDefs::CBBF_CRIATUREPROFILES), 
  					        udOffset, 
					        hEntity);
  ASSERT_MSG(pNCriature->Criature.IsInitOk(), "Problemas creando criatura");

  // Se inserta en el map del area
  m_Map.Criatures.insert(CriaturesValType(hEntity, pNCriature));
  
  // Inserta posible tag pasado
  InsertTag(hEntity, szTag);

  // Se establece elevacion
  pNCriature->Criature.SetElevation(Elevation);

  // Se inserta en el escenario
  InsertWorldEntityInTile(hEntity, TilePos);

  // Luz asociada
  SetLight(hEntity, Light);

  // Se asocia area como observer de la criatura
  pNCriature->Criature.AddObserver(this);
  
  // Todo correcto, retorna handle
  return hEntity;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo comun para los dos relacionados con la creacion de items. Se
//   encargara de crear la instancia CItem e insertarla en el map de items.
// Parametros:
// - szProfile. Perfil a partir del cual crear la entidad.
// Devuelve:
// - Si se pudo crear la instancia CItem, en caso contrario NULL.
// Notas:
///////////////////////////////////////////////////////////////////////////////CItem* const
CItem* const
CArea::_CreateItem(const std::string& szProfile)
{
  // SOLO si instancia inicializada y cargada
  ASSERT(IsInitOk());
  ASSERT(IsAreaLoaded());

  // ¿El perfil recibido es NO es valida?
  iCGameDataBase* const pGDBase = SYSEngine::GetGameDataBase();
  ASSERT(pGDBase);
  if (!pGDBase->IsValidCBBFileSection(GameDataBaseDefs::CBBF_ITEMPROFILES, 
									  szProfile)) {
	// Si, se abandona accion
	return NULL;
  }

  // Se obtiene handle y crea instancia
  const AreaDefs::EntHandle hEntity = CreateHandle(RulesDefs::ITEM);	
  ASSERT(hEntity);
  ASSERT((GetEntityType(hEntity) == RulesDefs::ITEM) != 0);
  sNItem* pNItem = new sNItem;
  ASSERT(pNItem);

  // Se cargan datos del archivo de perfil
  dword udOffset = m_pGDBase->GetCBBFileOffset(GameDataBaseDefs::CBBF_ITEMPROFILES, szProfile);
  pNItem->Item.Init(m_pGDBase->GetCBBFileHandle(GameDataBaseDefs::CBBF_ITEMPROFILES), 
  					udOffset, 
					hEntity);
  ASSERT_MSG(pNItem->Item.IsInitOk(), "Problemas creando el ítem");

  // Se establecen valores basicos y se inserta entidad en el map
  pNItem->Item.SetOwner(0);
  pNItem->Item.SetElevation(0);  	  
  m_Map.Items.insert(ItemsValType(hEntity, pNItem));      

  // Retorna instancia creada
  return &pNItem->Item;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Creara de forma dinamica una entidad de tipo item sobre una posicion. 
// Parametros:
// - TilePos. Posicion donde crear la entidad.
// - szTag. Tag asociado a la entidad.
// - szProfile. Perfil a partir del cual crear la entidad.
// - Light. Posible luz asociada.
// Devuelve:
// - Si se pudo crear, el handle a la entidad. En caso contrario handle nulo.
// Notas:
///////////////////////////////////////////////////////////////////////////////
AreaDefs::EntHandle 
CArea::CreateItem(const std::string& szProfile,								
				  const AreaDefs::sTilePos& TilePos,
				  const std::string& szTag,				  
				  const GraphDefs::Light& Light)
{
  // SOLO si instancia inicializada y cargada
  ASSERT(IsInitOk());
  ASSERT(IsAreaLoaded());

  // ¿Es posicion de celda NO valida? o 
  // ¿celda SIN contenido? 
  iCGameDataBase* const pGDBase = SYSEngine::GetGameDataBase();
  ASSERT(pGDBase);
  if (!IsCellValid(TilePos) ||
	  !IsCellWithContent(TilePos)) {
	// Si, se abandona accion
	return 0;
  }

  // Se intenta crear la instancia a nivel basico
  CItem* const pItem = _CreateItem(szProfile);
  if (pItem) {
	// Se pudo crear, se inserta en la celda pasada
	InsertWorldEntityInTile(pItem->GetHandle(), TilePos);

    // Inserta posible tag pasado
    InsertTag(pItem->GetHandle(), szTag);

	// Se establece luz
	SetLight(pItem->GetHandle(), Light);

	// Todo correcto, se retorna
	return pItem->GetHandle();
  }

  // No se pudo crear item
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Creara de forma dinamica una entidad de tipo item y la asociara a una
//   entidad contenedora. 
// Parametros:
// - hOwner. Handle al dueño.
// - szTag. Tag asociado a la entidad.
// - szProfile. Perfil a partir del cual crear la entidad.
// - Light. Posible luz asociada.
// Devuelve:
// - Si se pudo crear, el handle a la entidad. En caso contrario handle nulo.
// Notas:
///////////////////////////////////////////////////////////////////////////////
AreaDefs::EntHandle 
CArea::CreateItem(const std::string& szProfile,
				  const AreaDefs::EntHandle& hOwner,
				  const std::string& szTag,				  
				  const GraphDefs::Light& Light)
{
  // SOLO si instancia inicializada y cargada
  ASSERT(IsInitOk());
  ASSERT(IsAreaLoaded());
  // SOLO si parametros correctos
  ASSERT(hOwner);

  // ¿El dueño es contenedor?
  CWorldEntity* const pOwner = GetWorldEntity(hOwner);
  ASSERT(pOwner);
  iCItemContainer* const pContainer = pOwner->GetItemContainer();
  if (pContainer) {
	// Si, se intenta crear la instancia a nivel basico
	CItem* const pItem = _CreateItem(szProfile);
	if (pItem) {
	  // Se pudo crear, se inserta en el contenedor deseado
	  pContainer->Insert(pItem->GetHandle());

	  // Inserta posible tag pasado
	  InsertTag(pItem->GetHandle(), szTag);

	  // Se establece luz
	  SetLight(pItem->GetHandle(), Light);

	  // Todo correcto, se retorna
	  return pItem->GetHandle();
	}
  }  

  // No se pudo crear item
  return 0;
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Creara de forma dinamica una entidad de tipo pared. Para que se pueda 
//   crear, la posicion pasada debe de ser valida y el valor del perfil debe 
//   de estar disponible.
// Parametros:
// - TilePos. Posicion donde crear la entidad.
// - szTag. Tag asociado a la entidad.
// - szProfile. Perfil a partir del cual crear la entidad.
// - Elevation. Elevacion asociada.
// - Light. Posible luz asociada.
// - bBlockAccessFlag. Flag de acceso inicial.
// Devuelve:
// - Si se pudo crear, el handle a la entidad. En caso contrario handle nulo.
// Notas:
///////////////////////////////////////////////////////////////////////////////
AreaDefs::EntHandle 
CArea::CreateWall(const std::string& szProfile,
				  const AreaDefs::sTilePos& TilePos,
				  const std::string& szTag,				  
				  const RulesDefs::Elevation& Elevation,
				  const GraphDefs::Light& Light,
				  const bool bBlockAccessFlag)
{
  // SOLO si instancia inicializada y cargada
  ASSERT(IsInitOk());
  ASSERT(IsAreaLoaded());

  // ¿Es posicion de celda NO valida? o 
  // ¿celda SIN contenido? o
  // ¿El perfil recibido es NO es valida?
  iCGameDataBase* const pGDBase = SYSEngine::GetGameDataBase();
  ASSERT(pGDBase);
  if (!IsCellValid(TilePos) ||
	  !IsCellWithContent(TilePos) ||
	  !pGDBase->IsValidCBBFileSection(GameDataBaseDefs::CBBF_WALLPROFILES, 
									  szProfile)) {
	// Si, se abandona accion
	return 0;
  }

  // Se obtiene handle y crea instancia
  const AreaDefs::EntHandle hEntity = CreateHandle(RulesDefs::WALL);	
  ASSERT(hEntity);
  ASSERT((GetEntityType(hEntity) == RulesDefs::WALL) != 0);
  sNWall* pNWall = new sNWall;
  ASSERT(pNWall);

  // Se cargan datos del archivo de perfil
  dword udOffset = m_pGDBase->GetCBBFileOffset(GameDataBaseDefs::CBBF_WALLPROFILES, szProfile);
  pNWall->Wall.Init(m_pGDBase->GetCBBFileHandle(GameDataBaseDefs::CBBF_WALLPROFILES), 
  					udOffset, 
					hEntity);
  ASSERT_MSG(pNWall->Wall.IsInitOk(), "Problemas creando pared");

  // Se inserta en el map del area
  m_Map.Walls.insert(WallsValType(hEntity, pNWall));

  // Inserta posible tag pasado
  InsertTag(hEntity, szTag);

  // Se establece elevacion
  pNWall->Wall.SetElevation(Elevation);

  // Se inserta en el escenario
  InsertWorldEntityInTile(hEntity, TilePos);

  // Se establece luz asociada
  SetLight(hEntity, Light);

  // Se establece flag de bloqueo
  if (!bBlockAccessFlag) {
	 pNWall->Wall.UnblockAccess();	
  }   

  // Todo correcto, retorna handle
  return hEntity;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Creara de forma dinamica una entidad de tipo objeto de escenario. Para
//   que se pueda crear, la posicion pasada debe de ser valida y el valor del
//   perfil debe de estar disponible.
// Parametros:
// - TilePos. Posicion donde crear la entidad.
// - szTag. Tag asociado a la entidad.
// - szProfile. Perfil a partir del cual crear la entidad.
// - Elevation. Elevacion asociada.
// - Light. Posible luz asociada.
// Devuelve:
// - Si se pudo crear, el handle a la entidad. En caso contrario handle nulo.
// Notas:
///////////////////////////////////////////////////////////////////////////////
AreaDefs::EntHandle 
CArea::CreateSceneObj(const std::string& szProfile,
					  const AreaDefs::sTilePos& TilePos,
					  const std::string& szTag,					  
					  const RulesDefs::Elevation& Elevation,
					  const GraphDefs::Light& Light)
{
  // SOLO si instancia inicializada y cargada
  ASSERT(IsInitOk());
  ASSERT(IsAreaLoaded());

  // ¿Es posicion de celda NO valida? o 
  // ¿celda SIN contenido? o
  // ¿El perfil recibido es NO es valida?
  iCGameDataBase* const pGDBase = SYSEngine::GetGameDataBase();
  ASSERT(pGDBase);
  if (!IsCellValid(TilePos) ||
	  !IsCellWithContent(TilePos) ||
	  !pGDBase->IsValidCBBFileSection(GameDataBaseDefs::CBBF_SCENEOBJPROFILES, 
									  szProfile)) {
	// Si, se abandona accion
	return 0;
  }

  // Se crea instancia y se obtiene handle
  sNSceneObj* const pNSceneObj = new sNSceneObj;
  ASSERT(pNSceneObj);	
  const AreaDefs::EntHandle hEntity = CreateHandle(RulesDefs::SCENE_OBJ);  
  ASSERT(hEntity);
  ASSERT((GetEntityType(hEntity) == RulesDefs::SCENE_OBJ) != 0);
	
  // Se cargan datos del archivo de perfiles
  dword udOffset = m_pGDBase->GetCBBFileOffset(GameDataBaseDefs::CBBF_SCENEOBJPROFILES, szProfile);
  pNSceneObj->SceneObj.Init(m_pGDBase->GetCBBFileHandle(GameDataBaseDefs::CBBF_SCENEOBJPROFILES), 
  							udOffset, 
							hEntity);
  ASSERT_MSG(pNSceneObj->SceneObj.IsInitOk(), "Problemas creando pared");

  // Se inserta en el map del area
  m_Map.SceneObjs.insert(SObjsValType(hEntity, pNSceneObj));

  // Inserta posible tag pasado
  InsertTag(hEntity, szTag);

  // Establece elevacion
  pNSceneObj->SceneObj.SetElevation(Elevation);

  // Se inserta en el escenario
  InsertWorldEntityInTile(hEntity, TilePos);

  // Establece luz asociada
  SetLight(hEntity, Light);

  // Todo correcto, retorna handle
  return hEntity;
}  

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo encargado de la destruccion de una entidad. Habra pasado el filtro
//   exterior con lo que es seguro que la entidad exista en CArea y se pueda
//   destruir (no sea el jugador).
// Parametros:
// - hEntity. Handle a la entidad a destruir.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CArea::DestroyEntity(const AreaDefs::EntHandle& hEntity)
{
  // SOLO si instancia inicializada y area cargada
  ASSERT(IsInitOk());
  ASSERT(IsAreaLoaded());

  // Se borran instancias a entidades, realizando pasos previos sobre 
  // posibles contenedores asociados.
  RulesDefs::eEntityType Type = GetEntityType(hEntity);
  switch (Type) {
	case RulesDefs::WALL: {
	  // Se extrae del area
	  RemoveWorldEntityFromTile(hEntity);

	  // Se borra instancia 
	  WallsIt MapIt(m_Map.Walls.find(hEntity));
	  ASSERT((MapIt != m_Map.Walls.end()) != 0);
	  delete MapIt->second;
	  m_Map.Walls.erase(MapIt);
	} break;

	case RulesDefs::SCENE_OBJ: {	  
	  // Se vacia inventario 
	  CSceneObj* const pSceneObj = GetSceneObj(hEntity);
	  ASSERT(pSceneObj);
	  CItemContainerIt It(*(pSceneObj->GetItemContainer()));
	  while (It.IsValid()) {		
		// Se cambia de localidad y se apunta al nuevo primer item en inv.
		// Nota: Es vital tomar el handle al item del iterador pues en el
		// momento en que se cambie de localidad, el mismo sera extraido del
		// contenedor y el iterador dejara de ser seguro.
		const AreaDefs::EntHandle hItem = It.GetItem();
		ChangeLocation(hItem, pSceneObj->GetTilePos());		
		It.Init(*(pSceneObj->GetItemContainer()));		

		// Se establecera el flag de pausa segun el estado del dueño
		CItem* const pItem = SYSEngine::GetWorld()->GetItem(hItem);
		ASSERT(pItem);
		pItem->SetPause(pSceneObj->IsInPause());
	  }
	  
	  // Se extrae del area
	  RemoveWorldEntityFromTile(hEntity);
	  
	  // Se borra instancia 
	  SObjsIt MapIt(m_Map.SceneObjs.find(hEntity));
	  ASSERT((MapIt != m_Map.SceneObjs.end()) != 0);
	  delete MapIt->second;
	  m_Map.SceneObjs.erase(MapIt);	  
	} break;

	case RulesDefs::CRIATURE: {
	  // Se vacian slots de equipamiento, llevando los items a inventario
	  CCriature* const pCriature = GetCriature(hEntity);
	  ASSERT(pCriature);
	  iCEquipmentSlots* const pEquipSlots = pCriature->GetEquipmentSlots();
	  ASSERT(pEquipSlots);	  
	  byte ubIt = 0;
	  for (; ubIt < RulesDefs::MAX_CRIATURE_EQUIPMENTSLOT; ++ubIt) {		
		// Se desaloja POSIBLE item en slot
		const RulesDefs::eIDEquipmentSlot Slot = RulesDefs::eIDEquipmentSlot(ubIt);
		const AreaDefs::EntHandle hItem = pEquipSlots->UnequipItem(Slot);
		if (hItem) {
		  // Item desalojado, se inserta ahora en inventario
		  pCriature->GetItemContainer()->Insert(hItem);
		}
	  }

	  // Se desaloja todo el inventario de la criatura
	  CItemContainerIt It(*(pCriature->GetItemContainer()));
	  while (It.IsValid()) {		
		// Se cambia de localidad y se apunta al nuevo primer item en inv.
		// Nota: Es vital tomar el handle al item del iterador pues en el
		// momento en que se cambie de localidad, el mismo sera extraido del
		// contenedor y el iterador dejara de ser seguro.
		const AreaDefs::EntHandle hItem = It.GetItem();
		ChangeLocation(hItem, pCriature->GetTilePos());		
		It.Init(*(pCriature->GetItemContainer()));		

		// Se establecera el flag de pausa segun el estado del dueño
		CItem* const pItem = SYSEngine::GetWorld()->GetItem(hItem);
		ASSERT(pItem);
		pItem->SetPause(pCriature->IsInPause());
	  }	  

	  // Se elimina area como observer de la criatura
	  pCriature->RemoveObserver(this);

   	  // Se extrae del area
	  RemoveWorldEntityFromTile(hEntity);

	  // Se borra instancia 
	  CriaturesIt MapIt(m_Map.Criatures.find(hEntity));
	  ASSERT((MapIt != m_Map.Criatures.end()) != 0);
	  delete MapIt->second;
	  m_Map.Criatures.erase(MapIt);
	} break;

	case RulesDefs::ITEM: {
	  // ¿Tiene dueño?
	  CItem* pItem = GetItem(hEntity);
	  ASSERT(pItem);	  
	  if (pItem->GetOwner()) {
		// Si, ¿Es un objeto de escenario?		
		if (RulesDefs::SCENE_OBJ == GetEntityType(pItem->GetOwner())) {
		  // Si, se extrae del contenedor del mismo
		  CSceneObj* const pSceneObj = GetSceneObj(pItem->GetOwner());
		  ASSERT(pSceneObj);
		  pSceneObj->GetItemContainer()->Extract(hEntity);
		} else {
		  // No, es una criatura, podra estar equipado o inventario		  
		  CCriature* const pCriature = GetCriature(pItem->GetOwner());
		  ASSERT(pCriature);
		  // ¿Esta equipado?
		  RulesDefs::EquipmentSlot Slot;
		  if (pCriature->GetEquipmentSlots()->WhereIsItemEquiped(hEntity, Slot)) {
			// Si, se elimina del slot de equipamiento
			pCriature->GetEquipmentSlots()->UnequipItem(Slot);
		  } else {
			// No, luego estara en inventario			
			pCriature->GetItemContainer()->Extract(hEntity);
		  }
		}
	  } else {
		// No, estara sobre un floor, se extrae del area
		RemoveWorldEntityFromTile(hEntity);
	  }	  

	  // Se borra instancia 	  
	  ItemsIt MapIt(m_Map.Items.find(hEntity));	  
	  ASSERT((MapIt != m_Map.Items.end()) != 0);
	  delete MapIt->second;	  
	  m_Map.Items.erase(MapIt);	  
	} break;

	default:
	  ASSERT(false);
  }; // ~ switch  

  // Se elimina el posible tag asociado
  RemoveTag(hEntity);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - A partir del handle ahEntity, localiza la entidad a la que pertence y
//   devuelve la misma como una instancia CWordEntity.
// Parametros:
// - hEntity. Handle a la entidad.
// Devuelve:
// - Instancia a la entidad. En caso de que no se hallara, se devolvería NULL.
//   En el caso de pasar un handle que no sea del tipo adecuado, se devolvera
//   tambien NULL.
// Notas:
///////////////////////////////////////////////////////////////////////////////
CWorldEntity* const 
CArea::GetWorldEntity(const AreaDefs::EntHandle& hEntity)
{ 
  // SOLO si instancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());

  // Localiza la entidad
  switch (GetEntityType(hEntity)) {
	case RulesDefs::SCENE_OBJ:	  
	  // Objetos de escenario
	  return GetSceneObj(hEntity);
	  break;

	case RulesDefs::ITEM:
	  return GetItem(hEntity);
	  break;

	case RulesDefs::PLAYER:
	  // Jugador
	  return GetPlayer();
	  break;

	case RulesDefs::CRIATURE:
	  // Criaturas
	  return GetCriature(hEntity);	  
	  break;

	case RulesDefs::WALL:
	  // Paredes
	  return GetWall(hEntity);	  
	  break;
  };

  // Handle desconocido
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - A partir del handle hCriature, localiza la criatura a la que pertenece.
// Parametros:
// - hCriature. Handle a la entidad.
// Devuelve:
// - Instancia a la criatura. Si no se encuentra, se devolvera NULL.
// Notas:
// - Dentro de criaturas entra el jugador tambien.
///////////////////////////////////////////////////////////////////////////////
CCriature* const 
CArea::GetCriature(const AreaDefs::EntHandle& hCriature)
{  
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Obtiene la instancia
  switch(GetEntityType(hCriature)) {
	case RulesDefs::PLAYER:
	  // Jugador	  
	  return GetPlayer();
	  break;
	
	case RulesDefs::CRIATURE:
	  // Npcs	  
	  const CriaturesIt It(m_Map.Criatures.find(hCriature));
	  return (It != m_Map.Criatures.end()) ? &It->second->Criature : NULL;		
	  break;
  };

  // No se hallo
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el identificador de habitacion asociado a la posicion TilePos.
// Parametros:
// - TilePos. Posicion del tile en donde buscar el identificador.
// Devuelve:
// - Identificador de habitacion asociado a TilePos. En caso de que no exista
//   ningun identificador de habitacion 0.
// Notas:
///////////////////////////////////////////////////////////////////////////////
AreaDefs::RoomID 
CArea::GetRoomAt(const AreaDefs::sTilePos& TilePos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());

  // Se obtiene IndexPos asociada a TilePos
  const AreaDefs::TileIndex TileIdx = GetTileIdx(TilePos);

  // Procede a comprobar si el tile recibido pertenece a una habitacion
  RoomInfoMapIt RoomIt(m_Map.RoomInfo.begin());
  for (; RoomIt != m_Map.RoomInfo.end(); ++RoomIt) {
	if (RoomIt->second.Cells.find(TileIdx) != RoomIt->second.Cells.end()) {
	  // Si, se retorna el identificador
	  return RoomIt->first;
	}
  }

  // No, se retorna identificador nulo
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el techo de una habitacion visible / invisible.
// Parametros:
// - RoomID. Identificador de la habitacion.
// - bVisible. Flag con la orden de establecimiento.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CArea::SetRoomRoofVisible(const AreaDefs::RoomID& RoomID,
						  const bool bVisible)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());
  // SOLO si parametros validos
  ASSERT(RoomID);

  // Obtiene iterador al nodo
  const RoomInfoMapIt RoomIt(m_Map.RoomInfo.find(RoomID));
  ASSERT((RoomIt != m_Map.RoomInfo.end()) != 0);

  // ¿El flag actual es distinto al recibido?
  if (RoomIt->second.bIsRoofVisible != bVisible) {	
	// Se realiza la operacion
	CellsInRoomSetIt CellIt(RoomIt->second.Cells.begin());
	for (; CellIt != RoomIt->second.Cells.end(); ++CellIt) {
	  // ¿Hay roof?
	  if (m_Map.pMap[*CellIt]->hRoof) {
		// Si, se establece el flag de visibilidad que proceda
		CRoof* pRoof = GetRoof(m_Map.pMap[*CellIt]->hRoof);
		ASSERT(pRoof);
		pRoof->SetVisible(bVisible);
	  }	  
	}
	// Se cambia el flag de visibilidad de roofs asociados a room
	RoomIt->second.bIsRoofVisible = bVisible;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de calcular las criaturas que se hallen en un determinado rango
//   e introducirlas en el conjunto recibido.
// Parametros:
// - Pos. Posicion donde comenzar a realizar la comprobacion.
// - Range. Rango a partir de esa posicion, donde comenzar a comprobar.
// - InRangeSet. Conjunto donde introducir a las entidades
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CArea::FindCriaturesInRangeAt(const AreaDefs::sTilePos& Pos,
							   const RulesDefs::CriatureRange& Range,
							   std::set<AreaDefs::EntHandle>& InRangeSet)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(IsCellValid(Pos));
  
  // Se procede a iterar por las posiciones, introduciendo los handles de
  // las criaturas que se hallen en el rango de vision
  const AreaDefs::sTilePos MaxPos(Pos.XTile + Range, Pos.YTile + Range);
  CCellIterator CellIt;
  AreaDefs::sTilePos PosIt;
  PosIt.YTile = Pos.YTile - Range;
  for (; PosIt.YTile < MaxPos.YTile; ++PosIt.YTile) {
	PosIt.XTile = Pos.XTile - Range;
	for (; PosIt.XTile < MaxPos.XTile; ++PosIt.XTile) {	  
	  // Si, se itera por entre las entidades si procede		
	  // Nota: La comprobacion de que la celda es valida y
	  // posee contenido, se realizara al intentar inicializar el iterador	  
	  if (CellIt.Init(this, PosIt)) {
	    for (; CellIt.IsItValid(); CellIt.Next()) {
	  	  // ¿El handle de entidad pertenece a criatura o jugador?		  
		  const RulesDefs::eEntityType Type = GetEntityType(CellIt.GetWorldEntity());
		  if (Type == RulesDefs::CRIATURE ||
		  	  Type == RulesDefs::PLAYER) {
			// Si, se introduce en conjunto
			InRangeSet.insert(CellIt.GetWorldEntity());
		  }
		}
	  }
	}
  }
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el handle de una criatura se halla en el interior de un
//   determinado rango asociado a una posicion particular.
// Parametros:
// - hCriature. Handle de la criatura.
// - Pos. Posicion donde comprobarlo.
// - Range. Rango
// Devuelve:
// - Si se halla en el rango true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CArea::IsCriatureInRangeAt(const AreaDefs::EntHandle& hCriature,
							const AreaDefs::sTilePos& Pos,
							const RulesDefs::CriatureRange& Range)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(IsCellValid(Pos));  

  // Se toma la pos. de la criatura
  CCriature* const pCriature = GetCriature(hCriature);
  ASSERT(pCriature);
  const AreaDefs::sTilePos CriatureTilePos(pCriature->GetTilePos());
  const sPosition CriaturePos(CriatureTilePos.XTile, CriatureTilePos.YTile);
  
  // Se halla el area
  const sRect Rect(Pos.XTile - Range, Pos.XTile + Range,
			       Pos.YTile - Range, Pos.YTile + Range);

  // Finalmente, se realiza la comprobacion
  return SYSEngine::GetMathUtil()->IsPointInRect(CriaturePos, Rect);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorrera todas las criaturas existentes en el area de juego y comprobara
//   si la recibida se halla en el rango de ellas, en caso de ser asi insertara
//   el handle de estas en el conjunto recibido.
// - No se tendra en cuenta a la criatura que sea igual al handle recibido ni
//   aquella que ya contuvieran a la criatura recibida.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CArea::FindCriaturesContainingCriatureInRange(const AreaDefs::EntHandle& hCriature,
											  std::set<AreaDefs::EntHandle>& InRangeSet)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hCriature);  

  // Recorre las criaturas existentes, si procede
  CCriatureIterator CriatureIt;
  if (CriatureIt.Init(this)) {
	for (; CriatureIt.IsItValid(); CriatureIt.Next()) {
	  // ¿Handle diferene al recibido? y
	  // ¿Criature recibida NO se halla ACTUALMENTE en el rango de la misma?
	  CCriature* const pCriature = CriatureIt.GetEntity();
	  ASSERT(pCriature);
	  if (pCriature->GetHandle() != hCriature &&
		  !pCriature->IsInRange(hCriature)) {	  
		// Si, se comprueba si la criatura recibida se halla en el rango de la misma
		if (IsCriatureInRangeAt(hCriature, 
								pCriature->GetTilePos(), 
								pCriature->GetRange())) {
		  // Se halla en rango, por lo que se añadira handle al conjunto
		  InRangeSet.insert(pCriature->GetHandle());
		}
	  }
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inserta la entidad con handle hEntity en el tile de posicion NewPos.
// Parametros:
// - hEntity. Handle a la entidad.
// - NewPos. Nueva posicion de insercion
// Devuelve:
// Notas:
// - Quedara en responsabilidad del exterior haber pasado un destino valido
///////////////////////////////////////////////////////////////////////////////
void
CArea::InsertWorldEntityInTile(const AreaDefs::EntHandle& hEntity,
							   const AreaDefs::sTilePos& NewPos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());
  // SOLO si parametros validos
  ASSERT(hEntity);

  // ¿La celda tiene contenido?
  if (IsCellWithContent(NewPos)) {
	// Se obtiene el tile donde insertar a la entidad
	sNCell* const pCell = m_Map.pMap[GetTileIdx(NewPos)];
	ASSERT(pCell);
 
	// Se Halla la posicion donde insertar a la entidad y se inserta
	const RulesDefs::eEntityType EntityType = GetEntityType(hEntity);
	ASSERT((EntityType != RulesDefs::NO_ENTITY) != 0);  
	CellEntitiesListIt It(pCell->Entities.begin());  
	while (It != pCell->Entities.end() && 
  		   EntityType >= GetEntityType(*It)) {
	  ++It;	
	}
	pCell->Entities.insert(It, hEntity);

	// Se asocia la posicion a la misma
	CWorldEntity* const pEntity = GetWorldEntity(hEntity);
	ASSERT(pEntity);	
	pEntity->SetTilePos(NewPos); 
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se elimina una entidad de un tile.
// Parametros:
// - hEntity. Handle a la entidad a quitar del tile en donde se encuentra.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CArea::RemoveWorldEntityFromTile(const AreaDefs::EntHandle& hEntity)
{
  // SOLO si intancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());
  // SOLO si parametros validos
  ASSERT(hEntity);

  // Se obtiene el tile donde quitar a la entidad
  CWorldEntity* const pEntity = GetWorldEntity(hEntity);
  ASSERT(pEntity);  
  sNCell* const pCell = m_Map.pMap[GetTileIdx(pEntity->GetTilePos())];
  ASSERT(pCell);
 
  // Se quita del tile en donde se halle
  const CellEntitiesListIt It(std::find(pCell->Entities.begin(),
  							            pCell->Entities.end(),
							            hEntity));
  ASSERT((It != pCell->Entities.end()) != 0);
  pCell->Entities.erase(It);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Cambia de localidad a una entidad PERO con el objetivo de realizar alguna
//   correccion de dibujado pues a titulo logico, la entidad seguira en otra
//   posicion distinta a la visual recibida. 
// Parametros:
// - hEntity. Entidad que queremos cambiar de posicion.
// - OriginalPos. Posicion original.
// - VisualPos. Posicion visual.
// Devuelve:
// - Si se ha podido realizar true, en caso contrario false
// Notas:
// - Este metodo dara por hecho muchas cosas, de ahi que los asertos hagan
//   que su ejecucion sea muy extricta.
// - Este metodo utilizara el flag m_bInFreeAreaMode para evitar efectos
//   laterales relativos a la destruccion del comando de movimiento.
///////////////////////////////////////////////////////////////////////////////
bool
CArea::ChangeLocationForVisuals(const AreaDefs::EntHandle& hEntity,
								const AreaDefs::sTilePos& OriginalPos,
								const AreaDefs::sTilePos& VisualPos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());
  // SOLO si parametros correctos
  ASSERT(hEntity);
  ASSERT(IsCellWithContent(OriginalPos));
  ASSERT(IsCellWithContent(VisualPos));
  
  // SOLO si NO se esta liberando el area
  if (!m_bInFreeAreaMode) {
	// ¿Posiciones distintas?
	if (OriginalPos != VisualPos) {
	  // Extrae de la posicion original
	  sNCell* pCell = m_Map.pMap[GetTileIdx(OriginalPos)];
	  ASSERT(pCell);
	  CellEntitiesListIt It(std::find(pCell->Entities.begin(),
  									  pCell->Entities.end(),
									  hEntity));
	  ASSERT((It != pCell->Entities.end()) != 0);
	  pCell->Entities.erase(It);

	  // Inserta en la nueva  
	  pCell = m_Map.pMap[GetTileIdx(VisualPos)];
	  ASSERT(pCell);  
	  const RulesDefs::eEntityType EntityType = GetEntityType(hEntity);
	  ASSERT((EntityType != RulesDefs::NO_ENTITY) != 0);  
	  It = pCell->Entities.begin();  
	  while (It != pCell->Entities.end() && 
  			 EntityType >= GetEntityType(*It)) {
		++It;	
	  }
	  pCell->Entities.insert(It, hEntity);

	  // Correcto
	  return true;
	}
  }

  // No se pudo realizar
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Cambia la localidad de una entidad.
// - La entidad a cambiar de posicion debera de ser:
//   * Criatura -> Cambia sin mas.
//   * Item -> Si no tiene dueño, cambiara sin mas. Si tuviera dueño,
//             obtendria el dueño, sacaria la entidad de su contenedor y
//             despues la depositaria sobre la posicion indicada.
// Parametros:
// - hEntity. Entidad a la que camiar de posicion.
// - NewPos. Nueva posicion en donde situar a la entidad
// Devuelve:
// - Si la operacion se ha realizado true. En caso contrario false.
// Notas:
// - El resto de entidades no contempladas para cambiar de posicion, se 
//   entendera que no se pueden mover
///////////////////////////////////////////////////////////////////////////////
bool 
CArea::ChangeLocation(const AreaDefs::EntHandle& hEntity,
					  const AreaDefs::sTilePos& NewPos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());
  // SOLO si parametros validos
  ASSERT(hEntity);

  // ¿La celda tiene contenido y es valida?
  if (IsCellValid(NewPos) &&
  	  IsCellWithContent(NewPos)) {
	// Se obtiene el tipo de la entidad y se actuara segun el tipo
	const RulesDefs::eEntityType Type = GetEntityType(hEntity);
	switch (Type) {
	  case RulesDefs::CRIATURE: 
	  case RulesDefs::PLAYER:{
	    // Extraccion y evento de extraccion de la entidad de un tile
	    CWorldEntity* const pEntity = GetWorldEntity(hEntity);
	    ASSERT(pEntity);
	    sNCell* pCell = m_Map.pMap[GetTileIdx(pEntity->GetTilePos())];  
	    ASSERT(pCell);
	    RemoveWorldEntityFromTile(hEntity);
	    pCell->Floor.OnSetOut(pEntity->GetTilePos(), hEntity);
			
	    // Intercambio de posicion y evento de insercion
	    InsertWorldEntityInTile(hEntity, NewPos);
	    pCell = m_Map.pMap[GetTileIdx(NewPos)];  
	    ASSERT(pCell);
	    pCell->Floor.OnSetIn(NewPos, hEntity);

	    // Se retorna
	    return true;		
	  } break;

	  case RulesDefs::ITEM: {
	    // Se obtiene instancia al item		
	    const CItem* const pItem = GetItem(hEntity);
	    ASSERT(pItem);
		  
	    // ¿El item tiene dueño?
	    if (pItem->GetOwner()) {
	  	  // Si, Se obtiene el dueño y se extrae el item del mismo
	  	  // Nota: El contenedor establecera dueño 0 al extraer el item
	  	  // y se encargara de localizarlo sobre el suelo
	  	  iCItemContainer* const pContainer = GetWorldEntity(pItem->GetOwner())->GetItemContainer();
	  	  ASSERT(pContainer);		
	  	  pContainer->Extract(hEntity);
	    } else {
		  // No, se extrae manualmente de un floor (generandose el evento)
		  // y se deposita sobre otro diferente.
		  CWorldEntity* const pEntity = GetWorldEntity(hEntity);
		  ASSERT(pEntity);
		  sNCell* pCell = m_Map.pMap[GetTileIdx(pEntity->GetTilePos())];  		  
		  ASSERT(pCell);
		  RemoveWorldEntityFromTile(hEntity);
		  pCell->Floor.OnSetOut(pEntity->GetTilePos(), hEntity);			
		}
			
		// Establecimiento en la nueva posicion y evento de insercion
		InsertWorldEntityInTile(hEntity, NewPos);
		sNCell* const pCell = m_Map.pMap[GetTileIdx(NewPos)];  
		ASSERT(pCell);
		pCell->Floor.OnSetIn(NewPos, hEntity);

		// Retorna
		return true;				
	  } break;
	}; // ~ switch	
  }  

  // No se pudo realizar la operacion
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Cambia la localizacion de un item, que se encuentra sobre el suelo, al
//   inventario de una criatura o bien a un objeto de escenario que sea 
//   contenedor.
// Parametros:
// - hItem. Handle al item.
// - hEntity. Handle a la entidad contenedora.
// Devuelve:
// - Si la operacion se pudo hacer true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CArea::ChangeLocation(const AreaDefs::EntHandle& hItem,
					  const AreaDefs::EntHandle& hEntity)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());
  // SOLO si parametros validos
  ASSERT(hItem);
  ASSERT(hEntity);

  // ¿El item NO tiene dueño?
  CItem* const pItem = GetItem(hItem);
  ASSERT(pItem); 
  if (!pItem->GetOwner()) {
    // Obtiene handle al dueño
    CWorldEntity* const pEntity = GetWorldEntity(hEntity);
    ASSERT(pEntity);

    // ¿La entidad es contenedora?
    if (pEntity->GetItemContainer()) {
  	  // Toma celda donde esta el item
	  // Nota: Se tomara antes de ingresar el item porque despues la
	  // posicion del mismo sera la de su dueño
	  sNCell* pCell = m_Map.pMap[GetTileIdx(pItem->GetTilePos())];  		  
	  ASSERT(pCell);  

	  // Se elimina el item del tile donde este	  
	  RemoveWorldEntityFromTile(hItem);
		
	  // Se inserta en el contenedor
	  // Nota: El contenedor establecera dueño hEntity al insertar el item
	  pEntity->GetItemContainer()->Insert(hItem);

	  // Se notifica la salida del floor
	  pCell->Floor.OnSetOut(pItem->GetTilePos(), hItem);			  
  
	  // Retorna
	  return true;	 
	}
  }

  // No se pudo realizar la operacion
  return false;
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Localiza al jugador en el area de juego. En caso de que ya estuviera
//   asociado, desasociara la instancia previa que hiciera de jugador.
// Parametros:
// - pPlayer. Instancia jugador.
// - TilePos. Posicion del tile.
// Devuelve:
// - Si se ha podido situar true. En caso contrario false.
// Notas:
// - En caso de que no se logre insertar, no habra jugador que quede asociado
///////////////////////////////////////////////////////////////////////////////
bool
CArea::SetPlayer(CPlayer* const pPlayer,
			     const AreaDefs::sTilePos& TileDest)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());
  // SOLO si parametros validos
  ASSERT(IsCellValid(TileDest));
  ASSERT(pPlayer);
  ASSERT((pPlayer->IsInitOk()) != 0);
  
  // ¿Posicion valida?
  if (IsCellWithContent(TileDest)) {
	// Se quita el jugador actual
	UnsetPlayer();

	// Se inserta la instancia jugador
	m_PlayerInfo.pPlayer = pPlayer;    
	InsertWorldEntityInTile(m_PlayerInfo.pPlayer->GetHandle(), TileDest);

	// Se asocia area como observer y se retorna
	pPlayer->AddObserver(this);
	return true;
  }  

  // No se pudo localizar jugador
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Localiza la celda adyacente en la direccion Orientation desde TilePosSrc y
//   la deposita en TilePostDest. En caso de que la posicion adyacente no sea
//   valida, se devolvera false indicando que la operacion no pudo realizarse 
//   con exito.
// Parametros:
// - TilePosSrc. Posicion desde donde obtener la celda adyacente.
// - AdjTileDirection. Direccion hacia donde comprobar la celda adyacente.
// - TilePosDest. Referencia a la posicion del tile donde depositar la celda
//   adyacente.
// Devuelve:
// - Si se ha podido realizar la operacion true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CArea::GetAdjacentTilePos(const AreaDefs::sTilePos& TilePosSrc,
						  const IsoDefs::eDirectionIndex& AdjTileDirection,
						  AreaDefs::sTilePos& TilePosDest)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());

  // Forma la POSIBLE posicion adyacente
  sDelta* const pDeltas = GetDeltasValues(TilePosSrc);
  ASSERT(pDeltas);
  const sword swXTile = TilePosSrc.XTile + pDeltas[AdjTileDirection].sbXDelta;
  if (swXTile < 0) { 
	return false; 
  }
  const sword swYTile = TilePosSrc.YTile + pDeltas[AdjTileDirection].sbYDelta;
  if (swYTile < 0) { 
	return false; 
  }
  TilePosDest.XTile = swXTile;
  TilePosDest.YTile = swYTile;

  // Retorna el resultado
  return (IsCellValid(TilePosDest) && IsCellWithContent(TilePosDest));
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Localiza al jugador y lo elimina del tile donde se halle. quitando cualquier
//   instancia de tipo jugador.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void CArea::UnsetPlayer(void)
{
  // Procede a eliminar el handle del tile
  if (m_PlayerInfo.pPlayer) {
	// Se quita del tile actual
	RemoveWorldEntityFromTile(m_PlayerInfo.pPlayer->GetHandle());

	// Se remueve el area como observer
	m_PlayerInfo.pPlayer->RemoveObserver(this);

	// Se destruye instancia
	ASSERT(m_PlayerInfo.pPlayer->IsInitOk());
	delete m_PlayerInfo.pPlayer;
	m_PlayerInfo.pPlayer = NULL;  	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Calcula la mascara de acceso al tile teniendo en cuenta la mascara del
//   floor y de todos las entidades que estan por encima de esta.
// - Tambien tendra en cuenta si existe una pared adyacente al noreste y
//   noroeste, en cuyo caso comprobara si estan bloqueadas. Si lo estuvieran,
//   el acceso a este tile tambien quedara influido por ese hecho.
// Parametros:
// - TilePos. Posicion del mapa de donde obtener la mascara de acceso.
// - udNoEntitiesToCheck. Los tipos de entidades que NO se desean chequear.
//   Por defecto se querra chequear todas.
// Devuelve:
// - Mascara de acceso.
// Notas:
// - En caso de que en la celda haya una entidad que se halle en movimiento,
//   no sera tenido en cuenta su mascara de acceso ya que pudiera ser que
//   se estuviera movimiento para dejar el tile y, por lo tanto, la mascara
//   de acceso asociada realmente no importaria. Solo se tendra en cuenta
//   para las criaturas.
///////////////////////////////////////////////////////////////////////////////
AreaDefs::MaskTileAccess 
CArea::GetMaskTileAccess(const AreaDefs::sTilePos& TilePos,
						 const dword udNoEntitiesToCheck)
{
  // SOLO si entidad inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());

  // Obtiene mascara de acceso asociada al floor
  AreaDefs::MaskTileAccess MaskTileAccess = GetMaskFloorAccess(TilePos);
  
  // Calcula la mascara de acceso final con las entidades sobre el floor
  // Nota: Solo en caso de que en dicha celda exista contenido  
  sNCell* const pCell = m_Map.pMap[GetTileIdx(TilePos)];
  if (pCell) {
	CellEntitiesListIt It(pCell->Entities.begin());
	while (It != pCell->Entities.end() && 
		   MaskTileAccess != AreaDefs::NO_TILE_ACCESS) {
	  // El tipo de entidad actual se desea utilizar
	  const RulesDefs::eEntityType EntityType = GetEntityType(*It);	  
	  if (!(EntityType & udNoEntitiesToCheck)) {	
		// SI, luego podemos chequear
		// ¿NO es una criatura?
		if (EntityType != RulesDefs::CRIATURE &&
			EntityType != RulesDefs::PLAYER) {
		  // Si, se toma la mascara directamente
		  MaskTileAccess |= GetWorldEntity(*It)->GetObstacleMask();
		} else {
		  // No, se tomara la mascara SOLO si no hay movimiento
		  CCriature* const pCriature = GetCriature(*It);
		  ASSERT(pCriature);
		  if (!pCriature->IsWalking()) {
			MaskTileAccess |= pCriature->GetObstacleMask();
		  }
		}
	  }

	  // Sig.	  
	  ++It;
	}  
  }  
  
  // Se procede a obtener informacion de las paredes adyacentes
  
  // Paredes adyacentes con direccion suroeste
  // Norte
  if (!(MaskTileAccess & IsoDefs::NORTH_FLAG) &&
	  IsAdjWallBlocked(TilePos, IsoDefs::NORTH_INDEX, RulesDefs::WO_SOUTHWEST)) {
	MaskTileAccess |= IsoDefs::NORTH_FLAG;
  }
  
  // Noreste
  if (!(MaskTileAccess & IsoDefs::NORTHEAST_FLAG) &&
	  IsAdjWallBlocked(TilePos, IsoDefs::NORTHEAST_INDEX, RulesDefs::WO_SOUTHWEST)) {
	MaskTileAccess |= IsoDefs::NORTHEAST_FLAG;
  }
  // Este
  if (!(MaskTileAccess & IsoDefs::EAST_FLAG) &&
	  IsAdjWallBlocked(TilePos, IsoDefs::EAST_INDEX, RulesDefs::WO_SOUTHWEST)) {
	MaskTileAccess |= IsoDefs::EAST_FLAG;
  }

  // Paredes adyacentes con direccion sureste
  // Norte
  if (!(MaskTileAccess & IsoDefs::NORTH_FLAG) &&
	  IsAdjWallBlocked(TilePos, IsoDefs::NORTH_INDEX, RulesDefs::WO_SOUTHEAST)) {
	MaskTileAccess |= IsoDefs::NORTH_FLAG;
  }
  // Noroeste
  if (!(MaskTileAccess & IsoDefs::NORTHWEST_FLAG) &&
	  IsAdjWallBlocked(TilePos, IsoDefs::NORTHWEST_INDEX, RulesDefs::WO_SOUTHEAST)) {
	MaskTileAccess |= IsoDefs::NORTHWEST_FLAG;
  }
  // Oeste
  if (!(MaskTileAccess & IsoDefs::WEST_FLAG) &&
	  IsAdjWallBlocked(TilePos, IsoDefs::WEST_INDEX, RulesDefs::WO_SOUTHEAST)) {
	MaskTileAccess |= IsoDefs::WEST_FLAG;
  }
  
  // Retorna la mascara
  return MaskTileAccess;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dado el tile TilePosSrc, se comprueba si el que esta en la direccion
//   adyacente AdjTileDirection, tiene alguna pared con orientacion 
//    WallOrientation y ademas esta se encuentra bloqueada. 
// Parametros:
// - TilePosSrc. Tile desde donde se desean realizar las comprobaciones.
// - AdjTileDirection. Orientacion para el tile adyacente.
// - WallOrientation. Orientacion que debe de tener la pared orientada.
// Devuelve:
// - Si la pared esta bloqueada y tiene orientacion WallOrientation true.
//   En caso contrario false.
// Notas:
// - Este metodo servira de apoyo a GetMaskTileAccess para obtener la
//   mascara de acceso a un tile cuando adyacente a este pueda existir una
//   pared.
// - El metodo no supondra que la celda adyacente tiene que ser valida, por lo
//   que en caso de que no lo sea, devolvera directamente false.
///////////////////////////////////////////////////////////////////////////////
bool 
CArea::IsAdjWallBlocked(const AreaDefs::sTilePos& TilePosSrc,
						const IsoDefs::eDirectionIndex& AdjTileDirection,
						const RulesDefs::eWallOrientation& WallOrientation)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());
  
  // Se halla el tile adyacente
  AreaDefs::sTilePos AdjTilePos;
  if (GetAdjacentTilePos(TilePosSrc, AdjTileDirection, AdjTilePos)) {
	// ¿Celda valida y con contenido?
	if (IsCellValid(AdjTilePos) && IsCellWithContent(AdjTilePos)) {
	  // Se navega por la celda
	  sNCell* const pCell = m_Map.pMap[GetTileIdx(AdjTilePos)];
	  ASSERT(pCell);
	  CellEntitiesListIt It(pCell->Entities.begin());
	  for (; It != pCell->Entities.end(); ++It) {
		// ¿Estamos sobre una entidad que es pared?
		if (GetEntityType(*It) == RulesDefs::WALL) {
		  // Tomamos la entidad como pared
		  CWall* const pWall = GetWall(*It);
		  ASSERT(pWall);
		  // ¿Es de orientacion adecuada y esta bloqueada?
		  if (pWall->GetOrientation() == WallOrientation &&
			  pWall->IsBlocked()) {
			return true;
		  }
		}		
	  }  
	}
  }

  // No se encontro pared
  return false;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Calcula la direccion que hay que tomar para llegar desde el TilePosSrc a
//   TilePosDest, siendo ambas posiciones de tiles adyacentes.
// - En caso de que las posiciones suministradas NO SEAN adyacentes, se
//   calculara la orientacion aproximada a tomar.
// Parametros:
// - TilePosSrc. Posicion del tile origen.
// - TilePosDest. Posicion del tile adyacente
// Devuelve:
// - Direccion que hay que tomar desde TilePosSrc para llegar a TilePosDest.
//   En caso de que los dos tiles no sean adyacentes, no habra direccion
//   posible.
// Notas:
// - El metodo no tendra en cuenta posibles restricciones de acceso.
///////////////////////////////////////////////////////////////////////////////
IsoDefs::eDirectionIndex 
CArea::CalculeDirection(const AreaDefs::sTilePos& TilePosSrc,
						const AreaDefs::sTilePos& TilePosDest)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());
  // SOLO si parametros validos
  ASSERT(IsCellValid(TilePosSrc));
  ASSERT(IsCellValid(TilePosDest));

  // Vbles
  AreaDefs::sTilePos AdjTilePos; // Pos de tile adyacente

  // Procede a calcular la direccion
  byte ubIt = 0;
  for (; ubIt < IsoDefs::MAX_DIRECTIONS; ++ubIt) {
	// Se calcula posicion del tile adyacente que proceda
	const IsoDefs::eDirectionIndex DirIdx = IsoDefs::eDirectionIndex(ubIt);
	if (GetAdjacentTilePos(TilePosSrc, DirIdx, AdjTilePos)) {
	  // ¿Es el tile destino?
	  if (TilePosDest == AdjTilePos) {
		// Se devuelve la direccion
		return DirIdx;
	  }
	}
  }

  // Estamos ante un caso de NO adyacencia, por lo que se calculara
  // la orientacion de forma aproximada
  if (TilePosSrc.XTile < TilePosDest.XTile) {
	// Posiciones al este
	if (TilePosSrc.YTile < TilePosDest.YTile) {
	  return IsoDefs::SOUTHEAST_INDEX;
	} else if (TilePosSrc.YTile > TilePosDest.YTile) {
	  return IsoDefs::NORTHEAST_INDEX;
	} else {
	  return IsoDefs::EAST_INDEX;
	}
  } else if (TilePosSrc.XTile > TilePosDest.XTile) {
	// Posiciones al oeste
	if (TilePosSrc.YTile < TilePosDest.YTile) {
	  return IsoDefs::SOUTHWEST_INDEX;
	} else if (TilePosSrc.YTile > TilePosDest.YTile) {
	  return IsoDefs::NORTHWEST_INDEX;
	} else {
	  return IsoDefs::WEST_INDEX;
	}
  } else {
	// Posiciones paralelas
	if (TilePosSrc.YTile < TilePosDest.YTile) {
	  return IsoDefs::SOUTH_INDEX;
	} else if (TilePosSrc.YTile > TilePosDest.YTile) {
	  return IsoDefs::NORTH_INDEX;
	}
  }

  // No hay direccion posible (o misma direccion)
  return IsoDefs::NO_DIRECTION_INDEX;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dada la posicion TilePos y la orientacion Orientation, el metodo se
//   encargara de habilitar, o no, el acceso al terreno, Floor, si bCanAccess
//   esta a true y a la inversa si esta a false.
// Parametros:
// - TilePos. Posicion del tile.
// - Orientation. Orientacion.
// - bCanAccess. ¿Se habilita el acceso?
// Devuelve:
// Notas:
// - En la mascara, un 1 significa que no se puede franquear y un 0 que si.
///////////////////////////////////////////////////////////////////////////////
void 
CArea::SetFloorAccess(const AreaDefs::sTilePos& TilePos,
					  const IsoDefs::eDirectionFlag& Orientation,
					  const bool bCanAccess)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());

  // Se forma la nueva mascara para el floor
  AreaDefs::MaskTileAccess MaskFloorAccess = GetMaskFloorAccess(TilePos);
  if (bCanAccess) {	
	// Se puede acceder, luego se pone un 0 en el bit asociado a la direccion
	MaskFloorAccess &= !Orientation;
  } else {
	// No se puede acceder, luego se pone un 1 en el bit asociado a la direccion
	MaskFloorAccess |= Orientation;
  }

  // Ahora se establece formalmente la nueva mascara para ese tile
  SetMaskFloorAccess(TilePos, MaskFloorAccess);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece la mascara de acceso de un Floor.
// - Para ahorrar memoria, se utilizara un map en donde se guardaran los pares
//   posicion tile, mascara de acceso. Cuando no haya entrada para un tile, se
//   entendera que dicho tile tiene la mascara de acceso completamente libre.
//   Esto es, solo habra mascara de acceso para aquellos tiles con restricciones
//   de franqueo.
// Parametros:
// - sTilePos. Posicion del tile donde esta el floor.
// - MaskTileAccess. Mascara de acceso.
// Devuelve:
// Notas:
// - Debido a que este metodo se llamara en etapa de configuracion, no se
//   debera de encontrar informacion previa asociada a un tile
///////////////////////////////////////////////////////////////////////////////
void
CArea::SetMaskFloorAccess(const AreaDefs::sTilePos& TilePos,
					      const AreaDefs::MaskTileAccess& MaskTileAccess)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk() && IsAreaLoaded());

  // Se obtiene posible entrada en el indice de mascaras de accesos
  const AreaDefs::TileIndex TileIdx = GetTileIdx(TilePos);
  ASSERT((m_Map.IndexOfMaskTileAccess.find(TileIdx) == m_Map.IndexOfMaskTileAccess.end()) != 0);

  // ¿No es acceso total?
  if (AreaDefs::ALL_TILE_ACCESS != MaskTileAccess) {
	// Se crea entrada asociando mascara
    m_Map.IndexOfMaskTileAccess.insert(MaskTileAccessMapValType(TileIdx, MaskTileAccess));
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el iterador al map donde se localizan los tags. Se obtendra via
//   handle a la entidad de la que se desea averiguar si tiene tag asociado.
// Parametros:
// - hEntity. Handle a la entidad.
// Devuelve:
// - Iterador al posible tag asociado (sera nulo si no hay)
// Notas:
///////////////////////////////////////////////////////////////////////////////
CArea::TagMapIt 
CArea::GetMapTagIt(const AreaDefs::EntHandle& hEntity)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  ASSERT(IsAreaLoaded());
  // SOLO si parametros validos
  ASSERT(hEntity);

  // Realiza una busqueda en el map de tags
  TagMapIt It(m_EntityTags.begin());
  for (; It != m_EntityTags.end(); ++It) {
	// ¿Handle buscado?
	if (It->second == hEntity) {
	  // Se retorna
	  return It;
	}
  }

  // No se encontro
  return m_EntityTags.end();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el iterador. Para inicialiarse, sera necesario recibir el area
//   sobra la que se ira a iterar, asi como el tile en x e y. Si el tile apunta
//   a una zona no valida, no se inicializara. Lo mismo ocurrira si el area no
//   esta inicializado.
// Parametros:
// - pArea. Direccion del area.
// - TilePos. Posicion del tile.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - Por defecto, localiza el iterador en la posicion primera
///////////////////////////////////////////////////////////////////////////////
bool CCellIterator::Init(CArea* const pArea,
						 const AreaDefs::sTilePos& TilePos)
{
  // SOLO si parametros validos
  ASSERT(pArea);
  
  // Caso de una reinicializacion
  End();

  // ¿Area NO inicializada?
  if (!pArea->IsInitOk()) { 
	return false; 
  }
  
  // ¿Posicion NO valida? o 
  // ¿celda SIN contenido?
  if (!pArea->IsCellValid(TilePos) || 
	  !pArea->IsCellWithContent(TilePos)) { 
	return false; 
  }

  // Se obtiene la lista asociada al tile y se almacena
  ASSERT(pArea->m_Map.pMap);
  ASSERT(pArea->m_Map.pMap[pArea->GetTileIdx(TilePos)]);
  m_pList = &pArea->m_Map.pMap[pArea->GetTileIdx(TilePos)]->Entities;
  ASSERT(m_pList);

  // Se inicializan resto de vbles de miembro  
  m_bIsInitOk = true;
  m_pArea = pArea;
  SetAtFront();

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza el iterador.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void CCellIterator::End(void)
{
  // Se finaliza instancia
  if (IsInitOk()) {
	// Se desvinculan elementos
	m_pArea = NULL;
	m_pList = NULL;
	m_uwPos = 0;

	// Se baja flag
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia
// Parametros:
// - pArea. Direccion al area a utilizar.
// Devuelve:
// - Si todo es correcto true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CEntitiesIterator::Init(CArea* const pArea)
{
  // SOLO si parametros validos
  ASSERT(pArea);
  
  // Caso de una reinicializacion
  End();

  // ¿Area NO inicializada?
  if (!pArea->IsInitOk()) { 
	return false; 
  }
  
  // Se inicializan resto de vbles de miembro  
  m_bIsInitOk = true;  
  m_pArea = pArea;

  // Se asocia iterador al frente
  SetAtFront();

  // Todo correcto  
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza el iterador.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEntitiesIterator::End(void)
{
  // Finaliza si procede
  if (IsInitOk()) {
	// Desvincula punteros y finaliza
	ReleaseIt();
	m_pArea = NULL;
	m_bIsInitOk = false;
  }
}

