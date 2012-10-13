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
// CCBTEngineParser.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Especializa la clase CFileParser para que los metodos de trabajo sobre
//   la lectura de datos se realice usando el motor, esto es, a traves de
//   CrisolEngine.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CCBTENGINEPARSER_H_
#define _CCBTENGINEPARSER_H_

// Cabeceras
#ifndef _CCBTPARSER_H_
#include "CCBTParser.h"
#endif
#ifndef _ICFILESYSTEM_H_
#include "iCFileSystem.h"
#endif

// Definicion de clases / estructuras / espacios de nombres

// Clase CFileParser
class CCBTEngineParser: public CCBTParser
{
private:
  // Clase base
  typedef CCBTParser Inherited;
 
private:  
  // Vbles de miembro
  iCFileSystem*        m_pFileSys;   // Instancia al subsistema de fich.
  FileDefs::FileHandle m_hFile;      // Handle al fichero
  dword				   m_udFileSize; // Tamaño del fichero
    
public:
  // Constructor / destructor
  CCBTEngineParser(void): m_pFileSys(NULL),
						  m_hFile(0),
						  m_udFileSize(0) { }
  virtual ~CCBTEngineParser(void) { Inherited::End(); }

protected:
  // Metodos virtuales para el trabajo con el fichero
  bool OpenFile(const std::string& szFileName);
  word ReadLineFromFile(const dword udOffset,
						std::string& szLine) {
	ASSERT(Inherited::IsInitOk());
	ASSERT(IsFileOpen());
	// Lee linea
	ASSERT(m_pFileSys);
	return m_pFileSys->ReadLine(m_hFile, udOffset, szLine);
  }
  dword GetFileSize(void) {
	ASSERT(Inherited::IsInitOk());
	ASSERT(IsFileOpen());
	// Retorna tamaño
	return m_udFileSize;
  }
  bool IsFileOpen(void) {
	ASSERT(Inherited::IsInitOk());
	// Retorna flag
	return (m_hFile != 0);
  }
  void CloseFile(void) {
	ASSERT(Inherited::IsInitOk());
	ASSERT(IsFileOpen());
	// Cierra archivo
	if (m_hFile) {
	  ASSERT(m_pFileSys);
	  m_pFileSys->Close(m_hFile);
	  m_hFile = 0;	
	}
  }
};

#endif // ~ #ifdef _CCBTTOOLPARSER_H_
