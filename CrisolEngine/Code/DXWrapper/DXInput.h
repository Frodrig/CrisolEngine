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
// DXInput.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion: 
// - Clase que encapsula la creacion de un objeto DirectInput. Su
//   finalidad es la de servir de punto de apoyo al resto de clases
//   que componen el wrapper de DirectInput, para el trabajo con el
//   raton y el teclado.
//
// Notas:
// - Clase optimizada para DirectX 7.0.
// - Para su correcta compilacion hara falta incluir la libreria
//   "DInput.lib". 
// - Todas las clases DX deben de heredar de la clase base DXWrapper 
//   e implementar los metodos Init y Clean. 
// - Todas las estructuras, constantes, tipos enumerados, etc que
//   utilice la libreria de clases para DirectInput estaran en el
//   archivo de cabecera "DXDIDefs.h".
// - Para poder trabajar con la funcion que transforma codigos de error
//   de DirectInput a cadenas de caracteres se incluye "DXDIError.h".
///////////////////////////////////////////////////////////////////////
#ifndef _DXInput_H_
#define _DXInput_H_

// Includes
#include "DXWrapper.h"
#include "DXDIDefs.h"

// Acceso directo a los espacios de nombre

// Clase DXInput
class DXInput: public DXWrapper
{
protected:
  // Vbles de miembro
  LPDIRECTINPUT7 m_lpDI; // Objeto DirectInput
  HINSTANCE		 m_Inst; // Instancia de la aplicacion

public:
  // Constructores y destructores
  DXInput(void);	
  ~DXInput(void);

public:
  // Operaciones sobre el objeto DirectInput
  bool Create(const HINSTANCE& hInst);
  void Destroy(void) { Clean(); Init(); }
  inline const LPDIRECTINPUT7 GetObj(void) const { return m_lpDI; }
  
protected:
  // Inicializacion y liberacion de recursos <DXWrapper>
  void Init(void);
  void Clean(void);  
};
#endif
