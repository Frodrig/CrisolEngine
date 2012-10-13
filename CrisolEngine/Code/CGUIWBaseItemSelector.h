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
// CGUIWBaseItemSelector.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUIWBaseItemSelector 
//
// Descripcion:
// - Clase base que representa la logica para el control de la visualizacion
//   de los items que posee un contenedor. 
// - La representacion de los items que posee el contendor se hara en una 
//   listas de listas. En la lista principal los nodos contendran listas 
//   secundarias que, a su vez, contendran los handles del contenedor. Las
//   listas secundarias tendran un tamaño maximo igual al tamaño maximo del
//   area de visualizacion de dichos items, que estara representado por una
//   matriz de mxn pero que interiormente se representara por una lista lineal
//   de componentes CGUICSpriteSelector (pues los items no dejan de ser
//   sprites).
// - Cada vez que se inserte o se elimine un item del contendor que representan,
//   la clase podra recibir una notificacion a traves de dos de sus metodos.
//   Se entendera que la mayoria de las clases que hereden de esta seran 
//   observadores del contendor que representan para que asi reciban la 
//   notificacion de la insercion / eliminacin de items en estos.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWBASEITEMSELECTOR_H_
#define _CGUIWBASEITEMSELECTOR_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabecera
#ifndef _CGUIWINDOW_H_
#include "CGUIWindow.h"
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _CGUICSPRITESELECTOR_H_
#include "CGUICSpriteSelector.h"
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif

// Definicion de clases / estructuras / espacios de nombres
class CCBTEngineParser;
struct iCItemContainer;
struct iCGFXManager;

// clase CGUIWBaseItemSelector
class CGUIWBaseItemSelector: public CGUIWindow				
{
private:
  // Tipos
  typedef CGUIWindow Inherited; // Acceso a la clase base

private:  
  // Enumerados
  enum {
	// Valores maximos para el area de seleccion de items
	MAX_AREAWIDTH  = 8,
	MAX_AREAHEIGHT = 6
  };
  
  enum {	
	// Identificadores para componentes	
	ID_SPRITESELECTOR_BASE = 0X01, 	// Identificador inicial para los componentes
  };

protected:
  enum {
	// Identificador maximo para componentes
	ID_SPRITESELECTOR_MAX = MAX_AREAWIDTH * MAX_AREAHEIGHT
  };

private:
  // Clases forward
  class sFindIfColumPredicate;
  class sFindIfVisibleItemPredicate;

private:
  // Estructuras forward
  struct sNItemColum;
  struct sNItemFile;
  struct sNVisualizableItem;  

private:
  // Tipos
  // Listas que representan las filas en el mapeo de los items
  typedef std::list<sNItemFile*> FileList;
  typedef FileList::iterator     FileListIt;
  // Listas que representan los nodos de columna
  typedef std::list<sNItemColum*> ColumList;
  typedef ColumList::iterator     ColumListIt;
  // Lista a los items visualizables en pantalla
  typedef std::list<sNVisualizableItem*> VisualizableItemList;
  typedef VisualizableItemList::iterator VisualizableItemListIt;

private:
  // Estructuras
  struct sItemsIt {
	// Iteradores para saber a partir de que fila de la lista de lista
	// de items que hay en el contenedor, estamos viendo
	// Enumerados
	enum {
	  NO_ITEMS = -1
	};
	// Datos
	FileListIt FileIt;    // Iterador a lista
	sword      swNumFile; // Numero de fila asociado al iterador
  };

  struct sInterfazInfo {
	// Info asociada a la interfaz
	FileList             Items;             // Items en contenedor
	VisualizableItemList VisualizableItems; // Items actualmente visualizables
	sItemsIt             ItemsIt;           // Iteradores a items en contenedor
	word                 uwAreaWidth;       // Ancho del area
	word                 uwAreaHeight;      // Alto del area
  };

  struct sFloatingTextInfo {
	// Info asociada al texto flotante
	bool                              bShowNames; // ¿Se mostraran los nombres de los items?
	GraphDefs::sRGBColor              RGBColor;   // Color del texto
	GraphDefs::sAlphaValue            Alpha;      // Alpha asociado al texto
	AreaDefs::EntHandle               hActItem;   // Item del que se esta mostrando nombre
	GUIManagerDefs::eFloatingTextType Type;       // Tipo de formato a usar
  };

private:
  // Instancias a los distintos modulos
  iCGFXManager* m_pGFXManager; // Manager de GFXs

  // Resto de vbles de miembro
  sInterfazInfo     m_InterfazInfo;     // Info asociada a la interfaz
  sFloatingTextInfo m_FloatingTextInfo; // Info asociada al nombre de interfaz
  
public:
  // Constructor / destructor  
  CGUIWBaseItemSelector(void): m_pGFXManager(NULL) { }
  ~CGUIWBaseItemSelector(void) { End(); }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(void);
  void End(void);
protected:
  // Metodos de apoyo  
  bool TLoad(CCBTEngineParser* const pParser,
		     const byte ubDrawPlane,			 
			 const bool bSelectOnlyWithItem = true,
			 const bool bShowItemNames = true);
private:
  bool TLoadItemSelectors(CCBTEngineParser* const pParser,
					      const byte ubDrawPlane,
						  const bool bSelectOnlyWithItem);  
  bool TLoadFloatingTextInfo(CCBTEngineParser* const pParser,
							 const bool bShowItemNames);
private:
  void ReleaseItemsList(void);
  void ReleaseVisualizableItemList(void);

private:
  // Trabajo con el texto flotante para la descripcion de nombres
  void SetItemName(const AreaDefs::EntHandle& hItem,
				   const bool bSet);
  
public:
  // CGUIWindow / Activacion y desactivacion de la ventana de interfaz
  void Active(iCItemContainer* const pItemContainer,
			  iCGUIWindowClient* const pClient);  
  void Deactive(void);
private:
  // Metodos de apoyo
  void ActiveVisualizableSelectors(const bool bActive);
  void CreateItemList(iCItemContainer* const pItemContainer);
  void MapVisualizableItems(void);
  void SetItemsInSelectorsVisibles(const bool bVisible);

public:
  // Activacion / desactivacion de la entrada
  void SetInput(const bool bActive);

public:
  // Scroll sobre los items visualizables
  bool NextItemFile(void);
  bool PrevItemFile(void);

protected:
  // Notificaciones de insercion y eliminacion de un item
  void ItemInserted(const AreaDefs::EntHandle& hItem);
  void ItemRemoved(const AreaDefs::EntHandle& hItem);

public:
  // iCGUIWindow / Notificacion de eventos
  void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent);
private:
  // Metodos de apoyo
  void FindVisibleItemNode(const GUIDefs::GUIIDComponent& IDComponent,
						   VisualizableItemListIt& It);
protected:
  // Notificacion de que un item ha sido cliqueado con y sin item asociado
  // nota: Ojo, no poner referencias.
  virtual void OnItemLeftClick(const AreaDefs::EntHandle hItem,
							   CGUICSpriteSelector& Selector) = 0;
  virtual void OnItemRightClick(const AreaDefs::EntHandle hItem,
								CGUICSpriteSelector& Selector) = 0;

protected:
  // Dibujado de selectores
  void Draw(void);
}; // ~ CGUIWBaseItemSelector

#endif // ~ ifndef CGUIWBASEITEMSELECTOR