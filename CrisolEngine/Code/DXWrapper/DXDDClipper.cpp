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
// - Para mas informacion consultar DXDDClipper.h
///////////////////////////////////////////////////////////////////////
#include "DXDDClipper.h"

#include "DXDDError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Constructor
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
DXDDClipper::DXDDClipper(void)
{
  // Inicializa vbles de miembro
  Init();
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa las vbles de miembro
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
void DXDDClipper::Init(void)
{
  // Inicializa vbles
  m_pDDClipper = NULL;
  m_bWindowClip = m_bClipList = false;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destructor
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
DXDDClipper::~DXDDClipper(void)
{
  // Libera recursos
  Clean();
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera recursos de memoria
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
void DXDDClipper::Clean(void)
{  
  // Libera objeto DirectDrawClipper si procede
  if (m_pDDClipper) {
    m_pDDClipper->Release();
    m_pDDClipper = NULL;
  } 
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea objeto DirectDrawClipper
// Parametros:
// - poDXDraw: Puntero a instancia DXDraw para poder crear el clipper.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema.
// Notas:
// - En caso de que la clase ya contenga un clipper, simplemente 
//   procedera a liberarlo.
////////////////////////////////////////////////////////////////////// 
bool DXDDClipper::Create(DXDraw* const poDXDraw)
{
  // SOLO si existe instancia a DXDraw pasada
  DXASSERT(poDXDraw != NULL);
  
  // En caso de existir un clipper lo libera
  if (m_pDDClipper != NULL) {
    Clean();
    Init();
  }

  // Procede a crear el objeto IDirectDrawClipper
  m_CodeResult = poDXDraw->GetObj()->CreateClipper(0, &m_pDDClipper, NULL);
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
// - Crea una estructura de recorte del tamaño del RECT pasado
// Parametros:
// - rect: Area de recorte a establece en el Clipper
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema.
// Notas:
// - MUY IMPORTANTE: Este metodo simplemente establece una region
//   en el objeto IDirectDrawClipper, encapsulado en esta clase. 
//   NO asocia el mismo a la surface en cuestion. Para ello, se 
//   debera de invocar al metodo SetClipper de la superficie 
//   interesada.
////////////////////////////////////////////////////////////////////// 
bool DXDDClipper::SetClip(const RECT& rect)
{
  // SOLO si existe objeto DirectDrawClipper
  DXASSERT(m_pDDClipper != NULL);  
   
  bool            bResult;  // Valor de retorno
  LPRGNDATA       pRgnData; // Puntero a las regiones
    
  // Inicializaciones  
  pRgnData = (LPRGNDATA) new BYTE[sizeof(RGNDATAHEADER) + 1*sizeof(RECT)];  
  memset(pRgnData, 0, sizeof(RGNDATAHEADER) + 1*sizeof(RECT));    
  bResult = true;    
  
  // Se procede a reservar memoria para la region y se inicializa
  pRgnData = (LPRGNDATA) new BYTE[sizeof(RGNDATAHEADER) + 1*sizeof(RECT)];
  
  // Se rellenan los campos de la estructura RGNDATAHEADER    
  pRgnData->rdh.dwSize = sizeof(RGNDATAHEADER); // Tamaño estructura
  pRgnData->rdh.iType = RDH_RECTANGLES;         // Tipo               
  pRgnData->rdh.nCount = 1;                     // 1 sola estructura de recorte            
  pRgnData->rdh.nRgnSize = 1*sizeof(RECT);      // Tamaño cabecera de region    
  pRgnData->rdh.rcBound = rect;                 // Demension max del area
    
  // Se copia a pRgnData la unica estructura RECT a establecer
  memcpy(pRgnData->Buffer, &rect, 1*sizeof(RECT));
  
  // Se procede a asociar la estructura pRgnData al Clipper
  m_CodeResult = m_pDDClipper->SetClipList(pRgnData, 0);
  if (FAILED(m_CodeResult)){
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode); 
    // Actualiza flag si ya existia una lista de recorte
    m_bClipList = false; 
    return false;
  }  
  
  // Actualiza flag
  m_bClipList = true; 
  
  // Antes de salir, se libera memoria de pRgnData 
  delete[] pRgnData;
  pRgnData = NULL;
  
  // Se retorna
  return bResult;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea una lista de areas de recorte, a partir del puntero a
//   la estructura RGNDATA recibido.
// Parametros:
// - pRgnData: Puntero a la estructura RGNDATA con la lista de
//   areas de recorte a establecer
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema.
// Notas:
// - MUY IMPORTANTE: Este metodo simplemente establece una region
//   en el objeto IDirectDrawClipper, encapsulado en esta clase. 
//   NO asocia el mismo a la surface en cuestion. Para ello, se 
//   debera de invocar al metodo SetClipper de la superficie 
//   interesada.
////////////////////////////////////////////////////////////////////// 
bool DXDDClipper::SetClipList(LPRGNDATA pRgnData)
{
  // SOLO si existe objeto DirectDrawClipper
  DXASSERT(m_pDDClipper != NULL);  
  // SOLO si existe estructura RgnData
  DXASSERT(pRgnData != NULL);
    
  // Se procede a asociar la estructura pRgnData al Clipper
  m_CodeResult = m_pDDClipper->SetClipList(pRgnData, 0);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode); 
    // Actualiza flag si ya existia una lista de recorte
    m_bClipList = false; 
    return false;
  }  
  
  // Actualiza flag
  m_bClipList = true; 
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece como clipper el handle a la ventana pasada, con lo que
//   sera Windows y DirectDraw, los encargados de llevar el peso del
//   recorte. 
// Parametros:
// - hWnd: Handle a la ventana en donde proteger del recorte.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema.
// Notas:
// - Este metodo es deseable SOLO cuando se trabaja en modo ventana,
//   ya que libera el tener que calcular, para cambio del tamaño de la
//   ventana, el area de recorte.
// - MUY IMPORTANTE: Este metodo simplemente establece una region
//   en el objeto IDirectDrawClipper, encapsulado en esta clase. 
//   NO asocia el mismo a la surface en cuestion. Para ello, se 
//   debera de invocar al metodo SetClipper de la superficie 
//   interesada.
////////////////////////////////////////////////////////////////////// 
bool DXDDClipper::SetWindowClip(const HWND& hWnd)
{
  // SOLO si existe objeto DirectDrawClipper
  DXASSERT(m_pDDClipper != NULL);  
  // SOLO si existe el handle
  DXASSERT(hWnd != NULL);
    
  // Se procede a asociar al handle de ventana
  m_CodeResult = m_pDDClipper->SetHWnd(0, hWnd);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode); 
    // Actualiza flag si ya existia una lista de recorte
    m_bWindowClip = false; 
    return false;
  }  
  
  // Actualiza flag
  m_bWindowClip = true; 
  
  // Todo correcto
  return true;
}