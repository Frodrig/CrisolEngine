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
// DXDDClipper.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Clase que encapsula el trabajo con el objeto DirectDrawClipper,
//   para el trabajo con superficies que gestionan el recorte de las
//   operaciones de blitting.
//
// Notas:
// - Clase optimizada para DirectX 7.0.
// - Para su correcta compilacion hara falta incluir la libreria
//   "DDraw.lib" y el archivo de cabecera "ddraw.h". Este recurso se
//   encentran incluidos en el archivo "DXDDDefs.h".
// - Todas las estructuras, constantes, tipos enumerados, etc que
//   utilice la libreria de clases para DirectDraw estaran en el
//   archivo de cabecera "DXDDDefs.h".
// - Para poder trabajar con la funcion que transforma codigos de error
//   de DirectDraw a cadenas de caracteres se incluye "DXDDError.h".
// - El uso habitual es asociar un unico clipper al backbuffer.
///////////////////////////////////////////////////////////////////////
#ifndef __DXDDCLIPPER_H_
#define __DXDDCLIPPER_H_

// Includes
#include "DXWrapper.h"
#include "DXDraw.h"

// Clase DXDDClipper
class DXDDClipper: public DXWrapper
{
// Variables de miembro
protected:
  LPDIRECTDRAWCLIPPER  m_pDDClipper;  // Objeto DirectDrawClipper  
  bool                 m_bWindowClip; // Flag de clipper a ventana
  bool                 m_bClipList;   // Flag de lista de recorte  
    
public:
  // Constructor y destructor
  DXDDClipper(void);
  ~DXDDClipper(void);

public:
  // Operaciones sobre el clipper
  bool Create(DXDraw* poDXDraw);  
  void Destroy(void) { Clean(); Init(); }
  bool SetClip(const RECT& rect);
  bool SetClipList(LPRGNDATA pRgnData);
  bool SetWindowClip(const HWND& hWnd);

public:
  // Operaciones de consulta
  inline const LPDIRECTDRAWCLIPPER GetObj(void) const { return m_pDDClipper; }
  inline bool IsClipList(void) const { return m_bClipList; }  
  inline bool IsWindowClip(void) const { return m_bWindowClip; }  
    
protected:  
  // Metodos de apoyo
  void Init(void);
  void Clean(void);
};

#endif
  


