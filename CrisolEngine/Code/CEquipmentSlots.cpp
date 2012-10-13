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
// CEquipmentSlots.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CEquipmentSlots.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CEquipmentSlots.h"

#include "SYSEngine.h"
#include "iCWorld.h"
#include "iCGameDataBase.h"
#include "iCFileSystem.h"
#include "RulesDefs.h"
#include "CItem.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia.
// Parametros:
// - hOwner. Dueño del contenedor
// - uwSize. Tamaño del contenedor en cuanto al numero de slots que este posee
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se dejara reinicializar
// - Pese a reservar el numero maximo de posibles slots de equipamiento, las
//   reglas podran limitar el uso maximo de los mismos.
///////////////////////////////////////////////////////////////////////////////
bool 
CEquipmentSlots::Init(const AreaDefs::EntHandle& hOwner)
{
  // SOLO si parametros validos
  ASSERT(hOwner);
  
  // ¿Se intenta reinicializar?
  if (IsInitOk()) {
	return false;
  }

  // Se obtiene el numero de slots de equipamiento posibles  y se inicializan
  m_EquipSlotsInfo.EquipSlots.reserve(RulesDefs::MAX_CRIATURE_EQUIPMENTSLOT);
  word It = 0;
  for (; It < RulesDefs::MAX_CRIATURE_EQUIPMENTSLOT; ++It) {
	m_EquipSlotsInfo.EquipSlots.push_back(0);
  }

  // Se toma propietario
  m_EquipSlotsInfo.hOwner = hOwner;

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
CEquipmentSlots::End(void)
{
  // Finaliza si procede
  if (IsInitOk()) {
	// Libera contenedor y baja flag
	m_EquipSlotsInfo.EquipSlots.clear();
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
// - No se usara un metodo que recorra previamente el vector de slots para
//   hallar el numero de items equipados por motivos de optimizacion.
///////////////////////////////////////////////////////////////////////////////
void 
CEquipmentSlots::Save(const FileDefs::FileHandle& hFile, 
					  dword& udOffset)
{
  // SOLO si parametros correctos
  ASSERT(hFile);
 
  // Se guarda el numero de slots de equipamiento con datos de forma temporal
  iCFileSystem* const pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys); 
  word uwNumItems = 0;
  const dword udNumItemsOffset = udOffset;
  udOffset += pFileSys->Write(hFile, udOffset, (sbyte *)(&uwNumItems), sizeof(word));

  // Se itera por la lista de items guardandolos  
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);  
  byte ubSlotIt = 0;
  EquipSlotsVectorIt It(m_EquipSlotsInfo.EquipSlots.begin());
  for (; It != m_EquipSlotsInfo.EquipSlots.end(); ++It, ++ubSlotIt) {
	// ¿Slot con informacion?
	if (*It) {
	  // Se guardan datos
	  // Identificador slot
	  const RulesDefs::eIDEquipmentSlot Slot = RulesDefs::eIDEquipmentSlot(ubSlotIt);
	  udOffset += pFileSys->Write(hFile, 
							      udOffset, 
							      (sbyte *)(&Slot), 
							      sizeof(RulesDefs::eIDEquipmentSlot));	  

	  // Datos del item equipado
	  CItem* const pItem = pWorld->GetItem(*It);
	  ASSERT(pItem);
	  pItem->Save(hFile, udOffset);

	  // Se incrementa contador
	  ++uwNumItems;
	}
  }

  // Se guarda el valor final del numero de slots de equipamiento con datos
  if (uwNumItems > 0) {
	pFileSys->Write(hFile, udNumItemsOffset, (sbyte *)(&uwNumItems), sizeof(word));
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Equipa un item en el slot que proceda. El slot debera de ser valido. Las
//   posiciones siempre estaran entre 0 y el maximo numero de slots - 1.
// Parametros:
// - hItem. Handle al item.
// - Slot. Slot de equipamiento para el item.
// Devuelve:
// - El item previamente asociado al contenedor.
// Notas:
// - Se dejara en responsabilidad el exterior, desequipar antes de equipar
//   para que se genere un evento de notificacion al observer adecuado.
// - No hara falta manipular la pausa del item equipado, ya que proviene del
//   inventario y es seguro que en el inventario, salvo causa concreta, el
//   item se hallara pausado.
///////////////////////////////////////////////////////////////////////////////
AreaDefs::EntHandle 
CEquipmentSlots::EquipItem(const AreaDefs::EntHandle& hItem,
						   const RulesDefs::EquipmentSlot& Slot)
{
  // SOLO si instancia incializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hItem);  
  ASSERT((Slot < m_EquipSlotsInfo.EquipSlots.size()) != 0);

  // Se obtiene item en slot
  const AreaDefs::EntHandle hOldItem = GetItemEquipedAt(Slot);

  // Se obtiene instancia al item
  CItem* const pItem = SYSEngine::GetWorld()->GetItem(hItem);
  ASSERT(pItem);

  // Se asocia nuevo dueño y se inserta
  ASSERT((!pItem->GetOwner()) != 0)
  pItem->SetOwner(m_EquipSlotsInfo.hOwner);  
  m_EquipSlotsInfo.EquipSlots[Slot] = hItem;

  // Notifica al posible observer y retorna el item previamente equipado
  ObserverNotify(EquipmentSlotsDefs::ITEM_EQUIPED, Slot);	
  return hOldItem;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desequipa un item que estuviera equipado en el slot de equipamiento Slot.
//   Al desequipar el item lo devolvera al inventario de la criatura.
// Parametros:
// - Slot. Slot de equipamiento.
// Devuelve:
// - El item que estaba en el slot Slot
// Notas:
// - La notificacion al posible observer solo se producira si hubiera un
//   item equipado.
///////////////////////////////////////////////////////////////////////////////
AreaDefs::EntHandle 
CEquipmentSlots::UnequipItem(const RulesDefs::EquipmentSlot& Slot)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT((Slot < m_EquipSlotsInfo.EquipSlots.size()) != 0);

  // Obtiene el item en dicho slot y desequipa si procede
  const AreaDefs::EntHandle hItem = GetItemEquipedAt(Slot);
  if (hItem) {
	// Elimina el dueño del item
	CItem* const pItem = SYSEngine::GetWorld()->GetItem(hItem);
	ASSERT(pItem);
	pItem->SetOwner(0);

	// Desequipa y notifica al posible observer
	m_EquipSlotsInfo.EquipSlots[Slot] = 0;	
	ObserverNotify(EquipmentSlotsDefs::ITEM_UNEQUIPED, Slot);  

	// Retorna handle de item desequipado
	return hItem;
  }

  // No habia nada que desequipar
  return 0;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el slot donde esta el item hItem equipado.
// Parametros:
// - hItem. Item a localizar.
// - Slot. Referencia a slot donde depositar el slot donde esta el item
// Devuelve:
// - El slot donde se halla o -1 si no esta equipado.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool
CEquipmentSlots::WhereIsItemEquiped(const AreaDefs::EntHandle& hItem,
									RulesDefs::EquipmentSlot& Slot)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(hItem);

  // Busca el item  
  RulesDefs::EquipmentSlot SlotSearch = 0;
  EquipSlotsVectorIt It(m_EquipSlotsInfo.EquipSlots.begin());
  for (; It != m_EquipSlotsInfo.EquipSlots.end(); ++It, ++SlotSearch) {
	// ¿Localizado el item?
	if (*It == hItem) {
	  // Se deposita el slot y se retorna
	  Slot = SlotSearch;
	  return true;
	}
  }

  // No se hallo slot con el item hItem
  return false;
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
CEquipmentSlots::SetPause(const bool bPause)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Recorre lista de items
  iCWorld* const pWorld = SYSEngine::GetWorld();
  EquipSlotsVectorIt It(m_EquipSlotsInfo.EquipSlots.begin());
  for (; It != m_EquipSlotsInfo.EquipSlots.end(); ++It) {
	// ¿Hay item equipado?
	if (*It) {
	  // Si, se actua
	  CItem* const pItem = pWorld->GetItem(*It);
	  ASSERT(pItem);
	  pItem->SetPause(bPause);	
	}
  }
}

