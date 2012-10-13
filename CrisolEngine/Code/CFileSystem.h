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
// CFileSystem.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CFileSystem
//
// Descripcion:
// - Este sera el modulo encargado de coordinar el trabajo con los ficheros, 
//   ofreciendo una interfaz transparente para el trato con archivos embebidos
//   en CPAKs y archivos individuales.
//   Las operaciones de escritura con los ficheros solo podran realizarse si
//   estos no son CPAK.
//
// Notas:
// - La filosofia de funcionamiento sera la siguiente; cuando se solicite un
//   fichero mediante Open, primero se comprobara si dicho fichero existe 
//   individualmente en disco. En caso de que no exista en disco, se pasara
//   a buscarlo en los ficheros CPAK. 
///////////////////////////////////////////////////////////////////////////////
#ifndef _CFILESYSTEM_H_
#define _CFILESYSTEM_H_

// Cabeceras
#ifndef _ICFILESYSTEM_H_
#include "iCFileSystem.h"
#endif
#ifndef _CCPACKFILE_H_
#include "CCPAKFile.h"
#endif
#ifndef _FSTREAM_H_
#define _FSTREAM_H_   
#include <fstream> 
#endif
#ifndef _LIST_H_      
#define _LIST_H_
#include <list> 
#endif  
#ifndef _CMEMORYPOOL_H_
#include "CMemoryPool.h"
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif     

// Defincion de clases / estructuras / espacios de nombres
struct iCMathUtil;

// Clase CFileSystem
class CFileSystem: public iCFileSystem
{
private:
  // Constantes
  const word MAX_IDHANDLE_VALUE; // Valor maximo de un identificador

private:
  // Estructuras forward
  struct sCPAKFile;

private:
  // Tipos
  // Lista de CPAKs instalados en sistema
  typedef std::list<sCPAKFile*> CPAKList;  
  typedef CPAKList::iterator    CPAKListIt;
  
private:  
  // Estructuras forward
  struct sFileNode;
  
private:
  // Tipos
  // Map para trabajar con los handles asociados a los ficheros
  typedef std::map<FileDefs::FileHandle, sFileNode*> FileIndexMap;
  typedef FileIndexMap::iterator                     FileIndexMapIt;
  typedef FileIndexMap::value_type                   FileIndexMapValType;

private:
  // Predicado para la busqueda de ficheros por el nombre (valor)  
  template<class First, class Second>
  class CFileNameSearch {
  private:
	// Vbles de miembro
	std::string m_szFileName; // Nombre del fichero a localizar

  public:
	// Constructor
	CFileNameSearch(const std::string& szFileName): m_szFileName(szFileName) { }

  public:
	// Predicado de busqueda
	bool operator() (std::pair<const First, Second>& Node) {
	  return (0 == strcmp(m_szFileName.c_str(), Node.second->szFileName.c_str()));
	}  
  }; // ~ MapValueSearch

 
private:
  // Instancia Singlenton
  static CFileSystem* m_pFileSystem; // Unica instancia a la clase
  
  // Instancias a otros subsistemas
  iCMathUtil* m_pMathUtil; // Utilidades matematicas

  // Estructuras con archivos
  FileIndexMap     m_FileIndex; // Map con los handles a ficheros abiertos
  CPAKList         m_CPAKFiles; // Ficheros CPAK Instalados

  // Resto de vbles de miembro      
  dword m_udNumFilesOpen; // Numero de ficheros abiertos
  bool  m_bInitOk;        // ¿Clase inicializada correctamente?
   
protected:
  // Constructor / Destructor
  CFileSystem(void): MAX_IDHANDLE_VALUE(0x7FFF),
					 m_pMathUtil(NULL),
					 m_bInitOk(false) { }
public:
  ~CFileSystem(void) { 
    End(); 
  }
  
public:
  // Protocolo de inicio y fin de instancia
  bool Init(void);
  void End(void);
  inline bool IsInitOk(void) const { return m_bInitOk; }
 
public:
   // Obtencion y destruccion de la instancia singlenton
  static inline CFileSystem* const GetInstance(void) {
    if (NULL == m_pFileSystem) { 
      // Se crea la instancia
      m_pFileSystem = new CFileSystem; 
      ASSERT(m_pFileSystem)
    }
    return m_pFileSystem;
  }
  static inline void DestroyInstance(void) {
    if (m_pFileSystem) {
      delete m_pFileSystem;
      m_pFileSystem = NULL;
    }
  }

public:
  // Operaciones sobre ficheros
  dword CloseAll(void);
  inline dword GetNumFilesOpen(void) const { 
	ASSERT(IsInitOk());
	// Retorna la cantidad de ficheros abiertos
	return m_udNumFilesOpen; 
  }
  
public:
  // Operaciones sobre ficheros CPAK
  bool AddCPAKFile(const std::string& szFileName);
  void QuitAllCPAKFiles(void);
private:
  // Metodos de apoyo
  sCPAKFile* const OpenInCPAKFile(const std::string& szFileName);  

public:
  // iCFileSystem / Operaciones de apertura / cierre
  FileDefs::FileHandle Open(const std::string& szFileName,
 						    const bool bCreate = false);
  void Close(const FileDefs::FileHandle& hFile);  

public:
  // iCFileSystem / Operaciones de lectura / escritura
  dword Read(const FileDefs::FileHandle& hFile, 
             sbyte* const psbBuffer, 
			 const dword udBuffSize,
             const dword udInitOffset = 0);
  dword ReadLine(const FileDefs::FileHandle& hFile, 
				 const dword udInitOffset,
				 std::string& szDest);
  dword ReadStringFromBinary(const FileDefs::FileHandle& hFile,
						     const dword udOffset,
							 std::string& szDestStr);  
  dword WriteStringInBinary(const FileDefs::FileHandle& hFile,
							const dword udOffset,
							const std::string& szString);
  dword Write(const FileDefs::FileHandle& hFile, 
			  const dword udOffset,
              const sbyte* const psbBuffer, 
			  const dword udBuffSize);  

public:
  // iCFileSystem / Trabajo con ficheros
  dword GetFileSize(const FileDefs::FileHandle& hFile);
  void GetFileNamesFromPath(const std::string& szPath,
						    std::list<std::string>& DestList);

private:
  // Metodos de apoyo  
  dword CreateNewID(void);
};

#endif // ~ CFileSystem
