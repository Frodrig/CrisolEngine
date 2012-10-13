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
// CAnimCmd.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CAnimCmd.
//
// Descripcion:
// - Se encarga de realizar las llamadas NextFrame oportunas de una plantilla
//   de animacion.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CANIMCMD_H_
#define _CANIMCMD_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CCOMMAND_H_
#include "CCommand.h"
#endif
#ifndef _CLINEARINTERPOLATION_H_
#include "CLinearInterpolation.h"
#endif

// Definicion de clases / estructuras / espacios de nombres
struct iCAnimTemplate;

// Clase CAnimCmd
class CAnimCmd: public CCommand
{
private:
  // Tipos
  typedef CCommand Inherited;

private:
  // Estructuras  
  struct CmdInfo {
	// Informacion relativa al comando			
	iCAnimTemplate* pAnimTemplate; // Plantilla de animacion
	float           fTime;         // Tiempo relativo a los fps
  };

private:
  // Vbles de miembro
  CLinearInterpolation m_Interpolation; // Control de la interpolacion
  CmdInfo              m_CmdInfo;       // Informacion relativa al comando 
  
public:
  // Constructor / destructor
  CAnimCmd(void) { }
  ~CAnimCmd(void) { End(); }

public:
  // Protocolos de inicializacion / fin
  bool Init(iCAnimTemplate* const pAnimTemplate);
  void End(void);

public:
  // CCommand / Ejecucion
  void Execute(const float fDelta);    
};

#endif // ~ CAnimCmd

