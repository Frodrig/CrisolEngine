///////////////////////////////////////////////////////////////////////////////
// CPriorityQueue.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CPriorityQueue
//
// Descripcion:
// - Clase para el manejo de un heap o monticulo (cola de prioridad).
// - Algunas consideraciones: 
//   * Se utilizara la posicion 0 como centinela. El centinela debera de
//     tener una prioridad menor o igual que la mas alta (suponiendo que 1 es
//     mayor prioridad que 2). En este caso, se eligira 0.
//   * Para una pos. i, el hijo izq. en 2i y el dcho en 2i+1. El padre i/2.
// - Para las prioridades se usaran dword.
// - MUY IMPORTANTE: La prioridad mas alta es 0 y la mas baja 0xFFFFFFFF
//
// Notas:
// - IMPORTANTE: Para una correcta compilacion, sera necesario incluir 
//   el archivo "CPriorityQueue.cpp" en lugar del "CPriorityQueue.h". 
//   Esto es, realizar un #include "CPriorityQueue.cpp". 
// - ADVERTENCIA: La operacion Release solo libera la memoria ocupada
//   por la informacion si se indica mediante parametro. Por defecto no
//   lo hace. Si a la hora de destruir un objeto de tipo CPriorityQueue no 
//   disponemos de los punteros a la informacion almacenada, deberemos de
//   llamar a Release(true) pues de lo contrario, el destructor no liberara
//   la memoria ocupada por estos, tan solo lo hara con los nodos.
// - Los nodos del arbol contendran una par de elementos que el usuario
//   debera de facilitar a la hora de insertar datos: la llave y el puntero
//   a la informacion a almacenar. No se permitiran claves repetidas.
// - Por motivos de legibilidad, se usara el termino Heap (monticulo) en lugar 
//   de PriorityQueue, a la hora de trabajar en el codigo.
// - No se permitira reinicializar. Antes, sera necesario llamar a Release.
// - El iterador quedara desvinculado automaticamente si se realiza alguna
//   operacion que pudiera afectar a la estructura del Heap y este se 
//   encontraba vinculado.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CPRIORITYQUEUE_H_
#define _CPRIORITYQUEUE_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif

// Clase CPriorityQueue
template <class TypeInfo> 
class CPriorityQueue
{   
private:
  // Estructuras
  struct HeapNode { 
	// Nodo de la cola
    dword     udPriority; // Prioridad del nodo
    TypeInfo* pInfo;      // Puntero a elemento almacenado    
  };

private:
  // Variables de miembro
  HeapNode* m_pHeap;      // Monticulo
  dword     m_udNumItems; // Numero de elementos (nodos)
  dword     m_udHeapSize; // Tamaño del monticulo  
  HeapNode* m_pIt;        // Iterador  

public:
  // Constructor / Destructor
  CPriorityQueue(void): m_pHeap(NULL), 
                        m_pIt(NULL),
                        m_udNumItems(0),
                        m_udHeapSize(0) { }
  ~CPriorityQueue(void) { End(false); }

public:
  // Protocolo de inicio / fin de instancia
  bool Init(const dword udHeapSize);
  void End(const bool bReleaseInfo = false);

public:
  // Servicios basicos  
  bool Insert(const dword udPriority, TypeInfo* const pInfo);
  TypeInfo* const GetTopInfo(void);
  dword GetTopPriority(void);
  TypeInfo* RemoveTop(void);  
  void Release(const bool bReleaseInfo = false);

public:
  // Operaciones extendidas  
  bool DecPriority(const dword udDecValue, const TypeInfo* const pInfo);
  bool IncPriority(const dword udIncValue, const TypeInfo* const pInfo);
  bool RemoveNode(const TypeInfo* const pInfo);  

public:
  // Operaciones con el iterador  
  bool InitIt(void);
  void NextIt(void);
  void PrevIt(void);
  dword GetItPriority(void);
  TypeInfo* const GetItInfo(void);
  void ChangeItInfo(TypeInfo* pNewInfo);
  inline bool IsItVinculated(void) const { return (NULL != m_pIt); }

public:  
  // Operaciones de consulta
  inline dword GetNumItems(void) const { return m_udNumItems; }
  inline dword GetHeapSize(void) const { return (m_udHeapSize - 1); }  
  inline bool IsFull(void) const { return (GetHeapSize() == GetNumItems()); }
  inline bool IsEmpty(void) const { return (0 == m_udNumItems); }
  inline IsInitOk(void) const { return !(NULL == m_pHeap); }

private:
  // Metodos de apoyo / internos  
  void AFilter(const dword udPriority, dword& audPos);
  void DFilter(const dword udPriority, dword& audPos);
};
#endif // ~ CPriorityQueue
