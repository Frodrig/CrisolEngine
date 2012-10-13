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
// CShadow.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CShadow.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CShadow.h"

#include "iCGFXManager.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia, pasando el nombre de la imagen con la 
//   sombra a utilizar.
// Parametros:
// - szImageFileName. Nombre de la imagen.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CShadow::Init(const std::string& szImageFileName)
{
  // Se finaliza por si se intenta reinicializar
  End();

  // Se carga imagen
  if (m_Image.Init(szImageFileName, 
				   GraphDefs::BPP_16, 
				   GraphDefs::ALPHA_BPP_1)) {
	// Se asocia el alpha a los vertices
	iCGFXManager* const pGFXManager = SYSEngine::GetGFXManager();
	ASSERT(pGFXManager);
	byte ubIt = 0;
	for (; ubIt < 4; ++ubIt) {  	  
	  m_Image.SetAlphaValue(GraphDefs::eVertex(ubIt), pGFXManager->GetAlphaShadow());
	}

	// Se establecen resto de vbles de miembro
	m_szImageFileName = szImageFileName;
	m_bIsInitOk = true;
  }
  
  // Se retorna resultado
  return IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CShadow::End(void)
{
  // Finaliza si procede
  if (IsInitOk()) {
	m_Image.End();
	m_bIsInitOk = false;
  }
}
