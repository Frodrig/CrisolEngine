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
// DXDDSurfaceBack.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Clase que encapsula el trabajo con superficies no visibles. Tambien
//   sirve para definir BackBuffer que despues podran asociarse a superfies
//   complejas. Hereda de DXDDSurface   
//
// Notas:
// - Clase optimizada para DirectX 7.0.
///////////////////////////////////////////////////////////////////////
#ifndef __DXDDSURFACEBACK_H_
#define __DXDDSURFACEBACK_H_

// Includes
#include "DXDDSurface.h"
#include "DXDDDefs.h"

class DXDDSurfaceScreen;
class DXDraw;

// Clase DXDDSurfaceBack
class DXDDSurfaceBack: public DXDDSurface
{
  // Se declara DXDDSurfaceScreen
  friend class DXDDSurfaceScreen; 

public:
  // Constructor y desctructor
  DXDDSurfaceBack(void);
  ~DXDDSurfaceBack(void);

public:  
  // Creacion y configuracion de la superficie
  bool Create(const DXDraw* const poDXDraw, 
			  const dword dwWidth, 
              const dword dwHeight, 
			  const DXDDDefs::DXDDMemType& wMem = DXDDDefs::VIDEOMEM); 
  void Destroy(void) { Clean(); Init(); }

protected:
  // Metodos de apoyo
  void Init(void);
  void Clean(void);  
};
#endif
