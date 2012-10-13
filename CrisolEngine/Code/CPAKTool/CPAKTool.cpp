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
// CPAKTool.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Aplicacion destinada al control de un fichero CPAK desde la linea de
//   comandos. Hara que el control sobre los mismos sea mucho mas rapido y
//   comodo. 
// - La aplicacion esta construida como apoyo a Crisolengine.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

#include "..\\SYSDefs.h"
#include "..\\SYSAssert.h"
#include "CCPAKTool.h"
#include <iostream>
#include <list>
#include <string>

// Tipos
// Lista para el mantenimiento de los parametros leidos desde la linea de comandos
typedef std::list<std::string>    CommandLineList;
typedef CommandLineList::iterator CommandLineListIt;

// Ejecucion de la peticion pertinente
bool Execute(CCPAKTool& CPAKTool,
			 CommandLineList& CmdLineList);
void WriteHelp(void);
void WriteHead(void);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Entrada de la aplicacion, desde aqui se leeran los parametros 
//   introducidos y se ejecutara la logica que proceda.
// Parametros:
// - argc. Numero de parametros, incluido el propio nombre de la aplicacion.
// - argv. Array con los parametros.
// Devuelve:
// - Codigo de retorno que proceda (0)
// Notas:
///////////////////////////////////////////////////////////////////////////////
int 
main(int argc, 
	 char* argv[])
{
  // ¿Hay un numero minimo de parametros?
  if (argc < 2) {
	// No, se muestra ayuda y abandona
	WriteHelp();
  } else {
	// Si, se procede a crear la lista de parametros, la lista de parametros tendra
	// un string en cada posicion, empezando por la peticion introducida
	CommandLineList CommandLine;
	sword uwIt = 1;
	for (; uwIt < argc; ++uwIt) {
	  CommandLine.push_back(argv[uwIt]);
	}

	// Se procesa la peticion  
	WriteHead();
	CCPAKTool CPAKTool;
	CPAKTool.Init("GameData.pak");
	Execute(CPAKTool, CommandLine);
  }

  // Finaliza  
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Escribe la cabecera de ejecucion del compilador.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
WriteHead(void)
{
  // Escribe la cabecera
  std::cout << "\n\nCPAKTool / Fernando Rodríguez <frodrig99@yahoo.com>.\n";  
  std::cout << "CrisolEngine web: <http://usuarios.tripod.es/crisolengine/>.\n";  
  std::cout << "<V:1.0>\n\n";
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ejecuta la peticion que se halle en la lista de peticiones recibidas.
// Parametros:
// - CPAKTool. Tool para la realizacion de las operaciones
// - CmdLineList. Lista de peticiones rebidas.
// Devuelve:
// - Si se ha podido ejecutar la peticion true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
Execute(CCPAKTool& CPAKTool,
		CommandLineList& CmdLineList)
{  
  // SOLO si parametros correctos
  ASSERT(!CmdLineList.empty());
  ASSERT(CPAKTool.IsInitOk());

  // Se toma la orden que se desea ejecutar y se ejecuta si es posible
  if (!CmdLineList.empty()) {
	// Se obtiene el primer parametro de la accion a realizar
	const std::string szOrder(CmdLineList.front());
	CmdLineList.pop_front();
	
	// ¿Crear?
	if (0 == strcmpi(szOrder.c_str(), "-C")) {
	  return CPAKTool.CreateGameDataFile();
	} 
	
	// ¿Añadir archivo?
	if (0 == strcmpi(szOrder.c_str(), "-A")) {	  	  
	  if (!CmdLineList.empty()) {
		const std::string szFile(CmdLineList.front());
		return CPAKTool.AddFile(szFile);
	  } else {
		std::cout << "No se indicó el archivo a añadir.\n";
		return false;
	  }	  
	} 
	
	// ¿Añadir archivos?
	if (0 == strcmpi(szOrder.c_str(), "-Ae")) {
	  // Se extraen parametros
	  std::string szDirectory;
	  std::string szExtension;
	  if (CmdLineList.size() > 1) {		
		szDirectory = CmdLineList.front();
		CmdLineList.pop_front();
		szExtension = CmdLineList.front();
		CmdLineList.pop_front();
	  } else if (!CmdLineList.empty()) {
		szExtension = CmdLineList.front();
		CmdLineList.pop_front();
	  } else {
		std::cout << "Falta la extensión y el posible directorio.\n";
		return false;
	  }	  
	  return CPAKTool.AddFiles(szDirectory, szExtension);
	} 
	
	// Borrar archivo
	if (0 == strcmpi(szOrder.c_str(), "-R")) {
	  if (!CmdLineList.empty()) {
		const std::string szFile(CmdLineList.front());
		CmdLineList.pop_front();
		return CPAKTool.RemoveFile(szFile);
	  } else {
		std::cout << "No se indicó archivo a borrar.\n";
		return false;
	  }
	} 
	
	// Borrar todos los archivos
	if (0 == strcmpi(szOrder.c_str(), "-Ra")) {
	  return CPAKTool.RemoveAllFiles();
	} 
	
	// Extraer archivo
	if (0 == strcmpi(szOrder.c_str(), "-E")) {
	  if (!CmdLineList.empty()) {
		const std::string szFile(CmdLineList.front());
		CmdLineList.pop_front();
		return CPAKTool.ExtractFile(szFile);
	  } else {
		std::cout << "No se indicó archivo a extraer.\n";
		return false;
	  }
	} 
	
	// Extraer todos los archivos
	if (0 == strcmpi(szOrder.c_str(), "-Ea")) {
	  return CPAKTool.ExtractAllFiles();
	} 

	// Crear un listado de archivos
	if (0 == strcmpi(szOrder.c_str(), "-L")) {
	  if (!CmdLineList.empty()) {
		const std::string szFileName(CmdLineList.front());
		CmdLineList.pop_front();
		return CPAKTool.ListFiles(szFileName);
	  } else {
		std::cout << "No se indicó archivo donde volcar el listado.\n";
		return false;
	  }
	}
  }	

  // No se puede realizar operacion
  std::cout << "Parámetros incorrectos\n";
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se mandara a archivo la ayuda referente a la utilidad.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
WriteHelp(void)
{
  // Muestra la ayuda
  std::cout << "Comandos posibles:\n";
  std::cout << "-C                       -> Crea el archivo GameData.pak si no existiera.\n";
  std::cout << "-A archivo               -> Añade el archivo si no está añadido y existe.\n";
  std::cout << "-Ae directorio extensión -> Añade todos los archivos en el directorio (acabado\n";
  std::cout << "                            siempre en \\, ejemplo: Graphics\\) que tengan la\n";
  std::cout << "                            extensión especificada. Si no se pone el directorio\n";
  std::cout << "                            se entenderá que están en la misma carpeta en donde\n";
  std::cout << "                            se ejecuta CPAKTool.\n";
  std::cout << "-E archivo               -> Extrae el archivo si está añadido.\n";
  std::cout << "-Ea                      -> Extrae todos los archivos.\n";
  std::cout << "-R archivo               -> Elimina el archivo si esta añadido.\n";
  std::cout << "-Ra                      -> Elimina todos los archivos.\n";
  std::cout << "-L archivo               -> Crea un listado de los archivos añadidos y lo\n";
  std::cout << "                            deposita en el fichero especificado.\n";
  std::cout << "\n";
  std::cout << "Notas: * Todas las operaciones se realizarán sobre el archivo GameData.pak, por\n";
  std::cout << "         lo que si este archivo no existiera, no se ejecutarán.\n";
  std::cout << "       * Si un fichero ya está añadido y se vuelve a añadir, se contabilizará\n";
  std::cout << "         la operación pero no se efectuará.\n";
}
