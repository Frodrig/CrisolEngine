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
// CCinemaCurtainCmd.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CCinemCurtainCmd.
//
// Descripcion:
// - Clase base para los comandos que manejaran la apertura y cierre de la
//   cortina de cine.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CCINEMACURTAINCMD_H_
#define _CCINEMACURTAINCMD_H_

// Cabeceras
#ifndef _CCOMMAND_H_
#include "CCommand.h"
#endif
#ifndef _CLINEARINTERPOLATION_H_
#include "CLinearInterpolation.h"
#endif
#ifndef _CCINEMACURTAIN_H_
#include "CCinemaCurtain.h"
#endif

// Definicion de clases / estructuras / espacios de nombres
class CCinemaCurtain;

// Clase CCinemaCurtainCmd
class CCinemaCurtainCmd: public CCommand
{
private:
  // Tipos
  typedef CCommand Inherited;

protected:
  // Vbles de miembro  
  CCinemaCurtain*      m_pCinemaCurtain; // Cortina de cine a manipular
  word                 m_uwFPS;          // FPS de actualizacion
  float                m_fTime;          // Tiempo de actualizacion
  CLinearInterpolation m_Interpolation;  // Control de la interpolacion
  
public:
  // Constructor / destructor
  CCinemaCurtainCmd(void) { }
  ~CCinemaCurtainCmd(void) { End(); }

public:
  // Protocolos de inicializacion / fin
  bool Init(CCinemaCurtain* const pCinemaCurtain,
			const word uwFPS);
  void End(void);

protected:
  // Metodos de apoyo
  bool DoInterpolation(const float fDelta);

protected:
  // Manejo de la instancia 
  inline bool OpenCurtain(void) const {
	ASSERT(Inherited::IsActive());
	// Retorna instancia a cortina
	m_pCinemaCurtain->OpenCurtain(1);
	return m_pCinemaCurtain->IsOpen();
  }
  inline bool CloseCurtain(void) const {
	ASSERT(Inherited::IsActive());
	// Retorna instancia a cortina
	m_pCinemaCurtain->CloseCurtain(1);
	return m_pCinemaCurtain->IsClosed();
  }
};

#endif // ~ CCinemaCurtainCmd

