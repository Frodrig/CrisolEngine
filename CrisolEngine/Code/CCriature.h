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
// CCriature.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CCriature
//
// Descripcion:
// - Representa a una entidad de tipo criatura.
// - A la hora de notificar un accion realizada a los observadores, se
//   usara el word inferior para indicar la accion y en el word superior
//   los posibles parametros asociados.
//
// Notas:
// - Para las ordenes que le demos a la criatura, esta siempre realizara 
//   primero un acercamiento al tile sobre donde este la entidad (item) o bien
//   a uno adjacente. Una vez en dichos tiles realizara la orden, anteponiendo
//   antes la ralizacion de la animacion asociada.
// - Se usaran clases amigas para tener acceso al metodo para la notificacion
//   a los observadores
// - La criatura sera cliente de otras criaturas para poder mantener el 
//   conjunto asociado de criaturas dentro de su rango de accion y asi poder
//   obtener notificaciones cuando una de ellas cambie de posicion
///////////////////////////////////////////////////////////////////////////////
#ifndef _CCRIATURE_H_
#define _CCRIATURE_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CWORLDENTITY_H_
#include "CWorldEntity.h"
#endif
#ifndef _CMOVECMD_H_
#include "CMoveCmd.h"
#endif
#ifndef _CITEMCONTAINTER_H_
#include "CItemContainer.h"
#endif
#ifndef _CEQUIPMENTSLOTS_H_
#include "CEquipmentSlots.h"
#endif
#ifndef _ICCOMMANDCLIENT_H_
#include "iCCommandClient.h"
#endif
#ifndef _ICCRIATUREOBSERVER_H_
#include "iCCriatureObserver.h"
#endif
#ifndef _CRIATUREDEFS_H_
#include "CriatureDefs.h"
#endif
#ifndef _GAMEDATABASEDEFS_H_
#include "GameDataBaseDefs.h"
#endif
#ifndef _CRIATUREOBSERVERDEFS_H_
#include "CriatureObserverDefs.h"
#endif
#ifndef _SYSENGINE_H_
#include "SYSEngine.h"
#endif
#ifndef _ICCOMBATSYSTEM_H_
#include "iCCombatSystem.h"
#endif
#ifndef _SET_H_
#define _SET_H_
#include <set>
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif
#ifndef _ICANIMTEMPLATEOBSERVER_H_
#include "iCAnimTemplateObserver.h"
#endif

// Definicion de clases / estructuras / espacios de nombres
struct iCEquipmentSlots;
struct iCCriatureObserver;
class CCombatSystem;
class CWorld;

