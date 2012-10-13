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
// RulesDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Definiciones de los tipos basicos presentes en las reglas de juego. Tambien
//   se definiran las estructuras que representaran los atributos de las 
//   distintas entidades de juego.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _RULESDEFS_H_
#define _RULESDEFS_H_

// Cabeceras
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _STRING_H_
#include <string>
#define _STRING_H_
#endif

// Espacio de nombres
namespace RulesDefs
{ 
  // Estructuras  
  // Estructuras de apoyo general
  struct sPairValue {
	// Valor de atributo doble
	sword swBase; // Valor base
	sword swTemp; // Valor temporal
  };

  // Tipos  
  typedef byte BaseType; // Tipo base para referirnos al "type" de cualquier entidad
  
  // Generales  
  typedef byte LightFactor; // Factor de luz  
  typedef word Elevation;   // Elevacion de entidades sobre la elevacion del suelo  
  typedef byte MinDistance; // Distancia minima  
  
  // CFloor
  typedef byte FloorType; // Tipo de suelo  
  
  // CItem
  typedef byte                ItemType;         // Tipo de item
  typedef byte                ItemClass;        // Clase de item
  typedef sword               ItemGlobalAttrib; // Atributo global para el item
  typedef sword               ItemLocalAttrib;  // Atributo local para el item
  typedef AreaDefs::EntHandle ItemOwner;        // Dueño del item
  
  // CSceneObj
  typedef byte  SceneObjType;        // Tipo de objeto de escenario
  typedef byte  SceneObjClass;       // Clase del objeto de escenario
  typedef sword SceneObjLocalAttrib; // Atributo local para el obj. de escenario  
  
  // CCriature
  typedef byte       CriatureType;         // Tipo
  typedef byte       CriatureClass;        // Clase 
  typedef byte       CriatureGenre;        // Genero
  typedef sPairValue CriatureHealth;       // Salud 
  typedef byte       CriatureLevel;        // Nivel 
  typedef word       CriatureExp;          // Experiencia
  typedef word       CriatureHabilities;   // Habilidades
  typedef word       CriatureSymptoms;     // Sintomas
  typedef sPairValue CriatureExAttrib;     // Atributo extendido
  typedef byte       CriatureFloorsAccess; // Terreno pisable    
  typedef byte       CriatureActionPoints; // Ptos de accion de una criatura
  typedef byte       CriatureRange;        // Rango de una criatura
  
  // CCriature <equipamiento>
  typedef byte EquipmentSlot; // Slot de equipamiento
  
  // CWall
  typedef byte  WallType;        // Tipo de pared
  typedef sword WallLocalAttrib; // Atributo local

  // Enumerados
  // Eventos script
  enum eScriptEvents {
	// Recoge todos los posibles eventos que pueden suceder en juego
	// independientemente de la entidad a la que puedan estar asociados
	// Nota: SE = ScriptEvents
	SE_ONSTARTGAME = 0,           // World
	SE_ONCLICKHOURPANEL,          // Game
	SE_ONFLEECOMBAT,              // Game
	SE_ONKEYPRESSED,              // Game
	SE_ONSTARTCOMBATMODE,         // Game
	SE_ONENDCOMBATMODE,           // Game
	SE_ONNEWHOUR,                 // World
	SE_ONENTERINAREA,             // World
	SE_ONWORLDIDLE,               // World
	SE_ONSETIN,                   // Floor
	SE_ONSETOUT,                  // Floor
	SE_ONGET,                     // Item
	SE_ONDROP,                    // Item
	SE_ONOBSERVE,                 // Wall, Scene Object, Item, Criature
	SE_ONTALK,                    // Wall, Scene Object, Item, Criature
	SE_ONMANIPULATE,              // Wall, Scene Object, Item
	SE_ONDEATH,                   // Criature
	SE_ONRESURRECT,               // Criature
	SE_ONINSERTINEQUIPMENTSLOT,   // Criature
	SE_ONREMOVEFROMEQUIPMENTSLOT, // Criature
	SE_ONUSEHABILITY,             // Criature
	SE_ONACTIVATEDSYMPTOM,        // Criature
	SE_ONDEACTIVATEDSYMPTOM,      // Criature 
	SE_ONHIT,                     // Criature
	SE_ONSTARTCOMBATTURN,         // Criature
	SE_ONCRIATUREINRANGE,         // Criature
	SE_ONCRIATUREOUTOFRANGE,      // Criature
	SE_ONENTITYIDLE,              // Wall, Scene Object, Item, Criature
	SE_ONUSEITEM,                 // Item
	SE_ONTRADEITEM,               // Item
	SE_ONENTITYCREATED,           // Wall, Scene Object, Item, Criature
	// Script global
	SE_GLOBAL_SCRIPT
  };

