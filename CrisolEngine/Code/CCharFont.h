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
// CCharFont.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CCharFont
//
// Descripcion:
// - Representa a un objeto dibujable de tipo caracter. Estos objetos se
//   caracterizaran porque exijiran el uso de una region de dibujado en
//   su inicializacion.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CCHARFONT_H_
#define _CCHARFONT_H_

// Cabeceras
#ifndef _CDRAWABLE_H_
#include "CDrawable.h" 
#endif

// Clase CCharFont
class CCharFont: public CDrawable
{
private:
  // Tipos
  typedef CDrawable Inherited; // Clase base

private:
  // Estructuras
  struct sCharFontInfo {
	// Informacion asociada al quad
	word					   uwWidth;     // Anchura
	word				       uwHeight;    // Altura  
	ResDefs::TextureHandle     hIDTexture;  // Handle de textura asociado
	GraphDefs::sTextureRegion* pTextureRgn; // Region donde se halla el caracter
  };

private:
  // Vbles de miembro
  sCharFontInfo m_CharFontInfo; // Info asociado al caracter de fuente
  bool          m_bIsInitOk; // ¿Instancia inicializada correctamente?  
      
public:
  // Protocolos de inicilizacion / finalizacion de instancia.  
  CCharFont(void): m_bIsInitOk(false)  { }
  ~CCharFont(void) { End(); }

public:
  // Protocolos de inicializacion y fin
  bool Init(const ResDefs::TextureHandle hIDTexture,
			GraphDefs::sTextureRegion* const pTextureRgn,
			const word uwWidth, 
			const word uwHeight);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // CDrawable / Obtencion de valores basicos
  ResDefs::TextureHandle GetIDTexture(void) { 
	ASSERT(IsInitOk());
	// No hay textura asociada
	return m_CharFontInfo.hIDTexture; 
  }  
  GraphDefs::sTextureRegion* const GetTextureRegion(void) {
	ASSERT(IsInitOk());
	// No hay region asociada
	return m_CharFontInfo.pTextureRgn;
  }
  word GetWidth(void) { 
	ASSERT(IsInitOk());
	// Se devuelve anchura
	return m_CharFontInfo.uwWidth; 
  }
  word GetHeight(void) { 
	ASSERT(IsInitOk());
	// Se devuelve altura
	return m_CharFontInfo.uwHeight; 
  }
};

#endif // ~ #ifdef _CCHARFONT_H_