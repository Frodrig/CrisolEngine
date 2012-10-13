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
// DXDDSurfaceTexture.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Clase que encapsula el trabajo con superficies de tipo textura. 
//   Estas superficies son las utilizadas por Direct3D. Sobre ellas 
//   se puede realizar cualquier tipo de operacion habitual (como si 
//   se tratara de una superificie para usar solo en DirectDraw).
// - Estas son las unicas superficies que se pueden asociar a poligonos.
//
// Notas:
// - Clase optimizada para DirectX 7.0.
// - Las operaciones que trabajan a nivel de pixel o con componentes
//   de color (todas ellas heredadas de DXDDSurface), han sido
//   sobrecargadas para que puedan recibir el componente alpha.
///////////////////////////////////////////////////////////////////////
#ifndef __DXDDSURFACETEXTURE_H_
#define __DXDDSURFACETEXTURE_H_

// Includes
#include "DXDDSurface.h"
#include "DXDDDefs.h"

class DXDraw;

// Clase DXDDSurfaceTexture
class DXDDSurfaceTexture: public DXDDSurface
{  
public:
  // Vbles de miembro
  bool m_bHardware; // ¿Texture en Hardware?
  bool m_bPow2;     // ¿Textura cuyas dimensiones son potencia de 2?
  bool m_bSquare;   // ¿Textura cuadrada?

public:
  // Constructor y desctructor
  DXDDSurfaceTexture(void);
  ~DXDDSurfaceTexture(void);

public:  
  // Creacion y configuracion de la superficie
  bool Create(const DXDraw* const pDXDraw,
              const dword dwWidth, 
			  const dword dwHeight,
              const DDPIXELFORMAT* const pPixelFormat, 
              const bool bHardware, 
			  const bool bPow2, 
			  const bool bSquare);
  void Destroy(void) { Clean(); Init(); }

public:
  // Utilidades
  DWORD RGBAToColor(const byte ucRed, 
					const byte ucGreen, 
					const byte ucBlue,
                    const byte ucAlpha);
  
public:    
  // Operaciones de bliteado
  inline bool SetColorKey(const byte ucR, 
						  const byte ucG, 
						  const byte ucB, 
						  const byte ucA, 
                          const DXDDDefs::DXDDColorKeyType& dwTypeKey = DXDDDefs::SOURCECOLORKEY) { 
    return DXDDSurface::SetColorKey(RGBAToColor(ucR, ucG, ucB, ucA), dwTypeKey); 
  }
  inline bool BltColor(const byte ucR, 
					   const byte ucG, 
					   const byte ucB, 
                       const byte ucA, LPRECT pDestRect = NULL) { 
    return DXDDSurface::BltColor(RGBAToColor(ucR, ucG, ucB, ucA), pDestRect); 
  }

public:
  // Trabajo a nivel de pixel
  inline void PutPixel(const int& nPosx, 
					   const int& nPosy, 
					   const byte ucR, 
                       const byte ucG, 
					   const byte ucB, 
					   const byte ucA)  { 
    DXDDSurface::PutPixel(nPosx, nPosy, RGBAToColor(ucR, ucG, ucB, ucA)); 
  }

public:
  // Operaciones de consulta
  inline bool IsInHardware(void) const { return m_bHardware; }
  inline bool IsPow2(void) const { return m_bPow2; }
  inline bool IsSquare(void) const { return m_bSquare; }
  
protected:
  // Metodos de apoyo
  void Init(void);
  void Clean(void);  
};
#endif
