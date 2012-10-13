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
// GraphDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Definiciones para el trabajo con graficos
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHDEFS_H_
#define _GRAPHDEFS_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif

// Espacio de nombres
namespace GraphDefs
{
  // Tipos
  typedef byte Light; // Una luz sera un valor de intensidad entre 0..255

  // Constantes
  const byte MAX_BRACKGROUND_DRAWPLANES = 0; // Planos para el fondo
  const byte MAX_FX_DRAWPLANES = 0;          // Planos para FX
  const byte MAX_CURSOR_DRAWPLANES = 0;      // Planos para cursores
  const byte MAX_GAME_SCENE_DRAWPLANES = 9;  // Planos para la escena de juego
  const byte MAX_GUI_DRAWPLANES = 3;         // Planos para el interfaz
  
  // Tipos enumerados
  enum eVertex {
	// Tipos de vertices
	VERTEX_0 = 0,
	VERTEX_1,
	VERTEX_2,
	VERTEX_3
  };

  enum eFadeState {
	// Estados de Fade a pantalla completa
	FADEIN_STATE = 0,
	FADEOUT_STATE,
	TO_FADEIN_STATE,
	TO_FADEOUT_STATE
  };
  
  struct sTextureRegion { 
	// Contiene localizacion u,v de un caracter en la textura por vertice
	// Datos
    float uV0, vV0; // Vertice 0
    float uV1, vV1; // Vertice 1
    float uV2, vV2; // Vertice 2
    float uV3, vV3; // Vertice 3
	// Constructores
	sTextureRegion(void): uV0(0.0f), vV0(0.0f), 
						  uV1(1.0f), vV1(0.0f),
						  uV2(0.0f), vV2(1.0f),
						  uV3(1.0f), vV3(1.0f) { }
	
	sTextureRegion(const float fuV0, const float fvV0,
				   const float fuV1, const float fvV1,
				   const float fuV2, const float fvV2,
				   const float fuV3, const float fvV3): uV0(fuV0), vV0(fvV0),
													      uV1(fuV1), vV1(fvV1),
														  uV2(fuV2), vV2(fvV2),
														  uV3(fuV3), vV3(fvV3) { }
														  
	// Operador de asignacion	
	sTextureRegion& operator=(const sTextureRegion& ObjSrc) {
	  uV0 = ObjSrc.uV0; vV0 = ObjSrc.vV0;
	  uV1 = ObjSrc.uV1; vV1 = ObjSrc.vV1;
	  uV2 = ObjSrc.uV2; vV2 = ObjSrc.vV2;
	  uV3 = ObjSrc.uV3; vV3 = ObjSrc.vV3;
	}	
  };  

  enum eBpp { 
	// Bits por pixel de la textura
    BPP_8 = 8,   // 8Bpp (sin canal alpha)
    BPP_16 = 16, // 16Bpp (con canal alpha de 0 bits, 1 bit o 4 bits)
    BPP_24 = 24, // 24Bpp (sin canal alpha)
    BPP_32 = 32, // 32Bpp (con canal alpha de 8 bits)    
	BPP_NOVALID, // Bpp no valido
  };

  enum eABpp { 
	// Numero de bits destinados al canal alpha
    ALPHA_BPP_0 = 0,
    ALPHA_BPP_1 = 1,
    ALPHA_BPP_4 = 4,
    ALPHA_BPP_8 = 8,
  };

  enum eGraphicFile { 
	// Ficheros graficos aceptados
    TGAFile = 0,
  };

  enum eDrawZone {
	// Zonas de dibujado (DZ = Draw Zone)
	// La zona de texto y FX seran iguales ya que el texto no estara
	// almacenado en la misma estructura de datos que los demas lugares
	// El valor DZ_MAX_POS hara referencia al numero maximo de posiciones
	// en el array de planos / lugares.
	DZ_BACKGROUND = 0, // 1 plano (0)
	DZ_GAME_SCENE = 1, // 1 planos (1..1)
	DZ_GUI = 2,	       // 3 planos (2..4)
	DZ_FX = 5,         // 1 plano (5..5)
	DZ_CURSOR = 6,     // 1 plano (6..6)
	DZ_MAX_POS = 7
  };

  // Enumerados
  enum eTextureDim {
	// Posibles dimensiones asociadas a texturas
	TEXTURE_DIM_256 = 256,
	TEXTURE_DIM_128 = 128,
	TEXTURE_DIM_64  = 64,
	TEXTURE_DIM_32  = 32
  };

  struct sRGBColor { 
	// Componentes RGB asociadas a un vertice
	// Datos RGB
	byte ubRed;
	byte ubGreen;
	byte ubBlue;

