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
// DXDMManager.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Esta clase encapsula el objeto IDirectMusicPerformance, encargado
//   de manejar los segmentos que contendran los sonidos, y el objeto
//   IDirectMusicLoader, encargado de cargar y buscar los ficheros de
//   trabajo.
//
// Notas:
// - Clase preparada para DirectX 7.0.
// - Este API utiliza al 100% COM por lo que no hay libreria linkada.
// - Todas las estructuras, constantes, tipos enumerados, etc que
//   utilice la libreria de clases para DirectMusic estaran en el
//   archivo de cabecera "DXDMDefs.h".
// - Para poder trabajar con la funcion que transforma codigos de error
//   de DirectMusic a cadenas de caracteres se incluye "DXDMError.h".
///////////////////////////////////////////////////////////////////////
#ifndef __DXDMMANAGER_H_
#define __DXDMMANAGER_H_

// Includes
#include "DXWrapper.h"
#include "DXDMDefs.h"
#include <string>

class DXSound;

// Clase DXDMPerformance
class DXDMManager: public DXWrapper
{
protected:
  // Variables de miembro
  IDirectMusicPerformance*  m_pPerformance;   // Objeto IDirectMusicPerformance
  IDirectMusicLoader*       m_pLoader;        // Objeto IDirectMusicLoader  
  HANDLE                    m_hNotify;        // Handle de notificaciones
  std::string               m_oWorkDirectory; // Directorio de trabajo

public:
  // Constructores y destructores
  DXDMManager(void);
  ~DXDMManager(void);

public:
  // Creacion / destruccion
  bool Create(const HWND& hWnd, 
			  DXSound* const pDXSound = NULL);  
  void Destroy(void) { Clean(); Init(); }
protected:
  // Metodos de apoyo
  bool CreateLoader(void);

public:
  // Trabajo con notificadores
  bool AddNotification(const GUID guidNotification);
  bool GetNotification(DWORD& dwNotification);  
  inline const HANDLE& GetHandleNotification(void) const { 
	return m_hNotify; 
  }

public:
  // Trabajo con segmentos
  bool StopAllSegments(void);

public:
  // Trabajo con el Loader
  bool SetLoaderDirectory(const std::string& oDirectory);

public:
  // Operaciones inline
  inline IDirectMusicPerformance* GetPerformanceObj(void) const { return m_pPerformance; }
  inline IDirectMusicLoader* GetLoaderObj(void) const { return m_pLoader; }

protected:
  // Inicializacion y liberacion de recursos <DXWrapper>  
  void Init(void);
  void Clean(void);
};
#endif
