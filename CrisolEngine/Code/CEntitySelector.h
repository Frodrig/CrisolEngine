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
// CEntitySelector.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases
// - CEntitySelector
//
// Descripcion:
// - Implementa una clase que representa un elemento grafico util para 
//   visualizar la seleccion de entidades mediante un selector que ira en la
//   base de estas. En el caso de seleccionar Floors, el selector ira SOBRE
//   estas.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CENTITYSELECTOR_H_
#define _CENTITYSELECTOR_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _ICEntitySelector_H_
#include "iCEntitySelector.h"
#endif
#ifndef _CSPRITE_H_
#include "CSprite.h"
#endif
#ifndef _CCYCLICFADECMD_H_
#include "CCyclicFadeCmd.h"
#endif

// Defincion de clases / estructuras / espacios de nombres
struct iCGraphicSystem;

// Clase CEntitySelector
class CEntitySelector: public CSprite,
					   public iCEntitySelector
{
private:
  // Clase base
  typedef CSprite Inherited;

private:
  // Estructuras  
  struct sAlphaFadeSelectInfo {
	// Info relativa a la seleccion con fade Alpha
	GraphDefs::sAlphaValue AlphaSource;     // Alpha de inicio
	GraphDefs::sAlphaValue AlphaSelectFade; // Alpha fade para indicar seleccion
	CCyclicFadeCmd         FadeCmd;         // Comando de Fade
  };

private:
  // Instancias a otros subsistemas
  iCGraphicSystem* m_pGraphSys; // Subsistema grafico
  
  // Resto de vbles
  sAlphaFadeSelectInfo m_FadeSelect; // Info asociada al selector de fade
  
public:
  // Constructor / destructor
  CEntitySelector(void): m_pGraphSys(NULL) { }
  ~CEntitySelector(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const std::string& szAnimTemplate);
  void End(void);
private:
  // Metodos privados
  bool InitAlphaFade(void);
  void EndAlphaFade(void);
  
public:
  // iCEntitySelector / Operacion de dibujado del selector
  void Draw(const sPosition& DrawPos);

public:
  // iCEntitySelector / Trabajo con los fades
  void SelectWithAlphaFade(const bool bSelect);
  bool IsSelectWithAlphaFade(void) const { 
	ASSERT(IsInitOk());
	// Retorna flag
	return !m_FadeSelect.FadeCmd.IsInPause();
  };
};  

#endif // ~ #ifdef _CENTITYSELECTOR_H_
