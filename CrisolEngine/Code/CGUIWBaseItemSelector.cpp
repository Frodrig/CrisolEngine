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
// CGUIWBaseItemSelector.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWBaseItemSelector.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWBaseItemSelector.h"

#include "SYSEngine.h"
#include "iCItemContainer.h"
#include "iCWorld.h"
#include "iCGUIManager.h"
#include "CItemContainerIt.h"
#include "CCBTEngineParser.h"
#include "CGUICSpriteSelector.h"
#include "CItem.h"
#include "CMemoryPool.h"
#include <algorithm>

// Definicion de estructuras forward
struct CGUIWBaseItemSelector::sNItemColum {
  // Nodo asociado a las columnas en el mapeo de items
  AreaDefs::EntHandle hItem; // Handle al item
  // Constructor
  sNItemColum(const AreaDefs::EntHandle& ahItem): hItem(ahItem) { }
  sNItemColum(void): hItem(0) { }
  // Operadores
  bool operator==(const CGUIWBaseItemSelector::sNItemColum& ItemColum) const {
	return (hItem == ItemColum.hItem);
  }

  // Pool de memoria
  static CMemoryPool m_MPool;
  static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
  static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); }  
  /**/
};

struct CGUIWBaseItemSelector::sNItemFile {
  // Nodo asociado a la lista de filas
  ColumList Items; // Lista de los nodos asociados al nodo fila actual

  // Pool de memoria
  static CMemoryPool m_MPool;
  static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
  static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); }    
};

struct CGUIWBaseItemSelector::sNVisualizableItem {
  // Nodo asociado a la lista de items visualizables
  AreaDefs::EntHandle hItem;        // Handle al item
  CGUICSpriteSelector ItemSelector; // Selector del item
  
  // Constructor
  sNVisualizableItem(void): hItem(0) { }

  // Operadores
  bool operator==(const CGUIWBaseItemSelector::sNVisualizableItem& VisualizableItem) const {
	return (ItemSelector.GetID() == VisualizableItem.ItemSelector.GetID());
  }
  bool operator==(const AreaDefs::EntHandle& ahItem) const {
    return (hItem == ahItem);
  }
  // Pool de memoria
  static CMemoryPool m_MPool;
  static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
  static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); }  
};

// Clase predicado para la localizacion de nodos sNItemColum con find_if
class CGUIWBaseItemSelector::sFindIfColumPredicate {
  private:
	// Vbles de miembro
	sNItemColum m_ItemColumToSearch; // Nodo a buscar
  public:
	// Constructor
	sFindIfColumPredicate(const AreaDefs::EntHandle& hItem) { 
	  m_ItemColumToSearch.hItem = hItem;
	}
  public:
	// Predicado
	bool operator()(const sNItemColum* const pItemColum) const {
	  ASSERT(pItemColum);
	  return (*pItemColum == m_ItemColumToSearch);
	}
};

// Clase predicado para la localizacion de nodos sNVisualizableItem con find_if
class CGUIWBaseItemSelector::sFindIfVisibleItemPredicate {
  private:
	// Vbles de miembro
	GUIDefs::GUIIDComponent m_IDToSearch; // Identificador a encontrar (ID)

  public:
	// Constructores
	sFindIfVisibleItemPredicate(const GUIDefs::GUIIDComponent& IDToSearch) { 
	  m_IDToSearch = IDToSearch;
	}	

  public:
	// Predicado
	bool operator()(const sNVisualizableItem* const pVisualizableItem) const {
	  ASSERT(pVisualizableItem);
	  return (pVisualizableItem->ItemSelector.GetID() == m_IDToSearch);
	}
};

// Inicializacion del pool de memoria
CMemoryPool 
CGUIWBaseItemSelector::sNItemColum::m_MPool(4,
											sizeof(CGUIWBaseItemSelector::sNItemColum),
											true);
CMemoryPool 
CGUIWBaseItemSelector::sNItemFile::m_MPool(4,
											sizeof(CGUIWBaseItemSelector::sNItemFile),
											true);
