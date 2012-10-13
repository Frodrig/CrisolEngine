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
// CWorldEntity.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CWorldEntity.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CShadow.h"
#include "CWorldEntity.h"

#include "SYSEngine.h"
#include "iCCommandManager.h"
#include "iCAlarmManager.h"
#include "iCWorld.h"
#include "iCAudioSystem.h"
#include "iCFileSystem.h"
#include "iCVirtualMachine.h"
#include "IsoDefs.h"
#include "CFXBack.h"

// Definicion de estructuras forward
struct CWorldEntity::sTalkInfo {  
  // Enumerados
  enum eTalkType {
    // Tipo de habla
    TEXT_ONLY,   // Solo texto
    WAV_ONLY,    // Solo sonido WAV
    TEXT_AND_WAV // Texto y sonido WAV
  };
  // Datos		
  // Texto
  CGUICSingleText                Text;        // Componente de texto
  CFXBack                        FXBack;      // FXBack para el texto a mostrar
  eTalkType                      ShowType;    // Tipo de muestra del texto  
  CWorldEntity::eTalkTextJustify TextJustify; // Justificacion del texto
  // Handles a las alarmas
  AlarmDefs::AlarmHandle hEndText;         // Alarma de fin de habla de texto
  AlarmDefs::AlarmHandle hEndWAVText;      // Alarma de fin de WAV de habla de texto
  word                   uwWavTextSeconds; // Seg. asociados al texto con WAV   
  // Wav
  ResDefs::WAVSoundHandle hWAVSound; // Handle al sonido WAV
  // Datos precalculados
  word uwXDrawOffset; // Offset para el dibujado en X
  word uwYDrawOffset; // Offset para el dibujado en Y
  // Cliente
  iCAlarmClient* pClient; // Posible cliente asociado
  // Constructor
  sTalkInfo(void): hEndText(0), 
				   hEndWAVText(0), 
				   uwWavTextSeconds(0), 
				   hWAVSound(0),
				   pClient(NULL) { }
  // Manejador de memoria
  static CMemoryPool m_MPool;
  static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
  static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
};

struct CWorldEntity::sShadowInfo {
  // Info relativa a la sombra
  CShadow Shadow; // Sombra
  // Manejador de memoria
  static CMemoryPool m_MPool;
  static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
  static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); }   
};

// Inicializacion de los manejadores de memoria
CMemoryPool 
CWorldEntity::sTalkInfo::m_MPool(4, sizeof(CWorldEntity::sTalkInfo), true);
CMemoryPool 
CWorldEntity::sPortrait::m_MPool(2, sizeof(CWorldEntity::sPortrait), true);
CMemoryPool 
CWorldEntity::sShadowInfo::m_MPool(32, sizeof(CWorldEntity::sShadowInfo), true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia recuperando datos desde disco
// Parametros:
// - hFile. Handle al fichero
// - udOffset. Offset donde comienzan a estar los datos
// - hEntity. Handle a la entidad
// Devuelve:
// - Si todo va bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CWorldEntity::Init(const FileDefs::FileHandle& hFile,
				   dword& udOffset,
				   const AreaDefs::EntHandle& hEntity)
{
  // SOLO si parametros validos
  ASSERT(hFile);
  ASSERT(hEntity);

  // Se inicializa clase base
  if (Inherited::Init(hFile, udOffset, hEntity)) {
	// Se carga informacion	
	// Nombre
	iCFileSystem* pFileSys = SYSEngine::GetFileSystem();
	ASSERT(pFileSys);
	udOffset += pFileSys->ReadStringFromBinary(hFile, udOffset, m_szName);
	
	// Plantilla de animacion del retrato
	// Se registrara si procede
	std::string szValue;
	udOffset += pFileSys->ReadStringFromBinary(hFile, udOffset, szValue);
	if (!szValue.empty()) {
	  m_pPortrait = new sPortrait;
	  ASSERT(m_pPortrait);
	  m_pPortrait->Portrait.Init(szValue);
	  ASSERT(m_pPortrait->Portrait.IsInitOk());    
	} else {
	  m_pPortrait = NULL;
	}

	// Sombra asociada
	// Se registrara si procede
	udOffset += pFileSys->ReadStringFromBinary(hFile, udOffset, szValue);
	if (!szValue.empty()) {
	  m_pShadowInfo = new sShadowInfo;
	  ASSERT(m_pShadowInfo);
	  m_pShadowInfo->Shadow.Init(szValue);
	  ASSERT(m_pShadowInfo->Shadow.IsInitOk());
	} else {
	  m_pShadowInfo = NULL;
	}

	// Se lee valor temporal asociado al evento idle y se establece
	m_IdleEventInfo.hIdleAlarm = 0;
	m_IdleEventInfo.fTime = 0;
	float fIdleScriptTime;
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(&fIdleScriptTime),
							   sizeof(float),
							   udOffset);
	SetIdleEventTime(fIdleScriptTime);

	// Se establecen resto de vbles de miembro	
	m_pTalkInfo = NULL;
	m_FadeSelect.RGBSelectFade = GraphDefs::sRGBColor(125, 125, 125);	  	  
	m_FadeSelect.bSet = false;
  }

  // Se retorna resultado
  return Inherited::IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia.
