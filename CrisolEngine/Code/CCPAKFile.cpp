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
// CCPAKFile.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CCPAKFile.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CCPAKFile.h"

#include "CMemoryPool.h"
#include <io.h>

// Declaracion de estructuras forward
struct CCPAKFile::sNFile { 
  // Entrada en la tabla de indices de fichero
  byte      ubFileNameSize; // Longitud del nombre del fichero
  dword     udFileSize;     // Tamaño del fichero
  dword     udFileOffset;   // Offset en donde en contrar el fichero
  FileState State;          // Contiene el estado del fichero	
  // Pool de memoria
  static CMemoryPool MPool;
  static void* operator new(const size_t size) { return MPool.AllocMem(size); }
  static void operator delete(void* pItem)  { MPool.FreeMem(pItem); } 
};

// Inicializacion del manejador de memoria
CMemoryPool CCPAKFile::sNFile::MPool(16, sizeof(CCPAKFile::sNFile), true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Abre el fichero szFileName y rellena las estructuras de datos internas
//   con los contenidos del mismo. El modo en que se realizara la operacion
//   de apertura lo indicara el flag bOverWrite que por defecto vale false.
//   Tambien realiza la apertura y preparacion del fichero temporal para poder
//   tenerlo listo en el momento de realizar cualquier tipo de modificacion,
//   a traves de m_FileTemp.
// Parametros:
// - szFileName: Nombre del fichero.
// - bOverWrite: Si vale true, el fichero CPAK se sobreescribira si existe y en
//   caso contrario, se creara un nuevo archivo. Si vale false, solo se tendra
//   exito en la operacion de apertura si el fichero existia previamente. Por
//   defecto este flag vale false.
// Devuelve:
// - true: Si se ha podido abrir el fichero con exito. En caso de que haya
//   existido algun problema, bOverWrite sea false y el fichero no exista o
//   bien ya este abierto.
// Notas:
// - No se permitira abrir cuando haya otro ya abierto.
///////////////////////////////////////////////////////////////////////////////
bool 
CCPAKFile::Open(const std::string& szFileName, 
				const bool bOverWrite)
{
  // SOLO si parametros correctos
  ASSERT(!szFileName.empty());

  // ¿Hay otro fichero abierto?
  if (IsOpen()) { 
	return false; 
  }

  // Flags base para apertura
  const sword swOpenFlags = std::ios_base::in | 
							std::ios_base::out | 
							std::ios_base::binary;

  // Se procede a abrir el fichero en modo lectura / escritura
  m_szFileName = szFileName;

  // ¿Se desea abrir sobreescribiendo?
  m_File.clear();
  if (bOverWrite) { 
	// Si, apertura sobreescribiendo
    m_File.open(szFileName.c_str(), swOpenFlags | std::ios_base::trunc);
  } else { 
	// No, apertura sin sobreescribir
    m_File.open(szFileName.c_str(), swOpenFlags);
  }

  // ¿Hubo problemas?  
  if (!m_File.good()) { 
	return false; 
  }

  // Se obtiene el tamaño del fichero
  m_File.seekg(0, std::ios_base::end);
  const dword udFileSize = m_File.tellg();

  // Se inicializa cabecera
  // Nota:, si el fichero tiene tamaño 0, significara que se querra crear.
  if (!InitHeader((udFileSize > 0) ? true : false)) {
    m_File.close();    
    return false;
  }
  
  // Se prepara el fichero temporal para operaciones de modificacion
  m_szFileNameTemp = ChangeExtension(szFileName, "tmp");    
  m_FileTemp.open(m_szFileNameTemp.c_str(), swOpenFlags | std::ios_base::trunc);    
  if (!m_FileTemp.good()) { 	
    m_File.close();
	ReleaseFileIndex();
    return false;
  }
  
  // Se actualizan resto de vbles de miembro y retorna
  m_bFileModify = false;  
  m_bFileOpen = true; 
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de cerrar el fichero. Tendra en cuenta si el fichero ha sufrido
//   algun tipo de modificacion y debe de actualizar su contenido antes de 
//   cerrarse.
// - Tambien comprobara si el fichero ha sido creado por primera vez y no se
//   han almacenado elementos.
// Parametros:
// Devuelve:
// Notas:
// - Este metodo se llama automaticamente desde el destructor.
///////////////////////////////////////////////////////////////////////////////
void 
CCPAKFile::Close(void)
{
  // ¿Esta el fichero abierto?
  if (m_bFileOpen) { 
	// Si, ¿ha existido alguna modificacion?
    if (m_bFileModify) { 
	  // Si, se actualiza
      UpdateChanges();
    } else if (0 == m_CPAKHeader.udNumFiles) { 
	  // No, y ademas no contiene elementos luego se guarda la cabecera solo.
      m_File.seekp(0);
      m_File.write((sbyte *)(&m_CPAKHeader), sizeof(m_CPAKHeader));          
    }

    // Se cierra fichero CPAK y el temporal se elimina
    m_File.close();
    m_FileTemp.close();
    remove(m_szFileNameTemp.c_str());
    
	// Se libera tabla de ficheros y se baja flag de apertura
	ReleaseFileIndex();
    m_bFileOpen = false;
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la cabecera del fichero CPAK que se esta abriendo. En caso de
//   que se reciba por parametro flag false se entendera que el fichero CPAK
//   no existe y hay que crear la cabecera manualmente. En caso de recibir
//   true, el fichero existira y se debera de leer la cabecera para comprobar
//   que es un fichero CPAK valido.
// Parametros:
// - bFromDisk: Si vale true quiere decir que se inicializa cabecera desde
//   disco. En caso contrario, que se inicializa manualmente (se esta creando
//   un fichero CPAK).
// Devuelve:
// - Si todo va bien, se devolvera true. Si ha existido algun problema false.
// Notas:
// - Este metodo sera llamado desde Open.
///////////////////////////////////////////////////////////////////////////////
bool 
CCPAKFile::InitHeader(const bool bFromDisk)
{
  // ¿Se inicializa manualmente (no desde disco) ?
  if (!bFromDisk) {
    // Se inicializa la cabecera con firma, version, flags, num. ficheros 
	// y offset de tabla de indices, luego se retorna
    memcpy(m_CPAKHeader.sbFirm, "CPAK", 4);  
    m_CPAKHeader.ubVersionHi = CCPAKFile::VERSIONHI;
    m_CPAKHeader.ubVersionLo = CCPAKFile::VERSIONLO;
    m_CPAKHeader.udFlags = 0; 
    m_CPAKHeader.udNumFiles = 0;
    m_CPAKHeader.udOffsetIndex = 0;
    return true;
  }

  // En este punto se quiere inicializar desde disco
  ASSERT(m_File.is_open());

  // El fichero existe; se procede a leer la informacion del mismo,
  // comprobar que es valido e inicializar las estructuras de datos
  m_File.clear();
  m_File.seekp(0, std::ios_base::beg);
  m_File.read((sbyte *)(&m_CPAKHeader), sizeof(m_CPAKHeader));
  if (m_File.fail()) { 
	return false;
  }

  // ¿Firma INCORRECTA? o ¿Version NO valida?
  if (!IsValidFirm(m_CPAKHeader.sbFirm) ||
	  !IsValidVersion(m_CPAKHeader.ubVersionHi, m_CPAKHeader.ubVersionLo)) {
	return false;
  }

  // Se inicializara la tabla de ficheros CPAK
  m_File.clear();
  m_File.seekp(m_CPAKHeader.udOffsetIndex);  
  sbyte szFileName[256];
  dword udIt = 0;
  for (; udIt < m_CPAKHeader.udNumFiles; ++udIt) {    
    // Se crea nuevo nodo
    sNFile* const pFile = new sNFile;
    ASSERT(pFile);

    // Se lee el nombre del fichero, su tamaño y el offset donde se encuentra
	m_File.clear();
    m_File.read((sbyte *)(&pFile->ubFileNameSize), sizeof(pFile->ubFileNameSize));
    m_File.read(szFileName, pFile->ubFileNameSize);
    m_File.read((sbyte *)(&pFile->udFileSize),  sizeof(pFile->udFileSize));
    m_File.read((sbyte *)(&pFile->udFileOffset), sizeof(pFile->udFileOffset)); 
    
	// Se establece estado
    pFile->State = CCPAKFile::FILE_OK;

	// Se registra
	std::string szFileNameLowercase(szFileName);
	std::transform(szFileNameLowercase.begin(), szFileNameLowercase.end(), szFileNameLowercase.begin(), tolower);
	m_FileIndex.insert(CPAKFileIndexMapValType(szFileNameLowercase, pFile));
  }
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera el map con la relacion de todos los archivos embebidos
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCPAKFile::ReleaseFileIndex(void)
{
  // Procede a liberar map de ficheros embebidos
  CPAKFileIndexMapIt CPAKIt(m_FileIndex.begin());
  for (; CPAKIt != m_FileIndex.end(); CPAKIt = m_FileIndex.erase(CPAKIt)) {
	delete CPAKIt->second;
  }
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - A partir del nombre de un fichero y de una extension, fabrica un nuevo
//   nombre de fichero.
// Parametros:
// - szFileName: Alias del nombre del fichero sobre el que se quiere crear
//   el nombre del fichero temporal.
// - szExtension del fichero
// Devuelve:
// - El string con el nuevo nombre.
// Notas:
///////////////////////////////////////////////////////////////////////////////
std::string 
CCPAKFile::ChangeExtension(const std::string& szFileName, 
						   const std::string& szExtension)
{
  // SOLO si la longitud del nombre recibido es valida
  ASSERT(!szFileName.empty());
  // SOLO si la extesion tiene logitud correcta
  ASSERT(!szExtension.empty());
  ASSERT((szExtension.size() <= 3) != 0);

  // Se copia el nombre del fichero y se obtiene su longitud
  std::string szFileNameTemp(szFileName);
  const word uwFileSize = szFileNameTemp.size();

  // Se itera hasta llegar al final o al punto de extension
  dword udIt = 0;
  while (udIt < uwFileSize && 
		 szFileNameTemp[udIt] != '.') { 
	++udIt;
  }
  
  // Se calcula el espacio sobrante
  // ¿NO hay punto de extension?
  if (udIt == uwFileSize) { 
	// No, luego se reserva para el y para el resto
    szFileNameTemp.resize(uwFileSize + 4);
    szFileNameTemp[udIt] = '.';
  } else { 
	// Si, se determina el resto de espacio
    szFileNameTemp.resize(uwFileSize + 3 - (uwFileSize - (udIt + 1)));
  }

  // Se pone la extension del fichero temporal
  byte ubIt = 0;
  for (; ubIt < szExtension.size(); ++ubIt) {
	const byte ubFileNamePos = szFileNameTemp.size() - (ubIt + 1);
	const byte ubExtensionPos = szExtension.size() - (ubIt + 1);
    szFileNameTemp[ubFileNamePos] = szExtension[ubExtensionPos];
  } 
  
  // Se retorna el nombre
  return szFileNameTemp;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encargara de hacer efectivos los posibles cambios que se hayan efectuado
//   en el fichero CPAK.
// Parametros:
// Devuelve:
// - Si se han realizado los cambios true. En caso de que no se hayan realizado
//   bien porque no habia nada que modificar (m_bFileModify a false) o bien
//   porque hubo un error, se devolvera false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCPAKFile::UpdateChanges(void)
{
  // SOLO si hay fichero abierto
  ASSERT(IsOpen());

  // ¿NO existe ninguna modificacion?
  if (!m_bFileModify) { 
	return false; 
  }

  // Se recorre mediante iteradores el indice de ficheros para:
  // * Encontrar el tamaño maximo del buffer
  // * Borrar aquellos ficheros con flag FILE_REMOVED
  dword udSizeBuffer = 0;
  CPAKFileIndexMapIt CPAKFileIt(m_FileIndex.begin());
  while (CPAKFileIt != m_FileIndex.end()) {
	// ¿El fichero actual TIENE que borrarse?
	if (CCPAKFile::FILE_REMOVED == CPAKFileIt->second->State) {
	  // Si, se elimina y se pasa a sig. iteracion
	  delete CPAKFileIt->second;
	  CPAKFileIt = m_FileIndex.erase(CPAKFileIt);
	} else {
	  // No, ¿el tamaño es mayor que valor actual de buffer?
	  if (CPAKFileIt->second->udFileSize > udSizeBuffer) {
		// Si, se toma
		udSizeBuffer = CPAKFileIt->second->udFileSize;
	  }
	  ++CPAKFileIt;
	}
  }

  // Flags base para la nueva apartura de los ficheros
  const sword swBaseFlags = std::ios_base::in | std::ios_base::out | 
							std::ios_base::binary;	

  // ¿No quedan ficheros?
  if (m_FileIndex.empty()) {
	// No quedan, luego se inicializan estructuras y archivos
	// Se truncan ficheros temporales y actuales
	m_File.close();
	m_File.clear();
	m_File.open(m_szFileName.c_str(), swBaseFlags | std::ios_base::trunc);
	ASSERT(m_File.good());
	m_FileTemp.close();	
	m_FileTemp.clear();
	m_FileTemp.open(m_szFileNameTemp.c_str(), swBaseFlags | std::ios_base::trunc);
	ASSERT(m_FileTemp.good());

    // Se actualiza la cabecera y se guarda en disco
    m_CPAKHeader.udNumFiles = 0;
    m_CPAKHeader.udOffsetIndex = 0;
	m_File.clear();
	m_File.seekp(0, std::ios_base::beg);
    m_File.write((sbyte *)(&m_CPAKHeader), sizeof(m_CPAKHeader));
  } else {
	// Si hay ficheros contenidos, se procede a una actualizacion a nivel
	// de disco (FILE_MODIFY)	
	// Se abre el fichero CPAK actualizado	
	std::string szFileNameTemp(ChangeExtension(m_szFileName, "act"));
	std::fstream NewFile(szFileNameTemp.c_str(), 
						 swBaseFlags | std::ios_base::trunc);
	ASSERT(NewFile.good());
	
	// Se prepara buffer temporal para lectura de informacion  
	byte* pubBuffer = new byte [udSizeBuffer];
	ASSERT(pubBuffer);  
	
	// Se procede a guardar los archivos en NewFile, dejando espacio para 
	// la cabecera, que se guardara en ultimo lugar
	NewFile.seekp(sizeof(m_CPAKHeader));
	CPAKFileIndexMapIt CPAKFileIt(m_FileIndex.begin());
	for (; CPAKFileIt != m_FileIndex.end(); ++CPAKFileIt) {
	  // Se evalua estado del fichero actual	
	  switch(CPAKFileIt->second->State) {
		case CCPAKFile::FILE_OK: {
		  // Hay que guardar el fichero tal cual 
		  // Se lee desde el CPAK
		  m_File.clear();
		  m_File.seekg(CPAKFileIt->second->udFileOffset);
		  m_File.read((sbyte *)(pubBuffer), CPAKFileIt->second->udFileSize);
		} break;

		case CCPAKFile::FILE_MODIFY: {
		  // Es un fichero modificado 
		  // Se lee desde el temporal y se deja en estado "Ok"
		  m_FileTemp.clear();
		  m_FileTemp.seekg(CPAKFileIt->second->udFileOffset);
		  m_FileTemp.read((sbyte *)(pubBuffer), CPAKFileIt->second->udFileSize);      
		  CPAKFileIt->second->State = CCPAKFile::FILE_OK;
		} break;
	  }; // ~ switch

	  // Se actualiza los datos del fichero y se guarda la informacion
	  CPAKFileIt->second->udFileOffset = NewFile.tellp();    
	  NewFile.write((sbyte *)(pubBuffer), CPAKFileIt->second->udFileSize);
	}

	// Se libera el buffer y se cierran y borran los ficheros originales
	delete[udSizeBuffer] pubBuffer;
	pubBuffer = NULL;
	m_File.close();
	remove(m_szFileName.c_str());
	m_FileTemp.close();
	remove(m_szFileNameTemp.c_str());	    
	
	// Se completa los datos de la cabecera con el offset para la tabla
	// de indices y el numero de elementos y se guarda la cabecera  
	m_CPAKHeader.udOffsetIndex = NewFile.tellp();
	m_CPAKHeader.udNumFiles = m_FileIndex.size();
	NewFile.clear();
	NewFile.seekp(0, std::ios_base::beg);  
	NewFile.write((sbyte *)(&m_CPAKHeader), sizeof(m_CPAKHeader));
	
	// Se procede a almacenar la informacion sobre el indice de ficheros, que
	// ira siempre al final del todo
	NewFile.clear();
	NewFile.seekp(0, std::ios_base::end);  
	CPAKFileIt = m_FileIndex.begin();
	for (; CPAKFileIt != m_FileIndex.end(); ++CPAKFileIt) {
	  // Se guardara: longitud nombre del fichero, nombre fichero, tamaño del mismo y
	  // offset en donde localizarlo.
	  NewFile.clear();
	  NewFile.write((sbyte *)(&CPAKFileIt->second->ubFileNameSize), 
					sizeof(CPAKFileIt->second->ubFileNameSize));
	  NewFile.write(CPAKFileIt->first.c_str(), 
					CPAKFileIt->second->ubFileNameSize);
	  NewFile.write((sbyte *)(&CPAKFileIt->second->udFileSize), 
					sizeof(CPAKFileIt->second->udFileSize));        
	  NewFile.write((sbyte *)(&CPAKFileIt->second->udFileOffset), 
					sizeof(CPAKFileIt->second->udFileOffset));
	}
	
	// Se procede a cerrar NewFile y renombrarlo al nombre del fichero original    
	NewFile.close();
	szFileNameTemp = ChangeExtension(m_szFileName.c_str(), "act");
	rename(szFileNameTemp.c_str(), m_szFileName.c_str());
	
	// Se vuelve a abrir el fichero CPAK y el temporal para trabajar
	m_File.open(m_szFileName.c_str(), swBaseFlags);
	m_FileTemp.open(m_szFileNameTemp.c_str(), swBaseFlags | std::ios_base::trunc);
  }

  // Se establece a off el flag de modificaciones y se sale
  m_bFileModify = false;  
  return true;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la lectura del contenido asociado a un fichero. La lectura se
//   realiza en base al numero de bytes que se quieran leer. El buffer debera 
//   de haber sido reservado desde el exterior.
// Parametros:
// - szFileName: Nombre del fichero.
// - psbBuffer: Puntero al buffer de memoria.
// - udBufferSize: Tamaño del buffer.
// - udInitOffset. Posicion inicial de lectura. Por defecto valdra 0 indicando
//   que se quiere leer desde el principio.
// Devuelve:
// - El numbero de bytes leidos. Si se devuelve 0 es que ha existido algun
//   problema o no se ha hallado el fichero.
// Notas:
// - En caso de que el tamaño del fichero sea inferior a udBufferSize, se
//   leeran solos los bytes asociados al tamaño del fichero.
// - En caso de que la posicion inicial este fuera del rango del tamaño
//   del fichero, no se leera nada y se devolvera 0.
///////////////////////////////////////////////////////////////////////////////
dword 
CCPAKFile::Read(const std::string& szFileName, 
				sbyte* psbBuffer, 
				const dword udBufferSize,
				const dword udInitOffset)
{
  // SOLO si hay fichero abierto
  ASSERT(IsOpen());
  // SOLO si parametros correctos
  ASSERT(psbBuffer);
  ASSERT(!szFileName.empty());
  ASSERT(udBufferSize);

  // Se localiza nodo
  const CPAKFileIndexMapIt CPAKFileIt(GetCPAKFileIt(szFileName));

  // ¿Se encontro el fichero?
  if (CPAKFileIt != m_FileIndex.end()) {
	// Si, se acota la posicion de lectura en caso de que se quiera leer mas
	// de lo que realmente hay desde la posicion de inicio de lectura
	ASSERT((CPAKFileIt->second->udFileSize > udInitOffset) != 0);
	const dword udValidZoneToRead = CPAKFileIt->second->udFileSize - udInitOffset;
	dword udDataRead = udBufferSize;	
	if (udDataRead > udValidZoneToRead) {
	  udDataRead = udValidZoneToRead;
	}

	// Se lee y se retorna la cantidad de bytes leidos
	m_File.clear();
	m_File.seekp(CPAKFileIt->second->udFileOffset + udInitOffset, std::ios::beg);  
	m_File.read(psbBuffer, udDataRead);
	return m_File.fail() ? 0 : udDataRead;
  } else {
	// No, leidos 0 bytes
	return 0;
  }  
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
CCPAKFile::ReadLine(const std::string& szFileName, 
				    const dword udInitOffset,
				    std::string& szDest)
{
  // SOLO si hay fichero abierto
  ASSERT(IsOpen());
  // SOLO si parametros validos
  ASSERT(!szFileName.empty());

  // Se localiza nodo
  const CPAKFileIndexMapIt CPAKFileIt(GetCPAKFileIt(szFileName));

  // ¿Fichero valido?
  if (CPAKFileIt != m_FileIndex.end()) {
	// Si, se coloca cursor de lectura
	m_File.clear();
	m_File.seekg(CPAKFileIt->second->udFileOffset + udInitOffset,
				 std::ios_base::beg);  

	// Se leera una nueva linea, como antes de llegar a la nueva linea podra
	// haberse incluido el retorno de carro, este sera eliminado del string
	std::getline(m_File, szDest);
	if (!szDest.empty() && 
		'\r' == szDest[szDest.size() - 1]) {
	  szDest.resize(szDest.size() - 1);
	}	  
	
	// Se retorna el offset avanzado por el archivo
	return (word(m_File.tellg()) - (CPAKFileIt->second->udFileOffset + udInitOffset));
	//return (0 != m_File.eof()) ? szDest.size() + 2 : szDest.size();
  } else {
	// No, se leyeron 0 bytes
	return 0;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este metodo sobreescribe los datos de un fichero que se encuentre 
//   almacenado en el archivo CPAK. Como no se puede sobreescribir directamente,
//   se utilizara un fichero temporal en donde depositar los datos. Una llamada
//   a UpdateChanges se encargara de hacer efectivos dichos cambios. En el 
//   momento que un fichero es modificado no se podra trabajar con el hasta una
//   llamada a UpdateChanges.
// Parametros:
// - szFileName: Nombre del fichero sobre el que modificar los datos.
// - psbBuffer: Buffer con los nuevos datos del fichero.
// - udBufferSize: Tamaño del buffer.
// Devuelve:
// - Si se ha podido realizar la accion se devolvera true y false en caso
//   contrario.
// Notas:
// - Se podra hacer write siempre y cuando el fichero en cuestion NO este en
//   estado FILE_REMOVED.
///////////////////////////////////////////////////////////////////////////////
bool
CCPAKFile::Write(const std::string& szFileName, 
				 const sbyte* const psbBuffer,
				 const dword udBufferSize)
{
  // SOLO si hay fichero abierto
  ASSERT(IsOpen());
  // SOLO si parametros correctos
  ASSERT(psbBuffer);
  ASSERT(!szFileName.empty());
  ASSERT(udBufferSize);

  // Se localiza nodo
  const CPAKFileIndexMapIt CPAKFileIt(GetCPAKFileIt(szFileName));

  // ¿El fichero existe y NO esta pendiente de ser borrado?
  if (CPAKFileIt != m_FileIndex.end() &&
	  CCPAKFile::FILE_REMOVED == CPAKFileIt->second->State) {
	// Se procede a guardar la nueva informacion en el fichero temporal  
	m_FileTemp.clear();
	m_FileTemp.seekp(0, std::ios_base::end);
	const dword udFTempOffset = m_FileTemp.tellp();
	m_FileTemp.write(psbBuffer, udBufferSize);

	// ¿Todo correcto?
	if (m_FileTemp.good()) {
	  // Si, se cambia atributos y el flag de modificacion y se retorna
	  CPAKFileIt->second->State = FILE_MODIFY;
	  CPAKFileIt->second->udFileOffset = udFTempOffset;
	  CPAKFileIt->second->udFileSize = udBufferSize;  
	  m_bFileModify = true;
	  return true;
	} else {
	  // No, hubo problemas
	  return false;
	}	
  } else {
	// Se abandona sin poder realizar la operacion
	return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Marca el fichero recibido como borrado, de tal forma que en el siguiente
//   UpdateChanges, se descartara del archivo CPAK.
// Parametros:
// - szFileName: Nombre del fichero a borrar
// Devuelve:
// - Si se ha localizado y borrado el fichero, se devuelve true y false en caso
//   de que haya existido algun problema.
// Notas:
// - Este metodo no borrara en el acto el fichero, sino que lo marcara de tal
//   forma que en la siguiente llamada a UpdateChanges, se efecturan los
//   cambios introducidos.
///////////////////////////////////////////////////////////////////////////////
bool 
CCPAKFile::Remove(const std::string& szFileName)
{
  // SOLO si hay fichero abierto
  ASSERT(IsOpen());
  // SOLO si parametros validos
  ASSERT(!szFileName.empty());

  // Se localiza nodo
  const CPAKFileIndexMapIt CPAKFileIt(GetCPAKFileIt(szFileName));

  // ¿Hay fichero asociado?
  if (CPAKFileIt != m_FileIndex.end()) {
	// Si, se marca como borrado, establece flag de modificacion y retorna
	CPAKFileIt->second->State = CCPAKFile::FILE_REMOVED;
	m_bFileModify = true;
	return true;
  } else {
	// No, luego no se puede borrar
	return false;
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este metodo abrira un fichero de disco, leera su contenido y lo 
//   incorporara al archivo CPAK. En lugar de realizar la incorporacion en el
//   acto, incorporara los datos del mismo al fichero tempora m_FileTemp.
//   Tan solo se podra añadir un fichero si este existe y no esta registrado
//   en el CPAK o bien esta registrado pero el fichero se encuentra en estado
//   REMOVED.
// Parametros:
// - szFileName: Nombre del fichero a incorporar.
// - bRemoveFromDisk: Si vale true, el fichero que se ha añadido sera eliminado.
// Devuelve:
// - Si se ha podido añadir devuelve true y si ha existido algun problema
//   false.
// Notas:
// - Recordar; en caso de que el fichero exista solo se podra realizar la
//   operacion si este se encuentra en estado de eliminacion: FILE_REMOVED.
//   En cualquier otro caso no se permitira la operacion, por tanto, no se
//   permitira mediante esta operacion sobreescribir ficheros.
///////////////////////////////////////////////////////////////////////////////
bool 
CCPAKFile::Add(const std::string& szFileName, 
			   const bool bRemoveFromDisk)
{
  // SOLO si hay fichero abierto
  ASSERT(IsOpen());
  // SOLO si parametros validos
  ASSERT(!szFileName.empty());

  // Se localiza nodo
  const CPAKFileIndexMapIt CPAKFileIt(GetCPAKFileIt(szFileName));

  // ¿Se encontro?
  if (CPAKFileIt != m_FileIndex.end()) {
	// Si, ¿Esta en estado de eliminacion?
	if (CCPAKFile::FILE_REMOVED == CPAKFileIt->second->State) {
	  // Si, luego se abandona sin realizar operacion
	  return false;
	}	
  }

  // No se encontro, luego se abre fichero para lectura  
  std::fstream NewFile(szFileName.c_str(), 
					   std::ios_base::in | std::ios_base::binary);
  if (!NewFile.good()) { 
	return false; 
  }

  // Se obtiene el tamaño del buffer para leer el fichero
  // Nota: En caso de que el fichero tenga tamaño 0, se abandonara
  NewFile.seekg(0, std::ios_base::end);
  const dword udSizeBuffer = NewFile.tellg();
  if (0 == udSizeBuffer) {
	return false;
  }

  // Se prepara espacio necesario para realizar la lectura de datos y se cierra
  byte* pubBuffer = new byte[udSizeBuffer];
  ASSERT(pubBuffer);
  NewFile.clear();
  NewFile.seekg(0, std::ios_base::beg);
  NewFile.read((sbyte *)(pubBuffer), udSizeBuffer);  
  NewFile.close();
  
  // Se procede a escribir en el temporal, siempre por el final
  m_FileTemp.clear();
  m_FileTemp.seekp(0, std::ios_base::end);
  const dword udFTempOffset = dword(m_FileTemp.tellp());  
  m_FileTemp.write((sbyte *)(pubBuffer), udSizeBuffer); 
  ASSERT(m_FileTemp.fail());  

  // Se libera buffer
  delete[udSizeBuffer] pubBuffer;
  pubBuffer = NULL;

  // Se toma el nodo asociado al fichero
  // ¿Estaba registrado el fichero?
  sNFile* pFileNode = NULL;
  if (CPAKFileIt != m_FileIndex.end()) {		
	// Si, se tomara la direccion del nodo del iterador
	pFileNode = CPAKFileIt->second;
	ASSERT(pFileNode);
  } else {
	// No, luego se creara nodo y se insertara en el indice de ficheros
	pFileNode = new sNFile;
	ASSERT(pFileNode);	
	std::string szFileNameLowercase(szFileName);
	std::transform(szFileNameLowercase.begin(), szFileNameLowercase.end(), szFileNameLowercase.begin(), tolower);
	m_FileIndex.insert(CPAKFileIndexMapValType(szFileNameLowercase, pFileNode));
  }

  // Se procede a actualizar atributos del fichero
  pFileNode->State = FILE_MODIFY;
  pFileNode->ubFileNameSize = szFileName.size() + 1; // donde +1 = '\0'
  pFileNode->udFileOffset = udFTempOffset;
  pFileNode->udFileSize = udSizeBuffer;

  // ¿Hay que eliminar el fichero añadido?
  if (bRemoveFromDisk) { 
	// Se elimina szFileName
    remove(szFileName.c_str());
  }  
    
  // Se activa flag de modificacion y se retorna  
  m_bFileModify = true;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Extrae un fichero mantenido a disco. En caso de que se pase el flag de
//   borrado a true, se eliminara tambien de fichero.
// - En caso de que el fichero se haya añadido desde un directorio, este
//   sera creado.
// Parametros:
// - szFileName: Nombre del fichero a extraer.
// - bRemoveFromPak: Flag de borrado. Si true, el fichero extraido 
//   tambien se eliminara del CPAK. Por defecto vale false.
// Devuelve:
// - Si la operacion sale bien true y false en caso contrario.
// Notas:
// - La operacion de extraccion sera instantanea, mientras que la de borrado
//   no.
// - La operacion (tanto extraccion y el posible borrad) siempre y cuando el
//   fichero este en estado OK, de lo contrario se devolvera tambien false.
///////////////////////////////////////////////////////////////////////////////
bool 
CCPAKFile::Extract(const std::string& szFileName, 
				   const bool bRemoveFromPak)
{
  // SOLO si hay fichero abierto
  ASSERT(IsOpen());
  // SOLO si parametros correctos
  ASSERT(!szFileName.empty());

  // Se localiza nodo  
  const CPAKFileIndexMapIt CPAKFileIt(GetCPAKFileIt(szFileName));

  // ¿Se encontro fichero?
  if (CPAKFileIt != m_FileIndex.end()) {
	// Si, ¿el estado no es "Ok"?
	if (FILE_OK != CPAKFileIt->second->State) {
	  // No lo es, se abandona
	  return false;
	}
  } else {
	// No, se abandona directamente
	return false;
  }

  // Se busca el directorio de donde cuelga el archivo y en caso de no
  // ser la raiz, sera creado para guardar ahi el archivo.
  word uwDirItBeg = 0;
  word uwDirItEnd = 0;
  std::string szPath;
  for (; uwDirItEnd < CPAKFileIt->first.size(); ++uwDirItEnd) {
	if (CPAKFileIt->first[uwDirItEnd] == '\\') {	  
	  szPath += szFileName.substr(uwDirItBeg, uwDirItEnd - uwDirItBeg) + '\\';
	  CreateDirectory(szPath.c_str(), NULL);
	  uwDirItBeg = uwDirItEnd + 1;
	}
  }

  // Se crea el buffer donde alojar la informacion
  sbyte* pubBuffer = new sbyte[CPAKFileIt->second->udFileSize];
  ASSERT(pubBuffer);

  // Se del fichero la informacion
  m_File.clear();
  m_File.seekp(CPAKFileIt->second->udFileOffset, std::ios::beg);  
  m_File.read((sbyte *)(pubBuffer), CPAKFileIt->second->udFileSize);
  
  // Se crea el nuevo fichero y se vuelca la informacion leida
  std::fstream FileCreated(szFileName.c_str(), 
						   std::ios_base::out | std::ios_base::trunc | 
						   std::ios_base::binary);
  ASSERT(FileCreated.good());
  FileCreated.seekp(0, std::ios_base::beg);
  FileCreated.write((sbyte *)(pubBuffer), CPAKFileIt->second->udFileSize);
  
  // ¿Una vez volcado el fichero, hay que eliminar el fuente?
  if (bRemoveFromPak) { 
	// Si, se marca como eliminado
    CPAKFileIt->second->State = FILE_REMOVED;
  } 

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Añade ficheros en virtud de su extension, añadiendose todos los ficheros
//   que se encuentren con dicha extension. 
// - Este metodo se limitara a naventar entre los ficheros, dejando la 
//   verdadera responsabilidad al metodo Add.
// Parametros:
// - szDirectory. Directorio.
// - szExtension. Extension.
// Devuelve:
// - El numero de ficheros añadidos.
// Notas:
///////////////////////////////////////////////////////////////////////////////
dword 
CCPAKFile::AddFilesByExtension(const std::string& szDirectory,
							   const std::string& szExtension)
{
  // SOLO si CPAK abierto
  ASSERT(IsOpen());
  // SOLO si parametros validos
  ASSERT(!szExtension.empty());

  // Se forma el camino de busqueda
  std::string szFiles(szDirectory);
  szFiles += "*.";
  szFiles += szExtension.c_str();

  // Se localiza el primer fichero
  _finddata_t FileInfo; // Estructura con la info. del fichero localizado
  dword udFilesInserted = 0;
  const long hFile = _findfirst(szFiles.c_str(), &FileInfo);
  if (-1L != hFile) {
	// Si se localizo el fichero, se procede a registrar y a iniciar
	// el resgistro del resto de ficheros.
	// Nota: se añadiran sin borrarse de disco	
	do {
	  // Se añade fichero y se incrementa contador si procede
	  // Nota: el nombre del fichero debera de tener como prefijo el directorio
	  if (Add(szDirectory + FileInfo.name, false)) {
		++udFilesInserted;
	  }
	} while((0 == _findnext(hFile, &FileInfo)));

	// Se cierra estructura
	_findclose(hFile);
  }

  // Se retorna el numero de ficheros añadido
  return udFilesInserted;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Extrae todos los ficheros que se encuentren embebidos en el CPAK. Para
//   ello, bastara con hacer un recorrido y llamar al metodo Extract.
// - Una vez que se hayan borrado todos, se llamara a UpdateChanges.
// Parametros:
// - bRemoveFromCPAK. ¿Se desea eliminar a los archivos del CPAK?
// Devuelve:
// - La cantidad de ficheros extraidos
// Notas:
///////////////////////////////////////////////////////////////////////////////
dword
CCPAKFile::ExtractAll(const bool bRemoveFromCPAK)
{
  // SOLO si hay fichero abierto
  ASSERT(IsOpen());

  // Se procede a recorrer el indice de ficheros extrayendolos
  dword udFilesExtracted = 0;
  CPAKFileIndexMapIt CPAKFileIt(m_FileIndex.begin());
  for (; CPAKFileIt != m_FileIndex.end(); ++CPAKFileIt) {
	if (Extract(CPAKFileIt->first, bRemoveFromCPAK)) {
	  ++udFilesExtracted;
	}
  }

  // Actualiza cambios y retorna la cantidad de ficheros extraidos
  UpdateChanges();
  return udFilesExtracted;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realizara un borrado de todos los ficheros embebidos en el CPAK, para
//   ello se apoyara en el metodo Remove.
// - Una vez realizada la operacion, se realiza la actualizacion mediante
//   UpdateChanges.
// Parametros:
// Devuelve:
// - El numero de ficheros borrados. Si se devuelve 0 significara que no
//   se ha borrado ninguno o que ha existido algun error.
// Notas:
// - El borrado no sera algo instantaneo, sino que se activara despues de una
//   llamada a UpdateChanges.
// - Se realizara borrado sobre todo los ficheros, tanto si estan en estado
//   OK como si estan en estado de Modificacion.
///////////////////////////////////////////////////////////////////////////////
dword 
CCPAKFile::RemoveAll(void)
{
  // SOLO si hay fichero abierto
  ASSERT(IsOpen());

  // Se procede a recorrer el indice de ficheros extrayendolos
  dword udFilesRemoved = 0;
  CPAKFileIndexMapIt CPAKFileIt(m_FileIndex.begin());
  for (; CPAKFileIt != m_FileIndex.end(); ++CPAKFileIt) {
	if (Remove(CPAKFileIt->first)) {
	  ++udFilesRemoved;
	}
  }

  // Actualiza cambios y retorna la cantidad de ficheros borrados
  UpdateChanges();
  return udFilesRemoved;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el tamaño del fichero embebido en el CPAK, siempre y cuando este
//   exista y este en estado OK.
// Parametros:
// - szFileName: Nombre del fichero
// Devuelve:
// - El tamaño del fichero. En caso de no existir o de existir algun error
//   devolvera 0.
// Notas:
// - Se entendera que todos aquellos ficheros en estado de modificacion y
//   en estado de borrado no pueden ser accesibles, por lo que se tomaran
//   como ficheros inexistentes.
///////////////////////////////////////////////////////////////////////////////
dword 
CCPAKFile::GetFileSize(const std::string& szFileName)
{
  // SOLO si hay fichero abierto
  ASSERT(IsOpen());
  // SOLO si parametros validos
  ASSERT(!szFileName.empty());

  // Se localiza nodo
  const CPAKFileIndexMapIt CPAKFileIt(GetCPAKFileIt(szFileName));

  // ¿Se encontro fichero Y esta en estado Ok?
  if (CPAKFileIt != m_FileIndex.end() &&
	  FILE_OK == CPAKFileIt->second->State) {
	// Si, se retorna tamaño
	return CPAKFileIt->second->udFileSize;
  } else {
	// No, se devuelve tamaño nulo
	return 0;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve true si el fichero pasado existe y ademas esta en estado OK.
// Parametros:
// - szFileName: Nombre del fichero a comprobar.
// Devuelve:
// - Si existe true y false en caso contrario.
// Notas:
// - Los ficheros con flag Removed seran considerados como NO existentes.
///////////////////////////////////////////////////////////////////////////////
bool 
CCPAKFile::IsFilePresent(const std::string& szFileName)
{
  // SOLO si hay fichero abierto
  ASSERT(IsOpen());
  // SOLO si parametros validos
  ASSERT(!szFileName.empty());

  // Se localiza nodo
  const CPAKFileIndexMapIt CPAKFileIt(GetCPAKFileIt(szFileName));

  // ¿Existe nodo y ademas NO esta en estado de borrado?
  if (CPAKFileIt != m_FileIndex.end() &&
	  FILE_REMOVED != CPAKFileIt->second->State) {
	// Si, se entendera que existe	
	return true;
  } else {
	// No, se entendera que no existe	
	return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Deposita en la lista recibida, los nombres de los archivos que se hallan
//   en el CPAK.
// Parametros:
// - StrList. Lista donde depositar los nombres de los archivos.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCPAKFile::GetFileList(std::list<std::string>& StrList)
{
  // SOLO si intancia inicializada
  ASSERT(IsOpen());

  // Recorre el map y procede a depositar los nombres de archivo
  CPAKFileIndexMapIt CPAKFileIt(m_FileIndex.begin());
  for (; CPAKFileIt != m_FileIndex.end(); ++CPAKFileIt) {
	// ¿El fichero actual NO ESTA pendiente de ser borrado?
	if (CCPAKFile::FILE_REMOVED != CPAKFileIt->second->State) {
	  StrList.push_back(CPAKFileIt->first);
	}
  }
}
