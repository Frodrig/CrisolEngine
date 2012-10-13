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
// CGUIWCombat.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUIWCombat 
//
// Descripcion:
// - Muestra la informacion relativa a la entidad que posee el turno de
//   combate. La informacion sera relativa a mostrar los puntos de accion
//   que posee dicha criatura, enseñando una barra de porcentaje.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWCOMBAT_H_
#define _CGUIWCOMBAT_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabecera
#ifndef _CGUIWINDOW_H_
#include "CGUIWindow.h"
#endif
#ifndef _CGUICLINETEXT_H_
#include "CGUICLineText.h"
#endif
#ifndef _CGUICPERCENTAGEBAR_H_
#include "CGUICPercentageBar.h"
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _CFXBACK_H_
#include "CFXBack.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif

// Definicion de clases / estructuras / espacios de nombres
class CCBTEngineParser;
class CCriature;

// clase CGUIWCombat
class CGUIWCombat: public CGUIWindow
{
private:
  // Tipos
  typedef CGUIWindow Inherited; // Acceso a la clase base

private:  
  // Enumerados
  enum {	
	// Identificadores para componentes
	ID_TEXTINFO = 0x01,
	ID_ACTIONPOINTSPECENTAGE
  };
 
private:
  // Estructuras  
  struct sCriatureInfo {
	// Estructuras
	struct sActionPoints {	  
	  CGUICPercentageBar Percentage; // Informacion en porcentaje
	};
	// Info asociada a la criatura
	// Componentes de informacion
	sActionPoints ActionPoints; // Puntos de accion
	CGUICLineText TextInfo;     // Texto con el nombre y los ptos de accion
	CFXBack       FXBack;       // FXBack para situar la barra de porcentaje
	// Resto
	CCriature* pCriature;  // Instancia a la criatura
	// Constructor
	sCriatureInfo(void): pCriature(NULL) { }
  };  

private:   
  // Info asociada a los componentes
  sCriatureInfo m_CriatureInfo; // Info asociada a la criatura
      
public:
  // Constructor / destructor  
  CGUIWCombat(void) { }
  ~CGUIWCombat(void) { End(); }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(void);
  void End(void);
private:
  // Metodos de apoyo para la carga de componentes
  bool TLoad(void);
  bool TLoadCriatureInfo(CCBTEngineParser* const pParser);
  void EndComponents(void);

public:
  // Obtencion del tipo de ventana
  inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el tipo de Window
	return GUIManagerDefs::GUIW_COMBAT;
  }

public:
  // CGUIWindow / Activacion y desactivacion de la ventana de interfaz
  void Active(const AreaDefs::EntHandle& hCriature);	
  void Deactive(void);
private:
  // Metodos de apoyo
  void ActiveComponents(const bool bActive);

public:
  // Establecimiento de la criatura
  void SetCriature(const AreaDefs::EntHandle& hCriature);

public:
  // CGUIWindow / Activacion / desactivacion de la entrada
  void SetInput(const bool bInput);

public:
  // Trabajo con la modificacion de los puntos de combate
  void SetActionPoints(const RulesDefs::CriatureActionPoints& ActionPoints);

public:
  // Operacion de dibujado
  void Draw(void);
}; // ~ CGUIWADVICEDialog

#endif