  // Generales para entidades
  enum eEntityType {
	// Tipos de handles a elementos representables en el universo
	// En los handles se reservaran solo 4 bits para indicar
	// el tipo de entidad
	NO_ENTITY = 0x00,
	FLOOR     = 0X01,
	SCENE_OBJ = 0X03,	
	PLAYER    = 0X04,
	CRIATURE  = 0X06,
	WALL      = 0X09,
	ITEM      = 0X0B,
	ROOF      = 0X0C
  };

  // CCriature
  enum eIDSymptom {
	// Define los sintomas que se pueden consultar	
	SYMPTOM_01 = 0X0001,
	SYMPTOM_02 = 0X0002,
	SYMPTOM_03 = 0X0004,	
	SYMPTOM_04 = 0X0008,
	SYMPTOM_05 = 0X0010,
	SYMPTOM_06 = 0X0020,
	SYMPTOM_07 = 0X0040,
	SYMPTOM_08 = 0X0080,
	SYMPTOM_09 = 0X0100,
	SYMPTOM_10 = 0X0200,
	SYMPTOM_11 = 0X0400,
	SYMPTOM_12 = 0X0800,
	SYMPTOM_13 = 0X1000,
	SYMPTOM_14 = 0X2000,
	SYMPTOM_15 = 0X4000,
	SYMPTOM_16 = 0X8000	
  };

  enum eIDHability {
	// Define los sintomas que se pueden consultar	
	HABILITY_01 = 0X01,
	HABILITY_02 = 0X02,
	HABILITY_03 = 0X04,
	HABILITY_04 = 0X08,
	HABILITY_05 = 0X10,
	HABILITY_06 = 0X20,
	HABILITY_07 = 0X40,
	HABILITY_08 = 0X80		
  };

  enum eIDEquipmentSlot {
	// Identificadores a los slots de equipamiento
	EQUIPMENT_SLOT_0 = 0,
	EQUIPMENT_SLOT_1,
	EQUIPMENT_SLOT_2,
	EQUIPMENT_SLOT_3,
	EQUIPMENT_SLOT_4,
	EQUIPMENT_SLOT_5,
	EQUIPMENT_SLOT_6,
	EQUIPMENT_SLOT_7
  };

  enum eIDExAttrib {
	// Identificador al atributo extendido requerido
	EX_ATTRIB_0 = 0,
	EX_ATTRIB_1,
	EX_ATTRIB_2,
	EX_ATTRIB_3,
	EX_ATTRIB_4,
	EX_ATTRIB_5,
	EX_ATTRIB_6,
	EX_ATTRIB_7,
	EX_ATTRIB_8
  };  

  enum {
	// Estados de ANIMACION asociados a las acciones de las criaturas
	// Se declarara como un enumerado anonimo, ya que los estados no vendran
	// a ser mas que constantes
	// Nota: No confundir con eCriatureAction
	// AS = AnimState
	AS_STOP = 0,  // Estarse quieto
	AS_WALK,      // Andar
	AS_RUN,       // Correr
	AS_GETITEM,   // Coger un item del escenario
	AS_INTERACT,  // Interactuar con escenario (usar item, registrar, manipular)
	AS_DECHEALTH, // Recibir un golpe
	AS_DEATH,     // Morir
  };

  enum eCriatureAction {
	// Acciones que son posibles de realizar en modo combate
	// Nota: Las acciones desde WALK a HIT seran permitidas en combate y
	// deberan de figurar en este orden, pues el orden se utilizara para
	// cargar el valor de combat points desde el archivo de reglas.
	// CA = CriatureActions
	// Acciones de combate
	CA_WALK = 0,
	CA_EQUIP,
	CA_UNEQUIP,
	CA_USEITEM,
	CA_OBSERVE,
	CA_GETITEM,	
	CA_MANIPULATE,
	CA_USEHABILITY,
	CA_HIT,	
	// Acciones generales
	CA_DROPITEM,
	CA_STOPWALK,
	//CA_INITWALK,
	//CA_ENDWALK,
	// Maximo numero de acciones posibles
	MAX_CRIATUREACTIONS
  };