// Clase CCriature
class CCriature: public CWorldEntity,
				 public iCCommandClient,
				 public iCAnimTemplateObserver,
				 public iCCriatureObserver
{
private:
  // Tipos
  typedef CWorldEntity Inherited; // Clase base

private:
  // Clases amigas
  friend CMoveCmd;      // Comando de movimiento
  friend CCombatSystem; // Subsistema de combate 
  friend CWorld;        // Universo de juego

private:
  // Enumerados
  enum {
	// Bits asociados al significado del movimiento de la criatura
	TO_WALK        = 0X0000, // Se anda simplemente para caminar
	TO_GETITEM     = 0X1000, // Se anda para coger un item
	TO_USEITEM     = 0X2000, // Se anda para usar un item en inv. sobre entidad
	TO_MANIPULATE  = 0X3000, // Se anda para manipular una entidad
	TO_OBSERVE     = 0X4000, // Se anda para observar una entidad
	TO_USEHABILITY = 0X5000, // Se anda para usar una habilidad	
	TO_HIT         = 0X6000, // Se anda para poder golpear
  };

  enum eActOrder {
	// Ordenes que se pueden dar a una criatura (NPC)
	// Mientras la criatura se halle ejecutando estas ordenes NO
	// se la permitira realizar ninguna otra
	// Nota: AO = ActOrder
	AO_NONE = 0,        // Ninguna
	AO_EQUIPITEM,       // Equipar item
	AO_REMOVEEQUIPITEM, // Quitar item equipado
	AO_USEHABILITY,	    // Usar habilidad
	AO_HITENTITY,       // Golpear a entidad
	AO_USEITEM,         // Usar item
	AO_GETITEM,         // Coger item
	AO_MANIPULATE,      // Manipular
	AO_DROPITEM,        // Soltar item
	AO_MOVETO           // Mover a una posicion
  };
  
private:
  // Estructuras
  struct sAnimActionActive {
	// Enumerados
	enum eAction {
	  // Acciones posibles asociadas a esta estructura
	  GET_ITEM,   // Coger item
	  MANIPULATE, // Interactuar
	  REGISTER,   // Registrar
	  USE_ITEM,   // Usar item sobre escenario
	  DEC_HEALTH, // La criatura recibe un daño y decrementa su salud sin morir
	  DEATH       // La criatura recibe un daño y muere
	};
	// Datos asociados a la animacion de una accion que esta a punto de realizarse
	eAction   Action;        // Accion asociada
	bool	  bIsInProgress; // ¿Animacion en progreso?
	dword	  udParam;       // Parametros asociados  
	// Constructor
	sAnimActionActive(const eAction& aAction,
					  const dword audParam): Action(aAction),										
											  udParam(audParam) { 
	  bIsInProgress = true;
	}
	// Manejador de memoria
	static CMemoryPool m_MPool;
	static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
	static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
  };

private:
  // Tipos
  // Map de animaciones activas
  typedef std::map<AreaDefs::EntHandle, sAnimActionActive*> AnimActionActiveMap;
  typedef AnimActionActiveMap::iterator                     AnimActionActiveMapIt;
  typedef AnimActionActiveMap::value_type				    AnimActionActiveMapValType;

private:
  // Estructuras
  struct sWalkInfo {
	// Info asociada al desplazamiento
	CMoveCmd MoveCmd;         // Comando de movimiento
	bool     bRunMode;        // ¿Modo correr activo?
	bool     bGhostMoveMode;  // ¿Modo fantasma (andar con libertad) activo?
  
  };

private:
  // Tipos
  // Lista para el control de observadores de criaturas
  typedef std::list<iCCriatureObserver*> CriatureObserverList;
  typedef CriatureObserverList::iterator CriatureObserverListIt;
  // Conjunto de criaturas dentro del rango de accion
  typedef std::set<AreaDefs::EntHandle> CriaturesInRangeSet;
  typedef CriaturesInRangeSet::iterator CriaturesInRangeSetIt;

private:
  // Estructuras
  struct sObserversInfo {
	// Info asociada a los observers
	CriatureObserverList  Observers;         // Lista de observadores
	CriatureObserverList  ObPendingToRemove; // Observers pendientes de borrado
	CriatureObserverList  ObPendingToAdd;    // Observers pendientes de ser añadidos
	bool                  bNotifyActive;     // ¿Proceso de notificacion activa?	
  };

private:
  // Vbles de miembro static
  static AnimActionActiveMap m_AnimActionActiveMap; // Map de anim. de acciones activas

  // Resto de vbles de miembro
  RulesDefs::sCriatureAttrib m_Attributes;       // Atributos
  CItemContainer             m_Inventory;        // Inventario
  CEquipmentSlots            m_EquipSlots;       // Slots de equipamiento
  sWalkInfo                  m_WalkInfo;         // Comandos asociados a las ordenes   
  sObserversInfo             m_ObserversInfo;    // Info asociada a observers
  CriaturesInRangeSet        m_CriaturesInRange; // Criaturas en rango de vision
  eActOrder                  m_ActOrder;         // Orden actual
  bool                       m_bStopWalkOrder;   // Indica orden de detencion
  bool                       m_bTmpCriature;     // Flag de criatura temporal
  bool                       m_bAccesibleFloor;  // Flag de acceso a floor
      
public:
  // Constructor / destructor
  CCriature(void) { }
  ~CCriature(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const FileDefs::FileHandle& hFile,
			dword& udOffset,
			const AreaDefs::EntHandle& hCriature);  
  bool Init(const std::string& szATSprite,
			const AreaDefs::EntHandle& hEntity,
			const std::string& szName,			
			const std::string& szATPortrait,	
			const std::string& szShadow,
		    const RulesDefs::sCriatureAttrib& Attribs);
  void InitEquipmentSlots(const AreaDefs::EntHandle& hItem,
						  const RulesDefs::EquipmentSlot& Slot);  
  void End(void);

public:
  // Operacion de almacenamiento
  void Save(const FileDefs::FileHandle& hFile, 
			dword& udOffset);

public:
  // Operacion de notificacion, para cuando acabe un comando
  void EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
					const dword udInstant,
					const dword udExtraParam);

