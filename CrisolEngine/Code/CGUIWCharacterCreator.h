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
// CGUIWCharacterCreator.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CCharacterCreator
//
// Descripcion:
// - Implementa el interfaz para la creacion del personaje. Este interfaz
//   sera sumamente importante y obligatorio para comenzar una nueva partida.
//   Desde el mismo se definira como se querra que sea el personaje a controlar.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWCHARACTERCREATOR_H_
#define _CGUIWCHARACTERCREATOR_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabecera
#ifndef _CGUIWINDOW_H_
#include "CGUIWindow.h"
#endif
#ifndef _CGUICANIMBITMAP_H_
#include "CGUICAnimBitmap.h"
#endif
#ifndef _CGUICBUTTON_H_
#include "CGUICButton.h"
#endif
#ifndef _CGUICLINETEXT_H_
#include "CGUICLineText.h"
#endif
#ifndef CGUICINPUT_H_
#include "CGUICInput.h"
#endif
#ifndef _CGUICPERCENTAGEBAR_H_
#include "CGUICPercentageBar.h"
#endif
#ifndef _CFXBACK_H_
#include "CFXBack.h"
#endif
#ifndef _AUDIODEFS_H_
#include "AudioDefs.h"
#endif
#ifndef _RULESDEFS_H_
#include "RulesDefs.h"
#endif
#ifndef _RESDEFS_H_
#include "ResDefs.h"
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif

// Definicion de clases / estructuras / espacios de nombres
struct iCGUIManager;
struct iCInputManager;
struct iCGraphicSystem;
struct iCResourceManager;
struct iCGameDataBase;
class CCBTEngineParser;
class CSprite;

// clase CGUIWCharacterCreator
class CGUIWCharacterCreator: public CGUIWindow
{
private:
  // Tipos
  typedef CGUIWindow Inherited; // Acceso a la clase base

private:
  // Clases amigas
  // Cargador de datos
  friend class CCfgLoader;

private:  
  // Enumerados
  enum {
	// Identificadores para los fades
	ID_ENDSTARTFADE = 0X01, // Finaliza el fade de inicio
	ID_ENDCANCELFADE,       // Finaliza el fade despues de cancelar
	ID_ENDACCEPTFADE        // Finaliza el fade despues de aceptar
  };