  enum eCriatureActionResult {
	// Codigo con el resultado de haber realizado una accion
	// CAR = CriatureActionResult
	CAR_FAIL = 0,
	CAR_INPROGRESS,
	CAR_DONE
  };
  
  enum eIDRuleType {
	// Tipos de identificadores asociados a las reglas
	ID_FLOOR_TYPE = 0,
	ID_CRIATURE_TYPE,
	ID_CRIATURE_CLASS,
	ID_CRIATURE_GENRE,
	ID_CRIATURE_EXTATTRIB,
	ID_CRIATURE_EQUIPMENTSLOT,
	ID_CRIATURE_HABILITY,
	ID_CRIATURE_USABLEHABILITY,
	ID_CRIATURE_SYMPTOM,
	ID_SCENEOBJ_TYPE,
	ID_SCENEOBJ_CLASS,
	ID_SCENEOBJ_LOCALATTRIB,
	ID_WALL_TYPE,
	ID_WALL_LOCALATTRIB,
	ID_ITEM_GLOBALATTRIB,
	ID_ITEM_TYPE,
	ID_ITEM_LOCALATTRIB,
	ID_ITEM_CLASS,
  };

  enum eMinDistance {
	// Acciones que tienen asociada una minima distancia absoluta para efectuarse
	// MD = MinDistance
	MD_TALK        = 0X01, // Hablar
	MD_USEHABILITY,        // Usar habilidad
	MD_HITWITHWEAPON,      // Golpear con arma
  };

  // CWall
  enum eWallOrientation {
	// Orientaciones de pared (WO = WallOrientation)
	WO_NONE = 0,
	WO_SOUTHWEST,
	WO_FRONT,
	WO_SOUTHEAST
  };

  // Combate
  enum eCombatOrderValue {
	// Valor con el que realizar el orden de combate
	// COV = CombatOrderValue
	COV_BYINCLUSION, // Por orden de inclusion
	COV_BYEXATTRIB,  // Por atributo extendido
  };

  // Varios
  enum ePairValueType {
	// Tipo de valor par a tratar
	TEMP_VALUE = 0, // Valor temporal
	BASE_VALUE      // Valor base
  };

  enum eNoVisibleInScreenMode {
	// Modo de responder al evento de no visibilidad en pantalla
	// NVS = NoVisibleScreen
	NVS_PAUSE = 0, // La entidad se pausara
	NVS_SILENCE    // No emitira sonido
  };
   
  // Constantes  
  // General
  const byte MAX_INTERACT_EVENTS = 5; // Num. maximo de eventos de interaccion

  // Eventos
  const byte NUM_WORLD_EVENTS    = 4;  // Eventos asociados al universo de juego
  const byte NUM_GAME_EVENTS     = 5;  // Eventos asociados al juego en general
  const byte NUM_FLOOR_EVENTS    = 2;  // Eventos asociados a los floors (tiles)
  const byte NUM_ROOF_EVENTS     = 0;  // Eventos asociados a los techos
  const byte NUM_ITEMS_EVENTS    = 9;  // Eventos asociados a los items
  const byte NUM_CRIATURE_EVENTS = 16; // Eventos asociados a las criaturas
  const byte NUM_WALL_EVENTS     = 6;  // Eventos asociados a las paredes
  const byte NUM_SCENEOBJ_EVENTS = 6;  // Eventos asociados a los objetos de escenario
  
  // Combate  
  const MinDistance MIN_ACTIONS_DISTANCE_MAX = 20; // Distancia minima para poder 
  const MinDistance MIN_ACTIONS_DISTANCE_MIN = 1;  // realizar ciertas acciones     
  const byte        MAX_CRIATURECOMBATACTIONS = 9; // Maximas acciones de criaturas en combate

  // Floors
  const byte MAX_FLOOR_TYPES           = 8;   // Maximo numero de tipos de floors  
  
