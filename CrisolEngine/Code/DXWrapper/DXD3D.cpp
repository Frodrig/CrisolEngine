///////////////////////////////////////////////////////////////////////////////
// DX7Wrapper - DirectX 7 Wrapper
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

///////////////////////////////////////////////////////////////////////
// DXD3D.cpp
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXD3D.h.
///////////////////////////////////////////////////////////////////////
#include "DXD3D.h"

#include "DXDraw.h"
#include "DXD3DError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Constructor
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
DXD3D::DXD3D(void)
{
  // Llama a Init para inicializar vbles de miembro
  Init();
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destructor
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
DXD3D::~DXD3D(void)
{
  // Llama a clean para liberar vbles de miembro
  Clean();
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de inicializar las vbles de miembro.
// Parametros:
// Devuelve:
// Notas:
// - Este metodo es llamado siempre por el constructor
////////////////////////////////////////////////////////////////////// 
void DXD3D::Init(void)
{
  // Procede a inicializar las vbles de miembro
  m_lpD3D = NULL;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de liberar los recursos
// Parametros:
// Devuelve:
// Notas:
// - Este metodo es llamado siempre por el destructor
////////////////////////////////////////////////////////////////////// 
void DXD3D::Clean(void)
{
  // Se procede a liberar el objeto D3D si procede
  if (NULL != m_lpD3D) {
    m_lpD3D->Release();
    m_lpD3D = NULL;
  }
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea el objeto Direct3D.
// Parametros:
// - pDXDraw: Instancia al objeto DirectDraw para poder crear el
//   objeto Direct3D.
// Devuelve:
// - true: Todo ha ido bien.
// - false: Ha existido algun tipo de problema.
// Notas:
// - En caso de que ya existiera un objeto Direct3D creado, este se
//   liberara.
////////////////////////////////////////////////////////////////////// 
bool DXD3D::Create(const DXDraw* const pDXDraw)
{
  // SOLO si la instancia a DXDraw es valida
  DXASSERT(NULL != pDXDraw);
  DXASSERT(NULL != pDXDraw->GetObj());

  // Si existe un obj. D3D creado, se libera
  
  if (NULL != m_lpD3D) {
    Init();
    Clean();
  }
  
  // Se procede a crear el objeto Direct3D
  m_CodeResult = pDXDraw->GetObj()->QueryInterface(IID_IDirect3D7, 
                                                   (LPVOID*)&m_lpD3D);
  if (FALSE(m_CodeResult)) {
    // Hay problemas
	DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);   
    return false;
  }

  // Todo correcto
  return true;   
}