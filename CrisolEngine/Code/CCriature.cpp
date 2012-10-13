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
// CCriature.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CCriature.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CCriature.h"

#include "iCGraphicSystem.h"
#include "iCCommandManager.h"
#include "iCGameDataBase.h"
#include "iCWorld.h"
#include "iCCriatureObserver.h"
#include "iCGUIManager.h"
#include "iCEntitySelector.h"
#include "iCGFXManager.h"
#include "iCFileSystem.h"
#include "iCVirtualMachine.h"
#include "CMemoryPool.h"
#include "CItem.h"
#include <algorithm>

// Vbles de miembro static
CCriature::AnimActionActiveMap CCriature::m_AnimActionActiveMap;

// Inicializacion de los manejadores de memoria
CMemoryPool 
CCriature::sAnimActionActive::m_MPool(8, sizeof(CCriature::sAnimActionActive), true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia leyendo datos desde archivo.
// Parametros:
// - hFile. Handle al fichero.
// - udOffset. Offset desde donde comenzar a leer.
// - hCriature. Handle de la criatura.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - El area que ocupe una criatura no podra ser ocupado por otra criatura.
///////////////////////////////////////////////////////////////////////////////
bool 
CCriature::Init(const FileDefs::FileHandle& hFile,
				dword& udOffset,
				const AreaDefs::EntHandle& hCriature)
{
  // SOLO si parametros validos
  ASSERT(hFile);
  ASSERT(hCriature);

  // Se inicializa clase base
  if (Inherited::Init(hFile, udOffset, hCriature)) {
	// Se cargan valores
    // Tipo
	iCFileSystem* pFileSys = SYSEngine::GetFileSystem();
	ASSERT(pFileSys);
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(&m_Attributes.Type),
							   sizeof(RulesDefs::BaseType),
							   udOffset);
	
	// Clase
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(&m_Attributes.Class),
							   sizeof(RulesDefs::BaseType),
							   udOffset);
	
	// Genero
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(&m_Attributes.Genre),
							   sizeof(RulesDefs::BaseType),
							   udOffset);
	
	//  Nivel / Experiencia
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(&m_Attributes.Level),
							   sizeof(RulesDefs::CriatureLevel),
							   udOffset);
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(&m_Attributes.Experience),
							   sizeof(RulesDefs::CriatureExp),
							   udOffset);
	
	// Atributos
	// Puntos de combate
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(&m_Attributes.ActionPoints),
							   sizeof(RulesDefs::CriatureActionPoints),
							   udOffset);
	
	// Salud
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(&m_Attributes.Health),
							   sizeof(RulesDefs::sPairValue),
							   udOffset);
	
	// Atributos extendidos
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(m_Attributes.ExAttribs),
							   sizeof(RulesDefs::sPairValue) * RulesDefs::MAX_CRIATURE_EXATTRIB,
							   udOffset);	

	// Habilidades
	m_Attributes.Habilities = 0x00;
	byte ubIt = 0;
	for (; ubIt < RulesDefs::MAX_CRIATURE_HABILITIES; ++ubIt) {
	  // Se lee flag y establece si procede
	  bool bFlag = false;
	  udOffset += pFileSys->Read(hFile, (sbyte *)(&bFlag), sizeof(bool), udOffset);	
	  m_Attributes.Habilities |= bFlag ? (0x01 << ubIt) : 0;
	}
	
	// Sintomas. Si no se halla se supone que no se posee.
	m_Attributes.Symptoms = 0x0000;
	ubIt = 0;
	for (; ubIt < RulesDefs::MAX_CRIATURE_SYMPTOMS; ++ubIt) {
	  // Se lee flag y establece si procede
	  bool bFlag = false;
	  udOffset += pFileSys->Read(hFile, (sbyte *)(&bFlag), sizeof(bool), udOffset);		  
	  m_Attributes.Symptoms |= bFlag ? (0x0001 << ubIt) : 0;
	}  

	// Rango
	udOffset += pFileSys->Read(hFile, 
							  (sbyte *)(&m_Attributes.Range), 
							  sizeof(RulesDefs::CriatureRange), 
							  udOffset);		  	
	
	// Se lee flag de modo transparencia y establece
	bool bFlag = false;
	udOffset += pFileSys->Read(hFile, (sbyte *)(&bFlag), sizeof(bool), udOffset);	
	SetTransparentMode(bFlag);
	
	// Se lee flag de modo correr y se establece
	udOffset += pFileSys->Read(hFile, (sbyte *)(&bFlag), sizeof(bool), udOffset);		
	SetRunMode(bFlag);	

	// Se lee flag de movimiento en modo fantasma
	udOffset += pFileSys->Read(hFile, (sbyte *)(&bFlag), sizeof(bool), udOffset);
	SetGhostMoveMode(bFlag);

	// Flag de acceso a floor
	udOffset += pFileSys->Read(hFile, (sbyte *)(&m_bAccesibleFloor), sizeof(bool), udOffset);
		
	// Se inicializan resto de valores
    // Se inicializa inventario
    m_Inventory.Init(Inherited::GetHandle());
    ASSERT(m_Inventory.IsInitOk());	 
	  
    // Se inicializan los slots de equipamiento
    m_EquipSlots.Init(Inherited::GetHandle());
    ASSERT(m_EquipSlots.IsInitOk());	  
	
    // Se instala como observador de la plantilla de animacion
    Inherited::GetAnimTemplate()->AddObserver(this);

	// Establece estado de animacion inicial 
	// Nota: Es necesario porque CSprite guardara el estado de animacion actual
	// y las criaturas son las unicas entidades que pueden ejecutar ordenes
	// concretas. Como a la hora de cargar, esas ordenes se cancelan, se debera
	// de establecer estado de reposo SALVO si esta muerta, en cuyo caso se
	// establecera el estado pertinente	
	if (GetHealth(RulesDefs::TEMP_VALUE) > 0) {
	  Inherited::GetAnimTemplate()->SetState(RulesDefs::AS_STOP);
	} else {
	  Inherited::GetAnimTemplate()->SetState(RulesDefs::AS_DEATH);
	}

	// Notificacion de no vision en pantalla
	VisibleInScreen(false);

	// Resto de vbles
	m_ObserversInfo.bNotifyActive = false;	
	m_bStopWalkOrder = false;
	m_bTmpCriature = false;
	m_CriaturesInRange.clear();
	m_ActOrder = CCriature::AO_NONE;
  }

  // Se retorna resultado
  return Inherited::IsInitOk();
}  

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia.
// - Este metodo sera necesario para cuando haya una inicializacion por
//   parte del jugador.
// Parametros:
// - szATSprite. Plantilla de animacion al sprite.
// - szName. Nombre del jugador.
// - szATPortrait. Plantilla de animacion con el retrato.
// - szShadow. Nombre del archivo con la sombra.
// - hEntity. Handle a la entidad
// - Attribs. Atributos asociados a las criaturas.
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
// - No permite reinicializar
///////////////////////////////////////////////////////////////////////////////
bool 
CCriature::Init(const std::string& szATSprite,
				const AreaDefs::EntHandle& hEntity,
				const std::string& szName,
				const std::string& szATPortrait,		
				const std::string& szShadow,
				const RulesDefs::sCriatureAttrib& Attribs)
{
  // SOLO si parametros correctos
  ASSERT(szATSprite.size());
  ASSERT(hEntity);
  
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  // Se inicializa clase base
  if (Inherited::Init(szATSprite, 
					  hEntity, 					  
					  szName, 
					  szATPortrait,
					  szShadow)) {
	// Se copian atributos
	m_Attributes = Attribs;
	
	// Se inicializa inventario
	m_Inventory.Init(Inherited::GetHandle());
	ASSERT(m_Inventory.IsInitOk());	 		

	// Se inicializan los slots de equipamiento
	m_EquipSlots.Init(Inherited::GetHandle());	  
	ASSERT(m_EquipSlots.IsInitOk());

	// Se inicializa info de desplazamiento
	SetRunMode(false);
	SetGhostMoveMode(false);

	// Se instala como observador de la plantilla de animacion
	Inherited::GetAnimTemplate()->AddObserver(this);	

	// Notificacion de no vision en pantalla
	VisibleInScreen(false);

	// Resto de vbles
	m_ObserversInfo.bNotifyActive = false;
	m_bTmpCriature = false;
	m_bAccesibleFloor = false;
	m_CriaturesInRange.clear();
	m_ActOrder = CCriature::AO_NONE;
  }

  // Retorna resultado
  return Inherited::IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa los slots de equipamiento, equipando directamente el item 
//   recibido.
// - Este metodo sera llamado por el area cuando proceda a cargar criaturas.
// Parametros:
// - hItem. Item a equipar.
// - Slot. Slot donde equipar
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCriature::InitEquipmentSlots(const AreaDefs::EntHandle& hItem,
							  const RulesDefs::EquipmentSlot& Slot)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(hItem);

  // Se equipa directamente
  m_EquipSlots.EquipItem(hItem, Slot);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia.
// Parametros:
// Devuelve:
// Notas:
// - No se desvinculara si la criatura es cliente de otra porque esta ultima
//   se halla en su rango, pues la lista de observadores se elimina de forma
//   automatica.
///////////////////////////////////////////////////////////////////////////////
void 
CCriature::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {
	// Notifica su destruccion inmediata	
	ObserversNotify(CriatureObserverDefs::DESTROY, 0);

	// A continuacion se recorrera el conjunto de criaturas que estaban en
	// su rango con el objetivo de desvicular a esta de las mismas
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);
	CriaturesInRangeSetIt RangeIt(m_CriaturesInRange.begin());
	for (; RangeIt != m_CriaturesInRange.end(); RangeIt = m_CriaturesInRange.erase(RangeIt)) {
	  // Se toma instancia
	  CCriature* const pCriature = pWorld->GetCriature(*RangeIt);
	  if (pCriature) {
		// Se elimina esta manualmente de la misma
		pCriature->m_CriaturesInRange.erase(Inherited::GetHandle());
		pCriature->RemoveObserver(this);
	  }
	}

	// Se libera posible lista de observers (deberia de estar ya vaciada)
	m_ObserversInfo.Observers.clear();
	
	// Se desasocia posible alineacion de combate
	SYSEngine::GetCombatSystem()->SetAlingment(Inherited::GetHandle(),
											   CombatSystemDefs::NO_ALINGMENT);

	// Orden de finalizacion de movimiento y desaloja del comando
	StopWalk(true);	

	// Se finalizan contenedores
	m_Inventory.End();
	m_EquipSlots.End();

	// Se desinstala como observer de la plantilla de animacion
	// y en caso de existir alguna anim. de accion en progreso, se eliminara
	Inherited::GetAnimTemplate()->RemoveObserver(this);
	const AnimActionActiveMapIt AnimIt(m_AnimActionActiveMap.find(Inherited::GetHandle()));	
	if (AnimIt != m_AnimActionActiveMap.end()) {
	  delete AnimIt->second;
	  m_AnimActionActiveMap.erase(AnimIt);
	} 

	// Se eliminan los posibles scripts pendientes
	SYSEngine::GetVirtualMachine()->ReleaseScripts(this);

	// Finaliza clase base
	Inherited::End();	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Guarda los datos propios de una criatura.
// Parametros:
// - hFile. Handle al fichero donde almacenar.
// - udOffset. Offset donde depositar la info.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCriature::Save(const FileDefs::FileHandle& hFile, 
				dword& udOffset)
{
  // SOLO si parametros correctos
  ASSERT(hFile);

  // Propaga
  Inherited::Save(hFile, udOffset);

  // Almacena tipo
  iCFileSystem* pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys);
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&m_Attributes.Type), 
							  sizeof(RulesDefs::BaseType));

  // Almacena clase
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&m_Attributes.Class), 
							  sizeof(RulesDefs::BaseType));  

  // Genero
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&m_Attributes.Genre), 
							  sizeof(RulesDefs::BaseType));  

  // Nivel / Experiencia
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&m_Attributes.Level), 
							  sizeof(RulesDefs::CriatureLevel));  
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&m_Attributes.Experience), 
							  sizeof(RulesDefs::CriatureExp));  

  // Atributos / Ptos de combate
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&m_Attributes.ActionPoints), 
							  sizeof(RulesDefs::CriatureActionPoints));  

  // Salud
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&m_Attributes.Health), 
							  sizeof(RulesDefs::sPairValue));  

  // Atributos extendidos
  udOffset += pFileSys->Write(hFile, 
							  udOffset,
							  (sbyte *)(m_Attributes.ExAttribs),
							  sizeof(RulesDefs::sPairValue) * RulesDefs::MAX_CRIATURE_EXATTRIB);	
  // Habilidades
  byte ubIt = 0;
  for (; ubIt < RulesDefs::MAX_CRIATURE_HABILITIES; ++ubIt) {
    // Se lee el flag asociado y se escribe
    const bool bFlag = (m_Attributes.Habilities & (0x01 << ubIt)) ? true : false;
    udOffset += pFileSys->Write(hFile, udOffset, (sbyte *)(&bFlag), sizeof(bool));	
  }

  // Sintomas
  ubIt = 0;
  for (; ubIt < RulesDefs::MAX_CRIATURE_SYMPTOMS; ++ubIt) {
    // Se lee flag y establece si procede
    const bool bFlag = (m_Attributes.Symptoms & (0x0001 << ubIt)) ? true : false;
    udOffset += pFileSys->Write(hFile, udOffset, (sbyte *)(&bFlag), sizeof(bool));
  }  

  // Rango
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&m_Attributes.Range), 
							  sizeof(RulesDefs::CriatureRange));

  // Flag de transparencia
  const bool bFlag = IsTransparentMode();
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&bFlag), 
							  sizeof(bool));

  // Flag de correr
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&m_WalkInfo.bRunMode), 
							  sizeof(bool));		

  // Flag de modo fantasma
  udOffset += pFileSys->Write(hFile,
							  udOffset,
							  (sbyte *)(&m_WalkInfo.bGhostMoveMode),
							  sizeof(bool));

  // Flag de acceso a floor
  udOffset += pFileSys->Write(hFile, 
	                          udOffset,
							  (sbyte *)(&m_bAccesibleFloor), 
							  sizeof(bool));	   

  // Flag de criatura temporal
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&m_bTmpCriature), 
							  sizeof(bool));		

  // Obtiene el posible tag asociado con esta entidad y lo guarda
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  udOffset += pFileSys->WriteStringInBinary(hFile, 
											udOffset, 
											pWorld->GetTagFromHandle(Inherited::GetHandle()));
  
  // Elevacion
  const RulesDefs::Elevation Elevation = Inherited::GetElevation();
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&Elevation), 
							  sizeof(RulesDefs::Elevation));

  // Luz
  const GraphDefs::Light Light = pWorld->GetLight(Inherited::GetHandle());
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&Light), 
							  sizeof(GraphDefs::Light));

  // Items en inventario
  m_Inventory.Save(hFile, udOffset);

  // Items equipados
  m_EquipSlots.Save(hFile, udOffset);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado de la criatura. En este metodo en concreto, simplemente
