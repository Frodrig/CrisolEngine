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
// CWorld.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CWorld
//
// Descripcion:
// - Subsistema que representara el universo de juego. El universo de juego
//   estara formado por el area visualizable y el motor isometrico, que sera
//   el encargado de trabajar con los datos del area visualizable.
// - CWorld actuara principalmente como una facade en donde se hallen todos los
//   metodos para la obtencion de informacion del universo de juego y para 
//   la actuacion sobre el mismo.
// - Todos los scripts relativos al universo (World) y al juego como aplicacion
//   (Game) seran guardados y consultados desde aqui. Al comenzar una partida
//   nueva, se leera desde un archivo .cbt los nombres de los scripts y cuando
//   se realice una grabacion, seran pasados al archivo de grabacion.
//
// Notas:
// - La clase sera definida como un singlenton.
// - Aunque desde esta clase se pueda controlar el permiso de guardar partidas,
//   se tendra tambien en cuenta y con mayor prioridad, que no
//   se este en modo combate y que no se este ejecutando una CutScene.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CWORLD_H_
#define _CWORLD_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CISOENGINE_H_
#include "CIsoEngine.h"
#endif
#ifndef _CPLAYER_H_
#include "CPlayer.h"
#endif
#ifndef _ICWORLD_H_
#include "iCWorld.h"
#endif
#ifndef _CAREA_H_
#include "CArea.h"
#endif
#ifndef _ICALARMCLIENT_H_
#include "iCAlarmClient.h"
#endif
#ifndef _ICSCRIPTCLIENT_H_
#include "iCScriptClient.h"
#endif
#ifndef _ICVIRTUALMACHINE_H_
#include "iCVirtualMachine.h"
#endif
#ifndef _WORLDOBSERVERDEFS_H_
#include "WorldObserverDefs.h"
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif

// Defincion de clases / estructuras / espacios de nombres
struct iCGraphicSystem;
struct iCAlarmManager;
struct iCGameDataBase;
struct iCWorldObserver;
struct iCResourceManager;
struct iCAudioSystem;
struct iCFileSystem;
class CGSInGame;

