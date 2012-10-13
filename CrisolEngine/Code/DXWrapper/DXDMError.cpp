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
// DXDMError.cpp
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXDMError.h
///////////////////////////////////////////////////////////////////////
#include "DXDMError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Convierte un codigo de error de las DirectMusic a una cadena
//   string.
// Parametros:
// - CodeError: Codigo de error producido.
// - poStrError: Puntero al objeto string en donde depositar la
//   cadena con el nombre del error.
// Devuelve:
// - poStrError: Contendra string con el codigo del error.
//
// Notas:
// - Esta funcion cubre todos los posibles errores de cada una de
//   las interfaces presentes en DirectMusic v7.0.
// - En caso de que no se reconozca el error se devolvera el codigo
//   "DMERR_?".
////////////////////////////////////////////////////////////////////// 
void DXDMError::DXDMErrorToString(const HRESULT CodeError, 
								  std::string& aoStrError)
{
  // Evalua codigo de error recibido
  switch (CodeError)
  {
    case DMUS_E_ALL_TOOLS_FAILED:
      aoStrError = "DMUS_E_ALL_TOOLS_FAILED"; break;
    case DMUS_E_ALL_TRACKS_FAILED:
      aoStrError = "DMUS_E_ALL_TRACKS_FAILED"; break;
    case DMUS_E_ALREADY_ACTIVATED:
      aoStrError = "DMUS_E_ALREADY_ACTIVATED"; break;
    case DMUS_E_ALREADY_DOWNLOADED:
      aoStrError = "DMUS_E_ALREADY_DOWNLOADED"; break;
    case DMUS_E_ALREADY_EXISTS:
      aoStrError = "DMUS_E_ALREADY_EXISTS"; break;
    case DMUS_E_ALREADY_INITED:
      aoStrError = "DMUS_E_ALREADY_INITED"; break;
    case DMUS_E_ALREADY_LOADED:
      aoStrError = "DMUS_E_ALREADY_LOADED"; break;
    case DMUS_E_ALREADY_SENT:
      aoStrError = "DMUS_E_ALREADY_SENT"; break;
    case DMUS_E_ALREADYCLOSED:
      aoStrError = "DMUS_E_ALREADYCLOSED"; break;
    case DMUS_E_ALREADYOPEN:
      aoStrError = "DMUS_E_ALREADYOPEN"; break;
    case DMUS_E_BADARTICULATION:
      aoStrError = "DMUS_E_BADARTICULATION"; break;
    case DMUS_E_BADINSTRUMENT:
      aoStrError = "DMUS_E_BADINSTRUMENT"; break;
    case DMUS_E_BADOFFSETTABLE:
      aoStrError = "DMUS_E_BADOFFSETTABLE"; break;
    case DMUS_E_BADWAVE:
      aoStrError = "DMUS_E_BADWAVE"; break;
    case DMUS_E_BADWAVELINK:
      aoStrError = "DMUS_E_BADWAVELINK"; break;
    case DMUS_E_BUFFER_EMPTY:
      aoStrError = "DMUS_E_BUFFER_EMPTY"; break;
    case DMUS_E_BUFFER_FULL:
      aoStrError = "DMUS_E_BUFFER_FULL"; break;
    case DMUS_E_BUFFERNOTAVAILABLE:
      aoStrError = "DMUS_E_BUFFERNOTAVAILABLE"; break;
    case DMUS_E_BUFFERNOTSET:
      aoStrError = "DMUS_E_BUFFERNOTSET"; break;
 //   case DMUS_E_CANNOT_CONVERT:
 //     aoStrError = "DMUS_E_CANNOT_CONVERT"; break;
    case DMUS_E_CANNOT_FREE:
      aoStrError = "DMUS_E_CANNOT_FREE"; break;
    case DMUS_E_CANNOT_OPEN_PORT:
      aoStrError = "DMUS_E_CANNOT_OPEN_PORT"; break;
    case DMUS_E_CANNOTREAD:
      aoStrError = "DMUS_E_CANNOTREAD"; break;
    case DMUS_E_CANNOTSEEK:
      aoStrError = "DMUS_E_CANNOTSEEK"; break;
    case DMUS_E_CANNOTWRITE:
      aoStrError = "DMUS_E_CANNOTWRITE"; break;
    case DMUS_E_CHUNKNOTFOUND:
      aoStrError = "DMUS_E_CHUNKNOTFOUND"; break;
    case DMUS_E_DESCEND_CHUNK_FAIL:
      aoStrError = "DMUS_E_DESCEND_CHUNK_FAIL"; break;
    case DMUS_E_DEVICE_IN_USE:
      aoStrError = "DMUS_E_DEVICE_IN_USE"; break;
    case DMUS_E_DMUSIC_RELEASED:
      aoStrError = "DMUS_E_DMUSIC_RELEASED"; break;
    case DMUS_E_DRIVER_FAILED:
      aoStrError = "DMUS_E_DRIVER_FAILED"; break;
    case DMUS_E_DSOUND_ALREADY_SET:
      aoStrError = "DMUS_E_DSOUND_ALREADY_SET"; break;
    case DMUS_E_DSOUND_NOT_SET:
      aoStrError = "DMUS_E_DSOUND_NOT_SET"; break;
    case DMUS_E_GET_UNSUPPORTED:
      aoStrError = "DMUS_E_GET_UNSUPPORTED"; break;
    case DMUS_E_INSUFFICIENTBUFFER:
      aoStrError = "DMUS_E_INSUFFICIENTBUFFER"; break;
    case DMUS_E_INVALID_BAND:
      aoStrError = "DMUS_E_INVALID_BAND"; break;
    case DMUS_E_INVALID_DOWNLOADID:
      aoStrError = "DMUS_E_INVALID_DOWNLOADID"; break;
    case DMUS_E_INVALID_EVENT:
      aoStrError = "DMUS_E_INVALID_EVENT"; break;
    case DMUS_E_INVALID_TOOL_HDR:
      aoStrError = "DMUS_E_INVALID_TOOL_HDR"; break;
    case DMUS_E_INVALID_TRACK_HDR:
      aoStrError = "DMUS_E_INVALID_TRACK_HDR"; break;
    case DMUS_E_INVALIDBUFFER:
      aoStrError = "DMUS_E_INVALIDBUFFER"; break;
    case DMUS_E_INVALIDFILE:
      aoStrError = "DMUS_E_INVALIDFILE"; break;
    case DMUS_E_INVALIDOFFSET:
      aoStrError = "DMUS_E_INVALIDOFFSET"; break;
    case DMUS_E_INVALIDPATCH:
      aoStrError = "DMUS_E_INVALIDPATCH"; break;
    case DMUS_E_INVALIDPOS:
      aoStrError = "DMUS_E_INVALIDPOS"; break;
    case DMUS_E_LOADER_BADPATH:
      aoStrError = "DMUS_E_LOADER_BADPATH"; break;
    case DMUS_E_LOADER_FAILEDCREATE:
      aoStrError = "DMUS_E_LOADER_FAILEDCREATE"; break;
    case DMUS_E_LOADER_FAILEDOPEN:
      aoStrError = "DMUS_E_LOADER_FAILEDOPEN"; break;
    case DMUS_E_LOADER_FORMATNOTSUPPORTED:
      aoStrError = "DMUS_E_LOADER_FORMATNOTSUPPORTED"; break;
    case DMUS_E_LOADER_NOCLASSID:
      aoStrError = "DMUS_E_LOADER_NOCLASSID"; break;
    case DMUS_E_LOADER_NOFILENAME:
      aoStrError = "DMUS_E_LOADER_NOFILENAME"; break;
    case DMUS_E_LOADER_OBJECTNOTFOUND:
      aoStrError = "DMUS_E_LOADER_OBJECTNOTFOUND"; break;
    case DMUS_E_NO_MASTER_CLOCK: 
      aoStrError = "DMUS_E_NO_MASTER_CLOCK"; break;
    case DMUS_E_NOARTICULATION:
      aoStrError = "DMUS_E_NOARTICULATION"; break;
    case DMUS_E_NOSYNTHSINK:
      aoStrError = "DMUS_E_NOSYNTHSINK"; break;
    case DMUS_E_NOT_DOWNLOADED_TO_PORT:
      aoStrError = "DMUS_E_NOT_DOWNLOADED_TO_PORT"; break;
    case DMUS_E_NOT_FOUND: 
      aoStrError = "DMUS_E_NOT_FOUND"; break;
    case DMUS_E_NOT_INIT:
      aoStrError = "DMUS_E_NOT_INIT"; break;
    case DMUS_E_NOTADLSCOL:
      aoStrError = "DMUS_E_NOTADLSCOL"; break;
    case DMUS_E_NOTMONO: 
      aoStrError = "DMUS_E_NOTMONO"; break;
    case DMUS_E_NOTPCM:
      aoStrError = "DMUS_E_NOTPCM"; break;
    case DMUS_E_OUT_OF_RANGE:
      aoStrError = "DMUS_E_OUT_OF_RANGE"; break;
    case DMUS_E_PORT_NOT_CAPTURE: 
      aoStrError = "DMUS_E_PORT_NOT_CAPTURE"; break;
    case DMUS_E_PORT_NOT_RENDER:
      aoStrError = "DMUS_E_PORT_NOT_RENDER"; break;
    case DMUS_E_PORTS_OPEN:
      aoStrError = "DMUS_E_PORTS_OPEN"; break;
    case DMUS_E_SEGMENT_INIT_FAILED:
      aoStrError = "DMUS_E_SEGMENT_INIT_FAILED"; break;
    case DMUS_E_SET_UNSUPPORTED:
      aoStrError = "DMUS_E_SET_UNSUPPORTED"; break;
    case DMUS_E_SYNTHACTIVE:
      aoStrError = "DMUS_E_SYNTHACTIVE"; break;
    case DMUS_E_SYNTHINACTIVE:
      aoStrError = "DMUS_E_SYNTHINACTIVE"; break;
    case DMUS_E_SYNTHNOTCONFIGURED:
      aoStrError = "DMUS_E_SYNTHNOTCONFIGURED"; break;
    case DMUS_E_TIME_PAST:
      aoStrError = "DMUS_E_TIME_PAST"; break;
    case DMUS_E_TOOL_HDR_NOT_FIRST_CK:
      aoStrError = "DMUS_E_TOOL_HDR_NOT_FIRST_CK"; break;
    case DMUS_E_TRACK_HDR_NOT_FIRST_CK:
      aoStrError = "DMUS_E_TRACK_HDR_NOT_FIRST_CK"; break;
    case DMUS_E_TRACK_NOT_FOUND:
      aoStrError = "DMUS_E_TRACK_NOT_FOUND"; break;
    case DMUS_E_TYPE_DISABLED:
      aoStrError = "DMUS_E_TYPE_DISABLED"; break;
    case DMUS_E_TYPE_UNSUPPORTED: 
      aoStrError = "DMUS_E_TYPE_UNSUPPORTED"; break;
    case DMUS_E_UNKNOWNDOWNLOAD:
      aoStrError = "DMUS_E_UNKNOWNDOWNLOAD"; break;
    case DMUS_E_UNKNOWN_PROPERTY:
      aoStrError = "DMUS_E_UNKNOWN_PROPERTY"; break;
    case DMUS_E_UNSUPPORTED_STREAM:
      aoStrError = "DMUS_E_UNSUPPORTED_STREAM"; break;
    case DMUS_E_WAVEFORMATNOTSUPPORTED:
      aoStrError = "DMUS_E_WAVEFORMATNOTSUPPORTED"; break;
    case DMUS_S_DOWN_OCTAVE:
      aoStrError = "DMUS_S_DOWN_OCTAVE"; break;
    case DMUS_S_END:
      aoStrError = "DMUS_S_END"; break;
    case DMUS_S_FREE:
      aoStrError = "DMUS_S_FREE"; break;
    case DMUS_S_LAST_TOOL:
      aoStrError = "DMUS_S_LAST_TOOL"; break;
    case DMUS_S_NOBUFFERCONTROL:
      aoStrError = "DMUS_S_NOBUFFERCONTROL"; break;
    case DMUS_S_OVER_CHORD:
      aoStrError = "DMUS_S_OVER_CHORD"; break;
    case DMUS_S_PARTIALDOWNLOAD:
      aoStrError = "DMUS_S_PARTIALDOWNLOAD"; break;
    case DMUS_S_PARTIALLOAD:
      aoStrError = "DMUS_S_PARTIALLOAD"; break;
    case DMUS_S_REQUEUE:
      aoStrError = "DMUS_S_REQUEUE"; break;
    case DMUS_S_STRING_TRUNCATED:
      aoStrError = "DMUS_S_STRING_TRUNCATED"; break;
    case DMUS_S_UP_OCTAVE:
      aoStrError = "DMUS_S_UP_OCTAVE"; break;
    case E_FAIL:
      aoStrError = "E_FAIL"; break;
    case E_INVALIDARG:
      aoStrError = "E_INVALIDARG"; break;
  //  case E_NOAGGREGATION:
  //    aoStrError = "E_NOAGGREGATION"; break;
    case E_NOINTERFACE:
      aoStrError = "E_NOINTERFACE"; break;
    case E_NOTIMPL:
      aoStrError = "E_NOTIMPL"; break;
    case E_OUTOFMEMORY:
      aoStrError = "E_OUTOFMEMORY"; break;
    case E_POINTER:
      aoStrError = "E_POINTER"; break;
    case REGDB_E_CLASSNOTREG:
      aoStrError = "REGDB_E_CLASSNOTREG"; break;
    case S_FALSE:
      aoStrError = "S_FALSE"; break;
    default:
      aoStrError = "DMERR_?"; break;
  }; // del swich  
}





