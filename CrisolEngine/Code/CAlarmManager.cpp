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
// CAlarmManager.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CAlarmManager.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

#include "CAlarmManager.h"
#include "SYSEngine.h"     
#include "iCAlarmClient.h"
#include "iCResourceManager.h"
#include "iCLogger.h"   
#include "CMemoryPool.h"

// Definicion de estructuras forward
struct CAlarmManager::sNTimeClientInfo {
  // Info asociada a un cliente para alarma de tiempo
  // Datos
  iCAlarmClient*         pClient;    // Client
  float                  fActTime;   // Tiempo de alarma transcurrido
  AlarmDefs::AlarmHandle hAlarm;     // Handle a la alarma  
  bool                   bNewClient; // ¿Cliente nuevo?
  // Constructores
  sNTimeClientInfo(iCAlarmClient* const apClient,
				   const AlarmDefs::AlarmHandle& ahAlarm): pClient(apClient),														   
														   hAlarm(ahAlarm),
														   fActTime(0.0f),
														   bNewClient(true) { }
  sNTimeClientInfo(void): pClient(NULL), 
						  fActTime(0.0f), 
						  hAlarm(0), 
						  bNewClient(true) { }
  // Pool de memoria
  static CMemoryPool m_MPool;
  static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
  static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
};

struct CAlarmManager::sNAlarmTime {
  // Info asociada a un nodo de la lista principal de alarmas de tiempo
  // Datos 
  float          fAlarmTime; // Tiempo asociado a la alarma
  TimeClientList Clients;    // Lista de clientes asociados a ese tiempo
  // Constructores
  sNAlarmTime(void): fAlarmTime(0.0f) { }
  sNAlarmTime(const float afAlarmTime): fAlarmTime(afAlarmTime) { }
  // Pool de memoria
  static CMemoryPool m_MPool;
  static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
  static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
};

struct CAlarmManager::sNWAVClientInfo {
  // Info asociada a un nodo de la lista principal de alarmas WAV
  // Datos
  iCAlarmClient*          pClient;    // Cliente
  ResDefs::WAVSoundHandle hWAVSound;  // Handle al fichero WAV
  AlarmDefs::AlarmHandle  hAlarm;     // Handle a la alarma  
  bool                    bNewClient; // ¿Cliente nuevo? 
  // Constructores
  sNWAVClientInfo(void): pClient(NULL), 
						 hWAVSound(0), 
						 hAlarm(AlarmDefs::WAV_ALARM), 
						 bNewClient(true) { }
  sNWAVClientInfo(iCAlarmClient* const apClient, 
				  ResDefs::WAVSoundHandle ahWAVSound,
				  const AlarmDefs::AlarmHandle& ahAlarm): pClient(apClient),
														  hWAVSound(ahWAVSound),
														  hAlarm(ahAlarm),
														  bNewClient(true) { }
  // Pool de memoria
  static CMemoryPool m_MPool;
  static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
  static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
};

// Inicializacion de los manejadores de memoria
CMemoryPool 
CAlarmManager::sNTimeClientInfo::m_MPool(8, sizeof(CAlarmManager::sNTimeClientInfo), true);
CMemoryPool 
CAlarmManager::sNAlarmTime::m_MPool(16, sizeof(CAlarmManager::sNAlarmTime), true);
CMemoryPool
CAlarmManager::sNWAVClientInfo::m_MPool(8, sizeof(CAlarmManager::sNWAVClientInfo), true);

