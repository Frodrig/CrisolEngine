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
// DXDIError.cpp
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXDIError.h
///////////////////////////////////////////////////////////////////////

#include "DXDIError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Convierte un codigo de error de las DirectInput a una cadena
//   BIRString.
// Parametros:
// - CodeError: Codigo de error producido.
// - asStrError: Referencia al objeto string en donde depositar la
//   cadena con el nombre del error.
// Devuelve:
// - asStrError: Contendra string con el codigo del error.
// Notas:
// - Esta funcion cubre todos los posibles errores de cada una de
//   las interfaces presentes en DirectInput v7.0.
// - En caso de que no se reconozca el error se devolvera el codigo
//   "DIERR_?".
////////////////////////////////////////////////////////////////////// 
void DXDIError::DXDIErrorToString(const HRESULT CodeError, 
								  std::string& asStrError)
{
  // Evalua codigo de error recibido
  switch (CodeError)
  {
    case DIERR_ACQUIRED:
      asStrError = "DIERR_ACQUIRED"; break;
    case DIERR_ALREADYINITIALIZED:
      asStrError = "DIERR_ALREADYINITIALIZED"; break;
    case DIERR_BADDRIVERVER:
      asStrError = "DIERR_BADDRIVERVER"; break;
    case DIERR_BETADIRECTINPUTVERSION: 
      asStrError = "DIERR_BETADIRECTINPUTVERSION"; break;
    case DIERR_DEVICEFULL: 
      asStrError = "DIERR_DEVICEFULL"; break;
    case DIERR_DEVICENOTREG: 
      asStrError = "DIERR_DEVICENOTREG"; break;
    case DIERR_EFFECTPLAYING:
      asStrError = "DIERR_EFFECTPLAYING"; break;
    case DIERR_HASEFFECTS: 
      asStrError = "DIERR_HASEFFECTS"; break;
    case DIERR_GENERIC: 
      asStrError = "DIERR_GENERIC"; break;
    case DIERR_HANDLEEXISTS: 
      asStrError = "DIERR_HANDLEEXISTS"; break;
    case DIERR_INCOMPLETEEFFECT: 
      asStrError = "DIERR_INCOMPLETEEFFECT"; break;
    case DIERR_INPUTLOST: 
      asStrError = "DIERR_INPUTLOST"; break;
    case DIERR_INVALIDPARAM: 
      asStrError = "DIERR_INVALIDPARAM"; break;
    case DIERR_MOREDATA: 
      asStrError = "DIERR_MOREDATA"; break;
    case DIERR_NOAGGREGATION:
      asStrError = "DIERR_NOAGGREGATION"; break;
    case DIERR_NOINTERFACE:  
      asStrError = "DIERR_NOINTERFACE"; break;
    case DIERR_NOTACQUIRED:  
      asStrError = "DIERR_NOTACQUIRED"; break;
    case DIERR_NOTBUFFERED:  
      asStrError = "DIERR_NOTBUFFERED"; break;
    case DIERR_NOTDOWNLOADED:  
      asStrError = "DIERR_NOTDOWNLOADED"; break;
    case DIERR_NOTEXCLUSIVEACQUIRED:  
      asStrError = "DIERR_NOTEXCLUSIVEACQUIRED"; break;
    case DIERR_NOTFOUND: 
      asStrError = "DIERR_NOTFOUND"; break;
    case DIERR_NOTINITIALIZED:
      asStrError = "DIERR_NOTINITIALIZED"; break;
//    case DIERR_OBJECTNOTFOUND: 
//      asStrError = "DIERR_OBJECTNOTFOUND"; break;
    case DIERR_OLDDIRECTINPUTVERSION: 
      asStrError = "DIERR_OLDDIRECTINPUTVERSION"; break;
//    case DIERR_OTHERAPPHASPRIO: 
//      asStrError = "DIERR_OTHERAPPHASPRIO"; break;
    case DIERR_OUTOFMEMORY: 
      asStrError = "DIERR_OUTOFMEMORY"; break;
//    case DIERR_READONLY: 
//      asStrError = "DIERR_READONLY"; break;
    case DIERR_REPORTFULL:  
      asStrError = "DIERR_REPORTFULL"; break;
    case DIERR_UNPLUGGED: 
      asStrError = "DIERR_UNPLUGGED"; break;
    case DIERR_UNSUPPORTED:  
      asStrError = "DIERR_UNSUPPORTED"; break;
    case DI_OK: 
      break;
    default:
      asStrError = "DIERR_?"; break;
  }; // del swich  
}

