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
// CWorldEntity.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CWorldEntity
//
// Descripcion:
// - Clase base para las entidades representables en el area de juego. 
//
// Notas:
// - Ninguna clase heredada de CWorldEntity necesitara que esta posea de un
//   destructor virtual, ya que todas las instancias seran mantenidas en su
//   clase derivada.
// - El formato del flag de acciones sera el siguiente:
//   *b7 b6 b5 b4 b3 b2 b1 b0
//    b0> Observar, b1> Hablar, b2> Usar, b3> Recoger, 
//    b4> Usar inventario, b5> Usar habilidades
///////////////////////////////////////////////////////////////////////////////
#ifndef _CWORLDENTITY_H_
#define _CWORLDENTITY_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CENTITY_H_
#include "CEntity.h"
#endif
#ifndef _WORLDDEFS_H_
#include "WorldDefs.h"
#endif
#ifndef _CCYCLICFADECMD_H_
#include "CCyclicFadeCmd.h"
#endif
#ifndef _ICALARMCLIENT_H_
#include "iCAlarmClient.h"
#endif
#ifndef _CMEMORYPOOL_H_
#include "CMemoryPool.h"
#endif
#ifndef _GAMEDATABASEDEFS_H_
#include "GameDataBaseDefs.h"
#endif
#ifndef _CGUICSINGLETEXT_H_
#include "CGUICSingleText.h"
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif

// Definicion de clases / estructuras / espacios de nombres
struct iCAlarmClient;

