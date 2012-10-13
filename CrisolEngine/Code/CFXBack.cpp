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
// CFXBack.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CBitmap.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CFXBack.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia.
// Parametros:
// - DrawPos. Posicion de dibujado.
// - uwWidth. Ancho del quad.
// - uwHeight. Alto del quad.
// - RGBColor. Color de RGB a asociar.
// - Alpha. Valor alpha a asociar.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - Se permitira reinicializar
///////////////////////////////////////////////////////////////////////////////
bool 
CFXBack::Init(const sPosition DrawPos,
			  const byte ubDrawPlane,
			  const word uwWidth, 
			  const word uwHeight, 
			  const GraphDefs::sRGBColor& RGBColor, 
			  const GraphDefs::sAlphaValue& Alpha)
{
  // SOLO si parametros validos
  ASSERT(uwWidth);
  ASSERT(uwHeight);

  // Se finaliza instancia por si se quiere reinicializar
  End();

  // Se inicializa quad
  if (m_Quad.Init(uwWidth, uwHeight)) {
	// Se toman vbles de miembro
	m_ubDrawPlane = ubDrawPlane;
	m_DrawPos = DrawPos;
	m_RGBColor = RGBColor;
	m_Alpha = Alpha;

	// Se asocia color y alpha recibitos a los vertices del quad
	byte ubIt = 0;
	for (; ubIt < 4; ++ubIt) {
	  const GraphDefs::eVertex Vertex = GraphDefs::eVertex(ubIt);
	  m_Quad.SetRGBColor(Vertex, &m_RGBColor);
	  m_Quad.SetAlphaValue(Vertex, &m_Alpha);
	}

	// Todo correcto
	m_bIsInitOk = true;
	return true;
  }
  
  // Hubo problemas
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CFXBack::End(void)
{
  // Finaliza si procede
  if (IsInitOk()) {
	m_Quad.End();
	m_bIsInitOk = false;
  }
}
