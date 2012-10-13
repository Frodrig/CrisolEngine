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
// CBitmap.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CBitmap.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CBitmap.h"

#include "SYSEngine.h"
#include "iCGraphicSystem.h"
#include "iCGraphicLoader.h"
#include "CImage.h"
#include "CMemoryPool.h"

// Estructuras
struct CBitmap::sNImage { 
  // Nodo asociado a una imagen
  CImage Image; // Imagen con una porcion del bitmap
  // Manejador de memoria
  static CMemoryPool m_MPool;
  static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
  static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
};

// Inicializacion de los manejadores de memoria
CMemoryPool 
CBitmap::sNImage::m_MPool(8, sizeof(CBitmap::sNImage), true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia. Creara un array de imagenes que se corresponda
//   al numero en que se pueda dividir la imagen a cargar. En caso de que
//   se quiera reinicializar se podra.
// Parametros:
// - InitData. Datos de inicializacion.
// Devuelve:
// - Si todo va bien true. En caso contrario false.
// Notas:
// - A la hora de obtener el numero de CImage's necesarias a lo ancho y alto,
//   todo lo que sobre de la imagen y no quepa, sera completamente desechado.
///////////////////////////////////////////////////////////////////////////////
bool 
CBitmap::Init(const GraphDefs::sBitmapInitData& InitData)
{
  // SOLO si parametros validos
  ASSERT(InitData.szFileName.size());

  // ¿Se intenta reinicializar?
  if (IsInitOk()) { 
	End(); 
  }

  // Se obtiene interfaz al cargador grafico
  iCGraphicLoader* const pGraphLoader = SYSEngine::GetGraphicLoader();
  ASSERT(pGraphLoader);

  // Se obtiene el numero de CImage's necesarias a lo ancho y alto
  m_BitmapInfo.szFileName = InitData.szFileName;  
  SYSEngine::MakeLowercase(m_BitmapInfo.szFileName);
  if (!pGraphLoader->TGALoader(m_BitmapInfo.szFileName)) { 
	return false; 
  }  

  // Numero de imagenes a lo ancho
  m_BitmapInfo.uwWidth = pGraphLoader->GetWidth() / InitData.WidthTextureDim;
  ASSERT(m_BitmapInfo.uwWidth);

  // Numero de imagenes a lo alto
  m_BitmapInfo.uwHeight = pGraphLoader->GetHeight() / InitData.HeightTextureDim;
  ASSERT(m_BitmapInfo.uwHeight);
  
  // Se reserva memoria para las imagenes
  m_BitmapInfo.pImages = new sNImage[m_BitmapInfo.uwWidth * m_BitmapInfo.uwHeight];
  ASSERT(m_BitmapInfo.pImages);

  // Se inicializan estructuras RGB y Alpha  
  m_BitmapInfo.RGBColor = InitData.RGBColor;  
  m_BitmapInfo.Alpha = InitData.Alpha;

  // ¿Hay que utilizar texture alpha blending?
  const bool bTextureAlpha = (InitData.ABpp != GraphDefs::ALPHA_BPP_0) ? true : false;

  // Se procede a inicializar las imagenes y a asociar estructuras RGB y Alpha
  sRect rImageArea;
  for (word uwYIt = 0; uwYIt < m_BitmapInfo.uwHeight; ++uwYIt) {
	for (word uwXIt = 0; uwXIt < m_BitmapInfo.uwWidth; ++uwXIt) {
	  // Se calcula la zona donde esta la porcion de imagen a copiar
	  rImageArea.swLeft = uwXIt * InitData.WidthTextureDim;
	  rImageArea.swRight = rImageArea.swLeft + InitData.WidthTextureDim - 1;
	  rImageArea.swTop = uwYIt * InitData.HeightTextureDim;
	  rImageArea.swBottom = rImageArea.swTop + InitData.HeightTextureDim - 1;
	  
	  // Se inicializa la imagen
	  CImage* const pImage = &m_BitmapInfo.pImages[uwYIt * m_BitmapInfo.uwWidth + uwXIt].Image;
	  ASSERT(pImage);
	  if (!pImage->Init(InitData.szFileName, 
						rImageArea, 
						InitData.Bpp, 
						InitData.ABpp)) {
		// Problemas
		FreeImages();
		return false;
	  }

	  // Se le asocia estructura RGB y Alpha
	  pImage->SetRGBColor(GraphDefs::VERTEX_0, &m_BitmapInfo.RGBColor);	  	  
	  pImage->SetRGBColor(GraphDefs::VERTEX_1, &m_BitmapInfo.RGBColor);
	  pImage->SetRGBColor(GraphDefs::VERTEX_2, &m_BitmapInfo.RGBColor);
	  pImage->SetRGBColor(GraphDefs::VERTEX_3, &m_BitmapInfo.RGBColor);
	  pImage->SetAlphaValue(GraphDefs::VERTEX_0, &m_BitmapInfo.Alpha);	  	  
	  pImage->SetAlphaValue(GraphDefs::VERTEX_1, &m_BitmapInfo.Alpha);
	  pImage->SetAlphaValue(GraphDefs::VERTEX_2, &m_BitmapInfo.Alpha);
	  pImage->SetAlphaValue(GraphDefs::VERTEX_3, &m_BitmapInfo.Alpha);	  
	}	
  }    

  // Todo correcto
  m_bIsInitOk = true;
  return true;    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia, liberando todo los recursos que procedan.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CBitmap::End(void)
{
  // Finaliza si procede
  if (IsInitOk()) {
	// Libera array de imagenes
	FreeImages();

	// Baja el flag
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza un bucle por todas las imagenes que tiene asociadas y va realizando
//   sucesivas llamadas al subsisteam grafico para dibujarlas.
// Parametros:
// - Zone. Zona de dibujo.
// - ubPlane. Plano de dibujo.
// - swXPos, swYPos. Posicion en XY donde dibujar
// - bDarkFactor. ¿Uso del factor de oscuridad?
// - pLight. Luz asociada.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CBitmap::Draw(const GraphDefs::eDrawZone& Zone, const byte ubPlane,
			  const sword& swXPos, const sword& swYPos, 			
			  const bool bDarkFactor, 
			  GraphDefs::sLight* const pLight)
{
  // SOLO si intancia inicializada
  ASSERT(IsInitOk());

  // Obtiene intancia al subsistema grafico
  iCGraphicSystem* const pGraphicSystem = SYSEngine::GetGraphicSystem();
  ASSERT(pGraphicSystem);
  
  // Realiza el dibujado
  for (word uwYIt = 0; uwYIt < m_BitmapInfo.uwHeight; ++uwYIt) {
	for (word uwXIt = 0; uwXIt < m_BitmapInfo.uwWidth; ++uwXIt) {
	  // Se obtiene imagen
	  CImage* const pImage = &m_BitmapInfo.pImages[uwYIt * m_BitmapInfo.uwWidth + uwXIt].Image;
	  
	  // Realiza el dibujado
	  pGraphicSystem->Draw(Zone, 
						   ubPlane, 
						   swXPos + uwXIt * pImage->GetWidth(), 
						   swYPos + uwYIt * pImage->GetHeight(), 
						   pImage);

	}
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera los punteros a las imagenes que esten en memoria.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CBitmap::FreeImages(void)
{  
  // Obtiene dimension del array
  const word uwSize = m_BitmapInfo.uwWidth * m_BitmapInfo.uwHeight; 
  if (!uwSize) { 
	return; 
  }

  // Libera espacio y resetea dimensiones
  ASSERT(m_BitmapInfo.pImages);
  delete[uwSize] m_BitmapInfo.pImages;
  m_BitmapInfo.pImages = NULL;
  m_BitmapInfo.uwHeight = m_BitmapInfo.uwWidth = 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la anchura global del bitmap y la devuelve.
// Parametros:
// Devuelve:
// - Anchura del bitmap
// Notas:
///////////////////////////////////////////////////////////////////////////////
word 
CBitmap::GetWidth(void) const
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Retorna anchura
  return (m_BitmapInfo.pImages[0].Image.GetWidth() * m_BitmapInfo.uwWidth);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la altura global del bitmap y la devuelve.
// Parametros:
// Devuelve:
// - Altura del bitmap
// Notas:
///////////////////////////////////////////////////////////////////////////////
word 
CBitmap::GetHeight(void) const
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Retorna altura
  return (m_BitmapInfo.pImages[0].Image.GetHeight() * m_BitmapInfo.uwHeight);
}