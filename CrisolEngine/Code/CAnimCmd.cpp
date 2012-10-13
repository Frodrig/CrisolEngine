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
// CAnimCmd.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CAnimCmd.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CAnimCmd.h"

#include "SYSEngine.h"
#include "iCAnimTemplate.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el comando de animacion tomando la plantilla a animar asi como
//   preparando valores internos.
// Parametros:
// - pSprite. Sprite sobre el que realizar la animacion.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CAnimCmd::Init(iCAnimTemplate* const pAnimTemplate)
{
  // SOLO si parametros validos
  ASSERT(pAnimTemplate);
  
  // Se finalizara si se intenta reinicializar
  End();

  // Se procede a inicializar
  if (Inherited::Init()) {
	// Se obtiene sprite
	m_CmdInfo.pAnimTemplate = pAnimTemplate;
	// Se establece distancia a interpolar segun frames
	m_CmdInfo.fTime = 1.0f / float(m_CmdInfo.pAnimTemplate->GetFPS());
	m_Interpolation.Init(0.0f, 1.0f, m_CmdInfo.fTime);
  }

  // Se retorna resultado operacion
  return IsActive();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instacia.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAnimCmd::End(void)
{
  // Procede a finalizar la instancia
  if (Inherited::IsActive()) {
	// Finaliza clase base
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la ejecucion del comando de animacion
// Parametros:
// - fDelta. Delta con el que ajustar el tiempo transcurrido.
// Devuelve:
// Notas:
// - Si la interpolacion ha finalizado, GetRemainingTime devolvera siempre un
//   valor menor o igual a 0, luego para evitar tener que llamar a una funcion
//   que nos lo pase a valor absoluto, simplemente se lo sumaremos al tiempo
//   original.
///////////////////////////////////////////////////////////////////////////////
void 
CAnimCmd::Execute(const float fDelta)
{
  // SOLO si comando inicializado y activo
  ASSERT(IsActive());

  // Se realiza la interpolacion
  if (m_Interpolation.Interpolate(fDelta)) {
	// Interpolacion finalizada, pasamos de frame
	m_CmdInfo.pAnimTemplate->NextFrame();
	
	// Establecemos valor para la siguiente interpolacion
	m_Interpolation.Init(0.0f, 1.0f, m_CmdInfo.fTime + m_Interpolation.GetRemainingTime());
  } 
}

