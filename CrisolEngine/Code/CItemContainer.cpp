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
// CItemContainer.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CItemContainer.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CItemContainer.h"

#include "SYSEngine.h"
#include "iCWorld.h"
#include "iCFileSystem.h"
#include "CItem.h"
#include "CItemContainerIt.h"
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia.
// Parametros:
// - hOwner. Propietarion del contenedor.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CItemContainer::Init(const AreaDefs::EntHandle& hOwner)
{
  // SOLO si parametros validos
  ASSERT(hOwner);

  // ¿Se intenta reinicializar?
  if (IsInitOk()) {
	return false;
  }

  // Se asocia propietario
  m_ContainerInfo.hOwner = hOwner;

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
CItemContainer::End(void)
{
  // Finaliza si procede
  if (IsInitOk()) {
	// Se libera lista de items
	m_ContainerInfo.Items.clear();

	// Se baja el flag
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Guarda los items almacenados en disco.
// Parametros:
// - hFile. Handle al fichero donde almacenar.
// - udOffset. Offset donde depositar la info.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CItemContainer::Save(const FileDefs::FileHandle& hFile, 
					 dword& udOffset)
{
  // SOLO si parametros correctos
  ASSERT(hFile);
 
  // Se guarda el numero de items en inventario
  iCFileSystem* const pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys);  
  const word uwNumItems = m_ContainerInfo.Items.size();
  udOffset += pFileSys->Write(hFile, udOffset, (sbyte *)(&uwNumItems), sizeof(word));
  
  // Se iterara por la lista de items y se guardaran
  if (uwNumItems > 0) {
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);
	ItemListIt It(m_ContainerInfo.Items.begin());
	for (; It != m_ContainerInfo.Items.end(); ++It) {
	  CItem* const pItem = pWorld->GetItem(*It);
	  ASSERT(pItem);
	  pItem->Save(hFile, udOffset);
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inserta el item hItem en el contenedor. 
// - Para que la insercion se pueda llevar a cabo, el Item no debera de tener
//   propietario y ademas se le asociara el dueño del contenedor.
// Parametros:
// - hItem. Item a insertar.
// Devuelve:
// Notas:
// - Todos los items en el contenedor se pausaran al introducirse, la pausa
//   se quitara a titulo particular al mostrarse en interfaz o depositarlos
//   sobre terreno visible.
///////////////////////////////////////////////////////////////////////////////
void
CItemContainer::Insert(const AreaDefs::EntHandle& hItem)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hItem);

  // Se obtiene instancia al item
  CItem* const pItem = SYSEngine::GetWorld()->GetItem(hItem);
  ASSERT(pItem);

  // Se asocia nuevo dueño y se inserta  
  ASSERT(!pItem->GetOwner());
  pItem->SetOwner(m_ContainerInfo.hOwner);
  m_ContainerInfo.Items.push_back(hItem);

  // Se pausa
  pItem->SetPause(true);
  
  // Notifica al posible observer y retorna
  ObserverNotify(ItemContainerDefs::ITEM_INSERTED, hItem);	
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Extrae de la lista el item hItem y notifica.
// Parametros:
// - hItem. Item a extraer.
// Devuelve:
// - Si se pudo localizar el item y extraer true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CItemContainer::Extract(const AreaDefs::EntHandle& hItem)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hItem);

  // Se localiza el nodo con hItem y se borra
  ItemListIt It = std::find(m_ContainerInfo.Items.begin(),
							m_ContainerInfo.Items.end(),
							hItem);
  ASSERT((It != m_ContainerInfo.Items.end()) != 0)
  m_ContainerInfo.Items.erase(It);
	
  // Se obtiene instancia al item y se le desasocia el dueño
  CItem* const pItem = SYSEngine::GetWorld()->GetItem(hItem);
  ASSERT(pItem);
  pItem->SetOwner(0);

  // Notifica al posible observer y retorna
  ObserverNotify(ItemContainerDefs::ITEM_REMOVED, hItem);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Procede a extraer todos y cada uno de los items.
// Parametros:
// Devuelve:
// Notas:
// - Internamente, el metodo basara su funcionamiento en continuas llamadas
//   a Extract
///////////////////////////////////////////////////////////////////////////////
void 
CItemContainer::ExtractAll(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Procede a eliminar elementos
  while (m_ContainerInfo.Items.size()) {
	Extract(m_ContainerInfo.Items.front());
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Actual sobre todos los items que esten en el contenedor
// Parametros:
// - bPause. Flag de pausa
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CItemContainer::SetPause(const bool bPause)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Recorre lista de items
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  ItemListIt It(m_ContainerInfo.Items.begin());
  for (; It != m_ContainerInfo.Items.end(); ++It) {
    CItem* const pItem = pWorld->GetItem(*It);
    ASSERT(pItem);
    pItem->SetPause(bPause);
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa a partir de otro iterador.
// Parametros:
// - It. Referencia al iterador a partir del que inicializar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CItemContainer::InitItemContainerIt(CItemContainerIt& It) 
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Pasa la direccion de la lista de items
  It.m_pItems = &m_ContainerInfo.Items;
}
