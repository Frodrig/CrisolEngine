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
// CGUICInput.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUICInput
//
// Descripcion:
// - Define un componente capaz de recoger la entrada desde el exterior, 
//   preguntando al usuario.
//
// Notas:
// - La entrada se utilizara instalando este componente, en concreto, como
//   un cliente de entrada para caracteres.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUICINPUT_H_
#define _CGUICINPUT_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CGUICOMPONENT_H_
#include "CGUIComponent.h"
#endif
#ifndef _RESDEFS_H_
#include "ResDefs.h"
#endif
#ifndef _ICALARMCLIENT_H_
#include "iCAlarmClient.h"
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CGUICInput
class CGUICInput: public CGUIComponent,
				  public iCAlarmClient
{
private:
  // Tipos
  typedef CGUIComponent Inherited; // Acceso a la clase base

public:
  // Estructuras
  struct sGUICInputInitData: public CGUIComponent::sGUIComponentInitData {
	// Inicializacion de CGUICInput
	// Datos	
	// General
	word uwMaxCharacters; // Num. maximo de caracteres
	// Texto
	std::string          szFont;          // Fuente
	GraphDefs::sRGBColor RGBTextSelect;   // Color texto seleccionado
	GraphDefs::sRGBColor RGBTextUnselect; // Color texto no seleccionado
	std::string          szInitText;      // Texto inicial asociado
  };

private:
  // Estructuras
  struct sInputInfo {
	// Informacion sobre el componente de boton		
	// Datos asociados a la informacion recogida
	std::string szActText;            // Input actual
	std::string szTmpText;            // Input temporal, antes de entrada	
	word        uwInputCursorPos;     // Pos. del cursor de insercion
	word        uwPrevInputCursorPos; // Pos. del cursor de ins. antes de entrada
	word        uwMaxCharacters;      // Num. maximo de caracteres
	std::string szFont;               // Fuente a utilizar para el texto
	// Info de colores	
	GraphDefs::sRGBColor   RGBTextSelect;    // Texto seleccionado
	GraphDefs::sRGBColor   RGBTextUnselect;  // Texto no seleccionado
	GraphDefs::sRGBColor*  pRGBActTextColor; // Color de texto actual	
  };

private:
  // Vbles de miembro
  sInputInfo			 m_InputInfo;       // Info asociada al boton
  AlarmDefs::AlarmHandle hEndMarkAlarm;     // Handle a alarma de marca fin de entrada
  bool					 bIsSelect;	        // ¿Seleccionado?		
  bool					 bIsAcquiringInput; // ¿Se esta obteniendo texto del usuario?
  bool					 bIsInputActive;    // ¿Entrada activada?

public:
  // Constructor / destructor
  CGUICInput(void) { }
  ~CGUICInput(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const sGUICInputInitData& InitData);
  void End(void);

public:
  // CGUIComponent / Activacion / desactivacion de la entrada
  void SetInput(const bool bActive);

public:
  // CGUIComponent / Dibujado
  void Draw(void);

public:
  // CGUICComponent / Trabajo con el area del componente
  word GetWidth(void);
  word GetHeight(void);

public:
  // CGUIComponent / Operaciones sobre el estado de la seleccion
  void Select(const bool bSelect);    
  inline bool IsSelect(void) const { 
	ASSERT(Inherited::IsInitOk());
	// ¿Boton seleccionado?
	return bIsSelect;
  }   

public:
  // CGUIComponent / Trabajo con los eventos de entrada
  bool DispatchEvent(const InputDefs::sInputEvent& aInputEvent);
private:
  // CGUIComponent / Metodos de apoyo
  void InstallClient(void);    
  void UnistallClient(void);

public:
  // iCAlarmClient / Notificacion de la finalizacion de una alarma
  void AlarmNotify(const AlarmDefs::eAlarmType& AlarmType,
				   const AlarmDefs::AlarmHandle& hAlarm);

public:
  // Obtencion de datos por parte del usuario
  void AcquireInput(const bool bAcquire);
  inline bool IsAcquiringInput(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna flag
	return bIsAcquiringInput;
  }

public:
  // Trabajo con el texto recogido
  void SetActInput(const std::string& szActText);
  inline std::string GetActInput(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna texto introducido
	return m_InputInfo.szActText;
  }
  inline bool IsInputInserted(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna flag
	return m_InputInfo.uwInputCursorPos ? true : false;
  }

private:
  // Metodos de apoyo
  void OnNewCharacterEvent(const InputDefs::sInputCharInfo& CharInfo);

}; // ~ CGUICInput

#endif _CGUICINPUT_H_
