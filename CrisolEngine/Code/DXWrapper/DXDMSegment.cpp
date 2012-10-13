///////////////////////////////////////////////////////////////////////////////
// DX7Wrapper - DirectX 7 Wrapper
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

///////////////////////////////////////////////////////////////////////
// DXDMSegment.cpp
// BlackDice
//
// Descripcion:
// - Para mas informacion consultar DXDMSegment.h
///////////////////////////////////////////////////////////////////////
#include "DXDMSegment.h"

#include "DXDMManager.h"
#include "DXDMError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Constructor;
// Parametros:
// Devuelve:
// Notas:
// - Llama al metodo Init para realizar la inicializacion
////////////////////////////////////////////////////////////////////// 
DXDMSegment::DXDMSegment(void)
{
  // Inicializa punteros
  Init(); 
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicialia vbles de miembro
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
void DXDMSegment::Init(void)
{
  // Inicializacion de punteros
  m_pSegment = NULL;
  m_pSegState = NULL;
  m_poDXDMManager = NULL;  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destructor;
// Parametros:
// Devuelve:
// Notas:
// - Para realizar la opearcion de liberacion de memoria llama
//   al metodo Clean. Como el metodo Clean se podra llamar tambien
//   desde otros puntos, se incluye en el destructor la desvinculacion
//   de la instancia al manager MIDI.
////////////////////////////////////////////////////////////////////// 
DXDMSegment::~DXDMSegment(void)
{ 
  // Libera memoria
  Clean(); 

  // Desvincula instancia al manager de musica
  m_poDXDMManager = NULL; 
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Descarga toda la informacion referida al segmento actual.
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
void DXDMSegment::Unload(void)
{  
  if (m_pSegment) {
    // Si la musica esta sonando la detiene
    Stop();     
    // Libera los instrumentos del segmento
    m_pSegment->SetParam(GUID_Unload, 
                         -1, 0, 0, 
                         (void*)m_poDXDMManager->GetPerformanceObj());   
  }
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera de memoria los punteros de la clase.
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
void DXDMSegment::Clean(void)
{  
  if (m_pSegment)
  {// Procede a liberar objetos    
    // Si la musica esta sonando la detiene
    Stop(); 
    
    // Libera los instrumentos del segmento
    m_pSegment->SetParam(GUID_Unload, 
                         -1, 0, 0, 
                         (void*)m_poDXDMManager->GetPerformanceObj());  
    
    // Libera objeto IDirectMusicSegmentState si procede
    if (m_pSegState) {
      m_pSegState->Release();
      m_pSegState = NULL;
    }
    
    // Libera objeto IDirectMusicSegment
    m_pSegment->Release();
    m_pSegment = NULL;
  }  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la instancia a la clase que mantiene el objeto
//   IDirectMusicPerformance, que permitira que en el metodo Load
//   se pueda crear un objeto Segment.
// Parametros:
// - poDXDMManager. Instancia Manager. Interesa por el Performance.
// Devuelve:
// - true: Si todo ha ido bien
// - false si ha existido algun problema.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDMSegment::Create(DXDMManager* const poDXDMManager)
{
  // SOLO si hay referencia a DXDMPerformance
  DXASSERT(poDXDMManager != NULL);

  // Toma la instancia y sale
  m_poDXDMManager = poDXDMManager;
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un segmento con el fichero fichero MIDI pasado como parametro
// Parametros:
// - oMIDIFile: Nombre del fichero MIDI con la musica.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema.
// Notas:
// - En caso de que el segmento ya se halle creado, este sera liberado
//   de memoria para crear uno nuevo. Asi mismo, se liberaran los 
//   instrumentos asociados al segmento que estaba cargado.
// - Se supone que los instrumentos no se descargan de forma manual
//   por lo que este metodo incorporara una llamada a la descarga de
//   los mismos en el segmento.
////////////////////////////////////////////////////////////////////// 
bool DXDMSegment::LoadMIDISegment(const std::string& oMIDIFile)
{
  // SOLO si hay referencia a DXDMPerformance
  DXASSERT(m_poDXDMManager != NULL);

  // Ya existe un segmento; se detiene, libera instrumentos y objetos.
  if (m_pSegment) {  Clean(); }

  DMUS_OBJECTDESC  ObjDesc;                 // Descripcion del Segmento
  WCHAR            wszFileName[_MAX_PATH];  // Fichero en Uni Code

  // Inicializaciones  
  MULTI_TO_WIDE(wszFileName, oMIDIFile.c_str());

  // Prepara la descripcion del segmento
  ObjDesc.guidClass = CLSID_DirectMusicSegment;
  ObjDesc.dwSize = sizeof(DMUS_OBJECTDESC);
  wcscpy(ObjDesc.wszFileName, wszFileName);
  ObjDesc.dwValidData = DMUS_OBJ_CLASS |
                        DMUS_OBJ_FILENAME |
                        DMUS_OBJ_FULLPATH;

  // Procede a crear el segmento y cargar el fichero
  m_CodeResult = m_poDXDMManager->GetLoaderObj()->GetObject(&ObjDesc,
                                                            IID_IDirectMusicSegment,
                                                            (void **) &m_pSegment);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDMError::DXDMErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }  
      
  // Indica que se va a crear un segmento para almacenar ficheros MIDI
  m_CodeResult = m_pSegment->SetParam((REFGUID)GUID_StandardMIDIFile, 
                                      -1, 0, 0, 
                                      (void*)m_poDXDMManager->GetPerformanceObj());
  if (FALSE(m_CodeResult)) {
    // Han existido problemas
    DXDMError::DXDMErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }  

  // Descarga los instrumentos al segmento de forma manual
  m_CodeResult = m_pSegment->SetParam((REFGUID)GUID_Download, 
                                      -1, 0, 0, 
                                      (void*)m_poDXDMManager->GetPerformanceObj());
 if (FALSE(m_CodeResult)) {
   // Han existido problemas
   DXDMError::DXDMErrorToString(m_CodeResult, m_sLastCode);
   return false;
 }
 
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Reproduce el fichero MIDI almacenando en la cola de
//   reproduccion. Cuando le llegue el turno se reproducira.
// Parametros:
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si existe algun problema.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDMSegment::PlayQueue(void)
{
  // SOLO si hay Segment
  DXASSERT(m_pSegment != NULL);
  // SOLO si hay referencia a DXDMPerformance
  DXASSERT(m_poDXDMManager != NULL);
  
  // Reproduce la musica almacenando el segmento en una cola. Cuando le
  // llegue el turno se reproducira automaticamente.
  m_CodeResult = m_poDXDMManager->GetPerformanceObj()->PlaySegment(m_pSegment,
                                                                     DMUS_SEGF_QUEUE,
                                                                     0,
                                                                     &m_pSegState);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDMError::DXDMErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Reproduce el fichero MIDI de forma inmediata y cancelando la cola
//   de reproduccion.
// Parametros:
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si existe algun problema.
// Notas:
// - Si se estaba tocando en modo Queue, este quedara cancelado.
////////////////////////////////////////////////////////////////////// 
bool DXDMSegment::PlayNow(void)
{
  // SOLO si hay Segment
  DXASSERT(m_pSegment != NULL);
  // SOLO si hay referencia a DXDMPerformance
  DXASSERT(m_poDXDMManager != NULL);
  
  // Reproduce la musica de forma instantanea, sin respetar los 
  // segmentos que estuvieran en la cola
  m_CodeResult = m_poDXDMManager->GetPerformanceObj()->PlaySegment(m_pSegment,
                                                                     DMUS_SEGF_BEAT,
                                                                     0,
                                                                     &m_pSegState);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDMError::DXDMErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Detiene el Segmento actual si es que esta reproduciendose.
// Parametros:
// Devuelve:
// - true: Si no hay problemas.
// - false: Si ha existido algun problema.
// Notas:
// - Si no hay segmento creado, simplemente devuelve false. Se
//   evita poner aqui DXASSERT para garantizar que el destructor
//   no encuentre problemas.
////////////////////////////////////////////////////////////////////// 
bool DXDMSegment::Stop(void)
{
  // No hay segmento creado
  if (!m_pSegment) { return false; }

  // Detiene el segmento
  m_CodeResult = m_poDXDMManager->GetPerformanceObj()->Stop(m_pSegment,
                                                            m_pSegState,
                                                            0,0);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDMError::DXDMErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el segmento actual esta siendo reproducido.
// Parametros:
// Devuelve:
// - true: Si esta siendo reproducido
// - false: Si no esta siendo reproducido. Tambien devuelve false
//   si ha existido un posible error.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDMSegment::IsPlaying(void)
{
  // SOLO si hay Segment
  DXASSERT(m_pSegment != NULL);
  // SOLO si hay StateSegment 
  DXASSERT(m_pSegState != NULL);
  // SOLO si hay referencia a DXDMPerformance
  DXASSERT(m_poDXDMManager != NULL);

  // Comprueba si se esta tocando el segmento actual
  m_CodeResult = m_poDXDMManager->GetPerformanceObj()->IsPlaying(m_pSegment,
                                                                 m_pSegState);
  // ¿Esta sonando?
  if (m_CodeResult == S_FALSE) {
    // No esta sonando
    // Establecemos m_CodeResult a S_OK para indicar que el false devuelto
    // no esta relacionado con un error de ejecucion
    m_CodeResult = S_OK; 
    return false;
  }
  else if (m_CodeResult != S_OK) {
    // Hay problemas
    DXDMError::DXDMErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
  
  // Esta sonando
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Reproduce el segmento esperando un espacio de tiempo. 
// Parametros:
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si existe algun problema.
// Notas:
// - El flag DMUS_SEGF_AFTERPREPARETIME es utilizado para lograr una
//   sincronizacion con el final del segmento anterior y en respuesta
//   al evento DMUS_NOTIFICATION_SEGALMOSTEND. Ideal para realizar
//   loops a la hora de hacer sonar una pieza MIDI.
////////////////////////////////////////////////////////////////////// 
bool DXDMSegment::PlayElapsed(void)
{
  // SOLO si hay Segment
  DXASSERT(m_pSegment != NULL);
  // SOLO si hay referencia a DXDMPerformance
  DXASSERT(m_poDXDMManager != NULL);
  
  // Reproduce la musica guardando un intervalo de tiempo entre que se
  // ordena que suene y el momento en que de verdad comenzara a hacerlo
  m_CodeResult = m_poDXDMManager->GetPerformanceObj()->PlaySegment(m_pSegment,
                                                                   DMUS_SEGF_AFTERPREPARETIME,
                                                                   0,
                                                                   &m_pSegState);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDMError::DXDMErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Todo correcto
  return true;
}