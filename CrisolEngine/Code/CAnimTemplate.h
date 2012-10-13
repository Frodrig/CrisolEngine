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
// CAnimTemplate.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases
// - CAnimTemplate.
//
// Descripcion:
// - Esta clase se encarga de tratar los datos de una instancia 
//   CAnimTemplateData añadiendo la logica del tratamiento y las vbles de 
//   miembro particulares para trabajar con una instancia de plantilla de
//   animacion concreta. 
// - El trabajo con sonido seguira el siguiente esquema:
//   * Si el estado de enlace es el mismo, se reproducira de forma ciclica
//     o bien solo nada mas comenzar. Esto ultimo se indicara mediante 
//     un flag.
//   * Si el estado de enlace es distinto, se reproducira al realizar un
//     NextFrame en el primer estado y no se pasara al estado de enlace
//     hasta que se haya terminado el sonido. La espera podra ser en el
//     ultimo estado (se llega al ultimo estado y se espera) o en uno 
//     cualquiera (aunque no se haya llegado al ultimo estado). Esto ultimo
//     se indicara por flag.
// - Los estados ciclicos de mas de un frame, podran tener asociados un
//   temporizador, de tal forma que una vez que se produzca un ciclo completo,
//   se instalara dicho temporizador y no se avanzara al sig. frame hasta que
//   el temporizador haya vencido.
// - En todos los casos, el trabajo con las animaciones asociados a un estado,
//   podra ser en un sentido izquierda derecha o derecha izquierda (reversible).
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CANIMTEMPLATE_H_
#define _CANIMTEMPLATE_H_

// Cabeceras
#ifndef _ICANIMTEMPLATE_H_
#include "iCAnimTemplate.h"
#endif
#ifndef _ICANIMTEMPLATEOBSERVER_H_
#include "iCAnimTemplateObserver.h"
#endif
#ifndef _ICANIMTEMPLATEDATA_H_
#include "iCAnimTemplateData.h"
#endif
#ifndef _ICALARMCLIENT_H_
#include "iCAlarmClient.h"
#endif
#ifndef _CANIMCMD_H_
#include "CAnimCmd.h"
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif
#ifndef _VECTOR_H_
#define _VECTOR_H_
#include <vector>
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif
#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_
#include <algorithm>
#endif

// Definicion de clases / estructuras / espacios de nombres

