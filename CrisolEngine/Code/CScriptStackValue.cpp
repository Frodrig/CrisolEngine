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
// CScriptStackValue.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CScriptStackValue.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CScriptStackValue.h"

#include "SYSEngine.h"
#include "iCFileSystem.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa sin valor asociado
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
CScriptStackValue::CScriptStackValue(void)
{
  // Finaliza
  End();

  // Sin valor
  m_ValueType = NO_VALUE;;
}
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa para valores float
// Parametros:
// - fValue. Valor a asociar
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
CScriptStackValue::CScriptStackValue(const float fValue)
{
  // Finaliza
  End();

  // Asocia
  m_StackValue.fValue = fValue;
  m_ValueType = FLOAT_VALUE;;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa para valores dword.
// Parametros:
// - udValue. Valor a asociar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
CScriptStackValue::CScriptStackValue(const dword udValue)
{
  // Finaliza
  End();

  // Asocia
  m_StackValue.udValue = udValue;
  m_ValueType = DWORD_VALUE;;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa para valores string
// Parametros:
// - szValue. Valor a asociar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
CScriptStackValue::CScriptStackValue(const std::string& szValue)
{
  // Finaliza
  End();

  // Asocia, creando antes espacio en memoria
  m_StackValue.szValue = new std::string(szValue);
  ASSERT(m_StackValue.szValue);
  m_ValueType = STRING_VALUE;;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Copia un objeto.
// Parametros:
// - ScriptStackValue. Objeto a copiar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
CScriptStackValue::CScriptStackValue(const CScriptStackValue& ScriptStackValue)
{    
  // Finaliza posible instancia
  End();

  // Asocia segun el tipo
  switch(ScriptStackValue.m_ValueType) {
    case FLOAT_VALUE: {
	  m_StackValue.fValue = ScriptStackValue.m_StackValue.fValue;
	} break;

	case DWORD_VALUE: {
	  m_StackValue.udValue = ScriptStackValue.m_StackValue.udValue;
	} break;

	case STRING_VALUE: {
	  ASSERT(ScriptStackValue.m_StackValue.szValue);		  
	  m_StackValue.szValue = new std::string(*ScriptStackValue.m_StackValue.szValue);
	  ASSERT(m_StackValue.szValue);
	} break;
  }; // ~ switch

  // Asocia el tipo
  m_ValueType = ScriptStackValue.m_ValueType;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CScriptStackValue::End(void)
{
  // Si es un valor string, libera memoria
  if (STRING_VALUE == m_ValueType) {
	// ASSERT(m_StackValue.szValue); Tiene que existir algun BUG 
	// pq este assert no deberia de fallar
	delete m_StackValue.szValue;	
	m_StackValue.szValue = NULL;
  }

  // Establece sin valor
  m_ValueType = NO_VALUE;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Guarda en disco el valor asociado.
// - El modo de almacenamiento sera: 
//    * Tipo de valor.
//    * Valor.
// Parametros:
// - hFile. Handle al fichero.
// - udOffset. Offset donde comenzar a almacenzar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CScriptStackValue::Save(const FileDefs::FileHandle& hFile,
						dword& udOffset)
{
  // Guarda el tipo de valor
  iCFileSystem* const pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys);
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&m_ValueType), 
							  sizeof(CScriptStackValue::eValueType));

  // Segun sea el tipo de valor guardado, asi se almacenara su valor
  switch(m_ValueType) {
	case CScriptStackValue::FLOAT_VALUE: {
	  // Valor float
	  udOffset += pFileSys->Write(hFile, 
								  udOffset, 
								  (sbyte *)(&m_StackValue.fValue), 
								  sizeof(float));
	} break;

	case CScriptStackValue::DWORD_VALUE: {
	  // Valor dword
	  udOffset += pFileSys->Write(hFile, 
								  udOffset, 
								  (sbyte *)(&m_StackValue.udValue), 
								  sizeof(dword));
	} break;

	case CScriptStackValue::STRING_VALUE: {
	  // Valor string 
	  udOffset += pFileSys->WriteStringInBinary(hFile, 
												udOffset, 
												*m_StackValue.szValue);
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recupera de disco un valor previamente guardado a disco.
// Parametros:
// - hFile. Handle al fichero.
// - udOffset. Offset desde donde comenzar a leer la informacion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CScriptStackValue::Load(const FileDefs::FileHandle& hFile,
						dword& udOffset)
{
  // Finaliza valores previos
  End();

  // Obtiene el tipo de valor que se debera de leer
  iCFileSystem* const pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys);
  udOffset += pFileSys->Read(hFile, 
						     (sbyte *)(&m_ValueType), 
						     sizeof(CScriptStackValue::eValueType),
						     udOffset);

  // Segun sea el tipo de valor guardado, asi se almacenara su valor
  switch(m_ValueType) {
	case CScriptStackValue::FLOAT_VALUE: {
	  // Valor float
	  udOffset += pFileSys->Read(hFile, 
								 (sbyte *)(&m_StackValue.fValue), 
								  sizeof(float),
								  udOffset);
	} break;

	case CScriptStackValue::DWORD_VALUE: {
	  // Valor dword
	  udOffset += pFileSys->Read(hFile, 
								 (sbyte *)(&m_StackValue.udValue), 
								 sizeof(dword),
								 udOffset);
	} break;

	case CScriptStackValue::STRING_VALUE: {
	  // Valor string (se debera de crear antes en memoria)
	  m_StackValue.szValue = new std::string;
	  ASSERT(m_StackValue.szValue);	  
	  udOffset += pFileSys->ReadStringFromBinary(hFile, 
											     udOffset, 
												 *m_StackValue.szValue);
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Operador de asignacion a nivel de objetos.
// Parametros:
// - Instancia al objeto con el que trabajar
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
CScriptStackValue& 
CScriptStackValue::operator=(const CScriptStackValue& ScriptStackValue) 
{
  // Han de ser del mismo tipo
  ASSERT((m_ValueType == ScriptStackValue.m_ValueType) != 0);

  // Asocia segun el tipo
  switch(ScriptStackValue.m_ValueType) {
    case FLOAT_VALUE: {
	  m_StackValue.fValue = ScriptStackValue.m_StackValue.fValue;
	} break;

	case DWORD_VALUE: {
	  m_StackValue.udValue = ScriptStackValue.m_StackValue.udValue;
	} break;

	case STRING_VALUE: {
	  ASSERT(m_StackValue.szValue);
	  ASSERT(ScriptStackValue.m_StackValue.szValue);		  
	  *m_StackValue.szValue = *ScriptStackValue.m_StackValue.szValue;
	} break;

	default:
	  ASSERT(false);
  }; // ~ switch

  // Retorna
  return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Operador de igualdad
// Parametros:
// - Instancia al objeto con el que trabajar
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CScriptStackValue::operator==(const CScriptStackValue& ScriptStackValue) const 
{
  // SOLO si son del mismo tipo
  ASSERT((m_ValueType == ScriptStackValue.m_ValueType) != 0);

  // Compara a nivel de contenido
  switch (m_ValueType) {
    case FLOAT_VALUE: {
      return (m_StackValue.fValue == ScriptStackValue.m_StackValue.fValue);
    } break;

    case DWORD_VALUE: {
      return (m_StackValue.udValue == ScriptStackValue.m_StackValue.udValue);
    } break;

    case STRING_VALUE: {
      ASSERT(m_StackValue.szValue);
      ASSERT(ScriptStackValue.m_StackValue.szValue);
      return (0 == strcmpi(m_StackValue.szValue->c_str(), ScriptStackValue.m_StackValue.szValue->c_str()));
    } break;
  }; // ~ switch
	
  // Iguales (caso de no tener valor asociado)
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Operador mayor que
// Parametros:
// - Instancia al objeto con el que trabajar
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CScriptStackValue::operator>(const CScriptStackValue& ScriptStackValue) const 
{
  // SOLO si son del mismo tipo
  ASSERT((m_ValueType == ScriptStackValue.m_ValueType) != 0);

  // Compara a nivel de contenido
  switch (m_ValueType) {
    case FLOAT_VALUE: {
      return (m_StackValue.fValue > ScriptStackValue.m_StackValue.fValue);
    } break;

    case DWORD_VALUE: {
      return (m_StackValue.udValue > ScriptStackValue.m_StackValue.udValue);
    } break;

    case STRING_VALUE: {
      ASSERT(m_StackValue.szValue);
      ASSERT(ScriptStackValue.m_StackValue.szValue);
      return (m_StackValue.szValue > ScriptStackValue.m_StackValue.szValue);
    } break;
  }; // ~ switch
	
  // No es mayor (caso de no tener valor asociado)
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Operador mayor o igual que
// Parametros:
// - Instancia al objeto con el que trabajar
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CScriptStackValue::operator>=(const CScriptStackValue& ScriptStackValue) const 
{
  // SOLO si son del mismo tipo
  ASSERT((m_ValueType == ScriptStackValue.m_ValueType) != 0);

  // Compara a nivel de contenido
  switch (m_ValueType) {
    case FLOAT_VALUE: {
      return (m_StackValue.fValue >= ScriptStackValue.m_StackValue.fValue);
    } break;

    case DWORD_VALUE: {
      return (m_StackValue.udValue >= ScriptStackValue.m_StackValue.udValue);
    } break;

    case STRING_VALUE: {
      ASSERT(m_StackValue.szValue);
      ASSERT(ScriptStackValue.m_StackValue.szValue);
      return (m_StackValue.szValue >= ScriptStackValue.m_StackValue.szValue);
    } break;
  }; // ~ switch
	
  // No es mayor o igual (caso de no tener valor asociado)
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Operador menor que
// Parametros:
// - Instancia al objeto con el que trabajar
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CScriptStackValue::operator<(const CScriptStackValue& ScriptStackValue) const 
{
  // SOLO si son del mismo tipo
  ASSERT((m_ValueType == ScriptStackValue.m_ValueType) != 0);

  // Compara a nivel de contenido
  switch (m_ValueType) {
    case FLOAT_VALUE: {
      return (m_StackValue.fValue < ScriptStackValue.m_StackValue.fValue);
    } break;

    case DWORD_VALUE: {
      return (m_StackValue.udValue < ScriptStackValue.m_StackValue.udValue);
    } break;

    case STRING_VALUE: {
      ASSERT(m_StackValue.szValue);
      ASSERT(ScriptStackValue.m_StackValue.szValue);
      return (m_StackValue.szValue < ScriptStackValue.m_StackValue.szValue);
    } break;
  }; // ~ switch
	
  // No es menor (caso de no tener valor asociado)
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Operador menor o igual que
// Parametros:
// - Instancia al objeto con el que trabajar
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CScriptStackValue::operator<=(const CScriptStackValue& ScriptStackValue) const 
{
  // SOLO si son del mismo tipo
  ASSERT((m_ValueType == ScriptStackValue.m_ValueType) != 0);

  // Compara a nivel de contenido
  switch (m_ValueType) {
    case FLOAT_VALUE: {
      return (m_StackValue.fValue <= ScriptStackValue.m_StackValue.fValue);
    } break;

    case DWORD_VALUE: {
      return (m_StackValue.udValue <= ScriptStackValue.m_StackValue.udValue);
    } break;

    case STRING_VALUE: {
      ASSERT(m_StackValue.szValue);
      ASSERT(ScriptStackValue.m_StackValue.szValue);
      return (m_StackValue.szValue <= ScriptStackValue.m_StackValue.szValue);
    } break;
  }; // ~ switch
	
  // No es menor o igual (caso de no tener valor asociado)
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Operador distinto que
// Parametros:
// - Instancia al objeto con el que trabajar
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CScriptStackValue::operator!=(const CScriptStackValue& ScriptStackValue) const 
{
  // SOLO si son del mismo tipo
  ASSERT((m_ValueType == ScriptStackValue.m_ValueType) != 0);

  // Compara a nivel de contenido
  switch (m_ValueType) {
    case FLOAT_VALUE: {
      return (m_StackValue.fValue != ScriptStackValue.m_StackValue.fValue);
    } break;

    case DWORD_VALUE: {
      return (m_StackValue.udValue != ScriptStackValue.m_StackValue.udValue);
    } break;

    case STRING_VALUE: {
      ASSERT(m_StackValue.szValue);
      ASSERT(ScriptStackValue.m_StackValue.szValue);
      return (0 != strcmpi(m_StackValue.szValue->c_str(), ScriptStackValue.m_StackValue.szValue->c_str()));
    } break;
  }; // ~ switch
	
  // No es menor o igual (caso de no tener valor asociado)
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retorna la suma de dos valores en pila. 
// Parametros:
// - First, Second. Parametros.
// Devuelve:
// - Un nuevo objeto.
// Notas:
///////////////////////////////////////////////////////////////////////////////
CScriptStackValue operator+(const CScriptStackValue& First,
						    const CScriptStackValue& Second)
{
  // SOLO si tipos iguales
  ASSERT((First.m_ValueType == Second.m_ValueType) != 0);

  // Segun el tipo, asi retornara
  switch(First.m_ValueType) {
	case CScriptStackValue::FLOAT_VALUE: {
	  return CScriptStackValue(First.m_StackValue.fValue + Second.m_StackValue.fValue);	  
	} break;
	
	case CScriptStackValue::DWORD_VALUE: {
	  return CScriptStackValue(First.m_StackValue.udValue + Second.m_StackValue.udValue);	  
	} break;
	
	case CScriptStackValue::STRING_VALUE: {
	  ASSERT(First.m_StackValue.szValue);
	  ASSERT(Second.m_StackValue.szValue);
	  return CScriptStackValue((*First.m_StackValue.szValue) + (*Second.m_StackValue.szValue));	  
	} break;
  }; // ~ switch

  // Problemas
  ASSERT(false);
  return (CScriptStackValue());
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retorna la resta de dos valores SOLO si son float o string
// Parametros:
// - First, Second. Parametros.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
CScriptStackValue operator-(const CScriptStackValue& First,
						    const CScriptStackValue& Second)
{
  // SOLO si tipos iguales
  ASSERT((First.m_ValueType == Second.m_ValueType) != 0);

  // Segun el tipo, asi retornara
  switch(First.m_ValueType) {
	case CScriptStackValue::FLOAT_VALUE: {
	  return CScriptStackValue(First.m_StackValue.fValue - Second.m_StackValue.fValue);	  
	} break;
	
	case CScriptStackValue::DWORD_VALUE: {
	  return CScriptStackValue(First.m_StackValue.udValue - Second.m_StackValue.udValue);	  
	} break;	
  }; // ~ switch

  // Problemas
  ASSERT(false);
  return (CScriptStackValue());
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retorna la division de dos valores SOLO si son float o string
// Parametros:
// - First, Second. Parametros.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
CScriptStackValue operator/(const CScriptStackValue& First,
						    const CScriptStackValue& Second)
{
  // SOLO si tipos iguales
  ASSERT((First.m_ValueType == Second.m_ValueType) != 0);
  
  // Segun el tipo, asi retornara
  switch(First.m_ValueType) {
	case CScriptStackValue::FLOAT_VALUE: {
	  return CScriptStackValue(First.m_StackValue.fValue / Second.m_StackValue.fValue);	  
	} break;
	
	case CScriptStackValue::DWORD_VALUE: {
	  return CScriptStackValue(First.m_StackValue.udValue / Second.m_StackValue.udValue);	  
	} break;	
  }; // ~ switch

  // Problemas
  ASSERT(false);
  return (CScriptStackValue());
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retorna la multiplicacion de dos valores SOLO si son float o string
// Parametros:
// - First, Second. Parametros.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
CScriptStackValue operator*(const CScriptStackValue& First,
							const CScriptStackValue& Second)
{
  // SOLO si tipos iguales
  ASSERT((First.m_ValueType == Second.m_ValueType) != 0);

  // Segun el tipo, asi retornara
  switch(First.m_ValueType) {
	case CScriptStackValue::FLOAT_VALUE: {
	  return CScriptStackValue(First.m_StackValue.fValue * Second.m_StackValue.fValue);	  
	} break;
	
	case CScriptStackValue::DWORD_VALUE: {
	  return CScriptStackValue(First.m_StackValue.udValue * Second.m_StackValue.udValue);	  
	} break;	
  }; // ~ switch

  // Problemas
  ASSERT(false);
  return (CScriptStackValue());
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retorna el modulode dos valores SOLO si son float o string. En el caso 
//   de los float, se tendra que realizar un cast previo a word
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
CScriptStackValue operator%(const CScriptStackValue& First,
					        const CScriptStackValue& Second)
{
  // SOLO si tipos iguales
  ASSERT((First.m_ValueType == Second.m_ValueType) != 0);

  // Segun el tipo, asi retornara
  switch(First.m_ValueType) {
	case CScriptStackValue::FLOAT_VALUE: {
	  return CScriptStackValue(float((sdword)(First.m_StackValue.fValue) % (sdword)(Second.m_StackValue.fValue)));	  
	} break;
	
	case CScriptStackValue::DWORD_VALUE: {
	  return CScriptStackValue(First.m_StackValue.udValue % Second.m_StackValue.udValue);	  
	} break;	
  }; // ~ switch

  // Problemas
  ASSERT(false);
  return (CScriptStackValue());
}