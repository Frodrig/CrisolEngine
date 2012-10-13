///////////////////////////////////////////////////////////////////////////////
// DX7Wrapper - DirectX 7 Wrapper
// Copyright (C) 2002 Fernando Rodr�guez Mart�nez
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
// DXD3DError.h
// Fernando Rodr�guez 
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

#ifndef __DXD3DERROR_H_
#define __DXD3DERROR_H_

// Includes
#include "d3d.h"
#include <string>

namespace DXD3DError
{
  // defines
  // Errores no relacionados con codigo de devolucion de DirectX
  #define DXD3D_NOMEMFORCREATEQUAD_MSG "No hay memoria para crear los vertices"
  
  // Funciones
  void DXD3DErrorToString(const HRESULT CodeError, 
						  std::string& szStrError);
}

#endif
