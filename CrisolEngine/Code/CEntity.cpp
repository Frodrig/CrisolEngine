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
// CEntity.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CEntity.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CEntity.h"

#include "SYSEngine.h"
#include "iCGraphicSystem.h"
#include "iCGFXManager.h"
#include "iCFileSystem.h"
#include "IsoDefs.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializacion de la instancia cargando datos desde disco.
// Parametros:
// - hFile. Handle al fichero.
// - udOffset. Offset en donde comienzan a estar los datos
// - hEntity. Handle asociado a la instancia.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CEntity::Init(const FileDefs::FileHandle& hFile,
			  dword& udOffset,
			  const AreaDefs::EntHandle& hEntity)
{
  // SOLO si parametros validos
  ASSERT(hFile);
  
  // Inicializa base
  // Nota: Es importante asociar el handle antes de propagar a clase base
  m_hEntity = hEntity;	  
  if (Inherited::Init(hFile, udOffset)) {	
	// Se lee el modo de respuesta ante el evento de no estar en pantalla
	iCFileSystem* const pFileSys = SYSEngine::GetFileSystem();
	ASSERT(pFileSys);	
	udOffset += pFileSys->Read(hFile,
	                           (sbyte *)(&m_NoVisibleMode),
							   sizeof(RulesDefs::eNoVisibleInScreenMode),
							   udOffset);

	// Se proceden a cargar los scripts asociados
	// Numero de scrips
	byte ubNumScriptEvents;
	udOffset += pFileSys->Read(hFile,
	                           (sbyte *)(&ubNumScriptEvents),
							   sizeof(byte),
							   udOffset);

	// Se leen
	byte ubIt = 0;
	for (; ubIt < ubNumScriptEvents; ++ubIt) {
	  // Se lee el codigo del evento y nombre del script asociado y se mapea
	  RulesDefs::eScriptEvents ScriptEvent;
	  udOffset += pFileSys->Read(hFile,
	                             (sbyte *)(&ScriptEvent),
								 sizeof(RulesDefs::eScriptEvents),
							     udOffset);
	  std::string szScriptFile;
	  udOffset += pFileSys->ReadStringFromBinary(hFile,
												 udOffset,
												 szScriptFile);
	  m_ScriptEvents.insert(ScriptEventsMapValType(ScriptEvent, szScriptFile));
	};	  

	// Resto de inicializaciones internas	
	m_Elevation = 0;
	m_bInPause = false;		
  }

  // Retorna resultado
  return Inherited::IsInitOk();
}  

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia.
// Parametros:
// - szATSprite. Plantilla de animacion al sprite.
// - hEntity. Handle a la entidad
// - ObstacleMask. Mascara de obstaculizacion
// Devuelve:
// - Si todo va bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CEntity::Init(const std::string& szATSprite,	
			  const AreaDefs::EntHandle& hEntity)
{
  // SOLO si parametros validos
  ASSERT(szATSprite.size());
  ASSERT(hEntity);

  // ¿Se intenta reicinizalizar?
  if (Inherited::IsInitOk()) { 
	return false; 
  }

  // Inicializa base
  if (Inherited::Init(szATSprite)) {	
	// Inicializaciones internas
	m_NoVisibleMode = RulesDefs::NVS_SILENCE;
	m_hEntity = hEntity;	
	m_Elevation = 0;	
	m_bInPause = false;		  
  }
    
  // Retorna resultado
  return Inherited::IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia.
// Parametros:
// Devuelve:
// Notas:
// - Al finalizar siempre se realizara una llamada al GFXManager para que
//   se desinstalen todos los posibles GFX que esten asociados.
///////////////////////////////////////////////////////////////////////////////
void 
CEntity::End(void) 
{
  // Finaliza la instancia si procede
  if (Inherited::IsInitOk()) {
	// Solicita al GFXManager que finalice todos los posibles GFX asociados
	SYSEngine::GetGFXManager()->ReleaseAllGFX(GetHandle());
	
	// Libera map de eventos asociados
	m_ScriptEvents.clear();
	
	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Guarda los datos propios de la entidad.
// Parametros:
// - hFile. Handle al fichero donde almacenar.
// - udOffset. Offset donde depositar la info.
// Devuelve:
// Notas:
// - Pese a que la elevacion se guarda en esta clase, sera almacenada desde
//   las entidades inferiores por motivos de compatibilidad con el orden
//   el codigo de almacenamiento.
///////////////////////////////////////////////////////////////////////////////
void 
CEntity::Save(const FileDefs::FileHandle& hFile, 
			  dword& udOffset)
{
  // SOLO si parametros correctos
  ASSERT(hFile);

  // Propaga
  Inherited::Save(hFile, udOffset);

  // Guarda respuesta ante el evento de no estar en pantalla
  iCFileSystem* const pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys);  
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&m_NoVisibleMode),
							  sizeof(RulesDefs::eNoVisibleInScreenMode));  

  // Guarda datos relacioandos con los eventos scripts
  // Numero de eventos script
  const byte ubNumEvents = m_ScriptEvents.size();
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&ubNumEvents),
							  sizeof(byte));  

  // Por cada evento guardaremos ahora su codigo de evento y el nombre del script
  ScriptEventsMapIt It(m_ScriptEvents.begin());
  for (; It != m_ScriptEvents.end(); ++It) {
	// Guarda el codigo y script
	udOffset += pFileSys->Write(hFile, 
							    udOffset, 
							    (sbyte *)(&It->first),
								sizeof(RulesDefs::eScriptEvents));  

	// Guarda el nombre del archivo asociado
	udOffset += pFileSys->WriteStringInBinary(hFile, udOffset, It->second);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece scripts asociados a los distintos eventos. En caso de existir 
//   un nombre previo, este quedara sustituido y en caso de que se reciba un 
//   nombre vacio para un determinado evento, este sera quitado.
// Parametros:
// - Event. Codigo del evento.
// - szFileName. Nombre del evento.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEntity::SetScriptEvent(const RulesDefs::eScriptEvents& Event,
					    const std::string& szFileName)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Se desea eleminar entrada?
  if (szFileName.empty()) {
  	// Si, se borra
	m_ScriptEvents.erase(Event);
  } else {
  	// No, se establece / sustituyo nueva entrada	
	ScriptEventsMapIt It(m_ScriptEvents.find(Event));
	if (It != m_ScriptEvents.end()) {
	  It->second = szFileName;
	} else {
	  m_ScriptEvents.insert(ScriptEventsMapValType(Event, szFileName));
	}	
  }
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado de la entidad. La practica totalidad de las entidades
//   que hereden de CEntity llamaran a este metodo.
// Parametros:
// - swXPos, swYPos. Posicion en x e y donde dibujar.
// - DrawPlane. Plano de dibujado.
// - Light. Informacion luminosa.
// - bIsFadeSelectActive. En el caso de las CWorldEntity, estas pueden tener
//   el fade de seleccion activo. Cuando esto ocurre, el efecto de luz no se
//   aplicara sobre las mismas, para que no se interrumpa el efecto. El valor
//   de este flag por defecto sera false.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEntity::Draw(const sword& swXPos, 
			  const sword& swYPos, 
			  const IsoDefs::eDrawPlane& DrawPlane,			  
			  const GraphDefs::sLight& Light,
			  const bool bIsFadeSelectActive)
{
  // Calcula coordenadas de dibujado
  const sword swXDrawPos = swXPos + Inherited::GetXPos();
  const sword swYDrawPos = swYPos + Inherited::GetYPos() + 
						   (IsoDefs::TILE_HEIGHT - Inherited::GetHeight());  
   
  // ¿Esta activo el FadeSelect?
  if (!bIsFadeSelectActive) {
	// Se dibuja usando efecto de luz
	SYSEngine::GetGraphicSystem()->Draw(GraphDefs::DZ_GAME_SCENE,
										DrawPlane,
										swXDrawPos,
										swYDrawPos,
										this,
										Light);
  } else {
	// Se dibuja sin usar efecto de luz
	SYSEngine::GetGraphicSystem()->Draw(GraphDefs::DZ_GAME_SCENE,
										DrawPlane,
										swXDrawPos,
										swYDrawPos,
										this);
  }  

  // Dibuja posibles GFXs asociados
  SYSEngine::GetGFXManager()->DrawGFXAttached(GetHandle(),	
											  swXDrawPos,
											  swYDrawPos,
											  GraphDefs::DZ_GAME_SCENE,
											  DrawPlane);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La accion a tomar con una entidad que no sea visible en pantalla podra
//   ser la de pausarla o bien la de hacer que no emita sonido. Se valorara
//   cada caso
// Parametros:
// - bVisible. Flag de escenario visible / no visible.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEntity::VisibleInScreen(const bool bVisible)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Guarda flag (necesario hacerlo en PRIMER lugar)
  m_bVisible = bVisible;

  // Se evalua que hacer
  switch(m_NoVisibleMode) {
	case RulesDefs::NVS_PAUSE: {
	  // Sistema de pausa
	  SetPause(!bVisible);
	} break;

	case RulesDefs::NVS_SILENCE: {	
	  // Sistema de sonido
	  if (bVisible) {
		Inherited::GetAnimTemplate()->SetWAVSoundVolume(AudioDefs::WAV_VOLUME_MAX);	
	  } else {
		Inherited::GetAnimTemplate()->SetWAVSoundVolume(AudioDefs::WAV_VOLUME_MIN);	
	  }
	};
  }; //~ switch  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Pausa el sprite o lo reanuda.
// Parametros:
// - bPause. Flag de poner / quitar pausa
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEntity::SetPause(const bool bPause)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se actua solo si procede
  if (bPause != IsInPause()) {
	// ¿Se desea quitar la pausa?
	if (!bPause) {
	  // ¿NO es posible?
	  if (!CanQuitPause()) {
  		return;
	  }	
	}	
	
	// Se actuara sobre el comando de animacion del sprite base
	Inherited::GetAnimTemplate()->ActiveAnimCmd(!bPause);	
	
	// Se establece flag
	m_bInPause = bPause;
  }
}
