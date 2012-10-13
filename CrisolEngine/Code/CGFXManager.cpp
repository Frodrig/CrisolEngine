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
// CGUIManager.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIManager.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGFXManager.h"

#include "SYSEngine.h"
#include "iCGraphicSystem.h"
#include "iCLogger.h"
#include "iCWorld.h"
#include "iCGameDataBase.h"
#include "iCAlarmManager.h"
#include "iCCommandManager.h"
#include "iCCommandClient.h"
#include "CCBTEngineParser.h"
#include "CMemoryPool.h"
#include "CGFX.h"

// Inicializacion de la unica instancia singlenton
CGFXManager* CGFXManager::m_pGFXManager = NULL;

// Declaracion de estructuras forward
struct CGFXManager::sNGFX {
  // Estructura con una instancia CGFX
  CGFX GFX; // Instancia CGFX
  // Manejador de memoria
  static CMemoryPool m_MPool;
  static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
  static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
};

// Inicializacion de pools de memoria
CMemoryPool 
CGFXManager::sNGFX::m_MPool(8, sizeof(CGFXManager::sNGFX), true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
// - No se permitira reinicializar
///////////////////////////////////////////////////////////////////////////////
bool 
CGFXManager::Init(void)
{
  // ¿Se intenta reinicializar?
  if (IsInitOk()) {
	return false;
  }

  #ifdef ENGINE_TRACE    
     SYSEngine::GetLogger()->Write("CGFXManager::Init> Inicializando manager de efectos gráficos.\n");
  #endif 

  // Se obtienen instancias a distintos subsistemas
  m_pGDBase = SYSEngine::GetGameDataBase();
  ASSERT(m_pGDBase);
  m_pAlarmManager = SYSEngine::GetAlarmManager();
  ASSERT(m_pAlarmManager);
  m_pGraphSys = SYSEngine::GetGraphicSystem();
  ASSERT(m_pGraphSys);
  m_pCmdManager = SYSEngine::GetCommandManager();
  ASSERT(m_pCmdManager);

  // Se instala como observer del universo
  SYSEngine::GetWorld()->AddObserver(this);

  // Se activa flag para permitir llamada a End en caso de error
  m_bIsInitOk = true;

  // Se cargan datos estaticos
  if (TLoad()) {
	// Inicializa datos basicos de Fade
	if (InitFXFadeInfo()) {	  
	  // Todo correcto	
	  #ifdef ENGINE_TRACE    
		SYSEngine::GetLogger()->Write("                 | Ok.\n");
	  #endif 
	  return true;
	}	
  }  

  // Hubo problemas
  End();  
  #ifdef ENGINE_TRACE    
    SYSEngine::GetLogger()->Write("                 | Error.\n");
  #endif
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
// - El map de informacion asociada a los GFX DEBERA de estar vacio pues
//   las entidades llamaran, desde sus metodos End, al metodo ReleaseAllGFX.
///////////////////////////////////////////////////////////////////////////////
void 
CGFXManager::End(void)
{
  // Finaliza si procede
  if (IsInitOk()) {
	#ifdef ENGINE_TRACE    
	  SYSEngine::GetLogger()->Write("CGFXManager::End> Finalizando manager de efectos gráficos.\n");
	#endif 

	// No debe de existir ningun GFX instalado
	ASSERT(m_GFXInfo.GFXProfiles.empty());
	ASSERT(m_GFXInfo.GFXEndByTimeInfo.empty());

	// Finaliza posible comando de Fade y lo elimina del CommandManager
	if (m_FXFadeInfo.FadeCmd.IsActive()) {
	  m_FXFadeInfo.FadeCmd.End();
	  m_pCmdManager->QuitCommand(&m_FXFadeInfo.FadeCmd);
	}

	// Se elimina observer de CWorld
	SYSEngine::GetWorld()->RemoveObserver(this);

	// Se baja flag de inicializacion
	#ifdef ENGINE_TRACE    
	  SYSEngine::GetLogger()->Write("                | Ok.\n");
	#endif 
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de coordinar la carga de valores ESTATICOS desde el archivo de
//   configuracion.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGFXManager::TLoad(void)
{
  // Carga info asociada a los alpha de las criaturas para cuando estas
  // se pongan en modo transparente
  if (!TLoadAlphaCriatures()) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga el valor alpha para las criaturas cuando estas se ponga en
//   modo transparente.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGFXManager::TLoadAlphaCriatures(void)
{
  // Se obtiene el parser
  CCBTEngineParser* const pParser = m_pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_GFX_DEFS,
                                                            "[FXDefs]");
  ASSERT(pParser);

  // Se establece el valor del prefijo y se carga el alpha
  pParser->SetVarPrefix("Criatures.");
  m_FXCriaturesInfo.AlphaCriatures = pParser->ReadAlpha("TransparentMode.");

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa datos basicos de Fade. El Fade por defecto sera el FadeOut.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CGFXManager::InitFXFadeInfo(void)
{
  // Se inicializan datos basicos de Fade
  m_FXFadeInfo.uwFadeSpeed = 256;
  m_FXFadeInfo.FadeOutColor = GraphDefs::sRGBColor(0, 0, 0);
  m_FXFadeInfo.FadeInIntensity = GraphDefs::sAlphaValue(0);
  m_FXFadeInfo.FadeOutIntensity = GraphDefs::sAlphaValue(255);
  m_FXFadeInfo.FadeActIntensity = m_FXFadeInfo.FadeOutIntensity;

  // Se inicializa el quad para el fade  
  if (!m_FXFadeInfo.FadeQuad.Init(m_pGraphSys->GetVideoWidth(),
								  m_pGraphSys->GetVideoHeight())) {
	return false;  
  }  

  // Se asocia el valor de intensidad actual y color
  byte ubIt = 0;
  for (; ubIt < 4; ++ubIt) {
	// Se obtiene ID al vertice
	const GraphDefs::eVertex Vertex = GraphDefs::eVertex(ubIt);
	// Se establece color y alpha
	m_FXFadeInfo.FadeQuad.SetRGBColor(Vertex, &m_FXFadeInfo.FadeOutColor);
	m_FXFadeInfo.FadeQuad.SetAlphaValue(Vertex, &m_FXFadeInfo.FadeActIntensity);	
  }  

  // Todo correcto
  return true;
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la notificacion de un suceso en CWorld.
// - Seran de interes las notificaciones:
//    * De nueva hora.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGFXManager::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
								 const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se comprueba el tipo de notificacion
  switch(NotifyType) {
	case WorldObserverDefs::NEW_HOUR: {
	  // Se ha producido una nueva hora, se leera el valor alpha asociado
	  TLoadAlphaShadowValue(udParam);
	} break;

	case WorldObserverDefs::PAUSE_ON: {
	  // Se entra en modo pausa
	  SetPause(true);
	} break;

	case WorldObserverDefs::PAUSE_OFF: {
	  // Se sale del modo pausa
	  SetPause(false);
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre la lista de GFX asociados a alarmas, estableciendoles en modo
//   pausa o sacandoles del mismo.
// Parametros:
// - bPause. Flag de modo pausa
// Devuelve:
// Notas:
// - Si se crearan nuevos GFX estos nuevos NO estaran pausados
///////////////////////////////////////////////////////////////////////////////
void 
CGFXManager::SetPause(const bool bPause)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Se itera y se actua
  GFXEndByTimeInfoMapIt It(m_GFXInfo.GFXEndByTimeInfo.begin());
  for (; It != m_GFXInfo.GFXEndByTimeInfo.end(); ++It) {
	m_pAlarmManager->SetPause(It->first, bPause);	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la luz ambiente para una determinada hora.
// Parametros:
// - ubHour. Hora para la que queremos tomar el valor alpha.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
GraphDefs::Light 
CGFXManager::GetAmbientLight(const byte ubHour)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT((ubHour < 24) != 0);

  // Se obtiene el parser
  CCBTEngineParser* const pParser = m_pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_GFX_DEFS,
                                                            "[FXDefs]");
  ASSERT(pParser);

  // Se lee el valor y se acota
  std::string szVar;
  SYSEngine::PassToString(szVar, "AmbientDark.Hour[%u].", ubHour);
  pParser->SetVarPrefix(szVar);
  GraphDefs::Light Light = pParser->ReadNumeric("Dark", false);
  if (Light > 255) {
	Light = 255;
  }

  // Retorna el valor leido
  return Light;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Lee y carga el valor alpha asociado a la nueva hora para las sombras.
// Parametros:
// - ubHour. Hora sobre la que hay que leer el valor alpha.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGFXManager::TLoadAlphaShadowValue(const byte ubHour)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT((ubHour < 24) != 0);

  // Se obtiene el parser
  CCBTEngineParser* const pParser = m_pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_GFX_DEFS,
                                                            "[FXDefs]");
  ASSERT(pParser);

  // Se establece el valor del prefijo y se carga el alpha
  std::string szPrefix;
  SYSEngine::PassToString(szPrefix, "Shadows.Hour[%u].", ubHour);
  pParser->SetVarPrefix(szPrefix);
  m_FXShadowsInfo.AlphaShadow = pParser->ReadAlpha();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia a la entidad hEntity un perfil de GFX. En caso de que dicho perfil
//   ya exista, no asociara nada.
// Parametros:
// - hEntity. Handle a la entidad.
// - szGFXProfile. Nombre del perfil a asociar
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGFXManager::AttachGFX(const AreaDefs::EntHandle& hEntity,
					   const std::string& szGFXProfile)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hEntity);
  ASSERT(!szGFXProfile.empty());

  // ¿Esta el perfil ya asociado para esa entidad?
  if (IsGFXAttached(hEntity, szGFXProfile)) {
	// Si, se retorna sin asociar
	return false;
  }

  // Se obtiene el parser
  std::string szVar;
  SYSEngine::PassToString(szVar, "[GFXProfile<%s>]", szGFXProfile.c_str());
  CCBTEngineParser* const pParser = m_pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_GFX_DEFS,
													        szVar);
  if (NULL == pParser) {
	SYSEngine::FatalError("No se pudo localizar el perfil de GFX %s.", szGFXProfile.c_str());
	return false;
  }

  // Se obtienen datos de configuracion
  // Plantilla de animacion
  pParser->SetVarPrefix("");
  const std::string szGFXAnimTemplate = pParser->ReadString("AnimTemplate");

  // Modo de dibujado
  // Nota: En caso de que no exista al menos un flag, habra un error fatal
  GFXManagerDefs::eGFXDrawPosition DrawMode;
  pParser->SetVarPrefix("DrawMode.");
  if (pParser->ReadFlag("OverEntityTopFlag", false)) {
	DrawMode = GFXManagerDefs::OVER_ENTITY_TOP;
  } else if (pParser->ReadFlag("AtEntityTopFlag", false)) {
	DrawMode = GFXManagerDefs::AT_ENTITY_TOP;
  } else if (pParser->ReadFlag("AtEntityBaseFlag", false)) {
	DrawMode = GFXManagerDefs::AT_ENTITY_BOTTOM;
  } else if (pParser->ReadFlag("UnderEntityBaseFlag")) {
	DrawMode = GFXManagerDefs::UNDER_ENTITY_BOTTOM;
  }  

  // Se crea nodo de GFX y se inicializa
  sNGFX* const pGFX = new sNGFX;
  ASSERT(pGFX);
  pGFX->GFX.Init(hEntity, szGFXAnimTemplate, DrawMode);
  ASSERT(pGFX->GFX.IsInitOk());

  // Se completa la informacion de GFX, asociando valores de color
  byte ubIt = 0;
  for (; ubIt < 4; ++ubIt) {
	// Se lee el valor Alpha y de RGB
	SYSEngine::PassToString(szVar, "FX.Vertex[%u].", ubIt);
	pParser->SetVarPrefix(szVar);
	// Se asocian estructuras a los vertices
	const GraphDefs::eVertex Vertex = GraphDefs::eVertex(ubIt);
	pGFX->GFX.SetAlphaValue(Vertex, pParser->ReadAlpha("", false));
	pGFX->GFX.SetRGBValue(Vertex, pParser->ReadRGBColor("", false));
  }

  // Se lee el modo de finalizacion
  // ¿Es modo por tiempo?    
  pParser->SetVarPrefix("EndMode.");
  const float fVar = pParser->ReadNumeric("ByTime", false);
  if (pParser->WasLastParseOk() && 
	  fVar > 0.0f) {
	// La finalizacion es en funcion del tiempo, se instala la alarma
	const AlarmDefs::AlarmHandle hAlarm = m_pAlarmManager->InstallTimeAlarm(this, fVar);
	ASSERT(hAlarm);

	// Se asocia el identificador de la alarma al GFX al que pertenece y se
	// instala en el map de apoyo
	const sGFXEndByTimeInfo GFXEndByTimeInfo(szGFXProfile, hEntity);
	m_GFXInfo.GFXEndByTimeInfo.insert(GFXEndByTimeInfoMapValType(hAlarm, GFXEndByTimeInfo));
  } 

  // Una vez que se ha configurado todo completamente, se instalara el GFX
  // en la estructura de informacion general para GFXs

  // ¿La entidad ya tenia perfiles?
  std::string szGFXProfileLowercase(szGFXProfile);
  SYSEngine::MakeLowercase(szGFXProfileLowercase);
  const GFXProfilesInfoMapIt ProfilesIt(m_GFXInfo.GFXProfiles.find(hEntity));
  if (ProfilesIt == m_GFXInfo.GFXProfiles.end()) {
	// NO, luego se crea una entrada y luego se inserta la info
	sGFXEntityInfo GFXEntityInfo;	
	GFXEntityInfo.GFXMap.insert(NGFXMapValType(szGFXProfileLowercase, pGFX));	
	m_GFXInfo.GFXProfiles.insert(GFXProfilesInfoMapValType(hEntity, GFXEntityInfo));	
  } else {
	// SI, se inserta el GFX directamente
	ProfilesIt->second.GFXMap.insert(NGFXMapValType(szGFXProfileLowercase, pGFX));	
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Borra un GFX asociado
// Parametros:
// - hEntity. Entidad para la que realizar el borrado.
// - szGFXProfile. Perfil de GFX a borrar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGFXManager::ReleaseGFX(const AreaDefs::EntHandle& hEntity,
						const std::string& szGFXProfile)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hEntity);
  
  // Obtiene iterador a nodo con la lista de GFX asociados al perfil
  const GFXProfilesInfoMapIt ProfilesIt(m_GFXInfo.GFXProfiles.find(hEntity));
  if (ProfilesIt != m_GFXInfo.GFXProfiles.end()) {
	// Se localiza el iterador a la entidad
	std::string szGFXProfileLowercase(szGFXProfile);
	SYSEngine::MakeLowercase(szGFXProfileLowercase);
	NGFXMapIt GFXIt(ProfilesIt->second.GFXMap.find(szGFXProfileLowercase));
	if (GFXIt != ProfilesIt->second.GFXMap.end()) {
	  // Se borra la instancia y el nodo
	  delete GFXIt->second;
	  ProfilesIt->second.GFXMap.erase(GFXIt);
	  
	  // ¿Ya no quedan entidades asociadas al GFX de perfil szGFXProfile?
	  if (ProfilesIt->second.GFXMap.empty()) {
		// Si, se borrara la entrada para ese perfil
		m_GFXInfo.GFXProfiles.erase(ProfilesIt);
	  }
	}
	
	// Se busca por si existia alguna alarma de finalizacion, borrandose el nodo
	GFXEndByTimeInfoMapIt AlarmIt(m_GFXInfo.GFXEndByTimeInfo.begin());
	for (; AlarmIt != m_GFXInfo.GFXEndByTimeInfo.end(); ++AlarmIt) {
	  // ¿Coincide el handle de la entidad?
	  if (hEntity == AlarmIt->second.hEntity) {
		// Si, se desinstala la alarma y se borra el nodo
		m_pAlarmManager->UninstallAlarm(AlarmIt->first);
		m_GFXInfo.GFXEndByTimeInfo.erase(AlarmIt);
		break;
	  }
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Liberara todos los GFX asociados a una entidad
// Parametros:
// - hEntity. Entidad sobre la que trabajar.
// Devuelve:
// Notas:
// - Para eliminar, sera necesario guardar los nombres de los perfiles de
//   la entidad en una lista, ya que en el caso de querer borrar segun
//   se van visitando los nodos con los perfiles, se producirian violaciones
//   de memoria.
// - Este metodo sera llamado por todas las entidades CEntity, al finalizar.
//   De tal manera, que desde el metodo End se comprobara, via ASSERT que
//   no exista ningun GFX registrado.
///////////////////////////////////////////////////////////////////////////////
void 
CGFXManager::ReleaseAllGFX(const AreaDefs::EntHandle& hEntity)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hEntity);
  
  // Se localiza el nodo asociado a la entidad
  GFXProfilesInfoMapIt ProfilesIt(m_GFXInfo.GFXProfiles.find(hEntity));
  if (ProfilesIt != m_GFXInfo.GFXProfiles.end()) {
	// Tipos
	// Lista para insertar los nombres de los perfiles de GFX a borrar
	typedef std::list<std::string>   GFXProfileList;
	typedef GFXProfileList::iterator GFXProfileListIt;

	// Se recorre todo el arbol de perfiles asociados a la entidad y se
	// insertan en la lista de entidades a eliminar
	GFXProfileList ProfileList;
	NGFXMapIt GFXIt(ProfilesIt->second.GFXMap.begin());
	for (; GFXIt != ProfilesIt->second.GFXMap.end(); ++GFXIt) {
	  // Se inserta el nombre del perfil
	  ProfileList.push_back(GFXIt->first);	  
	}	

	// Se borran todos los perfiles hallados
	GFXProfileListIt RemoveIt(ProfileList.begin());
	for (; RemoveIt != ProfileList.end(); RemoveIt = ProfileList.erase(RemoveIt)) {
	  // Se elimina el perfil asociado a la entidad
	  ReleaseGFX(hEntity, *RemoveIt);
	}	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el perfil szGFXProfile esta asociado a la entidad.
// Parametros:
// - hEntity. Entidad sobre la que trabajar.
// - szGFXProfile. Perfil a comprobar si esta asociado.
// Devuelve:
// - Si el perfil esta asociado true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGFXManager::IsGFXAttached(const AreaDefs::EntHandle& hEntity,
						   const std::string& szGFXProfile)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hEntity);
  ASSERT(!szGFXProfile.empty());

  // Obtiene iterador a nodo con la lista de GFX asociados al perfil
  const GFXProfilesInfoMapIt ProfilesIt(m_GFXInfo.GFXProfiles.find(hEntity));
  if (ProfilesIt == m_GFXInfo.GFXProfiles.end()) {
	// No hay ninguna entidad asociada
	return false;
  }

  // Se busca para ver si figura la entidad como poseedora el GFX
  std::string szGFXProfileLowercase(szGFXProfile);
  SYSEngine::MakeLowercase(szGFXProfileLowercase);
  NGFXMapIt GFXIt(ProfilesIt->second.GFXMap.find(szGFXProfileLowercase));
  return (GFXIt != ProfilesIt->second.GFXMap.end());
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dibuja todos los GFX asociados a la entidad hEntity segun los parametros
//   recibidos.
// Parametros:
// - hEntity. Entidad para la que dibujar los GFX asociados.
// - swXDrawPos, swYDrawPos. Posicion de dibujado.
// - DrawZone. Zona de dibujado.
// - ubDrawPlane. Plano de dibujado
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGFXManager::DrawGFXAttached(const AreaDefs::EntHandle& hEntity,
							 const sword& swXDrawPos,
							 const sword& swYDrawPos,
							 const GraphDefs::eDrawZone& DrawZone,
							 const byte ubDrawPlane)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
 
  // SOLO si parametros validos
  ASSERT(hEntity);

  // Obtiene iterador al nodo de la entidad
  const GFXProfilesInfoMapIt ProfilesIt(m_GFXInfo.GFXProfiles.find(hEntity));
  if (ProfilesIt != m_GFXInfo.GFXProfiles.end()) {
	// Recorre el map de GFXs realizando dibujado
    NGFXMapIt GFXIt(ProfilesIt->second.GFXMap.begin());
	for (; GFXIt != ProfilesIt->second.GFXMap.end(); ++GFXIt) {	  
	  GFXIt->second->GFX.Draw(swXDrawPos, swYDrawPos, DrawZone, ubDrawPlane);
	}
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Atiende una notificacion del sistema de alarmas. La notificacion es
//   seguro que correspondera a un GFX que tiene asociado el sistema de 
//   finalizacion por tiempo.
// Parametros:
// - AlarmType. Tipo de alarma.
// - hAlarm. Handle asociado al tipo de alarma.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGFXManager::AlarmNotify(const AlarmDefs::eAlarmType& AlarmType,
						 const AlarmDefs::AlarmHandle& hAlarm)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hAlarm);

  // La alarma solo puede ser de tipo tiempo
  ASSERT((AlarmType == AlarmDefs::TIME_ALARM) != 0);

  // Se localiza el nodo de informacion, para saber que GFX borrar
  GFXEndByTimeInfoMapIt AlarmIt(m_GFXInfo.GFXEndByTimeInfo.find(hAlarm));
  ASSERT((AlarmIt != m_GFXInfo.GFXEndByTimeInfo.end()) != 0);

  // Se obtiene la informacion, se borra el nodo
  // Nota: Es vital hacer estos pasos antes, pues de lo contrario, si se
  // borrara antes el GFX, el metodo realizaria a su vez un borrado del nodo
  // con el que se esta trabajando, ya que comprobaria si el GFX tiene alguna
  // alarma asociada (como es el caso)
  const sGFXEndByTimeInfo GFXInfo(AlarmIt->second);
  m_GFXInfo.GFXEndByTimeInfo.erase(AlarmIt);  

  // Se realiza el borrado del GFX
  ReleaseGFX(GFXInfo.hEntity, GFXInfo.szGFXProfile);    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ordena la realizacion de un Fade.  En el caso de que ya exista un
//   comando fade activo, este se interrumpira.
// Parametros:
// - Fade. Tipo de Fade a realizar.
// - bDoNow. Si esta activo, significara que se quiere un Fade inmediato.
//   Por defecto false.
// - pClient. Posible cliente externo al que avisar cuadno el Fade finalice.
//   Por defecto NULL.
// - IDCommand. Identificador externo para el comando de fade. Por defecto 0.
// - DrawZone. Zona de dibujado.
// - ubPlane. Plano de dibujado, por defecto 0.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGFXManager::DoFade(const GFXManagerDefs::eFadeType& Fade,
					const bool bDoNow,
					iCCommandClient* const pClient,
					const CommandDefs::IDCommand& IDCommand,
					const GraphDefs::eDrawZone& DrawZone,
					const byte ubPlane)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se toma la direccion de la intensidad que proceda
  const GraphDefs::sAlphaValue* const pAlphaIntensity = (Fade == GFXManagerDefs::FADE_IN) ?
														 &m_FXFadeInfo.FadeInIntensity : 
														 &m_FXFadeInfo.FadeOutIntensity;
  // ¿Fade inmediato?
  if (bDoNow) {
    // Si, se establece la intensidad
    ASSERT(pAlphaIntensity);
    m_FXFadeInfo.FadeActIntensity = *pAlphaIntensity;
    // Se notifica al posible cliente asociado
    if (pClient) {
  	  pClient->EndCmdNotify(IDCommand, 0, 0);
	}
  } else {
    // No, el Fade es gradual	  
    // Se inicializa comando de fade	
    if (!m_FXFadeInfo.FadeCmd.Init(&m_FXFadeInfo.FadeActIntensity,
								   *pAlphaIntensity,
								   m_FXFadeInfo.uwFadeSpeed)) {
	  return false;
	}

	// Se coloca en manager de comandos	  
	m_pCmdManager->PostCommand(&m_FXFadeInfo.FadeCmd, pClient, IDCommand);
  }	

  // Se guarda la zona donde realizar el Fade y el plano de dibujado
  // Nota: El plano de dibujado siempre sera el plano maximo de la zona
  m_FXFadeInfo.DrawZone = DrawZone;
  m_FXFadeInfo.ubDrawPlane = ubPlane;

  // Operacion realizada
  return true; 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado de los fades.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGFXManager::DrawFXFade(void) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿NO se esta en FadeIn total?
  if (m_FXFadeInfo.FadeInIntensity != m_FXFadeInfo.FadeActIntensity) {
	// Se dibuja 
	m_pGraphSys->Draw(m_FXFadeInfo.DrawZone, 
					  m_FXFadeInfo.ubDrawPlane, 
					  0, 
					  0, 
					  &m_FXFadeInfo.FadeQuad);	
  }  
}