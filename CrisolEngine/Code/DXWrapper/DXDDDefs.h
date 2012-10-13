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
// DXDDDefs.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Se define el espacio de nombres con todas las definiciones y 
//   tipos existentes en el ambito DXDraw
//
// Notas:
///////////////////////////////////////////////////////////////////////
#ifndef __DXDDDEFS_H_
#define __DXDDDEFS_H_

#include <ddraw.h> 

namespace DXDDDefs
{
  // Tipos enumerados
  enum DXDDCooperativeType
  {// Tipos de modos cooperativos
    FULLSCREEN = 0, // Modo exclusivo (pantalla completa)
    WINDOW          // Modo ventana
  };

  enum DXDDMemType
  {// Tipos de memoria
    VIDEOMEM = 0, // Memoria de video
    SYSTEMMEM     // Memoria de sistema
  };

  enum DXDDSurfaceType
  {// Tipos de surfaces
    SURFACETYPE_NOCREATED = 0,  // Superficie no creada
    SURFACETYPE_PRIMARY,        // Superficie primaria simple
    SURFACETYPE_COMPLEX,        // Superficie primaria compleja  
    SURFACETYPE_ATTACHED,       // Superficie asociada a primaria
    SURFACETYPE_BACK,           // Superficie secundaria
    SURFACETYPE_TEXTURE         // Superficie de tipo textura
  };

  enum DXDDRGBComponent
  {// Tipos de componentes RGB
    REDCOMPONENT = 0,   // Componente R (Rgb)
    GREENCOMPONENT,     // Componente G (rGb)
    BLUECOMPONENT       // Componente B (rgB)
  };

  enum DXDDColorKeyType
  {// Tipos de Color Key para la operacion Blit
    SOURCECOLORKEY = DDCKEY_SRCBLT,       // Color Key de origen
    DESTINATIONCOLORKEY = DDCKEY_DESTBLT  // Color Key de destino 
  };

  enum DXDDMirrorType
  {// Tipos de operaciones Blt con efecto espejo
    HMIRROR = DDBLTFX_MIRRORLEFTRIGHT,                         // Horizontal
    VMIRROR = DDBLTFX_MIRRORUPDOWN,                            // Vertical
    HVMIRROR = DDBLTFX_MIRRORLEFTRIGHT  | DDBLTFX_MIRRORUPDOWN // Diagonal
  };
} // namespace

#endif