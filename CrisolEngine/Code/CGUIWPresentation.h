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
// CGUIWPresentation.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases
// - CGUIWPresentation.h
//
// Descripcion:
// - Clase encargada de representar la ventana de interfaz encargada de
//   gestionar las presentaciones.
// - Sera necesario recibir un cliente en la activacion para avisar de
//   que se ha finalizado el primer FadeOut. Esto sera vital cuando se
//   trabaje desde la activacion en MainInterfaz y PlayerProfile, siendo el
//   cliente el GUIManager siempre.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWPRESENTATION_H_
#define _CGUIWPRESENTATION_H_

// Cabecera
#ifndef _CGUIWINDOW_H_
#include "CGUIWindow.h"
#endif
#ifndef _CGUICANIMBITMAP_H_
#include "CGUICAnimBitmap.h"
#endif
#ifndef _CGUICSINGLETEXT_H_
#include "CGUICSingleText.h"
#endif
#ifndef _RESDEFS_H_
#include "ResDefs.h"
#endif
#ifndef _ICALARMCLIENT_H_
#include "iCAlarmClient.h"
#endif
#ifndef _AUDIODEFS_H_
#include "AudioDefs.h"
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif

// Definicion de clases / estructuras / espacios de nombres
struct iCAudioSystem;
struct iCResourceManager;
struct iCFileSystem;
struct iCGameDataBase;
struct iCAlarmManager;
class CCBTEngineParser;

// clase CGUIWPresentation
class CGUIWPresentation: public CGUIWindow,
						 public iCAlarmClient
{  
private:
  // Tipos
  typedef CGUIWindow Inherited; // Acceso a la clase base

private:  
  // Enumerados
  enum {	
	// Identificadores	
	ID_BACKIMG = 0x01, // Imagen de fondo
	ID_TEXT,	       // Identificadores para los textos
	ID_FADECMD         // Comando fade	
  }; 

  enum {
	// Fades posibles
	// Siempre se producira el siguiente ciclo:
	// * FadeOut inicial (solo si no se viene de MainMenu)
	// * NextSlide 
	// * FadeIn
	// - Peticion de NextSlide (Manual o Alarma) -> FadeOut
	// * NextSlide
	// * FadeIn
	// Ir a -
	ID_FADEIN = 0x01, // Fade In 
	ID_INITFADEOUT,   // Fade Out de inicio
	ID_FADEOUT,       // Fade Out normal
  };

private:
  // Estructuras forward
  struct sNText;

private:
  // Tipos
  // Lista para el mantenimiento de los textos activos
  typedef std::list<sNText*> TextList;
  typedef TextList::iterator TextListIt;

private:
  // Estructuras
  struct sBackImgInfo {
	// Info asociada a la imagen de fondo
	CGUICAnimBitmap AnimBackImg; // Imagen de fondo	
  };

  struct sMIDIMusicInfo {
	// Info asociada al fichero MIDI actual
	std::string szMIDIFileName; // Nombre del fichero de MIDI actual
	bool        bCyclicPlay;    // ¿Reproduccion ciclica?
  };

  struct sWAVSoundInfo {
	// Info asociada a los ficheros WAV
	ResDefs::WAVSoundHandle hWAVCyclic; // Sonido WAV ciclico
	ResDefs::WAVSoundHandle hWAVLinear; // Sonido WAV linear
  };

  struct sTextInfo {
	// Info asociada al texto activo
	TextList ActiveText; // Texto activo
  };

  struct sAutomaticSlidePass {
	// Info asociada al mecanismo de pase automatico de slides
	AlarmDefs::AlarmHandle hAlarm;      // Handle a la alarma
	float                  fMaxVisTime; // Tiempo maximo de visualizacion por slide
  };

  struct sActSlideFadeInfo {
	// Info asociada al FadeOut del slide actual
	// Nota: La velocidad del FadeIn sera obligatoria si el slide anterior
	// poseia FadeOut. En particular, el Slide 0 siempre debera de poseer 
	// un valor de velocidad para el FadeIn.
	GraphDefs::sRGBColor RGBFadeOut;     // Color del FadeOut
	word                 uwFadeOutSpeed; // Velocidad asociada al FdeOut
	word                 uwFadeInSpeed;  // Velocidad asociada al FadeIn
	bool                 bFadeOutActive; // ¿Esta activo para el slide actual?
  };

  struct sPrevMusic {
	// Guarda la informacion relativa al sonido MIDI y WAV (ambiente)
	// que estuviera reproduciendose antes de activar esta ventana
	bool				     bPrevMIDIMusic;       // ¿Habia sonido MIDI previo?
	std::string			     szMIDIFileName;       // Nombre archivo MIDI
	AudioDefs::eMIDIPlayMode MIDIPlayMode;         // Modo de reproduccion
	bool                     bAmbientWAVSound;     // ¿Sonido ambiente previo?
	std::string              szAmbientWAVFileName; // Sonido ambiente WAV
  };

private:
  // Instancias a distintos subsistemas
  iCAudioSystem*     m_pAudioSys;     // Subsistema de audio
  iCResourceManager* m_pResManager;   // Subsistema gestor de recursos
  iCFileSystem*      m_pFileSys;      // Subsistema de ficheros
  iCGameDataBase*    m_pGDBase;       // Subsistema de base de datos
  iCAlarmManager*    m_pAlarmManager; // Subsistema de alarmas

  // Info asociada al slide actual
  sword				  m_swActSlide;       // Index al slide actual
  sBackImgInfo		  m_BackImgInfo;      // Imagen de fondo
  sMIDIMusicInfo	  m_MIDIMusic;        // Musica MIDI de fondo
  sWAVSoundInfo		  m_WAVSoundInfo;     // Sonido WAV asociado
  sTextInfo			  m_TextInfo;         // Texto asociado
  sActSlideFadeInfo   m_ActSlideFadeInfo; // Info asociado al fade del slide actual
  bool                m_bFadeActive;      // ¿Hay un fade activo?
  sAutomaticSlidePass m_AutomaticPass;    // Info para el control del pase automatico
  sPrevMusic          m_PrevMusic;        // Info de sonido previo

  // Info para el control manual
  bool m_bESCKeyActive; // ¿Tecla ESC activa? 

  // Resto de vbles
  std::string					 m_szProfileName; // Nombre del perfil de la presentacion
  GUIManagerDefs::eGUIWindowType m_PrevInterfaz;  // Interfaz previo a la activacion
  word                           m_uwNumSlides;	  // Numero de slides  

public:
  // Constructor / destructor  
  CGUIWPresentation(void): m_pAudioSys(NULL),
						   m_pResManager(NULL),
						   m_pFileSys(NULL),
						   m_pGDBase(NULL),
						   m_pAlarmManager(NULL) { }
  ~CGUIWPresentation(void) { End(); }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(void);
  void End(void);

public:
  // CGUIWindow / Obtencion del tipo de ventana
  inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el tipo de interfaz
	return GUIManagerDefs::GUIW_PRESENTATION;
  }
  
