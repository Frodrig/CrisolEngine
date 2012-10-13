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
// CFloor.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CFloor
//
// Descripcion:
// - Representa a una entidad de tipo suelo (la representacion del suelo)
// - La inicializacion de la entidad se realizara pasando el archivo en donde
//   se hallan los perfiles asi como el identificador de perfil que se desea.
//
// Notas:
// - Para ahorrar en recursos, los strings con los nombres de los archivos
//   WAVs se almacenaran en un map estatico. La entrada estara regida por los
//   tipos de los Floors.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CFLOOR_H_
#define _CFLOOR_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CENTITY_H_
#include "CEntity.h"
#endif
#ifndef _WORLDDEFS_H_
#include "WorldDefs.h"
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif

// Definicion de clases / estructuras / espacios de nombres

// Clase CFloor
class CFloor: public CEntity
{
private:
  // Tipos
  typedef CEntity Inherited; // Nombre de la clase base

private:
  // Estructuras
  struct sWAVWalkInfo {
	// Info asociada a un sonido WAV asociado a un Floor
	std::string szWAVFile; // Nombre del fichero
	word        uwRefs;    // Referencias
	// Constructores
	sWAVWalkInfo(const std::string& szWAVFile): szWAVFile(szWAVFile) {
	  uwRefs = 1;
	}
	sWAVWalkInfo(const sWAVWalkInfo& WAVWalkInfo) {
	  // Se copia directamente
	  szWAVFile = WAVWalkInfo.szWAVFile;
	  uwRefs = WAVWalkInfo.uwRefs;
	}
	// Operador de asignacion
	operator=(const sWAVWalkInfo& WAVWalkInfo) {
	  // Se toman los datos
	  szWAVFile = WAVWalkInfo.szWAVFile;
	  uwRefs = WAVWalkInfo.uwRefs;
	}
  };

private:
  // Tipos
  // Map para el almacenamiento de los sonidos asociados a Floor
  typedef std::map<RulesDefs::FloorType, sWAVWalkInfo> WAVWalkMap;
  typedef WAVWalkMap::value_type                       WAVWalkMapValType;
  typedef WAVWalkMap::iterator                         WAVWalkMapIt;
  
private:
  // Vbles static
  // Map con sonidos WAV asociados al caminar sobre Floors
  static WAVWalkMap m_WAVWalkMap;

private:
  // Vbles de miembro
  RulesDefs::sFloorAttrib m_Attributes; // Atributos  

public:
  // Constructor / destructor
  CFloor(void) { }
  ~CFloor(void) { End(); }

public:
  // Protocolos de inicializacion / finalizacion
  bool Init(const FileDefs::FileHandle& hFile,
		    dword& udOffset);
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
	ASSERT(IsInitOk());
	// Se propaga la peticion
	Inherited::Draw(swXPos, swYPos, IsoDefs::FLOOR_DRAW_PLANE, Light);
  }

public:
  // Obtencion del nombre del archivo WAV asociado
  std::string GetWAVWalkSoundFileName(void) {
	// Obtiene el iterador
	const WAVWalkMapIt It(m_WAVWalkMap.find(m_Attributes.Type));
	return (It != m_WAVWalkMap.end()) ? It->second.szWAVFile : "";
  }
private:
  // Metodos de apoyo
  static void InsertWAVWalkFile(const RulesDefs::FloorType& Type,
								const std::string& szWAVWalkFileName);
  static void ReleaseWAVWalkFile(const RulesDefs::FloorType& Type);

public:
  // Obtencion del contenedor
  inline iCItemContainer* const GetItemContainer(void) {
	ASSERT(Inherited::IsInitOk());
	// El Floor no es contenedor
	return NULL;
  }

public:
  // Obtencion del tipo del Floor
  inline RulesDefs::FloorType GetType(void) const {
	ASSERT(IsInitOk());
	// Retorna el tipo
	return m_Attributes.Type;
  }

public:
  // CEntity / Obtencion de informacion
  inline RulesDefs::eEntityType GetEntityType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se devuelve tipo de entidad
	return RulesDefs::FLOOR;
  }

public:
  // Notificacion de los eventos
  void OnSetIn(const AreaDefs::sTilePos& Pos,
			   const AreaDefs::EntHandle& hTheEntity);
  void OnSetOut(const AreaDefs::sTilePos& Pos,
				const AreaDefs::EntHandle& hTheEntity);

public:
  // iCScriptClient / Operacion de notificacion, para cuando acabe un comando
  void ScriptNotify(const RulesDefs::eScriptEvents& ScriptEvent,
				    const ScriptClientDefs::eScriptNotify& Notify,
					const dword udParams);
}; // ~ CFloor


#endif // ~ #ifdef _CFLOOR_H_
