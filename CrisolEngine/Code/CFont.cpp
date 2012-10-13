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
// CFont.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CFont.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CFont.h"

#include "iCResourceManager.h"
#include "SYSEngine.h"        
#include "math.h"             
#include <string>    

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia. Inicializar supone reservar en el servidor de
//   recursos una textura vacia y llamar al metodo que crea una fuente 
//   true type. Tambien supone volcar en la textura los caracteres que componen
//   la fuente true type. Todas las fuentes se crearan de un tamaño de 16x16.
// Parametros:
// - szTrueTypeFont. Nombre de la fuente true type. En caso de que no este
//   instalada se creara una lo mas parecido posible.
// - CPoints. Tamaño de la fuente en cuanto a la resolucion de los puntos
//   asociados a los caracteres. Por defecto vale FONT_CHAR_POINT_10. Consultar
//   FontDefs para mas informacion.
// Devuelve:
// - Si todo va bien true. En caso de existir algun problema false.
// Notas:
// - Las fuentes se almacenan en una textura de 256x256 de tal forma que el
//   tamaño maximo que puede ocupar un caracter es una region de 16x16, de 
//   este modo, la resolucion de los quads que las encerraran seran siempre
//   ctes (16x16). Los puntos se utilizaran internamente pera definir el
//   tamaño de los caracteres que formen la fuente true type al crear esta.
///////////////////////////////////////////////////////////////////////////////
bool 
CFont::Init(const std::string& szTrueTypeFont, 
 		    const FontDefs::eCharPoints& CPoints)
{
  // ¿Se intenta reinicializar?
  if (IsInitOk()) {
	return false;
  }
  
  // Se obtiene instancia al subsistema de recursos
  iCResourceManager* const pResManager = SYSEngine::GetResourceManager();
  ASSERT(pResManager); 

  // Se almacena nombre y tamaño
  m_FontInfo.szName = szTrueTypeFont;
  m_FontInfo.CPoints = CPoints;
  m_FontInfo.ubQuadSize = 16;
  m_FontInfo.swDeciPt = CPoints * 10;
    
  // Se crea la imagen  
  if (!m_FontInfo.TextureImg.Init(szTrueTypeFont, 
								  256, 256,
								  GraphDefs::BPP_16, 
								  GraphDefs::ALPHA_BPP_0)) {
	// Problemas
	return false;
  }

  // Se configuran los parametros de la imagen
  m_FontInfo.TextureImg.SetTextureAlphaBlending(true);
  m_FontInfo.TextureImg.SetVertexAlphaBlending(true);
  m_FontInfo.TextureImg.SetBilinearFiltering(false);
  m_FontInfo.TextureImg.SetColorKey(true);
    
  // Se vuelca el color (255, 0, 255), como color transparente
  // y se establece el ColorKey en la textura
  DXDDSurfaceTexture* const pTexture = pResManager->GetTexture(m_FontInfo.TextureImg.GetIDTexture());
  pTexture->BltColor(255, 0, 255, 255);  
  pTexture->SetColorKey(255, 0, 255, 255);
    
  // Se procede a crear la fuente
  m_FontInfo.hFont = NULL;
  m_FontInfo.hPrevFont = NULL;
  
  // Se crea la fuente y se dibuja sobre la textura
  HDC hdc;  
  pTexture->GetDC(&hdc);    
  CreateTTFont(hdc, m_FontInfo.swDeciPt);  
  DrawTTFont(hdc, true);
  DestroyTTFont(hdc);
  pTexture->ReleaseDC(hdc);

  // Se levanta el flag y se regresa 
  m_bIsInitOk = true;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea la fuente true type, pero NO la selecciona. 
// Parametros:
// - hdc. DC a la superficie sobre la que se va a crear la fuente.
// - swDeciPt. Tamaño de la fuentes en puntos por 10. Si nuestra fuente va a
//   tener un tamaño de 16, debera de valer 16x10 = 160.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CFont::CreateTTFont(const HDC& hdc, const sword& swDeciPt)
{
  // SOLO si los parametros son validos
  ASSERT(hdc);
  ASSERT(swDeciPt);

  // Configura el DC recibido  
  SetGraphicsMode(hdc, GM_ADVANCED);
  ModifyWorldTransform(hdc, NULL, MWT_IDENTITY);
  SetViewportOrgEx(hdc, 0, 0, NULL);
  SetWindowOrgEx(hdc, 0, 0, NULL);

  // Se establece la res. logica
  float cxDpi = (float) GetDeviceCaps(hdc, LOGPIXELSX);
  float cyDpi = (float) GetDeviceCaps(hdc, LOGPIXELSY);

  // Define ptos. por pulgada
  POINT pt;
  pt.x = 0;
  pt.y = (sword) (swDeciPt * cyDpi / 72);
  DPtoLP(hdc, &pt, 1);

  // Configura y crea la fuente
  LOGFONT lf;
  memset(&lf, 0, sizeof(lf));
  lf.lfHeight = -(sword) (fabs(pt.y) / 10.0 + 0.5);
  strcpy(lf.lfFaceName, m_FontInfo.szName.c_str());
  m_FontInfo.hFont = CreateFontIndirect(&lf);
  lf.lfQuality = ANTIALIASED_QUALITY;

  // Se obtiene datos sobre metrica y anchura
  m_FontInfo.hPrevFont = (HFONT) SelectObject(hdc, m_FontInfo.hFont);
  GetTextMetrics(hdc, &m_FontInfo.tm);
  GetCharABCWidths(hdc, 32, 255, m_FontInfo.abc);

  // Restaura el estado del hdc anterior  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destruye la fuente seleccionada y creada con CreateTTFont.
// Parametros:
// - hdc. Referencia al DC de la superficie en donde se creo la fuente.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CFont::DestroyTTFont(const HDC& hdc)
{
  ASSERT(m_FontInfo.hFont);
  ASSERT(m_FontInfo.hPrevFont);
  DeleteObject(SelectObject(hdc, m_FontInfo.hPrevFont));
  m_FontInfo.hPrevFont = NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Dibuja los caracteres que forman la fuente en la superficie a la que
//   pertenezca el DC recibido. Se esperara que sea siempre de la textura.
//   Al dibujar los caracteres solo se contemplaran los caracteres que se
//   hallen entre las posiciones 32 y 256. Los 31 primeros no se tendran en
//   cuenta.
// Parametros:
// - hdc. DC a la textura.
// - bCalcUVPos. Si vale true se calcularan las posiciones u,v de cada caracter.
//   En caso contrario se evitara dicho calculo.
// Devuelve:
// Notas:
// - Se esperara que el metodo bCalcUVPos sea solo puesto a true en la llamada
//   desde Init. En el resto de los casos (cuando se llame desde Restore) dicho
//   parametros estara a false, para evitar codigo redundante.
///////////////////////////////////////////////////////////////////////////////
void 
CFont::DrawTTFont(const HDC& hdc, const bool bCalcUVPos)
{
  // SOLO si los parametros son correctos
  ASSERT(hdc);

  // Vbles
  sword swTop;     // Posicion top del caracter
  sword swLeft;    // Posicion left del caracter
  byte  ubActChar; // Caracter actual
  sword swCWidth;  // Anchura del caracter
  sword swCHeight; // Altura del caracter
  byte  ubABCPos;  // Posicion del caracter en el array ABC

  // Se selecciona la fuente y se establecen valores para la misma
  ASSERT(m_FontInfo.hFont);  
  SetBkMode(hdc, TRANSPARENT);

  // Se procede a dibujar los caracteres. Estos estaran alineados
  // en una matriz de 14 filas por 16 columnas
  sword swYPos = 0;
  for (; swYPos < 14; swYPos++) {
    // Se obtiene posicion top
    swTop = swYPos * 16;
    
	// Obtiene altura del caracter si procede
    if (bCalcUVPos) { 
	  swCHeight = swTop + m_FontInfo.ubQuadSize; 	
	}

	// Itera a lo ancho
	sword swXPos = 0;
    for (; swXPos < 16; swXPos++) {
      // Se obtiene posicion left y el caracter actual
      swLeft = swXPos * 16; //m_FontInfo.tm.tmMaxCharWidth;
      ubActChar = byte(swYPos * 16 + swXPos + 32);
      sbyte sbChar = (sbyte) ubActChar;
    
	  // Halla la posicion donde encontrar los datos del caracter
      ubABCPos = ubActChar - 32;
      
	  // Se dibuja la sombra del texto en la textura	  	  
	  SetTextColor(hdc, 0x00000000);
      TextOut(hdc, 
              swLeft - m_FontInfo.abc[ubABCPos].abcA + 1, 
              swTop + 1, 
              &sbChar,
              1);
	  
	  // Se dibuja el texto en la textura	  
	  SetTextColor(hdc, 0x00FFFFFF);
      TextOut(hdc, 
              swLeft - m_FontInfo.abc[ubABCPos].abcA, 
              swTop, 
              &sbChar,
              1);
      
	  // Si procede, se calcula las coordenadas u,v
      if (bCalcUVPos) {
        // Obtiene anchura del caracter respecto a la textura
        swCWidth = swLeft + m_FontInfo.ubQuadSize;
      
		// Calcula las posiciones u,v        		
        m_FontInfo.CharsPos[ubABCPos].uV0 = float(swLeft) / 256.0f;
        m_FontInfo.CharsPos[ubABCPos].vV0 = float(swTop) / 256.0f;
        m_FontInfo.CharsPos[ubABCPos].uV1 = float(swCWidth) / 256.0f;
        m_FontInfo.CharsPos[ubABCPos].vV1 = m_FontInfo.CharsPos[ubABCPos].vV0;
        m_FontInfo.CharsPos[ubABCPos].uV2 = m_FontInfo.CharsPos[ubABCPos].uV0;
        m_FontInfo.CharsPos[ubABCPos].vV2 = float(swCHeight) / 256.0f;
        m_FontInfo.CharsPos[ubABCPos].uV3 = m_FontInfo.CharsPos[ubABCPos].uV1;
        m_FontInfo.CharsPos[ubABCPos].vV3 = m_FontInfo.CharsPos[ubABCPos].vV2;
		
        // Se calcula la anchura del caracter respecto a pantalla
        m_FontInfo.Widths[ubABCPos] = m_FontInfo.abc[ubABCPos].abcA +
                                      m_FontInfo.abc[ubABCPos].abcB +
                                      m_FontInfo.abc[ubABCPos].abcC;
      }
    } // ~ for
  } // ~ for 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera recursos y destruye la fuente true type seleccionada
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CFont::End(void)
{
  if (IsInitOk()) {
    // Se destruye la fuente true type
    DeleteObject(m_FontInfo.hFont);
    m_FontInfo.hFont = NULL;
    
	// Se libera la textura asociada    
	m_FontInfo.TextureImg.End();
    
	// Se baja el flag
    m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encargara de restaurar la fuente. Para ello, obtendra la instancia
//   CTextureRes (que garantiza una restauracion a nivel DX) y vuelca sobre
//   ella los caracteres que componen la fuente true type.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CFont::Restore(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Textura asociada al ID de la fuente
  iCResourceManager* const pResManager = SYSEngine::GetResourceManager();
  ASSERT(pResManager);  
  DXDDSurfaceTexture* const pTexture = pResManager->GetTexture(m_FontInfo.TextureImg.GetIDTexture());
  ASSERT(pTexture);
  
  // Se vuelca el color (255, 0, 255) como color transparente
  pTexture->BltColor(255, 0, 255, 255);  
 
  // Se crea la fuente y se dibuja sobre la textura
  HDC hdc;
  pTexture->GetDC(&hdc);    
  CreateTTFont(hdc, m_FontInfo.swDeciPt);  
  DrawTTFont(hdc, false);
  DestroyTTFont(hdc);
  pTexture->ReleaseDC(hdc);  
}