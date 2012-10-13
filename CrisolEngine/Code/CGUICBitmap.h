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
// CGUICBitmap.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUICBitmap
//
// Descripcion:
// - Define un componente de tipo Bitmap. Estos componentes contendran un
//   objeto CBitmap para mantener a una imagen.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUICBITMAP_H_
#define _CGUICBITMAP_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CGUICOMPONENT_H_
#include "CGUIComponent.h"
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif
#ifndef _CBITMAP_H_
#include "CBitmap.h"
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CGUICBitmap
class CGUICBitmap: public CGUIComponent
{
private:
  // Tipos
  typedef CGUIComponent Inherited; // Acceso a la clase base

public:
  struct sGUICBitmapInitData: public Inherited::sGUIComponentInitData {
	// Inicializacion de CGUICBitmap
	// Datos
	GraphDefs::sBitmapInitData BitmapInfo;     // Info asociada al bitmap	
	GraphDefs::sRGBColor       RGBSelectColor; // Color de seleccion		
  };

private:
  // Estructuras
  struct sBitmapInfo {
	// Informacion sobre el componente de tipo bitmap
	CBitmap              Bitmap;         // Bitmap asociado
	GraphDefs::sRGBColor RGBSelectColor; // Color RGB de seleccion	
	bool                 bSelect;        // ¿Componente seleccionado?
  };

private:
  // Vbles de miembro
  sBitmapInfo m_BitmapInfo; // Info asociada al componente

public:
  // Constructor / destructor
  CGUICBitmap(void) { }
  ~CGUICBitmap(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const sGUICBitmapInitData& InitData);
  void End(void);

public:
  // CGUIComponent / Dibujado
  void Draw(void);

public:
  // Operaciones sobre el aspecto del boton
  inline void SetAlphaValue(const GraphDefs::sAlphaValue& AlphaValue) {	
	ASSERT(Inherited::IsInitOk());
	// Se establece el valor alpha
	m_BitmapInfo.Bitmap.SetAlphaValue(AlphaValue);
  }

public:
  // CGUIComponent / Operaciones sobre el estado de la seleccion
  void Select(const bool bSelect);
  inline bool IsSelect(void) const { 
	ASSERT(Inherited::IsInitOk());
	// ¿Boton seleccionado?
	return m_BitmapInfo.bSelect;
  }

public:
  // CGUICComponent / Trabajo con el area del componente
  word GetWidth(void) {
	ASSERT(Inherited::IsInitOk());
	// Se devuelve la anchura
	return m_BitmapInfo.Bitmap.GetWidth();
  }
  word GetHeight(void) {
	ASSERT(Inherited::IsInitOk());
	// Se devuelve la altura
	return m_BitmapInfo.Bitmap.GetHeight();
  }

public:
  // Obtencion del nombre de la imagen
  inline std::string GetBitmapFileName(void) const {
	ASSERT(IsInitOk());
	// Retorna el nombre de la imagen
	return m_BitmapInfo.Bitmap.GetBitmapFileName();
  }
}; // ~ CGUICBitmap

#endif _CGUICBITMAP_H_
