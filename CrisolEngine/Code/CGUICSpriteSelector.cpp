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
// CGUICSpriteSelector.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUICSpriteSelector.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUICSpriteSelector.h"

#include "SYSEngine.h"
#include "iCGraphicSystem.h"
#include "iCGFXManager.h"
#include "CSprite.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia
// Parametros:
// - InitData. Estructura con datos de inicializacion
// Devuelve:
// - Si instancia inicializada true. En caso contrario false.
// Notas:
// - No se permitira reinicializar
// - Inicialmente el componente se establecera en estado de deseleccion
///////////////////////////////////////////////////////////////////////////////
bool 
CGUICSpriteSelector::Init(const sGUICSpriteSelectorInitData& InitData)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  // Se inicializa clase base
  if (Inherited::Init(InitData)) {
	// Se crean los FXBack
	// Seleccion	
	if (m_SelectorInfo.SelectFXBack.Init(InitData.Position,
										 Inherited::GetDrawPlane(),
									     InitData.uwSelectorWidth,
										 InitData.uwSelectorHeight,
										 InitData.RGBSelect,
										 InitData.AlphaSelect)) {
	  // Deseleccion
	  if (m_SelectorInfo.UnselectFXBack.Init(InitData.Position,
											 Inherited::GetDrawPlane(),
											 InitData.uwSelectorWidth,
											 InitData.uwSelectorHeight,
											 InitData.RGBUnselect,
											 InitData.AlphaUnselect)) {
		// Se establecen resto de vbles
		m_SelectorInfo.pActFXBack = &m_SelectorInfo.UnselectFXBack;
		m_SelectorInfo.pAttachedSprite = NULL;
		m_SelectorInfo.bSelectOnlyWithSprite = InitData.bSelectOnlyWithSprite;
		
		// Todo correcto
		return true;
	  }
	}
  }

  // No se pudo inicializar
  End();
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICSpriteSelector::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	// Finaliza FXBacks
	m_SelectorInfo.SelectFXBack.End();
	m_SelectorInfo.UnselectFXBack.End();
	
	// Desasocia punteros
	m_SelectorInfo.pAttachedSprite = NULL;
	m_SelectorInfo.pActFXBack = NULL;

	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia el sprite pSprite como sprite actual, devolviendo el que estuviera
//   previamente seleccionado.
// - En caso de que se quiera desasociar el sprite actual, bastara con que
//   pSprite valga NULL.
// Parametros:
// - pSprite. Direccion al sprite que queremos asociar.
// - hEntity. Handle a la posible entidad a la que pertenece el sprite asociado.
//            Se utilizara para dibujar los posibles GFX asociados.
// Devuelve:
// - Sprite previamente asociado o NULL si no lo hubiera.
// Notas:
///////////////////////////////////////////////////////////////////////////////
CSprite* const 
CGUICSpriteSelector::AttachSprite(CSprite* const pSprite,
								  const AreaDefs::EntHandle& hEntity)
{
  // SOLO si intancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se toma el sprite recibido y se devuelve el previo  
  CSprite* const pOldSprite = m_SelectorInfo.pAttachedSprite;
  m_SelectorInfo.pAttachedSprite = pSprite;
  m_SelectorInfo.hEntity = hEntity;
  
  // En caso de desasociar sprite, se deseleccionara SOLO si es obligatorio
  // Habia seleccion
  if (IsSelect()) {
	// ¿Se desasocia sprite? Y
	// ¿Es es obligatorio que haya sprite para estar seleccionado?
	if (NULL == pSprite &&
	    m_SelectorInfo.bSelectOnlyWithSprite) {
	  // Si, se deselecciona
	  Select(false);
	}    
  }
  
  // Retorna
  return pOldSprite;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la operacion de dibujado
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICSpriteSelector::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Dibuja si procede
  if (Inherited::IsActive()) {
	// FXBack actual
	m_SelectorInfo.pActFXBack->Draw();
	// Sprite asociado si procede
	if (m_SelectorInfo.pAttachedSprite) {
	  // Dibujado del sprite
	  SYSEngine::GetGraphicSystem()->Draw(GraphDefs::DZ_GUI,
										  Inherited::GetDrawPlane(),
										  Inherited::GetXPos(),
										  Inherited::GetYPos(),
										  m_SelectorInfo.pAttachedSprite);

	  // Dibujado del posible GFX asociado
	  // Nota: Solo si se paso handle a entidad
   	  if (m_SelectorInfo.hEntity) {
		SYSEngine::GetGFXManager()->DrawGFXAttached(m_SelectorInfo.hEntity,
												    Inherited::GetXPos(),
												    Inherited::GetYPos(),
												    GraphDefs::DZ_GUI,
												    Inherited::GetDrawPlane());
	  }	 
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Selecciona / deselecciona a la entidad.
// - En caso de querer seleccionar y estar activo el flag que obliga a que haya
//   sprite asociado, la llamada se desechara.
// Parametros:
// - bSelect. Flag de seleccion. 
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICSpriteSelector::Select(const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Se quiere seleccionar?
  if (bSelect) {	
	// ¿Es posible?	
	if ((!m_SelectorInfo.bSelectOnlyWithSprite) ||
	    (m_SelectorInfo.bSelectOnlyWithSprite &&
		 m_SelectorInfo.pAttachedSprite)) {	
	  // Se establece flag y se cambia de FX
	  m_SelectorInfo.bIsSelect = bSelect;
	  m_SelectorInfo.pActFXBack = &m_SelectorInfo.SelectFXBack;
	}
  } else {
	// Se establece flag y se cambia de FX
	m_SelectorInfo.bIsSelect = bSelect;
	m_SelectorInfo.pActFXBack = &m_SelectorInfo.UnselectFXBack;
  }
}
