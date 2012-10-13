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
// CCPAKTool.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CCPAKTool.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CCPAKTool.h"

#include "..\\CCPAKFile.h"
#include <iostream>
#include <sstream>

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia.
// Parametros:
// - szGameDataFileName. Nombre del archivo PAK a manejar.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
// - Se permitira reinicializar
///////////////////////////////////////////////////////////////////////////////
bool 
CCPAKTool::Init(const std::string& szGameDataFileName)
{
  // SOLO si parametros correctos
  ASSERT(!szGameDataFileName.empty());

  // ¿Se intenta reinicializar?
  if (IsInitOk()) {
	// Finaliza
	End();
  }

  // Se toma el nombre
  m_szGameDataFileName = szGameDataFileName;

  // Se retorna
  m_bIsInitOk = true;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCPAKTool::End(void)
{
  // Finaliza
  if (IsInitOk()) {
	m_bIsInitOk = false;
  }
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea el archivo de datos SOLO si este no existe. Al crearlo lo dejara
//   simplemente vacio.
// Parametros:
// Devuelve:
// - Si lo creo true. En caso de que no lo haya creado por existir false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCPAKTool::CreateGameDataFile(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se intenta abrir SIN crear para ver si existe
  WriteMsg("Intentando crear el fichero " + m_szGameDataFileName);
  CCPAKFile CPAKFile;
  if (CPAKFile.Open(m_szGameDataFileName)) {
	// Se abandona porque ya existe
	WriteMsg("No se pudo crear porque el fichero ya existe");
	CPAKFile.Close();
	return false;
  }

  // No existe,  luego se cierra y se procede a abrir creandolo
  CPAKFile.Close();
  if (CPAKFile.Open(m_szGameDataFileName, true)) {
	// Todo correcto
	WriteMsg("Operación realizada");
	CPAKFile.Close();
	return true;
  }

  // Hubo problemas
  WriteMsg("Problemas creando el fichero");
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Añade un fichero.
// Parametros:
// - szFileName. Nombre del fichero
// Devuelve:
// - Resultado de la operacion
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCPAKTool::AddFile(const std::string& szFileName)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Abre fichero CPAK
  WriteMsg("Añadiendo fichero " + szFileName);
  CCPAKFile CPAKFile;
  if (CPAKFile.Open(m_szGameDataFileName)) {
	// Se intenta añadir
	const bool bResult = CPAKFile.Add(szFileName);
	CPAKFile.Close();
	if (bResult) {
	  WriteMsg("Operación realizada");
	} else {
	  WriteMsg("No se pudo añadir el fichero");
	}

	// Retorna
	return bResult;
  }

  // Problemas abriendo
  WriteMsg("Problemas creando el fichero");
  return false; 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Añade archivos, se asociaran pasando un directorio y una extension.
// Parametros:
// - szDirectory. Nombre del directorio.
// - szExtension. Extension de los archivos a añadir.
// Devuelve:
// - Resultado de la operacion
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCPAKTool::AddFiles(const std::string& szDirectory,
					const std::string& szExtension)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Abre fichero CPAK
  WriteMsg("Añadiendo ficheros en directorio " + szDirectory + " y extensión " + szExtension);
  CCPAKFile CPAKFile;
  if (CPAKFile.Open(m_szGameDataFileName)) {
	// Asocia archivos por extension
	const dword udNumFiles = CPAKFile.AddFilesByExtension(szDirectory, szExtension);
	CPAKFile.Close();	

	// Muestra el num. de archivos asociados
	std::ostringstream StrText;
	StrText << "Ficheros añadidos: " << udNumFiles;
	WriteMsg(StrText.str());

	// Retorna
	return true;
  }

  // Problemas abriendo
  WriteMsg("Problemas abriendo el fichero " + m_szGameDataFileName);
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Extrae un archivo
// Parametros:
// - szFileName. Nombre del archivo.
// Devuelve:
// - Resultado.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCPAKTool::ExtractFile(const std::string& szFileName)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Abre fichero CPAK
  WriteMsg("Extrayendo el fichero " + szFileName);
  CCPAKFile CPAKFile;
  if (CPAKFile.Open(m_szGameDataFileName)) {
	// Intenta extraer
	const bool bResult = CPAKFile.Extract(szFileName);	
	CPAKFile.Close();	
	if (bResult) {	  
	  WriteMsg("Operación realizada");
	} else {
	  WriteMsg("No se pudo extraer el fichero");
	}

	// Retorna
	return bResult;
  }

  // Problemas abriendo
  WriteMsg("Problemas abriendo el fichero " + m_szGameDataFileName);
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Extrae todos los archivos
// Parametros:
// Devuelve:
// - Resultado de la operacion
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCPAKTool::ExtractAllFiles(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Abre fichero CPAK
  WriteMsg("Extrayendo todos los ficheros");
  CCPAKFile CPAKFile;
  if (CPAKFile.Open(m_szGameDataFileName)) {
	// Intenta extraer todos
	const dword udNumFiles = CPAKFile.ExtractAll();	
	CPAKFile.Close();	

	// Num. de extraidos
	std::ostringstream StrText;
	StrText << "Ficheros extraidos: " << udNumFiles;
	WriteMsg(StrText.str());

	// Retorna
	return true;
  }

  // Problemas abriendo
  WriteMsg("Problemas abriendo el fichero " + m_szGameDataFileName);
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Borra un archivo.
// Parametros:
// - szFileName. Nombre del archivo
// Devuelve:
// - Resultado de la operacion.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCPAKTool::RemoveFile(const std::string& szFileName)
{
  // Abre fichero CPAK
  WriteMsg("Borrando fichero " + szFileName);
  CCPAKFile CPAKFile;
  if (CPAKFile.Open(m_szGameDataFileName)) {	
	// Intenta borrar
	const bool bResult = CPAKFile.Remove(szFileName);	
	CPAKFile.Close();	
	if (bResult) {	  
	  WriteMsg("Operación realizada");
	} else {
	  WriteMsg("No se pudo borrar el fichero");
	}

	// Retorna
	return true;
  }

  // Problemas abriendo
  WriteMsg("Problemas abriendo el fichero " + m_szGameDataFileName);
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina todos los archivos.
// Parametros:
// Devuelve:
// - Resultado de la operacion
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCPAKTool::RemoveAllFiles(void)
{
  // Abre fichero CPAK
  WriteMsg("Borrando todos los ficheros");
  CCPAKFile CPAKFile;
  if (CPAKFile.Open(m_szGameDataFileName)) {
	// Intenta borrar todos
	const dword udNumFiles = CPAKFile.RemoveAll();
	CPAKFile.Close();

	// Num. de borrados
	std::ostringstream StrText;
	StrText << "Ficheros borrados: " << udNumFiles;
	WriteMsg(StrText.str());

	// Retorna
	return true;
  }

  // Problemas abriendo
  WriteMsg("Problemas abriendo el fichero " + m_szGameDataFileName);
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene una lista con los nombres de los ficheros que estan en el CPAK y
//   produce un archivo en donde se contendra esa informacion.
// Parametros:
// - szListFileName. Nombre del archivo donde guardar la lista de ficheros.
// Devuelve:
// - Resultado
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCPAKTool::ListFiles(const std::string& szListFileName)
{
  // Abre fichero CPAK
  WriteMsg("Creando lista de ficheros contenidos");
  CCPAKFile CPAKFile;
  if (CPAKFile.Open(m_szGameDataFileName)) {
	// Obtiene lista
	std::list<std::string> StrList;
	CPAKFile.GetFileList(StrList);

	// Procede a guardar el resultado
	std::ofstream File;
	File.open(szListFileName.c_str());
	if (File.is_open()) {
	  File << "Lista de ficheros en " << m_szGameDataFileName << "." << std::endl;
	  File << "Número de ficheros: " << StrList.size() << "." << std::endl << std::endl;
	  File << "================================================================================" << std::endl;
	  std::list<std::string>::iterator It(StrList.begin());
	  for (; It != StrList.end(); ++It) {
		File << *It << std::endl;
	  }
	  File << "================================================================================" << std::endl << std::endl;	  
	  File << "CPAKTool / Fernando Rodriguez <frodrig99@yahoo.com>." << std::endl;  
	  File << "Crisolengine web: <http://usuarios.tripod.es/crisolengine/>." << std::endl;  
	  File << "<V:1.0>";

	  // Cierra y retorna
	  File.close();
	  return true;
	}

	// Problemas creando el fichero de salida
	WriteMsg("Problemas creando el fichero " + szListFileName);
	return false;
  }

  // Problemas abriendo
  WriteMsg("Problemas abriendo el fichero " + m_szGameDataFileName);
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Escribre mensaje
// Parametros:
// - szMsg. Mensaje a escribir
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCPAKTool::WriteMsg(const std::string& szMsg)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Muestra mensaje
  std::cout << "> " << szMsg << ".\n";
}
