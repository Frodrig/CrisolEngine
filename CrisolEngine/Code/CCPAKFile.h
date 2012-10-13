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
// CCPAKFile.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CPAKFile
//
// Descripcion:
// - Clase encargada de tratar ficheros CPAK. Estos ficheros se caracterizaran
//   por ser capaces de mantener un numero muy grande de ficheros de muy 
//   distintos tipos. Esta clase dara el soporte para tratarlos directamente.
// - En el caso de añadir ficheros, siempre se debera de tener en cuenta que si
//   se pasa dicho fichero a traves de un path, se insertara el path como
//   elemento que forme parte del nombre del fichero y en consecuencia, habra
//   que indicar el nombre del path para obtener el fichero.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CCPAKFILE_H_
#define _CCPAKFILE_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"    
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif
#ifndef _FSTREAM_H_
#define _FSTREAM_H_
#include <fstream> 
#define _FSTREAM_H_
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>        
#define _STRING_H_
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif
#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_
#include <algorithm>
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CCPAKFile
class CCPAKFile
{  
private:
  // Constantes
  const byte VERSIONHI; // Version alta con la que se trabaja
  const byte VERSIONLO; // Version baja con la que se trabaja

private:
  // Enumerados    
  enum FileState {    
	// Estado del fichero
    FILE_OK     = 1, // Fichero accesible
    FILE_MODIFY,     // Fichero modificado
    FILE_REMOVED     // Fichero eliminado
  };

// Obliga al compilador a que las estructuras las alinee en bytes
#pragma pack(push, 1)

  // Estructuras
private:
  struct sCPAKHeader { 
	// Cabecera del fichero CPAK, en total 16 Bytes
    sbyte sbFirm[4];     // Firma de la cabecera
    byte  ubVersionHi;   // Version superior
    byte  ubVersionLo;   // Version inferior
    dword udFlags;       // Flags fichero
    dword udNumFiles;    // Numero de ficheros mantenidos
    dword udOffsetIndex; // Offset, desde el comienzo, en donde hallar indice
  };

// Se retorna a la alineacion de estructuras por defecto
#pragma pack(pop)  

private:
  // Estructuras forward
  struct sNFile;

private:
  // Tipos
  // Indice de ficheros asociados al CPAK
  typedef std::map<std::string, sNFile*> CPAKFileIndexMap;
  typedef CPAKFileIndexMap::iterator     CPAKFileIndexMapIt;
  typedef CPAKFileIndexMap::value_type   CPAKFileIndexMapValType;

  // Vbles de miembro
private:
  std::string      m_szFileName;     // Nombre del fichero  
  std::fstream     m_File;           // Fichero CPAK
  std::string      m_szFileNameTemp; // Nombre del fichero temporal
  std::fstream     m_FileTemp;       // Fichero temporal    
  sCPAKHeader      m_CPAKHeader;     // Cabecera del fichero CPAK
  CPAKFileIndexMap m_FileIndex;      // Tabla con todos los ficheros CPAK
  bool             m_bFileOpen;      // ¿Fichero abierto correctamente?
  bool             m_bFileModify;    // ¿Hay que llamar a UpdateChanges()?

public:
  // Constructor / Destructor
  CCPAKFile(void): VERSIONHI(1),
				   VERSIONLO(0),
				   m_bFileOpen(false) { }
  ~CCPAKFile(void) { Close(); }
  
public:
  // Interfaz de inicio / fin
  bool Open(const std::string& szFileName, 
			const bool bOverWrite = false);
  void Close(void);  
private:
  // Metodos de apoyo
  bool InitHeader(const bool bFromDisk);
  void ReleaseFileIndex(void);
  
public:
  // Operaciones sobre ficheros individuales
  dword Read(const std::string& szFileName, 
             sbyte* pubBuffer, 
			 const dword udBufferSize,
             const dword udInitOffset = 0);
  dword ReadLine(const std::string& szFileName, 
				 const dword udInitOffset,
				 std::string& szDest);
  bool Write(const std::string& szFileName, 
			 const sbyte* const pubBuffer,
             const dword udBufferSize);
  bool Remove(const std::string& szFileName);
  bool Add(const std::string& szFileName, 
		   const bool bRemoveFromDisk = false);
  bool Extract(const std::string& szFileName, 
			   const bool bRemoveFromPak = false);
  
public:
  // Operaciones sobre la totalidad de los ficheros
  dword AddFilesByExtension(const std::string& szDirectory,
							const std::string& szExtension);
  dword ExtractAll(const bool bRemoveFromCPAK = false);
  dword RemoveAll(void);
  
public:
  // Operaciones de consulta
  inline bool IsOpen(void) const { 
	return m_bFileOpen; 
  }  
  dword GetFileSize(const std::string& szFileName);
  inline std::string GetCPAKFileName(void) const { 
	ASSERT(IsOpen());
	// Se retorna el nombre
	return m_szFileName; 
  }
  bool IsFilePresent(const std::string& szFileName);
  void GetFileList(std::list<std::string>& StrList);

public:
  // Operaciones de actualizacion
  bool UpdateChanges(void);
  
private:
  // Metodos de apoyo  
  std::string ChangeExtension(const std::string& szFileName, 
							  const std::string& szExtension);
  inline CPAKFileIndexMapIt GetCPAKFileIt(const std::string& szFileName) {
	ASSERT(IsOpen());
	// Se retorna iterador al fichero szFileName
	std::string szFileNameLowercase(szFileName);
	std::transform(szFileNameLowercase.begin(), szFileNameLowercase.end(), szFileNameLowercase.begin(), tolower);
	return m_FileIndex.find(szFileNameLowercase);
  }
  inline bool IsValidFirm(const sbyte* psbFirm) const {
	// Retorna el flag
	return ('C' != psbFirm[0] || 'P' != psbFirm[1] || 
			'A' != psbFirm[2] || 'K' != psbFirm[3]) ? false : true;	
  }
  inline bool IsValidVersion(const byte VersionHi, 
							 const byte VersionLo) const {
	// Retorna flag de version valida
	return (CCPAKFile::VERSIONHI > m_CPAKHeader.ubVersionHi ||
		    CCPAKFile::VERSIONLO > m_CPAKHeader.ubVersionLo) ? false : true;
  }  
};

#endif // ~ CCPAKFile
