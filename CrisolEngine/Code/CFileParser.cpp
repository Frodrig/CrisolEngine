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
// CFileParser.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CFileParser.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CFileParser.h"

#include<iostream>

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la intancia abriendo un fichero para procesar la informacion
//   que este contenga. 
//   Por ultimo, se recibira la marca de fin de seccion que servira para
//   delimitar a estas. Por defecto se recibira la marca END.
// Parametros:
// - szFileName. Nombre del fichero (junto con su extension)
// - szEndSectionMark. Marca de fin de seccion. Por defecto vale END.
// Devuelve:
// - En caso de que todo haya sido correcto, devolvera true. En caso contrario
//   devolvera false.
// Notas:
// - Se permitira reinicializar.
// - Inicializamente no habra ningun tipo de seccion activa y esto se 
//   determinara poniendo el offset a -1.
///////////////////////////////////////////////////////////////////////////////
bool 
CFileParser::Init(const std::string& szFileName,
			      const std::string& szEndSectionMark)
{
  // SOLO si parametros validos
  ASSERT(szFileName.size());    
  ASSERT(szEndSectionMark.size());

  // ¿Se intenta reinicializar?
  if (IsInitOk()) { 
	End(); 
  }

  if (!OpenFile(szFileName)) {
	return false;
  }

  // Se establecen offsets
  m_udInitSectionOffset = 0;
  m_udInSectionOffset = 0;
  m_udFirstLineSectionOffset = 0;

  // Se actualizan resto de vbles de miembro  
  m_szFileName = szFileName;
  m_szEndSectionMark = szEndSectionMark;  
  m_szActSection = "";
  m_bParseError = true;

  // Todo correcto
  m_bIsInitOk = true;
  return true;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia cerrando cualquier fichero que estuviera abierto
//   y bajando el flag de inicializacion.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CFileParser::End(void)
{
  // Procede a finalizar la instancia
  if (IsInitOk()) {
	CloseFile();
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la busqueda de una seccion.
// - Inicialmente la busqueda se realizara desde la ultima posicion en donde
//   se halla situado el offset (ultima seccion) y en caso de que no se
//   encuentre desde el principio hasta esa misma posicion.
// Parametros:
// - szSection. Nombre de la seccion.
// Devuelve:
// - Si todo ha ido bien true en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CFileParser::SetSection(const std::string& szSection)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(!szSection.empty());
	
  // Se eliminan espacios de la seccion a buscar  
  std::string szSectionSrc(szSection);
  QuitSpacesAt(szSectionSrc, CFileParser::START);  
  QuitSpacesAt(szSectionSrc, CFileParser::END);

  // ¿No hay nombre de seccion?
  if (szSectionSrc.empty()) {
	return false;
  }

  // ¿La seccion a buscar es la actual?
  if (IsSectionActive() && 
	  0 == strcmpi(szSection.c_str(), m_szActSection.c_str())) {
	return true;  
  }

  // Se busca desde la posicion actual en el archivo hasta el final
  dword udOffsetIt = m_udInitSectionOffset;
  dword udSectionSize = FindSection(szSectionSrc, udOffsetIt, GetFileSize());
  if (0 == udSectionSize) {	
	// ¿No se estaba buscando desde el comienzo?
	if (m_udInitSectionOffset > 0) {
	  // Se busca desde el inicio hasta la posicion donde estaba el cursor		
	  udOffsetIt = 0;
	  udSectionSize = FindSection(szSectionSrc, udOffsetIt, m_udInitSectionOffset);
	  if (0 == udSectionSize) {	
		// No se encontro
		return false;
	  }
	} else {
	  // Se estaba al comenzo, luego no se encontro
	  return false;
	}
  }

  // En este punto se ha localizado la seccion
  // Se actualizan offsets y nombre de la seccion actual
  m_udInitSectionOffset = udOffsetIt;
  m_udFirstLineSectionOffset = udOffsetIt + udSectionSize;
  m_udInSectionOffset = m_udFirstLineSectionOffset;
  m_szActSection = szSectionSrc;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Navega por el archivo para encontrar la seccion szSection. Comenzara
//   desde la posicion udOffsetIt y podra llegar como maximo a la posicion
//   udEndOffset.
// Parametros:
// - szSection. Nombre de la seccion.
// - udOffsetIt. Offset donde comenzar la busqueda.
// - udEndOffset. Offset tope.
// Devuelve:
// - El tamaño del string con la seccion
// Notas:
///////////////////////////////////////////////////////////////////////////////
dword
CFileParser::FindSection(const std::string& szSection,
						 dword& udOffsetIt,
						 const dword udEndOffset)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(!szSection.empty());
  
  // Procede a localizar  
  while (udOffsetIt < udEndOffset) {
	// Se lee linea 
	std::string szLine;
	const dword udDataRead = ReadLineFromFile(udOffsetIt, szLine);
		
	// Se eliminan espacios al comienzo y final	
	QuitSpacesAt(szLine, CFileParser::START);  
	QuitSpacesAt(szLine, CFileParser::END);
	
	// ¿Es la seccion buscada?
	if (0 == _stricmp(szSection.c_str(), szLine.c_str())) {
	  // Seccion localizada, se devuelve su longitud
	  return szLine.size();
	} else {
	  // No, se incrementa el offset y se itera de nuevo	  
	  udOffsetIt += udDataRead;
	}
  }

  // No se encontro, se devuelve longitud 0
  return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Busca un valor a partir de una posicion que se ha establecido en el
//   exterior. Dicha posicion correspondera a una seccion.
// Parametros:
// - szVar. Nombre de la variable que contendra el valor.
// - udOffsetIt. Offset desde donde comenzar la busqueda, en caso de hallar
//   el valor buscado, contendra la posicion en donde se hallo el mismo.
// - szValue. String en donde se colocara el valor.
// Devuelve:
// - El resultado de busqueda.
// Notas:
// - Se leera solo si hay una seccion establecida y hasta que se halle un
//   valor fin de seccion o bien el final del fichero.
///////////////////////////////////////////////////////////////////////////////
CFileParser::eParseResult 
CFileParser::SearchValue(const std::string& szVar, 
						 dword& udOffsetIt,
						 std::string& szValue)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // Se recorre el fichero desde la posicion actual    
  const dword udFileSize = GetFileSize();
  while (udOffsetIt < udFileSize) {	
	// Se lee linea
	std::string szLine;	
	const dword udDataRead = ReadLineFromFile(udOffsetIt, szLine);

	// Se eliminan espacios al comienzo
	QuitSpacesAt(szLine, CFileParser::START); 	
	
	// ¿NO es un comentario?
	if (!szLine.empty() && 
		';' != szLine[0]) { 
	  // Se eliminan espacios al final
	  QuitSpacesAt(szLine, CFileParser::END);	
	  
	  // ¿NO es una linea vacia despues del limpiado de caracteres?
	  if (!szLine.empty()) { 
		// ¿Es final de seccion?
		if (0 == stricmp(szLine.c_str(), m_szEndSectionMark.c_str())) {
		  // Si, luego no se encontro valor
		  return CFileParser::VALUE_NO_FOUND;
		}

		// No es final de seccion; se parsea la linea
		sValuesParsed ValuesParsed;
		if (!ParseLine(szLine, ValuesParsed)) { 
		  // Hubo problemas parseando la linea
		  return CFileParser::ERROR_IN_SENTENCE;
		}

		// ¿El valor recibido es el buscado?
		if (0 == stricmp(szVar.c_str(), ValuesParsed.szVar.c_str())) {
		  // Si, se toma el valor parseado y se abandona el bucle
		  szValue = ValuesParsed.szValue;		  
		  break;
		}
	  }	  
	}	

	// Se pasa a sig linea
	udOffsetIt += udDataRead;
  }

  // Valor encontrado
  return CFileParser::VALID_VALUE;
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la busqueda principal de valores. Estos valores seran alojados
//   en un string.
// Parametros:
// - szVar. Nombre de la variable que contendra el valor.
// - szValue. Direccion del string en donde se colocara el valor.
// Devuelve:
// - Resultado de la busqueda.
// Notas:
///////////////////////////////////////////////////////////////////////////////
CFileParser::eParseResult
CFileParser::GetValue(const std::string& szVar, 
					  std::string& szValue)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(!szVar.empty());

  // ¿Falta una seccion vinculada?
  if (!IsSectionActive()) { 
	return CFileParser::NO_SECTION; 
  }
  
  // En primer lugar se busca desde la ultima posicion dentro de la seccion
  dword udOffsetIt = m_udInSectionOffset;
  CFileParser::eParseResult Result = SearchValue(szVar, udOffsetIt, szValue);
  if (Result <= 0) {
	// No se encontro, luego se intenta desde el comienzo de seccion
	// Nota: No sera en la zona [Seccion] sino en la primera asignacion
	udOffsetIt = m_udFirstLineSectionOffset;
	Result = SearchValue(szVar, udOffsetIt, szValue);
	if (Result <= 0) {
	  // Hubo problemas en la busqueda; ¿hay que invocar metodo de error?
	  if (IsParseErrorFlag()) {
		ParseError(Result, szVar);
	  }
	  return Result;
	}		
  }
  
  // Localizado, se establece nueva posicion en offset y se retorna  
  if (udOffsetIt >= GetFileSize()) {
	m_udInSectionOffset = m_udFirstLineSectionOffset;
  } else {
	m_udInSectionOffset = udOffsetIt;  
  }
  return Result;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se apoya en el metodo que recibe un string como segundo parametro. Lo unico
//   que hace este metodo es llamar al anterior y con el string recibido realizar
//   una conversion a un double, que sera lo que se devuelva por parametro.
// Parametros:
// - szVar. Nombre de la variable que contendra el valor.
// - fValue. Direccion del double que contendra el valor recibido.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
// - Se apoya por completo en el GetValue que recibe string.
// - En el caso de que el valor a transformar a numerico no sea valido, se
//   devolvera un 0.
///////////////////////////////////////////////////////////////////////////////
CFileParser::eParseResult 
CFileParser::GetValue(const std::string& szVar, 
 					  float& fValue)
{
  // SOLO si intancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(!szVar.empty());
  
  // Se obtiene el valor
  std::string szValue;
  CFileParser::eParseResult Result = GetValue(szVar, szValue);

  // Se realiza la conversion solo si procede
  if (CFileParser::VALID_VALUE == Result) {
	fValue = atof(szValue.c_str());
  } else {
	fValue = 0;
  }

  // Se retorna
  return Result;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe la direccion de un string y se encarga de eliminar los espacios
//   asociados al comienzo o fin del mismo hasta que se encuentra un caracter
//   distinto de space.
// Parametros:
// - szLine. Direccion del string al que quitar los espacios.
// - Zone. Zona donde comenzar a quitar los espacios. La zona puede ser el
//   comienzo o el final del string.
// Devuelve:
// - El numero de espacios eliminados.
// Notas:
///////////////////////////////////////////////////////////////////////////////
word
CFileParser::QuitSpacesAt(std::string& szLine, 
						  const eLineZone& Zone)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // ¿Es linea vacia?
  if (szLine.empty()) {
	return 0;
  }
  
  // Se establece el flag indicativo de por donde comenzar
  const bool bStartAtBegin = (Zone == CFileParser::START);

  // Establece tipo de incremento y zona de inicio
  sword	swInc;               // Incrementos a usar  
  sword	swPosIt;             // Iterador de la posicion donde comenzar a chequear
  word  uwDeletedSpaces = 0; // Espacios eliminados 
  if (bStartAtBegin) {
	swInc = 1;
	swPosIt = 0;
  } else {
	swInc = -1;
	swPosIt = szLine.size() - 1;
  }

  // Procede a localizar la posicion en donde aparece el primer caracter
  // teniendo salida si es una linea vacia  
  while (uwDeletedSpaces < szLine.size() && 
		 ' ' == szLine[swPosIt]) {
	// Se actualiza posicion de lectura y num. espacios quitados
	swPosIt += swInc;
	++uwDeletedSpaces;
  }

  // Se borra los caracteres que procedan teniendo en cuenta si
  // la posicion es de inicio o fin y se devuelve el num. de ellos borrados
  szLine.erase(bStartAtBegin ? 0 : swPosIt + 1, uwDeletedSpaces);  
  return uwDeletedSpaces;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de recibir una linea sin espacios al comienzo y final y 
//   separar los tokens que forma la sentencia. Los tokens estaran formados
//   por la variable (una palabra), el operador de asignacion ('=') y el
//   valor asignado a dicha variable, que podra ser un numero o una o varias
//   palabras o bien, un valor vacio (Variable = ). En este ultimo caso se
//   devolvera un string vacio.
// Parametros:
// - szLine. Linea a parsear.
// - ValuesParsed. Direccion de la estructura donde depositar el resultado de
//   haber parseado.
// Devuelve:
// - Si se ha parseado una linea valida true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CFileParser::ParseLine(const std::string& szLine, 
					   sValuesParsed& ValuesParsed)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(!szLine.empty());
  
  // Se localiza la variable. Para ello se buscara hasta el primer
  // espacio en blanco o caracter de asignacion '='  
  std::string szLineTmp(szLine);
  sword swIt = 0;
  while (swIt < szLineTmp.size() && 
		 szLineTmp[swIt] != ' ' && 
		 szLineTmp[swIt] != '=') {	
	++swIt;
  }

  // ¿NO hay vble?
  if (szLineTmp.size() == swIt) { 
	return false; 
  }  

  // Se extrae el nombre de la vble y se elimina esa porcion del string
  ValuesParsed.szVar = szLine.substr(0, swIt);
  szLineTmp.erase(0, swIt);
  
  // Se quitan espacios al comienzo y se comprueba que queda el
  // operador de asignacion
  QuitSpacesAt(szLineTmp, CFileParser::START);  
  if (!szLineTmp.size() || szLineTmp[0] != '=') { 
	return false; 
  }  

  // Se quita la asignacion y se comprueba que aun quede espacio
  szLineTmp.erase(0, 1);
  if (szLineTmp.empty()) { 
	return false; 
  }

  // Por ultimo, se obtiene todo el valor asociado a la vble y se retorna
  QuitSpacesAt(szLineTmp, CFileParser::START);  
  ValuesParsed.szValue = szLineTmp;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este metodo sera llamado siempre que el flag m_bParseError este activo
//   y se produzca un error en la lectura de una vble. Internamente no hara
//   nada, pero al ser virtual, se esperara que las clase base lo implementen
//   para que se codifiquen las acciones especificas.
// Parametros:
// - Error. Codigo del error producido.
// - szVar. Variable no encontrada que hizo que se produjera el error.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CFileParser::ParseError(const CFileParser::eParseResult& Error,
						const std::string& szVar)
{
}
