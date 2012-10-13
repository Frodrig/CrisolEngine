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
// CGFXManager.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGFXManager
//
// Descripcion:
// - Modulo encargado de mantener los elementos de FX del motor. Tambien
//   se encargara de coordinar, de forma particular, los graficos GFX que
//   podran asociarse a las entidades.
// - El resumen de responsabilidades:
//   * Mantendra el alpha de las sombras
//   * Mantendra el alpha de las criaturas transparentes
//   * Se encargara de mantener y coordinar el funcionamiento de los graficos
//     de GFX.
//   * Efectuara las peticiones de FadeIn / FadeOut
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGFXMANAGER_H_
#define _CGFXMANAGER_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _ICGFXManager_H_
#include "iCGFXManager.h"
#endif
#ifndef _ICWORLDOBSERVER_H_
#include "iCWorldObserver.h"
#endif
#ifndef _ICALARMCLIENT_H_
#include "iCAlarmClient.h"
#endif
#ifndef _CQUADFORM_H_
#include "CQuadForm.h"
#endif
#ifndef _CFADECMD_H_
#include "CFadeCmd.h"
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif

// Definicion de clases / estructuras / espacios de nombres
struct iCGameDataBase;
struct iCAlarmManager;
struct iCGraphicSystem;
struct iCCommandManager;

// Clase CGFXManager
class CGFXManager: public iCGFXManager,
				   public iCWorldObserver,
				   public iCAlarmClient
{
private:
  // Estructuras forward
  struct sNGFX;

private:
  // Tipos
  // Lista de GFXs asociados a una entidad
  typedef std::map<std::string, sNGFX*> NGFXMap;
  typedef NGFXMap::iterator				NGFXMapIt;
  typedef NGFXMap::value_type           NGFXMapValType;

private:
  // Estructuras
  struct sGFXEntityInfo {
	// Info asociada a una entidad, en cuanto a sus GFXs
	NGFXMap GFXMap; // Map de GFXs
  };  

private:
  // Tipos
  typedef std::map<AreaDefs::EntHandle, sGFXEntityInfo> GFXProfilesInfoMap;
  typedef GFXProfilesInfoMap::iterator					GFXProfilesInfoMapIt;
  typedef GFXProfilesInfoMap::value_type				GFXProfilesInfoMapValType;

private:
  // Estructuras
  struct sGFXEndByTimeInfo {
	// Info asociada a la finalizacion por tiempo
	// Esta estructura estara asociada a un handle de alarma
	std::string         szGFXProfile; // Perfil del GFX
	AreaDefs::EntHandle hEntity;      // Handle a la entidad
	// Constructores
	sGFXEndByTimeInfo(const sGFXEndByTimeInfo& GFXEndByTimeInfo) {
	  szGFXProfile = GFXEndByTimeInfo.szGFXProfile;
	  hEntity = GFXEndByTimeInfo.hEntity;
	}
	sGFXEndByTimeInfo(const std::string& aszGFXProfile,
					  const AreaDefs::EntHandle& ahEntity): szGFXProfile(aszGFXProfile),
															hEntity(ahEntity) { }
  };

private:
  // Map de informacion de apoyo para alarmas de GFX
  typedef std::map<AlarmDefs::AlarmHandle, sGFXEndByTimeInfo> GFXEndByTimeInfoMap;
  typedef GFXEndByTimeInfoMap::iterator                       GFXEndByTimeInfoMapIt;
  typedef GFXEndByTimeInfoMap::value_type                     GFXEndByTimeInfoMapValType;

private:
  // Estructuras
  struct sFXShadowsInfo {
	// Info asociada al valor alpha para las sombras
	GraphDefs::sAlphaValue AlphaShadow; // Valor alpha actual para las sombras
  };

  struct sFXCriaturesInfo {
	// Info de FX asociados con criaturas
	GraphDefs::sAlphaValue AlphaCriatures; // Valor cuando criaturas transparentes
  };

  struct sGFXInfo {
	// Info asociada a graficos de GFX
	GFXProfilesInfoMap  GFXProfiles;      // GFXs alojados en memoria
	GFXEndByTimeInfoMap GFXEndByTimeInfo; // Info de apoyo para fin por tiempo
  };

  struct sFXFadeInfo {
	// Datos asociados al efecto de fade a pantalla completa  
	CFadeCmd               FadeCmd;          // Comando de fade
	word				   uwFadeSpeed;      // Velocidad de fade
	GraphDefs::sRGBColor   FadeOutColor;     // Color del fade in
	GraphDefs::sAlphaValue FadeInIntensity;  // Intensidad del fade en modo In
	GraphDefs::sAlphaValue FadeOutIntensity; // Intensidad del fade en modo Out
	GraphDefs::sAlphaValue FadeActIntensity; // Intensidad actual del quad  
	CQuadForm              FadeQuad;         // Quad para simular el fade
	GraphDefs::eDrawZone   DrawZone;         // Zona donde hacer el fade
	byte                   ubDrawPlane;      // Plano de dibujado
  };

private:
  // Instancia singlenton
  static CGFXManager* m_pGFXManager;

  // Instancias a distintos subsistemas
  iCGameDataBase*   m_pGDBase;       // Instancia a la base de datos
  iCAlarmManager*   m_pAlarmManager; // Instancia al manager de alarmas
  iCGraphicSystem*  m_pGraphSys;     // Instancia al subsistema grafico
  iCCommandManager* m_pCmdManager;   // Instancia al manejador de comandos
  
  // Resto
  sGFXInfo         m_GFXInfo;         // Info de GFX
  sFXShadowsInfo   m_FXShadowsInfo;   // Info de FX para sombras
  sFXCriaturesInfo m_FXCriaturesInfo; // Info de FX para criaturas
  sFXFadeInfo      m_FXFadeInfo;      // Info de FX para Fades
  bool			   m_bIsInitOk;       // ¿Clase inicializada?

protected:
  // Constructor / destructor
  CGFXManager(void): m_pGDBase(NULL),
				     m_pAlarmManager(NULL),
					 m_pGraphSys(NULL),
					 m_pCmdManager(NULL),
					 m_bIsInitOk(false) { }
public:
  ~CGFXManager(void) { 
    End(); 
  }

public:
  // Obtencion y destruccion de la instancia singlenton
  static inline CGFXManager* const GetInstance(void) {
    if (NULL == m_pGFXManager) { 
      m_pGFXManager = new CGFXManager; 
      ASSERT(m_pGFXManager)
    }
    return m_pGFXManager;
  }
  static inline void DestroyInstance(void) {
    if (m_pGFXManager) {
      delete m_pGFXManager;
      m_pGFXManager = NULL;
    }
  }

public:
  // Protocolo de inicio y fin de instancia
  bool Init(void);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }
