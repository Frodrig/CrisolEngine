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
// CGUICJustifyText.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUICJustifyText.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUICJustifyText.h"

#include "SYSEngine.h"
#include "iCFontSystem.h"
#include "iCFont.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
//- Inicializa la instancia tomando los datos base y pasando la responsabilidad
//  luego a CGUIComponent.
// Parametros:
// - InitData. Datos de inicializacion.
// Devuelve:
// - Si todo bien true. En caso contrario false.
// Notas:
// - No se dejara reinicializar.
///////////////////////////////////////////////////////////////////////////////
bool 
CGUICJustifyText::Init(const sGUICJustifyTextInitData& InitData)
{
  // SOLO si parametros validos
  ASSERT(InitData.ID);
  ASSERT(InitData.szFont.size());
  ASSERT(InitData.uwWidthJustify);

  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) { return false; }

  // Se iniciliza clase base
  if (!Inherited::Init(InitData)) {
	// Problemas
	return false;
  }

  // Se toman vbles de miembro
  m_TextInfo.szFont = InitData.szFont;
  m_TextInfo.uwWidthJustify = InitData.uwWidthJustify;
  m_TextInfo.RGBSelectColor = InitData.RGBSelectColor;
  m_TextInfo.RGBUnselectColor = InitData.RGBUnselectColor;

  // Se deselecciona el texto
  Select(false);

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia
// Parametros:
// Devuelve:
// Notas:
// - Al desinstalar, se desinstalara a su vez la fuente que se hubiera 
//   instalado.
///////////////////////////////////////////////////////////////////////////////
void 
CGUICJustifyText::End(void)
{
  // Se desinstala si procede
  if (Inherited::IsInitOk()) {
	// Se finaliza clase base
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Justifica el texto recibido en szText de tal manera que devuelve la 
//   posicion final, a partir de uwInitPos, en donde todo el texto que lo
//   comprende son palabras completas que caben en los pixels definidos en
//   m_TextInfo.uwWidthJustify. El valor en uwEndPos acota la posicion maxima
//   por la que se podra navegar en el string.
// Parametros:
// - szText. Texto sobre el que realizar la justificacion.
// - swInitPos. Posicion dentro del string donde comenzar a justificar.
// - swEndPos. Posicion del string maxima donde poder acceder. Se debera de
//   cumplir que uwEndPos sea menor que el tamaño del string. En el caso
//   de que su valor sea 0, se querra decir que se quiere iterar teniendo
//   como tope el tamaño del string. Por defecto vale 0.
// Devuelve:
// - Posicion en el string donde se podra ir desde uwInitPos respetando la
//   justificacion descrita. En caso de devolver 0 querra decir que el string,
//   desde la posicion actual, contiene una palabra tan larga que no cabe
//   en el espacio reservado.
// Notas:
// - Los caracteres de nueva linea seran tratados de tal forma que si durante
//   la justificacion se encuentra uno, se retorna inmediatamente.
///////////////////////////////////////////////////////////////////////////////
sword 
CGUICJustifyText::JustifyText(const std::string& szText, 
							  const sword& swInitPos,
							  const sword& swEndPos)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(szText.size());
  ASSERT((swInitPos >= 0) != 0);  

  // Pixels disponibles  
  sword swDispPixels = m_TextInfo.uwWidthJustify;
  
  // Iterador para recorrer las posiciones del string
  sword swItPos = swInitPos;
  
  // Posicion maxima donde puede llegar uwItPos
  const sword swMaxPos = (0 != swEndPos) ? swEndPos + 1 : szText.size();

  // Se inicia la justificacion encontrando la posicion maxima para el
  // iterador uwItPos, sin tener en cuenta si se termina en una palabra
  // incompleta, pero controlando caracteres saltos de linea, que se este
  // en la posicion tope y que haya espacio disponible en pixels  
  iCFont* const pFont = SYSEngine::GetFontSystem()->GetFont(m_TextInfo.szFont);
  ASSERT(pFont);
  while (swItPos < swMaxPos && szText[swItPos] != '\n') {
	// Se halla el nuevo espacio disponible
	swDispPixels -= pFont->GetCharWidth(szText[swItPos]);

	// ¿Es un espacio correcto?	
	if (swDispPixels > 0) {
	  ++swItPos;	  	  
	} else {
	  break; 
	} 
  }

  // ¿Se llego a nueva linea o fuera de una palabra o al final del texto?
  if ('\n' == szText[swItPos] || 
	  ' ' == szText[swItPos] || 
	  swItPos == swMaxPos) { 
	return swItPos; 
  }

  // Se esta en mitad de una palabra
  // Hay que retroceder posicion hasta localizarnos al comienzo de la palabra
  while (swItPos > swInitPos && ' ' != szText[swItPos]) { 
	--swItPos; 
  }
  
  // Se devuelve el resultado final. Si estamos ante una palabra que ocupa
  // todo el espacio disponible, se devolvera 0
  return (swItPos == swInitPos && szText[swItPos + 1] != ' ') ? 0 : swItPos;
}
