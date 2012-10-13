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
// CScript.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clase:
// - CScriptStackValue
//
// Descripcion:
// - Representa un valor almacenable en la pila
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CSCRIPTSTACKVALUE_H_
#define _CSCRIPTSTACKVALUE_H_

// Includes
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _FILEDEFS_
#include "FileDefs.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif


// Clase CScriptStackValue
class CScriptStackValue 
{
private:
  // Enumerados
  enum eValueType {
	// El tipo del valor almacenado
	NO_VALUE = 0,     
	FLOAT_VALUE,  
	DWORD_VALUE,  
	STRING_VALUE	
  };

private:
  // Uniones
  union uStackValue {
    // Union con los valores
	float        fValue; 
	dword        udValue;
	std::string* szValue;
  };

private:
  // Vbles de miembro
  uStackValue m_StackValue; // Valor almacenado
  eValueType  m_ValueType;  // Tipo de valor

public:
  // Constructor / Destructor
  CScriptStackValue(void);
  CScriptStackValue(const float fValue);
  CScriptStackValue(const dword udValue);
  CScriptStackValue(const std::string& szValue);
  CScriptStackValue(const CScriptStackValue& ScriptStackValue);
  ~CScriptStackValue(void) { 
    End();
  } 
  
private:
  // Metodos de apoyo
  void End(void);

public:
  // Operaciones de almacenamiento y recuperacion de datos
  void Save(const FileDefs::FileHandle& hFile,
			dword& udOffset);
  void Load(const FileDefs::FileHandle& hFile,
			dword& udOffset);

public:
  // Operadores
  void operator=(const float fValue) {
	ASSERT((FLOAT_VALUE == m_ValueType) != 0);
	m_StackValue.fValue = fValue;
  }
  void operator=(const dword udValue) {
	ASSERT((DWORD_VALUE == m_ValueType) != 0);
	m_StackValue.udValue = udValue;
  }
  void operator=(const std::string& szValue) {
	ASSERT((STRING_VALUE == m_ValueType) != 0);
	*m_StackValue.szValue = szValue;
  }
  CScriptStackValue& operator=(const CScriptStackValue& ScriptStackValue);
  bool operator==(const CScriptStackValue& ScriptStackValue) const;
  bool operator>(const CScriptStackValue& ScriptStackValue) const;
  bool operator>=(const CScriptStackValue& ScriptStackValue) const;
  bool operator<(const CScriptStackValue& ScriptStackValue) const;
  bool operator<=(const CScriptStackValue& ScriptStackValue) const;
  bool operator!=(const CScriptStackValue& ScriptStackValue) const;

public:
  // Operadores friend
  friend CScriptStackValue operator+(const CScriptStackValue& First,
								     const CScriptStackValue& Second);
  friend CScriptStackValue operator-(const CScriptStackValue& First,
								     const CScriptStackValue& Second);
  friend CScriptStackValue operator/(const CScriptStackValue& First,
								     const CScriptStackValue& Second);
  friend CScriptStackValue operator*(const CScriptStackValue& First,
								     const CScriptStackValue& Second);
  friend CScriptStackValue operator%(const CScriptStackValue& First,
								     const CScriptStackValue& Second);

public:
  // Obtencion de valores
  inline float GetFloatValue(void) const {
	ASSERT((FLOAT_VALUE == m_ValueType) != 0);
	return m_StackValue.fValue;
  }
  inline dword GetDWordValue(void) const {
	ASSERT((DWORD_VALUE == m_ValueType) != 0);
	return m_StackValue.udValue;
  }
  inline std::string GetStringValue(void) const {
	ASSERT((STRING_VALUE == m_ValueType) != 0);
	ASSERT(m_StackValue.szValue);
	return *m_StackValue.szValue;
  }  
}; // ~ CScriptStackValue

#endif