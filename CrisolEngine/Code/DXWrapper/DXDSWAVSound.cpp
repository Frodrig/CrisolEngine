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
// DXDSWAVSound.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXDSWAVSound.h.
///////////////////////////////////////////////////////////////////////
#include "DXDSWAVSound.h"

#include "DXSound.h"
#include "DXDSWAVBuffer.h"
#include "DXDSError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Constructor; Inicializa variables de miembro
// Parametros:
// Devuelve:
// Notas:
// - Se debe de realizar una llamada al constructor de DXDSWAVSound.
////////////////////////////////////////////////////////////////////// 
DXDSWAVSound::DXDSWAVSound(void)
{  
  // Inicializa vbles de miembro
  Init();  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa las vbles de miembro.
// Parametros:
// Devuelve:
// Notas:
// - Este metodo sera necesario cuando se necesite realizar un
//   "reseteo" de la clase para un nuevo uso.
////////////////////////////////////////////////////////////////////// 
void DXDSWAVSound::Init(void)
{  
  // Inicializa vbles de miembro
  m_lpDSBuffer = NULL;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destructor; Libera vbles de miembro
// Parametros:
// Devuelve:
// Notas:
// - Llamara al metodo encargado de la tarea de liberacion de memoria.
////////////////////////////////////////////////////////////////////// 
DXDSWAVSound::~DXDSWAVSound(void)
{  
  // Llama al metodo apropiado
  Clean();
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera vbles de miembro
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
void DXDSWAVSound::Clean(void)
{  
  // Desvincula punteros
  if (m_lpDSBuffer) {
    m_lpDSBuffer->Release();
    m_lpDSBuffer = NULL;
  }  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nuevo buffer que utiliza los mismos recursos e informacion
//   que el recibido como parametro. Este metodo NO aloja en memoria un
//   fichero .wav.
// Parametros:
// - poDXSound: Objeto DirectSound.
// - poDSBufferSource: Buffer origen sobre el que realizar la duplicacion.
// Devuelve:
// - true: Si no ha existido ningun problema.
// - false: Si ha ocurrido algun problema en el parametro recibido o
//   bien al ir a duplicar.
// Notas:
// - MUY IMPORTANTE: La llamada a este metodo no siempre tendra exito.
//   DirectX no dejara crear un buffer duplicado en software si el
//   original esta en hardware.
// - Al duplicar un buffer, realmente se crea una referencia a otro
//   buffer ya existente pero pudiendo modificar sus atributos y 
//   pudiendo tocarlo o pararlo libremente. El unico problema
//   se derivara al querer cambiar el contenido del buffer. Realizar 
//   un cambio en el mismo afectara a todos los buffers duplicados 
//   que apunten a el, incluido el buffer original.
// - En el caso de que ya existiera un buffer secundario, este se
//   liberara.
////////////////////////////////////////////////////////////////////// 
bool DXDSWAVSound::Create(DXSound* const poDXSound, 
                          DXDSWAVBuffer* const poDSBufferSource)
{
  // SOLO si hay existencia de objeto DirectSound
  DXASSERT(poDXSound != NULL);
  DXASSERT(poDXSound->GetObj() != NULL);
  
  // Si ya existia buffer secundario se libera
  if (m_lpDSBuffer) { 
	Destroy(); 
  }

  // Duplica buffer
  m_CodeResult = poDXSound->GetObj()->DuplicateSoundBuffer(poDSBufferSource->GetObj(),
                                                           &m_lpDSBuffer);  
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDSError::DXDSErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Restaura a un buffer perdido el archivo de sonido.
// Parametros:
// Devuelve:
// - true: Si se ha podido realizar la operacion con exito.
// - false: Si han existido problemas al hacerlo.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDSWAVSound::Restore(void)
{
  // Intenta recuperar buffer secundario
  m_CodeResult = m_lpDSBuffer->Restore();
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDSError::DXDSErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }  

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Reproduce el sonido almacenado (o asociado) en el buffer secundario.
// Parametros:
// - dwFlags: Modo de reproduccion. Por defecto reproduce una 
//   sola vez el sonido y acaba. Si recibe DSBPLAY_LOOPING se
//   reproducira el sonido continuamente.
// Devuelve:
// - true: Si la operacion se realiza con exito.
// - false: Existe algun problema.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDSWAVSound::Play(const dword dwFlags)
{
  // SOLO si hay buffer secundario
  DXASSERT(m_lpDSBuffer != NULL);
  
  DWORD dwStatus;    // Estado del buffer secundario

  // Inicializaciones  
  dwStatus = m_lpDSBuffer->GetStatus(&dwStatus);

  // ¿Se ha perdido el buffer?
  if (dwStatus &  DSBSTATUS_BUFFERLOST) {
    // Si; se llama al metodo Restore para recuperarlo    
    Restore();
  }

  // Se pone en marcha el sonido
  m_CodeResult = m_lpDSBuffer->Play(0, 0, dwFlags);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDSError::DXDSErrorToString(m_CodeResult, m_sLastCode);
    return false;
  } 

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Detiene el sonido que se este reproduciendo.
// Parametros:
// - bInitPlayPos: Indica si hay que parar y poner al comienzo el 
//   cursor de reproduccion. Por defecto vale true, con lo que se
//   llevara al comienzo el cursor si no se recibe otro parametro.
// Devuelve:
// - true: Si se detiene el sonido o el sonido.
// - false: Existe algun tipo de problema.
// Notas:
// - Aunque el buffer se encuentre sin ser reproducido, al llamar a 
//   stop se devolvera true si todo sale bien.
////////////////////////////////////////////////////////////////////// 
bool DXDSWAVSound::Stop(const bool bInitPlayPos)
{
  // SOLO si hay buffer secundario
  DXASSERT(m_lpDSBuffer != NULL);
  
  // Para la reproduccion
  m_CodeResult = m_lpDSBuffer->Stop();
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDSError::DXDSErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // ¿Hay que resituar el cursor?
  if (bInitPlayPos) {
    // Si; ademas de parar hay que situar el cursor en la pos. inicial
    if (!SetPlayPosition()) {
      // No se pudo situar el cursor; se establece codigo de error interno
      m_CodeResult = S_OK;
      m_sLastCode = DXDSBUFFERS_SETPLAYPOSERROR_MSG;
      return false;
    }
  }   

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el cursor de reproduccion. Por defecto establece el
//   cursor en el comienzo del buffer.
// Parametros:
// - dwPlayPos: Posicion en la que establecer el cursor de 
//   reproduccion.
// Devuelve:
// - true: Si todo ha salido bien.
// - false: Si hay algun tipo de problema a la hora de actualizar la
//   posicion de reproduccion o en el propio buffer.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDSWAVSound::SetPlayPosition(const dword dwPlayPos)
{
  // SOLO si hay buffer secundario
  DXASSERT(m_lpDSBuffer != NULL);

  // Obtiene posicion de lectura
  m_CodeResult = m_lpDSBuffer->SetCurrentPosition(dwPlayPos);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDSError::DXDSErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la posicion del puntero de lectura sobre el buffer.
// Parametros:
// - pdwPlayPos: Direccion de la vble en donde almacenar la
//   posicion de lectura.
// Devuelve:
// - dwPlayPos: Referencia a la vble donde depositar el valor.
// - Por funcion:
//       - true: Si la operacion ha salido con exito.
//       - false: Si ha ocurrido algun problema.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDSWAVSound::GetPlayPosition(DWORD& dwPlayPos)
{
  // SOLO si hay buffer secundario
  DXASSERT(m_lpDSBuffer != NULL);

  // Obtiene posicion de lectura
  m_CodeResult = m_lpDSBuffer->GetCurrentPosition(&dwPlayPos, NULL);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDSError::DXDSErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el valor para el sonido. Por defecto establece una
//   atenuacion a medio camino entre el volumen maximo y el volumen
//   minimo.
// Parametros:
// - lVolume: Volumen para el sonido. Cuanto mas proximo a cero 
//   menor atenuacion y mayor volumen. El valor maximo del sonido
//   se encuentra en DSBVOLUME_MAX y el minimo en DSBVOLUME_MIN.
// Devuelve:
// - true: Si la operacion se ha realizado con exito. 
// - false: Si ha existido algun problema al modificar el volumen.
// Notas:
// - MUY IMPORTANTE: Para trabajar con este control es necesario
//   haber pasado al metodo Create el flag DSBCAPS_CTRLVOLUME.
// - En caso de sobrepasar los maximos y minimos del sonido con 
//   lVolume se devolvera true y se acotara convenientemente.
////////////////////////////////////////////////////////////////////// 
bool DXDSWAVSound::SetVolume(const LONG& lVolume)
{
  // SOLO si hay buffer secundario
  DXASSERT(m_lpDSBuffer != NULL);

  // Establece volumen
  m_CodeResult = m_lpDSBuffer->SetVolume(lVolume);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDSError::DXDSErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }  
  
  // Todo correcto
  return true;
}
  
//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve el valor actual del volumen del sonido.
// Parametros:
// - lVolume: Referencia a la vble en donde alojar el volumen.
// Devuelve:
// - plVolume: Valor del volumen.
// - Por funcion:
//        - true: Si todo se ha realizado con exito.
//        - false: si ha existido algun problema.
// Notas:
// - MUY IMPORTANTE: Para trabajar con este control es necesario
//   haber pasado al metodo Create el flag DSBCAPS_CTRLVOLUME.
////////////////////////////////////////////////////////////////////// 
bool DXDSWAVSound::GetVolume(LONG& lVolume)
{
  // SOLO si hay buffer secundario
  DXASSERT(m_lpDSBuffer != NULL);

  // Obtiene valor del volumen
  m_CodeResult = m_lpDSBuffer->GetVolume(&lVolume);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDSError::DXDSErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el objeto esta creado sobre dispositivo hardware
//   o software.
// Parametros:
// Devuelve:
// - Si esta en hardware true. Si esta en software false.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDSWAVSound::IsInHardware(void)
{
  // SOLO si objeto creado
  DXASSERT(m_lpDSBuffer);

  // Toma la informacion del buffer para saber donde se ha alojado
  DSBCAPS dsbcaps;
  memset(&dsbcaps, 0, sizeof(DSBCAPS));
  dsbcaps.dwSize = sizeof(DSBCAPS);
  m_CodeResult = m_lpDSBuffer->GetCaps(&dsbcaps);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDSError::DXDSErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Retorna resultado
  if (dsbcaps.dwFlags & DSBCAPS_LOCHARDWARE) { return true; }
  return false;
}
 
//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si se esta reproduciendo, o no, el sonido.
// Parametros:
// Devuelve:
// - Si se esta reproduccion devuelve true y false en caso contrario.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDSWAVSound::IsPlaying(void)
{
  // SOLO si objeto creado
  DXASSERT(m_lpDSBuffer);
  
  // Vbles
  DWORD  dwStatus = 0; // Donde se recogera el status
  
  // Se toma el status y se comprueba
  m_lpDSBuffer->GetStatus(&dwStatus);  
  // Evalua la comprobacion
  return (dwStatus & DSBSTATUS_PLAYING) ? true : false;      
}