  enum {	
	// Identificadores para componentes
	ID_BACKIMG      = 0X01, // Imagen de fondo
	ID_CANCELBUTTON,        // Boton de cancelar la creacion del caracter
	ID_ACCEPTBUTTON,        // Boton de aceptacion
	ID_NAMEOPTION,          // Texto informativo del nombre
	ID_NAMEINPUT,           // Entrada del nombre
	ID_GENREOPTION,         // Opcion para seleccionar el genero
	ID_GENRESELECT,         // Texto del genero seleccionado	
	ID_TYPEOPTION,          // Opcion para seleccionar tipo
	ID_TYPESELECT,          // Tipo seleccionado
	ID_CLASSOPTION,         // Opcion para seleccionar clase
	ID_CLASSSELECT,         // Clase seleccionada
	ID_NEXTPORTRAITBUTTON,  // Boton de retrato sig.
	ID_PREVPORTRAITBUTTON,  // Boton de retrato anterior	
	ID_DISTRIBUTEPOINTS,    // Puntos a distribuir para atributos y habilidades
	ID_PERCENTAGEBAR,       // Identificador de una barra de porcentaje
	ID_INCATTRIBPOINTS,     // Incrementa puntos de attributo
	ID_DECATTRIBPOINTS,     // Decrementa puntos de atributo
	ID_HEALTH,              // Identificador de salud
	ID_COMBATPOINTS,        // Identificador de puntos de combate
	ID_EXATTRIB_00,         // Atributo extendido 0
	ID_EXATTRIB_01,         // Atributo extendido 1
	ID_EXATTRIB_02,         // Atributo extendido 2
	ID_EXATTRIB_03,         // Atributo extendido 3
	ID_EXATTRIB_04,         // Atributo extendido 4
	ID_EXATTRIB_05,         // Atributo extendido 5
	ID_EXATTRIB_06,         // Atributo extendido 6
	ID_EXATTRIB_07,         // Atributo extendido 7
	ID_ATTRIBVALUE,         // Valor asociado a atributo
	ID_HABILITYICON,        // Icono de habilidad
	ID_HABILITY_00,         // Habilidad 0
	ID_HABILITY_01,         // Habilidad 1
	ID_HABILITY_02,         // Habilidad 2
	ID_HABILITY_03,         // Habilidad 3
	ID_HABILITY_04,         // Habilidad 4
	ID_HABILITY_05,         // Habilidad 5
	ID_HABILITY_06,         // Habilidad 6
	ID_HABILITY_07,         // Habilidad 7	
  };

private:
  // Estructuras forward
  struct sNSpritePortrait;
  struct sNProfileTypeInfo;
  struct sNProfileClassInfo;

private:
  // Tipos
  // Lista para almacenar retratos disponibles
  typedef std::list<sNSpritePortrait*> SpritePortraitList;
  typedef SpritePortraitList::iterator SpritePortraitListIt;
  // Lista con la informacion asociada a los tipos de perfil de jugador
  typedef std::list<sNProfileTypeInfo*> ProfileTypeInfoList;
  typedef ProfileTypeInfoList::iterator ProfileTypeInfoListIt;
  // Lista con la informacion asociada a las clases de perfil de jugador
  typedef std::list<sNProfileClassInfo*> ProfileClassInfoList;
  typedef ProfileClassInfoList::iterator ProfileClassInfoListIt;
  // Map para mapear los handles a los textos flotantes
  typedef std::map<GUIDefs::GUIIDComponent, std::string> FloatingTextMap;
  typedef FloatingTextMap::iterator                      FloatingTextMapIt;
  typedef FloatingTextMap::value_type                    FloatingTextMapValType;

private:
  // Estructuras  
  struct sProfileImproveCost {
	// Puntos asociados al coste de mejorar en una unidad un atributo o habilidad
	word uwCost; // Coste
  };

  struct sProfileAttribInfo: public sProfileImproveCost {
	// Valor base asociado a un atributo
	sword swBaseValue; // Valor base
  };    

  struct sFadeInfo {
	// Info asociada a un fade particular
	struct sFade {
	  GraphDefs::sRGBColor RGBColor; // Color fade
	  word                 uwSpeed;  // Velocidad
	};
	// Datos
	sFade StartFade;  // Fade de inicio
	sFade CancelFade; // Fade de cancelacion
	sFade AcceptFade; // Fade de aceptacion
  };

  struct sFXBack {
	// Info asociada al quad de fondo de FX
	CFXBack                FXBack;    // FXBack
	GraphDefs::sRGBColor   RGBQuad;   // Color asociado al Quad
	GraphDefs::sAlphaValue AlphaQuad; // Alpha asociado al Quad	
	sPosition              Position;  // Posicion
  };
  
  struct sProfileInfo {
	// Informacion de los perfilese de caracter existentes
	ProfileTypeInfoList TypeInfo; // Lista con los nodos de info por tipo
  };  

  struct sFloatingTextInfo {
	// Info asociada al texto flotante
	GraphDefs::sRGBColor    Color;             // Color del texto flotante
	GraphDefs::sAlphaValue  Alpha;             // Alpha del texto flotante
	FloatingTextMap         TextMap;           // Map con los handles al texto asociado
	GUIDefs::GUIIDComponent ActIDFloatingText; // Id. del texto flotante
  };

  struct sBackImgInfo {
	// Info asociada a la imagen de fondo
	CGUICAnimBitmap BackImg; // Imagen de fondo
  };

  struct sBackMIDIInfo {
	// Musica MIDI de fondo
	std::string              szBackMIDIMusic; // Archivo MIDI de fondo
	AudioDefs::eMIDIPlayMode PlayMode;        // Modo de reproduccion
  };

  struct sCancelInfo {
	// Info asociada a la cancelacion del proceso de creacion de caracter
	CGUICButton Button; // Boton de cancelacion
  };

