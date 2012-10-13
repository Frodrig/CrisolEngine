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
// CCBTParser.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Parser para el trabajo con los ficheros de datos de texto .cbt
//
// Notas:
// - Cuando se detecte un error al parsear, este sera entendido como error
//   fatal y dentendra el motor con una llamada FatalError.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CCBTPARSER_H_
#define _CCBTPARSER_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _CFILEPARSER_H_
#include "CFileParser.h"
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CCBTParser
class CCBTParser: public CFileParser
{ 
private:
  // Tipos
  // Clase base
  typedef CFileParser Inherited;

private:
  // Vbles de miembro
  std::string             m_szVarPrefix;     // Prefijo actual para vbles
  Inherited::eParseResult m_LastParseResult; // Ultimo resultado del parser

public:
  // Constructor / destructor
  CCBTParser(void) { }
  virtual ~CCBTParser(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion de instancia.  
  bool Init(const std::string& szFileName);
  void End(void);

public:
  // Trabajo con vbles
  inline void SetVarPrefix(const std::string& szVarPrefix) {
	ASSERT(Inherited::IsInitOk());
	// Establece el nombre de la vble
	m_szVarPrefix = szVarPrefix;
  }  
  inline std::string GetPrefixName(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el nombre del prefijo
	return m_szVarPrefix;
  }

protected:
  // CFileParser / Tratamiento de errores
  void ParseError(const Inherited::eParseResult& Error,
			      const std::string& szVar);

public:
  // Trabajo con el flag de error
  inline bool WasLastParseOk(void) const {
	ASSERT(Inherited::IsInitOk());
	// De vuelve el flag de error asociado a la ultima devolucion
	return m_LastParseResult ? true : false;
  }
  void ResetParseFlag(void) {
	ASSERT(Inherited::IsInitOk());
	// Se resetea el valor del resultado asociado al ultimo parseado 
	m_LastParseResult = Inherited::VALID_VALUE;
  }

public:
  // Obtencion de valores 
  std::string ReadString(const std::string& szVarName = "", 
						 const bool bCheckError = true);
  std::string ReadStringID(const std::string& szVarName = "", 
						   const bool bCheckError = true);
  float ReadNumeric(const std::string& szVarName = "",
					const bool bCheckError = true);
  bool ReadFlag(const std::string& szVarName = "",
			    const bool bCheckError = true);
  GraphDefs::sRGBColor ReadRGBColor(const std::string& szVarName = "",
 									const bool bCheckError = true);
  GraphDefs::sAlphaValue ReadAlpha(const std::string& szVarName = "",
								   const bool bCheckError = true);
  sPosition ReadPosition(const std::string& szVarName = "",
						 const bool bCheckError = true);  
private:
  // Metodos de apoyo
  bool CheckIdName(std::string& szIdName);
};

#endif // ~ #ifdef _CCBTPARSER_H_
