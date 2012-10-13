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
// CGUICAnimBitmap.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGUICAnimBitmap
//
// Descripcion:
// - Hereda de CGUICBitmap e incorpora la posibilidad de que el bitmap pueda
//   tener animaciones en determinadas posiciones
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGUICANIMBITMAP_H_
#define _CGUICANIMBITMAP_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CGUICBITMAP_H_
#include "CGUICBitmap.h"
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CGUICAnimBitmap
class CGUICAnimBitmap: public CGUICBitmap
{
private:
  // Tipos
  typedef CGUICBitmap Inherited; // Acceso a la clase base

public:
  // Enumerados de acceso general
  enum {
	MAX_ANIMS = 0XFF // Maximo numero de animaciones
  };

private:
  // Estructuras forward
  struct sNAnim;

private:
  // Tipos
  // Lista con las instancias de los Anims asociados
  typedef std::list<sNAnim*> AnimList;
  typedef AnimList::iterator AnimListIt;

private:
  // Vbles de miembro
  AnimList m_Anims; // Lista de Anims asociados a la imagen de fondo

public:
  // Constructor / destructor
  CGUICAnimBitmap(void) { }
  ~CGUICAnimBitmap(void) { End(); }

public:
  // Protocolos de inicializacion / find
  void End(void);

public:
  // CGUICBitmap / Dibujado
  void Draw(void);

public:
  // CGUIComponent / Establecimiento de componente activo / inactivo
  void SetActive(const bool bActive); 

public:
  // Operaciones sobre Anims
  bool AddAnim(const std::string& szAnimTemplate,
			   const sPosition& Position,
			   const GraphDefs::sAlphaValue& Alpha);
  void RemoveAllAnims(void);

}; // ~ CGUICAnimBitmap

#endif _CGUICAnimBitmap_H_
