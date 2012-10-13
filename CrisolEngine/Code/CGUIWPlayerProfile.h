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
// CGUIWPlayerProfile.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases
// - CGUIWPlayerProfile
//
// Descripcion:
// - Interfaz que gestiona la ficha del personaje, el inventario, los slots 
//   de equipamiento y demas informaciones importantes para el control del 
//   personaje. Este interfaz se desplegara cuando el jugador pulse sobre
//   el retrato del jugador.
//
// Notas:
// - Sera necesario que se instale como observer de CWorld para poder
//   conocer si un item es destruido, en cuyo caso podra suceder que si 
//   estaba seleccionado con el cursor debera de desvincularse completamente 
//   del mismo y retornar al cursor normal (en el resto de los casos habra
//   observers preparados).
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWPLAYERPROFILE_H_
#define _CGUIWPLAYERPROFILE_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabecera
#ifndef _CGUIWBASEITEMSELECTOR_H_
#include "CGUIWBaseItemSelector.h"
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _RULESDEFS_H_
#include "RulesDefs.h"
#endif
#ifndef _CGUICBLOCKTEXT_H_
#include "CGUICBlockText.h"
#endif
#ifndef _CGUICBUTTON_H_
#include "CGUICButton.h"
#endif
#ifndef _CGUICLINETEXT_H_
#include "CGUICLineText.h"
#endif
#ifndef _CGUICPERCENTAGEBAR_H_
#include "CGUICPercentageBar.h"
#endif
#ifndef _CGUICANIMBITMAP_H_
#include "CGUICAnimBitmap.h"
#endif
#ifndef _CGUICSPRITESELECTOR_H_
#include "CGUICSpriteSelector.h"
#endif
#ifndef _ICITEMCONTAINER_H_
#include "iCItemContainer.h"
#endif
#ifndef _ICEQUIPMENTSLOT_H_
#include "iCEquipmentSlots.h"
#endif
#ifndef _ICCRIATUREOBSERVER_H_
#include "iCCriatureObserver.h"
#endif
#ifndef _ICWORLDOBSERVER_H_
#include "iCWorldObserver.h"
#endif

// Defincion de clases / estructuras / espacios de nombres
struct iCWorld;
struct iCGameDataBase;
struct iCInputManager;
struct iCGUIManager;
struct iCGraphicSystem;
class CSprite;
class CItem;
class CCBTEngineParser;

