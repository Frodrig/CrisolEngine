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
// CCyclicFadeCmd.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CCyclicFadeCmd.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CCyclicFadeCmd.h"

#include "SYSEngine.h"
#include "iCLogger.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el comando de fade para una estructura Alpha, apoyandose en el 
//   comando m_FadeCmd y guardando los valores introducidos. Tambien realizara
//   la configuracion para saber cual sera el siguiente fade a realizar cuando
//   concluya el clico iniciado.
// Parametros:
// - pAlphaSrc. Direccion de la estructura alpha sobre la que realizar fade.
// - AlphaDest. Estructura alpha destino
// - uwFPS. Velocidad del fade.
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
// - No se permitira reinicializacion
///////////////////////////////////////////////////////////////////////////////
bool 
CCyclicFadeCmd::Init(GraphDefs::sAlphaValue* const pAlphaSrc, 
					 const GraphDefs::sAlphaValue& AlphaDest,
					 const word uwFPS)
{
  // ¿Se intenta reinicializar?
  if (IsActive()) { 
	return false; 
  }

  // Se procede a inicializar
  if (Inherited::Init()) {
	// ¿Se puede inicializar comando de Fade embebido?
	if (m_FadeCmd.Init(pAlphaSrc, AlphaDest, uwFPS)) {
	  // Se guarda informacion
	  m_CmdInfo.Type = CCyclicFadeCmd::ALPHA_FADE;
	  m_CmdInfo.uwFPS = uwFPS;	  
	  m_CmdInfo.AlphaInfo.pAlphaSrc = pAlphaSrc;
	  m_CmdInfo.AlphaInfo.AlphaDest = AlphaDest;
	  m_CmdInfo.AlphaInfo.AlphaSrc = *pAlphaSrc;

	  // Se determina cual es el siguiente fade a realizar
	  m_ExInfo.pAlphaNext = &m_CmdInfo.AlphaInfo.AlphaSrc;
	} else {
	  End();
	}
  } else {
	End();
  }

  // Se retorna resultado inicializacion
  return IsActive();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el comando de fade para una estructura RGB, apoyandose en el 
//   comando m_FadeCmd y guardando los valores introducidos. Tambien realizara
//   la configuracion para saber cual sera el siguiente fade a realizar cuando
//   concluya el clico iniciado.
// Parametros:
// - pAlphaSrc. Direccion de la estructura alpha sobre la que realizar fade.
// - AlphaDest. Estructura alpha destino
// - uwFPS. Velocidad del fade.
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
// - No se permitira reinicializacion
///////////////////////////////////////////////////////////////////////////////
bool 
CCyclicFadeCmd::Init(GraphDefs::sRGBColor* const pRGBSrc, 
					 const GraphDefs::sRGBColor& RGBDest,
					 const word uwFPS)
{
  // ¿Se intenta reinicializar?
  if (IsActive()) { 
	return false; 
  }

  // Se procede a inicializar
  if (Inherited::Init()) {
	// ¿Se puede inicializar comando de Fade embebido?	
	if (m_FadeCmd.Init(pRGBSrc, RGBDest, uwFPS)) {
	  // Se guarda informacion
	  m_CmdInfo.Type = CCyclicFadeCmd::RGB_FADE;
	  m_CmdInfo.uwFPS = uwFPS;
	  m_CmdInfo.RGBInfo.pRGBSrc = pRGBSrc;
	  m_CmdInfo.RGBInfo.RGBDest = RGBDest;
	  m_CmdInfo.RGBInfo.RGBSrc = *pRGBSrc;

	  // Se determina cual es el siguiente fade a realizar
	  m_ExInfo.pRGBNext = &m_CmdInfo.RGBInfo.RGBSrc;
	} else {
	  End();
	}
  } else {
	End();
  }

  // Se retorna resultado inicializacion
  return IsActive();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la ejecucion del comando.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCyclicFadeCmd::End(void)
{
  // Finaliza si procede
  if (IsActive()) {
	// Finaliza comando de apoyo
	m_FadeCmd.End();

	// Finaliza base
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la ejecucion del comando. Internamente, pasara la responsabilidad
//   al comando de fade y cuando este acabe, se volvera a inicializar teniendo
//   en cuenta la siguiente estructura destino y actualizando esta para la 
//   proxima vez que el comando finalice.
// Parametros:
// - fDelta. Delta de ejecucion para implementar interpolacion.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCyclicFadeCmd::Execute(const float fDelta)
{
  // SOLO si instancia inicializada
  ASSERT(IsActive());

  // Se ejecuta comando de fade 
  m_FadeCmd.Execute(fDelta);

  // ¿Se consiguio llegar al final del fade?
  if (!m_FadeCmd.IsActive()) {
	// Hay que volver a realizar el fade pero en el otro sentido
	switch (m_CmdInfo.Type) {
	  case CCyclicFadeCmd::ALPHA_FADE:
		// Fade sobre alphas
		// Se inicia el fade
		m_FadeCmd.Init(m_CmdInfo.AlphaInfo.pAlphaSrc, 
					   *m_ExInfo.pAlphaNext, 
					   m_CmdInfo.uwFPS);
		// Se determina cual va a ser el siguiente fade
		if (&m_CmdInfo.AlphaInfo.AlphaSrc == m_ExInfo.pAlphaNext) {
		  m_ExInfo.pAlphaNext = &m_CmdInfo.AlphaInfo.AlphaDest;
		} else {
		   m_ExInfo.pAlphaNext = &m_CmdInfo.AlphaInfo.AlphaSrc;
		}		
		break;

	  case CCyclicFadeCmd::RGB_FADE:
		// Fade sobre RGBs
		// Se inicia el fade
		m_FadeCmd.Init(m_CmdInfo.RGBInfo.pRGBSrc, 
					   *m_ExInfo.pRGBNext, 
					   m_CmdInfo.uwFPS);
		// Se determina cual va a ser el siguiente fade
		if (&m_CmdInfo.RGBInfo.RGBSrc == m_ExInfo.pRGBNext) {
		  m_ExInfo.pRGBNext = &m_CmdInfo.RGBInfo.RGBDest;
		} else {
		   m_ExInfo.pRGBNext = &m_CmdInfo.RGBInfo.RGBSrc;
		}		
		break;
	}; // ~ switch
  }
}

