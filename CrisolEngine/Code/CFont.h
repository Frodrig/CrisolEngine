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
// CFont.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CFont
//
// Descripcion:
// - Encapsula el trabajo con una fuente. Las fuentes van a ser representaciones
//   de fuentes true type sobre texturas de 256x256, de tal forma que en el
//   proceso de inicializacion de la clase, los caracteres de la fuente
//   true type son volcados a la textura.
//
// Notas:
// - El dibujado de las fuentes se realizara usando el efecto de sombra.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CFONT_H_
#define _CFONT_H_

// Cabeceras
#ifndef _DXDDSURFACETEXTURE_H_
#include "DXWrapper\\DXDDSurfaceTexture.h"
#endif
#ifndef _ICFONT_H_
#include "iCFont.h"
#endif
#ifndef _CIMAGE_H_
#include "CImage.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>    
#endif

// Definicion de clases / structuras / tipos referenciados

// Clase CFont
class CFont: public iCFont
{
private:
  // Estructuras
  struct sFontInfo { 
	// Informacion relativa a la fuente actual
	std::string				  szName;        // Nombre de la fuente
    HFONT					  hFont;         // Handle a la fuente true type
    HFONT					  hPrevFont;     // Fuente previa    
    CImage					  TextureImg;    // Imagen de la textura
    sword					  swDeciPt;      // Tamaño en decipuntos
	FontDefs::eCharPoints	  CPoints;       // Puntos de resolucion en anchura
    byte					  ubQuadSize;    // Dimension del quad que encierra el caracter
    TEXTMETRIC                tm;            // Datos sobre la metrica de la fuente
    ABC                       abc[224];      // Informacion sobre las anchuras de los caracteres
    byte                      Widths[224];   // Anchuras de los caracteres    
    GraphDefs::sTextureRegion CharsPos[224]; // Posiciones u,v de los caracteres precalculadas
  };

private:
  // Vbles de miembro
  sFontInfo m_FontInfo;  // Informacion relativa a la fuente
  bool      m_bIsInitOk; // ¿Instancia inicializada?  
     
public:
  // Constructor / destructor
  CFont(void): m_bIsInitOk(false) { }
  ~CFont(void) { End(); }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(const std::string& szTrueTypeFont, 
			const FontDefs::eCharPoints& CPoints = FontDefs::FONT_CHAR_POINT_8);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // iCFont / Operaciones sobre la fuente
  ResDefs::TextureHandle const GetIDFontImage(void) { 
	ASSERT(IsInitOk());
	// Retorna el ID de la textura con los caracteres
	return m_FontInfo.TextureImg.GetIDTexture(); 
  }
  byte GetCharWidth(const byte ubChar) const {
	ASSERT(IsInitOk());
	// Retorna la anchura de un caracter
	return (ubChar < 32) ? 16 : m_FontInfo.Widths[ubChar - 32];
  }
  FontDefs::eCharPoints GetCharPoints(void) const { 
	ASSERT(IsInitOk());
	// Retorna puntos de resolucion de los caracteres
	return m_FontInfo.CPoints; 
  }
  byte GetCharQuadRes(void) const { 
	ASSERT(IsInitOk());
	// Devuelve la resolucion del quad x,y siempre sera cuadrado
	return m_FontInfo.ubQuadSize; 
  }  
  GraphDefs::sTextureRegion* GetCharPos(const byte ubChar) {
	ASSERT(IsInitOk());
	//ASSERT((ubChar > 31) != 0);
	// Retorna la DIRECCION de la region en la textura donde hallar el caracter
	if (ubChar > 31) {
	  return &m_FontInfo.CharsPos[ubChar - 32];  
	} else {
	  return &m_FontInfo.CharsPos[' ' - 32];
	}
  }
public:  
  inline std::string GetFontName(void) const { 
	ASSERT(IsInitOk());
	// Retorna el nombre de la fuente
	return m_FontInfo.szName; 
  }  

public:
  // Restaura superficie perdida
  void Restore(void); 
  
private:
  // Metodos de apoyo
  void CreateTTFont(const HDC& hdc, const sword& swDeciPt);
  void DestroyTTFont(const HDC& hdc);
  void DrawTTFont(const HDC& hdc, const bool bCalcUVPos);
};

#endif // ~ #ifdef _CFONT_H_
