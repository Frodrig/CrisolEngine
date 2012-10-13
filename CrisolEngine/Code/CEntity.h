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
// CEntity.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CEntity
//
// Descripcion:
// - Clase base para cualquier tipo de entidad representable en el juego
//
// Notas:
// - Ninguna clase heredada de CEntity necesitara que esta posea de un
//   destructor virtual, ya que todas las instancias seran mantenidas en su
//   clase derivada.
// - La elevacion sera un valor que, aunque se defina en CEntity, tendra
//   dos interpretaciones distintas. Para todos los CFloor indicara la altitud
//   del floor de tal forma que los elementos que se dibujen encima, CArea les
//   restara tantas unidades en coordenada Y como indique la elevacion. En el
//   caso de los CWorldEntity, indicara la altitud a la que se encuentran por
//   si mismos en el tile en el que se hallen.
// - Todas las entidades inicializadas activaran SIEMPRE la notificacion de
//   que no son vistas en pantalla.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CENTITY_H_
#define _CENTITY_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CSPRITE_H_
#include "CSprite.h" 
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _RULESDEFS_H_
#include "RulesDefs.h"
#endif
#ifndef _ISODEFS_H_
#include "ISODefs.h"
#endif
#ifndef _ICSCRIPTCLIENT_H_
#include "iCScriptClient.h"
#endif
#ifndef _WORLDDEFS_H_
#include "WorldDefs.h"
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif

// Definicion de clases / estructuras / espacios de nombres
struct iCItemContainer;

// Clase CEntity
class CEntity: public CSprite,
               public iCScriptClient
{
private:
  // Tipos
  // Clase base
  typedef CSprite Inherited; 

private:
  // Tipos
  // Map con informacion de los scripts existentes
  typedef std::map<RulesDefs::eScriptEvents, std::string> ScriptEventsMap;
  typedef ScriptEventsMap::value_type                     ScriptEventsMapValType;
  typedef ScriptEventsMap::iterator                       ScriptEventsMapIt;

private:
  // Vbles de miembro
  AreaDefs::EntHandle               m_hEntity;       // Handle de la entidad  
  RulesDefs::Elevation              m_Elevation;     // Elevacion  
  ScriptEventsMap                   m_ScriptEvents;  // Eventos scripts
  RulesDefs::eNoVisibleInScreenMode m_NoVisibleMode; // Respuesta al evento de no estar en pantalla
  bool                              m_bVisible;      // ¿Visible en pantalla?
  bool					            m_bInPause;      // Flag de pausa
  
public:
  // Constructor / destructor
  CEntity(void): m_bInPause(false) { }
  ~CEntity(void) { End(); }

public:
  // Protocolos de inicializacion y finalizacion  
  bool Init(const FileDefs::FileHandle& hFile,
			dword& udOffset,
			const AreaDefs::EntHandle& hEntity);
  bool Init(const std::string& szATSprite,	
			const AreaDefs::EntHandle& hEntity);
  void End(void);

public:
  // Operacion de almacenamiento
  void Save(const FileDefs::FileHandle& hFile, 
			dword& udOffset);

public:
  // Trabajo con el valor de elevacion
  inline RulesDefs::Elevation GetElevation(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna altura
	return m_Elevation;
  }
  void SetElevation(const RulesDefs::Elevation& Elevation) {
	ASSERT(Inherited::IsInitOk());
	// Establece altura
	m_Elevation = Elevation;
  }

protected:
  // Dibujado   
  void Draw(const sword& swXPos, 
			const sword& swYPos, 
			const IsoDefs::eDrawPlane& DrawPlane,			
			const GraphDefs::sLight& Light,
			const bool bIsFadeSelectActive = false);

public:
  // Notificacion de que la entidad es / o no vista en pantalla
  void VisibleInScreen(const bool bVisible);
  inline bool IsVisibleInScreen(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna flag
	return m_bVisible;
  }
  inline RulesDefs::eNoVisibleInScreenMode GetNoVisibleMode(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el modo en que actua al no ser visible en pantalla
	return m_NoVisibleMode;
  }
  
public:
  // Obtencion de informacion  
  virtual inline RulesDefs::eEntityType GetEntityType(void) const = 0;
  inline AreaDefs::EntHandle GetHandle(void) const { 
	ASSERT(Inherited::IsInitOk());
	// Retorna handle
	return m_hEntity; 
  }

public:
  // Obtencion del contenedor
  virtual inline iCItemContainer* const GetItemContainer(void) = 0;
  
public:
  // Trabajo con la pausa
  virtual void SetPause(const bool bPause);
  inline bool IsInPause(void) const {
	ASSERT(IsInitOk());
	// Retorna flag
	return m_bInPause;
  }
protected:
  // Metodo de apoyo
  bool CanQuitPause(void) const {
	ASSERT(Inherited::IsInitOk());
	// Comprueba si es posible quitar una pausa sobre una entidad
	// Nota: Este metodo sera vital cuando se regrese de una pausa sobre
	// el universo de juego.
	// Solo se podra si la entidad es NO visible Y ADEMAS el modo de
	// actuacion cuando no sea visible consista en pausarse
	if (!IsVisibleInScreen() &&
	    GetNoVisibleMode() == RulesDefs::NVS_PAUSE) {
	  return false;
	} else {
	  return true;
	}
  }

public:
  // Trabajo con los scripts asociados a eventos
  void SetScriptEvent(const RulesDefs::eScriptEvents& Event,
					  const std::string& szFileName);
  inline std::string GetScriptEvent(const RulesDefs::eScriptEvents& Event) {
	ASSERT(Inherited::IsInitOk());
	// Comprueba si existe y devuelve nombre del string y en caso 
	// contrario el string vacio
	const ScriptEventsMapIt It(m_ScriptEvents.find(Event));
	return (It != m_ScriptEvents.end()) ? It->second : "";
  }
};

#endif // ~ #ifdef _CENTITY_H_
