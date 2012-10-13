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
// CMoveCmd.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CMoveCmd.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CMoveCmd.h"

#include "iCWorld.h"
#include "iCGUIManager.h"
#include "iCCombatSystem.h"
#include "iCGameDataBase.h"
#include "iCAnimTemplate.h"
#include "iCResourceManager.h"
#include "CPath.h"
#include "CFloor.h"
#include "CCriature.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el comando. Tomara la criatura a mover asi como el tile X e Y 
//   de destino. Si el comando ya estuviera ejecutandose, se reinicializara.
// - Para realizar el movimiento se pedira un estado de animacion. Cuando el
//   movimiento se termine, el estado de animacion original (el que tenia la
//   criatura antes de moverse) sera depuesto.
// - El comando permitira guardar peticiones si la actual no ha terminado, solo
//   en caso de que se hagan sobre la misma criatura.
// Parametros:
// - pCriature. Instancia de tipo criatura a mover
// - TileDest. Tile destino
// - AnimState. Estado de animacion a utilizar para realizar el movimiento.
// - uwDistanceMin. En el caso de que se quiera construir un camino de minima
//   distancia (consultar CIsoEngine::CPahtFinder) este valor indicara la 
//   minima distancia. Por defecto valdra 0, indicativo de que no sera tenido en
//   cuenta.
// - bGhostMode. Flag para saber si el camino se hallara en modo fantasma y, en
//   consecuencia, sin tener en cuenta obstaculos.
// Devuelve:
// - Si el movimiento es posible true. En caso contrario false.
// Notas:
// - Cuando se intente reicinializar, si hay algun problema, el estado anterior
//   del comando se perdera.
///////////////////////////////////////////////////////////////////////////////
bool 
CMoveCmd::Init(CCriature* const pCriature,
			   const AreaDefs::sTilePos& TileDest,
			   const byte AnimState,
			   const word uwDistanceMin,
			   const bool bGhostMode)
{
  // SOLO si parametros validos
  ASSERT(pCriature);  

  // Si esta activo el modo combate y la criatura no posee turno, finaliza
  iCCombatSystem* const pCombatSys = SYSEngine::GetCombatSystem();
  ASSERT(pCombatSys);
  if (pCombatSys->IsCombatActive() &&
	  pCombatSys->GetCriatureInTurn() != pCriature->GetHandle()) {
	return false;
  }

  // ¿Destino distinto que la posicion actual?
  if (pCriature->GetTilePos() != TileDest) {	
	// ¿Hay un movimiento ejecutandose?
	if (Inherited::IsActive()) { 
	  // Se almacenan los datos del movimiento pendiente	
	  m_PendingMove.TileDest = TileDest;
	  m_PendingMove.AnimState = AnimState;
	  m_PendingMove.uwDistanceMin = uwDistanceMin;
	  m_PendingMove.bGhostMode = bGhostMode;
	  m_bIsPending = true;

	  // Se cancela posible parada y retorna
	  m_bStopWalk = false;
	  return true;	
	} else {
	  // No, luego se inicializa la clase base
	  if (!Inherited::Init()) { 
		return false; 
	  }  
	}
  
	// ¿La entidad NO tiene suficiente velocidad de movimiento?
	if (!pCriature->GetXSpeed() && !pCriature->GetYSpeed()) { 
	  End();
	  return false; 
	}

	// Se establecen vbles de miembro
	m_CmdInfo.pCriature = pCriature;  
	m_CmdInfo.AnimState = pCriature->GetAnimTemplate()->GetAnimState();
	m_CmdInfo.pPath = NULL;
	m_ExInfo.hWAVWalkSound = 0;
	m_bIsPending = false;	
	m_bStopWalk = false;

	// Se procede a inicializar el movimiento
	if (!StartCmd(TileDest, AnimState, uwDistanceMin, bGhostMode)) { 
	  End();
	  return false;
	}

	// Todo correcto
	return true;
  }

  // No se realizo
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Pone en estado de no activa la instancia actual del comando. Tambien
//   libera posibles recursos.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CMoveCmd::End(void)
{
  if (Inherited::IsActive()) {
	// Regresa al estado de animacion original
	m_CmdInfo.pCriature->GetAnimTemplate()->SetState(m_CmdInfo.AnimState);	
	m_CmdInfo.pCriature->SetXPos(0.0f);
	m_CmdInfo.pCriature->SetYPos(0.0f);	

	// Bajara el estado de orden actual
	m_CmdInfo.pCriature->m_ActOrder = CCriature::AO_NONE;

	// Libera el camino si procede
	if (m_CmdInfo.pPath) {
	  delete m_CmdInfo.pPath;
	  m_CmdInfo.pPath = NULL;			
	}
	
	// Finaliza posible WAV asociado
	ReleaseWAVWalkSound();

	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de configurar el comando encontrando el camino de a recorrer
//   por la criatura asi como configurando el estado y orientacion de animacion.
//   El metodo sera llamado desde Init y desde Execute. En el ultimo caso, solo
//   cuando se detecte que existe un movimiento pendiente.
// Parametros:
// - TileDest. Tile destino
// - AnimState. Estado de animacion a utilizar para realizar el movimiento.
// - uwDistanceMin. Distancia minima, en caso de que se vaya a utilizar un
//   camino de minima distancia.
// Devuelve:
// - Si se logro inicializar el comando true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CMoveCmd::StartCmd(const AreaDefs::sTilePos& TileDest,
				   const byte AnimState,
				   const word uwDistanceMin,
				   const bool bGhostMode)
{    
  // SOLO si criatura establecida
  ASSERT(m_CmdInfo.pCriature);
  	
  // ¿Hay orden pendiente?
  if (m_bIsPending) {
	// Se libera el camino actual
	if (m_CmdInfo.pPath) {
	  delete m_CmdInfo.pPath;
	  m_CmdInfo.pPath = NULL;
	}
  }

  // Se obtiene el (nuevo) camino 
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  m_CmdInfo.pPath = pWorld->FindPath(m_CmdInfo.pCriature->GetTilePos(), 
								     TileDest,
									 uwDistanceMin,
									 bGhostMode);
  // ¿NO se consiguio camino?
  if (!m_CmdInfo.pPath) { 
	return false; 
  }  

  // Se establece estado animacion y offsets de desplazamiento
  m_CmdInfo.pCriature->GetAnimTemplate()->SetState(AnimState);
  m_CmdInfo.pCriature->SetXPos(0.0f);
  m_CmdInfo.pCriature->SetYPos(0.0f);

  // Establece flag de modo fantasma
  m_CmdInfo.bGhostMode = bGhostMode;
  
  // Se establece info sobre comando en ejecucion  
  m_ExInfo.bXMovComplete = false;
  m_ExInfo.bYMovComplete = false;

  // Hay que preparar el sig. movimiento
  m_ExInfo.bNextMove = true;

  // Se baja el flag de posibles peticiones pendientes
  m_bIsPending = false;

  // Todo correcto 
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ejecuta el comando comprobando si los movimientos en los dos ejes han sido
//   completados. En caso de ser asi, si hay mov. pendiente lo intenta realizar,
//   si el viaje es completado termina la ejecucion y en caso de que existan
//   mas tiles por visitar y no se este en ningun caso de los anteriores, se
//   comenzara a mover a la entidad a las nuevas posiciones solo si eso es
//   algo posible. Si no fuera asi (por ejemplo, hay otra entidad ocupando el
//   camino, se esperara hasta que sea posible.
// Parametros:
// - fDelta. Delta del procentaje recorrido. Necesario para interporlar.
// Devuelve:
// Notas:
// - El proceso de movimiento a otro tile funciona poniendo primero a la 
//   entidad sobre el otro tile y despues realizando el desplazamiento. Al 
//   hacerlo asi, es necesario tener en cuenta que se debera de retrasar la 
//   posicion en x,y del personaje pues de lo contrario, el engine lo dibujaria
//   deacuerdo a su nueva posicion.
// - Siempre existira notificacion cuando se realice la accion de mover
//   a la criatura de un tile A a un tile B.
///////////////////////////////////////////////////////////////////////////////
void 
CMoveCmd::Execute(const float fDelta)
{
  // SOLO si comando activo
  ASSERT(IsActive());
  // SOLO si comando no en pausa
  ASSERT(!IsInPause());

  // Instancia CWorld 
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);

  // ¿Se ha finalizado de mover la entidad a un nuevo tile?  
  if (m_ExInfo.bXMovComplete && 
	  m_ExInfo.bYMovComplete) { 			
	// Se cambia de localidad, teniendo en cuenta que si hubo que hacer un
	// cambio de correccion visual, ahora se debera de retornar a la posicion
	// original ANTES de realizar el cambio de facto
	if (MustChangeAtStart(m_CmdInfo.pPath->GetDirToActTilePos())) {
	  // Se retorna a pos. original
	  pWorld->ChangeLocationForVisuals(m_CmdInfo.pCriature->GetHandle(),
									   m_CmdInfo.pPath->GetActTilePos(),
									   m_CmdInfo.pCriature->GetTilePos());	  
	}

	// Se cambia oficialmente de posicion
	pWorld->ChangeLocation(m_CmdInfo.pCriature->GetHandle(),
						   m_CmdInfo.pPath->GetActTilePos());
	
	// Se notifica el movimiento realizado
	m_CmdInfo.pCriature->ObserversNotify(CriatureObserverDefs::ACTION_REALICE,
  										 RulesDefs::CA_WALK);	
	
	// Si se esta en modo combate y la criatura ya no posee el turno
	// significara que al andar se quedo sin puntos de accion, se finalizara
	iCCombatSystem* const pCombatSys = SYSEngine::GetCombatSystem();
	ASSERT(pCombatSys);
	if (pCombatSys->IsCombatActive()) {
	  if (pCombatSys->GetCriatureInTurn() != m_CmdInfo.pCriature->GetHandle()) {
		End();
		return;
	  }
	} 

	// ¿Orden de parar?
	if (m_bStopWalk) {
	  // Finaliza accion
	  End();
	  return;
	}	
	
	// ¿Mov. pendiente?
	if (m_bIsPending) {	  
	  // ¿Posicion actual distinta que destino?
	  // Nota: Es SEGURO que si hay movimiento pendiente, 
	  // no habra combate activo	  
	  if (m_PendingMove.TileDest != m_CmdInfo.pCriature->GetTilePos()) {
		// ¿NO se puede realizar el movimiento?
		if (!StartCmd(m_PendingMove.TileDest, 
					  m_PendingMove.AnimState,
					  m_PendingMove.uwDistanceMin,
					  m_PendingMove.bGhostMode)) {
		  // No se puede mover, finaliza accion.
		  End();
		  return;
		}
	  } else {
		// No se acepta el movimiento pendiente
		End();
		return;
	  }
	  
  	  // Libera posible sonido WAV de pasos
	  ReleaseWAVWalkSound();

	  // Resetea valores de posicion
	  m_CmdInfo.pCriature->SetXPos(0.0f);
	  m_CmdInfo.pCriature->SetYPos(0.0f);
	} 
	// ¿Viaje completo?
	else if (m_CmdInfo.pPath->IsWalkCompleted()) {	  
	  // Si, finaliza
	  m_CmdInfo.pCriature->ObserversNotify(CriatureObserverDefs::WALK_COMPLETE);
	  End();
	  return;	  
	} 

	// Se baja flag de mov. completo
	m_ExInfo.bXMovComplete = false;
	m_ExInfo.bYMovComplete = false;

	// Se indica que se prepare sig. movimiento	
	m_ExInfo.bNextMove = true;
  }

  // ¿Hay que preparar el sig. movimiento?
  if (m_ExInfo.bNextMove) {
	// ¿NO se esta en modo combate o si se esta, se puede hacer el mov?
	if (CanDoMovementInTurnMode()) {
	  // Si, se avanza al siguiente objetivo, reseteando posiciones si la direccion
	  // anterior es distinta que la actual y comprobando ANTES que el camino
	  // al sig. objetivo es posible (de forma dinamica se habran podido
	  // modificar los accesos).	  
	  const IsoDefs::eDirectionIndex PrevDir = m_CmdInfo.pPath->GetDirToActTilePos();	  
	  m_CmdInfo.pPath->Walk();	  
	  const AreaDefs::sTilePos TileDest(m_CmdInfo.pPath->GetActTilePos());		

	  // Se comprueba que SIGA habiendo camino transitable al destino
	  if (m_CmdInfo.bGhostMode ||
		  pWorld->CalculePathLenght(m_CmdInfo.pCriature->GetTilePos(), TileDest) == 2) {
		// Es transitable
		// Se resetean posiciones si procede
		if (PrevDir != m_CmdInfo.pPath->GetDirToActTilePos()) {
		  m_CmdInfo.pCriature->SetXPos(0.0f);
		  m_CmdInfo.pCriature->SetYPos(0.0f);			
		}
		
		// Si hay que hacer el cambio al comienzo, este sera solo visual
		if (MustChangeAtStart(m_CmdInfo.pPath->GetDirToActTilePos())) {
		  // Cambio visual
		  pWorld->ChangeLocationForVisuals(m_CmdInfo.pCriature->GetHandle(),
										   m_CmdInfo.pCriature->GetTilePos(),
										   m_CmdInfo.pPath->GetActTilePos());
		}
		
		// Configura el siguiente movimiento y reproduce ciclicamente el sonido
		SetNextMovement();	
		PlayWAVWalkSound(m_CmdInfo.pPath->GetActTilePos());
		
		// Se notifica que se esta moviendo, pasando el tile destino
		m_CmdInfo.pCriature->ObserversNotify(CriatureObserverDefs::MOVING,
  											 (TileDest.XTile << 16) | TileDest.YTile);

		// Se baja flag de sig. movimiento
		m_ExInfo.bNextMove = false;
	  } else {
		// El acceso no es transitable
		// Se detiene el comando, notificando que ha sido imposible mover
		const std::string szMsg(SYSEngine::GetGameDataBase()->GetStaticText(GameDataBaseDefs::ST_GENERAL_CANT_MOVE_TO));
		SYSEngine::GetGUIManager()->WriteToConsole(szMsg, true);
		m_CmdInfo.pCriature->ObserversNotify(CriatureObserverDefs::INTERRUPT_MOVING);
		End();
		return;
	  }
	} else {
	  // No se puede continuar el movimiento en modo turno, se notifica
	  // y finaliza la accion
	  m_CmdInfo.pCriature->ObserversNotify(CriatureObserverDefs::INSUFICIENT_ACTION_POINTS,
  										   RulesDefs::CA_WALK);  	
	  End();
	  return;
	}
  }	
	
  // Se actualiza el proceso de movimiento y se reproduce el posible sonido
  UpdateMovement(fDelta);      
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo que se encarga de comprobar si es posible realizar movimiento
//   cuando se esta en modo por turnos. Para que sea posible, la criatura
//   debera de tener suficientes puntos de accion, ademas de estar en modo
//   combate y con turno. Cuando no sea posible, se retornara false.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CMoveCmd::CanDoMovementInTurnMode(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsActive());

  // Se toma instancia al subsistma de combate
  iCCombatSystem* const pCombatSys = SYSEngine::GetCombatSystem();
  ASSERT(pCombatSys);

  // ¿Esta el modo combate activo?
  if (pCombatSys->IsCombatActive()) {	  
    // ¿NO hay suficientes puntos de accion?
    const RulesDefs::CriatureActionPoints ActionPoints = pCombatSys->GetCriatureActionPoints(m_CmdInfo.pCriature->GetHandle());
	if (SYSEngine::GetGameDataBase()->GetRulesDataBase()->GetCombatActionCost(RulesDefs::CA_WALK) > ActionPoints) {
	  // Si, se retorna 
	  return false;
   }
  }

  // Se puede realizar
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Configura el siguiente movimiento a realizar estableciendo los valores
//   de interpolacion y posicion para la entidad.
// - Los valores de interpolacion dependeran de si el movimiento requiere 
//   o no de un cambio de tile al comienzo.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CMoveCmd::SetNextMovement(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsActive());

  // Vbles con las distancias de recorrido
  float fXInitPos = 0.0f;
  float fXEndPos = 0.0f;
  float fYInitPos = 0.0f;
  float fYEndPos = 0.0f; 

  // Se inicializan vbles para sig. movimiento
  m_ExInfo.ActDirection = m_CmdInfo.pPath->GetDirToActTilePos();
  m_CmdInfo.pCriature->GetAnimTemplate()->SetOrientation(m_ExInfo.ActDirection);
	 
  // Segun sea el movimiento, se establecen las vbles de posicion
  switch (m_ExInfo.ActDirection) {
	case IsoDefs::NORTH_INDEX: 	
	  // Ajusta la posicion y establece distancia de interpolacion
	  fYInitPos = m_CmdInfo.pCriature->GetYPos();
	  if (fYInitPos < 0.0f) {//-IsoDefs::TILE_HEIGHT) {
		fYInitPos += IsoDefs::TILE_HEIGHT;
	  }
	  fYEndPos = -IsoDefs::TILE_HEIGHT;
	  m_CmdInfo.pCriature->SetYPos(fYInitPos);
	break;
	   
	case IsoDefs::NORTHEAST_INDEX:
	  // Ajusta la posicion y establece distancia de interpolacion
	  fYInitPos = m_CmdInfo.pCriature->GetYPos();
	  if (fYInitPos < 0.0f) {
		fYInitPos += IsoDefs::TILE_HEIGHT_DIV;
	  }
	  fYEndPos = -IsoDefs::TILE_HEIGHT_DIV;
	  fXInitPos = m_CmdInfo.pCriature->GetXPos();
	  if (fXInitPos > 0.0f) {
		fXInitPos -= IsoDefs::TILE_WIDTH_DIV;
	  }
	  fXEndPos = IsoDefs::TILE_WIDTH_DIV;
	  m_CmdInfo.pCriature->SetXPos(fXInitPos);
	  m_CmdInfo.pCriature->SetYPos(fYInitPos);
	break;
	 
	case IsoDefs::EAST_INDEX:		 	  
	  // Ajusta la posicion y establece distancia de interpolacion
	  //fXInitPos = m_CmdInfo.pCriature->GetXPos() - IsoDefs::TILE_WIDTH;
	  //m_CmdInfo.pCriature->SetXPos(fXInitPos);
	  fXInitPos = m_CmdInfo.pCriature->GetXPos();
	  if (fXInitPos > 0.0f) {
		fXInitPos -= IsoDefs::TILE_WIDTH;
	  }
	  fXEndPos = IsoDefs::TILE_WIDTH;
	  m_CmdInfo.pCriature->SetXPos(fXInitPos);
    break;
	 
	case IsoDefs::SOUTHEAST_INDEX:		 
	  // Ajusta la posicion y establece distancia de interpolacion
	  fXInitPos = m_CmdInfo.pCriature->GetXPos() - IsoDefs::TILE_WIDTH_DIV;
	  fYInitPos = m_CmdInfo.pCriature->GetYPos() - IsoDefs::TILE_HEIGHT_DIV;
	  m_CmdInfo.pCriature->SetYPos(fYInitPos);
	  m_CmdInfo.pCriature->SetXPos(fXInitPos);	  
    break;
	 
	case IsoDefs::SOUTH_INDEX:		 
	  // Ajusta la posicion y establece distancia de interpolacion
	  fYInitPos = m_CmdInfo.pCriature->GetYPos() - IsoDefs::TILE_HEIGHT;
	  m_CmdInfo.pCriature->SetYPos(fYInitPos);	  
	break;
	 
	case IsoDefs::SOUTHWEST_INDEX:
	  // Ajusta la posicion y establece distancia de interpolacion
	  fXInitPos = m_CmdInfo.pCriature->GetXPos();
	  if (fXInitPos < 0.0f) {
		fXInitPos = IsoDefs::TILE_WIDTH_DIV + fXInitPos;
	  } else {
		fXInitPos += IsoDefs::TILE_WIDTH_DIV;
	  }
	  fYInitPos = m_CmdInfo.pCriature->GetYPos() - IsoDefs::TILE_HEIGHT_DIV;
	  m_CmdInfo.pCriature->SetYPos(fYInitPos);
	  m_CmdInfo.pCriature->SetXPos(fXInitPos);	
    break;

	case IsoDefs::WEST_INDEX:		 
	  // Ajusta la posicion y establece distancia de interpolacion
	  fXInitPos = m_CmdInfo.pCriature->GetXPos();
	  if (fXInitPos < 0.0f) {
		fXInitPos += IsoDefs::TILE_WIDTH;
	  }
	  fXEndPos = -IsoDefs::TILE_WIDTH;
	  m_CmdInfo.pCriature->SetXPos(fXInitPos);
    break;
	 
	case IsoDefs::NORTHWEST_INDEX:
	  // Ajusta la posicion y establece distancia de interpolacion
	  fYInitPos = m_CmdInfo.pCriature->GetYPos();
	  if (fYInitPos < 0.0f) {
		fYInitPos += IsoDefs::TILE_HEIGHT_DIV;
	  }
	  fYEndPos = -IsoDefs::TILE_HEIGHT_DIV;
	  fXInitPos = m_CmdInfo.pCriature->GetXPos();
	  if (fXInitPos < 0.0f) {
		fXInitPos += IsoDefs::TILE_WIDTH_DIV;
	  }
	  fXEndPos = -IsoDefs::TILE_WIDTH_DIV;
	  m_CmdInfo.pCriature->SetXPos(fXInitPos);
	  m_CmdInfo.pCriature->SetYPos(fYInitPos);
	break;
  };
 
  // Se establece interpolacion en X si procede
  if (fXInitPos != fXEndPos) {
	m_XInterpolation.Init(fXInitPos, 
						  fXEndPos, 
						  fabs(fabs(fXInitPos) - fabs(fXEndPos)) / float(m_CmdInfo.pCriature->GetXSpeed()));
  }

  // Se establece interpolacion en Y si procede
  if (fYInitPos != fYEndPos) {
	m_YInterpolation.Init(fYInitPos,
						  fYEndPos,
						  fabs(fabs(fYInitPos) - fabs(fYEndPos)) / float(m_CmdInfo.pCriature->GetYSpeed()));
 }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la actualizacion de movimiento, llamando a los distintos 
//   metodos Move y actualizando los flags de movimiento completado.
// Parametros:
// - fDelta. Delta para realizar la interpolacion
// Devuelve:
// Notas:
// - Hace falta guardar las posiciones en las que debera de estar la entidad
//   al finalizar un desplazamiento a un tile en dos vbles temporales, que son
//   fNewXPos y fNewYPos, para que exista coordinacion en la posicion dentro
//   de los movimientos laterales
///////////////////////////////////////////////////////////////////////////////
void
CMoveCmd::UpdateMovement(const float fDelta)
{   
  // Procede a coordinar la actualizacion de offsets mediante el uso
  // de la vble con la direccion actual de movimiento
  switch (m_ExInfo.ActDirection) {
	case IsoDefs::NORTH_INDEX: {
	  // Norte
	  if (!m_ExInfo.bYMovComplete) { 
		MoveUp(fDelta); 		
		// ¿Movimiento finalizado?
		if (m_ExInfo.bYMovComplete) { 
		  m_ExInfo.bXMovComplete = true; 
		}
	  }	  
	} break;
	  
	case IsoDefs::NORTHEAST_INDEX: {
	  // Este
	  if (!m_ExInfo.bXMovComplete) { 	  
		MoveRight(fDelta); 
	  }
	  // Norte
	  if (!(m_ExInfo.bYMovComplete)) { 	  
		MoveUp(fDelta); 
	  }	  
	 } break;
	  
	case IsoDefs::EAST_INDEX: {
	  // Este
	  if (!(m_ExInfo.bXMovComplete)) { 	  
		MoveRight(fDelta); 
		// ¿Movimiento finalizado?
		if (m_ExInfo.bXMovComplete) { 
		  m_ExInfo.bYMovComplete = true; 
		}
	  }	  
	} break;
	  
	case IsoDefs::SOUTHEAST_INDEX: {
	  // Este
	  if (!(m_ExInfo.bXMovComplete)) { 	  
		MoveRight(fDelta); 
	  }
	  // Sur
	  if (!(m_ExInfo.bYMovComplete)) { 	  
		MoveDown(fDelta); 
	  }	  
	} break;
	  
	case IsoDefs::SOUTH_INDEX: {
	  // Sur
	  if (!(m_ExInfo.bYMovComplete)) { 	  
		MoveDown(fDelta); 		
		// ¿Movimiento finalizado?
		if (m_ExInfo.bYMovComplete) { 
		  m_ExInfo.bXMovComplete = true; 
		}
	  }
	} break;
	  
	case IsoDefs::SOUTHWEST_INDEX: {
	  // Oeste
	  if (!(m_ExInfo.bXMovComplete)) { 	  
		MoveLeft(fDelta); 
	  }
	  // Sur
	  if (!(m_ExInfo.bYMovComplete)) { 	  
		MoveDown(fDelta); 
	  }	  
	} break;
	  
	case IsoDefs::WEST_INDEX: {
	  // Este
	  if (!(m_ExInfo.bXMovComplete)) { 	  
		MoveLeft(fDelta); 	  
		// ¿Movimiento finalizado?
		if (m_ExInfo.bXMovComplete) { 
		  m_ExInfo.bYMovComplete = true; 
		}
	  }	  
	} break;
	  
	case IsoDefs::NORTHWEST_INDEX: {
	  // Oeste
	  if (!(m_ExInfo.bXMovComplete)) { 	  
		MoveLeft(fDelta); 
	  }
	  // Norte
	  if (!(m_ExInfo.bYMovComplete)) { 	  
		MoveUp(fDelta); 
	  }	  
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Actualiza los offsets de la entidad para mover a la dcha.
// Parametros:
// - fDelta. Delta para realizar la interpolacion.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CMoveCmd::MoveRight(const float fDelta)
{
  // Se realiza la interpolacion
  if (m_XInterpolation.Interpolate(fDelta)) {
	m_ExInfo.bXMovComplete = true;
  } else {
	m_CmdInfo.pCriature->SetXPos(m_XInterpolation.GetActPos());
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Actualiza los offsets de la entidad para mover a la izq.
// Parametros:
// - fDelta. Delta para realizar la interpolacion.
// Devuelve:
// Notas:
// - Las actualizaciones a la izquierda se realizaran con offsets negativos.
///////////////////////////////////////////////////////////////////////////////
void 
CMoveCmd::MoveLeft(const float fDelta)
{ 
  // Se realiza la interpolacion
  if (m_XInterpolation.Interpolate(fDelta)) {
	m_ExInfo.bXMovComplete = true;
  } else {
	m_CmdInfo.pCriature->SetXPos(m_XInterpolation.GetActPos());
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Actualiza los offsets de la entidad para mover arriba.
// Parametros:
// - fDelta. Delta para realizar la interpolacion.
// Devuelve:
// Notas:
// - Se utilizaran offset negativos en la posicion de la entidad
///////////////////////////////////////////////////////////////////////////////
void 
CMoveCmd::MoveUp(const float fDelta)
{ 
  // Se realiza la interpolacion
  if (m_YInterpolation.Interpolate(fDelta)) {
	m_ExInfo.bYMovComplete = true;
  } else {
	m_CmdInfo.pCriature->SetYPos(m_YInterpolation.GetActPos());
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Actualiza los offsets de la entidad para mover a la abajo.
// Parametros:
// - fDelta. Delta para realizar la interpolacion.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CMoveCmd::MoveDown(const float fDelta)
{
  // Se realiza la interpolacion
  if (m_YInterpolation.Interpolate(fDelta)) {
	m_ExInfo.bYMovComplete = true;
  } else {
	m_CmdInfo.pCriature->SetYPos(m_YInterpolation.GetActPos());
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Reproduce el sonido asociado a la posicion actual, si es que esta posicion
//   tiene sonido asociado. En caso de que el sonido actual sea IGUAL que el
//   sonido previo, no se hara operacion alguna.
// - La reproduccion SOLO se llevara a cabo si la entidad que pisa el suelo
//   tiene elvacion de 0.
// - La reproduccion de los sonidos sera ciclica.
// Parametros:
// - TilePos. Posicion en donde se halla el tile.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CMoveCmd::PlayWAVWalkSound(const AreaDefs::sTilePos& TilePos)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsActive());

  // ¿La entidad tiene elevacion 0?
  if (m_CmdInfo.pCriature->GetElevation() == 0) {
	// Si, se obtiene el nombre del fichero de sonido asociado al tile
	std::string szWAVSound(SYSEngine::GetWorld()->GetFloor(TilePos)->GetWAVWalkSoundFileName());
	SYSEngine::MakeLowercase(szWAVSound);

	// ¿Hay fichero asociado a la posicion recibida?
	if (!szWAVSound.empty()) {
	  // Se toma el nombre del sonido previo
	  iCResourceManager* const pResManager = SYSEngine::GetResourceManager();
	  ASSERT(pResManager);
	  const std::string szPrevWAVSound = (0 != m_ExInfo.hWAVWalkSound) ? 
										 pResManager->GetWAVSoundFileName(m_ExInfo.hWAVWalkSound) : "";

	  // ¿El sonido actual es DISTINTO al sonido previo?	
	  // Nota: seran distintos si antes no habia sonido o si lo habia y es
	  // simplemente distinto al actual
	  if (0 != szWAVSound.compare(szPrevWAVSound)) {
		// Si, se registra el sonido WAV actual, liberando antes el previo (si hay)
		ReleaseWAVWalkSound();
		ASSERT((0 == m_ExInfo.hWAVWalkSound) != 0);
		m_ExInfo.hWAVWalkSound = pResManager->WAVSoundRegister(szWAVSound);
		if (!m_ExInfo.hWAVWalkSound) {
		  SYSEngine::FatalError("No se pudo abrir el fichero de sonido \"%s\".\nFichero inexistente o incompatible.\n", szWAVSound.c_str());
		}

		// Se reproduce el sonido ciclicamente
		SYSEngine::GetAudioSystem()->PlayWAVSound(m_ExInfo.hWAVWalkSound,
												  AudioDefs::WAV_PLAY_LOOP);
	  }	
	} else {
	  // No hay fichero asociado, se finaliza el posible finchero anterior
	  ReleaseWAVWalkSound();
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera el POSIBLE sonido WAV asociado.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CMoveCmd::ReleaseWAVWalkSound(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsActive());
  
  // ¿Hay sonido WAV asociado?
  if (m_ExInfo.hWAVWalkSound) {
	SYSEngine::GetResourceManager()->ReleaseWAVSound(m_ExInfo.hWAVWalkSound);
	m_ExInfo.hWAVWalkSound = 0;
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el modo pausa pero teniendo en cuenta que en caso de que se
//   este reproduciendo un sonido asociado a pisar un floor, se debera de pausar
//   dicho sonido. Al final se propagara a la clase base la peticion.
// Parametros:
// - bPause. Flag de pausa
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CMoveCmd::SetPause(const bool bPause)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsActive());

  // ¿Hay asociado archivo de sonido?
  if (m_ExInfo.hWAVWalkSound) {
	// ¿Se desea pausar?
	if (bPause) {
	  // Si, se pausa
	  SYSEngine::GetAudioSystem()->PauseWAVSound(m_ExInfo.hWAVWalkSound);
	} else {
	  // No, se reanuda
	  SYSEngine::GetAudioSystem()->PlayWAVSound(m_ExInfo.hWAVWalkSound,
											    AudioDefs::WAV_PLAY_LOOP);
	}
  }

  // Propaga
  Inherited::SetPause(bPause);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Detiene a la criatura en su proceso de movimiento. La detencion podra
//   ser progresiva o inmediata.
// Parametros:
// - bNow. Flag para indicar detencion INMEDIATA. Por defecto valdra false.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CMoveCmd::StopWalk(const bool bNow)
{	
  // ¿Detencion INMEDIATA?
  if (bNow) {
	// Si, ¿Movimiento activo
	if (IsActive()) {
	  // Se detendra el comando inmediatamente, en caso de que la
	  // criatura hubiera hecho un cambio de posicion para la correcion
	  // visual, se devolvera esta a su pos. original.	  
	  ASSERT(m_CmdInfo.pPath);
	  if (MustChangeAtStart(m_CmdInfo.pPath->GetDirToActTilePos())) {
		// Se retorna a pos. original		
		SYSEngine::GetWorld()->ChangeLocationForVisuals(m_CmdInfo.pCriature->GetHandle(),
														m_CmdInfo.pPath->GetActTilePos(),
														m_CmdInfo.pCriature->GetTilePos());		
	  }

	  // Establece flags
  	  m_bStopWalk = true;
	  m_bIsPending = false;

	  // Finaliza instancia	  
	  End();
	}	
  } else {
	// Establece flag de detencion progresiva
	m_bStopWalk = true;
	m_bIsPending = false;
  } 
}
	
