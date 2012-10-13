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
// DXDMDefs.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Realiza la inclusion de cabeceras necesarias y globales para
//   todas las clases DirecMusic.
// - Incorpora las declaraciones de constantes, tipos enumerados, etc
//   utilizados en la libreria de clases de DirectMusic.
//
// Notas:
///////////////////////////////////////////////////////////////////////
#ifndef __DXDMDEFS_H_
#define __DXDMDEFS_H_

// Includes
#include <dmusicc.h>
#include <dmusici.h>
#include <direct.h>
#include <dsound.h>

namespace DXDMDefs {
  // Macros
  // Pasa el formato ByteCode a UniCode
  #define MULTI_TO_WIDE(x,y)  MultiByteToWideChar(CP_ACP, \
							  MB_PRECOMPOSED, y, -1, x, _MAX_PATH );
}
#endif