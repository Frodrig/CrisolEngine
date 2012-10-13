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
// CFadeCmd.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CFadeCmd
//
// Descripcion:
// - Se encarga de realizar una operacion de Fade (In / Out) sobre la
//   estructura Alpha o RGB recibida.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CFADECMD_H_
#define _CFADECMD_H_

// Cabeceras
#ifndef _CCOMMAND_H_
#include "CCommand.h"
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif
#ifndef _CLINEARINTERPOLATION_H_
#include "CLinearInterpolation.h"
#endif

// Definicion de clases / estructuras / espacios de nombres

// Clase CFadeCmd
class CFadeCmd: public CCommand
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
  struct sCmdAlphaInfo {
	// Info para comando sobre estructura alpha
	GraphDefs::sAlphaValue* pAlphaSrc; // Alpha fuente
	GraphDefs::sAlphaValue  AlphaDest; // Alpha destino
  };

  struct sCmdRGBInfo {
	// Info para el comando sobre estructura RGB
	GraphDefs::sRGBColor* pRGBSrc; // RGB fuente
	GraphDefs::sRGBColor  RGBDest; // RGB destino
  };

  struct sCmdInfo {
	// Enumerados	
	enum eStepType {
	  // Tipo de incremento
	  INC_STEP,
	  DEC_STEP
	};
	// Informacion relativa al comando
	FadeType  Type;        // Tipo de fade	
	eStepType StepType[3]; // Tipo de incremento relativo al paso
	word      uwFPS;       // Velocidad a la que realizar el fade
	word      uwStep;      // Cantidad a restar / añadir	
	float     fTime;       // Tiempo de actualizacion
	// Info sobre la estructura donde realizar el fade
	sCmdAlphaInfo AlphaInfo; // Fade sobre alpha
	sCmdRGBInfo   RGBInfo;   // Fade sobre rgb	
  }; 

private:
  // Vbles de miembro  
  sCmdInfo             m_CmdInfo;       // Informacion relativa al comando  
  CLinearInterpolation m_Interpolation; // Control de la interpolacion
  
public:
  // Constructor / destructor
  CFadeCmd(void) { }
  ~CFadeCmd(void) { End(); }

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
private:
  // Metodos de apoyo
  bool AlphaFade(void);
  bool ColorFade(void);
};

#endif // ~ CFadeCmd

