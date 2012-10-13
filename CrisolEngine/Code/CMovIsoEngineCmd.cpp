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
// CMovIsoEngineCmd.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CMovIsoEngineCmd.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CMovIsoEngineCmd.h"

#include "SYSEngine.h"
#include "iCWorld.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el comando para mover el motor isometrico en la direccion Dir,
//   con la velocidad uwSpeed y la distancia a recorrer fDistance. En caso de 
//   estar procesando otra orden y no haber acabado, la peticion se guardara 
//   para usarse al acabar la orden actual.
// Parametros:
// - Dir. Direccion de movimiento.
// - uwSpeed. Velocidad con la que realizar el scroll.
// - fDistance. Distancia a recorrer
// Devuelve:
// - Si todo va bien true.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CMovIsoEngineCmd::Init(const IsoDefs::eDirectionIndex& Dir, 
					   const word uwSpeed,
					   const float fDistance)
{ 
  // ¿Se esta procesando una orden?
  if (IsActive()) { 	
	// Se almacena la peticion como peticion pendiente	
	m_PendingInfo.bPending = true;
	m_PendingInfo.Dir = Dir;
	m_PendingInfo.uwSpeed = uwSpeed;
	m_PendingInfo.fDistance = fDistance;
	/**/
	return true;
  }

  // Se procede a inicializar la clase
  if (Inherited::Init()) {  
	// Se inicializan vbles de miembro
	m_Interpolation.Init(0.0f, fDistance, fDistance / float(uwSpeed));
	m_ExInfo.Dir = Dir;
	m_ExInfo.uwSpeed = uwSpeed;  

	// Se baja flag de orden pendiente
	m_PendingInfo.bPending = false;
  }

  // Se toma interfaz almotor isometrico
  m_pWorld = SYSEngine::GetWorld();
  ASSERT(m_pWorld);
  
  // Se retorna resultado inicializacion
  return IsActive();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CMovIsoEngineCmd::End(void)
{
  if (IsActive()) {	
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ejecuta el movimiento de scroll utilizando interpolacion.
// Parametros:
// - fDelta. Delta que queda para que termine el intervalo de tiempo hasta que
//   se produzca un tick de AI.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CMovIsoEngineCmd::Execute(const float fDelta)
{
  // SOLO si comando inicializado y activo
  ASSERT(IsActive()); 

  // Se realiza interpolacion y movimiento
  const bool bInterEnd = m_Interpolation.Interpolate(fDelta);
  const bool bDoMovement = DoMovement(m_Interpolation.GetLastStep());

  // ¿Finalizo el mov. actual o la interpolacion?
  if (!bDoMovement || bInterEnd) {
	// ¿Hay movimiento pendiente?
	if (m_PendingInfo.bPending) { 
	  PreparePendingMove(32); 
	} else { 
	  End(); 
	}
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba cual es la direccion de movimiento actual y realiza la llamada
//   al metodo adecuado del motor isometrico para realizar scroll.
// Parametros:
// - Offset. Offset con el que se movera el motor isometrico.
// Devuelve:
// - Si se realizo movimiento true. En caso contrario false, como indicativo
//   de que el movimiento ha finalizado
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CMovIsoEngineCmd::DoMovement(const float Offset)
{
  // SOLO si comando activo
  ASSERT(IsActive());

  // Se realiza el movimiento segun sea la direccion actual
  bool bMove = false;
  switch (m_ExInfo.Dir) {
  case IsoDefs::NORTH_INDEX:	
	bMove = m_pWorld->MoveNorth(Offset);
	break;

  case IsoDefs::NORTHEAST_INDEX:
	bMove = m_pWorld->MoveNorth(Offset);
	bMove |= m_pWorld->MoveRight(Offset);
	break;

  case IsoDefs::EAST_INDEX:
	bMove = m_pWorld->MoveRight(Offset);		
	break;

  case IsoDefs::SOUTHEAST_INDEX:
	bMove = m_pWorld->MoveSouth(Offset);		
	bMove |= m_pWorld->MoveRight(Offset);
	break;

  case IsoDefs::SOUTH_INDEX:
	bMove = m_pWorld->MoveSouth(Offset);		
	break;

  case IsoDefs::SOUTHWEST_INDEX:
	bMove = m_pWorld->MoveSouth(Offset);		
	bMove |= m_pWorld->MoveLeft(Offset);		
	break;

  case IsoDefs::WEST_INDEX:
	bMove = m_pWorld->MoveLeft(Offset);		
	break;

  case IsoDefs::NORTHWEST_INDEX:
	bMove = m_pWorld->MoveNorth(Offset);
	bMove |= m_pWorld->MoveLeft(Offset);
	break;	
  };

  // Se retorna resultado
  return bMove;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el movimiento que se halla pendiente y actualiza los valores
//   de ejecucion para reflejar la peticion pendiente.
// Parametros:
// - fActPos. Posicion en la que se ha producido la ejecucion de la peticion
//   pendiente.
// Devuelve:
// Notas:
// - La posicion inicial se establecera desde 0 si el movimiento pendiente es
//   distinto al actual y desde la posicion alcanzada con anterioridad, si el
//   movimiento pendinte enlaza con la misma direccion que el movimiento 
//   en curso.
///////////////////////////////////////////////////////////////////////////////
void 
CMovIsoEngineCmd::PreparePendingMove(const float fActPos)
{
  // SOLO si hay movimiento pendiente
  ASSERT(m_PendingInfo.bPending);
 
  m_ExInfo.Dir = m_PendingInfo.Dir;
  m_ExInfo.uwSpeed = m_PendingInfo.uwSpeed;

  // Se actualizan los datos sobre el movimiento    
  if (m_ExInfo.Dir == m_PendingInfo.Dir) {	 
	 m_Interpolation.Init(0.0f, 
						  m_PendingInfo.fDistance, 
						  (m_PendingInfo.fDistance / float(m_ExInfo.uwSpeed)) + 
						  m_Interpolation.GetRemainingTime()); 
  } else {
	m_Interpolation.Init(0.0f, 
						 m_PendingInfo.fDistance, 
						 m_PendingInfo.fDistance / float(m_ExInfo.uwSpeed));	
  } 
  
  // Se baja el flag de movimiento pendiente
  m_PendingInfo.bPending = false;
}