	// Constructores
	sRGBColor(void): ubRed(255), ubGreen(255), ubBlue(255) { }
	sRGBColor(const sRGBColor& RGBColorObj): ubRed(RGBColorObj.ubRed),
										     ubGreen(RGBColorObj.ubGreen),
										     ubBlue(RGBColorObj.ubBlue) { }
	sRGBColor(const byte ubR, const byte ubG, const byte ubB): ubRed(ubR), 
																  ubGreen(ubG), 
																  ubBlue(ubB) { }
	
	// Sobrecarga operador asignacion
	sRGBColor& operator=(const sRGBColor& RGBColorObj) {	  
	  ubRed = RGBColorObj.ubRed;
	  ubGreen = RGBColorObj.ubGreen;
	  ubBlue = RGBColorObj.ubBlue;
	  return *this;
	}
	// Sobrecarga operador igualdad
	bool operator==(const sRGBColor& RGBColorObj) {
	  return (RGBColorObj.ubRed == ubRed &&
			  RGBColorObj.ubGreen == ubGreen &&
			  RGBColorObj.ubBlue == ubBlue);
	}
  };

  struct sAlphaValue { 
	// Componente alpha asociado a un vertice
	// Dato alpha
	byte ubAlpha;

	// Constructores
	sAlphaValue(void): ubAlpha(255) { }
	sAlphaValue(const sAlphaValue& AlphaValueObj): ubAlpha(AlphaValueObj.ubAlpha) { }
	sAlphaValue(const byte ubAlphaValue): ubAlpha(ubAlphaValue) { }
	
	// Sobrecarga asignacion
	sAlphaValue& operator=(const sAlphaValue& AlphaValueObj) {
	  ubAlpha = AlphaValueObj.ubAlpha;
	  return *this;
	}
	// Sobrecarga operadores de comparacion
	bool operator==(const sAlphaValue& AlphaValueObj) const {
	  return (AlphaValueObj.ubAlpha == ubAlpha);
	}
	bool operator<(const sAlphaValue& AlphaValueObj) const {
	  return (ubAlpha < AlphaValueObj.ubAlpha);
	}
	bool operator>(const sAlphaValue& AlphaValueObj) const {
	  return (ubAlpha > AlphaValueObj.ubAlpha);
	}
	bool operator!=(const sAlphaValue& AlphaValueObj) const {
	  return (ubAlpha != AlphaValueObj.ubAlpha);
	}
  }; 

  struct sLight {
	// Luz posible para una entidad dibujable
	Light VertexLight[4]; // Luz asociada a cada vertice
	
	// Constructores
	sLight(void) {	  
	  byte ubIt = 0;
	  for (; ubIt < 4; ++ubIt) {
		VertexLight[ubIt] = 0;
	  }
	}
	sLight(const sLight& Light) {
	  byte ubIt = 0;
	  for (; ubIt < 4; ++ubIt) {
		VertexLight[ubIt] = Light.VertexLight[ubIt];
	  }
	}

	// Operadores
	sLight& operator=(const sLight& Light) {
	  byte ubIt = 0;
	  for (; ubIt < 4; ++ubIt) {
		VertexLight[ubIt] = Light.VertexLight[ubIt];
	  }
	  return *this;
	}
	const bool operator==(const sLight& Light) const {
	  return (VertexLight[0] == Light.VertexLight[0] &&
			  VertexLight[1] == Light.VertexLight[1] &&
			  VertexLight[2] == Light.VertexLight[2] &&
			  VertexLight[3] == Light.VertexLight[3]);			  
	}
  }; 
  
  struct sBitmapInitData {
	// Informacion asociada a un bitmap
	std::string            szFileName;       // Nombre del fichero
	GraphDefs::eTextureDim WidthTextureDim;  // Ancho de la particion de la textura
	GraphDefs::eTextureDim HeightTextureDim; // Alto de la particion de la textura
	GraphDefs::eBpp        Bpp;              // Bits por pixel
	GraphDefs::eABpp       ABpp;             // Bits en canal alpha
	GraphDefs::sRGBColor   RGBColor;         // Color RGB asociado
	GraphDefs::sAlphaValue Alpha;            // Valor alpha asociado
	// Constructor por defecto
	sBitmapInitData(void): Bpp(GraphDefs::BPP_16), ABpp(GraphDefs::ALPHA_BPP_1),
						   WidthTextureDim(GraphDefs::TEXTURE_DIM_32), 
						   HeightTextureDim(GraphDefs::TEXTURE_DIM_32) { }
  };

};

#endif // ~ #ifdef _GRAPHDEFS_H_

