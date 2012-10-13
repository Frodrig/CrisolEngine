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
// DXWrapper.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Clase base para los wrappers de DirectX. Incluye todos los
//   atributos y metodos que son comunes en todos (independientemente
//   del tipo que sea) los modulos DX (DXDraw, DXSound, DXMusic,
//   DXInput y DXPlay).
//
// Notas:
// - Clase base pura. Las clases derivadas deberan de implementar
//   dos metodos llamados Init y Clean para la inicializacion y
//   liberacion de vbles de miembro, respectivamente.
// - El wrapper usa la libreria STLport en lugar de apoyarse en la
//   que viene por defecto en el Visual C++ 6.0 (la cual no protege
//   de operaciones multihilo y no tiene tanta funcionalidad como
//   esta).
///////////////////////////////////////////////////////////////////////
#ifndef __DXWRAPPER_H_
#define __DXWRAPPER_H_

// Includes
#include "stdafx.h"
#include <string>

#ifdef _DEBUG
  #include <assert.h>
#endif

// Macro para debug
#ifdef _DEBUG
  #define DXASSERT(exp) assert(exp)
#else
  #define DXASSERT(exp)
#endif

// Clase DXWrapper
class DXWrapper
{
protected:
  // Vbles de miembro
  std::string  m_sLastCode;   // Ultimo codigo de error obtenido <string>
  HRESULT      m_CodeResult;  // Ultimo codigo de operacion

public:  
  // Operaciones inline para manejo de vbles de depuracion
  inline std::string GetLastStrCode(void) const { return m_sLastCode; }
  inline HRESULT GetLastCodeResult(void) const { return m_CodeResult; }
};
#endif
