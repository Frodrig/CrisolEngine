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
// CCommand.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CCommand
//
// Descripcion:
// - Clase base para todos los comandos. Se debera de implementar siempre
//   en los que hereden de esta clase, el comando Execute.
// - El destructor se encargara de desinstalar el comando de forma automatica
//   con lo que las clases que posean un comando no se tendran que preocupar
//   de realizar esta operacion en la cola de comandos.
//
// Notas:
// - Se seguira el convenio de que todas las clases que hereden de CCommand
//   se nombraran: C[NombreComando]Cmd. El Cmd final sera para indicar que
//   es un CoMmanD.
// - El flag m_bInQueue sera establecido / desestablecido por la cola de
//   comandos de tal modo que cuando un comando este en la cola se pondra a
//   true y cuando no este a false. Esto permitira acelerar operaciones y
//   proteger de efectos laterales (intruducir en la cola un comando dos veces;
//   aun cuando no esta activo, pudiera ser que la cola no lo haya eliminado
//   aun), y hacer que desde el exterior no tenga que existir preocupacion por
//   comprobar si el comando esta en la cola o no.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CCOMMAND_H_
#define _CCOMMAND_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif

// Clase CCommand
class CCommand
{
private:
  // Clases amigas
  // Manejador de comandos
  friend class CCommandManager;

private:
  // Vbles de miembro
  bool m_bIsActive; // ¿Comando activo?
  bool m_bInQueue;  // ¿Comando en cola?
  bool m_bInPause;  // ¿Comando pausado?

public:
  // Constructor / destructor
  CCommand(void): m_bIsActive(false),
				  m_bInQueue(false),
				  m_bInPause(false) { }
  ~CCommand(void);

public:
  // Protocolos de inicializacion finalizacion
  bool Init(void) {
	// Reinicializa
	End();	
	m_bIsActive = true;
	m_bInPause = false;	
	return true;
  }
  void End(void) {
	// Finaliza
	if (m_bIsActive) { 	 	  
	  m_bIsActive = false; 
	  m_bInPause = false;
	}
  }
  inline bool IsActive(void) const { return m_bIsActive; }
  
public:
  // Ejecucion
  virtual void Execute(const float fDelta) = 0;

public:
  // Trabajo con el modo pausa
  virtual void SetPause(const bool bPause) {
	ASSERT(IsActive());
	// Establece / quita el flag de modo pausa
	m_bInPause = bPause;
  }
  bool IsInPause(void) const {
	ASSERT(IsActive());
	// Retorna flag
	return m_bInPause;
  }
};

#endif // ~ #ifdef _CCOMMAND_H_