// clase CGUIWPlayerProfile
class CGUIWPlayerProfile: public CGUIWBaseItemSelector,
						  public iCItemContainerObserver,
						  public iCEquipmentSlotsObserver,
						  public iCCriatureObserver,
						  public iCWorldObserver
{
private:
  // Tipos
  // Acceso a clase base
  typedef CGUIWBaseItemSelector Inherited;
  
private:
  // Enumerados
  enum {
	// Identificador de fade
	ID_ENDFADEIN = 1 // Fin del FadeIn
  };

  enum eIDComponents {
	// Identificadores de los componentes	
	// Botones de scroll para el inventario
	ID_INV_BUTTON_UP = Inherited::ID_SPRITESELECTOR_MAX + 1,
	ID_INV_BUTTON_DOWN, 
	// Consola
	ID_CONSOLE,
	ID_CONSOLE_BUTTON_UP,
	ID_CONSOLE_BUTTON_DOWN,
	// Iconos de sintomas
	ID_SYMPTOM_00,
	ID_SYMPTOM_01,
	ID_SYMPTOM_02,
	ID_SYMPTOM_03,
	ID_SYMPTOM_04,
	ID_SYMPTOM_05,
	ID_SYMPTOM_06,
	ID_SYMPTOM_07,
	ID_SYMPTOM_08,
	ID_SYMPTOM_09,
	ID_SYMPTOM_10,
	ID_SYMPTOM_11,
	ID_SYMPTOM_12,
	ID_SYMPTOM_13,
	ID_SYMPTOM_14,
	ID_SYMPTOM_15,	
	// Slots de equipamiento
	ID_EQUIPMENTSLOT_00,
	ID_EQUIPMENTSLOT_01,
	ID_EQUIPMENTSLOT_02,
	ID_EQUIPMENTSLOT_03,
	ID_EQUIPMENTSLOT_04,
	ID_EQUIPMENTSLOT_05,	
	ID_EQUIPMENTSLOT_06,
	ID_EQUIPMENTSLOT_07,	
	// Nombres de los slots de equipamiento
	ID_EQUIPMENTSLOT_NAME_00,
	ID_EQUIPMENTSLOT_NAME_01,
	ID_EQUIPMENTSLOT_NAME_02,
	ID_EQUIPMENTSLOT_NAME_03,
	ID_EQUIPMENTSLOT_NAME_04,
	ID_EQUIPMENTSLOT_NAME_05,	
	ID_EQUIPMENTSLOT_NAME_06,
	ID_EQUIPMENTSLOT_NAME_07,	
	// Cara
	ID_Portrait,
	// Nombres de los atributos 
	ID_ATTRIB_TEXT,
	// Porcentajes
	ID_ATTRIB_PERCENTAGE,
	// Valores numericos asociados a los porcentajes
	ID_ATTRIB_NUM_VALUE,
	// Habilidades
	ID_HABILITY_00,
	ID_HABILITY_01,
	ID_HABILITY_02,
	ID_HABILITY_03,
	ID_HABILITY_04,
	ID_HABILITY_05,
	ID_HABILITY_06,
	ID_HABILITY_07,
	// Boton de abandonar interfaz
	ID_EXIT_BUTTON,
	// Imagen de fondo
	ID_BACKIMG
  };  
  
private:
  // Estructuras
  struct sBackImageInfo {
	// Informacion asociada a la imagen de fondo
	CGUICAnimBitmap BackImg; // Imagen de fondo  
  };

  struct sInventoryInfo {	
	// Info asociada al trabajo con items en inventario
	// Nota: La capacidad de seleccion de items se heredara de la clase base
	CGUICButton         UpButton;   // Mover inventario hacia arriba
	CGUICButton         DownButton; // Mover inventario hacia abajo
	AreaDefs::EntHandle hActItem;   // Item cogido de inventario
  };

  struct sEquipSlotsInfo {
	// Slots asociados al equipamiento del jugador
	// Slots
	// Array de slots
	CGUICSpriteSelector Slots[RulesDefs::MAX_CRIATURE_EQUIPMENTSLOT];     
	AreaDefs::EntHandle      hActItem;  // Item cogido de slot
	RulesDefs::EquipmentSlot ActSlot;   // Slot al que pertenece hActItem
	// Nombres
	bool bUseSlotsNames; // ¿Poner nombres de slots?	
	// Nombres de Slots   
	CGUICLineText Names[RulesDefs::MAX_CRIATURE_EQUIPMENTSLOT]; 		
  };

  struct sConsoleInfo {
	// Informacion referida a la consola
	CGUICBlockText Text;       // Consola de texto
	CFXBack        FXBack;     // FXBack de fondo para la consola
	CGUICButton    ButtonUp;   // Scroll hacia arriba en consola
	CGUICButton    ButtonDown; // Scroll hacia abajo en consola  
  };

  struct sSymptomsInfo {
	// Informacion relacionada con los sintomas
	CGUICButton Symptoms[RulesDefs::MAX_CRIATURE_SYMPTOMS]; // Iconos de sintomas 
	GraphDefs::sAlphaValue NoPresentAlpha;                  // Alpha sintoma no presente
	word                   uwNumSymptoms;                   // Numero de sintomas
  };

  struct sPortraitInfo {
	// Slot que mantendra la cara del personaje
	// La cara sera recibida desde el exterior por el metodo Active
	CGUICSpriteSelector PortraitSlot; // Slot con la cara
  };

  struct sAttributesInfo {
	// Estructuras
	struct sTextInfo {
	  // Texto simple
	  CGUICLineText Text; // Info de texto
	};
	struct sTextNumericInfo: public sTextInfo {
	  // Texto con info numerica asociada		
	  CGUICLineText Numeric; // Info numerica
	};
	struct sTextNumericBarInfo: public sTextNumericInfo {
	  // Texto con info numerica y de porcentaje asociada
	  CGUICPercentageBar Percentage; // Info de porcentaje
	};
	// Informacion asociada a los atributos del jugador
	sTextInfo           Name;            // Nombre
	sTextInfo           GenreTypeClass;  // Genero / Tipo / Clase
	sTextNumericInfo    Level;           // Nivel
	sTextNumericInfo    Experience;      // Experiencia
	sTextNumericInfo    CombatPoints;    // Puntos de combate
	sTextNumericBarInfo Health;          // Salud
	sTextNumericBarInfo EAttributes[8];  // Atributos extendidos 
	word                uwNumExtAttribs; // Numero de atributos extendidos
  };

  struct sHabilitiesInfo {
	// Botones asociados a las habilidades
	// Botones (iconos) de habilides
	CGUICButton Habilities[RulesDefs::MAX_CRIATURE_HABILITIES]; // Iconos
	word        uwNumHabilities;                                // Num. hab.
  };

  struct sExitInfo {
	// Informacion asociada al boton de salida
	CGUICButton ExitButton; // Boton de salida
  };

  struct sFloatingTextValues {
	// Valores globales para el uso del texto flotante
	GraphDefs::sRGBColor   RGBColor; // Color RGB
	GraphDefs::sAlphaValue Alpha;    // Valor alpha
  };

public:
  // Instancia singlenton
  static CGUIWPlayerProfile* m_pCGUIWPlayerProfile; // Instancia singlenton

  // InterPortraits a modulos del motor
  iCWorld*         m_pWorld;         // Instancia al universo de juego
  iCGameDataBase*  m_pGDBase;        // Instancia a la base de datos
  iCInputManager*  m_pInputManager;  // Intancia al input manager
  iCGUIManager*    m_pGUIManager;    // Interfaz al gestor de ventanas
  iCGraphicSystem* m_pGraphicSystem; // Subsistema grafico  

  // Instancias a los distintos componentes  
  sBackImageInfo  m_BackImgInfo;    // Imagen de fondo
  sInventoryInfo  m_InventoryInfo;  // Info asociada al inventario
  sEquipSlotsInfo m_EquipSlotsInfo; // Info asociada a los slots de equipamiento
  sConsoleInfo    m_ConsoleInfo;    // Consola de descriciones
  sSymptomsInfo   m_SymptomsInfo;   // Sintomas
  sPortraitInfo   m_PortraitInfo;   // Slot asociado a la cara
  sAttributesInfo m_AttribInfo;     // Descripcion de atributos del jugador
  sHabilitiesInfo m_HabilitiesInfo; // Habilidades del jugador
  sExitInfo       m_ExitInfo;       // Boton de salida

  // Resto
  sFloatingTextValues m_FloatingTextValues; // Valores para el texto flotante

public:
  // Constructor / destructor  
  CGUIWPlayerProfile(void): m_pWorld(NULL),			
							m_pInputManager(NULL),
							m_pGUIManager(NULL),
							m_pGDBase(NULL),
							m_pGraphicSystem(NULL) { }
  ~CGUIWPlayerProfile(void) { End(); }
  
public:
  // Protocolo de inicializacion / finalizacion
  bool Init(void);
  void End(void);
private:
  // Metodos de apoyo  
  bool InitInterfazInfo(void);  
  bool TLoad(void);  
  bool TLoadBackImg(CCBTEngineParser* const pParser);
  bool TLoadInventory(CCBTEngineParser* const pParser);
  bool TLoadConsole(CCBTEngineParser* const pParser);
  bool TLoadSymptomsInfo(CCBTEngineParser* const pParser);
  bool TLoadEquipmentSlots(CCBTEngineParser* const pParser);
  bool TLoadHabilitiesInfo(CCBTEngineParser* const pParser);  
  bool TLoadPortraitInfo(CCBTEngineParser* const pParser);
  bool TLoadExitInfo(CCBTEngineParser* const pParser);
  bool TLoadAttribInfo(CCBTEngineParser* const pParser);  
  bool TLoadFloatingTextValues(CCBTEngineParser* const pParser);
  void EndComponents(void);

public:
  // CGUIWindow / Obtencion del tipo de ventana
  inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el tipo de interfaz
	return GUIManagerDefs::GUIW_PLAYERPROFILE;
  }

