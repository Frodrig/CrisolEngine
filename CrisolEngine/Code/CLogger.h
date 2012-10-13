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
// CLogger.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CLogger
//
// Descripcion:
// - Este modulo se encargara de realizar las operaciones de traza en el
//   engine. Dispondra de dos metodos para realizar la misma tarea, con la
//   particularidad de que uno de ellos solo funcionara en modo debug.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CLOGGER_H_
#define _CLOGGER_H_

// Cabeceras
#ifndef _ICLOGGER_H_
#include "iCLogger.h" 
#endif
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"  
#endif
#ifndef _FSTREAM_H_
#include <fstream> 
#define _FSTREAM_H_
#endif
#ifndef _STRING_H_   
#include <string> 
#define _STRING_H_
#endif    

// Defincion de clases / estructuras / espacios de nombres

// Clase CLogger
class CLogger: public iCLogger
{
  // Vbles de miembro
private:    
  bool          m_bInitOk;       // ¿Clase inicializada correctamente?
  std::ofstream m_FileOutput;    // Fichero en donde guardar la informacion
  std::string   m_szFileName;    // Nombre fichero donde enviar traza
  bool          m_bWindowLogger; // ¿Sacar los datos por OutputDebugScreen?

  // Constructor / Destructor
public:
  CLogger(void): m_bInitOk(false) { }
  ~CLogger(void) { End(); }               

  // Protocolo de inicio y fin de instancia
public:
  bool Init(const std::string& szFileName, 
		    const bool bWindowLogger);
  void End(void);
  inline bool IsInitOk(void) const { return m_bInitOk; }

  // Operaciones de interfaz iCLogger
public:
  void Write(char *szTrace, ...);
};

#endif // ~ CLogger
