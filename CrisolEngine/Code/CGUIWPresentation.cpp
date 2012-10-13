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
// CGUIWPresentation.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWPresentation.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWPresentation.h"

#include "SYSEngine.h"
#include "iCAudioSystem.h"
#include "iCResourceManager.h"
#include "iCGameDataBase.h"
#include "iCGUIManager.h"
#include "iCInputManager.h"
#include "iCAlarmManager.h"
#include "iCWorld.h"
#include "iCLogger.h"
#include "CMemoryPool.h"
#include "CCBTEngineParser.h"
#include "CGUICSingleText.h"

// Declaracion de estructuras forward
struct CGUIWPresentation::sNText {
  // Info asociada a un texto
  CGUICSingleText Text; // Texto asociado
  // Pool de memoria
  static CMemoryPool m_MPool;
  static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
  static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); }  
};

// Pools de memoria
CMemoryPool 
CGUIWPresentation::sNText::m_MPool(1, sizeof(CGUIWPresentation::sNText), true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Incializa instancia.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWPresentation::Init(void)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  #ifdef ENGINE_TRACE  
    SYSEngine::GetLogger()->Write("CGUIWPresentation::Init> Interfaz de presentaciones.\n");
  #endif


  // Se inicializa clase base
  if (Inherited::Init()) {
	// Se toman instancias a distintos subsistemas
	m_pAudioSys = SYSEngine::GetAudioSystem();
	ASSERT(m_pAudioSys);
	m_pResManager = SYSEngine::GetResourceManager();
	ASSERT(m_pResManager);
	m_pFileSys = SYSEngine::GetFileSystem();
	ASSERT(m_pFileSys);
	m_pGDBase = SYSEngine::GetGameDataBase();
	ASSERT(m_pGDBase);
	m_pAlarmManager = SYSEngine::GetAlarmManager();
	ASSERT(m_pAlarmManager);

	// Todo correcto
	#ifdef ENGINE_TRACE  
	  SYSEngine::GetLogger()->Write("                       | Ok.\n");
	#endif
	return true;
  }

  // Hubo problemas
  End();
  #ifdef ENGINE_TRACE  
    SYSEngine::GetLogger()->Write("                       | Error.\n");
  #endif
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPresentation::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	// Desactiva y propaga
	Deactive();
	#ifdef ENGINE_TRACE  
	  SYSEngine::GetLogger()->Write("CGUIWPresentation::End> Finalizado.\n");
	#endif
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa la instancia
// Parametros:
// - szPresentationProfile. Nombre del perfil asociado a la presentacion.
// - PrevInterfaz. Interfaz previo al que se debera volver al finalizar.
// - pGUIWClient. Cliente asociado a la interfaz.
// Devuelve:
// Notas:
// - Se comprobara via ASSERT que se venga de alguno de los interfaces
//   validos que pueden activar la ventana de presentacion.
// - El FadeOut inicial se utilizar para suavizar los cambios desde el
//   MainInterfaz y PlayerProfile a este interfaz, pues desde MainInterfaz, 
//   existe la posibilidad de asociar un FadeOut de finalizacion.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPresentation::Active(const std::string& szPresentationProfile,
						  const GUIManagerDefs::eGUIWindowType& PrevInterfaz,
						  iCGUIWindowClient* const pGUIWClient)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  ASSERT(pGUIWClient);

  // ¿La instancia no esta activada?
  if (!Inherited::IsActive()) {
	// Se oculta el cursor
	SYSEngine::GetGUIManager()->SetGUICursor(GUIManagerDefs::NO_CURSOR);	

	// Se pausa el universo de juego si procede
	iCWorld* const pWorld = SYSEngine::GetWorld();		
	pWorld->SetPause(true);  

	// Se toma interfaz previo
	ASSERT((PrevInterfaz == GUIManagerDefs::GUIW_MAINMENU ||
			PrevInterfaz == GUIManagerDefs::GUIW_MAININTERFAZ ||
			PrevInterfaz == GUIManagerDefs::GUIW_PLAYERPROFILE) != 0);			
	m_PrevInterfaz = PrevInterfaz;

	// Se guarda informacion de fichero MIDI previo (si es que hay)
	m_PrevMusic.bPrevMIDIMusic = m_pAudioSys->GetMIDIInfo(m_PrevMusic.szMIDIFileName,
														  m_PrevMusic.MIDIPlayMode);

	// Se guarda informacion del sonido ambiente	
	m_PrevMusic.bAmbientWAVSound = pWorld->IsAmbientWAVSoundActive();
	if (m_PrevMusic.bAmbientWAVSound) {
	  m_PrevMusic.szAmbientWAVFileName = pWorld->GetAmbientWAVSound();
	  pWorld->SetAmbientWAVSound("");
	}
		
	// Se inicializan valores de informacion para distintos componentes
	m_MIDIMusic.szMIDIFileName = "";
	m_WAVSoundInfo.hWAVCyclic = 0;
	m_WAVSoundInfo.hWAVLinear = 0;

	// Se establece slide de inicio
	m_swActSlide = -1;

	// Se leen el numero de slides
	SYSEngine::PassToString(m_szProfileName, 
							"[Profile<%s>]", 
							szPresentationProfile.c_str());			
	CCBTEngineParser* const pParser = m_pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_PRESENTATION_PROFILES,
														      m_szProfileName);
	ASSERT(pParser);
	pParser->SetVarPrefix("GenConfig.");
	m_uwNumSlides = pParser->ReadNumeric("NumberOfSlides");

	// Se lee el flag que habilita o no el uso de la tecla ESC
	m_bESCKeyActive = pParser->ReadFlag("ESCKeyActiveFlag");

	// Se lee el tiempo maximo de visualizacion
	// Nota: Si el tiempo es menor que 1.0f, el pase sera SOLO manual
	m_AutomaticPass.hAlarm = 0;
	m_AutomaticPass.fMaxVisTime = pParser->ReadNumeric("MaxSlideVisualizationTime", false);
	if (m_AutomaticPass.fMaxVisTime < 1.0f) {
	  m_AutomaticPass.fMaxVisTime = 0.0f;
	}

	// Se indica que va a existir un Fade(Out) activo
	m_bFadeActive = true;

	// Se obliga a que el slide 0 tenga asociado velocidad de FadeIn
	m_ActSlideFadeInfo.bFadeOutActive = true;

	// Se lee el FadeOut inicial
	// Nota: En caso de no existir, se simulara su existencia y finalizacion
	// Este codigo SIEMPRE ira al final de este metodo para evitar efectos laterales
	pParser->SetVarPrefix("");
	const GraphDefs::sRGBColor RGBFadeOut(pParser->ReadRGBColor("Fade.Start.", false));
	if (pParser->WasLastParseOk()) {
	  // Se leyo informacion, luego se realizara el FadeOut, leyendose la velocidad
	  Inherited::DoFadeOut(CGUIWPresentation::ID_INITFADEOUT, 
						   RGBFadeOut, 
						   pParser->ReadNumeric("Fade.In.Speed"));
	} else {
	  // No se leyo, luego se simulara que finalizo
	  EndCmdNotify(CGUIWPresentation::ID_INITFADEOUT, 0, 0);
	}

	// Se propaga
	Inherited::Active(pGUIWClient);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva instancia, finalizando y liberando todo tipo de recursos que 
