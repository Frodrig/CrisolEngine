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
// CCinemaCurtain.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CCinemaCurtain
//
// Descripcion:
// - Esta clase simula el efecto de cine en pantalla. Se modela como una
//   clase en lugar de embeberla como un efecto FX en el CGraphicSystem porque
//   sera manejada por el CGUIManager y ademas, necesitara de una manipulacion
//   via comando que notifique cuando el cine ha acabado / comenzado para
//   habilitar / deshabilitar la entrada. 
//   Graficamente, con este efecto se lograra poner en la parte superior e
//   inferior los bloques negros que den sensacion cinematografica a la escena
//   que se esta dibujado. La forma en que seran puestos estos bloques sera
//   utilizando animacion, de ahi la necesidad de utilizar un comando que
//   actualice la posicion de los bloques Quad embebidos en esta clase.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CCINEMACURTAIN_H_
#define _CCINEMACURTAIN_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif

// Definicion de clases / estructuras / espacios de nombres
struct iCGraphicSystem;

// Clase CCinemaCurtain
class CCinemaCurtain
{
private:
  // Enumerados
  enum eCurtainState {
	// Estados posibles para la cortina
	OPEN_STATE = 0,
	OPENING_STATE,
	CLOSED_STATE,
	CLOSING_STATE
  };

private:
  // Estructuras
  struct sCurtainInfo; // Forward

private:  
  // Interfaces a otros modulos
  iCGraphicSystem* m_pGraphicSys; // Interfaz al subsistema grafico

  // Info asociada a las cortinas
  sCurtainInfo*        m_pTopCurtain;         // Info asociada a la cortina superior
  sCurtainInfo*        m_pBottomCurtain;      // Info asociada a la cortina inferior
  byte                 m_ubTopHeightCurtain;  // Altura de la cortina sup.
  byte                 m_ubBaseHeightCurtain; // Altura de la cortina inf.
  eCurtainState        m_State;               // Estado en que se encuentra la cortina
  GraphDefs::sRGBColor m_CurtainColor;        // Color de cortina <negro>
  bool                 m_bIsInitOk;           // ¿Se ha inicializado correctamente?
 
public:
  // Constructor / destructor
  CCinemaCurtain(void): m_pGraphicSys(NULL),
						m_pTopCurtain(NULL),
						m_pBottomCurtain(NULL),
						m_bIsInitOk(false) { }
  ~CCinemaCurtain(void) { End(); }

public:
  // Protocolos de inicializacion / fin
  bool Init(const byte ubTopHeightCurtain,
		    const byte ubBaseHeightCurtain);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // Manipulacion de la cortina
  bool OpenCurtain(const word uwStepScroll);
  bool CloseCurtain(const word uwStepScroll);

public:
  // Operacion de dibujado
  void Draw(void);

public:
  // Obtencion de informacion
  inline bool IsOpen(void) const {
	ASSERT(IsInitOk());
	// Retorna flag de apertura
	return (CCinemaCurtain::OPEN_STATE == m_State);
  }
  inline bool IsClosed(void) const {
	ASSERT(IsInitOk());
	// Retorna flag de cierre
	return (CCinemaCurtain::CLOSED_STATE == m_State);
  }
};

#endif // ~ CCinemaCurtain

