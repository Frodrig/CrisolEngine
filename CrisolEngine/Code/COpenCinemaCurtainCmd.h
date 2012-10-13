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
// COpenCinemaCurtainCmd.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - COpenCinemaCurtainCmd
//
// Descripcion:
// - Comando de apertura de la cortina
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _COPENCINEMACURTAINCMD_H_
#define _COPENCINEMACURTAINCMD_H_

// Cabeceras
#ifndef _CCINEMACURTAINCMD_H_
#include "CCinemaCurtainCmd.h"
#endif

// Definicion de clases / estructuras / espacios de nombres

// Clase COpenCinemaCurtainCmd
class COpenCinemaCurtainCmd: public CCinemaCurtainCmd
{
private:
  // Tipos
  typedef CCinemaCurtainCmd Inherited;
  
public:
  // Constructor / destructor
  COpenCinemaCurtainCmd(void) { }
  ~COpenCinemaCurtainCmd(void) { End(); }

public:
  // Protocolos de inicializacion / fin
  bool Init(CCinemaCurtain* const pCinemaCurtain,
			const word uwFPS);
  void End(void);

public:
  // CCommand / Ejecucion
  void Execute(const float fDelta);    
};

#endif // ~ COpenCinemaCurtainCmd

