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
// CFileParser.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clase:
// - CFileParser
//
// Descripcion:
// - Clase encargada de leer archivos de configuracion sencillos del estilo
//   .Ini. Estos archivos siempre deberan de estar formados con la siguiente
//   estructura:
//
//   SECCION
//   Variable0 operador valor
//   Variable2 operador valor
//   ...
//   VariableN operador valor
//   MARCA DE FIN
// 
//   En donde la SECCION servira para agrupar variables de un mismo tipo y 
//   siempre debera de existir al menos una. 
//   Las sentencias de asignacion estaran formadas de variables, un operador
//   de asingacion (que tendra un solo caracter) y un valor.
// - Se permitira usar comentarios de la forma ; Comentario. Los comentarios
//   deberan de estar en una sola linea.
// - La MARCA DE FIN tambien se podra definir desde en la inicializacion y
//   servira para tener una referencia de donde acaba una seccion. Podra ser
//   una marca de fin una palabra como END o bien la llave cerrada '}', etc.
// - El funcionamiento general vendra a ser el de establecer en una seccion 
//   el archivo .Init y a partir de esa seccion ir indicando las variables a 
//   leer. Todas las vbles seran devueltas como strings, dejando (por el 
//   momento) en respondabilidad del exterior el realizar la conversion de 
//   las mismas a valores numericos.
// - Esta clase no implementara los procesos de lectura de datos, sino que
//   dejara tal responsabilidad en unos metodos virtuales puros de tal forma
//   que se desentienda del modo en que los datos de disco son tomados. 
//   Unicamente existiran unos  metodos virtuales que deberan de ser 
//   implementados en las clases derivadas.
//
// Notas:
// - Se trataran por igual las mayusculas que las minusculas.
// - Se pondran poner espacios al comienzo y final de las lineas. En el caso 
//   de las secciones, los espacios contaran como elementos a comparar, por lo
//   que sera conveniente pensar en secciones con los caracteres todos unidos,
//   como: [VideoInfo] ya que esa seccion seria distinta a [ VideoInfo ].
// - En el caso de poner dos secciones iguales y dos variables dentro de una
//   misma seccion iguales la clase siempre trabajara con la primera que 
//   encuentre, obviando a la(s) que este(n) por debajo.
// - El operador de asignacion por defecto sera el igual '='.
// - Notese que sera muy importante que no se puedan dar casos en los que una
//   seccion sea identica a una sentecia de asignacion ya ocurririan graves
//   incongruencias.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CFILEPARSER_H_
#define _CFILEPARSER_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>  
#endif

// Definicion de clases / estructuras / espacios de nombres

// Clase CFileParser
class CFileParser
{
public:
  // Enumerados
  enum eParseResult {
	// Resultados asociados a una operacion de parseo	
	VALID_VALUE        = 1,  // Linea con valor valido	
	VALUE_NO_FOUND     = 0,  // No se encontro valor
	ERROR_IN_SENTENCE  = -1, // Se ha encontrado un error en el fichero
	NO_SECTION         = -2  // Falta por establecer una seccion
  };

private:
  enum eLineZone {
	// Zona asociada a una linea donde comenzar a quitar espacios
	START, // Comienzo
	END    // Final
  };

private:
  // Estructuras
  struct sValuesParsed {
	// Valores parseados de una linea con una sentencia
	// con variable y valor
	std::string szVar;   // Variable
	std::string szValue; // Valor
  };
 
private:
  // Vbles de miembro
  std::string m_szFileName;               // Nombre del archivo
  std::string m_szActSection;             // Seccion actual  
  std::string m_szEndSectionMark;         // Marca de fin de seccion
  dword       m_udInitSectionOffset;      // Offset al comienzo de seccion
  dword       m_udFirstLineSectionOffset; // Offset a la primera linea de seccion
  dword       m_udInSectionOffset;        // Offset en el interior de seccion
  bool		  m_bParseError;              // ¿Invocar ParseError ante un error?
  bool		  m_bIsInitOk;                // ¿Instancia incializada correctamente?  
 
public:
  // Constructor / destructor
  CFileParser(void): //m_hFile(0),
					 //m_pFileSys(NULL),
					 m_bIsInitOk(false) { }
  virtual ~CFileParser(void) { End(); }

public:
  // Protocolos de inicilizacion / finalizacion de instancia.  
  bool Init(const std::string& szFileName,
			const std::string& szEndSectionMark = "END");
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // Trabajo de lectura de valores
  bool SetSection(const std::string& szSection);
  CFileParser::eParseResult GetValue(const std::string& szVar, 
									 std::string& szValue);
  CFileParser::eParseResult GetValue(const std::string& szVar, 
									 float& fValue);  
protected:
  // Metodos de apoyo
  dword FindSection(const std::string& szSection,
				    dword& udOffsetIt,
				    const dword udEndOffset);
  CFileParser::eParseResult SearchValue(const std::string& szVar, 
										dword& udOffsetIt,
										std::string& szValue);
  word QuitSpacesAt(std::string& szLine, 
					const eLineZone& Zone);
  bool ParseLine(const std::string& szLine, 
				 sValuesParsed& ValuesParsed);

protected:
  // Metodos virtuales para el trabajo con el fichero
  virtual bool OpenFile(const std::string& szFileName) = 0;
  virtual word ReadLineFromFile(const dword udOffset,
							    std::string& szLine) = 0;
  virtual dword GetFileSize(void) = 0;
  virtual bool IsFileOpen(void) = 0;
  virtual void CloseFile(void) = 0;

public:
  // Tratamiento de errores
  inline bool IsParseErrorFlag(void) const {
	ASSERT(IsInitOk());
	// Devuelve flag
	return m_bParseError;
  }
  inline void SetParseErrorFlag(const bool bFlag) {
	ASSERT(IsInitOk());
	// Establece flag
	m_bParseError = bFlag;
  }
protected:
  virtual void ParseError(const CFileParser::eParseResult& Error,
						  const std::string& szVar);

public:
  // Operaciones de consulta
  inline bool IsSectionActive(void) const {	
	ASSERT(IsInitOk());
	// Comprueba si el nombre de la seccion no esta vacio
	return !m_szActSection.empty();
  }
  inline std::string GetSectionName(void) const {	
	ASSERT(IsInitOk());
	ASSERT(IsSectionActive());
	// Retorna la seccion actual
	return m_szActSection;
  }
  inline std::string GetFileName(void) const {
	ASSERT(IsInitOk());
	// Retorna el nombre del fichero
	return m_szFileName;
  }  
};

#endif // ~ #ifdef _CFILESPARSER_H_
