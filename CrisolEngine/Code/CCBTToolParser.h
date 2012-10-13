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
// CCBTToolParser.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Especializa la clase CFileParser para que los metodos de trabajo sobre
//   la lectura de datos se realice en un fichero que se halle sobre disco.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CCBTTOOLPARSER_H_
#define _CCBTTOOLPARSER_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CCBTPARSER_H_
#include "CCBTParser.h"
#endif
#ifndef _SET_H_
#define _SET_H_
#include <set>
#endif
#ifndef _FSTREAM_H_
#define _FSTREAM_H_
#include <fstream>
#endif

// Definicion de clases / estructuras / espacios de nombres

// Clase CFileParser
class CCBTToolParser: public CCBTParser
{
private:
  // Clase base
  typedef CCBTParser Inherited;
 
private:  
  // Vbles de miembro
  std::ifstream  m_File;      // Fichero sobre el que trabajar
  std::streampos m_lFileSize; // Tamaño del fichero
    
public:
  // Constructor / destructor
  CCBTToolParser(void): m_lFileSize(0) { }
  virtual ~CCBTToolParser(void) { Inherited::End(); }

protected:
  // Metodos virtuales para el trabajo con el fichero
  bool OpenFile(const std::string& szFileName) {
	// Se abre
	m_File.open(szFileName.c_str());
	if (m_File.fail()) { 
	  return false; 
	}  
  
	// Se obtiene el tamaño
	m_File.clear();
	m_File.seekg(0, std::ios::end);
	m_lFileSize = m_File.tellg();
	m_File.seekg(0, std::ios::beg);

	// Todo correcto
	return true;
  }
  word ReadLineFromFile(const dword udOffset,
						std::string& szLine) {
	ASSERT(Inherited::IsInitOk());
	ASSERT(IsFileOpen());
	// Lee linea
	m_File.clear();
	m_File.seekg(udOffset, std::ios::beg);
	std::getline(m_File, szLine);
	ASSERT((m_File.tellg() >= udOffset) != 0);	
	return (word(m_File.tellg()) - udOffset);
  }
  dword GetFileSize(void) {
	ASSERT(Inherited::IsInitOk());
	ASSERT(IsFileOpen());
	// Retorna tamaño
	return m_lFileSize;
  }
  bool IsFileOpen(void) {
	ASSERT(Inherited::IsInitOk());
	// Retorna flag
	return m_File.is_open();
  }
  void CloseFile(void) {
	ASSERT(Inherited::IsInitOk());
	ASSERT(IsFileOpen());
	// Cierra archivo
	m_File.close();
  }

public:
  // Creacion de lista de secciones
  bool CreateSetOfSections(std::set<std::string>& SetOfSections);
};

#endif // ~ #ifdef _CCBTTOOLPARSER_H_
