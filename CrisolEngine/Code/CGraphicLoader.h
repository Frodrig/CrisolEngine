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
// CGraphicLoader.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clase:
// - CGraphicLoader
//
// Descripcion:
// - Este modulo centralizara el proceso de carga de ficheros graficos para su
//   posterior dibujado en un recurso CTextureResource, por ejemplo. 
//   El funcionamiento general, consistira en una llamada al metodo cargador
//   asociado al formato grafico del fichero que queremos cargar. Una vez hecha
//   esa llamada, se leera el fichero y se podra utilizar el metodo Draw
//   para volcar la informacion grafica en el recurso destino adecuado.
// - Solo podra existir un cargador activo, de tal forma que para cargar y volcar
//   imagenes distintas, se perderan las informaciones leidas con cargadores
//   anteriores.
//
// Notas:
// - La clase sera definida como un singlenton.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGRAPHICLOADER_H_
#define _CGRAPHICLOADER_H_

// Cabeceras
#ifndef _DXDDSURFACETEXTURE_H_
#include "DXWrapper\\DXDDSurfaceTexture.h"
#define _DXDDSURFACETEXTURE_H_
#endif
#ifndef _ICGRAPHICLOADER_H_
#include "iCGraphicLoader.h" 
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CGraphicSystem
class CGraphicLoader: public iCGraphicLoader
{
private:
  // Cabeceras de formatos graficos
  struct TGAHeader; // Cabecera TGA.
  
private:
  // Estructuras
  struct ImageInfo
  { // Estructura con la informacion de la imagen cargada
    bool                    bImgLoaded;     // ¿Hay una imagen cargada?
	std::string             szFileName;     // Nombre del fichero
	GraphDefs::eGraphicFile FileFormat;     // Formato del fichero
    dword			  	    udHeaderLenght; // Longitud de la cabecera
    word                    uwHeight;       // Ancho de imagen
    word                    uwWidth;        // Alto de imagen
    byte                    udBpp;          // Bpp incluido canal alpha
  };
  
  // Vbles de miembro
private:
  // Instancia singlenton
  static CGraphicLoader* m_pGraphicLoader; // Unica instancia a la clase
  
  // Resto de vbles de miembro
  sbyte*    m_pImgFile;      // Buffer donde leer los ficheros
  dword     m_udImgFileSize; // Tamaño del buffer
  ImageInfo m_ActImgInfo;    // Informacion sobre la imagen actualmente cargada  
  bool      m_bIsInitOk;     // ¿Clase inicializada correctamente?
    
protected:
  // Constructor / Destructor
  CGraphicLoader(void): m_bIsInitOk(false),
                        m_pImgFile(NULL) { }
public:
  ~CGraphicLoader(void) { 
	End(); 
  }
  
public:
  // Protocolo de inicio y fin de instancia
  bool Init(const dword udBuffSize);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }
  
public:
  // Obtencion de la instancia singlenton
  static inline CGraphicLoader* const GetInstance(void) {
    if (NULL == m_pGraphicLoader) { 
      m_pGraphicLoader = new CGraphicLoader; 
      ASSERT(m_pGraphicLoader)
    }
    return m_pGraphicLoader;
  }   
  static inline void DestroyInstance(void) {
    if (m_pGraphicLoader) {
      delete m_pGraphicLoader;
      m_pGraphicLoader = NULL;
    }
  }
  
public:
  // iCGraphicLoader / Carga de graficos
  bool TGALoader(const std::string& szFileName);  

public:
  // iCGraphicLoader / Obtencion de informacion
  bool IsImageLoaded(void) const { 
	ASSERT(IsInitOk());
	// Retorna flag
	return m_ActImgInfo.bImgLoaded; 
  }
  word GetWidth(void) const { 
	ASSERT(IsInitOk());
	// Obtiene anchura
	return m_ActImgInfo.uwWidth; 
  }
  word GetHeight(void) const { 
	ASSERT(IsInitOk());
	// Obtiene altura
	return m_ActImgInfo.uwHeight; 
  }  

public:
  // iCGraphicLoader / Operaciones
  void UnLoad(void) { 
	ASSERT(IsInitOk());
	// Descarga imagen
	m_ActImgInfo.bImgLoaded = false; 
  }
  bool Draw(DXDDSurfaceTexture* const pTexture,
			const sRect* prSrcRect = NULL,
			const sRect* prDestRect = NULL);

private:
  // Metodos de apoyo
  bool CheckTGAFile(void);
  void ReadImage(const std::string& szFileName);
  bool TGADraw(DXDDSurfaceTexture* const pTexture, 
               const sRect& arSrcRect, const sRect& arDestRect);
};
#endif // ~ CGraphicLoader
