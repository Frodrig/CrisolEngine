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
// CGFX.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases
// - CGFX
//
// Descripcion:
// - Representa una clase de GFX que sera un grafico para realizar efectos
//   especiales en pantalla. La definicion del GFX estara establecida en un
//   perfil. La unica informacion del perfil que no se mantendra en esta 
//   instancia, sera la relativa a su finalizacion, ya que sera gestionada en 
//   el CGFXManager.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGFX_H_
#define _CGFX_H_

// Cabeceras
#ifndef _ICGRAPHICSYSTEM_H_
#include "iCGraphicSystem.h"
#endif
#ifndef _SYSENGINE_H_
#include "SYSEngine.h"
#endif
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _CSPRITE_H_
#include "CSprite.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _GFXMANGERDEFS_H_
#include "GFXManagerDefs.h"
#endif

// Clase CGFX
class CGFX
{
private:
  // Estructuras
  struct sFXInfo {
	// Info de FX
	CSprite			       Sprite;       // Sprite de FX
	sPosition              sDrawDelta;   // Deltas de dibujado
	GraphDefs::sAlphaValue Alphas[4];    // Estructuras Alpha asociadas
	GraphDefs::sRGBColor   RGBColors[4]; // Estructuras RGB asociadas	
  };

private:
  // Vbles de miembro  
  sFXInfo m_FXInfo;    // Info de FX  
  bool	  m_bIsInitOk; // ¿Instancia incializada correctamente?   

public:
  // Constructor / destructor
  CGFX(void): m_bIsInitOk(false) { }
  ~CGFX(void) { End(); }

public:	
  // Protocolos de inicializacion / finalizacion de instancia.  
  bool Init(const AreaDefs::EntHandle& hEntity,
			const std::string& szGFXAnimTemplate,		    
			const GFXManagerDefs::eGFXDrawPosition& DrawPosition);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // Establece valores RGB y Alpha por vertice
  inline void SetAlphaValue(const GraphDefs::eVertex& Vertex, 
							const GraphDefs::sAlphaValue& Alpha) {
	ASSERT(IsInitOk());
	// Asocia el componente Alpha a un determinado vertice
	m_FXInfo.Alphas[Vertex] = Alpha;
  }
  inline void SetRGBValue(const GraphDefs::eVertex& Vertex,
						  const GraphDefs::sRGBColor& RGBColor) {
	ASSERT(IsInitOk());
	// Asocia el componente RGB a un determinado vertice
	m_FXInfo.RGBColors[Vertex] = RGBColor;
  }

public:
  // Metodos de dibujado
  inline void Draw(const sword& swXDrawPos,
				   const sword& swYDrawPos,
				   const GraphDefs::eDrawZone& DrawZone,
				   const byte ubDrawPlane) {
	ASSERT(IsInitOk());
	// Se dibuja
	SYSEngine::GetGraphicSystem()->Draw(DrawZone, 
									    ubDrawPlane, 
									    swXDrawPos, 
									    swYDrawPos + m_FXInfo.sDrawDelta.swYPos,
									    &m_FXInfo.Sprite);
  }
}; 

#endif // ~ #ifdef _CGFX_H_
