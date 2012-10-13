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
// CAnimTemplate.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CAnimTemplate.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CAnimTemplate.h"

#include "SYSEngine.h"
#include "iCResourceManager.h"
#include "iCAudioSystem.h"
#include "iCGameDataBase.h"
#include "iCCommandManager.h"
#include "iCAlarmManager.h"
#include "iCLogger.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicia instancia.
// Parametros:
// - szAnimTemplate. Nombre del fichero con los datos
// - InitState. Estado inicial.
// - InitOrientation. Orientacion inicial
// - bActiveAnimCmd. ¿Se activa el comando de animacion?
// Devuelve:
// - Si se ha inicializado todo correctamente true. En caso contrario false.
// Notas:
// - No se permitira reinicializar.
// - El metodo recibira siempre el nombre del fichero NO la extension, esta
//   sera añadida aqui mismo SIEMPRE.
///////////////////////////////////////////////////////////////////////////////
bool 
CAnimTemplate::Init(const std::string& szAnimTemplate,
					const AnimTemplateDefs::AnimState& InitState,
					const AnimTemplateDefs::AnimOrientation& InitOrientation,
					const bool bActiveAnimCmd) 
{
  // SOLO si parametros validos
  ASSERT(szAnimTemplate.size());

  // ¿Se intenta reinicializar?
  if (IsInitOk()) { 
	return false; 
  }

  // Se obtiene la instancia CAnimTemplateData asociada con los datos
  // sobre la plantilla de animacion
  m_pATemplateData = SYSEngine::GetResourceManager()->RegisterAnimTemplateData(szAnimTemplate,
																			   this);
  ASSERT(m_pATemplateData);
  
  // Levanta flag de inicializacion
  m_bIsInitOk = true;  	

  // Se establece el volumen del sonido
  m_WAVVolume = AudioDefs::WAV_VOLUME_NORMAL;

  // Se inicializan resto de vbles de miembro
  // Nota: Aunque se establezca estado 0 en m_ActState, luego habra que 
  // establecer dicho estado de manera logica usando el metodo SetState
  m_hCycleAlarm = 0;
  m_ActOrient = InitOrientation;
  m_ActState = 0;

  // Para el mantenimiento de la logica, se supondra activo el comando de
  // animacion y se establecera el estado. Luego, se comprobara si el estado
  // de animacion debe de estar desactivado, en cuyo caso se desactivara
  m_bAnimCmdActive = true;
  SetState(InitState);
  if (!bActiveAnimCmd) {
	ActiveAnimCmd(false);
  }

  // Se establece flag
  m_bWAVPlayed = false; 

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera todas las estructuras de datos asociadas a la plantilla de
//   animacion y baja los flags oportunos.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAnimTemplate::End(void) 
{  
  // Finaliza si procede
  if (IsInitOk()) {
	// Libera la posible alarma instalada
    if (m_hCycleAlarm) {
	  SYSEngine::GetAlarmManager()->UninstallAlarm(m_hCycleAlarm);
	  m_hCycleAlarm = 0;
	}	
	
	// Se vacia la lista de observers
	m_Observers.clear();
	
	// Se finaliza comando de animacion
	m_AnimCmd.End();	
	
	// Se desvicula plantilla de animacion con los datos
	SYSEngine::GetResourceManager()->ReleaseAnimTemplateData(m_pATemplateData->GetAnimTemplateName(),
															 this);
	
	// Baja flag
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Incrementa el contador a sig. frame teniendo en cuenta si el estado actual.
// - Tambien se encargara de notificar si se ha producido un ciclo completo.
//   es o no reversible.
// Parametros:
// Devuelve:
// - Si se ha producido un ciclo completo true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CAnimTemplate::IncFrame(void) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Estado reversible?
  bool bCycleComplete;
  if (m_pATemplateData->IsReversibleState(m_ActState)) {
	// Si, se avanza hacia atras
	// Se decrementa frame; ¿Se ha realizado un ciclo completo?
	--m_ActFrame;
	if (m_ActFrame == -1) {
	  m_ActFrame = m_pATemplateData->GetNumFrames(m_ActState) - 1;
	  bCycleComplete = true;
	} else {
	  bCycleComplete = false;
	}
  } else {
	// No, se avanza de forma directa
	// Se incrementa frame; ¿Se ha realizado un ciclo completo?
	++m_ActFrame;
	if (m_ActFrame == m_pATemplateData->GetNumFrames(m_ActState)) {	  
	  m_ActFrame = 0; 
	  bCycleComplete = true;
	} else {
	  bCycleComplete = false;
	}
  }

  // ¿Se produjo un ciclo completo?
  if (bCycleComplete) {
	// Si, se notifica
	ObserversNotify(AnimTemplateObserverDefs::STATE_CYCLE_COMPLETE, m_ActState);		
  }

  // Se retorna
  return bCycleComplete;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Pasa un frame de animacion en un estado determinado. Tambien llevara
//   el control de cambio de estados, para estados no circulares, asi como
//   la reproduccion de ficheros WAV. 
// - Se sigue el convenio que la posicion del frame podra estar entre -1 y
//   (MaxFrames-1). Siendo el valor -1 indicativo del comienzo de reproduccion
//   de un estado. 
// Parametros:
// Devuelve:
// Notas:
// - El valor -1 hace referencia al valor STARTING_NOREVERSE_FRAME.
///////////////////////////////////////////////////////////////////////////////
void 
CAnimTemplate::NextFrame(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si esta activo el comando de animacion
  ASSERT(IsAnimCmdActive());
 
  // ¿Frame de inicio?
  if (CAnimTemplate::STARTING_NOREVERSE_FRAME == m_ActFrame ||
	  m_ActFrame == m_pATemplateData->GetNumFrames(m_ActState)) {
	NextInStartingState();			
  } else {
	// Se guarda el estado actual antes de realizar la operacion
	const AnimTemplateDefs::AnimState OldState = m_ActState;

	// ¿Estado actual igual a estado de enlace?
	if (m_ActState == m_pATemplateData->GetLinkState(m_ActState)) { 
	  // Avance circular
	  NextInCyclicState();	  
	} else {
	  // Avance NO circular	  
	  NextInLinearState();
	}

	// ¿Se cambio de estado?
	if (OldState != m_ActState) {
	  // Si, se notifica a los observadores
	  ObserversNotify(AnimTemplateObserverDefs::CHANGETO_LINKSTATE, OldState);
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Avanza el frame cuando se este en el estado de inicio (swActFrame == -1)
// - Tambien preparara el sonido asociado al estado, tanto si este es 
//   circular como si es lineal.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAnimTemplate::NextInStartingState(void)
{   
  // SOLO si parametros validos
  ASSERT(m_pATemplateData);

  // Se situa indice en primer frame
  m_ActFrame = m_pATemplateData->IsReversibleState(m_ActState) ?
			   m_pATemplateData->GetNumFrames(m_ActState) - 1 : 0;	

  // ¿Estado con sonido asociado?
  const ResDefs::WAVSoundHandle hWAVSound = m_pATemplateData->GetWAVSoundHandle(this, m_ActState);
  if (hWAVSound) {
	// Si, se reproduce y se levanta flag de sonido en reproduccion
	PlayWAVSound(hWAVSound);		
	m_bWAVPlayed = true;
  } else {
	// Si, se baja flag de sonido en reproduccion
	m_bWAVPlayed = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Avanza frames en forma circular.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAnimTemplate::NextInCyclicState(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se avanza sig. frame
  if (IncFrame()) {
	// Se produjo un ciclo completo
	// ¿Tiene asociado un tiempo entre ciclos?
	const float fCylcleWait = m_pATemplateData->GetTimeBetweenCycles(m_ActState);
	if (fCylcleWait >= 1.0f) {
	  // Si, se instala alarma
	  ASSERT((0 == m_hCycleAlarm) != 0);
	  m_hCycleAlarm = SYSEngine::GetAlarmManager()->InstallTimeAlarm(this, fCylcleWait);
	  ASSERT(m_hCycleAlarm);

	  // Se pausa el comando de animacion
	  m_AnimCmd.SetPause(true);
	}
  }
}								 

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Avanza tieniendo en cuenta que se esta sobre un estado de animacion de
//   tipo lineal.
// Parametros:
// - m_pATemplateData. Nodo con la informacion de la plantilla
// Devuelve:
// - Indice al primer frame (0)
// Notas:
// - Un estado lineal es aquel que tiene a otro estado distinto como estado de
//   transicion.
// - A la hora de cambiar de estado, es vital usar el metodo SetState en 
//   lugar de hacer el proceso manualmente
///////////////////////////////////////////////////////////////////////////////
void
CAnimTemplate::NextInLinearState(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Es estado SIN sonido asociado?
  const ResDefs::WAVSoundHandle hWAVSound = m_pATemplateData->GetWAVSoundHandle(this, m_ActState);
  if (0 == hWAVSound) {
	// Se avanza a sig. frame	
	if (IncFrame()) {
	  // Se dio una vuelta completa, luego se cambia de estado
	  SetState(m_pATemplateData->GetLinkState(m_ActState));
	}
	return;
  } 
  
  // Estado con sonido asociado; 
  // ¿Termino de reproducirse el sonido?
  if (!SYSEngine::GetResourceManager()->IsWAVSoundPlaying(hWAVSound)) {
    // Termino de reproducirse el sonido, se comprueba el modo de tratarlo
    if (!m_pATemplateData->GetWAVSoundFlag(this, m_ActState)) {
      // Modo de salto de estado al finalizar sonido
      SetState(m_pATemplateData->GetLinkState(m_ActState));
      m_ActFrame = 0;
    } else {    
      // Modo de espera en el ultimo estado, en caso contrario, salto
	  // instantaneo.
      if (m_ActFrame < m_pATemplateData->GetNumFrames(m_ActState) - 1) { 
        ++m_ActFrame;
      } else {
		SetState(m_pATemplateData->GetLinkState(m_ActState));
		m_ActFrame = 0;
      }
    }
	return;
  } 

  // Estado con sonido asociado 
  // NO Termino de reproducirse el sonido
  if (!m_pATemplateData->GetWAVSoundFlag(this, m_ActState)) {
	// Modo de salto de estado al finalizar sonido	
	if (IncFrame()) {
	  // Se dio una vuelta completa, luego se cambia de estado
	  SetState(m_pATemplateData->GetLinkState(m_ActState));
	}    
  } else {
	// Modo de espera en ultimo estado
	if (m_ActFrame < m_pATemplateData->GetNumFrames(m_ActState) - 1) { 
	  ++m_ActFrame;
	} 
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece orientacion asociada a la plantilla. En caso de que la 
//   orientacion se considere no valida, no se ejecutara la logica asociada.
// Parametros:
// - Orientation. Orientacion a establecer.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAnimTemplate::SetOrientation(const AnimTemplateDefs::AnimOrientation& Orientation) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Orientacion valida? y
  // ¿Comando de animacion activo?
  if (Orientation < m_pATemplateData->GetNumOrientations() &&
	  IsAnimCmdActive()) {
	// Se establece orientacion
	m_ActOrient = Orientation;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece un nuevo estado de animacion. En caso de que el estado no sea
//   valido, obviara la llamada.
// - Al establecer un estado de animacion, necesariamente se comprobara si
//   el estado tiene o no animacion. Si no tiene, no se activara el comando
//   de animacion y, si lo estuviera, se detendra. En caso de que tenga,
//   si el comando no esta activo se inicializara y se colocara en el command
//   manager.
// Parametros:
// - ubState. Estado a donde cambiar
// Devuelve:
// Notas:
// - En el caso de que se asocie un estado no valido, la operacion se ignorara.
///////////////////////////////////////////////////////////////////////////////
void 
CAnimTemplate::SetState(const AnimTemplateDefs::AnimState& State)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
    
  // ¿Estado valido?
  if (State < m_pATemplateData->GetNumStates()) {
	// Se atiende peticion
	// ¿Comando de animacion activo?
	if (IsAnimCmdActive()) {
	  // Antes de establecer el estado, se debera de comprobar si el actual
	  // tiene asociado sonido, en cuyo caso se detendra.
	  // Se toma el handle al posible sonido asociado	
	  const ResDefs::WAVSoundHandle hWAVSound = m_pATemplateData->GetWAVSoundHandle(this, m_ActState);
	  if (hWAVSound) {
		SYSEngine::GetAudioSystem()->StopWAVSound(hWAVSound);
	  }		

	  // ¿El estado actual tenia asociado alarma?
	  if (m_hCycleAlarm) {
		// Si, se desinstalara y se desvinculara alarma
		SYSEngine::GetAlarmManager()->UninstallAlarm(m_hCycleAlarm);
		m_hCycleAlarm = 0;
	  }	  

	  // Se establece estado y se pone el contador de frames a inicio
	  m_ActState = State;
	  m_ActFrame = m_pATemplateData->IsReversibleState(State) ? 
				   m_pATemplateData->GetNumFrames(State) : 
				   CAnimTemplate::STARTING_NOREVERSE_FRAME;

	  // Se avanza de frame de forma automatica, para situarlo en posicion 0
	  // Nota: Esto es necesario, porque en caso de que tenga un sonido asociado
	  // pero no se cumpla la condicion de animacion, debera de hacerse sonar
	  NextFrame();
	  
	  // ¿Se cumple condicion de animacion? Y
	  // ¿Esta activo el uso del comando de animacion?
	  if (m_pATemplateData->IsStateWithAnim(m_ActState)) {
		// ¿Esta activo el uso de comando de animacion? Y
		// ¿NO esta activo el comando de animacion?	  	
		if (IsAnimCmdActive() &&
			!m_AnimCmd.IsActive()) {
		  // Se inicializa y se inserta en el CommandManager
		  m_AnimCmd.Init(this);
		  ASSERT(m_AnimCmd.IsActive());
		  SYSEngine::GetCommandManager()->PostCommand(&m_AnimCmd);
		}		  
	  } else {
		// No hay estado de animacion, luego se finaliza comando
		// notificando antes que se ha realizado una vuelta completa
		m_AnimCmd.End();	  	  
		ObserversNotify(AnimTemplateObserverDefs::STATE_CYCLE_COMPLETE, m_ActState);
	  }	
	} 
  } else {
	// El estado no es valido, luego se pasa por alto la peticion
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CAnimTemplate::SetState> El estado de animación %u no es válido\n", State); 
	  SYSEngine::GetLogger()->Write("                       | Nombre de la plantilla: %s\n", GetAnimTemplateName().c_str()); 
	  SYSEngine::GetLogger()->Write("                       | Estados válidos [0..%u]\n", GetNumStates()-1); 	  
	#endif
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el nuevo valor de sonido asociado a todos los wav de la 
//   plantilla de animacion.
// Parametros:
// - Volume. Valor de sonido a asociar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAnimTemplate::SetWAVSoundVolume(const AudioDefs::eWAVVolume& Volume)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se recorren todos los estados, estableciendo el valor
  iCAudioSystem* const pAudioSys = SYSEngine::GetAudioSystem();
  ASSERT(pAudioSys);
  AnimTemplateDefs::AnimState StateIt = 0;
  const word uwNumStates = GetNumStates();
  for (; StateIt < uwNumStates; ++StateIt) {
	// ¿Hay WAV asociado?
	const ResDefs::WAVSoundHandle hWAVSound = m_pATemplateData->GetWAVSoundHandle(this, StateIt);
	if (hWAVSound) {
	  // Si, luego se establece el sonido asociado al mismo
	  pAudioSys->SetWAVSoundVolume(hWAVSound, Volume);
	} 
  }
  
  // Se guarda valor de sonido
  m_WAVVolume = Volume;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre la lista de observers, realizando la notificacion
// - Los formatos seran:
//    * CHANGETO_LINKSTATE -> Se ha cambiado a estado de enlace. Se pasa el
//      estado previo.
//    * STATE_CYCLE_COMPLETE -> Se ha llegado al ultimo estado. Se pasa el
//      estado actual.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro de notificacion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAnimTemplate::ObserversNotify(const AnimTemplateObserverDefs::eObserverNotifyType& NotifyType,
							   const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Recorre la lista de observers y notifica
  ATemplateObserverListIt It(m_Observers.begin());
  for (; It != m_Observers.end(); ++It) {
	(*It)->AnimTemplateObserverNotify(NotifyType, udParam);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el comando de animacion o lo desactiva. Este metodo es muy util
//   para mejorar la optimizacion del motor cuando una plantilla no tenga
//   necesidad de verse en pantalla pero su instancia se halle activa.
// Parametros:
// - bActive. Flag de activacion / desactivacion
// Devuelve:
// Notas:
// - Cuando el comando de animacion NO este activo, los cambios de estado y
//   de orientacion NO seran posibles y ademas, la llamada a NextFrame tampoco
//   lo sera utilizandse, en este ultimo caso, comprobacion via ASSERT. Asi,
//   si se quiere poner en marcha el comando de animacion y cambiar de estado,
//   SIEMPRE se debera de hacer primero la operacion de poner en marcha el
//   comando y luego la de cambiar de estado.
///////////////////////////////////////////////////////////////////////////////
void 
CAnimTemplate::ActiveAnimCmd(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
    
  // ¿El estado recibido es DISTINTO del actual?
  if (m_bAnimCmdActive != bActive) {	
	// Se establece flag
	m_bAnimCmdActive = bActive;

	// Se toma el handle al posible sonido asociado
	ASSERT(m_pATemplateData);
	const ResDefs::WAVSoundHandle hWAVSound = m_pATemplateData->GetWAVSoundHandle(this, m_ActState);

	// ¿Se desea establecer el estado de anim.?
	if (bActive) {
	  // Si, ¿El estado actual es estado de animacion?
	  if (m_pATemplateData->IsStateWithAnim(m_ActState)) {
		// Se inicializa el comando y se inserta en cola
		m_AnimCmd.Init(this);
		ASSERT(m_AnimCmd.IsActive());
		SYSEngine::GetCommandManager()->PostCommand(&m_AnimCmd);				
	  }
	  
	  //  Se continuara la reproduccion del sonido si procede		
	  // Nota: En caso de que se este en frame de inicio (-1 o final) se obviara
	  if ((m_ActFrame != CAnimTemplate::STARTING_NOREVERSE_FRAME || 
	  	   m_ActFrame != m_pATemplateData->GetNumFrames(m_ActState)) &&
		   hWAVSound) {				
		PlayWAVSound(hWAVSound);
	  }
	  
	  // Se quitara la pausa en posible alarma asociada al estado actual
	  if (m_hCycleAlarm) {
		SYSEngine::GetAlarmManager()->SetPause(m_hCycleAlarm, false);
	  }	
	} else {
	  // No, se finaliza el comando de animacion
	  m_AnimCmd.End();	  
	  
	  // Se pausara la reproduccion del sonido si procede
	  if (hWAVSound) {
		SYSEngine::GetAudioSystem()->PauseWAVSound(hWAVSound);
	  }		

	  // Se pausara posible alarma asociada al ciclo actual
	  if (m_hCycleAlarm) {
		SYSEngine::GetAlarmManager()->SetPause(m_hCycleAlarm, true);
	  }
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion de la finalizacion de la alarma temporal asociada al estado
//   actual que SEGURO que es ciclico.
// Parametros:
// - Tipo de alarma (debera de ser de tiempo)
// - hAlarm. Handle a la alarma.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAnimTemplate::AlarmNotify(const AlarmDefs::eAlarmType& AlarmType,
						   const AlarmDefs::AlarmHandle& hAlarm) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT((AlarmType == AlarmDefs::TIME_ALARM) != 0);
  ASSERT(hAlarm);
  ASSERT((hAlarm == m_hCycleAlarm) != 0);

  // Se quita la pausa del comando de animacion, para que siga produciendose
  // el paso de frames. Tambien se desvinculara la alarma
  ASSERT(m_AnimCmd.IsInPause());
  m_AnimCmd.SetPause(false);
  m_hCycleAlarm = 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Reproduce el sonido hWAVSound segun sea el estado en el que nos 
//   encontremos.
// Parametros:
// - hWAVSound. Handle al sonido.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAnimTemplate::PlayWAVSound(const ResDefs::WAVSoundHandle& hWAVSound)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hWAVSound);

  // Reproduce el sonido de forma ciclica o lineal segun proceda
  // ¿Se esta en estado ciclico y hay flag de sonido activo?
  if (m_ActState == m_pATemplateData->GetLinkState(m_ActState) &&
	  m_pATemplateData->GetWAVSoundFlag(this, m_ActState)) {
	// Si, luego la reproduccion sera en LOOP
	SYSEngine::GetAudioSystem()->PlayWAVSound(hWAVSound, AudioDefs::WAV_PLAY_LOOP);
  } else {
	// No, luego la reproduccion sera normal
	SYSEngine::GetAudioSystem()->PlayWAVSound(hWAVSound, AudioDefs::WAV_PLAY_NORMAL);
  } 
}