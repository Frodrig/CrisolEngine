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
// CResourceManager.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clase:
// - CResourceManager
//
// Descripcion:
// - Servidor de recursos. Aqui se registrar las texturas, sonidos WAV, 
//   plantillas de animacion y se tendran acceso a los textos asociados a
//   identificadores. El servidor se encargara de devolver identificadores
//   al exterior, de tal forma que dos o mas instancias que necesiten un
//   mismo recurso, las puedan repartir.
// - Para mantener los recursos se utilizaran arboles. 
// - Las texturas usaran el manager de DirectX.
// - Las plantillas de animacion se registran utilizando un ID con el que
//   el servidor de recursos acudira a un archivo. En dicho archivo sabra como
//   configurar la plantilla a partir de dicho ID.
// - El texto se localizara por ID. El servidor dispondra de un buffer igual
//   al texto mas grande. Desde el exterior se debera de ordenar al servidor de
//   recursos leer y despues se tendra que obtener la direccion de dicho buffer
//   que sera una direccion imposible de modificar.
//
// Notas:
// - La clase sera definida como un singlenton.
// - En la version actual son simples arboles de busqueda con asignacion 
//   aleatoria de claves, expecto para los recursos de animacion, estaria
//   pendiente la implementacion de arboles AVL.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CRESOURCEMANAGER_H_
#define _CRESOURCEMANAGER_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _DXDDSURFACETEXTURE_H_
#include "DXWrapper\\DXDDSurfaceTexture.h"
#define _DXDDSURFACETEXTURE_H_
#endif
#ifndef _DXDSWAVSOUND_H_
#include "DXWrapper\\DXDSWAVSound.h"
#define _DXDSWAVSOUND_H_
#endif
#ifndef _DXDSWAVBUFFER_H_
#include "DXWrapper\\DXDSWAVBuffer.h"
#define _DXDSWAVBUFFER_H_
#endif
#ifndef _SYSENGINE_H_
#include "SYSEngine.h" 
#endif
#ifndef _ICRESOURCEMANAGER_H_
#include "iCResourceManager.h"
#endif
#ifndef _CMEMORYPOOL_H_
#include "CMemoryPool.h"
#endif
#ifndef _CANIMTEMPLATEDATA_H_
#include "CAnimTemplateData.h"
#endif
#ifdef _ICMATHUTIL_H_
#include "iCMathUtil.h"
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif

// Defincion de clases / estructuras / espacios de nombres
struct iCGraphicLoader;
struct iCMathUtil;

// Clase CResourceManager
class CResourceManager: public iCResourceManager
{  
private:
  // Tipos  
  typedef word WAVBufferHandle; // Handle a buffers de sonido
  typedef word TextureParams;   // Parametros de textura

private:
  // Predicado para la busqueda de elementos en map por valor
  // Notese que todos los elementos almacenados en los maps seran punteros
  template<class First, class Second>
  class CMapValueSearch {
  private:
	// Vbles de miembro
	Second m_Value; // Valor a localizar

  public:
	// Constructor
	CMapValueSearch(const Second& Value): m_Value(Value) { }

