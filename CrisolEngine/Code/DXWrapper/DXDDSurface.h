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
// DXDDSurface.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Clase que encapsula el objeto DirectDrawSurface y ofrece metodos
//   para el trabajo con surfaces primarias y secundarias. Esta clase
//   sera una clase base que dispondra de los metodos comunes para
//   las clases encargadas de trabajar con superficies primarias y
//   complejas, no visibles (back) y texturas.
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
///////////////////////////////////////////////////////////////////////

#ifndef __DXDDSURFACE_H_
#define __DXDDSURFACE_H_

// Includes
#include "DXWrapper.h"
#include "DXDDDefs.h"

class DXDDClipper;

// Clase DXDDSurface
class DXDDSurface: public DXWrapper
{
protected:
  // Estructuras
  struct PixelInfo {
    DDPIXELFORMAT  PixelFormat;     // Informacion del formato del pixel
    DWORD          dwAlphaNumBits;  // Numero de bits para el componente alpha
    DWORD          dwAlphaShift;    // Bits a desplazar el valor alpha a la izq.
    DWORD          dwRNumBits;      // Numero de bits para el componente R
    DWORD          dwRShift;        // Bits a desplazar el valor R a la izq.
    DWORD          dwGNumBits;      // Numero de bits para el componente G
    DWORD          dwGShift;        // Bits a desplazar el valor G a la izq.
    DWORD          dwBNumBits;      // Numero de bits para el componente B
    DWORD          dwBShift;        // Bits a desplazar el valor B a la izq.    
  };

protected:  
  // Variables de miembro
  LPDIRECTDRAWSURFACE7		m_pDDSurface;     // Puntero a la superficie primaria  
  DWORD						m_dwWidth;        // Anchura de la surperficie
  DWORD						m_dwHeight;       // Altura de la surperficie
  PixelInfo				    m_RGBInfo;        // Toda la info. para manejar pixels
  DXDDDefs::DXDDSurfaceType m_SurfaceType;    // Tipo de superficie
  bool						m_bSrcColorKey;   // Color Key fuente activo / no activo
  DWORD						m_dwSrcColorKey;  // Color Key
  bool						m_bDestColorKey;  // Color Key destino activo / no activo
  DWORD						m_dwDestColorKey; // Color Key destino
  bool						m_bClipper;       // Flag de clipper asociado  
  DDSURFACEDESC2			m_DescLock;       // Descripcion de superficie bloqueada
  bool						m_bLock;          // Indica si se ha llamado a Lock      

public:
  // Constructor y desctructor
  DXDDSurface(void);
  ~DXDDSurface(void);

public:
  // Utilidades
  DWORD RGBToColor(const byte ucRed, 
				   const byte ucGreen, 
				   const byte ucBlue);   
protected:
  void SetPixelFormat(void);

public:    
  // Operaciones de bliteado
  inline bool SetColorKey(const byte ucR, 
						  const byte ucG, 
						  const byte ucB, 
                          const DXDDDefs::DXDDColorKeyType& dwTypeKey = DXDDDefs::SOURCECOLORKEY) { 
    return SetColorKey(RGBToColor(ucR, ucG, ucB), dwTypeKey); 
  }
  bool UnSetBltColorKey(const DXDDDefs::DXDDColorKeyType& dwTypeKey);
  bool Blt(const int& nPosX, 
		   const int& nPosY, 
		   DXDDSurface* const poSrcSurf, 
           const int& nScaledX = 0, 
		   const int& nScaledY = 0, 
		   LPRECT pSrcRect = NULL);
  bool BltMirror(const int& nPosX, 
				 const int& nPosY, 
				 DXDDDefs::DXDDMirrorType& dwMirrorFlag,
                 DXDDSurface* const poSrcSurf, 
				 const int& nScaledX = 0, 
				 const int& nScaledY = 0,
                 LPRECT pSrcRect = NULL);    
  inline bool BltColor(const byte ucR, 
					   const byte ucG, 
					   const byte ucB, 
                       LPRECT pDestRect = NULL) { 
    return BltColor(RGBToColor(ucR, ucG, ucB), pDestRect); 
  }
protected: 
 bool SetColorKey(const dword dwColor, 
                  const DXDDDefs::DXDDColorKeyType& dwTypeKey = DXDDDefs::SOURCECOLORKEY);  
 bool BltColor(const dword dwColor, 
			   LPRECT pDestRect = NULL);
 
public:
  // Trabajo con clipper
  bool SetClipper(DXDDClipper* const poDXClipper);
  bool UnSetClipper(void);  
  
public:
  // Trabajo a nivel de pixel
  bool Lock(void);
  bool UnLock(void);  
  inline void PutPixel(const int& nPosx, 
					   const int& nPosy, 
                       const byte ucR, 
					   const byte ucG, 
					   const byte ucB) { 
    PutPixel(nPosx, nPosy, RGBToColor(ucR, ucG, ucB)); 
  }
  DWORD GetPixel(const int& nPosx, const int& nPosy);
protected:
   void PutPixel(const int& nPosx, const int& nPosy, const dword dwColor);
  
public:
  // Operaciones con GDI
  bool GetDC(HDC* pDC);
  bool ReleaseDC(HDC& aDC);
  
public:
  // Recuperacion de superficie
  bool IsLost(void);
  bool Restore(void);

public:
  // Operaciones de consulta / informacion
  inline const LPDIRECTDRAWSURFACE7 GetObj(void) const { return m_pDDSurface; }  
  inline DXDDDefs::DXDDSurfaceType GetType(void) const { return m_SurfaceType; }
  inline DWORD GetWidth(void) const { return m_dwWidth; }
  inline DWORD GetHeight(void) const { return m_dwHeight; }
  inline DWORD GetBpp(void) const { return m_RGBInfo.PixelFormat.dwRGBBitCount; }
  inline bool IsClipperPresent(void)  const { return m_bClipper; }
  inline bool IsSrcColorKeyPresent(void) const { return m_bSrcColorKey; }
  inline bool IsDestColorKeyPresent(void) const { return m_bDestColorKey; }    

protected:
  // Metodos de apoyo
  virtual void Init(void) = 0;
  virtual void Clean(void) = 0;  
};
#endif
