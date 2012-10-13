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
// CGUICLineText.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUICLineText
//
// Descripcion:
// - Representa a una linea simple.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUICLINETEXT_H_
#define _CGUICLINETEXT_H_

// Cabeceras
#ifndef _CGUICOMPONENT_H_
#include "CGUIComponent.h"
#endif
#ifndef _STRING_H_
#include <string>
#define _STRING_H_
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CGUICLineText
class CGUICLineText: public CGUIComponent
{  
private:
  // Tipos
  typedef CGUIComponent Inherited; // Clase base

public:
  struct sGUICLineTextInitData: public CGUIComponent::sGUIComponentInitData {
	// Inicializacion de CGUICLineText
	// Datos
	std::string            szLine;           // Texto a poner
	std::string            szFont;           // Fuente con la que poner el texto		
	GraphDefs::sRGBColor   RGBSelectColor;   // Color con seleccion
	GraphDefs::sRGBColor   RGBUnselectColor; // Color sin seleccion
	GraphDefs::sAlphaValue Alpha;            // Valor alpha asociado al texto
  };

private:
  // Estructuras
  struct sTextInfo {
	// Info basica relativa al texto
	std::string            szLine;           // Linea de texto
	std::string            szFont;           // Nombre de la fuente	
	word                   uwAreaWidth;      // Anchura de linea
	word                   uwAreaHeight;     // Altura de la linea
	GraphDefs::sRGBColor   RGBSelectColor;   // Color rgb de seleccion
	GraphDefs::sRGBColor   RGBUnselectColor; // Color rgb de no seleccion
	GraphDefs::sAlphaValue Alpha;            // Valor alpha
	GraphDefs::sRGBColor*  pRGBActColor;     // Color actual 
  };

private:
  // Vbles de miembro
  sTextInfo m_TextInfo; // Info relativa al texto  
 
public:
  // Constructor / destructor
  CGUICLineText(void) { }	
  ~CGUICLineText(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const sGUICLineTextInitData& InitData);
  void End(void);

public:
  // Cambio del valor asociado a la linea de texto
  void ChangeText(const std::string& szNewText); 

public:
  // CGUIComponent / Dibujado
  void Draw(void);  

public:
  // Trabajo con valores alpha
  inline void SetAlphaValue(const byte ubAlphaValue) {
	// SOLO si instancia inicializada
	ASSERT(Inherited::IsInitOk());
	// Se toma valor alpha
	m_TextInfo.Alpha.ubAlpha = ubAlphaValue;
  }
  inline GraphDefs::sAlphaValue GetAlphaValue(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el alpha 
	return m_TextInfo.Alpha;
  }

public:
  // Establecimiento de colores
  void inline SetSelectColor(const GraphDefs::sRGBColor& RGBColor) {
	ASSERT(Inherited::IsInitOk());
	// Establece color
	m_TextInfo.RGBSelectColor = RGBColor;
  }
  void inline SetUnselectColor(const GraphDefs::sRGBColor& RGBColor) {
	ASSERT(Inherited::IsInitOk());
	// Establece color
	m_TextInfo.RGBUnselectColor = RGBColor;
  }

public:
  // Operaciones de seleccion
  inline void Select(const bool bSelect) {	
	ASSERT(Inherited::IsInitOk());
	// Se establece color actual
	m_TextInfo.pRGBActColor = bSelect ? &m_TextInfo.RGBSelectColor : &m_TextInfo.RGBUnselectColor;
	ASSERT(m_TextInfo.pRGBActColor);
  }
  inline bool IsSelect(void) const { 	
	ASSERT(Inherited::IsInitOk());
	// ¿Componente seleccionado?
	return (m_TextInfo.pRGBActColor == &m_TextInfo.RGBSelectColor);
  }
  
public:
  // Obtencion de informacion
  inline std::string GetLineText(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el texto asociado a la linea
	return m_TextInfo.szLine;
  }
  inline std::string GetFont(void) const {
	ASSERT(Inherited::IsInitOk());
	// Regresa el nombre de la fuente
	return m_TextInfo.szFont;
  }
  inline GraphDefs::sRGBColor GetActColor(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el color actual
	return *m_TextInfo.pRGBActColor;
  }  

public:
  // CGUICComponent / Trabajo con el area del componente
  word GetWidth(void) {
	ASSERT(Inherited::IsInitOk());
	// Se devuelve la anchura
	return m_TextInfo.uwAreaWidth;
  }
  word GetHeight(void) {
	ASSERT(Inherited::IsInitOk());
	// Se devuelve la altura
	return m_TextInfo.uwAreaHeight;
  }
};

#endif 
