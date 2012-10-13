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
// DXD3DDevice.cpp
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXD3DDevice.h.
///////////////////////////////////////////////////////////////////////
#include "DXD3DDevice.h"

#include "DXDDSurfaceBack.h"
#include "DXDDSurfaceTexture.h"
#include "DXD3D.h"
#include "DXD3DQuad.h"
#include "DXD3DDefs.h"
#include "DXD3DError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Constructor
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
DXD3DDevice::DXD3DDevice(void)
{
  // Llama a Init para inicializar vbles de miembro
  Init();
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destructor
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
DXD3DDevice::~DXD3DDevice(void)
{
  // Llama a clean para liberar vbles de miembro
  Clean();
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de inicializar las vbles de miembro.
// Parametros:
// Devuelve:
// Notas:
// - Este metodo es llamado siempre por el constructor
////////////////////////////////////////////////////////////////////// 
void DXD3DDevice::Init(void)
{
  // Procede a inicializar las vbles de miembro
  m_lpD3DDevice = NULL;
  m_bSetViewport = false;
  memset(&m_DeviceDesc, 0, sizeof(m_DeviceDesc));
  m_DrawMode = DXD3DDefs::D3DDRAW_WIREFRAME;
  m_bAlphaEnable = false;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de liberar los recursos
// Parametros:
// Devuelve:
// Notas:
// - Este metodo es llamado siempre por el destructor
////////////////////////////////////////////////////////////////////// 
void DXD3DDevice::Clean(void)
{
  // Se procede a liberar el device
  if (NULL != m_lpD3DDevice) {
    m_lpD3DDevice->Release();
    m_lpD3DDevice = NULL;
  } 
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea el Device en donde se dibujaran los objetos 3D.
// Parametros:
// - pDXD3D: Instancia a la clase DXD3D que contiene el 
//   objeto Direct3D
// - pBackBuffer: Instancia al BackBuffer, que sera donde se dibujen
//   los objetos.
// - Device: Tipo de Device. Aqui se podra especificar uno de los
//   siguientes tipos:
//    * D3DDEVICE_TNT para tarjeta TNT
//    * D3DDEVICE_HARDWARE para dispositivo hardware por defecto
//    * D3DDEVICE_MMX para dispositivo MMX
//    * D3DDEVICE_SOFTWARE para dispositivo software
// Devuelve:
// Notas:
// - Por defecto se intentara crear un dispositivo en hardware, esto
//   es, en D3DDEVICE_HARDWARE.
// - Una vez creado el device tambien se obtienen las caracteristicas
//   del mismo para posteriores consultas.
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::Create(const DXD3D* const pDXD3D, 
                         const DXDDSurfaceBack* const pBackBuffer,
                         const DXD3DDefs::DXD3DDeviceType& Device)
{
  // SOLO si esta creada la instancia a DXD3D y el objeto Direct3D
  DXASSERT(NULL != pDXD3D);
  DXASSERT(NULL != pDXD3D->GetObj());
  
  // Variables
  GUID  IID_Device; // Variable con el device elegido
  
  // Se escoge el device adecuado
  switch (Device) {
    case DXD3DDefs::D3DDEVICE_TNT: IID_Device = IID_IDirect3DTnLHalDevice; break;    
    case DXD3DDefs::D3DDEVICE_HARDWARE: IID_Device = IID_IDirect3DHALDevice; break;      
    case DXD3DDefs::D3DDEVICE_MMX: IID_Device = IID_IDirect3DMMXDevice; break;      
    case DXD3DDefs::D3DDEVICE_SOFTWARE: IID_Device = IID_IDirect3DRGBDevice; break;      
  };

  // Se procede a crear el device    
#ifdef _DEBUG
  m_CodeResult = pDXD3D->GetObj()->CreateDevice(IID_Device, 
                                                pBackBuffer->GetObj(),   
                                                &m_lpD3DDevice);  
  if (FAILED(m_CodeResult)) {
    // Hay problemas 
    m_lpD3DDevice = NULL;
    DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }  
#else
  pDXD3D->GetObj()->CreateDevice(IID_Device, 
                                 pBackBuffer->GetObj(),   
                                 &m_lpD3DDevice);  
#endif

  // Se enumeran todas los formatos de textura soportados
  FindTextureFormats();
  
  // Se obtiene la descripcion del device
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->GetCaps(&m_DeviceDesc); 
  if (FAILED(m_CodeResult)) {
    // Hay problemas; se debe de resetear el objeto
    Clean();
    Init();
    DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
#else
  m_lpD3DDevice->GetCaps(&m_DeviceDesc); 
#endif
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve el tipo de device que esta siendo utilizado. El tipo de
//   device se devuelve en una vble DXD3DDevice.
// Parametros:
// Devuelve:
// - El tipo de device que esta siendo utilizado
// Notas:
////////////////////////////////////////////////////////////////////// 
DXD3DDefs::DXD3DDeviceType DXD3DDevice::GetDeviceType(void)
{
  // Vbles de miembro
  DXD3DDefs::DXD3DDeviceType   DeviceType; // Vble con el device

  // Se selecciona el tipo de device
  if (IID_IDirect3DHALDevice == m_DeviceDesc.deviceGUID) { 
    // Device en Hardware
    DeviceType = DXD3DDefs::D3DDEVICE_HARDWARE; 
  }
  else if (IID_IDirect3DTnLHalDevice == m_DeviceDesc.deviceGUID) { 
    // Device sobre una tarjeta TNT
    DeviceType = DXD3DDefs::D3DDEVICE_TNT;
  }
  else if (IID_IDirect3DMMXDevice == m_DeviceDesc.deviceGUID) { 
    // Device sobre MMX
    DeviceType = DXD3DDefs::D3DDEVICE_MMX;
  }
  else { 
    // Device sobre software
    DeviceType = DXD3DDefs::D3DDEVICE_SOFTWARE;
  }
  
  // Retorna el device seleccionado
  return DeviceType;  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Llama a la funcion para la enumeracion de formatos de textura
//   soportados. Todos los formatos soportados seran introducidos en
//   el vector m_TextureFormats que, a su vez, sera pasado como 
//   extructura extra a la funcion callback.
// Parametros:
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema.
// Notas:
// - Este metodo es llamado desde create y reunira la informacion 
//   necesaria para poder crear texturas de diferentes formatos.
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::FindTextureFormats(void)
{
  // SOLO si se ha creado el device
  DXASSERT(NULL != m_lpD3DDevice);

#ifdef _DEBUG
  // Llama a la funcion de enumeracion, pasando la funcion callback
  m_CodeResult = m_lpD3DDevice->EnumTextureFormats((LPD3DENUMPIXELFORMATSCALLBACK)
                                                    CallBackEnumTextures,
                                                    &m_TextureFormats); 
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
#else
  m_lpD3DDevice->EnumTextureFormats((LPD3DENUMPIXELFORMATSCALLBACK)
                                    CallBackEnumTextures,
                                    &m_TextureFormats); 
#endif

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Funcion callback que es llamada cada vez que EnumTexturesFormats,
//   encuentra un nuevo tipo de formato de textura soportado. Se
//   encarga entonces de crear una estructura TextureInfo para alojar
//   en su interior el puntero a la estructura DDPixelFormat pasado y
//   el numero de bits del canal alpha. Despues guarda esa estructura
//   en el vector m_TextureFormats.
// Parametros:
// - pPixelFormat: Direccion de la estructura con el formato de pixel
//   asociado al nuevo formato de textura encontrado.
// - pExtraData: Contiene la direccion del vector en donde ir alojando
//   los formatos encontrados
// Devuelve:
// - Siempre devolvera D3DENUMRET_OK para indicar que se quiere seguir
//   buscando mas formatos
// Notas:
// - Esta funcion es estatica y protegida.
////////////////////////////////////////////////////////////////////// 
HRESULT CALLBACK DXD3DDevice::CallBackEnumTextures(LPDDPIXELFORMAT pPixelFormat, 
                                                   LPVOID pDevices)
{
  // Se comprueba que los parametros no sean direcciones nulas
  DXASSERT(NULL != pPixelFormat);
  DXASSERT(NULL != pDevices);

  // Variables
  std::vector<TextureInfo>* TextureFormats;   // Para realizar el cast a pDevices
  TextureInfo				NewTextureFormat; // Nueva entrada para el vector
  DWORD						dwABitMask;       // Bitmask del canal alpha

  // Inicializaciones
  TextureFormats = (std::vector<TextureInfo>*) pDevices;
  NewTextureFormat.ucNumBitsAlpha = 0;
  dwABitMask = pPixelFormat->dwRGBAlphaBitMask;
  
  // Se obtiene el numero de bits del canal alpha  
  while (dwABitMask && !(dwABitMask & 0x01L)) { 
    // Se quitan todos los 0 a la decha del bitmask solo si hay alpha
    dwABitMask >>= 1; 
  }

  while (dwABitMask & 0x01L) { 
    // Se cuentan el numero de bits a 1 de la bitmask solo si hay alpha
    NewTextureFormat.ucNumBitsAlpha++;
    dwABitMask >>= 1;
  }
  
  // Se copian todos los datos referentes al formato del pixel
  memcpy(&NewTextureFormat.PixelFormat, pPixelFormat, sizeof(DDPIXELFORMAT));

  // Se inserta la entrada nueva en el vector
  TextureFormats->push_back(NewTextureFormat);
  TextureFormats = NULL;

  // Se indica que se siga enumerando
  return D3DENUMRET_OK; 
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de devolver la estructura DDPIXELFORMAT que este en
//   relacion al numero de bits por pixel y al numero de bits para
//   el canal alpha recibidos por parametro.
// Parametros:
// - ucBpp: Numero de bits por pixel requeridos para la estructura
// - ucAlphaBits: Numero de bits para el canal alpha requeridos.
// Devuelve:
// - La direccion de la estructura DDPIXELFORMAT que contiene la
//   informacion requerida para crear la textura. En caso de que
//   no exista ninguna, devolvera NULL.
// Notas:
// - Este metodo hace uso del vector m_TextureFormats, creado durante
//   la enumeracion de los formatos de texturas soportados por la
//   tarjeta. Dicha enumeracion se llevo a cabo a la hora de crear
//   el device.
////////////////////////////////////////////////////////////////////// 
DDPIXELFORMAT* DXD3DDevice::GetTexturePixelFormat(const byte ucBpp, 
                                                  const byte ucAlphaBits)
{
  // SOLO si se ha creado el device
  DXASSERT(NULL != m_lpD3DDevice);

  // Se busca la textura que cumpla los requisitos recibidos
  for (unsigned int i=0; i < m_TextureFormats.size(); i++) {
    if (ucAlphaBits == m_TextureFormats[i].ucNumBitsAlpha && 
        ucBpp == m_TextureFormats[i].PixelFormat.dwRGBBitCount) {
      // Se encontro el formato requerido
      return &m_TextureFormats[i].PixelFormat;      
    }
  }

  // La tarjeta no soporta el formato recibido
  return NULL;  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el RenderState para realizar un efecto de blending
//   usando la formula:
//   SourceColor*SourceAlpha + DestinationColor*(1-SourceAlpha)
// Parametros:
// - bSet: Si true activa el RenderState y si false lo desactiva.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema.
// Notas:
// - Esta es la operacion que configura el efecto AlphaBlending 
//   estandar.
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::RSAlphaBlending(const bool bSet)
{
  // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);
  DXASSERT(false != m_bSetViewport);

  // Procede a activar / desactivar el RenderState
  if (bSet && !m_bAlphaEnable) { 
#ifdef _DEBUG
    // Se activa
    m_CodeResult = m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 
                                                 TRUE);
    if (FAILED(m_CodeResult)) { goto error; } // Hay problemas  

    m_CodeResult = m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, 
                                                 D3DBLEND_SRCALPHA);
    if (FAILED(m_CodeResult)) { goto error; } // Hay problemas
    
    m_CodeResult = m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,
                                                 D3DBLEND_INVSRCALPHA);
    if (FAILED(m_CodeResult)) { goto error; } // Hay problemas    
#else
	m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
    m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
#endif

    // Levanta el flag
    m_bAlphaEnable = true;   
  }
  else if (m_bAlphaEnable) { 
    // Se desactiva
#ifdef _DEBUG
    m_CodeResult = m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 
                                                 FALSE);
    if (FAILED(m_CodeResult)) { goto error; } // Hay problemas  
#else
	m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
#endif
    // Baja el flag
    m_bAlphaEnable = false;   
  }

  // Todo correcto
  return true;

#ifdef _DEBUG
error:
  // Hay problemas
  DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
  return false;
#endif
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el RenderState para realizar un efecto de blending
//   usando la formula:
//   SourceColor*SourceColor + DestinationColor*(1-SourceColor)
// Parametros:
// - bSet: Si true activa el RenderState y si false lo desactiva.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::RSColorBlending(const bool bSet)
{
  // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);
  DXASSERT(false != m_bSetViewport);

  // Procede a activar / desactivar el RenderState
  if (bSet) { 
    // Se activa
#ifdef _DEBUG
    m_CodeResult = m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, 
                                                 D3DBLEND_SRCCOLOR);
    if (FAILED(m_CodeResult)) { goto error; } // Hay problemas
    
    m_CodeResult = m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,
                                                 D3DBLEND_INVSRCCOLOR);
    if (FAILED(m_CodeResult)) { goto error; } // Hay problemas
    
    m_CodeResult = m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,
                                                 TRUE);
    if (FAILED(m_CodeResult)) { goto error; } // Hay problemas  
#else
	m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCCOLOR);
    m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCCOLOR);
    m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
#endif
  }
  else { 
    // Se desactiva
#ifdef _DEBUG
    m_CodeResult = m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,
                                                 FALSE);
    if (FAILED(m_CodeResult)) { goto error; } // Hay problemas  
#else
	m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
#endif
  }

  // Todo correcto
  return true;

#ifdef _DEBUG
error:
  // Hay problemas
  DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
  return false;
#endif
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la operacion de blending usando color como fuente y el
//   valor alpha como destino.
// Parametros:
// - bSet: Si true activa el RenderState y si false lo desactiva.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::RSColorAlphaBlending(const bool bSet)
{  
  // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);
  DXASSERT(false != m_bSetViewport);

  // Procede a activar / desactivar el RenderState
  if (bSet) { 
    // Se activa
#ifdef _DEBUG
    m_CodeResult = m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,
                                                 TRUE);
    if (FAILED(m_CodeResult)) { goto error; } // Hay problemas  

    m_CodeResult = m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, 
                                                 D3DBLEND_SRCCOLOR);
    if (FAILED(m_CodeResult)) { goto error; } // Hay problemas
    
    m_CodeResult = m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,
                                                 D3DBLEND_INVSRCALPHA);
    if (FAILED(m_CodeResult)) { goto error; } // Hay problemas        
#else
	m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCCOLOR);
    m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
