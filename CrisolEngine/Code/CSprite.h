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
// CSprite.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases
// - CSprite
//
// Descripcion:
// - Define la clase basica para representar imagenes con movimiento en pantalla.
//   Es una clase derivada de CDrawable.
//
// Notas:
// - La velocidad se tratara como un vector.
// - La inicializacion podra realizarse de 3 formas distintas:
//   * Pasando el nombre de la plantilla de animacion.
//   * Pasando una instancia CSprite para inicializar a partir de esta.
//   * Pasando un Loader que se halle dentro de un perfil. De ese perfil se
//     tomara el nombre de la plantilla de animacion, llamandose al primer
//     Init. Esta ultima opcion se hace muy interesante para trabajar con las
//     entidades del universo de juego en su carga de datos desde disco.
// - El cargador ha de definirse publico para poder heredarse.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CSPRITE_H_
#define _CSPRITE_H_

// Cabeceras
#ifndef _SYSENGINE_H_
#include "SYSEngine.h"
#endif
#ifndef _CDRAWABLE_H_
#include "CDrawable.h"
#endif
#ifndef _CANIMCMD_H_
#include "CAnimCmd.h"
#endif
#ifndef _GAMEDATABASEDEFS_H_
#include "GameDataBaseDefs.h"
#endif
#ifndef _CANIMTEMPLATE_H_
#include "CAnimTemplate.h"
#endif
#ifndef _ICMATHUTIL_H
#include "iCMathUtil.h"
#endif
#ifndef _FILEDEFS_H_
#include "FileDefs.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif

// Clase CSprite
class CSprite: public CDrawable
{
private:
  // Clase base
  typedef CDrawable Inherited;

private:
  // Estructuras  
  struct MoveInfo { 
	// Info referida al movimiento del sprite    
	word  uwSpeed; // Velocidad en pixels por segundo    
	float fXPos;   // Posicion en X del elemento a dibujar
	float fYPos;   // Posicion en Y del elemento a dibujar
  };

private:
  // Vbles de miembro
  bool			m_bIsInitOk;	// ¿Instancia incializada correctamente?  
  CAnimTemplate m_AnimTemplate; // Plantilla de animacion	
  MoveInfo		m_MoveInfo;		// Informacion referida al movimiento
  bool          m_bInPause;     // ¿Sprite en pausa?

public:
  // Constructor / destructor
  CSprite(void): m_bIsInitOk(false),
				 m_bInPause(false) { }
  ~CSprite(void) { End(); }

public:	
  // Protocolos de inicilizacion / finalizacion de instancia.  
  bool Init(const std::string& szATemplate,
			const AnimTemplateDefs::AnimState& InitState = 0,
			const AnimTemplateDefs::AnimOrientation& InitOrientation = 0,
			const bool bActiveAnimCmd = true);
  bool Init(const FileDefs::FileHandle& hFile,
			dword& udOffset,
			const bool bActiveAnimCmd = true);
  
public:
  void End(void);
  bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // Operacion de almacenamiento
  virtual void Save(const FileDefs::FileHandle& hFile, 
					dword& udOffset);

public:
  // CDrawable / Obtencion del handle a la textura actual
  ResDefs::TextureHandle GetIDTexture(void) {
	ASSERT(IsInitOk());
	// Se obtiene el frame actual
	return m_AnimTemplate.GetIDTextureFrame();
  }

public:
  // CDrawable / Obtencion de las dimensiones
  word GetWidth(void) { 
	ASSERT(IsInitOk());
	// Se devuelve anchura
	return m_AnimTemplate.GetFramesWidth();
  }
  word GetHeight(void) { 
	ASSERT(IsInitOk());
	// Se devuelve altura
	return m_AnimTemplate.GetFramesHeight();
  }

public:
  // Obtencion de la plantilla de animacionç
  iCAnimTemplate* const GetAnimTemplate(void) {
	ASSERT(IsInitOk());
	// Retorna el interfaz a plantilla de animacion
	return &m_AnimTemplate;
  }

public:
  // Operaciones sobre desplazamiento / velocidad
  inline void SetSpeed(const word uwSpeed) { 
	ASSERT(IsInitOk());
	// Establece velocidad
	m_MoveInfo.uwSpeed = uwSpeed; 
  }  
  inline word GetGlobalSpeed(void) const { 
	ASSERT(IsInitOk());
	// Devuelve velocidad
	return m_MoveInfo.uwSpeed; 
  }
  inline float GetXSpeed(void) {
	ASSERT(IsInitOk());
	// Retorna la velocidad en X teniendo en cuenta el angulo
	return m_MoveInfo.uwSpeed * SYSEngine::GetMathUtil()->Cos(GetActAngle());
  }
  inline float GetYSpeed(void) {
	ASSERT(IsInitOk());
	// Retorna la velocidad en Y teniendo en cuenta el angulo
	return m_MoveInfo.uwSpeed * SYSEngine::GetMathUtil()->Sin(GetActAngle());
  }

public:
  // Operaciones sobre desplazamiento / posicion  
  inline void SetXPos(const float fXPos) { 
	ASSERT(IsInitOk());
	// Establece la posicion en X
	m_MoveInfo.fXPos = fXPos; 
  }
  inline void SetYPos(const float fYPos) { 
	ASSERT(IsInitOk());
	// Establece la posicion en Y
	m_MoveInfo.fYPos = fYPos; 
  }
  virtual float GetXPos(void) const { 
	ASSERT(IsInitOk());
	// Devuelve la posicion en X
	return m_MoveInfo.fXPos; 
  }
  virtual float GetYPos(void) const { 
	ASSERT(IsInitOk());
	// Devuelve la posicion en Y
	return m_MoveInfo.fYPos; 
  }

private:
  // Operaciones sobre desplazamiento / obtencion del angulo
  float GetActAngle(void);

public:
  // CDrawable / Obtencion de parametros de dibujado
  bool IsTextureAlphaBlending(void) const {
	ASSERT(IsInitOk());
	// Retorna flag
	return true;
  }
  bool IsVertexAlphaBlending(void) const  {
	ASSERT(IsInitOk());
	// Retorna flag
	return true;
  }
  bool IsBilinearFiltering(void) const  {
	ASSERT(IsInitOk());
	// Retorna flag
	return false;
  }
  bool IsColorKey(void) const  {
	ASSERT(IsInitOk());
	// Retorna flag
	return false;
  }
};
  

#endif // ~ #ifdef _CSPRITE_H_
