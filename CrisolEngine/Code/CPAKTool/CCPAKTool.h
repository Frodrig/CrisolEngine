///////////////////////////////////////////////////////////////////////////////
// CPAKTool - CrisolEngine's packed file manager
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
// CCPakTool.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CCPAKTool
//
// Descripcion:
// - Clase que se encarga de implementar la utilidad CPAKTool para el 
//   manejo del archivo GameData.pak asociado e imprescindible para el motor.
// - Los mensajes de error / advertencia seran lanzados desde esta propia clase.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CCPAKTOOL_H_
#define _CCPAKTOOL_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _SYSDEFS_H_
#include "..\\SYSDefs.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CCPakTool
class CCPAKTool
{

private:
  // Vbles de miembro
  std::string m_szGameDataFileName; // Nombre del archivo CPAK a utilizar
  bool		  m_bIsInitOk;         // ¿Instancia inicializada correctamente?

   
public:
  // Constructor / Destructor
  CCPAKTool(void): m_bIsInitOk(false) { }
  ~CCPAKTool(void) { 
	End(); 
  }
  
public:
  // Protocolo de inicio y fin de instancia
  bool Init(const std::string& szGameDataFileName);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // Operacion de creacion
  bool CreateGameDataFile(void);

public:
  // Operacion de añadir
  bool AddFile(const std::string& szFileName);
  bool AddFiles(const std::string& szDirectory,
				const std::string& szExtension);

public:
  // Operacion de extraccion
  bool ExtractFile(const std::string& szFileName);
  bool ExtractAllFiles(void);

public:
  // Operacion de borrado
  bool RemoveFile(const std::string& szFileName);
  bool RemoveAllFiles(void);

public:
  // Operacion de listado
  bool ListFiles(const std::string& szListFileName);

private:
  // Metodos de apoyo
  void WriteMsg(const std::string& szMsg);
};

#endif // ~ _CCPAKTOOL_H_
