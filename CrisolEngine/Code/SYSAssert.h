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
// SYSAssert.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Este modulo se encarga de realizar las operaciones de assercion de
//   expresiones. Todo modulo del engine tendra acceso a estas operaciones.
//
// Notas:
// - SYSAssert solo entrara en accion cuando se compile en modo debug. Se 
//   supondra en dicho caso que se halla habilitado el flag _DEBUG
// - Cuando se produzca un asserto se realizara un reporte a disco y a ventana
//   de depuracion. Acto seguido se detendra la ejecucion obligando al
//   depurador a que detenga la ejecucion del programa.
// - SYSAssert dispondra de su propio espacio de nombres SYSAssert. Para poder
//   trabajar correctamente en el espacio de nombres, sera necesario definir
//   using namespace SYSAssert;
///////////////////////////////////////////////////////////////////////////////
#ifndef _SYSASSERT_H_
#define _SYSASSERT_H_

// Archivos de cabecera
#ifdef _SYSASSERT
  #include <fstream>  
  #include <stdlib.h>   
#endif

// Este modulo estara definido en el espacio de nombres SYSAssert
namespace SYSAssert
{
  #ifdef _SYSASSERT
    // Nombre del fichero a donde enviar el reporte del error
    static const char* szAssertFile = "AssertReport.txt";

    // Se reporta el suceso de la asercion
    void ReportAssertError(const char* szExp, const char* szFile, 
                           const int& nLine, const char* szMsg = 0);

    // Macro ASSERT sin mensaje de texto    
    #define ASSERT(EXP) \
    if (!EXP) { ReportAssertError(#EXP, __FILE__, __LINE__); }  

    // Macro ASSERT con mensaje de texto introducido por el usuario
    #define ASSERT_MSG(EXP, MSG) \
    if (!EXP) { ReportAssertError(#EXP, __FILE__, __LINE__, MSG); }  
  #endif
  
  #ifndef _SYSASSERT
    #define ASSERT(EXP)
    #define ASSERT_MSG(EXP, MSG)
  #endif
} // ~ SYSAssert

#endif // ~#ifdef _ASSERT_H_