// Clase CAnimTemplate
class CAnimTemplate: public iCAnimTemplate,
					 public iCAlarmClient
{
private:
  // Enumerados
  enum {
	// Frame de inicio en modo no reversible
	// Nota: en el caso del modo reversible, no se podra saber la posicion
	// ya que esta dependera del numero de frames del estado.
	STARTING_NOREVERSE_FRAME = -1, 
	// Identificador de alarma para estados ciclicos con esperas
	ID_CYCLICSTATE = 0X01,
  };

private:
  // Tipos
  // Lista de observers
  typedef std::list<iCAnimTemplateObserver*> ATemplateObserverList;
  typedef ATemplateObserverList::iterator    ATemplateObserverListIt;
  typedef ATemplateObserverList::value_type  ATemplateObserverListValType;

private:    
  // Vbles de miembro
  iCAnimTemplateData*               m_pATemplateData; // Datos de la plantilla
  ATemplateObserverList             m_Observers;      // Observadores
  CAnimCmd							m_AnimCmd;        // Comando de animacion
  AnimTemplateDefs::AnimState       m_ActState;		  // Estado actual
  AnimTemplateDefs::AnimOrientation m_ActOrient;	  // Orientacion actual
  AnimTemplateDefs::AnimFrame       m_ActFrame;		  // Frame actual
  AlarmDefs::AlarmHandle            m_hCycleAlarm;    // Posible alarma asociada
  AudioDefs::eWAVVolume             m_WAVVolume;      // Volumen asociado al sonido
  bool								m_bWAVPlayed;	  // Control de reproduccion de un WAV
  bool                              m_bAnimCmdActive; // ¿Comando de anim. activo?
  bool								m_bIsInitOk;      // ¿Clase inicializada correctamente?  
  
public:
  // Constructor / destructor
  CAnimTemplate(void): m_bIsInitOk(false) { }
  ~CAnimTemplate(void) { End(); }

public:
  // Protocolo de inicio y fin de instancia
  bool Init(const std::string& szAnimTemplate,
			const AnimTemplateDefs::AnimState& InitState,
			const AnimTemplateDefs::AnimOrientation& InitOrientation,
			const bool bActiveAnimCmd);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // Obtencion del nombre de la plantilla de animacion
  std::string GetAnimTemplateName(void) const {
	ASSERT(IsInitOk());
	// Retorna el nombre de la plantilla
	return m_pATemplateData->GetAnimTemplateName();
  }

private:
  // iCAnimTemplate / Trabajo con las frames
  void NextFrame(void);
public:
  word GetNumFrames(const AnimTemplateDefs::AnimState& State) {
	ASSERT(IsInitOk());
	ASSERT((State < m_pATemplateData->GetNumStates()) != 0);
	// Retorna el numero de frames asociados a un estado	
	return m_pATemplateData->GetNumFrames(State);
  }
  bool IsReversibleState(const AnimTemplateDefs::AnimState& State) {
	ASSERT(IsInitOk());
	// Comprueba si el estado State es o no reversible
	return m_pATemplateData->IsReversibleState(State);
  }

private:
  // Metodos de apoyo  
  bool IncFrame(void);
  void NextInStartingState(void);
  void NextInLinearState(void);
  void NextInCyclicState(void);
  inline GetActFrame(void) {
	ASSERT(IsInitOk());
	// Retorna el identificador al frame actual
	// Nota: Es muy importante llamar a este metodo para consultar el frame
	// actual, pues en caso de que se este en estado de inicio, que podra ser
	// -1 si es no reversible y NumFrames si lo es, el metodo devolvera
	// indicador de 0 y NumFrames-1 respectivamente
	if (m_ActFrame == CAnimTemplate::STARTING_NOREVERSE_FRAME) {
	  return 0;
	} else {
	  const AnimTemplateDefs::AnimFrame Frames = GetNumFrames(m_ActState);	  
	  if (m_ActFrame == Frames) {
		return (Frames - 1);
	  } else { 
		return m_ActFrame;
	  }
	} 
  } 

public:
  // iCAnimTemplate / Trabajo con orientaciones de animacion
  void SetOrientation(const AnimTemplateDefs::AnimOrientation& Orientation);
  AnimTemplateDefs::AnimOrientation GetOrientation(void) const {
	ASSERT(IsInitOk());
	// Retorna la orientacion actual
	return m_ActOrient;
  }
  word GetNumOrientations(void) { 
	ASSERT(IsInitOk());
	// Returna el numero de orientaciones
	// Nota: bastara con retornar el numero de orientaciones del primer estado
	return m_pATemplateData->GetNumOrientations();
  }
  void NextOrientation(void) {
	ASSERT(IsInitOk());
	// Avanza de forma circular la orientacion
	++m_ActOrient;
	if (m_ActOrient == GetNumOrientations()) {
	  m_ActOrient = 0;
	}
  }  

public:
  // iCAnimTemplate / Trabajo con estados de animacion
  void SetState(const AnimTemplateDefs::AnimState& State);
  word GetNumStates(void) {
	ASSERT(IsInitOk());
	// Returna el numero de estados de animacion	
	return m_pATemplateData->GetNumStates();
  }
  AnimTemplateDefs::AnimState GetAnimState(void) const {
	ASSERT(IsInitOk());
	// Retorna el estado de animacion actual
	return m_ActState;
  }

private:
  // Metodos de apoyo
  inline bool IsStateWithAnim(const AnimTemplateDefs::AnimState& State) {
	ASSERT(IsInitOk());
	// Retorna flag
	return m_pATemplateData->IsStateWithAnim(State);
  }

public:
  // iCAnimTemplate / Obtencion del identificador del frame actual
  ResDefs::TextureHandle GetIDTextureFrame(void) {
	ASSERT(IsInitOk());
	// Se obtiene instancia a estructura de informacion
	return m_pATemplateData->GetIDTexture(m_ActState, m_ActOrient, GetActFrame());
  }

public:
  // iCAnimTemplate / Obtecion de informacion general  
   AnimTemplateDefs::FramesPerSecond GetFPS(void) {
	ASSERT(IsInitOk());
	// Retorna el numero de FPS estandar
	return m_pATemplateData->GetFPS();
  }
   word GetFramesWidth(void) {
	ASSERT(IsInitOk());
	// Retorna el ancho de los frames
	return m_pATemplateData->GetFramesWidth();
  }
   word GetFramesHeight(void) {
	ASSERT(IsInitOk());
	// Retorna el alto de los frames
	return m_pATemplateData->GetFramesHeight();
  }

public:
  // iCAnimTemplate / Trabajo con el control del comando de animacion
  void ActiveAnimCmd(const bool bActive);
  bool IsAnimCmdActive(void) const {
	ASSERT(IsInitOk());
	// Retorna flag
	return m_bAnimCmdActive;
  }

public:
  // Trabajo con el sonido
  void SetWAVSoundVolume(const AudioDefs::eWAVVolume& Volume);
  AudioDefs::eWAVVolume GetWAVSoundVolume(void) const {
	ASSERT(IsInitOk());
	// Retorna el valor del sonido actual
	return m_WAVVolume;
  }
private:
  // Metodos de apoyo
  void PlayWAVSound(const ResDefs::WAVSoundHandle& hWAVSound);

public:
  // iCAnimTemplate / Trabajo con observers
  void AddObserver(iCAnimTemplateObserver* const pObserver) {	
	ASSERT(IsInitOk());  
	ASSERT(pObserver);
	#ifdef _SYSASSERT  
	  const ATemplateObserverListIt It(std::find(m_Observers.begin(),
												 m_Observers.end(),
												 pObserver));
	  ASSERT((It == m_Observers.end()) != 0);
	#endif  
	// Se inserta el observer
	m_Observers.push_back(pObserver);  
  }
  void RemoveObserver(iCAnimTemplateObserver* const pObserver) {
	ASSERT(IsInitOk());
	ASSERT(pObserver);
	// Se borra el observer
	ATemplateObserverListIt It(std::find(m_Observers.begin(),
										 m_Observers.end(),
										 pObserver));
	ASSERT((It != m_Observers.end()) != 0);
	m_Observers.erase(It);
  }
private:
  // Metodos de apoyo
  void ObserversNotify(const AnimTemplateObserverDefs::eObserverNotifyType& NotifyType,
					   const dword udParam = 0);

public:
  // iCAlarmClient / Notificacion de la finalizacion de una alarma
  void AlarmNotify(const AlarmDefs::eAlarmType& AlarmType,
				   const AlarmDefs::AlarmHandle& hAlarm);
};

#endif // ~ #ifdef _CANIMTEMPLATE_H_