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
// CRoof.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CRoof
//
// Descripcion:
// - Representa una entidad de tipo techo. 
//
// Notas:
// - Los Roofs seran una de las entidades mas sencillas porque ni siquiera
//   tendran asociado tipo. Solo perfil.
// - Para saber si un Roof es visible y tiene que dibujarse, se mantendra
//   un set static (compartido por todos) en donde se asociara el handle de
//   del propio Roof. Si el handle se encuentra en el set, significara que
//   el Roof NO ES VISIBLE y si no se encuentra que si es visible y por lo
//   tanto, debe de propagar la peticion de dibujado.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CROOF_H_
#define _CROOF_H_

// Cabeceras
#ifndef _CENTITY_H_
#include "CEntity.h"
#endif
#ifndef _WORLDDEFS_H_
#include "WorldDefs.h"
#endif
#ifndef _SET_H_
#define _SET_H_
#include <set>
#endif

// Definicion de clases / estructuras / espacios de nombres

// Clase CRoof
class CRoof: public CEntity
{
private:
  // Tipos
  typedef CEntity Inherited; // Nombre de la clase base

private:
  // Tipos
  // Set static de invisibilidad
  typedef std::set<AreaDefs::EntHandle> InvisibleRoofSet;
  typedef InvisibleRoofSet::iterator    InvisibleRoofSetIt;

private:
  // Vbles static
  static InvisibleRoofSet m_InvisibleRoofSet; // Set de de roofs invisibles

public:
  // Constructor / destructor
  CRoof(void) { }
  ~CRoof(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const FileDefs::FileHandle& hFile,
			dword& udOffset,
			const AreaDefs::EntHandle& hRoof);  
  void End(void);

public:
  // Operacion de almacenamiento
  void Save(const FileDefs::FileHandle& hFile, 
			dword& udOffset);

public:
  // CEntity / Dibujado   
  void Draw(const sword& swXPos, 
			const sword& swYPos,
			const GraphDefs::sLight& Light) {
	ASSERT(Inherited::IsInitOk());
	// Filtrara el dibujado, solo si el Roof no se halla en el set de Roofs
	// invisibles, propagando la llamada a la clase base
	if (IsVisible()) {
	  // Propaga
	  Inherited::Draw(swXPos, 
					  swYPos - Inherited::GetElevation(), 
					  IsoDefs::ROOF_DRAW_PLANE,
					  Light);
	}
  }  

public:
  // Trabajo con el flag de visibilidad del Roof
  void SetVisible(const bool bVisible);
  inline bool IsVisible(void) {
	ASSERT(Inherited::IsInitOk());
	// Si no se halla el flag en el set de invisibilidad, sera visible
	return (m_InvisibleRoofSet.find(Inherited::GetHandle()) == m_InvisibleRoofSet.end());
  }

public:
  // Obtencion del contenedor
  inline iCItemContainer* const GetItemContainer(void) {
	ASSERT(Inherited::IsInitOk());
	// El Roof no es contenedor
	return NULL;
  }

public:
  // CEntity / Obtencion de informacion
  inline RulesDefs::eEntityType GetEntityType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se devuelve tipo de entidad
	return RulesDefs::ROOF;
  }

public:
  // iCScriptClient / Operacion de notificacion, para cuando acabe un comando
  void ScriptNotify(const RulesDefs::eScriptEvents& ScriptEvent,
				    const ScriptClientDefs::eScriptNotify& Notify,
					const dword udParams) { }
}; // ~ CRoof

#endif // ~ #ifdef _CROOF_H_