public:
  // iCGUIWindow / Notificacion de eventos
  void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent);
private:
  // Metodos de apoyo
  void OnSelectNotify(const GUIDefs::GUIIDComponent& IDComponent, 
					  const bool bSelect);
  void OnButtonRightPressedNotify(const GUIDefs::GUIIDComponent& IDComponent);
  void OnButtonLeftPressedNotify(const GUIDefs::GUIIDComponent& IDComponent);

public:
  // Activacion y desactivacion de la ventana de interfaz
  void Active(const bool bDoFadeIn = false);
  void Deactive(void);
private:
  // Metodos de apoyo  
  void ActiveComponents(const bool bActive);
  void MapEquipmentSlots(void);
  
private:
  // Establecimiento en pantalla de los valores numericos del jugador
  void SetLevelNumericValues(void);
  void SetExperienceNumericValues(void);
  void SetCombatPointsNumericValues(void);
  void SetHealthNumericValues(void);
  void SetExtAttributesNumericValues(const byte ubExtAttribute);

public:
  // Activacion / desactivacion de la entrada
  void SetInput(const bool bActive);

public:
  // iCGUIWindow / Manejo de los eventos de entrada  
  bool DispatchEvent(const InputDefs::sInputEvent& aInputEvent);
private:
  // Metodos de apoyo
  void InstallClient(void);
  void UnistallClient(void);
  bool OnButtonMouseRight(void);

