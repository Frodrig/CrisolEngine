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
// CAudioSystem.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clase:
// - CAudioSystem
//
// Descripcion:
// - Subistema de audio. Desde aqui se controlara todo el trabajo relacionado
//   con la inicializacion de la tarjeta de sonido y el proceso de 
//   reproduccion de ficheros musicales.
// - A la hora de reproducir un sonido de tipo WAV, solo lo hara si dicho 
//   sonido NO SE HALLA reproduciendose.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CAUDIOSYSTEM_H_
#define _CAUDIOSYSTEM_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _DXSOUND_H_
#include "DXWrapper\\DXSound.h"
#define _DXSOUND_H_
#endif
#ifndef _DXDMMANAGER_H_
#include "DXWrapper\\DXDMManager.h"
#define _DXDMMANAGER_H_
#endif
#ifndef _DXDMSEGMENT_H_
#define _DXDMSEGMENT_H_
#include "DXWrapper\\DXDMSegment.h"
#endif
#ifndef _ICAUDIOSYSTEM_H_
#include "iCAudioSystem.h"
#endif
#ifndef _RESDEFS_H_
#include "ResDefs.h"
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif

// Defincion de clases / estructuras / espacios de nombres
struct iCResourceManager;

// Funcion que controla el bucle del fichero MIDI
static DWORD WINAPI tMIDIControl(LPVOID lpParams);

// Clase CAudioSystem
class CAudioSystem: public iCAudioSystem
{
private:
  // Estructuras
  struct sWAVSound { 
	// Enumerados
	enum eWAVAction {
	  // Codigo de accion a realizar sobre un fichero WAV
	  WAV_PLAY_NORMAL = 0x00, // Reproduccion normal (lineal)
	  WAV_PLAY_LOOP,		  // Reproduccion cíclica
	  WAV_STOP,               // Parada
	  WAV_PAUSE,              // Pausa
	  WAV_SETVOLUME           // Establece el volumen
	};
	// Estructura con la informacion de un WAV
	ResDefs::WAVSoundHandle hWAVSound; // Identificador del WAV
	eWAVAction              Action;    // Accion a realizar con el WAV
	dword                   udParam;   // Posibles parametros asociados
	// Constructor
	sWAVSound(const ResDefs::WAVSoundHandle& ahWAVSound,
			  const eWAVAction& aAction,
			  const dword audParam = 0): hWAVSound(ahWAVSound),
										  Action(aAction),
										  udParam(audParam) { }
  };

private:
  // Tipos
  // Lista de sonidos WAV
  typedef std::list<sWAVSound>   WAVSoundList;
  typedef WAVSoundList::iterator WAVSoundListIt;

private:
  // Estructuras
  struct sMIDISound { 
	// Estructura con la informacion MIDI
    DXDMSegment				 MIDISeg;        // Segmento MIDI    
    HANDLE					 hThread;        // Handle al thread del MIDI    
    dword					 udIDThread;     // ID del thread del MIDI    
	std::string				 szMIDIFileName; // Nombre del sonido MIDI actual
	AudioDefs::eMIDIPlayMode MIDIPlayMode;   // Modo de reproduccion
  };  

  // Vbles de miembro
private:
  // Instancia singlenton
  static CAudioSystem* m_pAudioSystem; // Unica instancia a la clase

  // Instancias a otros subsistemas
  iCResourceManager* m_pResManager; // Subsistema de recursos
  
  // Instancias principales al wrapper de DX
  DXSound     m_DXSound;     // Objeto DirectSound
  DXDMManager m_DXDMManager; // Performance y Loader para DirectMusic

  // Vbles para el control de la reproduccion
  WAVSoundList m_WAVList;   // Lista de peticiones WAV
  sMIDISound   m_MIDISound; // Datos relativos al MIDI a sonar

  // Resto de vbles de miembro   
  bool m_bIsInitOk; // ¿Clase inicializada correctamente?   
  
protected:
  // Constructor / Destructor
  CAudioSystem(void): m_pResManager(NULL),
					  m_bIsInitOk(false) { }
public:
  ~CAudioSystem(void) { 
    End();
  }               
  
public:
  // Protocolo de inicio y fin de instancia
  bool Init(const byte ubFrequency,
			const byte ubNumChannels,
			const byte ubBpm);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }
private:
  // Metodos de apoyo
  bool InitDirectSound(const byte ubFrequency,
					   const byte ubNumChannels,
					   const byte ubBpm);
  bool InitDirectMusic(void);
  
public:
  // Obtencion y destruccion de la instancia singlenton
  static inline CAudioSystem* const GetInstance(void) {
    if (!m_pAudioSystem) {
      m_pAudioSystem = new CAudioSystem; 
      ASSERT(m_pAudioSystem)
    }
    return m_pAudioSystem;
  } 
  static inline DestroyInstance(void) {
    if (m_pAudioSystem) {
      delete m_pAudioSystem;
      m_pAudioSystem = NULL;
    }
  }

public:
  // Actualizacion
  inline void Update(void) {	  
	ASSERT(IsInitOk());
	// Se actualiza WAV y MIDI
	UpdateWAVSounds();
  }
private:
  // Metodos de apoyo
  void UpdateWAVSounds(void);

public:
  // iCAudioSystem / Operaciones sobre ficheros WAV
  void PlayWAVSound(const ResDefs::WAVSoundHandle& hWAVSound, 
					const AudioDefs::eWAVPlayMode& Mode);
  void StopWAVSound(const ResDefs::WAVSoundHandle& hWAVSound);
  void PauseWAVSound(const ResDefs::WAVSoundHandle& hWAVSound);
  void SetWAVSoundVolume(const ResDefs::WAVSoundHandle& hWAVSound,
						 const AudioDefs::eWAVVolume& Volume);
  bool IsWAVSoundPlaying(const ResDefs::WAVSoundHandle& hWAVSound);

public:
  // iCAudioSystem / Operaciones sobre ficheros MIDI
  void PlayMIDIMusic(const std::string& szFileName, 
					 const AudioDefs::eMIDIPlayMode& Mode = AudioDefs::MIDI_PLAY_LOOP);
  bool GetMIDIInfo(std::string& szMIDIFileName,
				   AudioDefs::eMIDIPlayMode& MIDIPlayMode);
  void StopMIDIMusic(void) {
	ASSERT(IsInitOk());
	// Detiene el sonido MIDI en reproduccion	
	QuitMIDIMusic();
  }  
  bool IsPlayingMIDIMusic(void) {
	ASSERT(IsInitOk());
	// Retorna flag
	return m_MIDISound.MIDISeg.IsPlaying();
  }
private:
  // Metodos de apoyo
  void QuitMIDIMusic(void);
  void DestroyMIDIControlThread(void);
  void InstallMIDIControlThread(void);

public:
  // iCAudioSystem / Operaciones de obtencion de informacion
  DXSound* const GetDXSound(void) { 
	ASSERT(IsInitOk());
	// Se retorna objeto
	return &m_DXSound; 
  }
};

#endif // ~ CAudioSystem
