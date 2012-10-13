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
// DXDSError.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Funcion que convierte un codigo de error de las DirectSound
//   a un string para su posterior envio al Debugger y/o logger.
//
// Notas:
// - Esta funcion reconoce errores hasta la version 7.0 de las
//   DirectX.
///////////////////////////////////////////////////////////////////////
#ifndef __DXDSERROR_H_
#define __DXDSERROR_H_

// Includes
// Includes
#include <DSound.h>
#include <string>

namespace DXDSError {
  // Strings con errores no relacionados con los codigos de devolucion de DirectX
  #define DXDSBUFFERS_FLAGSERROR_MSG "Fallo en los buffers de control"
  #define DXDSBUFFERS_FILENAMEERROR_MSG "No se puede encontrar fichero WAV"
  #define DXDSBUFFERS_SETPLAYPOSERROR_MSG "No se puede situar cursor de reproduccion"

  // Funciones
  void DXDSErrorToString(const HRESULT& CodeError, std::string& aoStrError);
};

#endif