// Clase CWorld
class CWorld: public iCWorld,
			  public iCAlarmClient,
			  public iCScriptClient
{
private:
  // Clases amigas
  friend class CGSInGame;

private:
  // Tipos
  // Lista para el control de los observadores del universo de juego
  typedef std::list<iCWorldObserver*> WorldObserverList;
  typedef WorldObserverList::iterator WorldObserverListIt;
  // Map para el contro de los scripts asociados a eventos instalados
  typedef std::map<RulesDefs::eScriptEvents, std::string> ScriptEventsMap;
  typedef ScriptEventsMap::value_type                     ScriptEventsMapValType;
  typedef ScriptEventsMap::iterator                       ScriptEventsMapIt;  
  // Map para el control de los archivos WAV anonimos
  typedef std::map<AlarmDefs::AlarmHandle, ResDefs::WAVSoundHandle> WAVSoundMap;
  typedef WAVSoundMap::iterator										WAVSoundMapIt;
  typedef WAVSoundMap::value_type                                   WAVSoundMapValType;

private:
  // Estructuras
  struct sTimeInfo {
	// Info relacionada con el tiempo
	// Datos
	AlarmDefs::AlarmHandle hAlarm;           // Alarma del identificador de tiempo			
	byte                   ubHour;           // Hora
	byte                   ubMinute;         // Minuto
  };

  struct sModeInfo {
	// Controla info asociada al modo de juego
	WorldDefs::eWorldMode Mode; // Modo de funcionamiento
  };

  struct sAmbientWAVSound {
	// Info asociada al sonido WAV ambiente
	ResDefs::WAVSoundHandle hWAVAmbientSound; // Handle al sonido
	std::string             szWAVSoundFile;   // Nombre del sonido
  };

  struct sIdleEventInfo {
	// Info asociada para el control dal evento idle
	float                  fTime;      // Tiempo de evento Idle
	AlarmDefs::AlarmHandle hIdleAlarm; // Handle a la alarma
  };

  struct sObservers {
	// Info asociada a los observers
	WorldObserverList Observers;  // Lista de observadores  
	WorldObserverList ObToAdd;    // Observers pendientes de añadirse
	WorldObserverList ObToRemove; // Observers pendientes de eliminarse
	bool bInNotify; // ¿Se esta en proceso de notificacion?
  };

private:
  // Enumerados
  enum eStartMode {
	// Indica la forma en la que se ha inicializado el universo de juego
	SM_NEWGAME, // Se ha comenzado una nueva partida
	SM_LOADGAME // Se ha recuperado una partida previamente guardada
  };
  
private:
  // Instancia singlenton
  static CWorld* m_pWorld; // Unica instancia a la clase

  // Instancias a otros interfaces
  iCGraphicSystem*   m_pGraphSys;     // Instancia al subsistema grafico
  iCAlarmManager*    m_pAlarmManager; // Manejador de alarmas
  iCGameDataBase*    m_pGDBase;       // Base de datos del juego
  iCResourceManager* m_pResManager;   // Servidor de recursos
  iCAudioSystem*     m_pAudioSys;     // Subsistema de audio
  iCFileSystem*      m_pFileSys;      // Subsistema de ficheros
  iCVirtualMachine*  m_pVMachine;     // Maquina virtual para scripts
  
  // Areas y motor isometrico
  CArea      m_Area;      // Area <deberia de existir una lista>
  CIsoEngine m_IsoEngine; // Motor isometrico
  
  // Controla el modo de juego
  sModeInfo m_ModeInfo; // Info asociada al modo actual de universo de juego

  // Control del tiempo
  sTimeInfo m_TimeInfo; // Info asociada al tiempo

  // Control del sonido
  sAmbientWAVSound m_AmbientWAVSound; // Info asociada al sonido ambiente
  WAVSoundMap      m_WAVSounds;       // Sonidos anonimos activos  

  // Sobre scripts
  ScriptEventsMap m_ScriptEvents;  // Scripts instalados
  sIdleEventInfo  m_IdleEventInfo; // Info asociada al evento Idle

  // Observers
  sObservers m_Observers; // Observadores

  // Modo en que se ha inicializado el juego
  eStartMode m_StartMode; // Modo de inicializacion
    
  // Resto de vbles    
  bool m_bSavePermission;      // Permiso para guardar partidas
  bool m_bWorldTimePause;      // ¿Paso de las horas pausado?
  bool m_bIsLoadingASavedGame; // ¿Se esta cargando una partida guardada?
  bool m_bIsInPause;           // ¿Esta en pausa?
  bool m_bIsInitOk;            // ¿Clase inicializada?
    
protected:
  // Constructor / destructor
  CWorld(void): m_bIsInitOk(false),				
	            m_bIsInPause(false),
				m_bSavePermission(true),
				m_bIsLoadingASavedGame(false),
				m_pGraphSys(NULL),
				m_pAlarmManager(NULL),
				m_pResManager(NULL),
				m_pAudioSys(NULL),
				m_pGDBase(NULL),
				m_pFileSys(NULL),
			    m_pVMachine(NULL) { }
public:
  ~CWorld(void) { 
    End(); 
  }

public:
  // Protocolo de inicio y fin de instancia
  bool Init(void);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }
private:
  // Metodos de apoyo
  void InitTimeInfo(const byte ubHour, 
					const byte ubMinute);
  void EndTimeInfo(void);  
  
public:
  // Obtencion y destruccion de la instancia singlenton
  static inline CWorld* const GetInstance(void) {
    if (NULL == m_pWorld) { 
      m_pWorld = new CWorld; 
      ASSERT(m_pWorld)
    }
    return m_pWorld;
  }
  static inline void DestroyInstance(void) {
    if (m_pWorld) {
      delete m_pWorld;
      m_pWorld = NULL;
    }
  }

public:
  // Dibujado del universo
  inline void Draw(void) {
	ASSERT(IsInitOk());
	// Ordena al isometrico que dibujo
	m_IsoEngine.Draw();
  }

public:
  // Metodo de prueba para desvincular la inicializacion de un area
  // del metodo Init
  void InitArea(void);

public:
  // iCWorld / Trabajo con el establecimiento de modos temporales en el universo de juego
  void SetMode(const WorldDefs::eWorldMode& Mode);
  WorldDefs::eWorldMode GetMode(void) const {
	ASSERT(IsInitOk());
	// Retorna el modo de juego actual
	return m_ModeInfo.Mode;
  }

private:
  // Se indica que ha comenzado un nuevo juego
  void OnStartGame(void);

