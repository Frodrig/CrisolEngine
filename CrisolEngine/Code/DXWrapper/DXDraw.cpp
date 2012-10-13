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
// DXDraw.cpp
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXDraw.h
///////////////////////////////////////////////////////////////////////
#include "DXDraw.h"

#include "DXDDError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Constructor
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
DXDraw::DXDraw(void)
{
  // Inicializa vbles de miembro
  Init();
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destructor
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
DXDraw::~DXDraw(void)
{
  // Libera punteros y objetos
  Clean();  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa vbles de miembro.
// Parametros:
// Devuelve:
// Notas:
// - Este metodo es llamado desde el constructor y siempre que se
//   quiere inicializar la clase despues de haber hecho un Clean
////////////////////////////////////////////////////////////////////// 
void DXDraw::Init(void)
{
  // Inicializacion de vbles de miembro
  m_pDDraw = NULL;  
  m_nMode = DXDDDefs::FULLSCREEN;
  m_hWnd = NULL;
  m_dwHeight = m_dwWidth = m_dwBpp = 0;
  m_nActDevice = -1;
  m_Devices.clear();
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera punteros
// Parametros:
// Devuelve:
// Notas:
// - Metodo llamado desde el destructor. Siempre hace falta llamarlo
//   cuando se quiera reinicializar la clase. Despues, habra que 
//   llamar a Init.
////////////////////////////////////////////////////////////////////// 
void DXDraw::Clean(void)
{
  // Libera objeto DirectDraw si procede
  if (m_pDDraw)
  {
    m_pDDraw->Release();
    m_pDDraw = NULL;    
  }  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Enumera todos los dispositivos de video que existan en el equipo.
// Parametros:
// Devuelve:
// - true si no ha existido ningun problema
// - false si ha habido algun problema
// Notas:
// - Este metodo llama a DirectDrawEnumerate que a su vez, ejecutara
//   la funcion callback EnumerateCallback, definida como un metodo
//   estatico y privado. La funcion callback ira alojando en el
//   vector m_Devices los dispositivos encontrados. Siempre que
//   se encuentre uno, se invocara a la funcion callback.
////////////////////////////////////////////////////////////////////// 
bool DXDraw::FindDDrawDevices(void)
{
  // Procede a enumerar dispositivos <tanto primarios como secundarios>
  m_CodeResult = DirectDrawEnumerate(DXDraw::DDEnumCallback, &m_Devices);	
  if (FAILED(m_CodeResult)) {
    // Hay problemas
	DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Esta es la funcion callback que es llamada cada vez que se
//   halla un dispositivo, dentro del metodo FindDDrawDevices.
//   Internamente, lo unico que hace es añadir la informacion
//   relativa al dispositivo encontrado, en un vector interno que
//   los registrara a todos.
// Parametros:
// - lpGUID: Guid encontrado
// - lpDeviceDesc: Descripcion del device
// - lpDeviceName: Nombre del device
// - lpUser: Es un puntero void. Por defecto, se pasara la direccion
//   del vector que almacenara todos los dispositivos.
// - hMonitor: Tiene sentido cuando se trabaje con 2 monitores. Por
//   defecto se supondra que se trabaja con un solo monitor.
// Devuelve:
// - TRUE si se desea continuar con la enumeracion. 
// - FALSE si no se desea continuar.
// Notas:
// - Por defecto, este metodo siempre devolvera el indicativo de que
//   se siga enumerando.
////////////////////////////////////////////////////////////////////// 
BOOL WINAPI DXDraw::DDEnumCallback(GUID FAR* lpGUID, LPSTR lpDeviceDesc, 
                                   LPSTR lpDeviceName, LPVOID lpUser)
{
  // Se crea una nueva entrada para el vector
  DeviceInfo    NewDevice;

  // Se inserta la informacion sobre el GUID
  if (NULL == lpGUID) {
    // En caso de ser el device por defecto, se rellena todo de 0
    memset(&NewDevice.DeviceGUID, 0, sizeof(GUID));
  }
  else {
    // En caso contrario, se toma el GUID
    NewDevice.DeviceGUID = *lpGUID;
  }

  // Informacion relativa al nombre y descripcion
  NewDevice.sDeviceDesc = lpDeviceDesc;
  NewDevice.sDeviceName = lpDeviceName;

  // Se toma el puntero lpUser y se hace un cast a vector<DeviceInfo>
  // Despues se inserta la informacion y se desreferencia el puntero
  // auxiliar para evitar llamada al destructor
  std::vector<DeviceInfo>*  Devices = (std::vector<DeviceInfo> *) lpUser;  
  Devices->push_back(NewDevice);
  Devices = NULL;  

  // Se indica que continue la enumeracion
  return DDENUMRET_OK;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea el objeto DirectDraw. El dispositivo sobre el que se creara
//   sera el Device que en el vector m_Devices ocupe la posicion
//   indicada por bIDDevice. En este sentido, sera obligatorio haber
//   llamado antes a FindDDrawDevices. 
//   Una vez creado el objeto DirectDraw, se invocara a GetCaps para
//   almacenar las capacidades del HAL y el HEL.
// Parametros:
// - bIDDevice: Identificador del device sobre el que se desea crear
//   el objeto DirectDraw.
// Devuelve:
// - true: Todo se ha realizado correctamente.
// - false: Han existido problemas, no existe el device con el id
//   pasado o bien no se ha llamado aun a FindDDrawDevice.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDraw::Create(const byte bIDDevice)
{
  // Si aun no se ha llamado a FindDDrawDevice se retorna false  
  if (bIDDevice < 0) {
    m_sLastCode = DXDDRAW_CALLENUMERATEDEVICES_MSG;
    m_CodeResult = DD_OK;
    return false;
  }
  
  // Si el ID del Device no se encuentra se retorna false
  if (bIDDevice > m_Devices.size() - 1) {
    m_sLastCode = DXDDRAW_DEVICENOFOUND_MSG;
    m_CodeResult = DD_OK;
    return false;
  }
  
  // Si existe objeto DirectDraw lo libera
  if (m_pDDraw != NULL) {
    Clean();
    Init();
  }    
  
  // Crea objeto DirectDraw asociado al dispositivo recibido como
  // parametro. Se utilizara DirectDrawCreateEx para aprovechar
  // posibles caracteristicas futuras de Direct3D.
  m_CodeResult = DirectDrawCreateEx(&m_Devices[bIDDevice].DeviceGUID, 
                                    (void**) &m_pDDraw,	
								                    IID_IDirectDraw7, 
                                    NULL );
	
  if (FAILED(m_CodeResult)) {
    // Hay problemas
	DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Ahora se establecen las capacidades del objeto DirectDraw creado
  memset(&m_HALCaps, 0, sizeof(m_HALCaps));  
  memset(&m_HELCaps, 0, sizeof(m_HELCaps));  
  m_HALCaps.dwSize = sizeof(m_HALCaps);
  m_HELCaps.dwSize = sizeof(m_HELCaps);
  m_CodeResult = m_pDDraw->GetCaps(&m_HALCaps, &m_HELCaps);
  if (FAILED(m_CodeResult)) {
    // Hay problemas    
	DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
  
  // Actualiza vbles de miembro
  m_nActDevice = bIDDevice; // Numero del device actual <en m_Devices>
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece modo de funcionamiento de DirectDraw (modo cooperativo).
//   Es decir, en pantalla completa o en ventana.
// Parametros:
// - hWnd: Instancia a la ventana en donde asociar DirectDraw.
// - nMode: Modo cooperativo. Sus valores pueden ser:
//   * FULLSCREEN: Modo a pantalla completa y exclusivo.
//   * WINDOW: Modo ventana.
//   Por defecto se establece el modo FULLSCREEN.
// Devuelve:
// - true: Todo correcto.
// - false: Ha existido algun problema.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDraw::SetCooperativeMode(const HWND& hWnd, 
                                const DXDDDefs::DXDDCooperativeType& nMode)
{  
  // SOLO si existe objeto DirectDraw
  DXASSERT(m_pDDraw != NULL);

  // Vbles
  DWORD   dwFlags; // Flags para el modo cooperativo
  
  // Dependiendo del modo en que se desee establecer la
  // cooperatividad, asi se estableceran los flags necesarios
  switch(nMode)
  {    
    case DXDDDefs::FULLSCREEN: 
      // Modo exclusivo a pantalla completa
      dwFlags = DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_FPUSETUP;  
      break;    
    case DXDDDefs::WINDOW: 
      // Modo ventana
      dwFlags = DDSCL_NORMAL;        
      break;
   }
      
  // Establece el modo cooperativo 
  m_CodeResult = m_pDDraw->SetCooperativeLevel(hWnd, dwFlags);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
	DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
    return false;   
  }

  // Actualiza vbles de miembro
  m_nMode = nMode; // Modo cooperativo actual
      
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece las caracteristicas de visualizacion, esto es, el modo
//   de video.
// Parametros:
// - dwWidth: Anchura del dispositivo.
// - dwHeight: Altura del dispositivo.
// - dwBpp: Bits por pixel
// - dwFrec: Frecuencia de refresco. 0 es el valor por defecto.
// - dwFlags: Flags. 0 es el valor por defecto y no se debe poner otro
//   a no ser que se quiera trabajar en modo 13.
// Devuelve:
// - true: Todo correcto.
// - false: Ha existido algun tipo de problema.
// Notas:
// - No es necesario preocuparse de establecer los valores de dwFrec y
//   dwFlags, ya que poseen por defecto los valores que interesaran
//   para la practiva totalidad de los casos de uso.
// - Si se esta en modo ventana NO se debe de llamar a este metodo.
////////////////////////////////////////////////////////////////////// 
bool DXDraw::SetDisplayMode(const dword dwWidth, 
							const dword dwHeight, 
                            const dword dwBpp, 
							const dword dwFrec, 
                            const dword dwFlags)
{
  // SOLO si hay objeto DirectDraw
  DXASSERT(m_pDDraw != NULL);
  // SOLO si NO se esta en modo ventana
  DXASSERT(m_nMode != DXDDDefs::WINDOW);

  // Establece el modo de video
  m_CodeResult = m_pDDraw->SetDisplayMode(dwWidth, dwHeight, dwBpp, 
                                          dwFrec, dwFlags);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Actualiza vbles de miembro  
  m_dwWidth = dwWidth;   // Anchura
  m_dwHeight = dwHeight; // Altura
  m_dwBpp = dwBpp;       // Bits por pixel

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Todas las superficies asociadas al objeto DirectDraw son 
//   restauradas automaticamente.
// Parametros:
// Devuelve:
// - true: Todo ha ido bien.
// - false: Ha existido algun problema.
// Notas:
// - Este metodo evita tener que ir recorriendo surface a surface
//   invocando el metodo de restauracion de surfaces.
////////////////////////////////////////////////////////////////////// 
bool DXDraw::RestoreAllSurfaces(void)
{
  // SOLO si hay objeto DirectDraw
  DXASSERT(m_pDDraw != NULL);
  
  // Restatura todas las surfaces
  m_CodeResult = m_pDDraw->RestoreAllSurfaces();
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);    
    return false;
  }

  // Todo correcto
  return true;
}
