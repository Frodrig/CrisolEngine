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
// CBitmap.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CBitmap
//
// Descripcion:
// - Representa un bitmap. El bitmap estara modelado de tal forma que en su
//   interior se mantendra un array de instancias CImage. Para poder construir
//   el array se pasara la dimension de los bloques de la imagen que se tomaran
//   a lo ancho y alto para construir las instancias CImage.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CBITMAP_H_
#define _CBITMAP_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif
#ifndef _STRING_H_
#include <string>
#define _STRING_H_
#endif

// Clase CBitmap
class CBitmap
{
private:
  // Estructuras forward
  struct sNImage; 

private:
  struct sBitmapInfo: public GraphDefs::sBitmapInitData {
	// Informacion asociada al bitmap
	word     uwWidth;  // Numero de imagenes a lo ancho
	word     uwHeight; // Numero de imagenes a lo ancho		
	sNImage* pImages;  // Array con las imagenes
  };

private:
  // Vbles de miembro
  bool        m_bIsInitOk;  // ¿Instancia incializada correctamente?
  sBitmapInfo m_BitmapInfo; // Info relacionada con el bitmap

public:
  // Constructor / destructor
  CBitmap(void): m_bIsInitOk(false) { }
  ~CBitmap(void) { End(); }

public:
  // Protocolos de inicilizacion / finalizacion de instancia.  
  bool Init(const GraphDefs::sBitmapInitData& InitData);
  void End(void);
  inline bool IsInitOk(void) const { 
	return m_bIsInitOk; 
  }

public:
  // Dibujado
  void Draw(const GraphDefs::eDrawZone& Zone, const byte ubPlane,
			const sword& swXPos, const sword& swYPos, 			
			const bool bDarkFactor = false, 
			GraphDefs::sLight* const pLight = NULL);

public:
  // Operaciones sobre la imagen referidas al color
  inline void ResetRGBValues(void) {
	ASSERT(IsInitOk());
	// Pone a 255 el valor RGB
	m_BitmapInfo.RGBColor.ubRed = m_BitmapInfo.RGBColor.ubGreen = m_BitmapInfo.RGBColor.ubBlue = 255;
  }
  inline void ResetAlphaValues(void) {
	ASSERT(IsInitOk());
	// Pone a 255 el valor alpha
	m_BitmapInfo.Alpha = 255;
  }
  inline void SetRGBColor(const GraphDefs::sRGBColor& RGBColor) {
	ASSERT(IsInitOk());
	// Asocia el nuevo color RGB a la imagen
	m_BitmapInfo.RGBColor = RGBColor;
  }
  inline void SetAlphaValue(const GraphDefs::sAlphaValue AlphaValue) {
	ASSERT(IsInitOk());
	// Asocia el nuevo valor alpha
	m_BitmapInfo.Alpha = AlphaValue;
  }  
  inline GraphDefs::sAlphaValue GetAlpha(void) const {
	ASSERT(IsInitOk());
	// Retorna el valor alpha
	return m_BitmapInfo.Alpha;
  }
  inline GraphDefs::sRGBColor GetRGBColor(void) const {
	ASSERT(IsInitOk());
	// Retorna el valor RGB
	return m_BitmapInfo.RGBColor;
  }

public:
  // Obtencion de informacion referida a dimensiones
  word GetWidth(void) const; 
  word GetHeight(void) const;

public:
  // Obtencion del nombre de la imagen
  inline std::string GetBitmapFileName(void) const {
	ASSERT(IsInitOk());
	// Retorna el nombre de la imagen
	return m_BitmapInfo.szFileName;
  }

private:
  // Metodos de apoyo
  void FreeImages(void);
};

#endif // ~ #ifdef _CBITMAP_H_
