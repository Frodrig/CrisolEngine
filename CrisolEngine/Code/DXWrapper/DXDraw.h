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
// DXDraw.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Clase que encapsula el objeto DirectDraw y permite realizar todas
//   las operaciones necesarias sobre el. Su finalidad es la de servir 
//   de punto de apoyo al resto de clases que componen el wrapper 
//   de DirectDraw y Direct3D.
//     
// Notas:
// - Clase basada en DirectX 7.0.
// - Para su correcta compilacion hara falta incluir la libreria
//   "DDraw.lib" y el archivo de cabecera "DDraw.h". Este recurso se
//   encentran incluidos en el archivo "DXDDDefs.h".
// - Todas las clases DX deben de heredar de la clase base DXWrapper 
//   e implementar los metodos Init y Clean. 
// - Todas las estructuras, constantes, tipos enumerados, etc que
//   utilice la libreria de clases para DirectDraw estaran en el
//   archivo de cabecera "DXDDDefs.h".
// - Para poder trabajar con la funcion que transforma codigos de error
//   de DirectDraw a cadenas de caracteres se incluye "DXDDError.h".
///////////////////////////////////////////////////////////////////////
#ifndef __DXDRAW_H_
#define __DXDRAW_H_

// Includes
#include "DXWrapper.h"
#include "DXDDDefs.h"
#include <vector>

// Acceso directo a los namespaces de definiciones y error

// Clase DXDraw
class DXDraw: public DXWrapper
{
private:
  // Estructuras   
  struct DeviceInfo
  { // Esta estructura servira para definir un dispositivo de los
    // hallados en el periodo de enumeracion
    std::string    sDeviceName;  // Nombre del device
    std::string    sDeviceDesc;  // Descripcion del device
    GUID           DeviceGUID;   // GUID del dispositivo
  };

protected:
  // Variables de miembro
  LPDIRECTDRAW7					m_pDDraw;   // Objeto DirectDraw  
  DDCAPS						m_HALCaps;  // Capacidades del driver creado
  DDCAPS						m_HELCaps;  // Capacidades del HEL <software>
  DXDDDefs::DXDDCooperativeType m_nMode;    // Modo de ejecucion (ventana o exclusivo)
  HWND							m_hWnd;     // Handle a la ventana
  DWORD							m_dwHeight; // Resolucion vertical
  DWORD							m_dwWidth;  // Resolucion horizontal
  DWORD							m_dwBpp;    // Bits por pixel
  
private:
  // Variables de miembro
  std::vector<DeviceInfo>  m_Devices;     // Vector con todos los devices hallados
  int                      m_nActDevice;  // ID del Device actual (-1 = no hay)

public:
  // Constructor y destructor
  DXDraw(void);
  ~DXDraw(void);

public:
  // Operaciones relacionadas con la enumeracion de dispositivos  
  bool FindDDrawDevices(void);
  static BOOL WINAPI DDEnumCallback(GUID FAR* lpGUID, 
									LPSTR lpDeviceDesc, 
                                    LPSTR lpDeviceName, 
									LPVOID lpUser);

public:
  // Operaciones de creacion e inicializacion del objeto DirectDraw
  bool Create(const byte bIDDevice);
  void Destroy(void) { Clean(); Init(); }
  bool SetCooperativeMode(const HWND& hWnd, 
                          const DXDDDefs::DXDDCooperativeType& nMode = DXDDDefs::FULLSCREEN);
  bool SetDisplayMode(const dword dwWidth, 
					  const dword dwHeight, 
					  const dword dwBpp, 
                      const dword dwFreq = 0, 
					  const dword dwFlags = 0);

public:
  // Operaciones sobre las capacidades del Device
  inline DDCAPS GetHALCaps(void) const { return m_HALCaps; }
  inline DDCAPS GetHELCaps(void) const { return m_HELCaps; }
  
public:
  // Operaciones generales
  bool RestoreAllSurfaces(void);  

public:
  // Operaciones de consulta / obtencion
  inline const LPDIRECTDRAW7 GetObj(void) const { return m_pDDraw; } 
  inline int GetNumDevices(void) const { return m_Devices.size(); }
  inline int GetActDevice(void) const { return m_nActDevice; }
  inline GUID GetDeviceGuid(BYTE ucIDDevice) const { return m_Devices[ucIDDevice].DeviceGUID; }
  inline std::string GetDeviceName(const byte ucIDDevice) const { return m_Devices[ucIDDevice].sDeviceName; }
  inline std::string GetDeviceDesc(const byte ucIDDevice) const { return m_Devices[ucIDDevice].sDeviceDesc; }
  inline DXDDDefs::DXDDCooperativeType GetMode(void) const { return m_nMode; }
  inline DWORD GetDisplayWidth(void) const { return m_dwWidth; }
  inline DWORD GetDisplayHeight(void) const { return m_dwHeight; }
  inline DWORD GetDisplayBpp(void) const { return m_dwBpp; }
  
protected:
  // Metodos de apoyo
  void Init(void);
  void Clean(void);
}; 
#endif