// Parametros:
// - szATSprite. Plantilla de animacion al sprite.
// - szName. Nombre del jugador.
// - szATPortrait. Plantilla de animacion con el retrato.
// - hEntity. Handle a la entidad
// - szShadow. Nombre del archivo con la sombra
// Devuelve:
// - Si todo va bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CWorldEntity::Init(const std::string& szATSprite,	
				   const AreaDefs::EntHandle& hEntity,
				   const std::string& szName,
				   const std::string& szATPortrait,
				   const std::string& szShadow)

{
  // SOLO si parametros validos
  ASSERT(hEntity);
  ASSERT(szATSprite.size());
  
  // ¿Se intenta reicinizalizar?
  if (Inherited::IsInitOk()) { 
	return false; 
  }

  // Se inicializa clase base
  if (Inherited::Init(szATSprite, hEntity)) {		
	// ¿Hay retrato asociado?
	if (!szATPortrait.empty()) {
	  // Se crea instancia y se inicializa
	  m_pPortrait = new sPortrait;
	  ASSERT(m_pPortrait);
	  m_pPortrait->Portrait.Init(szATPortrait);
	  ASSERT(m_pPortrait->Portrait.IsInitOk());
	} else {
	  m_pPortrait = NULL;
	}

	// ¿Hay sombra asociada?	
	if (!szShadow.empty()) {	  
	  // Se crea instancia
	  m_pShadowInfo = new sShadowInfo;
	  ASSERT(m_pShadowInfo);
	  m_pShadowInfo->Shadow.Init(szShadow);
	  ASSERT(m_pShadowInfo->Shadow.IsInitOk());
	} else {
	  m_pShadowInfo = NULL;
	}
	
	// Se inicializan resto de vbles de miembro
	m_IdleEventInfo.fTime = 0.0f;
	m_IdleEventInfo.hIdleAlarm = 0;	
	m_pTalkInfo = NULL;
	m_szName = szName;
	m_FadeSelect.RGBSelectFade = GraphDefs::sRGBColor(125, 125, 125);
	m_FadeSelect.bSet = false;
  }

  // Se retorna resultado
  return Inherited::IsInitOk();
}  

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWorldEntity::End(void) 
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {	
	// Se desvincula cualquier tipo de luz asociada
	SYSEngine::GetWorld()->SetLight(Inherited::GetHandle(), 0);
	
	// Libera retrato si procede
	if (m_pPortrait) {
	  delete m_pPortrait;
	  m_pPortrait = NULL;
	}
	
	// Se libera sombra si procede
	if (m_pShadowInfo) {
	  delete m_pShadowInfo;
	  m_pShadowInfo = NULL;
	}
	
	// Por si estuviera hablando, se le manda callar
	ShutUp();
	
	// Se desinstala posible evento idle
	SetIdleEventTime(0.0f);
	
	// Se propaga finalizacion
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Guarda datos propios de CWorldEntity
// Parametros:
// - hFile. Handle al fichero donde almacenar.
// - udOffset. Offset donde depositar la info.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWorldEntity::Save(const FileDefs::FileHandle& hFile, 
		           dword& udOffset)
{
  // SOLO si parametros correctos
  ASSERT(hFile);

  // Propaga
  Inherited::Save(hFile, udOffset);

  // Almacena nombre
  iCFileSystem* pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys);
  udOffset += pFileSys->WriteStringInBinary(hFile, udOffset, m_szName);

  // Plantilla de animacion del retrato
  std::string szValue;
  if (m_pPortrait) {	
	szValue = m_pPortrait->Portrait.GetAnimTemplate()->GetAnimTemplateName();
  } 
  udOffset += pFileSys->WriteStringInBinary(hFile, udOffset, szValue);
  
  // Sombra asociada
  if (m_pShadowInfo) {	
	szValue = m_pShadowInfo->Shadow.GetShadowFileName();
  } else {
	szValue = "";
  }
  udOffset += pFileSys->WriteStringInBinary(hFile, udOffset, szValue);

  // Almacena tiempo del evento script idle
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&m_IdleEventInfo.fTime),
							  sizeof(float));  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ordena a la entidad que hable usando exclusivamente texto.
