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
// SYSAssert.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar SYSAssert.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "SYSAssert.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza un reporte sobre la asercion ocurrida lo escribe en disco.
// Parametros:
// - szExp: Expresion que produjo el aserto.
// - szFile: Archivo en donde se halla el aserto.
// - nLine: Numero de la linea del archivo en donde se produjo el aserto.
// - szMsg: Mensaje pasado por el programador. Si NULL no hay mensaje.
// Devuelve:
// Notas:
// - El nombre del archivo en donde escribir el aserto se halla en la
//   constante szAssertFile.
// - Solo definida en modo _DEBUG
///////////////////////////////////////////////////////////////////////////////
#ifdef _SYSASSERT
void 
SYSAssert::ReportAssertError(const char* szExp, const char* szFile, 
							 const int& nLine, const char* szMsg)
{
  // Se abre y escribe en el fichero
  std::ofstream File;
  File.open(szAssertFile, std::ios::trunc);
  if (File.good()) {
    // La apertura fue correcta, se procede a escribir la informacion
    File << "[SYSAssert // Se ha producido una aserción no válida]\n";
    File << "-\n";    
    File << "Fichero: " << szFile << "\n";
    File << "Línea: " << nLine << "\n";
    File << "Expresión: " << szExp << "\n";
    if (szMsg) { File << "Comentario: " << szMsg << "\n"; }
  }

  // Se cierra archivo
  File.close();

  // Se aborta la ejecucion
  exit(1);
}
#endif