  struct sAcceptInfo {
	// Info asociada al boton de aceptacion
	CGUICButton Button; // Boton de aceptacion
  };

  struct sNameInputInfo {
	// Info asociada a la entrada del nombre del caracter
	CGUICLineText NameText;     // Identificativo de la palabra "Nombre"
	CGUICInput    NameInput;    // Componente para la introduccion del nombre	
	sPosition     LastMousePos; // Ultima posicion del raton
  };
  
  struct sGenreInfo {
	// Info asociada al genero	
	// Datos
	CGUICLineText GenreOption; // Linea con la palabra "Genero" para seleccion
	CGUICLineText GenreSelect; // Linea con el genero seleccionado
	CGUICButton   NextButton;  // Boton a sig. retrato
	CGUICButton   PrevButton;  // Boton a anterior retrato
	// Listas de retratos por genero
	SpritePortraitList PortraitsList[RulesDefs::MAX_CRIATURE_GENRES]; 
	// Iteradores al ultimo nodo de las listas de retratos
	SpritePortraitListIt     EndPortraitsIt[RulesDefs::MAX_CRIATURE_GENRES]; 	
	SpritePortraitListIt     ItPortrait;       // Iterador al retrato actual	
	sPosition                PortraitsDrawPos; // Pos. de dibujado de retratos
	RulesDefs::CriatureGenre ActGenre;         // Idx al genero actual
	sFXBack                  FXBack;           // Quad FX de fondo
  };

  struct sTypeClassInfo {
	// Info asociada al tipo / clase seleccionado
	// Datos
	CGUICLineText          TypeOption;     // Linea con texto asociado a "Tipo" para seleccion
	CGUICLineText          TypeSelect;     // Linea con el texto del "Tipo" seleccionado
	CGUICLineText          ClassOption;    // Linea con texto asociado a "Clase" para seleccion
	CGUICLineText          ClassSelect;    // Linea con el texto de la "Clase" seleccionada	
	ProfileTypeInfoListIt  ActTypeInfoIt;  // Iterador a nodo de info actual para tipo
    ProfileClassInfoListIt ActClassInfoIt; // Iterador a clase actual para el tipo actual
	sFXBack                FXTypeBack;     // Quad FX de fondo para tipo
	sFXBack                FXClassBack;    // Quad FX de fondo para clase
  };
  
  struct sAttributesInfo {
	// Info asociada a atributos
	// Se define la estructura asociada a un atributo
	struct sModifyAttribInfo {
	  // Info asociada a un atributo modificable
	  CGUICLineText AttribName;    // Nombre del atributo
	  CGUICLineText AttribValue;   // Valor asociado al atributo	  
	  byte          uwAsingPoints; // Puntos de incremento asignados
	  // Constructor
	  sModifyAttribInfo(void): uwAsingPoints(0) { }
	};

	struct sActAttribute {
	  // Info asociada al atributo actual
	  GUIDefs::GUIIDComponent IDAttribute;   // ID del Atributo actual
	  word                    uwImproveCost; // Coste de incremento
	  sModifyAttribInfo*      pAttribute;    // Direccion del atributo actual
	  sword                   swBaseValue;   // Valor base
	  // Constructor
	  sActAttribute(void): pAttribute(NULL), IDAttribute(0), swBaseValue(0) { }
	};	

	// Datos
	sModifyAttribInfo Health;	                                    // Salud
	sModifyAttribInfo CombatPoints;                                 // Ptos combate	      
	sModifyAttribInfo ExtAttribs[RulesDefs::MAX_CRIATURE_EXATTRIB]; // Atrib. ext.	
	sActAttribute     ActAttribute;                                 // Atrib. actual	
	// Atributo establecido
	GUIDefs::GUIIDComponent IDAttributeSet; // Identificador atributo
	// Tratamiento de los puntos de reparto
	byte               uwImprovePoints;   // Puntos de mejora que quedan	
	CGUICLineText      ImprovePoints;     // Linea con los puntos de mejora que quedan
	CGUICPercentageBar ImprovePointsPBar; // Barra de porcentaje de los puntos
	CGUICButton        IncButton;         // Incremento 
	CGUICButton        DecButton;         // Decremento	
	// Colores para los valores en estado base y aumentado
	GraphDefs::sRGBColor RGBBaseValue;    // Valor base
	GraphDefs::sRGBColor RGBImproveValue; // Valor aumentado
	// Resto
	sFXBack FXBack; // Quad de fondo para realizar efecto de FX
  };