//   se hallen activos.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPresentation::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Desactiva si procede
  if (Inherited::IsActive()) {
	// Libera posible alarma asociada, para el pase automatico
	UninstallAlarm();

	// Finaliza imagen de fondo
	m_BackImgInfo.AnimBackImg.End();

	// ¿Habia musica MIDI previa?
	if (m_PrevMusic.bPrevMIDIMusic) {
	  // Si, se reproduce
	  m_pAudioSys->PlayMIDIMusic(m_PrevMusic.szMIDIFileName,
								 m_PrevMusic.MIDIPlayMode);
	} else {
	  // No, se detiene la que pudiera estar sonando
	  m_pAudioSys->StopMIDIMusic();
	}

	// ¿Habia sonido WAV Ambiente previo?
	if (m_PrevMusic.bAmbientWAVSound) {
	  // Si, se establece
	  SYSEngine::GetWorld()->SetAmbientWAVSound(m_PrevMusic.szAmbientWAVFileName);
	}	

	// Libera posibles recursos de sonido activos
	if (m_WAVSoundInfo.hWAVCyclic) {
	  m_pResManager->ReleaseWAVSound(m_WAVSoundInfo.hWAVCyclic);
	}
	if (m_WAVSoundInfo.hWAVLinear) {
	  m_pResManager->ReleaseWAVSound(m_WAVSoundInfo.hWAVLinear);
	}

	// Libera todo el texto que pudiera estar asociado
	RemoveAllTextInfo();

	// Se quita la pausa del universo de juego
	SYSEngine::GetWorld()->SetPause(false);  

	// Propaga
	Inherited::Deactive();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece la ventana de interfaz que proceda, deacuerdo a los valores
//   indicados en la activacion.
// - Este metodo se llamara cuadno se alcance la ultima slide o bien se 
//   pulse a la tecla ESC.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPresentation::SetPrevInterfazWindow(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si instancia activa
  ASSERT(Inherited::IsActive());

  // Si, se notifica al GUIManager el cambio de interfaz
  switch (m_PrevInterfaz) {
    case GUIManagerDefs::GUIW_MAINMENU: {
	  SYSEngine::GetGUIManager()->SetMainMenuWindow();
    } break;

    case GUIManagerDefs::GUIW_MAININTERFAZ: {
	  SYSEngine::GetGUIManager()->SetMainInterfazWindow();
    } break;

    case GUIManagerDefs::GUIW_PLAYERPROFILE: {
	  SYSEngine::GetGUIManager()->SetPlayerProfileWindow();
    } break;

	default:
	  ASSERT_MSG(false, "El estado previo no es valido");
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo encargado de coordinar el paso de slide. Cuando se este en el 
//   ultimo slide, se pondra en contacto con el GUIManager para cambiar de
//   estado de interfaz.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPresentation::NextSlide(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());  
  
  // Incrementa slide actual
  ++m_swActSlide;

  // ¿Se ha llegado hasta el ultimo slide?
  if (m_swActSlide == m_uwNumSlides) {
	// Si, se notifica al GUIManager el cambio de interfaz
	SetPrevInterfazWindow();
  } else {
	// No, se obtiene parser y se establece prefijo de lectura	
	CCBTEngineParser* const pParser = m_pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_PRESENTATION_PROFILES,
														      m_szProfileName);
	ASSERT(pParser);
	std::string szPrefix;
	SYSEngine::PassToString(szPrefix, "Slide[%d].", m_swActSlide);
	pParser->SetVarPrefix(szPrefix);

	// Se procede a leer la informacion asociada al slide actual
	TLoadSlideBackImg(pParser);
	TLoadMIDIMusic(pParser);
	TLoadWAVSound(pParser);
	TLoadTextInfo(pParser);
	TLoadFadeOutInfo(pParser);	
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la imagen de fondo asociado con el slide recibido.
// - La informacion del slide anterior se sabra por como este inicializado
//   el componente AnimBackImg. 
// - El proceso de mantenimiento sera asi:
//    * Si se esta leyendo el Slide 0, obligatoriamente debera de existir una
//      imagen.
//    * Si el slide actual no es 0 y no existe imagen, se mantendra la actual y
//      si existe y es distinta a la anterior, se finalizara la anterior asi
//      como sus animaciones asociadas. En caso de ser igual, las posibles 
//      animaciones asociadas se añadiran a esta.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// Notas:
// - El prefijo de pParser apuntara al slide actual
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPresentation::TLoadSlideBackImg(CCBTEngineParser* const pParser)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(pParser);

  // Vbles
  CGUICAnimBitmap::sGUICBitmapInitData CfgData; // Cfg de la imagen de fondo

  // Se lee el nombre de la imagen asociada al slide actual
  CfgData.BitmapInfo.szFileName = pParser->ReadString("BackImage.FileName", false);
  if (!pParser->WasLastParseOk()) {
	// No se leyo informacion, se mantendra la informacion anterior PERO debera 
	// de cumplirse que el slide actual no sea el slide 0
	if (0 == m_swActSlide) {
	  SYSEngine::FatalError("No se ha especificado imagen de fondo en Slide 0. Perfil: %s\n", m_szProfileName.c_str());
	}
  } else {
	// ¿Es distinta imagen de fondo? o
	// ¿Se esta en slide 0? (y por lo tanto, no hay imagen de fondo asociada)
	// Nota: El nombre de la imagen se mantiene en minusculas en el componente
	SYSEngine::MakeLowercase(CfgData.BitmapInfo.szFileName);	
	if (0 == m_swActSlide ||
	    (0 != CfgData.BitmapInfo.szFileName.compare(m_BackImgInfo.AnimBackImg.GetBitmapFileName()))) {
	  // Si, se finaliza instancia anterior	  
	  m_BackImgInfo.AnimBackImg.End();	  
	  
	  // Se carga nueva imagen	  
	  CfgData.ID = CGUIWPresentation::ID_BACKIMG;	  
	  CfgData.BitmapInfo.Bpp = GraphDefs::BPP_16;
	  CfgData.BitmapInfo.ABpp = GraphDefs::ALPHA_BPP_0;
	  m_BackImgInfo.AnimBackImg.Init(CfgData);
	  ASSERT(m_BackImgInfo.AnimBackImg.IsInitOk());	  
	  
	  // Se activa
	  m_BackImgInfo.AnimBackImg.SetActive(true);
	}
  }

  // Se procede a cargar los posibles animaciones asociadas
  word uwIt = 0;
  for (; uwIt < CGUICAnimBitmap::MAX_ANIMS; ++uwIt) {
	// Se establece el nombre de la animacion actual a leer
	std::string szAnim;
	SYSEngine::PassToString(szAnim, "Anim[%u].", uwIt);

	// Se lee plantilla de animacion
	const std::string szAnimTemplate(pParser->ReadStringID(szAnim + "AnimTemplate", false));	
	if (pParser->WasLastParseOk()) {
	  // Hay informacion, se lee la restante y se asocia animacion
	  // Nota: La posicion sera obligatoria y el alpha optativo	  
	  if (!m_BackImgInfo.AnimBackImg.AddAnim(szAnimTemplate, 
											 pParser->ReadPosition(szAnim), 
											 pParser->ReadAlpha(szAnim, false))) {
		SYSEngine::FatalError("CGUIWPresentation> Problemas añadiendo la plantilla %s", szAnimTemplate.c_str());
	  }
	} else {
	  // No hay informacion para animacion actual, se abandona
	  break;
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la informacion musical asociada al slide actual.
// - En primer lugar se mirara si esta activo el flag de parada de musica. Si
//   lo esta, se ordenara detenerse a la musica actual. En caso contrario,
//   se leera el nombre del fichero MIDI actual. Si no hay, se seguira con el
//   estado actual y si hay, se leera, a su vez, el flag de modo de reproduccion
//   ciclico. Con estos datos, se comprobara el estado actual de MIDI y si es
//   distinto, se asociara el nuevo estado leido.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// Notas:
// - El prefijo de pParser apuntara al slide actual
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPresentation::TLoadMIDIMusic(CCBTEngineParser* const pParser)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(pParser);

  // Se lee el flag de detencion (por defecto false)
  if (pParser->ReadFlag("MIDIMusic.StopFlag", false)) {
	// Se detiene el MIDI actual
	m_pAudioSys->StopMIDIMusic();
	// Se elimina cualquier tipo de nombre de fichero
	m_MIDIMusic.szMIDIFileName = "";
  } else {
	// No hay que detener el MIDI actual, se lee posible nuevo fichero
	std::string szNewMIDIFile(pParser->ReadString("MIDIMusic.FileName", false));	
	if (pParser->WasLastParseOk()) {
	  // Se encontro un nombre de fichero, se lee flag de reproduccion ciclica
	  const bool bCyclicPlay = pParser->ReadFlag("MIDIMusic.CyclicPlayFlag", false);

	  // ¿El archivo a reproducir es distinto al actual?
	  SYSEngine::MakeLowercase(szNewMIDIFile);
	  if ((0 != szNewMIDIFile.compare(m_MIDIMusic.szMIDIFileName)) ||
		  m_MIDIMusic.bCyclicPlay != bCyclicPlay) {
		// Si, luego se ordena la nueva reproduccion
		m_pAudioSys->PlayMIDIMusic(szNewMIDIFile, 
								   bCyclicPlay ? AudioDefs::MIDI_PLAY_LOOP : AudioDefs::MIDI_PLAY_NORMAL);
	
		// Se guardan valores leidos
		m_MIDIMusic.szMIDIFileName = szNewMIDIFile;
		m_MIDIMusic.bCyclicPlay = bCyclicPlay;
	  }
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la informacion asociada a la reproduccion de ficheros WAV.
// - La reproduccion siempre sera de dos formas:
//  * WAVLineales. En este caso, los WAVs se reproduciran una sola vez y jamas
//    podran usarse los que provengan de slide anterior, ya que quedaran cortados.
//    Bastara indicar el nombre del WAV que se desea reproducir. Si no se halla
//    nombre, no se reproducira nada.
//   * WAVCiclicos. En primer lugar se leera un flag de parada de WAV ciclico.
//     Dicho flag servira para indicar que si hay un WAV ciclico sonando de la
//     anterior slide, debera de finalizar. En caso de no hallarse dicho flag,
//     se leera el posible nombre del WAV ciclico asociado al slide actual. Si no
//     se hallara o fuera igual que el actual, entonces se entendera que se
//     quiere seguir con el WAV actual. En caso de que sea distinto, se entendera
//     que se desea finalizar el actual y hacer sonar el nuevo.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// Notas:
// - El prefijo de pParser apuntara al slide actual
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPresentation::TLoadWAVSound(CCBTEngineParser* const pParser)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(pParser);

  // Se trabaja con los WAVs lineales
  // Fin del posible WAV lineal actual
  if (m_WAVSoundInfo.hWAVLinear) {
	m_pResManager->ReleaseWAVSound(m_WAVSoundInfo.hWAVLinear);
	m_WAVSoundInfo.hWAVLinear = 0;
  }

  // Se carga el posible nuevo sonido WAV lineal
  std::string szWAVFileName(pParser->ReadString("WAVSound.Linear.FileName", false));
  if (pParser->WasLastParseOk()) {
	// Se registra WAV y se hace sonar
	m_WAVSoundInfo.hWAVLinear = m_pResManager->WAVSoundRegister(szWAVFileName);
	if (!m_WAVSoundInfo.hWAVLinear) {
	  SYSEngine::FatalError("No se pudo abrir el fichero de sonido \"%s\".\nFichero inexistente o incompatible.\n", szWAVFileName.c_str());
	  return;
	}
	ASSERT(m_WAVSoundInfo.hWAVLinear);
	m_pAudioSys->PlayWAVSound(m_WAVSoundInfo.hWAVLinear, 
							  AudioDefs::WAV_PLAY_NORMAL);
  }

  // Se trabaja con WAVs ciclicos
  // Se lee el flag de finalizacion de reproduccion de WAV ciclico
  if (pParser->ReadFlag("WAVSound.Cyclic.StopFlag", false)) {
	// Se finaliza el WAV ciclico actual si procede
	if (m_WAVSoundInfo.hWAVCyclic) {
	  m_pResManager->ReleaseWAVSound(m_WAVSoundInfo.hWAVCyclic);
	  m_WAVSoundInfo.hWAVCyclic = 0;
	}
  } else {
	// No hay flag de finalizacion, se lee si se desea cargar un nuevo WAV	
	szWAVFileName = pParser->ReadString("WAVSound.Cyclic.FileName", false);
	if (pParser->WasLastParseOk()) {
	  // Se leyo nombre fichero
	  // Se finaliza posible sonido WAV anterior
	  if (m_WAVSoundInfo.hWAVCyclic) {
		m_pResManager->ReleaseWAVSound(m_WAVSoundInfo.hWAVCyclic);
	  }
	  
	  // Se registra nuevo sonido WAV y se reproduce ciclicamente
	  m_WAVSoundInfo.hWAVCyclic = m_pResManager->WAVSoundRegister(szWAVFileName);
	  if (!m_WAVSoundInfo.hWAVCyclic) {
		SYSEngine::FatalError("No se pudo abrir el fichero de sonido \"%s\".\nFichero inexistente o incompatible.\n", szWAVFileName.c_str());
		return;
	  }
	  m_pAudioSys->PlayWAVSound(m_WAVSoundInfo.hWAVCyclic, AudioDefs::WAV_PLAY_LOOP);
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la informacion asociada al texto.
// - El texto podra tomarse de dos modos diferentes. Por un lado desde archivo
//   y por el otro directamente desde el perfil de la presentacion.
// Parametros:
// - pParser. Parser a utilizar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPresentation::TLoadTextInfo(CCBTEngineParser* const pParser)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(pParser);

  // Se libera el posible texto registrado del slide anterior
  RemoveAllTextInfo();

  // Vble donde cargar la configuracion del texto
  CGUICSingleText::sGUICSingleTextInitData CfgData; // Datos de configuracion

  // Se establecen valores basicos
  CfgData.ID = CGUIWPresentation::ID_TEXT;
  CfgData.szFont = "Arial";  

  // Se procede a cargar texto
  bool bOk = true;
  word uwIt = 0;
  for (; bOk; ++uwIt) {
	// Se estAblece prefijo del texto a leer
	std::string szText;
	SYSEngine::PassToString(szText, "Text[%u].", uwIt);
	
	// Se lee la posicion
	// Nota: En caso de no hallar info, se entendera que no hay texto asociado
	CfgData.Position = pParser->ReadPosition(szText, false);
	if (pParser->WasLastParseOk()) {
	  // Se lee color y anchura
	  CfgData.RGBSelectColor = pParser->ReadRGBColor(szText);
	  CfgData.RGBUnselectColor = CfgData.RGBSelectColor;
	  CfgData.uwWidthJustify = pParser->ReadNumeric(szText + "AreaWidth");

	  // Se lee justificacion
	  // Nota: El orden derecha, centro e izquierda y de no hallarse a
	  // izquierda por defecto siempre.
	  CGUICSingleText::eDrawJustify DrawJustify;
	  if (pParser->ReadFlag(szText + "Justify.CenterFlag", false)) {
		// Justificacion central
		DrawJustify = CGUICSingleText::JUSTIFY_CENTER;
	  } else if (pParser->ReadFlag(szText + "Justify.RightFlag", false)) {
		// Justificacion a la derecha
		DrawJustify = CGUICSingleText::JUSTIFY_RIGHT;
	  } else {
		// Justificacion a izquierda
		DrawJustify = CGUICSingleText::JUSTIFY_LEFT;
	  }
	  
	  // Se establece el texto
	  CfgData.szText = pParser->ReadString(szText + "Text", false);
	  if (!pParser->WasLastParseOk()) {
		// No hay texto definido en el archivo de perfil de presentaciones,
		// por lo que DEBERA de estar registrado en un archivo
		// Se lee el nombre del archivo y se abre
		const std::string szTextFileName(pParser->ReadString(szText + "TextFileName"));
		const FileDefs::FileHandle hFile = m_pFileSys->Open(szTextFileName, false);
		ASSERT(hFile);
		
		// Se lee contenido y se guarda
		const dword udFileSize = m_pFileSys->GetFileSize(hFile);
		ASSERT(udFileSize);
		sbyte* pReadBuff = new sbyte[udFileSize + 1];
		ASSERT(pReadBuff);
		const dword udRead = m_pFileSys->Read(hFile, pReadBuff, udFileSize, 0);
		ASSERT((udRead == udFileSize));
		pReadBuff[udFileSize] = '\0';				
		CfgData.szText = "";
		CfgData.szText.assign(pReadBuff);					
		
		// Se cierra fichero y se libera buffer
		delete[udFileSize + 1] pReadBuff;
		m_pFileSys->Close(hFile);
	  }			

	  // Se crea nuevo nodo con el componente de texto
	  sNText* const pNText = new sNText;
	  ASSERT(pNText);

	  // Se inicializa, justifica, registra y activa
	  pNText->Text.Init(CfgData);	  
	  ASSERT(pNText->Text.IsInitOk());
	  pNText->Text.DrawJustifyTo(DrawJustify);
	  m_TextInfo.ActiveText.push_back(pNText);
	  pNText->Text.SetActive(true);
	} else {
	  // No, se interrumpe busqueda
	  bOk = false;
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la informacion de FadeOut asociada al slide actual.
// Parametros:
// - pParser. Parser a utilizar
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPresentation::TLoadFadeOutInfo(CCBTEngineParser* const pParser)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(pParser);

  // La velocidad del FadeIn sera obligatoria, se el SlideAnterior 
  // tenia un valor asociado de FadeOut.
  // Siempre sera obligatoria para el primer slide.
  m_ActSlideFadeInfo.uwFadeInSpeed = pParser->ReadNumeric("Fade.In.Speed",
														  m_ActSlideFadeInfo.bFadeOutActive);

  // Se carga el valor RGB del FadeOut del slide actual
  m_ActSlideFadeInfo.RGBFadeOut = pParser->ReadRGBColor("Fade.Out.", false);

  // Si se leyo informacion, se obtiene la velocidad de fade y se
  // establece el flag de fade existente
  if (pParser->WasLastParseOk()) {
	m_ActSlideFadeInfo.bFadeOutActive = true;
	m_ActSlideFadeInfo.uwFadeOutSpeed = pParser->ReadNumeric("Fade.Out.Speed");	
  } else {
	m_ActSlideFadeInfo.bFadeOutActive = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera toda la informacion de texto que estuviera asociada.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPresentation::RemoveAllTextInfo(void)
{
  // SOLO si intancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Recorre la lista con el texto asociado, borrando instancias y nodos
  TextListIt It(m_TextInfo.ActiveText.begin());
  for (; It != m_TextInfo.ActiveText.end(); It = m_TextInfo.ActiveText.erase(It)) {
	// Borra instancia
	delete *It;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado de todos los componentes.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPresentation::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Dibuja si procede  
  if (Inherited::IsActive() && 
	  m_BackImgInfo.AnimBackImg.IsInitOk()) {
	// Imagen animada de fondo
	m_BackImgInfo.AnimBackImg.Draw();
	// Texto asociado
	TextListIt It(m_TextInfo.ActiveText.begin());
	for (; It != m_TextInfo.ActiveText.end(); ++It) {	  
	  (*It)->Text.Draw();
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a los eventos de interfaz.
// - Solo se atendera si no hay un Fade activo.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWPresentation::DispatchEvent(const InputDefs::sInputEvent& aInputEvent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si instancia activada
  ASSERT(Inherited::IsActive());

  // ¿Hay un fade activo?
  if (m_bFadeActive) {
	// Se retorna
	return InputDefs::SHARE_INPUT_EVENT;
  }

  // Evalua el evento producido 
  switch(aInputEvent.EventType) {
    case InputDefs::BUTTON_EVENT: {
  	  // Pulsacion de tecla
	  switch(aInputEvent.EventButton.Code) {
	    case InputDefs::IE_KEY_ESC: {		  	  	  
		  // Se ha pulsado la tecla ESC, se cambiara de interfaz si procede
		  if (m_bESCKeyActive) {
			SetPrevInterfazWindow();
		  }
		} break;

		case InputDefs::IE_BUTTON_MOUSE_LEFT: {		
		  // Se ha pulsado el boton izq. del raton.
		  // Se desinstalara POSIBLE alarma asociada
		  UninstallAlarm();
		  // Logica asociada a la orden de pasar de slide
		  OnNextSlide();
		} break;
  	  }; // ~ switch
	 } break;
  }; // ~ switch

  // En caso de que la ventana este desactivada, el evento se compartira siempre
  return InputDefs::NO_SHARE_INPUT_EVENT;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Instala los eventos de interfaz asociados. Estos eventos seran:
//    * Tecla ESC -> para finalizar la ventana
//    * El boton izquierdo del raton -> Para pasar de slide.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPresentation::InstallClient(void)
{
  // Se establecen eventos de entrada
  iCInputManager* const pInputManager = SYSEngine::GetInputManager();
  ASSERT(pInputManager);
  pInputManager->SetInputEvent(InputDefs::IE_BUTTON_MOUSE_LEFT,
						       this,
							   InputDefs::BUTTON_PRESSED_DOWN);
  pInputManager->SetInputEvent(InputDefs::IE_KEY_ESC, 
							   this, 
							   InputDefs::BUTTON_PRESSED_DOWN);
  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desinstala los eventos de entrada
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPresentation::UnistallClient(void)
{
  // Se quitan eventos de entrada
  iCInputManager* const pInputManager = SYSEngine::GetInputManager();
  ASSERT(pInputManager);  
  pInputManager->UnsetInputEvent(InputDefs::IE_BUTTON_MOUSE_LEFT, 
								 this,
							     InputDefs::BUTTON_PRESSED_DOWN);
  pInputManager->UnsetInputEvent(InputDefs::IE_KEY_ESC, 
								 this,
							     InputDefs::BUTTON_PRESSED_DOWN);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe la notificacion del gestor de alarmas. La notificacion unicamente
//   podra deberse a que ha vencido el temporizador que indica el tiempo
//   maximo de visualizacion de un slide.
// Parametros:
// - AlarmType. Tipo de alarma.
// - hAlarm. Handle a la alarma.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPresentation::AlarmNotify(const AlarmDefs::eAlarmType& AlarmType,
							   const AlarmDefs::AlarmHandle& hAlarm)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(hAlarm);
  ASSERT((AlarmType == AlarmDefs::TIME_ALARM) != 0);

  // Se desvincula handle a alarma
  m_AutomaticPass.hAlarm = 0;

  // Logica asociada a la orden de pasar de slide
  OnNextSlide();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encargara de preparar el siguiente slide. Este metodo se llamara cuando
//   se pulse a pasar el slide actual o finalice la alarma asociada al pase
//   automatico.
// - Internamente comprobara si hay un FadeOut para el slide actual. Si lo hay
//   lo activara. En caso contrario, si el slide actual no es el ultimo
//   instalara la posible alarma existente y pasara a sig. slide.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGUIWPresentation::OnNextSlide(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // ¿El slide actual tiene FadeOut asociado?
  if (m_ActSlideFadeInfo.bFadeOutActive) {
	// Si, luego se activa el FadeOut y se levanta el flag de FadeOut activo
	Inherited::DoFadeOut(CGUIWPresentation::ID_FADEOUT,
						 m_ActSlideFadeInfo.RGBFadeOut,
						 m_ActSlideFadeInfo.uwFadeOutSpeed);
	m_bFadeActive = true;
  } else {
	// ¿El slide actual NO ES el ultimo slide?
	if (m_swActSlide < (m_uwNumSlides - 1)) {
	  // Se instala la POSIBLE alarma asociada
	  InstallAlarm();
	} 
	// Se pasa de slide	
	NextSlide();
  }
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion de la finalizacion de un comando.
// - Se utilizara para conocer cuando ha finalizado el fade de finalizacion y
//   asi realizar la accion que sea pertinente. Segun la accion que se pulsara,
//   asi se asignara el comando de fade.
// Parametros:
// - IDCommand. ID del comando.
// - udInstant. Instante de finalizacion.
// - udExtraParam. Parametro extra.
// Devuelve:
// - Si todo bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWPresentation::EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
								const dword udInstant,
								const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se comprueba codigo de fade recibido
  switch(IDCommand) {
	case CGUIWPresentation::ID_FADEIN: {
	  // Fin del FadeIn.
	  // Se bajara flag de FadeActivo y se instalara POSIBLE alarma asociada
	  m_bFadeActive = false;
	  InstallAlarm();
	} break;
	  
	case CGUIWPresentation::ID_INITFADEOUT: {
	  // Finalizo el FadeOut inicial
	  // Se realizara un NextSlide e instalara el FadeIn	  
	  NextSlide();
	  Inherited::DoFadeIn(CGUIWPresentation::ID_FADEIN, 
						  m_ActSlideFadeInfo.uwFadeInSpeed);
  	  // Por ultimo, se notifica al GUIManager de que ha finalizado el
	  // primer FadeOut, pasando el identificador del interfaz previo.
	  // Con esta notificacion, el GUIManager desactivara los interfaces
	  // que procedan para que no se dibujen (MainInterfaz y PlayerProfile)
	  Inherited::GUIWindowNotify(m_PrevInterfaz);
	} break;

	case CGUIWPresentation::ID_FADEOUT: {
	  // FadeOut tipico.
	  // En caso de no estar en el ultimo slide, pasaremos de slide e instalaremos
	  // un FadeIn. En caso contrario, simplemente pasaremos de slide	  	  	  
	  NextSlide();
	  if (m_swActSlide < m_uwNumSlides) {
		// Se hara un FadeIn
		Inherited::DoFadeIn(CGUIWPresentation::ID_FADEIN, 
							m_ActSlideFadeInfo.uwFadeInSpeed);		
	  }
	} break;
 }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Instala la POSIBLE alarma asociada para el paso automatico de Slides
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGUIWPresentation::InstallAlarm(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si no hay alarma instalada
  ASSERT((0 == m_AutomaticPass.hAlarm) != 0);

  // Se instala la alarma de pase automatico si procede
  if (m_AutomaticPass.fMaxVisTime > 0.0f) {
    m_AutomaticPass.hAlarm = m_pAlarmManager->InstallTimeAlarm(this, 
															   m_AutomaticPass.fMaxVisTime);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desinstala la posible alarma asociada para el paso de slides
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGUIWPresentation::UninstallAlarm(void)
{
   // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se desintala si procede
  if (m_AutomaticPass.hAlarm) {
	m_pAlarmManager->UninstallAlarm(m_AutomaticPass.hAlarm);
	m_AutomaticPass.hAlarm = 0;
  }
}