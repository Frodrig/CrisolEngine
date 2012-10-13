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
// CGUICJustifyText.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUICJustifyText
//
// Descripcion:
// - Clase base para componentes de texto justificado. Los componentes de texto 
//   de este tipo seran dos: CGUICSingleText y CGUICBlockText. Por justificado
//   se entendra aquellos que tendran una anchura maxima para representarse y 
//   que deberan de ajustarse a esa anchura escribiendose en una o mas lineas.
//
// Notas:
// - Clase base abstracta
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUICJUSTIFYTEXT_H_
#define _CGUICJUSTIFYTEXT_H_

// Cabeceras
#ifndef _CGUICOMPONENT_H_
#include "CGUIComponent.h"
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif
#ifndef _STRING_H_
#include <string>
#define _STRING_H_
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CGUICJustifyText
class CGUICJustifyText: public CGUIComponent
{
private:
  // Tipos
  typedef CGUIComponent Inherited; // Clase base

public:
  // Estructuras
  struct sGUICJustifyTextInitData: public CGUIComponent::sGUIComponentInitData {
	// Inicializacion de CGUICJustifyTextInitData
	// Datos
	std::string            szFont;           // Fuente	
	word                   uwWidthJustify;   // Anchura de justificacion
	GraphDefs::sRGBColor   RGBSelectColor;   // Color de seleccion
	GraphDefs::sRGBColor   RGBUnselectColor; // Color de deseleccion				
	GraphDefs::sAlphaValue Alpha;            // Valor alpha
  };

private:
  // Estructuras
  struct sTextInfo {
	// Info basica relativa al texto
	std::string            szFont;           // Fuente
	word                   uwWidthJustify;   // Anchura de justificacion
	GraphDefs::sRGBColor   RGBSelectColor;   // Color de seleccion
	GraphDefs::sRGBColor   RGBUnselectColor; // Color de deseleccion			
	GraphDefs::sAlphaValue Alpha;            // Valor alpha
	GraphDefs::sRGBColor*  pRGBActColor;     // Color actual 
  };

private:
  // Vbles de miembro
  sTextInfo m_TextInfo; // Info relativa al texto  
 
public:
  // Constructor / destructor
  CGUICJustifyText(void) { }	
  ~CGUICJustifyText(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const sGUICJustifyTextInitData& InitData);
  void End(void);

protected:
  // Metodo de justificacion
  sword JustifyText(const std::string& szText, 
				    const sword& swInitPos,
				    const sword& swEndPos = 0);

public:
  // Obtencion de lineas con texto visible
  virtual word GetVisibleTextLines(void) const = 0;

public:
  // Trabajo con valores alpha
  inline void SetAlphaValue(const GraphDefs::sAlphaValue Alpha) {
	// SOLO si instancia inicializada
	ASSERT(Inherited::IsInitOk());
	// Se toma valor alpha
	m_TextInfo.Alpha = Alpha;
  }
  inline GraphDefs::sAlphaValue GetAlphaValue(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el alpha 
	return m_TextInfo.Alpha;
  }

public:
  // Operaciones de seleccion
  inline void Select(const bool bSelect) {	
	ASSERT(Inherited::IsInitOk());
	// Se establece color actual
	m_TextInfo.pRGBActColor = bSelect ? &m_TextInfo.RGBSelectColor : 
									    &m_TextInfo.RGBUnselectColor;
	ASSERT(m_TextInfo.pRGBActColor);
  }
  inline bool IsSelect(void) const { 	
	ASSERT(Inherited::IsInitOk());
	// ¿Componente seleccionado?
	return (m_TextInfo.pRGBActColor == &m_TextInfo.RGBSelectColor);
  }

public:
  // CGUICComponent / Trabajo con el area del componente
  word GetWidth(void) {
	ASSERT(Inherited::IsInitOk());
	// Retorna la anchura de justificacion
	return m_TextInfo.uwWidthJustify;
  }
  
public:
  // Obtencion de informacion
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
};

#endif 