// Clase CWorldEntity
class CWorldEntity: public CEntity,
					public iCAlarmClient
{
private:
  // Tipos
  // Clase base
  typedef CEntity Inherited;

private:
  // Estructuras forward
  struct sTalkInfo; 
  struct sShadowInfo;

public:
  // Enumerados accesibles
  enum eTalkTextJustify {
	// Justificacion para el texto cuando se hable
	UP_JUSTIFY = 0, // El texto se mostrara por encima del personaje
	LEFT_JUSTIFY,   // El texto se mostrara a la izq. del personaje
	RIGHT_JUSTIFY   // El texto se mostrara a la dcha. del personaje
  };

private:
  // Estructuras
  struct sPortrait {
	// Estructura con el sprite del retrato asociado a la entidad
	CSprite Portrait; // Sprite con el retrato
	// Manejador de memoria
	static CMemoryPool m_MPool;
	static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
	static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
  };

private:
  // Estructuras
  struct sFadeSelectInfo {
	// Info relativa a la seleccion con fade
	bool                 bSet;          // Estado del fade (activo / desactivado)
	GraphDefs::sRGBColor RGBSource;     // Color inicio para el fade
	GraphDefs::sRGBColor RGBSelectFade; // Color de fade para indicar seleccion
	CCyclicFadeCmd       FadeCmd;       // Comando de fade
  };

  struct sIdleEventInfo {
	// Info asociada para el control dal evento idle
	float                  fTime;      // Tiempo de evento Idle
	AlarmDefs::AlarmHandle hIdleAlarm; // Handle a la alarma
  };

private:
  // Vbles de miembro
  std::string        m_szName;        // Nombre	
  AreaDefs::sTilePos m_TilePos;       // Posicion en el tile
  sPortrait*         m_pPortrait;     // Posible retrato asociado
  sTalkInfo*         m_pTalkInfo;     // Info asociada al habla de la entidad
  sShadowInfo*       m_pShadowInfo;   // Info asociada a la sombra
  sFadeSelectInfo    m_FadeSelect;    // Seleccion con fade
  sIdleEventInfo     m_IdleEventInfo; // Info asociada al evento Idle
  
public:
  // Constructor / destructor
  CWorldEntity(void): m_pPortrait(NULL),
					  m_pTalkInfo(NULL),
					  m_pShadowInfo(NULL) { }
  ~CWorldEntity(void) { End(); }

public:
  // Protocolos de inicializacion y finalizacion  
  bool Init(const FileDefs::FileHandle& hFile,
			dword& udOffset,
			const AreaDefs::EntHandle& hEntity);
  bool Init(const std::string& szATSprite,	
			const AreaDefs::EntHandle& hEntity,
			const std::string& szName,
			const std::string& szATPortrait,
			const std::string& szShadow);
  void End(void);

public:
  // Operacion de almacenamiento
  void Save(const FileDefs::FileHandle& hFile, 
			dword& udOffset);

public:
  // Operaciones sobre el retrato
  inline CSprite* const GetPortrait(void) { 
	ASSERT(Inherited::IsInitOk());
	// Retorna el sprite que representa el retrato de la criatura, si tiene
	return m_pPortrait ? &m_pPortrait->Portrait : NULL;
  }

public:
  // Establecimiento de la localizacion de la entidad
  virtual void SetTilePos(const AreaDefs::sTilePos& TilePos) { 
	ASSERT(Inherited::IsInitOk());
	// Establece la posicion
	m_TilePos = TilePos;
  }

public:
  // Obtencion de la localizacion de la entidad
  virtual AreaDefs::sTilePos GetTilePos(void) { 
	ASSERT(Inherited::IsInitOk());
	// Devuelve la posicion
	return m_TilePos;
  }

public:
  // Operaciones con la seleccion de la entidad
  void SetFadeSelect(const bool bSet);
  inline bool IsFadeSelectActive(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el flag
	return m_FadeSelect.bSet;
  }

public:
  // Dibujado   
  virtual void Draw(const sword& swXPos, 
					const sword& swYPos,
					const GraphDefs::sLight& Light);

public:
  // Operaciones de habla
  bool Talk(const std::string& szText,			
		    const GraphDefs::sRGBColor& RGBTextColor,
			const eTalkTextJustify& TextJustify,
			const word uwViewTextTime,
			iCAlarmClient* const pClient = NULL);
  bool Talk(const std::string& szWAVFileName,
		    const std::string& szText,
			const GraphDefs::sRGBColor& RGBTextColor,
			const eTalkTextJustify& TextJustify,
			const word uwViewTextTime,
			iCAlarmClient* const pClient = NULL);
  bool Talk(const std::string& szWAVFileName,
			iCAlarmClient* const pClient = NULL);
  void ShutUp(void);
  inline bool IsTalking(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna flag
	return (NULL != m_pTalkInfo);
  }
private:
  // Metodos de apoyo
  void SetTalkText(const std::string& szText,				   
				   const GraphDefs::sRGBColor& RGBColor,
				   const eTalkTextJustify& TextJustify);
  void DrawTalkText(const sword& swXScreen,
				    const sword& swYScreen);
  
public:
  // iCAlarmClient / Notificacion de alarmas
  void AlarmNotify(const AlarmDefs::eAlarmType& AlarmType,
				   const AlarmDefs::AlarmHandle& hAlarm);

public:
  // Trabajo con el nombre
  inline std::string GetName(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el nombre
	return m_szName;
  }
  inline void SetName(const std::string& szName) {
	ASSERT(Inherited::IsInitOk());
	// Establece el nuevo nombre
	m_szName = szName;
  }

public:
  // Obtencion del atributo Type
  virtual RulesDefs::BaseType GetType(void) const = 0;

public:
  // Obtencion de la mascara de obstaculizacion concreta a una entidad  
  virtual inline AreaDefs::MaskTileAccess GetObstacleMask(void) const = 0;

public:
  // Obtencion de flag de interaccion
  virtual bool IsInteractuable(void) const = 0;

public:
  // Trabajo con la pausa
  void SetPause(const bool bPause);

public:
  // Notificacion de eventos generales para todas las entidades  
  void OnObserve(const AreaDefs::EntHandle& hTheCriature);
  void OnTalk(const AreaDefs::EntHandle& hTheCriature);  

public:
  // iCScriptClient / Operacion de notificacion, para cuando acabe un comando
  void ScriptNotify(const RulesDefs::eScriptEvents& ScriptEvent,
				    const ScriptClientDefs::eScriptNotify& Notify,
					const dword udParams);

public:
  // Trabajo con el evento idle
  void SetIdleEventTime(const float fTime);
};

#endif // ~ #ifdef _CWORLDENTITY_H_