public:
  // CGUIWindow / Activacion y desactivacion de la ventana de interfaz
  void Active(const std::string& szPresentationProfile,
			  const GUIManagerDefs::eGUIWindowType& PrevInterfaz,
			  iCGUIWindowClient* const pGUIWClient);  
  void Deactive(void);

private:
  // Trabajo con los slides
  void NextSlide(void);
  void OnNextSlide(void);
  void TLoadSlideBackImg(CCBTEngineParser* const pParser);
  void TLoadMIDIMusic(CCBTEngineParser* const pParser);
  void TLoadWAVSound(CCBTEngineParser* const pParser);
  void TLoadTextInfo(CCBTEngineParser* const pParser);
  void TLoadFadeOutInfo(CCBTEngineParser* const pParser);
  void RemoveAllTextInfo(void);  

private:
  // Notificacion de cambio de interfaz
  void SetPrevInterfazWindow(void);

public:
  // iCInputClient / Manejo de los eventos de entrada
  bool DispatchEvent(const InputDefs::sInputEvent& aInputEvent);
private:
  void InstallClient(void);
  void UnistallClient(void);

public:
  // iCAlarmClient / Notificacion de la finalizacion de una alarma
  void AlarmNotify(const AlarmDefs::eAlarmType& AlarmType,
				   const AlarmDefs::AlarmHandle& hAlarm);
  
public:
  // iCCommandClient / Notificacion para la finalizacion del comando Fade
  void EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
					const dword udInstant,
					const dword udExtraParam);  

public:
  // Operacion de dibujado
  void Draw(void);

public:
  // Trabajo con las alarmas de paso automatico de slides
  void InstallAlarm(void);
  void UninstallAlarm(void);
};

#endif // ~ ifndef CGUIWPresentation