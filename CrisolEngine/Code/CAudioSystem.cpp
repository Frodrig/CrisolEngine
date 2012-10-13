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
// CAudioSystem.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CAudioSystem.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CAudioSystem.h"

#include "DXWrapper\\DXDSWAVSound.h"
#include "iCLogger.h"
#include "iCResourceManager.h"
#include "SYSEngine.h"

// Inicializacion de la unica instancia singlenton
CAudioSystem* CAudioSystem::m_pAudioSystem = NULL;

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el modulo de audio. En la inicializacion solicitara las vbles
//   de miembro para poner a punto el objeto DirectSound. Con el objeto 
//   DirectSound tambien inicializaria la instancia DXDMManager.
// Parametros:
// - ubFrequency. Frecuencia de inicializacion: 11, 22 y 44.
// - ubNumChannels. Numero de canales a utilizar: 1 y 2.
// - ubBmp. Bits por muestra: 8 y 16.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - En caso de que el modulo ya estuviera inicializado no se dejara
//   realizar la operacion de reinicializacion. Se debera de realizar desde
//   exterior.
///////////////////////////////////////////////////////////////////////////////
bool 
CAudioSystem::Init(const byte ubFrequency,
				   const byte ubNumChannels,
				   const byte ubBpm)
{
  // ¿Ya esta inicializada la instancia?
  if (IsInitOk()) { 
	return false; 
  }
  
  #ifdef ENGINE_TRACE  
	SYSEngine::GetLogger()->Write("CAudioSystem::Init> Inicializando subsistema de Audio.\n");
  #endif

  // Obtiene instancias a otros subsistemas
  m_pResManager = SYSEngine::GetResourceManager();
  ASSERT(m_pResManager);

  // Se inicializa DirectSound
  if (!InitDirectSound(ubFrequency, ubNumChannels, ubBpm)) { 
	return false; 
  }

  // Se inicializa DirectMusic
  if (!InitDirectMusic()) { 
	return false; 
  }
      
  // Todo correcto
  #ifdef ENGINE_TRACE  
    SYSEngine::GetLogger()->Write("                  | Ok.\n");
  #endif
  m_bIsInitOk = true;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa DirectSound.
// Parametros:
// - ubFrequency. Frecuencia de inicializacion: 11, 22 y 44.
// - ubNumChannels. Numero de canales a utilizar: 1 y 2.
// - ubBmp. Bits por muestra: 8 y 16.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CAudioSystem::InitDirectSound(const byte ubFrequency,
							  const byte ubNumChannels,
							  const byte ubBpm)
{  
  // Inicializacion DirectSound  
  if (!m_DXSound.Create(SYSEngine::GethWnd(), 
					    DXDSDefs::DXDSFrequency(ubFrequency),
						DXDSDefs::DXDSChannel(ubNumChannels),
						DXDSDefs::DXDSBPM(ubBpm))) { 
	// Problemas
    // Traza
	#ifdef ENGINE_TRACE  
	  SYSEngine::GetLogger()->Write("                    | Problemas en DXSound al crearse.\n");
	  SYSEngine::GetLogger()->Write("                    | Código del error: %s.\n",m_DXSound.GetLastStrCode().c_str());
	#endif
  return false;
  }

  // Todo correcto
  #ifdef ENGINE_TRACE  
	SYSEngine::GetLogger()->Write("                    | DXSound inicializado.\n");
  #endif
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa DirectMusic.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CAudioSystem::InitDirectMusic(void)
{
  // Inicializacion DirectMusic  
  if (!m_DXDMManager.Create(SYSEngine::GethWnd(), 
							&m_DXSound)) { 
	// Problemas
    // Traza
	#ifdef ENGINE_TRACE  
	  SYSEngine::GetLogger()->Write("                    | Problemas en DXDMManager al crearse.\n");
	  SYSEngine::GetLogger()->Write("                    | Código del error: %s.\n",m_DXDMManager.GetLastStrCode().c_str());
	#endif
  }  

  // Creacion del segmento  
  if (!m_MIDISound.MIDISeg.Create(&m_DXDMManager)) { 
	// Problemas
	// Traza
	#ifdef ENGINE_TRACE  
	  SYSEngine::GetLogger()->Write("                   | Problemas al crearse el segmento.\n");
	  SYSEngine::GetLogger()->Write("                   | Código del error: %s.\n", m_MIDISound.MIDISeg.GetLastStrCode().c_str());
	#endif
  }

  // Se inicializan resto de vbles de miembro
  m_MIDISound.hThread = NULL;

  // Todo correcto
  #ifdef ENGINE_TRACE  
	SYSEngine::GetLogger()->Write("                    | DXMusic inicializado.\n");
  #endif
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia al modulo liberando todos los recursos.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAudioSystem::End(void)
{
  if (IsInitOk()) {
    #ifdef ENGINE_TRACE  
	  SYSEngine::GetLogger()->Write("CAudioSystem::End> Finalizando subsistema de audio.\n");	
    #endif
	
    // Se quita la posible musica y segmento actual
	QuitMIDIMusic();
	m_MIDISound.MIDISeg.Destroy();

	// Se vacia lista de sonidos WAV
	m_WAVList.clear();

	// Se destruyen objetos DXSound	
    m_DXDMManager.Destroy();
    m_DXSound.Destroy();
	
    // Se baja el flag
	#ifdef ENGINE_TRACE  
	  SYSEngine::GetLogger()->Write("                 | Ok.\n");	
    #endif
    m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo de apoyo a Update, realiza la coordinacion de las peticiones
//   sobre ficheros de sonido WAV.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAudioSystem::UpdateWAVSounds(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
       
  // Procede a realizar las peticiones sobre ficheros WAV
  WAVSoundListIt It(m_WAVList.begin());
  for (; It != m_WAVList.end(); It = m_WAVList.erase(It)) {
	// Se toma la instancia al sonido WAV	
	ASSERT(It->hWAVSound);	
	DXDSWAVSound* const pDXDSWAVSound = m_pResManager->GetWAVSound(It->hWAVSound);
	
	// Si el sonido ya no esta registrado, se pasa a otra peticion
	// Nota: Esto podra suceder cuando una instancia haya hecho una peticion
	// de tratamiento sobre un sonido antes de ser destruida
	if (pDXDSWAVSound) {	  
	  // Operaciones
	  switch(It->Action) {
		case sWAVSound::WAV_PLAY_LOOP: {
		  // Reproduccion ciclica		  
    	  pDXDSWAVSound->Play(DSBPLAY_LOOPING);		
		} break;

		case sWAVSound::WAV_PLAY_NORMAL: {
		  // Reproduccion lineal
		  pDXDSWAVSound->Play();		
		} break;

		case sWAVSound::WAV_STOP: {
		  // Se detiene la reproduccion, regresando el lector al principio
		  pDXDSWAVSound->Stop();		
		} break;	

		case sWAVSound::WAV_PAUSE: {
		  // Se detiene la reproduccion, congelando el lector
		  pDXDSWAVSound->Stop(false);		
		} break;	

		case sWAVSound::WAV_SETVOLUME: {
		  // Se establece el volumen de sonido, segun lo recibido
		  const AudioDefs::eWAVVolume Volume = AudioDefs::eWAVVolume(It->udParam);
		  switch (Volume) {
			case AudioDefs::WAV_VOLUME_MIN: {
			  pDXDSWAVSound->SetVolume(DSBVOLUME_MIN);
			} break;

			case AudioDefs::WAV_VOLUME_NORMAL: {
			  pDXDSWAVSound->SetVolume();
			} break;

			case AudioDefs::WAV_VOLUME_MAX: {
			  pDXDSWAVSound->SetVolume(DSBVOLUME_MAX);
			} break;
		  }; // ~ switch		  
		} break;
	  }; // ~ switch		
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Registra el ID de un recurso WAV para reproducirlo desde Update.
// Parametros:
// - hWAVSound. Identificador del recurso WAV a reproducir.
// - Mode. Modo de reproduccion del recurso WAV.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAudioSystem::PlayWAVSound(const ResDefs::WAVSoundHandle& hWAVSound, 
						   const AudioDefs::eWAVPlayMode& Mode)
{
  // SOLO si modulo inicializado
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hWAVSound);

  // Introduce sonido WAV en lista, segun sea la peticion
  switch (Mode) {
    case AudioDefs::WAV_PLAY_NORMAL: {
  	  // Reproduccion normal
	  m_WAVList.push_back(sWAVSound(hWAVSound, sWAVSound::WAV_PLAY_NORMAL));  
	} break;

	case AudioDefs::WAV_PLAY_LOOP: {
	  // Reproduccion ciclica
	  m_WAVList.push_back(sWAVSound(hWAVSound, sWAVSound::WAV_PLAY_LOOP));  
	} break;
  }; // ~ switch	   
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Registra el ID de un recurso WAV para deternerlo en la actualizacion
// Parametros:
// - hWAVSound. Identificador del recurso WAV a reproducir.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAudioSystem::StopWAVSound(const ResDefs::WAVSoundHandle& hWAVSound)
{
  // SOLO si modulo inicializado
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hWAVSound);

  // Se registra la peticion
  m_WAVList.push_back(sWAVSound(hWAVSound, sWAVSound::WAV_STOP));  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Registra un sonido WAV para pausarlo. Un sonido pausado se volvera a
//   a reproducir con un Play.
// Parametros:
// - hWAVSound. Identificador del recurso WAV a reproducir.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAudioSystem::PauseWAVSound(const ResDefs::WAVSoundHandle& hWAVSound)
{
  // SOLO si modulo inicializado
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hWAVSound);

  // Se registra la peticion
  m_WAVList.push_back(sWAVSound(hWAVSound, sWAVSound::WAV_PAUSE));  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el volumen asociado a un sonido WAV.
// Parametros:
// - hWAVSound. Handle al sonido.
// - Volume. Volumen asociado a un sonido WAV, en caso de que el sonido wav no
//   se haya creado con el flag de control de sonido conveniente, la orden
//   no se atendera.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAudioSystem::SetWAVSoundVolume(const ResDefs::WAVSoundHandle& hWAVSound,
								const AudioDefs::eWAVVolume& Volume)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hWAVSound);

  // Se registra la peticion
  m_WAVList.push_back(sWAVSound(hWAVSound, sWAVSound::WAV_SETVOLUME, Volume));
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el sonido esta siendo reproducido.
// Parametros:
// - hWAVSound. Handle al recurso de sonido
// Devuelve:
// - Si esta siendo reproducido true. En caso contrario o en caso de no
//   hallarse el sonido, false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CAudioSystem::IsWAVSoundPlaying(const ResDefs::WAVSoundHandle& hWAVSound)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hWAVSound);

  // Obtiene instancia y retorna flag.
  DXDSWAVSound* const pDXDSWAVSound = m_pResManager->GetWAVSound(hWAVSound);
  if (pDXDSWAVSound) {
	return pDXDSWAVSound->IsPlaying();
  } else {
	return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Reproduce el sonido MIDI recibido de la forma indicada. La orden sera 
//   ejecutada de inmediato.
// Parametros:
// - szFileName. Nombre del fichero.
// - Mode. Modo de reproduccion.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAudioSystem::PlayMIDIMusic(const std::string& szFileName, 
						    const AudioDefs::eMIDIPlayMode& Mode)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Hay un thread de sonido activo?
  if (m_MIDISound.hThread) {
	// Se carga MIDI y se reproduce con un tiempo elapsed
	if (m_MIDISound.MIDISeg.LoadMIDISegment(szFileName)) {
	  // ¿Es reproduccion NO ciclica?
	  if (AudioDefs::MIDI_PLAY_LOOP == Mode) {
		// Se desinstala posible thread instalado
		DestroyMIDIControlThread();
	  } else {
		// Se instala thread
		InstallMIDIControlThread();
	  }
	  // Se reproduce dejando un espacio de tiempo con respecto a la pista actual
	  m_MIDISound.MIDISeg.PlayElapsed();	  
	  // Se guarda la informacion
	  m_MIDISound.MIDIPlayMode = Mode;
	  m_MIDISound.szMIDIFileName = szFileName;
	}  	
  } else {
	// No, se carga MIDI, se crea Thread y se reproduce al instante
	if (m_MIDISound.MIDISeg.LoadMIDISegment(szFileName)) {
	  // ¿La reproduccion es ciclica?
	  if (AudioDefs::MIDI_PLAY_LOOP == Mode) {
		// Se instala thread
		InstallMIDIControlThread();
	  }
	  // Se ordena reproduccion inmediata
	  m_MIDISound.MIDISeg.PlayNow();
	  // Se guarda la informacion
	  m_MIDISound.MIDIPlayMode = Mode;
	  m_MIDISound.szMIDIFileName = szFileName;
	}	
  }    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene informacion sobre el estado asociado a la reproduccion MIDI y la
//   depositara por parametro.
// Parametros:
// - szMIDIFileName. String donde depositar el nombre del fichero MIDI a
//   reproducir.
// - MIDIPlayMode. Modo de reproduccion del fichero MIDI actual.
// Devuelve:
// - En caso de que el fichero MIDI se este reproducciendo true. En caso
//   contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CAudioSystem::GetMIDIInfo(std::string& szMIDIFileName,
						  AudioDefs::eMIDIPlayMode& MIDIPlayMode) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Esta sonando musica?
  if (m_MIDISound.MIDISeg.IsPlaying()) {
    // Si, se deposita informacion y se retorna
    szMIDIFileName = m_MIDISound.szMIDIFileName;
    MIDIPlayMode = m_MIDISound.MIDIPlayMode;
    return true;
  } else {
    // No, se retorna
    return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Detendra un posible sonido MIDI activo y liberara de memoria el hilo 
//   encargado de controlarlo.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAudioSystem::QuitMIDIMusic(void)
{
  // SOLO si modulo inicializado
  ASSERT(IsInitOk());

  // Detiene musica MIDI y desinstala posible thread asociado
  m_MIDISound.MIDISeg.Stop();
  DestroyMIDIControlThread();

  // Se desvincula cualquier nombre de sonido MIDI
  m_MIDISound.szMIDIFileName = "";
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desinstala posible thread de control de musica MIDI instalado.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAudioSystem::DestroyMIDIControlThread(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Hay thread instalado?
  if (m_MIDISound.hThread) {
	// Si, se desinstala
    TerminateThread(m_MIDISound.hThread, 0);
    m_MIDISound.hThread = NULL;
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Instala un thread para el control de un sonido MIDI. Sera necesario
//   siempre que estemos trabajando con reproduccion ciclica.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAudioSystem::InstallMIDIControlThread(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿NO hay thread instalado?
  if (NULL == m_MIDISound.hThread) {
	// No, se creara el hilo para el control del sonido
	m_MIDISound.hThread = CreateThread(NULL,       
									   0,          
									   &tMIDIControl,
									   &m_MIDISound.MIDISeg,
									   0,           
									   &m_MIDISound.udIDThread);
	ASSERT(m_MIDISound.hThread);
	SetThreadPriority(m_MIDISound.hThread, THREAD_PRIORITY_LOWEST);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Implementa la rutina que se encarga de controlar el loop de un segmento
//   MIDI. Recordemos que esta rutina estara localizada en un hilo de ejecucion.
// Parametros:
// - lpParams. Puntero void a una instancia DXDMSegment.
// Devuelve:
// - En caso de que existiera algun problema 0. Si todo va bien un valor >= 1.
// Notas:
// - El flag se utiliza para evitar procesar varios eventos de fin de
//   reproduccion de archivo MIDI.
///////////////////////////////////////////////////////////////////////////////
DWORD 
WINAPI tMIDIControl(LPVOID lpParams)
{
  // Inicializaciones
  ASSERT(lpParams);
  DXDMSegment* pMIDISeg = (DXDMSegment*)(lpParams);
  ASSERT(pMIDISeg);
  ASSERT(pMIDISeg->GetSegmentObj()); 
  DXDMManager* pManager = pMIDISeg->GetDXManager();
  ASSERT(pManager);
 
  // Comprobacion
  dword udNotif;
  bool bFlag = false;
  while(true) {
	// Espera por notificacion o a que pasen 100 ms
	WaitForSingleObject(pManager->GetHandleNotification(), 100);
    // ¿Hay alguna notificacion?	
    if (pManager->GetNotification(udNotif)) {
      // ¿Indica que el MIDI esta a punto de finalizar?
      if ((DMUS_NOTIFICATION_SEGALMOSTEND == udNotif) && 
		  !bFlag) {        
		// Indicamos que la musica vuelva a comenzar con un intervalo
		// de espera para lograr la transicion correcta
		pMIDISeg->PlayElapsed();
		bFlag = true;
      }
      else if (DMUS_NOTIFICATION_SEGSTART == udNotif && bFlag) {
		// Se levanta el flag para esperar nuevos eventos
		bFlag = false;
	  } // ~ else	
	} // ~ if
  } // ~ while
  
  // Todo correcto
  return 1;
}
