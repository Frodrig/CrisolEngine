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
// CGraphicLoader.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGraphicLoader.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGraphicLoader.h"

#include "iCLogger.h"        
#include "iCFileSystem.h"    
#include "SYSEngine.h"       

// Inicializacion de la unica instancia singlenton
CGraphicLoader* CGraphicLoader::m_pGraphicLoader = NULL;

// Se definen las estructuras con cabecera
struct CGraphicLoader::TGAHeader {
  // Cabecera del fichero TGA
  byte IDLenght;        // Longitud del campo ID
  byte ColorMap;        // 0 si no hay ColorMap y 1 si lo hay
  byte ImageType;       // Tipo de imagen. Por el momento solo tipo 2.
  byte ColorMapDesc[5]; // Especificaciones del ColorMap
  word XOrigin;         // Posicion X de origen
  word YOrigin;         // Posicion Y de origen
  word Width;           // Anchura de la imagen
  word Height;          // Altura de la imagen
  byte Bpp;             // Bits por pixel
  byte ImageDesc;       // Descripcion de la imagen
};

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia y prepara el buffer en donde se volcaran las
//   imagenes que se vayan cargando. Conforme se vayan cargando imagenes de
//   mayor tamaño, el buffer se ira adaptando y tendra siempre el tamaño del
//   fichero grafico de mayor tamaño que se haya cargado.
// Parametros:
// - udBuffSize: Tamaño inicial del buffer. Por defecto valdra 0.
// Devuelve:
// - Si todo ha ido bien true. Si ha existido algun fallo false.
// Notas:
// - Por razones de eficiencia, seria recomendable pasar el valor maximo que
//   tendra el buffer. Esto permitira no tener que liberar ni reservar
//   memoria de forma reiterada.
///////////////////////////////////////////////////////////////////////////////
bool 
CGraphicLoader::Init(const dword udBuffSize)
{
  // Si la clase esta inicializada hay que llamar a End desde fuera
  if (IsInitOk()) { 
	return false; 
  }

  // ¿Se desea reservar memoria al comienzo?
  if (udBuffSize) { 
	// Si, se crea buffer
    m_pImgFile = new sbyte[udBuffSize];
	ASSERT(m_pImgFile);
	m_udImgFileSize = udBuffSize;    
  } else {
	// No
    m_udImgFileSize = 0;
	m_pImgFile = NULL;
  }
    
  // Se establecen flags internos
  m_ActImgInfo.bImgLoaded = false;
  m_bIsInitOk = true;

  // Traza
  #ifdef ENGINE_TRACE  
	SYSEngine::GetLogger()->Write("CGraphicLoader:Init> Cargador de ficheros gráficos instalado.\n");
	SYSEngine::GetLogger()->Write("                   | Tamaño del buffer de lectura: %u.\n", m_udImgFileSize);  
  #endif

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia de la clase
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGraphicLoader::End(void)
{
  if (IsInitOk()) {
    if (m_pImgFile) { 
      // Se procede a liberar el buffer
	  delete[m_udImgFileSize] m_pImgFile;
	  m_pImgFile = NULL;	  
    }	
    
	#ifdef ENGINE_TRACE  
	  SYSEngine::GetLogger()->Write("CGraphicLoader:End> Cargador de ficheros gráficos desinstalado.\n");  
	#endif    
    
	  // Se baja el flag de carga
    m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Abre el fichero szFileName, lee el contenido del mismo en m_pImgFile y 
//   luego lo cierra. En caso de que el tamaño del buffer sea insuficiente,
//   se liberara y se reservara el espacio necesario.
// Parametros:
// - aszFileName: Referencia al nombre del fichero
// - m_pImgFile: Contendra el fichero y se podra ajustar.
// - m_udImgFileSize: Podra contener una nueva cantidad si m_pImgFile es 
//   ajustado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGraphicLoader::ReadImage(const std::string& aszFileName)
{
  // Se abre el fichero y se procede a cargar la informacion en el buffer    
  iCFileSystem* const pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys);
  const FileDefs::FileHandle hFile = pFileSys->Open(aszFileName);
  if (!hFile) {
	SYSEngine::FatalError("No se ha podido encontrar la imagen %s.\n", aszFileName.c_str());
  }

  // ¿Hay suficiente espacio en el buffer de lectura?
  const dword udFileSize = pFileSys->GetFileSize(hFile);  
  ASSERT(udFileSize);
  if (udFileSize > m_udImgFileSize) { 
	// Se libera el buffer y se reserva mas espacio
	if (m_pImgFile) { 
	  delete[m_udImgFileSize] m_pImgFile; 
	}    
    m_pImgFile = new sbyte[udFileSize];
    ASSERT(m_pImgFile);
    m_udImgFileSize = udFileSize;
  }

  // Se lee del fichero y despues se cierra
  pFileSys->Read(hFile, m_pImgFile, udFileSize);
  pFileSys->Close(hFile);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el fichero recibido es un TGA valido. Obtiene dimensiones.
// Parametros:
// Devuelve:
// - Si el buffer contiene un fichero TGA valido true. En caso contrario, false.
// Notas:
// - Solo se aceptaran fichero TGA SIN comprimir y SIN color map
///////////////////////////////////////////////////////////////////////////////
bool 
CGraphicLoader::CheckTGAFile(void)
{
  // SOLO si existe buffer donde haber leido
  ASSERT(m_pImgFile);

  // Lee cabecera
  TGAHeader Header;
  memcpy(&Header, m_pImgFile, sizeof(TGAHeader));  
  
  // Se comprueba que el TGA no este comprimido ni tenga color map
  if (0 != Header.ColorMap || 
	  2 != Header.ImageType) { 
	return false; 
  }
  
  // Rellena campos de informacion de la imagen
  m_ActImgInfo.uwHeight = Header.Height;
  m_ActImgInfo.uwWidth = Header.Width;  
  m_ActImgInfo.udBpp = Header.Bpp;
  m_ActImgInfo.udHeaderLenght = sizeof(Header) + Header.IDLenght;
  
  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este metodo se encargara de cargar en el buffer una imagen TGA.
// Parametros:
// - szFileName: Nombre del archivo TGA.
// Devuelve:
// - Si se ha podido cargar en el buffer true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGraphicLoader::TGALoader(const std::string& szFileName)
{
  // Namespaces
  using namespace GraphDefs;

  // SOLO si el modulo esta inicializado
  ASSERT(IsInitOk());

  // ¿Hay un fichero abierto?
  if (m_ActImgInfo.bImgLoaded) { 
    // ¿El fichero actual es el mismo? 
    if (0 == m_ActImgInfo.szFileName.compare(szFileName)) { 
      // No hace falta cargar -> imagen en buffer
      return true;
    }
  }

  // Se lee el fichero y se comprueba que sea valido
  ReadImage(szFileName);  
  if (!CheckTGAFile()) { 
    // Hay problemas
	#ifdef ENGINE_TRACE  
		SYSEngine::GetLogger()->Write("CGraphicLoader:TGALoader> El fichero %s no es TGA.\n", szFileName);  
	#endif
    return false; 
  } 

  // Rellena campos de informacion de la imagen
  m_ActImgInfo.bImgLoaded = true;
  m_ActImgInfo.FileFormat = TGAFile;
  m_ActImgInfo.szFileName = szFileName;

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Vuelca la porcion prSrcRect de informacion grafica situada en m_pImgFile, 
//   sobre la zona prDestRect de la textura pTexture.
// Parametros:
// - pTexture: Direccion de la textura en donde volcar la informacion.
// - prSrcRect: Direccion de la estructura sRect que contiene la zona de la
//   imagen a volcar.
// - prDestRect: Direccion de la estructura sRect que contiene la zona de la
//   textura en donde volcar esa informacion.
// Devuelve:
// Notas:
// - Por defecto las dos estructuras sRect son NULL. Eso significara que se
//   querra tomar la dimension global de la imagen o la textura respectivamente
// - Se efectuaran operaciones de clippeado para tener estructuras sRect 
//   logicas.
///////////////////////////////////////////////////////////////////////////////
bool 
CGraphicLoader::Draw(DXDDSurfaceTexture* const pTexture, 
					 const sRect* prSrcRect, const sRect* prDestRect)
{
  // Namespaces
  using namespace GraphDefs;

  // SOLO si clase inicializada
  ASSERT(IsInitOk());
  // SOLO si la textura es valida
  ASSERT(pTexture);  
  // SOLO si se ha cargado antes imagen
  ASSERT(IsImageLoaded());

  // Se ajusta el area origen  
  sRect rSrc;
  if (!prSrcRect) { 
	// Se toma el tamaño total
    rSrc.swLeft = 0;
    rSrc.swTop = 0;
    rSrc.swRight = m_ActImgInfo.uwWidth - 1;
    rSrc.swBottom = m_ActImgInfo.uwHeight - 1;
  }
  else { 
	// Se copia area origen    
	rSrc = *prSrcRect;
    ASSERT((rSrc.swLeft < rSrc.swRight) != 0);
    ASSERT((rSrc.swTop < rSrc.swBottom) != 0);
  }

  // Se ajusta el area destino
  sRect rDest;
  if (!prDestRect) { 
	// Se toma el tamaño total
    rDest.swLeft = 0;
    rDest.swTop = 0;
    rDest.swRight = pTexture->GetWidth() - 1;
    rDest.swBottom = pTexture->GetHeight() - 1;
  }
  else { 
	// Se copia area destino    
	rDest = *prDestRect;
    ASSERT(bool(rDest.swLeft < rDest.swRight));
    ASSERT(bool(rDest.swTop < rDest.swBottom));
  }
  
  // Clipper sobre origen
  if (rSrc.swRight > m_ActImgInfo.uwWidth) { 
    rDest.swRight = m_ActImgInfo.uwWidth; 
  }
  if (rSrc.swBottom > m_ActImgInfo.uwHeight) { 
    rDest.swBottom = m_ActImgInfo.uwHeight; 
  }

  // Clipper sobre destino
  if (rDest.swRight > pTexture->GetWidth()) { 
     rDest.swRight = pTexture->GetWidth();
  }
  if (rDest.swBottom > pTexture->GetHeight()) { 
      rDest.swBottom = pTexture->GetHeight();
  }

  // Se comprueba formato grafico y se procede a dibujar en textura  
  bool bResult = false;
  switch(m_ActImgInfo.FileFormat) {
    case TGAFile:
      bResult = TGADraw(pTexture, rSrc, rDest);      
      break;
  };

  // Se devuelve resultado de la operacion
  return bResult;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Vuelca la porcion prSrcRect de informacion grafica del fichero TGA leido
//   en m_pImgFile sobre la zona prDestRect de la textura pTexture. Este
//   metodo sera llamado desde Draw.
// Parametros:
// - pTexture: Direccion de la textura en donde volcar la informacion.
// - arSrcRect: Estructura sRect que contiene la zona de la imagen A VOLCAR.
// - arDestRect: Estructura sRect que contiene la zona de la textura EN DONDE 
//   volcar esa informacion.
// Devuelve:
// - Si ha existido algun problema false. En caso contrario true.
// Notas:
// - En este metodo se esperara recibir valores para las estructuras sRect 
//   validos. En el caso de que las estructuras sRect origen y destino sean
//   diferentes, se rompera en cuanto se halle el extremo mas pequeño de la
//   ventana de lectura o escritura, de tal forma que existira un clipper
//   automatico.
// - Se va a trabajar bajo el supuesto de que los archivos TGA estan grabados
//   desde el photoshop.
///////////////////////////////////////////////////////////////////////////////
bool 
CGraphicLoader::TGADraw(DXDDSurfaceTexture* const pTexture, 
						const sRect& arSrcRect, 
						const sRect& arDestRect)

{
  // SOLO si clase inicializada
  ASSERT(IsInitOk());
  // SOLO si la textura es valida
  ASSERT(pTexture);
  // SOLO si las estructuras sRect son validas
  ASSERT((arSrcRect.swRight > arSrcRect.swLeft) != 0);
  ASSERT((arSrcRect.swBottom > arSrcRect.swTop) != 0);
  ASSERT((arDestRect.swRight > arDestRect.swLeft) != 0);
  ASSERT((arDestRect.swBottom > arDestRect.swTop) != 0);
  // SOLO si se ha cargado antes imagen
  ASSERT(IsImageLoaded());
  // SOLO si el buffer con los datos del TGA es valido
  ASSERT(m_pImgFile);
  
  // Vbles
  dword   udBpp;    // Bits por posicion
  dword   udOffset; // Offset para el desplazamiento en el buffer
  byte    ubR;      // Componente R
  byte    ubG;      // Componente G
  byte    ubB;      // Componente B
  byte    ubAlpha;  // Componente Alpha
  dword   udYSrc;   // Iterador Y para la imagen
  dword   udXSrc;   // Iterador X para la imagen
  dword   udYDest;  // Iterador Y para la textura
  dword   udXDest;  // Iterador X para la textura

  // Inicializaciones
  udBpp = m_ActImgInfo.udBpp / 8;

  // Se bloqea la superficie
  if (!pTexture->Lock()) { 
    // Hay problemas
	#ifdef ENGINE_TRACE  
		SYSEngine::GetLogger()->Write("CGraphicLoader:Draw> Problemas al bloquear la superficie DX.\n");  
		SYSEngine::GetLogger()->Write("                   | Código DX de error: %s.\n", pTexture->GetLastStrCode().c_str());
	#endif
    return false; 
  }
    
  // Se vuelca la informacion pixel a pixel
  // Se recorren las estructuras sRect y se detiene la iteracion en cuanto
  // se llegue al limite de la que sea mas pequeña
  udYSrc = arSrcRect.swTop;
  udYDest = arDestRect.swTop;
  while (udYSrc <= arSrcRect.swBottom &&
	     udYDest <= arDestRect.swBottom) { 
    udXSrc = arSrcRect.swLeft;
    udXDest = arDestRect.swLeft;
    while (udXSrc <= arSrcRect.swRight && 
		   udXDest <= arDestRect.swRight) {            
      // Se calculan valores previos. Este codigo es dependiente de
      // la forma en que Adobe Photoshop guarda los archivos
      udOffset = m_ActImgInfo.udHeaderLenght +  // Siempre se salta la cabecera
                 (m_ActImgInfo.uwHeight - udYSrc - 1) * 
                 m_ActImgInfo.uwWidth* udBpp + udXSrc * udBpp;      
      
      // Se calculan los componentes RGB
      ubR = m_pImgFile[udOffset + 2];
      ubG = m_pImgFile[udOffset + 1];
      ubB = m_pImgFile[udOffset];
      
      // Se calcula el valor alpha
      if (4 == udBpp) { 
		ubAlpha = m_pImgFile[udOffset + 3]; 
	  } else { 
		ubAlpha = 0xFF; 
	  }      
      
      // Se dibuja el bitmap en la superficie
      pTexture->PutPixel(udXDest, udYDest, ubR, ubG, ubB, ubAlpha);

      // Se incrementan iteradores
      udXSrc++;
      udXDest++;
    } // while x

    // Se incrementan iteradores
    udYSrc++;
    udYDest++;
  } // while y
  
  // Se desbloquea la superficie
   if (!pTexture->UnLock()) { 
     // Hay problemas 
	  #ifdef ENGINE_TRACE  
		  SYSEngine::GetLogger()->Write("CGraphicLoader:Draw> Problemas al desbloquear la superficie DX.\n");
		  SYSEngine::GetLogger()->Write("                   | Código DX de error: %s.\n", pTexture->GetLastStrCode().c_str());
	  #endif
    return false; 
  }
  
  // Todo correcto
  return true;
}