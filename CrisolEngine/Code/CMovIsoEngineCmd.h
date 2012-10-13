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
// CMovIsoEngineCmd.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CMovIsoEngineCmd
//
// Descripcion:
// - Comando que coordina el motor isometrico para realizar el scroll en este
//   en una determinada direccion. En caso de inicializarlo y estar procesando
//   otro movimiento (ya que el comando movera por distancias y no por pixels)
//   se guardara la peticion para que una vez acabada la orden actual, se 
//   procese la ULTIMA peticion realizada.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CMOVISOENGINECMD_H_
#define _CMOVISOENGINECMD_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CCOMMAND_H_
#include "CCommand.h"
#endif
#ifndef _ISODEFS_H_
#include "IsoDefs.h"
#endif
#ifndef _CLINEARINTERPOLATION_H_
#include "CLinearInterpolation.h"
#endif

// Definicion de clases / estructuras / espacios de nombres
struct iCWorld;

class CMovIsoEngineCmd: public CCommand
{
private:
  // Tipos
  typedef CCommand Inherited;

  // Estructuras  
  struct sExecuteInfo {
	// Informacion relativa al comando en ejecucion
	IsoDefs::eDirectionIndex Dir;        // Direccion del movimiento
	word	           uwSpeed;    // Velocidad de movimiento
  };

  struct sPendingInfo {
	// Informacion relativa al movimiento pendiente
	bool               bPending;  // ¿Hay movimiento pendiente?
	IsoDefs::eDirectionIndex Dir;       // Direccion del movimiento
	word               uwSpeed;   // Velocidad de movimiento
	float              fDistance; // Distancia a recorrer
  };

private:
  // Vbles de miembro
  CLinearInterpolation m_Interpolation; // Control de la interpolacion
  sExecuteInfo         m_ExInfo;        // Info del comando en ejecucion
  sPendingInfo         m_PendingInfo;   // Info sobre movimientos pendientes  
  iCWorld*             m_pWorld;        // Interfaz al motor isometico
    
public:
  // Constructor / destructor
  CMovIsoEngineCmd(void): m_pWorld(NULL) { }
  ~CMovIsoEngineCmd(void) { End(); }

public:
  // Protocolos de inicializacion / fin
  bool Init(const IsoDefs::eDirectionIndex& Dir, 
			const word uwSpeed, 
			const float fDistance);
  void End(void);

public:
  // CCommand / Ejecucion
  void Execute(const float fDelta);   
  
private:
  // Metodos de apoyo
  bool DoMovement(const float Offset);
  void PreparePendingMove(const float fActPos);
};

#endif // ~ CMovIsoEngineCmd

