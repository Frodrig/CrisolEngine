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

///////////////////////////////////////////////////////////////////////////////
// CrisolEngine - Computer Role-Play Game Engine
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
// DXD3D.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Esta clase se encarga de encapsular el objeto Direct3D, para 
//   trabajar en modo grafico 3D.
//     
// Notas:
// - Clase optimizada para DirectX 7.0.
// - Todas las clases DX deben de heredar de la clase base DXWrapper 
//   e implementar los metodos Init y Clean. 
// - Todas las estructuras, constantes, tipos enumerados, etc que
//   utilice la libreria de clases para Direct3D estaran en el
//   archivo de cabecera "DXD3DDefs.h".
// - Para poder trabajar con la funcion que transforma codigos de error
//   de Direct3D a cadenas de caracteres se incluye "DXD3DError.h".
///////////////////////////////////////////////////////////////////////

#ifndef __DXD3D_H_
#define __DXD3D_H_

// Includes
#include "DXWrapper.h"
#include "DXD3DDefs.h"

class DXDraw;

// Clase DXD3D
class DXD3D: public DXWrapper
{  
protected:
  // Variables de miembro
  LPDIRECT3D7  m_lpD3D;  // Objeto Direct3D

public:
  // Constructor y destructor
  DXD3D(void);
  ~DXD3D(void);

public:
  // Operaciones sobre el objeto Direct3D
  bool Create(const DXDraw* const pDXDraw);
  void Destroy(void) { Clean(); Init(); }
  inline const LPDIRECT3D7 GetObj(void) const { return m_lpD3D; }

protected:
  // Metodos de apoyo
  void Init(void);
  void Clean(void);
}; 
#endif