#endif
  }
  else { 
    // Se desactiva
#ifdef _DEBUG
    m_CodeResult = m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,
                                                 FALSE);
    if (FAILED(m_CodeResult)) { goto error; } // Hay problemas  
#else
    m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
#endif
  }

  // Todo correcto
  return true;
#ifdef _DEBUG
error:
  // Hay problemas
  DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
  return false;
#endif
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa o desactiva el ColorKey en la textura. Activarlo, signfica
//   que a la hora de realizar el render se tendra en cuenta y
//   desactivarlo supondra lo contrario.
// Parametros:
// - bSet: Si vale true se activa y si vale false se desactiva. Por
//   defecto vale true.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha exitido algun problema.
// Notas:
// - En caso de que se quiera utilizar, se debera de llamar antes de
//   de realizar el dibujado. Una vez que se vuelquen todas las
//   texturas que tenga que utilizar Color Key, es aconsejable
//   llamar al metodo con false, para desactivarlo.
// - No se poseera un registro del estado del render, por lo que
//   si se activa / desactiva y ya estaba activado / desactivado,
//   se reallizara una operacion redundante.
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::RSColorKey(const bool bSet)
{
  // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);
  DXASSERT(false != m_bSetViewport);

  // Procede a establecer el estado del render
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE,
                                               bSet);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