  // CSceneObj
  const word MAX_SCENEOBJ_TYPES        = 256; // Maximo numero de tipos de obj. escenario
  const word MAX_SCENEOBJ_CLASSES      = 256; // Maximo numero de clases asociadas a tipos
  const byte MAX_SCENEOBJ_LOCALATTRIB  = 4;   // Maximo numero de objetos de escenario
  
  // CItem
  const word MAX_ITEM_TYPES             = 256; // Maximo numero de tipos de items
  const word MAX_ITEM_CLASSES           = 256; // Maximo numero de clases de items
  const byte MAX_ITEM_GLOBALATTRIB      = 3;   // Maximo numero de atributos globales
  const byte MAX_ITEM_LOCALATTRIB       = 6;   // Maximo numero de atributos locales 
  
  // CCriature
  const word MAX_CRIATURE_TYPES         = 256; // Maximo numero de tipos
  const word MAX_CRIATURE_CLASSES       = 256; // Maximo numero de clases
  const byte MAX_CRIATURE_GENRES        = 2;   // Maximo numero de generos
  const byte MAX_CRIATURE_EXATTRIB      = 8;   // Maximo numero de atributos extendidos
  const byte MAX_CRIATURE_HABILITIES    = 8;   // Maximo numero de habilidades
  const byte MAX_CRIATURE_SYMPTOMS      = 16;  // Maximo numero de sintomas  
  const byte MAX_CRIATURE_ACTIONPOINTS  = 255; // Max. puntos de accion de una criatura
  const byte MAX_CRIATURE_RANGE         = 10;  // Max. rango en las criaturas
  const byte CRIATURE_RUN_SPEED         = 128; // Velocidad de la criatura corriendo
  const byte CRIATURE_WALK_SPEED        = 64;  // Velocidad de la criatura andando  
  
  // CCriature <equipamiento>
  const byte MAX_CRIATURE_EQUIPMENTSLOT = 8;   // Maximo numero de slot de equipamiento <2+6>  
  
  // CWall
  const byte MAX_WALL_LOCALATTRIB       = 4;   // Maximo numero de atributos locales
  const word MAX_WALL_TYPES             = 256; // Maximo numero de tipos
  const byte MAX_WALL_ORIENTATIONS      = 3;   // Maximo numero de orientaciones
  
  // Estructuras para representar atributos
  // CFloor
  struct sFloorAttrib {	
	// Atributos de un floor
	// Datos
	FloorType Type; // Tipo de terreno
	// Operador de asignacion
	sFloorAttrib& operator=(const sFloorAttrib& FloorAttrib) {
	  // Se copian datos
	  Type = FloorAttrib.Type;	  
	  // Se devuelve instancia
	  return *this;
	}
  };

  // CSceneObj
  struct sSceneObjAttrib {
	SceneObjType  Type;  // Tipo
	SceneObjClass Class; // Clase	
	// Atributos locales
	SceneObjLocalAttrib LocalAttribs[MAX_SCENEOBJ_LOCALATTRIB]; 
	// Operador de asignacion
	sSceneObjAttrib& operator=(const sSceneObjAttrib& SceneObjAttrib) {
	  // Se copian datos
	  Type = SceneObjAttrib.Type;
	  Class = SceneObjAttrib.Class;	  
	  byte ubIt = 0;
	  for (; ubIt < MAX_SCENEOBJ_LOCALATTRIB; ++ubIt) {
		LocalAttribs[ubIt] = SceneObjAttrib.LocalAttribs[ubIt];
	  }
	  return *this;
	}
  };

