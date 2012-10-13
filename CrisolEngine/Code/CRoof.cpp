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
// CRoof.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CRoof.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CRoof.h"

#include "iCFileSystem.h"

// Vbles static
CRoof::InvisibleRoofSet CRoof::m_InvisibleRoofSet;

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia
// Parametros:
// - hFile. Handle al fichero con datos asociados a la instancia.
// - udOffset. Offset donde localizar los datos.
// - hRoof. Handle de la instancia
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar.
///////////////////////////////////////////////////////////////////////////////
bool 
CRoof::Init(const FileDefs::FileHandle& hFile,
			dword& udOffset,
			const AreaDefs::EntHandle& hRoof)
{
  // SOLO si parametros validos
  ASSERT(hFile);
  ASSERT(hRoof);

  // Se inicializa clase base
  if (Inherited::Init(hFile, udOffset, hRoof)) {
	// Notificacion de no vision en pantalla
	VisibleInScreen(false);
  }

  // Se retorna resultado
  return Inherited::IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CRoof::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	// Elimina presencia del set
	m_InvisibleRoofSet.erase(Inherited::GetHandle());
	
	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Guarda los datos propios de CRoof
// Parametros:
// - hFile. Handle al fichero donde almacenar.
// - udOffset. Offset donde depositar la info.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CRoof::Save(const FileDefs::FileHandle& hFile, 
		    dword& udOffset)
{
  // SOLO si parametros correctos
  ASSERT(hFile);

  // Propaga
  Inherited::Save(hFile, udOffset);

  // Almacena elevacion
  iCFileSystem* const pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys);
  const RulesDefs::Elevation Elevation = Inherited::GetElevation();
  udOffset += pFileSys->Write(hFile,
							  udOffset,
							  (sbyte *)(&Elevation), 
							  sizeof(RulesDefs::Elevation));
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el Roof como visible / invisible. Si se establece como visible,
//   se sacara el set de invisibilidad y viceversa.
// Parametros:
// - bVisible. Flag de visibilidad / invisibilidad.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CRoof::SetVisible(const bool bVisible)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿El estado NO es el que se quiere especificar?
  if (IsVisible() != bVisible) {
	// ¿Se quiere poner visible?
	if (bVisible) {
	  // Si, entonces se sacara del set	  
	  m_InvisibleRoofSet.erase(Inherited::GetHandle());
	} else {
	  // No, se introduce
	  m_InvisibleRoofSet.insert(Inherited::GetHandle());
	}
  }
}
