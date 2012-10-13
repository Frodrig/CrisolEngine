///////////////////////////////////////////////////////////////////////////////
// CSCompiler - CrisolScript Compiler
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

////////////////////////////////////////////////////////////////////////////////
// Error.h
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Modulo encargado de los chequeos de error.
//
// Notas:
////////////////////////////////////////////////////////////////////////////////

#ifndef _ERROR_H_
#define _ERROR_H_

#include "TypesDefs.h"

// Interfaz
void ReportErrorf(const sbyte* szFileName, 
				  const sbyte* szText, 
				  const sdword nLine, 
				  const sbyte *szMsg, ...);
int yyerror(char* szMsg);
void ReportWarningf(const sbyte* szFileName, 
					const sdword nLine, 
					const sbyte* szMsg, ...);
word ErrorReport(void);
word GetNumErrors(void);
void ReportSetFileBuffer(sbyte* szFileName);
void ReportReturnToFileBuffer(sbyte* szFileName);
#endif