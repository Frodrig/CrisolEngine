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
// CCBTParser.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CCBTParser.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CCBTParser.h"

#include <iostream>
#ifdef _CRISOLENGINE
 #include "SYSEngine.h"
 #include "iCLogger.h"
#endif

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la clase.
// Parametros:
// - szFileName. Nombre del fichero.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCBTParser::Init(const std::string& szFileName)
{
  // SOLO si parametros validos
  ASSERT(!szFileName.empty());
  
  // Inicializa clase base
  Inherited::Init(szFileName, "End");

  // Inicializa vbles de miembro
  m_szVarPrefix = "";  
  m_LastParseResult = Inherited::VALID_VALUE;

  // Se retorna resultado
  return Inherited::IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCBTParser::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Detiene la ejecucion realizando un error fatal.
// Parametros:
// - Error. Codigo del error producido.
// - szVar. Variable no encontrada que hizo que se produjera el error.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CCBTParser::ParseError(const Inherited::eParseResult& Error,
					   const std::string& szVar)
{
  // Se establece el tipo de error
  std::string szErrorType; // Tipo de error
  switch(Error) {
	case Inherited::VALUE_NO_FOUND: {
	  szErrorType = "VALUE_NO_FOUND \\ Valor no encontrado";
	} break;
	
	case Inherited::ERROR_IN_SENTENCE: {
	  szErrorType = "ERROR_IN_SENTENCE \\ Error en sentencia";
	} break;
	
	case Inherited::NO_SECTION: {
	  szErrorType = "NO_SECTION \\ No existe la sección";
	} break;
	
	default: {
	  szErrorType = "Desconocido";
	} break;
  };

  // Hace una llamada de error fatal
  #ifdef _CRISOLENGINE
	SYSEngine::FatalError("Fallo interpretando: %s.\nSección: %s.\nVariable: %s.\nTipo de error: %s.\n",
						  Inherited::GetFileName().c_str(),
						  Inherited::GetSectionName().c_str(), 
						  szVar.c_str(),
						  szErrorType.c_str());
  #else 	
	fprintf(stderr,
			"Error> Fallo interpretando: %s.\nSección: %s.\nVariable: %s.\nTipo de error: %s.\n",
			Inherited::GetFileName().c_str(),
			Inherited::GetSectionName().c_str(), 
			szVar.c_str(),
			szErrorType.c_str());
	exit(1);
  #endif
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el identificador recibido es correcto. Un identificador se
//   considerara correcto si no tiene espacios internos y no esta vacio.
// Parametros:
// - szIdName. Nombre del identificador.
// Devuelve:
// - Si es correcto true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCBTParser::CheckIdName(std::string& szIdName)
{
  // Comprueba si tiene espacios  
  std::string::iterator It = szIdName.begin();
  for (; It != szIdName.end(); ++It) {
	if (' ' == *It) { break; }
  }

  // Retorna resultado
  return (!szIdName.size() || It != szIdName.end()) ? false : true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Lee la informacion referida a una cadena de caracteres.
// Parametros:
// - szVarName. Nombre de la vble.
// - bCheckError. Flag de comprobacion de error de lectura.
// Devuelve:
// - Valor leido
// Notas:
///////////////////////////////////////////////////////////////////////////////
std::string 
CCBTParser::ReadString(const std::string& szVarName, 
					   const bool bCheckError)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si seccion activa
  ASSERT(Inherited::IsSectionActive());

  // Obtiene valor  
  Inherited::SetParseErrorFlag(bCheckError);	
  std::string szVar(m_szVarPrefix);  
  szVar += szVarName;
  std::string szValue;
  m_LastParseResult = Inherited::GetValue(szVar, szValue);  

  // Se retorna valor
  return szValue;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Lee la informacion referida a una cadena de caracteres pero de tipo
//   identificador. Un identificador no podra tener espacios en blanco ni 
//   acentos.
// Parametros:
// - szVarName. Nombre de la vble.
// - bCheckError. Flag de comprobacion de error de lectura.
// Devuelve:
// - Valor leido
// Notas:
///////////////////////////////////////////////////////////////////////////////
std::string 
CCBTParser::ReadStringID(const std::string& szVarName, 
						 const bool bCheckError) 
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si seccion activa
  ASSERT(Inherited::IsSectionActive());

  // Obtiene valor
  std::string szValue = ReadString(szVarName, bCheckError);

  // Comprueba la validez del identificador
  if (!CheckIdName(szValue)) {
	szValue = "";
	if (Inherited::IsParseErrorFlag())  {
	  std::string szVar(m_szVarPrefix);
	  szVar += szVarName;  
	  ParseError(Inherited::ERROR_IN_SENTENCE, szVar);
	} 
  }

  // Retorna el valor
  return szValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Lee informacion numerica
// Parametros:
// - szVarName. Nombre de la vble.
// - bCheckError. Flag de comprobacion de error de lectura.
// Devuelve:
// - Valor leido
// Notas:
///////////////////////////////////////////////////////////////////////////////
float 
CCBTParser::ReadNumeric(const std::string& szVarName,
						const bool bCheckError)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si seccion activa
  ASSERT(Inherited::IsSectionActive());

  // Obtiene valor  
  Inherited::SetParseErrorFlag(bCheckError);
  std::string szVar(m_szVarPrefix);
  szVar += szVarName;  
  float fValue;
  m_LastParseResult = Inherited::GetValue(szVar, fValue);  

  // Se retorna valor
  return fValue;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene un flag (bool). Los flags se leeran como palabras true / false.
// Parametros:
// - szVarName. Nombre de la vble.
// - bCheckError. Flag de comprobacion de error de lectura.
// Devuelve:
// - Valor leido
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CCBTParser::ReadFlag(const std::string& szVarName,
					 const bool bCheckError)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si seccion activa
  ASSERT(Inherited::IsSectionActive());

  // Obtiene valor
  const std::string szValue = ReadStringID(szVarName, bCheckError);
  
  // Comprueba el valor
  if (0 == stricmp(szValue.c_str(), "true")) {	
	return true;
  } else if (0 == stricmp(szValue.c_str(), "false")) {
	return false;
  } 
  
  // Identificador no conocido
  if (Inherited::IsParseErrorFlag())  {
	std::string szVar(m_szVarPrefix);
	szVar += szVarName;  
	ParseError(Inherited::ERROR_IN_SENTENCE, szVar);
  } 
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Lee valores RGB
// Parametros:
// - szVarName. Nombre de la vble.
// - bCheckError. Flag de comprobacion de error de lectura.
// Devuelve:
// - Valor leido
// Notas:
// - Se asociaran los sufijos
//   * RedColor
//   * GreenColor
//   * BlueColor
///////////////////////////////////////////////////////////////////////////////
GraphDefs::sRGBColor 
CCBTParser::ReadRGBColor(const std::string& szVarName,
						 const bool bCheckError)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si seccion activa
  ASSERT(Inherited::IsSectionActive());

  // Se obtienen valores
  // Rojo
  std::string szVar(szVarName);
  szVar += "RedColor";  
  const float fValue = ReadNumeric(szVar, bCheckError);
  GraphDefs::sRGBColor RGBColor;
  RGBColor.ubRed = fValue < 0 ? 0 : fValue;  
  // ¿Se leyo informacion?
  if (m_LastParseResult) {
	// Verde
	szVar = szVarName;
	szVar += "GreenColor";  
	const float fValue = ReadNumeric(szVar, bCheckError);
	RGBColor.ubGreen = fValue < 0 ? 0 : fValue;  
	// ¿Se leyo informacion?
	if (m_LastParseResult) {
	  // Azul
	  szVar = szVarName;
	  szVar += "BlueColor";  
	  const float fValue = ReadNumeric(szVar, bCheckError);
	  RGBColor.ubBlue = fValue < 0 ? 0 : fValue;  
	}	
  }  

  // Se retorna valor
  return RGBColor;    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Lee valor Alpha
// Parametros:
// - szVarName. Nombre de la vble.
// - bCheckError. Flag de comprobacion de error de lectura.
// Devuelve:
// - Valor leido
// Notas:
// - Se asociaran los sufijos
//   * Alpha
///////////////////////////////////////////////////////////////////////////////
GraphDefs::sAlphaValue
CCBTParser::ReadAlpha(const std::string& szVarName,
					  const bool bCheckError)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si seccion activa
  ASSERT(Inherited::IsSectionActive());

  // Obtiene valor
  GraphDefs::sAlphaValue Alpha;
  std::string szVar(szVarName);
  szVar += "Alpha";  
  const float fValue = ReadNumeric(szVar, bCheckError);
  if (m_LastParseResult) {
	Alpha.ubAlpha = fValue < 0 ? 0 : fValue;
  }

  // Se retorna valor
  return Alpha;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene datos de posicion.
// Parametros:
// - szVarName. Nombre de la vble.
// - bCheckError. Flag de comprobacion de error de lectura.
// Devuelve:
// - Valor leido
// Notas:
// - Se asociaran los sufijos
//   * XPos
//   * YPos
///////////////////////////////////////////////////////////////////////////////
sPosition 
CCBTParser::ReadPosition(const std::string& szVarName,
						 const bool bCheckError)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si seccion activa
  ASSERT(Inherited::IsSectionActive());

  // Obtiene valores de posicion
  // Posicion en X
  std::string szVar(szVarName);
  szVar += "XPos";  
  sPosition Position;
  Position.swXPos = ReadNumeric(szVar, bCheckError);
  // ¿Se leyo informacion?
  if (m_LastParseResult) {
	// Posicion en Y
	szVar = szVarName;
	szVar += "YPos";
	Position.swYPos = ReadNumeric(szVar, bCheckError);
  }

  // Se retorna valor
  return Position;  
}
