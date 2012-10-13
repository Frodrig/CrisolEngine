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
// CGUIWEntityInteract.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases
// - CGUIWEntityInteract
//
// Descripcion:
// - Implementa el menu flotante que aparece cuando se desea interactuar sobre
//   una entidad del universo de juego. El menu se activara durante el juego y
//   en la hoja del personaje. En este ultimo caso, sera para realizar operaciones
//   sobre los items que posea el jugador.
//   El menu flotante siempre se desplegara en horizontal y se ajustara en
//   pantalla en una posicion pero reajustandose si en dicha posicion
//   resulta que se sale de la pantalla por el extremo derecho.
// - Las posibles acciones seran:
//   * Basicas -> OBSERVAR | HABLAR | USAR 
//   * Extendidas -> USAR_ITEM | USAR_HABILIDAD
//   * Impuesta -> SALIR
//   Las acciones basicas se comprobaran en la entidad que recibe la accion y
//   las extendidas en la que lo produce. En el caso USAR_ITEM, se comprobara
//   si el jugador posee algun item para poder usar dicha accion. En el caso
//   de USAR_HABILIDAD lo que se comprobara es si el jugador posee alguna 
//   habilidad de uso.
//   La accion de USAR sera interpretada de forma distinta dependiendo de la
//   entidad de la que se trate:
//   * Si criatura -> REGISTRAR
//   * Si item -> RECOGER
//   * Resto -> MANIPULAR
//   En el inventario del jugador, las acciones posibles seran las de OBSERVAR,
//   TIRAR y USAR_HABILIDAD.
// - Las habilidades apareceran en un segundo menu flotante que sustituira
//   al principal.
// - En todos los casos, aparecera un texto flotante de ayuda para indicar 
//   sobre que opcion se esta trabajando.
//
// Notas:
// - Cuando en el menu de acciones principales no haya ninguna accion, no se
//   activara la interfaz.
// - Sera necesario que sea un observer de CWorld para conocer cuando se
//   produzca una notificacion asociada a la eliminacion de una entidad
//   ya que en caso de que sea la asociada se debera de cerrar la interfaz.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWENTITYINTERACT_H_
#define _CGUIWENTITYINTERACT_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabecera
#ifndef _CGUIWINDOW_H_
#include "CGUIWindow.h"
#endif
#ifndef _CGUICBUTTON_H_
#include "CGUICButton.h"
#endif
#ifndef _ICWORLD_H_
#include "iCWorld.h"
#endif
#ifndef _ICWORLDOBSERVER_H_
#include "iCWorldObserver.h"
#endif
#ifndef _ICCRIATUREOBSERVER_H_
#include "iCCriatureObserver.h"
#endif
#ifndef _CWORLDENTITY_H_
#include "CWorldEntity.h"
#endif
#ifndef _CITEM_H_
#include "CItem.h"
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif
#ifndef _VECTOR_H_
#define _VECTOR_H_
#include <vector>
#endif

// Defincion de clases / estructuras / espacios de nombres
struct iCGameDataBase;
struct iCGUIManager;
class CCriature;

