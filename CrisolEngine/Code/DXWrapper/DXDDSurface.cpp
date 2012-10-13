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
// DXDDSurface.cpp
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXDDSurface.h
///////////////////////////////////////////////////////////////////////
#include "DXDDSurface.h"

#include "DXDDClipper.h"
#include "DXDDError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Constructor
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
DXDDSurface::DXDDSurface(void)
{
  // Inicializa vbles de miembro
  Init();
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa variables de miembro
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
void DXDDSurface::Init(void)
{
  // Inicializa vbles
  m_pDDSurface = NULL;
  m_SurfaceType = DXDDDefs::SURFACETYPE_NOCREATED;
  memset(&m_RGBInfo, 0, sizeof(m_RGBInfo));
  m_dwWidth = m_dwHeight = 0;
  m_bSrcColorKey = m_bDestColorKey = false;
  m_bClipper = false;
  m_bLock = false;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destructor
// Parametros:
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
DXDDSurface::~DXDDSurface(void)
{
  // Se liberan recursos
  Clean();  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera objeto DirectDrawSurface
// Parametros:
// Devuelve:
// Notas:
// - Las superficies de tipo ATTACHED no pueden invocar a Release, pues
//   incurririan en un error. Simplemente se desreferencian ya que la
//   COMPLEX asociada a ellas, las liberara al hacer Release.
////////////////////////////////////////////////////////////////////// 
void DXDDSurface::Clean(void)
{
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el Color Key de la superficie.
// Parametros:
// - dwColor: Color key
// - wTypeKey: Tipo de color key a configurar en la superficie. Los
//   valores pueden ser:
//   * SOURCECOLORKEY: Establece el color que no se va a tener en 
//     cuenta (el que no se va a copiar) en la superficie origen, 
//     cuando vayamos a realizar una operacion de blit sobre la actual.
//   * DESTINATIONCOLORKEY: Establece el color sobre el que no se va a 
//     dibujar en la superficie destino (la actual), cuando vayamos 
//     a realizar una operacion de blit.} 
// Devuelve:
// - true: Todo ha ido bien.
// - false: Ha existido algun tipo de problema.
// Notas:
// - SOURCECOLORKEY es la tecnica que mas se utiliza. Se la conoce
//   tambien como la del color invisible.
// - DESTINATIONCOLORKEY, se utiliza menos que SOURCECOLORKEY, 
//   pero puede ser util para establecer objetos alejados.
// - Se supone que dwColor posee el formato adecuado
// - Este metodo no tiene en cuenta gradientes de color para
//   establecerlos como color key, es decir, solo establece un color key
////////////////////////////////////////////////////////////////////// 
bool DXDDSurface::SetColorKey(const dword dwColor, 
                              const DXDDDefs::DXDDColorKeyType& dwTypeKey)
{
  // SOLO si hay superficie creada
  DXASSERT(m_pDDSurface != NULL);
  
  DDCOLORKEY  descColorKey; // Estructura con la informacion del colorKey
  DWORD       dwFlags;      // Tipo de color a utilizar
    
  // Inicializciones
  memset(&descColorKey, 0, sizeof(descColorKey));  
  
  // Inicializa los flags de indicacion de color key
  dwFlags = dwTypeKey;

  // Establece los colores clave
  descColorKey.dwColorSpaceHighValue = dwColor;
  descColorKey.dwColorSpaceLowValue = dwColor;

  // Procede a asociar color key de origen
  m_CodeResult = m_pDDSurface->SetColorKey(dwFlags, &descColorKey);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Se actualizan vbles de miembroActualiza flag correspondiente
  if (DXDDDefs::SOURCECOLORKEY == dwTypeKey) { 
    m_bSrcColorKey = true; 
    m_dwSrcColorKey = dwColor;
  }
  else { 
    m_bDestColorKey = true;
    m_dwDestColorKey = dwColor;
  }
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina el color key asociado a la superficie
// Parametros:
// - wTypeKey: Tipo de color key a quitar en la superficie. Los
//   valores pueden ser:
//   * SOURCECOLORKEY: Color key de tipo fuente
//   * DESTINATIONCOLORKEY: Color key de tipo destino
// Devuelve:
// - true: Todo ha ido bien.
// - false: Ha existido algun tipo de problema.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDDSurface::UnSetBltColorKey(const DXDDDefs::DXDDColorKeyType& dwTypeKey)
{
  // SOLO si hay superficie creada
  DXASSERT(m_pDDSurface != NULL);
  
  // Procede a quitar la asociacion al color key de origen
  m_CodeResult = m_pDDSurface->SetColorKey(dwTypeKey, NULL);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Actualiza flag correspondiente
  switch(dwTypeKey)
  {
    case DXDDDefs::SOURCECOLORKEY:
      m_bSrcColorKey = false;
      break;
    case DXDDDefs::DESTINATIONCOLORKEY:
      m_bDestColorKey = false;
      break;   
  };

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza una operacion de Blt simple, volcando el contenido de la
//   superficie pasada sobre la actual. 
// Parametros:
// - nPosX y nPosY: Coordenadas destino donde realizar el Blit.
// - poSrcSurf: Instancia a la superficie origen, esto es, la que posee
//   el contenido que se quiere volcar sobre la actual.
// - pSrcRect: Puntero a la estructura rect que contiene la porcion
//   de la superficie original a volcar. Por defecto vale NULL, lo que
//   quiere decir que se volcara todo.
// - nScaledX y nScaledY: Factor de escalado en las coordenadas X e Y,
//   a la hora de hacer la operacion Blt. Por defecto vale 0, por lo que
//   no se realizara escalado.
// Devuelve:
// - true: Todo ha ido bien.
// - false: Ha existido algun tipo de problema.
// Notas:
// - En el caso de no existir un Clipper asociado a la superficie,
//   se realizara de modo manual.
// - Se tendran en cuenta los color key tanto origen como destino.
////////////////////////////////////////////////////////////////////// 
bool DXDDSurface::Blt(const int& nPosX, 
					  const int& nPosY, 
					  DXDDSurface* const poSrcSurf, 
                      const int& nScaledX, 
					  const int& nScaledY, 
					  LPRECT pSrcRect)
{
  // SOLO si hay superficie creada
  DXASSERT(m_pDDSurface != NULL);
  // SOLO si existe la superficie origen pasada
  DXASSERT(poSrcSurf != NULL);

  // Vbles
  int anPosX = nPosX;
  int anPosY = nPosY;

  // Si no hay objeto Clipper asociado, el clipping se hace manualmente  
  if (!m_bClipper) {
    // Clipping horizontal
    if (anPosX < 0) { anPosX = 0; }
    else if (anPosX > (int)(m_dwWidth)) { anPosX = m_dwWidth; }

    // Clipping vertical
    if (anPosY < 0) { anPosY = 0; }
    else if (anPosY > (int)(m_dwHeight)) { anPosY = m_dwHeight;  }
  }
  
  DWORD       dwFlags;  // Banderas de volcado  
  RECT        DestRect; // Area destino donde efectuar el blit  

  // Establece flags de volcado
  dwFlags = DDBLT_WAIT;
  if (poSrcSurf->IsSrcColorKeyPresent()) { dwFlags |= DDBLT_KEYSRC; }
  if (m_bDestColorKey) { dwFlags |= DDBLT_KEYDEST; }

  // Calcula area destino
  DestRect.left = anPosX;
  DestRect.top = anPosY;
  
  if (pSrcRect == NULL) {
    // Anchura a considerar la de la superficie pasada
    DestRect.right = anPosX + poSrcSurf->GetWidth() + nScaledX;
    DestRect.bottom = anPosY + poSrcSurf->GetHeight() + nScaledY;
  }
  else {
    // A considerar la que se indica en la estructura RECT pasada
    DestRect.right = anPosX + (pSrcRect->right-pSrcRect->left) + nScaledX;
    DestRect.bottom = anPosY + (pSrcRect->bottom-pSrcRect->top) + nScaledY;
  }  
  
  // Procede a realizar la operacion de blit
  m_CodeResult = m_pDDSurface->Blt(&DestRect, 
								   poSrcSurf->GetObj(), 
								   pSrcRect,
                                   dwFlags, 
								   NULL);
  if(FAILED(m_CodeResult)) {
    // Hay problemas    
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);     
    return false;
  }
   
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Rellena la porcion queria de la superficie, de un color.
// Parametros:
// - dwColor: Color con el que rellenar la superficie
// - pDestRect: Area a rellenar en la superficie. Por defecto vale NULL,
//   lo que querra decir que se rellenara toda la superficie.
// Devuelve:
// - true: Todo ha ido bien.
// - false: Ha existido algun tipo de problema.
// Notas:
// - En el caso de no existir un Clipper asociado a la superficie,
//   se realizara de modo manual.
// - Se tendran en cuenta los color key tanto origen como destino.
// - Se supone que el valor recibido en dwColor ya posee la conversion
//   correspondiente (en caso de estar trabajando con un modo de
//   video que no posee paleta)
////////////////////////////////////////////////////////////////////// 
bool DXDDSurface::BltColor(const dword dwColor, 
						   LPRECT pDestRect)
{
  // SOLO si hay superficie creada
  DXASSERT(m_pDDSurface != NULL);

  // Prepara flags
  DWORD       dwFlags; // Bandenas de volcado
  DDBLTFX     descFX;  // Estructura de FX con el color a utilzar
  
  // Inicializaciones  
  memset(&descFX, 0, sizeof(descFX));
  descFX.dwSize = sizeof(descFX);  
  
  // Se establecen flags
  dwFlags = DDBLT_WAIT | DDBLT_COLORFILL;
  descFX.dwFillColor = dwColor;

  // Se procede a rellenar de un color la porcion deseada
  m_CodeResult = m_pDDSurface->Blt(pDestRect, NULL, NULL, dwFlags, &descFX);                                   
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
// - Realiza la operacion de Blit utilizando el efecto espejo.
// Parametros:
// - nPosX y nPosY: Coordenadas destino donde realizar el Blit.
// - dwMirrorFlag: Flag con el tipo de efecto espejo a aplicar. Los 
//   valores posibles son:
//   * HMIRROR: Efecto espejo en el eje horizontal.
//   * VMIRROR: Efecto espejo en el eje vertical.
//   * HVMIRROR: Efecto espejo en el eje horizontal y vertical.
// - poSrcSurf: Instancia a la superficie origen, esto es, la que posee
//   el contenido que se quiere volcar sobre la actual.
// - pSrcRect: Puntero a la estructura rect que contiene la porcion
//   de la superficie original a volcar. Por defecto vale NULL, lo que
//   quiere decir que se volcara todo.
// - nScaledX y nScaledY: Factor de escalado en las coordenadas X e Y,
//   a la hora de hacer la operacion Blt. Por defecto vale 0, por lo que
//   no se realizara escalado.
// Devuelve:
// - true: Todo ha ido bien.
// - false: Ha existido algun tipo de problema.
// Notas:
// - En el caso de no existir un Clipper asociado a la superficie,
//   se realizara de modo manual.
// - Se tendran en cuenta los color key tanto origen como destino.
////////////////////////////////////////////////////////////////////// 
bool DXDDSurface::BltMirror(const int& nPosX, 
						    const int& nPosY, 
						    DXDDDefs::DXDDMirrorType& dwMirrorFlag,
						    DXDDSurface* const poSrcSurf, 
						    const int& nScaledX, 
						    const int& nScaledY,
						    LPRECT pSrcRect)				
{
  // SOLO si hay superficie creada
  DXASSERT(m_pDDSurface != NULL);
  // SOLO si existe la superficie origen pasada
  DXASSERT(poSrcSurf != NULL);

  // Vbles
  int anPosX = nPosX;
  int anPosY = nPosY;

  // Si no hay objeto Clipper asociado, el clipping se hace manualmente    
  if (!m_bClipper) {
    // Clipping horizontal
    if (anPosX < 0) anPosX = 0; 
    else if (anPosX > (int)(m_dwWidth)) anPosX = m_dwWidth;

    // Clipping vertical
    if (anPosY < 0) anPosY = 0; 
    else if (anPosY > (int)(m_dwHeight)) anPosY = m_dwHeight;  
  }
  
  DWORD       dwFlags;  // Banderas de volcado  
  DDBLTFX     descFX;   // Estructura de FX con el color a utilzar
  RECT        DestRect; // Area destino donde efectuar el blit

  // Inicializaciones  
  memset(&descFX, 0, sizeof(descFX));
  descFX.dwSize = sizeof(descFX);  
  
  // Establece flags de volcado y efectos especiales
  dwFlags = DDBLT_DDFX | DDBLT_WAIT;
  if (m_bSrcColorKey) dwFlags |= DDBLT_KEYSRC;
  if (m_bDestColorKey) dwFlags |= DDBLT_KEYDEST;
  descFX.dwDDFX = dwMirrorFlag;

  // Calcula area destino
  DestRect.left = anPosX;
  DestRect.top = anPosY;
  if (pSrcRect == NULL) {
    // Anchura a considerar la de la superficie pasada
    DestRect.right = anPosX + poSrcSurf->GetWidth() + nScaledX;
    DestRect.bottom = anPosY + poSrcSurf->GetHeight() + nScaledY;
  }
  else {
    // A considerar la que se indica en la estructura RECT pasada
    DestRect.right = anPosX + (pSrcRect->right-pSrcRect->left) + nScaledX;
    DestRect.bottom = anPosY + (pSrcRect->bottom-pSrcRect->top) + nScaledY;
  }  
    
  // Procede a realizar la operacion de blit
  m_CodeResult = m_pDDSurface->Blt(&DestRect, 
								   poSrcSurf->GetObj(), 
								   pSrcRect,
                                   dwFlags, 
								   &descFX);
  if(FAILED(m_CodeResult)) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
   
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia el objeto DXDDClipper pasado a la superficie, lo que
//   implicara que los blitters tengan en cuenta el area de recorte.
// Parametros:
// - poDXDDClipper: Instancia a clase DXDDClipper, con clipper creado
//   y valido.
// Devuelve:
// - true: Todo ha ido bien.
// - false: Ha existido algun tipo de problema.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDDSurface::SetClipper(DXDDClipper* const poDXClipper)
{
  // SOLO si hay superficie creada
  DXASSERT(m_pDDSurface != NULL);
  // SOLO si instancia a DXDDClipper es valida
  DXASSERT(poDXClipper != NULL);
  // SOLO si el clipper se ha creado
  DXASSERT(poDXClipper->GetObj() != NULL);
    
  // Se asocia el clipper
  m_CodeResult = m_pDDSurface->SetClipper(poDXClipper->GetObj());
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Se actualiza flag
  m_bClipper = true;

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera el posible clipper que pudiera estar asociado a la 
//   superficie
// Parametros:
// Devuelve:
// - true: Todo ha ido bien.
// - false: Ha existido algun tipo de problema.
// Notas:
// - En caso de no exisistir ninguno, devolvera true como si lo
//   hubiera liberado.
////////////////////////////////////////////////////////////////////// 
bool DXDDSurface::UnSetClipper(void)
{
  // SOLO si hay superficie creada
  DXASSERT(m_pDDSurface != NULL);  
  
  // Si no habia clipper asociado, retorna sin mas
  if (!m_bClipper) { return true; }

  // Se libera el clipper que estuviera asociado
  m_CodeResult = m_pDDSurface->SetClipper(NULL);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Se actualiza flag
  m_bClipper = false;

  // Todo correct
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Bloquea la superficie para acceder a nivel de pixel
// Parametros:
// Devuelve:
// - true: Todo correcto.
// - false: Hay problemas
// Notas:
// - Cuando se bloquea una superficie, no se puede mandar nada con
//   OutputDebugString.
////////////////////////////////////////////////////////////////////// 
bool DXDDSurface::Lock(void)
{
  // SOLO si hay superficie creada
  DXASSERT(m_pDDSurface != NULL);  

  // Inicializaciones
  memset(&m_DescLock, 0, sizeof(m_DescLock));
  m_DescLock.dwSize = sizeof(m_DescLock);  

  // Procede a bloquear
  m_CodeResult = m_pDDSurface->Lock(NULL, &m_DescLock, DDLOCK_NOSYSLOCK | DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR | DDLOCK_WRITEONLY, NULL);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }   
  
  // Se establece superficie como bloqueada
  m_bLock = true;
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desbloquea la superficie.
// Parametros:
// Devuelve:
// - true: todo ha ido bien.
// - false: ha existido algun problema
// Notas:
// - Es imprescindible haber llamado antes a Lock
////////////////////////////////////////////////////////////////////// 
bool DXDDSurface::UnLock(void)
{
  // SOLO si hay superficie creada
  DXASSERT(m_pDDSurface != NULL);
  // SOLO esta bloqueda
  DXASSERT(m_bLock);
 
  // Desbloquea la superficie
  m_CodeResult = m_pDDSurface->Unlock(NULL);
  if (FAILED(m_CodeResult)) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }
  
  // Se establece superficie como desbloqueada
  m_bLock = false;

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza una conversion de los componentes recibidos por parametro
//   a un DWORD que contendra el color final.
// Parametros:
// - ucRed: Componente rojo.
// - ucGreen: Componente verde.
// - ucBlue: Componente azul.
// Devuelve:
// - Por funcion devuelve el DWORD que contiene el color deseado.
// Notas:
// - Este metodo utiliza la informacion que crea el metodo 
//   SetPixelFormat (el cual es llamado al crear la superficie).
// - Es un metodo universal, es decir, funciona para cualquier
//   configuracion RGB existente.
// - No tiene en cuenta el canal alpha, es decir, en caso de que este
//   presente en la superficie lo pondra a su valor maximo (lo tratara
//   como valor solido).
////////////////////////////////////////////////////////////////////// 
DWORD DXDDSurface::RGBToColor(const byte ucRed, 
                              const byte ucGreen, 
                              const byte ucBlue)
{
  // Variables
  float fAFactor; // Factor alpha
  float fRFactor; // Factor Red
  float fGFactor; // Factor Green
  float fBFactor; // Factor Blue
  DWORD dwColor;  // Color a devolver
  
  // Se obtiene la fraccion de cada componente utilizado. Los valores
  // estaran entre 0.0 y 1.0
  fRFactor = ucRed / 255.0f;
  fGFactor = ucGreen / 255.0f;
  fBFactor = ucBlue / 255.0f;
  // El valor alpha se pone a solido
  fAFactor = 1.0;

  // Se forma el DWORD multiplicando (realmente dividiendo) el bitmask 
  // de cada componente por el factor utilizado. 
  // Al resultado se le aplica una operacion AND para dejar solo 
  // los bits que sean coherentes con el componente
  dwColor = (DWORD(m_RGBInfo.PixelFormat.dwRGBAlphaBitMask * fAFactor) & // A
             m_RGBInfo.PixelFormat.dwRGBAlphaBitMask) 
			 |
            (DWORD(m_RGBInfo.PixelFormat.dwRBitMask * fRFactor) & // R
             m_RGBInfo.PixelFormat.dwRBitMask) 
			 |
            (DWORD(m_RGBInfo.PixelFormat.dwGBitMask * fGFactor) & // G
             m_RGBInfo.PixelFormat.dwGBitMask) 
			 |
            (DWORD(m_RGBInfo.PixelFormat.dwBBitMask * fBFactor) & // B
             m_RGBInfo.PixelFormat.dwBBitMask);

  // Se retorna el color
  return dwColor;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Pone un pixel en la superficie.
// Parametros:
// - nPosx y nPosy: Coordenadas donde poner el pixel.
// - dwColor: Color con el que se pondra el pixel. 
// Devuelve:
// Notas:
// - Es imprescindible haber bloqueado la superficie con Lock.
// - El metodo realiza una operacion de recorte automatica.
// - Se supone que el valor recibido en dwColor ya posee la conversion
//   correspondiente (en caso de estar trabajando con un modo de
//   video que no posee paleta)
////////////////////////////////////////////////////////////////////// 
void DXDDSurface::PutPixel(const int& nPosx, 
						   const int& nPosy, 
						   const dword dwColor)
{
  // SOLO si hay superficie creada
  DXASSERT(m_pDDSurface != NULL);
  // SOLO esta bloqueda
  DXASSERT(m_bLock);
 
  // Realiza operacion de clippeado
  if(nPosx < 0 || nPosx > int(m_dwWidth)) { return; }
  if(nPosy < 0 || nPosy > int(m_dwHeight)) { return; }

  BYTE*   pbMemSurface; // Memoria ocupada por la superficie

  // Inicializaciones
  pbMemSurface = (BYTE*) m_DescLock.lpSurface; // Puntero a superficie
  
  // Se halla el offset. Hay que multiplicar la posicion x por el numero
  // de bytes que hay en cada posicion >> 3 es lo mismo que / 8.
  pbMemSurface += nPosy * m_DescLock.lPitch + 
                  nPosx * (m_DescLock.ddpfPixelFormat.dwRGBBitCount >> 3);
  
  // Dependiendo del Bpp el acceso a la informacion sera distinto
  switch(GetBpp())
  {
    case 8:			
			*pbMemSurface = (BYTE)dwColor;
			break;
    case 15:
    case 16:           
      *((WORD*)pbMemSurface) = (WORD)dwColor;
      break;
    case 24:      
    case 32:
       *((DWORD*)pbMemSurface) = dwColor;
       break;
    default:
       break;
   }  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene un pixel de la superficie.
// Parametros:
// nPosx y nPosy: Coordenadas de donde obtener los pixels.
// Devuelve:
// - El color obtenido de la superficie. 
// Notas:
// - Es imprescindible haber bloqueado la superficie con Lock.
// - El metodo realiza una operacion de recorte automatica.
////////////////////////////////////////////////////////////////////// 
DWORD DXDDSurface::GetPixel(const int& nPosx, 
							const int& nPosy)
{
  // SOLO si hay superficie creada
  DXASSERT(m_pDDSurface != NULL);
  // SOLO si esta bloqueda
  DXASSERT(m_bLock);
 
  // Realiza operacion de clippeado
  if(nPosx < 0 || nPosx > (int)(m_dwWidth)) { return 0; }
  if(nPosy < 0 || nPosy > (int)(m_dwHeight)) { return 0; }

  DWORD   Pixel;        // Pixel a devolver
  BYTE*   pbMemSurface; // Memoria ocupada por la superficie

  // Inicializaciones
  pbMemSurface = (BYTE*)m_DescLock.lpSurface;
 
  // Se halla el offset. Hay que multiplicar la posicion x por el numero
  // de bytes que hay en cada posicion >> 3 es lo mismo que / 8.
  pbMemSurface += nPosy * m_DescLock.lPitch + 
                  nPosx * (m_DescLock.ddpfPixelFormat.dwRGBBitCount >> 3) ;
  
  // Dependiendo del Bpp el acceso a la informacion sera distinto
  switch(GetBpp())
  {
    case 8:      
			Pixel = (DWORD)(*pbMemSurface);    
      break;
    case 15:
    case 16:            
			Pixel = (DWORD)(*((WORD*)pbMemSurface));      
      break;
    case 24:      
    case 32:            
      Pixel = (DWORD)(*((DWORD*)pbMemSurface));
      break;
    default:
      Pixel = 0;
      break;
  }

  // Se retorna el color
  return Pixel;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Toma el DC de la superficie para poder trabajar con funciones
//   GDI.
// Parametros:
// - pDC: DC a tomar.
// Devuelve:
// - true: Todo ha ido bien.
// - false: Ha existido algun tipo de problema.
// - por funcion, el DC obtenido.
// Notas:
// - Puede ocurrir que no se pueda tomar el DC porque se ha perdido
//   la superficie. Sera tarea del manejador revisar el codigo 
//   devuelto por m_CodeResult y ver si es DDERR_SURFACELOST, para
//   realizar una operacion de Restore.
// - Recodar llamar a ReleaseDC despues de acabar con el DC
////////////////////////////////////////////////////////////////////// 
bool DXDDSurface::GetDC(HDC* pDC)
{
  // SOLO si hay superficie creada
  DXASSERT(m_pDDSurface != NULL);
  
  // Procede a obtener el DC
  m_CodeResult = m_pDDSurface->GetDC(pDC);
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
// - Libera el DC de la superficie.
// Parametros:
// Devuelve:
// - true: Todo ha ido bien.
// - false: Ha existido algun tipo de problema.
// Notas:
////////////////////////////////////////////////////////////////////// 
bool DXDDSurface::ReleaseDC(HDC& DC)
{
  // SOLO si hay superficie creada
  DXASSERT(m_pDDSurface != NULL);
  
  // Procede a liberar el DC
  m_CodeResult = m_pDDSurface->ReleaseDC(DC);
  if (FAILED(m_CodeResult)) {// Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
    return false;
  }

  // Todo correcto  
  return true;
}
  
//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si la superficie esta perdida.
// Parametros:
// Devuelve:
// - true: Esta perdida.
// - false: No esta perdida. Verificar que m_ResultCode vale 
//   DD_OK, pues de lo contrario el false correspondera a un
//   error al llamar a la funcion del API correspondiente.
// Notas:
// - Aunque se puede llamar a este metodo con una superficie de tipo
//   ATTACHED, se producira un error si esa superficie esta perdida
//   y se intenta llamar a Restore. Hay que llamar a ese metodo pero
//   en la superficie compleja.
////////////////////////////////////////////////////////////////////// 
bool DXDDSurface::IsLost(void)
{ 
  // SOLO si hay superficie creada
  DXASSERT(m_pDDSurface != NULL);
  
  m_CodeResult = m_pDDSurface->IsLost();
  if (m_CodeResult == DDERR_SURFACELOST) {
    // Superficie perdida
    return true;
  }
  else if (m_CodeResult != DD_OK) {
    // Hay problemas
    DXDDError::DXDDErrorToString(m_CodeResult, m_sLastCode);
  }
  
  // Superficie no perdida o error al ejecutar IsLost
  return false;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Restaura la superficie perdida. Se debera de utilizar en 
//   combinacion con IsLost para saber cuando se ha perdido la
//   superficie.
// Parametros:
// Devuelve:
// - true: Todo ha ido bien
// - false: Ha existido algun problema.
// Notas:
// - Nunca hay que utilizar este metodo con superficies ATTACHED, pues
//   la compleja de la que depende, se encargan de restaurarlas cuando
//   es invocado su metodo Restore.
// - Se recomienda utilizar el metodo RestoreAllSurfaces de la clase
//   DXDraw, pues restaura todas las superficies que dependan de el.
////////////////////////////////////////////////////////////////////// 
bool DXDDSurface::Restore(void)
{
  // SOLO si hay superficie creada
  DXASSERT(NULL != m_pDDSurface);
  // SOLO si la superficie NO es de tipo ATTACHED
  DXASSERT(DXDDDefs::SURFACETYPE_ATTACHED != m_SurfaceType);

  // Procede a recuperar la superficie perdida
  m_CodeResult = m_pDDSurface->Restore();
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
// - Este metodo tomara la estructura m_RGBInfo y la rellenara. Los
//   datos que insertaran seran relativos al formato del pixel en la
//   superficie (recogidos con GetPixelFormat) y, para cada componente,
//   (RGB y canal alpha), los numeros de bits que son utilizados asi
//   como su posicion en el DWORD del color (mediante el calculo del
//   numero de bits a desplazar a la izq.).
// Parametros:
// Devuelve:
// Notas:
// - Cada superficie puede tener un formato independiente del modo
//   de video que se halle activo en la aplicacion.
// - Es necesario saber el numero de bits de cada componente y
//   su posicion en el DWORD para poder calcular correctamente
//   el color.
// - Cuando se trabaje solo con DirectDraw, la superficies back no
//   haran uso del canal alpha, por lo que todos los datos relativos
//   a ese componente estaran a 0.
// - Cuando se trabaje con texturas, el canal alpha podra tener sentido
//   para implementar efectos transparentes y translucidos.
////////////////////////////////////////////////////////////////////// 
void DXDDSurface::SetPixelFormat(void)
{
  // SOLO si la superficie ha sido creada
  DXASSERT(NULL != m_pDDSurface);
    
  // Variables
  DWORD    dwBitMask; // Bitmask temporal

  // Inicializaciones
  m_RGBInfo.PixelFormat.dwSize = sizeof(m_RGBInfo.PixelFormat);  
  m_RGBInfo.dwAlphaNumBits = m_RGBInfo.dwAlphaShift = 0;
  m_RGBInfo.dwRNumBits = m_RGBInfo.dwRShift = 0;
  m_RGBInfo.dwGNumBits = m_RGBInfo.dwGShift = 0;
  m_RGBInfo.dwBNumBits = m_RGBInfo.dwBShift = 0;

  // Se rellena la estructura DDPIXELINFO
  m_pDDSurface->GetPixelFormat(&m_RGBInfo.PixelFormat);

  // Se procede a calcular para cada componente el numero de bits
  // que ocupa y el numero de bits que tendra que desplazarse.

  // Componente Alpha
  dwBitMask = m_RGBInfo.PixelFormat.dwRGBAlphaBitMask;  
  while (dwBitMask && !(dwBitMask & 0x01L)) {
    // Calculo de la posicion 
    m_RGBInfo.dwAlphaShift++;
    dwBitMask >>= 1;
  }
  while (dwBitMask & 0x01L) {
    // Calculo del numero de bits
    m_RGBInfo.dwAlphaNumBits++;
    dwBitMask >>= 1;
  }

  // Componente R
  dwBitMask = m_RGBInfo.PixelFormat.dwRBitMask;  
  while (!(dwBitMask & 0x01L)) {
    // Calculo de la posicion 
    m_RGBInfo.dwRShift++;
    dwBitMask >>= 1;
  }
  while (dwBitMask & 0x01L) {
    // Calculo del numero de bits
    m_RGBInfo.dwRNumBits++;
    dwBitMask >>= 1;
  }

  // Componente G
  dwBitMask = m_RGBInfo.PixelFormat.dwGBitMask;  
  while (!(dwBitMask & 0x01L)) {
    // Calculo de la posicion 
    m_RGBInfo.dwGShift++;
    dwBitMask >>= 1;
  }
  while (dwBitMask & 0x01L) {
    // Calculo del numero de bits
    m_RGBInfo.dwGNumBits++;
    dwBitMask >>= 1;
  }

  // Componente B
  dwBitMask = m_RGBInfo.PixelFormat.dwBBitMask;  
  while (!(dwBitMask & 0x01L)) {
    // Calculo de la posicion 
    m_RGBInfo.dwBShift++;
    dwBitMask >>= 1;
  }
  while (dwBitMask & 0x01L) {
    // Calculo del numero de bits
    m_RGBInfo.dwBNumBits++;
    dwBitMask >>= 1;
  }

}