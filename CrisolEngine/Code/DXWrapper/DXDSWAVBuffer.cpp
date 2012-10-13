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
// DXDSWAVBuffer.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXDSWAVBuffer.h.
///////////////////////////////////////////////////////////////////////
#include "DXDSWAVBuffer.h"

#include "DXSound.h"
#include "DXDSError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Constructor; Inicializa variables de miembro
// Parametros:
// Devuelve:
// Notas:
// - Se debe de realizar una llamada al constructor de DXDSWAVBuffer.
////////////////////////////////////////////////////////////////////// 
DXDSWAVBuffer::DXDSWAVBuffer(void)
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
void DXDSWAVBuffer::Init(void)
{  
  // Inicializa vbles de miembro
  m_lpDSBuffer = NULL;
  m_dwDSBuffFlags = DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_STATIC;    
  memset(&m_WAVFile.WaveFormat, 0, sizeof(WAVEFORMATEX));
  m_WAVFile.hmmio = NULL;
  m_WAVFile.dwDataLength = 0;
  m_WAVFile.bFileLoad = false;  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destructor; Libera vbles de miembro
// Parametros:
// Devuelve:
// Notas:
// - Llamara al metodo encargado de la tarea de liberacion de memoria.
////////////////////////////////////////////////////////////////////// 
DXDSWAVBuffer::~DXDSWAVBuffer(void)
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
void DXDSWAVBuffer::Clean(void)
{  
  // Desvincula punteros
  if (m_lpDSBuffer) {
    m_lpDSBuffer->Release();
    m_lpDSBuffer = NULL;
  }  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un buffer secundario de tipo estatico intentando alojarlo
//   primero en hardware y si falla en software. Este metodo aloja
//   en memoria un archivo .wav.
// Parametros:
// - poDXSound: Puntero a la instancia DXSound para poder 
//   obtener el objeto DirectSound.
// - oaFileName: Nombre del fichero .WAV en donde cargar el sonido.
// - dwControlFlags: Valor de los flags que se quieren asignar. Los
//   flags podran ser los siguientes:
//    * DSBCAPS_CTRLFREQUENCY: Se podra controlar la frecuencia.
//    * DSBCAPS_CTRLPAN: Se podra controlar el desplazamiento.
//    * DSBCAPS_CTRLPOSITIONNOTIFY: Se podran utilizar eventos de 
//      notificacion.
//    * DSBCAPS_CTRLVOLUME: Se podra controlar el volumen.
//    * DSBCAPS_GLOBALFOCUS: El sonido continuara pese a cambiar de
//      aplicacion.
//    * DSBCAPS_LOCHARDWARE: Se crea buffer en hardware o falla.
//    * DSBCAPS_LOCSOFTWARE: Se crea buffer en mem. del sistema o falla.
// Devuelve:
// - true: Si todo ha salido bien.
// - false: Si ha existido algun problema al crear el buffer.
// Notas:
// - MUY IMPORTANTE: Utilizar SOLO los controles que sean necesarios
//   pues asignarlos consumen muchos recursos. Pefectamente se puede
//   crear un buffer secundario sin recibir controles por parametro.
// - En el caso de que ya existiera un buffer secundario, este se
//   liberara.
// - Se asignaran por defecto los controles DSBCAPS_STATIC (que
//   obligara a crear buffer estatico primero en hardware y si
//   falla en software) y DSBCAPS_GETCURRENTPOSITION2 que optimizara
//   el manejo del cursor de lectura / escritura.
// - No se permitiran recibir los controles DSBCAPS_CTRL3D, 
//   DSBCAPS_MUTE3DATMAXDISTANCE, DSBCAPS_PRIMARYBUFFER y
//   DSBCAPS_STICKYFOCUS.
// - Los flags DSBCAPS_LOCHARDWARE y DSBCAPS_LOCSOFTWARE no se pueden
//   usar a la vez, caso de hacerlo se devolvera false. 
////////////////////////////////////////////////////////////////////// 
bool DXDSWAVBuffer::Create(DXSound* const poDXSound, 
						   const std::string& oFileName, 
                           const dword dwControlFlags)
{
  // SOLO si hay existencia de objeto DirectSound
  DXASSERT(poDXSound != NULL);
  DXASSERT(poDXSound->GetObj() != NULL);
  
  // Si ya existia buffer secundario, se libera
  if (m_lpDSBuffer) { Destroy(); }

  // Comprueba flags de control
  if ((dwControlFlags & DSBCAPS_MUTE3DATMAXDISTANCE) || 
      (dwControlFlags & DSBCAPS_PRIMARYBUFFER) ||
      (dwControlFlags & DSBCAPS_STICKYFOCUS)) {
    // Hay problemas; se establece codigo de error interno
    m_CodeResult = S_OK;
    m_sLastCode = DXDSBUFFERS_FLAGSERROR_MSG;
    return false;
  }

  DWORD           dwDataLength; // Longitud de los datos WAV
  LPWAVEFORMATEX  lpWaveFormat; // Formato del fichero WAV
  DSBUFFERDESC    dsbdesc;      // Descripcion del buffer secundario  
    
  // Obtiene informacion de fichero WAV
  if (!Load(oFileName)) {
    // Hay problemas; se establece codigo de error interno
    m_CodeResult = S_OK;
    m_sLastCode = DXDSBUFFERS_FLAGSERROR_MSG;
    return false;
  }
  dwDataLength = GetDataLength();      // Longitud datos WAV
  GetFormat(&lpWaveFormat);            // Formato archivo WAV

  // Inicializa descricipcion del buffer a crear
  memset(&dsbdesc, 0, sizeof(DSBUFFERDESC));
  dsbdesc.dwSize = sizeof(DSBUFFERDESC);
  m_dwDSBuffFlags |= dwControlFlags;
  dsbdesc.dwFlags = m_dwDSBuffFlags;
  dsbdesc.dwBufferBytes = dwDataLength;
  dsbdesc.lpwfxFormat = lpWaveFormat;

  // Crea buffer secundario
  m_CodeResult = poDXSound->GetObj()->CreateSoundBuffer(&dsbdesc,
                                                        &m_lpDSBuffer,
                                                        NULL);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDSError::DXDSErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
  
  BYTE* pDSBuffData;  // Puntero a la zona de escritura del buffer

  // Bloquea y obtiene puntero a zona de escritura del buffer
  m_CodeResult = m_lpDSBuffer->Lock(0, dwDataLength, (LPVOID *) &pDSBuffData,
                                    &dwDataLength, NULL, 0, 0);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDSError::DXDSErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Escribe datos WAV en el buffer
  dwDataLength = Read(pDSBuffData, dwDataLength);

  // Desbloquea buffer secundario
  m_CodeResult = m_lpDSBuffer->Unlock(pDSBuffData, dwDataLength, NULL, 0);
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
bool DXDSWAVBuffer::Restore(void)
{
  BYTE* pDSBuffData;  // Puntero a la zona de escritura del buffer
  DWORD dwDataLength; // Longitud de los datos WAV
  
  // Inicializaciones  
  dwDataLength = GetDataLength();

  // Intenta recuperar buffer secundario
  m_CodeResult = m_lpDSBuffer->Restore();
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDSError::DXDSErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }  
  
  // Se logro recuperar
  // Pide bloqueo del buffer para volver a escribir el sonido
  m_CodeResult = m_lpDSBuffer->Lock(0, dwDataLength, (LPVOID *) &pDSBuffData,
    &dwDataLength, NULL, 0, 0);
  if (FAILED(m_CodeResult)){
    // Hay problemas
    DXDSError::DXDSErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
  
  // Escribe datos WAV en el buffer
  dwDataLength = Read(pDSBuffData, dwDataLength);
  
  // Desbloquea buffer secundario
  m_CodeResult = m_lpDSBuffer->Unlock(pDSBuffData, dwDataLength, NULL, 0);
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
// - Asocia handle al nombre del archivo .WAV recibido.
// Parametros:
// - oFileName: Nombre del archivo.
// Devuelve:
// - true: Operacion realizada con exito.
// - false: Ha existido algun problema.
// Notas:
// - Este metodo no aloja en memoria el archivo .WAV indicado sino que
//   simplemente prepara las estructuras internas para vincularlas
//   a un determinado archivo.
// - Solo trabaja con archivos .WAV que son ficheros.
// - En el caso de que se llame existiendo otro archivo asociado al
//   objeto, este quedara liberado.
// - En caso de existir algun error, el objeto quedara sin archivo
//   .WAV asociado.
////////////////////////////////////////////////////////////////////// 
bool DXDSWAVBuffer::Load(const std::string& aoFileName)
{
  // Resetea si hay archivo asociado
  if (m_WAVFile.bFileLoad) Init();      
  
  // Obtiene nombre fichero
  m_WAVFile.oFileName = aoFileName;    
  
  // Abre archivo utilizando el IO API para multimedia    
  m_WAVFile.hmmio = mmioOpen((char *)((LPCTSTR)m_WAVFile.oFileName.c_str()), NULL, MMIO_READ);
  if(!m_WAVFile.hmmio) {
    // No se puede abrir el fichero wav
    return false;
  }

  // Obtiene campo informacion del archivo .WAV
  if(!GetWaveFormat(&m_WAVFile.WaveFormat)) { return false; }
  // Obtiene el campo longitud del archivo .WAV
  if(!GetWaveDataLength(&m_WAVFile.dwDataLength)) { return false; }

  // Cierra archivo .WAV y rellena vbles de miembro
  if(m_WAVFile.hmmio) {
  	mmioClose(m_WAVFile.hmmio, 0);
  	m_WAVFile.hmmio = NULL;
  }

  // Todo correcto
  m_WAVFile.bFileLoad = true;
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el formato .WAV del archivo pasado al metodo Load.
// Parametros:
// - pWf: Pasara a apuntar a una direccion con el valor de la 
//       estructura WAVEFORMATEX del fichero .WAV
// Devuelve:
// - true: Operacion realizada con exito.
// - false: Ha existido algun problema.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDSWAVBuffer::GetWaveFormat(WAVEFORMATEX* const pWf)
{
  // Comprueba que el parámetro sea correcto
  if (pWf == NULL) {
    // Problemas con el parametro
	  return false;
  }
	
  // Comprueba existencia de archivo abierto
  if(!m_WAVFile.hmmio)  {
    // Handle no valido  	
	  return false;
  }  
  
  MMCKINFO	mmWave;   // Para lectura del chunk WAVE
  MMCKINFO	mmFmt;    // Para lectura del chunk fmt
  MMRESULT	mResult;  // Resultado operacion

  // Inicializa estructuras
  memset(&mmWave, 0, sizeof(mmWave));
  memset(&mmFmt, 0, sizeof(mmFmt));

  // Cursor de lectura al inicio de fichero
  mResult = mmioSeek(m_WAVFile.hmmio, 0, SEEK_SET);
	
  // Busca chunk WAVE
  mmWave.fccType = mmioStringToFOURCC("WAVE", 0);
  mmFmt.ckid = mmioStringToFOURCC("fmt ", 0);
  mResult = mmioDescend(m_WAVFile.hmmio, &mmWave, NULL, MMIO_FINDRIFF);
  if(mResult != MMSYSERR_NOERROR) {
  	// No se encuentra el chunk
  	return false;
  }

  // Busca chunk fmt
  mResult = mmioDescend(m_WAVFile.hmmio, &mmFmt, &mmWave, MMIO_FINDCHUNK);
  if(mResult != MMSYSERR_NOERROR) {
  	// No se encuentra el chunk
  	return false;  
  }

  // Obtiene estructura WAVEFORMATEX y la almacena en wf
  mResult = mmioRead(m_WAVFile.hmmio, (HPSTR)pWf, sizeof(WAVEFORMATEX));
  if(mResult == -1) {
    // Error al leer estructura WAVEFORMATEX
  	return false;
  }
  mmioAscend(m_WAVFile.hmmio, &mmFmt, 0);

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene longitud de los campos de datos del archivo .WAV
// Parametros:
// - pdwLenght: Direccion de la variable en donde alojar el
//   valor de la longitud de los datos del fichero .WAV
// Devuelve:
// - true: Operacion realizada con exito
// - false: Ha existido algun problema.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDSWAVBuffer::GetWaveDataLength(DWORD* const pdwLength)
{
  // Comprueba existencia de archivo abierto
  if(!m_WAVFile.hmmio) {
    // Handle no valido  	
	  return false;
  }

  MMCKINFO	mmWave;  // Para lectura del chunk WAVE
  MMCKINFO	mmData;  // Para lectura chunk datos
  MMRESULT	mResult; // Resultado operacion
  
  // Inicializa estructuras
  memset(&mmWave, 0, sizeof(mmWave));
  memset(&mmData, 0, sizeof(mmData));

  // Posiciona cursor lectura
  mResult = mmioSeek(m_WAVFile.hmmio, 0, SEEK_SET);

  // Busca el chunk WAVE
  mmWave.fccType = mmioStringToFOURCC("WAVE", 0);
  mmData.ckid = mmioStringToFOURCC("data", 0);
  mResult = mmioDescend(m_WAVFile.hmmio, &mmWave, NULL, MMIO_FINDRIFF);
  if(mResult != MMSYSERR_NOERROR) {
    // No se encuentra chunk
  	return false;
  }

  // Busca el chunk DATA
  mResult = mmioDescend(m_WAVFile.hmmio, &mmData, &mmWave, MMIO_FINDCHUNK);
  if(mResult != MMSYSERR_NOERROR) {
    // No se encuentra chunk  	
	  return false;
  }
  
  // Da valor a parametro recibido
  *pdwLength = mmData.cksize;

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Lee del archivo .WAV previamente cargado, un numero dwLenght de
//   datos y los almacena en el buffer lpBuffer.
// Parametros:
// - lpBuffer: Buffer en donde se almacenan, o devuelven, los datos 
//             leidos
// - dwLenght: Numero de bytes a leer.
// - nOrigin: Origen del cursor de lectura. El valor para el origen
//   podra ser:
//      * SEEK_CUR: Lee desde pos. actual + dwOffset.
//      * SEEK_END: Lee desde pos. final - dwOffset.
//      * SEEK_SET: Lee desde el principio + dwOffset.
// - dwOffset: Offset a sumar al cursor de lectura establecido por
//   nOrigin.
// Devuelve:
// - Numero de bytes leidos.
//   
// Notas:
// - Para comprobar que se ha leido lo requerido, habra que comparar
//   el valor devuelto con dwLenght.
// - Si devuelve 0 probablemente indicara error.
////////////////////////////////////////////////////////////////////// 
DWORD DXDSWAVBuffer::Read(VOID* const lpBuffer, 
                          const dword dwLength, 
                          const int& nOrigin, 
                          const dword dwOffset)
{
  // Comprueba parametros
  if(dwLength - dwOffset > GetDataLength() || !lpBuffer)  {
    // Parametros no validos  	
	  return 0;
  }

  MMRESULT	mResult; // Resultado operacion
  MMIOINFO	mmInfo;  // Chunk info del fichero .WAV

  // Inicializa estructuras
  memset(&mmInfo, 0, sizeof(mmInfo));
  
  // Abre archivo utilizando el IO API multimedia
  m_WAVFile.hmmio = mmioOpen((char *)((LPCTSTR)m_WAVFile.oFileName.c_str()), NULL, MMIO_READ);
  if(!m_WAVFile.hmmio) {
    // No se puede abrir el fichero .WAV  	
  	return 0;
  }

  MMCKINFO	mmWave; // Chunk WAVE
  MMCKINFO	mmData; // Chunk DATA

  // Inicializa estructuras
  memset(&mmWave, 0, sizeof(mmWave));
  memset(&mmData, 0, sizeof(mmData));

  // Posiciona curso de lectura
  if(mmioSeek(m_WAVFile.hmmio, dwOffset, nOrigin) == -1) {
    // No se puede situar el cursor de lectura  	
	  return 0;
  }

  // Busca chunk WAVE
  mmWave.fccType = mmioStringToFOURCC("WAVE", 0);
  mmData.ckid = mmioStringToFOURCC("data", 0);
  mResult = mmioDescend(m_WAVFile.hmmio, &mmWave, NULL, MMIO_FINDRIFF);
  if(mResult != MMSYSERR_NOERROR) {
    // No se puede encontrar chunk  	
	  return 0;
  }

  // Busca chunk DATA
  mResult = mmioDescend(m_WAVFile.hmmio, &mmData, &mmWave, MMIO_FINDCHUNK);
  if(mResult != MMSYSERR_NOERROR) {
    // No se puede encontrar chunk  	
	  return 0;
  }
		
  // Obtiene los datos
  mResult = mmioRead(m_WAVFile.hmmio, (HPSTR)lpBuffer, dwLength);
  if(mResult == -1) {
    // Error al leer datos del fichero	
	  return 0;
  }

  // Cierra archivo
  if(m_WAVFile.hmmio) {
  	mmioClose(m_WAVFile.hmmio, 0);
	  m_WAVFile.hmmio = NULL;
  }

  // Todo correcto
  return mResult;
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
bool DXDSWAVBuffer::IsInHardware(void)
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
 