#else
  m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, bSet);
#endif

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Habilita / deshabilita el color specular en el render. 
// Parametros:
// - bSet: Si vale true lo activa y si vale false lo desactiva. Por
//   defecto vale true.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema.
// Notas:
// - En caso de que se quiera utilizar, se debera de llamar antes de
//   de realizar el dibujado. Una vez volcadas los quads con uso del
//   color specular, se recomienda llamar con false.
// - No se poseera un registro del estado del render, por lo que
//   si se activa / desactiva y ya estaba activado / desactivado,
//   se reallizara una operacion redundante.
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::RSSpecular(bool const& bSet)
{
  // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);
  DXASSERT(false != m_bSetViewport);

#ifdef _DEBUG
  // Procede a establecer el estado del render
  m_CodeResult = m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE,
                                               bSet);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
#else
  m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE, bSet);
#endif

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia una textura al stage recibido. 
// Parametros:
// - pTexture: Puntero a la intancia que contiene la superficie con
//   la textura. Si vale NULL querra decir que se perderan todas
//   las configuraciones asociadas al stage.
// - TextureStage: Stage en donde asociar la textura. Por defecto
//   es la stage 0.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema.
// Notas:
// - Sera habitual querer perder todas las configuraciones asociadas
//   a un stage, despues de dibujarlo (metodo Draw).
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::SetTexture(const DXDDSurfaceTexture* const pTexture,
                             const DXD3DDefs::DXD3DStages& TextureStage)
{
  // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);  
  DXASSERT(false != m_bSetViewport);
  // SOLO si la textura es valida
  DXASSERT(NULL != pTexture);

  // Se procede a asociar la textura con la stage recbidia
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->SetTexture(DXD3DDefs::D3DSTAGE0, pTexture->GetObj());  
  if (FAILED(m_CodeResult)) { 
    // Hay problemas
    DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
#else
  m_lpD3DDevice->SetTexture(DXD3DDefs::D3DSTAGE0, pTexture->GetObj());  
#endif

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina la textura que este asociada al stage TextureStage
// Parametros:
// - TextureStage: Stage de donde quitar la textura. Por defecto
//   es la stage 0.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema.
// Notas:
// - Este metodo puede no ser util si nos encontramos en el bucle de
//   dibujado y estamos asociando textura cada x tiempo.
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::UnsetTexture(const DXD3DDefs::DXD3DStages& TextureStage)
{
  // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);  
  DXASSERT(false != m_bSetViewport);

  // Se desea eliminar todas las configuraciones asociadas al stage
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->SetTexture(DXD3DDefs::D3DSTAGE0, NULL);  
  if (FAILED(m_CodeResult)) { 
    // Hay problemas
    DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
#else
  m_lpD3DDevice->SetTexture(DXD3DDefs::D3DSTAGE0, NULL);  
#endif

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece que el valor alpha a utilizar en el rastericer procedera
//   unicamente del canal alpha que exista en la textura.
// Parametros:
// - TextureStage: Stage en donde se va a realizar la configuracion.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::TSAlphaFromTexture(const DXD3DDefs::DXD3DStages& TextureStage)
{
  // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);
  DXASSERT(false != m_bSetViewport);

  // Se procede a establecer que el valor alpha sera solo de la textura
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->SetTextureStageState(TextureStage,
                                                     D3DTSS_ALPHAOP,
                                                     D3DTOP_SELECTARG1);
  if (FAILED(m_CodeResult)) { goto error; } // Hay problemas

  m_CodeResult = m_lpD3DDevice->SetTextureStageState(TextureStage,
                                                     D3DTSS_ALPHAARG1,
                                                     D3DTA_TEXTURE);
  if (FAILED(m_CodeResult)) { goto error; } // Hay problemas
#else
  m_lpD3DDevice->SetTextureStageState(TextureStage, 
									  D3DTSS_ALPHAOP,
                                      D3DTOP_SELECTARG1);
  m_lpD3DDevice->SetTextureStageState(TextureStage,
                                      D3DTSS_ALPHAARG1,
                                      D3DTA_TEXTURE);
#endif

  // Todo correcto
  return true;

#ifdef _DEBUG
error:
  // Hay problemas
  DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
  return false;
#endif
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece que el valor alpha se tomara solo de los vertices en
//   donde se encuentre la textura.
// Parametros:
// - TextureStage: Stage en donde se va a realizar la configuracion.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::TSAlphaFromVertex(const DXD3DDefs::DXD3DStages& TextureStage)
{
  // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);
  DXASSERT(false != m_bSetViewport);

  // Se procede a establecer que el valor alpha sera solo de los vertices
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->SetTextureStageState(TextureStage,
                                                     D3DTSS_ALPHAOP,
                                                     D3DTOP_SELECTARG1);
  if (FAILED(m_CodeResult)) { goto error; } // Hay problemas

  m_CodeResult = m_lpD3DDevice->SetTextureStageState(TextureStage,
                                                     D3DTSS_ALPHAARG1,
                                                     D3DTA_DIFFUSE);
  if (FAILED(m_CodeResult)) { goto error; } // Hay problemas
#else
  m_lpD3DDevice->SetTextureStageState(TextureStage,
                                      D3DTSS_ALPHAOP,
                                      D3DTOP_SELECTARG1);
  m_lpD3DDevice->SetTextureStageState(TextureStage,
                                      D3DTSS_ALPHAARG1,
                                      D3DTA_DIFFUSE);
#endif

  // Todo correcto
  return true;

#ifdef _DEBUG
error:
  // Hay problemas
  DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
  return false;
#endif
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - El valor alpha se tomara tanto de la textura como de los vertices.
//   Realmente sera un valor obtenido de multiplicar los valores
//   alpha de la textura y de los vertices.
// Parametros:
// - TextureStage: Stage en donde se va a realizar la configuracion.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::TSAlphaFromTextureAndVertex(const DXD3DDefs::DXD3DStages& TextureStage)
{
  // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);
  DXASSERT(false != m_bSetViewport);

  // Se procede a establecer como valor alpha un valor modulado
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->SetTextureStageState(TextureStage, 
                                                     D3DTSS_ALPHAOP, 
                                                     D3DTOP_MODULATE);
  if (FAILED(m_CodeResult)) { goto error; } // Hay problemas
  
  /*
  m_CodeResult = m_lpD3DDevice->SetTextureStageState(TextureStage,
                                                     D3DTSS_ALPHAARG1,
                                                     D3DTA_TEXTURE);
  if (FAILED(m_CodeResult)) { goto error; } // Hay problemas

  m_CodeResult = m_lpD3DDevice->SetTextureStageState(TextureStage,
                                                     D3DTSS_ALPHAARG2,
                                                     D3DTA_DIFFUSE);
  if (FAILED(m_CodeResult)) { goto error; } // Hay problemas
  /**/
#else
  m_lpD3DDevice->SetTextureStageState(TextureStage, 
                                      D3DTSS_ALPHAOP, 
                                      D3DTOP_MODULATE);
  /*
  m_lpD3DDevice->SetTextureStageState(TextureStage,
                                      D3DTSS_ALPHAARG1,
                                      D3DTA_TEXTURE);
  m_lpD3DDevice->SetTextureStageState(TextureStage,
                                      D3DTSS_ALPHAARG2,
                                      D3DTA_DIFFUSE);  
  /**/
#endif

  // Todo correcto
  return true;

#ifdef _DEBUG
error:
  DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
  return false;
#endif
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece que el color a enviar al rastericer vendra solo de
//   la textura.
// Parametros:
// - TextureStage: Stage en donde se va a realizar la configuracion.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::TSColorFromTexture(const DXD3DDefs::DXD3DStages& TextureStage)
{
  // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);
  DXASSERT(false != m_bSetViewport);

  // Se procede a establecer que el valor de color sera solo de la textura
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->SetTextureStageState(TextureStage,
                                                     D3DTSS_COLOROP,
                                                     D3DTOP_SELECTARG1);
  if (FAILED(m_CodeResult)) { goto error; } // Hay problemas

  m_CodeResult = m_lpD3DDevice->SetTextureStageState(TextureStage,
                                                     D3DTSS_COLORARG1,
                                                     D3DTA_TEXTURE);
  if (FAILED(m_CodeResult)) { goto error; } // Hay problemas
#else
  m_lpD3DDevice->SetTextureStageState(TextureStage,
                                      D3DTSS_COLOROP,
                                      D3DTOP_SELECTARG1);  
  m_lpD3DDevice->SetTextureStageState(TextureStage,
                                      D3DTSS_COLORARG1,
                                      D3DTA_TEXTURE);  
#endif

  // Todo correcto
  return true;

#ifdef _DEBUG
error:
  // Hay problemas
  DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
  return false;
#endif
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se establece que el color a enviar al rastericer sera solo de
//   del poligono en donde se halle la textura.
// Parametros:
// - TextureStage: Stage en donde se va a realizar la configuracion.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::TSColorFromVertex(const DXD3DDefs::DXD3DStages& TextureStage)
{
  // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);
  DXASSERT(false != m_bSetViewport);

  // Se procede a establecer que el valor de color sera solo de la textura
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->SetTextureStageState(TextureStage,
                                                     D3DTSS_COLOROP,
                                                     D3DTOP_SELECTARG1);
  if (FAILED(m_CodeResult)) { goto error; } // Hay problemas

  m_CodeResult = m_lpD3DDevice->SetTextureStageState(TextureStage,
                                                     D3DTSS_COLORARG1,
                                                     D3DTA_DIFFUSE);
  if (FAILED(m_CodeResult)) { goto error; } // Hay problemas
#else
  m_lpD3DDevice->SetTextureStageState(TextureStage,
                                      D3DTSS_COLOROP,
                                      D3DTOP_SELECTARG1);
  m_lpD3DDevice->SetTextureStageState(TextureStage,
                                      D3DTSS_COLORARG1,
                                      D3DTA_DIFFUSE);
#endif

  // Todo correcto
  return true;

#ifdef _DEBUG
error:
  // Hay problemas
  DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
  return false;
#endif
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece como color la multiplicacion del que exista en la
//   textura y el que se halle en los vertices.
// Parametros:
// - TextureStage: Stage en donde se va a realizar la configuracion.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::TSColorFromTextureAndVertex(const DXD3DDefs::DXD3DStages& TextureStage)
{
  // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);
  DXASSERT(false != m_bSetViewport);

  // El color sera la multiplicacion del de la textura y el vertice
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->SetTextureStageState(TextureStage,
                                                     D3DTSS_COLOROP,
                                                     D3DTOP_MODULATE);
  if (FAILED(m_CodeResult)) { goto error; } // Hay problemas
  /*
  m_CodeResult = m_lpD3DDevice->SetTextureStageState(TextureStage,
                                                     D3DTSS_COLORARG1,
                                                     D3DTA_TEXTURE);
  if (FAILED(m_CodeResult)) { goto error; } // Hay problemas

  m_CodeResult = m_lpD3DDevice->SetTextureStageState(TextureStage,
                                                     D3DTSS_COLORARG2,
                                                     D3DTA_DIFFUSE);
  if (FAILED(m_CodeResult)) { goto error; } // Hay problemas
  /**/
#else
  m_lpD3DDevice->SetTextureStageState(TextureStage,
                                      D3DTSS_COLOROP,
                                      D3DTOP_MODULATE);
  /*
  m_lpD3DDevice->SetTextureStageState(TextureStage,
                                      D3DTSS_COLORARG1,
                                      D3DTA_TEXTURE);
  m_lpD3DDevice->SetTextureStageState(TextureStage,
                                      D3DTSS_COLORARG2,
                                      D3DTA_DIFFUSE);  
  /**/
#endif
  // Todo correcto
  return true;

#ifdef _DEBUG
error:
  // Hay problemas
  DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
  return false;
#endif
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el Efecto Bilinear sobre la TextureStage, para cuando
//   se aumenta el tamaño.
// Parametros:
// - TextureStage: Stage en donde se va a realizar la configuracion.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::TSMagBilinearFiltering(const DXD3DDefs::DXD3DStages& TextureStage)
{
  // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);
  DXASSERT(false != m_bSetViewport);

  // Se establece el efecto bilinear al aumentar el tamaño
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->SetTextureStageState(TextureStage, 
                                                     D3DTSS_MAGFILTER, 
                                                     D3DTFG_LINEAR);
  if (FAILED(m_CodeResult)){ goto error; } // Hay problemas
#else
  m_lpD3DDevice->SetTextureStageState(TextureStage, 
                                      D3DTSS_MAGFILTER, 
                                      D3DTFG_LINEAR);
#endif

  // Todo correcto
  return true;

#ifdef _DEBUG
error:
  // Hay problemas
  DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
  return false; 
#endif
}
 
//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el Efecto Bilinear sobre la TextureStage, para cuando
//   se disminuye el tamaño.
// Parametros:
// - TextureStage: Stage en donde se va a realizar la configuracion.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::TSMinBilinearFiltering(const DXD3DDefs::DXD3DStages& TextureStage)
{
  // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);
  DXASSERT(false != m_bSetViewport);

  // Se establece el efecto bilinear al disminuir el tamaño
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->SetTextureStageState(TextureStage, 
                                                     D3DTSS_MINFILTER, 
                                                     D3DTFG_LINEAR);
  if (FAILED(m_CodeResult)) { goto error; } // Hay problemas
#else
  m_lpD3DDevice->SetTextureStageState(TextureStage, 
                                      D3DTSS_MINFILTER, 
                                      D3DTFG_LINEAR);  
#endif

  // Todo correcto
  return true;

#ifdef _DEBUG
error:
  // Hay problemas
  DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
  return false; 
#endif
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina el efecto bilinear que pudiera estar asociado. Para ello,
//   establecera el valor D3DTFG_POIN.
// Parametros:
// - TextureStage: Stage en donde se va a realizar la configuracion.
//   Por defecto sera la stage0.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::TSQuitBilinearEffect(const DXD3DDefs::DXD3DStages& TextureStage)
{
    // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);
  DXASSERT(false != m_bSetViewport);

  // Se establece el efecto bilinear en modo POINT, que es lo mismo que
  // decir que se quita el efecto bilinear.
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->SetTextureStageState(TextureStage, 
                                                     D3DTSS_MINFILTER, 
                                                     D3DTFG_POINT);
  if (FAILED(m_CodeResult)) { goto error; } // Hay problemas
#else
  m_lpD3DDevice->SetTextureStageState(TextureStage, 
                                      D3DTSS_MINFILTER, 
                                      D3DTFG_POINT);
#endif

  // Todo correcto
  return true;

#ifdef _DEBUG
error:
  // Hay problemas
  DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
  return false; 
#endif
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el inicio de una escena. Este metodo es absolutamente
//   necesario de llamar, en caso de que se quiera dibujar algo en
//   el viewport.
// Parametros:
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::BeginScene(void)
{
  // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);
  DXASSERT(false != m_bSetViewport);

  // Se procede a iniciar la scena
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->BeginScene();
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
#else
  m_lpD3DDevice->BeginScene();
#endif

  // Todo correcto
  return true;  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza una escena. En el momento en que se llama a este metodo,
//   no se podra realizar ningun dibujado sobre el viewport, hara
//   falta llamar a BeginScene.
// Parametros:
// Devuelve:
// - true: Se ha finalizado la escena correctamente
// - false: Ha existido algun tipo de problema.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::EndScene(void)
{
  // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);
  DXASSERT(false != m_bSetViewport);

  // Se procede a finalizar la scena
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->EndScene();
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
#else
  m_lpD3DDevice->EndScene();
#endif

  // Todo correcto
  return true;  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Rellena el device (realmente el BackBuffer), con un color
//   especificado.
// Parametros:
// - dwColor: Color con el que rellenar. Por defecto vale 0 (negro)
// Devuelve:
// Notas:
// - Esta operacion tiene el mismo efecto que efectuar un BltColor
//   sobre el BackBuffer.
// - Recordar el uso de 
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::Clear(const dword dwColor)
{
  // SOLO si existe device y se ha asociado el viewport
  DXASSERT(NULL != m_lpD3DDevice);
  DXASSERT(false != m_bSetViewport);

  // Procede a realizar la operacion
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->Clear(0, 
                                      NULL, 
                                      D3DCLEAR_TARGET, 
                                      dwColor,
                                      0.0f,
                                      0);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
#else
  m_lpD3DDevice->Clear(0, 
                       NULL, 
                       D3DCLEAR_TARGET, 
                       dwColor,
                       0.0f,
                       0);
#endif

  // Todo correcto
  return true;  
}
  
//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el modo en que se dibujaran los poligonos en el
//   viewport. Por defecto, se dibujaran solidos, pero se podra
//   establecer el dibujado por puntos y en wireframe.
// Parametros:
// - DrawMode: Contendra el modo de dibujado elegido. Por defecto sera
//   en modo solido. Los posibles valores seran:
//    * D3DDRAW_SOLID -> Modo solido (normal).
//    * D3DDRAW_POINT -> Modo a puntos.
//    * D3DDRAW_WIREFRAME -> Modo wireframe  
// Devuelve:
// - true: Si no ha existido ningun problema.
// - false: Si ha ocurrido algun problema.
// Notas:
// - En caso de que exista algun problema, no se modificara el valor
//   de m_DrawMode
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::SetDrawMode(const DXD3DDefs::DXD3DDrawMode& DrawMode)
{
  // SOLO si existe device
  DXASSERT(NULL != m_lpD3DDevice);
  // SOLO si se ha establecido el viewport
  DXASSERT(false != m_bSetViewport);

  // Se procede a establecer el modo de dibujo
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_FILLMODE,
                                               DrawMode);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
#else
  m_lpD3DDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, DrawMode);
#endif
  
  // Se actualizan vbles de miembro
  m_DrawMode = DrawMode;  

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado de la primitiva pasada y segun los ajustes
//   que se hayan realizado en los RenderStates y TexturesStages.
// Parametros:
// - pQuad: Puntero a una intancia DXD3DQuad con el Quad que posee los
//   verteces en donde se mapeara la textura.
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema.
// Notas:
// - Antes de realizar el dibujado, sera necesario haber llamado a
//   BeginScene y estar seguro de haber establecido los parametros
//   correctos de RenderState (metodos RS) y TextureStage (TS).
// - Una vez finalizada la operacion de dibujado, sera importante
//   llamar al metodo SetTextureStage con parametro NULL, para eliminar
//   todas las configuraciones del stage. En caso de haber activado
//   un RenderState en concreto, sera necesario tambien, llamar al
//   metodo RS apropiado con parametro false.
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::Draw(DXD3DQuad* const pQuad)
{
  // SOLO si existe device
  DXASSERT(NULL != m_lpD3DDevice);
  // SOLO si se ha establecido el viewport
  DXASSERT(false != m_bSetViewport);
  // SOLO si los parametros recibidos son correctos
  DXASSERT(NULL != pQuad);
  DXASSERT(NULL != pQuad->GetVertex());
  
  // Se dibuja en el viewport (backbuffer)
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
                                              D3DFVF_TLVERTEX,
                                              pQuad->GetVertex(),
                                              MAXVERTEXQUAD,
                                              0);
  if (FAILED(m_CodeResult))  { 
    // Hay problemas
    DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
    return false;    
  }
#else
  m_lpD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,
                               D3DFVF_TLVERTEX,
                               pQuad->GetVertex(),
                               MAXVERTEXQUAD,
                               0);
#endif
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el area del viewport. El area debera de venir vinculado
//   a coordenadas de pantalla.
// Parametros:
// - dwX: Posicion X
// - dwY: Posicion Y.
// - dwWidth: Anchura (a sumar a la posicion x)
// - dwHeight: Altura (a sumar a la posicion y)
// Devuelve:
// - true: Si todo ha ido bien.
// - false: Si ha existido algun problema
// Notas:
// - El viewport esta asociado siempre al device, por lo que una vez
//   que se hayan establecidos los atributos del viewport, se debera
//   de asociar dicho viewport con el device.
// - En caso de que ya hubiera un viewport asociado, este sera 
//   reemplazado.
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::SetViewport(const dword dwX, 
							  const dword dwY, 
                              const dword dwWidth, 
							  const dword dwHeight)
{
  // SOLO si esta creado el device
  DXASSERT(NULL != m_lpD3DDevice);

  // Se establecen los atributos recibidos del viewport
  m_Viewport.dwX = dwX;
  m_Viewport.dwY = dwY;
  m_Viewport.dwWidth = dwWidth;
  m_Viewport.dwHeight = dwHeight;
  // Las coordenadas relativas a ZBuffer no se utilizaran
  m_Viewport.dvMinZ = 0.0f;
  m_Viewport.dvMaxZ = 1.0f;

  // Se asocia el viewport con el device
#ifdef _DEBUG
  m_CodeResult = m_lpD3DDevice->SetViewport(&m_Viewport);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXD3DError::DXD3DErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
#else
  m_lpD3DDevice->SetViewport(&m_Viewport);
#endif

  // Se actualizan vbles de miembro
  m_bSetViewport = true;

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el device actual es hardware o software.
// Parametros:
// Devuelve:
// - true: El device es hardware
// - false: El device es software
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::IsHardwareDevice(void) const
{
  // Se comprueba si es alguno de los posibles devices de hardware  
  if (IID_IDirect3DHALDevice == m_DeviceDesc.deviceGUID) { return true; }
  if (IID_IDirect3DTnLHalDevice == m_DeviceDesc.deviceGUID) { return true; }
  
  // El device es software
  return false;    
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si la textura tiene que ser necesariamente potencia de 2.
// Parametros:
// Devuelve:
// - True: Si tiene que ser potencia de 2.
// - False: Si no tiene pq ser.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::IsPow2TextureDevice(void) const
{  
  if (D3DPTEXTURECAPS_POW2 & m_DeviceDesc.dpcTriCaps.dwTextureCaps) { 
    // Es potencia de 2
    return true; 
  }
  
  // No es potencia de 2
  return false;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si la textura tiene que ser necesariamente cuadrada
// Parametros:
// Devuelve:
// - true: Si tiene que serlo
// - false: Si no es asi.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXD3DDevice::IsSquareTextureDevice(void) const
{  
  if (D3DPTEXTURECAPS_SQUAREONLY & m_DeviceDesc.dpcTriCaps.dwTextureCaps) { 
    // Tiene que ser cuadrada
    return true; 
  } 
  
  // No tiene que ser cuadrada
  return false;
}