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
// DXSound.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXSound.h
///////////////////////////////////////////////////////////////////////
#include "DXSound.h"

#include "DXDSError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Constructor;
// Parametros:
// Devuelve:
// Notas:
// - Llama al metodo Init para realizar la inicializacion
////////////////////////////////////////////////////////////////////// 
DXSound::DXSound(void)
{
  // Inicializa punteros
  Init();    
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicialia vbles de miembro
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
void DXSound::Init(void)
{
  // Procede a la inicialiacion
  m_lpDSObj = NULL;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destructor;
// Parametros:
// Devuelve:
// Notas:
// - Para realizar la opearcion de liberacion de memoria llama
//   al metodo Clean
////////////////////////////////////////////////////////////////////// 
DXSound::~DXSound(void)
{ 
  // Libera memoria
  Clean();  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera de memoria variables de miembro y las reinicializa
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
void DXSound::Clean(void)
{ 
  if (m_lpDSObj) {
    // Hay que destruir objeto DirectSound
    m_lpDSObj->Release();
    m_lpDSObj = NULL;
  }
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea objeto DirectSound y establece las caracteristicas del
//   buffer primario como mezclador.
// Parametros:
// - hWnd: Handle de la ventana principal de la aplicacion.
// - freqMode: Frecuencia para el buffer primario. A 44Hz por defecto.
// - chanMode: Numero de canales. Por defecto 2 (stereo).
// - bpm: Bits por muestra. Por defecto 16 bits.
// - lpGuid: GUID del dispositivo en donde crear el objeto. Se creara
//           en el dispositivo por defecto.
// Devuelve:
// - true: Si la operacion se ha realizado con exito.
// - false si ha existido algun problema.
// Notas:
// - Caso de existir el objeto lo libera y vuelve a crearlo.
////////////////////////////////////////////////////////////////////// 
bool DXSound::Create(const HWND& hWnd, 
                     const DXDSDefs::DXDSFrequency& freqMode, 
                     const DXDSDefs::DXDSChannel& chanMode, 
                     const DXDSDefs::DXDSBPM& bpm,
                     GUID* const lpGuid)
{   
  DSBUFFERDESC        dsbdesc;     // Descripcion del buffer primario
  LPDIRECTSOUNDBUFFER lpDSBuff;    // Buffer primario
  WAVEFORMATEX        wfm;         // Formato del sonido   

  // Si existe objeto lo libera
  if (m_lpDSObj) {
    Clean();
    Init();
  }
  
  // Crea objeto DirectSound
  m_CodeResult = DirectSoundCreate(lpGuid, &m_lpDSObj, NULL);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDSError::DXDSErrorToString(m_CodeResult, m_sLastCode);
    m_lpDSObj = NULL;
    return false;
  }
  
  // Establece nivel de cooperatividad
  m_CodeResult = m_lpDSObj->SetCooperativeLevel(hWnd, DSSCL_PRIORITY);
  if (FAILED(m_CodeResult)) {
    // Hay problemas    
    DXDSError::DXDSErrorToString(m_CodeResult, m_sLastCode);
    Clean();
    return false;
  }
  
  // A continuacion vienen los pasos para configurar 
  // el buffer primario, lugar en donde se van a mezclar los sonidos
  // procedentes de los distintos buffers secundarios. La calidad con la
  // que se ajuste este buffer, determinara la calidad con la que se
  // escucharan los sonidos de los buffers secundarios
  
  // Iniciar estructura de descripcion del buffer
  memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
  dsbdesc.dwSize = sizeof(DSBUFFERDESC);
  dsbdesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
  dsbdesc.dwBufferBytes = 0;  // 0 para el buffer primario
  dsbdesc.lpwfxFormat = NULL; // NULL para el buffer primario

  // Iniciar estructura para el formato deseado
  memset(&wfm, 0, sizeof(WAVEFORMATEX));
  wfm.wFormatTag = WAVE_FORMAT_PCM;
  wfm.nChannels = chanMode;
  wfm.nSamplesPerSec = freqMode;
  wfm.wBitsPerSample = bpm;
  wfm.nBlockAlign = wfm.wBitsPerSample / 8 * wfm.nChannels;
  wfm.nAvgBytesPerSec = wfm.nSamplesPerSec * wfm.nBlockAlign;
  
  // Obtener acceso a buffer primario
  m_CodeResult = m_lpDSObj->CreateSoundBuffer(&dsbdesc, &lpDSBuff, NULL);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    Clean();
    DXDSError::DXDSErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Configurar el buffer primario con el formato establecido.
  // Si falla la llamada el buffer primario se configurara a
  // su valor por defecto: 22Hz, 2 canales y 8 bpm
  m_CodeResult = lpDSBuff->SetFormat(&wfm);
  
  // Todo correcto  
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Configura los altavoces del usuario segun el tipo y 
//   su geometria.
// Parametros:
// - dwType: Tipo de altavoces entre:
//     * DSSPEAKER_HEADPHONE <auriculares>
//     * DSSPEAKER_MONO <sistema mono>
//     * DSSPEAKER_QUAD <sistema cuadrafonico>
//     * DSSPEAKER_STEREO <sistema estereo>
//     * DSSPEAKER_SURROUND <sistema doble surround>
// - dwGeometry: Geometria de los altavoces con respecto al usuario:
//     * DSSPEAKER_GEOMETRY_MIN <5 grados>
//     * DSSPEAKER_GEOMETRY_NARROW <10 grados>
//     * DSSPEAKER_GEOMETRY_WIDE <20 grados>
//     * DSSPEAKER_GEOMETRY_MAX <180 grados>
// Devuelve:
// true: Si se han configurado altavoces con exito.
// false: Si no se ha podido realizar la configuracion.
// Notas:
// - La informacion de la geometria solo tiene sentido cuando se
//   habla de altavoces stereo en cuyo caso tiene valor por defecto
//   a DSSPEAKER_GEOMETRY_WIDE.
// - En el caso de que no se haya creado objeto DirectSound el 
//   metodo retorna sin hacer nada.
////////////////////////////////////////////////////////////////////// 
bool DXSound::SetSpeakers(const dword dwType, 
						  const dword dwGeometry)
{  
  // SOLO si hay objeto DirectSound
  DXASSERT(m_lpDSObj != NULL);
  
    // Efectua configuracion de altavoces
  if (dwType != DSSPEAKER_STEREO) {
    // Configuracion no estereo
    m_CodeResult = m_lpDSObj->SetSpeakerConfig(dwType);
  }
  else {
    // Configuracion estereo; hay que combinar flags
    m_CodeResult = m_lpDSObj->SetSpeakerConfig(DSSPEAKER_COMBINED(dwType, dwGeometry));
  }

  // Comprueba resultado operacion
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDSError::DXDSErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
  
  // Todo correcto
  return true;
}