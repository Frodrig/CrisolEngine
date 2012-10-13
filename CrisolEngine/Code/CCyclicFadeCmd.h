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
// CCyclicFadeCmd.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
// Clases:
// - CCyclicFadeCmd
//
// Descripcion:
// - Se encarga de realizar una operacion de Fade (In / Out) sobre la
//   estructura Alpha o RGB recibida pero de forma ciclica.
//
// Notas:
// - Se apoya en la implementacion del comando CFadeCmd.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CCYCLICFADECMD_H_
#define _CCYCLICFADECMD_H_

// Cabeceras
#ifndef _CCOMMAND_H_
#include "CCommand.h"
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif
#ifndef _CFADECMD_H_
#include "CFadeCmd.h"
#endif

// Definicion de clases / estructuras / espacios de nombres

// Clase CCyclicFadeCmd
class CCyclicFadeCmd: public CCommand
{
private:
  // Tipos
  typedef CCommand Inherited;

private:
  // Tipos
  enum FadeType { 
	// Tipos de fade
	ALPHA_FADE, // Fade sobre estructura Alpha
    RGB_FADE    // Fade sobre estructura RGB
  };

private:
  // Estructuras    
  struct CmdAlphaInfo {
	// Info para comando sobre estructura alpha
	GraphDefs::sAlphaValue* pAlphaSrc;  // Direccion Alpha fuente
	GraphDefs::sAlphaValue  AlphaDest;  // Valor del Alpha destino
	GraphDefs::sAlphaValue  AlphaSrc;   // Valor del Alpha fuente	
  };

  struct CmdRGBInfo {
	// Info para el comando sobre estructura RGB
	GraphDefs::sRGBColor* pRGBSrc;  // Direccion del RGB fuente
	GraphDefs::sRGBColor  RGBDest;  // Valor del RGB destino
	GraphDefs::sRGBColor  RGBSrc;   // Valor del RGB fuente	
  };

  struct CmdInfo {
	// Informacion relativa al comando
	word     uwFPS;  // Velocidad a la que realizar el fade
	FadeType Type;   // Tipo de fade	
	// Info sobre la estructura donde realizar el fade
	CmdAlphaInfo AlphaInfo; // Fade sobre alpha
	CmdRGBInfo   RGBInfo;   // Fade sobre rgb		
  }; 

  struct ExecuteInfo {
	// Info del comando en ejecucion	
	union {
	  // Direccion con el siguiente valor para hacer un fade
	  GraphDefs::sAlphaValue* pAlphaNext; // Dir. del sig. paso Alpha para fade
	  GraphDefs::sRGBColor*   pRGBNext;   // Dir. del sig. paso RGB para fade
	};
  };

private:
  // Vbles de miembro
  CFadeCmd    m_FadeCmd; // Comando de fade
  CmdInfo     m_CmdInfo; // Informacion relativa al comando
  ExecuteInfo m_ExInfo;  // Info del comando en ejecucion  
  
public:
  // Constructor / destructor
  CCyclicFadeCmd(void) { }
  ~CCyclicFadeCmd(void) { End(); }

public:
  // Protocolos de inicializacion / fin
  bool Init(GraphDefs::sAlphaValue* const pAlphaSrc, 
			const GraphDefs::sAlphaValue& AlphaDest,
			const word uwFPS);
  bool Init(GraphDefs::sRGBColor* const pRGBSrc, 
			const GraphDefs::sRGBColor& RGBDest,
			const word uwFPS);
  void End(void);

public:
  // CCommand / Ejecucion
  void Execute(const float fDelta);    
};

#endif // ~ CCyclicFadeCmd

