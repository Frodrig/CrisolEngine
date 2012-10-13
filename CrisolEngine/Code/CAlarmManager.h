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
// CAlarmManager.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CAlarmManager.
//
// Descripcion:
// - Manager de alarmas. Las alarmas estaran definidas como interfaces 
//   iCAlarmClient. Las alarmas podran estar asociadas a distintos tipos
//   de eventos:
//   * Finalizacion de un tiempo asignado.
//   * Finalizacion en la reproduccion de un fichero WAV.
//   Cuando uno de esos eventos se produzca, se notificara al cliente mediante
//   la llamada al metodo definido en iCAlarmClient que estos deberan de 
//   implementar.
// - Al instalarse un cliente como alarma se le devolvera un handle. El handle
//   tendra dos partes. La parte superior indicara el tipo de handle y la 
//   inferior el numero asociado al handle.
//
// Notas:
// - El siguiente handle no se debera de tomar directamente de la vble de
//   miembro, habra que llamar al metodo GetNextHandle() que se encargara
//   de realizar el tratamiento correcto, segun sea el tipo de alarma.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CALARMMANAGER_H_
#define _CALARMMANAGER_H_

// Cabeceras
#ifndef _ICALARMMANAGER_H_
#include "iCAlarmManager.h"
#endif
#ifndef _SET_H_
#define _SET_H_
#include <set>
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif

// Defincion de clases / estructuras / espacios de nombres
struct iCResourceManager;

// Clase CAlarmManager
class CAlarmManager: public iCAlarmManager
{   
private:
  // Estructuras forward
  struct sNTimeClientInfo; 
  struct sNAlarmTime;
  struct sNWAVClientInfo;

private:
  // Tipos
  // Alarmas de tiempo
  // Lista de clientes asociados a un tiempo de finalizacion
  typedef std::list<sNTimeClientInfo*> TimeClientList;
  typedef TimeClientList::iterator     TimeClientListIt;
  // Lista de tiempos de finalizacion asociados a listas de clientes
  typedef std::list<sNAlarmTime*> AlarmTimeList;
  typedef AlarmTimeList::iterator AlarmTimeListIt;
  // Ficheros WAV
  typedef std::list<sNWAVClientInfo*> WAVClientList;
  typedef WAVClientList::iterator     WAVClientListIt;

private:
  // Template para formar estructura de informacion asociada a alarmas
  template<class AlarmType>
  struct sAlarmInfo {
	// Info asociada a las alarmas de tiempo instaladas
	AlarmType              Alarms;     // Lista de alarmas  
	AlarmDefs::AlarmHandle LastHandle; // Siguiente handle a asignar	
  };

private:
  // Tipos
  // Set para el control de alarmas en pausa.
  typedef std::set<AlarmDefs::AlarmHandle> AlarmsInPauseSet;
  typedef AlarmsInPauseSet::iterator       AlarmsInPauseSetIt;

private:  
  // Instancia singlenton
  static CAlarmManager* m_pAlarmManager; // Unica instancia a la clase      

  // Instancias a otros subsistemas / clases de apoyo
  iCResourceManager* m_pResManager; // Servidor de recursos
  
  // Resto de vbles de miembro
  AlarmsInPauseSet          m_AlarmsInPause; // Alarmas en pausa
  sAlarmInfo<AlarmTimeList> m_TimeAlarms;    // Alarmas de tiempo
  sAlarmInfo<WAVClientList> m_WAVAlarms;     // Alarmas WAV
  bool				        m_bIsInitOk;     // ¿Instancia inicializada?
    
protected:
  // Constructor / Destructor
  CAlarmManager(void): m_pResManager(NULL),
					   m_bIsInitOk(false) { }
public:
  ~CAlarmManager(void) { End(); }               
  
public:
  // Protocolo de inicio y fin de instancia
  bool Init(void);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }
private:
  // Metodos de apoyo
  void ReleaseTimeAlarms(void);
  void ReleaseWAVAlarms(void);
  
public:
  // Obtencion y destruccion de la instancia singlenton
  static inline CAlarmManager* const GetInstance(void) {
    if (NULL == m_pAlarmManager) { 
      // Se crea la instancia
      m_pAlarmManager = new CAlarmManager; 
      ASSERT(m_pAlarmManager)
    }
    return m_pAlarmManager;
  } 
  static inline DestroyInstance(void) {
    if (m_pAlarmManager) {
      delete m_pAlarmManager;
      m_pAlarmManager = NULL;
    }
  }

public:
  // Actualizacion del manager
  void Update(const float fDelta);
private:
  // Metodos de apoyo
  void UpdateTimeAlarms(const float fDelta);
  void UpdateWAVAlarms(void);

public:
  // iCAlarmManager / Instalacion de alarmas
  AlarmDefs::AlarmHandle InstallTimeAlarm(iCAlarmClient* const pClient,
										  const float fTime);
  AlarmDefs::AlarmHandle InstallWAVAlarm(iCAlarmClient* const pClient,
										 const ResDefs::WAVSoundHandle& hWAVSound);
private:
  // Metodos de apoyo
  AlarmDefs::AlarmHandle GetNextHandle(const AlarmDefs::eAlarmType& AlarmType);

public:
  // iCAlarmManager / Finalizacion de una alarma
  bool UninstallAlarm(const AlarmDefs::AlarmHandle& hAlarm);  
private:
  // Metodos de apoyo
  bool UninstallTimeAlarm(const AlarmDefs::AlarmHandle& hAlarm);
  bool UninstallWAVAlarm(const AlarmDefs::AlarmHandle& hAlarm);


public:
  // iCAlarmManager / Manipulacion del handle
  void SetPause(const AlarmDefs::AlarmHandle& hAlarm, 
			    const bool bPause);
  bool IsInPause(const AlarmDefs::AlarmHandle& hAlarm) const {
	ASSERT(IsInitOk());
	// Comprueba si la alarma hAlarm esta o no en pausa
	return (m_AlarmsInPause.find(hAlarm) != m_AlarmsInPause.end());
  }
  AlarmDefs::eAlarmType GetHandleType(const AlarmDefs::AlarmHandle& hAlarm) const {	
	ASSERT(IsInitOk());	
	ASSERT(hAlarm);
	// Retorna el tipo
	return AlarmDefs::eAlarmType(hAlarm & !AlarmDefs::MAX_HANDLE_VALUE);
  }

};
#endif // ~ CAlarmManager