// clase CGUIWEntityInteract
class CGUIWEntityInteract: public CGUIWindow,
                           public iCWorldObserver,
						   public iCCriatureObserver
{
private:
  // Tipos
  typedef CGUIWindow Inherited; // Acceso a la clase base

private:  
  // Enumerados
  enum {
	// Identificadores de opciones en menu
	// Acciones basicas
	ID_OBSERVE = 0X01,
	ID_TALK,
	ID_REGISTER,
	ID_GET, 
	ID_MANIPULATE,
	ID_MAINMENUEXIT,
	// Acciones extendidas
	ID_USEINVENTORY,
	ID_USEHABILITY,	
	// Acciones unicas para inventario
	ID_DROPITEM,		
	// Identificadores para acciones en menu de habilidades
	ID_HABILITY00,
	ID_HABILITY01,
	ID_HABILITY02,
	ID_HABILITY03,
	ID_HABILITY04,
	ID_HABILITY05,
	ID_HABILITY06,
	ID_HABILITY07,
	ID_HABILITYMENUEXIT,	
  };  

private:
  // Estructuras forward
  struct sNOption;

private:
  // Tipos
  // Map con la relacion identificador opcion, puntero a componente boton
  typedef std::map<GUIDefs::GUIIDComponent, sNOption*> OptionsMap;
  typedef OptionsMap::iterator                         OptionsMapIt;
  typedef OptionsMap::value_type                       OptionsMapValType;
  // Vector para mantener los identificadores de las acciones asociadas a un menu
  typedef std::vector<GUIDefs::GUIIDComponent> OptionsVector;
  typedef OptionsVector::iterator			   OptionsVectorIt;
  
private:
  // Estructuras
  struct sMenusInfo {
	// Info asociada a los posibles menus	
	// Menus
	OptionsVector* pActMenu;     // Direccion al menu actual
	OptionsVector  MainMenu;     // Menu de opciones principales
	OptionsVector  HabilityMenu; // Menu de habilidades		
	// Otros
	GraphDefs::sAlphaValue NoSelectAlpha;     // Alpha en opcion no seleccionada
	GraphDefs::sRGBColor   RGBFlotaingText;   // Color texto flotante
	std::string            szActFloatingText; // Texto flotante actual
  };

  struct sEntitiesInfo {
	// Info asociada a las entidades actuales
	// La entidad que produce la accion siempre sera una criatura (el jugador)
	CCriature*         pEntitySrc;    // Entidad que produce la accion
	CWorldEntity*      pEntityDest;   // Entidad que recibe la accion
	AreaDefs::sTilePos EntityDestPos; // Pos. original de la entidad destino
  };

public:
  // Vbles de miembro
  // Instancias a subsistemas
  iCWorld*        m_pWorld;      // Instancia al universo de juego
  iCGUIManager*   m_pGUIManager; // Manager de interfaz
  iCGameDataBase* m_pGDBase;     // Instancia a la base de datos

  // Info de interfaz
  OptionsMap					 m_Options;       // Map con los componentes de las opciones posibles
  sMenusInfo					 m_MenusInfo;     // Info asociada a menus 
  sEntitiesInfo					 m_EntitiesInfo;  // Info asociada a las entidades  
  GUIManagerDefs::eGUIWindowType m_ActiveContext; // Contexto de activacion

public:
  // Constructor / destructor
  CGUIWEntityInteract(void): m_pWorld(NULL), 
							 m_pGDBase(NULL),
                             m_pGUIManager(NULL) { }
  ~CGUIWEntityInteract(void) { End(); }

public:
  // Protocolos de inicializacion y fin
  bool Init(void);
  void End(void);
private:
  // Metodos de apoyo
  bool TLoad(void);
  bool CreateOption(const CGUICButton::sGUICButtonInitData& CfgButton);
  void ReleaseOptions(void);
  void ReleaseMenus(void);

public:
  // CGUIWindow / Obtencion del tipo de ventana
  inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el tipo de interfaz
	return GUIManagerDefs::GUIW_ENTITYINTERACT;
  }  

private:
  // Metodo de apoyo para la localizacion de opciones
  sNOption* const GetOption(const GUIDefs::GUIIDComponent& IDOption) {
	ASSERT(Inherited::IsInitOk());
	// Se localiza el nodo y se devuelve
	const OptionsMapIt OptionsIt(m_Options.find(IDOption));
	ASSERT((OptionsIt != m_Options.end()) != 0);	
	return OptionsIt->second;
  }

public:
  // CGUIWindow / Activacion y desactivacion de la ventana de interfaz
  void Active(const sPosition& InitDrawPos,
			  CCriature* const pEntitySrc,
			  CWorldEntity* const pEntityDest,
			  const GUIManagerDefs::eGUIWindowType& ActiveContext);  
  void Deactive(void);
private:
  // Metodos de apoyo
  void ActiveMenu(OptionsVector& Menu, const bool bActive);  
  
private:
  // Salida del interfaz teniendo en cuenta el contexto
  void ExitFromInterfaz(void);

private:
  // Metodos de creacion de menus
  void CreateInGameMainMenu(CWorldEntity* const pEntityDest);
  void CreateInInventoryMainMenu(CWorldEntity* const pEntityDest,
							     const bool bIsItemOfEntitySrc);  
  void CreateHabilityMenu(const CCriature* const pEntitySrc);
  
private:
  // Metodos de localizacion de menus
  sPosition GetCorrectInitDrawPos(const sPosition& InitDrawPos);
  void SetMenuOptionsDrawPos(OptionsVector& Menu,
							 const sPosition& InitDrawPos);
public:
  // iCGUIWindow / Notificacion de eventos
  void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent);
private:
  // Metodos de apoyo
  void OnOptionSelect(const GUIDefs::GUIIDComponent& IDOption,
					  const bool bSelect);
  void OnOptionLeftClick(const GUIDefs::GUIIDComponent& IDOption);

public:
  // CGUIWindow / Manejo de los eventos de entrada
  bool DispatchEvent(const InputDefs::sInputEvent& aInputEvent);
private:
  void InstallClient(void);
  void UnistallClient(void);

public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);

public:
  // iCCriatureObserver / Operacion de notificacion
  void CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
							  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
							  const dword udParam = 0);

public:
  // Operacion de dibujado
  void Draw(void);
}; // ~ CGUIWEntityInteract

#endif // ~ ifndef CGUIWEntityInteract