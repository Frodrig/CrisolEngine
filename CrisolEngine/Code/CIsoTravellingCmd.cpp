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
// CIsoTravellingCmd.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CIsoTravellingCmd.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CIsoTravellingCmd.h"

#include "SYSEngine.h"
#include "iCWorld.h"
#include "CMovIsoEngineCmd.h"
#include "iCCommandManager.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el comando tomando la posicion actual del motor isometrico
//   y realizando un travelling hasta dejar a la posicion destino en el extremo
//   superior izquierdo.
// Parametros:
// - fXEndPos, fYEndPos. Posicion destino en x,y.
// - uwSpeed. Velocidad de movimiento.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
// - El travelling se implementara sin apoyarnos en movimientos diagonales.
// - La posiciones siempre seran relativas a coordenadas universales del area.
// - No se permitira comandos pendientes, de tal forma que hasta que el 
//   comando actual no se suspenda o termine, no podra realizarse otro.
// - El travelling nunca realizara movimientos diagonales, primero hara un
//   movimiento en horizontal y luego en vertical.
// - Al inicializar un travelling, automaticamente se le comunicara al motor
//   isometrico que suspenda el procesamiento de la entrada
///////////////////////////////////////////////////////////////////////////////
bool
CIsoTravellingCmd::Init(const float fXEndPos, const float fYEndPos, 
						const word uwSpeed)
{
  // SOLO si parametros correctos
  ASSERT((fXEndPos >= 0) != 0.0f);
  ASSERT((fYEndPos >= 0) != 0.0f);

  // ¿Se intenta reinicializar?
  if (IsActive()) { 
	return false; 
  }

  // Se inicializa clase base
  if (Inherited::Init()) {		
	// Const
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);
	const float fXIsoPos = pWorld->GetXWorldPos();
	const float fYIsoPos = pWorld->GetYWorldPos();

	// Inicializacion de flags de movimiento
	m_ExInfo.bIsHorizMov = false;
	m_ExInfo.bIsVertMov = false;

	// Comando de mov. en horizontal	
	if (fXEndPos > fXIsoPos) {
	  m_ExInfo.bIsHorizMov = true;
	  m_ExInfo.HorizMov.Init(IsoDefs::EAST_INDEX, uwSpeed, fXEndPos - fXIsoPos);
	} else if (fXEndPos < fXIsoPos) {
	  m_ExInfo.bIsHorizMov = true;
	  m_ExInfo.HorizMov.Init(IsoDefs::WEST_INDEX, uwSpeed, fXIsoPos - fXEndPos);
	}

	// Comando para el mov. en vertical
	if (fYEndPos > fYIsoPos) {
	  m_ExInfo.bIsVertMov = true;
	  m_ExInfo.VertMov.Init(IsoDefs::SOUTH_INDEX, uwSpeed, fYEndPos - fYIsoPos);
	} else if (fYEndPos < fYIsoPos) {
	  m_ExInfo.bIsVertMov = true;
	  m_ExInfo.VertMov.Init(IsoDefs::NORTH_INDEX, uwSpeed, fYIsoPos - fYEndPos);
	}
  }

  // Se devuelve resultado de inicializacion
  return IsActive();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia.
// Parametros:
// Devuelve:
// Notas:
// - Se hara una llamada al motor isometrico para que vuelva a habilitar el
//   interfaz (en caso de que no fuera posible, se dejara en responsabilidad
//   del motor isometrico).
///////////////////////////////////////////////////////////////////////////////
void
CIsoTravellingCmd::End(void)
{
  // Se termina instancia
  if (IsActive()) {
	// Se finalizan comandos	
	m_ExInfo.HorizMov.End();
	m_ExInfo.VertMov.End();

	// Se finaliza instancia
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ejecuta el comando. Primero se comprobara si hay algun comando pendiente 
//   de ser insertado en la cola. Habra que tener en cuenta, que siempre se
//   ejecutaran los movimientos de travelling haciendo primero movimiento en
//   horizontal y luego en vertical. El comando finalizara cuando ambos
//   comandos de movimiento hayan acabado.
// Parametros:
// Devuelve:
// Notas:
// - No sera necesario utilizar fDelta, pues este comando coordina las 
//   funcionalidades de otros
///////////////////////////////////////////////////////////////////////////////
void
CIsoTravellingCmd::Execute(const float fDelta)
{
  // SOLO si comando activo
  ASSERT(IsActive());

  // ¿Hay pendiente algun movimiento para insertar en la cola?
  if (m_ExInfo.bIsHorizMov) {
	SYSEngine::GetCommandManager()->PostCommand(&m_ExInfo.HorizMov);
	m_ExInfo.bIsHorizMov = false;
  } else if (!m_ExInfo.HorizMov.IsActive() && m_ExInfo.bIsVertMov){
	SYSEngine::GetCommandManager()->PostCommand(&m_ExInfo.VertMov);
	m_ExInfo.bIsVertMov = false;
  }

  // Se comprueba si hay que finalizar la ejecucion.
  if (!m_ExInfo.HorizMov.IsActive() && 
	  !m_ExInfo.VertMov.IsActive()) { 
	End(); 
  }
}
