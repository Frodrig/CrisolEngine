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
// CSprite.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CSprite.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CSprite.h"

#include "iCGameDataBase.h"
#include "iCFileSystem.h"
#include "IsoDefs.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Incializara la clase obteniendo el nombre de la plantilla de animacion 
//   desde disco.
// Parametros:
// - File. Fichero con los datos.
// - udOffset. Offset desde donde comenzar a leer
// - bActiveAnimCmd. Flag para activar el comando de animacion. Por defecto
//   valdra true.
// Devuelve:
// - Siempre devolvera true, pues en caso de fallo se producira un FatalError.
// Notas:
// - El metodo se declarara como protegido.
///////////////////////////////////////////////////////////////////////////////
bool 
CSprite::Init(const FileDefs::FileHandle& hFile,
			  dword& udOffset,
			  const bool bActiveAnimCmd)
{
  // SOLO si parametros validos
  ASSERT(hFile);

  // ¿Se intenta reicinializar?
  if (IsInitOk()) { 
	return false; 
  }

  // Obtiene el nombre de la plantilla de animacion
  iCFileSystem* pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys);
  std::string szAnimProfile;
  udOffset += pFileSys->ReadStringFromBinary(hFile, udOffset, szAnimProfile);

  // Obtiene el estado actual de animacion
  AnimTemplateDefs::AnimState AnimState;
  udOffset += pFileSys->Read(hFile, 
							 (sbyte *)(&AnimState),
							 sizeof(AnimTemplateDefs::AnimState),
							 udOffset);

  // Obtiene orientacion actual
  AnimTemplateDefs::AnimOrientation AnimOrientation;
  udOffset += pFileSys->Read(hFile, 
							 (sbyte *)(&AnimOrientation),
							 sizeof(AnimTemplateDefs::AnimOrientation),
							 udOffset);

  // Inicializa
  if (!Init(szAnimProfile, AnimState, AnimOrientation, bActiveAnimCmd)) {
	SYSEngine::FatalError("Cargando la plantilla de animacion en CSprite con id. %s.\n", szAnimProfile.c_str());
  } 

  // Se retorna
  return m_bIsInitOk;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Operaciones principales de inicializacion y metodo de apoyo para los
//   Init que reciben la informacion de la plantilla de animacin.
// Parametros:
// - szAnimTemplate. Nombre de la plantilla de animacion a utilizar.
// - InitState. Estado inicial.
// - InitOrientation. Orientacion inicial.
// - bActiveAnimCmd. Flag para activar el comando de animacion. Por defecto
//   valdra true.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - MUY IMPORTANTE: El ID de la textura asociada al frame actual no se 
//   puede establecer ya que se esta en la posicion -1. Sera necesario hacer 
//   un NextFrame.  
// - Si la clase ya esta inicializada se devolvera false.
// - La velocidad sera tratada como un vector donde la velocidad sera la
//   longitud y la orientacion el angulo.
///////////////////////////////////////////////////////////////////////////////
bool 
CSprite::Init(const std::string& szAnimTemplate,
			  const AnimTemplateDefs::AnimState& InitState,
			  const AnimTemplateDefs::AnimOrientation& InitOrientation,
			  const bool bActiveAnimCmd)
{  
  // ¿Se intenta reicinializar?
  if (IsInitOk()) { 
	return false; 
  }
  
  // Se inicializa la plantilla de animacion
  if (m_AnimTemplate.Init(szAnimTemplate, InitState, InitOrientation, bActiveAnimCmd)) {
	// Se establece en posicion 0,0
	m_MoveInfo.fXPos = 0.0f;
	m_MoveInfo.fYPos = 0.0f;

	// Establece los valores relacionados con la velocidad  
	m_MoveInfo.uwSpeed = 0.0f;
  
	// Establece formato de dibujo por defecto
	Inherited::SetVertexAlphaBlending(true);
	Inherited::SetTextureAlphaBlending(true);	  
  
	// Todo correcto
	m_bIsInitOk = true;
	return true;   
  }
  
  // Hubo problemas
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Guarda los datos asociados al sprite:
//    * Nombre plantilla de animacion
//    * Estado actual de animacion
// Parametros:
// - hFile. Handle al fichero donde almacenar.
// - udOffset. Offset donde depositar la info.
// Devuelve:
// Notas:
// - Se actualizara la pos. del offset.
///////////////////////////////////////////////////////////////////////////////
void 
CSprite::Save(const FileDefs::FileHandle& hFile, 
			  dword& udOffset)
{
  // SOLO si parametros correctos
  ASSERT(hFile);

  // Almacena nombre de la plantilla de animacion
  iCFileSystem* const pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys);
  udOffset += pFileSys->WriteStringInBinary(hFile, 
											udOffset, 
											m_AnimTemplate.GetAnimTemplateName());  

  // Almacena estado actual de animacion
  const AnimTemplateDefs::AnimState AnimState = m_AnimTemplate.GetAnimState();
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&AnimState),
							  sizeof(AnimTemplateDefs::AnimState));  

  // Almacena orientacion actual
  const AnimTemplateDefs::AnimOrientation AnimOrientation = m_AnimTemplate.GetOrientation();
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&AnimOrientation),
							  sizeof(AnimTemplateDefs::AnimOrientation));  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CSprite::End(void)
{
  if (IsInitOk()) {	
	// Finaliza animacion
	m_AnimTemplate.End();

	// Baja flag
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Calcula el angulo del vector desplazamiento en funcion de la orientacion
//   actual del sprite.
// Parametros:
// Devuelve:
// Notas:
// - Este metodo contiene codigo completamente DESECHABLE y MEJORABLE.
///////////////////////////////////////////////////////////////////////////////
float 
CSprite::GetActAngle(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Vbles
  float fResult; // Vble con el resultado
  
  // Se obtiene angulo de orientacion  
  // Nota: El motor trabaja con la velocidad en terminos absolutos, para evitar
  // tener que usar la funcion abs, se ponen angulos simetricos para las 
  // orientaciones.
  switch(IsoDefs::eDirectionIndex(m_AnimTemplate.GetOrientation())) {
	case IsoDefs::NORTH_INDEX:
	case IsoDefs::SOUTH_INDEX: {
		fResult = 270; 
	} break;

	case IsoDefs::EAST_INDEX:
	case IsoDefs::WEST_INDEX: {
	  fResult = 0;
	} break;

	case IsoDefs::NORTHEAST_INDEX:
	case IsoDefs::NORTHWEST_INDEX:
	case IsoDefs::SOUTHEAST_INDEX:  
	case IsoDefs::SOUTHWEST_INDEX: {
	  // El valor 25.56.. es el angulo que toma el vector cuando nos movemos
	  // en diagonal	  
	  fResult = 360.0f - 26.56505117f;	  
	} break;

	default:
	  ASSERT(false);
  };

  // Retorna el resultado
  return fResult;
}
