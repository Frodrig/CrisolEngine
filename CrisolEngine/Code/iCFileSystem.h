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
// iCFileSystem.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCFileSystem
//
// Descripcion:
// - Define la clase que hara de interfaz general (clase base) para CFileSystem
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICFILESYSTEM_H_
#define _ICFILESYSTEM_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _FILEDEFS_H_ 
#include "FileDefs.h"
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif

// Defincion de clases / estructuras / espacios de nombres

// Interfaz iCFileSystem
struct iCFileSystem
{
public:
  // Operaciones de apertura / cierre
  virtual FileDefs::FileHandle Open(const std::string& szFileName,
									const bool bCreate = false) = 0;
  virtual void Close(const FileDefs::FileHandle& hFile) = 0;

public:
  // Operaciones de lectura / escritura
  virtual dword Read(const FileDefs::FileHandle& hFile, 
                     sbyte* const psbBuffer, 
					 const dword udBuffSize,
                     const dword udInitOffset = 0) = 0;
  virtual dword ReadLine(const FileDefs::FileHandle& hFile, 
						 const dword udInitOffset,
						 std::string& szDest) = 0;  
  virtual dword ReadStringFromBinary(const FileDefs::FileHandle& hFile,
									 const dword udOffset,
									 std::string& szDestStr) = 0;  
  virtual dword WriteStringInBinary(const FileDefs::FileHandle& hFile,
								    const dword udOffset,
								    const std::string& szString) = 0;
  virtual dword Write(const FileDefs::FileHandle& hFile, 
					  const dword udOffset,
					  const sbyte* const psbBuffer, 
					  const dword udBuffSize) = 0;  

public:
  // Trabajo con ficheros
  virtual dword GetFileSize(const FileDefs::FileHandle& hFile) = 0;
  virtual void GetFileNamesFromPath(const std::string& szPath,
									std::list<std::string>& DestList) = 0;
};

#endif // ~ #ifdef _ICFILESYSTEM_H_