//   se comprobara si el modo combate esta activo, en cuyo caso se obtendra
//   el posible selector de la criatura y se dibujara. El selector de combate
//   de una criatura, es Sprite base que se asocia a todas las criaturas
//   alineadas en un determinado bando (jugador o enemigo del jugador) para
//   indicar visualmente su alineacion. 
// - Una vez dibujado el posible selector de combate, la orden se propagara
//   a la clase base.
// Parametros:
// - swXPos, swYPos. Posicion de dibujado.
// - Light. Estructura con la informacion de luz asociada a entidades
// Devuelve:
// Notas:
// - El selector de combate solo se dibujara cuando la criatura no se este
//   moviendo ya que en caso de moverse, el orden de dibujado haria que 
//   los tiles adyacentes macharan al selector.
///////////////////////////////////////////////////////////////////////////////
void 
CCriature::Draw(const sword& swXPos, 
				const sword& swYPos,
				const GraphDefs::sLight& Light)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Se esta en modo combate?
  iCCombatSystem* const pCombatSys = SYSEngine::GetCombatSystem();
  ASSERT(pCombatSys);	
  if (pCombatSys->IsCombatActive()) {
    // Si, Se toma el posible selector
    iCEntitySelector* const pSelector = pCombatSys->GetCombatientSelector(Inherited::GetHandle());
    // ¿Hay selector?
    if (pSelector) {
  	  // Si, se configura y se dibuja en pantalla
  	  const sPosition DrawPos(swXPos + Inherited::GetXPos(),
  							  Inherited::GetYPos() + swYPos);
  	  pSelector->Draw(DrawPos);  
    }
  }  
  
  // Se propaga
  Inherited::Draw(swXPos, swYPos, Light);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Detiene el comando de movimiento sacandolo de la cola de comandos para que
