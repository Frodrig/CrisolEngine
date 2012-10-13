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
// CFXBack.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases
// - CFXBack
//
// Descripcion:
// - Representa una clase que esta compuesta por un Quad de unas determinadas
//   dimensiones con un color determinado y con un valor alpha tambien
//   determinado y todos asociados a los cuatro vertices. Este quad se 
//   utilizara como efecto especial para construir fondos para que los textos
//   que se pongan encima de ellos resalten mejor y para situaciones
//   esteticas similares.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CFXBACK_H_
#define _CFXBACK_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabecera
#ifndef _ICGRAPHICSYSTEM_H_
#include "iCGraphicSystem.h"
#endif
#ifndef _SYSENGINE_H_
#include "SYSEngine.h"
#endif
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _CQUADFORM_H_
#include "CQuadForm.h"
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif

// Definicion de clases / estructuras / espacios de nombres
struct iCGraphicSystem;

// clase CFXBack
class CFXBack 
{
private:
  // Vbles de miembro
  sPosition              m_DrawPos;     // Posicion de dibujado
  byte                   m_ubDrawPlane; // Plano de dibujado
  CQuadForm              m_Quad;        // Quad
  GraphDefs::sAlphaValue m_Alpha;       // Valor alpha asociado
  GraphDefs::sRGBColor   m_RGBColor;    // Color RGB asociado
  bool                   m_bIsInitOk;   // ¿Clase inicializa correctamente?

public:
  // Constructor / destructor
  CFXBack(void): m_bIsInitOk(false) { }
  ~CFXBack(void) { End(); }

public:
  // Protocolos de inicio y fin
  bool Init(const sPosition DrawPos,
			const byte ubDrawPlane,
			const word uwWidth, 
			const word uwHeight, 
			const GraphDefs::sRGBColor& RGBColor = GraphDefs::sRGBColor(255, 255, 255), 
			const GraphDefs::sAlphaValue& Alpha = GraphDefs::sAlphaValue(255));
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // Operacion de dibujado
  inline void Draw(void) {
	ASSERT(IsInitOk());
	// Realiza el dibujado
	SYSEngine::GetGraphicSystem()->Draw(GraphDefs::DZ_GUI,
									    m_ubDrawPlane,
									    m_DrawPos.swXPos,
									    m_DrawPos.swYPos,
									    &m_Quad);
  }

public:
  // Obtencion de dimensiones
  inline word GetWidth(void) {
	ASSERT(IsInitOk());
	// Retorna anchura
	return m_Quad.GetWidth();
  }
  inline word GetHeight(void) {
	ASSERT(IsInitOk());
	// Retorna altura
	return m_Quad.GetHeight();
  }

public:
  //  Modificacion de la posicion
  inline void SetPosition(const sPosition& DrawPos) {
	ASSERT(IsInitOk());
	// Cambia la posicion
	m_DrawPos = DrawPos;
  }
}; // ~ CFXBack

#endif