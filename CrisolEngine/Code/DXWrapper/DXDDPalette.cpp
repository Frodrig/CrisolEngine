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
// - Para mas informacion consultar DXDDPalette.h
///////////////////////////////////////////////////////////////////////
#include "DXDDPalette.h"

#include "DXDDError.h"
#include "DXDraw.h"   

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Constructor
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
DXDDPalette::DXDDPalette(void)
{
  // Inicializa vbles de miembro
  Init();  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la inicializacion de las vbles de miembro
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
void DXDDPalette::Init(void)
{
  // Inicializa variables de miembro
  m_pDDPalette = NULL;  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destructor
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
DXDDPalette::~DXDDPalette(void)
{
  // Libera objetos
  Clean();
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la operacion de liberacion de memoria de los objetos
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
void DXDDPalette::Clean(void)
{  
  // Libera objeto si procede
  if (m_pDDPalette) {
    m_pDDPalette->Release();
    m_pDDPalette = NULL;
  } 
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un objeto IDirectDrawPalette asociandole la paleta recibida.
// Parametros:
// - poDXDraw: Instancia a la clase DXDraw con el objeto IDirectDraw.
// - pPalEntry: Paleta a asociar al objeto IDirectDrawPalette.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema
// Notas:
// - Este metodo solo esta preparado para trabajar en la creacion e
//   paletas de 8 Bpp.
////////////////////////////////////////////////////////////////////// 
bool DXDDPalette::Create(DXDraw* const poDXDraw, 
						 PALETTEENTRY* const pPalEntry)
{
  // SOLO si existe instancia a DXDraw pasada
  DXASSERT(poDXDraw != NULL);
  // SOLO si se esta en 8 Bpp
  DXASSERT(poDXDraw->GetDisplayBpp() == 8);
  // SOLO si el puntero a la paleta es valido
  DXASSERT(pPalEntry != NULL);

  // Si ya existia un objeto paleta lo libera
  if (m_pDDPalette != NULL) {
    Clean();
    Init();
  }
  
  // Se procede a crear la paleta para 8 Bpp
  m_CodeResult = poDXDraw->GetObj()->CreatePalette(DDPCAPS_8BIT,
                                                   pPalEntry,
                                                   &m_pDDPalette,
                                                   NULL);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode); 
    return false;
  }  

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Sustituye la paleta actual por la recibida en NewPalette, en el
//   rango wStart y wEnd.
// Parametros:
// - NewPalette: Contiene referencia a la nueva paleta.
// - wStart y wEnd: Rango de inicio y fin de donde establecer los datos
//   de la nueva paleta. Por defecto a las posiciones 1 y 254.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema
// Notas:
// - Por defecto wStart y wEnd tienen los valores 1 y 254, para respetar
//   el convenio.
////////////////////////////////////////////////////////////////////// 
bool DXDDPalette::SetEntries(LPPALETTEENTRY pNewPalette, 
                             const word wStart, 
							 const word wEnd)
{
  // Se procede a establecer la nueva paleta.
  m_CodeResult = m_pDDPalette->SetEntries(0, wStart, wEnd, pNewPalette);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode); 
    return false;
  }  

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene, en el rango indicado, la paleta asociada al objeto 
//   DirecDrawPalette
// Parametros:
// - pPalette: puntero a paleta a devolver
// - wStart y wEnd: Rango de inicio y fin de donde tomar los datos
//   de la paleta de DirectDrawPalette. Por defecto a las 
//   posiciones 1 y 254.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema
// Notas:
// - Por defecto wStart y wEnd tienen los valores 1 y 254, para respetar
//   el convenio.
////////////////////////////////////////////////////////////////////// 
bool DXDDPalette::GetEntries(LPPALETTEENTRY pPalette, 
                             const word wStart, 
							 const word wEnd)
{
  // SOLO si existe objeto DirectDrawPalette
  DXASSERT(m_pDDPalette != NULL);
  // SOLO si los intervalos de lectura de la paleta son correctos
  DXASSERT(wStart <= wEnd && wEnd <= 255 && wStart >= 0);

  // Se toma
  m_CodeResult = m_pDDPalette->GetEntries(0, wStart, wEnd, pPalette);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode); 
    return false;
  }  

  // Todo correcto
  return true;
}  

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Rellena la paleta con un color, en el intervalo que se
//   especifique.   
// Parametros:
// - bColor: Color a rellenar.
// - wStart y wEnd: Rango de inicio y fin de donde rellenar. Por 
//   defecto a las posiciones 1 y 254.
//   de la paleta de DirectDrawPalette.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema
// Notas:
// - Por defecto wStart y wEnd tienen los valores 1 y 254, para respetar
//   el convenio.
////////////////////////////////////////////////////////////////////// 
bool DXDDPalette::Fill(const byte bColor, 
					   const word wStart, 
					   const word wEnd)
{  
  // SOLO si existe objeto DirectDrawPalette
  DXASSERT(m_pDDPalette != NULL);
  // SOLO si color pasado es correcto (0-256)
  DXASSERT (bColor <= 255);
  // SOLO si los intervalos de lectura de la paleta son correctos
  DXASSERT(wStart <= wEnd && wEnd <= 255 && wStart >= 0);
  
  PALETTEENTRY*   pPalette;  // Paleta auxiliar
  
  // Inicializaciones
  pPalette = NULL;
  
  // Toma la paleta
  if (!GetEntries(pPalette, wStart, wEnd)) {
    // Hay problemas; El codigo de error asignado por GetEntries
    return false;
  }
  
  // Modifica las entradas de la paleta
  for(int i = wStart; i < wEnd; i++) {    
    pPalette[i].peRed = bColor;
    pPalette[i].peGreen = bColor;
    pPalette[i].peBlue = bColor;
  }

  // Asocia la paleta modificada
  if (!SetEntries(pPalette, wStart, wEnd)) {
    // Hay problemas; El codigo de error asignado por SetEntries
    return false;
  }

  // Todo correcto
  return true;
}
