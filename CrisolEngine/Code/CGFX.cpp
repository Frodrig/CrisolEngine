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
// CGUIManager.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIManager.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGFX.h"

#include "iCWorld.h"
#include "iCGraphicSystem.h"
#include "CWorldEntity.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia
// Parametros:
// - hEntity. Handle a la entidad para la que esta asociada
// - szGFXAnimTemplate. Plantilla de animacion asociada.
// - DrawPosition. Posicion de dibujado para el GFX.
// Devuelve:
// - Si todo es correcto true. En caso contrario false.
// Notas:
// - No se permitira reinicializar
///////////////////////////////////////////////////////////////////////////////
bool 
CGFX::Init(const AreaDefs::EntHandle& hEntity,
		   const std::string& szGFXAnimTemplate,
		   const GFXManagerDefs::eGFXDrawPosition& DrawPosition)
{
  // SOLO si parametros correctos
  ASSERT(!szGFXAnimTemplate.empty());

  // ¿Se intenta reinicializar?
  if (IsInitOk()) {
	return false;
  }

  // Se inicializa sprite que representara al GFX
  if (!m_FXInfo.Sprite.Init(szGFXAnimTemplate)) {
	return false;
  }
  
  // Se halla el delta de dibujado, segun sean las dimensiones de la entidad
  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(hEntity);
  ASSERT(pWorldEntity);
  switch (DrawPosition) {
	case GFXManagerDefs::OVER_ENTITY_TOP: {
	  // Por encima de la entidad
	  m_FXInfo.sDrawDelta.swYPos = -m_FXInfo.Sprite.GetHeight();
	} break;

	case GFXManagerDefs::AT_ENTITY_TOP: {
	  // En la misma posicion con respecto a la entidad
	  m_FXInfo.sDrawDelta.swYPos = 0;
	} break;

	case GFXManagerDefs::AT_ENTITY_BOTTOM: {
	  m_FXInfo.sDrawDelta.swYPos = pWorldEntity->GetHeight() - m_FXInfo.Sprite.GetHeight();
	  if (m_FXInfo.sDrawDelta.swYPos < 0) {
		m_FXInfo.sDrawDelta.swYPos = 0;
	  }
	} break;

	case GFXManagerDefs::UNDER_ENTITY_BOTTOM: {
	  // Por debajo de la base de la entidad
	  m_FXInfo.sDrawDelta.swYPos = pWorldEntity->GetHeight();
	} break;
  }; // ~ switch  

  // Asocia las direcciones de los componentes alpha y RGB
  byte ubIt = 0;
  for (; ubIt < 4; ++ubIt) {
	// Se asocian estructuras a los vertices
	const GraphDefs::eVertex Vertex = GraphDefs::eVertex(ubIt);
	m_FXInfo.Sprite.SetRGBColor(Vertex, &m_FXInfo.RGBColors[ubIt]);
	m_FXInfo.Sprite.SetAlphaValue(Vertex, &m_FXInfo.Alphas[ubIt]);
  }

  // Todo correcto
  m_bIsInitOk = true;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGFX::End(void)
{
  // Finaliza si procede
  if (IsInitOk()) {
	// Finaliza FXsprite
	m_FXInfo.Sprite.End();	
	
	// Se resetean los posibles valores en las estructuras Alpha y RGB
	byte ubIt = 0;
	for (; ubIt < 4; ++ubIt) {
	  m_FXInfo.RGBColors[ubIt] = GraphDefs::sRGBColor(255, 255, 255);
	  m_FXInfo.Alphas[ubIt] = GraphDefs::sAlphaValue(255);
	}

	// Se baja flag
	m_bIsInitOk = false;
  }
}
