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
// CGraphicSystem.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGraphicSystem.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGraphicSystem.h"

#include "SYSEngine.h"
#include "iCResourceManager.h" 
#include "iCDrawable.h"        
#include "iCLogger.h"          

// Inicializacion de los pools de memoria
CMemoryPool 
CGraphicSystem::sNDrawObj::m_MPool(64,
                                   sizeof(CGraphicSystem::sNDrawObj),
                                   true);

// Inicializacion de la unica instancia singlenton
CGraphicSystem* CGraphicSystem::m_pGraphicSystem = NULL;

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el modo de video segun las descripciones que se hayan obtenido
//   en la configuracion del engine. Tambien inicializa el resto de subsistemas
//   asociados al modulo grafico.
// Parametros:
// - ubGraphicDevice. ID del device grafico a utilizar.
// - ub3DIDDeviceType. ID del device 3D sobre el que inicializar. Los valores
//   seran 0 Tnt, 1 Hardware, 2 MMX y 3 Software.
// - udVideoWidth, udVideoHeight, udVideoBpp. Resolucion de video.
// - ubNumBuffers. Numero de buffers. 1 Doble buffer y 2 Triple buffer.
// - bVSync. ¿Sincronizacion en pantalla?
// Devuelve:
// - En caso de que todo haya sido correctamente inicializado true. Si ha 
//   ocurrido algun problema false.
// Notas:
// - No se permitira reicinializar desde aqui. Antes habra que llamar a End.
///////////////////////////////////////////////////////////////////////////////
bool 
CGraphicSystem::Init(const byte ubGraphicDevice,
					 const byte ub3DIDDeviceType,
					 const dword udVideoWidth,
					 const dword udVideoHeight,
					 const dword udVideoBpp,
					 const byte ubNumBuffers,
					 const bool bVSync)
{
  // ¿Ya esta inicializada la instancia?
  if (IsInitOk()) { 
	return false; 
  }

  #ifdef ENGINE_TRACE    
	SYSEngine::GetLogger()->Write("CGraphicSystem::Init> Instalando subsistema gráfico.\n");    
  #endif
  
  // Se recogen los interfaces a otros subistemas  
  m_pResManager = SYSEngine::GetResourceManager();
  ASSERT(m_pResManager);
  
  // Se levanta flag para permitir llamadas a End en caso de error
  m_bInitOk = true;

  // Se inicializan DirectDraw y Direct3D
  if (InitDirectX(ubGraphicDevice, 
				  ub3DIDDeviceType, 
				  udVideoWidth, 
				  udVideoHeight,
				  udVideoBpp,
				  ubNumBuffers)) { 
	// Se inicializan subsistema de render
	if (InitRenderSystem(bVSync)) {
	  // Todo correcto
	  #ifdef ENGINE_TRACE    
		SYSEngine::GetLogger()->Write("                    | Ok.\n");    
	  #endif
	  return true;
	} else {
	  #ifdef ENGINE_TRACE    
		SYSEngine::GetLogger()->Write("                    | Error en subsistema de render.\n");    
	  #endif
	}
  } else {
	#ifdef ENGINE_TRACE    
	  SYSEngine::GetLogger()->Write("                    | Error en subsistema DX.\n");    
	#endif
  }

  // Han existido problemas  
  End();
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea y configura todos los objetos DirectX para la pantalla. Los datos
//   de configuracion seran leidos de SYSEngine.
// Parametros:
// - ubGraphicDevice. ID del device grafico a utilizar.
// - ub3DIDDeviceType. ID del device 3D sobre el que inicializar. Los valores
//   seran 0 Tnt, 1 Hardware, 2 MMX y 3 Software.
// - udVideoWidth, udVideoHeight, udVideoBpp. Resolucion de video.
// - ubNumBuffers. Numero de buffers. 1 Doble buffer y 2 Triple buffer.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CGraphicSystem::InitDirectX(const byte ubGraphicDevice,
						    const byte ub3DIDDeviceType,
						    const dword udVideoWidth,
						    const dword udVideoHeight,
						    const dword udVideoBpp,
						    const byte ubNumBuffers) 
{  
  // Se buscan los dispositivos de video disponibles
  if (!m_DXDraw.FindDDrawDevices()) { 
    // Hay problemas
	#ifdef ENGINE_TRACE  
	  SYSEngine::GetLogger()->Write("CGraphicSystem::Init> Problemas en DXDraw al enumerar dispositivos.\n");
	  SYSEngine::GetLogger()->Write("                    | Código del error: %s.\n",m_DXDraw.GetLastStrCode().c_str());	
	#endif
    return false;
  }

  // Creacion del dispostivo DirectDraw    
  if (!m_DXDraw.Create(ubGraphicDevice)) { 	
    // Hay problemas
	#ifdef ENGINE_TRACE  
	  SYSEngine::GetLogger()->Write("CGraphicSystem::Init> Problemas en m_DXDraw al crear el objeto DirectDraw.\n");
  	  SYSEngine::GetLogger()->Write("                    | Código del error: %s.\n",m_DXDraw.GetLastStrCode().c_str());
	#endif
    return false;
  }
  
  // Ahora se establece  
  if (!m_DXDraw.SetCooperativeMode(SYSEngine::GethWnd(), 
								   DXDDDefs::FULLSCREEN)) { 
    // Hay problemas
	#ifdef ENGINE_TRACE  
		SYSEngine::GetLogger()->Write("CGraphicSystem::Init> Problemas en m_DXDraw al establecer el modo cooperativo.\n");
		SYSEngine::GetLogger()->Write("                    | Código del error: %s.\n",m_DXDraw.GetLastStrCode().c_str());	
	#endif
    return false;
  }

  // Se configura AltoxAnchoxBpp del modo de video si se trabaja a ventana completa
  if (!m_DXDraw.SetDisplayMode(udVideoWidth,
                               udVideoHeight,
                               udVideoBpp)) { 
	// Hay problemas	
	#ifdef ENGINE_TRACE  
		SYSEngine::GetLogger()->Write("CGraphicSystem::Init> Problemas en m_DXDraw al establecer AnchoxAltoxBpp.\n");
		SYSEngine::GetLogger()->Write("                    | Código del error: %s.\n",m_DXDraw.GetLastStrCode().c_str());	  
	#endif
    return false;
  }  
  
  if (!m_DXFrontBuffer.CreateComplex(&m_DXDraw, 
			    				     ubNumBuffers,
									 &m_DXBackBuffer, 
                                     DXDDDefs::VIDEOMEM, 
									 true)) { 
	// Se intenta en memoria del sistema (mas lenta)
    if (!m_DXFrontBuffer.CreateComplex(&m_DXDraw, 
									   ubNumBuffers,
									   &m_DXBackBuffer,
									   DXDDDefs::SYSTEMMEM, 
									   true)) { 
      // Hay problemas
	  #ifdef ENGINE_TRACE  
			SYSEngine::GetLogger()->Write("CGraphicSystem::Init> Problemas en m_pDXFrontBuffer al crear superficie compleja.\n");
			SYSEngine::GetLogger()->Write("                    | Código del error: %s.\n",m_DXFrontBuffer.GetLastStrCode().c_str());
	  #endif
      return false;
    }
  }
  
  // Creacion del objeto Direct3D
  if (!m_DXD3D.Create(&m_DXDraw)) { 
    // Hay problemas
	#ifdef ENGINE_TRACE  
		SYSEngine::GetLogger()->Write("CGraphicSystem::Init> Problemas en m_DXD3D al crear objeto Direct3D.\n");
		SYSEngine::GetLogger()->Write("                    | Código del error: %s.\n", m_DXD3D.GetLastStrCode().c_str());  
	#endif
    return false;
  }
  
  // Creacion del device para D3D.  
  if (!m_DXD3DDevice.Create(&m_DXD3D, 
							&m_DXBackBuffer, 
							DXD3DDefs::DXD3DDeviceType(ub3DIDDeviceType))) { 
	// Hay problemas
	#ifdef ENGINE_TRACE  
	  SYSEngine::GetLogger()->Write("CGraphicSystem::Init> Problemas en m_pDXD3DDevice al crear dispositivo.\n");
	  SYSEngine::GetLogger()->Write("                    | Código del error: %s.\n", m_DXD3DDevice.GetLastStrCode().c_str());  
	#endif
    return false;
  }   

  // Se establece el viewport de trabajo para D3D
  if (!m_DXD3DDevice.SetViewport(0, 
								 0, 
								 udVideoWidth, 
                                 udVideoHeight)) { 
	// Hay problemas
	#ifdef ENGINE_TRACE  
		SYSEngine::GetLogger()->Write("CGraphicSystem::Init> Problemas en m_pDXD3DDevice al crear el Viewport.\n");
		SYSEngine::GetLogger()->Write("                    | Código del error: %s.\n", m_DXD3DDevice.GetLastStrCode().c_str());	
	#endif
	return false;
  }    

  // Todo correcto
  return true;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el subsistema de render, creando en memoria m_pRenderSys e
//   inicializando sus datos asociados.
// Parametros:
// - bVSync. ¿Sincronizacion en pantalla?
// Devuelve:
// - Si todo bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGraphicSystem::InitRenderSystem(const bool bVSync)
{  
  // Se inicializa Pool de reciclado como no automatico
  m_RenderSys.Pool.Init();

  // Se establece tipo de flip
  m_RenderSys.udFlipFlags = bVSync ? DDFLIP_WAIT : DDFLIP_WAIT | DDFLIP_NOVSYNC;

  // Se inicializan flags de render
  m_RenderSys.bVertexAlphaBlending = false;
  m_RenderSys.bTextureAlphaBlending = false;
  m_RenderSys.bBilinearFiltering = false;
  m_RenderSys.bColorKey = false;		

  // Se inicializan resto de datos
  m_RenderSys.hLastTexture = 0;
  m_RenderSys.DarkFactor = 0; 
  m_RenderSys.pTextureRgn = NULL;

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia liberando todos los recursos que pudieran existir
//   en memoria.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGraphicSystem::End(void)
{
  if (IsInitOk()) {
    // Se finalizan subsistemas
	EndDirectX();

	// Libera pool de memoria
	m_RenderSys.Pool.End();

	#ifdef ENGINE_TRACE    
	  SYSEngine::GetLogger()->Write("CGraphicSystem::End> Subsistema gráfico desinstalado.\n");  
	#endif
    
	// Baja el flag de inicializacion
    m_bInitOk = false; 
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destruye todas las instancias DX de forma correcta.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGraphicSystem::EndDirectX(void) 
{
  // Se procede a destruir las instancias DX. El orden es importante.
  m_DXD3DDevice.Destroy();
  m_DXD3D.Destroy();
  m_DXBackBuffer.Destroy();
  m_DXFrontBuffer.Destroy();
  m_DXDraw.Destroy();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de recorrer las zonas de dibujado, obtener las peticiones,
//   configurar el device y realizar finalmente el dibujado.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGraphicSystem::Render(void)
{   
  // SOLO si la instancia esta correctamente inicializada
  ASSERT(IsInitOk());
  
  // Se comienza la escena de dibujado y se borra el viewport  
  m_DXD3DDevice.Clear();
  m_DXD3DDevice.BeginScene();

  // Se recorren los planos hasta llegar al texto
  byte ubIt = GraphDefs::DZ_BACKGROUND;
  for (; ubIt < GraphDefs::DZ_MAX_POS; ++ubIt) {		
	// Listas de dibujado asociadas a los planos BACKGROUND...CURSOR	
	ItDrawList It(m_RenderSys.DrawPlanes[ubIt].begin());
	while (It != m_RenderSys.DrawPlanes[ubIt].end()) {	  
	  // Se dibuja peticion
	  DrawQuad(*It);	  

	  // Se elimina peticion de lista <NO SE ELIMINA EL NODO / es un RecyclePool>
	  It = m_RenderSys.DrawPlanes[ubIt].erase(It);
	}	
  }
    
  // Se finaliza la escena de dibujado
  m_DXD3DDevice.EndScene();

  // Se efectua el flip
  m_DXFrontBuffer.Flip(m_RenderSys.udFlipFlags);  
  
  // Se desconfigura render para el siguiente dibujado
  SetTexture(0);
  
  // Se resetea el pool de nodos de peticion de dibujado
  m_RenderSys.Pool.ResetPool();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece una textura nueva en el device. En caso de recibir NULL, se
//   entendera que solo se desea eliminar posibles texturas del device.
// - En caso de que se desee establecer una misma textura, este metodo
//   protegera de realizar ninguna operacion.
// Parametros:
// - hTexture. Textura a asociar al Device. Si vale 0, se entendra que
//   solo se deseara quitar la textura que hubiera asociada.
// Devuelve:
// Notas:
// - En caso de que se desee quitar la textura actual del device, los valores
//   de configuracion del RenderSystem relativos al anterior parametro asi
//   como los flags de configuracion de device, se pondran todos a 0 / false.
///////////////////////////////////////////////////////////////////////////////
void 
CGraphicSystem::SetTexture(const ResDefs::TextureHandle& hTexture)
{
  // Se procede a establecer la textura indicada
  if (!hTexture && m_RenderSys.hLastTexture) {
	// Se elimina cualquier textura
	m_DXD3DDevice.UnsetTexture();				  
	m_RenderSys.bVertexAlphaBlending = false;
	m_RenderSys.bTextureAlphaBlending = false;
	m_RenderSys.bBilinearFiltering = false;
	m_RenderSys.bColorKey = false;		
	m_RenderSys.hLastTexture = 0;	
  } else if (hTexture != m_RenderSys.hLastTexture) {			
	// Se establece una nueva textura, eliminando la anterior si procede	
	m_DXD3DDevice.UnsetTexture(); 	
	m_DXD3DDevice.SetTexture(m_pResManager->GetTexture(hTexture));
	m_RenderSys.bVertexAlphaBlending = false;
	m_RenderSys.bTextureAlphaBlending = false;	
	m_RenderSys.bBilinearFiltering = false;
	m_RenderSys.bColorKey = false;		
	m_RenderSys.hLastTexture = hTexture;
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la configuracion de los valores de alpha blending, almacenandolos
//   en m_RenderSystem y realizando la coordinacion en DirectX.
// - El metodo protegera de operaciones redundates en la configuracion.
// Parametros:
// - bTexture. Alpha blending para las texturas.
// - bVertex. Alpha blending para los vertices.
// Devuelve:
// Notas:
// - Por defecto, los valores alpha valdran true.
///////////////////////////////////////////////////////////////////////////////
void 
CGraphicSystem::SetAlphaBlending(const bool bTexture, const bool bVertex)
{ 
  // Configuracion de los valores AlphaBlending      
  if (bTexture && bVertex) {
	// Si la configuracion actual ya existe, se abandona	
	if (m_RenderSys.bTextureAlphaBlending &&
  		m_RenderSys.bVertexAlphaBlending) { 
	  return; 
	}  
	
    // Alpha Blending desde textura y vertices	
    if (!m_DXD3DDevice.IsAlphaBlendingEnable()) {
      m_DXD3DDevice.RSAlphaBlending(true);
	}	
	m_DXD3DDevice.TSAlphaFromTextureAndVertex();	        	
  } else if (bTexture) {
	// Si la configuracion actual ya existe, se abandona	
	if (m_RenderSys.bTextureAlphaBlending) { return; }      	
    
	// Alpha blending desde la textura SOLO
    if (!m_DXD3DDevice.IsAlphaBlendingEnable()) {
      m_DXD3DDevice.RSAlphaBlending(true);      
    }
    m_DXD3DDevice.TSAlphaFromTexture();
  } else {
	// Si la configuracion actual ya existe, se abandona	
	if (!m_RenderSys.bTextureAlphaBlending &&
  		!m_RenderSys.bVertexAlphaBlending) { return; }  	
    // Se sobreentiende que no debe de estar activado el AlphaBlending  
    m_DXD3DDevice.RSAlphaBlending(false);
  }
  
  // Guarda los valores
  m_RenderSys.bTextureAlphaBlending = bTexture;
  m_RenderSys.bVertexAlphaBlending = bVertex;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la configuracion del efecto bilinear en DirectX y guarda los
//   valores asociados al mismo en m_RenderSys.
// - El metodo protege de operaciones redundantes si ya existia la 
//   configuracion recibida en el device.
// Parametros:
// - bSet. Indicacion del establecimiento del efecto bilinear.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGraphicSystem::SetBilinearFiltering(const bool bSet)
{
  // Si la configuracion actual ya existe, se abandona
  if (bSet != m_RenderSys.bBilinearFiltering) { 
	// Se procede a establecer valores)
	if (!bSet) {
	  m_DXD3DDevice.TSQuitBilinearEffect();
	} else {
	  m_DXD3DDevice.TSMinBilinearFiltering();
	  m_DXD3DDevice.TSMagBilinearFiltering();
	}

	// Se guarda el valor
	m_RenderSys.bBilinearFiltering = bSet; 
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el uso del color key en el device. Se supone que desde el
//   exterior, se habra asociado el color key a la textura.
// - El metodo protege de operaciones redundantes si ya existia la 
//   configuracion recibida en el device.
// Parametros:
// - bSet. Flag indicativo de si se desea o no establecer el color key.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGraphicSystem::SetColorKey(const bool bSet)
{
  // Se comprueba la configuracion del device
  if (bSet != m_RenderSys.bColorKey) { 
	// Se establece la configuracion y se guarda
	m_DXD3DDevice.RSColorKey(bSet);
	m_RenderSys.bColorKey = bSet;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado de una peticion localizada en la estructura NDrawObj,
//   sobre el Device. Coordinando todas las acciones de configuracion 
//   pertinente. Tambien esta preparado para recibir la direccion de una
//   estructura TextureRegion que sirve para especificar la zona de la
//   textura que se desea copiar en el quad. Por defecto vale NULL indicando 
//   que se tomara toda la region.
// Parametros:
// -  pDrawObj. Direccion de la sig. peticion de dibujado
// Devuelve:
// Notas:
// - Cuando pRgn != NULL, se tomaran las dimensiones de alto y ancho recibidas
//   en la textura y se obviaran las que esten asociadas a la textura
//   embebida en el objeto Drawable.
// - Realiza toda la coordinacion en la llamada a los metodos de configuracion
//   del device.
///////////////////////////////////////////////////////////////////////////////
void 
CGraphicSystem::DrawQuad(sNDrawObj* const pDrawObj)
{ 
  // SOLO si parametros validos
  ASSERT(pDrawObj);
  ASSERT(pDrawObj->pDrawable);
  
  // Se inicializan las estructuras de luz
  GraphDefs::sLight DarkLightDif; // Oscuridad - Luz por vertice
  if (pDrawObj->bDarkFactor) {
	byte ubIt = 0;
	for (; ubIt < 4; ++ubIt) {
	  if (m_RenderSys.DarkFactor > pDrawObj->Light.VertexLight[ubIt]) {
		DarkLightDif.VertexLight[ubIt] = m_RenderSys.DarkFactor - pDrawObj->Light.VertexLight[ubIt];
	  } else {
		DarkLightDif.VertexLight[ubIt] = 0;
	  }
	}	
  }

  // Realiza la configuracion del Device
  SetTexture(pDrawObj->pDrawable->GetIDTexture());
  SetAlphaBlending(pDrawObj->pDrawable->IsTextureAlphaBlending(),
				   pDrawObj->pDrawable->IsVertexAlphaBlending());  
  SetColorKey(pDrawObj->pDrawable->IsColorKey());

  // ¿Region actual distinta que la anterior?
  if (m_RenderSys.pTextureRgn != pDrawObj->pDrawable->GetTextureRegion()) {
	// Se toma la direccion de la nueva estructura de region
	m_RenderSys.pTextureRgn = pDrawObj->pDrawable->GetTextureRegion();
	// Se establecera la region segun proceda
	if (m_RenderSys.pTextureRgn) {
	  // Es una zona de la textura 	  
	  m_RenderSys.Quad.SetTextureRegion(m_RenderSys.pTextureRgn->uV0, m_RenderSys.pTextureRgn->vV0,
										m_RenderSys.pTextureRgn->uV1, m_RenderSys.pTextureRgn->vV1,
										m_RenderSys.pTextureRgn->uV2, m_RenderSys.pTextureRgn->vV2,
										m_RenderSys.pTextureRgn->uV3, m_RenderSys.pTextureRgn->vV3);										   
										   
	} else {
	  // Es toda la textura
	  m_RenderSys.Quad.SetTextureRegion(0.0f, 0.0f,
										1.0f, 0.0f,
										0.0f, 1.0f,
										1.0f, 1.0f);
	}
  }

  // Se establece la posicion del quad
  m_RenderSys.Quad.SetXYPos(DXD3DDefs::VERTEX0, 
							pDrawObj->swXPos, 
							pDrawObj->swYPos);  
  m_RenderSys.Quad.SetXYPos(DXD3DDefs::VERTEX1, 
	                        pDrawObj->swXPos + pDrawObj->pDrawable->GetWidth(), 
							pDrawObj->swYPos);
  m_RenderSys.Quad.SetXYPos(DXD3DDefs::VERTEX2, 
	                        pDrawObj->swXPos, 
							pDrawObj->swYPos + pDrawObj->pDrawable->GetHeight());
  m_RenderSys.Quad.SetXYPos(DXD3DDefs::VERTEX3, 
							m_RenderSys.Quad.GetXPos(DXD3DDefs::VERTEX1), 
							m_RenderSys.Quad.GetYPos(DXD3DDefs::VERTEX2)); 
  

  // Se asignan los valores RGB y alpha  
  byte ubIt = 0;
  for (; ubIt < 4; ++ubIt) {	
	// Se obtienen los valores alpha y vertex
	const GraphDefs::eVertex Vertex = GraphDefs::eVertex(ubIt);
	
	// Valores base en caso de no existir estructura asociada
	GraphDefs::sRGBColor   RGBBase;   // Valor base de RGB
	GraphDefs::sAlphaValue AlphaBase; // Valor base de alpha    		
	
	// Direccion a estructura alpha
	GraphDefs::sAlphaValue* pAlphaTmp = pDrawObj->pDrawable->GetAlphaValue(Vertex);
	if (!pAlphaTmp) { 
	  pAlphaTmp = &AlphaBase; 
	}

	// Direccion a estructura RGB
	GraphDefs::sRGBColor* pRGBTmp = pDrawObj->pDrawable->GetRGBColor(Vertex);  
	if (!pRGBTmp) { 
	  pRGBTmp = &RGBBase; 
	}

	// Se obtiene el color final restando la oscuridad en el ambiente
	// y teniendo cuidado de no hacer underflow
	pRGBTmp->ubRed -= DarkLightDif.VertexLight[ubIt] > pRGBTmp->ubRed ? 0 : DarkLightDif.VertexLight[ubIt];
	pRGBTmp->ubGreen -= DarkLightDif.VertexLight[ubIt] > pRGBTmp->ubGreen ? 0 : DarkLightDif.VertexLight[ubIt];
	pRGBTmp->ubBlue -= DarkLightDif.VertexLight[ubIt] > pRGBTmp->ubBlue ? 0 : DarkLightDif.VertexLight[ubIt];

	// Se establece el color en el vertice adecuado	  
	m_RenderSys.Quad.SetVertexColor(DXD3DDefs::DXD3DVertex(ubIt),
  									pRGBTmp->ubRed,
									pRGBTmp->ubGreen,
									pRGBTmp->ubBlue,
									pAlphaTmp->ubAlpha);	
  }

  // Se dibuja
  m_DXD3DDevice.Draw(&m_RenderSys.Quad);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo de dibujado para elementos en pantalla que sean sensibles al 
//   uso del factor de oscuridad, pues tendran asociadas una estructura de luz.
//   (aunque dicha estructura de luz sea 0). Este tipo de elementos a dibujar
//   seran aquellos que aparezcan en el area de juego.
// Parametros:
// - Zone. Zona de dibujado.
// - ubPlane. Plano de dibujado. Los valores para cada zona estan indicandos 
//   en GraphDefs.
// - swXPos, swYPos. Posicion en x,y de dibujado.
// - pDrawObj. Direccion del objeto a dibujar.
// - bDarkFactor. ¿Se usa el factor de oscuridad?
// - pLight. Direccion de la estructura con la luz. Se obviara si bDarkFactor
//   vale false.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGraphicSystem::Draw(const GraphDefs::eDrawZone& Zone, 
					 const byte ubPlane,
					 const sword& swXPos,
					 const sword& swYPos,
					 iCDrawable* const pDrawObj,					 
					 const GraphDefs::sLight& Light)
{
  // SOLO si modulo inicializado
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pDrawObj);

  // Se crea nuevo nodo con la peticion
  sNDrawObj* const pNewNode = m_RenderSys.Pool.GetNewNode();
  ASSERT(pNewNode);
  
  // Establece valores
  pNewNode->swXPos = swXPos;
  pNewNode->swYPos = swYPos;
  pNewNode->pDrawable = pDrawObj;  
  pNewNode->bDarkFactor = true;
  pNewNode->Light = Light;  

  // Se inserta peticion
  m_RenderSys.DrawPlanes[Zone + ubPlane].push_back(pNewNode);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo de dibujado que contemplara a entidades para las que no se usara
//   el factor de oscuridad y que, por lo tanto, no tendran asociada estructura
//   de luz (elementos de interfaz).
// Parametros:
// - Zone. Zona de dibujado.
// - ubPlane. Plano de dibujado. Los valores para cada zona estan indicandos 
//   en GraphDefs.
// - swXPos, swYPos. Posicion en x,y de dibujado.
// - pDrawObj. Direccion del objeto a dibujar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGraphicSystem::Draw(const GraphDefs::eDrawZone& Zone, 
					 const byte ubPlane,
					 const sword& swXPos,
					 const sword& swYPos,
					 iCDrawable* const pDrawObj)
{
  // SOLO si modulo inicializado
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pDrawObj);

  // Se crea nuevo nodo con la peticion
  sNDrawObj* pNewNode = m_RenderSys.Pool.GetNewNode();
  ASSERT(pNewNode);
  
  // Establece valores
  pNewNode->swXPos = swXPos;
  pNewNode->swYPos = swYPos;
  pNewNode->pDrawable = pDrawObj;  
  pNewNode->bDarkFactor = false;
  
  // Se inserta peticion
  m_RenderSys.DrawPlanes[Zone + ubPlane].push_back(pNewNode);
}
