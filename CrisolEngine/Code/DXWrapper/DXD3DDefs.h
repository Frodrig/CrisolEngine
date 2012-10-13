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
// DXD3DDefs.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Se define el espacio de nombres con todas las definiciones y 
//   tipos existentes en el ambito DXD3D
//
// Notas:
///////////////////////////////////////////////////////////////////////
#ifndef __DXD3DDEFS_H_
#define __DXD3DDEFS_H_

#include "d3d.h"
#include "d3dtypes.h"

namespace DXD3DDefs
{
  // Numero maximo de vertices para un quad
  #define MAXVERTEXQUAD 4 
  
  // Tipos enumerados
  enum DXD3DDeviceType { 
	// Tipos de Devices    
    D3DDEVICE_TNT      = 0, // Tarjeta TNT
    D3DDEVICE_HARDWARE = 1, // Dispositivo hard por defecto
    D3DDEVICE_MMX      = 2, // Dispositivo MMX
    D3DDEVICE_SOFTWARE = 3  // Dispositivo por software
  };

  enum DXD3DDrawMode { 
	// Tipos de modos de dibujado
    D3DDRAW_SOLID = D3DFILL_SOLID,        // Dibujado normal (solido)
    D3DDRAW_POINT = D3DFILL_POINT,        // Dibujado a puntos
    D3DDRAW_WIREFRAME = D3DFILL_WIREFRAME // Dibujado en wireframe    
  };

  enum DXD3DStages { 
	// Tipos de textures stages
    D3DSTAGE0 = 0, // Stage 0
    D3DSTAGE1      // Stage 1
  };

  enum DXD3DVertex { 
	// Tipos de vertices con los que trabajar con quads
    // Dependiendo del quad, la localizacion de los vertices 
    // sera distinta
    VERTEX0 = 0, // Vertice 0
    VERTEX1,     // Vertice 1
    VERTEX2,     // Vertice 2
    VERTEX3      // Vertice 3
  };

} // namespace

#endif