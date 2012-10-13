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
// DXDMManager.cpp
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXDMManager.h.
///////////////////////////////////////////////////////////////////////
#include "DXDMManager.h"

#include "DXSound.h"
#include "DXDMError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Constructor;
// Parametros:
// Devuelve:
// Notas:
// - Llama al metodo Init para realizar la inicializacion
////////////////////////////////////////////////////////////////////// 
DXDMManager::DXDMManager(void)
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
void DXDMManager::Init(void)
{
  // Inicializacion de punteros y vbles de miembro
  m_pPerformance = NULL;
  m_pLoader = NULL;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destructor;
// Parametros:
// Devuelve:
// Notas:
// - Para realizar la opearcion de liberacion de memoria llama
//   al metodo Clean
////////////////////////////////////////////////////////////////////// 
DXDMManager::~DXDMManager(void)
{ 
  // Libera memoria
  Clean(); 
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera de memoria los punteros de la clase.
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
void DXDMManager::Clean(void)
{  
  if (m_pPerformance) {
    // Elimina cualquier tipo de notificacion
    m_pPerformance->RemoveNotificationType(GUID_NULL);

	if (m_pLoader) {
	  // Libera objeto IDirectMusicLoader
	  m_pLoader->Release();
	  m_pLoader = NULL;
	}

    // Cierra y libera objeto IDirectMusicPerformance
    m_pPerformance->CloseDown();
    m_pPerformance->Release();
    m_pPerformance = NULL;
  }
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un objeto IDirectMusicPerformance y llama a CreateLoader 
//   para hacer lo propio con IDirectMusicLoader.
// Parametros:
// - pDSound: Puntero al objeto IDirectSound. Si vale NULL, hara falta
//   recibir el parametro hWnd. Se pasara NULL cuando no se trabaje
//   en coordinacion con un objeto DirectSound.
// - hWnd: Handle a la ventana principal. En caso de que IDirectSound
//   valga distinto de NULL, este parametro sera ignorado.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDMManager::Create(const HWND& hWnd, 
						 DXSound* const pDXSound)
{
  // SOLO si performance no esta creado
  DXASSERT(m_pPerformance == NULL);
  
  // Crea el objeto IDirectMusicPerformance
  m_CodeResult = CoCreateInstance(CLSID_DirectMusicPerformance,
                                  NULL,
                                  CLSCTX_INPROC,
                                  IID_IDirectMusicPerformance2,
                                  (void **)&m_pPerformance);
  if (FAILED(m_CodeResult)) {
    // Han existido problemas
    DXDMError::DXDMErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Inicializa objeto IDirectMusicPerformance.
  m_CodeResult = m_pPerformance->Init(NULL, 
                                      pDXSound->GetObj(),
                                      hWnd);
  if (FAILED(m_CodeResult))
  {// Han existido problemas
    DXDMError::DXDMErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
  
  // Se añade el puerto por defecto al Performance. 
  m_CodeResult = m_pPerformance->AddPort(NULL);
  if (FAILED(m_CodeResult)) {
    // Han existido problemas
    DXDMError::DXDMErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
  
  // Se crea el objeto IDirectMusicLoader
  if (!CreateLoader()) {
    // Hay problemas; se establece error interno
    m_CodeResult = S_OK;
    m_sLastCode = DXDMMANAGER_LOADERCREATEERROR_MSG;
    return false;
  }

  // Establece handle de notificaciones
  m_hNotify = CreateEvent(NULL, FALSE, FALSE, NULL);  
  m_pPerformance->SetNotificationHandle(m_hNotify, 0);
  
  // Añade notificaciones para los segmentos
  GUID guid = GUID_NOTIFICATION_SEGMENT;
  m_pPerformance->AddNotificationType(guid);

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Añade una notificacion. 
// Parametros:
// - guidNotification: Notificacion sobre la que queremos ser
//   informados. Las posibles notificaciones que se pueden establecer
//   son, en la version 7.0, las siguientes:
//    - GUID_NOTIFICATION_CHORD 
//    - GUID_NOTIFICATION_COMMAND 
//    - GUID_NOTIFICATION_MEASUREANDBEAT 
//    - GUID_NOTIFICATION_PERFORMANCE 
//    - GUID_NOTIFICATION_SEGMENT 
//   Por defecto, al crear un performance, se indica que se quiere 
//   estar informado segmentos, esto es: GUID_NOTIFICATION_SEGMENT
// Devuelve:
// - true: Todo ha ido bien
// - false: Han existido problemas.
// Notas:
// - Se supone que se ha creado el objeto Performance
////////////////////////////////////////////////////////////////////// 
bool DXDMManager::AddNotification(const GUID guidNotification)
{
  // SOLO si se ha creado el objeto performance
  DXASSERT(m_pPerformance != NULL);

  // Se añade notifiacion. Recordar que por defecto existe la
  // notificacion GUID_NOTIFICATION_SEGMENT
  m_CodeResult = m_pPerformance->AddNotificationType(guidNotification);
  if (FAILED(m_CodeResult)) {
    // Han existido problemas
    DXDMError::DXDMErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el mensaje dejado por la notificacion que ocasiono el
//   ultimo evento.
// Parametros:
// - pdwNotification: Direccion al evento dejado por la ultima 
//   notificacion. 
// Devuelve:
// - true: Ha habido evento y en adwNotification se encuentra la 
//   notificacion.
// - false: Han existido problemas o bien no ha habido notificacion y
//   adwNotification no contiene ningun valor.
// Notas:
// - El uso normal es llamar a este metodo desde una funcion que se
//   ejecute en modo multitarea para saber y actuar correctamente
//   ante los eventos que se vayan desencadenando en DirectMusic.
//   Al obtener el valor de la notificacion, se debera de interpretar
//   el resultado.
////////////////////////////////////////////////////////////////////// 
bool DXDMManager::GetNotification(DWORD& dwNotification)
{
  // SOLO si se ha creado el performance
  DXASSERT(m_pPerformance != NULL);
  // SOLO si parametros validos
  DXASSERT(pdwNotification);
  
  // Comprueba si hay notificacion
  DMUS_NOTIFICATION_PMSG* pEvent;  
  m_CodeResult = m_pPerformance->GetNotificationPMsg(&pEvent);
  if (m_CodeResult == S_OK) {
    // Hay notificacion    
    // Devuelve el codigo de operacion por parametro que se debera
    // de interpretar desde fuera
    dwNotification = pEvent->dwNotificationOption;
    
	// Libera el evento
    m_pPerformance->FreePMsg((DMUS_PMSG*) pEvent);

    // Ha habido evento / notificacion
    return true;
  }

  // No hay notificacion
  return false;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Detiene todos los segmentos que este sonando y ademas libera los
//   que se encuentren en la cola.
// Parametros:
// Devuelve:
// - true: Todo ha ido correctamente.
// - false: Ha existido algun tipo de problema.
// Notas:
// - Por DXASSERT ha de
////////////////////////////////////////////////////////////////////// 
bool DXDMManager::StopAllSegments(void)
{
  // SOLO si se ha creado el performance
  DXASSERT(m_pPerformance != NULL);  
  
  // Detiene el segmento que este sonando y libera los que esten
  // en cola esperando.
  m_CodeResult = m_pPerformance->Stop(NULL, NULL, 0, 0);
  if (FAILED(m_CodeResult)) {
    // Han habido problemas
    DXDMError::DXDMErrorToString(m_CodeResult, m_sLastCode);   
    return false;
  }

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea el objeto IDirectMusicLoader y establece como directorio
//   de trabajo, el directorio actual.
// Parametros:
// Devuelve:
// - true: Si todo ha ido bien
// - false: Si ha existido algun problema.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDMManager::CreateLoader(void)
{
  // SOLO si se ha creado objeto IDirectMusicPerformance
  DXASSERT(m_pPerformance != NULL);

  char  szDir[_MAX_PATH];  // Directorio actual en ByteCode
  WCHAR wszDir[_MAX_PATH]; // Directorio actual en UniCode

  // Se crea Loader
  m_CodeResult = CoCreateInstance(CLSID_DirectMusicLoader,
                                  NULL,
                                  CLSCTX_INPROC,
                                  IID_IDirectMusicLoader,
                                  (void **)&m_pLoader);
  if (FAILED(m_CodeResult)) {
    // Han existido problemas
    DXDMError::DXDMErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Obtiene directorio de trabajo en byte code y lo pasa a unicode
  _getcwd(szDir, _MAX_PATH);
  MULTI_TO_WIDE(wszDir, szDir);

  // Establece el directorio de busqueda como el actual
  m_CodeResult = m_pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes,
                                               wszDir,
                                               FALSE);
  if (FAILED(m_CodeResult)) {
    // Hay existido problemas
    DXDMError::DXDMErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el directorio de trabajo para IDirectMusicLoader
// Parametros:
// - oDirectory: string con el directorio de trabajo.
// Devuelve:
// - true: Todo ha ido bien.
// - false: Ha existido algun problema
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDMManager::SetLoaderDirectory(const std::string& oDirectory)
{
   // SOLO si se ha creado objeto IDirectMusicPerformance
  DXASSERT(m_pPerformance != NULL);

  WCHAR  wszDir[_MAX_PATH]; // Directorio actual en UniCode

  // Pasa el directorio de trabajo a Uni Code
  MULTI_TO_WIDE(wszDir, oDirectory.c_str());

  // Establece el directorio de busqueda como el actual
  m_CodeResult = m_pLoader->SetSearchDirectory(GUID_DirectMusicAllTypes,
                                               wszDir,
                                               FALSE);
  if (FAILED(m_CodeResult)) {
    // Hay existido problemas
    DXDMError::DXDMErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Todo correcto
  return true;
}
 