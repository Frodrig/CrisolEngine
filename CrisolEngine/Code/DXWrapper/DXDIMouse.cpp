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
// DXDIMouse.cpp
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXDIMouse.h
//////////////////////////////////////////////////////////////////////
#include "DXDIMouse.h"

#include "DXInput.h"
#include "DXDIError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Constructor.
// Parametros:
// Devuelve:
// Notas:
// - Realiza llamada a Init para inicializar vbles de miembro.
//////////////////////////////////////////////////////////////////////
DXDIMouse::DXDIMouse(void)
{
  // Procede a inicializar vbles de miembro
  Init();
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa las vbles de miembro
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////
void DXDIMouse::Init(void)
{ 
  // Procede a inicializar vbles
  m_lpDIDMouse = NULL;
  m_bMouseAcquired = false;
  memset(m_bButtonsState, false, DXDIDefs::MOUSEBUTTONS);
  memset(m_nButtonsSubState, DXDIDefs::MOUSE_STATE_UNPRESSED, DXDIDefs::MOUSEBUTTONS);  
  m_nPosX = m_nPosY = 0;  
  m_nXDelta = m_nYDelta = m_nZDelta = 0;
  m_MovArea.left = m_MovArea.top = 0;   
  m_MovArea.bottom = m_MovArea.right = 0;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destructor.
// Parametros:
// Devuelve:
// Notas:
// - Realiza llamada a Done para liberar recursos.
//////////////////////////////////////////////////////////////////////
DXDIMouse::~DXDIMouse(void)
{
  // Libera recursos
  Clean();
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera recursos que esten en memoria como punteros.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////
void DXDIMouse::Clean(void)
{ 
  // Procede a liberar recursos
  if (NULL != m_lpDIDMouse) {
    // Hay que liberar dipositivo
    m_lpDIDMouse->Release();
    m_lpDIDMouse = NULL;
  }  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un dispositivo de tipo raton, a partir del objeto 
//   DirectInput encapsulado y recibido en lpDXI. Tambien establece
//   el area de movimiento valido para el raton, llamando a SetMovArea.
// Parametros:
// - lpoDXI: Puntero a la calse DXInput
// - hWnd: Manejador de la ventana
// - nAreaLeft, nAreaTop, nAreaRight, nAreaBottom. Puntos que determinan
//   el area en donde se podra mover el raton.
// Devuelve:
// - true si todo ha ido bien y false en caso contrario.
// Notas:
//////////////////////////////////////////////////////////////////////
bool DXDIMouse::Create(DXInput* const lpoDXI, 
					   const HWND& hWnd, 
					   const int& nAreaLeft, const int& nAreaTop,
					   const int& nAreaRight, const int& nAreaBottom)
{
  // SOLO si instancia es valida
  DXASSERT(NULL != lpoDXI);
  // SOLO si existe objeto DirectInput
  DXASSERT(NULL != lpoDXI->GetObj());

  // Crea dispositivo de tipo raton  
  m_CodeResult = lpoDXI->GetObj()->CreateDeviceEx(GUID_SysMouse, 
				                                  IID_IDirectInputDevice7,
					                              (void **) &m_lpDIDMouse,
                                                  NULL);      
  if (FAILED(m_CodeResult)) { 
    // Hay problemas    
    // Se evita llamada a Release al no haberse creado el objeto
    m_lpDIDMouse = NULL; 
    goto error;    
  }

  // Se establece el formato de datos del teclado
  m_CodeResult = m_lpDIDMouse->SetDataFormat(&c_dfDIMouse);
  if (FAILED (m_CodeResult)) {
    // Hay problemas
    goto error; 
  }

  // Se procede a establecer el modo cooperativo
  m_CodeResult = m_lpDIDMouse->SetCooperativeLevel(hWnd, 
                                                   DISCL_EXCLUSIVE | 
                                                   DISCL_FOREGROUND);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    goto error;
  }

  // Se asocia evento al dispositivo, para saber cuando hay datos
  if (!SetEventNotification()) {
    // Hay problemas
    goto error;
  }

  // Se establece el tamaño del buffer para la recogida de datos
  if (!SetBufferedInput()) {
    // Hay problemas
    goto error;
  }

  // Se adquiere el dispositivo
  if (!Acquire()) {
    // Hay problemas
    goto error;
  }

  // Se establece el area de movimiento
  RECT MovArea; 
  MovArea.left = nAreaLeft;
  MovArea.top = nAreaTop;
  MovArea.right = nAreaRight;
  MovArea.bottom = nAreaBottom;
  if (!SetMovArea(MovArea)) {
    // Hay problemas
    goto error;
  }

  // Todo correcto
  return true;

  // Han existido errores al inicializar el dispositivo de teclado
error:  
  // Se registra error
  DXDIError::DXDIErrorToString(m_CodeResult, m_sLastCode);  
  if (m_lpDIDMouse) {
    // Si se creo el objeto se libera
    m_lpDIDMouse->Release();    
    m_lpDIDMouse = NULL;
  }
  // Se bajan flags de inicializacion
  m_bMouseAcquired = false;

  // Hubo problemas
  return false;	
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia un evento al raton de tal forma que, en cuanto ocurra un
//   cambio de estado en el mismo, una interrupcion hardware indicara
//   que hay datos disponibles.
// Parametros:
// Devuelve:
// - True si todo ha ido bien y se ha podido crear el evento. False
//   en caso contrario.
// Notas:
// - IMPORTANTE: Pudiera ocurrir que se devolviera false y el valor
//   devuelto por GetLastCodeResult ser igual a DI_OK. Esto sera
//   debido a que el fallo ha sido producido por una llamada a 
//   una funcion del API de Windows. Bastara llamara GetLastStrCode y
//   obtendremos el mensaje que ha producido el error.
//////////////////////////////////////////////////////////////////////
bool DXDIMouse::SetEventNotification(void)
{ 
  // SOLO si existe dispositivo teclado
  DXASSERT(NULL != m_lpDIDMouse);
  
  // Crea evento para el raton
  m_hMouseEvent = CreateEvent(0, 0, 0, 0);
  if (m_hMouseEvent == NULL) {
    // Hay problemas utilizando el API de Windows, al crear un evento
    m_CodeResult = DI_OK; // Ya que el problema no depende de las DirectX
    m_sLastCode = DXDIKEYBOARD_NOEVENTCREATE_MSG;
    return false;
  }

  // Una vez creado el evento, se asocia al dispositivo de raton
  m_CodeResult = m_lpDIDMouse->SetEventNotification(m_hMouseEvent);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDIError::DXDIErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Todo correcto
  return true;    
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el tamaño maximo para el buffer de DirectInput
// Parametros:
// Devuelve:
// - True si todo ha ido bien y se ha podido crear el buffer. 
//   False en caso contrario.
// Notas:
//////////////////////////////////////////////////////////////////////
bool DXDIMouse::SetBufferedInput(void)
{ 
  // SOLO si existe dispositivo teclado
  DXASSERT(NULL != m_lpDIDMouse);
  
  DIPROPDWORD dipdw =
  {
    // Cabecera
    {
      sizeof(DIPROPDWORD),    // diph.dwSize
      sizeof(DIPROPHEADER),   // diph.dwHeaderSize
      0,                      // diph.dwObj
      DIPH_DEVICE,            // diph.dwHow
    },
    // Dato
	DXDIDefs::MOUSEBUFFERSIZE,          // dwData
  }; 

  // Establece las propiedades
  m_CodeResult = m_lpDIDMouse->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDIError::DXDIErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Todo correcto
  return true;    
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el area de movimiento para el raton.
// Parametros:
// - MovArea: Nueva area de movimiento del raton
// Devuelve:
// - true si el area pasado es valido y false en caso contrario.
// Notas:
// - No se permitiran areas de movimiento negativas.
// - Por defecto, la posicion del raton se situara en la esquina 
//   superior izquierda del area.
// - Notese que si se obtiene error, el llamar a GetLastCodeResult nos
//   devolvera DI_OK al no ser, este metodo, dependiente del API de
//   DirectX.
//////////////////////////////////////////////////////////////////////
bool DXDIMouse::SetMovArea(const RECT& MovArea)
{
  // SOLO si existe dispositivo teclado
  DXASSERT(NULL != m_lpDIDMouse);

  // Inicializaciones
  m_CodeResult = DI_OK;

  if (MovArea.top < 0 || MovArea.left < 0 || 
      MovArea.right < 0 || MovArea.bottom < 0) {
    // No se acepta un area negativa    
    m_sLastCode = DXDIMOUSE_AREAINCORRECT_MSG;
    return false;
  }

  if (MovArea.top > MovArea.bottom ||
      MovArea.left > MovArea.right) {
    // Area incorrecta
    m_sLastCode = DXDIMOUSE_AREAINCORRECT_MSG;
    return false;
  }

  // Actualizamos area de movimiento del raton 
  m_MovArea = MovArea;

  // Actualizamos coordenadas del raton
  m_nPosX = m_MovArea.left;
  m_nPosY = m_MovArea.top;
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece nuevas coordenadas del raton, siempre que estas caigan
//   dentro del area de movimiento.
// Parametros:
// - dwPosX y dwPosY: Nuevas posiciones x e y, respectivamente, 
//   del raton.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////
void DXDIMouse::SetMousePos(const int& nPosX, 
							const int& nPosY)
{
  // SOLO si existe dispositivo teclado
  DXASSERT(NULL != m_lpDIDMouse);

  // Inicializaciones
  m_CodeResult = DI_OK;

  // Actualizamos posiciones del raton
  m_nPosX = nPosX;
  m_nPosY = nPosY;

  // Realizamos clipping si procede
  if (m_nPosY < m_MovArea.top) { m_nPosY = m_MovArea.top; }
  else if (m_nPosY > m_MovArea.bottom) { m_nPosY = m_MovArea.bottom; }
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Intenta readquirir el raton. Si ya estuviera adquirido, el 
//   metodo no tendra ningun efecto.
// Parametros:
// Devuelve:
// - true si se readquirio el raton. False si no se pudo u courrio
//   cualquier otro problema.
// Notas:
// - Cuando se llama a Acquire, el metodo devuelve DI_OK si ha logrado
//   adquirir el teclado. En caso de que ya lo estuviera, devolvera
//   S_false. Hay que tener cuidado, pues, con utilizar FAILED.
// - Otra forma de comprobar si el teclado esta adquirido, despues
//   de esta llamada, sera invocar al metodo IsAdquired.
//////////////////////////////////////////////////////////////////////
bool DXDIMouse::Acquire(void)
{
  // SOLO si el dispositivo esta creado
  DXASSERT(NULL != m_lpDIDMouse);

  // Inicializaciones
  m_bMouseAcquired = true;
  
  // Intengo adquirir otra vez el teclado
  m_CodeResult = m_lpDIDMouse->Acquire();  
  if ((m_CodeResult != S_FALSE) && (m_CodeResult != DI_OK)) {
    // Hay problemas    
    DXDIError::DXDIErrorToString(m_CodeResult, m_sLastCode);
    m_bMouseAcquired = false;
	  return false;
  }
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera, en caso de estar adquirido, el raton.
// Parametros:
// Devuelve:
// - True si se libero el raton. False si falla la llamada.
// Notas:
// - En caso de que el teclado no este adquirido, la llamada a
//   Unacquire devolvera DI_NOEFECT, indicando que la operacion no
//   ha tenido efecto alguno. En este caso, se tomara por valida
//   la llamada y se devolvera true.
//////////////////////////////////////////////////////////////////////
bool DXDIMouse::Unacquire(void)
{
  // SOLO si el dispositivo esta creado
  DXASSERT(m_lpDIDMouse != NULL);

  // Libera el dispositivo; En caso de que no estuviera adquirido
  // esta operacion no tendra efecto y se devolvera DI_NOEFECT. Por lo
  // tanto, este caso se obviara
  m_CodeResult = m_lpDIDMouse->Unacquire();
  if (FAILED(m_CodeResult) && (m_CodeResult != DI_NOEFFECT)) {
    // Hay problemas
    DXDIError::DXDIErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
  
  // Se actualiza flag de control
  m_bMouseAcquired = false;
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene los datos que esten en el buffer del raton, para
//   procesarlos y actualizar convenientemente los valores.
// Parametros:
// Devuelve:
// - True si la actualizacion ha ido bien y False si ha ocurrido algun
//   problema al solicitar los datos.
// Notas:
// - Antes de comprobar los valores de posicion y estado de los
//   botones del raton, se debera de llamar a este metodo para que 
//   actualice los datos. En caso de no hacerlo, se estara trabajando
//   con los datos de la ultima actualizacion que se hizo.
// - Pudiera ser que el raton no estuviera adquirido, bastara con 
//   comprobar si el valor devuelto por GetLastCodeResult es 
//   DIERR_INPUTLOST o bien, si al llamar al metodo IsAdquired se nos
//   devuelve un valor false. Si ocurriera esto, habra que llamar a 
//   Acquire y despues volver a Update.
// - Recordar que el boton 0 equivale al boton izquierdo y 1 al derecho.
// - Para mas informacion sobre los subestados de los botones, 
//   consultar documentacion.
//////////////////////////////////////////////////////////////////////
bool DXDIMouse::Update(void)
{
  // SOLO si el dispositivo esta creado
  DXASSERT(m_lpDIDMouse != NULL);
  // Buffer con los datos
  DIDEVICEOBJECTDATA BuffData[DXDIDefs::MOUSEBUFFERSIZE]; 
  DWORD              dwItems; // Num. datos en buffer
  DWORD              dwIndex; // Vble auxiliar
  
  // Inicializaciones
  dwItems = DXDIDefs::MOUSEBUFFERSIZE;
  m_nXDelta = m_nYDelta = m_nZDelta = 0;
  
  // Se actualiza los estados de los botones, segun estos se encuentren
  // o no pulsados. Para mas informacion sobre el funcionamiento, consultar
  // documentacion o el apartado de Notas de la cabecera de este metodo
  for (dwIndex = 0; dwIndex < DXDIDefs::MOUSEBUTTONS; dwIndex++) {
    // Actualiza estado de los botones
    if (m_bButtonsState[dwIndex]) {
      // Si boton presionado
      if (m_nButtonsSubState[dwIndex] == DXDIDefs::MOUSE_STATE_PRESSEDDOWN) {
        // Si en el estado anterior se presiono el boton por primera vez,
        // pasara a estar en estado de pulsacion permanente
        m_nButtonsSubState[dwIndex] = DXDIDefs::MOUSE_STATE_HOLDDOWN;
      }
    }
    else {
      // Si boton no presionado
      if (m_nButtonsSubState[dwIndex] != DXDIDefs::MOUSE_STATE_UNPRESSED) { 
        // Si no esta en estado sin pulsar, hacemos avanzar estados hasta
        // llegar a el
        switch (m_nButtonsSubState[dwIndex])
        {// Emula el paso del estado actual al de sin pulsacion
          case DXDIDefs::MOUSE_STATE_PRESSEDDOWN: // Si recien pulsado a pulsacion permanente
            m_nButtonsSubState[dwIndex] = DXDIDefs::MOUSE_STATE_HOLDDOWN;
            break;

          case DXDIDefs::MOUSE_STATE_HOLDDOWN: // Si en pulsacion permanente a liberado
            m_nButtonsSubState[dwIndex] = DXDIDefs::MOUSE_STATE_RELEASED;
            break;

         case DXDIDefs::MOUSE_STATE_RELEASED: // Si recien liberado a sin presionar
			m_nButtonsSubState[dwIndex] = DXDIDefs::MOUSE_STATE_UNPRESSED;            
            break;
        }; // Del switch
      } // Del if
    } // Del else
  } // Del for  
  
  
  // Se obtienen datos del dispositivo
  m_CodeResult = m_lpDIDMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),
    				                                 BuffData, 
                                             &dwItems, 
                                             0);
  if (FAILED(m_CodeResult)) {
    // Hay problemas      
     DXDIError::DXDIErrorToString(m_CodeResult, m_sLastCode);
     if (m_CodeResult == DIERR_INPUTLOST) {
       // Se ha perdido el dispositivo
        m_bMouseAcquired = false; // Se actualiza flag
		    // Se intenta adquirir
		    Acquire();
     }
     return false;
  }

  // Se obtienen datos del raton hasta que no haya nada en buffer
  for (dwIndex = 0; dwIndex < dwItems; dwIndex++) {
    // Hay elementos que comprobar    
    bool   bActButton = false; // Flag para actualizacion de algun boton
    int    nButton;            // Indice del boton a actualizar

    switch (BuffData[dwIndex].dwOfs)
    {// Seleccionamos el tipo de evento recibido
      case DIMOFS_X: // Hay desplazamiento en x        
        // Se toma nuevo valor de desplazamiento y actualiza coordenada en X
        m_nXDelta += BuffData[dwIndex].dwData;
        m_nPosX += m_nXDelta;
        // Se realiza operacion de clipping 
        if (m_nPosX < m_MovArea.left) { m_nPosX = m_MovArea.left; }
        else if (m_nPosX > m_MovArea.right) { m_nPosX = m_MovArea.right; }
        break;
      
      case DIMOFS_Y: // Hay desplazamiento en y
        // Se toma nuevo valor de desplazamiento y actualiza coordenada en Y
        m_nYDelta += BuffData[dwIndex].dwData; 
        m_nPosY += m_nYDelta;
        // Se realiza operacion de clipping 
        if (m_nPosY < m_MovArea.top) { m_nPosY = m_MovArea.top; }
        else if (m_nPosY > m_MovArea.bottom) { m_nPosY = m_MovArea.bottom; }
        break;
      
      case DIMOFS_Z: // Hay desplazamiento en z <rueda>
        m_nZDelta += BuffData[dwIndex].dwData;
        break;
        
      case DIMOFS_BUTTON0: // Hay que actualizar el boton 0
        // Se levanta flag y se indica el boton
        bActButton = true;
        nButton = 0;
        break;
      
      case DIMOFS_BUTTON1: // Hay que actualizar el boton 1
        // Se levanta flag y se indica el boton
        bActButton = true;
        nButton = 1;
        break;

      case DIMOFS_BUTTON2: // Hay que actualizar el boton 2
        // Se levanta flag y se indica el boton
        bActButton = true;
        nButton = 2;
        break;

      case DIMOFS_BUTTON3: // Hay que actualizar el boton 3
        // Se levanta flag y se indica el boton
        bActButton = true;
        nButton = 3;
        break;
    }; // Del switch

    // En caso de que haya que actualizar algun boton del raton
    // llevamos el proceso fuera del switch pues es codigo comun para
    // todos los botones. Con saber el indice, en nButton, nos basta
    if (bActButton) {
      // Hay que actualizar algun boton
      if (BuffData[dwIndex].dwData & 0x80) {
        // Boton nButton presionado          
        m_bButtonsState[nButton] = true;
        if ((m_nButtonsSubState[nButton] == DXDIDefs::MOUSE_STATE_UNPRESSED) ||
            (m_nButtonsSubState[nButton] == DXDIDefs::MOUSE_STATE_RELEASED)) { 
          // Si el boton estaba sin presionar o recien soltado 
          // entonces pasa a estado de recien pulsado
          m_nButtonsSubState[nButton] = DXDIDefs::MOUSE_STATE_PRESSEDDOWN;
        }
      }
      else {
        // Boton nButton no presionado 
        m_bButtonsState[nButton] = false;
      }

     // Bajamos el flag de actualizacion de botones
     bActButton = false;
    }
  }// Del for
  
  // Todo correcto
  return true;
}
