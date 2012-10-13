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
// DXDSError.cpp
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXDSError.h
///////////////////////////////////////////////////////////////////////

#include "DXDSError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Convierte un codigo de error de las DirectSound a un string
// Parametros:
// - CodeError: Codigo de error producido.
// - aoStrError: Referencia al objeto string en donde depositar la
//   cadena con el nombre del error.
// Devuelve:
// - poStrError: Contendra string con el codigo del error.
//
// Notas:
// - Esta funcion cubre todos los posibles errores de cada una de
//   las interfaces presentes en DirectSound v7.0.
// - En caso de que no se reconozca el error se devolvera el codigo
//   "DSERR_?".
////////////////////////////////////////////////////////////////////// 
void DXDSError::DXDSErrorToString(const HRESULT& CodeError, 
								  std::string& aoStrError)
{
  // Evalua codigo de error recibido
  switch (CodeError)
  {
    case DSERR_ACCESSDENIED:
      aoStrError = "DSERR_ACCESSDENIED"; break;
    case DSERR_ALLOCATED:
      aoStrError = "DSERR_ALLOCATED"; break;
    case DSERR_ALREADYINITIALIZED:
      aoStrError = "DSERR_ALREADYINITIALIZED"; break;
    case DSERR_BADFORMAT:
      aoStrError = "DSERR_BADFORMAT"; break;
    case DSERR_BUFFERLOST:
      aoStrError = "DSERR_BUFFERLOST"; break;
    case DSERR_CONTROLUNAVAIL:
      aoStrError = "DSERR_CONTROLUNAVAIL"; break;
    case DSERR_GENERIC:
      aoStrError = "DSERR_GENERIC"; break;
    //case DSERR_HWUNAVAIL:
      //aoStrError = "DSERR_HWUNAVAIL"; break;
    case DSERR_INVALIDCALL:
      aoStrError = "DSERR_INVALIDCALL"; break;
    case DSERR_INVALIDPARAM:
      aoStrError = "DSERR_INVALIDPARAM"; break;
    case DSERR_NOAGGREGATION:
      aoStrError = "DSERR_NOAGGREGATION"; break;
    case DSERR_NODRIVER:
      aoStrError = "DSERR_NODRIVER"; break;
    case DSERR_NOINTERFACE:
      aoStrError = "DSERR_NOINTERFACE"; break;
    case DSERR_OTHERAPPHASPRIO:
      aoStrError = "DSERR_OTHERAPPHASPRIO"; break;
    case DSERR_OUTOFMEMORY:
      aoStrError = "DSERR_OUTOFMEMORY"; break;
    case DSERR_PRIOLEVELNEEDED:
      aoStrError = "DSERR_PRIOLEVELNEEDED"; break;
    case DSERR_UNINITIALIZED:
      aoStrError = "DSERR_UNINITIALIZED"; break;
    case DSERR_UNSUPPORTED:
      aoStrError = "DSERR_UNSUPPORTED"; break;
    default:
      aoStrError = "DSERR_?"; break;
  }; // del swich  
  
}