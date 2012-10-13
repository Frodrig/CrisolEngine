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
// CFileSystem.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CFileSystem.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CFileSystem.h"

#include "SYSEngine.h"   
#include "iCLogger.h"    
#include "iCMathUtil.h"
#include <fstream>
#include <algorithm>
#include <io.h>

// Inicializacion de la unica instancia singlenton
CFileSystem* CFileSystem::m_pFileSystem = NULL;

// Declaracion de estructuras forward
struct CFileSystem::sFileNode { 
  // Enumerados
  enum eFILE_TYPE { 
    // Tipo de fichero
    CPAK_FILE = 1, 
    DISK_FILE      
  };

  // Datos
  eFILE_TYPE  FileType;    // Tipo de fichero	
  std::string szFileName;  // Nombre del fichero
  word        uwInstances; // Numero de instancias al fichero
  union {
	sCPAKFile*    pCPAKFile; // Puntero a CPAK con el fichero en cuestion
	std::fstream* pDiskFile; // Puntero a objeto fstream al fichero en cuestion
  };
  // Constructor por defecto
  sFileNode(const std::string& aszFileName,
			sCPAKFile* const apCPAKFile): szFileName(aszFileName),
										  FileType(CPAK_FILE), 
										  pCPAKFile(apCPAKFile),
										  uwInstances(1) { }
  sFileNode(const std::string& aszFileName,
		    std::fstream* const apDiskFile): szFileName(aszFileName),
										     FileType(DISK_FILE), 
										     pDiskFile(apDiskFile),
											 uwInstances(1) { }
  // Pool de memoria
  static CMemoryPool MPool;
  static void* operator new(const size_t size) { return MPool.AllocMem(size); }
  static void operator delete(void* pItem)  { MPool.FreeMem(pItem); } 
};

struct CFileSystem::sCPAKFile {
  // Datos asociados a un fichero CPAK
  CCPAKFile File; // Fichero CPAK.
  // Constructor
  sCPAKFile(void) { }
  // Pool de memoria
  static CMemoryPool MPool;
  static void* operator new(const size_t size) { return MPool.AllocMem(size); }
  static void operator delete(void* pItem)  { MPool.FreeMem(pItem); } 
};
  
// Inicializacion del manejador de memoria
CMemoryPool CFileSystem::sFileNode::MPool(16,
                                          sizeof(CFileSystem::sFileNode), 
                                          true);