private:
  // Operaciones relacionadas con botones de inventario
  void OnInvButtonSelect(const GUIDefs::GUIIDComponent& IDButton,
						 const bool bSelect);
  void OnInvButtonLeftClick(const GUIDefs::GUIIDComponent& IDButton);

private:
  // Operaciones relacionadas con los slots de equipamiento
  void OnEquipmentSlotSelect(const RulesDefs::EquipmentSlot& Slot,
							 const bool bSelect);
  void OnEquipmentSlotLeftClick(const RulesDefs::EquipmentSlot& Slot);
  void OnEquipmentSlotRightClick(const RulesDefs::EquipmentSlot& Slot);
  inline void SelectEquipmentSlot(const RulesDefs::EquipmentSlot& Slot,
								  const bool bSelect) {
	ASSERT(Inherited::IsInitOk());	
	// Se selecciona / deseleccion ael slot de equipamiento Slot 
	m_EquipSlotsInfo.Slots[Slot].Select(bSelect);  
	if (m_EquipSlotsInfo.bUseSlotsNames) {
	  m_EquipSlotsInfo.Names[Slot].Select(bSelect);
	}  
  }
  
public:
  // Operaciones relacionadas con la consola
  inline void WriteToConsole(const std::string& szText, const bool bPlaySound) {
	ASSERT(Inherited::IsInitOk());
	// Escribe
	m_ConsoleInfo.Text.InsertText(szText, bPlaySound);  	
  }
  inline void ResetConsoleInfo(void) {
	ASSERT(Inherited::IsInitOk());
	// Libera informacion
	m_ConsoleInfo.Text.Reset();
  }

private:
  void OnConsoleButtonLeftClick(const byte ubButton);
  void OnConsoleButtonSelect(const byte ubButton, const bool bSelect);

private:
  // Operaciones sobre la cara / personaje
  void OnPortraitLeftClick(void);
  void OnPortraitSelect(const bool bSelect);

private:
  // Operaciones relacionads con los sintomas
  void CGUIWPlayerProfile::OnSymptomSelect(const word uwIdxSymptom,
										   const bool bSelect);
private:
  // Operaciones relacionadas con el boton de salida
  void OnExitSelect(const bool bSelect);
  void OnExitLeftClick(void);

protected:
  // CGUIWBaseItemSelector / Notificacion de que un item ha sido cliqueado con y sin item asociado
  void OnItemLeftClick(const AreaDefs::EntHandle hItem, 
					   CGUICSpriteSelector& Selector);
  void OnItemRightClick(const AreaDefs::EntHandle hItem,
						CGUICSpriteSelector& Selector);
private:
  // Metodos de apoyo
  void OnInventoryLeftClick(const AreaDefs::EntHandle hItem);
  
public:
  // iCItemContainerObserver / Operacion de notificacion
  void ItemContainerNotify(const ItemContainerDefs::eObserverNotifyType& NotifyType,
						   const AreaDefs::EntHandle& hItem);

public:
  // iCEquipmentSlotsObserver / Operacion de notificacion
  void EquipmentSlotNotify(const EquipmentSlotsDefs::eObserverNotifyType& NotifyType,
						   const RulesDefs::EquipmentSlot& Slot);

public:
  // iCCriatureObserver / Operacion de notificacion
  void CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
							  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
							  const dword udParam = 0);

public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);

public:
  // Operacion de dibujado
  void Draw(void);

private:
  // Metodos de apoyo generales
  inline AreaDefs::EntHandle IsAnyItemAttachedToGUI(void) const {
	ASSERT(Inherited::IsInitOk());
	// Comprueba si hay algun item cogido devolviendo su handle o 0
	if (m_InventoryInfo.hActItem) {
	  return m_InventoryInfo.hActItem;
	} else if (m_EquipSlotsInfo.hActItem) {
	  return m_EquipSlotsInfo.hActItem;
	}
	return 0;
  }
};

#endif // ~ ifndef CGUIWPlayerProfile