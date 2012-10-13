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
// CIsoTravellingCmd.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CIsoTravellingCmd
//
// Descripcion:
// - Comando que realiza el efecto de travelling sobre el motor isometrico,
//   moviendo la vista desde una posicion a otra.
//
// Notas:
// - Se implementara como un comando logico, pues en realidad estara compuesto
//   por dos fisicos que haran el trabajo.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CISOTRAVELLINGCMD_H_
#define _CISOTRAVELLINGCMD_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CCOMMAND_H_
#include "CCommand.h"
#endif
#ifndef _CMOVISOENGINECMD_H_
#include "CMovIsoEngineCmd.h"
#endif

// Definicion de clases / estructuras / espacios de nombres
class CIsoTravellingCmd: public CCommand
{
private:
  // Tipos
  typedef CCommand Inherited;

private:
  // Estructuras
  struct ExecuteInfo {
	// Informacion del comando en ejecucion
	bool             bIsHorizMov; // ¿Hay movimiento en horizontal?
	bool             bIsVertMov;  // ¿Hay movimiento en vertical?
	CMovIsoEngineCmd HorizMov;    // Comando para el movimiento en horizontal
	CMovIsoEngineCmd VertMov;     // Comando para el movimiento en vertical	
  };

private:
  // Vbles de miembro
  ExecuteInfo m_ExInfo; // Info del comando en ejecucion
    
public:
  // Constructor / destructor
  CIsoTravellingCmd(void) { }
  ~CIsoTravellingCmd(void) { End(); }

public:
  // Protocolos de inicializacion / fin
  bool Init(const float fXEndPos, const float fYEndPos, const word uwSpeed);
  void End(void);

public:
  // CCommand / Ejecucion
  void Execute(const float fDelta);   
};

#endif // ~ CIsoTravellingCmd

