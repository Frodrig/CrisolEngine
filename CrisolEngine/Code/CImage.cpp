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
// CImage.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CImage.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CImage.h"

#include "iCResourceManager.h"
#include "DXWrapper\\DXDDSurfaceTexture.h"
#include "SYSEngine.h"
#include <string>

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia registrando en el servidor de recursos la imagen
//   recibida. En caso de querer reinicializar se podra.
// Parametros:
// - szFileName. Nombre del fichero con la textura o porcion de textura.
// - rDimension. Localizacion de la porcion de la textura en la imagen.
// - Bpp, ABpp. Bits por pixel y alpha.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CImage::Init(const std::string& szFileName,
			 const sRect& rDimension,
			 const GraphDefs::eBpp& Bpp,
			 const GraphDefs::eABpp& ABpp)
{
  // SOLO si parametros validos
  ASSERT(szFileName.size());
  ASSERT((rDimension.swLeft < rDimension.swRight) != 0);
  ASSERT((rDimension.swTop < rDimension.swBottom) != 0);

  // Se finaliza por si se intentara reinicializar
  End();

  // Se registra la imagen en el servidor de recursos
  m_hTexture = SYSEngine::GetResourceManager()->TextureRegister(szFileName,
															    rDimension,
															    Bpp,
															    ABpp);
  ASSERT(m_hTexture);

  // Todo correcto
  m_bIsInitOk = true;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa una textura a partir de una imagen, supeditando su dimension
//   a la dimension de esta imagen.
// Parametros:
// - szFileName. Nombre del fichero con la imagen.
// - Bpp, ABpp. Bits por pixels y para el canal alpha
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CImage::Init(const std::string& szFileName,
			 const GraphDefs::eBpp& Bpp,
			 const GraphDefs::eABpp& ABpp)
{
  // SOLO si parametros validos
  ASSERT(!szFileName.empty());

  // Se finaliza por si se intentara reinicializar
  End();

  // Se registra la imagen en el servidor de recursos
  m_hTexture = SYSEngine::GetResourceManager()->TextureRegister(szFileName, 
																Bpp, 
																ABpp);
  ASSERT(m_hTexture);

  // Todo correcto
  m_bIsInitOk = true;
  return true;  
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la imagen a partir de una textura vacia. Se permitira la
//   reinicializacion
// Parametros:
// - szFileName. Nombre del fichero con la imagen.
// - uwWidth, uwHeight. Anchura y altura de la textura.
// - Bpp, ABpp. Bits por pixels y para el canal alpha
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CImage::Init(const std::string& szFileName,
			 const word uwWidth,
			 const word uwHeight,
			 const GraphDefs::eBpp& Bpp,
			 const GraphDefs::eABpp& ABpp)
{
  // SOLO si parametros validos
  ASSERT(szFileName.size());
  
  // Se finaliza por si se intentara reinicializar
  End();

  // Se registra la imagen en el servidor de recursos
  m_hTexture = SYSEngine::GetResourceManager()->TextureRegister(szFileName,
															    uwWidth,
															    uwHeight,
															    Bpp,
															    ABpp);
  ASSERT(m_hTexture);

  // Todo correcto
  m_bIsInitOk = true;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia y libera el recurso de tipo textura.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void CImage::End(void)
{
  if (IsInitOk()) {
    // Se libera el recurso de tipo textura
    SYSEngine::GetResourceManager()->ReleaseTexture(GetIDTexture());
    m_hTexture = 0;

    // Se baja el flag
    m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve la anchura de la imagen
// Parametros:
// Devuelve:
// - Anchura de la imagen
// Notas:
///////////////////////////////////////////////////////////////////////////////
word 
CImage::GetWidth(void) 
{ 
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Se devuelve anchura
  return SYSEngine::GetResourceManager()->GetTexture(m_hTexture)->GetWidth();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve la anchura de la imagen
// Parametros:
// Devuelve:
// - Anchura de la imagen
// Notas:
///////////////////////////////////////////////////////////////////////////////
word 
CImage::GetHeight(void) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se devuelve altura
  return SYSEngine::GetResourceManager()->GetTexture(m_hTexture)->GetHeight();
}
