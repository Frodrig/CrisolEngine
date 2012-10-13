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
// DXDDSurfaceScreen.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Clase que trabaja con las superficies que representaran la pantalla,
//   es decir, con superficies primarias y complejas. 
//   Hereda de DXDDSurface.
//
// Notas:
///////////////////////////////////////////////////////////////////////
#ifndef __DXDDSURFACESCREEN_H_
#define __DXDDSURFACESCREEN_H_

// Includes
#include "DXDDSurface.h"
#include "DXDraw.h"

class DXDDSurfaceBack;
class DXDDPalette;

// Clase DXDDSurface
class DXDDSurfaceScreen: public DXDDSurface
{
// Variables de miembro
protected:
  DXDDSurfaceBack*          m_poBackBuffer;   // Puntero al BackBuffer (si procede)
  LPDIRECTDRAWGAMMACONTROL  m_lpGammaControl; // Objeto para controlar el gamma
  DDGAMMARAMP               m_GammaRamp;      // Valores gamma
  WORD                      m_wFadeSpeed;     // Rapidez en el efecto gamma
  BYTE                      m_ucFadeWait;     // Valor de espera entre fade
  bool                      m_bPalette;       // Flag de paleta asociada

public:
  // Constructor y desctructor
  DXDDSurfaceScreen(void);
  ~DXDDSurfaceScreen(void);

public:  
  // Creacion y configuracion de la superficie primaria y compleja
  bool CreatePrimary(const DXDraw* const poDXDraw, 
					 const DXDDDefs::DXDDMemType& wMem = DXDDDefs::VIDEOMEM,
                     const bool bD3DSurface = false);  
  bool CreateComplex(const DXDraw* const poDXDraw, 
					 const byte ubNumBackBuffers,
                     DXDDSurfaceBack* const poBackBuffer, 
					 const DXDDDefs::DXDDMemType& wMem = DXDDDefs::VIDEOMEM,
                     const bool bD3DSurface = false);
  void Destroy(void) { Clean(); Init(); }

private:
  // Metodos de apoyo en para el trabajo con una superficie compleja
  bool CreateComplex(const DXDraw* const poDXDraw, 
					 const byte ubNumBackBuffers,
                     const DXDDDefs::DXDDMemType& wMem = DXDDDefs::VIDEOMEM, 
					 const bool bD3DSurface = false);  
  bool AttachToComplex(DXDDSurfaceBack* const poAttachSurface);
  inline DXDDSurfaceBack* const GetBackBuffer(void) const { return m_poBackBuffer; }

public:
  // Trabajo con paletas
  bool SetPalette(DXDDPalette* const poDXPalette);  
  bool UnSetPalette(void);
  inline bool IsPalettePresent(void) { return m_bPalette; }

public:  
  // Operaciones de intercambio de paginas y bliteado
  bool Flip(const dword dwFlags = DDFLIP_WAIT | DDFLIP_NOVSYNC);

protected:
  // Metodos de apoyo para la inicializacion y liberacion
  void Init(void);
  void Clean(void);  
};
#endif
