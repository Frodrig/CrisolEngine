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
// DXDDSurfaceBack.cpp
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXDDSurfaceBack.h
///////////////////////////////////////////////////////////////////////
#include "DXDDSurfaceBack.h"

#include "DXDraw.h"
#include "DXDDError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Constructor
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
DXDDSurfaceBack::DXDDSurfaceBack()
{
  // Inicializa vbles de miembro
  Init();
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa variables de miembro
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
void DXDDSurfaceBack::Init(void)
{
  // Inicializa vbles  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destructor
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
DXDDSurfaceBack::~DXDDSurfaceBack()
{
  // Se liberan recursos
  Clean();  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera objeto DirectDrawSurface
// Parametros:
// Devuelve:
// Notas:
// - Las superficies de tipo ATTACHED no pueden invocar a Release, pues
//   incurririan en un error. Simplemente se desreferencian ya que la
//   COMPLEX asociada a ellas, las liberara al hacer Release.
////////////////////////////////////////////////////////////////////// 
void DXDDSurfaceBack::Clean(void)
{
  // Libera superficie si esta creada
  if (NULL != m_pDDSurface)
  {
    if (DXDDDefs::SURFACETYPE_ATTACHED != m_SurfaceType)
    {// Si es ATTACHED se liberara en la COMPLEX a la que pertenece
      m_pDDSurface->Release();
    }      
    m_pDDSurface = NULL;
    m_SurfaceType = DXDDDefs::SURFACETYPE_NOCREATED;
  }
}
  
//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea una superficie no visible, BACK, de las dimensiones
//   especificadas por parametro.
// Parametros:
// - poDXDraw: Instancia DXDraw.
// - dwWidth, dwHeight: Anchura y altura respectivamente.
// - wMem: Memoria en donde crear la superficie.
// Devuelve:
// - true: todo ha ido bien.
// - false: Ha existido algun problema.
// Notas:
// - Este tipo de superficies estan destinadas a almacenar informacion
//   grafica o bien a construir pantallas.
// - En caso de que ya haya creada una superficie, que no sea de tipo
//   ATTACH, la liberara.
////////////////////////////////////////////////////////////////////// 
bool DXDDSurfaceBack::Create(const DXDraw* const poDXDraw, 
							 const dword dwWidth, 
                             const dword dwHeight, 
							 const DXDDDefs::DXDDMemType& wMem)
{
  // SOLO si hay instancia a DXDraw
  DXASSERT(NULL != poDXDraw);
  // SOLO si la superficie actual no es ATTACH
  DXASSERT(DXDDDefs::SURFACETYPE_ATTACHED != m_SurfaceType);      
  
  DDSURFACEDESC2  desc;      // Descripcion de la superficie
  DWORD           dwFlagMem; // Flag con el tipo de memoria a utilizar

  // Inicializaciones
  memset(&desc, 0, sizeof(desc));  

  // Si la surface esta ocupada la libera
  if (m_pDDSurface != NULL) {
    Clean();
    Init();
  }    
  
  // Procede a rellenar los campos de la estructura
  desc.dwSize = sizeof(desc);
  desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;      
  switch(wMem)
  {// ¿Memoria en donde crear la superficie?
    case DXDDDefs::VIDEOMEM: // Memoria de video
      dwFlagMem = DDSCAPS_VIDEOMEMORY;
      break;
    case DXDDDefs::SYSTEMMEM: // Memoria de sistema
      dwFlagMem = DDSCAPS_SYSTEMMEMORY;
      break;    
   }    
  desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | dwFlagMem;
  desc.dwWidth = dwWidth;
  desc.dwHeight = dwHeight;
  
  // Procede a crear la superficie
  m_CodeResult = poDXDraw->GetObj()->CreateSurface(&desc, 
                                                   &m_pDDSurface, 
                                                   NULL);
  if (FAILED(m_CodeResult))
  {// Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
    
  // Establece los datos de la superficie creada  
  m_dwWidth = dwWidth;                    
  m_dwHeight = dwHeight;                  
  SetPixelFormat();                       
  m_SurfaceType = DXDDDefs::SURFACETYPE_BACK; 
 
  // Todo correcto
  return true;
}