private:
  // iCWorld / Inicio del universo de juego / carga de areas
  // Nota: estos metodos estaran reservados a las clases amigas
  bool StartWorld(const word uwIDArea,
				  CPlayer* const pPlayer,
				  const AreaDefs::sTilePos& PlayerPos);
  bool ChangeArea(const word uwIDArea,
				  const AreaDefs::sTilePos& PlayerPos);
  void RemoveTmpDir(void);
  void InitGlobalScriptEvents(void);
  void ExecuteOnEntityCreatedEvent(void);

public:
  // iCWorld / Trabajo con el establecimiento de los distintos eventos
  void SetScriptEvent(const RulesDefs::eScriptEvents& Event,
					  const std::string& szFileName);
  std::string GetScriptEventFile(const RulesDefs::eScriptEvents& Event) {
	ASSERT(IsInitOk());
	// Se retorna el nombre del archivo asociado al evento recibido si existe
	const ScriptEventsMapIt It(m_ScriptEvents.find(Event));
	return (It != m_ScriptEvents.end()) ? It->second : "";
  }
  
public:
  // Finalizacion de una sesion
  void EndSesion(void);
private:
  void InitSesion(const byte ubHour = 0,
				  const byte ubMinute = 0,
				  const std::string& szWAVAmbientSound = "",
				  const std::string& szMIDIMusic = "",
				  const AudioDefs::eMIDIPlayMode& MIDIPlayMode = AudioDefs::MIDI_PLAY_NORMAL);  
  
public:
  // iCWorld / Guarda / carga una partida
  void SaveGame(const word uwIDSaveSlot,
				const std::string& szSaveDesc);
  void LoadGame(const word uwIDLoadSlot);
  bool IsLoadingASavedGame(void) const {
	ASSERT(IsInitOk());
	// Retorna el flag
	return m_bIsLoadingASavedGame;
  }

public:
  // iCWorld / Flag de permiso de guardar partidas
  bool CanSaveGame(void);
  void SetSavePermission(const bool bPermission) {
	ASSERT(IsInitOk());
	// Establece flag de permiso
	m_bSavePermission = bPermission;
  }
  bool GetSavePermission(void) const {
	ASSERT(IsInitOk());
	// Retorna flag de permiso
	return m_bSavePermission;
  }
  
public:
  // iCWorld / Operaciones sobre el sonido ambiente
  bool SetAmbientWAVSound(const std::string& szSound);
  bool IsAmbientWAVSoundActive(void) const {
	ASSERT(IsInitOk());
	// Retorna flag
	return (0 != m_AmbientWAVSound.hWAVAmbientSound)? true : false;
  }
  std::string GetAmbientWAVSound(void) const {
	ASSERT(IsInitOk());
	// Retorna el nombre del sonido ambiente
	return m_AmbientWAVSound.szWAVSoundFile;
  }
private:
  // Metodos de apoyo
  void StopAmbientWAVSound(void);  

public:
  // iCWorld / Operaciones sobre los sonidos anonimos
  bool PlayWAVSound(const std::string& szSound);
private:
  // Metodos de apoyo
  void ReleaseWAVSounds(void);

public:
  // iCWorld / Trabajo con los valores horarios y factor de oscuridad
  void SetWorldTimePause(const bool bPause);
  bool IsWorldTimeInPause(void);
  byte GetHour(void) const {
	ASSERT(IsInitOk());
	// Obtiene la hora actual
	return m_TimeInfo.ubHour;
  }
  byte GetMinute(void) const {
	ASSERT(IsInitOk());
	// Obtiene los minutos actuales
	return m_TimeInfo.ubMinute;
  }
  void SetHour(byte ubHour);
  void SetMinute(byte Minute);  
private:
  // Metodos de apoyo
  void UpdateTime(void);
  void SetHourDarkFactor(void);

public:
  // iCAlarmClient / Notificacion de la finalizacion de una alarma
  void AlarmNotify(const AlarmDefs::eAlarmType& AlarmType,
				   const AlarmDefs::AlarmHandle& hAlarm);

