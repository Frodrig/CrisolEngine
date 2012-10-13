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
// CGUICPercentageBar.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUICPercentageBar
//
// Descripcion:
// - Representa a un componente que simula una barra de porcentaje. La barra
//   de porcentaje podra ser horizontal o vertical y sera controlada por dos
//   variables, el valor maximo y el valor actual, que internamente se llamaran
//   valor temp y valor base. Para modelar la barra, se utilizara un objeto
//   CQuadForm.
//
// Notas:
// - El color asociado a la barra se podra poner con gradiente o sin gradiente.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUICPERCENTAGEBAR_H_
#define _CGUICPERCENTAGEBAR_H_

// Cabeceras
#ifndef _CGUICOMPONENT_H_
#include "CGUIComponent.h"
#endif
#ifndef _CQUADFORM_H_
#include "CQuadForm.h"
#endif
#include "SYSEngine.h"
#include "iCLogger.h"
// Defincion de clases / estructuras / espacios de nombres

// Clase CGUICPercentageBar
class CGUICPercentageBar: public CGUIComponent
{
private:
  // Tipos
  typedef CGUIComponent Inherited; // Acceso a la clase base

public:
  struct sGUICPercentageBarInitData: public CGUIComponent::sGUIComponentInitData {
	// Inicializacion de CGUIPercentageBarInitData
	// Datos	
	bool			       bDrawHoriz;     // ¿Dibujado en horizontal?	
	word			       uwLenght;       // Longitud (en pixels)
	word				   uwWidth;        // Anchura (en pixels)
	sword				   swBaseValue;    // Valor base
	sword                  swTempValue;    // Valor temporal
	GraphDefs::sAlphaValue Alpha;          // Valor alpha
	// Constructor por defecto
	sGUICPercentageBarInitData(void): swBaseValue(1), swTempValue(0) { }
  };

private:
  // Estructuras
  struct PercentageBarInfo {
	// Informacion sobre la barra de porcentaje
	bool			           bDrawHoriz;     // ¿Dibujado en horizontal?
	sPosition			       Position;       // Posicion de dibujado
	word			           uwLenght;       // Longitud (en pixels)
	word				       uwWidth;        // Anchura (en pixels)
	sword			   	       swBaseValue;    // Valor base
	sword                      swTempValue;    // Valor temporal
	// Nota: Los colores en el array seran 0 Start y 1 End.
	GraphDefs::sRGBColor       RGBMaxValue[2]; // Color cuando valor 50%..100%
	GraphDefs::sRGBColor       RGBMedValue[2]; // Color cuando valor 25%..49%
	GraphDefs::sRGBColor       RGBMinValue[2]; // Color cuando valor 0%..24%
	GraphDefs::sAlphaValue     Alpha;          // Valor alpha	
	GraphDefs::eVertex         VertexOrder[4]; // Orden de los vertices 0,1 Start y 2,3 End    
	CQuadForm                  QuadForm;       // Forma quad a dibujar
	bool                       bSelect;        // ¿Barra seleccionada?		
    word					   uwAreaWidth;    // Ancho barra porcentaje
	word					   uwAreaHeight;   // Alto barra porcentaje

  };

private:
  // Vbles de miembro
  PercentageBarInfo m_PerBarInfo; // Info asociada a la barra

public:
  // Constructor / destructor
  //CGUICPercentageBar(void) { }
  ~CGUICPercentageBar(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const sGUICPercentageBarInitData& InitData);
  void End(void);

public:
  // Trabajo con los valores de porcentaje
  void ChangeBaseValue(const sword& swBaseValue) {
	ASSERT(Inherited::IsInitOk());
	// Se cambia el valor base
	m_PerBarInfo.swBaseValue = swBaseValue;
	// Se calcula nueva dimension
	SetPercentageBarSize();
  }
  void ChangeTempValue(const sword& swTempValue) {
	ASSERT(Inherited::IsInitOk());
	// Se cambia el valor temporal
	m_PerBarInfo.swTempValue = swTempValue;
	// Se calcula nueva dimension
	SetPercentageBarSize();
  }
  void ChangeBaseAndTempValue(const sword& swBaseValue, 
							  const sword& swTempValue) {
	ASSERT(Inherited::IsInitOk());
	// Se cambia el valor base
	m_PerBarInfo.swBaseValue = swBaseValue;	
	m_PerBarInfo.swTempValue = swTempValue;
	// Se calcula nueva dimension
	SetPercentageBarSize();
  }
  inline sword GetBaseValue(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna valor base
	return m_PerBarInfo.swBaseValue;
  }
  inline sword GetTempValue(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna valor temporal
	return m_PerBarInfo.swTempValue;
  }

private:
  // Metodos de apoyo
  void SetPercentageBarSize(void);
  inline float Min(const float fValue1, const float fValue2) {	
	// Retorna el minimo acotandolo a 0
	const float fMin = (fValue1 < fValue2) ? fValue1 : fValue2;
	return (fMin <= 0) ? 0: fMin;
  }
  void SetRGBColor(const float fPercentage);
  void SetAlphaValue(void);

public:
  // CGUIComponent / Dibujado
  void Draw(void);

public:
  // CGUIComponent / Trabajo con el area del componente
  // No se permitira cambiar ni la anchura ni la altura 
  inline void ChangeAreaWidth(const word uwWidth) { }
  inline void ChangeAreaHeight(const word uwHeight) { }

public:
  // CGUIComponent / Operaciones sobre el estado de la seleccion
  inline void Select(const bool bSelect) {
	ASSERT(Inherited::IsInitOk());
	// Se establece el flag de seleccion
	m_PerBarInfo.bSelect = bSelect;
  }
  inline bool IsSelect(void) const { 
	ASSERT(Inherited::IsInitOk());
	// Se devuelve flag de seleccion
	return m_PerBarInfo.bSelect;
  }

public:
  // CGUICComponent / Trabajo con el area del componente
  inline sword GetYPos(void) {
	ASSERT(IsInitOk());
	// ¿Se trabaja en modo horizontal?
	if (m_PerBarInfo.bDrawHoriz) {
	  // Si, se propaga a la clase base
	  return Inherited::GetYPos();
	} else {
	  // No, la posicion en Y sera la obtenida de la clase base
	  // mas la resta de la altura maxima y la altura actual
	  return (Inherited::GetYPos() + m_PerBarInfo.uwLenght - m_PerBarInfo.QuadForm.GetHeight());
	}
  }  
  word GetWidth(void) {
	ASSERT(Inherited::IsInitOk());
	// Se devuelve la anchura
	return m_PerBarInfo.uwAreaWidth;
  }
  word GetHeight(void) {
	ASSERT(Inherited::IsInitOk());
	// Se devuelve la altura
	return m_PerBarInfo.uwAreaHeight;
  }
}; // ~ CGUICPercentageBar

#endif _CGUICPERCENTAGEBAR_H_
