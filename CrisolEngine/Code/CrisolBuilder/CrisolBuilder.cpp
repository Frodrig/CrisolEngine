///////////////////////////////////////////////////////////////////////////////
// CrisolBuilder - CrisolEngine's Config files processor
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
// CrisolBuilder.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Utilidad para la creacion de los archivos .cbb a partir de los .cbt que
//   se especificaran en un archivo llamado CrisolBuilder.ini.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////

// Includes
#include "CCrisolBuilder.h"
#include <iostream>

// Prototipos de funciones
void WriteHead(void);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Entrada a la utilidad.
//   Por defecto, el archivo CrisolBuilder.ini sera el que se busque para
//   leer la configuracion.
// Parametros:
// - argc. Numero de argumentos pasados, incluidos el nombre del programa.
// - argv. Cadena con el nombre de los argumentos
// Devuelve:
// - Codigo de retorno para exit.
// Notas:
///////////////////////////////////////////////////////////////////////////////
int
main(int argc, char**argv)
{
  // Escribe cabecera
  WriteHead();

  // Construye archivos
  CCrisolBuilder CBuilder;
  if (CBuilder.Init("CrisolBuilder.ini")) {
	// Se construyen archivos y se muestran resultados
	CBuilder.Build();
	std::cout << std::endl;
	std::cout << "Advertencias: " << CBuilder.GetNumWarnings() << std::endl;
	std::cout << "Errores: " << CBuilder.GetNumErrors() << std::endl;	
	
	// Abandona
	return 0;
  }
  
  // Hubo problemas inicializando
  std::cout << "\nHubo problemas al inicializar\n";
  exit(0);  
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
  std::cout << "\n\nCrisolBuilder / Fernando Rodríguez <frodrig99@yahoo.com>.\n";  
  std::cout << "CrisolEngine web: <http://usuarios.tripod.es/crisolengine/>.\n";  
  std::cout << "<V:1.0>\n\n";
}
