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
// CItem.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CItem.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CItem.h"

#include "SYSEngine.h"
#include "iCFileSystem.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia cargando datos desde disco.
// Parametros:
// - hFile. Handle al fichero con los datos.
// - udOffset. Offset donde comienzan a estar los datos
// - hItem. Handle de la entidad.
// Devuelve:
// - Si todo ha ido correctamente true. En caso contrario false.
// Notas:
// - Los items siempre dejaran paso libre cuando se hallen sobre una celda
///////////////////////////////////////////////////////////////////////////////
bool 
CItem::Init(const FileDefs::FileHandle& hFile,
			dword& udOffset,
			const AreaDefs::EntHandle& hItem)
{
  // SOLO si parametros correctos
  ASSERT(hFile);
  ASSERT(hItem);

  // Se inicializa clase base
  if (Inherited::Init(hFile, udOffset, hItem)) {
	// Se cargan valores
    // Tipo
	iCFileSystem* pFileSys = SYSEngine::GetFileSystem();
	ASSERT(pFileSys);
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(&m_Attributes.Type),
							   sizeof(RulesDefs::BaseType),
							   udOffset);	
	// Clase
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(&m_Attributes.Class),
							   sizeof(RulesDefs::BaseType),
							   udOffset);

	// Atributos globales y locales
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(m_Attributes.GlobalAttribs), 
							   sizeof(RulesDefs::ItemGlobalAttrib) * RulesDefs::MAX_ITEM_GLOBALATTRIB, 
							   udOffset);	  	
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(m_Attributes.LocalAttribs), 
							   sizeof(RulesDefs::ItemLocalAttrib) * RulesDefs::MAX_ITEM_LOCALATTRIB, 
							   udOffset);	  							   

	// Notificacion de no vision en pantalla
	VisibleInScreen(false);
  }

  // Se retorna resultado
  return Inherited::IsInitOk();
}  

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CItem::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {	
	// Propaga
	Inherited::End();	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Guarda datos propios de CItem
// Parametros:
// - hFile. Handle al fichero donde almacenar.
// - udOffset. Offset donde depositar la info.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CItem::Save(const FileDefs::FileHandle& hFile, 
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

  // Almacena clase
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&m_Attributes.Class), 
							  sizeof(RulesDefs::BaseType));
	
  // Atributos globales y locales
  udOffset += pFileSys->Write(hFile, 
	                          udOffset,
							  (sbyte *)(m_Attributes.GlobalAttribs), 
							  sizeof(RulesDefs::ItemGlobalAttrib) * RulesDefs::MAX_ITEM_GLOBALATTRIB);
  udOffset += pFileSys->Write(hFile, 
	                          udOffset,
							  (sbyte *)(m_Attributes.LocalAttribs), 
							  sizeof(RulesDefs::ItemLocalAttrib) * RulesDefs::MAX_ITEM_LOCALATTRIB);
  
  // Obtiene el posible tag asociado con esta entidad
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  udOffset += pFileSys->WriteStringInBinary(hFile, 
											udOffset, 
											pWorld->GetTagFromHandle(Inherited::GetHandle()));
  
  // Almacena luz
  const GraphDefs::Light Light = pWorld->GetLight(Inherited::GetHandle());
  udOffset += pFileSys->Write(hFile, 
							  udOffset, 
							  (sbyte *)(&Light), 
							  sizeof(GraphDefs::Light));
}

