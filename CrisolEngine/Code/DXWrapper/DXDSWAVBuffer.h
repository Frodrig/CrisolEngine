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
// - Esta clase mantendrá un buffer de sonido WAV. La creacion podra 
//   ser de dos tipos, a partir de un fichero WAV (la clase implementa
//   los metodos necesarios para cargar el fichero WAV y volcarlo en
//   el buffer). Unicamente contendra informacion, por lo que no
//   incluira metodos para reproducirla.
// - Se debera de incluir la libreria "winmm.lib".
//
// Notas:
// - Clase optimizada para DirectX 7.0.
// - Se recuerda que todas las clases DX deben de heredar de la clase
//   base DXWrapper y reescribir los metodos Init y Clean. Para mas
//   informacion ver documentacion de DXWrapper.
// - La funcionalidad para cargar archivos de sonido WAV en el buffer
//   estara embebida en esta clase.
///////////////////////////////////////////////////////////////////////
#ifndef __DXDSWAVBUFFER_H_
#define __DXDSWAVBUFFER_H_

// Includes
#include "DXWrapper.h"
#include <mmsystem.h>
#include <string>
#include "DXDSDefs.h"

class DXSound;

// Clase DXDSWAVBuffer
class DXDSWAVBuffer: public DXWrapper
{
private:
  // Estructura para trabajo con el fichero WAV
  struct WAVFile 
  {
    WAVEFORMATEX  WaveFormat;	  // Informacion del fichero WAV
    HMMIO		  hmmio;		    // Handle hmmio al fichero WAV	
    DWORD		  dwDataLength; // Longitud de los datos WAV
	std::string	  oFileName;	  // Nombre del fichero WAV
    bool          bFileLoad;    // Indica si fichero WAV cargado
  };

protected:
  // Variables de miembro
  LPDIRECTSOUNDBUFFER  m_lpDSBuffer;    // Objeto DirectSoundBuffer.
  DWORD                m_dwDSBuffFlags; // Flags pasados al buffer.
  WAVFile              m_WAVFile;       // Info. fichero WAV
  
public:
  // Constructores y destructores
  DXDSWAVBuffer(void);
  ~DXDSWAVBuffer(void);

public:
  // Creacion / destruccion del buffer
  bool Create(DXSound* const poDXSound, 
			  const std::string& oFileName, 
              const dword dwControlFlags = DSBCAPS_STATIC);
  void Destroy(void) { Clean(); Init(); }

public:
  // Restauracion buffer perdido
  bool Restore(void);

private:
  // Metodos para trabajar con los ficheros WAV
  bool Load(const std::string& aoFileName);
  DWORD	Read(VOID* const lpBuffer, 
             const dword dwLength, 
             const int& nOrigin = SEEK_SET,
    		 const dword dwOffset = 0);
  inline void GetFormat(LPWAVEFORMATEX* const lplpWf) { 
	*lplpWf = &m_WAVFile.WaveFormat;
  }
  inline DWORD GetDataLength(void) const { 
	return m_WAVFile.dwDataLength; 
  }
	
protected:
  // Metodos de apoyo
  bool GetWaveFormat(WAVEFORMATEX* const pWf);
  bool GetWaveDataLength(DWORD* const pdwLength);
    
public:
  // Obtencion del objeto DirectSoundBuffer
  inline LPDIRECTSOUNDBUFFER GetObj(void) const { 
	return m_lpDSBuffer; 
  }

public:
  // Operaciones de consulta
  inline bool IsBufferLoad(void) const { 
	return m_WAVFile.bFileLoad; 
  }
  inline std::string GetWAVFileName(void) const { 
	return m_WAVFile.oFileName; 
  }
  bool IsInHardware(void);

protected:
  // Inicializacion y liberacion de recursos <DXWrapper>
  void Init(void);
  void Clean(void);
  
};
#endif
