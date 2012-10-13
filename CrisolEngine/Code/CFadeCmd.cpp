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
// CFadeCmd.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CFadeCmd.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CFadeCmd.h"

#include "SYSEngine.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el comando de Fade para realizar el efecto sobre una estructura
//   alpha.
// Parametros:
// - pAlphaSrc. Direccion de la estructura alpha sobre la que realizar el fade.
// - AlphaDest. Estructura alpha que alcanzar.
// - uwFPS. Velocidad en la realizacion del efecto.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CFadeCmd::Init(GraphDefs::sAlphaValue* const pAlphaSrc, 
			   const GraphDefs::sAlphaValue& AlphaDest,
			   const word uwFPS) 
{
  // SOLO si parametros validos
  ASSERT(pAlphaSrc);
  ASSERT(uwFPS);

  // Se finaliza por si acaso hay intento de reinicializar
  End();
  
  // Se procede a inicializar el comando
  if (Inherited::Init()) {
	// Se almacena info sobre el comando
	m_CmdInfo.Type = CFadeCmd::ALPHA_FADE;
	m_CmdInfo.StepType[0] = *pAlphaSrc < AlphaDest ? sCmdInfo::INC_STEP : sCmdInfo::DEC_STEP;
	m_CmdInfo.uwFPS = uwFPS;
	m_CmdInfo.uwStep = uwFPS * SYSEngine::GetIASECTickTime();
	if (!m_CmdInfo.uwStep) { 
	  m_CmdInfo.uwStep = 1; 
	}
	m_CmdInfo.AlphaInfo.pAlphaSrc = pAlphaSrc;
	m_CmdInfo.AlphaInfo.AlphaDest = AlphaDest;
	m_CmdInfo.fTime = 1.0f / float(uwFPS);
	m_Interpolation.Init(0.0f, 1.0f, m_CmdInfo.fTime);
  }

  // Se retorna resultado de la inicializacion
  return Inherited::IsActive();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el comando de Fade para realizar el efecto sobre una estructura
//   rgb.
// Parametros:
// - pRGBSrc. Direccion de la estructura rgb sobre la que realizar el fade.
// - RGBDest. Estructura rgba que alcanzar.
// - uwFPS. Velocidad en la realizacion del efecto.
// - ubIt. La cantidad a restar o añadir en cada actualizacion. Por defecto 1.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CFadeCmd::Init(GraphDefs::sRGBColor* const pRGBSrc, 
			   const GraphDefs::sRGBColor& RGBDest,
			   const word uwFPS)
{  
  // SOLO si parametros validos
  ASSERT(pRGBSrc);
  ASSERT(uwFPS);

  // Se finaliza por si acaso hay intento de reinicializar
  End();
  
  // Se procede a inicializar el comando
  if (Inherited::Init()) {
	// Se almacena info sobre el comando
	m_CmdInfo.Type = CFadeCmd::RGB_FADE;
	m_CmdInfo.StepType[0] = pRGBSrc->ubRed < RGBDest.ubRed ? sCmdInfo::INC_STEP : sCmdInfo::DEC_STEP;
	m_CmdInfo.StepType[1] = pRGBSrc->ubGreen < RGBDest.ubGreen ? sCmdInfo::INC_STEP : sCmdInfo::DEC_STEP;
	m_CmdInfo.StepType[2] = pRGBSrc->ubBlue < RGBDest.ubBlue ? sCmdInfo::INC_STEP : sCmdInfo::DEC_STEP;
	m_CmdInfo.uwFPS = uwFPS;
	m_CmdInfo.uwStep = uwFPS * SYSEngine::GetIASECTickTime();
	if (!m_CmdInfo.uwStep) { 
	  m_CmdInfo.uwStep = 1; 
	}
	m_CmdInfo.RGBInfo.pRGBSrc = pRGBSrc;
	m_CmdInfo.RGBInfo.RGBDest = RGBDest;
	m_CmdInfo.fTime = 1.0f / float(uwFPS);
	m_Interpolation.Init(0.0f, 1.0f, m_CmdInfo.fTime);	
  }

  // Se retorna resultado de la inicializacion
  return Inherited::IsActive();  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia del comando, dejandolo inactivo.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CFadeCmd::End(void)
{
  // Finaliza si procede
  if (Inherited::IsActive()) {
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la ejecucion del comando. Para ello, comprobara primeramente si
//   la estructura asociada es Alpha o RGB y en virtud de ello, actualizara el
//   contador de resta para las mismas.
// Parametros:
// - fDelta. Delta de actualizacion del tick
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CFadeCmd::Execute(const float fDelta)
{
  // SOLO si comando inicializado y activo
  ASSERT(Inherited::IsActive());
 
  // Se compara la posicion actual con la anterior y se pasan frames     
  if (m_Interpolation.Interpolate(fDelta)) {
	// Hay que efectuar operacion de fade
	switch (m_CmdInfo.Type) {
	  case ALPHA_FADE:
		// Realiza el fade de alpha
		if (AlphaFade()) {
		  End();		  
		  return;
		}
	  break;

	  case RGB_FADE:
		// Realiza el fade en color
		if (ColorFade()) {
		  End();
		  return;
		}
		break;
	}; // ~ switch

	// Fade aun no concluido; se recarga interpolacion	
	m_Interpolation.Init(0.0f, 1.0f, m_CmdInfo.fTime);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el Fade de alpha.
// Parametros:
// Devuelve:
// - Si el fade concluyo true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CFadeCmd::AlphaFade(void)
{
  // Vbles
  sword swActFade = m_CmdInfo.AlphaInfo.pAlphaSrc->ubAlpha; // Valor actual de fade
		
  // Se actualiza el fade
  if (m_CmdInfo.StepType[0] == sCmdInfo::INC_STEP) {
    swActFade += m_CmdInfo.uwStep;		 
    // Clipping
    if (swActFade > m_CmdInfo.AlphaInfo.AlphaDest.ubAlpha) {
  	  *m_CmdInfo.AlphaInfo.pAlphaSrc = m_CmdInfo.AlphaInfo.AlphaDest;
	} else {
	  m_CmdInfo.AlphaInfo.pAlphaSrc->ubAlpha = swActFade;
	}
  } else {
    swActFade -= m_CmdInfo.uwStep;		  
    // Clipping
    if (swActFade < m_CmdInfo.AlphaInfo.AlphaDest.ubAlpha) {
		*m_CmdInfo.AlphaInfo.pAlphaSrc = m_CmdInfo.AlphaInfo.AlphaDest;
	} else {
	  m_CmdInfo.AlphaInfo.pAlphaSrc->ubAlpha = swActFade;
	}
  }

  // ¿Fade completo?
  return *m_CmdInfo.AlphaInfo.pAlphaSrc == m_CmdInfo.AlphaInfo.AlphaDest;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el Fade de color
// Parametros:
// Devuelve:
// - Si el fade concluyo true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CFadeCmd::ColorFade(void)
{
  // Vbles
  sword swActValue; // Valor actual de un componente de color

  // Realiza actualizacion componente rojo, segun proceda, y con clipping
  swActValue = m_CmdInfo.RGBInfo.pRGBSrc->ubRed;
  if (m_CmdInfo.StepType[0] == sCmdInfo::INC_STEP) {	
	swActValue += m_CmdInfo.uwStep;	
	m_CmdInfo.RGBInfo.pRGBSrc->ubRed = (swActValue > m_CmdInfo.RGBInfo.RGBDest.ubRed) ?
										m_CmdInfo.RGBInfo.RGBDest.ubRed : swActValue;	
  } else {	
	swActValue -= m_CmdInfo.uwStep;	
	m_CmdInfo.RGBInfo.pRGBSrc->ubRed = (swActValue < m_CmdInfo.RGBInfo.RGBDest.ubRed) ?
										m_CmdInfo.RGBInfo.RGBDest.ubRed : swActValue;
  }

  // Realiza actualizacion componente verde segun proceda, y con clipping
  swActValue = m_CmdInfo.RGBInfo.pRGBSrc->ubGreen;
  if (m_CmdInfo.StepType[1] == sCmdInfo::INC_STEP) {	
	swActValue += m_CmdInfo.uwStep;	
	m_CmdInfo.RGBInfo.pRGBSrc->ubGreen = (swActValue > m_CmdInfo.RGBInfo.RGBDest.ubGreen) ?
										  m_CmdInfo.RGBInfo.RGBDest.ubGreen : swActValue;	
  } else {	
	swActValue -= m_CmdInfo.uwStep;	
	m_CmdInfo.RGBInfo.pRGBSrc->ubGreen = (swActValue < m_CmdInfo.RGBInfo.RGBDest.ubGreen) ?
										  m_CmdInfo.RGBInfo.RGBDest.ubGreen : swActValue;
  }

  // Realiza actualizacion componente azul segun proceda, y con clipping
  swActValue = m_CmdInfo.RGBInfo.pRGBSrc->ubBlue;
  if (m_CmdInfo.StepType[2] == sCmdInfo::INC_STEP) {	
	swActValue += m_CmdInfo.uwStep;	
	m_CmdInfo.RGBInfo.pRGBSrc->ubBlue = (swActValue > m_CmdInfo.RGBInfo.RGBDest.ubBlue) ?
										 m_CmdInfo.RGBInfo.RGBDest.ubBlue : swActValue;	
  } else {	
	swActValue -= m_CmdInfo.uwStep;	
	m_CmdInfo.RGBInfo.pRGBSrc->ubBlue = (swActValue < m_CmdInfo.RGBInfo.RGBDest.ubBlue) ?
										 m_CmdInfo.RGBInfo.RGBDest.ubBlue : swActValue;
  }

  // ¿Fade completo?
  return (*m_CmdInfo.RGBInfo.pRGBSrc == m_CmdInfo.RGBInfo.RGBDest);	
}


