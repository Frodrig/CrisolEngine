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
// CIsoEngine.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CIsoEngine.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)
#include "CIsoEngine.h"

#include "DXWrapper\\DXDDSurfaceTexture.h"
#include "iCGraphicSystem.h"
#include "iCLogger.h"
#include "iCCommandManager.h"
#include "iCGameDataBase.h"
#include "CCBTEngineParser.h"
#include "CPath.h"
#include "CWorldEntity.h"
#include <algorithm>
#include <math.h>

// Inicializacion del subsistema de memoria
CMemoryPool 
CIsoEngine::CPathFinder::sNodeSearch::MPool(256,
											sizeof(CIsoEngine::CPathFinder::sNodeSearch), 
											true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el motor isometrico. Principalmente, se encarga de crear las
//   estructuras de datos para mantener la informacion de las celdas y el mapa
//   asociado al area activo.
// - Todos los parametros son considerados variables del sistema y tomados
//   desde el motor.
// Parametros:
// Modifica:
// - m_bIsInitOk. Levanta el flag.
// - Se inicializan todos los subsistemas del motor.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se dejara reinicializar. Para eso, sera necesario llamar antes a End.
///////////////////////////////////////////////////////////////////////////////
bool 
CIsoEngine::Init(void)
{
  // ¿Se intenta inicializar?
  if (IsInitOk()) { return false; }

  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("CIsoEngine::Init> Inicializando el motor isométrico.\n");
  #endif
  
  // Se obtiene interfaz a otros modulos del motor  
  m_pCmdManager = SYSEngine::GetCommandManager();
  ASSERT(m_pCmdManager);
  m_pGraphSystem = SYSEngine::GetGraphicSystem();
  ASSERT(m_pGraphSystem);
  m_pResManager = SYSEngine::GetResourceManager();
  ASSERT(m_pResManager);
  m_pMathUtil = SYSEngine::GetMathUtil();
  ASSERT(m_pMathUtil);

  // Se levanta el flag de inicializacion para permitir llamadas End
  m_bIsInitOk = true;

  // Procede a inicializar subsistemas y elementos de informacion  

  // Mapa isometrico  
  if (InitIsoMap()) { 
  	// Interfaz sobre el mapa isometrico  
	if (InitSelectCellSys()) { 
	  // Sistema de transparencia para objetos  
	  if (InitTranspSys()) {
		// Sistema de camara  
		if (InitIsoCamera()) { 
		  // Sistema de busqueda
		  if (m_PathFinder.Init()) {   
			// Todo correcto
			#ifdef ENGINE_TRACE
			  SYSEngine::GetLogger()->Write("                | Ok.\n");
			#endif    
			return true;
		  }
		}
	  }
	}
  }

  // Hubo problemas inicializando
  End();
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de crear un array de valores Delta que contiene los
//   valores delta para poder hallar correctamente el tile en el que se encuentra
//   un punto en el universo de juego. Para ello, utilizara un grafico que
//   contendra un tile isometrico encerrado en un rectangulo y dividido en 5
//   zonas, cada una de un color. Lo que se hara entonces, sera recorrer dicho
//   grafico y dependiendo del color del pixel en el que nos encontremos,
//   guardar unos valores delta determinados. Esto se hara asi para incrementar
//   la velocidad de calculo cuando se utilice despues el motor isometrico.
// - Toda la informacion se aloja y obtiene de m_IsoMap.
// Parametros:
// - szMaskFileName. Nombre del archivo con la mascara de deltas.
// Devuelve:
// - Si todo ha ido bien true. En caso de hallar algun fallo false.
// Notas:
// - La codificacion por zonas sera:
//   * Zona 0 -> Rojo   (-1,-1)
//   * Zona 1 -> Verde  (0,-1)
//   * Zona 2 -> Azul   (-1,1)
//   * Zona 3 -> Negro  (0,1)
//   * Zona 4 -> Blanco (0,0)
///////////////////////////////////////////////////////////////////////////////
void
CIsoEngine::InitMaskDeltas(const std::string& szMaskFileName)
{
  // SOLO si parametros validos
  ASSERT(szMaskFileName.size());

  // Se carga la mascara del tile
  iCResourceManager* const pResManager = SYSEngine::GetResourceManager();
  ASSERT(pResManager);
  const ResDefs::TextureHandle hTileMask = pResManager->TextureRegister(szMaskFileName,
																		GraphDefs::BPP_16,
																		GraphDefs::ALPHA_BPP_0);
  ASSERT(hTileMask);
  
  // Obtiene el codigo de color por zona
  const dword udZoneColor[] = {
	pResManager->GetTexture(hTileMask)->GetPixel(0, 0),
	pResManager->GetTexture(hTileMask)->GetPixel(IsoDefs::TILE_WIDTH - 1, 0),
	pResManager->GetTexture(hTileMask)->GetPixel(0, IsoDefs::TILE_HEIGHT - 1),
	pResManager->GetTexture(hTileMask)->GetPixel(IsoDefs::TILE_WIDTH - 1, IsoDefs::TILE_HEIGHT - 1),
	pResManager->GetTexture(hTileMask)->GetPixel(IsoDefs::TILE_WIDTH_DIV, IsoDefs::TILE_HEIGHT_DIV)
  };
  
  // Se bloquea la textura para poder acceder a nivel de pixel
  pResManager->GetTexture(hTileMask)->Lock();
   
  // Se procede a recorrer la mascara, obteniendo los valores delta  
  sword YIt = 0;
  for (; YIt < IsoDefs::TILE_HEIGHT; YIt++) {
	sword XIt = 0;
    for (; XIt < IsoDefs::TILE_WIDTH; XIt++) {
      // Se toma el color y se halla la posicion donde almacenar deltas
      // en el array de valores deltas a devolver
      const dword udColor = pResManager->GetTexture(hTileMask)->GetPixel(XIt, YIt);
      const dword udZonePos = YIt * IsoDefs::TILE_WIDTH + XIt;	  
      // Se comprueba el color y se asignan valores delta
      if (udColor == udZoneColor[0]) {		
        m_SelectCellSys.Mask[udZonePos].sbXDelta = -1;
        m_SelectCellSys.Mask[udZonePos].sbYDelta = -1;      		
      } else if (udColor == udZoneColor[1]) {	
		m_SelectCellSys.Mask[udZonePos].sbXDelta = 0;
        m_SelectCellSys.Mask[udZonePos].sbYDelta = -1;		
      } else if (udColor == udZoneColor[2]) {		
		m_SelectCellSys.Mask[udZonePos].sbXDelta = -1;
        m_SelectCellSys.Mask[udZonePos].sbYDelta = 1;		
      } else if (udColor == udZoneColor[3]) {		
		m_SelectCellSys.Mask[udZonePos].sbXDelta = 0;
        m_SelectCellSys.Mask[udZonePos].sbYDelta = 1;		
      } else if (udColor == udZoneColor[4]) {		
		m_SelectCellSys.Mask[udZonePos].sbXDelta = 0;
        m_SelectCellSys.Mask[udZonePos].sbYDelta = 0;		
      }
    } // ~ for x
  } // ~ for y

  // Se desbloquea textura y se libera
  pResManager->GetTexture(hTileMask)->UnLock();
  pResManager->ReleaseTexture(hTileMask);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el sistema de seleccion de celdas. 
// Parametros:
// Devuelve:
// - Resultado de la inicializacion.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CIsoEngine::InitSelectCellSys(void)
{
  // Se cargan deltas
  InitMaskDeltas(SYSEngine::GetGameDataBase()->GetTileRegionMaskFileName());
  
  // Se obtienen los valores que hace que 2 a ese valor sea igual a la altura
  // y anchura de los tiles. Esto servira para permitir que en el metodo de
  // ScreenToTile, se puedan realizar calculos mas rapidamente de forma independiente
  // a las dimensiones de los tiles.
  // Altura
  byte ubTemp = IsoDefs::TILE_HEIGHT;
  m_SelectCellSys.ubHeightPow = 0;
  do { 
	m_SelectCellSys.ubHeightPow++; ubTemp >>= 1; 
  } while (1 != ubTemp);  
  // Anchura
  ubTemp = IsoDefs::TILE_WIDTH;
  m_SelectCellSys.ubWidthPow = 0;
  do { 
	m_SelectCellSys.ubWidthPow++; ubTemp >>= 1; 
  } while (1 != ubTemp);  
  

  // Inicialmente la seleccion estara desactivada
  m_SelectCellSys.bDraw = false;

  // Asocia la posicion 0,0 de dibujado
  m_SelectCellSys.TilePos = AreaDefs::sTilePos(0, 0);
  m_SelectCellSys.IdxPos = 0;

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el mapa isometrico.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CIsoEngine::InitIsoMap(void)
{
  // Se inicializan las posiciones de comienzo de dibujado en pantalla
  m_IsoMap.sbYScrInit = -IsoDefs::TILE_HEIGHT_DIV;
  m_IsoMap.sbXScrEvenInit = -IsoDefs::TILE_WIDTH_DIV;
  m_IsoMap.sbXScrOddInit = 0;	

  // Posiciones iniciales
  m_IsoMap.TilePos = AreaDefs::sTilePos(0, 0);

  // Flag de dibujado
  m_IsoMap.bDraw = false;

  // Flags de entidades de dibujado
  m_IsoMap.ubNormalDrawFlags = AreaDefs::DRAW_FLOOR | AreaDefs::DRAW_SCENE_OBJ | 
  							   AreaDefs::DRAW_ITEM | AreaDefs::DRAW_CRIATURE | 
							   AreaDefs::DRAW_WALL | AreaDefs::DRAW_ROOF; 
  m_IsoMap.ubEntityMovExDrawFlags = AreaDefs::DRAW_CRIATURE;	 
  m_IsoMap.ubExLayersDrawFlags = AreaDefs::DRAW_FLOOR |
								 AreaDefs::DRAW_ITEM |
								 AreaDefs::DRAW_SCENE_OBJ | 
								 AreaDefs::DRAW_CRIATURE | 
								 AreaDefs::DRAW_WALL | 
								 AreaDefs::DRAW_ROOF;  
  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el sistema de scroll y la ventana de visualizacion.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CIsoEngine::InitIsoScroll(void)
{  
  // Velocidad de scroll
  m_ScrollInfo.uwSpeed = 196;

  // Se establecen valores de coordenadas
  m_ScrollInfo.fXWorld = 0.0f;
  m_ScrollInfo.fYWorld = 0.0f;

  // Se inicializan valores asociados al area en tiles visible
  m_IsoMap.rTileViewArea = sRect(0, 0, 0, 0);
  
  // Se inicializa la ventana de dibujado
  m_IsoMap.rWindow.swLeft = 0;
  m_IsoMap.rWindow.swTop = 0;
  SetDrawWindow(m_pGraphSystem->GetVideoWidth(), 
				m_pGraphSystem->GetVideoHeight());    
  m_ScrollInfo.ubXOffset = 0.0f;
  m_ScrollInfo.ubYOffset = 0.0f;

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el subsistema de camara.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CIsoEngine::InitIsoCamera(void)
{  
  // Valores no configurables
  m_CameraInfo.hEntity = 0;

  // Habitacion asociada
  m_CameraInfo.Room = 0;

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el subsistema para el trabajo con la transparencia de objetos.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CIsoEngine::InitTranspSys(void)
{ 
  // Se establece el valor alpha de transparencia
  CCBTEngineParser* const pParser = SYSEngine::GetGameDataBase()->GetCBTParser(GameDataBaseDefs::CBTF_GFX_DEFS,
																			   "[FXDefs]");
  ASSERT(pParser);
  pParser->SetVarPrefix("Walls.");
  m_TranspSys.Alpha = pParser->ReadAlpha("AutoTransparentMode.");

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::End(void)
{
  if (IsInitOk()) {
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CIsoEngine::End> Finalizando motor isométrico.\n");        
	#endif	
	
	// Se desvincula posible area
	SetArea(NULL);
	
	// Se finaliza buscador de areas
	m_PathFinder.End();

	// Baja el flag
	m_bIsInitOk = false;
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("               | Ok.\n");        
	#endif	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Vincula un area al motor isometrico, para que este pueda tener datos con
//   los que trabajar. En caso de pasar NULL, el motor isometrico desvinculara
//   cualquier area que tuviera asociada.
// - Tambien establece los topes para el sistema de scroll.
// - Al asociar un nuevo area, tambien se instala el cliente de entrada y al
//   desvincularlo, se desinstala.
// - Por ultimo, en la asociacion y eliminacion de areas, se activaran y se 
//   quitara la ventana de interfaz principal
// Parametros:
// - pArea. Direccion del area con el que se va a trabajar.
// Devuelve:
// Notas:
// - El interfaz se desinstalara automaticamente cuando se asocie un area 
//   nula, sin embargo no se asociara cuando se vincule un area valida. Esta
//   tarea se dejar siempre a los metodos de asociacion de camara.
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::SetArea(CArea* const pArea)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Se quiere desvincular el area actual?
  if (NULL == pArea) { 
	// ¿Hay area actualmente asociada?
	if (m_IsoMap.pArea) {
	  // Se desvincula como observer de CWorld
	  SYSEngine::GetWorld()->RemoveObserver(this);
	  
	  // Se desvincula camara
	  QuitCamera();	
	  
	  // Se baja el flag de dibujado
	  SetDrawFlag(false);
	  
	  // Se deselecciona selector de floor
	  SetDrawSelectCell(false);	
	  
	  // Se desvincula area
	  m_IsoMap.pArea = NULL;
	  
	  // Se finalizan todos los posibles comandos activos
	  m_ScrollInfo.ScrollCmd.End();
	  m_CameraInfo.TravellingCmd.End();
	  m_CameraInfo.MovIsoCmd.End();
	  
	  // Se asocia area nula al buscador de caminos
	  m_PathFinder.SetArea(NULL);		
	}
  } else {
	// Se vincula un nuevo area
	m_IsoMap.pArea = pArea;  

	// Se inicializa localizador de caminos
	m_PathFinder.SetArea(pArea);
	ASSERT(m_PathFinder.IsInitOk());
  
	// Se levanta flag de dibujado
	SetDrawFlag(true);

	// Inicializacion de la camara
	InitIsoCamera();

	// Se inicializan los valores para el scroll
	InitIsoScroll();  

	// Se vincula como observer de CWorld
	SYSEngine::GetWorld()->AddObserver(this);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dara una posicion de pantalla swXScreen, swYScreen se pasara a una
//   posicion de celda y devolvera true en caso de que sea una posicion valida.
//   Tambien guardara dicha informacion para establecer la nueva posicion de
//   dibujado de la representacion visual de una celda seleccionada.
// Parametros:
// - swXScreen, swYScreen. Posicion en pantalla asociada a la celda que se
//   quiere seleccionar.  
// Devuelve:
// - El tipo de celda que se pretendia seleccionar. Consultar IsoDefs
// Notas:
///////////////////////////////////////////////////////////////////////////////
IsoDefs::eSelectCellType  
CIsoEngine::SelectCell(const sword& swXScreen, 
					   const sword& swYScreen)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Obtiene la celda asociada a esa posicion de pantalla
  AreaDefs::sTilePos TilePos;
  ScreenToTile(swXScreen, swYScreen, TilePos);

  // ¿Celda valida y con contenido?
  if (m_IsoMap.pArea->IsCellValid(TilePos) &&
	  m_IsoMap.pArea->IsCellWithContent(TilePos)) {	
	// Se actualizan valores
	m_SelectCellSys.TilePos = TilePos;
	m_SelectCellSys.IdxPos = m_IsoMap.pArea->GetTileIdx(TilePos);

	// Se establece selector si procede
	iCEntitySelector* const pSelector = SYSEngine::GetGUIManager()->GetMapSelector();
	ASSERT(pSelector);
	if (m_SelectCellSys.bDraw) {	  
	  m_IsoMap.pArea->SetFloorSelector(m_SelectCellSys.TilePos,
									   pSelector);
	}
	
	// ¿Celda accesible?
	if (m_IsoMap.pArea->GetMaskTileAccess(TilePos) != AreaDefs::NO_TILE_ACCESS) {
	  // Si, se retorna
	  return IsoDefs::SCT_ACCESIBLECELL;	  
	} else {
	  // No, se retorna
	  return IsoDefs::SCT_NOACCESIBLECELL;	  
	}
  } else {
	// Se desactiva, celda NO valida
	m_IsoMap.pArea->UnsetFloorSelector();	
	return IsoDefs::SCT_NOVALIDCELL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dada la posicion de pantalla uwXScreen, uwYScreen, se debera de obtener
//   la posicion del tile asociado.
// Parametros:
// - uwXScreen, uwYScreen. Coordenada a comprobar a que tile pertenece.
// - TilePos. Posicion del tile, aqui se depositara la informacion.
// Devuelve:
// - Por parametro, el tile al que corresponde el punto.
// Notas:
// - El metodo se apoyara en WorldToTile para obtener el tile asociado.
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::ScreenToTile(const sword& swXScreen, 
						 const sword& swYScreen,
						 AreaDefs::sTilePos& TilePos)
{
  // SOLO si clase inicializada
  ASSERT(IsInitOk());
  // SOLO si area vinculada
  ASSERT(IsAreaSet());
  
  // Para realizar la operacion, se generara una posicion universal a partir
  // de la posicion universal actual, de la posicion de pantalla recibida y
  // teniendo en cuenta que los tiles comienzan a dibujarse con -TILE_WIDTH_DIV y
  // -TILE_HEIGHT_DIV con offsets.  
  const float fXWorldPos = m_ScrollInfo.fXWorld + swXScreen + IsoDefs::TILE_WIDTH_DIV;
  const float fYWorldPos = m_ScrollInfo.fYWorld + swYScreen + IsoDefs::TILE_HEIGHT_DIV;

  // Se obtiene el tile a partir de la posicion universal generada
  WorldToTile(fXWorldPos, fYWorldPos, TilePos);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dada una posicion dentro del universo del area, se devolvera el tile al
//   que corresponde. 
// Parametros:
// - udXWorld, udYWorld. Posicion en x,y del area.
// - TilePos. Posicion del tile. Aqui se depositara la informacion.
// Devuelve:
// - Por parametro, el tile al que corresponde el punto.
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::WorldToTile(const float fXWorld, 
						const float fYWorld,
						AreaDefs::sTilePos& TilePos)
{ 
  // SOLO si clase inicializada
  ASSERT(IsInitOk());
  // SOLO si area vinculada
  ASSERT(IsAreaSet());

  // static const
  // Valores con los que realizar AND a modo de mod
  static const byte TILE_WIDTH_MOD = IsoDefs::TILE_WIDTH - 1;
  static const byte TILE_HEIGHT_MOD = IsoDefs::TILE_HEIGHT - 1;

  // Const
  word uwXWorld = word(fXWorld); // Cast a la posicion X 
  word uwYWorld = word(fYWorld); // Cast a la posicion Y 

  // Se halla la posicion del tile de forma bruta  
  TilePos.XTile = uwXWorld >> 6;        // uwXWorld / TILE_WIDTH
  TilePos.YTile = (uwYWorld >> 5) << 1; // 2 * (uwYWorld / TILE_HEIGHT)

  // Se obtienen los offsets de las posiciones
  const word uwXOff = uwXWorld & TILE_WIDTH_MOD;  // uwXWorld % TILE_WIDTH
  const word uwYOff = uwYWorld & TILE_HEIGHT_MOD; // uwYWorld % TILE_HEIGHT

  // Se calcula el valor final del tile añadiendo los deltas  
  const word uwIndex = uwYOff * IsoDefs::TILE_WIDTH + uwXOff;
  TilePos.XTile += m_SelectCellSys.Mask[uwIndex].sbXDelta;
  TilePos.YTile += m_SelectCellSys.Mask[uwIndex].sbYDelta;  
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado de la porcion del area que corresponda en pantalla.
//   Desde este metodo tambien se realizara el dibujado del cursor de movimiento
//   si procede.
// - Durante el proceso de dibujado, se realizaran chequeos para saber si hay
//   que realizar dibujados de las entidades moviles en los extremos del
//   area que se dibuja. Esto es asi porque la naturaleza del comando de
//   movimiento hace que en el mismo instante que una entidad comienza a 
//   moverse sea cambiada de posicion al tile adyacente, con lo que ese tile
//   no se dibujara si es adyacente al limite del area de dibujado y, por 
//   lo tanto, tampoco se dibujaran las entidades que haya encima. 
// Parametros:
// Devuelve:
// Notas:
// - Los valores de comienzo en x par e y estan almacenados en negativo.
// - Los offsets estan almacenados en positivo, luego se deberan de restar.
// - En caso de que no este asociada la camara, se abandonara sin dibujar.
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::Draw(void)
{  
  // SOLO si la clase esta inicializada
  ASSERT(IsInitOk());  
  
  // ¿HAY area asociada? Y
  // ¿HAY camara asociada? Y
  // ¿ESTA el flag de dibujado?
  if (IsAreaSet() &&
	  IsDrawFlagActive() &&
	  IsCameraAttached()) { 
	// Si, se procede a dibujar suelo y elementos sobre el mismo
	DrawArea(CDrawAreaFloor());	
	DrawArea(CDrawAreaOnFloor());	
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Implementa el bucle de dibujado de la porcion de area visible. Para saber
//   que elementos ha de dibujar, se recibira por parametro la instancia a la
//   clase que sobrecarga el operador () para implementar dicho metodo.
// Parametros:
// - DrawMode. Instancia a la clase con el metodo de dibujado.
// Devuelve:
// Notas:
// - Los valores de comienzo en x par e y estan almacenados en negativo.
// - Los offsets estan almacenados en positivo, luego se deberan de restar.
// - En caso de que no este asociada la camara, se abandonara sin dibujar.
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::DrawArea(CDrawArea& Draw)
{
  // SOLO si la clase esta inicializada
  ASSERT(IsInitOk());  

  // Inicializaciones en Y
  AreaDefs::sTilePos TilePos(m_IsoMap.TilePos);
  sword swYScreen = m_IsoMap.sbYScrInit - m_ScrollInfo.ubYOffset;  

  // Bucle central de dibujado
  while (swYScreen < m_IsoMap.swMaxYScreenDrawPos &&
		 TilePos.YTile < m_IsoMap.pArea->GetHeight()) {	
	// Inicializaciones en X
	TilePos.XTile = m_IsoMap.TilePos.XTile;	
	sword swXScreen = -m_ScrollInfo.ubXOffset;
	if (m_pMathUtil->IsEven(TilePos.YTile)) { 
	  swXScreen += m_IsoMap.sbXScrEvenInit; 
	}			
	
	// Bucle de dibujado principal	
	while (swXScreen < m_IsoMap.rWindow.swRight) {
	  ASSERT((TilePos.XTile < m_IsoMap.pArea->GetWidth()) != 0);	  
	  // Se dibuja
	  Draw(m_IsoMap.pArea, TilePos, swXScreen, swYScreen);

	  // Actualiza iteradores
	  ++TilePos.XTile;
	  swXScreen += IsoDefs::TILE_WIDTH;
	} // ~ while
	
	// Actualiza iteradores
	++TilePos.YTile;	
	swYScreen += IsoDefs::TILE_HEIGHT_DIV;
  } // ~ while	
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establecera el area de la ventana de dibujado. Siempre sera con respecto
//   a la esquina superior (0,0), asi, lo que realmente se hara sera
//   ajustar la anchura y altura de dibujado maxima permitida.
// - Tambien se encargara de ajustar las posiciones maximas de dibujado en
//   pantalla en X e Y.
// Parametros:
// - uwXTop, uwYTop. Topes validos en donde establecer los valores.
// Devuelve:
// Notas:
// - La ventana de dibujado especificara la porcion del mapa isometrico que
//   se querra dibujar.
// - La posicion maxima de dibujado en Y debera de tener en cuenta el espacio
//   que sera necesario para las 4 capas inferiores de pantalla que no se ven,
//   pues la altura de los elementos que tengan en su interior, si que podrian
//   sobresalir y verse.
///////////////////////////////////////////////////////////////////////////////
void CIsoEngine::SetDrawWindow(const word uwXTop, 
							   const word uwYTop)
{
  // SOLO si los parametros son validos
  ASSERT(uwXTop);
  ASSERT(uwYTop);
  ASSERT((uwXTop <= m_pGraphSystem->GetVideoWidth()) != 0);
  ASSERT((uwYTop <= m_pGraphSystem->GetVideoHeight()) != 0);
  ASSERT_MSG(m_IsoMap.pArea, "Error, aún no se ha asociado ningún área");
  
  // Almacena los valores
  m_IsoMap.rWindow.swRight = uwXTop;
  m_IsoMap.rWindow.swBottom = uwYTop;

  // Se establece las posiciones maximas para el universo de juego
  m_ScrollInfo.fXWorldMax = IsoDefs::TILE_WIDTH * m_IsoMap.pArea->GetWidth() - 
						    (m_IsoMap.rWindow.swRight - m_IsoMap.rWindow.swLeft) -
							IsoDefs::TILE_WIDTH;
  if (m_ScrollInfo.fXWorldMax < 0.0f) {
	m_ScrollInfo.fXWorldMax = 0.0f;
  }

  m_ScrollInfo.fYWorldMax = IsoDefs::TILE_HEIGHT_DIV * m_IsoMap.pArea->GetHeight() -
							(m_IsoMap.rWindow.swBottom - m_IsoMap.rWindow.swTop) - 
							IsoDefs::TILE_HEIGHT;
  if (m_ScrollInfo.fYWorldMax < 0.0f) {
	m_ScrollInfo.fYWorldMax = 0.0f;
  }  

  // Establece posiciones maximas de dibujado en pantalla
  // Nota: A la posicion maxima en Y se le añaden 256 pixels mas que es la
  // altura maxima que puede tener un elemento.
  m_IsoMap.swMaxXScreenDrawPos = m_IsoMap.rWindow.swRight + IsoDefs::TILE_WIDTH; 
  m_IsoMap.swMaxYScreenDrawPos = m_IsoMap.rWindow.swBottom + 8 * IsoDefs::TILE_HEIGHT;

  // Se establece el numero de tiles maximos posibles a DIBUJAR en horizontal y en vertical
  m_IsoMap.ubNumTilesAtHoriz = (m_IsoMap.swMaxXScreenDrawPos / IsoDefs::TILE_WIDTH);/* + 1;*/
  m_IsoMap.ubNumTilesAtVert = /*8 + */(m_IsoMap.swMaxYScreenDrawPos / IsoDefs::TILE_HEIGHT_DIV);
  //m_IsoMap.ubNumTilesAtHoriz = (m_IsoMap.swMaxXScreenDrawPos / IsoDefs::TILE_WIDTH);/* + 1;*/
  //m_IsoMap.ubNumTilesAtVert = /*8 + */(m_IsoMap.swMaxYScreenDrawPos / IsoDefs::TILE_HEIGHT_DIV);  

  // Se establece el area de dibujado en lo referido a tiles, notificando
  // la puesta, y no, en escena de las distintas entidades
  const sRect rOldArea(m_IsoMap.rTileViewArea);  
  SetTileViewArea();
  EntitiesVisiblesInScreenNotify(AreaDefs::sTilePos(rOldArea.swLeft, rOldArea.swTop),
								 AreaDefs::sTilePos(rOldArea.swRight, rOldArea.swBottom),
								 false);
  EntitiesVisiblesInScreenNotify(AreaDefs::sTilePos(m_IsoMap.rTileViewArea.swLeft, m_IsoMap.rTileViewArea.swTop),
								 AreaDefs::sTilePos(m_IsoMap.rTileViewArea.swRight, m_IsoMap.rTileViewArea.swBottom),
								 true);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desplaza la vision del area hacia la dcha. una distancia de fOffset unidades.
// Parametros:
// - fOffset. Distancia a desplazar.
// Devuelve:
// - Si hay desplazamiento true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CIsoEngine::MoveRight(const float fOffset)
{
  // SOLO si area vinculada
  ASSERT(IsAreaSet());

  // ¿Podemos hacer el desplazamiento?
  if (m_ScrollInfo.fXWorld == m_ScrollInfo.fXWorldMax) { 
	return false; 
  }  
  if (IsCameraAttachedToPlayer() &&
	  m_ScrollInfo.fXWorld >= m_CameraInfo.fXScrollMax) { 
	return false; 
  }
 
  // Se realiza el desplazamiento con offset aplicando clipping
  m_ScrollInfo.fXWorld += fOffset;
  if (m_ScrollInfo.fXWorld > m_ScrollInfo.fXWorldMax) {
	m_ScrollInfo.fXWorld = m_ScrollInfo.fXWorldMax;
  }

  // Se establece la nueva posicion del mapa y su offset
  m_IsoMap.TilePos.XTile = word(m_ScrollInfo.fXWorld) / IsoDefs::TILE_WIDTH;
  m_ScrollInfo.ubXOffset = word(m_ScrollInfo.fXWorld) & (IsoDefs::TILE_WIDTH - 1);      
  
  // Se recalcula area de vision y se notifica a entidades dentro y fuera del mismo  
  const sRect rOldArea(m_IsoMap.rTileViewArea);
  SetTileViewArea();  
  EntitiesVisiblesInScreenNotify(AreaDefs::sTilePos(rOldArea.swLeft, rOldArea.swTop),
								 AreaDefs::sTilePos(m_IsoMap.rTileViewArea.swLeft - 1, m_IsoMap.rTileViewArea.swBottom),
								 false);
  EntitiesVisiblesInScreenNotify(AreaDefs::sTilePos(rOldArea.swRight + 1, rOldArea.swTop),
								 AreaDefs::sTilePos(m_IsoMap.rTileViewArea.swRight, m_IsoMap.rTileViewArea.swBottom),
								 true);
  
  // Movimiento realizado
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desplaza la vision del area hacia la izq. una distancia de fOffset unidades.
// Parametros:
// - fOffset. Distancia a desplazar.
// Devuelve:
// - Si hay desplazamiento true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CIsoEngine::MoveLeft(const float fOffset)
{
  // SOLO si area vinculada
  ASSERT(IsAreaSet());

  // ¿Podemos hacer el desplazamiento?
  if (m_ScrollInfo.fXWorld == 0) { 
	return false; 
  }
  
  if (IsCameraAttachedToPlayer() &&
	  m_ScrollInfo.fXWorld <= m_CameraInfo.fXScrollMin) { 
	return false; 
  } 

  // Se realiza el desplazamiento con offset aplicando clipping
  m_ScrollInfo.fXWorld -= fOffset;
  if (m_ScrollInfo.fXWorld < 0) {
	m_ScrollInfo.fXWorld = 0;
  }

  // Se establece la nueva posicion del mapa y su offset
  m_IsoMap.TilePos.XTile = word(m_ScrollInfo.fXWorld) / IsoDefs::TILE_WIDTH;
  m_ScrollInfo.ubXOffset = word(m_ScrollInfo.fXWorld) & (IsoDefs::TILE_WIDTH - 1);

  // Se recalcula area de vision y se notifica a entidades dentro y fuera del mismo  
  const sRect rOldArea(m_IsoMap.rTileViewArea);
  SetTileViewArea();
  EntitiesVisiblesInScreenNotify(AreaDefs::sTilePos(m_IsoMap.rTileViewArea.swRight + 1, m_IsoMap.rTileViewArea.swTop),
								 AreaDefs::sTilePos(rOldArea.swRight, rOldArea.swBottom),
								 false);
  EntitiesVisiblesInScreenNotify(AreaDefs::sTilePos(m_IsoMap.rTileViewArea.swLeft, m_IsoMap.rTileViewArea.swTop),
								 AreaDefs::sTilePos(rOldArea.swLeft - 1, rOldArea.swBottom),
								 true);
  
  // Movimiento realizado
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desplaza la vision del area hacia arriba una distancia de fOffset unidades.
// Parametros:
// - fOffset. Distancia a desplazar.
// Devuelve:
// - Si hay desplazamiento true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CIsoEngine::MoveNorth(const float fOffset)
{
  // SOLO si area vinculada
  ASSERT(IsAreaSet());

  // ¿Podemos hacer el desplazamiento?
  if (m_ScrollInfo.fYWorld == 0) { 
	return false; 
  }  
  if (IsCameraAttachedToPlayer() &&
	  m_ScrollInfo.fYWorld <= m_CameraInfo.fYScrollMin) { 
	return false; 
  } 

  // Se realiza el desplazamiento con offset aplicando clipping
  m_ScrollInfo.fYWorld -= fOffset;
  if (m_ScrollInfo.fYWorld < 0) { 
	m_ScrollInfo.fYWorld = 0; 
  }

  // Se establece la nueva posicion del mapa y su offset
  m_IsoMap.TilePos.YTile = word(m_ScrollInfo.fYWorld) / IsoDefs::TILE_HEIGHT_DIV;
  m_ScrollInfo.ubYOffset = word(m_ScrollInfo.fYWorld) & (IsoDefs::TILE_HEIGHT_DIV - 1);
  
  // Se recalcula area de vision y se notifica a entidades dentro y fuera del mismo  
  const sRect rOldArea(m_IsoMap.rTileViewArea);
  SetTileViewArea();
  EntitiesVisiblesInScreenNotify(AreaDefs::sTilePos(m_IsoMap.rTileViewArea.swLeft, m_IsoMap.rTileViewArea.swBottom + 1),
								 AreaDefs::sTilePos(rOldArea.swRight, rOldArea.swBottom),
								 false);
  EntitiesVisiblesInScreenNotify(AreaDefs::sTilePos(m_IsoMap.rTileViewArea.swLeft, m_IsoMap.rTileViewArea.swTop),
								 AreaDefs::sTilePos(rOldArea.swRight, rOldArea.swTop - 1),
								 true);  

  // Movimiento realizado
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desplaza la vision del area hacia abajo una distancia de fOffset unidades.
// Parametros:
// - fOffset. Distancia a desplazar.
// Devuelve:
// - Si hay desplazamiento true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CIsoEngine::MoveSouth(const float fOffset)
{
  // SOLO si area vinculada
  ASSERT(IsAreaSet());
  
  // ¿Podemos hacer el desplazamiento?
  if (m_ScrollInfo.fYWorld == m_ScrollInfo.fYWorldMax) { 
	return false; 
  }

  if (IsCameraAttachedToPlayer() &&
	  m_ScrollInfo.fYWorld >= m_CameraInfo.fYScrollMax) { 
	return false; 
  } 

  // Se realiza el desplazamiento con offset aplicando clipping
  m_ScrollInfo.fYWorld += fOffset;
  if (m_ScrollInfo.fYWorld > m_ScrollInfo.fYWorldMax) {
	m_ScrollInfo.fYWorld = m_ScrollInfo.fYWorldMax;
  }

  // Se establece la nueva posicion del mapa y su offset
  m_IsoMap.TilePos.YTile = word(m_ScrollInfo.fYWorld) / IsoDefs::TILE_HEIGHT_DIV;  
  m_ScrollInfo.ubYOffset = word(m_ScrollInfo.fYWorld) & (IsoDefs::TILE_HEIGHT_DIV - 1);  
  
  // Se recalcula area de vision y se notifica a entidades dentro y fuera del mismo  
  const sRect rOldArea(m_IsoMap.rTileViewArea);
  SetTileViewArea();
  EntitiesVisiblesInScreenNotify(AreaDefs::sTilePos(rOldArea.swLeft, rOldArea.swTop),
								 AreaDefs::sTilePos(m_IsoMap.rTileViewArea.swRight, m_IsoMap.rTileViewArea.swTop - 1),
								 false);
  EntitiesVisiblesInScreenNotify(AreaDefs::sTilePos(rOldArea.swLeft, rOldArea.swBottom + 1),
								 AreaDefs::sTilePos(m_IsoMap.rTileViewArea.swRight, m_IsoMap.rTileViewArea.swBottom),
								 true);

  // Movimiento realizado
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de postear el comando de scroll en la cola de comandos. Este
//   metodo sera el utilizado para realizar un scroll en el mapa desde el
//   interfaz.
// Parametros:
// - Direction. Direccion en la que se realizara el movimiento del scroll.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::MoveTo(const IsoDefs::eDirectionIndex& Direction)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se procede a establecer el comando
  const bool bMustPost = !m_ScrollInfo.ScrollCmd.IsActive();
  m_ScrollInfo.ScrollCmd.Init(Direction, GetScrollSpeed(), IsoDefs::TILE_WIDTH);
  if (bMustPost) {
	m_pCmdManager->PostCommand(&m_ScrollInfo.ScrollCmd);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de quitar la camara, desvinculando valores. Este metodo se
//   llamara siempre antes de cambiar asociar una camara, para liberar 
//   informacion previa, que pudiera estar asociada a otra camara.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::QuitCamera(void) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Estaba asociada?
  if (m_CameraInfo.bAttached) {
	// Si la entidad ERA una criatura, se desvincula el isometrico como observer
	if (m_CameraInfo.hEntity) {
	  const RulesDefs::eEntityType EntityType = m_IsoMap.pArea->GetEntityType(m_CameraInfo.hEntity);
	  if (EntityType == RulesDefs::PLAYER ||
  		  EntityType == RulesDefs::CRIATURE) {
		CCriature* const pCriature = m_IsoMap.pArea->GetCriature(m_CameraInfo.hEntity);
		ASSERT(pCriature);
		pCriature->RemoveObserver(this);			  
	  }
	}

	// Se extra el comando del subsistema si procede
	if (m_CameraInfo.TravellingCmd.IsActive()) {
	  m_CameraInfo.TravellingCmd.End();
	  SYSEngine::GetCommandManager()->QuitCommand(&m_CameraInfo.TravellingCmd);  
	}  

  
	// Se desvinculan resto de valores	
	m_CameraInfo.hEntity = 0;
	m_CameraInfo.bAttached = false; 
	m_CameraInfo.bAttachedToPlayer = false;
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Centra la camara sobre la entidad que la tiene asociada siempre y cuando
//   no se este produciendo un scroll.
// Parametros:
// Devuelve:
// Notas:
// - Este metodo solo funcionara si la camara NO esta asociada a una posicion.
///////////////////////////////////////////////////////////////////////////////
void
CIsoEngine::CenterCamera(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Hay alguna entidad con la camara asociada?
  if (m_CameraInfo.hEntity) {
	// ¿NO hay un scroll activo?
	if (!m_ScrollInfo.ScrollCmd.IsActive()) {
	  // Se toma la instancia a entidad
	  CWorldEntity* const pEntity = m_IsoMap.pArea->GetWorldEntity(m_CameraInfo.hEntity);
	  ASSERT(pEntity);
	  
	  // Se halla la posicion central y se centra
	  float fXNewPos = 0.0f; 
	  float fYNewPos = 0.0f;  
	  FindCenterPosition(pEntity->GetTilePos().XTile * IsoDefs::TILE_WIDTH, 
						 pEntity->GetTilePos().YTile * IsoDefs::TILE_HEIGHT_DIV,
						 fXNewPos, 
						 fYNewPos);
	  SetWorldView(fXNewPos, fYNewPos);
	}
  }
} 

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asociara la camara a una entidad, de tal forma que todos los movimientos
//   o cambios que esta haga sobre el escenario seran reflejados de inmediato
//   en la zona de vision.
// Parametros:
// - hEntity. Handle a la entidad.
// - uwSpeed. Velocidad de travelling. Si vale 0 no habra travelling.
// - pClient, IDCommand, udExtraParam. Info asociada al posible cliente que
//   desee recibir notificacion de la finalizacion del travelling.
// Devuelve:
// Notas:
// - Si no se asocia por travelling y la entidad no es el jugador, el
//   interfaz se desactivara y viceversa.
// - En caso de que la entidad NO este activa, no se realizara asociacion 
//   alguna, respetandose la asociacion anterior.
// - Siempre se calcularan los valores maximos de scroll permitidos 
//   asociados al handle que tiene la camara. Lo normal es que estos 
//   valores interesen solo en caso de que la camara se asocie al jugador.  
// - Se podra asociar un cliente para recibir notificacion de la finalizacion
//   de la accion. En caso de no existir proceso de travelling, la notificacion
//   NUNCA se emitira ya que se entendera que la asociacion sera directa.
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::AttachCamera(const AreaDefs::EntHandle& hEntity, 
						 const word uwSpeed,
						 iCCommandClient* const pClient,
						 const CommandDefs::IDCommand& IDCommand,
						 const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si area asociada
  ASSERT(IsAreaSet());
  // SOLO si handle valido
  ASSERT(hEntity);

  // Si se permite asociar camara
  // Se desvincula info de anterior camara
  QuitCamera();
	  
  // Se almacena handle y se obtiene instancia a entidad
  m_CameraInfo.hEntity = hEntity;
  CWorldEntity* const pEntity = m_IsoMap.pArea->GetWorldEntity(hEntity);
  ASSERT(pEntity);

  // Nuevas posiciones de movimiento
  float fXNewPos = 0.0f; 
  float fYNewPos = 0.0f;  
  FindCenterPosition(pEntity->GetTilePos().XTile * IsoDefs::TILE_WIDTH, 
  					 pEntity->GetTilePos().YTile * IsoDefs::TILE_HEIGHT_DIV,
					 fXNewPos, 
					 fYNewPos);
  
  // ¿Hay que hacer travelling?
  if (uwSpeed > 0) {
    // Se inicializa el comando y se inserta en cola de comandos	
    m_CameraInfo.TravellingCmd.Init(fXNewPos, fYNewPos, uwSpeed);
	SYSEngine::GetCommandManager()->PostCommand(&m_CameraInfo.TravellingCmd,
												pClient,
												IDCommand,
												udExtraParam);
  } else {
    // Se centra la vista directamente
    SetWorldView(fXNewPos, fYNewPos);
  }

  // Se ajustan vbles de miembro
  if (hEntity != m_IsoMap.pArea->GetPlayer()->GetHandle()) {
    // Camara asociada a entidad distinta al jugador
    m_CameraInfo.bAttachedToPlayer = false;
  } else {
    // Camara asociada al jugador
    m_CameraInfo.bAttachedToPlayer = true;
    // Se calcula valores maximos de scroll permitido	  
    CalculeCameraScrollValues();
  }  

  // Se determina la visiblidad del posible techo asociado
  const AreaDefs::RoomID NewRoom = m_IsoMap.pArea->GetRoomAt(pEntity->GetTilePos());
  DetermineRoofVisibility(m_CameraInfo.Room, NewRoom);
  m_CameraInfo.Room = NewRoom;

  // Se actualizan los valores de transparencia asociados a paredes
  UpdateTransparentSys(pEntity->GetTilePos(), CIsoEngine::WALL_GROUP);   

  // ¿La entidad asociada es una criatura?
  if (pEntity->GetEntityType() == RulesDefs::PLAYER ||
	  pEntity->GetEntityType() == RulesDefs::CRIATURE) {
	// Si, se asocia el isometrico como observer
	CCriature* const pCriature = m_IsoMap.pArea->GetCriature(hEntity);
	ASSERT(pCriature);	
	pCriature->AddObserver(this);	
  }

  // Se establece camara como asociada
  m_CameraInfo.bAttached = true;        
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia la camara a una posicion del universo de juego, pero siendo esta
//   asociada a un tile.
// Parametros:
// - TilePos. Posicion del area.
// - uwSpeed. Velocidad de travelling. Si vale 0 no habra travelling.
// - pClient, IDCommand, udExtraParam. Info asociada al posible cliente que
//   desee recibir notificacion de la finalizacion del travelling.
// Devuelve:
// Notas:
// - Al establecerse la camara a una posicion NO podra usarse la entrada.
// - Se podra asociar un cliente para recibir notificacion de la finalizacion
//   de la accion. En caso de no existir proceso de travelling, la notificacion
//   NUNCA se emitira ya que se entendera que la asociacion sera directa.
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::AttachCamera(const AreaDefs::sTilePos& TilePos,
						 const word uwSpeed,
						 iCCommandClient* const pClient,
						 const CommandDefs::IDCommand& IDCommand,
						 const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si area asociada
  ASSERT(IsAreaSet());
  // SOLO si posicion valida
  ASSERT(m_IsoMap.pArea);
  ASSERT(m_IsoMap.pArea->IsCellValid(TilePos));
  
  // Si se permite asociar camara
  // Se desvincula info de anterior camara
  QuitCamera();

  // Nuevas posiciones de movimiento
  float fXNewPos = 0.0f; 
  float fYNewPos = 0.0f;  
  FindCenterPosition(TilePos.XTile * IsoDefs::TILE_WIDTH, 
  					 TilePos.YTile * IsoDefs::TILE_HEIGHT_DIV,
					 fXNewPos, 
					 fYNewPos);
  
  // ¿Hay que hacer travelling?
  if (uwSpeed > 0) {  
    // Se inicializa el comando y se inserta en cola de comandos	
    m_CameraInfo.TravellingCmd.Init(fXNewPos, fYNewPos, uwSpeed);
	SYSEngine::GetCommandManager()->PostCommand(&m_CameraInfo.TravellingCmd,
												pClient,
												IDCommand,
												udExtraParam);
  } else {
    // Se centra la vista directamente
    SetWorldView(fXNewPos, fYNewPos);
  }

  // Se determina la visiblidad del posible techo asociado
  const AreaDefs::RoomID NewRoom = m_IsoMap.pArea->GetRoomAt(TilePos);
  DetermineRoofVisibility(m_CameraInfo.Room, NewRoom);
  m_CameraInfo.Room = NewRoom;

  // Se actualizan los valores de transparencia asociados a paredes
  UpdateTransparentSys(TilePos, CIsoEngine::WALL_GROUP);   

  // Se establece camara asociada
  m_CameraInfo.bAttached = true;
  m_CameraInfo.bAttachedToPlayer = false;    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia la camara a una posicion de universo de juego, sobreescribiendo 
//   cualquier asociacion anterior.
// Parametros:
// - uwXWorldPos, uwYWorldPos. Posicion en x,y donde asociar la camara.
// - uwSpeed. Velocidad de travelling. Si vale 0 no habra travelling.
// - pClient, IDCommand, udExtraParam. Info asociada al posible cliente que
//   desee recibir notificacion de la finalizacion del travelling.
// Devuelve:
// Notas:
// - Al establecerse la camara a una posicion NO podra usarse la entrada.
// - Se podra asociar un cliente para recibir notificacion de la finalizacion
//   de la accion. En caso de no existir proceso de travelling, la notificacion
//   NUNCA se emitira ya que se entendera que la asociacion sera directa.
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::AttachCamera(const float fXWorldPos, 
						 const float fYWorldPos,
						 const word uwSpeed,
						 iCCommandClient* const pClient,
						 const CommandDefs::IDCommand& IDCommand,
						 const dword udExtraParam)
{
  // SOLO si instancia activa
  ASSERT(IsInitOk());
  // SOLO si area asociada
  ASSERT(IsAreaSet());

  // Se quita posible camara asociada
  QuitCamera();

  // Se localiza la posicion en donde centrar y desvincula handle a entidad
  float fXNewPos = 0.0f; // Nueva posicion en X
  float fYNewPos = 0.0f; // Nueva posicion en Y
  FindCenterPosition(fXWorldPos, fYWorldPos, fXNewPos, fYNewPos);

  // ¿Hay que hacer travelling?
  if (uwSpeed > 0) {  
    // Se inicializa el comando y se inserta en cola de comandos
    m_CameraInfo.TravellingCmd.Init(fXNewPos, fYNewPos, uwSpeed);
	SYSEngine::GetCommandManager()->PostCommand(&m_CameraInfo.TravellingCmd,
												pClient,
												IDCommand,
												udExtraParam);	
  } else {
    // Se centra la vista directamente y se desvincula el interfaz
    SetWorldView(fXNewPos, fYNewPos);
  }

  // Se determina la visiblidad del posible techo asociado
  AreaDefs::sTilePos TilePos;
  WorldToTile(fXNewPos, fYNewPos, TilePos);
  const AreaDefs::RoomID NewRoom = m_IsoMap.pArea->GetRoomAt(TilePos);
  DetermineRoofVisibility(m_CameraInfo.Room, NewRoom);
  m_CameraInfo.Room = NewRoom;

  // Se actualizan los valores de transparencia asociados a paredes
  UpdateTransparentSys(TilePos, CIsoEngine::WALL_GROUP);   

  // Se establece camara asociada
  m_CameraInfo.bAttached = true;
  m_CameraInfo.bAttachedToPlayer = false;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notifica a toda un area de celdas, con sus respectivas entidades, sobre
//   que han pasado a ser o no visibles en pantalla.
// Parametros:
// - TileArea. Area sobre la que actuar en terminos de posiciones de tiles.
// - bVisibles.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::EntitiesVisiblesInScreenNotify(AreaDefs::sTilePos InitTilePos,
										   AreaDefs::sTilePos EndTilePos,
										   const bool bVisibles)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se acotan valores  
  ClipTilePos(InitTilePos);
  ClipTilePos(EndTilePos);

  // Se recorre el area recibida
  AreaDefs::sTilePos TilePosIt(InitTilePos);
  for (; TilePosIt.YTile <= EndTilePos.YTile; ++TilePosIt.YTile) {
	TilePosIt.XTile = InitTilePos.XTile;
	for (; TilePosIt.XTile <= EndTilePos.XTile; ++TilePosIt.XTile) {
	  // ¿Esta la celda en el interior del mapa? y 
	  // ¿Tiene la celda contenido?
	  if (m_IsoMap.pArea->IsCellValid(TilePosIt) &&
		  m_IsoMap.pArea->IsCellWithContent(TilePosIt)) {
		// Se toma el floor
		CEntity* pEntity = m_IsoMap.pArea->GetFloor(TilePosIt);
		ASSERT(pEntity);
		pEntity->VisibleInScreen(bVisibles);
		
		// Se recorren las entidades
		CCellIterator CellIt;
		CellIt.Init(m_IsoMap.pArea, TilePosIt);
		ASSERT(CellIt.IsInitOk());
		for (; CellIt.IsItValid(); CellIt.Next()) {
		  CWorldEntity* pEntity = m_IsoMap.pArea->GetWorldEntity(CellIt.GetWorldEntity());
		  ASSERT(pEntity);
		  pEntity->VisibleInScreen(bVisibles);
		}
	  }
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Controla la activacion / desactivacion de la pausa. La pausa funcionara
//   sobre el area de entidades visibles siempre que estas sean estaticas (no
//   se puedan mover) y sobre todas aquellas entidades que si se puedan mover
//   (las criaturas y jugador)
// Parametros:
// - bPause. Flag de activacion / desactivacion de la pausa.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::SetPause(const bool bPause)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se establecen los tiles de fin
  const AreaDefs::sTilePos EndTilePos(m_IsoMap.pArea->GetWidth(), 
									  m_IsoMap.pArea->GetHeight());
  
  // Se recorre el area visible
  AreaDefs::sTilePos TilePosIt(0, 0);
  for (; TilePosIt.YTile < EndTilePos.YTile; ++TilePosIt.YTile) {
	TilePosIt.XTile = 0;
	for (; TilePosIt.XTile < EndTilePos.XTile; ++TilePosIt.XTile) {
	  // ¿Tiene la celda contenido?
	  if (m_IsoMap.pArea->IsCellWithContent(TilePosIt)) {
		// Se toma el floor
		CFloor* const pFloor = m_IsoMap.pArea->GetFloor(TilePosIt);
		ASSERT(pFloor);
		pFloor->SetPause(bPause);

		// Se toma el posible techo
		CRoof* const pRoof = m_IsoMap.pArea->GetRoof(TilePosIt);
		if (pRoof) {
		  pRoof->SetPause(bPause);
		}
		
		// Se recorren las entidades
		CCellIterator CellIt;
		CellIt.Init(m_IsoMap.pArea, TilePosIt);
		ASSERT(CellIt.IsInitOk());
		for (; CellIt.IsItValid(); CellIt.Next()) {
		  CWorldEntity* const pEntity = m_IsoMap.pArea->GetWorldEntity(CellIt.GetWorldEntity());
		  ASSERT(pEntity);
		  pEntity->SetPause(bPause);		  
		}
	  }
	}
  }

  // Se finaliza cualquier comando de scroll que se halle activo
  m_ScrollInfo.ScrollCmd.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Localiza la posicion que hace que las coordenadas uwXWorldSrc y uwYWorldSrc
//   se hallen en el centro de pantalla, depositandolas en puwXWorldDest y
//   puwXWorldDest.
// Parametros:
// - uwXWorldSrc, uwYWorldSrc. Posicion en x,y a centrar
// - fXWorldDest, fYWorldDest. Posicion en x,y de la posicion anterior
//   centrada en pantalla.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::FindCenterPosition(const float fXWorldSrc, 
							   const float fYWorldSrc,
							   float& fXWorldDest, 
							   float& fYWorldDest)
{
  // Ancho y alto de la pantalla
  const sword swScreenWidth = m_IsoMap.rWindow.swRight - m_IsoMap.rWindow.swLeft;
  const sword swScreenHeight = m_IsoMap.rWindow.swBottom - m_IsoMap.rWindow.swTop;
  
  // Localiza la posicion central sin clipping
  float fXTemp = fXWorldSrc - float(swScreenWidth >> 1);
  float fYTemp = fYWorldSrc - float(swScreenHeight >> 1);

  // Clipping
  if (fXTemp < 0.0f) { 
	fXTemp = 0.0f; 
  } else if (fXTemp > m_ScrollInfo.fXWorldMax) { 
	fXTemp = m_ScrollInfo.fXWorldMax; 
  }
  if (fYTemp < 0.0f) { 
	fYTemp = 0.0f; 
  } else if (fYTemp > m_ScrollInfo.fYWorldMax) { 
	fYTemp = m_ScrollInfo.fYWorldMax; 
  }

  // Se asigna la posicion finalmente y se retorna
  fXWorldDest = word(fXTemp);
  fYWorldDest = word(fYTemp);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el calculo de la "ventana" en la que se podra realizar scroll a
//   partir de la posicion asociada a la entidad que tiene la camara.
// Parametros:
// Devuelve:
// Notas:
// - Este metodo servira para limitar la zona de vision de la entidad con la
//   camara.
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::CalculeCameraScrollValues(void)
{  
  // Const
  CWorldEntity* const pEntity = m_IsoMap.pArea->GetWorldEntity(m_CameraInfo.hEntity);
  ASSERT(pEntity);
  const word uwXWorldPos = pEntity->GetTilePos().XTile * IsoDefs::TILE_WIDTH;
  const word uwYWorldPos = pEntity->GetTilePos().YTile * IsoDefs::TILE_HEIGHT_DIV;

  // Se calcula scroll maximo permitido en X segun pos. de la entidad con camara
  m_CameraInfo.fXScrollMax = uwXWorldPos;
  if (m_CameraInfo.fXScrollMax > m_ScrollInfo.fXWorldMax) {
	m_CameraInfo.fXScrollMax = m_ScrollInfo.fXWorldMax;
  }

  // Se calcula scroll minimo permitido en X segun pos. de la entidad con camara
  m_CameraInfo.fXScrollMin = uwXWorldPos - 
							 (m_IsoMap.rWindow.swRight - m_IsoMap.rWindow.swLeft);
  if (m_CameraInfo.fXScrollMin < 0.0f) { 
	m_CameraInfo.fXScrollMin = 0.0f; 
  }

  // Se calcula scroll maximo permitido en Y segun pos. de la entidad con camara
  m_CameraInfo.fYScrollMax = uwYWorldPos;
  if (m_CameraInfo.fYScrollMax > m_ScrollInfo.fYWorldMax) { 
	m_CameraInfo.fYScrollMax = m_ScrollInfo.fYWorldMax;
  }

  // Se calcula scroll minimo permitido en Y segun pos. de la entidad con camara
  m_CameraInfo.fYScrollMin = uwYWorldPos -
							 (m_IsoMap.rWindow.swBottom - m_IsoMap.rWindow.swTop);
  if (m_CameraInfo.fYScrollMin < 0.0f) { m_CameraInfo.fYScrollMin = 0.0f; }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el filtrado del cambio de localidad en CArea para comprobar si la
//   entidad que se cambia de localidad, lo hace a una posicion visible o no
//   y asi comunicar de tal hecho.
// Parametros:
// - hEntity. Handle a la entidad.
// - NewPos. Nueva posicion donde cambiar de localidad.
// Devuelve:
// - Si se ha podido realizar el cambio true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CIsoEngine::ChangeLocation(const AreaDefs::EntHandle& hEntity,
						   const AreaDefs::sTilePos& NewPos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se intenta cambio de localidad
  if (m_IsoMap.pArea->ChangeLocation(hEntity, NewPos)) {
	// Se ha podido realizar, se realiza la notificacion de si
	// la entidad es visible o no y se retorna.
	CWorldEntity* const pEntity = m_IsoMap.pArea->GetWorldEntity(hEntity);
	ASSERT(pEntity);
	pEntity->VisibleInScreen(IsTilePosVisible(NewPos));
	return true;
  } else {
	// No se pudo realizar
	return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Cambia de localidad a un item, la nueva localidad sera un nuevo dueño
//   para el mismo. Al asignarle un dueño, es seguro que dejara de ser visible
//   en pantalla.
// Parametros:
// - hItem. Handle al item.
// - hEntity. Handle al dueño.
// Devuelve:
// - Si se ha podido efectuar el cambio true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CIsoEngine::ChangeLocation(const AreaDefs::EntHandle& hItem,
						   const AreaDefs::EntHandle& hEntity)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hItem);
  ASSERT(hEntity);

  // Se intenta realizar el cambio
  if (m_IsoMap.pArea->ChangeLocation(hItem, hEntity)) {
	// Se baja el flag de entidad visible y se retorna
	CItem* const pItem = m_IsoMap.pArea->GetItem(hItem);
	ASSERT(pItem);
	pItem->VisibleInScreen(false);
	return true;
  } else {
	// No se pudo realizar el cambio
	return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Cambia de localidad a una entidad PERO con el objetivo de realizar alguna
//   correccion de dibujado pues a titulo logico, la entidad seguira en otra
//   posicion distinta a la visual recibida. 
// - Este metodo sera utilizado en el movimiento de las criaturas cuando
//   el destino sera sur, sudoeste o sudeste pues en esos casos habra 
//   incongruencias visuales.
// Parametros:
// - hEntity. Entidad que queremos cambiar de posicion.
// Devuelve:
// Notas:
// - Este metodo dara por hecho muchas cosas, de ahi que los asertos hagan
//   que su ejecucion sea muy extricta.
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::ChangeLocationForVisuals(const AreaDefs::EntHandle& hEntity,
									 const AreaDefs::sTilePos& OriginalPos,
									 const AreaDefs::sTilePos& VisualPos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hEntity);

  // Se intenta realizar el cambio
  if (m_IsoMap.pArea->ChangeLocationForVisuals(hEntity, 
											   OriginalPos, 
											   VisualPos)) {
	// Se ha podido realizar, se realiza la notificacion de si
	// la entidad es visible o no y se retorna.
	CWorldEntity* const pEntity = m_IsoMap.pArea->GetWorldEntity(hEntity);
	ASSERT(pEntity);
	pEntity->VisibleInScreen(IsTilePosVisible(VisualPos));  
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece una nueva posicion en terminos de coordenadas universales en
//   en el area.
// Parametros:
// - uwXWorldPos, uwYWorldPos. Nuevas posiciones en x e y donde establecer
//   la posicion.
// Devuelve:
// Notas:
// - Este metodo actua primeramente sobre las posiciones universales.
// - Los offset se configuraran a 0.
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::SetWorldView(const float fXWorldPos, 
						 const float fYWorldPos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Inicilizaciones
  m_ScrollInfo.fXWorld = fXWorldPos;
  m_ScrollInfo.fYWorld = fYWorldPos;

  // Se hace clipping si procede
  if (m_ScrollInfo.fXWorld > m_ScrollInfo.fXWorldMax) { 
	m_ScrollInfo.fXWorld = m_ScrollInfo.fXWorldMax; 
  }
  if (m_ScrollInfo.fYWorld > m_ScrollInfo.fYWorldMax) { 
	m_ScrollInfo.fYWorld = m_ScrollInfo.fYWorldMax; 
  }

  // Se establecen offsets a 0
  m_ScrollInfo.ubXOffset = 0;
  m_ScrollInfo.ubYOffset = 0;

  // Se hallan los indices asociados sobre el mapa
  m_IsoMap.TilePos.XTile = m_ScrollInfo.fXWorld / IsoDefs::TILE_WIDTH;
  m_IsoMap.TilePos.YTile = m_ScrollInfo.fYWorld / IsoDefs::TILE_HEIGHT_DIV;
  
  // Se recalcula area de vision y se notifica a entidades dentro y fuera del mismo  
  const sRect rOldArea(m_IsoMap.rTileViewArea);
  SetTileViewArea();
  EntitiesVisiblesInScreenNotify(AreaDefs::sTilePos(rOldArea.swLeft, rOldArea.swTop),
								 AreaDefs::sTilePos(rOldArea.swRight, rOldArea.swBottom),
								 false);
  EntitiesVisiblesInScreenNotify(AreaDefs::sTilePos(m_IsoMap.rTileViewArea.swLeft, m_IsoMap.rTileViewArea.swTop),
								 AreaDefs::sTilePos(m_IsoMap.rTileViewArea.swRight, m_IsoMap.rTileViewArea.swBottom),
								 true);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la posicion en la que se encuentra la entidad pEntity y la coloca
//   en los parametros swXScreen y swYScreen. En caso de que la entidad no sea
//   visible no realizara operacion alguna. 
// Parametros:
// - pEntity. Entidad sobre la que se quiere saber la posicion que ocupa en
//   pantalla.
// - swXScreen, swYScreen. Vbles en donde se depositiara la posicion de la
//   entidad en pantalla
// Devuelve:
// - En caso de que se hallan alojado las posiciones true. En caso contrario
//   false.
// Notas:
// - La posicion devuelta se correspondera con el tile base en donde la entidad
//   se dibujara. Sera responsabilidad del exterior adecuar esa posicion a los
//   propositos que sean.
///////////////////////////////////////////////////////////////////////////////
bool 
CIsoEngine::GetEntityScreenPos(CWorldEntity* const pEntity, 
							   sword& swXScreen, 
							   sword& swYScreen)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pEntity);
  ASSERT(pEntity->IsInitOk());

  // ¿Entidad NO visible?
  if (!IsTilePosVisible(pEntity->GetTilePos())) { 
	return false; 
  }

  // Se procede a determinar la posicion de la entidad en pantalla
  // Posicion en X
  swXScreen = -m_ScrollInfo.ubXOffset;
  if (m_pMathUtil->IsEven(m_IsoMap.TilePos.YTile)) { 
	swXScreen += m_IsoMap.sbXScrEvenInit; 
  }			
  swXScreen += (m_IsoMap.TilePos.XTile + pEntity->GetTilePos().XTile) * 
			    IsoDefs::TILE_WIDTH;
  // Posicion en Y
  swYScreen = m_IsoMap.sbYScrInit - m_ScrollInfo.ubYOffset + 
			  (m_IsoMap.TilePos.YTile + pEntity->GetTilePos().YTile) * 
			  IsoDefs::TILE_HEIGHT_DIV;

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el cambio de la entidad hEntity al tile adyacente que esta
//   en direccion Dir. En caso de que bForce valga true, forzara el uso
//   de un filtrado segun camino de acceso a dicho tile.
// - En caso de que la criatura tenga la camara asociada, realizara tambien
//   un movimiento en el motor isometrico, para tenerlo siempre centrado.
// Parametros:
// - hEntity. Handle de la entidad a mover.
// - Dir. Direccion del tile adyacente donde cambiar.
// - bForce. Si true, se comprobara si el tile adyacente tiene acceso en la
//   direccion deseada. Si false, no contemplara este hecho.
// Devuelve:
// - Si se ha podido cambiar de area true, en caso contrario false.
// Notas:
// - En caso de que el tile destino sea no valido, devolvera false.
///////////////////////////////////////////////////////////////////////////////
bool 
CIsoEngine::MoveEntity(const AreaDefs::EntHandle& hEntity,
					   const IsoDefs::eDirectionIndex& Dir, 
					   const bool bForce)
{
  // SOLO si la clase esta inicializada
  ASSERT(IsInitOk());  
  // SOLO si area vinculada
  ASSERT(IsAreaSet());
      
  // Obtiene la entidad criatura
  CWorldEntity* const pEntity = m_IsoMap.pArea->GetWorldEntity(hEntity);
  ASSERT(pEntity);
  
  // Localiza el tile destino en funcion de la direccion de movimiento  
  AreaDefs::sTilePos TileDest;
  if (!m_IsoMap.pArea->GetAdjacentTilePos(pEntity->GetTilePos(), Dir, TileDest)) {
	return false;
  }

  // ¿ Hay que filtrar?  
  if (!bForce) {	
	// ¿NO se puede pasar al tile destino en la direccion deseada?
	if (m_IsoMap.pArea->GetMaskTileAccess(TileDest) & GetDirectionFlag(Dir)) { 
	  return false; 
	}
  }  

  // Ordena al area el cambio de localizacion para la entidad
  if (!m_IsoMap.pArea->ChangeLocation(hEntity, TileDest)) {
	// No se pudo mover
	return false;
  }

  // ¿Se mueve entidad con camara?
  if (hEntity == m_CameraInfo.hEntity) {
   // Se calcula valores maximos de scroll permitido
   CalculeCameraScrollValues();

   // Se procede a ajustar los valores de transparencia   
   UpdateTransparentSys(TileDest, CIsoEngine::WALL_GROUP);
   
   // Se determina la visiblidad del posible techo asociado
   const AreaDefs::RoomID NewRoom = m_IsoMap.pArea->GetRoomAt(TileDest);
   DetermineRoofVisibility(m_CameraInfo.Room, NewRoom);
   m_CameraInfo.Room = NewRoom;
  }

  // Movimiento realizado
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo encargado de determinar la visibilidad de los techos que se hallan
//   en la habitacion NewRoom, si es que esta es una habitacion valida.
// Parametros:
// - PrevRoom. Identificador de la posible habitacion previa.
// - NewRoom. Identificador de la posible habitacion nueva.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::DetermineRoofVisibility(const AreaDefs::RoomID& PrevRoom,
								    const AreaDefs::RoomID& NewRoom)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Los identificadores son distintos?
  if (PrevRoom != NewRoom) {
	// ¿Es el identificador previo valido?
	if (PrevRoom) {
	  // Si, se hace visible el techo asociado
	  m_IsoMap.pArea->SetRoomRoofVisible(PrevRoom, true);
	}
	// ¿Es el nuevo identificador valido?
	if (NewRoom) {
	  // Si, se hace invisible el techo asociado
	  m_IsoMap.pArea->SetRoomRoofVisible(NewRoom, false);
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Tranforma una direccion en formato indice a formato flag
// Parametros:
// - aubDir. Referencia a la direccion.
// Devuelve:
// - El flag con el path de acceso
// Notas:
///////////////////////////////////////////////////////////////////////////////
IsoDefs::eDirectionFlag 
CIsoEngine::GetDirectionFlag(const IsoDefs::eDirectionIndex& aDir) 
{
  // Vbles
  IsoDefs::eDirectionFlag DirFlag; // Flag con la direccion

  // Obtiene la direccion
  DirFlag = IsoDefs::NODIRECTION_FLAG;
  switch(aDir) {
	case IsoDefs::NORTH_INDEX:
	  DirFlag = IsoDefs::NORTH_FLAG;
	  break;

	case IsoDefs::NORTHEAST_INDEX:
	  DirFlag = IsoDefs::NORTHEAST_FLAG;
	  break;

	case IsoDefs::EAST_INDEX:
	  DirFlag = IsoDefs::EAST_FLAG;
	  break;

	case IsoDefs::SOUTHEAST_INDEX:
	  DirFlag = IsoDefs::SOUTHEAST_FLAG;
	  break;

	case IsoDefs::SOUTH_INDEX:
	  DirFlag = IsoDefs::SOUTH_FLAG;
	  break;

	case IsoDefs::SOUTHWEST_INDEX:
	  DirFlag = IsoDefs::SOUTHWEST_FLAG;
	  break;

	case IsoDefs::WEST_INDEX:
	  DirFlag = IsoDefs::WEST_FLAG;
	  break;

	case IsoDefs::NORTHWEST_INDEX:
	  DirFlag = IsoDefs::NORTHWEST_FLAG;
	  break;  
  };

  // Devuelve resultado
  return DirFlag;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo para obtener una entidad a partir de su posicion. Ver el metodo
//   SelectEntity sobrecargado con el valor del tile en X e Y asociado a la
//   posicion aqui recibida.
// Parametros:
// - uwXScreen, uwYScreen. Posicion en pantalla donde se esta seleccionando
//   un objeto.
// Devuelve:
// - Instancia universal a la entidad
// Notas:
// - La implementacion real se hallara en el metodo extendido.
///////////////////////////////////////////////////////////////////////////////
CWorldEntity* const 
CIsoEngine::GetEntityAt(const word uwXScreen, 
					    const word uwYScreen)
{
  // ¿Posicion valida?
  if (uwXScreen < m_IsoMap.rWindow.swLeft || 
	  uwXScreen > m_IsoMap.rWindow.swRight ||
	  uwYScreen < m_IsoMap.rWindow.swTop ||
	  uwYScreen > m_IsoMap.rWindow.swBottom) {
	return 0;
  }

    
  // Se obtiene el tile donde cae la seleccion
  AreaDefs::sTilePos TilePos;
  ScreenToTile(uwXScreen, uwYScreen, TilePos);

  // Se busca para obtener la entidad
  return GetEntityAt(TilePos, uwXScreen, uwYScreen);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza una busqueda de aquella entidad que se esta seleccionando. En caso
//   de hallar una, devolvera la instancia a la misma y NULL en caso contrario.
// - La busqueda se hara posicionandonos en la posicion mas alejada posible y
//   retrocediendo a la posicion XTile, YTile. Siempre que swXScreen, swYScreen
//   se hallen dentro del rectangulo asociado a una entidad que puede ser
//   seleccionada de alguna forma, se hara otra comprobacion a nivel de pixel
//   para ver si efectivamente se esta seleccionando a dicha entidad.
// Parametros:
// - TilePos. Posicion donde buscar
// - uwXScreen, uwYScreen. Posicion en pantalla donde se halla la seleccion.
// Devuelve:
// - La instancia a la entidad seleccionada. En caso de no hallarla NULL.
// Notas:
// - swXScreen, swYScreen se corresponden a una posicion de pantalla que cae
//   dentro del tile XTile, YTile.
///////////////////////////////////////////////////////////////////////////////
CWorldEntity* const 
CIsoEngine::GetEntityAt(const AreaDefs::sTilePos& TilePos,
					    const word uwXScreen,
					    const word uwYScreen)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se obtiene el array de deltas apropiado
  sDelta* const pDeltas = m_IsoMap.pArea->GetDeltasValues(TilePos);

  // Se ajusta la posicion maxima 
  word uwYMaxTile = TilePos.YTile + 2 * m_IsoMap.ubNumTilesAtVert;
  if (uwYMaxTile >= m_IsoMap.pArea->GetHeight()) {
	uwYMaxTile -= 2;
  }

  // Se establecen las direcciones a examinar
  const IsoDefs::eDirectionIndex AdyacentDir[] = { 
	IsoDefs::NO_DIRECTION_INDEX, IsoDefs::SOUTHWEST_INDEX, IsoDefs::SOUTHEAST_INDEX
  };

  // Procede a examinar comenzando por el tile mas alejado y retrocediendo
  // Recordar que dada la naturaleza isometrica del mapa, hay que decrementar en 2
  sword swYTileIt = uwYMaxTile;
  for (; swYTileIt >= TilePos.YTile; swYTileIt-=2) {
	// Se navegara por: Tile centra, adyacente al oeste, adyacente al este
	byte ubIt = 0;
	for (; ubIt < 3; ++ubIt) {	  
	  // Se obtiene la posicion asociada a la direccion.
	  // Se convendra que la posicion 0 sera la central y por lo tanto no
	  // habra que añadir ningun tipo de delta
	  const sword swXTile = TilePos.XTile + (ubIt > 0 ? pDeltas[AdyacentDir[ubIt]].sbXDelta : 0);
	  if (swXTile < 0) { 
		continue; 
	  }
	  const sword swYTile = swYTileIt + (ubIt > 0 ? pDeltas[AdyacentDir[ubIt]].sbYDelta : 0); 	  
	  if (swYTile < 0) { 
		continue; 
	  }
	  const AreaDefs::sTilePos NextTilePos(swXTile, swYTile);
	  // ¿NO es posicion valida o SIN contenido?
	  if (!m_IsoMap.pArea->IsCellValid(NextTilePos) ||
		  !m_IsoMap.pArea->IsCellWithContent(NextTilePos)) { 
		continue; 
	  }

	  // Se obtiene el floor
	  CFloor* const pFloor = m_IsoMap.pArea->GetFloor(NextTilePos);
	  ASSERT(pFloor);
	  
	  // Se procede a navegar por la celda
	  CCellIterator CellIt;
	  CellIt.Init(m_IsoMap.pArea, NextTilePos);
	  ASSERT(CellIt.IsInitOk());	  
	  for (; CellIt.IsItValid(); CellIt.Next()) {
		// ¿La entidad actual es interactuable?
		CWorldEntity* const pEntity = m_IsoMap.pArea->GetWorldEntity(CellIt.GetWorldEntity());
		ASSERT(pEntity);		
		if (pEntity->IsInteractuable()) {		  
		  // Se comprueba si la pos. de seleccion cae en su area delimitada
		  sRect EntityRect;
		  MakeEntityRect(pEntity, pFloor, EntityRect);
		  if (m_pMathUtil->IsPointInRect(sPosition(uwXScreen, uwYScreen), 
										 EntityRect)) {
			// Se realizara la ultima comprobacion a nivel de pixel
			// Se obtiene la textura que representa a la entidad y se comprueba
			// si la posicion es transparente. Si no lo es tenemos a la entidad
			DXDDSurfaceTexture* const pTexture = m_pResManager->GetTexture(pEntity->GetIDTexture());
			ASSERT(pTexture);			
			// Se obtiene posicion normalizada dentro de la textura
			const sword swXPixel = uwXScreen - EntityRect.swLeft;
			const sword swYPixel = uwYScreen - EntityRect.swTop;
			ASSERT((swXPixel >= 0) != 0);
			ASSERT((swXPixel < pTexture->GetWidth()) != 0);			
			ASSERT((swYPixel >= 0) != 0);
			ASSERT((swYPixel < pTexture->GetHeight()) != 0);
			
			// Se obtiene el pixel
			pTexture->Lock();	  			
			const dword udPixel = pTexture->GetPixel(swXPixel, swYPixel);
			pTexture->UnLock();
					
			// ¿NO es color invisible?
			if (udPixel != pTexture->RGBAToColor(255, 0, 255, 1)) { 			 
			  return pEntity; 
			}	
		  }
		}
	  } // ~ for (; CellIt.IsItValid(); CellIt.Next())
	} // ~ for (; ubIt < 3; ++ubIt)
  } // ~ for (; YTileIt >= YTile; YTileIt-=2)

  // No se pudo encontrar entidad
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de la coordinacion en la actualizacion del sistema de
//   transparencia. Para ello, recibira la posicion del tile donde supuestamente
//   va a localizarse el jugador (XTile, YTile). A partir de esa posicion y del
//   conjunto de entidades por el que se desea realizar transparencia, en
//   uwEntities, se ira recorriendo hacia abajo segun la directriz:
//   * Tile central, tile adyacente a la derecha, tile adyacente a la izquierda.
// - La profundidad a la que se bajara siempre sera YTile + NumYTilesPantalla.
// Parametros:
// - TilePos. Posicion.
// - EntitiesGroupToCheck. Grupo de entidades a comprobar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::UpdateTransparentSys(const AreaDefs::sTilePos& TilePos,
								 const GroupMask& EntitiesGroupToCheck)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Copia el set principal en un set temporal
  TranspEntitiesSet TempSet(m_TranspSys.TranspEntities);

  // Se obtiene el array de deltas apropiado
  sDelta* const pDeltas = m_IsoMap.pArea->GetDeltasValues(TilePos);

  // Se ajusta la posicion maxima 
  word uwYMaxTile = TilePos.YTile + 2 * m_IsoMap.ubNumTilesAtVert;
  if (uwYMaxTile >= m_IsoMap.pArea->GetHeight()) {
	uwYMaxTile -= 2;
  }

  // Se obtiene el rectangulo delimitador del jugador
  sRect PlayerRect;
  MakeEntityRect(m_IsoMap.pArea->GetPlayer(),
				 m_IsoMap.pArea->GetFloor(TilePos),
				 PlayerRect);

  // Se establecen las direcciones a examinar
  const IsoDefs::eDirectionIndex AdyacentDir[] = { 
	IsoDefs::NO_DIRECTION_INDEX, IsoDefs::SOUTHWEST_INDEX, IsoDefs::SOUTHEAST_INDEX
  };
  
  // Procede a navegar por entre todos los tiles a examinar
  // Notese que el incremento ha de ser de dos en dos unidades debido
  // a la naturaleza isometrica del mapa
  AreaDefs::TilePosType YTileIt = TilePos.YTile;
  for (; YTileIt <= uwYMaxTile; YTileIt+=2) {
	// Se navegara por: Tile centra, adyacente al oeste, adyacente al este
	byte ubIt = 0;
	for (; ubIt < 3; ++ubIt) {	  
	  // Se obtiene la posicion asociada a la direccion.
	  // Se convendra que la posicion 0 sera la central y por lo tanto no
	  // habra que añadir ningun tipo de delta
  	  const sword swXTile = TilePos.XTile + (ubIt > 0 ? pDeltas[AdyacentDir[ubIt]].sbXDelta : 0);
	  if (swXTile < 0) { continue; }
	  const sword swYTile = YTileIt + (ubIt > 0 ? pDeltas[AdyacentDir[ubIt]].sbYDelta : 0); 	  
	  if (swYTile < 0) { continue; }
	  const AreaDefs::sTilePos NextTilePos(swXTile, swYTile);
	  // ¿NO es posicion valida o SIN contenido?
	  if (!m_IsoMap.pArea->IsCellValid(NextTilePos) ||
		  !m_IsoMap.pArea->IsCellWithContent(NextTilePos)) { 
		continue; 
	  }

	  // Se hacen transparentes los tiles que procedan
	  MakeTranspEntitiesOver(NextTilePos, PlayerRect, TempSet, EntitiesGroupToCheck);		  
	}	
  } 

  // De las entidades en el set temporal se haran solidas y se borraran
  // del Set principal y temporal
  TranspEntitiesSetIt SetIt = TempSet.begin();
  while(SetIt != TempSet.end()) {
	SetEntityTransparent(m_IsoMap.pArea->GetWorldEntity(*SetIt), false);
	m_TranspSys.TranspEntities.erase(*SetIt);
	SetIt = TempSet.erase(SetIt);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dada la celda situada en XTile e YTile, este metodo recorrera todas las
//   entidades que se hallan sobre ella. Para cada entidad que localice, si
//   es de algun tipo definido en EntitiesToCheck, construira su rectangulo
//   y comprobara si colisiona con PlayerRect, que sera el que represente
//   la posicion universal del jugador.
// - Cuando localice una entidad, mirara antes en TempSet, de tal forma que
//   si dicha entidad se halla ahi no la ingresara en el set principal en
//   m_TranspSys pero si la borrara de TempSet. En caso de no hallarala
//   en TempSet, la ingresara automaticamente en el set de m_TranspSys
//   habiendola hecho antes transparente.
// Parametros:
// - XTile, YTile. Tile en X e Y donde localizar a las entidades.
// - PlayerRect. El rectangulo que delimita la posicion del jugador.
// - TempSet. Set temporal.
// - EntitiesGroupToCheck. Identificadores de las entidades a comprobar. Se 
//   contemplaran solo para paredes y objetos de escenario.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::MakeTranspEntitiesOver(const AreaDefs::sTilePos& TilePos,
								   const sRect& PlayerRect,
								   TranspEntitiesSet& TempSet,
								   const GroupMask& EntitiesGroupToCheck)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Obtiene floor
  CFloor* const pFloor = m_IsoMap.pArea->GetFloor(TilePos);

  // Procede a navegar entre las entidades
  CCellIterator CellIt;
  CellIt.Init(m_IsoMap.pArea, TilePos);
  ASSERT(CellIt.IsInitOk());
  for (; CellIt.IsItValid(); CellIt.Next()) {
	// ¿La entidad actual es una entidad a chequear?
	if (IsEntityInGroup(CellIt.GetWorldEntity(), EntitiesGroupToCheck)) {
	  // Se comprueba si hay colision		
	  CWorldEntity* const pEntity = m_IsoMap.pArea->GetWorldEntity(CellIt.GetWorldEntity());
	  ASSERT(pEntity);
	  sRect EntityRect;
	  MakeEntityRect(pEntity, pFloor, EntityRect);
	  if (m_pMathUtil->RectsCollision(PlayerRect, 
									  EntityRect)) {
		// Hay colision
		// ¿La entidad se halla en el TempSet?
		TranspEntitiesSetIt SetIt = TempSet.find(CellIt.GetWorldEntity());
		if (SetIt != TempSet.end()) {
		  // Si, simplemente se elimina de este y se continua iteracion
		  TempSet.erase(SetIt);
		  continue;
		} else {
		  // Se inserta en el set principal y se hace transparente a la entidad
		  m_TranspSys.TranspEntities.insert(CellIt.GetWorldEntity());
		  SetEntityTransparent(pEntity, true);		  
		}
	  } 	  	  
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dada una altura y posicion en x e y en donde se halla una entidad, asi
//   como la elevacion que tiene esta y el terreno en donde se halla
// Parametros:
// - pEntity. Entidad de la que se quiere obtener el rect.
// - pFloor. Floor sobre el que esta la entidad.
// - EntityRect. Rect a establecer.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CIsoEngine::MakeEntityRect(CWorldEntity* const pEntity,
						   CFloor* const pFloor,
						   sRect& EntityRect) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pEntity);
  ASSERT(pFloor);

  // Ancho
  EntityRect.swLeft = -m_ScrollInfo.ubXOffset + 
					  (pEntity->GetTilePos().XTile - m_IsoMap.TilePos.XTile) *
					  IsoDefs::TILE_WIDTH;
  if (m_pMathUtil->IsEven(pEntity->GetTilePos().YTile)) { 
	EntityRect.swLeft += m_IsoMap.sbXScrEvenInit;
  }			
  EntityRect.swRight = EntityRect.swLeft + IsoDefs::TILE_WIDTH;
  // Alto
  EntityRect.swBottom = (m_IsoMap.sbYScrInit - m_ScrollInfo.ubYOffset) +
					    (pEntity->GetTilePos().YTile - m_IsoMap.TilePos.YTile) *
					    IsoDefs::TILE_HEIGHT_DIV + 
						IsoDefs::TILE_HEIGHT - 
						pFloor->GetElevation() - 
						pEntity->GetElevation(); 						
  EntityRect.swTop = EntityRect.swBottom - pEntity->GetHeight();					 
}						   
						   
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dada la entidad pEntity, si bTransp es true, asociara a sus cuatro 
//   vertices el valor Alpha almacenado en m_TranspSys. En caso contrario,
//   desasociara cualquier valor de transparencia que hubiera en este.
// - Este metodo se utilizara para hacer transparente / solido a una entidad.
// Parametros:
// - pEntity. Direccion de la entidad.
// - bTransp. Flag para saber si hacer transparente o solida a la entidad.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::SetEntityTransparent(CWorldEntity* const pEntity, 
								 const bool bTransp)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pEntity);

  // Se establece la direccion de la estructura alpha segun convenga
  GraphDefs::sAlphaValue* const pAlpha = bTransp ? &m_TranspSys.Alpha : NULL;

  // Se asocia dicho valor alpha a los vertices
  pEntity->SetAlphaValue(GraphDefs::VERTEX_0, pAlpha);
  pEntity->SetAlphaValue(GraphDefs::VERTEX_1, pAlpha);
  pEntity->SetAlphaValue(GraphDefs::VERTEX_2, pAlpha);
  pEntity->SetAlphaValue(GraphDefs::VERTEX_3, pAlpha);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Decide si el handle hEntity pertenece a una entidad que se halla en el
//   grupo de entidades Group.
// Parametros:
// - hEntity. Handle a la entidad.
// - Group. Mascara del grupo
// Devuelve:
// - Si se halla true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CIsoEngine::IsEntityInGroup(const AreaDefs::EntHandle& hEntity,
  							const GroupMask& Group)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hEntity);

  // Comprueba el tipo de la entidad
  switch(m_IsoMap.pArea->GetEntityType(hEntity)) {
	case RulesDefs::SCENE_OBJ: {
	  return Group & CIsoEngine::SCENE_OBJ_GROUP ? true : false;
	} break;

	case RulesDefs::CRIATURE: {
	  return Group & CIsoEngine::CRIATURE_GROUP ? true : false;
	} break;

	case RulesDefs::WALL: {
	  return Group & CIsoEngine::WALL_GROUP ? true : false;
	} break;
  };

  // No se halla
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dada una posicion origen TileSrc y una posicion destino TileDest, el 
//   metodo calcula una posicion adjacente a TileDest tal que entre TileSrc y
//   TileDest la distancia sea minima y ademas posible. 
// Parametros:
// - TilSrc. Posicion inicial desde donde calcular.
// - TileDest. Posicion final a partir de la cual calcular.
// - AdjacentTilePos. Referencia donde depositar el tile adjacente calculado.
// Devuelve:
// - Si el calculo ha dado una posicion de tile adjacente tal que existe un
//   camino entre TileSrc y TileDest valido, devolvera la longitud del mismo
//   que podra ser mayor o igual a 0. En caso contrario, si no se ha hallado un
//   resultado, se devolvera un valor negativo
// Notas:
///////////////////////////////////////////////////////////////////////////////
sword
CIsoEngine::CalculeAdjacentPosInDestination(const AreaDefs::sTilePos& TileSrc, 				  
										    const AreaDefs::sTilePos& TileDest,
										    AreaDefs::sTilePos& AdjacentTilePos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(m_IsoMap.pArea->IsCellValid(TileSrc));
  ASSERT(m_IsoMap.pArea->IsCellValid(TileDest));

  // Procede a calcular las posiciones adjacentes para un tile
  sword swBestPathLenght = -1;
  byte ubDirIt = IsoDefs::NORTH_INDEX;  
  for (; ubDirIt != IsoDefs::NO_DIRECTION_INDEX; ++ubDirIt) {
	// Se halla la posicion adjacente al TileDest en la dir. que toque
	AreaDefs::sTilePos AdjacentTmpTilePos;  
	if (m_IsoMap.pArea->GetAdjacentTilePos(TileDest, 
										   IsoDefs::eDirectionIndex(ubDirIt), 
										   AdjacentTmpTilePos)) {	  
	  // Direccion adjacente valida
	  // Se calcula la longitud del camino y se guarda si es mejor que el actual
	  const sword swPathLenght = m_PathFinder.CalculePathLenght(TileSrc, AdjacentTmpTilePos);
	  if (swPathLenght != -1  && 
		  (-1 == swBestPathLenght || swBestPathLenght > swPathLenght)) {
		swBestPathLenght = swPathLenght;	  
		AdjacentTilePos = AdjacentTmpTilePos;
	  } 
	}
  }	

  // Se retorna el resultado  	
  return swBestPathLenght;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el tile de posicion TileSrc es adjacente a TileDest.
// Parametros:
// - TileSrc, TileDest. Tile de posicion origen y destino.
// Devuelve:
// - Si es adjacente true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CIsoEngine::IsAdjacentTo(const AreaDefs::sTilePos& TileSrc,
						 const AreaDefs::sTilePos& TileDest)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se hallan los tiles adjacentes a TileDest comprobandose si alguno
  // es igual a TileSrc
  AreaDefs::sTilePos AdjacentTilePos;
  byte ubDirIt = IsoDefs::NORTH_INDEX;
  for (; ubDirIt != IsoDefs::NO_DIRECTION_INDEX; ++ubDirIt) {
	// Se halla la posicion adjacente al TileDest en la dir. que toque
	if (m_IsoMap.pArea->GetAdjacentTilePos(TileDest, 
										   IsoDefs::eDirectionIndex(ubDirIt), 
										   AdjacentTilePos)) {
	  // Dir. adjacente valida, ¿es igual a TileSrc?
	  if (TileSrc == AdjacentTilePos) {
		// Si, se retorna
		return true;
	  }
	}
  }

  // No, no se cumple que la posicion sea adjacente
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia.
// Parametros:
// Devuelve:
// - Si todo ha ido correctamente true. En caso contrario false.
// Notas:
// - NO se permitira reinicializar
// - La tabla m_MaskFlagToCheck sirve para saber que direccion en el tile
//   adyacente en el que nos encontrabamos, demos de chequear para saber si
//   podemos pasar. Siempre se cumplira que la direccion a chequear sera la
//   simetrica de la que hemos usado para entrar en dicho tile.
///////////////////////////////////////////////////////////////////////////////
bool 
CIsoEngine::CPathFinder::Init(void) 
{
  // ¿Se pretende reinicializar?
  if (IsInitOk()) {
	return false;
  }

  // Se inicializa el recycle node pool
  if (!m_RecycleNodePool.Init()) { 
	return false; 
  }

  // Se establecen la informacion por direccion a visitar
  byte ubIt = 0;
  for (; ubIt < IsoDefs::MAX_DIRECTIONS; ++ubIt) {
	// Se establece direccion
	m_DirsToVisit[ubIt] = IsoDefs::eDirectionIndex(ubIt);		
  }

  // Se establecen los flags asociados a las direcciones de visita
  // pero con respecto a la mascara de acceso en el tile a visitar
  // Se cumplira que siempre seran las simetricas
  m_MaskFlagToCheck[IsoDefs::NORTH_INDEX] = IsoDefs::SOUTH_FLAG;
  m_MaskFlagToCheck[IsoDefs::NORTHEAST_INDEX] = IsoDefs::SOUTHWEST_FLAG;
  m_MaskFlagToCheck[IsoDefs::EAST_INDEX] = IsoDefs::WEST_FLAG;
  m_MaskFlagToCheck[IsoDefs::SOUTHEAST_INDEX] = IsoDefs::NORTHWEST_FLAG;
  m_MaskFlagToCheck[IsoDefs::SOUTH_INDEX] = IsoDefs::NORTH_FLAG;
  m_MaskFlagToCheck[IsoDefs::SOUTHWEST_INDEX] = IsoDefs::NORTHEAST_FLAG;
  m_MaskFlagToCheck[IsoDefs::WEST_INDEX] = IsoDefs::EAST_FLAG;
  m_MaskFlagToCheck[IsoDefs::NORTHWEST_INDEX] = IsoDefs::SOUTHEAST_FLAG;

  // Se establece area nula
  m_pActArea = NULL;

  // Todo correcto
  m_bIsInitOk = true;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::CPathFinder::End(void)
{
  // Finaliza si procede
  if (IsInitOk()) {
	m_RecycleNodePool.End();
	m_pActArea = NULL;
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Apoyandose en FindPath ordena la creacion de un camino de busqueda y 
//   retorna unicamente la longitud del mismo si es que existe. En caso de no
//   hallar camino de busqueda entre dos nodos retornara un valor negativo.
// Parametros:
// - TileSrc. Posicion del tile origen.
// - TileDest. Posicion del tile destino.
// Devuelve:
// - La longitud del camino de busqueda si existe. Si no existiera, un valor
//   de -1. Esto tambien se aplicara si los tiles no son validos en cuanto a
//   que no pertenezcan al mapa.
// Notas:
///////////////////////////////////////////////////////////////////////////////
sword 
CIsoEngine::CPathFinder::CalculePathLenght(const AreaDefs::sTilePos& TileSrc,
										   const AreaDefs::sTilePos& TileDest)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  ASSERT(IsAreaAttached());

  // ¿Posiciones validas?

  // SOLO si parametros correctos
  ASSERT(m_pActArea->IsCellValid(TileSrc));
  ASSERT(m_pActArea->IsCellValid(TileDest));

  // ¿Los tiles pertenecen a posiciones validas?
  ASSERT(IsAreaAttached());
  if (m_pActArea->IsCellValid(TileSrc) &&
	  m_pActArea->IsCellValid(TileDest)) {
	// Obtiene camino
	const CPath* pPath = FindPath(TileSrc, TileDest);

	// ¿Camino valido?
	if (pPath) {
	  // Se retorna la longitud y se destruye
	  const word uwSize = pPath->GetSize();	  
	  delete pPath;
	  return uwSize;
	}  
  }

  // El camino no es valido / los tiles no son validos
  return -1;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dada una posicion inicio y una posicion final, localiza, si es posible,
//   el camino de busqueda. El camino de busqueda se devolvera en una clase
//   CPath. En caso de que no hubiera camino posible, se devolveria NULL.
// - El metodo tambien sera capaz de hallar un camino de minima distancia.
//   Un camino de minima distancia sera aquel que ira desde TileSrc a un
//   tile que podra ser o no igual a TileDest pero que tendra que cumplir que
//   la distancia desde el hasta TileDest sea menor o igual que la minima
//   distancia. Para indicar que se desea hallar un camino de minima distancia,
//   el parametro uwDistanceMin debera de ser mayor que 0. Por defecto valdra
//   cero.
// Parametros:
// - TileSrc. Posicion del tile origen.
// - TileDest. Posicion del tile destino.
// - uwDistanceMin. Distancia minima, solo usable en caso de querer hallar un
//   camino de minima distancia. Por defecto 0.
// - bGhostMode. Flag para indicar si se quiere hallar el camino de acceso
//   SIN tener en cuenta obstaculos.
// Devuelve:
// - Direccion a la clase CPath
// Notas:
// - Este metodo actua como una "Factory" de clases CPath, siendo 
//   responsabilidad del exterior eliminarla.
///////////////////////////////////////////////////////////////////////////////
CPath* const 
CIsoEngine::CPathFinder::FindPath(const AreaDefs::sTilePos& TileSrc,
								  const AreaDefs::sTilePos& TileDest,
								  const word uwDistanceMin,
								  const bool bGhostMode)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  ASSERT(IsAreaAttached());
  // SOLO si parametros correctos
  ASSERT(m_pActArea->IsCellValid(TileSrc));
  ASSERT(m_pActArea->IsCellValid(TileDest));

  // Se resetea el RecycleNodePool (de una posible llamada anterior)
  m_RecycleNodePool.ResetPool();

  // Se inicializa el nodo de comienzo y se inserta en la Open
  sNodeSearch* pNode = m_RecycleNodePool.GetNewNode();
  ASSERT(pNode);
  pNode->pParentState = NULL;
  pNode->TilePos = TileSrc;
  pNode->fCostToThis = 0.0f;
  pNode->fCostToDest = GetHeuristicValue(TileSrc, TileDest);
  OpenPush(pNode);
  
  // Se procede a localizar el camino hasta que se halle solucion o no
  while (m_Open.size()) {
	// Se obtiene nodo de la Open (seguro que tiene el menor coste global)
	OpenPop(pNode);
	ASSERT(pNode);

	// ¿Se ha hallado el objetivo?
	bool bGoalOk = false; 
	if (uwDistanceMin > 0) {
	  // Si se deseaba hallar un camino de minima distancia
	  // ¿Se ha alcanzado la distancia minima?
	  if (CalculeAbsoluteDistance(pNode->TilePos, TileDest) <= uwDistanceMin) {
		bGoalOk = true;
	  }
	} else if (TileDest == pNode->TilePos) {
	  // Si se deseaba hallar un camino normal
	  bGoalOk = true;
	}

	// ¿Hemos alcanzado el nodo destino?
	if (bGoalOk) {
	  // Se alcanzo el destino y se construye hacia atras el camino
	  CPath* pPath = new CPath;
	  ASSERT(pPath);
	  pPath->Init(TileSrc);
	  ASSERT(pPath->IsInitOk());
	  _CreatePath(pNode, pPath);
	  
	  // Se liberan listas
	  CleanMainNodeIndex();
	  CleanOpen();	  

	  // Se retorna camino generado
	  return pPath;
	}

	// Vbles
	AreaDefs::sTilePos       NewTilePos;     // Pos. del nuevo tile a visitar
	float                    fNewCostToThis; // Coste del nuevo tile a visitar
	IsoDefs::eDirectionIndex DirToSrc;       // Direccion a nodo origen actual
	sNodeSearch*             pNewNode;       // Puntero a nuevo nodo

	// Se obtiene la direccion hasta el tile origen actual
	if (pNode->pParentState) {
	  DirToSrc =  m_pActArea->CalculeDirection(pNode->pParentState->TilePos, 
										       pNode->TilePos);
	} else {
	  DirToSrc = IsoDefs::NO_DIRECTION_INDEX;
	}

	// Para cada posible direccion se comprueba si es visitable	
	byte ubIt = 0;
	for (; ubIt < IsoDefs::MAX_DIRECTIONS; ++ubIt) {
	  // ¿Existe tile adyacente en la direccion que procede?
	  if (m_pActArea->GetAdjacentTilePos(pNode->TilePos, 
										 m_DirsToVisit[ubIt], 
										 NewTilePos)) {
		// ¿Se puede acceder a dicho tile adyacente?
		// Nota: Se podra acceder si la mascara de acceso lo permite y ademas
		// si los tiles adyacentes a la posicion destino permiten el franqueo en
		// al menos un paso (esto ultimo solo en determinadas direcciones).
		// EN CASO de estar activo el modo fantasma, flag bGhostMode, no se
		// tendra en cuenta los posibles obstaculos
		if (bGhostMode ||
		   (!(m_pActArea->GetMaskTileAccess(NewTilePos) & m_MaskFlagToCheck[ubIt]) &&
			AccessValidInAdjacentsOfTileDest(pNode->TilePos, IsoDefs::eDirectionIndex(ubIt)))) {
		  // Se calcula el coste a dicho tile
		  // El coste premiara aquellas direcciones consecutivas iguales,
		  // penalizando las que sean en diagonal
		  if (DirToSrc != m_DirsToVisit[ubIt]) {
			fNewCostToThis = pNode->fCostToThis + 2.0f;
		  } else {
			fNewCostToThis = pNode->fCostToThis + 1.0f;
		  }
		  
		  // ¿El nodo a dicha posicion ya ese encuentra registrado?
		  const AreaDefs::TileIndex NewTilePosIdx = m_pActArea->GetTileIdx(NewTilePos);
		  MainNodeIndexIt It(m_MainNodeIndex.find(NewTilePosIdx));
		  if (It != m_MainNodeIndex.end()) {
			// Se toma y se comprueba si es PEOR esta nueva version del nodo
			pNewNode = It->second;			
			if (pNewNode->fCostToThis <= fNewCostToThis) {
			  continue;
			}
		  } else {
			// Se toma un nuevo nodo y se registra en el MainNodeIndex
			pNewNode = m_RecycleNodePool.GetNewNode();
			ASSERT(pNewNode);
			m_MainNodeIndex.insert(MainNodeIndexValType(NewTilePosIdx, pNewNode));
			pNewNode->NodeAlloc = sNodeSearch::NO_ALLOC;
		  }

		  // Se almacena la nueva informacion o la informacion mejorada
		  pNewNode->pParentState = pNode;
		  pNewNode->TilePos = NewTilePos;
		  pNewNode->fCostToThis = fNewCostToThis;
		  pNewNode->fCostToDest = fNewCostToThis + 
								  GetHeuristicValue(NewTilePos, TileDest);

		  // Se asienta el nuevo nodo
		  if (sNodeSearch::OPEN == pNewNode->NodeAlloc) {
			// Se actualiza posicion segun la nueva prioridad
			OpenUpdateNodePriority(pNewNode);
		  } else {
			// Si no estaba asentado o si estaba en CLOSED se inserta en Open
			OpenPush(pNewNode);
		  }
		}
	  }	  
	} // ~ for

	// Una vez examinado el nodo sacado de la Open, se inserta en la Closed
	pNode->NodeAlloc = sNodeSearch::CLOSED;
  } // ~ while

  // No se logro encontrar el camino
  // Se liberan listas y se retorna
  CleanMainNodeIndex();
  CleanOpen();	  
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba, para una determinada direccion, si es posible el acceso a los
//   tiles adyacentes a la misma. Teniendo en cuenta que los tiles adyacentes
//   se referiran a la direccion anterior y posterior de la recibida. 
//   Este calculo sera necesario para evitar situaciones en las que, por ejemplo,
//   dos paredes esten en posicion suroeste y sureste, mostrandose visualmente
//   adyacentes, pero que logicamente puedan franquerase via sur.
// - Dependiendo de la direccion del tile destino, se debera de comprobar una
//   set de direcciones distintas.
// - SOLO se comprobara para las direcciones NORTE / ESTE / SUR / OESTE ya que
//   en el resto de los casos, no sera aplicacable.
// - No se considerara la existencia de criaturas.
// Parametros:
// - TileSrc. Posicion del tile desde donde parte el reconocimiento.
// - TileDestDir. Direccion del tile destino.
// Devuelve:
// - Si el acceso es valido true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CIsoEngine::CPathFinder::AccessValidInAdjacentsOfTileDest(const AreaDefs::sTilePos& TileSrc,
											              const IsoDefs::eDirectionIndex& TileDestDir)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  ASSERT(IsAreaAttached());
  // SOLO si parametros correctos
  ASSERT(m_pActArea->IsCellValid(TileSrc));
  ASSERT((TileDestDir != IsoDefs::NO_DIRECTION_INDEX) != 0);

  // Se determinan las direcciones a chequear
  IsoDefs::eDirectionIndex AdjacentDirs[2];
  switch (TileDestDir) {
	case IsoDefs::NORTH_INDEX: {
	  AdjacentDirs[0] = IsoDefs::NORTHWEST_INDEX;
	  AdjacentDirs[1] = IsoDefs::NORTHEAST_INDEX;	  
	} break;

	case IsoDefs::NORTHEAST_INDEX: {
	  //AdjacentDirs[0] = IsoDefs::NORTH_INDEX;
	  //AdjacentDirs[1] = IsoDefs::EAST_INDEX;	  
	  return true;
	} break;

	case IsoDefs::EAST_INDEX: {
	  AdjacentDirs[0] = IsoDefs::NORTHEAST_INDEX;
	  AdjacentDirs[1] = IsoDefs::SOUTHEAST_INDEX;	  
	} break;

	case IsoDefs::SOUTHEAST_INDEX: {
	  //AdjacentDirs[0] = IsoDefs::EAST_INDEX;
	  //AdjacentDirs[1] = IsoDefs::SOUTH_INDEX;	  
	  return true;
	} break;

	case IsoDefs::SOUTH_INDEX: {
	  AdjacentDirs[0] = IsoDefs::SOUTHEAST_INDEX;
	  AdjacentDirs[1] = IsoDefs::SOUTHWEST_INDEX;	  
	} break;

	case IsoDefs::SOUTHWEST_INDEX: {
	  //AdjacentDirs[0] = IsoDefs::SOUTH_INDEX;
	  //AdjacentDirs[1] = IsoDefs::WEST_INDEX;	  
	  return true;
	} break;

	case IsoDefs::WEST_INDEX: {
	  AdjacentDirs[0] = IsoDefs::SOUTHWEST_INDEX;
	  AdjacentDirs[1] = IsoDefs::NORTHWEST_INDEX;	  
	} break;

	case IsoDefs::NORTHWEST_INDEX: {
	  //AdjacentDirs[0] = IsoDefs::NORTH_INDEX;
	  //AdjacentDirs[1] = IsoDefs::WEST_INDEX;	  
	  return true;
	} break;
  }; // ~ switch

  // Se comprueba para cada una de las direcciones halladas si pueden
  // ser accedidas desde la posicion origen. En el primer caso en el que 
  // esto sea posible, se retornara.
  // Nota: Se considerara que no podra accederse cuando no exista tile adyacente.
  // Nota: No incluiremos en el chequeo la comprobacion de criaturas	  	
  const dword udNoEntToCheck = RulesDefs::PLAYER | RulesDefs::CRIATURE;
  byte ubIt = 0;
  for (; ubIt < 2; ++ubIt) {
	// ¿Existe tile adyacente en la direccion que procede?
	AreaDefs::sTilePos NewTilePos;
	ASSERT((AdjacentDirs[ubIt] != IsoDefs::NO_DIRECTION_INDEX) != 0);
	if (m_pActArea->GetAdjacentTilePos(TileSrc, AdjacentDirs[ubIt], NewTilePos)) {
	  // ¿La mascara de acceso permite acceder al tile anterior?
	  if (!(m_pActArea->GetMaskTileAccess(NewTilePos, udNoEntToCheck) & m_MaskFlagToCheck[AdjacentDirs[ubIt]])) {
		// Si, retorna
		return true;
	  }
	}
  }

  // No hay posibilidad de acceso
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Construye el camino CPath a partir de la sucesion de nodos que lo forman
//   y que sabemos que son validos. El nodo recibido, pNodeSearch, sera el 
//   ultimo nodo de tal forma que navegando por los nodos padres se llegara al
//   nodo inicio y a partir de ahi se debera de construir el camino.
// - El metodo sera recursivo y no se querra incluir en el camino de busqueda
//   el nodo padre.
// Parametros:
// - pNodeSearch. La llamada del exterior pasara siempre el ultimo nodo de
//   busqueda.
// - pPath. Instancia CPath donde se iran asociando los nodos que formen el
//   camino
// Devuelve:
// Notas:
// - Nunca se debera de llamar a este metodo con el nodo inicial de busqueda.
//   El camino debera de estar formando por al menos dos nodos, el incial y
//   y el nodo destino.
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::CPathFinder::_CreatePath(sNodeSearch* const pNodeSearch,
									 CPath*& pPath)
{
  // SOLO si parametros validos
  ASSERT(pNodeSearch);
  ASSERT(pPath);
  ASSERT(pPath->IsInitOk());

  // ¿NO estamos en el primer nodo del camino, excluido el nodo inicial?
  if (pNodeSearch->pParentState->pParentState) {	
	_CreatePath(pNodeSearch->pParentState, pPath);
  }

  // Procedemos a insertar nodo en el camino
  const IsoDefs::eDirectionIndex DirToWalk = m_pActArea->CalculeDirection(pNodeSearch->pParentState->TilePos,
																		  pNodeSearch->TilePos);
  pPath->AddPosition(pNodeSearch->TilePos, DirToWalk);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Procede a liberar la MainNodeIndex.
// Parametros:
// Devuelve:
// Notas:
// - Los nodos no seran eliminados de memoria, al utilizar un CRecycleNodePool
///////////////////////////////////////////////////////////////////////////////
void
CIsoEngine::CPathFinder::CleanMainNodeIndex(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se procede a eliminar elementos
  MainNodeIndexIt It = m_MainNodeIndex.begin();
  while (It != m_MainNodeIndex.end()) {
	It = m_MainNodeIndex.erase(It);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Extrae un nodo de la Open
// Parametros:
// - pNodeSearch, nodo a devolver
// Devuelve:
// - Un nodo de la open pero por parametro (debido a problemas de ambito para
//   devolverlo por funcion)
// Notas:
// - El metodo pop_heap lo que hace es pone el nodo que estaba al comienzo al
//   final, posicion N, y despues ordenar los nodos entre la posicion 1 y N-1
//   para que cumplan la propiedad de monticulo, usando el predicado CHeapComp.
//   Una vez hecho esto, el nodo en la posicion N sabemos que no nos interesa
//   y lo quitamos.
///////////////////////////////////////////////////////////////////////////////
void
CIsoEngine::CPathFinder::OpenPop(sNodeSearch*& pNodeSearch)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
 
  // Extrae elemento
  ASSERT(m_Open.size());
  pNodeSearch = m_Open.front();
  ASSERT(pNodeSearch);
 
  // Reconstruye monticulo
  CHeapComp HeapComp;
  std::pop_heap(m_Open.begin(), m_Open.end(), HeapComp);
  m_Open.pop_back();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inserta un nodo en la Open.
// Parametros:
// - pNode. Nodo a insertar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::CPathFinder::OpenPush(sNodeSearch* const pNode)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pNode);
  
  // Se inserta el nodo al final y se reordena el monticulo
  pNode->NodeAlloc = sNodeSearch::OPEN;
  m_Open.push_back(pNode);
  CHeapComp HeapComp;
  std::push_heap(m_Open.begin(), m_Open.end(), HeapComp);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Despues de haber actualizado la prioridad de un nodo, se debera de
//   llamar a este metodo para que se localice la posicion del nodo en el
//   Heap y se reordene segun su cambio de prioridad
// Parametros:
// - pNodeUpdated. Nodo actualizo
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::CPathFinder::OpenUpdateNodePriority(sNodeSearch* const pNodeUpdated)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pNodeUpdated);
  ASSERT((pNodeUpdated->NodeAlloc == sNodeSearch::OPEN) != 0);

  // Se procede a localizar el nodo en el heap
  HeapIt It = std::find(m_Open.begin(), m_Open.end(), pNodeUpdated);
  ASSERT((It != m_Open.end()) != 0);
  
  // Se reordean elementos en el heap
  CHeapComp HeapComp;
  std::push_heap(m_Open.begin(), ++It, HeapComp);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera la lista en Open.
// Parametros:
// Devuelve:
// Notas:
// - Los nodos no seran eliminados de memoria, al utilizar un CRecycleNodePool
///////////////////////////////////////////////////////////////////////////////
void
CIsoEngine::CPathFinder::CleanOpen(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Se procede a eliminar nodos
  HeapIt It = m_Open.begin();
  while (It != m_Open.end()) {	
	It = m_Open.erase(It);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Calcula la distancia entre las dos posiciones de tile usando pitagoras
// Parametros:
// - TileSrc, TileDest. Posiciones inicial y destino
// Devuelve:
// - Valor de la distancia entre ambos tiles, teniendo en cuenta que se usara
//   como referencia unitaria la posicion de los mismos en un sistema de
//   coordenadas. En caso de que alguna posicion no sea valida, se 
//   retornara -1.
// Notas:
///////////////////////////////////////////////////////////////////////////////
sword 
CIsoEngine::CPathFinder::CalculeAbsoluteDistance(const AreaDefs::sTilePos& TileSrc,
												 const AreaDefs::sTilePos& TileDest)
{
  // SOLO si instancia inicializada  
  ASSERT(IsInitOk());

  // ¿Los tiles pertenecen a posiciones validas?
  ASSERT(IsAreaAttached());
  if (m_pActArea->IsCellValid(TileSrc) &&
	  m_pActArea->IsCellValid(TileDest)) {	
	// Calcula la distancia entre coordenadas
	const sword swXValue = TileSrc.XTile - TileDest.XTile;
	const sword swYValue = TileSrc.YTile - TileDest.YTile;

	// Aplica pitagoras
	dword udResult = sqrt((swXValue * swXValue) + (swYValue * swYValue));

	// Para corregir las variaciones isometricas, si el resultado es 1 y no
	// son tiles adyacentes, se incrementara el resultado
	if (udResult < 2) {
	  if (!SYSEngine::GetWorld()->IsAdjacentTo(TileSrc, TileDest)) {
		udResult++;
	  }
	}

	// Retorna
	return udResult;
  }

  // Posiciones no validas
  return -1;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe la notificacion de una criatura. El motor isometrico sera observer
//   siempre de aquella criatura que posea la camara.
// Parametros:
// - hCriature. Handle a la criatura.
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
								   const CriatureObserverDefs::eObserverNotifyType& NotifyType,
								   const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hCriature);

  // ¿Se mueve entidad con camara?
  if (hCriature == m_CameraInfo.hEntity) {
	// ¿La notificacion es inicio de cambio de localidad?
	if (NotifyType == CriatureObserverDefs::MOVING) {
	  // Se calcula valores maximos de scroll permitido
	  CalculeCameraScrollValues();

	  // Se procede a ajustar los valores de transparencia   
	  const AreaDefs::sTilePos TileDest(udParam >> 16, udParam & 0X0000FFFF);
	  UpdateTransparentSys(TileDest, CIsoEngine::WALL_GROUP);
	  
	  // Se determina la visiblidad del posible techo asociado
	  const AreaDefs::RoomID NewRoom = m_IsoMap.pArea->GetRoomAt(TileDest);
	  DetermineRoofVisibility(m_CameraInfo.Room, NewRoom);
	  m_CameraInfo.Room = NewRoom;
	}   
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la notificacion de un suceso en CWorld.
// - Seran de interes las notificaciones:
//    * Destruccion de una entidad.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CIsoEngine::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
							    const dword udParam)
{
  // SOLO si instancia inicializada y activada
  ASSERT(IsInitOk());
  ASSERT(IsAreaSet());

  // Comprueba el tipo de notificacion
  switch(NotifyType) {
	case WorldObserverDefs::ENTITY_DESTROY: {
	  // ¿Es la entidad destruida la asociada a la camara?
	  if (udParam == m_CameraInfo.hEntity) {
		// Si, se establecera el jugador con el poseedor de la camara
		AttachCamera(SYSEngine::GetWorld()->GetPlayer()->GetHandle());
	  }
	} break;
  }; // ~ switch
}
