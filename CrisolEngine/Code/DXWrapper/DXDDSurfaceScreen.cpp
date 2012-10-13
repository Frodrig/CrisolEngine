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
// DXDDSurfaceScreen.cpp
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXDDSurfaceScreen.h
///////////////////////////////////////////////////////////////////////
#include "DXDDSurfaceScreen.h"

#include "DXDDSurfaceBack.h"
#include "DXDDPalette.h"
#include "DXDDError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Constructor
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
DXDDSurfaceScreen::DXDDSurfaceScreen(void)
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
void DXDDSurfaceScreen::Init(void)
{
  // Inicializa vbles  
  m_lpGammaControl = NULL;
  memset(&m_GammaRamp, 0, sizeof(m_GammaRamp));
  m_wFadeSpeed = 100;
  m_ucFadeWait = 1;
  m_bPalette = false;
  m_poBackBuffer = NULL;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destructor
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
DXDDSurfaceScreen::~DXDDSurfaceScreen(void)
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
// Las superficies de tipo COMPLEX, al liberarse, liberan tambien la
// superfice ATTACHED (back) que tuvieran asociada.
////////////////////////////////////////////////////////////////////// 
void DXDDSurfaceScreen::Clean(void)
{
  // Libera superficie si esta creada
  if (NULL != m_pDDSurface) { 
    // Se libera si procede el control del gamma
    if (NULL != m_lpGammaControl) {
      m_lpGammaControl->Release();
      m_lpGammaControl = NULL;
    }

    // Libera la superficie y anula referencia posibles a BackBuffer.
    m_poBackBuffer = NULL; 
    m_pDDSurface->Release();
    m_pDDSurface = NULL;
    m_SurfaceType = DXDDDefs::SURFACETYPE_NOCREATED;
  }
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea una superficie primaria simple.
// Parametros:
// - poDXDraw: Puntero a instancia DXDraw.
// - wMem: Memoria en donde crear la superficie primaria
// - bD3DSurface: True si la superficie se va a utilizar con un device
//   3D y false en caso contrario. Por defecto vale false.
// Devuelve:
// - true: Todo ha ido correctamente.
// - false: Ha ocurrido algun error.
// Notas:
// - Este metodo crea una superficie primaria simple. Una superficie
//   primaria simple NO puede tener asociados BackBuffer, pero si
//   puede realizar la operacion de Flip. 
// - Este tipo de superficies primarias NO son recomendadas 
//   para el trabajo a ventana completa
// - Este tipo de superficies primarias son las UNICAS que se pueden
//   utilizar en modo ventana.
////////////////////////////////////////////////////////////////////// 
bool DXDDSurfaceScreen::CreatePrimary(const DXDraw* const poDXDraw, 
                                      const DXDDDefs::DXDDMemType& wMem, 
                                      const bool bD3DSurface)
{
  // SOLO si hay instancia a DXDraw
  DXASSERT(NULL != poDXDraw);
  // SOLO si la superficie actual no es ATTACH
  DXASSERT(DXDDDefs::SURFACETYPE_ATTACHED != m_SurfaceType);

  DDSURFACEDESC2  desc;      // Estructura de definicion de surface
  DWORD           dwFlagMem; // Flag con el tipo de memoria
  
  // Inicializaciones
  memset(&desc, 0, sizeof(desc));
  desc.dwSize = sizeof(desc);  
    
  // Si la surface esta ocupada la libera
  if (m_pDDSurface != NULL) {
    Clean();
    Init();
  }    
  
  // Establece flags dependiendo del tipo de memoria
  switch(wMem)
  {
    case DXDDDefs::VIDEOMEM: // Memoria de video
      dwFlagMem = DDSCAPS_VIDEOMEMORY;
      break;
    case DXDDDefs::SYSTEMMEM: // Memoria de sistema
      dwFlagMem = DDSCAPS_SYSTEMMEMORY;
      break;    
   }
    
  // Establece la descripcion para una superficie 
  // primaria simple.
  desc.dwFlags = DDSD_CAPS;
  desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | dwFlagMem;
  // ¿Se va a utilizar con un device 3D?
  if (bD3DSurface) { desc.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE; }  

  // Procede a crear la superficie
  m_CodeResult = poDXDraw->GetObj()->CreateSurface(&desc, 
                                                   &m_pDDSurface, 
                                                   NULL);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
  
  // Obtiene datos sobre la superficie creada  
  m_dwWidth = poDXDraw->GetDisplayWidth();    
  m_dwHeight = poDXDraw->GetDisplayHeight();  
  m_SurfaceType = DXDDDefs::SURFACETYPE_PRIMARY;
  SetPixelFormat();         
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea una superficie primaria compleja.
// Parametros:
// - poDXDraw: Puntero a instancia DXDraw.
// - ubNumBackBuffers. Numero de buffers secundarios.
// - wMem: Memoria en donde crear la superficie primaria
// - bD3DSurface. ¿Surface comptatible con D3D?
// Devuelve:
// - true: Todo ha ido correctamente.
// - false: Ha ocurrido algun error.
// Notas:
// - Este tipo de superficies son las mas comunes a utilizar. Para
//   completar la construccion de la misma, se debera de llamar a 
//   continuacion al metodo AttachToComplex, que asocia un backbuffer
//   a la superficie primaria compleja.
// - Desde este tipo de superficie, se realizaran las operaciones
//   Flip. Desde el backbuffer asociado, se realizaran las operaciones
//   Blt.
// - Solo existira un backbuffer asociado a la superficie primaria
//   compleja. Independientemente de si es de doble buffer o de 
//   triple buffer, o mayor.
// - NO se permitira este tipo de superficies en modo ventana.
// - En caso de que ya haya creada una superficie, que no sea de tipo
//   ATTACH, la liberara.
////////////////////////////////////////////////////////////////////// 
bool DXDDSurfaceScreen::CreateComplex(const DXDraw* const poDXDraw, 
                                      const byte ubNumBackBuffers,
                                      const DXDDDefs::DXDDMemType& wMem,
                                      const bool bD3DSurface)
{
  // SOLO si hay instancia a DXDraw
  DXASSERT(NULL != poDXDraw);
  // SOLO si la superficie actual no es ATTACH
  DXASSERT(DXDDDefs::SURFACETYPE_ATTACHED != m_SurfaceType);

  if (DXDDDefs::WINDOW == poDXDraw->GetMode()) {
    // No se permite este tipo de superficies en modo ventana
    m_CodeResult = DD_OK;
    return false;
  }

  DDSURFACEDESC2  desc;      // Estructura de definicion de surface
  DWORD           dwFlagMem; // Flag con el tipo de memoria
  
  // Inicializaciones
  memset(&desc, 0, sizeof(desc));
  desc.dwSize = sizeof(desc);  
    
  // Si la surface esta ocupada la libera
  if (m_pDDSurface != NULL) {
    Clean();
    Init();
  }  
  
  // Establece flags dependiendo del tipo de memoria
  switch(wMem)
  {
    case DXDDDefs::VIDEOMEM: // Memoria de video
      dwFlagMem = DDSCAPS_VIDEOMEMORY;
      break;
    case DXDDDefs::SYSTEMMEM: // Memoria de sistema
      dwFlagMem = DDSCAPS_SYSTEMMEMORY;
      break;    
   }

  // Establece la descripcion para una superficie 
  //  primaria compleja.
  desc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
  desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
                        DDSCAPS_COMPLEX | 
                        DDSCAPS_FLIP | 
                        dwFlagMem;
  // ¿Se va a utilizar con un device 3D?
  if (bD3DSurface) { desc.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE; }  
  desc.dwBackBufferCount = ubNumBackBuffers;
  //desc.ddsCaps.dwCaps2 = DDCAPS2_FLIPNOVSYNC;
  // Procede a crear la superficie
  m_CodeResult = poDXDraw->GetObj()->CreateSurface(&desc,
                                                   &m_pDDSurface,
                                                   NULL);  
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);    
    return false;
  }
  
  // Obtiene datos sobre la superficie creada  
  m_dwWidth = poDXDraw->GetDisplayWidth();    
  m_dwHeight = poDXDraw->GetDisplayHeight();  
  SetPixelFormat();                           
  m_SurfaceType = DXDDDefs::SURFACETYPE_COMPLEX;
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia una superficie invisible (back) a la superficie primaria
//   de tipo complejo creada.
// Parametros:
// - poAttachSurface: Superficie ATTACHED a asociar a la superficie 
//   primaria compleja.
// Devuelve:
// - true: Todo ha ido bien.
// - false: Han existido problemas.
// Notas:
// - Si ya hay una superficie back asociada, no se tendra de en
//   cuenta. Esto es asi, dado que hay operaciones que trabajan
//   con varias superficies ATTACH asociadas.
// - Se puede crear manualmente una superficie compleja utilizando
//   CreateComplex y despues este metodo.
// - La superficie rebida por parametro pasa a ser del tipo ATTACH y
//   sus atributos son adecuadamente cambiados en este metodo.
// - Se puede evitar el uso de CreateComplex y AttachToComplex, 
//   utilizando la version de CreateComplex que recibe los mismos
//   parametros que los dos primeros metodos citados.
////////////////////////////////////////////////////////////////////// 
bool DXDDSurfaceScreen::AttachToComplex(DXDDSurfaceBack* const poAttachSurface)
{
  // SOLO si hay superficie compleja creada
  DXASSERT (NULL != m_pDDSurface);
  DXASSERT (DXDDDefs::SURFACETYPE_COMPLEX == m_SurfaceType);
  // SOLO si existe puntero a la supercie pasada
  DXASSERT(NULL != poAttachSurface);
  // SOLO si superficie pasada no esta creada
  DXASSERT(DXDDDefs::SURFACETYPE_NOCREATED == poAttachSurface->GetType());
  
  DDSCAPS2    ddscaps; // Descripcion de superficie
  
  // Inicializaciones
  memset(&ddscaps, 0, sizeof(ddscaps));  

  // Establece descripcion
  ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

  // Asocia superficie pasada a primaria compleja
  m_CodeResult = m_pDDSurface->GetAttachedSurface(&ddscaps,
                                                  &poAttachSurface->m_pDDSurface);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }    
  
  // Actualiza valores de superficie ATTACH pasada y asociada
  poAttachSurface->m_dwWidth = m_dwWidth;                 
  poAttachSurface->m_dwHeight = m_dwHeight;               
  SetPixelFormat();                                       
  poAttachSurface->m_SurfaceType = DXDDDefs::SURFACETYPE_ATTACHED; 
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea una superficie compleja y le asocia una superficie para que
//   sea backbuffer. En caso de que todo vaya bien, se intentara
//   crear un objeto para controla el gamma.
// Parametros:
// - poDXDraw: Instancia a clase DXDraw con objeto DirectDraw.
// - ubNumBackBuffers: Numero de buffers en superficie compleja.
// - poBackBuffer: Puntero a superfice que hara de backbuffer y sera
//   de tipo ATTACHED, acabando asociada a la compleja creada.
// - wMem: Tipo de memoria en donde crear la superficie compleja.
// - bD3DSurface: True si la superficie se va a utilizar con un device
//   3D y false en caso contrario. Por defecto vale false.
// Devuelve:
// - true: Todo fue bien.
// - false: Existieron problemas
// Notas:
// - Este metodo es ideal para crear, rapidamente, un sistema de
//   intercambio con superficie compleja. Simplemente realiza las
//   dos llamadas que hay que efectuar por separado, aqui dentro.
// - En caso de que ya haya creada una superficie, la liberara.
////////////////////////////////////////////////////////////////////// 
bool DXDDSurfaceScreen::CreateComplex(const DXDraw* const poDXDraw, 
                                      const byte ubNumBackBuffers,
                                      DXDDSurfaceBack* const poBackBuffer, 
                                      const DXDDDefs::DXDDMemType& wMem,
                                      const bool bD3DSurface)
{
  // SOLO si existe puntero a la supercie pasada
  DXASSERT(NULL != poBackBuffer);
  // SOLO si superficie pasada no esta creada
  DXASSERT(DXDDDefs::SURFACETYPE_NOCREATED == poBackBuffer->GetType());
 
  if (CreateComplex(poDXDraw, ubNumBackBuffers, wMem, bD3DSurface)) {
    // Se ha creado la superficie compleja
    if(AttachToComplex(poBackBuffer)) {
      // Se logro asociar el backbuffer
      
      // Se guarda la referencia al BackBuffer
      m_poBackBuffer = poBackBuffer; 
            
      // Todo correcto
      return true;
    }
  }
  
  // Existieron problemas
  return false;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia una paleta a la superficie, siempre y cuando esta sea de
//   8 Bpp.
// Parametros:
// - poDXPalette: Instancia valida a clase DXDDPalette.
// Devuelve:
// - true: Todo ha ido bien.
// - false: Ha existido algun tipo de problema.
// Notas:
// - Recordar que es imprescindible que que la superficie sea de 8 Bpp.
// - Si se quiere quitar una paleta, basta con llamar a UnSetPalette.
// - Se pueden asociar multiples paletas a una superficie. El efecto
//   sera quitar la antigua y poner la nueva. De inmediato, se veran
//   los cambios en la informacion grafica de la superficie.
////////////////////////////////////////////////////////////////////// 
bool DXDDSurfaceScreen::SetPalette(DXDDPalette* const poDXPalette)
{
  // SOLO si hay superficie creada
  DXASSERT(m_pDDSurface != NULL);  
  // SOLO si poDXPalette es una instancia valida
  DXASSERT(poDXPalette != NULL);
  // SOLO si la superficie es de mas de 8 Bpp
  DXASSERT(GetBpp() > 8);

  // Procede a asociar la paleta
  m_CodeResult = m_pDDSurface->SetPalette(poDXPalette->GetObj());
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Actualiza flags
  m_bPalette = true;
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina cualquier referencia posible a paleta en la superficie
// Parametros:
// Devuelve:
// - true: Todo ha ido bien.
// - false: Ha existido algun tipo de problema.
// Notas:
// - Solo valido si la superficie es de mas de 8 Bpp
////////////////////////////////////////////////////////////////////// 
bool DXDDSurfaceScreen::UnSetPalette(void)
{
  // SOLO si hay superficie creada
  DXASSERT(m_pDDSurface != NULL);  
  // SOLO si la superficie es de mas de 8 Bpp
  DXASSERT(GetBpp() > 8);
  
  // Si no tenia asociada paleta, retorna sin mas
  if (!m_bPalette) { return true; }

  // Procede a asociar la paleta
  m_CodeResult = m_pDDSurface->SetPalette(NULL);
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
// - Realiza la operacion de intercambio de paginas en una superficie
//   primaria compleja (COMPLEX).
//   En caso de que se este trabajando con una superficie PRIMARY
//   que posea un backbuffer falso asociado, la operacion consistira
//   en copiar el contenido del backbuffer sobre la superficie primaria
//   mediante un simple bliteado.
// Parametros:
// - dwFlags: Flag con los que hacer el intercambio de paginas. Por
//   defecto es DDFLIP_WAIT. Consultar ayuda de DirectX para mas 
//   informacion.
// Devuelve:
// - true: Todo ha ido bien.
// - false: Ha existido algun tipo de problema.
// Notas:
// - Es imprescindible que este metodo lo llame la superficie
//   primaria (pero siempre y cuando, esta superficie primera tenga
//   un BackBuffer asociado, es decir, que sea una sup. compleja).
////////////////////////////////////////////////////////////////////// 
bool DXDDSurfaceScreen::Flip(const dword dwFlags)
{
  // SOLO si la superficie es COMPLEX
  DXASSERT(DXDDDefs::SURFACETYPE_COMPLEX == m_SurfaceType);
  
  // Se realiza el flip
  #ifdef _DEBUG
	m_CodeResult = m_pDDSurface->Flip(NULL, dwFlags);  
	if (FAILED(m_CodeResult)) {
	  // Hay problemas
	  DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
	  return false;
	} 
  #else
	m_pDDSurface->Flip(NULL, dwFlags);
  #endif
  
  //m_pDDSurface->BltFast(0, 0, m_poBackBuffer->GetObj(), NULL, DDBLTFAST_WAIT);
  //  Todo correcto
  return true;
}