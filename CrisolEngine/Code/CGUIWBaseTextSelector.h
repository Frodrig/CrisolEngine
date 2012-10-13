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
// CGUIWBaseTextSelector.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases
// - CGUIWBaseTextSelector
//
// Descripcion:
// - Clase base para encargarse del tratamiento de las ventanas que deban de
//   gestionar una lista de opcioens de texto seleccionables. Internamente,
//   funcionara en tres estados:
//    * Por un lado debera de activarse. Al activarse no tendra asociada 
//      ninguna opcion de seleccion, por lo que se dibujara vacia (sin nada)
//    * En segundo lugar, vendra el proceso de solicitud de insercion de 
//      opciones. Mientras se inserte NO seran vistas las opciones insertadas
//      sino que seran almacenadas.
//    * En tercer lugar, vendra la orden de pregunta al jugador por una opcion
//      de las que se insertaron. En este caso las opcione seran dibujadas en
//      pantalla a la espera de que el jugador escoja una.
//   Cuando el jugador escoja una opcion, se ejecutara un metodo virtual
//   que se podra implementar en la clase base PERO SIEMPRE debera de ser
//   propagado a esta superior pues aqui se realizaran una serie de pasos
//   muy importantes que dejaran todos como cuando la clase fue activada.
// - Los valores para los identificadores de opciones deberan de estar entre
//   1 y MAX_IDOPTION_VALUE de lo contrario seran rechazadas.
//
// Notas:
// - Clase abstracta
// - Para añadir una opcion no hara falta que se encuentre activa.
// - Los componentes de texto para representar las opciones, seran creados
//   estaticamente por motivos de violacion de memoria, ya que si se usa
//   el modelo dinamico al finalizar la clase necesitaran destruirse sin 
//   tener la seguridad de que se este fuera del codigo de los mismos. Por
//   ejemplo, ante una llamada Dispatch de un componente, podra cerrarse una
//   interfaz, destruyendose los mismos, incluido el que origino la llamada.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWBASETEXTSELECTOR_H_
#define _CGUIWBASETEXTSELECTOR_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabecera
#ifndef _CGUIWINDOW_H_
#include "CGUIWindow.h"
#endif
#ifndef _CGUICSINGLETEXT_H_
#include "CGUICSingleText.h"
#endif
#ifndef _CFXBACK_H_
#include "CFXBack.h"
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif
#ifndef _VECTOR_H_
#define _VECTOR_H_
#include <vector>
#endif

// Definicion de clases / estructuras / espacios de nombres
class CCBTEngineParser;

// clase CGUIWBaseTextSelector
class CGUIWBaseTextSelector: public CGUIWindow
{  
private:
  // Tipos
  typedef CGUIWindow Inherited; // Acceso a la clase base

protected:
  // enumerados / constantes
  enum {
	// Valor maximo para los identificadores de opciones
	MAX_IDOPTION_VALUE = 256
  };

private:
  // Estructuras
  struct sOption {
	// Solicitud de opcion
	std::string szText;     // Texto asociado
	bool        bCanSelect; // ¿Puede seleccionarse?
	// Constructores
	sOption(const std::string& aszText,
			const bool abCanSelect): szText(aszText),
									  bCanSelect(abCanSelect) { }
	sOption(const sOption& Option) {
	  szText = Option.szText;
	  bCanSelect = Option.bCanSelect;
	}
  };

private:
  // Tipos
  // Map de las opciones solicitadas para ser añadidas
  typedef std::map<GUIDefs::GUIIDComponent, sOption> OptionsMap;
  typedef OptionsMap::value_type                     OptionsMapValType;
  typedef OptionsMap::iterator                       OptionsMapIt;
  // Vector con los componentes de texto
  //typedef std::vector<CGUICSingleText>  TextComponentVector;
  //typedef TextComponentVector::iterator TextComponentVectorIt;
  // Lista de opciones de texto a elegir
  typedef std::list<word>       OptionsList;
  typedef OptionsList::iterator OptionsListIt;

private:
  // Estructuras
  struct sOptionsInfo {	
	// Info asociada a las opciones de texto establecidas	
	OptionsList Options; // Lista de opciones de texto
	CFXBack     FXBack;  // Fx de fondo
	
	// Iteradores para el control de las opciones
	OptionsListIt FirstViewOptionIt; // Iterador a primera opcion visible
	OptionsListIt LastViewOptionIt;  // Iterador a ultima opcion visible
	OptionsListIt LastOptionIt;      // Iterador a ultima opcion
	OptionsListIt EndTopIt;          // Iterador "end" desde FirstViewOptionIt;
	
	// Configuracion base utilizada para la inicializacion de opciones	
	CGUICSingleText::sGUICSingleTextInitData CfgOptions;    // Para texto
	word                                     uwNumMaxLines; // Max. lineas	
  };

private:
  // Vbles de miembro    
  // Componentes
  CGUICSingleText m_TextComponents[MAX_IDOPTION_VALUE];

  // Resto
  OptionsMap   m_OptionsToAdd;     // Opciones a añadir	  
  sOptionsInfo m_Options;          // Info asociada a las opciones de texto
  bool         m_bInGetOptionMode; // ¿Se esta preguntando por opciones?  
  
public:
  // Constructor / destructor  
  CGUIWBaseTextSelector(void): m_bInGetOptionMode(false) { }
  ~CGUIWBaseTextSelector(void) { End(); }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(void);
  void End(void);
protected:
  // Metodos de apoyo  
  bool TLoadTextOptionsInfo(CCBTEngineParser* const pParser,
						    const byte ubDrawPlane);  

public:
  // Trabajo con la configuracion y mantenimiento de las opciones de texto
  bool AddOption(const GUIDefs::GUIIDComponent& IDOption,
				 const std::string& szText,
				 const bool bCanSelect = true);  
  bool GetOption(iCGUIWindowClient* const pClient);
  inline void ReleaseOptions(void) {
	ASSERT(Inherited::IsInitOk());
	// Vacia el map de opciones solicitadas para ser añadidas
	m_OptionsToAdd.clear();
  }
  inline word GetNumOptions(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna el numero de opciones registradas
	return m_OptionsToAdd.size();
  }
  
protected:
  // Metodos de control para clases base
  bool NextOption(void);
  bool PrevOption(void);  
private:
  // Metodos de apoyo
  void InitIterators(void);  
  void SetLastViewOptionIt(void);
  void ActiveVisibleOptions(const bool bActive);    
  void ReleaseOptionList(void);
  void SetEndTopIt(void) {
	ASSERT(Inherited::IsInitOk());
	// Se localiza iterador tope final
	// Consultar nota en InitIterators para mas informacion
	m_Options.EndTopIt = m_Options.LastViewOptionIt;
	std::advance(m_Options.EndTopIt, 1);
  }

public:
  // CGUIWindow / Activacion y desactivacion de la ventana de interfaz
  void Active(void);  
  void Deactive(void);

public:
  // Activacion / desactivacion de la entrada
  void SetInput(const bool bInput);

public:
  // iCGUIWindow / Notificacion de eventos
  void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent);
private:
  // Metodos de apoyo  
  void OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
			    const bool bSelect);
protected:
  // Notificacion de la pulsacion de una opcion
  virtual void OnTextOptionLeftClick(const GUIDefs::GUIIDComponent& IDComponent);

protected:
  // Operacion de dibujado
  void DrawTextOptions(void);
};

#endif // ~ ifndef CGUIWBaseTextSelector