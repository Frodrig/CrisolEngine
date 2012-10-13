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
// CGUIWItemSelectToUse.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUIWItemSelectToUse
//
// Descripcion:
// - Interfaz utilizada para mostrar el inventario del jugador en el momento
//   en que este decida usar un objeto de su inventario sobre una entidad.
// - La clase heredara de CGUIWBaseWindowContainer.
//
// Notas:
// - La clase sera observer de CWorld para conocer si se destruye la entidad
//   sobre la que se desea usar un item. Cuando esto ocurra, la ventana se
//   cerrara automaticamente.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWITEMSELECTTOUSE_H_
#define _CGUIWITEMSELECTTOUSE_H_

// Cabecera
#ifndef _CGUIWBASEWINDOWCONTAINER_H_
#include "CGUIWBaseWindowContainer.h"
#endif
#ifndef _ICWORLDOBSERVER_H_
#include "iCWorldObserver.h"
#endif

// Definicion de clases / estructuras / espacios de nombres
class CCriature;
class CWorldEntity;
class CCBTEngineParser;

// clase CGUIWItemSelectToUse
class CGUIWItemSelectToUse: public CGUIWBaseWindowContainer,
							public iCWorldObserver
{
private:
  // Tipos
  typedef CGUIWBaseWindowContainer Inherited; // Acceso a la clase base

private:  
  // Enumerados
  enum {	
	ID_CANCEL = Inherited::ID_DOWNSCROLL + 1	
  };

private:
  // Estructuras
  struct sCancelInfo {
	// Info asociada al boton de cancelacion
	CGUICButton Cancel; // Cancelacion
  };

  struct sEntitiesInfo {
	// Info asociada a las entidades involucradas
    CCriature*    pMainEntity; // Entidad principal que sera seguro una criatura
	CWorldEntity* pDestEntity; // Entidad destino que recibe la accion
  };


private:
  // Vbles de miembro
  sCancelInfo   m_CancelInfo;   // Boton de cancelacion
  sEntitiesInfo m_EntitiesInfo; // Entidades involucradas
  
public:
  // Constructor / destructor  
  CGUIWItemSelectToUse(void) { }
  ~CGUIWItemSelectToUse(void) { End(); }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(void);
  void End(void);
private:
  // Metodos de apoyo
  bool TLoad(void);
  bool TLoadCancelInfo(CCBTEngineParser* const pParser);
  void EndComponents(void); 

public:
  // CGUIWindow / Obtencion del tipo de ventana
  inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el tipo de interfaz
	return GUIManagerDefs::GUIW_ITEMSELECTTOUSE;
  }

public:
  // CGUIWindow / Activacion y desactivacion de la ventana de interfaz
  void Active(CCriature* const pMainEntity,
			  CWorldEntity* const pDestEntity);  
  void Deactive(void);
private:
  // Metodos de apoyo
  void ActiveComponents(const bool bActive);

public:
  // iCGUIWindow / Notificacion de eventos
  void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent);
private:
  // Metodos de apoyo
  void OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
				const bool bSelect);
  void OnLeftClick(const GUIDefs::GUIIDComponent& IDComponent);
protected:
  // Notificacion de que un item ha sido cliqueado con y sin item asociado
  // nota: Ojo, no poner referencias.
  void OnItemLeftClick(const AreaDefs::EntHandle hItem,
					   CGUICSpriteSelector& Selector);
  void OnItemRightClick(const AreaDefs::EntHandle hItem,
						CGUICSpriteSelector& Selector) { }

public:
  // iCGUIWindow / Manejo de los eventos de entrada  
  bool DispatchEvent(const InputDefs::sInputEvent& InputEvent);
private:
  void InstallClient(void);
  void UnistallClient(void);

public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);

public:
  // Operacion de dibujado
  void Draw(void);
};

#endif // ~ ifndef CGUIWItemSelectToUse