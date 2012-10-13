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
// CFontSystem.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CFontSystem
//
// Descripcion:
// - Subsistema encargado de manejar todas las fuentes instaladas y realizar
//   la coordinacion de escritura de texto en pantalla. 
// - Internamente instalara todas las fuentes en un map, de tal forma
//   que las fuentes se hallaran utilizando su nombre. 
//   Los textos se dibujaran como objetos CCharFont, clase que heredara de
//   iCDrawable.
//   Los caracteres siempre se dibujaran en el plano DZ_GUI y con cualquiera de
//   la profundidad admitida en dicho plano de dibujado. 
// - La ordenacion de dibujado se basara siempre en el tipo de caracter a 
//   dibujar en pantalla. Esto es asi, para racionar al maximo el cambio de
//   la region de dibujado.
// - Los nodos que mantengan a los objetos CCharFont seran mantenidos en un
//   CRecycleNodePool automatico. 
//
// Notas:
// - Clase singlenton.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CFONTSYSTEM_H_
#define _CFONTSYSTEM_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _ICFONTSYSTEM_H_
#include "iCFontSystem.h"
#endif
#ifndef _CFONT_H_
#include "CFont.h"
#endif
#ifndef _CMEMORYPOOL_H_
#include "CMemoryPool.h"
#endif
#ifndef _CRECYCLENODEPOOL_CPP
#include "CRecycleNodePool.cpp"
#endif
#ifndef _CCHARFONT_H_
#include "CCharFont.h"
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif

// Definicion de clases / estructuras / espacios de nombres
struct iCGraphicSystem;

// Clase CFontSystem
class CFontSystem: public iCFontSystem
{
private:
  // Estructuras (nodos)
  struct sNFont {
	// Nodo asociado a una fuente instalada
	CFont Font; // Fuente instalada
	// Pool de memoria
	static CMemoryPool m_MPool;
	static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
	static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
  };

private:
  // Tipos
  // Map para el mantenimiento de las fuentes por nombre
  typedef std::map<std::string, sNFont*> FontTableMap;
  typedef FontTableMap::iterator         FontTableMapIt;
  typedef FontTableMap::value_type       FontTableMapValType;

private:
  // Estructuras
  struct sNCharacter {
	// Nodo asociado a un caracter de dibujado
	CCharFont DrawableChar; // Caracter a dibujar
	// Pool de memoria
	static CMemoryPool m_MPool;
	static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
	static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
  };

  struct sNTextGraphInfo {
	// Nodo asociado a la informacion grafica del texto a dibujar
	GraphDefs::sRGBColor   RGBColor; // Color RGB
	GraphDefs::sAlphaValue Alpha;    // Valor Alpha
	// Pool de memoria
	static CMemoryPool m_MPool;
	static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
	static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
  };

  struct sRecyclePoolInfo {
	// Info asociada a los pools de memoria "reciclable"
	CRecycleNodePool<sNCharacter>	  CharacterPool; // Pool de caracteres
	CRecycleNodePool<sNTextGraphInfo> TextGraphPool; // Pool de info grafica de textos
  };  

  struct sFontSysInfo {
	// Info general asociada al subsistema de fuentes
	FontTableMap   FontTable;     // Tabla de fuentes instaladas	
	FontTableMapIt DefaultFontIt; // Iterador a la fuente por defecto
  };

private:
  // Instancia singlenton
  static CFontSystem* m_pFontSystem;

  // Insterfaces a subsistemas del motor
  iCGraphicSystem* m_pGraphSys; // Subsistema grafico

  // Resto vbles
  sFontSysInfo     m_FontSysInfo; // Info general asociada al sub. de fuentes
  sRecyclePoolInfo m_PoolInfo;    // Info del sistema de reciclado de mem.
  bool			   m_bIsInitOk;   // ¿Clase inicializada?

protected:
  // Constructor / destructor
  CFontSystem(void): m_pGraphSys(NULL),					 
					 m_bIsInitOk(false) { }
public:
  ~CFontSystem(void) { 
    End(); 
  }

public:
  // Obtencion y destruccion de la instancia singlenton
  static inline CFontSystem* const GetInstance(void) {
    if (NULL == m_pFontSystem) { 
      m_pFontSystem = new CFontSystem; 
      ASSERT(m_pFontSystem)
    }
    return m_pFontSystem;
  }
  static inline void DestroyInstance(void) {
    if (m_pFontSystem) {
      delete m_pFontSystem;
      m_pFontSystem = NULL;
    }
  }

public:
  // Protocolo de inicio y fin de instancia
  bool Init(const std::string& szDefaultFont);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }
private:
  // Metodos de apoyo
  void EndFonts(void);
  
public:
  // Trabajo con las fuentes
  bool AddFont(const std::string& szFontName);
  void RestoreFonts(void);
private:
  // Metodos de apoyo
  inline FontTableMapIt GetFontIt(const std::string& szFontName) {
	ASSERT(IsInitOk());
	// Se obtiene el iterador a la fuente pasada, en caso de que no este
	// registrada, se devolvera iterador a End.
	std::string szFontNameLowercase(szFontName);
	SYSEngine::MakeLowercase(szFontNameLowercase);
	return m_FontSysInfo.FontTable.find(szFontNameLowercase); 
  }

private:
  // iCFontSystem / Clases amigas para interfaz privado  
  void ResetPoolMemory(void) {
	ASSERT(IsInitOk());
	// Resetea los pools de memoria. 
	// Este metodo sera llamado por GraphicSystem cada vez que se 
	// haga un render a la escena.
	m_PoolInfo.CharacterPool.ResetPool();
	m_PoolInfo.TextGraphPool.ResetPool();
  }

public:
  // iCFontSystem / Escritura de texto
  void Write(const sword& swXPos,
			 const sword& swYPos,
			 const byte ubPlane,
			 const std::string& szText,
			 const GraphDefs::sRGBColor& RGBColor = GraphDefs::sRGBColor(255, 255, 255),
			 const GraphDefs::sAlphaValue& Alpha = GraphDefs::sAlphaValue(255),
			 const std::string& szFontName = "");
public:
  // iCFontSystem / Obtencion de instancias a fuentes
  iCFont* const GetFont(const std::string szFontName);

public:
  // Metodos de utilidad general
  void GetTextDim(const std::string& szText,
				  const std::string& szFontName,
				  word& uwWidth,
				  word& uwHeight);
};

#endif // ~ CFontSystem