  // CItem
  struct sItemAttrib {
	// Atributos asociados a un item
	// Datos
	ItemType  Type;  // Tipo
	ItemClass Class; // Clase 	
	ItemOwner Owner; // Dueño
	// Atributos globales
	ItemGlobalAttrib GlobalAttribs[MAX_ITEM_GLOBALATTRIB];
	// Atributos locales
	ItemLocalAttrib LocalAttribs[MAX_ITEM_LOCALATTRIB]; 	
	// Constructor
	sItemAttrib(void): Type(0), Class(0), Owner(0) {
	  byte ubIt = 0;
	  for (; ubIt < MAX_ITEM_GLOBALATTRIB; ++ubIt) {
		GlobalAttribs[ubIt] = 0;
	  }
	  ubIt = 0;
	  for (; ubIt < MAX_ITEM_LOCALATTRIB; ++ubIt) {
		LocalAttribs[ubIt] = 0;
	  }
	}
	// Operador de asignacion
	sItemAttrib& operator=(const sItemAttrib& ItemAttrib) {
	  // El dueño no sera un elemento que se copie
	  Type = ItemAttrib.Type;
	  Class = ItemAttrib.Class;	  
	  byte ubIt = 0;
	  for (; ubIt < MAX_ITEM_GLOBALATTRIB; ++ubIt) {
		GlobalAttribs[ubIt] = ItemAttrib.GlobalAttribs[ubIt];
	  }
	  ubIt = 0;
	  for (; ubIt < MAX_ITEM_LOCALATTRIB; ++ubIt) {
		LocalAttribs[ubIt] = ItemAttrib.LocalAttribs[ubIt];
	  }
	  return *this;
	}
  };

  // CCriature
  struct sCriatureAttrib {
	// Atributos asociados a una criatura
	// Datos	
	CriatureType         Type;         // Tipo
	CriatureClass        Class;        // Clase
	CriatureGenre        Genre;        // Genero
	CriatureHealth       Health;       // Salud
	CriatureLevel        Level;        // Nivel
	CriatureExp          Experience;   // Experiencia
	CriatureActionPoints ActionPoints; // Puntos de accion para combate
	CriatureHabilities   Habilities;   // Habilidades
	CriatureSymptoms     Symptoms;     // Sintomas	
	CriatureRange        Range;        // Rango
	LightFactor          LightFactor;  // Factor de luminosidad	
	// Atributos extendidos
	CriatureExAttrib ExAttribs[MAX_CRIATURE_EXATTRIB];	
	// Constructor
	sCriatureAttrib(void): Type(0), Class(0), Genre(0), 
						   Level(0), Experience(0), ActionPoints(0),
						   Habilities(0), Symptoms(0), LightFactor(0),
						   Range(0) {
	  Health.swBase = Health.swTemp = 0;
	  byte ubIt = 0;
	  for (; ubIt < MAX_CRIATURE_EXATTRIB; ++ubIt) {
		ExAttribs[ubIt].swBase = ExAttribs[ubIt].swTemp = 0;
	  }
	}
	// Operador de asignacion
	sCriatureAttrib& operator=(const sCriatureAttrib& CriatureAttrib) {
	  // Se copian datos	  
	  Type = CriatureAttrib.Type;
	  Class = CriatureAttrib.Class;
	  Genre = CriatureAttrib.Genre;
	  Health = CriatureAttrib.Health;
	  Level = CriatureAttrib.Level;
	  Experience = CriatureAttrib.Experience;
	  Habilities = CriatureAttrib.Habilities;
	  Symptoms = CriatureAttrib.Symptoms;
	  LightFactor = CriatureAttrib.LightFactor;
	  ActionPoints = CriatureAttrib.ActionPoints;
	  Range = CriatureAttrib.Range;
	  byte ubIt = 0;
	  for (; ubIt < MAX_CRIATURE_EXATTRIB; ++ubIt) {
		ExAttribs[ubIt] = CriatureAttrib.ExAttribs[ubIt];
	  }
	  // Se retorna instancia (copias complejas)
	  return *this;
	}
  };

  // CWall
  struct sWallAttrib {
	WallType         Type;        // Tipo			
	eWallOrientation Orientation; // Orientacion
	// Atributos locales
	WallLocalAttrib LocalAttribs[MAX_WALL_LOCALATTRIB]; 
	// Operador de asignacion
	sWallAttrib& operator=(const sWallAttrib& aWallAttrib) {	  
	  Type = aWallAttrib.Type;
	  Orientation = aWallAttrib.Orientation;
	  for (byte ubIt = 0; ubIt < MAX_WALL_LOCALATTRIB; ++ubIt) {
		LocalAttribs[ubIt] = aWallAttrib.LocalAttribs[ubIt];
	  }
	  return *this;
	}
  };
};

#endif // ~ #ifdef _RULESDEFS_H_
