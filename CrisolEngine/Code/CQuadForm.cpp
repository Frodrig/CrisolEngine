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
// CQuadForm.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CQuadForm.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CQuadForm.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia estableciendo dimensiones del quad asociado. 
// Parametros:
// - uwWidth, uwHeight. Anchura y altura.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - Se podra reinicializar.
///////////////////////////////////////////////////////////////////////////////
bool 
CQuadForm::Init(const word uwWidth, 
				const word uwHeight)
{
  // Se finaliza por si hay reinicio
  End();

  // Se toman los valores recibidos  
  m_QuadInfo.uwWidth = uwWidth;
  m_QuadInfo.uwHeight = uwHeight;

  // Activa el vertex alpha blending
  Inherited::SetVertexAlphaBlending(true);
  
  // Se levanta flag y se retorna.
  m_bIsInitOk = true;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia
// Parametros:
// Modifica:
// - m_bQuadInfo. Se pondran a NULL tablas de color y alpha
// - m_bIsInitOk. Se bajara el flag
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CQuadForm::End(void)
{
  // Finaliza la insancia si procede
  if (IsInitOk()) {
	// Se baja el flag
	m_bIsInitOk = false;
  }
}