protected:
  // Dibujado   
  void Draw(const sword& swXPos, 
			const sword& swYPos,
			const GraphDefs::sLight& Light);

public:
  // Notificacion de eventos de inicio y fin de combate
  void OnStartCombatTurn(void);  
  
public:
  // Establecimiento de informacion
  void SetSymptom(const RulesDefs::eIDSymptom& Symptom,
				  const bool bActive);
  void SetHability(const RulesDefs::eIDHability& Hability,
				   const bool bSet);
  bool SetHealth(const RulesDefs::ePairValueType& Type,
				 const sword& swValue);
  void SetExAttribute(const RulesDefs::eIDExAttrib& ExAttrib, 
					  const RulesDefs::ePairValueType& Type,
					  const sword& swValue); 
  inline void SetLevel(const RulesDefs::CriatureLevel& NewLevel) {
	ASSERT(IsInitOk());
	// Establece nuevo nivel y notifica
	m_Attributes.Level = NewLevel;
	ObserversNotify(CriatureObserverDefs::LEVEL_MODIFY);
  }
  inline void SetExperience(const RulesDefs::CriatureExp& NewExp) {
	ASSERT(IsInitOk());
	// Establece nueva experiencia y notifica
	m_Attributes.Experience = NewExp;
	ObserversNotify(CriatureObserverDefs::EXPERIENCE_MODIFY);
  }
  inline void SetActionPoints(const RulesDefs::CriatureActionPoints& APoints) {
	ASSERT(Inherited::IsInitOk());
	// Establece los puntos de combate y notifica
	m_Attributes.ActionPoints = APoints;
	ObserversNotify(CriatureObserverDefs::ACTION_POINTS_MODIFY);
  }

public:
  // Obtencion de atributos
  inline RulesDefs::CriatureClass GetClass(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna la clase de la criatura
	return m_Attributes.Class;
  }
  inline RulesDefs::CriatureGenre GetGenre(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna ID del genero
	return m_Attributes.Genre;	
  }
  inline sword GetHealth(const RulesDefs::ePairValueType& Type) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el valor de salud requerido
	return (RulesDefs::BASE_VALUE == Type) ? m_Attributes.Health.swBase : 
											 m_Attributes.Health.swTemp;
  }
  inline RulesDefs::CriatureLevel GetLevel(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el nivel
	return m_Attributes.Level;
  }
  inline RulesDefs::CriatureExp GetExperience(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna experiencia
	return m_Attributes.Experience;
  }
  inline RulesDefs::CriatureActionPoints GetActionPoints(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna los puntos de combate
	return m_Attributes.ActionPoints;
  }
  inline sword GetExAttribute(const RulesDefs::eIDExAttrib& ExAttrib, 
							  const RulesDefs::ePairValueType& Type) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el valor de atributo extendido requerido
	return (CriatureDefs::BASE_VALUE == Type) ? m_Attributes.ExAttribs[ExAttrib].swBase : 
												m_Attributes.ExAttribs[ExAttrib].swTemp;
  }
  inline bool IsSymptomActive(const RulesDefs::eIDSymptom& Symptom) const {
	ASSERT(Inherited::IsInitOk());	
	// Comprueba si el sintoma recibido esta activo
	return (m_Attributes.Symptoms & Symptom) ? true : false;
  }
  inline bool IsHabilityActive(const RulesDefs::eIDHability& Hability) const {
	ASSERT(Inherited::IsInitOk());
	// Comprueba si la habilidad recibida esta activa
	return (m_Attributes.Habilities & Hability) ? true : false;
  }
  
public:
  // CWorldEntity / Obtencion del atributo Type
  RulesDefs::BaseType GetType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el atributo tipo de la entidad
	return m_Attributes.Type;
  }

