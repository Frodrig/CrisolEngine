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
// CGUIWGameLoading.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWGameLoading.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWGameLoading.h"

#include "SYSEngine.h"
#include "iCGraphicSystem.h"
#include "iCLogger.h"
#include "iCWorld.h"
#include "iCGameDataBase.h"
#include "iCGUIManager.h"
#include "iCFontSystem.h"
#include "iCMathUtil.h"
#include <map>
#include "GraphDefs.h"
#include "CCBTEngineParser.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia. En caso de que pPlayer sea NULL, significara que
//   se desea cargar un juego desde un archivo de partida salvada.
// Parametros:
// - uwID. Identificador del area a cargar o del archivo de partida guardada
// - pPlayer. Instancia al jugador.
// - PlayerPos. Posicion del jugador en el area a cargar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false
// Notas:
// - No se dejara reinicializar
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWGameLoading::Init(const word uwID,
					   CPlayer* const pPlayer,
					   const AreaDefs::sTilePos& PlayerPos)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) { 
	return false; 
  }
  
  #ifdef ENGINE_TRACE
     SYSEngine::GetLogger()->Write("CGUIWGameLoading::Init> Inicializando ventana indicativa de carga de jueog.\n");	 
  #endif

  // Se toman instancias a otros subsistemas
  m_pGraphSys = SYSEngine::GetGraphicSystem();
  ASSERT(m_pGraphSys);
  m_pFontSys = SYSEngine::GetFontSystem();
  ASSERT(m_pFontSys);

  // Se inicializa clase base
  if (Inherited::Init()) { 
	// Carga datos desde disco inicializando los componentes de GUI
	if (TLoad()) { 
	  // Se guardan datos
	  m_InitInfo.uwID = uwID;
	  m_InitInfo.pPlayer = pPlayer;
	  m_InitInfo.PlayerPos = PlayerPos;
	  
	  // Todo correcto
	  #ifdef ENGINE_TRACE
		SYSEngine::GetLogger()->Write("                      | Ok.\n");
	  #endif
	  return true;	
	}
  }  

  // Finaliza inicializacion con problemas  
  #ifdef ENGINE_TRACE
     SYSEngine::GetLogger()->Write("                      | Error.\n");
  #endif
  End();
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameLoading::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	#ifdef ENGINE_TRACE
      SYSEngine::GetLogger()->Write("CGUIWGameLoading::End> Finalizando ventana indicativa de carga de un área.\n");
	#endif

	// Se desactivan componentes si procede
	Deactive();

    // Finaliza imagen de fondo
	m_BackImg.BackImg.End();
	 
	// Finaliza clase base
	Inherited::End();
	#ifdef ENGINE_TRACE
      SYSEngine::GetLogger()->Write("                     | Ok.\n");
	#endif
  }
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la carga de datos desde disco.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CGUIWGameLoading::TLoad(void)
{
  // Obtiene el parser a utilizar situandolo en la seccion correspondiente
  iCGameDataBase* const pGDBase = SYSEngine::GetGameDataBase();
  ASSERT(pGDBase);
  CCBTEngineParser* const pParser = pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
													      "[GameLoading]");
  ASSERT(pParser);

  // Procede a cargar cada una de las subsecciones
  // Imagen de fondo
  if (!TLoadBackImgInfo(pParser)) { 
	return false; 
  }
  
  // Fade
  if (!TLoadFadeValues(pParser)) { 
	return false; 
  } 

  // Mensaje de carga de area
  m_szAreaLoadingMsg = SYSEngine::GetGameDataBase()->GetStaticText(GameDataBaseDefs::ST_GENERAL_GAME_LOADING);
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga desde disco la imagen de fondo y la inicializa
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - El interfaz para la muestra de una imagen cargando, tendra la posibilidad
//   de definir varias imagenes de fondo, de esta forma, la imagen de fondo
//   se escogera de modo aleatorio.
///////////////////////////////////////////////////////////////////////////////
bool
CGUIWGameLoading::TLoadBackImgInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Se procesan todos los nombres de imagenes y se almacenan en un map  
  typedef std::map<byte, std::string> StrMap;
  typedef StrMap::iterator            StrMapIt;
  typedef StrMap::value_type          StrMapValType;
  StrMap BackImgMap;
  byte uwIt = 0;  
  pParser->SetVarPrefix("BackImage");  
  for (; uwIt < 256; ++uwIt) {
	// Se intenta leer nombre de la imagen
	std::string szImgIdx;
	SYSEngine::PassToString(szImgIdx, "[%u].FileName", uwIt);
	const std::string szImage(pParser->ReadString(szImgIdx, false));

	// ¿Se leyo la imagen?
	if (pParser->WasLastParseOk()) {
	  // Si, se guarda en el map 
	  BackImgMap.insert(StrMapValType(uwIt, szImage));
	} else {
	  // No, se abandona
	  #ifdef _SYSASSERT
		if (0 == uwIt) {
		  SYSEngine::FatalError("No hay ninguna imagen para GameLoading definida.");
		}
	  #endif
	  break;
	}
  }  

  // Se escoge de forma aleatoria el nombre de la imagen
  iCMathUtil* const pMathUtil = SYSEngine::GetMathUtil();
  ASSERT(pMathUtil);
  pMathUtil->GenSeed();  
  const word uwRandValue = pMathUtil->GetRandValue(0, 255);
  const StrMapIt It(BackImgMap.find(uwRandValue % BackImgMap.size()));
  ASSERT((It != BackImgMap.end()) != 0);
  ASSERT(It->second.size());
 
  // Se completan datos para incializar la imagen de fondo
  CGUICBitmap::sGUICBitmapInitData BackImgData;
  BackImgData.BitmapInfo.szFileName = It->second;
  BackImgData.ID = CGUIWGameLoading::ID_BACKIMG;
  BackImgData.ubDrawPlane = 1;
  BackImgData.BitmapInfo.WidthTextureDim = GraphDefs::TEXTURE_DIM_32;
  BackImgData.BitmapInfo.HeightTextureDim = GraphDefs::TEXTURE_DIM_32;
  BackImgData.BitmapInfo.Bpp = GraphDefs::BPP_16;
  BackImgData.BitmapInfo.ABpp = GraphDefs::ALPHA_BPP_0;

  // Se realiza la inicializacion
  if (!m_BackImg.BackImg.Init(BackImgData)) {
	return false;
  }
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga los valores asociados a los fades de la ventana.
// Parametros:
// - pParser-> Parser a utilizar.
// Devuelve:
// - Si todo va bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CGUIWGameLoading::TLoadFadeValues(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Valores de FadeIn
  pParser->SetVarPrefix("Fade.");
  m_FadeInfo.FadeInSpeed = pParser->ReadNumeric("In.Speed");

  // Valores de FadeOut
  m_FadeInfo.RGBFadeOut = pParser->ReadRGBColor("Out.");
  m_FadeInfo.FadeOutSpeed = pParser->ReadNumeric("Out.Speed");

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el interfaz y a todos sus componentes.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameLoading::Active(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se activa si procede
  if (!Inherited::IsActive()) {   
	// Activa imagen de fondo
	m_BackImg.BackImg.SetActive(true);

	// Se oculta el cursor de GUI
	SYSEngine::GetGUIManager()->SetGUICursor(GUIManagerDefs::NO_CURSOR);
  
	// Se realiza el FadeIn
	Inherited::DoFadeIn(CGUIWGameLoading::ID_FADEIN_END, 
						m_FadeInfo.FadeInSpeed);
  
	// Se propaga activacion
	Inherited::Active();
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva los componentes.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameLoading::Deactive(void)
{  
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se desactiva si procede
  if (Inherited::IsActive()) {
	// Desactiva imagen de fondo
	m_BackImg.BackImg.SetActive(false);

	// Se propaga desactivacion
	Inherited::Deactive();
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado de todos los componentes.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameLoading::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se dibuja si procede
  if (Inherited::IsActive()) {
	// Imagen de fondo
	m_BackImg.BackImg.Draw();

	// Texto indicativo de carga
	m_pFontSys->Write(5, 
					  m_pGraphSys->GetVideoHeight() - 16,
					  2,
					  m_szAreaLoadingMsg);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe la notificacion de la finalizacion de comandos. En este caso en
//   concreto se utilizara para saber cuando ha terminado el fade inicial.
//   En ese momento, 
// Parametros:
// - IDCommand. Identificador del comando.
// - udInstant. Instante de notificacion.
// - udExtraParam. Parametro extra.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWGameLoading::EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
							   const dword udInstant,
							   const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se evalua codigo de comando
  switch(IDCommand) {
	case CGUIWGameLoading::ID_FADEIN_END: {
	  // Comando FadeIn concluido	  
	  // ¿Hay jugador asociado?
	  if (m_InitInfo.pPlayer) {
		// Si, se esta comenzando una nueva partida
		SYSEngine::GetWorld()->StartWorld(m_InitInfo.uwID, 
									      m_InitInfo.pPlayer, 
										  m_InitInfo.PlayerPos);	  	  		
	  } else {
		// No, se esta recuperando una partida guardada
		SYSEngine::GetWorld()->LoadGame(m_InitInfo.uwID);		
	  }

	  // Se activa el FadeOut
	  Inherited::DoFadeOut(CGUIWGameLoading::ID_FADEOUT_END,
						   m_FadeInfo.RGBFadeOut,
						   m_FadeInfo.FadeOutSpeed);
	} break;

	case CGUIWGameLoading::ID_FADEOUT_END: {
	  // Comando de FadeOut finalizado
	  // Se establece estado de juego
	  SYSEngine::SetInGameState();
	} break;

	default:
	  ASSERT(false);
  };
}