  struct sHabilitiesInfo {
	// Info asociada a habilidades
	// Estructuras
	struct sHabilityInfo {
	  // Info asociada a una habilidad concreta
	  CGUICButton   HabilityIcon; // Icono de habilidad
	  CGUICLineText HabilityName; // Nombre de la habilidad
	  sFXBack       FXBackName;   // Quad de FX asociado al nombre
	};	
	// Datos
	// Habilidades
	sHabilityInfo Habilities[RulesDefs::MAX_CRIATURE_HABILITIES];
	// Control de puntos
	word               uwHabDistributePoints;   // Puntos de reparto para habilidades
	CGUICLineText      HabDistributePoints;     // Linea con los puntos para obtener habilidad
	CGUICPercentageBar HabDistributePointsPBar; // Barra de porcentaje de los puntos	
  };

private:
  // Vbles de miembro    
  // Interfaces a otros subsistemas
  iCGUIManager*      m_pGUIManager;   // Interfaz al manager de GUI
  iCInputManager*    m_pInputManager; // Interfaz al manager de entrada
  iCGraphicSystem*   m_pGraphSys;     // Interfaz al subsistema grafico
  iCGameDataBase*    m_pGDBase;       // Interfaz a la base de datos del juego

  // Info asociada a componentes / perfiles
  sBackImgInfo      m_BackImgInfo;      // Info de la imagen de fondo
  sBackMIDIInfo     m_BackMIDIInfo;     // Info de musica de fondo
  sCancelInfo       m_CancelInfo;       // Info de la cancelacion del proceso de creacion
  sAcceptInfo       m_AcceptInfo;       // Info de la aceptacion del jugador generado
  sNameInputInfo    m_NameInputInfo;    // Info de la entrada del nombre
  sGenreInfo        m_GenreInfo;        // Info relativa a la seleccion de genero
  sTypeClassInfo    m_TypeClassInfo;    // Info relativo a la seleccion de tipo y clase
  sAttributesInfo   m_AttributesInfo;   // Info relativa a atributos
  sHabilitiesInfo   m_HabilitiesInfo;   // Info asociada a habilidades

  // Resto  
  sFloatingTextInfo  m_FloatingTextInfo; // Info global
  sFadeInfo          m_FadeInfo;         // Info relacionada con los fades  
  sProfileInfo       m_ProfileInfo;      // Informacion de los perfiles
  
public:
  // Constructor / destructor  
  CGUIWCharacterCreator(void): m_pGUIManager(NULL),
							   m_pInputManager(NULL),
							   m_pGraphSys(NULL),
							   m_pGDBase(NULL) { }
  ~CGUIWCharacterCreator(void) { End(); }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(void);
  void End(void);
private:
  // Metodos de apoyo para la inicializacion / carga / finalizacion de componentes
  bool TLoad(void);
  bool TLoadFloatingTextInfo(CCBTEngineParser* const pParser);
  bool TLoadFadeInfo(CCBTEngineParser* const pParser);
  bool TLoadPlayerProfilesInfo(void);
  bool TLoadBackImg(CCBTEngineParser* const pParser);  
  bool TLoadCancelInfo(CCBTEngineParser* const pParser);
  bool TLoadAcceptInfo(CCBTEngineParser* const pParser);
  bool TLoadNameInfo(CCBTEngineParser* const pParser);  
  bool TLoadGenreInfo(CCBTEngineParser* const pParser);
  bool TLoadTypeClassInfo(CCBTEngineParser* const pParser);
  bool TLoadAttributesInfo(CCBTEngineParser* const pParser);
  bool TLoadHabilitiesInfo(CCBTEngineParser* const pParser);
  bool TLoadPlayerCommonInfo(void);
  bool TLoadMusicInfo(CCBTEngineParser* const pParser);    
  void TLoadLineOption(CGUICLineText::sGUICLineTextInitData& InitLineOption, 
					   const GUIDefs::GUIIDComponent& IDComponent,
					   CCBTEngineParser* const pParser);
  void TLoadLineSelect(CGUICLineText::sGUICLineTextInitData& InitLineOption, 
					   const std::string szText,
					   const GUIDefs::GUIIDComponent& IDComponent,
					   CCBTEngineParser* const pParser);
  void TLoadOptionButton(CGUICButton::sGUICButtonInitData& InitButton,
						 const GUIDefs::GUIIDComponent& IDComponent,
						 CCBTEngineParser* const pParser);
  void TLoadPercentBar(CGUICPercentageBar::sGUICPercentageBarInitData& InitPercentBar,
					   CCBTEngineParser* const pParser);    
  void InitFXBack(sFXBack& FXBack, 
				  const std::string& szText);
  void EndPlayerProfileInfo(void);
  void EndBackImg(void);
  void EndCancelInfo(void);
  void EndAcceptInfo(void);
  void EndNameInfo(void);
  void EndGenreInfo(void);    
  void EndTypeClassInfo(void);
  void EndAttributesInfo(void);
  void EndHabilitiesInfo(void);

public:
  // CGUIWindow / Obtencion del tipo de ventana
  inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el tipo de interfaz
	return GUIManagerDefs::GUIW_CHARACTERCREATOR;
  }