public:
  // Obtencion de otra informacion
  inline bool IsTmpCriature(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna flag
	return m_bTmpCriature;
  }

public:
  // Metodos relacionados con la orden de desplazamiento  
  bool Walk(const AreaDefs::sTilePos& TileDest,
			const word uwMinDistance = 0,
			iCCommandClient* const pClient = NULL,
			const CommandDefs::IDCommand& IDCommand = 0,
			const dword udExtraParam = 0);  
  void SetRunMode(const bool bRunMode);
  void StopWalk(const bool bRemoveCmd = false);  
  inline bool IsInRunMode(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el flag
	return m_WalkInfo.bRunMode;
  }
  inline void SetGhostMoveMode(const bool bSet) {
	ASSERT(Inherited::IsInitOk());
	// Se establece el valor
	m_WalkInfo.bGhostMoveMode = bSet;
  }
  inline bool IsGhostMoveModeActive(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el flag
	return m_WalkInfo.bGhostMoveMode;
  }
  inline bool IsWalking(void) const { 
	ASSERT(Inherited::IsInitOk());
	// Se retorna flag de actividad en comando de movimiento
	return m_WalkInfo.MoveCmd.IsActive(); 
  }
private:
  // Metodos de apoyo
  inline byte GetWalkAnimState(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el estado de animacion asociado al modo de desplazamiento actual
	return (m_WalkInfo.bRunMode) ? RulesDefs::AS_RUN : RulesDefs::AS_WALK;  			
  } 

public:
  // Realizacion de acciones  
  RulesDefs::eCriatureActionResult GetItem(const AreaDefs::EntHandle& hItem);
  RulesDefs::eCriatureActionResult DropItem(const AreaDefs::EntHandle& hItem);
  RulesDefs::eCriatureActionResult UseItem(const AreaDefs::EntHandle& hItem,
										   const AreaDefs::EntHandle& hEntity);
  RulesDefs::eCriatureActionResult Manipulate(const AreaDefs::EntHandle& hEntity);
  RulesDefs::eCriatureActionResult Observe(const AreaDefs::EntHandle& hEntity);
  RulesDefs::eCriatureActionResult UseHability(const AreaDefs::EntHandle& hEntity,
											   const RulesDefs::eIDHability& Hability);
  RulesDefs::eCriatureActionResult EquipItem(const AreaDefs::EntHandle& hItem,
											 const RulesDefs::EquipmentSlot& Slot);
  RulesDefs::eCriatureActionResult UnequipItemAt(const RulesDefs::EquipmentSlot& Slot);
  RulesDefs::eCriatureActionResult Hit(const AreaDefs::EntHandle& hEntity,
									   const RulesDefs::eIDEquipmentSlot& WeaponSlot);  
private:
  // Metodos de apoyo
  bool CanExecuteActionInCombat(const RulesDefs::eCriatureAction& CombatAction,
								const AreaDefs::EntHandle& hEntity = 0,
								const dword udParam = 0);
  inline bool CanExecuteOrder(void) const {
	ASSERT(Inherited::IsInitOk());
	// Comprobara si es posible realizar una orden. 
	// NO sera posible si la criatura se halla realizando otra orden o 
	// esta muerta (salud temporal menor o igual a 0).
	if (m_ActOrder != CCriature::AO_NONE ||
	    GetHealth(RulesDefs::TEMP_VALUE) <= 0) {
	  return false;
	} else {
	  return true;
	}
  }
  
  inline CommandDefs::IDCommand MakeIDComandForAction(const dword udAction,
													  const AreaDefs::EntHandle& hEntity) {
	ASSERT(Inherited::IsInitOk());
	// La accion en los primeros 16 bits y la entidad sobre la que recae en los
	// ultimos 16 bits
	return ((udAction << 16) | hEntity);
  }
  
  

public:
  // CWorldEntity / Obtencion del contenedor
  inline iCItemContainer* const GetItemContainer(void) {
	ASSERT(Inherited::IsInitOk());
	// Se retorna la direccion
	return &m_Inventory;
  } 

public:
  // Obtencion de los slots de equipamiento
  inline iCEquipmentSlots* const GetEquipmentSlots(void) {
	ASSERT(Inherited::IsInitOk());
	// Retorna interfaz a slots de equipamiento
	return &m_EquipSlots;
  }  