public:
  // iCWorld / Creacion y destruccion dinamica de entidades
  bool DestroyEntity(const AreaDefs::EntHandle& hEntity);
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
public:
  // iCWorld / Trabajo con el motor isometrico
  void AttachCamera(const AreaDefs::EntHandle& hEntity, 
					const word uwSpeed = 0,
					iCCommandClient* const pClient = NULL,
					const CommandDefs::IDCommand& IDCommand = 0,
					const dword udExtraParam = 0) {
	ASSERT(IsInitOk());	
	// Se asocia la camara
	m_IsoEngine.AttachCamera(hEntity, uwSpeed, pClient, IDCommand, udExtraParam);
  }
  void AttachCamera(const AreaDefs::sTilePos& TilePos,
				    const word uwSpeed = 0,
					iCCommandClient* const pClient = NULL,
					const CommandDefs::IDCommand& IDCommand = 0,
					const dword udExtraParam = 0) {
	ASSERT(IsInitOk());
	// Se asocia la camara
	m_IsoEngine.AttachCamera(TilePos, uwSpeed, pClient, IDCommand, udExtraParam);
  }    
  void AttachCamera(const float fXWorldPos, 
					const float fYWorldPos,
					const word uwSpeed = 0,
					iCCommandClient* const pClient = NULL,
					const CommandDefs::IDCommand& IDCommand = 0,
					const dword udExtraParam = 0) {
	ASSERT(IsInitOk());	
	// Se asocia la camara
	m_IsoEngine.AttachCamera(fXWorldPos,  fYWorldPos, uwSpeed, pClient, IDCommand, udExtraParam);
  }  
  void CenterCamera(void) {
	ASSERT(IsInitOk());	
	// Se centra la camara sobre la entidad asociada
	m_IsoEngine.CenterCamera();
  }
  IsoDefs::eSelectCellType SelectCell(const sword& swXScreen,
									  const sword& swYScreen) {
	ASSERT(IsInitOk());	
	// Selecciona la celda basada en una posicion de pantalla
	return m_IsoEngine.SelectCell(swXScreen, swYScreen);
  }
  void SetDrawSelectCell(const bool bDraw) {
	ASSERT(IsInitOk());	
	// Establece el flag de dibujado de la celda de seleccion
	m_IsoEngine.SetDrawSelectCell(bDraw);
  }
  AreaDefs::sTilePos GetSelectCellPos(void) const {
	ASSERT(IsInitOk());
	
	// Retorna la posicion donde esta la celda seleccionada
	return m_IsoEngine.GetSelectCellPos();
  }
  CWorldEntity* const GetEntityAt(const sword swXScreen,
							      const sword swYScreen) {
	ASSERT(IsInitOk());	
	// Localiza la entidad en la posicion recibida
	return m_IsoEngine.GetEntityAt(swXScreen, swYScreen);
  }
  sRect GetVisibleWorldArea(void) const {
	ASSERT(IsInitOk());
	
	// El area visible sera la ventana de dibujado del isometrico
	return m_IsoEngine.GetViewWindow();
  }
  void SetDrawArea(const bool bDraw) {
	ASSERT(IsInitOk());	
	// Activa / desactiva el dibujado del area
	m_IsoEngine.SetDrawFlag(bDraw);
  }
  bool IsDrawAreaActive(void) const {
	ASSERT(IsInitOk());
	
	// Obtiene el flag asociado al dibujado del area actual
	return m_IsoEngine.IsDrawFlagActive();
  }
  bool IsTilePosVisible(const AreaDefs::sTilePos& TilePos) const {  
	ASSERT(IsInitOk());
	
	// Comprueba si la entidad es visible
	return m_IsoEngine.IsTilePosVisible(TilePos);
  }
  bool GetEntityScreenPos(CWorldEntity* const pEntity, 
						  sword& swXScreen, 
						  sword& swYScreen) {
	ASSERT(IsInitOk());	
	// Obtiene la posicion de la entidad en pantalla
	return m_IsoEngine.GetEntityScreenPos(pEntity, swXScreen, swYScreen);	
  }
  void SetDrawWindow(const word uwXTop, 
					 const word uwYTop) {
	ASSERT(IsInitOk());
	// Establece la nueva ventana de dibujado
	m_IsoEngine.SetDrawWindow(uwXTop, uwYTop);
  }
  float GetXWorldPos(void) const {
	ASSERT(IsInitOk());
	
	// Retorna posicion en X donde comienza el universo
	return m_IsoEngine.GetXWorldPos();
  }
  float GetYWorldPos(void) const {
	ASSERT(IsInitOk());
	
	// Retorna posicion en Y donde comienza el universo
	return m_IsoEngine.GetYWorldPos();
  }
  bool MoveRight(const float fOffset) {
	ASSERT(IsInitOk());
	// Mueve universo a la derecha y notifica si procede
	if (m_IsoEngine.MoveRight(fOffset)) {	  
	  ObserversNotify(WorldObserverDefs::VISIBLE_WORLD_UPDATE, 0);
	  return true;
	} else{
	  return false;
	}
  }
  bool MoveLeft(const float fOffset) {
	ASSERT(IsInitOk());
	// Mueve universo a la izquierda
	if (m_IsoEngine.MoveLeft(fOffset)) {
	  ObserversNotify(WorldObserverDefs::VISIBLE_WORLD_UPDATE, 0);
	  return true;
	} else {
	  return false;
	}
  }
  bool MoveNorth(const float fOffset) {
	ASSERT(IsInitOk());	
	// Mueve universo hacia el norte
	if (m_IsoEngine.MoveNorth(fOffset)) {
	  ObserversNotify(WorldObserverDefs::VISIBLE_WORLD_UPDATE, 0);
	  return true;
	} else {
	  return false;
	}
  }
  bool MoveSouth(const float fOffset) {
	ASSERT(IsInitOk());	
	// Mueve universo hacia el sur
	if (m_IsoEngine.MoveSouth(fOffset)) {
	  ObserversNotify(WorldObserverDefs::VISIBLE_WORLD_UPDATE, 0);
	  return true;
	} else {
	  return false;
	}
  }
  void MoveTo(const IsoDefs::eDirectionIndex& Direction) {
	ASSERT(IsInitOk());	
	// Mueve el universo en la direccion indicada
	m_IsoEngine.MoveTo(Direction);	
  }
  bool MoveEntity(const AreaDefs::EntHandle& hEntity,
				  const IsoDefs::eDirectionIndex& Dir, 
				  const bool bForce) {
	ASSERT(IsInitOk());
	// Mueve entidad
	return m_IsoEngine.MoveEntity(hEntity, Dir, bForce);
  }
  CPath* FindPath(const AreaDefs::sTilePos& TileSrc, 
				  const AreaDefs::sTilePos& TileDest,
				  const word uwDistanceMin = 0,
				  const bool bGhostMode = false) {
	ASSERT(IsInitOk());
	// Encuentra camino de movimiento
	return m_IsoEngine.FindPath(TileSrc, TileDest, uwDistanceMin, bGhostMode);	
  }
  sword CalculePathLenght(const AreaDefs::sTilePos& TileSrc,
						  const AreaDefs::sTilePos& TileDest) {
	ASSERT(IsInitOk());
	// Calcula la longitud del camino entre TileSrc y TileDest si es que existe
	return m_IsoEngine.CalculePathLenght(TileSrc, TileDest);
	
  }
  sword CalculeAdjacentPosInDestination(const AreaDefs::sTilePos& TileSrc,
									    const AreaDefs::sTilePos& TileDest,
										AreaDefs::sTilePos& AdjacentTilePos) {
	ASSERT(IsInitOk());
	
	// Calcula una posicion adjacente a TileDest tal que el camino entre
	// TileSrc y esa pos. adjacente sea minimo, siempre y cuando exista ese camino
	return m_IsoEngine.CalculeAdjacentPosInDestination(TileSrc, 
														 TileDest, 
														 AdjacentTilePos);
  }
  sword CalculeAbsoluteDistance(const AreaDefs::sTilePos& TileSrc,
								const AreaDefs::sTilePos& TileDest) {
	ASSERT(IsInitOk());
	// Calcula la distancia absoluta (en pixels) entre TileSrc y TileDest
	return m_IsoEngine.CalculeAbsoluteDistance(TileSrc, TileDest);
  }
  bool IsAdjacentTo(const AreaDefs::sTilePos& TileSrc,
				    const AreaDefs::sTilePos& TileDest) {
	ASSERT(IsInitOk());
	// Comprueba si TileSrc es adjacente a TileDest
	return m_IsoEngine.IsAdjacentTo(TileSrc, TileDest);
  }
  sPosition PassTilePosToWorldPos(const AreaDefs::sTilePos& TilePos) {
	ASSERT(IsInitOk());
	// Traslada la responsabilidad al motor isometrico y retorna
	return m_IsoEngine.PassTilePosToWorldPos(TilePos);
  }
  bool ChangeLocation(const AreaDefs::EntHandle& hEntity,
					  const AreaDefs::sTilePos& NewPos) {
	ASSERT(IsInitOk());
	// Ordena el cambio de posicion
	return m_IsoEngine.ChangeLocation(hEntity, NewPos);
  }
  bool ChangeLocation(const AreaDefs::EntHandle& hItem,
					  const AreaDefs::EntHandle& hEntity) {
	ASSERT(IsInitOk());
	// Ordena el cambio de posicion
	return m_IsoEngine.ChangeLocation(hItem, hEntity);
  }
  void ChangeLocationForVisuals(const AreaDefs::EntHandle& hEntity,
								const AreaDefs::sTilePos& OriginalPos,
								const AreaDefs::sTilePos& VisualPos) {
	ASSERT(IsInitOk());
	// Cambia de posicion para correciones visuales
	m_IsoEngine.ChangeLocationForVisuals(hEntity, OriginalPos, VisualPos);
  }
  
