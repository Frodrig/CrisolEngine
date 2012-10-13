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
// DXDMError.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Funcion que convierte un codigo de error de las DirectMusic
//   a un objeto string para su posterior envio al Debugger y/o
//   logger.
//
// Notas:
// - Esta funcion reconoce errores hasta la version 7.0 de las
//   DirectX.
///////////////////////////////////////////////////////////////////////
#ifndef __DXDMERROR_H_
#define __DXDMERROR_H_

// Includes
#include <string>
#include <dmusici.h>

namespace DXDMError {
  // Defines
  // Errores no relacionados con codigo de devolucion de DirectX
  #define DXDMMANAGER_LOADERCREATEERROR_MSG "No se puede crear Loader"
  #define DXDMSEGMENT_CHECKPLAYERROR_MSG "No se puede chequear reproduccion"

  // Funciones
  void DXDMErrorToString(const HRESULT CodeError, std::string& aoStrError);
}

#endif