//   no se produzca ningun tipo de aviso. Al no producirse un aviso, la accion
//   que se estuviera realizando no se ejecutara.
// Parametros:
// - bRemoveCmd. Flag indicativo de desalojo del comando.
// Devuelve:
// Notas:
// - Se utilizara el flag m_bStopWalkOrder con el objetivo de que la
//   notificacion del sistema de animacion al recibir un cambio de estado
//   asociado a parada no vuelva a ordenar la accion que estaba en progreso.
///////////////////////////////////////////////////////////////////////////////
void 
CCriature::StopWalk(const bool bRemoveCmd) 
{ 
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // ¿Comando activo?
  if (m_WalkInfo.MoveCmd.IsActive()) {
	// Se levanta flag de orden de parada forzada
	// Nota: Es muy importante que se haga ANTES de parar el comando
	// para que tenga efecto su accion.
	m_bStopWalkOrder = true;

	// ¿Finalizacion inmediata?
	if (bRemoveCmd) {
	  // Si, se ordena finalizacion inmediata y se extrae comando
	  m_WalkInfo.MoveCmd.StopWalk(true);
	  SYSEngine::GetCommandManager()->QuitCommand(&m_WalkInfo.MoveCmd);	  
	} else {
	  // No, se finaliza progresivamente
	  m_WalkInfo.MoveCmd.StopWalk(false); 		
	}
	
	// Se notifica a los observers
	ObserversNotify(CriatureObserverDefs::ACTION_REALICE, 
					RulesDefs::CA_STOPWALK);

	// Se establece valor de orden
	m_ActOrder = CCriature::AO_NONE;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el modo correr / andar y la velocidad asociada al mismo.
// Parametros:
// - bRunMode. Flag de modo correr / caminar
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCriature::SetRunMode(const bool bRunMode) 
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Establece el flag de modo correr
  m_WalkInfo.bRunMode = bRunMode;
  
  // Establece la velocidad asociada
  Inherited::SetSpeed(bRunMode ? RulesDefs::CRIATURE_RUN_SPEED : RulesDefs::CRIATURE_WALK_SPEED);  

  // ¿Esta activo el comando de desplazamiento?
  if (m_WalkInfo.MoveCmd.IsActive()) {
	// Si, se cambia el estado de animacion al asociado con el modo de desp.
	Inherited::GetAnimTemplate()->SetState(GetWalkAnimState());
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ordena a la criatura moverse a un destino.
// - Al ordenarla, se puede recibir una notificacion cuando dicha accion
//   termine pasando el ciente y su identificador. Por defecto, dichos valores
//   seran NULL y 0.
// - Se podra ordenar que la criatura cambie el destino aunque no haya
//   finalizado el actuar SOLO si no se esta en modo combate.
// - El movimiento tendra en cuenta el flag de modo fantasma para ser realizado
// Parametros:
// - TileDest. Tile destino.
// - uwMinDistance. Distancia minima en caso de querer usar un camino de 
//   minima distancia. El valor por defecto sera de 0.
// - pClient. Cliente al que notificar el final del movimiento.
// - IDCommand. Identificador asociado al cliente.
// - udExtraParam. Parametro extra
// Devuelve:
// - Si se puede andar true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CCriature::Walk(const AreaDefs::sTilePos& TileDest,		
				const word uwMinDistance,
				iCCommandClient* const pClient,
				const CommandDefs::IDCommand& IDCommand,
				const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿NO se puede acatar la orden? o
  // ¿NO se puede intentar realizar la accion en combate?
  if (!CanExecuteOrder() ||
      !CanExecuteActionInCombat(RulesDefs::CA_WALK)) {
	// Se abandona
	return false;
  } 

  // Si estaba andando, se detiene, generando una notificacion
  // y se baja el flag de detencion
  StopWalk();
  m_bStopWalkOrder = false;

  // Se inicializa el comando
  bool bResult;
  if (m_WalkInfo.MoveCmd.Init(this, 
							  TileDest, 
							  GetWalkAnimState(), 
							  uwMinDistance,							  
							  IsGhostMoveModeActive())) {
	// Se postea en el sistema de comandos
	bResult = true;
    SYSEngine::GetCommandManager()->PostCommand(&m_WalkInfo.MoveCmd, 
												pClient, 
												IDCommand,
												udExtraParam);	

	// Se establece valor de orden
	m_ActOrder = CCriature::AO_MOVETO;
  } else if (RulesDefs::PLAYER == GetEntityType()) {
	// Se envia mensaje de movimiento no posible SOLO si se trata del jugador
	const std::string szMsg(SYSEngine::GetGameDataBase()->GetStaticText(GameDataBaseDefs::ST_GENERAL_CANT_MOVE_TO));
	SYSEngine::GetGUIManager()->WriteToConsole(szMsg, true);
	bResult = false;
  }
  
  // Se notifica que se ha recibido una orden de movimiento
  ObserversNotify(CriatureObserverDefs::WALK_ORDER);

  // Retorna resultado  
  return bResult;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ordena a la criatura que coja el item hItem que se supone no tendra
//   dueño. Para realizar la accion se debera de estar en pos. adyacente.
// Parametros:
// - hItem. Item a recoger.
// Devuelve:
// - El resultado de la accion, que podra ser no realizado, en progreso o
//   realizado.
// Notas:
// - Accion con animacion previa asociada sobre la criatura
///////////////////////////////////////////////////////////////////////////////
RulesDefs::eCriatureActionResult
CCriature::GetItem(const AreaDefs::EntHandle& hItem)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿NO se puede acatar la orden?
  // ¿NO se puede intentar realizar la accion en combate?
  if (!CanExecuteOrder() ||
	  !CanExecuteActionInCombat(RulesDefs::CA_GETITEM, hItem)) {
	return RulesDefs::CAR_FAIL;
  }

  // Se obtiene la posicion destino donde colocar a la criatura
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CItem* const pItem = pWorld->GetItem(hItem);
  ASSERT(pItem);
  ASSERT((0 == pItem->GetOwner()) != 0)
  AreaDefs::sTilePos DestPos;
  // ¿NO se esta en una posicion adyacente?
  if (!pWorld->IsAdjacentTo(Inherited::GetTilePos(), pItem->GetTilePos())) {
    // Se halla la pos. adyacente si es que es posible
    if (-1 == pWorld->CalculeAdjacentPosInDestination(Inherited::GetTilePos(), 
												      pItem->GetTilePos(),
  												      DestPos)) {
	  // No existe tile adyacente al destino valido
	  return RulesDefs::CAR_FAIL;
	}
  } else {
    // Si que se esta en posicion adyacente, luego pos. actual es destino
    DestPos = Inherited::GetTilePos();
  }  

  // ¿Se esta en la posicion adecuada para realizar la accion?
  if (DestPos == Inherited::GetTilePos()) {	
	// ¿NO hay que activar la anim. asociada porque no esta en progreso?	
	// Nota: Si se ha llegado aqui es porque existe nodo asociado a esta accion
	// y ademas ya no se encuentra con la animacion en progreso
	if (m_AnimActionActiveMap.find(Inherited::GetHandle()) != m_AnimActionActiveMap.end()) {
	  // Se notifica el evento para realizar la accion
	  // Nota: El script estara asociado al item que se va a coger pero
	  // sera lanzado como si estuviera asociado a la criatura pues esta es
	  // la que en ultima instancia tiene que realizar la accion.
	  SYSEngine::GetVirtualMachine()->OnGetItem(this,
										        pItem->GetScriptEvent(RulesDefs::SE_ONGET),
										        pItem->GetHandle(),
										        Inherited::GetHandle());	  
	} else {
	  // Se hace que la criatura mire al item
      const IsoDefs::eDirectionIndex Dir = pWorld->CalculeDirection(DestPos,
																    pItem->GetTilePos());
      ASSERT((Dir != IsoDefs::NO_DIRECTION_INDEX) != 0);
      Inherited::GetAnimTemplate()->SetOrientation(Dir); 
	  
	  // Se establece estado de animacion asociado a la accion
	  SetAnimOfAction(sAnimActionActive::GET_ITEM, hItem);
	}	

	// Accion en progreso
	m_ActOrder = CCriature::AO_GETITEM;
	return RulesDefs::CAR_INPROGRESS;
  } 

  // No estan en la misma posicion, se crea comando de accion y se intenta
  // posicionar sobre el lugar a la criatura
  const CommandDefs::IDCommand IDCommand = MakeIDComandForAction(CCriature::TO_GETITEM, 
															     hItem);
  if (Walk(DestPos, 0, this, IDCommand)) {
	return RulesDefs::CAR_INPROGRESS;
  } else {
	return RulesDefs::CAR_FAIL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Tira el item hItem suponiendo que dicho item se posea en el inventario o
//   bien en los slots de equipamiento.
// - El lugar en donde se tirara sera siempre sobre el tile en donde se halle
//   la criatura.
// - La accion de tirar un item no sera considerada una accion de combate.
// Parametros:
// - hItem. Item a tirar.
// Devuelve:
// - El resultado de la accion, que podra ser no realizado, en progreso o
//   realizado.
// Notas:
// - Tirar un item no consumira puntos de accion
// - Al tirar un item y estar en modo pausa la criatura, el item se debera
//   de establecer tambien en modo pausa siempre y cuando sea desalojar el
//   item desde el inventario al suelo.
///////////////////////////////////////////////////////////////////////////////
RulesDefs::eCriatureActionResult
CCriature::DropItem(const AreaDefs::EntHandle& hItem)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(hItem);

  // ¿NO se puede acatar la orden? o
  // ¿El item no se posee?
  if (!CanExecuteOrder() ||
	  (!m_Inventory.IsItemIn(hItem) && !m_EquipSlots.IsItemEquiped(hItem))) {
	return RulesDefs::CAR_FAIL;
  }

  // ¿NO se puede realizar la accion por estar en combate?
  // Nota: Esta accion no consumira puntos de combate, por lo que no se
  // delegara la comprobacion al metodo CanExecuteActionInCombat
  iCCombatSystem* const pCombatSys = SYSEngine::GetCombatSystem();
  ASSERT(pCombatSys);
  if (pCombatSys->IsCombatActive() &&
	  pCombatSys->GetCriatureInTurn() != Inherited::GetHandle()) {
	// No se puede realizar la accion
	return RulesDefs::CAR_FAIL;
  }  

  // Se lanza el evento script asociado y retorna
  // Nota: El script estara asociado al item que se va a coger pero
  // sera lanzado como si estuviera asociado a la criatura pues esta es
  // la que en ultima instancia tiene que realizar la accion.  
  CItem* const pItem = SYSEngine::GetWorld()->GetItem(hItem);
  if (!pItem) {
	return RulesDefs::CAR_FAIL;
  }
  SYSEngine::GetVirtualMachine()->OnDropItem(this,
											 pItem->GetScriptEvent(RulesDefs::SE_ONDROP),
											 hItem,
											 Inherited::GetHandle());
  m_ActOrder = CCriature::AO_DROPITEM;
  return RulesDefs::CAR_INPROGRESS;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ejecutara el metodo Use de CItem sobre la entidad hEntity.
// - Si hEntity es la propia entidad o bien es un item que posee la propia
//   entidad, el uso sera directo. En cualquier otro caso, se entendera que
//   sera una entidad que estara sobre el escenario, debiendose colocar
//   a la entidad sobre una posicion adyacente. Una vez posicionada la criatura
//   se ejecutara el metodo Use de CItem. En cualquier caso, la posibilidad
//   de uso de un item sobre otra entidad dependera de si este tiene asociado
//   un script.
// - Debido a que se utilizaran dos handles, se usara el parametro extra 
//   para la notificacion de la finalizacion del comando de caminar,
//   colocandose en el parametro extra el handle a la entidad sobre la que
//   utilizar el item.
// Parametros:
// - hItem. Item a utilizar.
// - hEntity. Entidad sobre la que queremos posicionarnos para usar un item
//   del inventario
// Devuelve:
// - El resultado de la accion, que podra ser no realizado, en progreso o
//   realizado.
// Notas:
// - Accion con animacion previa asociada sobre la criatura
///////////////////////////////////////////////////////////////////////////////
RulesDefs::eCriatureActionResult
CCriature::UseItem(const AreaDefs::EntHandle& hItem,
				   const AreaDefs::EntHandle& hEntity)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hEntity);
  
  // ¿NO se puede acatar la orden?
  // ¿NO se puede intentar realizar la accion en combate?
  if (!CanExecuteOrder() ||
      !CanExecuteActionInCombat(RulesDefs::CA_USEITEM, hEntity)) {
	return RulesDefs::CAR_FAIL;
  }

  // Si no hay script asociado, se retornara sin accion alguna
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);  
  CItem* const pItem = pWorld->GetItem(hItem);
  ASSERT(pItem);
  ASSERT((pItem->GetOwner() == Inherited::GetHandle()) != 0);  
  const std::string szScript(pItem->GetScriptEvent(RulesDefs::SE_ONUSEITEM));
  if (szScript.empty()) {
	return RulesDefs::CAR_DONE;
  }

  // ¿La entidad destino es la propia criatura? o
  // ¿La entidad destino es un item y el dueño es la propia entidad?
  CWorldEntity* const pEntity = pWorld->GetWorldEntity(hEntity);
  if (!pEntity) {
	return RulesDefs::CAR_FAIL;
  }
  if (hEntity == Inherited::GetHandle() ||
	  (pEntity->GetEntityType() == RulesDefs::ITEM &&
	  GetItemContainer()->IsItemIn(hEntity))) {
	// Si, se lanzara el posible script asociado y se notifica
	SYSEngine::GetVirtualMachine()->OnUseItem(this,
											  szScript,
											  pItem->GetHandle(),
											  hEntity,
											  Inherited::GetHandle());
	m_ActOrder = CCriature::AO_USEITEM;
	return RulesDefs::CAR_INPROGRESS;
  }
  
  // Se trata de una entidad que esta sobre el escenario por lo que
  // se debera de acercar a la posicion adecuada para usar sobre ella el item  
  AreaDefs::sTilePos DestPos;
  // ¿NO se esta en una posicion adyacente?
  if (!pWorld->IsAdjacentTo(Inherited::GetTilePos(), pEntity->GetTilePos())) {
    // Se halla la pos. adyacente si es que es posible
    if (-1 == pWorld->CalculeAdjacentPosInDestination(Inherited::GetTilePos(), 
												      pEntity->GetTilePos(),
  												      DestPos)) {
	  // No existe tile adyacente al destino valido
	  return RulesDefs::CAR_FAIL;
	}
  } else {
    // Si que se esta en posicion adyacente, luego pos. actual es destino
    DestPos = Inherited::GetTilePos();
  }  

  // ¿La entidad se halla en el tile destino?
  if (Inherited::GetTilePos() == DestPos) {	
	// ¿NO hay que activar la anim. asociada porque no esta en progreso?	
	// Nota: Si se ha llegado aqui es porque existe nodo asociado a esta accion
	// y ademas ya no se encuentra con la animacion en progreso
	if (m_AnimActionActiveMap.find(Inherited::GetHandle()) != m_AnimActionActiveMap.end()) {
	  // Se lanza el posible script asociado y se notifica
	  SYSEngine::GetVirtualMachine()->OnUseItem(this,
											    szScript,
											    pItem->GetHandle(),
											    hEntity,
											    Inherited::GetHandle());

	  // Accion en progreso
	  m_ActOrder = CCriature::AO_USEITEM;
	  return RulesDefs::CAR_INPROGRESS;
	} else {
      // Si la posicion destino fuera adjacente, se hara que la criatura
	  // mire a la entidad sobre la que usar item
	  if (pWorld->IsAdjacentTo(DestPos, pEntity->GetTilePos())) {
  		const IsoDefs::eDirectionIndex Dir = pWorld->CalculeDirection(DestPos,
																	  pEntity->GetTilePos());	  
		ASSERT((Dir != IsoDefs::NO_DIRECTION_INDEX) != 0);	  
		Inherited::GetAnimTemplate()->SetOrientation(Dir); 
	  }

	  // Se establece estado de animacion asociado a la accion
	  SetAnimOfAction(sAnimActionActive::USE_ITEM, (hItem << 16) | hEntity);

	  // Accion en progreso
	  m_ActOrder = CCriature::AO_USEITEM;
	  return RulesDefs::CAR_INPROGRESS;
	}
  }

  // Hay que aproximar a la criatura a la posicion destino
  // Se crea el identificador del comando y se la ordena andar a esa posicion
  // Nota: El parametro extra guardara el handle a la entidad
  const CommandDefs::IDCommand IDCommand = MakeIDComandForAction(CCriature::TO_USEITEM, 
															 hItem);
  
  if (Walk(DestPos, 0, this, IDCommand, hEntity)) {
	return RulesDefs::CAR_INPROGRESS;
  } else {
	return RulesDefs::CAR_FAIL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Esta es la accion de manipular una entidad. La entidad podra ser un
//   contenedor o bien una entidad normal y corriente. Las entidades que se
//   pueden someter a esta accion seran los objetos de escenario, paredes,
//   criaturas (con salud de 0) e items. En el caso de las criaturas, seran
//   tratadas en la misma forma que lo son los objetos de escenario 
//   contenedores y en el caso de los items, estos SEGURO que estaran en el
//   inventario de la criatura.
// - En el caso de estar en modo combate, la accion de manipular SOLO podra
//   llevarse a cabo si la entidad es una pared.
// Parametros:
// - hEntity. Handle a la entidad
// Devuelve:
// - El resultado de la accion, que podra ser no realizado, en progreso o
//   realizado.
// Notas:
// - Al manipular se debera de ejecutar una animacion asociada previa.
// - Accion con animacion previa asociada sobre la criatura
///////////////////////////////////////////////////////////////////////////////
RulesDefs::eCriatureActionResult
CCriature::Manipulate(const AreaDefs::EntHandle& hEntity)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(hEntity);

  // ¿NO se puede acatar la orden?
  // ¿NO se puede intentar realizar la accion en combate?
  if (!CanExecuteOrder() ||  
      !CanExecuteActionInCombat(RulesDefs::CA_MANIPULATE, hEntity)) {
	return RulesDefs::CAR_FAIL;
  }
  
  // Se comprobara que si el item a manipular se posee, se lanzara
  // el evento script asociado sin mas.
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CWorldEntity* const pEntity = pWorld->GetWorldEntity(hEntity);
  ASSERT(pEntity);
  if (pEntity->GetEntityType() == RulesDefs::ITEM) {
	// Si, se lanza posible evento script asociado si y solo si el item se posee
	if (m_Inventory.IsItemIn(hEntity)) {	  
	  SYSEngine::GetVirtualMachine()->OnManipulateEntity(this,
														 pEntity->GetScriptEvent(RulesDefs::SE_ONMANIPULATE),
														 hEntity,
														 Inherited::GetHandle());
	  
	  m_ActOrder = CCriature::AO_MANIPULATE;
	  return RulesDefs::CAR_INPROGRESS;
	} else {
	  // El item no se posee
	  return RulesDefs::CAR_FAIL;
	}
  }
  
  // ¿La criatura se halla en un tile adjacente con respecto a hEntity?
  if (pWorld->IsAdjacentTo(Inherited::GetTilePos(), pEntity->GetTilePos())) {
	// ¿NO hay que activar la anim. asociada porque no esta en progreso?	
	// Nota: Si se ha llegado aqui es porque existe nodo asociado a esta accion
	// y ademas ya no se encuentra con la animacion en progreso
	if (m_AnimActionActiveMap.find(Inherited::GetHandle()) != m_AnimActionActiveMap.end()) {
	  // Se lanza el posible script asociado al evento, teniendo en cuenta que
	  // a la vuelta, si se ha ejecutado la accion por defecto y la entidad
	  // a manipular es contenedora, se abrira el interfaz de intercambio.	  
	  SYSEngine::GetVirtualMachine()->OnManipulateEntity(this,
											             pEntity->GetScriptEvent(RulesDefs::SE_ONMANIPULATE),
											             hEntity,
														 Inherited::GetHandle());
	  m_ActOrder = CCriature::AO_MANIPULATE;
	  return RulesDefs::CAR_INPROGRESS;
	} else {
	  // Se coloca a la criatura mirando a la entidad
	  const IsoDefs::eDirectionIndex Dir = pWorld->CalculeDirection(Inherited::GetTilePos(),
																	pEntity->GetTilePos());
	  ASSERT((Dir != IsoDefs::NO_DIRECTION_INDEX) != 0);
	  Inherited::GetAnimTemplate()->SetOrientation(Dir);   

	  // Se establece estado de animacion asociado a la accion
	  SetAnimOfAction(sAnimActionActive::MANIPULATE, hEntity);	  

	  // Accion en progreso
	  m_ActOrder = CCriature::AO_MANIPULATE;
	  return RulesDefs::CAR_INPROGRESS;
	}	
  } 
  
  // Se debera de estar en un tile adjancente
  AreaDefs::sTilePos ValidTileDest;
  if (-1 != pWorld->CalculeAdjacentPosInDestination(Inherited::GetTilePos(), 
											        pEntity->GetTilePos(),
											        ValidTileDest)) {
	// Se crea el identificador del comando y se la ordena andar a esa posicion
	const CommandDefs::IDCommand IDCommand = MakeIDComandForAction(CCriature::TO_MANIPULATE, 
																   hEntity);
	if (Walk(ValidTileDest, 0, this, IDCommand)) {
	  m_ActOrder = CCriature::AO_MANIPULATE;
	  return RulesDefs::CAR_INPROGRESS;
	}
  }

  // No se ha podido realizar la operacion
  return RulesDefs::CAR_FAIL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se realizara una accion de observar a la entidad hEntity.
// - La observacion puede ser sobre una entidad cualquiera, sobre una entidad
//   que siendo item la posea la propia entidad o sobre la propia criatura.
// Parametros:
// - hEntity. Entidad a observar.
// Devuelve:
// - El resultado de la accion, que podra ser no realizado, en progreso o
//   realizado.
// Notas:
///////////////////////////////////////////////////////////////////////////////
RulesDefs::eCriatureActionResult
CCriature::Observe(const AreaDefs::EntHandle& hEntity)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(hEntity);

  // ¿NO se puede intentar realizar la accion en combate?
  if (!CanExecuteActionInCombat(RulesDefs::CA_OBSERVE, hEntity)) {
	return RulesDefs::CAR_FAIL;
  }

  // Se obtiene instancia a la entidad y se comprueba que exista
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CWorldEntity* const pEntity = pWorld->GetWorldEntity(hEntity);
  if (!pEntity) {
	return RulesDefs::CAR_FAIL;
  }

  // Vbles
  bool bCanObserve = false; // ¿Puede observarse la entidad?

  // ¿La entidad es un item que posee la criatura? o
  // ¿La entidad es la propia criatura?
  if ((pEntity->GetEntityType() == RulesDefs::ITEM &&
	   GetItemContainer()->IsItemIn(pEntity->GetHandle())) ||
	   (pEntity->GetHandle() == Inherited::GetHandle())) {
	bCanObserve = true;	
  } 
  // ¿La criatura se halla en un tile adjacente con respecto a hEntity?
  else if (pWorld->IsAdjacentTo(Inherited::GetTilePos(), pEntity->GetTilePos())) {	
	// Se coloca a la criatura mirando a la entidad
	const IsoDefs::eDirectionIndex Dir = pWorld->CalculeDirection(Inherited::GetTilePos(),
																  pEntity->GetTilePos());
	ASSERT((Dir != IsoDefs::NO_DIRECTION_INDEX) != 0);
	Inherited::GetAnimTemplate()->SetOrientation(Dir);   

	// Puede observarse
	bCanObserve = true;	
  } 

  // ¿Puede observarse directamente?
  if (bCanObserve) {
	// Operacion realizada, se realiza la notificacion logica
  	ObserversNotify(CriatureObserverDefs::ACTION_REALICE, 
				    RulesDefs::CA_OBSERVE);

	// Lanza el posible evento script asociado a la entidad observada y retorna
	CWorldEntity* const pEntity = pWorld->GetWorldEntity(hEntity);
	ASSERT(pEntity);
	pEntity->OnObserve(Inherited::GetHandle());
	return RulesDefs::CAR_DONE;
  }

  // No se puede observar directamente, hay que situar en tile adyacente
  AreaDefs::sTilePos ValidTileDest;
  if (-1 != pWorld->CalculeAdjacentPosInDestination(Inherited::GetTilePos(), 
											        pEntity->GetTilePos(),
											        ValidTileDest)) {
	// Se crea el identificador del comando y se la ordena andar a esa posicion
	const CommandDefs::IDCommand IDCommand = MakeIDComandForAction(CCriature::TO_OBSERVE, 
																   hEntity);
	Walk(ValidTileDest, 0, this, IDCommand);
  }

  // Se retorna resultado del inicio de posicionamiento
  return IsWalking() ? RulesDefs::CAR_INPROGRESS : RulesDefs::CAR_FAIL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Usa la habilidad Hability sobre la entidad hEntity. Para poder usar
//   dicha habilidad, se debera de situar a la criatura en una distancia
//   minima y ademas la habilidad debera de ser usable (esto ultimo se
//   comprobara via ASSERT).
// - La entidad destino podra ser la propia criatura tambien o un item que
//   posea la criatura.
// Parametros:
// - hEntity. Entidad sobre la que usar la habilidad.
// - Hability. Habilidad a usar.
// Devuelve:
// - El resultado de la accion, que podra ser no realizado, en progreso o
//   realizado.
// Notas:
// - Al notificar la accion de uso de habilidad, se usara en el parametro de 
//   la notificacion el formato:
//   WordSuperior -> Habilidad utilizada.
//   WordInferior -> Accion realizada.
///////////////////////////////////////////////////////////////////////////////
RulesDefs::eCriatureActionResult
CCriature::UseHability(const AreaDefs::EntHandle& hEntity,
					   const RulesDefs::eIDHability& Hability)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(hEntity);
  // SOLO si la habilidad es usable
  ASSERT(SYSEngine::GetGameDataBase()->GetRulesDataBase()->IsUsableHability(Hability));
  
  // ¿NO se posee la habilidad recibida?
  if (!IsHabilityActive(Hability)) {
	return RulesDefs::CAR_FAIL;
  }

  // ¿NO se puede intentar realizar la accion en combate?
  if (!CanExecuteActionInCombat(RulesDefs::CA_USEHABILITY, hEntity, Hability)) {
	return RulesDefs::CAR_FAIL;
  }
  
  // Se toma instancia a entidad y se comprueba que exista
  iCWorld* pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CWorldEntity* const pEntity = pWorld->GetWorldEntity(hEntity);
  if (!pEntity) {
	return RulesDefs::CAR_FAIL;
  }

  // Vbles
  word uwMinDist = 1;   // Distancia minima
  bool bCanUse = false; // ¿Se puede usar la habilidad?

  // ¿Se desea usar sobre la propia criatura? o
  // ¿Si se trata de un item, ¿La criatura posee dicho item?
  if (Inherited::GetHandle() == hEntity || 
	  (pWorld->GetEntityType(hEntity) == RulesDefs::ITEM &&
	  GetItemContainer()->IsItemIn(hEntity))) {
	// Se puede usar
	bCanUse = true;
  } else {
	// Se obtiene la distancia minima
	// Nota: Es seguro que la minima distancia valdra como minimo 1
	uwMinDist = SYSEngine::GetGameDataBase()->GetRulesDataBase()->GetMinDistanceFor(RulesDefs::MD_USEHABILITY,
																					Hability); 
	ASSERT(uwMinDist);

	// ¿Se puede usar habilidad (al estar a distancia minima)?
    if (uwMinDist >= pWorld->CalculeAbsoluteDistance(Inherited::GetTilePos(),
												     pEntity->GetTilePos())) {
	  // Se puede usar
	  bCanUse = true;
	}
  }

  // ¿Se puede usar la habilidad?
  if (bCanUse) {
	// Llegados a este punto, es seguro que la criatura se halla en posicion
	// deseada para poder realizar la accion de uso de habilidad
	// Se coloca a la criatura mirando a la entidad si procede
	const IsoDefs::eDirectionIndex Dir = pWorld->CalculeDirection(Inherited::GetTilePos(),
																  pEntity->GetTilePos());

	if (Dir != IsoDefs::NO_DIRECTION_INDEX) {
	  Inherited::GetAnimTemplate()->SetOrientation(Dir);   
	}

	// Se lanza el posible evento script, pasando como parametro la informacion
	// necesaria para que luego se pueda notificar a los clientes
	SYSEngine::GetVirtualMachine()->OnUseHability(this,
												  Inherited::GetScriptEvent(RulesDefs::SE_ONUSEHABILITY),
												  Inherited::GetHandle(),
												  Hability,
												  pEntity->GetHandle());
	return RulesDefs::CAR_INPROGRESS;
  }

  // En el caso de que no se cumplan las condiciones de distancia para usar la 
  // habilidad, se debera de intentar situar a la criatura
  // Se debera de estar en un tile adjancente o distancia minima
  // Se crea el identificador del comando y se la ordena andar a esa posicion
  const CommandDefs::IDCommand IDCommand = MakeIDComandForAction(CCriature::TO_USEHABILITY, 
																 hEntity);  
  if (Walk(pEntity->GetTilePos(), uwMinDist, this, IDCommand, Hability)) {
	return RulesDefs::CAR_INPROGRESS;
  } else {
	return RulesDefs::CAR_FAIL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la accion de golpear a una entidad por parte de la
//   criatura. Para ello, se indicara el slot principal usado para golpear.
//   En caso de que en dicho slot no exista un arma ofensiva (un item que
//   tenga coste de uso superior a 0 o bien que se usen los puños) no se
//   realizara accion.
// Parametros:
// - hEntity. Entidad a la que se va a golpear.
// - WeaponSlot. Slot donde esta el arma para golpear.
// Devuelve:
// - El resultado de la accion, que podra ser no realizado, en progreso o
//   realizado.
// Notas:
///////////////////////////////////////////////////////////////////////////////
RulesDefs::eCriatureActionResult
CCriature::Hit(const AreaDefs::EntHandle& hEntity,
			   const RulesDefs::eIDEquipmentSlot& WeaponSlot)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(hEntity);

  // ¿NO se puede acatar la orden? o
  // ¿NO se puede intentar realizar la accion en combate?
  // ¿Se desea golpear sobre la misma entidad? o
  // ¿La entidad a golpear es un item?
  iCWorld* pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CWorldEntity* const pEntity = pWorld->GetWorldEntity(hEntity);  
  ASSERT(pEntity);
  if (!CanExecuteOrder() ||
      !CanExecuteActionInCombat(RulesDefs::CA_HIT, hEntity, WeaponSlot) ||
	  Inherited::GetHandle() == hEntity ||
	  pEntity->GetEntityType() == RulesDefs::ITEM) {
	return RulesDefs::CAR_FAIL;
  }
  
  #ifdef _SYSASSERT
	// ¿Es un slot NO valido?    
	if (!(WeaponSlot == RulesDefs::EQUIPMENT_SLOT_0 || 
	      WeaponSlot == RulesDefs::EQUIPMENT_SLOT_1)) {
	  ASSERT_MSG(false, "No se esta atacando con un slot principal!");
	}
  #endif
  
  // ¿En el slot WeaponSlot NO hay equipado un arma ofensiva?
  // Nota: Un arma ofensiva sera aquel item equipado de coste de uso > 0
  // Cuando no hay item equipado se usan los puños, luego hay "arma" ofensiva
  word uwMinDist = 1;
  const AreaDefs::EntHandle hItemEquip = GetEquipmentSlots()->GetItemEquipedAt(WeaponSlot);
  CItem* const pItem = hItemEquip ? pWorld->GetItem(hItemEquip) : NULL;
  if (pItem) {
	// ¿No tiene puntos de uso el item con el que golpear?
	if (0 == SYSEngine::GetGameDataBase()->GetRulesDataBase()->GetUseWeaponItemCost(pItem->GetType())) {
	  return RulesDefs::CAR_FAIL;
	}	

	// Se toma la minima distancia
    // Nota: Es seguro que la minima distancia valdra como minimo 1
    uwMinDist = SYSEngine::GetGameDataBase()->GetRulesDataBase()->GetMinDistanceFor(RulesDefs::MD_HITWITHWEAPON,
																					pItem->GetType(),
																					pItem->GetClass()); 	
	ASSERT(uwMinDist);
  }
      
  // ¿Se puede golpear (al estar a distancia minima)?
  if (uwMinDist >= pWorld->CalculeAbsoluteDistance(Inherited::GetTilePos(),
												   pEntity->GetTilePos())) {
	// En este punto, la criatura se hallara en una posicion correcta por
	// lo que la estara permitido el golpear
	// Se coloca a la criatura mirando a la entidad si procede
	const IsoDefs::eDirectionIndex Dir = pWorld->CalculeDirection(Inherited::GetTilePos(),
																  pEntity->GetTilePos());
	if (Dir != IsoDefs::NO_DIRECTION_INDEX) {
	  Inherited::GetAnimTemplate()->SetOrientation(Dir);   	  
	}

	// Operacion realizada, se realiza la notificacion
	// En los 16 ultimos bits la accion realizada y en los primeros el slot
	// de equipamiento donde se halla el arma usada	
	SYSEngine::GetVirtualMachine()->OnHitEntity(this,
												pEntity->GetScriptEvent(RulesDefs::SE_ONHIT),
												Inherited::GetHandle(),
												WeaponSlot,
												pEntity->GetHandle());

	// Accion en progreso
	m_ActOrder = CCriature::AO_HITENTITY;
	return RulesDefs::CAR_INPROGRESS;
  }

  // No se puede golpear, al no estar a distancia minima
  // Se crea el identificador del comando y se la ordena andar a esa posicion,
  // retornando el resultado de dicha orden de aproximacion
  const CommandDefs::IDCommand IDCommand = MakeIDComandForAction(CCriature::TO_HIT,
																 hEntity);  	
  if (Walk(pEntity->GetTilePos(), uwMinDist, this, IDCommand, WeaponSlot)) {	
	return RulesDefs::CAR_INPROGRESS;
  } else {
	return RulesDefs::CAR_FAIL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Equipa el item hItem en el slot Slot. 
// - Sera condicion necesaria que la criatura posea dicho item en el inventario
//   o bien que ya lo posea equipado, en cuyo caso lo desequipara y luego
//   equipara el nuevo.
// Parametros:
// - hItem. Item a equipar.
// - Slot. Slot de equipamiento.
// Devuelve:
// - El resultado de la accion, que podra ser no realizado, en progreso o
//   realizado.
// Notas:
///////////////////////////////////////////////////////////////////////////////
RulesDefs::eCriatureActionResult
CCriature::EquipItem(const AreaDefs::EntHandle& hItem,
					 const RulesDefs::EquipmentSlot& Slot)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(hItem);
  
  // ¿NO se puede acatar la orden?
  // ¿NO se puede intentar realizar la accion en combate?
  if (!CanExecuteOrder() ||
      !CanExecuteActionInCombat(RulesDefs::CA_EQUIP, hItem, Slot)) {
	return RulesDefs::CAR_FAIL;
  }
  
  // Se procede a localizar el item a equipar
  // ¿El item a equipar esta en el inventario?
  if (m_Inventory.IsItemIn(hItem)) {
	// Se extrae del inventario
	m_Inventory.Extract(hItem);	
  } else {
	// No esta en el inventario luego TIENE que estar equipado
	RulesDefs::EquipmentSlot PrevSlot;
	if (m_EquipSlots.WhereIsItemEquiped(hItem, PrevSlot)) {
	  // ¿El item NO esta en el slot donde se desea recolocar?
	  if (Slot != PrevSlot) {
		// Se desequipa
		m_EquipSlots.UnequipItem(PrevSlot);
	  } else {
		// Se retorna pues se quiere hacer operacion redundante				
		return RulesDefs::CAR_DONE;
	  }
	} else {
	  // Se intenta equipar un item que no se posee
	  return RulesDefs::CAR_FAIL;
	}
  }  

  // Se desequipa el slot destino y se equipa el item deseado en el
  UnequipItemAt(Slot);  
  m_EquipSlots.EquipItem(hItem, Slot);

  // Se notifica y retorna
  SYSEngine::GetVirtualMachine()->OnEquippedItem(this, 
												 Inherited::GetScriptEvent(RulesDefs::SE_ONINSERTINEQUIPMENTSLOT),
												 hItem,
												 Inherited::GetHandle(),
												 RulesDefs::eIDEquipmentSlot(Slot));
  m_ActOrder = CCriature::AO_EQUIPITEM;
  return RulesDefs::CAR_INPROGRESS;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desequipa un item en slot Slot y lo devuelve al inventario
// Parametros:
// - Slot desde donde se quiere desequipar.
// Devuelve:
// - El resultado de la accion, que podra ser no realizado, en progreso o
//   realizado.
// Notas:
// - Se considerara que aunque no haya item para desequipar, la accion
//   habra sido realizada.
///////////////////////////////////////////////////////////////////////////////
RulesDefs::eCriatureActionResult
CCriature::UnequipItemAt(const RulesDefs::EquipmentSlot& Slot)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿NO se puede acatar la orden?
  // ¿NO se puede intentar realizar la accion en combate?
  if (!CanExecuteOrder() ||
      !CanExecuteActionInCombat(RulesDefs::CA_UNEQUIP)) {
	return RulesDefs::CAR_FAIL;
  }

  // Se desequipa item en slot Slot y si de verdad hubiera item en dicho slot
  // se devolvera al inventario
  const AreaDefs::EntHandle hItem = m_EquipSlots.UnequipItem(Slot);
  if (hItem) {
	// Se inserta y se notifica	
    m_Inventory.Insert(hItem);
    SYSEngine::GetVirtualMachine()->OnRemoveEquippedItem(this, 
												         Inherited::GetScriptEvent(RulesDefs::SE_ONREMOVEFROMEQUIPMENTSLOT),
  												         hItem,
												         Inherited::GetHandle(),
												         RulesDefs::eIDEquipmentSlot(Slot));
	m_ActOrder = CCriature::AO_REMOVEEQUIPITEM;
	return RulesDefs::CAR_INPROGRESS;
  }

  // Accion no realizada
  return RulesDefs::CAR_FAIL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si la criatura puede realizar una accion en combate.
// - Para que una criatura pueda realizar una accion se debera de cumplir:
//   * Sera necesario que la criatura posea el turno. En caso de que lo posea 
//     podra ocurrir que para la accion en concreto la criatura no tenga suficientes 
//     puntos. En ese  caso no podra realizar la accion.
// - En el caso de que la accion no se pueda realizar por falta de puntos de
//   accion se realizara una notificacion a todos observadores.
// Parametros:
// - CombatAction. Accion a realizar.
// - hEntity. Entidad sobre la que reacae la accion. Por defecto valdra 0,
//   ya que solo sera necesaria para algunas acciones.
// - udParam. Parametro asociado. Por defecto valdra 0.
// Devuelve:
// - Si la accion se puede realizar true. En caso contrario false.
// Notas:
// - En el caso del jugador, algunas operaciones aqui definidas podran
//   ser redundantes pues de que se cumplan se habra encargado la interfaz
//   de interaccion con entidades, no mostrando la opcion.
///////////////////////////////////////////////////////////////////////////////
bool 
CCriature::CanExecuteActionInCombat(const RulesDefs::eCriatureAction& CombatAction,
									const AreaDefs::EntHandle& hEntity,
									const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // ¿Se esta en modo turno?
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  if (pWorld->GetMode() == WorldDefs::TURN_MODE) {
	// ¿La criatura NO posee el turno?
	iCCombatSystem* const pCombatSys = SYSEngine::GetCombatSystem();
	ASSERT(pCombatSys);
	if (pCombatSys->GetCriatureInTurn() != Inherited::GetHandle()) {
	  return false;
	}

	// Se obtienen los puntos de accion necesarios
	RulesDefs::CriatureActionPoints ActionPointsRequired = 0; 
	switch (CombatAction) {
	  case RulesDefs::CA_USEHABILITY: {
		// Uso de una habilidad
		// Se toma el coste
		iCGameDataBase* const pGDBase = SYSEngine::GetGameDataBase();
		ASSERT(pGDBase);
		const RulesDefs::eIDHability Hability = RulesDefs::eIDHability(udParam);
		ActionPointsRequired = pGDBase->GetRulesDataBase()->GetCombatActionCost(CombatAction) +
							   pGDBase->GetRulesDataBase()->GetUseHabilityCost(Hability);		
	  } break;
	  
	  case RulesDefs::CA_MANIPULATE: {
		// Solo se atendera si la entidad recibida es una pared o bien es
		// un objeto de escenario no contenedor
		CWorldEntity* const pEntity = pWorld->GetWorldEntity(hEntity);
		ASSERT(pEntity);
		if (pEntity->GetEntityType() == RulesDefs::WALL ||
			(pEntity->GetEntityType() == RulesDefs::SCENE_OBJ &&	
			 !pEntity->GetItemContainer())) {
		  // Es valido, se obtiene coste
		  ActionPointsRequired = SYSEngine::GetGameDataBase()->GetRulesDataBase()->GetCombatActionCost(CombatAction);
		} else {
		  // Entidad no valida
		  return false;
		}
	  } break;

	  case RulesDefs::CA_HIT: {
		// Accion de golpear a otra criatura			
		// Se obtiene el coste de uso
		iCGameDataBase* const pGDBase = SYSEngine::GetGameDataBase();
		ASSERT(pGDBase);
		ASSERT ((udParam == RulesDefs::EQUIPMENT_SLOT_0 ||
				udParam == RulesDefs::EQUIPMENT_SLOT_1) != 0);
		const AreaDefs::EntHandle hItemEquip = GetEquipmentSlots()->GetItemEquipedAt(udParam);
		CItem* const pItem = hItemEquip ? pWorld->GetItem(hItemEquip) : NULL;
		if (pItem) {
		  // Hay un item equipado se obtienen los puntos de uso
		  // Nota: Es seguro que en este punto el arma sea ofensiva
		  ActionPointsRequired = pGDBase->GetRulesDataBase()->GetUseWeaponItemCost(pItem->GetType());
		  ASSERT((ActionPointsRequired > 0) != 0);
		}
				
		// Se añade el coste base de golpear
		ActionPointsRequired += pGDBase->GetRulesDataBase()->GetCombatActionCost(CombatAction);
							   
	  } break;
	  	  
	  case RulesDefs::CA_WALK:
	  case RulesDefs::CA_UNEQUIP:	
	  case RulesDefs::CA_USEITEM:
	  case RulesDefs::CA_OBSERVE:
	  case RulesDefs::CA_GETITEM: {
		// Se obtiene el coste directamente
		ActionPointsRequired = SYSEngine::GetGameDataBase()->GetRulesDataBase()->GetCombatActionCost(CombatAction);
	  } break;

	  case RulesDefs::CA_EQUIP: {
		// En el caso de equipar, habra que tener en cuenta, ademas del costo
		// de la accion, el posible costo asociado de desequipar si es que
		// hay que realizar dicha accion
		// En udParam estara almacenado el slot donde equipar
		// Se toma coste base
		ActionPointsRequired = SYSEngine::GetGameDataBase()->GetRulesDataBase()->GetCombatActionCost(CombatAction);
		
		// ¿Hay item equipado en el slot a utilizar?
		if (m_EquipSlots.GetItemEquipedAt(udParam)) {
		  // Si, luego se añade el coste de desequipar dicho item de slot
		  ActionPointsRequired += SYSEngine::GetGameDataBase()->GetRulesDataBase()->GetCombatActionCost(RulesDefs::CA_UNEQUIP);
		}

		// ¿El item a equipar esta equipado en slot distinto al recibido?
		RulesDefs::EquipmentSlot EquipSlot;		
		if (m_EquipSlots.WhereIsItemEquiped(hEntity, EquipSlot)) {
		  if (EquipSlot != udParam) {
			// Si, luego se añade el coste de desequiparlo
			ActionPointsRequired += SYSEngine::GetGameDataBase()->GetRulesDataBase()->GetCombatActionCost(RulesDefs::CA_UNEQUIP);
		  }
		}		
	  } break;

	  default: {
		ASSERT_MSG(false, "Accion no permitida");
	  } break;
	}; // ~ switch

	if (ActionPointsRequired > pCombatSys->GetCriatureActionPoints(Inherited::GetHandle())) {
	  // Se notifica y se sale
	  ObserversNotify(CriatureObserverDefs::INSUFICIENT_ACTION_POINTS,
					  CombatAction);
	  return false;
	}
  }

  // Se puede realizar la accion
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notifica la finalizacion de un comando.
// - Se recibiran notificaciones siempre y cuando se realice un movimiento
//   para acercar el personaje y que este pueda realizar una determinada accion.
// Parametros:
// - IDCommand. Codigo del comando.
// - udInstant.	Instante en que se ha producido
// - udExtraParam. Parametro Extra.
// Devuelve:
// Notas:
// - El codigo IDCommand estara compuesto en el word alto por el bit que 
//   indica la operacion de acercamiento y en el word bajo el parametro 
//   asociado.
///////////////////////////////////////////////////////////////////////////////
void 
CCriature::EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
						const dword udInstant,
						const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿NO ha habido una parada FORZADA por ORDEN?
  if (!m_bStopWalkOrder) {
	// No, se toma el parametro y codigo indicativo de la operacion
	const word uwParam = IDCommand;
	const word uwActionCode = IDCommand >> 16;
  
	// Se comprueba la accion
	switch(uwActionCode) {
	  case CCriature::TO_GETITEM: {
		// La criatura termino de acercarse para recoger un item, se intenta recoger
		GetItem(uwParam);
	  } break;
  	  
	  case CCriature::TO_USEITEM: {
		// La criatura termino de acercarse para usar un item, se intenta usar
		// En el parametro extra ira el handle a la entidad sobre la que usar el item
		UseItem(uwParam, udExtraParam);
	  } break;
	  
  	  case CCriature::TO_MANIPULATE: {
		// La criatura termino de acercarse para manipular una entidad
		Manipulate(uwParam);
	  } break;

  	  case CCriature::TO_OBSERVE: {
		// La criatura termino de acercarse para observar a una entidad	  
		Observe(uwParam);
	  } break;

	  case CCriature::TO_USEHABILITY: {
		// Uso de una habilidad. 	  
		// En el parametro extra se incluye el idnetificador de habilidad
		UseHability(uwParam, RulesDefs::eIDHability(udExtraParam));
	  } break;

	  case CCriature::TO_HIT: {
		// Golpear a una entidad. 	  
		// En el parametro extra se incluye el slot de equipamiento a utilizar
		Hit(uwParam, RulesDefs::eIDEquipmentSlot(udExtraParam));
	  } break;
	};  
  }
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece / quita el sintoma Symptom.
// Parametros:
// - Symptom. Sintoma con el que trabajar.
// - bActive. Flag de activacion / quitar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCriature::SetSymptom(const RulesDefs::eIDSymptom& Symptom,
					  const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Se desea activar el sintoma?
  if (bActive) {
	// Si, se selecciona y notifica
	m_Attributes.Symptoms |= Symptom;
	SYSEngine::GetVirtualMachine()->OnActivatedSymptom(this,
													   Inherited::GetScriptEvent(RulesDefs::SE_ONACTIVATEDSYMPTOM),
													   Inherited::GetHandle(),
													   Symptom);
  } else {
	// No, se deselecciona y notifica
	m_Attributes.Symptoms &= !Symptom;
	SYSEngine::GetVirtualMachine()->OnDesactivatedSymptom(this,
													      Inherited::GetScriptEvent(RulesDefs::SE_ONDEACTIVATEDSYMPTOM),
													      Inherited::GetHandle(),
													      Symptom);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece un valor de salud. En este metodo se controlaran los eventos
//   de resurreccion y muerte. Se lanzara el script de resurreccion si la
//   criatura pasa de un estado de salud temporal inferior o igual a 0 a uno
//   superior. Y se lanzara un evento de muerte cuando pase a un estado
//   de salud inferior a 0.
// - En caso de quitar salud y muerte, existiran animaciones asociadas que
//   deberan de ejecutarse antes de que se realice ningun tipo de notificacion.
// Parametros:
// - Type. Tipo de valor sobre el que actuar.
// - swValue. Valor a establecer.
// Devuelve:
// - En caso de haber existido un cambio de animacion true, en caso contrario
//   false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCriature::SetHealth(const RulesDefs::ePairValueType& Type,
					 const sword& swValue)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Establece segun sea el tipo
  if (RulesDefs::BASE_VALUE == Type) {
	// Valor base, ha de ser superior a 0
	if (swValue > 0) {
	  m_Attributes.Health.swBase = swValue;	
	}

	// Se notifica
	ObserversNotify(CriatureObserverDefs::HEALTH_MODIFY);
  } else {
	// Valor temporal, desde aqui se controlaran los diversos eventos		
	// Se actualizan valores
	const sword swPrevTempValue = m_Attributes.Health.swTemp;
	m_Attributes.Health.swTemp = swValue;

	// ¿Se ha decrementado el valor?
	if (swValue < swPrevTempValue) {
	  // Si, ¿la criatura ha muerto?
	  if (swValue <= 0) {
		// Si, ¿NO esta la animacion de muerte en progreso?
		if (!IsAnimOfActionInProgress(CCriature::sAnimActionActive::DEATH)) {
		  // No esta, luego se establece animacion
		  SetAnimOfAction(sAnimActionActive::DEATH, 0);
		}		
	  } else {
		// No, ¿NO esta la animacion de golpeado en progreso?
		if (!IsAnimOfActionInProgress(CCriature::sAnimActionActive::DEC_HEALTH)) {
		  // No esta, luego se establece animacion
		  SetAnimOfAction(sAnimActionActive::DEC_HEALTH, 0);
		}
	  }

	  // Retorna indicando cambio de animacion
	  return true;
	} else if (swPrevTempValue <= 0 && swValue > 0) {
	  // No, pero si ha resucitado, se lanza el evento y establece anim de muerte
	  Inherited::GetAnimTemplate()->SetState(RulesDefs::AS_STOP);
	  SYSEngine::GetVirtualMachine()->OnResurrect(this, 
												  Inherited::GetScriptEvent(RulesDefs::SE_ONRESURRECT),
												  Inherited::GetHandle());	  
	}

	// Se notifica
	ObserversNotify(CriatureObserverDefs::HEALTH_MODIFY);
  }

  // No hubo cambio de animacion
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece un nuevo valor para un atributo extendido.
// Parametros:
// - Type. Tipo de valor sobre el que actuar.
// - swValue. Valor a establecer.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCriature::SetExAttribute(const RulesDefs::eIDExAttrib& ExAttrib, 
						  const RulesDefs::ePairValueType& Type,
						  const sword& swValue)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Establece segun sea el tipo
  if (CriatureDefs::BASE_VALUE == Type) {
	m_Attributes.ExAttribs[ExAttrib].swBase = swValue;
  } else {
  	m_Attributes.ExAttribs[ExAttrib].swTemp = swValue;
  }

  // Notifica
  ObserversNotify(CriatureObserverDefs::EXTENDED_ATTRIBUTE_MODIFY, ExAttrib);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia un observer a la lista de observadores.
// Parametros:
// - pObserver. Direccion al interfaz del observer a asociar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCriature::AddObserver(iCCriatureObserver* const pObserver)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(pObserver);
  ASSERT((pObserver != this) != 0);

  // ¿El observer se halla instalado?
  if (std::find(m_ObserversInfo.Observers.begin(), 
				m_ObserversInfo.Observers.end(), 
				pObserver) != m_ObserversInfo.Observers.end()) {
	// Si, se cancela accion
	return;
  }
  
  // ¿Se esta notificando?
  if (m_ObserversInfo.bNotifyActive) {
	// Si, se inserta en lista pendiente
	m_ObserversInfo.ObPendingToAdd.push_back(pObserver);
  } else {
	// Inserta el observer al final
	m_ObserversInfo.Observers.push_back(pObserver);
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Localiza el observer pObserver y lo elimina de la lista de observadores.
// Parametros:
// - pObserver. Observer a localizar y eliminar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCriature::RemoveObserver(iCCriatureObserver* const pObserver)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(pObserver);

  // ¿Se esta notificando?
  if (m_ObserversInfo.bNotifyActive) {
	// Se inserta en lista de peticiones
	m_ObserversInfo.ObPendingToRemove.push_back(pObserver);
  } else {
	// Localiza el iterador al observer
	const CriatureObserverListIt It(std::find(m_ObserversInfo.Observers.begin(), 
											  m_ObserversInfo.Observers.end(), 
											  pObserver));
	if (It != m_ObserversInfo.Observers.end()) {
	  // Libera el nodo al observer
	  m_ObserversInfo.Observers.erase(It);
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre la lista de observers notificando a cada uno el evento 
//   NotifyType con parametro adjunto udParams.
// Parametros:
// - NotifyType. Tipo de notificacion (evento)
// - udParams. Parametros asociados.
// Devuelve:
// Notas:
// - A la hora de recorrer para notificar, no se atendera a aquellos clientes
//   que hayan solicitado eliminarse como observadores en el transcurso de la
//   llamada a este metodo.
///////////////////////////////////////////////////////////////////////////////
void 
CCriature::ObserversNotify(const CriatureObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParams)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Recorre notificando
  m_ObserversInfo.bNotifyActive = true;
  CriatureObserverListIt It(m_ObserversInfo.Observers.begin());
  for (; It != m_ObserversInfo.Observers.end(); ++It) {
	// ¿El cliente ha deseado eliminarse como observador?
	const CriatureObserverListIt RemoveIt(std::find(m_ObserversInfo.ObPendingToRemove.begin(),
												    m_ObserversInfo.ObPendingToRemove.end(),
													*It));
	if (RemoveIt == m_ObserversInfo.ObPendingToRemove.end()) {
	  // No, luego se puede notificar
	  (*It)->CriatureObserverNotify(Inherited::GetHandle(), NotifyType, udParams);
	}
  }
  m_ObserversInfo.bNotifyActive = false;

  // Recorre la lista de observers pendientes de borrado y borra
  It = m_ObserversInfo.ObPendingToRemove.begin();
  while (It != m_ObserversInfo.ObPendingToRemove.end()) {
	RemoveObserver(*It);
	It = m_ObserversInfo.ObPendingToRemove.erase(It);
  }

  // Recorre la lista de observers pendientes de ser añadidos y añade
  It = m_ObserversInfo.ObPendingToAdd.begin();
  while (It != m_ObserversInfo.ObPendingToAdd.end()) {
	AddObserver(*It);
	It = m_ObserversInfo.ObPendingToAdd.erase(It);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece la animacion previa a la accion Action.
// - Si hubiera una animacion ya activada, esta ejecutara su logica simulando 
//   que ha finalizado.
// Parametros:
// - Action. Accion sobre la que se va a establecer la accion previa.
// - udParam. Parametros asociados
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCriature::SetAnimOfAction(const sAnimActionActive::eAction& Action,
						   const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // ¿Hay una anim. previa?
  AnimActionActiveMapIt It(m_AnimActionActiveMap.find(Inherited::GetHandle()));
  if (It != m_AnimActionActiveMap.end()) {
	// ¿La accion a establecer es la misma que hay?
	if (Action == It->second->Action) {
	  // Si, luego se abandona
	  return;
	} else {
	  // No, luego se simula la accion previa y luego se inserta la nueva
	  // Nota: El parametro a pasar se guardara en otra vble pues durante
	  // la llamada a lo que apunta el iterador sera destruido.
	  const dword udParam = It->second->udParam;
	  OnAnimOfActionInProgressEnd(udParam);
	}
  }

  // Se crea instancia y se registra
  sAnimActionActive* pAnimActionActive = new sAnimActionActive(Action, udParam);
  ASSERT(pAnimActionActive);
  m_AnimActionActiveMap.insert(AnimActionActiveMapValType(Inherited::GetHandle(), pAnimActionActive));

  // Por ultimo, se cambia el estado de animacion
  switch(Action) {
	case sAnimActionActive::GET_ITEM: {
	  Inherited::GetAnimTemplate()->SetState(RulesDefs::AS_GETITEM);
	} break;

	case sAnimActionActive::MANIPULATE:
	case sAnimActionActive::REGISTER: 
	case sAnimActionActive::USE_ITEM: {
	  Inherited::GetAnimTemplate()->SetState(RulesDefs::AS_INTERACT);
	} break;
	
	case sAnimActionActive::DEC_HEALTH: {
	  Inherited::GetAnimTemplate()->SetState(RulesDefs::AS_DECHEALTH);
	} break;

	case sAnimActionActive::DEATH: {
	  Inherited::GetAnimTemplate()->SetState(RulesDefs::AS_DEATH);
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion asociada al trabajo con la plantilla de animacion. Sera
//   interesante añadir logica para:
//    * Siempre que se mande realizar una accion que tenga asociada un
//      proceso grafico de animacion (Manipular, recoger, etc) antes de
//      ejectuar la logica, se ordenara que se realice la animacion. En el
//      intante que la animacion termine, se ejecutara la logica. Gracias a
//      este metodo se podra averiguar cuando eso ha ocurrido.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCriature::AnimTemplateObserverNotify(const AnimTemplateObserverDefs::eObserverNotifyType& NotifyType,
									  const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Comprueba el tipo de notificacion
  switch(NotifyType) {
	//case AnimTemplateObserverDefs::CHANGETO_LINKSTATE:
	case AnimTemplateObserverDefs::STATE_CYCLE_COMPLETE: {
	  // Cambio a estado de enlace
	  // El valor udParam hara referencia al estado previo antes del cambio
	  OnAnimOfActionInProgressEnd(udParam);
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Logica asociada a la finalizacion de una animacion en progreso que estaba
//   asociada a una accion. El metodo describira que tipo de accion era esta
//   para volverla a ejecutar teniendo la seguridad de que en esta ocasion,
//   al haberse realizado la animacion, se ejecutara.
// Parametros:
// - udParam. Parametros asociados.
// Devuelve
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCriature::OnAnimOfActionInProgressEnd(const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se comprueba si hay alguna accion asociada
  const AnimActionActiveMapIt It(m_AnimActionActiveMap.find(Inherited::GetHandle()));
  if (It != m_AnimActionActiveMap.end()) {
	// Si, Se llamara al metodo apropiado
    switch (It->second->Action) {
  	  case sAnimActionActive::GET_ITEM: {
	    // Se toma el item
		m_ActOrder = CCriature::AO_NONE;
		GetItem(It->second->udParam);
	  } break;
			
	  case sAnimActionActive::MANIPULATE: {
	    // Manipular
		m_ActOrder = CCriature::AO_NONE;
		Manipulate(It->second->udParam);
	  } break;

	  case sAnimActionActive::REGISTER: {
	    // Registro a una criatura
		m_ActOrder = CCriature::AO_NONE;
		Manipulate(It->second->udParam);
	  } break;

	  case sAnimActionActive::USE_ITEM: {
	    // Uso de un item sobre otra entidad
	    // En parte superior el hItem y en la inf. la entidad
		m_ActOrder = CCriature::AO_NONE;
	    UseItem(It->second->udParam >> 16, It->second->udParam);
	  } break;

	  case sAnimActionActive::DEC_HEALTH: {
		// Fue ejecutada la animacion de decremento de salud, ahora
		// se notificara tal hecho
		ObserversNotify(CriatureObserverDefs::HEALTH_MODIFY);
	  } break;

	  case sAnimActionActive::DEATH: {
		// Se ha ejecutado la animacion de muerte, ahora se lanzara
		// el evento asociado. 
		// Nota: Cuando se produce una muerte NO se notificara el decremento
		// de salud, sino el de la muerte una vez sea ejecutado el script
		SYSEngine::GetVirtualMachine()->OnDeath(this, 
												Inherited::GetScriptEvent(RulesDefs::SE_ONDEATH),
												Inherited::GetHandle());
	  } break;

	}; // ~ switch
  
	// Eliminara instancia de informacion del map de instancias
	delete It->second;
	m_AnimActionActiveMap.erase(It);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece o quita el modo transparente en una criatura. 
// Parametros:
// - bSet. Flag para poner o quitar el modo Fade.
// Devuelve
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCriature::SetTransparentMode(const bool bSet)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Es seguro que no esta activo
  // Se asocia la direccion del alpha con el que trabajar
  GraphDefs::sAlphaValue* const pAlpha = bSet ? SYSEngine::GetGFXManager()->GetAlphaCriatures() : NULL;

  // Se asocia la estrucura alpha a los vertices
  Inherited::SetAlphaValue(GraphDefs::VERTEX_0, pAlpha);
  Inherited::SetAlphaValue(GraphDefs::VERTEX_1, pAlpha);
  Inherited::SetAlphaValue(GraphDefs::VERTEX_2, pAlpha);
  Inherited::SetAlphaValue(GraphDefs::VERTEX_3, pAlpha);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el modo transparencia esta activo.
// Parametros:
// Devuelve
// - Si el modo transparencia esta activo true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCriature::IsTransparentMode(void) 
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Comprueba si esta activo el modo transparente, lo estara siempre
  // que la estructura alpha asociada a los vertices sea igual a la
  // estructura alpha de transparencia
  // Nota: la comprobacion se podra limitar a un solo vertice
  return (SYSEngine::GetGFXManager()->GetAlphaCriatures() == Inherited::GetAlphaValue(GraphDefs::VERTEX_0));
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Pausa o quita la pausa de la criatura.
//   entonces 
// Parametros:
// - bPause. Flag de pausa
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCriature::SetPause(const bool bPause)
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

    // Actua sobre el movimiento si procede
	if (m_WalkInfo.MoveCmd.IsActive()) {
	  m_WalkInfo.MoveCmd.SetPause(bPause);
	}
	
	// Propaga
	Inherited::SetPause(bPause);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece / quita una habilidad en la criatura.
// Parametros:
// - Hability. Habilidad sobre la que actuar.
// - bSet. Flag de quitar o poner.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCriature::SetHability(const RulesDefs::eIDHability& Hability,
					   const bool bSet)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Se desea poner la habilidad?
  if (bSet) {
	// Si, se pone y notifica a los observers
	m_Attributes.Habilities |= Hability;
	ObserversNotify(CriatureObserverDefs::HABILITY_SET, Hability);
  } else {
	// No, se quita
	m_Attributes.Habilities &= !Hability;
	ObserversNotify(CriatureObserverDefs::HABILITY_UNSET, Hability);
  } 
}
  
//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion del evento de inicio del turno de combate para lanzar el 
//   posible script asociado al evento
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CCriature::OnStartCombatTurn(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Ejecuta el script asociado al evento
  SYSEngine::GetVirtualMachine()->OnStartCombatTurn(this,
													Inherited::GetScriptEvent(RulesDefs::SE_ONSTARTCOMBATTURN),
													Inherited::GetHandle());
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion que SEGURO pertenece a una criatura que se halla en el 
//   rango de la propia
// Parametros:
// - hCriature. Handle a la criatura.
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCriature::CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
							      const CriatureObserverDefs::eObserverNotifyType& NotifyType,
								  const dword udParam)
{
  // SOLO si instancia inicializada  
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hCriature);

  // Se comprueba el tipo de notificacion recibida
  switch (NotifyType) {
	case CriatureObserverDefs::DESTROY: {
	  // Es seguro que se va a destruir una criatura dentro del rango
	  // por lo que se extrera, eliminadose esta como observadora
	  ASSERT((m_CriaturesInRange.find(hCriature) != m_CriaturesInRange.end()) != 0);
	  m_CriaturesInRange.erase(hCriature);
	  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(hCriature);
	  ASSERT(pCriature);
	  pCriature->RemoveObserver(this);

	  // El siguiente paso sera eliminar a hCriature si es que esta registrado
	  // en esta instancia en concreto
	  RemoveObserver(pCriature);
	} break;

	case CriatureObserverDefs::ACTION_REALICE: {
	  // Se comprueba la accion realizada
	  const RulesDefs::eCriatureAction Action = RulesDefs::eCriatureAction(udParam & 0x0000FFFF);
	  switch(Action) {
	    case RulesDefs::CA_WALK: {	
		  // La criatura ha realizado un cambio de posicion
		  // ¿Ya NO ES visible?
		  iCWorld* const pWorld = SYSEngine::GetWorld();
		  ASSERT(pWorld);
		  if (!pWorld->IsCriatureInRangeAt(hCriature, GetTilePos(), m_Attributes.Range)) {
			// No lo es, se extrarera del conjunto
			m_CriaturesInRange.erase(hCriature);

			// Si es posible se eliminara como observer y se lanzara evento
			CCriature* const pCriature = pWorld->GetCriature(hCriature);
			ASSERT(pCriature);
			pCriature->RemoveObserver(this);
			SYSEngine::GetVirtualMachine()->OnCriatureOutOfRange(this,
																 Inherited::GetScriptEvent(RulesDefs::SE_ONCRIATUREOUTOFRANGE),
																 Inherited::GetHandle(),
																 hCriature);						
		  }
		} break;
	  }; // ~ switch
	} break;
  }; // ~ switch
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establecimiento de una nueva posicion. En el caso de las criaturas se
//   debera de aprovechar para actualizar el conjunto de entidades en 
//   rango de vision.
// Parametros:
// - TilePos. La nueva posicion.
// Devuelve:
// Notas:
// - Durante el cambio de posicion, la unica comprobacion que no podra 
//   realizarse usando la funcion para la localizacion de criaturas dentro
//   del nuevo rango ni la notificacion a las que eran observadoras, sera
//   el relativo a conocer a otras criaturas para las cuales esta se 
//   halla ahora en su rango, de ahi que sea necesario hallarlo al final
//   y realizar un recorrido extra por las mismas.
//////////////////////////////////////////////////////////////////////////////
void 
CCriature::SetTilePos(const AreaDefs::sTilePos& TilePos)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Propaga para establecer el valor de posicion
  Inherited::SetTilePos(TilePos);

  // Obtiene el conjunto de criaturas en el rango
  // Nota: se quita el handle de la criatura propia del conjunto
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CriaturesInRangeSet NewRangeSet;
  pWorld->FindCriaturesInRangeAt(TilePos, m_Attributes.Range, NewRangeSet);  
  NewRangeSet.erase(Inherited::GetHandle());

  // Se recorre el nuevo conjunto obtenido y para todas aquellas criaturas que
  // no esten en el antiguo, instalaremos esta como observer y notificaremos
  // su visibilidad.
  iCVirtualMachine* const pVMachine = SYSEngine::GetVirtualMachine();
  ASSERT(pVMachine);
  CriaturesInRangeSetIt It(NewRangeSet.begin());
  for (; It != NewRangeSet.end(); ++It) {
	// ¿NO esta en el antiguo?
	if (m_CriaturesInRange.find(*It) == m_CriaturesInRange.end()) {
	  // Instala como observer, notifica y continua
	  // Nota: Solo se notificara si NO SE ESTA cargando una partida guardada	
	  CCriature* const pCriature = pWorld->GetCriature(*It);
	  ASSERT(pCriature);
	  ASSERT((*It != Inherited::GetHandle()) != 0);
	  pCriature->AddObserver(this);		
	  if (!pWorld->IsLoadingASavedGame()) {
	    pVMachine->OnCriatureInRange(this, 
									 Inherited::GetScriptEvent(RulesDefs::SE_ONCRIATUREINRANGE),
									 Inherited::GetHandle(),
									 *It);
	  }
	} else {
	  // En caso de estar en el antiguo, se extrera de este para que al
	  // final solo queden aquellas criaturas que ya no estan en el rango
	  m_CriaturesInRange.erase(*It);
	}
  }
  
  // Ahora se recorrera el conjunto antiguo, que estara lleno de criaturas
  // que dejaron de estar en el rango al moverse esta criatura. 
  It = m_CriaturesInRange.begin();
  for (; It != m_CriaturesInRange.end(); It = m_CriaturesInRange.erase(It)) {
	// ¿Criatura valida?
	CCriature* const pCriature = pWorld->GetCriature(*It);	
	if (pCriature) {
	  // Si, se desinstala esta como observer y se notifica
	  // Nota: es seguro que durante la carga de una partida guardada
	  // NO existan handles en el conjunto (de ahi que no se haga comprobacion
	  // acerca de si se esta, o no, cargando un area)
	  ASSERT((*It != Inherited::GetHandle()) != 0);
	  pCriature->RemoveObserver(this);
	  pVMachine->OnCriatureOutOfRange(this, 
									  Inherited::GetScriptEvent(RulesDefs::SE_ONCRIATUREOUTOFRANGE),
									  Inherited::GetHandle(),
									  *It);	  
	}
  }
  
  // Se establece el nuevo conjunto como el oficial
  m_CriaturesInRange = NewRangeSet;

  // Ahora se hallaran criaturas que contengan en su rango a la actual
  // Nota: El conjunto devuelto contendra SOLO a aquellas criaturas que 
  // no contuvieran anteriormente a esta criatura. Tampoco contendra
  // el handle de esta misma criatura.
  NewRangeSet.clear();
  pWorld->FindCriaturesContainingCriatureInRange(Inherited::GetHandle(), 
												 NewRangeSet);  

  // Ahora se recorrera el conjunto y para cada criatura recibida, se
  // asociara esta en su conjunto ademas de instalarse como observador de la misma
  It = NewRangeSet.begin();
  for (; It != NewRangeSet.end(); ++It) {
	// Se toma criatura
	CCriature* const pCriature = pWorld->GetCriature(*It);	
	ASSERT(pCriature);
	
	// Se añade ESTA al conjunto de criaturas en el rango de la criatura
	ASSERT((pCriature->m_CriaturesInRange.find(Inherited::GetHandle()) == pCriature->m_CriaturesInRange.end()) != 0);
	pCriature->m_CriaturesInRange.insert(Inherited::GetHandle());

	// Se instala como observador de esta
	ASSERT((*It != Inherited::GetHandle()) != 0);
	AddObserver(pCriature);

	// Finalmente, se notificara si procede
	if (!pWorld->IsLoadingASavedGame()) {
	  pVMachine->OnCriatureInRange(pCriature, 
								   pCriature->GetScriptEvent(RulesDefs::SE_ONCRIATUREINRANGE),
								   pCriature->GetHandle(),
								   Inherited::GetHandle());
	}
  }
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion por parte de los scripts lanzados desde esta clase.
// Parametros:
// - Notify. Tipo de notificacion.
// - ScriptEvent. Evento que lo lanza.
// - udParams. Parametros asociados.
// Devuelve:
// Notas:
// - Pese a que el script haya tenido errores, se debera de notificar a los
//   observadores sobre la realizacion de la accion.
//////////////////////////////////////////////////////////////////////////////
void 
CCriature::ScriptNotify(const RulesDefs::eScriptEvents& ScriptEvent,
						const ScriptClientDefs::eScriptNotify& Notify,
						const dword udParams)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Filtrado de evento que manda notificacion
  switch(ScriptEvent) {
	case RulesDefs::SE_ONINSERTINEQUIPMENTSLOT: {
	  // Sin comportamiento por defecto
	  // Notifica accion
	  m_ActOrder = CCriature::AO_NONE;
	  ObserversNotify(CriatureObserverDefs::ACTION_REALICE, 
					  (udParams & 0xFFFF0000) + RulesDefs::CA_EQUIP);  
	} break;

	case RulesDefs::SE_ONREMOVEFROMEQUIPMENTSLOT: {
	  // Sin comportamiento por defecto
	  // Notifica accion
	  m_ActOrder = CCriature::AO_NONE;
	  ObserversNotify(CriatureObserverDefs::ACTION_REALICE, 
					  (udParams & 0xFFFF0000) + RulesDefs::CA_UNEQUIP);	
	} break;

	case RulesDefs::SE_ONUSEHABILITY: {
	  // Sin comportamiento por defecto
	  // Notifica a clientes, quitando antes el valor de comportamiento por 
	  // defecto, que ocupara los 8 ultimos bits e insertando el tipo de accion
	  m_ActOrder = CCriature::AO_NONE;
	  ObserversNotify(CriatureObserverDefs::ACTION_REALICE, 
					  (udParams & 0xFFFF0000) + RulesDefs::CA_USEHABILITY);	
	} break;

	case RulesDefs::SE_ONACTIVATEDSYMPTOM: {
	  // Sin comportamiento por defecto
	  // Notifica, los clientes esperaran recibir el sintoma en la parte
	  // baja mientras que aqui se pasa en la parte alta
	  ObserversNotify(CriatureObserverDefs::SYMPTOM_ACTIVE, 
					  (udParams & 0xFFFF0000) >> 16);
	} break;

	case RulesDefs::SE_ONDEACTIVATEDSYMPTOM: {
	  // Sin comportamiento por defecto
	  // Notifica, los clientes esperaran recibir el sintoma en la parte
	  // baja mientras que aqui se pasa en la parte alta	
	  ObserversNotify(CriatureObserverDefs::SYMPTOM_DEACTIVE, 
					  (udParams & 0xFFFF0000) >> 16);	
	} break;	

	case RulesDefs::SE_ONHIT: {
	  // Dependiendo de si se dio, o no , la accion de golpear como valida
	  // se notificara. El subsistema de combate no interceptara la notificacion
	  // del valor INTERRUPT_HIT y no producira decremento en los puntos de 
	  // combate.
	  m_ActOrder = CCriature::AO_NONE;
	  if ((udParams & 0x0000FFFF) > 0) {		  
		ObserversNotify(CriatureObserverDefs::ACTION_REALICE, 
					    (udParams & 0xFFFF0000) + RulesDefs::CA_HIT);
	  } else {
		ObserversNotify(CriatureObserverDefs::INTERRUPT_HIT);		
	  }
	} break;	

    case RulesDefs::SE_ONSTARTCOMBATTURN: {
	  // Si la criatura NO ES el jugador, se pasara de turno si procede	  
	  // Antes se anulara cualquier orden que hubiera quedado pendiente
	  m_ActOrder = CCriature::AO_NONE;  
	  if (RulesDefs::CRIATURE == GetEntityType()) {		
		SYSEngine::GetCombatSystem()->NextTurn(Inherited::GetHandle());
	  }	
	} break;	

	case RulesDefs::SE_ONDEATH: {	  
	  // Se notificara a los observadores la posible muerte
	  m_ActOrder = CCriature::AO_NONE;
	  if (GetHealth(RulesDefs::TEMP_VALUE) < 1) {
		ObserversNotify(CriatureObserverDefs::IS_DEATH);
	  }
	} break;

	case RulesDefs::SE_ONRESURRECT: {
	  // Sin comportamiento por defecto
	  // Se notifica a los observadores en el caso de que este viva	  
	  m_ActOrder = CCriature::AO_NONE;
	  if (GetHealth(RulesDefs::TEMP_VALUE) > 0) {
		ObserversNotify(CriatureObserverDefs::IS_RESURRECT);
	  }	  
	} break;

	case RulesDefs::SE_ONUSEITEM: {
	  // Sin comportamiento por defecto
	  // Notifica accion
	  m_ActOrder = CCriature::AO_NONE;
	  ObserversNotify(CriatureObserverDefs::ACTION_REALICE, 
					 (udParams & 0xFFFF0000) + RulesDefs::CA_USEITEM);
	} break;

	case RulesDefs::SE_ONMANIPULATE: {
	  // ¿NO hubo ERRORES ejecutando el script?
	  if (ScriptClientDefs::SN_SCRIPT_ERROR != Notify) {
		// Se toma el handle a la entidad y se comprueba si es contenedora
		CWorldEntity* const pEntity = SYSEngine::GetWorld()->GetWorldEntity((udParams & 0xFFFF0000) >> 16); 
		ASSERT(pEntity);
		if (pEntity->GetItemContainer()) {
		  // Si, ¿Se puede realizar la accion por defecto?
		  if ((udParams & 0x0000FFFF) > 0) {
			// Se abre la ventana de intercambio
			SYSEngine::GetGUIManager()->SetTradeWindow(this, pEntity, NULL);
		  }
		}		
	  }

	  // Operacion realizada, se realiza la notificacion
	  m_ActOrder = CCriature::AO_NONE;
  	  ObserversNotify(CriatureObserverDefs::ACTION_REALICE, 
					  (udParams & 0xFFFF0000) + RulesDefs::CA_MANIPULATE);	
	} break;

	case RulesDefs::SE_ONGET: { 
	  // Se toman parametros
	  const AreaDefs::EntHandle hItem = ((udParams & 0xFFFF0000) >> 16);

	  // ¿NO hubo ERRORES ejecutando el script?
	  if (ScriptClientDefs::SN_SCRIPT_ERROR != Notify) {
		// ¿Se puede realizar la accion por defecto?
		if ((udParams & 0x0000FFFF) > 0) {		  
		  // Se coge (cambia localidad del escenario al item)		  
		  SYSEngine::GetWorld()->ChangeLocation(hItem, Inherited::GetHandle());			  
		}
	  }

	  // Operacion realizada, se notifica
	  m_ActOrder = CCriature::AO_NONE;
	  ObserversNotify(CriatureObserverDefs::ACTION_REALICE, 
					  (hItem << 16) | RulesDefs::CA_GETITEM);	  
	} break;

	case RulesDefs::SE_ONDROP: {	  	  
	  // Se toman parametros
	  const AreaDefs::EntHandle hItem = ((udParams & 0xFFFF0000) >> 16);
		  
	  // ¿NO hubo ERRORES ejecutando el script?
	  if (ScriptClientDefs::SN_SCRIPT_ERROR != Notify) {
		// ¿Hay que hacer accion por defecto?
		if ((udParams & 0x0000FFFF) > 0) {
		  // ¿Se posee el item en inventario?
		  if (m_Inventory.IsItemIn(hItem)) {
			// Se tira
			SYSEngine::GetWorld()->ChangeLocation(hItem, Inherited::GetTilePos());
			// Al pasarse al suelo, se debera de comprobar si la criatura esta
			// en modo pausa, en ese caso se dejara el item tambien pausado
			CItem* const pItem = SYSEngine::GetWorld()->GetItem(hItem);
			ASSERT(pItem);
			pItem->SetPause(Inherited::IsInPause());	
		  } else if (m_EquipSlots.IsItemEquiped(hItem)) {
			// No esta en inventario pero si en los slots de equipamiento
			// Se desequipa
			RulesDefs::EquipmentSlot Slot;
			m_EquipSlots.WhereIsItemEquiped(hItem, Slot);
			UnequipItemAt(Slot);
			// Se tira
			SYSEngine::GetWorld()->ChangeLocation(hItem, Inherited::GetTilePos());	
		  }
		}	    
	  }	 
	  
	  // Operacion realizada, se notifica
	  m_ActOrder = CCriature::AO_NONE;
	  ObserversNotify(CriatureObserverDefs::ACTION_REALICE, 
					  (hItem << 16) | RulesDefs::CA_DROPITEM);	  	
	} break;

	case RulesDefs::SE_ONCRIATUREINRANGE: {
	  // Sin comportamiento por defecto
	} break;

	case RulesDefs::SE_ONCRIATUREOUTOFRANGE: {
	  // Sin comportamiento por defecto
	} break;
	
	default: {
	  // Se propaga
	  Inherited::ScriptNotify(ScriptEvent, Notify, udParams);
	} break;
  }; // ~ switch
}