CMemoryPool 
CGUIWBaseItemSelector::sNVisualizableItem::m_MPool(1,
												   sizeof(CGUIWBaseItemSelector::sNVisualizableItem),
												   true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseItemSelector::Init(void)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  // Inicializa clase base
  if (Inherited::Init()) {
	// Toma instancia al GFXManager
	m_pGFXManager = SYSEngine::GetGFXManager();
	ASSERT(m_pGFXManager);

	// Todo correcto
	return true;
  }

  // Hubo problemas
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
CGUIWBaseItemSelector::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	// Desactiva
	Deactive();
	
	// Libera las listas
	ReleaseVisualizableItemList();
	ReleaseItemsList();

	// Propaga 
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la llamada a los distintos metodos TLoad
// Parametros:
// - pParser. Parser a utilizar.
// - ubDrawPlane. Plano de dibujado a asociar a los componentes
// - bSelectOnlyWithItem. Indica si se desea seleccionar solo con item o bien
//   se podra seleccionar tambien sin item asociado al slot. Por defecto estara
//   a true.
// - bShowItemNames. Flag para indicar si se desea mostrar el nombre de los
//   items que esten sobre un selector seleccionado. El nombre se mostrara
//   como texto flotante. Por defecto valdra true.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - En los metodos de carga no se establecera prefijo, se usara el que se 
//   haya puesto en la clase derivada
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseItemSelector::TLoad(CCBTEngineParser* const pParser,
							 const byte ubDrawPlane,
							 const bool bSelectOnlyWithItem,
							 const bool bShowItemNames)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Procede a cargar distintos componentes
  // Selectores de items
  if (!TLoadItemSelectors(pParser, ubDrawPlane, bSelectOnlyWithItem)) {
	return false;
  }

  // Info asociada al texto flotante
  if (!TLoadFloatingTextInfo(pParser, bShowItemNames)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de preparar la lista de items visualizable con todos los
//   componentes CGUICSpriteSelector necesarios, leyendo desde disco su
//   configuracion
// Parametros:
// - pParser. Parser a utilizar.
// - ubDrawPlane. Plano de dibujado a asociar a los componentes
// - bSelectOnlyWithItem. Indica si se desea seleccionar solo con item o bien
//   se podra seleccionar tambien sin item asociado al slot.
// - bShowItemNames. Flag para indicar si se desea mostrar el nombre de los
//   items que esten sobre un selector seleccionado. El nombre se mostrara
//   como texto flotante. Por defecto valdra true.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - La lista de items visualizables se creara sin ningun sprite asociado
//   ya que estos no se mapearan hasta que la interfaz sea activada.
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseItemSelector::TLoadItemSelectors(CCBTEngineParser* const pParser,
										  const byte ubDrawPlane,
										  const bool bSelectOnlyWithItem)
{
  // SOLO si parametros correctos
  ASSERT(pParser);
  
  // Obtiene ancho y alto del area de seleccion
  m_InterfazInfo.uwAreaWidth = pParser->ReadNumeric("ItemSelector.Area.Width");
  m_InterfazInfo.uwAreaHeight = pParser->ReadNumeric("ItemSelector.Area.Height");

  // Ajusta valores en anchura y altura
  // Ancho
  if (m_InterfazInfo.uwAreaWidth > CGUIWBaseItemSelector::MAX_AREAWIDTH) {
	m_InterfazInfo.uwAreaWidth = CGUIWBaseItemSelector::MAX_AREAWIDTH;
  } else if (0 == m_InterfazInfo.uwAreaWidth) {
	m_InterfazInfo.uwAreaWidth = 1;
  }
  // Alto
  if (m_InterfazInfo.uwAreaHeight > CGUIWBaseItemSelector::MAX_AREAHEIGHT) {
	m_InterfazInfo.uwAreaHeight = CGUIWBaseItemSelector::MAX_AREAHEIGHT;
  } else if (0 == m_InterfazInfo.uwAreaHeight) {
	m_InterfazInfo.uwAreaHeight = 1;
  }

  // Se lee posicion inicial de dibujado y se establece la maxima
  const sPosition InitDrawPos = pParser->ReadPosition("ItemSelector.");
  const sPosition MaxPosition(InitDrawPos.swXPos + (64 * m_InterfazInfo.uwAreaWidth), 
							  InitDrawPos.swYPos + (64 * m_InterfazInfo.uwAreaHeight));  

  // Se configura la lista de componentes con items visualizables
  // La configuracion mostrara los componentes en filas de tamañ uwAreaHeight
  // y con columnas para cada fila de tamaño uwAreaWidth  
  
  // Vbles
  // Estructura de cfg de componentes selectors de sprites (items)
  CGUICSpriteSelector::sGUICSpriteSelectorInitData CfgSelector;
  // Posicion actual para el componente
  sPosition ActPosition(InitDrawPos); 
  // Identificador del componente actual
  GUIDefs::GUIIDComponent IDAct = CGUIWBaseItemSelector::ID_SPRITESELECTOR_BASE;

  // Se establecen datos comunes en estructura de inicializacion
  CfgSelector.pGUIWindow = this;
  CfgSelector.GUIEvents = GUIDefs::GUIC_SELECT | 
						  GUIDefs::GUIC_UNSELECT |
						  GUIDefs::GUIC_BUTTONLEFT_PRESSED |
						  GUIDefs::GUIC_BUTTONRIGHT_PRESSED;
  CfgSelector.bSelectOnlyWithSprite = bSelectOnlyWithItem;
  CfgSelector.ubDrawPlane = ubDrawPlane;
  CfgSelector.RGBSelect = pParser->ReadRGBColor("ItemSelector.FXBack.Select.");
  CfgSelector.AlphaSelect = pParser->ReadAlpha("ItemSelector.FXBack.Select.");

  // Se leen valores de deseleccion solo si procede
  if (!bSelectOnlyWithItem) {
	CfgSelector.RGBUnselect = pParser->ReadRGBColor("ItemSelector.FXBack.Unselect.");
	CfgSelector.AlphaUnselect = pParser->ReadAlpha("ItemSelector.FXBack.Unselect.");
  } else {
	// En cualquier otro caso, seran completamente invisibles
	CfgSelector.RGBUnselect = GraphDefs::sRGBColor(0, 0, 0);
	CfgSelector.AlphaUnselect = 0;  
  }  
  
  // Se procede a inicializar lista
  while (ActPosition.swYPos < MaxPosition.swYPos) {	
	ActPosition.swXPos = InitDrawPos.swXPos;
	while (ActPosition.swXPos < MaxPosition.swXPos) {
	  // Se crea nodo y se inserta en lista
	  sNVisualizableItem* const pNode = new sNVisualizableItem;
	  ASSERT(pNode);
	  m_InterfazInfo.VisualizableItems.push_back(pNode);	  	    

	  // Se configura componente y se inicializa
	  CfgSelector.ID = IDAct++;
	  CfgSelector.Position = ActPosition;
	  if (!pNode->ItemSelector.Init(CfgSelector)) {
		ReleaseVisualizableItemList();
		return false;
	  }

	  // Se prepara sig. iteracion
	  ActPosition.swXPos += 64;
	}
	// Se prepara sig. iteracion
	ActPosition.swYPos += 64;
  }
  
  // Todo correcto
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga la informacion referida al texto flotante y establece los
//   valores por defecto. 
// Parametros:
// - pParser. Parser a utilizar.
// - bShowItemNames. Flag para indicar si hay que mostrar o no el
//   texto flotante con el nombre del item seleccionado.
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseItemSelector::TLoadFloatingTextInfo(CCBTEngineParser* const pParser,
											 const bool bShowItemNames)
{
  // SOLO si parametros correctos
  ASSERT(pParser);

  // ¿Se desea mostrar los nombres?
  if (bShowItemNames) {
	// Si, se carga el color RGB
	m_FloatingTextInfo.RGBColor = pParser->ReadRGBColor("ItemSelector.FloatingText.");
  } else {
	// No, se establece el color por defecto a blanco
	m_FloatingTextInfo.RGBColor = GraphDefs::sRGBColor(255, 255, 255);
  }

  // Se establecen resto de vbles
  // El valor alpha y el tipo de formato de salida no seran vbles
  m_FloatingTextInfo.Alpha = GraphDefs::sAlphaValue(255);
  m_FloatingTextInfo.Type = GUIManagerDefs::NORMAL_FLOATINGTEXT;
  m_FloatingTextInfo.bShowNames = bShowItemNames;
  m_FloatingTextInfo.hActItem = 0;

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Procede a liberar todos los nodos de informacion almacenados en la lista
//   de lista con los handles de los items que figuran en el contenedor 
//   que la interfaz podra visualizar.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseItemSelector::ReleaseItemsList(void)
{
  // Procede a liberar nodos
  FileListIt FileIt(m_InterfazInfo.Items.begin());
  while (FileIt != m_InterfazInfo.Items.end()) {
	ColumListIt ColumIt((*FileIt)->Items.begin());
	// Se navega por la lista de nodos asociado a la fila, liberandolos
	while (ColumIt != (*FileIt)->Items.end()) {
	  delete *ColumIt;
	  ColumIt = (*FileIt)->Items.erase(ColumIt);
	}
	// Se borra el nodo fila
	delete *FileIt;
	FileIt = m_InterfazInfo.Items.erase(FileIt);
  }

  // Se desasocian iteradores
  m_InterfazInfo.ItemsIt.FileIt = m_InterfazInfo.Items.end();
  m_InterfazInfo.ItemsIt.swNumFile = sItemsIt::NO_ITEMS;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - libera la lista que contiene todos los componentes CGUICSpriteSelector
//   con los items que son visualizados en un determinado momento.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseItemSelector::ReleaseVisualizableItemList(void)
{
  // Procede a liberar nodos almacenados en la lista
  VisualizableItemListIt It(m_InterfazInfo.VisualizableItems.begin());
  while (It != m_InterfazInfo.VisualizableItems.end()) {
	delete *It;
	It = m_InterfazInfo.VisualizableItems.erase(It);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece / quita el texto flotante actual SI Y SOLO SI se habilito la
//   muestra de los nombres de los items selectores.
// - En el caso de que se desee quitar un texto flotante, solo se podra 
//   realizar si el texto flotante actual sea igual al que se quiere poner.
//   Esto es necesario para sincronizar las acciones de establecer y quitar.
// - No se hara caso a llamadas con hItem a 0.
// Parametros:
// - hItem. Item para el que deseamos obtener su nombre y establecerlo.
// - bSet. Flag para establecer o quitar nombre de hItem.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseItemSelector::SetItemName(const AreaDefs::EntHandle& hItem,
								   const bool bSet)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Se habilito la muestra de nombres de items? y
  // ¿hItem es un item valido?
  if (m_FloatingTextInfo.bShowNames &&
	  hItem) {
	// Se obtiene item
	const CItem* const pItem = SYSEngine::GetWorld()->GetItem(hItem);
	ASSERT(pItem);
	  
	// ¿Se desea establecer?
	if (bSet) {
	  // Se establece su nombre
	  SYSEngine::GetGUIManager()->SetFloatingText(pItem->GetName(),
												  m_FloatingTextInfo.RGBColor,
												  m_FloatingTextInfo.Alpha,
												  m_FloatingTextInfo.Type);
	  m_FloatingTextInfo.hActItem = hItem;	  	  
	} else {
	  // No, se desea quitar
	  // ¿El item actual es IGUAL al item al establecer?
	  if (hItem == m_FloatingTextInfo.hActItem) {
		// Si, se quita cualquier texto flotante activo
		SYSEngine::GetGUIManager()->UnsetFloatingText(pItem->GetName());
		m_FloatingTextInfo.hActItem = 0;		
	  }
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el interfaz mapeando todo el contenido del contenedor pItemContainer
//   en la lista de listas que controla el interfaz con los handles a items
//   sensibles de ser visualizables. Tambien inicializara los componentes
//   CGUICSpriteSelector con los items visualizables que procedan.
// Parametros:
// - pItemContainer. Direccion del contenedor de items a visualizar.
// - pClient. Posible cliente asociado
// Devuelve:
// Notas:
// - Los selectores de items se activaran cuando se mapeen a los items
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseItemSelector::Active(iCItemContainer* const pItemContainer,
							  iCGUIWindowClient* const pClient)
{
  // SOLO si intancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(pItemContainer);

  // Activa si procede
  if (!Inherited::IsActive()) {
	// Crea la lista de items y mapea los items visualizables
	CreateItemList(pItemContainer);	
	MapVisualizableItems();

	// Activa sonido asociado a items
	SetItemsInSelectorsVisibles(true);
		
	// Se propaga
	Inherited::Active(pClient);	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva la interfaz
// - Al desactivar la interfaz se perderan todos los datos asociados al
//   contenedor que estuviera presente. Pero no se liberara la lista de
//   items visualizables.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseItemSelector::Deactive(void)
{
  // Desactiva si procede
  if (Inherited::IsActive()) {
	// Libera lista de items
	ReleaseItemsList();

	// Desactiva los componentes de la lista de items visualizables
	ActiveVisualizableSelectors(false);

	// Desactiva sonido asociado a items
	SetItemsInSelectorsVisibles(false);
	
	// Propaga
	Inherited::Deactive();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre la lista de items visualizables, activando o desactivando los
//   componentes CGUICSpriteSelector.
// Parametros:
// - bActive. Flag de activacion / desactivacion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseItemSelector::ActiveVisualizableSelectors(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Procede a activar / desactivar componentes
  VisualizableItemListIt It(m_InterfazInfo.VisualizableItems.begin());
  for (; It != m_InterfazInfo.VisualizableItems.end(); ++It) {
	// Activa selector
	(*It)->ItemSelector.SetActive(bActive);	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo encargado de recorrer todos los items asociados a selectores para
//   enviarles la notificacion acerca de si el item esta o no visible.
// Parametros:
// - bVisible.
// Devuelve:
// Notas:
// - Este metodo sera utilizado como metodo de apoyo en las operaciones
//   de scroll.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseItemSelector::SetItemsInSelectorsVisibles(const bool bVisible)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Procede a notificar la visibilidad de los items
  VisualizableItemListIt It(m_InterfazInfo.VisualizableItems.begin());
  for (; It != m_InterfazInfo.VisualizableItems.end(); ++It) {
	// ¿Tiene item asociado?
	if ((*It)->hItem) {
	  // Si, se manda la notificacion que proceda
	  CItem* const pItem = SYSEngine::GetWorld()->GetItem((*It)->hItem);
	  ASSERT(pItem);
	  pItem->VisibleInScreen(bVisible);
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea la lista de listas de items a visualizar a partir del contenedor
//   recibido.
// - La estructura de la lista de listas sera la siguiente:
//   * Existira una lista principal (lista de filas) que en cada nodo contendra
//     una lista secundaria (lista de columnas) cuyos nodos contendran el handle
//     de uno de los items a visualizar.
//   * La cantidad de nodos en la lista de filas sera identerminada mientras que
//     la cantidad en la lista de columnas sera siempre igual a la anchura del
//     area de visualizacion.
// Parametros:
// - pItemContainer. Direccion del contenedor a partir del cual iniciar la lista
//   de items.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseItemSelector::CreateItemList(iCItemContainer* const pItemContainer)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(pItemContainer);

  // Se inicializa iterador al contenedor
  CItemContainerIt ContainerIt(*pItemContainer);
  while (ContainerIt.IsValid()) {
	// Crea nodo fila
	sNItemFile* const pItemFile = new sNItemFile;
	ASSERT(pItemFile);	

	// Procede a asociar a los nodos de la lista de pItemFile los handles
	word uwItemsInColum = m_InterfazInfo.uwAreaWidth;
	while (ContainerIt.IsValid() && 
		   uwItemsInColum > 0) {
	  // Se crea nodo columa y se inserta en lista
	  sNItemColum* const pItemColum = new sNItemColum(ContainerIt.GetItem());
	  ASSERT(pItemColum);
	  pItemFile->Items.push_back(pItemColum);

	  // Se prepara sig iteracion
	  ContainerIt.Next();
	  --uwItemsInColum;
	}

	// ¿Se inserto algun elemento en esta fila?
	if (!pItemFile->Items.empty()) {
	  // Se inserta nodo de fila en lista
	  m_InterfazInfo.Items.push_back(pItemFile);
	} else {
	  // Se borra el nodo creado y se abandona bucle
	  delete pItemFile;
	  break;
	}
  }

  // Se inicializan iteradores a la lista de items
  if (!m_InterfazInfo.Items.empty()) {
	m_InterfazInfo.ItemsIt.FileIt = m_InterfazInfo.Items.begin();
	m_InterfazInfo.ItemsIt.swNumFile = 0;
  } else {
	m_InterfazInfo.ItemsIt.FileIt = m_InterfazInfo.Items.end();
	m_InterfazInfo.ItemsIt.swNumFile = sItemsIt::NO_ITEMS;
  } 
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se mapean los sprites que representan los handles a los items en la
//   lista de componentes selectores a partir siempre de la posicion del iterador
//   a la fila de nodos.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseItemSelector::MapVisualizableItems(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Iteradores a lista de items
  FileListIt FileIt(m_InterfazInfo.ItemsIt.FileIt);
  ColumListIt ColumIt;
  if (FileIt != m_InterfazInfo.Items.end()) {
	ColumIt = (*FileIt)->Items.begin();
  }						

  // Se toma instancia al universo de juego
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  
  // Se procede a navegar por la lista de selectores a items visualizables
  AreaDefs::EntHandle hActItem = 0;
  VisualizableItemListIt VisibleItemsIt(m_InterfazInfo.VisualizableItems.begin());
  for (; VisibleItemsIt != m_InterfazInfo.VisualizableItems.end(); 
	     ++VisibleItemsIt, hActItem = 0) {
	// Se obtiene sig. handle a item a establecer
	// ¿Estamos ante una fila valida? 
	if (FileIt != m_InterfazInfo.Items.end()) {
	  // ¿El iterador a columa apunta a un nodo NO valido?
	  if (ColumIt == (*FileIt)->Items.end()) {
		// Incrementamos el iterador a fila
	  	++FileIt;

		// ¿Iterador a fila apunta a fila valida?
		if (FileIt != m_InterfazInfo.Items.end()) {
		  // Si, inicializamos iterador a columna
		  ColumIt = (*FileIt)->Items.begin();
		} 
	  } 
	  // Se toma el handle al item
	  // Si no se ha llegado a esta linea, es seguro que hItem vale 0
	  if (FileIt != m_InterfazInfo.Items.end()) {
		hActItem = (*ColumIt)->hItem;
		++ColumIt;
	  }
	} 
		
	// Asociamos el sprite al selector desactivandolo antes para que se
	// produzca un chequeo de seleccion en caso de que el cursor se halle
	// sobre el area del mismo.
	(*VisibleItemsIt)->ItemSelector.SetInput(false);
	(*VisibleItemsIt)->ItemSelector.SetActive(false);			
	if (hActItem) {	  
	  // Se asocia item
	  CItem* const pItem = pWorld->GetItem(hActItem);
	  ASSERT(pItem);
	  (*VisibleItemsIt)->ItemSelector.AttachSprite(pItem, pItem->GetHandle());	  
	} else {
	  // Se desasocia item
	  (*VisibleItemsIt)->ItemSelector.AttachSprite(NULL, 0);	
	}

	// Nota: Es necesario establecer el handle del item AQUI porque de lo
	// contrario, si se ha querido mostrar texto flotante con el nombre
	// del item, no se mostraria nada al ponerse identificador 0 y desactivarse,
	// ya que el evento de deseleccion estaria trabajando con un handle nulo
	(*VisibleItemsIt)->hItem = hActItem;
	(*VisibleItemsIt)->ItemSelector.SetActive(true);		
	(*VisibleItemsIt)->ItemSelector.SetInput(true);		
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa / Desactiva la entrada en componentes
// Parametros:
// - bActive. Flag de activacion / desactivacion de entrada en componentes.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseItemSelector::SetInput(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Recorre la lista de selectores de items, activando / desactivando input
  VisualizableItemListIt VisibleItemsIt(m_InterfazInfo.VisualizableItems.begin());
  for (; VisibleItemsIt != m_InterfazInfo.VisualizableItems.end(); ++VisibleItemsIt) {
	(*VisibleItemsIt)->ItemSelector.SetInput(bActive);	
  }

  // Propaga
  Inherited::SetInput(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Avanza el iterador a fila de items una fila.
// - El tope maximo sera siempre la ultima fila.
// Parametros:
// Devuelve:
// - Si se pudo avanzar true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseItemSelector::NextItemFile(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Es posible avanzar el iterador?
  if (m_InterfazInfo.ItemsIt.swNumFile < m_InterfazInfo.Items.size() - 1) {
	// Se notifica items no visibles
	SetItemsInSelectorsVisibles(false);

	// Se incrementan iteradores
	++m_InterfazInfo.ItemsIt.FileIt;
	++m_InterfazInfo.ItemsIt.swNumFile;

	// Se mapean items visibles
	MapVisualizableItems();

	// Se notifica items visibles y retorna
	SetItemsInSelectorsVisibles(true);
	return true;
  }

  // No se pudo avanzar
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retrocede el iterador en filas visualizables.
// Parametros:
// Devuelve:
// - Si se pudo avanzar true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseItemSelector::PrevItemFile(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Es posible retrasar el iterador?
  if (m_InterfazInfo.ItemsIt.swNumFile > 0) {
	// Se notifica items no visibles
	SetItemsInSelectorsVisibles(false);
	
	// Se decrementan iteradores
	--m_InterfazInfo.ItemsIt.FileIt;
	--m_InterfazInfo.ItemsIt.swNumFile;
	
	// Se mapean items visibles
	MapVisualizableItems();

	// Se notifica items visibles y retorna
	SetItemsInSelectorsVisibles(true);
	return true;
  }

  // No se pudo retroceder
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo para la notificacion de la insercion de un item en el contenedor.
// - El nuevo item siempre sera visualizable al final de interfaz de 
//   visualizacion.
// Parametros:
// - hItem. Handle al item que se ha insertado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseItemSelector::ItemInserted(const AreaDefs::EntHandle& hItem)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(hItem);

  // Se notifica items no visibles
  SetItemsInSelectorsVisibles(false);

  // Se crea el nodo columna que va a mantener al nuevo handle
  sNItemColum* const pNewColum = new sNItemColum(hItem);
  ASSERT(pNewColum);
  
  // ¿Esta la lista de items vacia?
  if (m_InterfazInfo.Items.empty()) {
	// Se crea la primera fila y se inserta en la lista de items.
	sNItemFile* const pNewFile = new sNItemFile;
	ASSERT(pNewFile);
	m_InterfazInfo.Items.push_back(pNewFile);

	// Se inicializan iteradores globales
	m_InterfazInfo.ItemsIt.FileIt = m_InterfazInfo.Items.begin();
	m_InterfazInfo.ItemsIt.swNumFile = 0;
  }

  // Se localiza iterador a la fila donde insertar el nodo columna con el handle
  // nota: se cumplira que dicha fila siempre sera la ultima actual
  FileListIt It(m_InterfazInfo.Items.begin());
  std::advance(It, m_InterfazInfo.Items.size() - 1);
  ASSERT((It != m_InterfazInfo.Items.end()) != 0);
  // ¿No caben mas handles en esa columna?
  if (m_InterfazInfo.uwAreaWidth == (*It)->Items.size()) {
	// Se crea una nueva fila
	sNItemFile* const pNewFile = new sNItemFile;
	ASSERT(pNewFile);
	m_InterfazInfo.Items.push_back(pNewFile);

	// Se avanza iterador a la misma
	++It;
  }

  // Estamos en una fila en donde poder insertar el nodo columna con el handle
  // Lo insertamos y mapeamos visualmente los items si procede
  (*It)->Items.push_back(pNewColum);
  if (!m_InterfazInfo.VisualizableItems.back()->hItem) {	
	MapVisualizableItems();
  }

  // Se notifica items visibles
  SetItemsInSelectorsVisibles(true);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notifica que un handle asociado al contenedor que la interfaz muestra
//   ha sido borrado. En concreto, el handle hItem.
// - Al borrar un item, se sigue un proceso de localizacion de este en 
//   la estructura. Al localizarlo y borrar el nodo columna en donde estuviera,
//   se debera de reordenar la estructura desde esa posicion, trayendo un
//   nodo columna desde la siguiente fila y borrandolo a su vez de esta.
//   El proceso se repetiria hasta llegar a la ultima fila, pudiendose dar el
//   caso de que se elimine una fila por completo. 
// Parametros:
// - hItem. Handle que ha sido borrado
// Devuelve:
// Notas:
// - Se considerara que cuando se recibe una notificacion de este tipo, el 
//   handle ha de existir siempre en la lista de listas.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseItemSelector::ItemRemoved(const AreaDefs::EntHandle& hItem)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(hItem);

  // Antes de proceder, es seguro que el selector se hallara seleccionado
  // por lo que se desvinculara el texto flotante asociado al nombre
  SYSEngine::GetGUIManager()->UnsetFloatingText();

  // Se notifica items no visibles
  SetItemsInSelectorsVisibles(false);

  // Localizamos el iterador a la fila en donde esta el handle borrandolo
  const sFindIfColumPredicate FindPredicate(hItem);
  FileListIt FileIt(m_InterfazInfo.Items.begin());  
  for (; FileIt != m_InterfazInfo.Items.end(); ++FileIt) {
	// Se intenta localizar el nodo handle
	ColumListIt ColumIt(std::find_if((*FileIt)->Items.begin(),
									 (*FileIt)->Items.end(),
									 FindPredicate));
	if (ColumIt != (*FileIt)->Items.end()) {
	  // Encontrado, se borra el nodo y se abandona el bucle
	  delete *ColumIt;
	  ColumIt = (*FileIt)->Items.erase(ColumIt);
	  break;
	}	
  }

  // ¿Hemos borrado de la ultima fila TENIENDO EL ITERADOR EN LA MISMA?
  ASSERT((FileIt != m_InterfazInfo.Items.end()) != 0);
  if (m_InterfazInfo.Items.size() - 1 == m_InterfazInfo.ItemsIt.swNumFile) {
	// ¿NO quedan elementos en la ultima fila?
	if ((*FileIt)->Items.empty()) {	
	  // Borramos el nodo a la ultima fila
	  delete *FileIt;
	  m_InterfazInfo.Items.erase(FileIt);

	  // Recolocamos iteradores	  
	  --m_InterfazInfo.ItemsIt.swNumFile;
	  if (m_InterfazInfo.ItemsIt.swNumFile != sItemsIt::NO_ITEMS) {
		m_InterfazInfo.ItemsIt.FileIt = m_InterfazInfo.Items.begin();
		std::advance(m_InterfazInfo.ItemsIt.FileIt, m_InterfazInfo.Items.size() - 1);
	  } else {
		m_InterfazInfo.ItemsIt.FileIt = m_InterfazInfo.Items.end();
	  }
	}
  } else {
	// No hemos borrado de la ultima fila teniendo el iterador actual a la misma
	// Se crea iterador a sig. fila a partir de donde se borro el handle
	FileListIt FileSigIt(FileIt);
	++FileSigIt;
	for (; FileSigIt != m_InterfazInfo.Items.end(); ++FileSigIt, ++FileIt) {
	  // Se toma el primer nodo de FileSigIt y se inserta al final de FileIt
	  (*FileIt)->Items.push_back((*FileSigIt)->Items.front());
	  (*FileSigIt)->Items.pop_front();
	}
	
	// ¿La ultima fila se queda sin items?
	if ((*FileIt)->Items.empty()) {
	  // Descendemos una fila
	  delete *FileIt;
	  m_InterfazInfo.Items.erase(FileIt);
	}
  }

  // Se mapea visualmente los items
  MapVisualizableItems();

  // Se notifica items no visibles
  SetItemsInSelectorsVisibles(true);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion de un evento producido sobre un componente
// - Al seleccionar y deseleccionar se llamara tambien al metodo destinado a
//   establecer el nombre del item seleccionado como texto flotante.
// Parametros:
// - GUICEvent. Evento
// Devuelve:
// Notas:
// - En caso de que se halla elegido que solo se pueda escoger de un
//   selector cuando este tenga asociado un item, siempre se recibira
//   el handle a un item valido. En caso contrario, podra ser o no ser
//   un item valido.	  
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseItemSelector::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se localiza el nodo
  VisualizableItemListIt It;
  FindVisibleItemNode(GUICEvent.IDComponent, It);
  ASSERT((It != m_InterfazInfo.VisualizableItems.end()) != 0);

  // Comprueba el tipo de evento y realiza operacion
  switch(GUICEvent.Event) {
	case GUIDefs::GUIC_SELECT: {	
	  // Seleccion
	  (*It)->ItemSelector.Select(true);
	  SetItemName((*It)->hItem, true);
	} break;

  	case GUIDefs::GUIC_UNSELECT: {	
	  // Deseleccion
	  (*It)->ItemSelector.Select(false);
	  SetItemName((*It)->hItem, false);
	} break;
	
	case GUIDefs::GUIC_BUTTONLEFT_PRESSED: {	  	  
	  // Pulsacion con boton izquierdo sobre un selector de item.
	  OnItemLeftClick((*It)->hItem, (*It)->ItemSelector);	  
	} break;	  

	 case GUIDefs::GUIC_BUTTONRIGHT_PRESSED: {	  	  
	  // Pulsacion con boton derecho sobre un selector de item.	  
	  OnItemRightClick((*It)->hItem, (*It)->ItemSelector);	  
	} break;	  
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Localiza el nodo en donde se halla el selector cuyo identificador es
//   IDComponent y referencia It a dicho nodo.
// Parametros:
// - IDComponent. Identificador del componente a encontrar.
// - It. Iterador a dicho nodo
// Devuelve:
// Notas:
// - Se supone que SIEMPRE se debera de encontrar
//////////////////////////////////////////////////////////////////////////////
void
CGUIWBaseItemSelector::FindVisibleItemNode(const GUIDefs::GUIIDComponent& IDComponent,
										   VisualizableItemListIt& It)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(IDComponent);

  // Procede a localizar el nodo
  const sFindIfVisibleItemPredicate PredicateToSearch(IDComponent);
  It = std::find_if(m_InterfazInfo.VisualizableItems.begin(),
				    m_InterfazInfo.VisualizableItems.end(),
				    PredicateToSearch);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado de todos los componentes selectores de items
// Parametros:
// Devuelve:
// Notas:
// - Se dejara en responsabilidad de las clases derivadas comprobar si
//   hay activacion de interfaz.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseItemSelector::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Recorre la lista de items dibujado
  VisualizableItemListIt It(m_InterfazInfo.VisualizableItems.begin());
  for (; It != m_InterfazInfo.VisualizableItems.end(); ++It) {
	// Llamada de dibujado al selector
	(*It)->ItemSelector.Draw();  
  }
}