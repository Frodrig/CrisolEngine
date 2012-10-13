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

//////////////////////////////////////////////////////////////////////
// DXDIKeyboard.cpp
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXDIKeyboard.h
//////////////////////////////////////////////////////////////////////
#include "DXDIKeyboard.h"

#include "DXInput.h"
#include "DXDIError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Constructor
// Parametros:
// Devuelve:
// Notas:
// - Llama a Init para inicializar vbles de miembro
//////////////////////////////////////////////////////////////////////
DXDIKeyboard::DXDIKeyboard(void)
{
  // Inicia vbles de miembro
  Init();  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa las vbles de miembro
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////
void DXDIKeyboard::Init(void)
{ 
  // Procede a inicializar
  m_lpDIDKeyboard = NULL;
  memset(m_KeyboardStatus, 0, 256);  
  m_bAcquired = false;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destructor
// Parametros:
// Devuelve:
// Notas:
// - Llama a Clean para liberar recursos de memoria
//////////////////////////////////////////////////////////////////////
DXDIKeyboard::~DXDIKeyboard(void)
{
  // Inicia vbles de miembro
  Clean();  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera memoria ocupada por los objetos y recursos
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////
void DXDIKeyboard::Clean(void)
{ 
  if (NULL != m_lpDIDKeyboard) {
    // Hay que liberar objeto DirectInput
    m_lpDIDKeyboard->Release();
    m_lpDIDKeyboard = NULL;
  }  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea e inicia el dispositivo de teclado.
// Parametros:
// - lpoDXI: Puntero a la clase DXInput.
// - hWnd: Manejador de la ventana
// Devuelve:
// - True si se consigue iniciar. False en otro caso
// Notas:
//////////////////////////////////////////////////////////////////////
bool DXDIKeyboard::Create(const DXInput* lpoDXI, const HWND hWnd)
{  
  // SOLO si instancia es valida
  DXASSERT(NULL != lpoDXI);
  // SOLO si existe objeto DirectInput
  DXASSERT(NULL != lpoDXI->GetObj());

  // Crea dispositivo de teclado
  m_CodeResult = lpoDXI->GetObj()->CreateDeviceEx(GUID_SysKeyboard, 
				                                          IID_IDirectInputDevice7,
					                                        (void **) &m_lpDIDKeyboard,
                                                  NULL);    
  if (FAILED(m_CodeResult)) {
    // Hay problemas 
    m_lpDIDKeyboard = NULL; 
    goto error; 
  } 

  // Establece el formato de datos del teclado, nivel cooperativo
  m_CodeResult = m_lpDIDKeyboard->SetDataFormat(&c_dfDIKeyboard);
  if (FAILED(m_CodeResult)) {
    // Hay problemas 
    goto error; 
  }

  // Establece el modo cooperativo
  m_CodeResult = m_lpDIDKeyboard->SetCooperativeLevel(hWnd,
				                                      DISCL_FOREGROUND | 
                                                      DISCL_NONEXCLUSIVE);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    goto error; 
  } 
  
  // Adquire el teclado
  if (!Acquire()) {
    // Hay problemas
    goto error; 
  } 
  
  // Se actualizan flags
  m_bAcquired = true;

  // Todo correcto
  return true;
  
error: // Han existido errores al inicializar el dispositivo de teclado
  // Se registra error
  DXDIError::DXDIErrorToString(m_CodeResult, m_sLastCode);  
  if (m_lpDIDKeyboard) {
    // Si se creo el objeto se libera
    m_lpDIDKeyboard->Release();    
    m_lpDIDKeyboard = NULL;
  }
  // Se actualizan flags
  m_bAcquired = false;

  // Hubo problemas
  return false;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Adquiere (lo intenta) el teclado
// Parametros:
// Devuelve:
// - True si se adquirio el teclado porque no lo estaba, o 
//   porque ya lo estaba. False si falla la llamada.
// Notas:
// - Cuando se llama a Acquire, el metodo devuelve DI_OK si ha logrado
//   adquirir el teclado. En caso de que ya lo estuviera, devolvera
//   S_FALSE. Hay que tener cuidado, pues, con utilizar FAILED.
// - Otra forma de comprobar si el teclado esta adquirido, despues
//   de esta llamada, sera invocar al metodo IsAdquired.
//////////////////////////////////////////////////////////////////////
bool DXDIKeyboard::Acquire(void)
{
  // SOLO si el dispositivo esta creado
  DXASSERT(NULL != m_lpDIDKeyboard);

  // Inicializaciones
  m_bAcquired = true; // Suponemos el teclado adquirido
  
  // Intenta adquirir el teclado
  m_CodeResult = m_lpDIDKeyboard->Acquire();  
  if ((S_FALSE != m_CodeResult) && (DI_OK != m_CodeResult)) {
    // Hay problemas
    m_bAcquired = false; // No se pudo adquirir
    DXDIError::DXDIErrorToString(m_CodeResult, m_sLastCode);
	return false;
  }   

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera, en caso de estar adquirido, el teclado.
// Parametros:
// Devuelve:
// - True si se libero el teclado. False si falla la llamada.
// Notas:
// - En caso de que el teclado no este adquirido, la llamada a
//   Unacquire devolvera DI_NOEFECT, indicando que la operacion no
//   ha tenido efecto alguno. En este caso, se tomara por valida
//   la llamada y se devolvera true.
//////////////////////////////////////////////////////////////////////
bool DXDIKeyboard::Unacquire(void)
{
  // SOLO si el dispositivo esta creado
  DXASSERT(NULL != m_lpDIDKeyboard);

  // Inicializaciones
  m_bAcquired = false; // En cualquier caso se baja el flag

  // Libera el dispositivo; En caso de que no estuviera adquirido
  // esta operacion no tendra efecto y se devolvera DI_NOEFECT. Por lo
  // tanto, este caso se obviara
  m_CodeResult = m_lpDIDKeyboard->Unacquire();
  if (FAILED(m_CodeResult) && (DI_NOEFFECT != m_CodeResult)) {
    // Hay problemas
    DXDIError::DXDIErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene los datos del teclado
// Parametros:
// Devuelve:
// - True si se pudo realizar la operacion. 
// - False en caso de que:
//    * El teclado no estuviera adquirido. Habra que adquirirlo 
//      desde fuera mediante una llamada a ReacquireKeyboard().
//    * Cualquier otro problema al refrescar el teclado. Comprobar
//      metodos de depuracion
// Notas:
// - Pudiera ser que el teclado no estuviera adquirido, bastara con 
//   comprobar si el valor devuelto por GetLastCodeResult es 
//   DIERR_INPUTLOST o bien, si al llamar al metodo IsAdquired se nos
//   devuelve un valor false.
//////////////////////////////////////////////////////////////////////
bool DXDIKeyboard::Update(void)
{
  // SOLO si el dispositivo esta creado
  DXASSERT(m_lpDIDKeyboard != NULL);

  // Inicializaciones
  memset(m_KeyboardStatus, 0, sizeof(m_KeyboardStatus));
  
  // Se procede a "refrescar" el teclado
  m_CodeResult = m_lpDIDKeyboard->GetDeviceState(256, &m_KeyboardStatus);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDIError::DXDIErrorToString(m_CodeResult, m_sLastCode);
    if (m_CodeResult == DIERR_INPUTLOST) {
      // Se ha perdido el dispositivo
      m_bAcquired = false; // Se actualiza flag
	  // Se intenta adquirir otra vez
	  Acquire();
    }
    return false;
  }

  // Todo correcto
  return true;
}