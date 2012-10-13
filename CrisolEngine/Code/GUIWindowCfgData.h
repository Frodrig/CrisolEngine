///////////////////////////////////////////////////////////////////////////////
// GUIWindowCfgData.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Define las estructuras donde se alojaran los datos que se cargen desde
//   fichero para los distintos modulos de interfaz de GUI.
//
// Notas:
// - El proceso de carga de informacion se realizara en el modulo CGUIManager
///////////////////////////////////////////////////////////////////////////////
#ifndef _GUIWINDOWCFGDATA_H_
#define _GUIWINDOWCFGDATA_H_

// Cabeceras
#ifndef _RESDEFS_H_
#include "ResDefs.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif

namespace GUIWindowCfgData
{  
  // Estructuras de apoyo  
  struct sImgInfo {
	// Info asociada a una imagen 
	std::string            szFileName; // Nombre del fichero con la imagen
	sPosition              Pos;        // Posicion
	GraphDefs::sAlphaValue Alpha;      // Valor alpha
	GraphDefs::eTextureDim TextWidth;  // Anchura de la textura de particion
	GraphDefs::eTextureDim TextHeight; // Altura de la textura de particion
  };

  struct sButtonInfo {
	// Info asociada a un boton simple
	ResDefs::AnimTemplateHandle hATemplate; // Plantilla de animacion
	sPosition                   Pos;        // Posicion
  };

  struct sConsoleInfo {
	// Info asociada a una consola
	// General
	std::string          szFont;      // Fuente
	GraphDefs::sRGBColor TextColor;   // Color del texto
	word                 uwCapacitiy; // Capacidad
	word                 uwWidth;     // Anchura
	word                 uwLines;     // Lineas capaz de mostrar
	sPosition            Pos;          // Posicion
	// Sonido
	std::string szWAVBipFile; // Nombre del fichero wav con sonido WAV
	// FX
	GraphDefs::sRGBColor   RGBFXBack;   // Color FX de fondo	
	GraphDefs::sAlphaValue AlphaFXBack; // Alpha FX asociado al fondo
  };

  struct sTextInfo {
	// Info asociada a un valor de texto
	std::string szText;  // Texto
	sPosition   TextPos; // Posicion
  };

  struct sTextNumericInfo: public sTextInfo {
	// Info asociada a un valor de texto con info numerica
	sPosition NumericPos; // Posicion
  };

  struct sTextNumericBarInfo: public sTextNumericInfo {
	// Info asociada a un valor de texto con info numerica y barra de porcentaje
	sPosition BarPos; // Posicion
  };

  struct sBarCfgInfo {
	// Info asociada a la config asociada a las barras
	bool                   bBarHoriz;      // ¿Barra horizontal?
	word                   uwSize;         // Tamaño en pixels
	word                   uwWidth;        // Anchura en pixels
	GraphDefs::sRGBColor   RGBMaxValue[2]; // RGB Maximo de la barra Start / End
	GraphDefs::sRGBColor   RGBMedValue[2]; // RGB Medio de la barra Start / End
	GraphDefs::sRGBColor   RGBMinValue[2]; // RGB Minimo de la barra Start / End
	GraphDefs::sAlphaValue Alpha;          // Valor Alpha de la barra
  };

  // Estructuras donde alojar la info leida
  struct sGUIManagerCfgData {
	// Datos de cfg asociados al manager de GUI
	// Cursores
	ResDefs::AnimTemplateHandle hGUICursors; // Handle con los cursores
  };
  
  struct sGameMenuCfgData {
	// Datos de cfg asociado al menu de juego	
	// Imagen de fondo
	sImgInfo BackImg; // Info asociada a la imagen de fondo
	// Opciones: <Guardar, Cargar, Configurar, Volver, Salir>
	sButtonInfo Options[5]; // Botones de opciones
  };

  struct sEntityInteractCfgData {
	// Datos de cfg asociados al interfaz de interaccion con entidades
	// General
	bool                   bDrawHoriz;    // ¿Dibujado horizontal?
	GraphDefs::sAlphaValue AlphaNoSelect; // Alpha cuando no hay seleccion
	// Botones de acciones: Observar, Hablar, Coger, Registrar, Manipular, 
	// Tirar, Usar Inventario, Usar Habilidades, Salir.		
	ResDefs::AnimTemplateHandle hActionButtons[9]; // Botones de acciones
	// Botones de habilidades
	ResDefs::AnimTemplateHandle hHabButtons[8];    // Botones de habilidades
	ResDefs::AnimTemplateHandle hHabReturnButton;  // Boton de retorno de hab.
  };

