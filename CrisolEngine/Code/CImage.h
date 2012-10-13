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
// CImage.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CImage
//
// Descripcion:
// - Define la clase basica para representar imagenes estaticas en pantalla.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CIMAGE_H_
#define _CIMAGE_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CDRAWABLE_H_
#include "CDrawable.h"
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif

// Clase CImage
class CImage: public CDrawable
{
private:
  // Clase base
  typedef CDrawable Inherited;

private:
  // Vbles de miembro
  ResDefs::TextureHandle m_hTexture;  // Handle a textura
  bool					 m_bIsInitOk; // ¿Instancia incializada correctamente?

public:
  // Constructor / destructor
  CImage(void): m_hTexture(0),
			    m_bIsInitOk(false) { }
  ~CImage(void) { End(); }

public:
  // Protocolos de inicilizacion / finalizacion de instancia.  
  bool Init(const std::string& szFileName,
			const sRect& rDimension,
			const GraphDefs::eBpp& Bpp,
			const GraphDefs::eABpp& ABpp);
  bool Init(const std::string& szFileName,
			const GraphDefs::eBpp& Bpp,
			const GraphDefs::eABpp& ABpp);
  bool Init(const std::string& szFileName,
			const word uwWidth,
			const word uwHeight,
			const GraphDefs::eBpp& Bpp,
			const GraphDefs::eABpp& ABpp);    
  void End(void);
  bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // CDrawable / Obtencion de identificador de textura
  ResDefs::TextureHandle GetIDTexture(void) {
	ASSERT(IsInitOk());
	// Retorna handle
	return m_hTexture;
  }

public:
  // CDrawable / Obtencion de las dimensiones
  word GetWidth(void);
  word GetHeight(void);
};

#endif // ~ #ifdef _CIMAGE_H_
