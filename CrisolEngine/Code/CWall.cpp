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
// CWall.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CWall.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CWall.h"

#include "SYSEngine.h"
#include "iCFileSystem.h"
#include "iCWorld.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia cargando datos desde disco.
// Parametros:
// - hFile. Handle al fichero.
// - udOffset. Offset donde comienzan a estar los datos
// - hWall. Handle para la instancia
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - Los accesorios de pared no se copiaran.
///////////////////////////////////////////////////////////////////////////////
bool 
CWall::Init(const FileDefs::FileHandle& hFile,
			dword& udOffset,
			const AreaDefs::EntHandle hEntity)
{
  // SOLO si parametros validos
  ASSERT(hFile);
  ASSERT(hEntity);

  // Se inicializa clase base
  if (Inherited::Init(hFile, udOffset, hEntity)) {
	// Se carga informacion asociada
	iCFileSystem* pFileSys = SYSEngine::GetFileSystem();
	ASSERT(pFileSys);

	// Tipo
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(&m_Attributes.Type), 
							   sizeof(RulesDefs::BaseType), 
							   udOffset);

	// Atributos locales
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(m_Attributes.LocalAttribs), 
							   sizeof(RulesDefs::WallLocalAttrib) * RulesDefs::MAX_WALL_LOCALATTRIB, 
							   udOffset);

	// Orientacion
	udOffset += pFileSys->Read(hFile,
							   (sbyte *)(&m_Attributes.Orientation),
							   sizeof(RulesDefs::eWallOrientation),
							   udOffset);

	// Flag de interaccion
	udOffset += pFileSys->Read(hFile, (sbyte *)(&m_bIsInteractuable), sizeof(bool), udOffset);

	// Por defecto estara bloqueado
	BlockAccess();

	// Notificacion de no vision en pantalla
	VisibleInScreen(false);
  } 

  // Se retorna resultado
  return Inherited::IsInitOk();
}  
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWall::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {
	// Propaga
	Inherited::End();	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Guarda datos propios de CWall
// Parametros:
// - hFile. Handle al fichero donde almacenar.
// - udOffset. Offset donde depositar la info.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWall::Save(const FileDefs::FileHandle& hFile, 
		    dword& udOffset)
{
  // SOLO si parametros correctos
  ASSERT(hFile);
  
  // Propaga
  Inherited::Save(hFile, udOffset);
  
  // Almacena tipo
  iCFileSystem* const pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys);
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&m_Attributes.Type), 
							  sizeof(RulesDefs::BaseType));
  
  // Atributos locales
  udOffset += pFileSys->Write(hFile, 
	                          udOffset,
							  (sbyte *)(m_Attributes.LocalAttribs),
							  sizeof(RulesDefs::WallLocalAttrib) * RulesDefs::MAX_WALL_LOCALATTRIB);	  	    
  
  // Orientacion
  udOffset += pFileSys->Write(hFile,
	                          udOffset,
							  (sbyte *)(&m_Attributes.Orientation),
							  sizeof(RulesDefs::eWallOrientation));
  
  // Flag de interaccion
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&m_bIsInteractuable), 
							  sizeof(bool));
  
  // Obtiene el posible tag asociado con esta entidad
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  udOffset += pFileSys->WriteStringInBinary(hFile, 
											udOffset, 
											pWorld->GetTagFromHandle(Inherited::GetHandle()));
  
  // Elevacion
  const RulesDefs::Elevation Elevation = Inherited::GetElevation();
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&Elevation), 
							  sizeof(RulesDefs::Elevation));
  
  // Luz
  const GraphDefs::Light Light = pWorld->GetLight(Inherited::GetHandle());
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&Light), 
							  sizeof(GraphDefs::Light));  

  // Flag de bloqueo
  const bool bFlag = IsBlocked();
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&bFlag), 
							  sizeof(bool));  
}


///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Bloquea segun la orientacion
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWall::BlockAccess(void)
{
  // Bloquea accesos  
  switch(m_Attributes.Orientation) {
	case RulesDefs::WO_SOUTHWEST: {
	  // Pared suroeste
	  // Se bloquean accesos al oeste, suroeste y sur
	  m_WallAccess = IsoDefs::WEST_FLAG |
					 IsoDefs::SOUTHWEST_FLAG |
		             IsoDefs::SOUTH_FLAG;
	} break;

	case RulesDefs::WO_FRONT: {
	  // Pared frontal
	  // Se bloquean todos los accessos
	  m_WallAccess = AreaDefs::NO_TILE_ACCESS;
	} break;

	case RulesDefs::WO_SOUTHEAST: {
	  // Pared sureste
	  // Se bloquean los accesos al sur, sureste y este	  
	  m_WallAccess = IsoDefs::SOUTH_FLAG |
		             IsoDefs::SOUTHEAST_FLAG |
					 IsoDefs::EAST_FLAG;			
	} break;
  };
}
