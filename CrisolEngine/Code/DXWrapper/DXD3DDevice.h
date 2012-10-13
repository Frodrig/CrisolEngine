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
// DXD3DDevice.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Esta clase encapsula el Device 3D, esto es, el dispositivo que se
//   encargara de dibujar en 3D sobre la pantalla. Tambien mantendra
//   el viewport (area de la pantalla, en donde se realizaran las
//   operaciones).
// - ADVERTENCIA: Es muy importante que desde el exterior se llame el
//   minimo numero de veces a operaciones como SetTexture, Draw, o las
//   RTX y STX, pues se sufrira un importante bajon en las fps. Se espera
//   que desde el exterior existan algoritmos que se encarguen de realizar
//   unas llamadas a los metodos del wrapper lo mas economicas posibles.
// - La clasificacion del modo en que se podran realizar las diferentes
//   pruebas, sera el siguiente:
//   * Color Transparente (uso del canal alpha en la textura): 
//     RSAlphaBlending(true)
//     TSAlphaFromTexture
//     Draw
//     RSAlphaBlending(false)
//   * Fundidos (uso del canal alpha en los vertices del quad): 
//     RSAlphaBlending(true)
//     TSAlphaFromVertex
//     Draw
//     RSAlphaBlending(false)
//   * Color Transparente + Fundidos (alpha en textura y vertices): 
//     RSAlphaBlending(true)
//     TSAlphaFromTextureAndVertex
//     Draw
//     RSAlphaBlending(false)
//   * Efecto Bilinear al Aumentar
//     TSMagBilinearFiltering
//     Draw
//   * Efecto Bilinear al Disminuir
//     TSMinBilinearFiltering
//     Draw
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

#ifndef __DXD3DDEVICE_H_
#define __DXD3DDEVICE_H_

// Includes
#include "DXWrapper.h"
#include "DXD3D.h"
#include <vector>

class DXDDSurfaceBack;
class DXDDSurfaceTexture;
class DXD3DQuad;

// Clase DXD3DDevice
class DXD3DDevice: public DXWrapper
{
private:
  // Estructuras
  struct TextureInfo
  {// Estructura con los formatos de textura soportados
    BYTE          ucNumBitsAlpha;  // Numero de bits en el canal alpha
    DDPIXELFORMAT PixelFormat;     // Estructura con la info de la textura
  };

protected:
  // Variables de miembro
  LPDIRECT3DDEVICE7        m_lpD3DDevice;    // Objeto Direct3DDevice
  D3DVIEWPORT7             m_Viewport;       // Objeto Viewport
  D3DDEVICEDESC7           m_DeviceDesc;     // Descripcion del device
  DXD3DDefs::DXD3DDrawMode m_DrawMode;       // Contiene el modo de dibujado
  std::vector<TextureInfo> m_TextureFormats; // Vector con el formato de las texturas
  bool                     m_bSetViewport;   // ¿Se ha establecido el viewport?
  bool                     m_bAlphaEnable;   // ¿Esta el Alpha Blending activo?
  
public:
  // Constructor y destructor
  DXD3DDevice(void);
  ~DXD3DDevice(void);

public:
  // Operaciones sobre el device
  bool Create(const DXD3D* const pDXD3D, 
			  const DXDDSurfaceBack* const pBackBuffer,
              const DXD3DDefs::DXD3DDeviceType& Device = DXD3DDefs::D3DDEVICE_HARDWARE);
  void Destroy(void) { Clean(); Init(); }
  DXD3DDefs::DXD3DDeviceType GetDeviceType(void);
  bool BeginScene(void);
  bool EndScene(void);
  bool Clear(const dword dwColor = 0);
  inline const LPDIRECT3DDEVICE7 GetDevice(void) const { return m_lpD3DDevice; }  

private:
  // Operaciones relacionadas con la enumeracion de texturas
  bool FindTextureFormats(void);
  static HRESULT CALLBACK CallBackEnumTextures(LPDDPIXELFORMAT pPixelFormat, 
                                               LPVOID pExtraData);
public:
  DDPIXELFORMAT* GetTexturePixelFormat(const byte ucBpp, 
                                       const byte ucAlphaBits);
public:
  // Operaciones sobre el RenderState
  bool RSAlphaBlending(const bool bSet = true);
  bool RSColorBlending(const bool bSet = true);
  bool RSColorAlphaBlending(const bool bSet = true);
  bool RSColorKey(const bool bSet = true);
  bool RSSpecular(const bool bSet = true);

public:
  // Operaciones sobre las TextureStages
  bool SetTexture(const DXDDSurfaceTexture* const pTexture,
                  const DXD3DDefs::DXD3DStages& TextureStage = DXD3DDefs::D3DSTAGE0);
  bool UnsetTexture(const DXD3DDefs::DXD3DStages& TextureStage = DXD3DDefs::D3DSTAGE0);
  bool TSAlphaFromTexture(const DXD3DDefs::DXD3DStages& TextureStage = DXD3DDefs::D3DSTAGE0);
  bool TSAlphaFromVertex(const DXD3DDefs::DXD3DStages& TextureStage = DXD3DDefs::D3DSTAGE0);
  bool TSAlphaFromTextureAndVertex(const DXD3DDefs::DXD3DStages& TextureStage = DXD3DDefs::D3DSTAGE0);
  bool TSColorFromTexture(const DXD3DDefs::DXD3DStages& TextureStage = DXD3DDefs::D3DSTAGE0);
  bool TSColorFromVertex(const DXD3DDefs::DXD3DStages& TextureStage = DXD3DDefs::D3DSTAGE0);
  bool TSColorFromTextureAndVertex(const DXD3DDefs::DXD3DStages& TextureStage = DXD3DDefs::D3DSTAGE0);
  bool TSMagBilinearFiltering(const DXD3DDefs::DXD3DStages& TextureStage = DXD3DDefs::D3DSTAGE0);
  bool TSMinBilinearFiltering(const DXD3DDefs::DXD3DStages& TextureStage = DXD3DDefs::D3DSTAGE0);
  bool TSQuitBilinearEffect(const DXD3DDefs::DXD3DStages& TextureStage = DXD3DDefs::D3DSTAGE0);
  
public:
  // Operaciones de dibujado
  bool SetDrawMode(const DXD3DDefs::DXD3DDrawMode& DrawMode = DXD3DDefs::D3DDRAW_SOLID);
  bool Draw(DXD3DQuad* const pQuad);

public:
  // Operaciones sobre el viewport
  bool SetViewport(const dword dwX, 
				   const dword dwY, 
				   const dword dwWidth, 
                   const dword dwHeight);  
  inline D3DVIEWPORT7 GetViewport(void) const { return m_Viewport; }

public:
  // Operaciones de consulta
  bool IsHardwareDevice(void) const; 
  bool IsPow2TextureDevice(void) const;
  bool IsSquareTextureDevice(void) const;
  inline bool IsAlphaBlendingEnable(void) const { return m_bAlphaEnable; }
  
protected:
  // Metodos de apoyo
  void Init(void);
  void Clean(void);
}; 
#endif