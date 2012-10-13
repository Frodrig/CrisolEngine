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
// DXDIError.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Espacio de nombres en donde se define la funcion encargada de
//   transformar un codigo de devolucion (de error, principalmente),
//   a una cadena de caracteres (a un string).
//   En este espacio de nombres tambien se incluyen los mensajes
//   que no corresponden a ningun error concreto de DirectX.
//
// Notas:
// - Esta funcion reconoce errores hasta la version 7.0 de las
//   DirectX.
///////////////////////////////////////////////////////////////////////
#ifndef __DXDIERROR_H_
#define __DXDIERROR_H_

// Includes
#include <DInput.h>
#include <string>

namespace DXDIError
{
  // Defines
  // Errores no relacionados con codigo de devolucion de DirectX que
  // no dependen de las interfaces DirectX.
  #define DXDIKEYBOARD_NOEVENTCREATE_MSG "No se pudo crear el evento"
  #define DXDIMOUSE_AREAINCORRECT_MSG "El area de movimiento es incorrecta"
  #define DXDIMOUSE_COORDINATESINCORRECT_MSG "Coordenadas de posicion incorrectas"

  // Funciones
  void DXDIErrorToString(const HRESULT CodeError, std::string& asStrError);
}
#endif
