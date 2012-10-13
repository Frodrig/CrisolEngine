///////////////////////////////////////////////////////////////////////////////
// CPriorityQueue.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CPriorityQueue.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////

// Necesarios al trabajar con templates en archivos .h y .cpp
#ifndef _CPRIORITYQUEUE_CPP_
#define _CPRIORITYQUEUE_CPP_

#include "CPriorityQueue.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el monticulo. Un motinculo tendra siempre un tamaño fijo y
//   estara representado por un array. En este caso, el array sera de nodos 
//   HeapNode. No se permitira la reinicializacion, para ello habra que llamar
//   antes al metodo Release.
// Parametros:
// - udHeapSize. Tamaño del monticulo
// Devuelve:
// - En caso de que todo vaya bien, true. Si no se puede inicializar porque
//   ya se encuentra inicializado o no hay memoria suficiente, false.
// Notas:
// - Recordar que siempre se utilizara la posicion 0 como centinela.
///////////////////////////////////////////////////////////////////////////////
template <class TypeInfo> bool 
CPriorityQueue<TypeInfo>::Init(const dword udHeapSize)
{
  // SOLO si el tamaño es valido
  ASSERT(udHeapSize);

  // ¿Se intenta reinicializar?
  if (IsInitOk()) { return false; }

  // Se procede a crear memoria
  m_pHeap = new HeapNode[udHeapSize + 1];
  if (!m_pHeap) {
    // No hay memoria suficiente
    return false;
  }
  
  // Se inicializan vbles de miembro
  m_udHeapSize = udHeapSize + 1;  
  m_udNumItems = 0;
  m_pIt = NULL;
  
  // Se inicializan punteros a info y centinela
  for (dword i = 1; i < m_udHeapSize; ++i) { m_pHeap[i].pInfo = NULL; }
  m_pHeap[0].udPriority = 0;  
  
  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la insercion de un elemento mas en el monticulo, siempre y cuando
//   haya suficiente espacio. La insercion se realizara teniendo en cuenta el
//   valor de Key, que sera una prioridad. El monticulo pasara a tener un nuevo
//   elemento. La insercion se realizara mediante un filtrado ascendente.
// Parametros:
// - udPriority. Prioridad de insercion.
// - pInfo. Informacion a alojar en el nodo.
// Devuelve:
// - En caso de que la insercion se haya efectuado true. Si no se ha podido
//   realizar (falta de espacio) false.
// Notas:
// - En la insercion se realizara un filtrado ascendente; el elemento se inserta
//   al final y se va promocionando su posicion hasta lograr el sitio adecuado.
//   Para ello, se llamara a AFilter, que dejara a la vble recibida en la
//   posicion justa donde habra que insertar el nuevo elemento (tanto su valor
//   como su prioridad).
///////////////////////////////////////////////////////////////////////////////
template <class TypeInfo> bool 
CPriorityQueue<TypeInfo>::Insert(const dword udPriority, TypeInfo* const pInfo)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si los parametros correctos
  ASSERT(pInfo);

  // ¿NO hay suficiente espacio?
  if (IsFull()) { 
	return false; 
  }

  // Se ajusta el nuevo tamaño y se inicializa 
  // udPos para someterla a un filtrado ascendente
  m_udNumItems++;  
  dword udPos = m_udNumItems;    
  AFilter(udPriority, udPos);

  // Se inserta el nuevo elemento y su prioridad en udPos
  ASSERT(m_pHeap);
  ASSERT(udPos);
  m_pHeap[udPos].udPriority = udPriority;
  m_pHeap[udPos].pInfo = pInfo;

  // Se desvincula iterador
  m_pIt = NULL;
  
  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el elemento de informacion que se encuentra en el tope del monticulo. 
//   Seguro que sera el de mayor prioridad (o uno de los de mayor prioridad)
// Parametros:
// Devuelve:
// - Puntero a la informacion almacenada en el nodo que ocupa el tope de
//   monticulo. En caso de que no haya elementos NULL
// Notas:
///////////////////////////////////////////////////////////////////////////////
template <class TypeInfo> TypeInfo* const 
CPriorityQueue<TypeInfo>::GetTopInfo(void)
{
  // SOLO si esta inicializado
  ASSERT(IsInitOk());

  // ¿NO hay elementos?
  if (0 == m_udNumItems) { 
	return NULL; 
  }

  // Devuelve la info del primer elemento
  ASSERT(m_pHeap[1].pInfo);
  return m_pHeap[1].pInfo;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el elemento de prioridad que se encuentra en el tope del monticulo. 
//   Seguro que sera el de mayor prioridad (o uno de los de mayor prioridad)
// Parametros:
// Devuelve:
// - Valor de prioridad. En caso de que no haya elementos, devolvera 0. Sera
//   aconsejable comprobar este hecho con una llamada a GetNumItems.
// Notas:
///////////////////////////////////////////////////////////////////////////////
template <class TypeInfo> dword 
CPriorityQueue<TypeInfo>::GetTopPriority(void)
{
  // SOLO si esta inicializado
  ASSERT(IsInitOk());

  // ¿NO hay elementos?
  if (0 == m_udNumItems) { 
	return 0; 
  }

  // Devuelve la info del primer elemento  
  return m_pHeap[1].udPriority;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el elemento que ocupa el tope del monticulo y borra el nodo en
//   donde se alojaba. El monticulo pasara a tener un elemento menos. Para
//   realizar esta operacion se llevara a cabo un filtrado descendente.
// Parametros:
// Modifica:
// - m_pHeap. Pasara a tener un elemento menos.
// - m_udNumItems. Se vera reducido el numero de elementos alojados
// - m_pIt. Si el iterador estaba vinculado lo desvincula.
// Devuelve:
// - El puntero al campo de informacion almacenado en el nodo eliminado. Si
//   por alguna razon no se ha podido obtener el elemento a eliminar o bien
//   no hay elementos disponibles, se devolvera NULL.
// Notas:
// - El filtrado descendente supone colocar en el primer nodo (el eliminado)
//   el ultimo elemento y bajarlo (filtrarlo descendentemente) hasta que halle
//   su sitio. De esta forma, borrar supone devolver el elemento que estaba
//   en el tope, colocar en esa posicion el ultimo y filtrarlo. Una vez filtrado
//   sabremos la nueva localizacion de ese ultimo elemento.
///////////////////////////////////////////////////////////////////////////////
template <class TypeInfo> TypeInfo* 
CPriorityQueue<TypeInfo>::RemoveTop(void)
{
  // SOLO si la instancia esta inicializada
  ASSERT(IsInitOk());

  // ¿Hay suficientes elementos?
  if (IsEmpty()) { 
	return NULL; 
  }

  // Se toma el valor a devolver
  ASSERT(m_pHeap);
  TypeInfo* const pInfo = m_pHeap[1].pInfo;
  ASSERT(pInfo);

  // Se toma el ultimo elemento y se reduce el temaño del monticulo
  dword udLastPriority = m_pHeap[m_udNumItems].udPriority;
  TypeInfo* const pLastInfo = m_pHeap[m_udNumItems].pInfo;    
  ASSERT(pLastInfo);
  m_udNumItems--;
  
  // Se realiza el filtrado descendente del ultimo elemento
  dword udPos = 1; // El ultimo elemento esta en la primera posicion
  DFilter(udLastPriority, udPos);

  // Se inserta el ultimo elemento en su nueva ubicacion
  // una vez que se ha filtrado descendentemente
  m_pHeap[udPos].udPriority = udLastPriority;
  m_pHeap[udPos].pInfo = pLastInfo;

  // Se desvincula iterador
  m_pIt = NULL;

  // Se devuelve la informacion que estaba en el tope
  return pInfo;
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destruye el monticulo. Las intancias que mantuviera se destruiran solo si
//   bRelease es true.
// Parametros:
// - bReleaseInfo. Indica si hay que eliminar solo nodos (false) o bien nodos
//   y punteros a informacion (true). Por defecto vale false.
// - m_pIt. Si el iterador estaba vinculado lo desvincula.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
template <class TypeInfo> 
void CPriorityQueue<TypeInfo>::End(const bool bReleaseInfo)
{  
  // Se evalua el tipo de borrado
  if (IsInitOk()) {
    // Se libera la informacion
    Release(bReleaseInfo);
    
    // Se borran todos los nodos del monticulo
    delete[m_udHeapSize] m_pHeap;
    m_pHeap = NULL;	
  }
} 

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Vacia el monticulo. En caso de pasara como parametro true,
//   tambien se realizara un borrado sobre los punteros de informacion
//   almacenados. Por defecto, bReleaseInfo vale false, con lo cual SOLO se
//   liberan nodos.
// Parametros:
// - bReleaseInfo. Indica si hay que eliminar solo nodos (false) o bien nodos
//   y punteros a informacion (true). Por defecto vale false.
// - udNumItems. Pasara a valer 0.
// - m_pIt. Si el iterador estaba vinculado lo desvincula.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
template <class TypeInfo> void 
CPriorityQueue<TypeInfo>::Release(const bool bReleaseInfo)
{  
  // Se evalua el tipo de borrado
  if (IsInitOk()) {
    if (bReleaseInfo) {
      // La liberacion NO incluye a los punteros de informacion
      for (dword i = 1; i < m_udNumItems; ++i) {
        ASSERT(m_pHeap[i].pInfo);
        delete m_pHeap[i].pInfo;
        m_pHeap[i].pInfo = NULL;
      }
    }     
    
    // No hay elementos
    m_udNumItems = 0;   
    
    // Se desvincula iterador
    m_pIt = NULL;
  }
} 

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza un filtro ascendente desde la posicion audPos, hasta la que tiene
//   una prioridad MENOR que udPriority (es decir, que su campo de prioridad
//   vale mas que udPriority)
// Parametros:
// - udPriority. Prioridad del elemento a filtrar.
// - udPos. Posicion desde donde comenzar a filtrar
// Devuelve:
// - audPos. Contendra la posicion del nodo en donde se debera de realizar la
//   insercion del elemento (tanto del contenido como de la prioridad). 
//   debida.
// Notas:
// - Recordar que la posicion del nodo padre de audPos es audPos / 2.
///////////////////////////////////////////////////////////////////////////////
template <class TypeInfo> void 
CPriorityQueue<TypeInfo>::AFilter(const dword audPriority, dword& audPos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si posicion correcta
  ASSERT(audPos);
  ASSERT((audPos <= GetNumItems()) != 0);
  
  // Vbles
  dword udPosParent; // Posicion del padre

  // Procede a realizar el filtro
  ASSERT(m_pHeap);
  udPosParent = audPos >> 1;
  while (m_pHeap[udPosParent].udPriority > audPriority) {
    m_pHeap[audPos].udPriority = m_pHeap[udPosParent].udPriority;
    m_pHeap[audPos].pInfo = m_pHeap[udPosParent].pInfo;
    audPos = udPosParent;
    udPosParent = audPos >> 1;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza una operacion de filtrado descendente. Esta operacion consiste en
//   bajar un elemento que se halla en la posicion audPos y tiene prioridad
//   audPriority, hasta su nueva localizacion. Una vez acabada la operacion
//   el valor en donde ubicar este elemento se hallara en audPos.
// Parametros:
// - audPriority. Prioridad del elemento a filtrar.
// - audPos. Posicion desde donde filtrar.
// Devuelve:
// Notas:
// - En la nueva posicion sera necesario introducir la prioridad y la 
//   informacion asociada al elemento filtrado.
// - Recordar que dada una posicion, el hijo derecho esta en posicion * 2 y
//   el izquierdo en posicion * 2 + 1.
///////////////////////////////////////////////////////////////////////////////
template <class TypeInfo> void 
CPriorityQueue<TypeInfo>::DFilter(const dword audPriority, dword& audPos)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si posicion correcta
  ASSERT(audPos);
  
  // Procede
  dword udPosSon = audPos << 1;
  bool bFilterEnd = false;
  while ( (udPosSon < m_udNumItems) && (!bFilterEnd)) {
    // Se busca el hijo menor    
    if (m_pHeap[udPosSon + 1].udPriority < m_pHeap[udPosSon].udPriority) {
      udPosSon++;
    }    

    // Una vez hallado el hijo menor se compara con el valor de prioridad
    // del elemento a filtrar. Si es menor, se sigue filtrando. En caso
    // contrario se ha localizado el sitio adecuado (estara en audPos)
    if (audPriority > m_pHeap[udPosSon].udPriority) {      
      m_pHeap[audPos].udPriority = m_pHeap[udPosSon].udPriority;
      m_pHeap[audPos].pInfo = m_pHeap[udPosSon].pInfo;
      audPos = udPosSon;
      udPosSon = audPos << 1;
    }
    else { 
      bFilterEnd = true; 
    }
  } // ~ while

  // Si se ha salido sin realizar un filtrado, se debe de hacer una
  // ultima comprobacion, pues estaremos en un caso en el que el elemento
  // hijo a comparar no tiene hijo izquierdo
  if ((udPosSon == m_udNumItems) && (!bFilterEnd)) {
    if (audPriority > m_pHeap[udPosSon].udPriority) {      
      m_pHeap[audPos].udPriority = m_pHeap[udPosSon].udPriority;
      m_pHeap[audPos].pInfo = m_pHeap[udPosSon].pInfo;
      audPos = udPosSon;     
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Busca el primer nodo con el puntero pInfo en su interior. Si lo halla
//   suma a su cantidad de prioridad el valor udDecValue y le aplica un filtro
//   descendente. En caso de que al sumar udDecValue nos salgamos del rango 
//   del dword, se realizara una acotacion.
// Parametros:
// - udDecValue. Valor a añadir a la prioridad
// - pInfo. Puntero con informacion a localizar.
// - m_pIt. Si estaba vinculado lo desvincula.
// Devuelve:
// - Si ha encontrado al nodo true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
template <class TypeInfo> bool 
CPriorityQueue<TypeInfo>::DecPriority(const dword udDecValue, 
                                      const TypeInfo* const pInfo)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Vbles
  dword    udPos; // Posicion donde esta el nodo
  HeapNode Node;  // Nodo a filtrar

  // Se procede a localizar el nodo
  for(udPos = 0; udPos <= m_udNumItems; ++udPos) {
    if (m_pHeap[udPos].pInfo == pInfo) {
      // Elemento localizado
      break;
    }
  }

  // ¿No se localizo el elemento?
  if (udPos > m_udNumItems) { return false; }

  // Se incrementa su valor de prioridad, que es lo mismo que
  // decir que se baja su prioridad
  if (m_pHeap[udPos].udPriority + udDecValue > 0XFFFFFFFF) {
    // Hay que acotar
    m_pHeap[udPos].udPriority = 0xFFFFFFFF;
  }
  else {
    m_pHeap[udPos].udPriority += udDecValue;
  }

  // Se realiza un filtro descendente
  Node.udPriority = m_pHeap[udPos].udPriority;
  Node.pInfo = m_pHeap[udPos].pInfo;
  DFilter(Node.udPriority, udPos);

  // Se inserta en la nueva posicion hallada en el filtrado descendente
  ASSERT((udPos <= m_udNumItems) != 0);
  m_pHeap[udPos].udPriority = Node.udPriority;
  m_pHeap[udPos].pInfo = Node.pInfo;

  // Se desvincula iterador
  m_pIt = NULL;

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Busca el primer nodo que tenga como informacion el puntero pInfo y realiza
//   un incremento de su prioridad, el valor udIncValue al valor de prioridad que
//   tenia. En caso de que haya un underflow se acotara a 0. Una vez que se
//   haya restado el valor udIncValue, se aplicara un filtrado ascendente.
// Parametros:
// - udIncValue. Valor con el que se incrementara la prioridad.
// - pInfo. Puntero al elemento de informacion almacenado.
// - m_pIt. Si estaba vinculado lo desvincula.
// Devuelve:
// - Si se encuentra el nodo true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
template <class TypeInfo> bool 
CPriorityQueue<TypeInfo>::IncPriority(const dword udIncValue, 
                                      const TypeInfo* const pInfo)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Vbles
  dword    udPos; // Posicion donde esta el nodo
  HeapNode Node;  // Nodo a filtrar

  // Se procede a localizar el nodo
  for(udPos = 0; udPos <= m_udNumItems; ++udPos) {
    if (m_pHeap[udPos].pInfo == pInfo) {
      // Elemento localizado
      break;
    }
  }

  // ¿No se localizo el elemento?
  if (udPos > m_udNumItems) { 
	return false; 
  }

  // Se incrementa su valor de prioridad, que es lo mismo que
  // decir que se baja su prioridad
  if (m_pHeap[udPos].udPriority < udIncValue) {
    // Hay que acotar
    m_pHeap[udPos].udPriority = 0;    
  }
  else {
    m_pHeap[udPos].udPriority -= udIncValue;    
  }

  // Se realiza un filtro ascendente
  Node.udPriority = m_pHeap[udPos].udPriority;
  Node.pInfo = m_pHeap[udPos].pInfo;
  AFilter(Node.udPriority, udPos);

  // Se inserta en la nueva posicion hallada en el filtrado ascendente
  ASSERT((udPos > 0) != 0);
  m_pHeap[udPos].udPriority = Node.udPriority;
  m_pHeap[udPos].pInfo = Node.pInfo;

  // Se desvincula iterador
  m_pIt = NULL;

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Busca el primer elemento que tenga como elemento de informacion la
//   direccion pInfo y lo elimina del monticulo. Para eliminarlo, le establece 
//   una prioridad de 0 y le aplica filtrado descendente. Si al finalizar el
//   filtrado la posicion obtenida no es la 1, intercambia la posicion devuelta
//   con la primera y realiza una operacion RemoveTop.
// Parametros:
// - pInfo. Puntero al elemento de informacion almacenado en el nodo a eliminar.
// Devuelve:
// Notas:
// - La operacion efectiva de eliminado y reajuste de tamaño se realizara en
//   el metodo RemoveTop.
///////////////////////////////////////////////////////////////////////////////
template <class TypeInfo> bool 
CPriorityQueue<TypeInfo>::RemoveNode(const TypeInfo* const pInfo)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se procede a localizar el nodo
  dword udPos = 0;
  for(; udPos < m_udNumItems; ++udPos) {
    if (m_pHeap[udPos].pInfo == pInfo) {
      // Elemento localizado
      break;
    }
  }

  // ¿No se localizo el elemento?
  if (udPos > m_udNumItems) { 
	return false; 
  }

  // Se establece la prioridad a 0 y se realiza un filtro ascendente
  HeapNode Node;
  m_pHeap[udPos].udPriority = 0;
  Node.udPriority = m_pHeap[udPos].udPriority;
  Node.pInfo = m_pHeap[udPos].pInfo;
  AFilter(Node.udPriority, udPos);

  // Se comprueba si la posicion es la primera. De no ser asi, se intercambia
  ASSERT(udPos);
  if (1 != udPos) {
    m_pHeap[udPos].udPriority = m_pHeap[1].udPriority;
    m_pHeap[udPos].pInfo = m_pHeap[1].pInfo;
    udPos = 1;
  }
  m_pHeap[udPos].udPriority = Node.udPriority;
  m_pHeap[udPos].pInfo = Node.pInfo;

  // Se realiza un RemoveTop, teniendo la seguidad de que el nodo con el
  // elemento a borrar se halla en el tope del monticulo
  RemoveTop();  

  // Se desvincula iterador
  m_pIt = NULL;

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Vincula el iterador si este no se encontraba vinculado o bien lo 
//   asocia a la primera posicion del Heap si ya lo estaba. Todo esto siempre
//   y cuando haya elementos en el Heap.
// Parametros:
// Devuelve:
// - Si se logra vincular true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
template <class TypeInfo> bool 
CPriorityQueue<TypeInfo>::InitIt(void)
{
  // SOLO si la instancia esta inicialiada
  ASSERT(IsInitOk());

  // ¿No hay elementos?
  if (!m_udNumItems) { 
	return false; 
  }
  
  // Se vincula
  m_pIt = &m_pHeap[1];
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Avanza el iterador por el Heap siempre y cuando se encuentre vinculado.
//   En caso de que se salga del Heap, quedara desvinculado. Se recomienda
//   realizar una llamada a IsItVinculated despues de utilizar este metodo.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
template <class TypeInfo> void 
CPriorityQueue<TypeInfo>::NextIt(void)
{
  // SOLO si esta inicializa la instancia
  ASSERT(IsInitOk());

  // Si el iterador esta vinculado se avanza
  if (m_pIt) {
    if (m_pIt == &m_pHeap[m_udNumItems]) { 
      // El iterador estaba apuntando a la ultima posicion -> se desvincula
      m_pIt = NULL;
    }
    else { 
      // Se avanza a la sig. posicion
      ++m_pIt;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retrocede el iterador por el Heap siempre y cuando se encuentre vinculado.
//   En caso de que se salga del Heap, quedara desvinculado. Se recomienda
//   realizar una llamada a IsItVinculated despues de utilizar este metodo.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
template <class TypeInfo> void 
CPriorityQueue<TypeInfo>::PrevIt(void)
{
  // SOLO si esta inicializa la instancia
  ASSERT(IsInitOk());

  // Si el iterador esta vinculado se avanza
  if (m_pIt) {
    if (m_pIt == &m_pHeap[1]) { 
      // El iterador estaba apuntando a la primera posicion -> se desvincula
      m_pIt = NULL;
    }
    else { 
      // Se retrocede a la posicion anterior
      --m_pIt;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Toma la prioridad del nodo al que esta vinculado el iterador. Si el
//   iterador no esta vinculado al Heap, devolvera 0XFFFFFFFF.
// Parametros:
// Devuelve:
// - Prioridad del nodo al que esta vinculado el iterador
// Notas:
///////////////////////////////////////////////////////////////////////////////
template <class TypeInfo> dword 
CPriorityQueue<TypeInfo>::GetItPriority(void)
{
  // SOLO si la instancia esta incializada
  ASSERT(IsInitOk());

  // ¿No esta vinculado?
  if (!m_pIt) { 
	return 0xFFFFFFFF; 
  }

  // Se devuelve la prioridad
  return m_pIt->udPriority;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve el campo de informacion almacenado en el nodo al que el iterador
//   esta vinculado.
// Parametros:
// Devuelve:
// - El campo pInfo almacenado en el nodo al que apunta el iterador. En caso de
//   que el iterador no este vinculado se devolvera NULL.
// Notas:
///////////////////////////////////////////////////////////////////////////////
template <class TypeInfo> TypeInfo* const 
CPriorityQueue<TypeInfo>::GetItInfo(void)
{
  // SOLO si la instancia esta inicializada
  ASSERT(IsInitOk());

  // ¿No esta vinculado?
  if (!m_pIt) { 
	return NULL; 
  }

  // Se retorna el campo de informacion
  return m_pIt->pInfo;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Sustituye el valor asociado al elemento en el que se halla el iterador,
//   perdiendo el valor anterior (esto quedara en responsabilidad del exterior).
// Parametros:
// - pNewInfo. Valor por el que sustituir el contenido de informacion actual.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
template <class TypeInfo> void 
CPriorityQueue<TypeInfo>::ChangeItInfo(TypeInfo* pNewInfo)
{
  // SOLO si la instancia esta inicializada
  ASSERT(IsInitOk());
  // SOLO si iterador vinculado
  ASSERT(m_pIt);

  // Se establece el nuevo valor
  m_pIt->pInfo = pNewInfo;
}
 
#endif // ~ CPriorityQueue.cpp