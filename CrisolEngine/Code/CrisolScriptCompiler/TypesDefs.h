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

///////////////////////////////////////////////////////////////////////////////
// TypesDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - En este fichero se definiran los tipos
//
// Notas:
// - Los tipos de datos se estableceran con #defines en lugar de con typedefs
//   para evitar conflictos de nombres en modulos con definiciones similares.
///////////////////////////////////////////////////////////////////////////////
#ifndef _TYPESDEFS_H_
#define _TYPESDEFS_H_

// Tipos de datos y estructuras
// NOTA: El prefijo 's' sobre un tipo ya definido, significa "signed".
// NOTA: Los tipos boolean (b), los float (f) y los double (d)
#define byte unsigned char     // 8 bits  (ub)
#define sbyte char             //         (sb)
#define word unsigned short    // 16 bits (uw)
#define sword short            //         (sw)
#define dword unsigned long    // 32 bits (ud)
#define sdword long            //         (sd)
#define qword unsigned _int64  // 64 bits (uq)
#define sqword __int64         //         (sq)

#endif // ~ #define _TYPESDEFS_H_