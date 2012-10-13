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
// CSceneObj.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CSceneObj.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CSceneObj.h"

#include "SYSEngine.h"
#include "iCFileSystem.h"
#include "iCWorld.h"
#include "iCVirtualMachine.h"

// Inicializacion de los manejadores de memoria
CMemoryPool 
CSceneObj::sContainer::m_MPool(1, sizeof(CSceneObj::sContainer), true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia cargando datos desde disco.
// Parametros:
// - hFile. Handle al fichero.
// - udOffset. Offset donde localizar los datos.
// - hSceneObj. Handle al objeto de escenario
// Devuelve:
// - Si todo ha ido correctamente true. En caso contrario false.
// Notas:
// - Los objetos de escenario no dejaran acceso libre.
///////////////////////////////////////////////////////////////////////////////
bool 
CSceneObj::Init(const FileDefs::FileHandle& hFile,
				dword& udOffset,
				const AreaDefs::EntHandle& hSceneObj)

{
  // SOLO si parametros correctos
  ASSERT(hFile);
  ASSERT(hSceneObj);

  // Se inicializa la clase base
  if (Inherited::Init(hFile, udOffset, hSceneObj)) {
	// Se cargan valores
    // Tipo
	iCFileSystem* pFileSys = SYSEngine::GetFileSystem();
	ASSERT(pFileSys);
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(&m_Attributes.Type),
							   sizeof(RulesDefs::BaseType),
							   udOffset);
	
	// Clase
	udOffset +=pFileSys->Read(hFile, 
							  (sbyte *)(&m_Attributes.Class),
							  sizeof(RulesDefs::BaseType),
							  udOffset);
	
	// Flag de objet contenedor
	bool bFlag;
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(&bFlag),
							   sizeof(bool),
							   udOffset);
	// ¿Es entidad contenedora?
	if (bFlag) {
	  // Si, se crea contenedor e inicializa
	  m_pContainer = new CSceneObj::sContainer;
	  ASSERT(m_pContainer);	
	  m_pContainer->Content.Init(hSceneObj);
	  ASSERT(m_pContainer->Content.IsInitOk());
	} else {
	  // No
	  m_pContainer = NULL;
	}
	
	// Se obtienen todos los valores de los atributos
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(m_Attributes.LocalAttribs), 
							   sizeof(RulesDefs::WallLocalAttrib) * RulesDefs::MAX_SCENEOBJ_LOCALATTRIB, 
							   udOffset);	  
	
	// Flag de interaccion
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(&m_bIsInteractuable), 
							   sizeof(bool), 
							   udOffset);

	// Flag de acceso a floor
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(&m_bAccesibleFloor), 
							   sizeof(bool), 
							   udOffset);

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
CSceneObj::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {	
	// Libera container si procede
	if (m_pContainer) {
	  delete m_pContainer;
	  m_pContainer = NULL;
	}

	// Finaliza posibles scripts asociados
	SYSEngine::GetVirtualMachine()->ReleaseScripts(this);

	// Propaga	  
	Inherited::End();	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Guarda los datos propios de CSceneObj
// Parametros:
// - hFile. Handle al fichero donde almacenar.
// - udOffset. Offset donde depositar la info.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CSceneObj::Save(const FileDefs::FileHandle& hFile, 
				dword& udOffset)
{
  // SOLO si parametros correctos
  ASSERT(hFile);

  // Propaga
  Inherited::Save(hFile, udOffset);

  // Almacena tipo
  iCFileSystem* pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys);
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&m_Attributes.Type), 
							  sizeof(RulesDefs::BaseType));

  // Almacena clase
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&m_Attributes.Class), 
							  sizeof(RulesDefs::BaseType));  

  // Flag contenedor
  const bool bFlag = m_pContainer ? true : false;
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&bFlag), 
							  sizeof(bool));  

  // Attributos locales
  udOffset += pFileSys->Write(hFile, 
	                          udOffset,
							  (sbyte *)(&m_Attributes.LocalAttribs), 
							  sizeof(RulesDefs::SceneObjLocalAttrib) * RulesDefs::MAX_SCENEOBJ_LOCALATTRIB);	 

  // Flag de interaccion
  udOffset += pFileSys->Write(hFile, 
	                          udOffset,
							  (sbyte *)(&m_bIsInteractuable), 
							  sizeof(bool));	   

  // Flag de acceso a floor
  udOffset += pFileSys->Write(hFile, 
	                          udOffset,
							  (sbyte *)(&m_bAccesibleFloor), 
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

  // Items
  // ¿Es contenedor?
  if (m_pContainer) {
	// Si, se guardan items
	m_pContainer->Content.Save(hFile, udOffset);
  } else {
	// No, luego hay 0 items
	const word uwNumItems = 0;
	udOffset += pFileSys->Write(hFile, 
							    udOffset, 
							    (sbyte *)(&uwNumItems), 
							    sizeof(word));
  }    
}