  public:
	// Predicado de busqueda
	bool operator() (std::pair<const First, Second*>& Node) {
	  return (*Node.second == m_Value);
	}  
  }; // ~ MapValueSearch
  
private:  
  // Estructuras
  struct sNTexture { 
	// Informacion para un recurso de textura
    std::string        szName;     // Nombre del recurso / archivo
    word               uwRefs;     // Referencias al recurso
	sRect              rDimension; // Dimensiones de la imagen
    TextureParams      Params;     // Parametros del recurso recibidos al registrarlo
    DXDDSurfaceTexture TextureRes; // Recurso
    // Pool de memoria
    static CMemoryPool m_MPool;
    static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
    static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
  };

private:
  // Tipos
  // Map para el control de las texturas
  typedef std::map<ResDefs::TextureHandle, sNTexture*> TextureInfoMap;
  typedef TextureInfoMap::iterator                     TextureInfoMapIt;
  typedef TextureInfoMap::value_type                   TextureInfoValType;

private:
  // Estructuras
  struct sNWAVBufferInstance {
	// Nodo con informacion sobre una instancia WAVBuffer
	DXDSWAVBuffer WAVBuffer; // Instancia
	word          uwRefs;    // Referencias
	// Constructor
	sNWAVBufferInstance(void): uwRefs(0) { }
	// Pool de memoria
    static CMemoryPool m_MPool;
    static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
    static void operator delete(void* pItem)  { m_MPool.FreeMem(pItem); } 
  };

private:
  // Tipos
  // Map para el control de instancias WAVBuffer <Parametros instancia, instancia>
  typedef std::map<dword, sNWAVBufferInstance*> WAVBufferInstancesMap;
  typedef WAVBufferInstancesMap::iterator       WAVBufferInstancesMapIt;
  typedef WAVBufferInstancesMap::value_type     WAVBufferInstancesMapValType;

private:
  // Estructuras
  struct sNWAVBufferInfo {
	// Nodo de instancias WAVBuffer asociadas a un fichero WAV especifico
	WAVBufferInstancesMap Instances; // Instancias
	// Pool de memoria
    static CMemoryPool m_MPool;
    static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
    static void operator delete(void* pItem)  { m_MPool.FreeMem(pItem); } 
  };

private:
  // Tipos
  // Map de las instancias WAVBuffer asociadas a un fichero WAV especifico
  typedef std::map<std::string, sNWAVBufferInfo*> WAVBufferInfoMap;
  typedef WAVBufferInfoMap::iterator              WAVBufferInfoMapIt;
  typedef WAVBufferInfoMap::value_type            WAVBufferInfoMapValType;

private:
  // Estructuras
  struct sNWAVSound {
	// Nodo de informacion asociada a un sonido WAV concreto
	DXDSWAVSound	        WAVSound;            // Sonido WAV	
	WAVBufferInstancesMapIt WAVBufferInstanceIt; // Iterador a instancia WAVBuffer
	WAVBufferInfoMapIt      WAVBufferInfoIt;     // Iterador a nodo de info WAVBuffer.
	// Pool de memoria
    static CMemoryPool m_MPool;
    static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
    static void operator delete(void* pItem)  { m_MPool.FreeMem(pItem); } 
  };

private:
  // Tipos
  // Map de instancias WAVSound asociadas a un handle
  typedef std::map<ResDefs::WAVSoundHandle, sNWAVSound*> WAVSoundMap;
  typedef WAVSoundMap::iterator                          WAVSoundMapIt;
  typedef WAVSoundMap::value_type                        WAVSoundMapValType;

private:
  // Estructuras
  struct sNAnimTemplateData {
	// Info para el mantenimiento de datos de una plantilla de animacion
	// de un determinado tipo
	CAnimTemplateData ATemplateData; // Datos de la plantilla
	dword             udRefs;        // Referencias
	// Constructor
	sNAnimTemplateData(void): udRefs(0) { }
	// Pool de memoria
    static CMemoryPool m_MPool;
    static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
    static void operator delete(void* pItem)  { m_MPool.FreeMem(pItem); } 
  };

private:
  // Tipos
  // Map para el mantenimiento de las plantillas de animacion
  typedef std::map<std::string, sNAnimTemplateData*> AnimTemplateDataMap;
  typedef AnimTemplateDataMap::iterator              AnimTemplateDataMapIt;
  typedef AnimTemplateDataMap::value_type            AnimTemplateDataMapValType;

private:
  // Instancia singlenton
  static CResourceManager* m_pResourceManager;
  
  // Instancias a otros subsistemas
  iCGraphicLoader* m_pGraphLoader; // Loader de graficos
  iCMathUtil*      m_pMathUtil;    // Utilidades matematicas

  // Bancos (maps) de recursos
  TextureInfoMap      m_TextureBank;       // Banco de texturas
  WAVBufferInfoMap    m_WAVBufferInfoBank; // Banco de buffers WAV
  WAVSoundMap         m_WAVSoundBank;      // Banco de sonidos WAV  
  AnimTemplateDataMap m_ATemplateDataBank; // Banco de CAnimTemplateDatas
     
  // Buffers de lectura
  sbyte* m_pReadBuffer;    // Buffer de lectura de datos desde archivo
  dword  m_udReadBuffSize; // Tamaño del buffer de lectura

