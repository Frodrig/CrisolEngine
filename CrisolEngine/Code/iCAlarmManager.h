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
// iCAlarmManager.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCAlarmManager.
//
// Descripcion:
// - Interfaz para CAlarmManager
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICALARMMANAGER_H_
#define _ICALARMMANAGER_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"         
#endif
#ifndef _ALARMDEFS_H_
#include "AlarmDefs.h"
#endif
#ifndef _RESDEFS_H_
#include "ResDefs.h"
#endif

// Definicion de clases / estructuras / tipos referenciados
struct iCAlarmClient;

// Interfaz iCAlarmManager
struct iCAlarmManager
{ 
public:
  // Instalacion de alarmas
  virtual AlarmDefs::AlarmHandle InstallTimeAlarm(iCAlarmClient* const pClient,
												  const float fTime) = 0;
  virtual AlarmDefs::AlarmHandle InstallWAVAlarm(iCAlarmClient* const pClient,
												 const ResDefs::WAVSoundHandle& hWAVSound) = 0;

public:
  // Finalizacion de una alarma
  virtual bool UninstallAlarm(const AlarmDefs::AlarmHandle& hAlarm) = 0;  

public:
  // Manipulacion del handle
  virtual AlarmDefs::eAlarmType GetHandleType(const AlarmDefs::AlarmHandle& hAlarm) const = 0;
  virtual void SetPause(const AlarmDefs::AlarmHandle& hAlarm, 
						const bool bPause) = 0;
  virtual bool IsInPause(const AlarmDefs::AlarmHandle& hAlarm) const = 0;
};

#endif // ~ #ifdef _ICALARMMANAGER_H_