  struct sMainInterfazCfgData {
	// Datos de cfg asociados a la ventana de interfaz principal
	// General
	sRect rArea; // Area ocupada por la ventana
	// Imagen de fondo
	sImgInfo BackImg; // Info de imagen de fondo
	// Consola
	sConsoleInfo ConsoleInfo; // Info de consola
	// Botones de consola
	sButtonInfo ConsoleButtonUp;   // Boton de subir datos en consola
	sButtonInfo ConsoleButtonDown; // Boton de bajar datos en consola
	// Slots asociados a items en manos
	sButtonInfo ItemLeftSlot;  // Slot izquierdo
	sButtonInfo ItemRightSlot; // Slot derecho
	// Slot asociado a la cara
	sButtonInfo FaceSlot; // Slot de cara
	// Boton de combate
	sButtonInfo CombatButton; // Boton de combate
	// Atributos a mostrar
	// En caso de que un atributo no tenga texto, significara que no se mostrara
	std::string          szTextFont;     // Fuente asociada al texto
	sword                swIDAttribs[2]; // ID's de los identificadores
	sTextNumericBarInfo  Attribs[2];     // Atributos a mostrarse
	GraphDefs::sRGBColor TextColor;      // Color del texto
	GraphDefs::sRGBColor NumericColor;   // Color info numerica
	sBarCfgInfo          BarCfg;         // Info asociado a las barras	
  };

  struct sPlayerProfileCfgData {
	// Imagen de fondo
	sImgInfo BackImg; // Imagen de fondo
	// Slots de inventario
	byte        ubNumInvSlots;    // Num. de slots de inventario
	sButtonInfo InventorySlot;    // AT de slots de inventario y posicion inicial
	byte        ubInvSlotsHeight; // Altura de los slots de inventario
    // Botones de inventario
	sButtonInfo InvButtonUp;   // Boton para subir elementos de inv.
	sButtonInfo InvButtonDown; // Boton para bajar elementos de inv.	
	// Consola
	sConsoleInfo Console; // Consola
	// Botones de consola
	sButtonInfo ConsoleButtonUp;   // Boton de subir datos en consola
	sButtonInfo ConsoleButtonDown; // Boton de bajar datos en consola
	// Iconos de sintomas
	sButtonInfo            SymptomButtons[16];   // Iconos de sintomas
	GraphDefs::sAlphaValue SymptomAlphaNoSelect; // Alpha para sintomas no seleccionados
	// Iconos de habilidades
	sButtonInfo HabButtons[8]; // Iconos de habilidad
	// Equipamiento
	sButtonInfo          EquipButtons[8];  // Slots de equipamiento
	bool                 bUseEquipDesc;    // ¿Mostrar nombre de los slots?
	std::string          szEquipDescFont;  // Fuente utilizada para poner los nombres
	sTextInfo            TextEquipDesc[8]; // Texto asociado a los slots
	GraphDefs::sRGBColor TextEquipColor;   // Color asociado al texto de los slots
	// Slot de cara
	sButtonInfo FaceSlot; // Slot de cara
	// Boton de salida
	sButtonInfo ExitButton; // Boton de salir
	// Atributos
	sTextInfo            Name;               // Nombre
	sTextInfo            Genre;              // Genero
	sTextInfo            Type;               // Tipo
	sTextInfo            Class;              // Clase
	sTextNumericInfo     Level;              // Nivel
	sTextNumericInfo     Experience;         // Experiencia
	sTextNumericInfo     CombatPoints;       // Puntos de combate
	sTextNumericBarInfo  Health;             // Salud
	sTextNumericBarInfo  EAttributes[8];     // Atributos extendidos
	std::string          szAttribTextFont;   // Fuente asociada al texto
	GraphDefs::sRGBColor AttribTextColor;    // Color del texto
	GraphDefs::sRGBColor AttribNumericColor; // Color info numerica
	sBarCfgInfo          AttribBarCfg;       // Info asociado a las barras	
  };
};

#endif // ~ #ifdef _GUIWINDOWCFGDATA_H_
