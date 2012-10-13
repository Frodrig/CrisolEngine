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
// CWorld.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CWorld.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CWorld.h"

#include "SYSEngine.h"
#include "iCGraphicSystem.h"
#include "iCGUIManager.h"
#include "iCAlarmManager.h"
#include "iCGameDataBase.h"
#include "iCLogger.h"
#include "iCWorldObserver.h"
#include "iCCombatSystem.h"
#include "iCGFXManager.h"
#include "iCResourceManager.h"
#include "iCAudioSystem.h"
#include "iCFileSystem.h"
#include "CrisolBuilder\\CBDefs.h"
#include "io.h"
#include "CCBTEngineParser.h"

// Inicializacion de la unica instancia singlenton
CWorld* CWorld::m_pWorld = NULL;

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar
///////////////////////////////////////////////////////////////////////////////
bool 
CWorld::Init(void) 
{
  // ¿Se intenta reinicializar?  
  if (IsInitOk()) { 
	return false; 
  }

  #ifdef ENGINE_TRACE
    SYSEngine::GetLogger()->Write("CWorld::Init> Inicializando universo de juego.\n");
  #endif

  // Toma instancias a otros subsistemas
  m_pGraphSys = SYSEngine::GetGraphicSystem();
  ASSERT(m_pGraphSys);
  m_pAlarmManager = SYSEngine::GetAlarmManager();
  ASSERT(m_pAlarmManager);
  m_pGDBase = SYSEngine::GetGameDataBase();
  ASSERT(m_pGDBase);
  m_pResManager = SYSEngine::GetResourceManager();
  ASSERT(m_pResManager);
  m_pAudioSys = SYSEngine::GetAudioSystem();
  ASSERT(m_pAudioSys);
  m_pFileSys = SYSEngine::GetFileSystem();
  ASSERT(m_pFileSys);
  m_pVMachine = SYSEngine::GetVirtualMachine();
  ASSERT(m_pVMachine);
  
  // Toma la instancia al motor isometrico y se inicializa este 
  m_IsoEngine.Init();
  
  // Por defecto, se comenzara en modo real
  m_ModeInfo.Mode = WorldDefs::REAL_MODE;

  // Se inicializan resto de vbles
  m_bSavePermission = true;
  m_bWorldTimePause = false;
  m_TimeInfo.hAlarm = 0;
  m_IdleEventInfo.fTime = 0.0f;
  m_IdleEventInfo.hIdleAlarm = 0;
  m_AmbientWAVSound.hWAVAmbientSound = 0;
  m_AmbientWAVSound.szWAVSoundFile = "";

  #ifdef ENGINE_TRACE
    SYSEngine::GetLogger()->Write("            | Ok\n");
  #endif
  
  // Todo correcto  
  m_bIsInitOk = true;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la inicializacion de los valores horarios, estableciendo el valor
//   de horas y minutos y la alarma de control asociada
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWorld::InitTimeInfo(const byte ubHour, 
					 const byte ubMinute)
{  
  // Se inicializa hora y minuto a valores irreales y despues se ajustan con
  // los valores recibidos.
  m_TimeInfo.ubHour = 255;
  m_TimeInfo.ubMinute = 255;
  SetHour(ubHour);
  SetMinute(ubMinute);

  // Inicializa temporizador si procede
  if (!m_TimeInfo.hAlarm) {
	m_TimeInfo.hAlarm = m_pAlarmManager->InstallTimeAlarm(this, 
														  m_pGDBase->GetRulesDataBase()->GetSecondsPerMinute());
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia.
// Parametros:
// Devuelve:
// Notas:
// - No sera necesario finalizar la sesion actual porque es seguro que cuando
//   se llame a este metodo se estara abandonando el motor y no habra ninguna
//   sesion activa.
///////////////////////////////////////////////////////////////////////////////
void 
CWorld::End(void)
{
  if (IsInitOk()) {		
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CWorld::End> Finalizando universo de juego.\n");
	#endif

	// Finaliza motor isometrico
	ASSERT(!m_Area.IsInitOk());
	m_IsoEngine.End();
	
	// Baja flag	
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("           | Ok.\n");
	#endif
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la sesion de juego actual, terminando la instancia que representa
//   al jugador y el area en donde este se halla en el preciso instante de
//   recibir la orden de finalizacion
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWorld::EndSesion(void) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  ASSERT(m_Area.IsInitOk());

  #ifdef ENGINE_TRACE
    SYSEngine::GetLogger()->Write("CWorld::End> Finalizando sesión de juego.\n");
  #endif

  // Finaliza cualquier combate activo y quita la alineacion del jugador
  iCCombatSystem* const pCombatSys = SYSEngine::GetCombatSystem();
  ASSERT(pCombatSys);  
  pCombatSys->EndCombat();  
  ASSERT(m_Area.GetPlayer());
  pCombatSys->SetAlingment(m_Area.GetPlayer()->GetHandle(), 
						   CombatSystemDefs::NO_ALINGMENT);  
  
  // Se borra el contenido del directorio temporal
  RemoveTmpDir();

  // Se notifica que se va a comenzar a finalizar la sesion actual
  ObserversNotify(WorldObserverDefs::END_SESION, 0);

  // Se desinstala posible evento idle
  SetIdleEventTime(0.0f);

  // Libera map de eventos scripts
  m_ScriptEvents.clear();

  // Finaliza posibles scripts asociados
  m_pVMachine->ReleaseScripts(this);

  // Finaliza los scripts que pudieran estar aun pendientes
  m_pVMachine->EndScripts();

  // Finaliza cualquier sonido ambiente activo	
  StopAmbientWAVSound();

  // Finaliza los sonidos anonimos
  ReleaseWAVSounds();

  // Baja flag de dibujado y desvincula camara del isometrico
  // Nota: SERA VITAL desvincular la camara ahora o de lo contrario,
  // el isometrico tendra fallos al haberse finalizado el area antes
  m_IsoEngine.SetDrawFlag(false);
  m_IsoEngine.QuitCamera();  

  // Finaliza area actual
  m_Area.End(); 
	
  // Desvincula area del isometrico
  m_IsoEngine.SetArea(NULL); 
	
  // Finaliza control del horario
  EndTimeInfo();    
	
  #ifdef ENGINE_TRACE
    SYSEngine::GetLogger()->Write("           | Ok.\n");
  #endif
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece un nuevo modo asociado al universo de juego. El modo no se
//   referira mas a la forma en que el universo de juego tratara al tiempo.
//   Esta forma sera en tiempo real o bien en tiempo fraccionado, mediante un
//   funcionamiento por turnos.
// Parametros:
// - Mode. Modo en que se establecera el universo de juego.
// Devuelve:
// Notas:
// - Al indicar el modo establecido en MainInterfaz, si se da el caso de que
//   se establece un modo por turnos, se pasara como criatura con turno 
//   la criatura nula (valor 0)
///////////////////////////////////////////////////////////////////////////////
void 
CWorld::SetMode(const WorldDefs::eWorldMode& Mode)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Establece el modo
  m_ModeInfo.Mode = Mode;

  // Lo propaga al MainInterfaz
  SYSEngine::GetGUIManager()->MainInterfazSetMode(Mode);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza los valores asociados a la informacion temporal
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWorld::EndTimeInfo(void)
{
  // Finaliza
  if (IsInitOk()) {
	// Desinstala alarma asociada al control de los segundos si procede
	if (m_TimeInfo.hAlarm) {
	  m_pAlarmManager->UninstallAlarm(m_TimeInfo.hAlarm);
	  m_TimeInfo.hAlarm = 0;	 
	}	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Reproduce un sonido WAV de tal forma que haga las funciones de sonido
//   WAV ambiente (reproduccion ciclia). O bien, eliminara cualquier tipo de
//   sonido asociado si la cadena esta vacia.
// - En caso de que hubiera otro sonido previo, se detendra.
// Parametros:
// - szSound. Sonido a reproducir.
// Devuelve:
// - Si se pudo establecer true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CWorld::SetAmbientWAVSound(const std::string& szSound)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Es posible establecer sonido ambiente
  if (m_Area.IsInitOk() &&
	  !IsInPause()) {
	// Se detiene el sonido actual (si procede)
	StopAmbientWAVSound();
  
	// ¿Se desea reproducir algun sonido?
	if (!szSound.empty()) {
	  // Si, Se registra sonido ambiente, liberando el posible sonido previo
	  ASSERT((0 == m_AmbientWAVSound.hWAVAmbientSound) != 0);
	  m_AmbientWAVSound.hWAVAmbientSound = m_pResManager->WAVSoundRegister(szSound, 
																		   true);
	  if (!m_AmbientWAVSound.hWAVAmbientSound) {
		SYSEngine::FatalError("No se pudo abrir el fichero de sonido \"%s\".\nFichero inexistente o incompatible.\n", szSound.c_str());
	  }

	  // Se reproduce de forma ciclia y se establece nombre
	  m_pAudioSys->PlayWAVSound(m_AmbientWAVSound.hWAVAmbientSound,
								AudioDefs::WAV_PLAY_LOOP);
	  m_AmbientWAVSound.szWAVSoundFile = szSound;
	}

	// Todo correcto
	return true;
  }

  // No se pudo
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se detiene posible sonido WAV actual, liberandolo del servidor de 
//   recursos.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWorld::StopAmbientWAVSound(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // ¿Hay sonido almacenado?
  if (m_AmbientWAVSound.hWAVAmbientSound) {
    // Si, se libera
    m_pResManager->ReleaseWAVSound(m_AmbientWAVSound.hWAVAmbientSound);
    m_AmbientWAVSound.hWAVAmbientSound = 0;
    m_AmbientWAVSound.szWAVSoundFile = "";
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se comienza una nueva partida, cargando un area y asociando al jugador
//   a una posicion inicial.
// Parametros:
// - uwIDArea. Identificador del area a cargar como inicial
// - pPlayer. Instancia al jugador actual.
// - PlayerPos. Posicion del jugador al comenzar en esa nueva area.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - Pese a que este metodo servira para inicializar el universo de juego
//   de cara a una nueva partida, se debera de recibir desde el exterior 
//   una llamada al metodo OnStartGame para validarlo completamente.
///////////////////////////////////////////////////////////////////////////////
bool
CWorld::StartWorld(const word uwIDArea,
				   CPlayer* const pPlayer,
				   const AreaDefs::sTilePos& PlayerPos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pPlayer);
  ASSERT(pPlayer->IsInitOk());

  #ifdef ENGINE_TRACE    
    SYSEngine::GetLogger()->Write("CWorld::StartWorld> Inicializando nuevo juego\n");
  #endif 

  // Se borra el contenido del directorio temporal
  RemoveTmpDir();
  
  // Se inicializa area y se carga el primer mapa
  m_Area.Init();  
  if (!m_Area.SetInitArea(uwIDArea, pPlayer, PlayerPos)) {
	SYSEngine::FatalError("Problemas inicializando el área con identificador: %u.\n", uwIDArea);
	return false;
  }
  
  // Prepara el script global por primera vez
  m_pVMachine->PrepareScripts();
  
  // Inicializa sesion
  InitSesion();  

  // Establece permiso de guardar partidas
  m_bSavePermission = true; 
  
  // Establece que NO hay pausa del sistema horario
  m_bWorldTimePause = false;

  // Inicializa valores script idle
  m_IdleEventInfo.hIdleAlarm = 0;
  m_IdleEventInfo.fTime = 0.0f;

  // Establece los scripts World y Game definidos por defecto
  // para comenzar una nueva partida
  InitGlobalScriptEvents();
  
  // Se establece flag de inicio a partir de un nuevo juego
  m_StartMode = CWorld::SM_NEWGAME; 

  // Todo correcto  
  #ifdef ENGINE_TRACE    
    SYSEngine::GetLogger()->Write("                  | Ok.\n");
  #endif 
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se ejecutara desde CGSInGame para indicar que finalmente se ha inicializado
//   el juego, una vez cargado los datos e inicializado otros elementos internos
//   del engine.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWorld::OnStartGame(void)
{
  ASSERT(IsInitOk());

  // Si se ha comenzado una nueva partida sin recuperarla, se ejecutara
  // el script asociado al evento de nueva partida
  if (m_StartMode == SM_NEWGAME) {
	// Se ejecutan los eventos asociados a la creacion de entidades
	ExecuteOnEntityCreatedEvent();

	// Se lanza el evento asociado a entrada en area
	m_pVMachine->OnEnterInArea(this, 
							   GetScriptEventFile(RulesDefs::SE_ONENTERINAREA),
							   m_Area.GetIDArea());

	// Se lanza evento asociado a comienzo de juego
	m_pVMachine->OnStartGameEvent(this, 
								  GetScriptEventFile(RulesDefs::SE_ONSTARTGAME));
  }
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la inicializacion de los nombres de ficheros asociados a los
//   scripts globales de World y Game.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWorld::InitGlobalScriptEvents(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se asocian todos los eventos existentes para World
  CCBTEngineParser* pParser = m_pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_GLOBALSCRIPTEVENTS,
												      "[WorldEvents]");
  ASSERT(pParser);
  pParser->SetVarPrefix("");
  SetScriptEvent(RulesDefs::SE_ONSTARTGAME, pParser->ReadString("ScriptEvent[OnStartGame]", false));
  SetScriptEvent(RulesDefs::SE_ONNEWHOUR, pParser->ReadString("ScriptEvent[OnNewHour]", false));
  SetScriptEvent(RulesDefs::SE_ONENTERINAREA, pParser->ReadString("ScriptEvent[OnEnterInArea]", false));
  SetScriptEvent(RulesDefs::SE_ONWORLDIDLE, pParser->ReadString("ScriptEvent[OnWorldIdle]", false));  

  // Se asocian todos los eventos existentes para Game
  pParser = m_pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_GLOBALSCRIPTEVENTS,
							        "[GameEvents]");
  ASSERT(pParser);
  SetScriptEvent(RulesDefs::SE_ONCLICKHOURPANEL, pParser->ReadString("ScriptEvent[OnClickHourPanel]", false));
  SetScriptEvent(RulesDefs::SE_ONFLEECOMBAT, pParser->ReadString("ScriptEvent[OnFleeCombat]", false));
  SetScriptEvent(RulesDefs::SE_ONKEYPRESSED, pParser->ReadString("ScriptEvent[OnKeyPressed]", false));
  SetScriptEvent(RulesDefs::SE_ONSTARTCOMBATMODE, pParser->ReadString("ScriptEvent[OnStartCombatMode]", false));
  SetScriptEvent(RulesDefs::SE_ONENDCOMBATMODE, pParser->ReadString("ScriptEvent[OnEndCombatMode]", false));
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece scripts asociados a los distintos eventos World y Game. En
//   caso de existir un nombre previo, este quedara sustituido y en caso de
//   que se reciba un nombre vacio para un determinado evento, este sera 
//   quitado.
// Parametros:
// - Event. Codigo del evento.
// - szFileName. Nombre del evento.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWorld::SetScriptEvent(const RulesDefs::eScriptEvents& Event,
					   const std::string& szFileName)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Se desea eleminar entrada?
  if (szFileName.empty()) {
  	// Si, se borra
	m_ScriptEvents.erase(Event);
  } else {
  	// No, se establece / sustituye nueva entrada
	ScriptEventsMapIt It(m_ScriptEvents.find(Event));
	if (It != m_ScriptEvents.end()) {
	  It->second = szFileName;
	} else {
	  m_ScriptEvents.insert(ScriptEventsMapValType(Event, szFileName));
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo para inicializar una sesion estableciendo una serie de valores
//   basicos. Estos valores estaran asignados por defecto pero seguro que
//   son leidos cuando se cargue una partida.
// - Tambien ajusta valores basicos de caracter general
// - En lo referido a scripts, se localizaran los que esten asociados a los
//   distintos eventos pero no se prepara el sistema ya que desde el 
//   exterior se podra querer recuperar el valor de las vbles globales.
// Parametros:
// - ubHour, ubMinute. Hora y minuto para empezar una sesion.
// - szWAVAmbientSound. Nombre del sonido ambiente.
// - szMIDIMusic, MIDIPlayMode. Nombre de la musica de fondo y su forma de
//   ser reproducido.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWorld::InitSesion(const byte ubHour,
				   const byte ubMinute,
				   const std::string& szWAVAmbientSound,
				   const std::string& szMIDIMusic,
				   const AudioDefs::eMIDIPlayMode& MIDIPlayMode)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  #ifdef ENGINE_TRACE    
     SYSEngine::GetLogger()->Write("CWorld::InitSesion> Inicializando sesión de juego.\n");
  #endif 

  // Inicializa valor horarios  
  InitTimeInfo(ubHour, ubMinute);	

  // ¿El area no usa el la luz ambiental del universo de juego?
  if (m_Area.GetAmbientLight() != 0) {
	// No la ultiliza, luego se establece la adecuada al area
	m_pGraphSys->SetDarkFactor(m_Area.GetAmbientLight());
  } 

  // Se establece area en el motor isometrico y ventana de dibujado   				
  m_IsoEngine.SetArea(&m_Area);    
  m_IsoEngine.SetDrawWindow(m_pGraphSys->GetVideoWidth(),
							  m_pGraphSys->GetVideoHeight() - 96);

  // Se alinea jugador en combate
  SYSEngine::GetCombatSystem()->SetAlingment(m_Area.GetPlayer()->GetHandle(), 
											 CombatSystemDefs::PLAYER_ALINGMENT);  

  // Por defecto se asociara la camara al jugador directamente
  m_IsoEngine.AttachCamera(m_Area.GetPlayer()->GetHandle());

  // Baja el flag de pausa
  // Nota: Sera necesario hacerlo ANTES de establecer sonido ambiente
  m_bIsInPause = false;
    
  // Se establece sonido ambiente
  SetAmbientWAVSound(szWAVAmbientSound);

  // Se establece musica de fondo si procede
  if (!szMIDIMusic.empty()) {
	m_pAudioSys->PlayMIDIMusic(szMIDIMusic, MIDIPlayMode);
  } else {
	m_pAudioSys->StopMIDIMusic();
  }  

  // Se notifica nueva sesion
  ObserversNotify(WorldObserverDefs::NEW_SESION, 0);

  #ifdef ENGINE_TRACE    
     SYSEngine::GetLogger()->Write("                  | Ok.\n");
  #endif 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Cambia de area. Este metodo se ejecutara dentro del juego cuando
//   exista un simple cambio de area.
// Parametros:
// - uwIDArea. Identificador del area.
// - PlayerPos. Posicion del jugador.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CWorld::ChangeArea(const word uwIDArea,
				   const AreaDefs::sTilePos& PlayerPos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  #ifdef ENGINE_TRACE    
     SYSEngine::GetLogger()->Write("CWorld::ChangeArea> Cambiando al área %u.\n", uwIDArea);
  #endif 

  // Se acaba posible combate activo  
  SYSEngine::GetCombatSystem()->EndCombat();  

  // Se liberan sonidos anonimos abiertos
  ReleaseWAVSounds();

  // Se inicializa el area actual, indicando que es un cambio de area
  if (!m_Area.ChangeArea(uwIDArea, PlayerPos)) {
	SYSEngine::FatalError("Problemas cambiando al área %u posición <%d, %d>.\n", 
						  uwIDArea, PlayerPos.XTile, PlayerPos.YTile);
	return false;
  }

  // ¿El area no usa el la luz ambiental del universo de juego?
  if (m_Area.GetAmbientLight() != 0) {
	// No la ultiliza, luego se establece la adecuada al area
	m_pGraphSys->SetDarkFactor(m_Area.GetAmbientLight());
  } else {
	// Se establece el valor de oscuridad asociado a la hora
	SetHourDarkFactor();
  }
  
  // Se establece area en el motor isometrico y ventana de dibujado   				
  // Nota: Para reestablecer el area, antes se eliminara la informacion posterior
  // * Se debera de contemplar la forma de hacer esto de manera mucho mas elegante
  m_IsoEngine.SetArea(NULL);    
  m_IsoEngine.SetArea(&m_Area);    
  m_IsoEngine.SetDrawWindow(m_pGraphSys->GetVideoWidth(),
							  m_pGraphSys->GetVideoHeight() - 96);

  // Se asocia camara al jugador directamente
  m_IsoEngine.AttachCamera(m_Area.GetPlayer()->GetHandle());

  // Se ejecutan los eventos asociados a la creacion de entidades
  // Nota: SOLO si el area cambiada es no temporal
  ExecuteOnEntityCreatedEvent();

  // Se lanza el evento asociado a entrada en area
  m_pVMachine->OnEnterInArea(this,
							 GetScriptEventFile(RulesDefs::SE_ONENTERINAREA),
							 m_Area.GetIDArea());
  
  // Todo correcto  
  #ifdef ENGINE_TRACE    
     SYSEngine::GetLogger()->Write("                  | Ok.\n");
  #endif 
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodos llamado cuando se cambia de area o se comienza una nueva partida
//   y que sirve para propagar los eventos de creacion de entidades en el
//   universo de juego. El area sobre la que iterar NO debera de ser temporal.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWorld::ExecuteOnEntityCreatedEvent(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿El area NO es temporal?
  if (!m_Area.IsTempArea()) {
	// Se itera a traves de items
	CItemIterator ItemIt;
	ItemIt.Init(&m_Area);
	ASSERT(ItemIt.IsInitOk());
	for (; ItemIt.IsItValid(); ItemIt.Next()) {
	  m_pVMachine->OnEntityCreated(ItemIt.GetEntity(),
								   ItemIt.GetEntity()->GetScriptEvent(RulesDefs::SE_ONENTITYCREATED),
								   ItemIt.GetEntity()->GetHandle());
	}

	// Se itera a traves de objetos de escenario
	CSceneObjIterator SceneObjIt;
	SceneObjIt.Init(&m_Area);
	ASSERT(SceneObjIt.IsInitOk());
	for (; SceneObjIt.IsItValid(); SceneObjIt.Next()) {
	  m_pVMachine->OnEntityCreated(SceneObjIt.GetEntity(),
								   SceneObjIt.GetEntity()->GetScriptEvent(RulesDefs::SE_ONENTITYCREATED),
								   SceneObjIt.GetEntity()->GetHandle());
	}

	// Se itera a traves de criaturas
	CCriatureIterator CriatureIt;
	CriatureIt.Init(&m_Area);
	ASSERT(CriatureIt.IsInitOk());
	for (; CriatureIt.IsItValid(); CriatureIt.Next()) {
	  m_pVMachine->OnEntityCreated(CriatureIt.GetEntity(),
								   CriatureIt.GetEntity()->GetScriptEvent(RulesDefs::SE_ONENTITYCREATED),
								   CriatureIt.GetEntity()->GetHandle());
	}

	// Se itera a traves de paredes
	CWallIterator WallIt;
	WallIt.Init(&m_Area);
	ASSERT(WallIt.IsInitOk());
	for (; WallIt.IsItValid(); WallIt.Next()) {
	  m_pVMachine->OnEntityCreated(WallIt.GetEntity(),
								   WallIt.GetEntity()->GetScriptEvent(RulesDefs::SE_ONENTITYCREATED),
								   WallIt.GetEntity()->GetHandle());
	}
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si es posible guardar una partida. Se podra guardar una partida
//   si no esta activo el modo combate, no se esta en una CutScene y no esta
//   desactivado el permiso manual de guardar partidas.
// Parametros:
// Devuelve:
// - El flag que indique si se puede o no guardar una partida.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CWorld::CanSaveGame(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se retorna flag
  iCCombatSystem* const pCombatSys = SYSEngine::GetCombatSystem();
  ASSERT(pCombatSys);
  iCGUIManager* const pGUIManager = SYSEngine::GetGUIManager();
  ASSERT(pGUIManager);
  return (!pCombatSys->IsCombatActive() &&
	      !pGUIManager->IsCutSceneActive() &&
		  GetSavePermission());
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Guarda una partida. Parte de la responsabilidad recaera en CArea.
// Parametros:
// - uwIDSaveSlot. Identificador del slot donde guardar.
// - szSaveDesc. Descripcion de la partida a guardar.
// Devuelve:
// Notas:
// - Solo se podra guardar una partida si se esta jugando (CArea se halla
//   activado)
///////////////////////////////////////////////////////////////////////////////
void 
CWorld::SaveGame(const word uwIDSaveSlot,
				 const std::string& szSaveDesc)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si se esta jugando
  ASSERT(m_Area.IsInitOk());
  ASSERT(m_Area.IsAreaLoaded());
  ASSERT(CanSaveGame());

  // Se abre archivo para almacenar datos
  std::string szValue;
  SYSEngine::PassToString(szValue, "save\\save_%u.sav", uwIDSaveSlot);
  const FileDefs::FileHandle hFile = m_pFileSys->Open(szValue, true);
  ASSERT(hFile);
  dword udOffset = 0;

  // Datos de cabecera 
  // Tipo de fichero
  udOffset += m_pFileSys->Write(hFile, 
								udOffset, 
								(sbyte *)(&WorldDefs::SaveFileType), 
								sizeof(byte));  

  // Version
  udOffset += m_pFileSys->Write(hFile, 
								udOffset, 
								(sbyte *)(&WorldDefs::SaveFileHVersion), 
								sizeof(byte));  
  udOffset += m_pFileSys->Write(hFile, 
								udOffset, 
								(sbyte *)(&WorldDefs::SaveFileLVersion), 
								sizeof(byte));

  // Descripcion
  udOffset += m_pFileSys->WriteStringInBinary(hFile, 
											  udOffset, 
											  szSaveDesc);

  // Guarda de forma temporal, la posicion donde se debera de insertar 
  // el offset a la zona del archivo donde se halla la descripcion del mismo
  // para ser mostrada en las interfaces de cargar y guardar archivos
  const dword udFileDescOffset = udOffset;
  udOffset += m_pFileSys->Write(hFile, 
  								udOffset, 
								(sbyte *)(&udFileDescOffset), 
								sizeof(dword));

  // Informacion del universo de juego
  // Hora y minutos
  udOffset += m_pFileSys->Write(hFile, 
								udOffset, 
								(sbyte *)(&m_TimeInfo.ubHour), 
								sizeof(byte));
  udOffset += m_pFileSys->Write(hFile, 
								udOffset, 
								(sbyte *)(&m_TimeInfo.ubMinute), 
								sizeof(byte));

  // Flag del persmiso del paso del tiempo
  udOffset += m_pFileSys->Write(hFile, 
								udOffset, 
								(sbyte *)(&m_bWorldTimePause), 
								sizeof(bool));

  // Posible sonido ambiente
  udOffset += m_pFileSys->WriteStringInBinary(hFile, 
											  udOffset, 
											  m_AmbientWAVSound.szWAVSoundFile);

  // Posible musica de fondo  
  AudioDefs::eMIDIPlayMode MIDIPlayMode;
  m_pAudioSys->GetMIDIInfo(szValue, MIDIPlayMode);  
  udOffset += m_pFileSys->WriteStringInBinary(hFile,
											  udOffset, 
											  szValue);
  udOffset += m_pFileSys->Write(hFile, 
								udOffset, 
								(sbyte *)(&MIDIPlayMode), 
								sizeof(AudioDefs::eMIDIPlayMode));  

  // Guarda flag asociado a guardar partidas
  udOffset += m_pFileSys->Write(hFile, 
								udOffset, 
								(sbyte *)(&m_bSavePermission), 
								sizeof(bool));

  // Flag de muestra de los FPS
  const bool bFPSShow = SYSEngine::IsFPSShowActive();
  udOffset += m_pFileSys->Write(hFile,
								udOffset,
								(sbyte *)(&bFPSShow),
								sizeof(bool));

  // Se guardan los eventos script instalados
  // Cantidad y por cada evento, codigo y nombre fichero
  const byte ubNumScriptEvents = m_ScriptEvents.size();
  udOffset += m_pFileSys->Write(hFile,
							    udOffset,
								(sbyte *)(&ubNumScriptEvents),
								sizeof(byte));
  ScriptEventsMapIt EventsIt(m_ScriptEvents.begin());
  for (; EventsIt != m_ScriptEvents.end(); ++EventsIt) {
	// Se guarda codigo y nombre fichero
	udOffset += m_pFileSys->Write(hFile,
	                              udOffset,
								  (sbyte *)(&EventsIt->first),
								  sizeof(RulesDefs::eScriptEvents));
	udOffset += m_pFileSys->WriteStringInBinary(hFile, udOffset, EventsIt->second);
  }

  // Tiempo asociado al evento idle
  udOffset += m_pFileSys->Write(hFile, 
								udOffset, 
								(sbyte *)(&m_IdleEventInfo.fTime), 
								sizeof(float));

  // Valores del script global  
  m_pVMachine->SaveGlobals(hFile, udOffset);

  // Se completa la informacion del archivo pasando la responsabilidad a
  // CArea que guardara los datos del jugador y los archivos temporales
  // de las areas
  m_Area.SaveGame(hFile, udOffset);

  // Guarda el offset donde se van a guardar los datos con la desc del archivo
  m_pFileSys->Write(hFile, 
					udFileDescOffset, 
					(sbyte *)(&udOffset), 
					sizeof(dword));

  // Guarda la desc. del archivo para interfaces
  // Nota: (Comprendera: Nombre area, nombre jugador, salud jugador y hora)
  // Nombre area
  udOffset += m_pFileSys->WriteStringInBinary(hFile, udOffset, m_Area.GetAreaName());
  // Nombre jugador
  udOffset += m_pFileSys->WriteStringInBinary(hFile, udOffset, m_Area.GetPlayer()->GetName());  
  // Salud jugador
  RulesDefs::sPairValue Health;
  Health.swBase = m_Area.GetPlayer()->GetHealth(RulesDefs::BASE_VALUE);
  Health.swTemp = m_Area.GetPlayer()->GetHealth(RulesDefs::TEMP_VALUE);
  udOffset += m_pFileSys->Write(hFile, 
								udOffset, 
								(sbyte *)(&Health), 
								sizeof(RulesDefs::sPairValue));
  // Hora y minutos
  udOffset += m_pFileSys->Write(hFile, 
								udOffset, 
								(sbyte *)(&m_TimeInfo.ubHour), 
								sizeof(byte));
  udOffset += m_pFileSys->Write(hFile, 
								udOffset, 
								(sbyte *)(&m_TimeInfo.ubMinute), 
								sizeof(byte));

  // Cierra fichero
  m_pFileSys->Close(hFile);	    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga una partida cuyo identificador de slot es uwIDLoadSlot.
// Parametros:
// - uwIDLoadSlot. Identificador de slot
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWorld::LoadGame(const word uwIDLoadSlot)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  #ifdef ENGINE_TRACE    
     SYSEngine::GetLogger()->Write("CWorld::LoadGame> Cargando juego en slot %u\n", uwIDLoadSlot);
  #endif 

  // Se levanta flag
  m_bIsLoadingASavedGame = true;

  // Se para posible sonido ambiente y musica
  m_pAudioSys->StopMIDIMusic();
  SetAmbientWAVSound("");

  // Se liberan sonidos anonimos abiertos
  ReleaseWAVSounds();

  // Resetea informacion de consola
  SYSEngine::GetGUIManager()->ResetConsoleInfo();
  
  // Se comprueba que el fichero sea el correcto
  std::string szValue;
  SYSEngine::PassToString(szValue, "save\\save_%u.sav", uwIDLoadSlot);
  const FileDefs::FileHandle hFile = m_pFileSys->Open(szValue);
  ASSERT(hFile);
  dword udOffset = 0;
  
  // Datos de cabecera 
  // Tipo de fichero
  byte ubFileType;
  udOffset += m_pFileSys->Read(hFile, 
	                           (sbyte *)(&ubFileType),
							   sizeof(byte),
								udOffset);  
  if (ubFileType != WorldDefs::SaveFileType) {
	SYSEngine::FatalError("El fichero %s no es un fichero correcto\n", szValue.c_str());
  }
  
  // Salta version, descripcion y offset con la descripcion del archivo para interfaces
  udOffset += sizeof(byte) * 2;
  udOffset += m_pFileSys->ReadStringFromBinary(hFile, udOffset, szValue);
  dword udFileDescOffset;
  udOffset += m_pFileSys->Read(hFile, 
	                           (sbyte *)(&udFileDescOffset),
							   sizeof(dword),
							   udOffset);  

  // Lectura de la info asociada al estado del universo de juego
  // Se lee hora y minutos.
  byte ubHour;
  udOffset += m_pFileSys->Read(hFile, 
	                           (sbyte *)(&ubHour),
							   sizeof(byte),
								udOffset);  
  byte ubMinute;
  udOffset += m_pFileSys->Read(hFile, 
	                           (sbyte *)(&ubMinute),
							   sizeof(byte),
								udOffset);  

  // Flag del permiso del paso del tiempo
  bool bWorldTimePause;
  udOffset += m_pFileSys->Read(hFile, 
	                           (sbyte *)(&bWorldTimePause),
							   sizeof(bool),
								udOffset);    
  // Posible sonido ambiente
  std::string szWAVAmbientSound;
  udOffset += m_pFileSys->ReadStringFromBinary(hFile, udOffset, szWAVAmbientSound);
  
  // Posible musica de fondo
  std::string szMIDIMusic;
  udOffset += m_pFileSys->ReadStringFromBinary(hFile, udOffset, szMIDIMusic);
  AudioDefs::eMIDIPlayMode MIDIPlayMode;
  udOffset += m_pFileSys->Read(hFile, 
	                           (sbyte *)(&MIDIPlayMode),
							   sizeof(AudioDefs::eMIDIPlayMode),
							   udOffset);  

  // Carga el flag que permite guardar partidas
  udOffset += m_pFileSys->Read(hFile, 
							   (sbyte *)(&m_bSavePermission), 
								sizeof(bool),
								udOffset);

  // Flag de muestra de los FPS
  bool bFPSShow;
  udOffset += m_pFileSys->Read(hFile,
							   (sbyte *)(&bFPSShow),
							   sizeof(bool),
							   udOffset);
  SYSEngine::ShowFPS(bFPSShow);

  // Se cargan los ficheros scripts asociados a los eventos
  byte ubNumScriptEvents;
  udOffset += m_pFileSys->Read(hFile,
							   (sbyte *)(&ubNumScriptEvents),
							   sizeof(byte),
							   udOffset);
  word uwEventsIt = 0;
  for (; uwEventsIt < ubNumScriptEvents; ++uwEventsIt) {
	// Lee codigo y nombre del fichero script para luego asociar
	RulesDefs::eScriptEvents Event;
	udOffset += m_pFileSys->Read(hFile,
	                              (sbyte *)(&Event),
								  sizeof(RulesDefs::eScriptEvents),
								  udOffset);
	std::string szFileName;
	udOffset += m_pFileSys->ReadStringFromBinary(hFile, udOffset, szFileName);
	SetScriptEvent(Event, szFileName);
  }

  // Obtiene tiempo asociado al evento idle y lo establece
  float fIdleTime;  
  udOffset += m_pFileSys->Read(hFile, 
							   (sbyte *)(&fIdleTime), 
							   sizeof(float),
							   udOffset);
  SetIdleEventTime(fIdleTime);

  // Prepara el script global, recuperando su estado de disco
  // Nota: Preparar los script supondra ejecutar el script global que 
  // unicamente inicializara las vbles globales
  m_pVMachine->PrepareScripts(hFile, udOffset);
  
  // ¿Area inicializada?
  if (m_Area.IsInitOk()) {
	// Se finaliza la sesion, borrandose tambien los archivos temporales
	EndSesion();
  } else {
	// Se borran archivos temporales unicamente
	RemoveTmpDir();
  }
  
  // Se inicializa area y se carga
  m_Area.Init();  
  m_Area.LoadGame(hFile, udOffset);  
  
  // Inicializa sesion 
  InitSesion(ubHour, ubMinute, szWAVAmbientSound, szMIDIMusic, MIDIPlayMode);  
  
  // Cierra fichero
  m_pFileSys->Close(hFile);

  // Se establece flag de inicio a partir de una partida guardada
  m_StartMode = CWorld::SM_LOADGAME; 

  // Si se leyo que el universo de juego tenia el sistema horario
  // pausado, se ejecutara tal orden
  if (bWorldTimePause) {
	SetWorldTimePause(bWorldTimePause);
  }

  // Se baja flag
  m_bIsLoadingASavedGame = false;

  #ifdef ENGINE_TRACE    
     SYSEngine::GetLogger()->Write("                | Ok.\n");
  #endif 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion asociada a alarmas. Respondera a las siguientes 
//   notificaciones:
//    * Ha finalizado un minuto de juego
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWorld::AlarmNotify(const AlarmDefs::eAlarmType& AlarmType,
				    const AlarmDefs::AlarmHandle& hAlarm)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hAlarm);

  // Se comprueba tipo de notificacion
  switch (AlarmType) {
	case AlarmDefs::TIME_ALARM: {
	  // Se comprueba a quien pertenece el handle	  
	  if (hAlarm == m_TimeInfo.hAlarm) {
		// Ha finalizado un minuto de juego
		// Se actualizan valores horarios y se reinstala temporizador
		UpdateTime();
		m_TimeInfo.hAlarm = m_pAlarmManager->InstallTimeAlarm(this, 
															  m_pGDBase->GetRulesDataBase()->GetSecondsPerMinute());
		ASSERT(m_TimeInfo.hAlarm);		
	  }	else if (hAlarm == m_IdleEventInfo.hIdleAlarm) {
		// Alarma asociada al evento Idle
		// Se lanza el POSIBLE script asociado, desvinculandose handle
		m_IdleEventInfo.hIdleAlarm = 0;
	    m_pVMachine->OnWorldIdleEvent(this, GetScriptEventFile(RulesDefs::SE_ONWORLDIDLE));
	  }
	} break;

	case AlarmDefs::WAV_ALARM: {
	  // Seguro que es una alarma asociada a un sonido anonimo
	  // Se libera nodo y recurso
	  WAVSoundMapIt It(m_WAVSounds.find(hAlarm));
	  ASSERT((It != m_WAVSounds.end()) != 0);
	  m_pResManager->ReleaseWAVSound(It->second);
	  m_WAVSounds.erase(It);
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Actualiza el valor temporal, incrementando los minutos y propagando la
//   actualizacion horaria segun proceda.
// Parametros:
// Devuelve:
// Notas:
// - La adecuacion del factor de oscuridad solo ocurrira si el area actual 
//   admite reflejo visual del transito horario.
///////////////////////////////////////////////////////////////////////////////
void
CWorld::UpdateTime(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Actualiza valores horarios
  m_TimeInfo.ubMinute++;
  if (m_TimeInfo.ubMinute >= 60) {
	// Nueva hora
	m_TimeInfo.ubMinute = 0;
	m_TimeInfo.ubHour++;
	if (m_TimeInfo.ubHour >= 24) {
	  m_TimeInfo.ubHour = 0;
	}
	
	// Se ajusta el factor de oscuridad
	SetHourDarkFactor();

	// Notifica a los posibles observers del cambio de hora
	ObserversNotify(WorldObserverDefs::NEW_HOUR, m_TimeInfo.ubHour);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ajusta la hora segun el valor ubHour
// Parametros:
// - ubHour. Nueva hora.
// Devuelve:
// Notas:
// - En caso de que la hora no sea valida, retornara sin hacer nada
///////////////////////////////////////////////////////////////////////////////
void 
CWorld::SetHour(byte ubHour)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Si la hora es mayor que la permitida ajusta
  if (ubHour > 23) { 
	ubHour = 23;
  }
  
  // ¿Estamos a distinta hora?
  if (ubHour != m_TimeInfo.ubHour) { 	
	// Cambia la hora y ajusta factor de oscuridad
	m_TimeInfo.ubHour = ubHour;
	SetHourDarkFactor();
  
	// Notifica a los posibles observers del cambio de hora
	ObserversNotify(WorldObserverDefs::NEW_HOUR, m_TimeInfo.ubHour);
	
	// Se genera el evento script
	m_pVMachine->OnNewHourEvent(this, 
								GetScriptEventFile(RulesDefs::SE_ONNEWHOUR), 
								m_TimeInfo.ubHour);
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ajusta los minutos
// Parametros:
// - ubMinute. Minutos a establecer
// Devuelve:
// Notas:
// - En caso de que los minutos no sean valida, retornara sin hacer nada
///////////////////////////////////////////////////////////////////////////////
void 
CWorld::SetMinute(byte ubMinute)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se ajusta si no es valido
  if (ubMinute > 59) { 
	ubMinute = 59;
  }
  
  // ¿Estamos a distintos minutos?
  if (ubMinute != m_TimeInfo.ubMinute) { 	
	// Almacena los minutos
	m_TimeInfo.ubMinute = ubMinute;
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el factor de oscuridad asociado a la hora actual. 
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWorld::SetHourDarkFactor(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si area cargada
  ASSERT(m_Area.IsAreaLoaded());

  // Se ajusta el factor de oscuridad SOLO si el area no tiene 
  // un factor de oscuridad particular.
  if (m_Area.GetAmbientLight() == 0) {
    m_pGraphSys->SetDarkFactor(SYSEngine::GetGFXManager()->GetAmbientLight(m_TimeInfo.ubHour));
  }	
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina el observador pObserver de la lista de observadores.
// Parametros:
// - pObserver. Instancia al observer
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWorld::AddObserver(iCWorldObserver* const pObserver) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pObserver);

  // ¿El observer se halla instalado?
  if (std::find(m_Observers.Observers.begin(), 
				m_Observers.Observers.end(), 
				pObserver) != m_Observers.Observers.end()) {
	// Si, se cancela accion
	return;
  }
  
  // ¿Se esta en proceso de notificacion?
  if (m_Observers.bInNotify) {
	// Si, se añade a la lista temporal
	m_Observers.ObToAdd.push_back(pObserver);
  } else {
	// No, se añade directamente
	m_Observers.Observers.push_back(pObserver);
  }
}
    
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina el observador pObserver de la lista de observadores.
// Parametros:
// - pObserver. Instancia al observer
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWorld::RemoveObserver(iCWorldObserver* const pObserver)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pObserver);

  // ¿Se esta en proceso de notificacion?
  if (m_Observers.bInNotify) {
	// Si, se añade a la lista temporal
	m_Observers.ObToRemove.push_back(pObserver);
  } else {	
	// No, localiza el nodo donde se halle el observador y lo elimina 
	const WorldObserverListIt It (std::find(m_Observers.Observers.begin(),
									        m_Observers.Observers.end(),
									        pObserver));
    ASSERT((It != m_Observers.Observers.end()) != 0);
    m_Observers.Observers.erase(It);
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notifica a los observadores del universo de juego por un determinado
//   suceso.
// Parametros:
// - NotifyType. Tipo de suceso.
// - udParam. Parametro de acompañamiento.
// Devuelve:
// Notas:
// - Durante el recorrido no se notificara a aquellos clientes que hayan
//   solicitado eliminarse como observadores durante la ejecucion de
//   este metodo
///////////////////////////////////////////////////////////////////////////////
void 
CWorld::ObserversNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
					    const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Activa el flag de notificacion
  m_Observers.bInNotify = true;

  // Recorre la lista de observadores notificando del suceso
  WorldObserverListIt It(m_Observers.Observers.begin());
  for (; It != m_Observers.Observers.end(); ++It) {
	// ¿El cliente NO solicito eliminarse?
	const WorldObserverListIt RemoveIt(std::find(m_Observers.ObToRemove.begin(),
												 m_Observers.ObToRemove.end(),
												 *It));
	if (RemoveIt == m_Observers.ObToRemove.end()) {
	  // Si, luego se notifica
	  (*It)->WorldObserverNotify(NotifyType, udParam);
	}
  }

  // Desactiva flag de notificacion y atiende la peticiones externas
  m_Observers.bInNotify = false;

  // Peticiones de añadir observadores
  It = m_Observers.ObToAdd.begin();
  while (It != m_Observers.ObToAdd.end()) {
	AddObserver(*It);
	It = m_Observers.ObToAdd.erase(It);
  }

  // Peticiones de eliminar observadores
  It = m_Observers.ObToRemove.begin();
  while (It != m_Observers.ObToRemove.end()) {
	RemoveObserver(*It);
	It = m_Observers.ObToRemove.erase(It);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece estado de la pausa SOLO si hay un juego activo
// Parametros:
// - bPause. Flag para establecer / quitar la pausa
// Devuelve:
// Notas:
// - Por el momento no se considerara el pausar la musica de fondo
///////////////////////////////////////////////////////////////////////////////
void 
CWorld::SetPause(const bool bPause)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Hay juego activo?
  if (m_Area.IsInitOk() &&
	  m_Area.IsAreaLoaded()) {	
	// Se pausa la alarma para el control horario
	if (m_TimeInfo.hAlarm) {
	  m_pAlarmManager->SetPause(m_TimeInfo.hAlarm, bPause);
	}

	// Se pausa alarma idle
	if (m_IdleEventInfo.hIdleAlarm) {
	  m_pAlarmManager->SetPause(m_IdleEventInfo.hIdleAlarm, bPause);
	}
	
	// Se actua sobre el sonido ambiente
	if (m_AmbientWAVSound.hWAVAmbientSound) {
	  if (bPause) {
		m_pAudioSys->PauseWAVSound(m_AmbientWAVSound.hWAVAmbientSound);
	  } else {
  		m_pAudioSys->PlayWAVSound(m_AmbientWAVSound.hWAVAmbientSound,
								  AudioDefs::WAV_PLAY_LOOP);
	  }
	}

	// Establece flag de pausa
	m_bIsInPause = bPause;

	// Traslada la peticion al motor isometrico
	m_IsoEngine.SetPause(bPause);

	// Se traslada peticion a los sonidos anonimos
	WAVSoundMapIt It(m_WAVSounds.begin());
	for (; It != m_WAVSounds.end(); ++It) {
	  // Se actua en subsistema de audio
	  if (bPause) {
		// Pausa
		m_pAudioSys->PauseWAVSound(It->second);		
	  } else {
		// Reanudacion
		m_pAudioSys->PlayWAVSound(It->second, AudioDefs::WAV_PLAY_NORMAL);
	  }

	  // Se actua en subsistema de alarmas
	  m_pAlarmManager->SetPause(It->first, bPause);
	}

	// Se notifica a los observers
	if (bPause) {
	  ObserversNotify(WorldObserverDefs::PAUSE_ON, 0);
	} else {
	  ObserversNotify(WorldObserverDefs::PAUSE_OFF, 0);
	}  
  }
}  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera todos los archivos .cbb que sean temporales de un area.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWorld::RemoveTmpDir(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Tipos
  typedef std::list<std::string> FileNameList;
  typedef FileNameList::iterator FileNameListIt;
  
  // Se obtiene la lista de archivos cbb
  FileNameList List;
  m_pFileSys->GetFileNamesFromPath("tmp\\*.cbb", List);

  // Se recorre y aquellos de tipo area tmp, se borra
  FileNameListIt It(List.begin());
  for (; It != List.end(); ++It) {
	// ¿Es el archivo de area temporal?
	const std::string szFileWithPath("tmp\\" + *It);
	const FileDefs::FileHandle hFile = m_pFileSys->Open(szFileWithPath);
	byte ubFileType;
	m_pFileSys->Read(hFile, (sbyte *)(&ubFileType), sizeof(byte), 0); 
	m_pFileSys->Close(hFile);
	if (ubFileType == CBDefs::CBBAreaTmpFile) {
	  // Si, se borra	  
	  remove(szFileWithPath.c_str());
	}
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de comprobar cuantos items existen en una determinada celda.
// Parametros:
// - TilePos. Posicion donde buscar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
word
CWorld::GetNumItemsAt(const AreaDefs::sTilePos& TilePos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si area cargada
  ASSERT(m_Area.IsInitOk());
  ASSERT(m_Area.IsAreaLoaded());

  // Se obtiene iterador a celda
  ASSERT(m_Area.IsCellValid(TilePos));
  word uwNumItems = 0;
  CCellIterator It;
  if (It.Init(&m_Area, TilePos)) {
	// Se buscan items
	It.SetAtFront();
	for (; It.IsItValid(); It.Next()) {
	  if (GetEntityType(It.GetWorldEntity()) == RulesDefs::ITEM) {
		++uwNumItems;
	  }
	}
  }

  // Se retorna resultado
  return uwNumItems;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de localizar el handle al item que se halla en un determinado
//   tile y en una determinada posicion dentro de la secuencia.
// Parametros:
// - TilePos. Posicion donde buscar.
// - uwLocation. Posicion del item.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
AreaDefs::EntHandle
CWorld::GetItemAt(const AreaDefs::sTilePos& TilePos,
				  const word uwLocation)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si area cargada
  ASSERT(m_Area.IsInitOk());
  ASSERT(m_Area.IsAreaLoaded());

  // Se obtiene iterador a celda
  ASSERT(m_Area.IsCellValid(TilePos));
  AreaDefs::EntHandle hItem = 0;
  CCellIterator It;
  if (It.Init(&m_Area, TilePos)) {
	// Se buscan items
	It.SetAtFront();
	word uwNumItems = 0;  
	for (; It.IsItValid(); It.Next()) {
	  if (GetEntityType(It.GetWorldEntity()) == RulesDefs::ITEM) {
		if (uwLocation == ++uwNumItems) {
		  // Posicion localizada
		  hItem = It.GetWorldEntity();
		  break;
		}
	  }
	}
  }

  // Se retorna resultado
  return hItem;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion por parte de los scripts lanzados desde esta clase.
// Parametros:
// - Notify. Tipo de notificacion.
// - ScriptEvent. Evento que lo lanza.
// - udParams. Parametros asociados.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CWorld::ScriptNotify(const RulesDefs::eScriptEvents& ScriptEvent,
					 const ScriptClientDefs::eScriptNotify& Notify,
					 const dword udParams)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Filtrado de evento que manda notificacion
  switch(ScriptEvent) {
	case RulesDefs::SE_ONSTARTGAME: {
	  // Sin comportamiento por defecto
	} break;

	case RulesDefs::SE_ONNEWHOUR: {
	  // Sin comportamiento por defecto
	} break;

  	case RulesDefs::SE_ONWORLDIDLE: {
  	  // ¿NO hubo ERRORES ejecutando el script?
	  if (ScriptClientDefs::SN_SCRIPT_ERROR != Notify) {
		// ¿Hay tiempo asociado?
		if (m_IdleEventInfo.fTime != 0.0f) {
		  // Si, se instala alarma
		  m_IdleEventInfo.hIdleAlarm = m_pAlarmManager->InstallTimeAlarm(this, 
																		 m_IdleEventInfo.fTime);
		  ASSERT(m_IdleEventInfo.hIdleAlarm);
		}
	  }	  
	} break;

	case RulesDefs::SE_ONENTERINAREA: {
	  // Sin comportamiento por defecto
	} break;

	case RulesDefs::SE_ONENTITYCREATED: {
	  // Sin comportamiento por defecto	  
	} break;

	default:
	  ASSERT(false);
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia un tiempo de ejecucion para el evento Idle. En caso de pasar un
//   tiempo inferior a 1 segundo, se entendera que se deseara desinstalar la
//   posible alarma activa.
// Parametros:
// - fTime. Tiempo asociado a la alarma.
// Devuelve:
// Notas:
// - Si ya hubiera un tiempo asociado, se cambiara la alarma antigua
//   desinstalandose primero
//////////////////////////////////////////////////////////////////////////////
void 
CWorld::SetIdleEventTime(const float fTime)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Se desea desinstalar?
  if (fTime < 1.0f) {
	// ¿Hay alarma asociada?
	if (m_IdleEventInfo.hIdleAlarm) {
	  // Si, se desinstala alarma
	  m_pAlarmManager->UninstallAlarm(m_IdleEventInfo.hIdleAlarm);
	  m_IdleEventInfo.hIdleAlarm = 0;
	}

	// Se establece tiempo
	m_IdleEventInfo.fTime = 0.0f;
  } else {
	// No, ¿El tiempo es distinto y hay alarma asociada?
	if (m_IdleEventInfo.fTime != fTime &&
		m_IdleEventInfo.hIdleAlarm) {
	  // Si, se desinstala alarma
	  m_pAlarmManager->UninstallAlarm(m_IdleEventInfo.hIdleAlarm);
	  m_IdleEventInfo.hIdleAlarm = 0;
	}

	// Se instala nueva alarma y toma tiempo
	m_IdleEventInfo.hIdleAlarm = m_pAlarmManager->InstallTimeAlarm(this, fTime);
	ASSERT(m_IdleEventInfo.hIdleAlarm);
	m_IdleEventInfo.fTime = fTime;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Propagara la llamada de creacion de una criatura de forma dinamica al area 
//   del juego. El item sera depositado sobre un Floor.
// Parametros:
// - TilePos. Posicion donde crear la entidad.
// - szTag. Tag asociado a la entidad.
// - szProfile. Perfil a partir del cual crear la entidad.
// - Elevation. Elevacion asociada a la criatura
// - Light. Posible luz asociada.
// - bTempCriature. Flag indicativo de si la criatura es temporal o no.
// Devuelve:
// - Si se pudo crear, el handle a la entidad. En caso contrario handle nulo.
// Notas:
///////////////////////////////////////////////////////////////////////////////
AreaDefs::EntHandle 
CWorld::CreateCriature(const std::string& szProfile,
					   const AreaDefs::sTilePos& TilePos,
   					   const std::string& szTag,
					   const RulesDefs::Elevation& Elevation,
					   const GraphDefs::Light& Light,
					   const bool bTempCriature) 
{
	ASSERT(IsInitOk());	

  // Llama al area para crear la criatura
  const AreaDefs::EntHandle hCriature =  m_Area.CreateCriature(szProfile, 
															   TilePos, 
															   szTag, 								 
															   Elevation, 
															   Light, 
															   bTempCriature);
  if (hCriature) {	
    // Si, se lanza evento de creacion de entidad
	// Nota: A la vez se establecera el flag de visibilidad sobre escenario
	CWorldEntity* const pEntity = GetWorldEntity(hCriature);
	ASSERT(pEntity);
	pEntity->VisibleInScreen(m_IsoEngine.IsTilePosVisible(pEntity->GetTilePos()));
	m_pVMachine->OnEntityCreated(pEntity, 
								 pEntity->GetScriptEvent(RulesDefs::SE_ONENTITYCREATED),
								 hCriature);
  }

  // Retorna
  return hCriature;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Propagara la llamada de creacion de un item de forma dinamica al area 
//   del juego. El item sera depositado sobre un Floor.
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
CWorld::CreateItem(const std::string& szProfile,
			   	   const AreaDefs::sTilePos& TilePos,
				   const std::string& szTag,			
				   const GraphDefs::Light& Light) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Se pudo crear entidad?
  const AreaDefs::EntHandle hEntity = m_Area.CreateItem(szProfile,
	                                                    TilePos,
													    szTag,													    
													    Light);
  if (hEntity) {
	// Si, se lanza evento de creacion de entidad
	// Nota: A la vez se establecera el flag de visibilidad sobre escenario	
	CWorldEntity* const pEntity = GetWorldEntity(hEntity);
	ASSERT(pEntity);
	pEntity->VisibleInScreen(m_IsoEngine.IsTilePosVisible(pEntity->GetTilePos()));
	m_pVMachine->OnEntityCreated(pEntity, 
								 pEntity->GetScriptEvent(RulesDefs::SE_ONENTITYCREATED),
								 hEntity);
  }

  // Retorna
  return hEntity;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Propagara la llamada de creacion de un item de forma dinamica al area 
//   del juego. El item sera asociado a una entidad contenedora.
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
CWorld::CreateItem(const std::string& szProfile,
				   const AreaDefs::EntHandle& hOwner,				   				
				   const std::string& szTag,
				   const GraphDefs::Light& Light) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Se pudo crear entidad?
  const AreaDefs::EntHandle hEntity = m_Area.CreateItem(szProfile,
	                                                    hOwner,
													    szTag,													    
													    Light);
  if (hEntity) {
	// Si, se lanza evento de creacion de entidad
	CWorldEntity* const pEntity = GetWorldEntity(hEntity);
	ASSERT(pEntity);
	m_pVMachine->OnEntityCreated(pEntity, 
								 pEntity->GetScriptEvent(RulesDefs::SE_ONENTITYCREATED),
								 hEntity);
  }

  // Retorna
  return hEntity;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Propagara la llamada de creacion de una pared de forma dinamica al area 
//   del juego.
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
CWorld::CreateWall(const std::string& szProfile,				   
				   const AreaDefs::sTilePos& TilePos,
				   const std::string& szTag,
				   const RulesDefs::Elevation& Elevation,
				   const GraphDefs::Light& Light,
				   const bool bBlockAccessFlag) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Se pudo crear entidad?
  const AreaDefs::EntHandle hEntity = m_Area.CreateWall(szProfile,
	                                                    TilePos,
													    szTag,													    
													    Elevation,
													    Light,
													    bBlockAccessFlag);
  if (hEntity) {
	// Si, se lanza evento de creacion de entidad
	// Nota: A la vez se establecera el flag de visibilidad sobre escenario	
	CWorldEntity* const pEntity = GetWorldEntity(hEntity);
	ASSERT(pEntity);
	pEntity->VisibleInScreen(m_IsoEngine.IsTilePosVisible(pEntity->GetTilePos()));
	m_pVMachine->OnEntityCreated(pEntity, 
								 pEntity->GetScriptEvent(RulesDefs::SE_ONENTITYCREATED),
								 hEntity);
  }

  // Retorna
  return hEntity;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Propagara la llamada de creacion de un objeto de escenario de forma 
//   dinamica al area del juego.
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
CWorld::CreateSceneObj(const std::string& szProfile,					   
					   const AreaDefs::sTilePos& TilePos,
					   const std::string& szTag,
					   const RulesDefs::Elevation& Elevation,
					   const GraphDefs::Light& Light) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Se pudo crear entidad?
  const AreaDefs::EntHandle hEntity = m_Area.CreateSceneObj(szProfile, 
	                                                        TilePos, 
														    szTag, 															
															Elevation, 
															Light);
  if (hEntity) {
	// Si, se lanza evento de creacion de entidad
	// Nota: A la vez se establecera el flag de visibilidad sobre escenario	
	CWorldEntity* const pEntity = GetWorldEntity(hEntity);
	ASSERT(pEntity);
	pEntity->VisibleInScreen(m_IsoEngine.IsTilePosVisible(pEntity->GetTilePos()));
	m_pVMachine->OnEntityCreated(pEntity, 
								 pEntity->GetScriptEvent(RulesDefs::SE_ONENTITYCREATED), 
								 hEntity);
  }

  // Retorna
  return hEntity;
}  

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la destruccion, de forma dinamica, de una entidad. Antes de
//   realizar la destruccion efectiva, realizara una notificacion para que
//   todos los observadores del universo de juego conozcan que una entidad
//   va a destruirse. El fin sera el de invalidad el uso de la entidad desde
//   cualquier punto del codigo en donde se este usando.
// - Se podran destruir las entidades que no sean Floor, Roof y Jugador.
// Parametros:
// - hEntity. Handle a la entidad a destruir.
// Devuelve:
// - Si se pudo destruir true, en caso contrario false.
// Notas:
// - En el caso de destruir una criatura la notificacion tambien se realizara
//   a nivel de observadores de la criatura, pues sera necesario para 
//   desvincular a todos los observadores que se hayan instalado para
//   conocer el rastro de las criaturas que actualmente estan en su rango
//////////////////////////////////////////////////////////////////////////////
bool 
CWorld::DestroyEntity(const AreaDefs::EntHandle& hEntity)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿La entidad pasada existe?
  if (m_Area.GetWorldEntity(hEntity)) {
	// ¿La entidad pasada se puede destruir?
	const RulesDefs::eEntityType Type = m_Area.GetEntityType(hEntity);
	if (Type == RulesDefs::SCENE_OBJ ||
		Type == RulesDefs::CRIATURE ||
		Type == RulesDefs::ITEM ||
		Type == RulesDefs::WALL) {
	  // Si, se procede a notificar que se va a realizar la destruccion
	  // Nota: En el caso de las criaturas, existe otra notificacion adicional
	  // en su metodo End de finalizacion
	  ObserversNotify(WorldObserverDefs::ENTITY_DESTROY, hEntity);

	  // Se pasa la responsabilidad a CArea para la destruccion y retorna
	  m_Area.DestroyEntity(hEntity);
	  return true;
	}
  }

  // No se pudo destruir entidad
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Pausa la hora del universo de juego SOLO si el universo de juego no se
//   halla pausado.
// Parametros:
// - bPause. Flag de establecimiento / eliminacion de la pausa.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CWorld::SetWorldTimePause(const bool bPause)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿El universo de juego NO esta pausado?
  if (!IsInPause()) {
	// ¿Hay alarma temporal asociada? Y
	// ¿No se haya en pausa?	
	if (m_TimeInfo.hAlarm) {
	  // Se establece accion
	  ASSERT(m_pAlarmManager);  
	  m_pAlarmManager->SetPause(m_TimeInfo.hAlarm, bPause);
	  m_bWorldTimePause = bPause;
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el tiempo universal se halla o no en pausa
// Parametros:
// Devuelve:
// - Si se halla en pausa true, en caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CWorld::IsWorldTimeInPause(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Retorna resultado
  ASSERT(m_pAlarmManager);
  return m_pAlarmManager->IsInPause(m_TimeInfo.hAlarm);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ordena la reproduccion de un sonido ambiente anonimo
// Parametros:
// - szSound. Sonido a reproducir
// Devuelve:
// - Si se pudo cargar y reproducir el sonido true, en caso contrario false.
// Notas:
// - La reproduccion de los sonidos WAV anonimos SIEMPRE sera NO ciclica
//////////////////////////////////////////////////////////////////////////////
bool 
CWorld::PlayWAVSound(const std::string& szSound)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Nombre correcto?
  if (!szSound.empty()) {
	// ¿Se puede cargar el sonido?
	const ResDefs::WAVSoundHandle hWAVSound = m_pResManager->WAVSoundRegister(szSound);
	if (hWAVSound) {
	  // Si, se ordenara su reproduccion y se vinculara una alarma	  
	  m_pAudioSys->PlayWAVSound(hWAVSound, AudioDefs::WAV_PLAY_NORMAL);
	  const AlarmDefs::AlarmHandle hWAVAlarm = m_pAlarmManager->InstallWAVAlarm(this, hWAVSound);
	  ASSERT(hWAVAlarm);
	  
	  // Se inserta en el map el handle del sonido y su alarma asociada y retorna
	  m_WAVSounds.insert(WAVSoundMapValType(hWAVAlarm, hWAVSound));
	  return true;
	}
  }  

  // Hubo problemas
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se recorre la lista de sonidos anonimos activos y se liberan
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CWorld::ReleaseWAVSounds(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Procede a recorrer y liberarlos
  WAVSoundMapIt It(m_WAVSounds.begin());
  for (; It != m_WAVSounds.end(); It = m_WAVSounds.erase(It)) {
	// Se libera recurso y se desinstala alarma
	m_pResManager->ReleaseWAVSound(It->second);
	m_pAlarmManager->UninstallAlarm(It->first);
  } 
}