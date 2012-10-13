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
// CFloor.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CFloor.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CFloor.h"

#include "SYSEngine.h"
#include "iCFileSystem.h"
#include "iCVirtualMachine.h"
#include "AreaDefs.h"

// Se declaran miembros static
// Map con sonidos WAV asociados al caminar sobre Floors
CFloor::WAVWalkMap CFloor::m_WAVWalkMap;

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia cargando datos desde disco.
// Parametros:
// - hFile. Handle al fichero.
// - udOffset. Offset donde localizar la informacion.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - El handle asociado al Floor sera el mismo para todos, se usara el
//   identificador comun.
///////////////////////////////////////////////////////////////////////////////
bool 
CFloor::Init(const FileDefs::FileHandle& hFile,
			 dword& udOffset)
{
  // SOLO si parametros validos
  ASSERT(hFile);

  // Se inicializa clase base
  if (Inherited::Init(hFile, udOffset, RulesDefs::FLOOR)) {
	// Se carga tipo y se asocia como atributo	
	iCFileSystem* pFileSys = SYSEngine::GetFileSystem();
	ASSERT(pFileSys);
	udOffset += pFileSys->Read(hFile, 
							   (sbyte *)(&m_Attributes.Type), 
							   sizeof(RulesDefs::BaseType),
							   udOffset);

	// Se carga el nombre del posible fichero WAV asociado
	std::string szWAVWalkFileName;
	udOffset += pFileSys->ReadStringFromBinary(hFile, udOffset, szWAVWalkFileName);	
	InsertWAVWalkFile(m_Attributes.Type, szWAVWalkFileName);

	// Notificacion de no vision en pantalla
	VisibleInScreen(false);
  }

  // Se retorna resultado
  return Inherited::IsInitOk();
}  

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CFloor::End(void)
{
  // Procede a finalizar
  if (Inherited::IsInitOk()) {
	// Libera sonido WAV asociado
	ReleaseWAVWalkFile(m_Attributes.Type);

	// Finaliza posibles scripts asociados
	SYSEngine::GetVirtualMachine()->ReleaseScripts(this);

	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Guarda los datos propios de CFloor
// Parametros:
// - hFile. Handle al fichero donde almacenar.
// - udOffset. Offset donde depositar la info.
// Devuelve:
// Notas:
// - La mascara de acceso asociada al floor se almacenara en CArea.
///////////////////////////////////////////////////////////////////////////////
void 
CFloor::Save(const FileDefs::FileHandle& hFile, 
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
  
  // Almacena nombre del wav asociado
  udOffset += pFileSys->WriteStringInBinary(hFile, 
											udOffset, 
											GetWAVWalkSoundFileName());  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inserta el posible fichero WAV en el map estatico. La insercion se
//   realizara en base al tipo del floor. Si ya hay registro, se debera de
//   cumplir que sea igual al archivo recibido. En este caso se incrementara
//   el contador de referencias.
// Parametros:
// - Type. Tipo del Floor
// - szWAVWalkFileName. Nombre del fichero
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CFloor::InsertWAVWalkFile(const RulesDefs::FloorType& Type,
						  const std::string& szWAVWalkFileName)
{
  // ¿Se ha recibido nombre de archivo?
  if (!szWAVWalkFileName.empty()) {
	// Se comprueba si ya se registro valor
	const WAVWalkMapIt It(m_WAVWalkMap.find(Type));
	if (It != m_WAVWalkMap.end()) {
	  // Si, luego se incrementa contador de referencias
	  ASSERT((0 == stricmp(szWAVWalkFileName.c_str(), It->second.szWAVFile.c_str())) != 0);
	  ++It->second.uwRefs;
	} else {
	  // No, luego se inserta en el map
	  m_WAVWalkMap.insert(WAVWalkMapValType(Type, sWAVWalkInfo(szWAVWalkFileName)));
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina el posible sonido WAV asociado a esta instancia. Como estaran
//   mantenidos por el tipo del floor, bastara comprobar si hay entrada para
//   el tipo.
// Parametros:
// - Type. Tipo del floor.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CFloor::ReleaseWAVWalkFile(const RulesDefs::FloorType& Type)
{
  // ¿Existe entrada?
  const WAVWalkMapIt It(m_WAVWalkMap.find(Type));
  if (It != m_WAVWalkMap.end()) {
	// ¿Ya no quedan referencias?
	if (1 == It->second.uwRefs) {
	  // No, se elimina entrada
	  m_WAVWalkMap.erase(It);
	} else {
	  // Si, se decrementa entrada
	  --It->second.uwRefs;
	}
  }	
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notifica que se ha colocado una entidad sobre el Floor para que este
//   active el script correspondiente
// Parametros:
// - Pos. Posicion donde esta situado el Floor / Tile
// - TheEntity. La entidad colocada en el Floor
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CFloor::OnSetIn(const AreaDefs::sTilePos& Pos,
				const AreaDefs::EntHandle& hTheEntity)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(hTheEntity);
  
  // Comunica a la maquina virtual el evento
  SYSEngine::GetVirtualMachine()->OnSetInFloor(this,
											   Inherited::GetScriptEvent(RulesDefs::SE_ONSETIN),
											   Pos.XTile,
											   Pos.YTile,
											   hTheEntity);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notifica que se ha extraido una entidad del Floor para que este
//   active el script correspondiente.
// Parametros:
// - Pos. Posicion del Floor / Tile
// - TheEntity. La entidad que se ha extraido.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CFloor::OnSetOut(const AreaDefs::sTilePos& Pos,
				 const AreaDefs::EntHandle& hTheEntity)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(hTheEntity);
  
  // Comunica a la maquina virtual el evento
  SYSEngine::GetVirtualMachine()->OnSetOutFloor(this,
											    Inherited::GetScriptEvent(RulesDefs::SE_ONSETOUT),
											    Pos.XTile,
											    Pos.YTile,
											    hTheEntity);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion por parte de los scripts lanzados desde esta clase.
// Parametros:
// - Notify. Tipo de notificacion.
// - ScriptEvent. Evento que lo lanza.
// - udParams. Parametros asociados.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CFloor::ScriptNotify(const RulesDefs::eScriptEvents& ScriptEvent,
				     const ScriptClientDefs::eScriptNotify& Notify,
					 const dword udParams)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Filtrado de evento que manda notificacion
  switch(ScriptEvent) {
	case RulesDefs::SE_ONSETIN: {
	  // Sin comportamiento por defecto
	} break;
	
	case RulesDefs::SE_ONSETOUT: {
	  // Sin comportamiento por defecto
	} break;	  	
  }; // ~ switch
}

