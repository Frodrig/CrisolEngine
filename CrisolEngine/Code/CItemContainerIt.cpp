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
// CItemContainerIt.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CItemContainerIt.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CItemContainerIt.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia sin acudir a constructor
// Parametros:
// - Referencia al contenedor sobre el que se quiere tomar la direccion de
//   su lista de items.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CItemContainerIt::Init(iCItemContainer& ItemContainer)
{
  // Se finaliza por si se quiere reinicializar
  End();

  // Se toma direccion de la lista de items y se establece iterador al comienzo
  ItemContainer.InitItemContainerIt(*this);
  m_It = m_pItems->begin();

  // Todo correcto
  m_bIsInitOk = true;
  return true;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CItemContainerIt::End(void)
{
  // Finaliza si procede
  if (IsInitOk()) {
	// Desasocia recursos
	m_It = m_pItems->end();
	m_pItems = NULL;

	// Baja flag
	m_bIsInitOk = false;
  }
}
  