// Parametros:
// - szText. Texto.
// - RGBTextColor. Color asociado al texto.
// - TextJustify. Justificacion elegida para el texto.
// - uwViewTextTime. Seg. escogidos para que se vea el texto. En caso de ser
//   0, el texto permanecera en pantalla.
// - pClient. Posible cliente a ser notificado cuando se acabe de hablar
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
// - En caso de que se este hablando (m_pTalkInfo distinto de NULL) no 
//   se realizara notificacion al cliente que pudiera estar asociado.
// - Si se esta en pausa no se podra hablar.
///////////////////////////////////////////////////////////////////////////////
bool
CWorldEntity::Talk(const std::string& szText,			
				   const GraphDefs::sRGBColor& RGBTextColor,
				   const eTalkTextJustify& TextJustify,
				   const word uwViewTextTime,
				   iCAlarmClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿NO se esta en pausa?
  if (!Inherited::IsInPause()) {
	// Se manda callar al personaje
	ShutUp();

	// Se crea memoria para el nodo del habla
	m_pTalkInfo = new sTalkInfo;
	ASSERT(m_pTalkInfo);  

	// Se configura el componente de texto
	SetTalkText(szText, RGBTextColor, TextJustify);

	// ¿Se desea mostrar el texto por un tiempo nada mas?
	if (uwViewTextTime) {
	  // Se instala alarma asociada al mismo
	  m_pTalkInfo->hEndText = SYSEngine::GetAlarmManager()->InstallTimeAlarm(this, 
																			 uwViewTextTime);
	  ASSERT(m_pTalkInfo->hEndText);
	}
  
	// Se establecen resto de datos
	m_pTalkInfo->TextJustify = TextJustify;
	m_pTalkInfo->ShowType = CWorldEntity::sTalkInfo::TEXT_ONLY;  
	m_pTalkInfo->pClient = pClient;	

	// Todo correcto
	return true;
  } 

  // No se pudo efectuar operacion
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ordena a la entidad que hable usando sonido wav y texto.
// Parametros:
// - szWAVFileName. Nombre del fichero WAV.
// - szText. Texto. Por defecto vacio.
// - TextColor. Color asociado al texto. Por defecto negro.
// - TextJustify. Justificacion para el texto.
// - uwViewTextTime. Tiempo que permanecera el texto en pantalla tan pronto
//   acabe el fichero WAV de reproducirse. En caso de ponerse 0, el texto
//   permanecera de forma indefinida en pantalla.
// - pClient. Posible cliente a ser notificado cuando se acabe de hablar
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
// - Si se esta en pausa no se podra hablar.
///////////////////////////////////////////////////////////////////////////////
bool
CWorldEntity::Talk(const std::string& szWAVFileName,
				   const std::string& szText,
				   const GraphDefs::sRGBColor& RGBTextColor,
				   const eTalkTextJustify& TextJustify,
				   const word uwViewTextTime,
				   iCAlarmClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(szWAVFileName.size());
  ASSERT(szText.size());

   // ¿NO se esta en pausa?
  if (!Inherited::IsInPause()) {
	// Se manda callar al personaje
	ShutUp();

	// Se crea memoria para el nodo del habla
	m_pTalkInfo = new sTalkInfo;
	ASSERT(m_pTalkInfo);  

	// Se configura el componente de texto
	SetTalkText(szText, RGBTextColor, TextJustify);

	// Se instala el fichero WAV y una alarma para tener la notificacion
	// del final de la reproduccion del mismo
	m_pTalkInfo->hWAVSound = SYSEngine::GetResourceManager()->WAVSoundRegister(szWAVFileName);
	if (!m_pTalkInfo->hWAVSound) {
	  SYSEngine::FatalError("No se pudo abrir el fichero de sonido \"%s\".\nFichero inexistente o incompatible.\n", szWAVFileName.c_str());
	}
	m_pTalkInfo->hEndWAVText = SYSEngine::GetAlarmManager()->InstallWAVAlarm(this, 
																			 m_pTalkInfo->hWAVSound);
	ASSERT(m_pTalkInfo->hEndWAVText);
  
	// Se establecen resto de datos
	m_pTalkInfo->uwWavTextSeconds = uwViewTextTime;
	m_pTalkInfo->TextJustify = TextJustify;
	m_pTalkInfo->ShowType = CWorldEntity::sTalkInfo::TEXT_AND_WAV;  
	m_pTalkInfo->pClient = pClient;
  
	// Se reproduce WAV
	SYSEngine::GetAudioSystem()->PlayWAVSound(m_pTalkInfo->hWAVSound,
											  AudioDefs::WAV_PLAY_NORMAL);

	// Todo correcto
	return true;
  }

  // No se pudo efectuar operacion
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Reproduce un sonido a modo de habla y sin texto.
// Parametros:
// - szWAVFileName. Nombre del fichero de sonido.
// - pClient. Posible cliente a ser notificado cuando se acabe de hablar
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
// - Si se esta en pausa no se podra hablar.
///////////////////////////////////////////////////////////////////////////////
bool
CWorldEntity::Talk(const std::string& szWAVFileName,
				   iCAlarmClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(szWAVFileName.size());
  
  // ¿NO se esta en pausa?
  if (!Inherited::IsInPause()) {
	// Se manda callar al personaje
	ShutUp();

	// Se crea memoria para el nodo del habla
	m_pTalkInfo = new sTalkInfo;
	ASSERT(m_pTalkInfo);  

	// Se instala el fichero WAV y una alarma para tener la notificacion
	// del final de la reproduccion del mismo
	m_pTalkInfo->hWAVSound = SYSEngine::GetResourceManager()->WAVSoundRegister(szWAVFileName);
	if (!m_pTalkInfo->hWAVSound) {
	  SYSEngine::FatalError("No se pudo abrir el fichero de sonido \"%s\".\nFichero inexistente o incompatible.\n", szWAVFileName.c_str());
	}
	m_pTalkInfo->hEndWAVText = SYSEngine::GetAlarmManager()->InstallWAVAlarm(this, 
																			 m_pTalkInfo->hWAVSound);
	ASSERT(m_pTalkInfo->hEndWAVText);

	// Se establecen resto de datos
	m_pTalkInfo->ShowType = CWorldEntity::sTalkInfo::WAV_ONLY;
	m_pTalkInfo->pClient = pClient;

	// Se reproduce WAV
	SYSEngine::GetAudioSystem()->PlayWAVSound(m_pTalkInfo->hWAVSound,
											  AudioDefs::WAV_PLAY_NORMAL);

	// Todo correcto
	return true;
  }

  // No se pudo efectuar operacion
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la accion de hablar por parte del personaje, liberando 
//   recursos.
// Parametros:
// Devuelve:
// Notas:
// - En el momento en que venzca la alarma asociada al fichero WAV, este se
//   liberara de memoria, por lo que si el handle es igual a 0, el recurso
//   no estara en memoria. En caso contrario estara ademas de la alarma.
// - Nunca podra darse el caso de que exista una alarma de texto y un fichero
//   WAV activo.
///////////////////////////////////////////////////////////////////////////////
void 
CWorldEntity::ShutUp(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());  

  // ¿La entidad se halla hablando?
  if (m_pTalkInfo) { 
	// Obtiene instancia al manejador de alarmas
	iCAlarmManager* const pAlarmManager = SYSEngine::GetAlarmManager();
	ASSERT(pAlarmManager);
	
	// ¿Hay alarma asociada al texto?
	if (m_pTalkInfo->hEndText) {
	  // Si, se libera
	  pAlarmManager->UninstallAlarm(m_pTalkInfo->hEndText);	  
	  
	  // Comunica fin al posible cliente
	  if (m_pTalkInfo->pClient) {
	    m_pTalkInfo->pClient->AlarmNotify(AlarmDefs::TIME_ALARM, 0);	  
	  }
	} else {
	  // No, se comprueba si hay un fichero WAV sonando
	  if (m_pTalkInfo->hWAVSound) {
		SYSEngine::GetResourceManager()->ReleaseWAVSound(m_pTalkInfo->hWAVSound);
		pAlarmManager->UninstallAlarm(m_pTalkInfo->hEndWAVText);		
	  }

	  // Comunica fin al posible cliente
	  if (m_pTalkInfo->pClient) {
	    m_pTalkInfo->pClient->AlarmNotify(AlarmDefs::WAV_ALARM, 0);	  
	  }
	}

	// Se libera de memoria el nodo
	delete m_pTalkInfo;
	m_pTalkInfo = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el texto asociado al habla, configurando el componente del habla.
// - Tambien se estableceran los offset de dibujado con respecto al texto y su
//   justificacion.
// - Por ultimo se configurara el FXBack del texto que tendra unos valores
//   por defecto de color negro y alpha leve.
// Parametros:
// - szText. Texto a asociar.
// - RGBColor. Color asociado al texto.
// - TextJustify. Justificacion del texto
// Devuelve:
// Notas:
// - Notese que la derecha y la izquierda estara siempre referida a la entidad.
///////////////////////////////////////////////////////////////////////////////
void 
CWorldEntity::SetTalkText(const std::string& szText,				
						  const GraphDefs::sRGBColor& RGBColor,
						  const eTalkTextJustify& TextJustify)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
    
  // Vbles
  CGUICSingleText::sGUICSingleTextInitData CfgText; // Cfg para el texto

  // Se configura e inicializa y activa el componente de texto  
  CfgText.szText = szText;
  CfgText.ubDrawPlane = 0;
  CfgText.ID = GUIDefs::GUIIDComponent(0x01);
  CfgText.RGBUnselectColor = RGBColor;
  CfgText.RGBSelectColor = RGBColor;
  CfgText.szFont = "Arial";
  CfgText.uwWidthJustify = 200;
  m_pTalkInfo->Text.Init(CfgText);  
  ASSERT(m_pTalkInfo->Text.IsInitOk());
  m_pTalkInfo->Text.SetActive(true);

  // Se justifica el texto al centro
  m_pTalkInfo->Text.DrawJustifyTo(CGUICSingleText::JUSTIFY_CENTER);

  // Se configuran offsets de dibujado con respecto al texto y su justificacion
  switch(TextJustify) {
	case CWorldEntity::UP_JUSTIFY: {	  
	  // Texto por encima de la cabeza
	  m_pTalkInfo->uwXDrawOffset = (m_pTalkInfo->Text.GetWidth() / 2) -
								   IsoDefs::TILE_WIDTH_DIV;
	  m_pTalkInfo->uwYDrawOffset = (IsoDefs::TILE_HEIGHT - Inherited::GetHeight()) -
							        m_pTalkInfo->Text.GetVisibleTextLines() * 
								    FontDefs::CHAR_PIXEL_HEIGHT;
	} break;

	case CWorldEntity::LEFT_JUSTIFY: {
	  // Texto a la izquierda de la entidad
	  m_pTalkInfo->uwXDrawOffset = -IsoDefs::TILE_WIDTH;
	  m_pTalkInfo->uwYDrawOffset = IsoDefs::TILE_HEIGHT - Inherited::GetHeight();
	} break;

	case CWorldEntity::RIGHT_JUSTIFY: {
	  // Texto a la derecha de la entidad
	  m_pTalkInfo->uwXDrawOffset = m_pTalkInfo->Text.GetWidth();
	  std::string szAncho;
	  m_pTalkInfo->uwYDrawOffset = IsoDefs::TILE_HEIGHT - Inherited::GetHeight();
	} break;  
  }; // ~ switch
    
  // Por ultimo se configura el FXBack.
  // Nota: La posicion se calculara en cada pasada de dibujado, por lo que en
  // la inicializacion se introducira el valor 0,0 por defecto
  m_pTalkInfo->FXBack.Init(sPosition(0,0),
						   0,
						   m_pTalkInfo->Text.GetWidthJustify(),
						   FontDefs::CHAR_PIXEL_HEIGHT * m_pTalkInfo->Text.GetVisibleTextLines(),
						   GraphDefs::sRGBColor(0, 0, 0),
						   GraphDefs::sAlphaValue(75));
  ASSERT(m_pTalkInfo->FXBack.IsInitOk());
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dibuja el texto en pantalla, justificandolo antes
// Parametros:
// - swXScreen, swYScreen. Posicion inicial de dibujado en pantalla.
// - Text. Componente de texto.
// - TextJustify. Justificacion para ese texto.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWorldEntity::DrawTalkText(const sword& swXScreen,
						   const sword& swYScreen)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si se esta hablando
  ASSERT(IsTalking());

  // Se establece la posicion del texto
  const sword swXPos = swXScreen + Inherited::GetXPos() -
				       m_pTalkInfo->uwXDrawOffset;
  const sword swYPos = swYScreen + Inherited::GetYPos() +
					   m_pTalkInfo->uwYDrawOffset;
  m_pTalkInfo->Text.SetXPos(swXPos);
  m_pTalkInfo->Text.SetYPos(swYPos);

  // Se hace lo propio con el FXBack
  m_pTalkInfo->FXBack.SetPosition(sPosition(swXPos, swYPos - 2));

  // Se dibuja el FXBack y el texto
  m_pTalkInfo->FXBack.Draw();
  m_pTalkInfo->Text.Draw();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza / detiene la seleccion de una entidad con fade. Este tipo de
//   seleccion supondra hacer un fade sobre los colores RGB de su estructura
//   haciendo a la entidad mas oscura / clara ciclicamente, de tal forma que
//   se resalte sobre el resto de elementos del entorno.
// Parametros:
// - bSet. Flag de activacion. Si true se activara si false se desactivara.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWorldEntity::SetFadeSelect(const bool bSet)
{
  // SOLO si instancia activa
  ASSERT(Inherited::IsInitOk());

  // ¿El estado a establecer es el actual?
  if (bSet == m_FadeSelect.bSet) {
	return;
  }

  // ¿Se desea activar?
  if (bSet) {
	// Si
	// Se establece el color asociado al fade
	m_FadeSelect.RGBSource = GraphDefs::sRGBColor(255, 255, 255);
	
	// Se establece los valores RGB a modificar en cada uno de los vertices
	Inherited::SetRGBColor(GraphDefs::VERTEX_0, &m_FadeSelect.RGBSource);
	Inherited::SetRGBColor(GraphDefs::VERTEX_1, &m_FadeSelect.RGBSource);
	Inherited::SetRGBColor(GraphDefs::VERTEX_2, &m_FadeSelect.RGBSource);
	Inherited::SetRGBColor(GraphDefs::VERTEX_3, &m_FadeSelect.RGBSource);
	
	// Se inicializa comando de fade y se coloca el la cola de comandos
	m_FadeSelect.FadeCmd.Init(&m_FadeSelect.RGBSource, 
							  m_FadeSelect.RGBSelectFade,
							  96);
	SYSEngine::GetCommandManager()->PostCommand(&m_FadeSelect.FadeCmd);
  } else {
	// No
	// Se desvincula el color de la entidad previa	
	Inherited::SetRGBColor(GraphDefs::VERTEX_0, NULL);
	Inherited::SetRGBColor(GraphDefs::VERTEX_1, NULL);
	Inherited::SetRGBColor(GraphDefs::VERTEX_2, NULL);
	Inherited::SetRGBColor(GraphDefs::VERTEX_3, NULL);   

	// Se finaliza comando de fade
	m_FadeSelect.FadeCmd.End();
  }

  // Se establece el nuevo estado y se retorna
  m_FadeSelect.bSet = bSet;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Sobrecarga el metodo de dibujado para tener en cuenta el dibujado del
//   texto que puedera estar asociado, luego propagara el metodo a CEntity para
//   que ahi se dibuje la entidad en si.
// Parametros:
// - swXPos, swYPos. Posicion en x,y de la pantalla donde realizar el dibujado.
// - Light. Informacion luminosa.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWorldEntity::Draw(const sword& swXPos, 
				   const sword& swYPos,
				   const GraphDefs::sLight& Light) 
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se calcula la posicion en Y base
  const sword swYDrawPos = swYPos - Inherited::GetElevation();

  // ¿Hay info de habla y ademas hay que dibujar texto?
  if (IsTalking() && 
	  CWorldEntity::sTalkInfo::WAV_ONLY != m_pTalkInfo->ShowType) {
	// Se dibuja el texto
	DrawTalkText(swXPos, swYDrawPos);
  }

  // Se dibuja sombra si procede
  if (m_pShadowInfo) {
	m_pShadowInfo->Shadow.Draw(swXPos + Inherited::GetXPos(), 
							   swYDrawPos + Inherited::GetYPos(),
							   IsoDefs::WORLDENTITY_DRAW_PLANE);
  }

  // Propaga dibujado a clase base, teniendo en cuenta la altura
  // Aunque se almacena en la clase base, se utiliza aqui porque dependiendo de
  // si la entidad es un CFloor o un CWorldEntity, el uso sera diferente.  
  Inherited::Draw(swXPos, 
				  swYDrawPos, 
				  IsoDefs::WORLDENTITY_DRAW_PLANE, 				  
				  Light,
				  IsFadeSelectActive());
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Pausa
// Parametros:
// - bPause. Flag de pausa
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWorldEntity::SetPause(const bool bPause)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se actua solo si procede
  if (bPause != Inherited::IsInPause()) {
	// ¿Se desea quitar la pausa?
	if (!bPause) {
	  // ¿NO es posible?
	  if (!Inherited::CanQuitPause()) {
		return;
	  }	
	}

	// Se detiene animacion del retrato si procede
	if (m_pPortrait) {
	  m_pPortrait->Portrait.GetAnimTemplate()->ActiveAnimCmd(!bPause);
	}

	// Se pausa seleccion de fade si procede
	if (m_FadeSelect.bSet) {
	  m_FadeSelect.FadeCmd.SetPause(bPause);
	}

	// Pausa evento idle si procede
	iCAlarmManager* const pAlarmManager = SYSEngine::GetAlarmManager();
	ASSERT(pAlarmManager);	
	if (m_IdleEventInfo.hIdleAlarm) {
	  pAlarmManager->SetPause(m_IdleEventInfo.hIdleAlarm, bPause);
	}

	// Se pausa reproduccion de texto si procede
	if (m_pTalkInfo) {
	  // Alarma de fin de habla de texto
	  if (m_pTalkInfo->hEndText) {
		pAlarmManager->SetPause(m_pTalkInfo->hEndText, bPause);
	  }

	  // Alarma de fin de WAV asociado a texto
	  if (m_pTalkInfo->hEndWAVText) {
		pAlarmManager->SetPause(m_pTalkInfo->hEndWAVText, bPause);
	  }

	  // Sonido WAV asociado
	  if (m_pTalkInfo->hWAVSound) {
		if (bPause) {
		  SYSEngine::GetAudioSystem()->PauseWAVSound(m_pTalkInfo->hWAVSound);
		} else {
		  SYSEngine::GetAudioSystem()->PlayWAVSound(m_pTalkInfo->hWAVSound,
													AudioDefs::WAV_PLAY_NORMAL);
		}
	  }
	}
 
	// Propaga
	Inherited::SetPause(bPause);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notifica que la entidad ha sido observada para que se lance el script
//   correspondiente.
// Parametros:
// - hTheCriature. Handle a la criatura que observa
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CWorldEntity::OnObserve(const AreaDefs::EntHandle& hTheCriature)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hTheCriature);

  // Notifica evento
  SYSEngine::GetVirtualMachine()->OnObserveEntity(this, 
												  Inherited::GetScriptEvent(RulesDefs::SE_ONOBSERVE),
												  Inherited::GetHandle(),
												  hTheCriature);   
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notifica que la entidad recibe un evento de inicio de conversacion por
//   parte de una criatura.
// Parametros:
// - hTheCriature. Handle a la criatura que inicia la conversacion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CWorldEntity::OnTalk(const AreaDefs::EntHandle& hTheCriature)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hTheCriature);

  // Notifica evento
  SYSEngine::GetVirtualMachine()->OnTalkToEntity(this, 
												 Inherited::GetScriptEvent(RulesDefs::SE_ONTALK),
												 Inherited::GetHandle(),
												 hTheCriature);   
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia un tiempo de ejecucion para el evento Idle. En caso de pasar un
//   tiempo cer0, se entendera que se deseara desinstalar la posible alarma 
//   activa.
// Parametros:
// - fTime. Tiempo asociado a la alarma.
// Devuelve:
// Notas:
// - Si ya hubiera un tiempo asociado, se cambiara la alarma antigua
//   desinstalandose primero
//////////////////////////////////////////////////////////////////////////////
void 
CWorldEntity::SetIdleEventTime(const float fTime)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Se desea desinstalar?
  if (fTime <= 0.0f) {
	// ¿Hay alarma asociada?
	if (m_IdleEventInfo.hIdleAlarm) {
	  // Si, se desinstala alarma
	  SYSEngine::GetAlarmManager()->UninstallAlarm(m_IdleEventInfo.hIdleAlarm);
	  m_IdleEventInfo.hIdleAlarm = 0;	  
	}

	// Se establece tiempo nulo
	m_IdleEventInfo.fTime = 0.0f;
  } else {
	// No, ¿Hay alarma asociada?
	iCAlarmManager* const pAlarmManager = SYSEngine::GetAlarmManager();
	ASSERT(pAlarmManager);
	if (m_IdleEventInfo.hIdleAlarm) {
	  // Si, se desinstala alarma
	  pAlarmManager->UninstallAlarm(m_IdleEventInfo.hIdleAlarm);
	  m_IdleEventInfo.hIdleAlarm = 0;
	}

	// Se instala nueva alarma y toma tiempo
	m_IdleEventInfo.hIdleAlarm = pAlarmManager->InstallTimeAlarm(this, fTime);
	ASSERT(m_IdleEventInfo.hIdleAlarm);
	m_IdleEventInfo.fTime = fTime;	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe la notificacion en la finalizacion de una alarma.
// Parametros:
// - AlarmType. Tipo de alarma.
// - hAlarm. Handle a la alarma.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWorldEntity::AlarmNotify(const AlarmDefs::eAlarmType& AlarmType,
						  const AlarmDefs::AlarmHandle& hAlarm)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
	  
  switch(AlarmType) {
	case AlarmDefs::TIME_ALARM: {
	  // ¿Alarma asociada a la muestra del texto?
	  if (m_pTalkInfo &&
		  hAlarm == m_pTalkInfo->hEndText) {
		// Se desvincula handle y se manda callar
		m_pTalkInfo->hEndText = 0;
		ShutUp();
	  } else if (hAlarm == m_IdleEventInfo.hIdleAlarm) {
		// Alarma asociada al evento Idle
		// Se lanza el POSIBLE script asociado, desvinculandose handle
		m_IdleEventInfo.hIdleAlarm = 0;
		SYSEngine::GetVirtualMachine()->OnEntityIdle(this, 
												     Inherited::GetScriptEvent(RulesDefs::SE_ONENTITYIDLE),
  												     Inherited::GetHandle());													 
	  }
	} break;

	case AlarmDefs::WAV_ALARM: {
	  // ¿Alarma asociada al texto de habla?
	  if (m_pTalkInfo->hEndWAVText == hAlarm){
		// Notificacion del final del fichero WAV
		// Se libera el recurso	    
		SYSEngine::GetResourceManager()->ReleaseWAVSound(m_pTalkInfo->hWAVSound);
		m_pTalkInfo->hWAVSound = 0;
    
		// ¿Era sonido WAV con texto asociado?
		if (m_pTalkInfo->ShowType == CWorldEntity::sTalkInfo::TEXT_AND_WAV) {
  		  // ¿El texto debe permanecer solo un tiempo?
		  if (m_pTalkInfo->uwWavTextSeconds) {
			m_pTalkInfo->hEndText = SYSEngine::GetAlarmManager()->InstallTimeAlarm(this, 
																				   m_pTalkInfo->uwWavTextSeconds);
			ASSERT(m_pTalkInfo->hEndText);
		  }
		} else {		
		  // Se manda callar directamente
		  ShutUp();		
		}
	  }
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion por parte de los scripts lanzados desde esta clase.
// Parametros:
// - Notify. Tipo de notificacion.
// - ScriptEvent. Evento que lo lanza.
// - udParams. Parametros asociados.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CWorldEntity::ScriptNotify(const RulesDefs::eScriptEvents& ScriptEvent,
						   const ScriptClientDefs::eScriptNotify& Notify,
						   const dword udParams)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Filtrado de evento que manda notificacion
  switch(ScriptEvent) {
	case RulesDefs::SE_ONOBSERVE: {
	  // Sin comportamiento por defecto
	} break;	

  	case RulesDefs::SE_ONTALK: {
	  // Sin comportamiento por defecto
	} break;	

	case RulesDefs::SE_ONENTITYCREATED: {
	  // Sin comportamiento por defecto
	} break;

	case RulesDefs::SE_ONENTITYIDLE: {
  	  // ¿NO hubo ERRORES ejecutando el script?
	  if (ScriptClientDefs::SN_SCRIPT_ERROR != Notify) {
		// ¿Hay tiempo asociado? Y
		// ¿No se ha asociado otra alarma?
		if (m_IdleEventInfo.fTime != 0.0f &&
			!m_IdleEventInfo.hIdleAlarm) {
		  // Si, se instala alarma
		  m_IdleEventInfo.hIdleAlarm = SYSEngine::GetAlarmManager()->InstallTimeAlarm(this, 
																					  m_IdleEventInfo.fTime);
		  ASSERT(m_IdleEventInfo.hIdleAlarm);
		}
	  }	  
	} break;
  }; // ~ switch
}