private:
  // Metodos de apoyo
  bool InitFXFadeInfo(void);
  bool TLoad(void);
  bool TLoadAlphaCriatures(void);  

public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
					       const dword udParam);

public:
  // iGFXManager / Trabajo con el valor de luz ambiente
  GraphDefs::Light GetAmbientLight(const byte ubHour);

public:
  // iCGFXManager / Trabajo con los valores alpha para sombras
  GraphDefs::sAlphaValue* const GetAlphaShadow(void) {
	ASSERT(IsInitOk());
	// Retorna el valor alpha a asociar a las sombras
	return &m_FXShadowsInfo.AlphaShadow;
  }
private:
  // Metodos de apoyo
  void TLoadAlphaShadowValue(const byte ubHour);

public:
  // Orden de dibujado de efecto de Fade
  void DrawFXFade(void);

public:
  // iCGFXManager / Trabajo con el alpha para efecto de transparencia en criaturas
  GraphDefs::sAlphaValue* const GetAlphaCriatures(void) {
	ASSERT(IsInitOk());
	// Retorna la direccion del alpha para efectos de transparencia en criaturas
	return &m_FXCriaturesInfo.AlphaCriatures;
  }

public:
  // iCGFXManager / Trabajo con el efecto de Fade
  void SetFadeSpeed(const word uwFadeSpeed) {
	ASSERT(IsInitOk());
	ASSERT(uwFadeSpeed);
	// Asocia la velocidad para los fades
	m_FXFadeInfo.uwFadeSpeed = uwFadeSpeed;  
  }
  void SetFadeOutColor(const GraphDefs::sRGBColor& RGBColor) {
	ASSERT(IsInitOk());  
	// Asocia el color para el FadeOut
	m_FXFadeInfo.FadeOutColor = RGBColor;
  }
  bool DoFade(const GFXManagerDefs::eFadeType& Fade,
			  const bool bDoNow = false,
			  iCCommandClient* const pClient = NULL,
			  const CommandDefs::IDCommand& IDCommand = 0,
			  const GraphDefs::eDrawZone& DrawZone = GraphDefs::DZ_CURSOR,
			  const byte ubPlane = 0);  

public:
  // iCGFXManager / Trabajo con graficos de GFX
  bool AttachGFX(const AreaDefs::EntHandle& hEntity,
			     const std::string& szGFXProfile);
  void ReleaseGFX(const AreaDefs::EntHandle& hEntity,
				  const std::string& szGFXProfile);
  void ReleaseAllGFX(const AreaDefs::EntHandle& hEntity);
  bool IsGFXAttached(const AreaDefs::EntHandle& hEntity,
					 const std::string& szGFXProfile);
  void DrawGFXAttached(const AreaDefs::EntHandle& hEntity,
					   const sword& swXDrawPos,
					   const sword& swYDrawPos,
					   const GraphDefs::eDrawZone& DrawZone,
					   const byte ubDrawPlane);
public:
  // iCAlarmClient / Notificacion de la finalizacion de una alarma
  void AlarmNotify(const AlarmDefs::eAlarmType& AlarmType,
				   const AlarmDefs::AlarmHandle& hAlarm);  

private:
  // Establecimiento del modo pausa
  void SetPause(const bool bPause);
};

#endif // ~ CGFXManager

