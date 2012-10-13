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
// iCScript.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCScript.
//
// Descripcion:
// - Interfaz a CScript
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICSCRIPT_H_
#define _ICSCRIPT_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"          
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif

// Definicion de clases / structuras / tipos referenciados
class CScriptStackValue;

// Interfaz iCScript
struct iCScript
{
public:
  // Operaciones sobre la pila
  virtual CScriptStackValue* Pop(void) = 0;
  virtual void Push(CScriptStackValue* const pValue) = 0;
  virtual void PopStackFrame(void) = 0;
  virtual bool PushStackFrame(const word uwCodeIdx) = 0;
  virtual CScriptStackValue* const GetValueAt(const word uwOffset) = 0;
  virtual void SetValueAt(const word uwOffset,
						  const float fValue) = 0;
  virtual void SetValueAt(const word uwOffset,
						  const AreaDefs::EntHandle& hEntity) = 0;
  virtual void SetValueAt(const word uwOffset, 
						  const std::string& szValue) = 0;

public:
  // Operaciones de ejecucion
  virtual void Pause(void) = 0;
  virtual void Resume(void) = 0;
  virtual void SetCodePos(const dword udCodePos) = 0;
  virtual void ErrorInterrupt(void) = 0;
  virtual void Stop(void) = 0;

public:
  // Operaciones con la tabla de strings
  virtual std::string GetString(const word uwStrIdx) = 0;

public:
  // Operaciones de consulta
  virtual bool IsGlobalScript(void) const = 0;
};

#endif // ~ #ifdef _ICSCRIPT_H_
