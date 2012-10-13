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
// DXDDSurfaceTextureTexture.cpp
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXDDSurfaceTexture.h
///////////////////////////////////////////////////////////////////////
#include "DXDDSurfaceTexture.h"

#include "DXDraw.h"
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
DXDDSurfaceTexture::DXDDSurfaceTexture(void)
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
void DXDDSurfaceTexture::Init(void)
{
  // Inicializa vbles
  m_bHardware = false;
  m_bPow2 = false;
  m_bSquare = false;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destructor
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
DXDDSurfaceTexture::~DXDDSurfaceTexture(void)
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
////////////////////////////////////////////////////////////////////// 
void DXDDSurfaceTexture::Clean(void)
{
  // Libera superficie si esta creada
  if (NULL != m_pDDSurface) {
    m_pDDSurface->Release();
    m_pDDSurface = NULL;
    m_SurfaceType = DXDDDefs::SURFACETYPE_NOCREATED;
  }
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea una superficie de tipo Textura, preparada para trabajar en
//   Direct3D. La textura podra ser creada segun las especificaciones
//   relativas al numero de bits por pixel y bits reservados para
//   el canal alpha.
// Parametros:
// - pDXDraw: Instancia DXDraw.
// - pD3DDevice: Instancia al device 3D
// - dwWidth, dwHeight: Anchura y altura respectivamente.
// - pPixelFormat: Direccion de la estructura con el formato de pixel
//   deseado para la textura a crear
// - bHardware: Si vale true, la textura podra alojarse en mem.
//   de video (device creado sobre hardware).
// - bPow2: Si vale true, la textura tiene que ser potencia de 2.
// - bSquare: Si vale true la textura tiene que ser cuadrada.
// Devuelve:
// - true: todo ha ido bien.
// - false: Ha existido algun problema.
// Notas:
// - La creacion de una superificie de este tipo tiene que comprobar
//   el tipo de device o tarjeta de video sobre la que se va a
//   trabajar, pues algunas tarjetas de video requieren que las
//   texturas tengan una dimension que sea potencia de dos y otras que
//   sean completamente cuadradas.
// - En caso de que el device sobre el que se vaya a trabajar sea
//   de hardware, la textura sera manejada automaticamente por D3D y
//   alojada en la memoria de video. En caso contrario, si el device
//   esta instalado en software, la textura se alojara en memoria del
//   sistema (mas lenta).
// - Para poder obtener la direccion de la estructura con el formato de
//   pixel para crear la textura, asi como los valores para los
//   flags indicativos de si la textura tiene que ser potencia de 2
//   y/o cuadrada, consultar la clase DXD3DDevice.
////////////////////////////////////////////////////////////////////// 
bool DXDDSurfaceTexture::Create(const DXDraw* const pDXDraw, 
                                const dword dwWidth, 
								const dword dwHeight,
                                const DDPIXELFORMAT* const pPixelFormat, 
                                const bool bHardware, 
								const bool bPow2, 
                                const bool bSquare)
{
  // SOLO si la instancia a DXDraw es correta
  DXASSERT(NULL != pDXDraw);
  DXASSERT(NULL != pDXDraw->GetObj());
  // SOLO si la direccion a la estructura PIXELFORMAT es correcta
  DXASSERT(NULL != pPixelFormat);
  // SOLO si la superficie actual no es ATTACH ni COMPLEX
  DXASSERT(DXDDDefs::SURFACETYPE_ATTACHED != m_SurfaceType);
  DXASSERT(DXDDDefs::SURFACETYPE_COMPLEX != m_SurfaceType);
  
  // Variables
  DDSURFACEDESC2   desc; // Descripcion de la superficie
  
  // Inicializaciones
  memset(&desc, 0, sizeof(DDSURFACEDESC2));  
  
  // Si la surface esta ocupada la libera
  if (NULL != m_pDDSurface) {
    Clean();
    Init();
  }    
  
  // Procede a rellenar los campos de la estructura
  desc.dwSize = sizeof(desc);
  desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
  desc.ddsCaps.dwCaps = DDSCAPS_TEXTURE;
  desc.dwWidth = dwWidth;
  desc.dwHeight = dwHeight;
  desc.ddpfPixelFormat = *pPixelFormat;
  
  // Se escoge donde alojar la textura dependiendo del tipo
  // de device 3D creado.
  if (bHardware) { desc.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE; }
  else { desc.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY; }

  /*
  // ¿Las dimensiones tiene que ser potencia de 2?  
  if (bPow2) {
    // Se aumenta el valor de la anchura y la altura de 
    // forma exponencial (con base 2) y en relacion a una 
    // misma dimension (la anchura).
    for (desc.dwWidth = 1; dwWidth > desc.dwWidth; desc.dwWidth <<= 1);
    for (desc.dwHeight = 1; dwWidth > desc.dwHeight; desc.dwHeight <<= 1);
  }
  

  // ¿Tiene que ser de forma cuadrada?
  if (bSquare) {
    // Se iguala la dimension menor a la mayor
    if (desc.dwWidth > desc.dwHeight) { desc.dwHeight = desc.dwWidth; }
    else { desc.dwWidth = desc.dwHeight; }
  }*/

  // Procede a crear la superficie
  m_CodeResult = pDXDraw->GetObj()->CreateSurface(&desc, 
                                                  &m_pDDSurface, 
                                                  NULL);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
	DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
 
  // Establece los datos de la superficie creada  
  m_dwWidth = desc.dwWidth;						 // Anchura 
  m_dwHeight = desc.dwHeight;					 // Altura
  SetPixelFormat();								 // Info del formato  
  m_SurfaceType = DXDDDefs::SURFACETYPE_TEXTURE; // Establece el tipo
  m_bHardware = bHardware;						 // ¿En hardware?
  m_bPow2 = bPow2;								 // ¿Potencia de 2 en dimensiones?
  m_bSquare = bSquare;							 // ¿Cuadrada?
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza una conversion de los componentes recibidos por parametro
//   a un DWORD que contendra el color final.
// Parametros:
// - ucRed: Componente rojo.
// - ucGreen: Componente verde.
// - ucBlue: Componente azul.
// - ucAlpha: Componente alpha
// Devuelve:
// - Por funcion devuelve el DWORD que contiene el color deseado.
// Notas:
// - Este metodo utiliza la informacion que crea el metodo 
//   SetPixelFormat (el cual es llamado al crear la superficie).
// - Es un metodo universal, es decir, funciona para cualquier
//   configuracion RGB existente.
////////////////////////////////////////////////////////////////////// 
DWORD DXDDSurfaceTexture::RGBAToColor(const byte ucRed, 
                                      const byte ucGreen, 
                                      const byte ucBlue,
                                      const byte ucAlpha)
{
  // Se forma el DWORD multiplicando (realmente dividiendo) el bitmask 
  // de cada componente por el factor utilizado. 
  // Al resultado se le aplica una operacion AND para dejar solo 
  // los bits que sean coherentes con el componente
  return (DWORD(m_RGBInfo.PixelFormat.dwRGBAlphaBitMask * (ucAlpha / 255.0f)) & // A
		  m_RGBInfo.PixelFormat.dwRGBAlphaBitMask) |
		  (DWORD(m_RGBInfo.PixelFormat.dwRBitMask * (ucRed / 255.0f)) & // R
		   m_RGBInfo.PixelFormat.dwRBitMask) |
		  (DWORD(m_RGBInfo.PixelFormat.dwGBitMask * (ucGreen / 255.0f)) & // G
		   m_RGBInfo.PixelFormat.dwGBitMask) |
		  (DWORD(m_RGBInfo.PixelFormat.dwBBitMask * (ucBlue / 255.0f)) & // B
		   m_RGBInfo.PixelFormat.dwBBitMask);
}
