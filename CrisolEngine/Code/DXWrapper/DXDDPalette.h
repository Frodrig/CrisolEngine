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
// DXDDPalette.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Clas que encapsula el trabajo con DirectDrawPalette, para el manejo
//   de paletas en modos de video de 8 Bpp. 
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
// - Para trabajar con paletas, basta con que se asocie una, la comun,
//   al front buffer. En cualquier momento, se pude quitar una paleta
//   del mismo, y asociar otra distinta. Esto permite la generacion
//   de efectos especiales y poder trabajar con un amplio numero de
//   paletas distintas de forma sencilla. Los cambios seran 
//   instantaneos.
///////////////////////////////////////////////////////////////////////

#ifndef __DXDDPALETTE_H_
#define __DXDDPALETTE_H_

// Includes
#include "DXWrapper.h"
#include "DXDDDefs.h"

class DXDraw;

// Clase DXDDPalette
class DXDDPalette: public DXWrapper
{
private:
  // Variables de miembro
  LPDIRECTDRAWPALETTE  m_pDDPalette;     // Objeto DirectDrawPalette  
  
public:
  // Constructor y destructor
  DXDDPalette();
  ~DXDDPalette();

public:
  // Operaciones de creacion y manipulacion de paleta  
  bool Create(DXDraw* const poDXDraw,  
			  PALETTEENTRY* const pPalEntry);  
  void Destroy(void) { Clean(); Init(); }
  bool SetEntries(LPPALETTEENTRY pNewPalette, 
                  const word wStart = 1, 
				  const word wEnd = 254);
  bool GetEntries(LPPALETTEENTRY pPalette, 
                  const word wStart = 1, 
				  const word wEnd = 254);
  bool Fill(const byte bColor, 
			const word wStart = 1, 
			const word wEnd = 254);
  
public:
  // Operaciones de consulta
  inline const LPDIRECTDRAWPALETTE GetObj(void) const { return m_pDDPalette; }
  
protected:  
  // Metodos de apoyo
  void Init(void);
  void Clean(void);
};
#endif
  