  // Resto de vbles
  bool m_bInitOk; // ¿Clase inicializada correctamente?
    
protected:
  // Constructor / Destructor
  CResourceManager(void): m_pGraphLoader(NULL),
						  m_pMathUtil(NULL),
						  m_bInitOk(false),
                          m_pReadBuffer(NULL) { }
public:  
  ~CResourceManager(void) { 
    End();
  }               
  
public:
  // Protocolo de inicio y fin de instancia
  bool Init(void);
  void End(void);
  inline bool IsInitOk(void) const { return m_bInitOk; }
private:
  // Metodos de apoyo
  void EndTextureBank(void);
  void EndAnimTemplateDataBank(void);
  void EndWAVSoundBank(void);
  void EndWAVBufferInfoBank(void);  
  
public:
  // Obtencion y destruccion de la instancia singlenton
  static inline CResourceManager* const GetInstance(void) {
    if (NULL == m_pResourceManager) { 
      // Se crea la instancia
      m_pResourceManager = new CResourceManager; 
      ASSERT(m_pResourceManager)
    }
    return m_pResourceManager;
  } 
  static inline DestroyInstance(void) {
    if (m_pResourceManager) {
      delete m_pResourceManager;
      m_pResourceManager = NULL;
    }
  }

public:
  // Restauracion de recursos
  void RestoreResources(void);
private:
  // Metodos de apoyo
  void RestoreWAVSounds(void);

public:
  // iCResourceManager / Trabajo con texturas
  ResDefs::TextureHandle TextureRegister(const std::string& szTextureName,
										 const GraphDefs::eBpp& Bpp,
										 const GraphDefs::eABpp& AlphaBits);
  ResDefs::TextureHandle TextureRegister(const std::string& szTextureName,
										 const word uwWidth,
										 const word uwHeight,
										 const GraphDefs::eBpp& Bpp,
										 const GraphDefs::eABpp& AlphaBits);  
  ResDefs::TextureHandle TextureRegister(const std::string& szTextureName,
										 const sRect& rDimension,
										 const GraphDefs::eBpp& Bpp,
										 const GraphDefs::eABpp& AlphaBits);  
  DXDDSurfaceTexture* const GetTexture(const ResDefs::TextureHandle& hTexture);
  void ReleaseTexture(const ResDefs::TextureHandle& hTexture);  
  std::string GetTextureFileName(const ResDefs::TextureHandle& hTexture);    
private:
  // Metodos de apoyo
  ResDefs::TextureHandle CreateTexture(const std::string& szTextureName,
									   const sRect& rDimension,
									   const GraphDefs::eBpp& Bpp,
									   const GraphDefs::eABpp& AlphaBits,
									   const bool bAttachToFile);
  TextureParams MakeTextureParams(const GraphDefs::eBpp& aBpp,
								  const GraphDefs::eABpp& aAlphaBits,
								  const bool abAttachToFile);
  inline word GetTextureWidth(const sRect& rDimension) {	
	ASSERT(IsInitOk());
	// Retorna la anchura
	return ((rDimension.swRight - rDimension.swLeft) + 1);
  }
  inline word GetTextureHeight(const sRect& rDimension) {
	ASSERT(IsInitOk());
	// Retorna la altura
	return (rDimension.swBottom - rDimension.swTop) + 1;
  }
  GraphDefs::eBpp GetTextureBpp(const TextureParams& Params);
  GraphDefs::eABpp GetTextureAlphaBits(const TextureParams& Params,
                                       const GraphDefs::eBpp& Bpp);
  inline bool IsTextureAttachToFile(const TextureParams& Params) const {
	ASSERT(IsInitOk());
	// Retorna flag
	return ((Params & 0x0010) != 0);
  }

public:
  // iCResourceManager / Trabajo con sonidos WAV
  ResDefs::WAVSoundHandle WAVSoundRegister(const std::string& szWAVFileName,
										   const bool bCtrlVolume = false,
										   const bool bCtrlFrequency = false,
										   const bool bCtrlPan = false);
  DXDSWAVSound* const GetWAVSound(const ResDefs::WAVSoundHandle& hWAVSound);
  void ReleaseWAVSound(const ResDefs::WAVSoundHandle& hWAVSound);
  std::string GetWAVSoundFileName(const ResDefs::WAVSoundHandle& hWAVSound);
  bool IsWAVSoundPlaying(const ResDefs::WAVSoundHandle& hWAVSound) {
	ASSERT(IsInitOk());
	ASSERT(hWAVSound);
	// Comprueba si un sonido WAV esta reproduciendose
	return GetWAVSound(hWAVSound)->IsPlaying();
  }
private:
  // Metodos de apoyo
  bool CreateWAVBufferAndSound(const std::string& szWAVFile, 
                               DXDSWAVBuffer* const pBuffer,
                               DXDSWAVSound* const pSound,
                               const dword udAlloc);

public:
  // iCResourceManager / Metodos de trabajo con las plantillas de animacion
  iCAnimTemplateData* const RegisterAnimTemplateData(const std::string& szAnimTemplate,
													 CAnimTemplate* const pAnimTemplate);
  void ReleaseAnimTemplateData(const std::string& szAnimTemplate,
							   CAnimTemplate* const pAnimTemplate);  
private:  
  // Metodos de apoyo
  ResDefs::TextureHandle CreateTextureHandle(void);  
  ResDefs::WAVSoundHandle CreateWAVSoundHandle(void);
};

#endif // ~ CResourceManager
