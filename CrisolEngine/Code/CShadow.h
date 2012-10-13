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
// CShadow.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CShadow
//
// Descripcion:
// - Representa una sombra asociada a una entidad. Las imagenes seran
//   asociadas en las clases CWorldEntity. No todas las entidades tendran 
//   sombra. La idea sera dibujar la sombra como un dibujo negro, en formato
//   de silueta, con un valor alpha asociado.
// - El sistema de sombreado trabajara intimamente relacionado con el 
//   GFXManager pues de este tomara la direccion a la estructura Alpha de
//   donde tomar la intensidad con la que dibujar la sombra.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CSHADOW_H_
#define _CSHADOW_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _ICGRAPHICSYSTEM_H_
#include "iCGraphicSystem.h"
#endif
#ifndef _SYSENGINE_H_
#include "SYSEngine.h"
#endif
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _CIMAGE_H_
#include "CImage.h"
#endif
#ifndef _STRING_H_
#include <string>
#define _STRING_H_
#endif
#ifndef _GRAPHDEFS_H_
#include "GraphDefs.h"
#endif
#ifndef _ISODEFS_H_
#include "IsoDefs.h"
#endif
#ifndef _ICRESOURCEMANAGER_H_
#include "iCResourceManager.h"
#endif

// Clase CShadow
class CShadow
{
private:
  // Resto de vbles
  CImage      m_Image;           // Imagen con la sombra
  std::string m_szImageFileName; // Nombre de la imagen
  bool	      m_bIsInitOk;       // ¿Instancia incializada correctamente?

public:
  // Constructor / destructor
  CShadow(void): m_bIsInitOk(false) { }
  ~CShadow(void) { End(); }

public:
  // Protocolos de inicilizacion / finalizacion de instancia.  
  bool Init(const std::string& szImageFileName);  
  void End(void);
  bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // Obtencion del nombre de la imagen
  std::string GetShadowFileName(void) const {
	ASSERT(IsInitOk());
	// Retorna el nombre de la imagen
	return m_szImageFileName;
  }

public:
  // Dibujado de la imagen
  inline void Draw(const sword& swXPos, 
				   const sword& swYPos, 
				   const IsoDefs::eDrawPlane& DrawPlane) {
	ASSERT(IsInitOk());
	// Realiza el dibujado
	SYSEngine::GetGraphicSystem()->Draw(GraphDefs::DZ_GAME_SCENE,
									    DrawPlane,
									    swXPos,
										swYPos + (IsoDefs::TILE_HEIGHT - m_Image.GetHeight()),
									    &m_Image);
  }
};

#endif // ~ #ifdef _CShadow_H_