// Inicializacion de la unica instancia singlenton
CAlarmManager* CAlarmManager::m_pAlarmManager = NULL;

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia.
// Parametros:
// Devuelve:
// - Si no ha existido ningun problema true. En caso contrario false.
// Notas:
// - No se permitira reinicializar.
///////////////////////////////////////////////////////////////////////////////
bool 
CAlarmManager::Init(void)
{
  // ¿Se intenta reinicializar?
  if (IsInitOk()) { return false; }

  #ifdef ENGINE_TRACE    
    SYSEngine::GetLogger()->Write("CAlarmManager::Init> Inicializando manager de alarmas.\n");    
  #endif

  // Se toman instancias a otros subsistemas / clases de utilidad
  m_pResManager = SYSEngine::GetResourceManager();
  ASSERT(m_pResManager);

  // Inicializa vbles de miembro
  // Alarmas de tiempo
  m_TimeAlarms.LastHandle = AlarmDefs::TIME_ALARM;
  // Alarmas asociadas a ficheros WAV
  m_WAVAlarms.LastHandle = AlarmDefs::WAV_ALARM;
  
  // Todo correcto
  m_bIsInitOk = true; 
  #ifdef ENGINE_TRACE    
    SYSEngine::GetLogger()->Write("                   | Ok.\n");    
  #endif
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia, liberando recursos
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAlarmManager::End(void)
{
  if (IsInitOk()) {
	#ifdef ENGINE_TRACE    
	  SYSEngine::GetLogger()->Write("CAlarmManager::End> Finalizando manager de alarmas.\n");    
	#endif
	
	// Libera posibles alarmas instaladas
	ReleaseTimeAlarms();
	ReleaseWAVAlarms();
	  
	// Baja flag
	m_bIsInitOk = false;
	#ifdef ENGINE_TRACE    
	  SYSEngine::GetLogger()->Write("                  | Ok.\n");    
	#endif
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera todos los clientes instalados de las alarmas de tiempo
// Parametros:
// Devuelve:
// Notas:
// - Al borrar no se realizara ningun tipo de notificacion. Lo ideal sera que
//  cuando se llame a este metodo, todos los clientes se hayan desinstalado
//  correctamente y, en consecuencia, no haya ninguno activo.
///////////////////////////////////////////////////////////////////////////////
void 
CAlarmManager::ReleaseTimeAlarms(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Procede a liberar todos los nodos
  AlarmTimeListIt AlarmsIt(m_TimeAlarms.Alarms.begin());
  while (AlarmsIt != m_TimeAlarms.Alarms.end()) {
	// Borra todos los clientes asociados al nodo de tiempo de alarma
	TimeClientListIt ClientsIt((*AlarmsIt)->Clients.begin());
	while (ClientsIt != (*AlarmsIt)->Clients.end()) {
	  // Borra nodo de cliente de memoria y de lista
	  delete *ClientsIt;
	  ClientsIt = (*AlarmsIt)->Clients.erase(ClientsIt);
	}
	// Borra el nodo de tiempo de alarma de memoria y lista
	delete *AlarmsIt;
	AlarmsIt = m_TimeAlarms.Alarms.erase(AlarmsIt);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera todos los clientes asociados a alarmas de tipo WAV.
// Parametros:
// Devuelve:
// Notas:
// - Al borrar no se realizara ningun tipo de notificacion. Lo ideal sera que
//  cuando se llame a este metodo, todos los clientes se hayan desinstalado
//  correctamente y, en consecuencia, no haya ninguno activo.
///////////////////////////////////////////////////////////////////////////////
void 
CAlarmManager::ReleaseWAVAlarms(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Procede a liberar todos los nodos
  WAVClientListIt AlarmsIt(m_WAVAlarms.Alarms.begin());
  while (AlarmsIt != m_WAVAlarms.Alarms.end()) {	
	// Borra el nodo de memoria y lista
	delete *AlarmsIt;
	AlarmsIt = m_WAVAlarms.Alarms.erase(AlarmsIt);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la llamada a los metodos para la actualizacion de los distintos
//   tipos de alarmas.
// Parametros:
// - fDelta. Valor delta a sumar a los distintos contadores.
// Devuelve:
// Notas:
// - No se realizara una comprobacion if (fDelta == 0) { return; } ya que este
//   caso sera muy complicado que se de.
///////////////////////////////////////////////////////////////////////////////
void 
CAlarmManager::Update(const float fDelta)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Actualiza alarmas
  UpdateTimeAlarms(fDelta);
  UpdateWAVAlarms();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre toda la lista, actualizando tiempos asociados a los clientes. En
//   caso de que un cliente supere, en su contador, el tiempo de alarma maximo,
//   se realizara una notificacion y se borrara tanto el nodo de la memoria 
//   como de la lista.
// - Las actualizaciones solo se realizaran si el flag bNewClient esta a false.
//   En caso contrario, simplemente se bajara para la siguiente iteracion.
// Parametros:
// - fDelta. Valor delta a sumar a los distintos contadores. Este delta seran
//   los 
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAlarmManager::UpdateTimeAlarms(const float fDelta)
{
  // Se procede a recorrer clientes y notificar eventos
  AlarmTimeListIt AlarmsIt(m_TimeAlarms.Alarms.begin());
  for (; AlarmsIt != m_TimeAlarms.Alarms.end(); ++AlarmsIt) {
	// Navega por los clientes asociados
	TimeClientListIt ClientsIt((*AlarmsIt)->Clients.begin());
	while (ClientsIt != (*AlarmsIt)->Clients.end()) {
	  // ¿Cliente nuevo?
	  if ((*ClientsIt)->bNewClient) {
		// Si, se baja el flag
		(*ClientsIt)->bNewClient = false;
	  } else if (!IsInPause((*ClientsIt)->hAlarm)) {
		// No, se actualiza contador
		(*ClientsIt)->fActTime += fDelta;

		// ¿Hay que notificar?
		if ((*ClientsIt)->fActTime >= (*AlarmsIt)->fAlarmTime) {
		  // Se notifica y borra
		  (*ClientsIt)->pClient->AlarmNotify(AlarmDefs::TIME_ALARM, (*ClientsIt)->hAlarm);		  
		  delete *ClientsIt;
		  ClientsIt = (*AlarmsIt)->Clients.erase(ClientsIt);
		
		  // Sig. iteracion
		  continue;
		}		
	  }
	  // Sig. cliente
	  ++ClientsIt;
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre la lista de alarmas asociadas a ficheros WAV. El proceso sera el
//   de tomar instancia al recurso WAV. Si por alguna razon el recurso WAV 
//   ya no estuviera instalado, se borrara la alarma directamente sin notificar
//   nada.
// - Antes de proceder a notificar se comprobara el flag de alarma recien 
//   instalada.
// Parametros:

// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAlarmManager::UpdateWAVAlarms(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se procede a recorrer clientes y notificar eventos
  WAVClientListIt It(m_WAVAlarms.Alarms.begin());
  while (It != m_WAVAlarms.Alarms.end()) {
	// ¿Cliente recien instalado?
	if ((*It)->bNewClient) {
	  // Se bajara SOLO si el fichero WAV esta sonando, de no hacerlo asi
	  // podrian producirse inconsistencias pues el subsistema de alarmas
	  // funciona mucho mas rapidamente que el de audio.	  
	  if (m_pResManager->IsWAVSoundPlaying((*It)->hWAVSound)) {
		(*It)->bNewClient = false;
	  }	  
	} else if (!IsInPause((*It)->hAlarm)) {
	  // No, obtiene instancia al recurso
	  // -> A CAMBIAR LA FORMA DE CONOCERLO
	  DXDSWAVSound* const pWAVSound = m_pResManager->GetWAVSound((*It)->hWAVSound);
	  // ¿NO se hallo el recurso?
	  if (!pWAVSound) {
		// Se elimina alarma
		delete *It;
		It = m_WAVAlarms.Alarms.erase(It);
		continue;
	  }
	  // ¿Ha finalizado la reproducion la reproduccion?
	  if (!m_pResManager->IsWAVSoundPlaying((*It)->hWAVSound)) {
		// Se notifica
		(*It)->pClient->AlarmNotify(AlarmDefs::WAV_ALARM, (*It)->hAlarm);
		// Se desinstala
		delete *It;
		It = m_WAVAlarms.Alarms.erase(It);
		continue;
	  }
	}
	// Sig. cliente
	++It;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Instala una nueva alarma de tiempo. 
// Parametros:
// - pClient. Direccion del cliente.
// - fTime. Tiempo en que se querra activar la alarma.
// Devuelve:
// - Handle asociada a la alarma. En caso de que el handle sea 0, habra 
//   ocurrido un problema.
// Notas:
// - Se permitira asociar alarmas con valores menores o iguales a 0. Dichas
//   alarmas seran, naturalmente, de notificacion inmediata.
// - Los nodos estaran colocados de menor tiempo a mayor tiempo.
///////////////////////////////////////////////////////////////////////////////
AlarmDefs::AlarmHandle 
CAlarmManager::InstallTimeAlarm(iCAlarmClient* const pClient,
							    const float fTime)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);  

  // Se localiza nodo de tiempos adecuado, si es que hay alguno.
  AlarmTimeListIt AlarmsIt(m_TimeAlarms.Alarms.begin());
  for (; AlarmsIt != m_TimeAlarms.Alarms.end(); ++AlarmsIt) {
	// ¿No hay nodo igual o bien se hallo un nodo con el mismo valor?
	if ((*AlarmsIt)->fAlarmTime >= fTime) {
	  break;
	}
  }  
  
  // Se crea nuevo nodo
  const AlarmDefs::AlarmHandle hAlarm = GetNextHandle(AlarmDefs::TIME_ALARM);
  ASSERT(hAlarm);
  sNTimeClientInfo* const pNewClient = new sNTimeClientInfo(pClient, hAlarm);
  ASSERT(pNewClient);

  // ¿Se hallo nodo con el mismo valor temporal?
  if (AlarmsIt != m_TimeAlarms.Alarms.end() && 
	  (*AlarmsIt)->fAlarmTime == fTime) {	
	(*AlarmsIt)->Clients.push_back(pNewClient);		  	
  } else {
	// No, hay que crear un nuevo nodo e insertarlo DESPUES (del localizado o final)
	sNAlarmTime* const pNewTime = new sNAlarmTime(fTime);
	m_TimeAlarms.Alarms.insert(AlarmsIt, pNewTime);
	
	// Por ultimo, se inserta el cliente
	pNewTime->Clients.push_back(pNewClient);
  }

  // Se retorna handle a la alarma
  return hAlarm;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Instala una nueva alarma asociada a fichero WAV
// Parametros:
// - pClient. Direccion del cliente.
// - hWAVSound. Handle al fichero wav.
// Devuelve:
// - Handle asociada a la alarma. En caso de que el handle sea 0, habra 
//   ocurrido un problema.
// Notas:
///////////////////////////////////////////////////////////////////////////////
AlarmDefs::AlarmHandle 
CAlarmManager::InstallWAVAlarm(iCAlarmClient* const pClient,
							   const ResDefs::WAVSoundHandle& hWAVSound)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pClient);  
  ASSERT(hWAVSound);

  // Se crea un nuevo nodo
  const AlarmDefs::AlarmHandle hAlarm = GetNextHandle(AlarmDefs::WAV_ALARM);
  ASSERT(hAlarm);
  sNWAVClientInfo* const pNewClient = new sNWAVClientInfo(pClient, hWAVSound, hAlarm);
  ASSERT(pNewClient);

  // Se inserta en la lista de clientes
  m_WAVAlarms.Alarms.push_back(pNewClient);

  // Se retorna handle a la alarma
  return hAlarm;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Quita la alarma que este instalada, averiguando el tipo de que se trata.
// Parametros:
// - hAlarm. Handle a la alarma a quitar.
// Devuelve:
// - En caso de que se encuentre true. Si no se encontrara (cosa rara) false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CAlarmManager::UninstallAlarm(const AlarmDefs::AlarmHandle& hAlarm)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hAlarm);

  // Obtiene el tipo y llama al metodo apropiado
  const AlarmDefs::eAlarmType Type = AlarmDefs::eAlarmType(hAlarm & AlarmDefs::TYPE_MASK);
  bool bResult;
  switch (Type) {	
	case AlarmDefs::TIME_ALARM: {
	  // Alarma de tiempo
	  bResult = UninstallTimeAlarm(hAlarm);
	} break;	  

	case AlarmDefs::WAV_ALARM: {
	  // Alarma de notificacion de la finalizacion de un fichero WAV
	  bResult = UninstallWAVAlarm(hAlarm);
	} break;

	default:
	  ASSERT(false);
  }; // ~ switch

  // ¿Se desinstalo alarma?
  if (bResult) {
	// Se quita el posible modo pausa asociado
	SetPause(hAlarm, false);
  }  

  // Se retorna
  return bResult;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Quita una alarma de tiempo.
// Parametros:
// - hAlarm. Handle a la alarma a quitar.
// Devuelve:
// - En caso de que se encuentre true. Si no se encontrara (cosa rara) false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CAlarmManager::UninstallTimeAlarm(const AlarmDefs::AlarmHandle& hAlarm)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hAlarm);

  // Se procede a localizar el handle.
  AlarmTimeListIt AlarmsIt(m_TimeAlarms.Alarms.begin());
  for (; AlarmsIt != m_TimeAlarms.Alarms.end(); ++AlarmsIt) {
	// Navega por los clientes asociados
	TimeClientListIt ClientsIt((*AlarmsIt)->Clients.begin());
	for (; ClientsIt != (*AlarmsIt)->Clients.end(); ++ClientsIt) {
	  // ¿El cliente actual posee el handle recibido?
	  if (hAlarm == (*ClientsIt)->hAlarm) {
		// Si, se elimina el cliente y se retorna correctamente		
		delete *ClientsIt;
		(*AlarmsIt)->Clients.erase(ClientsIt);		
		return true;
	  }	  
	}
  }

  // No se pudo localizar el cliente
  return false; 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Quita una alarma asociada a un fichero WAV.
// Parametros:
// - hAlarm. Handle a la alarma a quitar.
// Devuelve:
// - En caso de que se encuentre true. Si no se encontrara (cosa rara) false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CAlarmManager::UninstallWAVAlarm(const AlarmDefs::AlarmHandle& hAlarm)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hAlarm);

  // Se procede a localizar la alarma por handle
  WAVClientListIt It = m_WAVAlarms.Alarms.begin();
  for (; It != m_WAVAlarms.Alarms.end(); ++It) {
	// ¿Es el handle de la alarma buscada?
	if (hAlarm == (*It)->hAlarm) {
	  // Si, se borra la alarma y se retorna
	  delete *It;
	  It = m_WAVAlarms.Alarms.erase(It);
	  return true;
	}
  } 

  // No se pudo localizar el cliente
  return false; 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el siguiente handle asociado al tipo de alarma AlarmType.
// Parametros:
// - AlarmType. Tipo de alarma de la que obtener el siguiente handle.
// Devuelve:
// - El siguiente handle. Debera de ser distinto de 0.
// Notas:
// - Para la obtencion del tipo de alarma, se realizara un avance circular.
///////////////////////////////////////////////////////////////////////////////
AlarmDefs::AlarmHandle 
CAlarmManager::GetNextHandle(const AlarmDefs::eAlarmType& AlarmType)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se obtiene direccion al handle a actualizar
  AlarmDefs::AlarmHandle* pActHandle = NULL;
  switch (AlarmType) {
	case AlarmDefs::TIME_ALARM: {
	  // Alarma de tiempo
	  pActHandle = &m_TimeAlarms.LastHandle;	
	} break;
	  
	case AlarmDefs::WAV_ALARM: {
	  // Alarma asociada a finalizacion de fichero WAV
	  pActHandle = &m_WAVAlarms.LastHandle;
	} break;	
  }; // ~ switch

  // ¿NO se obtuvo handle asociado al tipo de alarma?
  if (!pActHandle) {	
	return 0;
  }

  // Se procede a actualizar handle
  word uwActValue = *pActHandle & AlarmDefs::MAX_HANDLE_VALUE;
  if (uwActValue == AlarmDefs::MAX_HANDLE_VALUE) {
	uwActValue = 0;
  } else {
	++uwActValue;
  }
  *pActHandle = AlarmType | uwActValue;

  // Se retorna el handle
  return *pActHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece / quita el modo pausa de una determinada alarma.
// - Para mantener los flags de pausa, en lugar de usar un bool asociado a
//   cada nodo, se usara un conjunto en donde todos aquellos handles que se
//   hallen en el, estaran en modo pausa.
// Parametros:
// - hAlarm. Handle a la alarma.
// - bPause. Flag que indica el nuevo modo.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAlarmManager::SetPause(const AlarmDefs::AlarmHandle& hAlarm, 
						const bool bPause)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Se desea poner la alarma en modo pausa?
  if (bPause) {
	// Si, se incluye en el conjunto 
	m_AlarmsInPause.insert(hAlarm);
  } else {
	// No, se elimina del conjunto
	m_AlarmsInPause.erase(hAlarm);
  }
}
