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
// CResourceManager.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CResourceManager.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CResourceManager.h"

#include "iCLogger.h" 
#include "iCGraphicLoader.h"
#include "iCGraphicSystem.h"
#include "iCAudioSystem.h"
#include "iCMathUtil.h"
#include <list>
#include <algorithm>

// Inicializacion de la unica instancia singlenton
CResourceManager* CResourceManager::m_pResourceManager = NULL;

// Inicializacion de los manejadores de memoria
CMemoryPool 
CResourceManager::sNTexture::m_MPool(32,
                                     sizeof(CResourceManager::sNTexture),
                                     true);
CMemoryPool
CResourceManager::sNWAVBufferInstance::m_MPool(32,
											   sizeof(CResourceManager::sNWAVBufferInstance),
											   true);
CMemoryPool
CResourceManager::sNWAVBufferInfo::m_MPool(16,
										   sizeof(CResourceManager::sNWAVBufferInfo),
										   true);
CMemoryPool
CResourceManager::sNWAVSound::m_MPool(32,
									  sizeof(CResourceManager::sNWAVSound),
									  true);
CMemoryPool
CResourceManager::sNAnimTemplateData::m_MPool(64,
											  sizeof(CResourceManager::sNAnimTemplateData),
											  true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia.
// Parametros:
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
// - No se dejara reinicializar.
// - Las estructuras de datos internas (array de listas y arbol con todos los
//   recursos registrados), habran sido inicializadas al llamar al metodo
//   End de esta clase.
///////////////////////////////////////////////////////////////////////////////
bool 
CResourceManager::Init(void)
{
  // ¿Esta el modulo inicializado?
  if (IsInitOk()) { 
	return false; 
  }
   
  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("CResourceManager::Init> Inicializando servidor de recursos.\n");      
  #endif

  // Se toman instancias a otros subsistemas
  m_pGraphLoader = SYSEngine::GetGraphicLoader();
  ASSERT(m_pGraphLoader);
  m_pMathUtil = SYSEngine::GetMathUtil();
  ASSERT(m_pMathUtil);
  
  // Todo correcto
  #ifdef ENGINE_TRACE    
	SYSEngine::GetLogger()->Write("                      | Ok.\n");      
  #endif
  m_bInitOk = true;  
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
// - La liberacion de map's, si todo va bien, no debera de liberar ningun 
//   nodo ya que indicara que desde el exterior estos han sido desinstalados
//   correctamente. Se activa, sin embargo, para evitar perdidas de memoria
///////////////////////////////////////////////////////////////////////////////
void 
CResourceManager::End(void)
{
  if (IsInitOk()) {
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CResourceManager::End> Desinstalando servidor de recursos.\n");        
	#endif

	// Se liberan map con los distintos tipos de recursos
	// Nota: en teoria los map deberian de estar vacios
	EndTextureBank();
	EndWAVSoundBank();
	EndWAVBufferInfoBank();
	EndAnimTemplateDataBank();	

	// Baja flag
	m_bInitOk = false;

	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("                     | Ok.\n");        
	#endif
  }
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Interfaz para el registro de una textura que se debe de crear a partir de
//   un fichero con un grafico asociado.
//   En caso de que la textura ya este creada y este asociada a un fichero se
//   devolvera el mismo identificador.
// Parametros:
// - szResDesc: Nombre de la textura y del fichero con el grafico asociado.
// - aBpp: Bits por pixel de la textura (8, 16, 24 o 32).
// - aAlphaBit: Bits que se utilizaran para el canal alpha (0, 1, 4 u 8).
// Devuelve:
// - El identificador del recurso. En caso de que haya existido algun tipo de
//   error, el indentificador valdra 0. 
// Notas:
// - Este metodo tan solo actuara de interfaz. El verdadero trabajo se llevara
//   en CreateTexture.
// - Todos los nombres se pasaran a minusculas.
///////////////////////////////////////////////////////////////////////////////
ResDefs::TextureHandle 
CResourceManager::TextureRegister(const std::string& szResDesc,
								  const GraphDefs::eBpp& Bpp,
								  const GraphDefs::eABpp& AlphaBits)
{
  // // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se crea la textura  
  std::string szResDescLowercase(szResDesc);
  SYSEngine::MakeLowercase(szResDescLowercase);
  return CreateTexture(szResDescLowercase, 
					   sRect(0, 0, 0, 0), 
					   Bpp, 
					   AlphaBits, 
					   true);
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Interfaz para el registro de una textura que se debe de crear a partir de
//   un fichero con un grafico asociado, pero volcando solo una zona de dicho
//   fichero. Esa zona determinara el tamaño de la textura.
//   En caso de que la textura ya este creada y este asociada a un fichero con
//   el mismo nombre y dimensiones, se devolvera el mismo identificador.
// Parametros:
// - szResDesc: Nombre de la textura y del fichero con el grafico asociado.
// - aBpp: Bits por pixel de la textura (8, 16, 24 o 32).
// - aAlphaBit: Bits que se utilizaran para el canal alpha (0, 1, 4 u 8).
// Devuelve:
// - El identificador del recurso. En caso de que haya existido algun tipo de
//   error, el indentificador valdra 0. 
// Notas:
// - Este metodo tan solo actuara de interfaz. El verdadero trabajo se llevara
//   en CreateTexture.
// - Todos los nombres se pasaran a minusculas.
///////////////////////////////////////////////////////////////////////////////
ResDefs::TextureHandle 
CResourceManager::TextureRegister(const std::string& szTextureName,
							      const sRect& rDimension,
								  const GraphDefs::eBpp& Bpp,
								  const GraphDefs::eABpp& AlphaBits)
{
  // // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos  
  ASSERT((rDimension.swRight > rDimension.swLeft) != 0);
  ASSERT((rDimension.swBottom > rDimension.swTop) != 0);

  // Se crea la textura  
  std::string szTextureNameLowercase(szTextureName);
  SYSEngine::MakeLowercase(szTextureNameLowercase);
  return CreateTexture(szTextureNameLowercase, 
					   rDimension, 
					   Bpp, 
					   AlphaBits, 
					   true);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Interfaz para el registro de una textura que se creara sin estar asociada
//   a un fichero grafico.
//   En caso de que la textura ya este creada y no este asociada a un fichero se
//   devolvera el mismo identificador.
// - Este metodo tan solo actuara de interfaz. El verdadero trabajo se llevara
//   en CreateTexture.
// Parametros:
// - szResDesc: Nombre de la textura.
// - uwWidht, uwHeight. Anchura y altura de la textura.
// - aBpp: Bits por pixel de la textura (8, 16, 24 o 32).
// - aAlphaBit: Bits que se utilizaran para el canal alpha (0, 1, 4 u 8).
// Devuelve:
// - El identificador del recurso. En caso de que haya existido algun tipo de
//   error, el indentificador valdra 0. 
// Notas:
// - Este metodo tan solo actuara de interfaz. El verdadero trabajo se llevara
//   en CreateTexture.
// - Todos los nombres se pasaran a minusculas.
// - Las dimensiones en el sRect se deberan de poner en una unidad menos, ya que
//   la formula para posteriormente hallar las dimensiones sera:
//   (right - left) + 1 y (bottom - top) + 1;
///////////////////////////////////////////////////////////////////////////////
ResDefs::TextureHandle 
CResourceManager::TextureRegister(const std::string& szResDesc,
								  const word uwWidth,
								  const word uwHeight,
								  const GraphDefs::eBpp& Bpp,
								  const GraphDefs::eABpp& AlphaBits)
{
  // // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(uwWidth);
  ASSERT(uwHeight);

  // Se crea la textura y se devuelve identificador
  std::string szResDescLowercase(szResDesc);
  SYSEngine::MakeLowercase(szResDescLowercase);
  return CreateTexture(szResDescLowercase, 
					   sRect(0, uwWidth - 1, 0, uwHeight - 1), 
					   Bpp, 
					   AlphaBits, 
					   false);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este metodo sera el que verdaderamente se encargue de crear las texturas
//   y alojarlas en el indice de texturas. Sera utilizado tanto si queremos
//   registrar una textura asociada a un fichero grafico como si no.
//   En caso de que la textura este asociada a un fichero, se encargara de
//   llevarlo a memoria y obtener sus dimensiones, pues los valores recibidos
//   por parametro se obviaran.
// - Siempre que se quiera registrar una textura que ya exista (por nombre)
//   se devolvera el mismo indentificador SIEMPRE Y CUANDO la textura a crear
//   sea igual a la creada en el campo indicativo de si esta asociada a un
//   fichero, de lo contrario se devolvera ID 0 indicando error.
// Parametros:
// - szResDesc: Referencia con el nombre de la textura y del 
//   archivo que contiene el grafico asociado (si es que la textura esta 
//   asociada a un fichero grafico).
// - rDimension. Rect con las dimensiones de la imagen y la zona del archivo
//   en donde coger la informacion.
// - aBpp: Referencia a los bits por pixel de la textura (8, 16, 24 o 32).
// - aAlphaBit: Referencia a los bits que se utilizaran para el canal 
//   alpha (0, 1, 4 u 8).
// - bAttachToFile: Referencia al bool indicativo de si la textura esta
//   asociada a un fichero grafico.
// Devuelve:
// - El identificador del recurso. En caso de que haya existido algun tipo de
//   error, el indentificador valdra 0.
// Notas:
// - No se evaluara desde dentro de este metodo el tipo de identificador
//   devuelto. Se deja la responsabilidad al modulo exterior.
// - En caso de que se quiera registrar una textura que ya lo este podran
//   ocurrir dos cosas. Si la textura ya registrada esta creada sobre un 
//   fichero, se obviaran los parametros recibidos en este metodo y se devolvera
//   el mismo identificador. En caso de que la textura ya creada no estuviera
//   construida sobre un fichero, se devolvera un identificador 0 indicando 
//   error.
// - No se evaluara desde dentro de este metodo el tipo de identificador
//   devuelto. Se deja la responsabilidad al modulo exterior.
///////////////////////////////////////////////////////////////////////////////
ResDefs::TextureHandle 
CResourceManager::CreateTexture(const std::string& szResDesc,
								const sRect& rDimension,
								const GraphDefs::eBpp& Bpp,
								const GraphDefs::eABpp& AlphaBits,
								const bool bAttachToFile)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(!szResDesc.empty());

  // Se itera por el banco de texturas para comprobar si existe algun nodo
  // con el mismo nombre de la textura.
  TextureInfoMapIt TextureBankIt(m_TextureBank.begin());
  for (; TextureBankIt != m_TextureBank.end(); ++TextureBankIt) {
	// ¿Es el mismo nombre de textura?
	if (0 == szResDesc.compare(TextureBankIt->second->szName)) { 
	  // ¿Se estan buscando nombres asociados a ficheros?
	  if (bAttachToFile) {
		// Si, ¿La textura encontrada es de igual dim. que se debe registrar?
		if (rDimension == TextureBankIt->second->rDimension) {
		  // Si, se abandona bucle		  
		  break;
		}
	  } else {
		// No, luego este nodo nos vale, se abandona bucle
		break;
	  }
	}
  }

  // ¿Se encontro textura?
  if (TextureBankIt != m_TextureBank.end()) {
	// Si, se incrementa el contador de referncias y se retorna
	ASSERT_MSG((bAttachToFile == IsTextureAttachToFile(TextureBankIt->second->Params)) != 0, "La textura ya esta registrada pero de distinta forma");
	TextureBankIt->second->uwRefs++;
	return TextureBankIt->first;
  }

  // No se encontro textura
  // Se crea un nuevo nodo en memoria
  sNTexture* const pIndexNode = new sNTexture;
  ASSERT(pIndexNode);

  // ¿La textura esta asociada a un fichero grafico?
  sRect rGraphZone(rDimension);
  if (bAttachToFile) {
	// Si, se carga la textura
	m_pGraphLoader->TGALoader(szResDesc);

	// ¿La dimension no contiene la zona del archivo a volcar?
	// Nota: Ocurrira si el valor swRigh es menor o igual a 0
	if (rGraphZone.swRight < 1) {	  
	  rGraphZone.swLeft = 0;
	  rGraphZone.swRight = m_pGraphLoader->GetWidth() - 1;	  
	  rGraphZone.swTop = 0;
	  rGraphZone.swBottom = m_pGraphLoader->GetHeight() - 1;
	}
  }

  // Se codifican los parametros
  const TextureParams Params = MakeTextureParams(Bpp, AlphaBits, bAttachToFile);
    
  // Genera un nuevo identificador para el recurso
  const ResDefs::TextureHandle hTextureReturn = CreateTextureHandle();
  ASSERT(hTextureReturn);
    
  // Se incializa el recurso
  iCGraphicSystem* const pGraphSys = SYSEngine::GetGraphicSystem();
  ASSERT(pGraphSys);
  if (!pIndexNode->TextureRes.Create(pGraphSys->GetDXDraw(),
                                     GetTextureWidth(rGraphZone),
                                     GetTextureHeight(rGraphZone),
                                     pGraphSys->GetTexturePixelFormat(Bpp, AlphaBits),
                                     pGraphSys->IsHardwareDevice(),
                                     pGraphSys->IsPow2Texture(),
                                     false)) {
	SYSEngine::FatalError("Error fatal al crear textura %s de dimensiones: %u x %u.\n", 
                          pIndexNode->szName.c_str(), 
						  GetTextureWidth(rGraphZone), 
						  GetTextureHeight(rGraphZone));
	return 0;
  }

  // Se vuelca el contenido de la textura asociada si procede
  if (bAttachToFile) { 
    m_pGraphLoader->Draw(&pIndexNode->TextureRes, &rGraphZone); 
  }

  // Se rellenan resto de campos en el nodo
  pIndexNode->szName = szResDesc;
  pIndexNode->uwRefs = 1;                
  pIndexNode->Params = Params;    
  pIndexNode->rDimension = rGraphZone;
    
  // Se inserta el recurso en el arbol y se retorna
  m_TextureBank.insert(TextureInfoValType(hTextureReturn, pIndexNode));
  return hTextureReturn; 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera todos los recursos de tipo textura. Este metodo en teoria, no
//   deberia de hallar informacion alguna ya que de lo contrario significara
//   que no ha habido una liberacion correcta en los clientes.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CResourceManager::EndTextureBank(void)
{
  // Libera todos los recursos de tipo textura
  TextureInfoMapIt It(m_TextureBank.begin());
  for (; It != m_TextureBank.end(); It = m_TextureBank.erase(It)) {
	delete It->second;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este metodo se encargara de devolver la instancia a la textura cuyo 
//   identificador sea auwIDResource. 
// Parametros:
// - hTexture: Identificador del recurso del cual se quiere obtener la
//   instancia.
// Devuelve:
// - La instancia al recurso de tipo textura.
// Notas:
// - Los casos en los que el identificador pasado no este asociado a ningun
//   nodo o bien este asociado a un recurso que no sea textura, se evaluara
//   mediante ASSERT.
///////////////////////////////////////////////////////////////////////////////
DXDDSurfaceTexture* const 
CResourceManager::GetTexture(const ResDefs::TextureHandle& hTexture)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si el identificador es valido
  ASSERT(hTexture);

  // Se obtiene nodo y se retorna
  const TextureInfoMapIt TextureBankIt(m_TextureBank.find(hTexture));
  ASSERT((TextureBankIt != m_TextureBank.end()) != 0);
  return &TextureBankIt->second->TextureRes;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera un recurso de tipo textura. Si el contador es superior a 1, solo
//   decrementara el numero de referencias. Si es igual a 1, destruira el nodo
//   y con el la instancia al recurso de tipo textura.
// Parametros:
// - auwIDResource: Referencia al identificador del recurso.
// Devuelve:
// Notas:
// - Se considerara que el identificador del recurso pasado existe, por lo que
//   la no existencia sera evaluada con un ASSERT.
///////////////////////////////////////////////////////////////////////////////
void 
CResourceManager::ReleaseTexture(const ResDefs::TextureHandle& hTexture)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hTexture);

  // Se obtiene nodo y se retorna
  const TextureInfoMapIt TextureBankIt(m_TextureBank.find(hTexture));
  ASSERT((TextureBankIt != m_TextureBank.end()) != 0);

  // ¿Hay mas de un cliente?
  if (TextureBankIt->second->uwRefs > 1) { 
    // Se decrementa el contador de referencias y se abandona
    TextureBankIt->second->uwRefs--;    
  } else{ 
	// No, el cliente hTexture es el unico, luego se liberara nodo de memoria y map
	TextureBankIt->second->TextureRes.Destroy();
	delete TextureBankIt->second;
	m_TextureBank.erase(TextureBankIt);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retorna el nombre del fichero asociado al handle de textura.
// Parametros:
// - hTexture. Handle a la textura.
// Devuelve:
// - Nombre del fichero.
// Notas:
///////////////////////////////////////////////////////////////////////////////
std::string
CResourceManager::GetTextureFileName(const ResDefs::TextureHandle& hTexture)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se obtiene nodo y se retorna
  const TextureInfoMapIt TextureBankIt(m_TextureBank.find(hTexture));
  ASSERT((TextureBankIt != m_TextureBank.end()) != 0);
  return TextureBankIt->second->szName;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de codificar los parametros de un recurso de tipo textura en
//   un word. El formato de los parametros de tipo textura sera el siguiente;
//   Dado un word: byte1 byte0
//   * byte0 -> b7 b6 b5 b4 b3 b2 b1 b0
//      * b0 b1 -> Definiran los Bpp:  
//                 - 00 -> 8Bpp, 01 -> 16Bpp, - 10 -> 24Bpp, 11 -> 32Bpp.
//      * b2 b3 -> Definiran los bits para el canal alpha:
//                 - Si hay 8Bpp o 24Bpp hay 0 bits. 
//                 - Si hay 32Bpp hay 8bits
//                 - Si hay 16Bpp: 00 -> 0Bits, 01 -> 1Bit, 10 -> 4Bits
//      * b4 -> Indica si la textura esta asociada aun fichero grafico, en
//              cuyo caso, su nombre sera el del fich. grafico tambien.
//              - 0 -> No asociada
//              - 1 -> Asociada
// Parametros:
// - aBpp: Bpp de la textura.
// - aAlphaBits: Bits en el canal alpha en la textura
// - abAttachToFile: Indica si esta asociada a un fichero grafico.
// Devuelve:
// - Devuelve un word con los parametros codificados, segun las directrices
//   descritas en el apartado de descripcion.
// Notas:
///////////////////////////////////////////////////////////////////////////////
CResourceManager::TextureParams 
CResourceManager::MakeTextureParams(const GraphDefs::eBpp& aBpp,
								    const GraphDefs::eABpp& aAlphaBits,
								    const bool abAttachToFile)
{
  // // SOLO si instancia inicializada
  ASSERT(IsInitOk());  

  // Bpp de la textura (tercer byte / b0 y b1) y canal alpha si procede (modo 16Bpp)
  CResourceManager::TextureParams Params = 0;
  switch (aBpp) {
	case GraphDefs::BPP_8: { // 00
      Params |= 0x0000;
	} break;

    case GraphDefs::BPP_16: { // 01
      Params |= 0x0001;
      // Bits del canal alpha (tercer byte / b2 y b3).
      switch (aAlphaBits) {
        case GraphDefs::ALPHA_BPP_0: // 00
          Params |= 0x0000;
          break;
        case GraphDefs::ALPHA_BPP_1: // 01
          Params |= 0x0004;
          break;       
        case GraphDefs::ALPHA_BPP_4: // 10
          Params |= 0x0008;
          break;  
      };
	} break;

	case GraphDefs::BPP_24: { // 10
      Params |= 0x0002;
	} break;

    case GraphDefs::BPP_32: { // 11
      Params |= 0x0003;
	} break;    
  };  
  
  // Se codifica el campo indicativo de asociacion a fich. grafico  
  if (abAttachToFile) {
    // Asociado tercer byte / bit 4 a 1
    Params |= 0x0010;
  } else {
    // No asociado tercer byte / bit 4 a 0
    Params |= 0x0000;
  }

  // Retorna los parametros codificados en un word
  return Params;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene los Bpp de la textura, a partir de los parametros codificados.
// Parametros:
// - auwParams: Referencia a los parametros codificados de la textura.
// Devuelve:
// - Los Bpp de la textura.
// Notas:
// - Consultar MakeTextureParams para saber la disposicion de los bits
//   de codificacion
///////////////////////////////////////////////////////////////////////////////
GraphDefs::eBpp 
CResourceManager::GetTextureBpp(const CResourceManager::TextureParams& Params)
{
  // SOLO si la instancia esta inicializada
  ASSERT(IsInitOk());

  // Comprueba el dispositivo de codificacion
  const word uwBpp = Params & 0x0003;  
  if (0 == uwBpp) { 
	return GraphDefs::BPP_8;  
  } else if (uwBpp & 0x0001) { 
	return GraphDefs::BPP_16; 
  } else if (uwBpp & 0x0002) { 
	return GraphDefs::BPP_24; 
  } else if (uwBpp & 0x0003) { 
	return GraphDefs::BPP_32; 
  }

  // No se sabe
  return GraphDefs::BPP_NOVALID;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve el numero de bits destinados al canal alpha. Como los bits del
//   canal alpha son dependiente de los Bpp, el metodo esperara recibir por
//   parametro el numero de Bpp's para poder descodificar correctamente.
// Parametros:
// - udParams: Referencia a los parametros codificados de la textura.
// - Bpp: Referencia a los Bpp de la textura.
// Devuelve:
// - El numero de bits utilizados para el canal alpha.
// Notas:
// - Consultar MakeTextureParams para saber la disposicion de los bits
//   de codificacion
///////////////////////////////////////////////////////////////////////////////
GraphDefs::eABpp 
CResourceManager::GetTextureAlphaBits(const CResourceManager::TextureParams& Params, 
                                      const GraphDefs::eBpp& Bpp)
{
  // SOLO si la instancia esta inicializada
  ASSERT(IsInitOk());  
    
  // Comprueba el dispositivo de codificacion, segun los Bpp recibidos
  const word uwAlphaBits = Params & 0x000C;
  switch (Bpp) {
	case GraphDefs::BPP_8: {
      // No hay canal alpha
      return GraphDefs::ALPHA_BPP_0;
	} break;
    
	case GraphDefs::BPP_16: {
      // El canal alpha puede ser de 0 bits, 1 bit o 4 bits
      if (0 == uwAlphaBits) { 
		return GraphDefs::ALPHA_BPP_0; 
	  } else if (uwAlphaBits & 0x0001) { 
		return GraphDefs::ALPHA_BPP_1; 
	  } else if (uwAlphaBits & 0x0002) { 
		return GraphDefs::ALPHA_BPP_4; 
	  }
	} break;

	case GraphDefs::BPP_24: {
      // No hay canal alpha
      return GraphDefs::ALPHA_BPP_0;
	} break;

    case GraphDefs::BPP_32: {
      // Hay 8 bits de canal alpha
      return GraphDefs::ALPHA_BPP_8;
	} break;
  }; // ~ switch

  // No hay canal alpha
  return GraphDefs::ALPHA_BPP_0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Registra un sonido WAV. El procedimiento de registro de un sonido, 
//   supondra en primer lugar la creacion de un buffer de sonido, que sera la
//   instancia que contendra los datos. El buffer de sonido podra ser compartido
//   pero siempre se debera de cumplir, que todos los sonidos WAV deberan de
//   poseer un buffer de sonido con los mismos parametros (de control y de 
//   zona de memoria en donde han sido creados) de lo contrario el proceso
//   fallara. Y esto sera asi, independientemente de que el fichero a 
//   reproducir sea el mismo.
// Parametros:
// - szWAVFileName. Nombre del fichero WAV.
// - bCtrlVolume. ¿Se desea controlar el volumen del fichero?
// - bCtrolFrecuency. ¿Se desea controlar la frecuencia?
// - bCtrlPan. ¿Se desea controlar el balanceo del sonido?
// Devuelve:
// - Si se ha podido crear la instancia true. En caso contrario false.
// Notas:
// - Añadir controladores innecesarios consumira mas recursos del sistema.
// - El metodo considerara siempre la creacion de un sonido en hardware.
///////////////////////////////////////////////////////////////////////////////
ResDefs::WAVSoundHandle
CResourceManager::WAVSoundRegister(const std::string& szWAVFileName,
								   const bool bCtrlVolume,
								   const bool bCtrlFrequency,
								   const bool bCtrlPan)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(!szWAVFileName.empty());

  // Se crea parametro basico
  dword udFlagsParams = 0;
  if (bCtrlVolume) {
	udFlagsParams |= DSBCAPS_CTRLVOLUME;
  }
  if (bCtrlFrequency) {
	udFlagsParams |= DSBCAPS_CTRLFREQUENCY;
  }
  if (bCtrlPan) {
	udFlagsParams |= DSBCAPS_CTRLPAN;
  }

  // Se obtiene nodo nodo del buffer de sonido asociado al nombre del fichero
  // WAV recibido. De no existir, se creara (pero no se insertara en map)
  sNWAVBufferInfo* pWAVBufferInfo = NULL;
  std::string szWAVFileNameLowercase(szWAVFileName);
  SYSEngine::MakeLowercase(szWAVFileNameLowercase);
  const WAVBufferInfoMapIt BufferInfoIt(m_WAVBufferInfoBank.find(szWAVFileNameLowercase));
  if (BufferInfoIt == m_WAVBufferInfoBank.end()) {
	// No existe, luego se crea nodo
	pWAVBufferInfo = new sNWAVBufferInfo;	
  } else {
	// Si, se toma nodo
	pWAVBufferInfo = BufferInfoIt->second;
  }
  ASSERT(pWAVBufferInfo);

  // Se establece array con las posibles localizaciones para el buffer
  const dword WAVBufferLocation[2] = {
	DSBCAPS_LOCHARDWARE, DSBCAPS_LOCSOFTWARE
  };

  // Instancia al subsistema de audio
  iCAudioSystem* const pAudioSys = SYSEngine::GetAudioSystem();
  ASSERT(pAudioSys);

  // Se procede a crear el sonido WAV    
  byte ubIt = 0;
  for (; ubIt < 2; ++ubIt) {
	// ¿Existe buffer con los mismos parametros?
	// Nota: el parametro de localizacion de creacion, sera el que indique ubIt
	sNWAVBufferInstance* pWAVBufferInstance = NULL;
	const dword udActFlagsParams = udFlagsParams | WAVBufferLocation[ubIt];
	const WAVBufferInstancesMapIt InstanceIt(pWAVBufferInfo->Instances.find(udActFlagsParams));
	if (InstanceIt == pWAVBufferInfo->Instances.end()) {
	  // No, se crea y se intenta inicializar instancia WAVSoundBuffer
	  pWAVBufferInstance = new sNWAVBufferInstance;	  
	  ASSERT(pWAVBufferInstance);	  
	  if (!pWAVBufferInstance->WAVBuffer.Create(pAudioSys->GetDXSound(),
												szWAVFileNameLowercase,
												udActFlagsParams)) {
		// Problemas, se libera nodo y se continua en sig. iteracion
		delete pWAVBufferInstance;
		continue;
	  }
	} else {
	  // Si, se toma nodo
	  pWAVBufferInstance = InstanceIt->second;
	}

	// Una vez que tenemos la instancia WAVBuffer, debemos de crear la
	// instancia WAVSound, para ello, creamos el nodo y la intentamos
	// inicializar a partir de la primera
	sNWAVSound* const pWAVSound = new sNWAVSound;
	ASSERT(pWAVSound);
	ASSERT(pWAVBufferInstance);	
	if (!pWAVSound->WAVSound.Create(pAudioSys->GetDXSound(),
								    &pWAVBufferInstance->WAVBuffer)) {
	  // Hay problemas, debemos de liberar instancia WAV sound y WAVBuffer SOLO
	  // si esta ultima no se hallo en el map, luego continuaremos iteracion
	  delete pWAVSound;
	  // Nota: En la modificacion al Wrapper, esta llamada Destroy seria
	  // importante que se hiciera automaticamente en el destructor.
	  pWAVBufferInstance->WAVBuffer.Destroy();
	  //
	  if (InstanceIt == pWAVBufferInfo->Instances.end()) {		
		delete pWAVBufferInstance;
	  }	  
	  continue;
	}

	// Se pudo crear sonido
	
	// Insertamos la instancia WAVBuffer en el map si procede, estableciendo
	// el valor del contador de referencias.
	if (InstanceIt == pWAVBufferInfo->Instances.end()) {
	  pWAVBufferInstance->uwRefs = 1;
	  pWAVBufferInfo->Instances.insert(WAVBufferInstancesMapValType(udActFlagsParams, pWAVBufferInstance));
	} else {
	  ++pWAVBufferInstance->uwRefs;
	}

	// Si el nodo con la informacion sobre instancias WAVBuffer asociadas al
	// fichero recibido se creo, tambien se insertara en el map.
	if (InstanceIt == pWAVBufferInfo->Instances.end()) {
	  m_WAVBufferInfoBank.insert(WAVBufferInfoMapValType(szWAVFileNameLowercase, pWAVBufferInfo));
	}	

	// Por ultimo se registra el sonido WAVSound guardando la instancia al 
	// WAVBuffer a partir del que fue creado, asi como la propia al nodo de info.
	const ResDefs::WAVSoundHandle hWAVSound = CreateWAVSoundHandle();
	ASSERT(hWAVSound);
	m_WAVSoundBank.insert(WAVSoundMapValType(hWAVSound, pWAVSound));	
	pWAVSound->WAVBufferInstanceIt = pWAVBufferInfo->Instances.find(udActFlagsParams);
	ASSERT((pWAVSound->WAVBufferInstanceIt != pWAVBufferInfo->Instances.end()) != 0);
	pWAVSound->WAVBufferInfoIt = m_WAVBufferInfoBank.find(szWAVFileNameLowercase);
	ASSERT((pWAVSound->WAVBufferInfoIt != m_WAVBufferInfoBank.end()) != 0);
	return hWAVSound;	
  }

  // En este punto, es seguro que no se pudo crear el sonido, se eliminara 
  // el nodo con la informacion sobre WAVBuffers asociados al archivo WAV 
  // recibido, si este nodo fue creado en el metodo, y se retorna con handle 0.
  if (BufferInfoIt == m_WAVBufferInfoBank.end()) {	
	delete pWAVBufferInfo;
  }
  return 0;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el recurso de sonido DXDSWAVSound y lo devuelve.
// Parametros:
// - hWAVSound. Handle al fichero de sonido.
// Devuelve:
// - Direccion del recurso DXDSWAVSound. En caso de no hallarlo devolvera
//   NULL.
// Notas:
///////////////////////////////////////////////////////////////////////////////
DXDSWAVSound* 
const CResourceManager::GetWAVSound(const ResDefs::WAVSoundHandle& hWAVSound)
{
  // SOLO si modulo inicializado
  ASSERT(IsInitOk());
  // SOLO si ID valido
  ASSERT(hWAVSound);

  // Obtiene nodo
  const WAVSoundMapIt It(m_WAVSoundBank.find(hWAVSound));  
  return (It != m_WAVSoundBank.end()) ? &It->second->WAVSound : NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el nombre asociado a un recurso de sonido WAV.
// Parametros:
// - hWAVSound. Handle.
// Devuelve:
// - El nombre del fichero WAV asociado al recurso WAV cuyo handle es hWAVSound.
// Notas:
///////////////////////////////////////////////////////////////////////////////
std::string 
CResourceManager::GetWAVSoundFileName(const ResDefs::WAVSoundHandle& hWAVSound)
{
  // SOLO si modulo inicializado
  ASSERT(IsInitOk());
  // SOLO si ID valido
  ASSERT(hWAVSound);

  // Obtiene nodo y retorna nombre
  const WAVSoundMapIt It(m_WAVSoundBank.find(hWAVSound));  
  ASSERT((It != m_WAVSoundBank.end()) != 0);
  return It->second->WAVBufferInfoIt->first;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera el sonido hWAVSound. Antes de liberar el nodo, se decrementara
//   el contador de referencias del buffer de sonido, si el contador queda a
//   cero, se liberara la entrada al buffer y si dicha entrada era la unica,
//   se liberara a su vez el nodo con la instancia.
// Parametros:
// - hWAVSound. Handle al sonido WAV
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CResourceManager::ReleaseWAVSound(const ResDefs::WAVSoundHandle& hWAVSound)
{
  // SOLO si modulo inicializado
  ASSERT(IsInitOk());
  // SOLO si ID valido
  ASSERT(hWAVSound);

  // Obtiene nodo y decrementa contador de refs. de buffer asociado
  const WAVSoundMapIt WAVSoundIt(m_WAVSoundBank.find(hWAVSound));  
  ASSERT((WAVSoundIt != m_WAVSoundBank.end()) != 0);  
  --WAVSoundIt->second->WAVBufferInstanceIt->second->uwRefs;

  // ¿No hay mas referencias a la instancia del buffer?
  if (0 == WAVSoundIt->second->WAVBufferInstanceIt->second->uwRefs) {
	// Se libera el nodo a la instancia del buffer
	WAVSoundIt->second->WAVBufferInstanceIt->second->WAVBuffer.Destroy();
	delete WAVSoundIt->second->WAVBufferInstanceIt->second;
	WAVSoundIt->second->WAVBufferInfoIt->second->Instances.erase(WAVSoundIt->second->WAVBufferInstanceIt);

	// ¿Era la ultima instancia del nodo de informacion de WAVBuffers?
	if (WAVSoundIt->second->WAVBufferInfoIt->second->Instances.empty()) {
	  // Si, luego se libera a su vez dicho nodo
	  delete WAVSoundIt->second->WAVBufferInfoIt->second;
	  m_WAVBufferInfoBank.erase(WAVSoundIt->second->WAVBufferInfoIt);
	}
  }

  // Se libera el nodo con la instancia WAVSound
  WAVSoundIt->second->WAVSound.Destroy();
  delete WAVSoundIt->second;
  m_WAVSoundBank.erase(WAVSoundIt);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Restaura los buffers de sonido, DXDSWAVBuffer, y los sonidos WAV, 
//   instancias DXDSWAVSound. Ademas lo realizara en ese orden.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CResourceManager::RestoreWAVSounds(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Restaura buffers de sonido
  WAVBufferInfoMapIt BufferInfoIt(m_WAVBufferInfoBank.begin());
  for (; BufferInfoIt != m_WAVBufferInfoBank.end(); ++BufferInfoIt) {
	// Recorre todos los buffers asociados al nodo de informacion
	WAVBufferInstancesMapIt InstanceIt(BufferInfoIt->second->Instances.begin());
	for (; InstanceIt != BufferInfoIt->second->Instances.end(); ++InstanceIt) {
	  InstanceIt->second->WAVBuffer.Restore();
	}
  }

  // Restaura WAVSounds
  WAVSoundMapIt WAVSoundIt(m_WAVSoundBank.begin());
  for (; WAVSoundIt != m_WAVSoundBank.end(); ++WAVSoundIt) {
	WAVSoundIt->second->WAVSound.Restore();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera el map de WAVSound, destruyendo a su vez insancias WAVSound.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CResourceManager::EndWAVSoundBank(void)
{  
  // Procede a eliminar todos los WAVSound
  WAVSoundMapIt WAVSoundIt(m_WAVSoundBank.begin());
  for (; WAVSoundIt != m_WAVSoundBank.end(); WAVSoundIt = m_WAVSoundBank.erase(WAVSoundIt)) {
	WAVSoundIt->second->WAVSound.Destroy();
	delete WAVSoundIt->second;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera el map de WAVBufferInfo, destruyendo a su vez insancias WAVBuffer.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CResourceManager::EndWAVBufferInfoBank(void)
{
  // Procede a liberar todos los nodos de informacion
  WAVBufferInfoMapIt BufferInfoIt(m_WAVBufferInfoBank.begin());
  for (; BufferInfoIt != m_WAVBufferInfoBank.end(); BufferInfoIt = m_WAVBufferInfoBank.erase(BufferInfoIt)) {
	// Recorre todos los buffers asociados al nodo de informacion, borrandolos
	WAVBufferInstancesMapIt InstanceIt(BufferInfoIt->second->Instances.begin());
	for (; InstanceIt != BufferInfoIt->second->Instances.end(); InstanceIt = BufferInfoIt->second->Instances.erase(InstanceIt)) {
	  InstanceIt->second->WAVBuffer.Destroy();
	  delete InstanceIt->second;
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo interfaz para la restauracion de recursos. Internamente llama
//   a los metodos encargados de restaurar texturas y buffers de sonido.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void CResourceManager::RestoreResources(void)
{
  // SOLO si modulo inicializado
  ASSERT(IsInitOk());

  // Restaura recursos de sonido
  RestoreWAVSounds();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Genera un nuevo handle para texturas
// Parametros:
// Devuelve:
// - El nuevo identificador creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
ResDefs::TextureHandle 
CResourceManager::CreateTextureHandle(void)
{
  // // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Genera el handle  
  ResDefs::TextureHandle hHandle = 0;
  m_pMathUtil->GenSeed();
  do {
	hHandle = m_pMathUtil->GetRandValue(1, 0x7FFF);
  } while (m_TextureBank.find(hHandle) != m_TextureBank.end());

  // Se devuelve
  return hHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Genera un nuevo handle para sonidos WAV
// Parametros:
// Devuelve:
// - El nuevo identificador creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
ResDefs::WAVSoundHandle 
CResourceManager::CreateWAVSoundHandle(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Genera el handle  
  ResDefs::WAVSoundHandle hWAVSound = 0;
  m_pMathUtil->GenSeed();
  do {
	hWAVSound = m_pMathUtil->GetRandValue(1, 0x7FFF);
  } while (m_WAVSoundBank.find(hWAVSound) != m_WAVSoundBank.end());

  // Se devuelve
  return hWAVSound;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea una instancia CAnimTemplateData asociada a la plantilla szAnimTemplate
//   en caso de que no lo estuviera y asocia a la misma la instancia logica
//   de plantilla de animacion CAnimTemplate.
// Parametros:
// - szAnimTemplate. Nombre de la plantilla de animacion.
// - pAnimTemplate. Parte logica de la plantilla de animacion a la que asociar
//   los datos que se registren.
// Devuelve:
// - La instancia CAnimTemplateData a traves de su interfaz.
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCAnimTemplateData* const
CResourceManager::RegisterAnimTemplateData(const std::string& szAnimTemplate,
										   CAnimTemplate* const pAnimTemplate)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(!szAnimTemplate.empty());
  ASSERT(pAnimTemplate);

  // ¿Esta registrada la plantilla?
  std::string szATemplateLowercase(szAnimTemplate);
  SYSEngine::MakeLowercase(szATemplateLowercase);    
  const AnimTemplateDataMapIt It(m_ATemplateDataBank.find(szATemplateLowercase));
  sNAnimTemplateData* pNAnimTemplateData;
  if (It == m_ATemplateDataBank.end()) {
	// No esta creado, se crea el nodo
	pNAnimTemplateData = new sNAnimTemplateData;	
	ASSERT(pNAnimTemplateData);
	
	// Se inicializa instancia
	pNAnimTemplateData->ATemplateData.Init(szAnimTemplate);
	ASSERT(pNAnimTemplateData->ATemplateData.IsInitOk());		

	// Se registra
	m_ATemplateDataBank.insert(AnimTemplateDataMapValType(szATemplateLowercase,
														  pNAnimTemplateData));
  } else {
	// Si esta creado, se toma la instancia directamente
	pNAnimTemplateData = It->second;
  }

  // Se incrementa contador de referencias y añade instancia CAnimTemplate  
  ASSERT(pNAnimTemplateData);    
  ASSERT(pNAnimTemplateData->ATemplateData.IsInitOk());
  (pNAnimTemplateData->udRefs)++;
  pNAnimTemplateData->ATemplateData.AddClient(pAnimTemplate);

  // Se retorna instancia
  return &pNAnimTemplateData->ATemplateData;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desvincula el cliente pAnimTemplate de la instancia para el mantenimiento 
//   de datos asociados a la plantilla de animacion szAnimTemplate y reduce
//   en una unidad el contador de refs. Si este llega a 0, se liberara el nodo
//   que lo mantiene.
// Parametros:
// - szAnimTemplate. Nombre de la plantilla de animacion.
// - pAnimTemplate. Instancia a la plantilla de animacion (logica)
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CResourceManager::ReleaseAnimTemplateData(const std::string& szAnimTemplate,
										  CAnimTemplate* const pAnimTemplate)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pAnimTemplate);
  ASSERT(!szAnimTemplate.empty());

  // Se obtiene nodo y se desvincula cliente y decrementan referencias
  std::string szATemplateLowercase(szAnimTemplate);
  SYSEngine::MakeLowercase(szATemplateLowercase);      
  AnimTemplateDataMapIt It(m_ATemplateDataBank.find(szATemplateLowercase));
  ASSERT((It != m_ATemplateDataBank.end()) != 0);
  It->second->ATemplateData.RemoveClient(pAnimTemplate);
  --It->second->udRefs;

  // ¿Ya no hay clientes asociados?
  if (0 == It->second->udRefs) {
	// Si, se finaliza instancia CAnimTemplateData y se libera el nodo
	It->second->ATemplateData.End();
	delete It->second;
	m_ATemplateDataBank.erase(It);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera todos los recursos de tipo AnimTemplateData. Este metodo en teoria, 
//   no deberia de hallar informacion alguna ya que de lo contrario significara
//   que no ha habido una liberacion correcta en los clientes.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CResourceManager::EndAnimTemplateDataBank(void)
{
  // Libera todos los recursos de tipo textura
  AnimTemplateDataMapIt It(m_ATemplateDataBank.begin());
  for (; It != m_ATemplateDataBank.end(); It = m_ATemplateDataBank.erase(It)) {
	delete It->second;
  }
}

