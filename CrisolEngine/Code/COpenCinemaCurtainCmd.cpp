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
// COpenCinemaCurtainCmd.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar COpenCinemaCurtainCmd.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "COpenCinemaCurtainCmd.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia, pasando toda la responsabilidad a la clase base
// Parametros:
// - pCinemaCurtain. Instancia a la cortina de cine.
// - uwFPS. Frames por segundo de actualiacion.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializacion
///////////////////////////////////////////////////////////////////////////////
bool 
COpenCinemaCurtainCmd::Init(CCinemaCurtain* const pCinemaCurtain,
							const word uwFPS)
{
  // SOLO si parametros validos
  ASSERT(pCinemaCurtain);
  ASSERT(pCinemaCurtain->IsInitOk());
  ASSERT(uwFPS);

  // ¿Se intenta reinicializar?
  if (Inherited::IsActive()) { return false; }

  // Se inicializa clase base
  if (!Inherited::Init(pCinemaCurtain,
					   uwFPS)) { 
	return false; 
  }

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
COpenCinemaCurtainCmd::End(void)
{
  // Finaliza la instancia si procede
  if (Inherited::IsActive()) {	
	// Finaliza clase base
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la apertura de la cortina de cine. El comando finalizara 
//   automaticamente cuando el estado de la cortina de cine sea abierto.
// Parametros:
// - fDelta. Delta para realizar la actualizacion en la interpolacion.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
COpenCinemaCurtainCmd::Execute(const float fDelta)
{
  // SOLO si instancia activa
  ASSERT(Inherited::IsActive());

  // Se realiza interpolacion y se actualiza si procede
  if (Inherited::DoInterpolation(fDelta)) {
	// Se realiza actualizacion
	if (Inherited::OpenCurtain()) {
	  // Comando finalizado
	  End();
	}
  }
}