public:
  // CGUIWindow / Activacion y desactivacion de la ventana de interfaz
  void Active(void);  
  void Deactive(void);
private:
  // Metodos de apoyo
  void SetComponentsActive(const bool bActive);
  void ActiveInterfaz(void);

public:
  // iCCommandClient / Notificacion para la finalizacion del comando Fade
  void EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
					const dword udInstant,					
					const dword udExtraParam);  

public:
  // iCGUIWindow / Notificacion de eventos
  void ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent);
private:
  // Metodos de apoyo  
  void OnSelectNotify(const GUIDefs::GUIIDComponent& IDComponent,
					  const bool bSelect);
  void OnLeftClickNotify(const GUIDefs::GUIIDComponent& IDComponent);
  void OnEndNameInput(void);

private:
  // Trabajo con el texto flotante
  void SetFloatingText(const GUIDefs::GUIIDComponent& IDComponent);
  void UnsetFloatingText(const GUIDefs::GUIIDComponent& IDComponent);
  void RegisterFloatingText(void);
  void ReleaseFloatingText(void);
  void MapFloatingText(const GUIDefs::GUIIDComponent& IDComponent,  
					   const word uwCost);
  void SetFloatingText(const GUIDefs::GUIIDComponent& IDComponent,
					   const bool bSet);

private:
  // Trabajo con los retratos y genero
  void NextGenre(void);
  void NextPortrait(void);
  void PrevPortrait(void);
  void InitPortraitSet(const RulesDefs::CriatureGenre& Genre);
  void EndPortraitSet(const RulesDefs::CriatureGenre& Genre);

private:
  // Trabajo con los tipos y clases
  void NextType(void);
  void NextClass(void);

private:
  // Trabajo con los atributos
  void IncAttributePoints(void);
  void DecAttributePoints(void);
  void SetAttributeTextPoints(void);
  void SelectAttribute(const GUIDefs::GUIIDComponent& IDComponent,
					   sAttributesInfo::sModifyAttribInfo* const pAttribute,
					   const word uwImproveCost,
					   const sword& swBaseValue);

private:
  // Trabajo con las habilidades
  void SelectHability(const byte ubIdxHability);  
  void UpdateHabilityPercentageBar(void);

public:
  // CGUIWindow / Activacion / desactivacion de la entrada
  void SetInput(const bool bInput);

private:
  // Metodos relacionados con la aceptacion de un caracter
  bool CanAcceptCharacter(void);
  void AcceptCharacter(void);

public:
  // Operacion de dibujado
  void Draw(void);
};

#endif // ~ ifndef CGUIWCharacterCreator