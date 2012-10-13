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
// CCBTToolParser.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CCBTToolParser.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CCBTToolParser.h"

#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Busca todas las secciones que se hallen en el archivo y las inserta en
//   SetOfSections.
// Parametros:
// - SetOfSections. Lista donde depositar los nombres de las secciones.
// Devuelve:
// - Si el archivo esta bien construido desde el punto de vista de
//   pares seccion, end true, en caso contrario o de que no se
//   hubiera podido abrir archivo, false.
// Notas:
// - La busqueda de la lista de las secciones no supondra trabajar con el
//   archivo original.
///////////////////////////////////////////////////////////////////////////////
bool
CCBTToolParser::CreateSetOfSections(std::set<std::string>& SetOfSections)
{
  // Solo si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Abre otra instancia al archivo
  std::ifstream File(Inherited::GetFileName().c_str());
  if (File.good()) {
	std::string szFileLine; // Linea del fichero
	while (std::getline(File, szFileLine)) {	
	  // Se eliminan espacios al comienzo y final
	  File.clear();
	  Inherited::QuitSpacesAt(szFileLine, CFileParser::START);  
	  Inherited::QuitSpacesAt(szFileLine, CFileParser::END);
	  
	  // ¿NO es una linea vacia? y
	  // ¿Los caracteres de inicio y fin son de seccion?
	  if (!szFileLine.empty() &&		
		  ('[' == szFileLine[0] && 
			']' == szFileLine[szFileLine.size() - 1])) {
		// Si, se inserta el nombre de la seccion
		std::transform(szFileLine.begin(), szFileLine.end(), szFileLine.begin(), tolower);
		SetOfSections.insert(std::set<std::string>::value_type(szFileLine));

		// Se avanza hasta localizar End
		while (std::getline(File, szFileLine)) {
		  // Se eliminan espacios al comienzo y final			
		  File.clear();
		  Inherited::QuitSpacesAt(szFileLine, CFileParser::START);  
		  Inherited::QuitSpacesAt(szFileLine, CFileParser::END);

		  // ¿Se localizo end?
		  if (0 == strcmpi("end", szFileLine.c_str())) {
		    // Si, se rompe while interno
		    break;
		  }
		}

		// ¿Se salio del while sin alcanzar end?
		if (File.eof()) {
		  File.close();
		  return false;
		}				  
	  }
	}
	
	// Todo correcto
	File.close();
	return true;
  }  

  // Hubo problemas
  return false;
}

