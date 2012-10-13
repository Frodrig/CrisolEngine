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
// CGUIWBaseWindowContainer.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUIWBaseWindowContainer 
//
// Descripcion:
// - Representa una ventana base para todas aquellas que quieran mostrar 
//   el contenido de un contenedor desde el punto de vista de unas reglas
//   esteticas relativas a que todas las interfaces que hereden de esta tendran
//   una imagen de fondo y unos botones de scroll.
// - Esta interfaz heredara a su vez de CGUIWBaseItemSelector.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWBASEWINDOWCONTAINER_H_
#define _CGUIWBASEWINDOWCONTAINER_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabecera
#ifndef _CGUIWBASEITEMSELECTOR_H_
#include "CGUIWBaseItemSelector.h"
#endif
#ifndef _CGUICBITMAP_H_
#include "CGUICBitmap.h"
#endif
#ifndef _CGUICBUTTON_H_
#include "CGUICButton.h"
#endif

// Definicion de clases / estructuras / espacios de nombres

// clase CGUIWBaseWindowContainer
class CGUIWBaseWindowContainer: public CGUIWBaseItemSelector
{
private:
  // Tipos
  typedef CGUIWBaseItemSelector Inherited; // Acceso a la clase base

protected:  
  // Enumerados
  enum {	
	// Identificadores para componentes	
	ID_BACKIMG = Inherited::ID_SPRITESELECTOR_MAX + 1,
	ID_UPSCROLL,
	ID_DOWNSCROLL
  };

private:
  // Estructuras    
  struct sBackImgInfo {
	// Datos asociados a la imagen de fondo
	CGUICBitmap BackImg; // Imagen de fondo
  };

  struct sScrollInfo {
	// Datos asociados a los botones de scroll
	CGUICButton UpButton;   // Boton subir
	CGUICButton DownButton; // Boton bajar
  };
  
private:
  // Componentes
  sBackImgInfo m_BackImgInfo; // Imagen de fondo
  sScrollInfo  m_ScrollInfo;  // Botones de scroll  
    
public:
  // Constructor / destructor  
  CGUIWBaseWindowContainer(void) { }
  ~CGUIWBaseWindowContainer(void) { 
	End(); 
  }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(void);
  void End(void);
protected:
  // Metodos de apoyo  
  bool TLoad(CCBTEngineParser* const pParser, 
		     const byte ubDrawPlane,
			 const bool bReadBackImgAlpha);
private:
  bool TLoadBackImgInfo(CCBTEngineParser* const pParser,
					    const byte ubDrawPlane,
						const bool bReadBackImgAlpha);
  bool TLoadScrollInfo(CCBTEngineParser* const pParser,
					   const byte ubDrawPlane);    
  
public:
  // CGUIWindow / Activacion y desactivacion de la ventana de interfaz
  void Active(iCItemContainer* const pItemContainer,
			  iCGUIWindowClient* const pClient);  
  void Deactive(void);

public:
  // Activacion / desactivacion de la entrada
  void SetInput(const bool bActive);

public:
  // iCGUIWindow / Notificacion de eventos
  void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent);
private:
  // Metodos de apoyo  
  void OnBaseWindowSelect(const GUIDefs::GUIIDComponent& IDComponent,
						  const bool bSelect);
  void OnBaseWindowLeftClick(const GUIDefs::GUIIDComponent& IDComponent);

protected:
  // Dibujado de componentes
  void Draw(void);
}; // ~ CGUIWBaseWindowContainer

#endif // ~ ifndef CGUIWBaseWindowContainer