CMemoryPool CFileSystem::sCPAKFile::MPool(16,
                                          sizeof(CFileSystem::sCPAKFile), 
                                          true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia.
// Parametros:
// Devuelve:
// - true: Si todo ha tenido exito. Y false en caso contrario.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CFileSystem::Init(void)
{
  // ¿Esta la clase ya inicializada?
  if (IsInitOk()) { return false; }

  #ifdef ENGINE_TRACE  
     SYSEngine::GetLogger()->Write("CFileSystem::Init> Inicializando subsistema de ficheros.\n");        
  #endif

  // Se toman instancias a otros subsistemas
  m_pMathUtil = SYSEngine::GetMathUtil();
  ASSERT(m_pMathUtil);

  // Se inicializan resto de vbles de miembro
  m_udNumFilesOpen = 0;
  m_bInitOk = true; 

  // Todo correcto
  #ifdef ENGINE_TRACE  
     SYSEngine::GetLogger()->Write("                 | Ok.\n");        
  #endif
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia.
// Parametros:
// Devuelve:
// Notas:
// - Para realizar el cierre de todos los ficheros se llamara a CloseAll. Al
//   cerrar todos los ficheros no se tendra en cuenta que puedan existir
//   aun identificadores en el exterior.
///////////////////////////////////////////////////////////////////////////////
void 
CFileSystem::End(void)
{
  if (IsInitOk()) { 
	#ifdef ENGINE_TRACE  
       SYSEngine::GetLogger()->Write("CFileSystem::End> Finalizando subsistema de ficheros.\n");        
	#endif

	// Se cierran, quitan y liberan archivos
    CloseAll();   
    QuitAllCPAKFiles();
    
    // Se baja el flag de inicializacion
    #ifdef ENGINE_TRACE  
       SYSEngine::GetLogger()->Write("                | Ok.\n");        
	#endif
    m_bInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Cierra todos los ficheros que se encuentren abiertos y libera todos los
//   nodos del arbol.
// Parametros:
// Devuelve:
// - El numero de ficheros liberados
// Notas:
///////////////////////////////////////////////////////////////////////////////
dword 
CFileSystem::CloseAll(void)
{
  // SOLO si la clase esta correctamente inicializada
  ASSERT(IsInitOk());

  // Num. de ficheros liberados
  const dword udFilesClosed = m_udNumFilesOpen; 

  // Se itera procede a cerrar ficheros y eliminar nodos    
  while (!m_FileIndex.empty()) {
	// Se obtiene identificador al primer nodo y se cierra fichero
	const FileDefs::FileHandle hFile = m_FileIndex.begin()->first;
	ASSERT(hFile);
	Close(hFile);
  }

  // Se devuelve el num. de ficheros cerrados.  
  return udFilesClosed;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Añade un fichero CPAK. Los ficheros CPAK se alojaran en una lista. No 
//   podran existir dos CPAK iguales instalados simultaneamente.
// Parametros:
// - szFileName. Nombre del fichero CPAK.
// Devuelve:
// - Si se ha asociado el fichero CPAK se devolvera true. En caso de que haya
//   existido algun problema, se devolvera false.
// Notas:
// - En caso de querer asociar un fichero CPAK que ya existiera, se devolvera
//   true.
///////////////////////////////////////////////////////////////////////////////
bool 
CFileSystem::AddCPAKFile(const std::string& szFileName)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(szFileName.size());

  // Se establece el nombre del CPAK en minusculas
  std::string szFileNameLowercase(szFileName);
  SYSEngine::MakeLowercase(szFileNameLowercase);

  // Busca a traves de la lista por si existe el fichero CPAK ya instalado
  CPAKListIt CPAKIt(m_CPAKFiles.begin());
  for (; CPAKIt != m_CPAKFiles.end(); ++CPAKIt) {
	// ¿El fichero ya esta instalado?
	if (0 == szFileNameLowercase.compare((*CPAKIt)->File.GetCPAKFileName())) {
	  // Si, luego se retorna sin instalar logicamente
	  return true;
	}
  }

  // En este punto, el fichero no estara instalado
  // Se crea nuevo nodo
  sCPAKFile* pCPAKFile = new sCPAKFile;
  ASSERT(pCPAKFile);

  // Se intenta abrir el fichero CPAK (en modo lectura)
  if (!pCPAKFile->File.Open(szFileNameLowercase, false)) {
	// No se pudo, se retorna
	delete pCPAKFile;
	return false;
  }

  // Se inserta en la lista y se retorna
  m_CPAKFiles.push_back(pCPAKFile);
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina todas las instancias a los ficheros CPAK existentes.
// Parametros:
// Devuelve:
// Notas:
// - Este metodo puede ser muy peligroso si se llama desde un punto en el que
//   no se este realizando una finalizacion de la instancia.
///////////////////////////////////////////////////////////////////////////////
void 
CFileSystem::QuitAllCPAKFiles(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se recorrere map eliminando el contenido de las listas y los propios nodos
  CPAKListIt CPAKIt(m_CPAKFiles.begin());
  for (; CPAKIt != m_CPAKFiles.end(); CPAKIt = m_CPAKFiles.erase(CPAKIt)) {
	// Se borra instancia CPAKFile
	delete *CPAKIt;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Abre un archivo especificado. 
// - El fichero se puede abrir exijiendo que exista o bien abrir 
//   sobreescribiendo cualquier fichero que puediera existir.
// - Los ficheros seran alojados en un arbol. En el caso de que el fichero se
//   halle embebido en un CPAK, se guardara la instancia a dicho CPAK. 
// Parametros:
// - szFileName: El nombre del fichero en cuestion.
// - bCreate: El flag que indica si el fichero se debe de crear al abrirlo
//   o bien debe de existir para poder abrirlo.
// Devuelve:
// - El identificador asociado al fichero abierto. Es importante tener en
//   cuenta que en caso de que la operacion no se lleve a cabo por algun
//   motivo, se devolvera un valor de 0 para el identificador.
// Notas:
// - RECORDAR: Devolver 0 significara que ha existido un error.
///////////////////////////////////////////////////////////////////////////////
FileDefs::FileHandle 
CFileSystem::Open(const std::string& szFileName,
				  const bool bCreate)
{  
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(!szFileName.empty());
  
  // Se busca por nombre
  const CFileNameSearch<FileDefs::FileHandle, sFileNode*> Predicate(szFileName);
  const FileIndexMapIt It(std::find_if(m_FileIndex.begin(),
									   m_FileIndex.end(),
									   Predicate));

  // ¿Fichero ya registrado?
  if (It != m_FileIndex.end()) { 
	// ¿Se pretendia crear?
    if (bCreate) { 
	  // No se dejara
	  return 0;
    } else { 
	  // Incrementa contador de referencias y retorna            
      ++It->second->uwInstances;      
      return It->first;
    }
  }

  // No se encontro, por lo que se abrira 
  // ¿Se desea abrir sobreescribiendo?
  sFileNode* pFileNode = NULL;
  if (bCreate) {
    // Se abre el nuevo fichero sobreescribiendolo si ya existia
	std::fstream* pDiskFile = new std::fstream;
    ASSERT(pDiskFile);	
    pDiskFile->open(szFileName.c_str(), 
				    std::ios_base::in | 
					std::ios_base::out | 
					std::ios_base::trunc | 
					std::ios_base::binary);
    if (pDiskFile->fail()) { 
	  // Problemas
      delete pDiskFile;
	  return 0;
    }
    
    // Se crea nodo
    pFileNode = new sFileNode(szFileName, pDiskFile);
    ASSERT(pFileNode);
  } else {
	// Se abre sin sobreescribir, luego el fichero ha de existir.
    // Se intenta en disco	
	std::fstream* pDiskFile = new std::fstream;
    ASSERT(pDiskFile);		
    pDiskFile->open(szFileName.c_str(), 
                    std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    if (pDiskFile->fail()) { 
	  // No se hallo, se borra el nodo a disco
      delete pDiskFile;	  	  
	  pDiskFile = NULL;

	  // Se intenta en los CPAKs instalados
      sCPAKFile* const pCPAKFile = OpenInCPAKFile(szFileName);
	  if (!pCPAKFile) { 
		// Problemas		
		return 0;
      } else {
		// Se abrio en CPAK
		pFileNode = new sFileNode(szFileName, pCPAKFile);
		ASSERT(pFileNode);
	  }
    } else {	  
	  // Se abrio en disco	  
	  pFileNode = new sFileNode(szFileName, pDiskFile);
	  ASSERT(pFileNode);
	}
  }   

  // Se inserta en map
  const FileDefs::FileHandle hNewFile = CreateNewID();
  ASSERT(hNewFile);
  m_FileIndex.insert(FileIndexMapValType(hNewFile, pFileNode));

  // Se incrementa el num. fichero abiertos
  m_udNumFilesOpen++;

  // Se sale
  return hNewFile;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de leer la informacion del fichero indicado en el identificador.
//   La informacion leida estara delimitada por el tamaño del buffer. En caso
//   de que la cantidad a leer sea superior al propio tamaño del fichero, 
//   se acotara la cantidad a leer al tamaño de este.
// Parametros:
// - hFile: Identificador del archivo desde donde leer.
// - pubBuffer: Puntero a la direccion del memoria en donde alojar los datos
//   a leer.
// - udBuffSize: Tamaño del buffer donde alojar los datos.
// - udInitOffset. Offset en donde comenzar a leer la informacion.
// Devuelve:
// - La cantidad de informacion leida.
// Notas:
// - La operacion de lectura estara permitida en todo tipo de fichero, tanto
//   de disco como CPAK.
// - La lectura siempre se realizara desde el comienzo del fichero.
// - Se considerara error fatal, via ASSERT, el que el identificador pasado
//   no este registrado en el arbol.
///////////////////////////////////////////////////////////////////////////////
dword CFileSystem::Read(const FileDefs::FileHandle& hFile, 
                        sbyte* const psbBuffer, 
						const dword udBuffSize,
                        const dword udInitOffset)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si los datos referentes al buffer son correctos
  ASSERT(psbBuffer);
  ASSERT(udBuffSize);

  // Vbles  
  dword udDataRead = 0; // Cantidad de informacion leida
  
  // Se obtiene nodo al fichero
  const FileIndexMapIt It(m_FileIndex.find(hFile));
  ASSERT((It != m_FileIndex.end()) != 0);

  // Se comprueba el tipo de fichero y dependiendo del mismo,
  // se realiza la lectura
  switch (It->second->FileType) {
	case CFileSystem::sFileNode::DISK_FILE: {
	  // Fichero fstream
      // Se ajusta correctamente la cantidad de tamaños a leer
      ASSERT(It->second->pDiskFile);
	  It->second->pDiskFile->clear();
      It->second->pDiskFile->seekg(0, std::ios_base::end);
      udDataRead = udBuffSize;
      const dword udFileSize = It->second->pDiskFile->tellg();
      
	  // ¿Se intenta leer mas informacion que la que realmente hay?
	  ASSERT((udInitOffset < udFileSize) != 0);
      if (udDataRead > udFileSize - udInitOffset) { 
        // Hay que acotar la cantidad posible a leer
        udDataRead = udFileSize - udInitOffset;
      }
      
	  // Se procede a leer la informacion      
	  It->second->pDiskFile->clear();
      It->second->pDiskFile->seekg(udInitOffset, std::ios_base::beg);
      It->second->pDiskFile->read(psbBuffer, udDataRead);
	} break;
					
    case CFileSystem::sFileNode::CPAK_FILE: {
	  // Fichero CPAK
      // La cantidad de datos leidos sera ajustada automaticamente 
      // por el fichero CPAK
      ASSERT(It->second->pCPAKFile);
	  udDataRead = It->second->pCPAKFile->File.Read(It->second->szFileName,
													psbBuffer,
													udBuffSize,
													udInitOffset);	  
	} break;
  }

  // Se devuelve la cantidad de bytes leidos
  return udDataRead;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo preparado para leer una linea a partir de un offset dado. Se
//   considerara linea a la secuencia que termine en un retorno de carro '\r'
//   seguido del caracter de nueva linea '\n'. Como se leera HASTA el caracter
//   '\r' el de nueva linea no sera incluido, pero el numero real de caracteres
//   leidos incluira a estos dos ultimos.
// - En caso de que se lea la ultima linea, no existira caracter de retorno
//   de carro ni de nueva linea, y la longitud sera unicamente la del numero de
//   caracteres que tenga el string.
// Parametros:
// - hFile: Identificador del archivo desde donde leer.
// - udInitOffset. Offset en donde comenzar a leer la informacion.
// - szDest. String donde depositar la informacion leida.
// Devuelve:
// - El offset que se ha movido para leer
// Notas:
///////////////////////////////////////////////////////////////////////////////
dword
CFileSystem::ReadLine(const FileDefs::FileHandle& hFile, 
					  const dword udInitOffset,
					  std::string& szDest)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hFile);

  // Se obtiene nodo al fichero
  const FileIndexMapIt It(m_FileIndex.find(hFile));
  ASSERT((It != m_FileIndex.end()) != 0);

  // Se comprueba el tipo de fichero y dependiendo del mismo,
  // se realiza la lectura
  switch (It->second->FileType) {
	case CFileSystem::sFileNode::DISK_FILE: {
	  // Fichero fstream
	  // Se coloca el offset
	  ASSERT(It->second->pDiskFile);
	  #ifdef _SYSASSERT
		//It->second->pDiskFile->clear();
		//It->second->pDiskFile->seekg(0, std::ios_base::end);
		ASSERT((udInitOffset <= It->second->pDiskFile->tellg()) != 0);
	  #endif	  
	  It->second->pDiskFile->clear();
	  It->second->pDiskFile->seekg(udInitOffset, std::ios_base::beg);	  

	  // Se leera una nueva linea, como antes de llegar a la nueva linea podra
	  // haberse incluido el retorno de carro, este sera eliminado del string
	  std::getline(*It->second->pDiskFile, szDest);	
	  if (!szDest.empty() && 
		  '\r' == szDest[szDest.size() - 1]) {
		szDest.resize(szDest.size() - 1);
	  }	  

	  // Se retorna el offset avanzado por el archivo
	  return (word(It->second->pDiskFile->tellg()) - udInitOffset);
	} break;
					
    case CFileSystem::sFileNode::CPAK_FILE: {
	  // Fichero CPAK
      ASSERT(It->second->pCPAKFile);
	  ASSERT((udInitOffset <= It->second->pCPAKFile->File.GetFileSize(It->second->szFileName)) != 0);
      return It->second->pCPAKFile->File.ReadLine(It->second->szFileName,		
										          udInitOffset,
										          szDest);
	} break;
  }
  
  // No se ha leido nada
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Lee desde un archivo en binario una cadena de caracteres. Siempre seguiran
//   el formato:
//   * Tamaño de la cadena (word)
//   * Caracteres que forman la cadena (un sbyte por cada uno)
// Parametros:
// - hFile. Handle al fichero.
// - udOffset. Offset de donde comenzar a leer.
// - szDestStr. Referencia al string donde depositar los datos
// Devuelve:
// - Cantidad de datos leidos del archivo.
// Notas:
// - Siempre se liberara el contenido de la cadena recibida
///////////////////////////////////////////////////////////////////////////////
dword 
CFileSystem::ReadStringFromBinary(const FileDefs::FileHandle& hFile,
								  const dword udOffset,
								  std::string& szDestStr)
{
  // Lee el tamaño de la cadena
  word uwStrSize;
  word udDataReaded = Read(hFile, (sbyte *)(&uwStrSize), sizeof(word), udOffset);
  
  // ¿Hay cadena asociada?  
  szDestStr = "";
  if (uwStrSize > 0) {
	// Se reserva espacio y procede a leer byte a byte	
	szDestStr.reserve(uwStrSize);
	word uwIt = 0;
	for (; uwIt < uwStrSize; ++uwIt) {
	  sbyte sbChar;
	  udDataReaded += Read(hFile, &sbChar, sizeof(sbyte), udOffset + udDataReaded);
	  szDestStr += sbChar;
	}
  }

  // Retorna
  return udDataReaded;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo destinado a escribir un string en un archivo binario. Se seguira
//   el convenio siguiente:
//   * Tamaño del string en un word
//   * String
//   Cuando el tamaño sea 0, no habra campo destinado al string. Tampoco se
//   guardara el caracter nulo (\0).
// Parametros:
// - hFile. Handle al fichero.
// - udOffset. Offset desde donde proceder a escribir
// - szString. String a escribir.
// Devuelve:
// - La cantidad de bytes ocupados en la escritura (word + tamaño cadena)
// Notas:
///////////////////////////////////////////////////////////////////////////////
dword
CFileSystem::WriteStringInBinary(const FileDefs::FileHandle& hFile,
								 const dword udOffset,
								 const std::string& szString)
{
  // SOLO si parametros correctos
  ASSERT(hFile);
 
  // Se toma el tamaño del string y se escribe este y el string
  dword udNewOffset = udOffset;
  const word uwStrSize = szString.size();  
  udNewOffset += Write(hFile, udNewOffset, (sbyte *)(&uwStrSize), sizeof(word));
  udNewOffset += Write(hFile, udNewOffset, szString.c_str(), uwStrSize * sizeof(sbyte));

  // Retorna
  return udNewOffset - udOffset;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Escribe datos sobre el fichero cuyo identificador sea uwID. En principio
//   solo se permitiran escribir sobre aquellos ficheros que se encuentren
//   como individuales en disco. Los que esten embebidos en un CPAK seran solo
//   de lectura.
// - Al escribirse se esperara recibir un offset que se acotara a la longitud
//   del fichero si este, el offset, es mayor que el mismo. 
// Parametros:
// - hFile: Identificador del fichero en donde queramos escribir.
// - udOffset. Offset en donde realizar la escritura.
// - pubBuffer: Puntero con la direccion de memoria en donde se encuentre la
//   informacion que se quiera escribir a disco.
// - udBuffSize: Cantidad de informacion a escribir.
// Devuelve:
// - La cantidad de informacion escrita (por norma general igual a udBuffSize).
// Notas:
// - Solo se efecturan operaciones de escritura en ficheros de tipo DISK.
///////////////////////////////////////////////////////////////////////////////
dword
CFileSystem::Write(const FileDefs::FileHandle& hFile, 
				   const dword udOffset,
                   const sbyte* const psbBuffer, 
				   const dword udBuffSize)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si los datos referentes al buffer son correctos
  ASSERT(psbBuffer);

  // Se obtiene nodo al fichero
  const FileIndexMapIt It(m_FileIndex.find(hFile));
  ASSERT((It != m_FileIndex.end()) != 0);
  ASSERT((CFileSystem::sFileNode::CPAK_FILE != It->second->FileType) != 0);
  
  // Se procede a realizar la escritura, preparando antes el fichero
  ASSERT(It->second->pDiskFile);
  It->second->pDiskFile->seekp(udOffset, std::ios_base::beg);    
  It->second->pDiskFile->write(psbBuffer, udBuffSize);

  // Retorna
  return udBuffSize;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Cierra un fichero especificado. Los nodos con la informacion sobre el
//   fichero solo se destruiran si el numero de instancias que hagan referencias
//   al mismo es 0.
// Parametros:
// - hFile: Identificador del fichero en el arbol.
// Devuelve:
// Notas:
// - Se considerara error fatal, via ASSERT, el que el identificador pasado
//   no este registrado en el arbol.
///////////////////////////////////////////////////////////////////////////////
void CFileSystem::Close(const FileDefs::FileHandle& hFile)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hFile);
  ASSERT((m_FileIndex.find(hFile) != m_FileIndex.end()) != 0);

  // Se obtiene nodo al fichero
  const FileIndexMapIt It(m_FileIndex.find(hFile));
  ASSERT((It != m_FileIndex.end()) != 0);
 
  // Se decrementa el num. de instancias y se evalua el resultado
  --It->second->uwInstances;
  if (!It->second->uwInstances) { 
    switch (It->second->FileType) {
	  case CFileSystem::sFileNode::DISK_FILE: {
		// Fichero fstream    
		It->second->pDiskFile->clear();
		It->second->pDiskFile->close();
		delete It->second->pDiskFile;
	  } break; 
  
	  case CFileSystem::sFileNode::CPAK_FILE: { 
		// Fichero en CPAK
		// En este caso, realmente no habra que realizar ningun tipo de
		// cierre sobre el fichero, pues estara embebido en el CPAK.
		It->second->pCPAKFile = NULL;
	  } break;
	}; // ~ switch

	// Finalmente se libera instancia y nodo
	delete It->second;
	m_FileIndex.erase(It);
  };  

  // Antes de salir se decrementa el contador de fich. abiertos
  m_udNumFilesOpen--;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve el tamaño del fichero asociado al indentificador pasado.
// Parametros:
// - hFile: El identificador del fichero sobre el que se quiere obtener el
//   tamaño.
// Devuelve:
// - El tamaño del fichero.
// Notas:
// - Se considerara error fatal, via ASSERT, el que el identificador pasado
//   no este registrado en el arbol.
///////////////////////////////////////////////////////////////////////////////
dword CFileSystem::GetFileSize(const FileDefs::FileHandle& hFile)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hFile);

  // Se obtiene nodo al fichero
  const FileIndexMapIt It(m_FileIndex.find(hFile));
  ASSERT((It != m_FileIndex.end()) != 0);

  // Se obtiene el tamaño del fichero, atentiendo antes al tipo
  // de fichero sobre el que se desee hacer la operacion.
  switch (It->second->FileType) {
	case CFileSystem::sFileNode::DISK_FILE: {
	  // Fichero fstream    
	  ASSERT(It->second->pDiskFile);
	  It->second->pDiskFile->clear();
	  It->second->pDiskFile->seekg(0, std::ios_base::end);
	  return (dword)(It->second->pDiskFile->tellg());
	} break; 
  
	case CFileSystem::sFileNode::CPAK_FILE: { 
	  // Fichero en CPAK
	  ASSERT(It->second->pCPAKFile);
	  return It->second->pCPAKFile->File.GetFileSize(It->second->szFileName);
	} break;
  };

  // Hubo algun problema
  return 0;
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Busca entre los ficheros CPAK y abre el archivo especificado, devolviendo
//   ademas la instancia al CPAK.
// Parametros:
// - szFileName: Nombre del archivo.
// Devuelve:
// - En caso de encontrarse un CPAK asociado con dicho fichero, el puntero al
//   mismo. Si no se encuentra, se devolvera NULL.
// Notas:
///////////////////////////////////////////////////////////////////////////////
CFileSystem::sCPAKFile* const 
CFileSystem::OpenInCPAKFile(const std::string& szFileName)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(!szFileName.empty());

  // Se busca entre los ficheros CPAK instalados, aquel que embeba a szFileName
  CPAKListIt CPAKIt(m_CPAKFiles.begin());
  for(; CPAKIt != m_CPAKFiles.end(); ++CPAKIt) {
	if ((*CPAKIt)->File.IsFilePresent(szFileName)) {
	  return *CPAKIt;
	}
  }

  // No se encontro un fichero CPAK con el fichero szFileName
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Genera un nuevo identificador con la condicion de que no sea 0 y que
//   tampoco se encuentre ya registrado en el arbol
// Parametros:
// Devuelve:
// - El nuevo identificador creado
// Notas:
// - Para crear el identificador se utilizara un generador de numeros aleatorios
///////////////////////////////////////////////////////////////////////////////
dword 
CFileSystem::CreateNewID(void)
{
  // SOLO si la instancia esta correctamente inicializada
  ASSERT(IsInitOk());

  // Se obtiene un ID
  m_pMathUtil->GenSeed();
  FileDefs::FileHandle hFile = 0;
  do {     
    hFile = m_pMathUtil->GetRandValue(1, 0x7FFF);
  } while (!(m_FileIndex.find(hFile) == m_FileIndex.end()));

  // Se retorna el identificador
  ASSERT(hFile);
  return hFile;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Deposita en una lista de strings los nombres de los archivos que se hallan
//   en un path en concreto. El path debera de pasarse indicando los tipos de
//   ficheros que se quieren tomar (*.cbb), (*.*), etc.
// Parametros:
// - DestList. Referencia a la lista de strings donde depositar los nombres.
// - szPath. Path en donde buscar los archivos.
// Devuelve:
// Notas:
// - Se utilizaran funciones del API de Win32 para implementar este 
///////////////////////////////////////////////////////////////////////////////
void 
CFileSystem::GetFileNamesFromPath(const std::string& szPath,
								  std::list<std::string>& DestList)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
 
  // Se localizan los ficheros
  _finddata_t FileInfo; 
  const long hFile = _findfirst(szPath.c_str(), &FileInfo);
  if (-1L != hFile) {
	// Se insertan en lista los nombres de los archivos localizados	
	do {	  
	  DestList.push_back(FileInfo.name);
	} while((0 == _findnext(hFile, &FileInfo)));

	// Se cierra estructura
	_findclose(hFile);
  }
}

