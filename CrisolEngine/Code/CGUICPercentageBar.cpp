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
// CGUIComponent.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIComponent.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUICPercentageBar.h"

#include "SYSEngine.h"
#include "iCGraphicSystem.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia apoyandose en la clase base. Los datos fundamentales
//   para esta instancia estaran relacionados con los valores numericos para la
//   barra de porcentaje.
// Parametros:
// - InitData. Info de inicializacion.
// Devuelve:
// - Si todo va correctamente true, en caso contrario false.
// Notas:
// - No se permitira reinicializar.
// - El tamaño y el grosor siempre estara supeditado al tipo de barra. En el
//   caso de las barras horizontales, el valor uwSize determinara la anchura y
//   uwWidth la altura. En el caso de las barras verticales, uwSize determinara
//   la altura y uwWidth la anchura.
// - El color tambien estara supeditado al tipo de barra con el que estemos
//   trabajando. Para el caso de barras horizontales, los vertices Start seran
//   v0 y v2 y los End v1 y v3. En el caso de las barras verticales, los
//   vertices Start v0 y v1 y los End v2 y v3.
// - Por defecto se comenzara NO seleccionado.
///////////////////////////////////////////////////////////////////////////////
bool 
CGUICPercentageBar::Init(const sGUICPercentageBarInitData& InitData)
{
  // SOLO si parametros validos
  ASSERT(InitData.swBaseValue);

  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) { 
	return false; 
  }
  
  // Se inicializa segun sea el tipo de barra
  if (InitData.bDrawHoriz) {
	// Dibujado en horizontal
	// Anchura y altura	
	m_PerBarInfo.uwAreaWidth = InitData.uwLenght;
	m_PerBarInfo.uwAreaHeight = InitData.uwWidth;

	// Se establecen los vertices
	m_PerBarInfo.VertexOrder[0] = GraphDefs::VERTEX_0;
	m_PerBarInfo.VertexOrder[1] = GraphDefs::VERTEX_2;
	m_PerBarInfo.VertexOrder[2] = GraphDefs::VERTEX_1;
	m_PerBarInfo.VertexOrder[3] = GraphDefs::VERTEX_3;
  } else {
	// Dibujado en vertical
	// Anchura y altura
	m_PerBarInfo.uwAreaWidth = InitData.uwWidth;
	m_PerBarInfo.uwAreaHeight = InitData.uwLenght;

	// Se establecen los vertices
	m_PerBarInfo.VertexOrder[0] = GraphDefs::VERTEX_2;
	m_PerBarInfo.VertexOrder[1] = GraphDefs::VERTEX_3;
	m_PerBarInfo.VertexOrder[2] = GraphDefs::VERTEX_0;
	m_PerBarInfo.VertexOrder[3] = GraphDefs::VERTEX_1;
  }
  
  // Se inicializa la clase base
  if (!Inherited::Init(InitData)) {	
	return false; 
  }

  // Se inicializa la forma quad de la barra
  if (!m_PerBarInfo.QuadForm.Init(m_PerBarInfo.uwAreaWidth, 
								  m_PerBarInfo.uwAreaHeight)) {
	End();
	return false;
  }
  
  // Se establecen colores
  GraphDefs::sRGBColor RGBMaxStart(0, 255, 0);
  GraphDefs::sRGBColor RGBMaxEnd(0, 155, 0);
  GraphDefs::sRGBColor RGBMedStart(255, 200, 0);
  GraphDefs::sRGBColor RGBMedEnd(155, 100, 0);
  GraphDefs::sRGBColor RGBMinStart(255, 0, 0);
  GraphDefs::sRGBColor RGBMinEnd(155, 0, 0);		
  m_PerBarInfo.RGBMaxValue[0] = RGBMaxStart;
  m_PerBarInfo.RGBMaxValue[1] = RGBMaxEnd;
  m_PerBarInfo.RGBMedValue[0] = RGBMedStart;
  m_PerBarInfo.RGBMedValue[1] = RGBMedEnd;
  m_PerBarInfo.RGBMinValue[0] = RGBMinStart;
  m_PerBarInfo.RGBMinValue[1] = RGBMinEnd;
  
  // Se almacenan resto de vbles de miembro
  m_PerBarInfo.Alpha = InitData.Alpha;
  m_PerBarInfo.bDrawHoriz = InitData.bDrawHoriz;
  m_PerBarInfo.swBaseValue = InitData.swBaseValue;
  m_PerBarInfo.swTempValue = InitData.swTempValue;
  m_PerBarInfo.bSelect = false;
  m_PerBarInfo.uwLenght = InitData.uwLenght;

  // Se establece el porcentaje de la barra y valor alpha  
  SetPercentageBarSize();    
  SetAlphaValue();  
  
  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia liberando recursos.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICPercentageBar::End(void)
{
  // Se finaliza instancia
  if (Inherited::IsInitOk()) {
	// Se finaliza instancia QuadForm
	m_PerBarInfo.QuadForm.End();

	// Se finaliza clase base
	Inherited::End();	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el tamaño de la barra de porcentaje segun sea el valor asociado
//   a temporal y base.
// Parametros:
// Devuelve:
// Notas:
// - Este metodo sera de apoyo y se invocara siempre que cambie el valor
//   base o temporal o ambos.
// - Desde aqui se llamara al metodo para establecer el color asociado
//   a la barra segun porcentaje.
///////////////////////////////////////////////////////////////////////////////
void 
CGUICPercentageBar::SetPercentageBarSize(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se establece el valor base, en caso de pasar valor 0, se "camuflara" como 1
  const float fBase = (m_PerBarInfo.swBaseValue > 0) ? m_PerBarInfo.swBaseValue : 1.0f;
  
  // Calcula la forma de la barra de porcentaje segun su valor temp y base  
  const float fPercentage = float(m_PerBarInfo.swTempValue) / fBase;
  float fSize = Min(1.0f, fPercentage) * m_PerBarInfo.uwLenght;	
  if (0.0f == fSize) { 
	fSize = 1.0f; 
  }
  
  // Ajusta el tamaño de la barra segun sea su forma
  if (m_PerBarInfo.bDrawHoriz) {
	m_PerBarInfo.QuadForm.ChangeWidth(fSize);
  } else {	
	m_PerBarInfo.QuadForm.ChangeHeight(fSize);	  	
  };

  // Establece el color asociado segun porcentaje
  SetRGBColor(fPercentage);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia el color RGB a la barra atendiendo al tipo de barra, vertical u
//   horizonal, y al valor de porcentaje que tiene la barra. En el caso de
//   un porcentaje de 50%..100% se asociara RGBMaxValue, en el caso de 
//   25%..49% se asociara RGBMedValue y en el caso de 0%..24% se asociara
//   RGBMinValue.
// Parametros:
// - fPercentage. Porcentaje de la barra.
// Devuelve:
// Notas:
// - Este metodo se llamara desde SetPercentageBarSize, luego el porcentaje
//   fPercentage recibido sera el actual de la barra.
///////////////////////////////////////////////////////////////////////////////
void 
CGUICPercentageBar::SetRGBColor(const float fPercentage)
{
  // Vbles
  GraphDefs::sRGBColor* pRGBStartColor; // Dir. del color a poner para inicio
  GraphDefs::sRGBColor* pRGBEndColor;   // Dir. del color a poner para final

  // Se elige estructura RGB segun porcentaje  
  if (fPercentage > 0.5f) {
	pRGBStartColor = &m_PerBarInfo.RGBMaxValue[0];
	pRGBEndColor = &m_PerBarInfo.RGBMaxValue[1];	
  } else if (fPercentage > 0.25f) {
	pRGBStartColor = &m_PerBarInfo.RGBMedValue[0];
	pRGBEndColor = &m_PerBarInfo.RGBMedValue[1];
  } else {
	pRGBStartColor = &m_PerBarInfo.RGBMinValue[0];
	pRGBEndColor = &m_PerBarInfo.RGBMinValue[1];
  }

  // Establece el color en el quad
  ASSERT(pRGBStartColor);
  ASSERT(pRGBEndColor);
  m_PerBarInfo.QuadForm.SetRGBColor(m_PerBarInfo.VertexOrder[0], pRGBStartColor);
  m_PerBarInfo.QuadForm.SetRGBColor(m_PerBarInfo.VertexOrder[1], pRGBStartColor);  
  m_PerBarInfo.QuadForm.SetRGBColor(m_PerBarInfo.VertexOrder[2], pRGBEndColor);
  m_PerBarInfo.QuadForm.SetRGBColor(m_PerBarInfo.VertexOrder[3], pRGBEndColor);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia a todos los vertices de la barra, la direccion de la estructura
//   con el valor alpha deseado si y solo si, el valor alpha es menor que 255
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICPercentageBar::SetAlphaValue(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Hay alpha para asociar?
  if (m_PerBarInfo.Alpha.ubAlpha < 255) {
	m_PerBarInfo.QuadForm.SetAlphaValue(GraphDefs::VERTEX_0, 
										&m_PerBarInfo.Alpha);
	m_PerBarInfo.QuadForm.SetAlphaValue(GraphDefs::VERTEX_1, 
										&m_PerBarInfo.Alpha);
	m_PerBarInfo.QuadForm.SetAlphaValue(GraphDefs::VERTEX_2, 
										&m_PerBarInfo.Alpha);
	m_PerBarInfo.QuadForm.SetAlphaValue(GraphDefs::VERTEX_3, 
										&m_PerBarInfo.Alpha);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se realiza el dibujado de la barra de porcentaje SOLO si esta se halla
//   activa.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICPercentageBar::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());  
  // Se dibuja si procede
  if (Inherited::IsActive()) {	
	SYSEngine::GetGraphicSystem()->Draw(GraphDefs::DZ_GUI,
										Inherited::GetDrawPlane(),
										GetXPos(),
										GetYPos(),
										&m_PerBarInfo.QuadForm);
  }
}
