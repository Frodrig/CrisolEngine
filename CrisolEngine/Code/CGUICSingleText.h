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
// CGUICSingleText.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUICSingleText
//
// Descripcion:
// - Implementa un componente de texto simple. Estos componentes representaran
//   una linea (o mas) de texto dibujable en pantalla. Podra responder a eventos
//   cuando el cursor de interfaz este sobre el area en el que se dibujan 
//   todas las lineas de texto.
//
// Notas:
// - El texto sera justificado y las lineas almacenadas en una lista. En dicha
//   lista, se guardara la posicion final y la posicion inicial en donde
//   se localizara el texto, dentro del string asociado, que figurara en la
//   linea.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUICSINGLETEXT_H_
#define _CGUICSINGLETEXT_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CGUICJUSTIFYTEXT_H_
#include "CGUICJustifyText.h"
#endif
#ifndef _LIST_H_
#include <list>
#define _LIST_H_
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CGUICSingleText
class CGUICSingleText: public CGUICJustifyText
{
public:
  // Enumerados
  enum eDrawJustify {
	// Tipos de justificacion para el texto
	// El enumerado sera publico para que el exterior tenga acceso al mismo
	JUSTIFY_CENTER, // Dibuja el texto justificado al centro
	JUSTIFY_RIGHT,  // Dibuja el texto justifificado a dcha
	JUSTIFY_LEFT    // Dibuja el texto justificado a izq
  };

public:
  // Estructuras
  struct sGUICSingleTextInitData: public CGUICJustifyText::sGUICJustifyTextInitData {
	// Inicializacion de GUICSingleTextInitData
	std::string szText; // Texto
  };

private:
  // Estructuras
  struct sLineInfo {
	// Informacion asociada a una linea
	sword swInitPos;        // Posicion de comienzo del texto asociado
	word  uwNumChars;       // Numero de caracteres
	word  uwXJustifyOffset; // Offset de justificacion en X
	// Constructores
	sLineInfo(void): swInitPos(0), uwNumChars(0), uwXJustifyOffset(0) { }
	sLineInfo(const sLineInfo& LineInfo) {
	  swInitPos = LineInfo.swInitPos;
	  uwNumChars = LineInfo.uwNumChars;
	  uwXJustifyOffset = LineInfo.uwXJustifyOffset;
	}
	// Operador de asignacion
	sLineInfo& operator=(const sLineInfo& LineInfo) {
	  // Se copian datos
	  swInitPos = LineInfo.swInitPos;
	  uwNumChars = LineInfo.uwNumChars;
	  // Se retorna instancia
	  return *this;
	}
  };

private:
  // Tipos
  // Clase base
  typedef CGUICJustifyText     Inherited;   // Clase base
  // Lista de lineas
  typedef std::list<sLineInfo> LinesList;
  typedef LinesList::iterator  LinesIt;

private:
  // Estructuras forward
  struct sAlphaFadeInfo;

  // Estructuras
  struct sSingleTextInfo {
	// Info relacionada con el texto mantenido
	std::string            szText;       // Texto
	LinesList              TextLines;    // Lineas con el texto justificado  
	word                   uwAreaWidth;  // Anchura del componente
	word                   uwAreaHeight; // Altura del componente
	GraphDefs::sAlphaValue UsableAlpha;  // Valor alpha a utilizar
	GraphDefs::sAlphaValue Alpha;        // Valor alpha original
  };

private:
  // Vbles de miembro
  sSingleTextInfo m_TextInfo;       // Info relativa al texto 
  sAlphaFadeInfo* m_pAlphaFadeInfo; // Info asociada al posible alpha fade
 
public:
  // Constructor / destructor
  CGUICSingleText(void): m_pAlphaFadeInfo(NULL) { }	
  ~CGUICSingleText(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const sGUICSingleTextInitData& InitData);
  void End(void);

public:
  // CGUIComponent / Establecimiento de componente activo / inactivo
  void SetActive(const bool bActive) {
	ASSERT(Inherited::IsInitOk());
	// Se desactiva fade y se propaga
	DeactiveAlphaFade();
	Inherited::SetActive(bActive);
  }

public:
  // Configuracion de la justificacion
  void DrawJustifyTo(const eDrawJustify& Justify);
private:
  // Metodos de apoyo
  word CalculeLineWidth(const std::string& szLine);

public:
  // Dibujado
  void Draw(void);  

public:
  // Operaciones de fade
  void ActiveAlphaFade(const GraphDefs::sAlphaValue& AlphaDest);
  void DeactiveAlphaFade(void);

public:
  // Obtencion de lineas con texto visible
  word GetVisibleTextLines(void) const;
  std::string GetLine(const word uwLine);

public:
  // Trabajo con el texto
  void ChangeText(const std::string& szText) {
	ASSERT(Inherited::IsInitOk());
	// Libera el texto que hubiera y establece el nuevo
	m_TextInfo.TextLines.clear();
	SetText(szText);
  }
private:
  // Metodos de apoyo
  bool SetText(const std::string& szText);

public:
  // CGUICComponent / Trabajo con el area del componente
  word GetWidthJustify(void) {
	ASSERT(Inherited::IsInitOk());
	// Retorna la anchura asociada al area
	// Nota: En la clase base se guardara este valor de anchura
	return Inherited::GetWidth();
  }
  word GetWidth(void) {
	ASSERT(Inherited::IsInitOk());
	// Retorna la anchura asociada a la linea mas larga
	return m_TextInfo.uwAreaWidth;
  }
  word GetHeight(void) {
	ASSERT(Inherited::IsInitOk());
	// Se devuelve la altura
	return m_TextInfo.uwAreaHeight;
  }
};

#endif 
