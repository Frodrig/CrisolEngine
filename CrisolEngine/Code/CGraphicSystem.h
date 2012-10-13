///////////////////////////////////////////////////////////////////////////////
// CrisolEngine - Computer Role-Play Game Engine
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

///////////////////////////////////////////////////////////////////////////////
// CGraphicSystem.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGraphicSystem
//
// Descripcion:
// - Este sera el modulo encargado de gestionar el subsistema grafico. Aqui se
//   realizaran las peticiones de dibujado y se mantendran todas las instancias
//   a los wrappers graficos de DX.
//
// Notas:
// - La clase definida como un singlenton.
// - El dibujado en pantalla se realizara en dos pasos. El primero consistira
//   en realizar una llamada a Draw por parte de cada uno de los objetos que
//   se deban de dibujar. Al realizar una llamada a este metodo, los objetos
//   se ordenaran segun su lugar y plano de dibujado
// - Las zonas de dibujados tendran un numero de planos preasignados. Indicar
//   un plano no valido (esto es, un plano superior al plano mas grande 
//   asociado a dicha zona), supondra asignar el dibujado al plano mas grande.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGRAPHICSYSTEM_H_
#define _CGRAPHICSYSTEM_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _DXDRAW_H_
#include "DXWrapper\\DXDraw.h" 
#define _DXDRAW_H_
#endif
#ifndef _DXDDSURFACESCREEN_H_
#include "DXWrapper\\DXDDSurfaceScreen.h" 
#define _DXDDSURFACESCREEN_H_
#endif
#ifndef _DXDDSURFACEBACK_H_
#include "DXWrapper\\DXDDSurfaceBack.h"   
#define _DXDDSURFACEBACK_H_
#endif
#ifndef _DXDDSURFACETEXTURE_H_
#include "DXWrapper\\DXDDSurfaceTexture.h"
#define _DXDDSURFACETEXTURE_H_
#endif
#ifndef _DXD3D_H_
#include "DXWrapper\\DXD3D.h"             
#define _DXD3D_H_
#endif
#ifndef _DXD3DDEVICE_H_
#include "DXWrapper\\DXD3DDevice.h"       
#define _DXD3DDEVICE_H_
#endif
#ifndef _DXD3DQUAD_H_
#include "DXWrapper\\DXD3DQuad.h"         
#define _DXD3DQUAD_H_
#endif
#ifndef _ICGRAPHICSYSTEM_H_          
#include "iCGraphicSystem.h"   
#endif
#ifndef _CRECYCLENODEPOOL_CPP_
#include "CRecycleNodePool.cpp"
#endif
#ifndef _CMEMORYPOOL_H_
#include "CMemoryPool.h"
#endif
#ifndef _LIST_H_
#define _LIST_H_              
#include <list> 
#endif               
#ifndef _VECTOR_H_
#define _VECTOR_H_
#include <vector>
#endif
#ifndef _RESDEFS_H_
#include "ResDefs.h"
#endif

// Defincion de clases / estructuras / espacios de nombres
struct iCResourceManager;
struct iCDrawable;

// Clase CGraphicSystem
class CGraphicSystem: public iCGraphicSystem
{
private:
  // Estructuras
  struct sNDrawObj {
	// Nodo con la peticion de dibujado de un elemento  
	// Datos
	sword             swXPos;      // Posicion en x
	sword             swYPos;      // Posicion en y  
	iCDrawable*       pDrawable;   // Objeto a dibujar
	bool              bDarkFactor; // ¿Uso del factor oscuridad del ambiente?
	GraphDefs::sLight Light;       // Luz asociada
	// Constructor
	sNDrawObj(void): swXPos(0), swYPos(0), pDrawable(NULL), bDarkFactor(false) { }
	// Manejador de memoria
	static CMemoryPool m_MPool;
	static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
	static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
  };

private:
  // Tipos
  typedef std::vector<sNDrawObj*> DrawList; // Lista con peticiones de dibujado
  typedef DrawList::iterator    ItDrawList; // Iterador a lista de peticiones de dibujado

private:
  // Estructuras
  struct sRenderSystem {
	// Info asociada al subsistema de dibujado  
	DrawList               DrawPlanes[GraphDefs::DZ_MAX_POS]; // Array de zonas / planos	  
	dword                  udFlipFlags;                       // Flags de flip
	DXD3DQuad		  	   Quad;                              // Quad de dibujado 
	GraphDefs::Light       DarkFactor;                        // Factor de oscuridad
	ResDefs::TextureHandle hLastTexture;                      // Ultima textura  
	bool                   bVertexAlphaBlending;              // ¿Activado el vertex alpha blending?
	bool                   bTextureAlphaBlending;             // ¿Activado el texture alpha blending?
	bool                   bBilinearFiltering;                // ¿Activado el efecto bilinear?
	bool                   bColorKey;	                      // ¿Activado el color key?    	
	// Valores de apoyo al render
	GraphDefs::sTextureRegion* pTextureRgn; // Direccion a la textura de region
	// Pool de nodos con peticiones de dibujado
	CRecycleNodePool<sNDrawObj> Pool;
  
	// Constructor
	sRenderSystem(void): pTextureRgn(NULL) { }
  };  

private:
  // Instancia singlenton
  static CGraphicSystem* m_pGraphicSystem; // Unica instancia a la clase
  
