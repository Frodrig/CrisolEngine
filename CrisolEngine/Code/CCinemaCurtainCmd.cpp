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
// CCinemaCurtainCmd.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CCinemaCurtainCmd.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CCinemaCurtainCmd.h"

#include "SYSEngine.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia, guardando la intancia a la cortina de cine y
//   estableciendo los pasos de apertura y cierre en funcion de los frames
//   de actualizacion recibidos.
// Parametros:
// - pCinemaCurtain. Instancia a la cortina de cine.
// - uwFPS. Frames por segundo de actualiacion.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializacion
///////////////////////////////////////////////////////////////////////////////
bool 
CCinemaCurtainCmd::Init(CCinemaCurtain* const pCinemaCurtain,
						const word uwFPS)
{
  // SOLO si parametros validos
  ASSERT(pCinemaCurtain);
  ASSERT(pCinemaCurtain->IsInitOk());
  ASSERT(uwFPS);

  // ¿Se intenta reinicializar?
  if (Inherited::IsActive()) { 
	return false; 
  }

  // Se inicializa clase base
  if (!Inherited::Init()) { 
	return false; 
  }

  // Se inicializa interpolacion. 
  // En 1 seg. se debera de actualizar tantos frames
  m_fTime = 1.0f / float(uwFPS);
  m_Interpolation.Init(0.0f, 1.0f, m_fTime);  

  // Se establecen resto de vbles de miembro
  m_uwFPS = uwFPS;
  m_pCinemaCurtain = pCinemaCurtain;

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCinemaCurtainCmd::End(void)
{
  // Finaliza la instancia si procede
  if (Inherited::IsActive()) {	
	// Finaliza clase base
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realizara la operacion de interpolacion, devolviendo true en caso de que
//   haya finalizado y false en cualquier otro caso. Cuando la interpolacion
//   finalice, se recargara automaticamente.
// Parametros:
// - fDelta. Delta para realizar la actualizacion en la interpolacion.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CCinemaCurtainCmd::DoInterpolation(const float fDelta)
{
  // SOLO si instancia activa
  ASSERT(Inherited::IsActive());

  // ¿Hay que recargar la interpolacion?
  if (m_Interpolation.Interpolate(fDelta)) {
	// Si, se recarga
	m_Interpolation.Init(0.0f, 1.0f, m_fTime + m_Interpolation.GetRemainingTime());	
	return true;
  }

  // No, se retorna
  return false;
}