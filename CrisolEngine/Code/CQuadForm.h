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
// CQuadForm.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CQuadForm
//
// Descripcion:
// - Representa un objeto dibujable de tipo quad sin ningun tipo de textura
//   asociada. El objeto se dibujara siempre con color solido y usando
//   alpha en los vertices nada mas.
//
// Notas:
// - Cuando no exista ningun valor RGB asociado al quad, se dibujara este
//   con color solido blanco y sin efecto alpha.
// - Las estructuras RGB y Alpha se asociaran por direccion
// - La posicion en x,y se pasara a la hora de dibujar la estructura.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CQUADFORM_H_
#define _CQUADFORM_H_

// Cabeceras
#ifndef _CDRAWABLE_H_
#include "CDrawable.h" 
#endif

// Clase CQuadForm
class CQuadForm: public CDrawable
{
private:
  // Tipos
  typedef CDrawable Inherited; // Clase base

private:
  // Estructuras
  struct sQuadFormInfo {
	// Informacion asociada al quad
	word uwWidth;  // Anchura
	word uwHeight; // Altura  	
  };

private:
  // Vbles de miembro
  sQuadFormInfo m_QuadInfo;  // Info asociada al quad
  bool          m_bIsInitOk; // ¿Instancia inicializada correctamente?  
      
public:
  // Protocolos de inicilizacion / finalizacion de instancia.  
  CQuadForm(void): m_bIsInitOk(false)  { }
  ~CQuadForm(void) { End(); }

public:
  // Protocolos de inicializacion y fin
  bool Init(const word uwWidth, const word uwHeight);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // Establecimiento de valores basicos
  inline void ChangeWidth(const word uwWidth) { 
	ASSERT(IsInitOk());
	// Se cambia anchura
	m_QuadInfo.uwWidth = uwWidth; 
  }
  inline void ChangeHeight(const word uwHeight) { 
	ASSERT(IsInitOk());
	// Se cambia altura
	m_QuadInfo.uwHeight = uwHeight; 
  }

public:
  // CDrawable / Obtencion del handle a la textura
  ResDefs::TextureHandle GetIDTexture(void) { 
	ASSERT(IsInitOk());
	// No hay textura asociada
	return 0; 
  }  

public:
  // CDrawable / Obtencion de las dimensiones
  word GetWidth(void) { 
	ASSERT(IsInitOk());
	// Se devuelve anchura
	return m_QuadInfo.uwWidth; 
  }
  word GetHeight(void) { 
	ASSERT(IsInitOk());
	// Se devuelve altura
	return m_QuadInfo.uwHeight; 
  }
};

#endif // ~ #ifdef _CQUADFORM_H_