  // Instancias al wrapper de DX
  DXDraw            m_DXDraw;        // Wrapper DX para el objeto DirectDraw
  DXDDSurfaceScreen m_DXFrontBuffer; // Wrapper DX para el FrontBuffer
  DXDDSurfaceBack   m_DXBackBuffer;  // Wrapper DX para el BackBuffer
  DXD3D             m_DXD3D;         // Wrapper DX para el objeto Direct3D
  DXD3DDevice       m_DXD3DDevice;   // Wrapper DX para el device D3D

  // Interfaces a otros sistemas
  iCResourceManager* m_pResManager; // Interfaz al servidor de recursos
  
  // Subsistemas
  sRenderSystem m_RenderSys; // Datos asociados al sistema de render  

  // Resto de vbles
  bool m_bInitOk; // ¿Instancia inicializada correctamente?   
  
protected:
  // Constructor / Destructor
  CGraphicSystem(void): m_bInitOk(false) { }
public:
  ~CGraphicSystem(void) { 
    End();
  }               
  
public:
  // Protocolo de inicio y fin de instancia
  bool Init(const byte ubGraphicDevice,
		    const byte ub3DIDDeviceType,
			const dword udVideoWidth,
			const dword udVideoHeight,
			const dword udVideoBpp,
			const byte ubNumBuffers,
			const bool bVSync);
  void End(void);
  inline bool IsInitOk(void) const { return m_bInitOk; }
private:
  // Metodos de apoyo
  bool InitDirectX(const byte ubGraphicDevice,
				   const byte ub3DIDDeviceType,
				   const dword udVideoWidth,
				   const dword udVideoHeight,
				   const dword udVideoBpp,
				   const byte ubNumBuffers);
  bool InitRenderSystem(const bool bVSync);
  void EndDirectX(void);
  
public:
  // Obtencion y destruccion de la instancia singlenton
  static inline CGraphicSystem* const GetInstance(void) {
    if (NULL == m_pGraphicSystem) { 
      // Se crea la instancia
      m_pGraphicSystem = new CGraphicSystem; 
      ASSERT(m_pGraphicSystem)
    }
    return m_pGraphicSystem;
  } 

  static inline DestroyInstance(void) {
    if (m_pGraphicSystem) {
      delete m_pGraphicSystem;
      m_pGraphicSystem = NULL;
    }
  }
  
public:
  // Operaciones sobre el subsistema grafico
  void Render(void);
private:
  // Metodos de apoyo al render
  void SetTexture(const ResDefs::TextureHandle& uwIDTexture);
  void SetAlphaBlending(const bool bTexture = true, const bool bVertex = true);
  void SetBilinearFiltering(const bool bSet = true);
  void SetColorKey(const bool bSet = true);  
  void DrawQuad(sNDrawObj* const pDrawObj);
  
public:
  // iCGraphicSystem / Dibujado
  void SetDarkFactor(const byte DarkFactor) {
	ASSERT(IsInitOk());
	// Establece el factor de oscuridad
	m_RenderSys.DarkFactor = DarkFactor;
  }
  void Draw(const GraphDefs::eDrawZone& Zone, 
			const byte ubPlane,
			const sword& swXPos,
			const sword& swYPos,
			iCDrawable* const pDrawObj,			
			const GraphDefs::sLight& Light);  
  void Draw(const GraphDefs::eDrawZone& Zone, 
			const byte ubPlane,
			const sword& swXPos,
			const sword& swYPos,
			iCDrawable* const pDrawObj);  

public:
  // iCGraphicSystem / Trabajo con DX
  DXDraw* const GetDXDraw(void) {
	ASSERT(IsInitOk());
	// Retorna instancia a la clase DXDraw
	return &m_DXDraw;
  }
  bool IsHardwareDevice(void) const {
	ASSERT(IsInitOk());
	// Retorna flag de consulta sobre si el dispositivo es de hardware
	return m_DXD3DDevice.IsHardwareDevice();
  }
  bool IsPow2Texture(void) const {
	ASSERT(IsInitOk());
	// Retorna flag de consulta sobre si las texturas han de ser potencia de 2
	return m_DXD3DDevice.IsSquareTextureDevice();
  }
  bool IsSquareTextureDevice(void) const {
	ASSERT(IsInitOk());
	// Retorna flag de consulta sobre si las texturas han de ser rectangulares
	return m_DXD3DDevice.IsSquareTextureDevice();
  }
  DDPIXELFORMAT* const GetTexturePixelFormat(const GraphDefs::eBpp& Bpp, 
									         const GraphDefs::eABpp& AlphaBits) {
	ASSERT(IsInitOk());
	// Retorna estructura con la descripcion para la creacion de
	// texturas con las caracteristicas Bpp y AlphaBits recibidas
	return m_DXD3DDevice.GetTexturePixelFormat(Bpp, AlphaBits);
  }

public:
  // iCGraphicSystem / Obtencion de informacion
  dword GetVideoWidth(void) const {
	ASSERT(IsInitOk());
	// Retorna la anchura de video
	return m_DXFrontBuffer.GetWidth();
  }
  dword GetVideoHeight(void) const {
	ASSERT(IsInitOk());
	// Retorna la altura de video
	return m_DXFrontBuffer.GetHeight();
  }
  inline dword GetVideoBpp(void) const {
	ASSERT(IsInitOk());
	// Retorna los bits por pixel del sistema de video
	return m_DXFrontBuffer.GetBpp();
  }  

public:
  // Operaciones de restauracion
  void RestoreSurfaces(void) {
	ASSERT(IsInitOk());
	// Restaura todas las superficiese creadas
	m_DXDraw.RestoreAllSurfaces();
  }
};
#endif // ~ CGraphicSystem
