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

////////////////////////////////////////////////////////////////////////
// DXInput.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXInput.h
////////////////////////////////////////////////////////////////////////
#include "DXInput.h"

#include "DXDIError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Constructor de la clase de DXInput
// Parametros:
// Devuelve:
// Notas:
// - Llama Init para realizar la inicializacion
//////////////////////////////////////////////////////////////////////
DXInput::DXInput(void)
{
  // Llamada a Init para inicializacion de vbles
  Init();
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa las vbles de miembro
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////
void DXInput::Init(void)
{
  // Procede a inicializar vbles de miembro
  m_lpDI = NULL;
  m_Inst = NULL;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destructor de DXInput
// Parametros:
// Devuelve:
// Notas:
// - Llama a Clean para liberar memoria
//////////////////////////////////////////////////////////////////////
DXInput::~DXInput(void)
{
  // Llamada a Clean para inicializacion de vbles
  Clean();
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera de memoria punteros y recursos
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////
void DXInput::Clean(void)
{
  // Se liberan punteros y objetos
  if (NULL != m_lpDI) {
    // Hay que liberar objeto DirectInput
    m_lpDI->Release();
    m_lpDI = NULL;
  }  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea el objeto DirectInput
// Parametros:
// - hInst: Instancia de la aplicacion
// Devuelve:
// - True si se consigio iniciarlo con exito. False
//   si no se pudo.
// Notas:
//////////////////////////////////////////////////////////////////////
bool DXInput::Create(const HINSTANCE& hInst)
{
  // SOLO si hInst es una instancia valida
  DXASSERT(NULL != hInst);
  
  // Crea el objeto DirectInput
  m_CodeResult = DirectInputCreateEx(hInst, 
                                     DIRECTINPUT_VERSION,
		                              IID_IDirectInput7, 
                                     (void**)&m_lpDI, 
                                     NULL);	
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDIError::DXDIErrorToString(m_CodeResult, m_sLastCode);
    m_lpDI = NULL;
    return false;
  }	

  // Todo correcto
  return true;
}

