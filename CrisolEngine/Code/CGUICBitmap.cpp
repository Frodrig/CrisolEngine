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
// CGUICBitmap.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUICBitmap.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUICBitmap.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia. Al inicializarse, siempre se hara teniendo en 
//   cuenta que NO estara seleccionada.
// Parametros:
// - InitData. Datos de inicializacion
// Devuelve:
// - Si todo ha ido bien true. En caso de haberse encontrado algun problema
//   false.
// Notas:
// - No se dejara reinicializar.
///////////////////////////////////////////////////////////////////////////////
bool 
CGUICBitmap::Init(const sGUICBitmapInitData& InitData)
{
  // SOLO si parametros validos
  ASSERT(InitData.ID);
  ASSERT(InitData.BitmapInfo.szFileName.size());

  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) { 
	return false; 
  }
 
  // Se inicializa clase base
  if (!Inherited::Init(InitData)) { 
	return false;  
  }

  // Se procede a inicializar el bitmap interno
  if (!m_BitmapInfo.Bitmap.Init(InitData.BitmapInfo)) {
	// Problemas
	End();
	return false;
  }
  
  // Se inicializan resto de vbles internas
  m_BitmapInfo.RGBSelectColor = InitData.RGBSelectColor;
  m_BitmapInfo.bSelect = false;

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICBitmap::End(void)
{
  // Finaliza la instancia si procede
  if (Inherited::IsInitOk()) {
	// Finaliza instancia al bitmap embebido
	m_BitmapInfo.Bitmap.End();
	// Finaliza clase base
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado del componente bitmap.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICBitmap::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Procede a dibujar si procede
  if (Inherited::IsActive()) {
	m_BitmapInfo.Bitmap.Draw(GraphDefs::DZ_GUI,
							 Inherited::GetDrawPlane(),
							 Inherited::GetXPos(),
							 Inherited::GetYPos());
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Selecciona el componente. Seleccionar significara levantar el flag de
//   seleccion y cambiar el color del bitmap asociado. No seleccionar todo 
//   el proceso inverso.
// Parametros:
// - bSelect. ¿Se quiere seleccionar o no?
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICBitmap::Select(const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Se desea seleccionar?
  if (bSelect) {
	// Si, se selecciona si procede
	if (!m_BitmapInfo.bSelect) {
	  m_BitmapInfo.bSelect = true;
	  m_BitmapInfo.Bitmap.SetRGBColor(m_BitmapInfo.RGBSelectColor);
	}
  } else {
	// No, se quita seleccion si procede
	if (m_BitmapInfo.bSelect) {
	  m_BitmapInfo.bSelect = false;
	  m_BitmapInfo.Bitmap.ResetRGBValues();
	}
  }
}