public:
  // iCWorld / Trabajo con el jugador
  void MovPlayerToSelectCell(void) const {
	ASSERT(IsInitOk());
	
	// Mueve al jugador a la celsa que este seleccionada en el isometrico
	m_IsoEngine.MovPlayerToSelectCell();
  }
  CPlayer* const GetPlayer(void) {
	ASSERT(IsInitOk());
	// Retorna instancia al jugador
	return m_Area.IsInitOk() ? m_Area.GetPlayer() : NULL;
  }
private:
  // Metodos de apoyo
  void EndPlayer(void);

public:
  // iCWorld / Trabajo con el area de juego
  std::string GetAreaName(void) const {
	ASSERT(IsInitOk());
	// Retorna area de juego
	return m_Area.GetAreaName();
  }
  word GetIDArea(void) const {
	ASSERT(IsInitOk());
	// Retorna el identificador del area
	return m_Area.GetIDArea();
  }
  word GetAreaWidth(void) const {
	ASSERT(IsInitOk());
	// Retorna anchura del area
	return m_Area.GetWidth();
  }
  word GetAreaHeight(void) const {
	ASSERT(IsInitOk());
	// Retorna altura del area
	return m_Area.GetHeight();
  }  
  RulesDefs::eEntityType GetEntityType(const AreaDefs::EntHandle& hEntity) const {
	ASSERT(IsInitOk());
	// Retorna instancia
	return m_Area.GetEntityType(hEntity);
  }
  AreaDefs::EntHandle GetHandleFromTag(const std::string& szTag) {
	ASSERT(IsInitOk());
	// Retorna tag
	return m_Area.GetHandleFromTag(szTag);
  }
  std::string GetTagFromHandle(const AreaDefs::EntHandle& hEntity) {
	ASSERT(IsInitOk());
	// Retorna handle
	return m_Area.GetTagFromHandle(hEntity);
  }
  CWorldEntity* const GetWorldEntity(const AreaDefs::EntHandle& hEntity) {
	ASSERT(IsInitOk());
	// Retorna instancia
	return m_Area.GetWorldEntity(hEntity);
  }
  CSceneObj* const GetSceneObj(const AreaDefs::EntHandle& hSceneObj) {
	ASSERT(IsInitOk());
	// Retorna instancia
	return m_Area.GetSceneObj(hSceneObj);
  }  
  CItem* const GetItem(const AreaDefs::EntHandle& hItem) {
	ASSERT(IsInitOk());
	// Retorna instancia
	return m_Area.GetItem(hItem);
  }  
  CCriature* const GetCriature(const AreaDefs::EntHandle& hCriature) {
	ASSERT(IsInitOk());
	// Retorna instancia
	return m_Area.GetCriature(hCriature);
  }  
  CWall* const GetWall(const AreaDefs::EntHandle& hWall) {
	ASSERT(IsInitOk());
	// Retorna instancia
	return m_Area.GetWall(hWall);
  }   
  CFloor* const GetFloor(const AreaDefs::sTilePos& TilePos) {
	ASSERT(IsInitOk());
	// Retorna la instancia al CFloor
	return m_Area.GetFloor(TilePos);
  }  
  IsoDefs::eDirectionIndex CalculeDirection(const AreaDefs::sTilePos& TilePosSrc,
										    const AreaDefs::sTilePos& TilePosDest) {
	ASSERT(IsInitOk());
	// Calcula la direccion desde TileSrc y TileDest, siendo ambos tiles adyacentes
	return m_Area.CalculeDirection(TilePosSrc, TilePosDest);
  }
  void SetLight(const AreaDefs::EntHandle& hEntity,
				const GraphDefs::Light& LightIntensity) {
	ASSERT(IsInitOk());
	// Establece la luz asociada al area
	m_Area.SetLight(hEntity, LightIntensity);
  }
  GraphDefs::Light GetLight(const AreaDefs::EntHandle& hEntity) {
	ASSERT(IsInitOk());
	// Retorna la intensidad de luz asociada a la entidad hEntity
	return m_Area.GetLight(hEntity);
  }
  GraphDefs::Light GetLightAt(const AreaDefs::sTilePos& TilePos) {
	ASSERT(IsInitOk());
	// Devuelve el valor luminoso asociado a un tile
	return m_Area.GetLightAt(TilePos);
  }
  bool IsFloorValid(const AreaDefs::sTilePos& TilePos) const {
	ASSERT(IsInitOk());
	// Retorna el flag de floor valido
	return (m_Area.IsCellValid(TilePos) && 
			m_Area.IsCellWithContent(TilePos));
  }
  GraphDefs::Light GetAmbientLight(void) const {
	ASSERT(IsInitOk());
	// Retorna el modelo luminoso
	return m_Area.GetAmbientLight();
  }
  AreaDefs::MaskTileAccess GetMaskTileAccess(const AreaDefs::sTilePos& TilePos) {
	ASSERT(IsInitOk());
	// Retorna la mascara de acceso de una posicion
	return m_Area.GetMaskTileAccess(TilePos);
  }
  word GetNumItemsAt(const AreaDefs::sTilePos& TilePos);
  AreaDefs::EntHandle GetItemAt(const AreaDefs::sTilePos& TilePos,
								const word uwLocation);  
  void FindCriaturesInRangeAt(const AreaDefs::sTilePos& Pos,
							  const RulesDefs::CriatureRange& Range,
							  std::set<AreaDefs::EntHandle>& InRangeSet) {
	ASSERT(IsInitOk());
	// Obtiene un conjunto con los handles de las criatuaras localizadas
	// en un determinado rango asociado a una posicion particular
	m_Area.FindCriaturesInRangeAt(Pos, Range, InRangeSet);
  }
  bool IsCriatureInRangeAt(const AreaDefs::EntHandle& hCriature,
						   const AreaDefs::sTilePos& Pos,
						   const RulesDefs::CriatureRange& Range) {
	ASSERT(IsInitOk());
	// Comprueba si una criatura se halla dentro del rango asociado a
	// una posicion particular
	return m_Area.IsCriatureInRangeAt(hCriature, Pos, Range);
  }
  void FindCriaturesContainingCriatureInRange(const AreaDefs::EntHandle& hCriature,
									          std::set<AreaDefs::EntHandle>& InRangeSet) {
	ASSERT(IsInitOk());
	// Obtiene el cojunto de criaturas que contienen a la recibida en su rango
	m_Area.FindCriaturesContainingCriatureInRange(hCriature, InRangeSet);
  }

public:
  // iCWorld / Trabajo con los observadores
  void AddObserver(iCWorldObserver* const pObserver);
  void RemoveObserver(iCWorldObserver* const pObserver);
  void ObserversNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
					   const dword udParam);

public:
  // iCWorld / Pausa
  void SetPause(const bool bPause);
  bool IsInPause(void) const {
	ASSERT(IsInitOk());
	ASSERT(m_Area.IsInitOk() && m_Area.IsAreaLoaded());
	// Retorna flag
	return m_bIsInPause;
  }

public:
  // iCScriptClient / Operacion de notificacion, para cuando acabe un comando
  void ScriptNotify(const RulesDefs::eScriptEvents& ScriptEvent,
					const ScriptClientDefs::eScriptNotify& Notify,
					const dword udParams);

public:
  // iCWorld / Trabajo con el evento idle
  void SetIdleEventTime(const float fTime);
};

#endif // ~ CWorld

