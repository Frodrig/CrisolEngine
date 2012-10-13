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
// CCharFont.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CCharFont.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CCharFont.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la clase asociando un handle de textura, que sera donde se
//   hallen todas las fuentes, la direccion a la estructura que contiene la
//   region donde hallar el caracter que esta clase representa en la textura
//   pasada y, por ultimo, la dimension del caracter.
// Parametros:
// - hIDTexture. ID de la textura en donde se halla el caracter.
// - pTextureRgn. Region en donde se halla el caracter en la textura recibida.
// - uwWidth, uwHeight. Anchura y altura del caracter.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - Se permitira reinicializar.
///////////////////////////////////////////////////////////////////////////////
bool 
CCharFont::Init(const ResDefs::TextureHandle hIDTexture,
				GraphDefs::sTextureRegion* const pTextureRgn,
				const word uwWidth, 
				const word uwHeight)
{
  // SOLO si parametros validos
  ASSERT(hIDTexture);
  ASSERT(pTextureRgn);
  ASSERT(uwWidth);
  ASSERT(uwHeight);

  // ¿Se intenta reinicializar?
  if (IsInitOk()) {
	End();
  }

  // Se establecen valores
  m_CharFontInfo.hIDTexture = hIDTexture;
  m_CharFontInfo.pTextureRgn = pTextureRgn;
  m_CharFontInfo.uwWidth = uwWidth;
  m_CharFontInfo.uwHeight = uwHeight;

  // Establece configuracion de dibujado
  Inherited::SetVertexAlphaBlending(true);
  Inherited::SetTextureAlphaBlending(true);
  Inherited::SetColorKey(true);

  // Se levanta el flag y se retorna
  m_bIsInitOk = true;
  return true;
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia
// Parametros:
// Modifica:
// - m_CharFontInfo. Se desvinculara informacion
// - m_bIsInitOk. Se bajara el flag
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCharFont::End(void)
{
  // Finaliza la insancia si procede
  if (IsInitOk()) {
	// Se desvincula informacion
	m_CharFontInfo.hIDTexture = 0;
	m_CharFontInfo.pTextureRgn = NULL;
	m_CharFontInfo.uwWidth = 0;
	m_CharFontInfo.uwHeight = 0;

	// Se baja el flag
	m_bIsInitOk = false;
  }
}