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
// CGUICBlockText.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUICBlockText
//
// Descripcion:
// - Implementa un componente de texto justificado pero capaz de albergar un
//   buffer de informacion asociada a lineas. Es decir, actuara como un 
//   "almacen" de texto que se podra ir insertando. Inicialmente tendra una
//   capacidad maxima asociada. Cuando se alcance dicha capacidad se podra
//   obligar a seguir insertando liberando el texto mas antiguo o bien se 
//   podra omitir cualquier otra nueva insercion.
// - La capacidad hara referencia siempre al numero maximo de lineas que el
//   bloque de texto podra almacenar y reutilizar memoria supondra liberar
//   las lineas mas antiguas de manera circular.
// - En caso de que la capacidad recibida sea 0, se entendera que estamos ante
//   una capacidad infinita o indeterminada. El bloque de texto ira alojando
//   lineas en memoria siempre que se necesiten. Este caso sera util cuando
//   no se pueda preveer el numero maximo de lineas.
//
// Notas:
// - El codigo para la posicion numerica del iterador sera:
//  * 0> No vinculado
//  * 1 ó +> Vinculado
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUICBLOCKTEXT_H_
#define _CGUICBLOCKTEXT_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CGUICJUSTIFYTEXT_H_
#include "CGUICJustifyText.h"
#endif
#ifndef _RESDEFS_H_
#include "ResDefs.h"
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
struct iCFontSystem;

// Clase GUICBlockText
class CGUICBlockText: public CGUICJustifyText
{
public:
  // Estructuras
  struct sGUICBlockTextInitData: public CGUICJustifyText::sGUICJustifyTextInitData {
	// Inicializacion de CGUIBlockTextInitData
	// Datos
	word        uwNumLines;    // Num. lineas visibles
	word        uwCapacity;    // Capacidad
	bool        bCanReutilice; // ¿Puedo reutilizar el bloque de texto?
	bool        bViewBottomUp; // ¿Ver los datos inicialmente abajo?
	std::string szWAVBipSound; // Nombre del fichero WAV	
	// Constructor por defecto
	sGUICBlockTextInitData(void): bViewBottomUp (true),
							      szWAVBipSound("") { }
  };

private:
  // Enumerados
  enum eStateBlock {
	// Estados del bloque de texto
	NORMAL_STATE,    // Estado normal, no hay reutilizacion de memoria
	REUTILICE_STATE, // Estado de reutilizacion de memoria
	FULL_STATE       // Estado de bloque lleno y sin reutilizacion
  };

private:
  // Estructuras
  struct sLineInfo; // Forward	

private:
  // Tipos
  // Clase base
  typedef CGUICJustifyText            Inherited; 
  // Trabajo con la lista de lineas
  typedef std::list<sLineInfo*>       LinesList;
  typedef LinesList::iterator         LinesListIt;
  typedef LinesList::reverse_iterator LinesListRevIt;
  
private:
  // Estructuras
  struct sBlockTextInfo {
	// Info relacionada con el texto mantenido	
	// Relativos al almacen de lineas
	LinesList   TextLines;      // Lista con las lineas de texto
	LinesListIt	ItLinePos;      // Iterador a la linea de texto actual
	word        uwNumLines;     // Num. lineas visibles
	word        uwCapacity;     // Capacidad
	bool        bCanReutilice;  // ¿Puedo reutilizar el bloque de texto?
	bool        bViewBottomUp;  // ¿Ver los datos inicialmente abajo?
	word		uwItNumPos;     // Posicion numerica para el iterador
	word		uwMaxBottomPos; // Posicion tope maxima		
	// Dibujado
	sword swInitYOffDrawPos; // Offset de dibujado en Y inicial
	// Sonido
	std::string             szWAVBipSound;  // Nombre del fichero WAV		
	ResDefs::WAVSoundHandle hWAVBipSound;   // Sonido asociado a la insercion	
  };

private:
  // Instancias a subsistemas
  iCFontSystem* m_pFontSys; // Interfaz al manejador de fuentes

  // Datos asociados al texto almacenado
  sBlockTextInfo m_TextInfo;   // Info relativa al texto 
  eStateBlock    m_StateBlock; // Estado asociado al bloque de texto
 
public:
  // Constructor / destructor
  CGUICBlockText(void): m_pFontSys(NULL) { }	
  ~CGUICBlockText(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const sGUICBlockTextInitData& InitData);
  void End(void);

public:
  // Dibujado
  void Draw(void);

public:
  // Obtencion de lineas con texto visible
  word GetVisibleTextLines(void) const;

public:
  // Insercion de texto al bloque
  bool InsertText(const std::string& szText, 
				  const bool bPlaySound);  
private:
  // Metodos de apoyo
  void InsertLine(const std::string& szTextSrc, 
				  const sword& swInitPos,
				  const word uwNumChars);
  void AdjustInitYOffDrawPos(void);

public:
  // Liberacion de informacion
  void Reset(void);

public:
  // Operaciones sobre el iterador
  bool MoveDown(void);
  bool MoveUp(void);
  void SetAtTop(void);
  void SetAtBottom(void);
private:
  // Metodos de apoyo
  inline void SetIt(void) {		
	// Vincula el iterador a la primera posicion
	m_TextInfo.ItLinePos = m_TextInfo.TextLines.begin();
	m_TextInfo.uwMaxBottomPos = m_TextInfo.uwItNumPos = (m_TextInfo.ItLinePos != m_TextInfo.TextLines.end()) ? 1 : 0;	
  }

public:
  // CGUICComponent / Trabajo con el area del componente
  word GetHeight(void) {
	ASSERT(Inherited::IsInitOk());
	// Se devuelve la altura
	return FontDefs::CHAR_PIXEL_HEIGHT * m_TextInfo.uwNumLines;
  }
};

#endif 
