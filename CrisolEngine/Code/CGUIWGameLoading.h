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
// CGUIWGameLoading.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases
// - CGUIWGameLoading.h
//
// Descripcion:
// - Esta ventana estara constituida por un componente simple que contendra
//   una imagen de fondo indicativa de que se esta cargando un juego. El juego
//   podra cargarse al comenzar una nueva partida o al continuarla usando un
//   archivo con una partida salvada.
// - El fade asociado a la ventana sera solo de comienzo, esto es, se enlazara
//   con el fade que este asociado al estado anterior (Ventana de carga de una
//   partida o creacion de un personaje). Esto es asi porque en el momento que
//   se cargue el area, esta ventana sera completamente desinstalada.
//
// Notas:
// - No realizara notifiacion a clientes asociados.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUIWGAMELOADING_H_
#define _CGUIWGAMELOADING_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabecera
#ifndef _CGUIWINDOW_H_
#include "CGUIWindow.h"
#endif
#ifndef _CGUICBITMAP_H_
#include "CGUICBitmap.h"
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif

// Definicion de clases / estructuras / espacios de nombres
struct iCGraphicSystem;
struct iCFontSystem;
class CCBTEngineParser;
class CPlayer;

// clase CGUIWGameLoading
class CGUIWGameLoading: public CGUIWindow
{  
private:
  // Tipos
  typedef CGUIWindow Inherited; // Acceso a la clase base

private:  
  // Enumerados
  enum {	
	// Identificadores	
	ID_BACKIMG = 0x01, // Imagen de fondo	
  };

  enum {
	// Identificadores de comandos de Fade
	ID_FADEIN_END = 0X01, // Finalizacion del FadeIn
	ID_FADEOUT_END,       // Finalizacion del FadeOut
  };

private:
  // Estructuras
  struct sBackImgInfo {
	// Info asociada a la imagen de fondo
	CGUICBitmap BackImg; // Imagen de fondo
  };

  struct sFadeInfo {
	// Valores asociados a los fades
	word                 FadeInSpeed;  // Velocidad del FadeIn
	GraphDefs::sRGBColor RGBFadeOut;   // Color FadeOut
	word                 FadeOutSpeed; // Velocidad FadeOut
  };

  struct sInitInfo {
	// Informacion relativa al modo de inicializar la clase
	word               uwID;      // Identificador del area / partida guardada
	CPlayer*           pPlayer;   // Instancia al jugador
	AreaDefs::sTilePos PlayerPos; // Posicion donde situar al jugador en area
  };

private:
  // Instancias a otros subsistemas
  iCGraphicSystem* m_pGraphSys; // Subsistema grafico
  iCFontSystem*    m_pFontSys;  // Subsistema de fuentes

  // Info asociada a los componentes y texto
  sBackImgInfo m_BackImg;          // Imagen de fondo
  std::string  m_szAreaLoadingMsg; // Mensaje de area cargando
  
  // Resto
  sFadeInfo  m_FadeInfo; // Info asociada al fade
  sInitInfo  m_InitInfo; // Info asociada a la inicializacion

public:
  // Constructor / destructor  
  CGUIWGameLoading(void): m_pGraphSys(NULL),
						  m_pFontSys(NULL) { }
  ~CGUIWGameLoading(void) { End(); }

public:
  // Protocolo de inicializacion / finalizacion
  bool Init(const word uwID,
			CPlayer* const pPlayer,
			const AreaDefs::sTilePos& PlayerPos);
  void End(void);
private:
  // Metodos de apoyo
  bool TLoad(void);
  bool TLoadBackImgInfo(CCBTEngineParser* const pParser);
  bool TLoadFadeValues(CCBTEngineParser* const pParser);  

public:
  // CGUIWindow / Obtencion del tipo de ventana
  inline GUIManagerDefs::eGUIWindowType GetGUIWindowType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se retorna el tipo de interfaz
	return GUIManagerDefs::GUIW_GAMELOADING;
  }
  
public:
  // CGUIWindow / Activacion y desactivacion de la ventana de interfaz
  void Active(void);  
  void Deactive(void);

public:
  // iCCommandClient / Notificacion para la finalizacion del comando Fade
  void EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
					const dword udInstant,
					const dword udExtraParam);  
public:
  // Operacion de dibujado
  void Draw(void);
};

#endif // ~ ifndef CGUIWGameLoading