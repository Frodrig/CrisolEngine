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
// DXD3DError.cpp
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXcase D3DError.h.
///////////////////////////////////////////////////////////////////////
#include "DXD3DError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Convierte un codigo de error de las Direct3D a una cadena
//   BIRString.
// Parametros:
// - CodeError: Codigo de error producido.
// - szStrError: Referencia al objeto string en donde depositar la
//   cadena con el nombre del error.
// Devuelve:
// - szStrError: Contendra string con el codigo del error.
// Notas:
// - Esta funcion cubre todos los posibles errores de cada una de
//   las interfaces presentes en DirectSound v7.0.
// - En caso de que no se reconozca el error se devolvera el codigo
//   "case D3DERR_?".
////////////////////////////////////////////////////////////////////// 
void DXD3DError::DXD3DErrorToString(const HRESULT CodeError, 
                                    std::string& szStrError)
{
  // Evalua codigo de error recibido
  switch (CodeError)
  {
    case D3D_OK:
      szStrError = "D3D_OK"; break;
    case DDERR_INVALIDPARAMS:
      szStrError = "DDERR_INVALIDPARAMS"; break;
    case D3DERR_BADMAJORVERSION:
      szStrError = "D3DERR_BADMAJORVERSION"; break;
    case D3DERR_BADMINORVERSION:
      szStrError = "D3DERR_BADMINORVERSION"; break;
    case D3DERR_COLORKEYATTACHED:
      szStrError = "D3DERR_COLORKEYATTACHED"; break;
    case D3DERR_CONFLICTINGTEXTUREFILTER:
      szStrError = "D3DERR_CONFLICTINGTEXTUREFILTER"; break;
    case D3DERR_CONFLICTINGTEXTUREPALETTE:
      szStrError = "D3DERR_CONFLICTINGTEXTUREPALETTE"; break;
    case D3DERR_CONFLICTINGRENDERSTATE:
      szStrError = "D3DERR_CONFLICTINGRENDERSTATE"; break;
    case D3DERR_DEVICEAGGREGATED:
      szStrError = "D3DERR_DEVICEAGGREGATED"; break;
    case D3DERR_EXECUTE_CLIPPED_FAILED:
      szStrError = "D3DERR_EXECUTE_CLIPPED_FAILED"; break;
    case D3DERR_EXECUTE_CREATE_FAILED:
      szStrError = "D3DERR_EXECUTE_CREATE_FAILED"; break;
    case D3DERR_EXECUTE_DESTROY_FAILED:
      szStrError = "D3DERR_EXECUTE_DESTROY_FAILED"; break;
    case D3DERR_EXECUTE_FAILED:
      szStrError = "D3DERR_EXECUTE_FAILED"; break;
    case D3DERR_EXECUTE_LOCK_FAILED:
      szStrError = "D3DERR_EXECUTE_LOCK_FAILED"; break;
    case D3DERR_EXECUTE_LOCKED:
      szStrError = "D3DERR_EXECUTE_LOCKED"; break;
    case D3DERR_EXECUTE_NOT_LOCKED:
      szStrError = "D3DERR_EXECUTE_NOT_LOCKED"; break;
    case D3DERR_EXECUTE_UNLOCK_FAILED:
      szStrError = "D3DERR_EXECUTE_UNLOCK_FAILED"; break;
    case D3DERR_INBEGIN:
      szStrError = "D3DERR_INBEGIN"; break;
    case D3DERR_INBEGINSTATEBLOCK:
      szStrError = "D3DERR_INBEGINSTATEBLOCK"; break;
    case D3DERR_INITFAILED:
      szStrError = "D3DERR_INITFAILED"; break;
    case D3DERR_INVALID_DEVICE:
      szStrError = "D3DERR_INVALID_DEVICE"; break;
    case D3DERR_INVALIDCURRENTVIEWPORT:
      szStrError = "D3DERR_INVALIDCURRENTVIEWPORT"; break;
    case D3DERR_INVALIDMATRIX:
      szStrError = "D3DERR_INVALIDMATRIX"; break;
    case D3DERR_INVALIDPALETTE:
      szStrError = "D3DERR_INVALIDPALETTE"; break;
    case D3DERR_INVALIDPRIMITIVETYPE:
      szStrError = "D3DERR_INVALIDPRIMITIVETYPE"; break;
    case D3DERR_INVALIDRAMPTEXTURE:
      szStrError = "D3DERR_INVALIDRAMPTEXTURE"; break;
    case D3DERR_INVALIDSTATEBLOCK:
      szStrError = "D3DERR_INVALIDSTATEBLOCK"; break;
    case D3DERR_INVALIDVERTEXFORMAT:
      szStrError = "D3DERR_INVALIDVERTEXFORMAT"; break;
    case D3DERR_INVALIDVERTEXTYPE:
      szStrError = "D3DERR_INVALIDVERTEXTYPE"; break;
    case D3DERR_LIGHT_SET_FAILED:
      szStrError = "D3DERR_LIGHT_SET_FAILED"; break;
    case D3DERR_LIGHTHASVIEWPORT:
      szStrError = "D3DERR_LIGHTHASVIEWPORT"; break;
    case D3DERR_LIGHTNOTINTHISVIEWPORT:
      szStrError = "D3DERR_LIGHTNOTINTHISVIEWPORT"; break;
    case D3DERR_MATERIAL_CREATE_FAILED:
      szStrError = "D3DERR_MATERIAL_CREATE_FAILED"; break;
    case D3DERR_MATERIAL_DESTROY_FAILED:
      szStrError = "D3DERR_MATERIAL_DESTROY_FAILED"; break;
    case D3DERR_MATERIAL_GETDATA_FAILED: 
      szStrError = "D3DERR_MATERIAL_GETDATA_FAILED"; break;
    case D3DERR_MATERIAL_SETDATA_FAILED: 
      szStrError = "D3DERR_MATERIAL_SETDATA_FAILED"; break;
    case D3DERR_MATRIX_CREATE_FAILED: 
      szStrError = "D3DERR_MATRIX_CREATE_FAILED"; break;
    case D3DERR_MATRIX_DESTROY_FAILED: 
      szStrError = "D3DERR_MATRIX_DESTROY_FAILED"; break;
    case D3DERR_MATRIX_GETDATA_FAILED: 
      szStrError = "D3DERR_MATRIX_GETDATA_FAILED"; break;
    case D3DERR_MATRIX_SETDATA_FAILED: 
      szStrError = "D3DERR_MATRIX_SETDATA_FAILED"; break;
    case D3DERR_NOCURRENTVIEWPORT: 
      szStrError = "D3DERR_NOCURRENTVIEWPORT"; break;
    case D3DERR_NOTINBEGIN: 
      szStrError = "D3DERR_NOTINBEGIN"; break;
    case D3DERR_NOTINBEGINSTATEBLOCK: 
      szStrError = "D3DERR_NOTINBEGINSTATEBLOCK"; break;
    case D3DERR_NOVIEWPORTS: 
      szStrError = "D3DERR_NOVIEWPORTS"; break;
    case D3DERR_SCENE_BEGIN_FAILED: 
      szStrError = "D3DERR_SCENE_BEGIN_FAILED"; break;
    case D3DERR_SCENE_END_FAILED: 
      szStrError = "D3DERR_SCENE_END_FAILED"; break;
    case D3DERR_SCENE_IN_SCENE: 
      szStrError = "D3DERR_SCENE_IN_SCENE"; break;
    case D3DERR_SCENE_NOT_IN_SCENE: 
      szStrError = "D3DERR_SCENE_NOT_IN_SCENE"; break;
    case D3DERR_SETVIEWPORTDATA_FAILED: 
      szStrError = "D3DERR_SETVIEWPORTDATA_FAILED"; break;
    case D3DERR_STENCILBUFFER_NOTPRESENT: 
      szStrError = "D3DERR_STENCILBUFFER_NOTPRESENT"; break;
    case D3DERR_SURFACENOTINVIDMEM: 
      szStrError = "D3DERR_SURFACENOTINVIDMEM"; break;
    case D3DERR_TEXTURE_BADSIZE: 
      szStrError = "D3DERR_TEXTURE_BADSIZE"; break;
    case D3DERR_TEXTURE_CREATE_FAILED: 
      szStrError = "D3DERR_TEXTURE_CREATE_FAILED"; break;
    case D3DERR_TEXTURE_DESTROY_FAILED: 
      szStrError = "D3DERR_TEXTURE_DESTROY_FAILED"; break;
    case D3DERR_TEXTURE_GETSURF_FAILED: 
      szStrError = "D3DERR_TEXTURE_GETSURF_FAILED"; break;
    case D3DERR_TEXTURE_LOAD_FAILED: 
      szStrError = "D3DERR_TEXTURE_LOAD_FAILED"; break;
    case D3DERR_TEXTURE_LOCK_FAILED: 
      szStrError = "D3DERR_TEXTURE_LOCK_FAILED"; break;
    case D3DERR_TEXTURE_LOCKED: 
      szStrError = "D3DERR_TEXTURE_LOCKED"; break;
    case D3DERR_TEXTURE_NO_SUPPORT: 
      szStrError = "D3DERR_TEXTURE_NO_SUPPORT"; break;
    case D3DERR_TEXTURE_NOT_LOCKED: 
      szStrError = "D3DERR_TEXTURE_NOT_LOCKED"; break;
    case D3DERR_TEXTURE_SWAP_FAILED: 
      szStrError = "D3DERR_TEXTURE_SWAP_FAILED"; break;
    case D3DERR_TEXTURE_UNLOCK_FAILED: 
      szStrError = "D3DERR_TEXTURE_UNLOCK_FAILED"; break;
    case D3DERR_TOOMANYOPERATIONS: 
      szStrError = "D3DERR_TOOMANYOPERATIONS"; break;
    case D3DERR_TOOMANYPRIMITIVES: 
      szStrError = "D3DERR_TOOMANYPRIMITIVES"; break;
    case D3DERR_UNSUPPORTEDALPHAARG: 
      szStrError = "D3DERR_UNSUPPORTEDALPHAARG"; break;
    case D3DERR_UNSUPPORTEDALPHAOPERATION: 
      szStrError = "D3DERR_UNSUPPORTEDALPHAOPERATION"; break;
    case D3DERR_UNSUPPORTEDCOLORARG: 
      szStrError = "D3DERR_UNSUPPORTEDCOLORARG"; break;
    case D3DERR_UNSUPPORTEDCOLOROPERATION: 
      szStrError = "D3DERR_UNSUPPORTEDCOLOROPERATION"; break;
    case D3DERR_UNSUPPORTEDFACTORVALUE: 
      szStrError = "D3DERR_UNSUPPORTEDFACTORVALUE"; break;
    case D3DERR_UNSUPPORTEDTEXTUREFILTER: 
      szStrError = "D3DERR_UNSUPPORTEDTEXTUREFILTER"; break;
    case D3DERR_VBUF_CREATE_FAILED: 
      szStrError = "D3DERR_VBUF_CREATE_FAILED"; break;
    case D3DERR_VERTEXBUFFERLOCKED: 
      szStrError = "D3DERR_VERTEXBUFFERLOCKED"; break;
    case D3DERR_VERTEXBUFFEROPTIMIZED: 
      szStrError = "D3DERR_VERTEXBUFFEROPTIMIZED"; break;
    case D3DERR_VERTEXBUFFERUNLOCKFAILED: 
      szStrError = "D3DERR_VERTEXBUFFERUNLOCKFAILED"; break;
    case D3DERR_VIEWPORTDATANOTSET: 
      szStrError = "D3DERR_VIEWPORTDATANOTSET"; break;
    case D3DERR_VIEWPORTHASNODEVICE: 
      szStrError = "D3DERR_VIEWPORTHASNODEVICE"; break;
    case D3DERR_WRONGTEXTUREFORMAT: 
      szStrError = "D3DERR_WRONGTEXTUREFORMAT"; break;
    case D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY: 
      szStrError = "D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY"; break;
    case D3DERR_ZBUFF_NEEDS_VIDEOMEMORY: 
      szStrError = "D3DERR_ZBUFF_NEEDS_VIDEOMEMORY"; break;
    case D3DERR_ZBUFFER_NOTPRESENT: 
      szStrError = "D3DERR_ZBUFFER_NOTPRESENT"; break;
    default:
      szStrError = "D3DERR_?"; break;
  }; // del swich
}