public:
  // CWorldEntity / Obtencion de informacion  
  inline AreaDefs::MaskTileAccess GetObstacleMask(void) const { 
	ASSERT(Inherited::IsInitOk());
	// Se retorna obstaculizacion en tiles
	return m_bAccesibleFloor ? AreaDefs::ALL_TILE_ACCESS : AreaDefs::NO_TILE_ACCESS;
  }

public:
  // CEntity / Obtencion de informacion
  inline RulesDefs::eEntityType GetEntityType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se devuelve tipo de entidad
	return RulesDefs::CRIATURE;
  }

public:
  // CWorldEntity / Obtencion de flag de interaccion
  bool IsInteractuable(void) const {
	ASSERT(Inherited::IsInitOk());
	// Las criaturas siempre seran interactuables
	return true;
  }

private:
  // Trabajo con los estados de animacion previos a determinadas acciones
  void SetAnimOfAction(const sAnimActionActive::eAction& Action,
					   const dword udParam);
  inline bool IsAnimOfActionInProgress(void) {
	ASSERT(Inherited::IsInitOk());
	// Se comprueba desde una accion que NO tiene asociada anim. en progreso	
	// Bastara con ver que hay una instancia
	return (m_AnimActionActiveMap.find(Inherited::GetHandle()) != m_AnimActionActiveMap.end());
  }
  inline bool IsAnimOfActionInProgress(const sAnimActionActive::eAction& Action) {
	ASSERT(Inherited::IsInitOk());
	// Se comprueba desde una accion que SI tiene asociada anim. en progreso
	const AnimActionActiveMapIt It(m_AnimActionActiveMap.find(Inherited::GetHandle()));
	if (It != m_AnimActionActiveMap.end()) {
	  return (It->second->Action == Action && !It->second->bIsInProgress) ? false : true;
	}
	// No esta activa
	return false;         
  }

public:
  // Establecimiento del flag de criatura temporal
  inline void SetTempCriature(const bool bSet) {
	ASSERT(IsInitOk());
	// Establece el flag
	m_bTmpCriature = bSet;
  }

public:
  // Trabajo con el modo transparencia
  void SetTransparentMode(const bool bSet);
  bool IsTransparentMode(void);

public:
  // Trabajo con observadores
  void AddObserver(iCCriatureObserver* const pObserver);
  void RemoveObserver(iCCriatureObserver* const pObserver);
private:
  // Metodos de apoyo
  void ObserversNotify(const CriatureObserverDefs::eObserverNotifyType& NotifyType,
					   const dword udParams = 0);

public:
  // iCAnimTemplateObserver / Notificacion de la plantilla de animacion
  void AnimTemplateObserverNotify(const AnimTemplateObserverDefs::eObserverNotifyType& NotifyType,
								  const dword udParam = 0);
private:
  // Metodos de apoyo
  void OnAnimOfActionInProgressEnd(const dword udParam);  

public:
  // CEntity / Trabajo con la pausa
  void SetPause(const bool bPause);

public:
  // CWorldEntity / Establecimiento de la localizacion de la entidad
  void SetTilePos(const AreaDefs::sTilePos& TilePos);

public:
  // Operaciones sobre el rango de vision
  inline RulesDefs::CriatureRange GetRange(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el rango de la criatura
	return m_Attributes.Range;
  }  
  inline bool IsInRange(const AreaDefs::EntHandle& hCriature) const {
	ASSERT(Inherited::IsInitOk());
	// Comprueba si la criatura recibida se halla dentro del rango de la propia
	return (m_CriaturesInRange.find(hCriature) != m_CriaturesInRange.end());
  }

private:
  // Operacion de notificacion para criaturas
  void CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
							  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
							  const dword udParam);
public:
  // iCScriptClient / Operacion de notificacion, para cuando acabe un comando
  void ScriptNotify(const RulesDefs::eScriptEvents& ScriptEvent,
				    const ScriptClientDefs::eScriptNotify& Notify,
					const dword udParams);
};

#endif // ~ #ifdef _CCRIATURE_H_
