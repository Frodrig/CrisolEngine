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
// CLogger.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CLogger.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CLogger.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la clase y establece y abre el nombre del fichero a donde se
//   mandaran los datos. Tambien establece el flag indicativo de mandar datos 
//   a la ventana del debugger de Windows con OutputDebugString.
// Parametros:
// - szFileName: String con el nombre del fichero en donde enviar 
//   la informacion.
// - bWindowLogger: Flag de uso de OutputDebugString. Por defecto valdra true.
// Devuelve:
// - true: Si se ha inicializado bien, false en caso contrario.
// Notas:
// - El archivo en donde enviar la traza se sobreescribira si ya existe.
///////////////////////////////////////////////////////////////////////////////
bool 
CLogger::Init(const std::string& szFileName, const bool bWindowLogger)
{
  // Si la clase estaba inicializada, la dejara finalizada antes
  if (m_bInitOk) { 
	End(); 
  }

  // Se inicializan vbles de miembro
  
  // Abre el archivo para escritura y comprueba operacion
  m_FileOutput.open(szFileName.c_str(), std::ios_base::trunc);
  if (m_FileOutput.good()) { 
	// Se establecen vbles de miembro
	m_szFileName = szFileName;
	m_bWindowLogger = bWindowLogger;
	
	// Se escribe cabecera del logger
    m_FileOutput << "-[Logger Activado]-";
    m_FileOutput << "\n\n";
	m_FileOutput.flush();
	
	// Se levanta flag
    m_bInitOk = true;
  }

  // ¿Se escribe en ventana de debugger?
  if (m_bWindowLogger) { 
	// Se escribe cabecera
    OutputDebugString("\n\n");
    OutputDebugString("-[Logger Activado]-");
    OutputDebugString("\n\n");
  }
  
  // Devuelve resultado de inicializacion
  return m_bInitOk;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia actual.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CLogger::End(void)
{
  if (IsInitOk()) {
    // Se manda info de cierre al disco
    m_FileOutput << "\n";
    m_FileOutput << "-[Logger Desactivado]-";    
    m_FileOutput.close();
    if (m_bWindowLogger) { 
	  // Se manda info de cierre a la ventana del debugger
      OutputDebugString("\n");
      OutputDebugString("-[Logger Desactivado]-");
      OutputDebugString("\n\n");
    }
    m_bInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe una cadena de texto con el formato "printf" y se encarga de 
//   mandarla a disco y a la ventana del debugger de Windows si es que esta
//   activo el flag.
// Parametros:
// - szTrace: Texto principal a enviar con los codigos de comando
// - ...: Resto de elementos del texto.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CLogger::Write(char *szTrace, ...)
{
  // Namespaces
  using namespace std;

  // Solo si la instancia esta correctamente inicializada.
  ASSERT(IsInitOk());

  // Vbles
  static char szTextBuffer[1024]; // Buffer con el texto a mostrar
  va_list     args;               // Lista de argumentos

  // Se crea una cadena con el texto recibido	  
  va_start(args, szTrace);
  vsprintf(szTextBuffer, szTrace, args);
  va_end(args);

  // Se manda la informacion a disco
  m_FileOutput << szTextBuffer;
  m_FileOutput.flush();

  // Se manda la informacion a ventana de Windows si procede
  if (m_bWindowLogger) { 
    OutputDebugString(szTextBuffer);    
  }
}
