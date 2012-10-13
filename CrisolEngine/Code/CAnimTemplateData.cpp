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
// CAnimTemplateData.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CAnimTemplateData.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CAnimTemplateData.h"

#include "SYSEngine.h"
#include "iCResourceManager.h"
#include "iCGameDataBase.h"
#include "iCFileSystem.h"
#include "iCLogger.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicia instancia.
// Parametros:
// - szAnimTemplate. Nombre del fichero con los datos
// Devuelve:
// - Si se ha inicializado todo correctamente true. En caso contrario false.
// Notas:
// - No se permitira reinicializar.
// - El metodo recibira siempre el nombre del fichero NO la extension, esta
//   sera añadida aqui mismo SIEMPRE.
///////////////////////////////////////////////////////////////////////////////
bool 
CAnimTemplateData::Init(const std::string& szAnimTemplate) 
{
  // SOLO si parametros validos
  ASSERT(szAnimTemplate.size());

  // ¿Se intenta reinicializar?
  if (IsInitOk()) { 
	return false; 
  }

  // Levanta flag de inicializacion para permitir llamada a End
  m_bIsInitOk = true;  	

  // Se establece el identificador de la plantilla en minusculas y se carga
  m_szAnimTemplate = szAnimTemplate;
  SYSEngine::MakeLowercase(m_szAnimTemplate);
  if (TLoad(m_szAnimTemplate)) {
	// Todo correcto	
	return true;
  }

  // Hubo problemas
  m_bIsInitOk = false;
  End();
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera todas las estructuras de datos asociadas a la plantilla de
//   animacion y baja los flags oportunos.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAnimTemplateData::End(void) 
{  
  // Finaliza si procede
  if (IsInitOk()) {
	// Se elimina los datos asociados a esta plantilla
	RemoveATemplateData();
	// Baja flag
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera los datos asociados a la plantilla de animacion. 
// Parametros:
// Devuelve:
// Notas:
// - No debera de existir ningun cliente asociado.
///////////////////////////////////////////////////////////////////////////////
void
CAnimTemplateData::RemoveATemplateData(void)
{
  // SOLO si no hay clientes asociados
  ASSERT(m_ATemplateData.Clients.empty());

  // Se toma instancia al servidor de recursos
  iCResourceManager* const pResManager = SYSEngine::GetResourceManager();
  ASSERT(pResManager);

  // Se recorren los estados    
  StateVectorIt StateIt(m_ATemplateData.States.begin());
  for (; StateIt != m_ATemplateData.States.end(); ++StateIt) {
	// Se recorren las orientaciones
	OrientationVectorIt OrientIt(StateIt->Orientations.begin());
	for (; OrientIt != StateIt->Orientations.end(); ++OrientIt) {
	  // Se recorren los frames
	  FramesVectorIt FramesIt(OrientIt->Frames.begin());
	  for (; FramesIt != OrientIt->Frames.end(); ++FramesIt) {
		// Se procede a liberar los handles a texturas
		pResManager->ReleaseTexture(*FramesIt);
	  }		  
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la carga de la plantilla de animacion desde un fichero de
//   datos en modo texto.
// Parametros:
// - szAnimTemplate. Identificador de la plantilla de animacion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CAnimTemplateData::TLoad(const std::string& szAnimTemplate)
{
  // SOLO si parametros correctos
  ASSERT(szAnimTemplate.size());

  // Se obtiene handle al fichero y el offset con el perfil
  iCGameDataBase* const pGDBase = SYSEngine::GetGameDataBase();
  ASSERT(pGDBase);
  const FileDefs::FileHandle hFile = pGDBase->GetCBBFileHandle(GameDataBaseDefs::CBBF_ANIMTEMPLATES);
  ASSERT(hFile);
  dword udOffsetData = pGDBase->GetCBBFileOffset(GameDataBaseDefs::CBBF_ANIMTEMPLATES,
												 szAnimTemplate);

  // Se carga configuracion de la plantilla
  word uwNumStates, uwNumOrients;
  if (TLoadCfg(hFile, udOffsetData, uwNumStates, uwNumOrients)) {
	// Carga estados
	word uwIt = 0;
	for (; uwIt < uwNumStates; ++uwIt) {
	  if (!TLoadState(hFile, udOffsetData, uwIt, uwNumStates, uwNumOrients)) {
		return false;
	  }
	}  

	// Todo correcto									   
	return true;	
  }  

  // Hubo problemas
  return false;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga datos de configuracion de la plantilla de animacion
// Parametros:
// - hFile. Handle al fichero.
// - udOffsetData. Offset donde comienzan los datos
// - uwNumStates, uwNumOrients. En estas vbles se dejara el numero de
//   estados y orientaciones para la posterior carga de la informacion.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - En cada lectura se incrementara el offset segun proceda.
///////////////////////////////////////////////////////////////////////////////
bool 
CAnimTemplateData::TLoadCfg(const FileDefs::FileHandle& hFile,
							dword& udOffsetData,
							word& uwNumStates,
							word& uwNumOrients)
{
  // SOLO si parametros validos
  ASSERT(hFile);
  
  // Obtiene estados de configuracion
  // Numero de estados  
  iCFileSystem* pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys);
  pFileSys->Read(hFile, (char*)(&uwNumStates), sizeof(word), udOffsetData);
  udOffsetData += sizeof(word);
  
  // Numero de orientaciones
  pFileSys->Read(hFile, (char*)(&uwNumOrients), sizeof(word), udOffsetData);
  udOffsetData += sizeof(word);
  
  // Ancho y alto de los frames
  pFileSys->Read(hFile, (char*)(&m_ATemplateData.uwFramesWidth), sizeof(word), udOffsetData);
  udOffsetData += sizeof(word);
  pFileSys->Read(hFile, (char*)(&m_ATemplateData.uwFramesHeight), sizeof(word), udOffsetData);
  udOffsetData += sizeof(word);

  // FPS  
  pFileSys->Read(hFile, (char*)(&m_ATemplateData.FPS), sizeof(AnimTemplateDefs::FramesPerSecond), udOffsetData);
  udOffsetData += sizeof(AnimTemplateDefs::FramesPerSecond);
  
  // Se preparan los vectores, reservando los estados necesarios
  m_ATemplateData.States.reserve(uwNumStates);
  
  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga datos asociados a un nuevo estado.
// Parametros:
// - hFile. Handle al fichero.
// - udOffsetData. Offset donde comienzan los datos
// - IDState. ID del estado a cargar
// - uwNumStates, uwNumOrients. Num. estados y orientaciones.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CAnimTemplateData::TLoadState(const FileDefs::FileHandle& hFile,
							  dword& udOffsetData,
							  const AnimTemplateDefs::AnimState& State,
							  const word uwNumStates,
							  const word uwNumOrients)
{
  // SOLO si parametros validos
  ASSERT(hFile);
  ASSERT(uwNumStates);
  ASSERT(uwNumOrients);  

  // Estado de enlace
  // En caso de que no este, se tomara el estado actual como estado de enlace  
  sNState NState;
  iCFileSystem* pFileSys = SYSEngine::GetFileSystem();
  ASSERT(pFileSys);
  pFileSys->Read(hFile, (char*)(&NState.LinkState), sizeof(AnimTemplateDefs::AnimFrame), udOffsetData);
  udOffsetData += sizeof(AnimTemplateDefs::AnimFrame);

  // Valor de espera entre frames
  pFileSys->Read(hFile, (char*)(&NState.fCycleWait), sizeof(float), udOffsetData);
  udOffsetData += sizeof(float);  

  // Flag de modo reversible
  pFileSys->Read(hFile, (char*)(&NState.bReversible), sizeof(bool), udOffsetData);
  udOffsetData += sizeof(bool);
  
  // Numero de frames
  word uwNumFrames;
  pFileSys->Read(hFile, (char*)(&uwNumFrames), sizeof(word), udOffsetData);
  udOffsetData += sizeof(word);
    
  // Se lee el posible nombre del WAV asociado 
  std::string szFramesFileName;
  udOffsetData += pFileSys->ReadStringFromBinary(hFile, udOffsetData, szFramesFileName);

  // Si hay WAV asociado, se crea un nodo para poder insertar en un futuro
  // clientes con sus handles particulres
  if (!szFramesFileName.empty()) {
	// Se registra WAV en la configuracion de la plantilla
	sNWAVState NWAVState;		
	pFileSys->Read(hFile, (char *)(&NWAVState.bWAVFlag), sizeof(bool), udOffsetData);  
	NWAVState.szWAVFileName = szFramesFileName;
	m_ATemplateData.WAVInAnimStates.insert(WAVAnimStateMapValType(State, NWAVState));
  }
  
  // Se salta el campo del flag asociado al wav bool
  udOffsetData += sizeof(bool);  

  // Se proceden a registrar los frames de cada una de las orientaciones
  NState.Orientations.reserve(uwNumOrients);
  byte ubOrientsIt = 0;
  for (; ubOrientsIt < uwNumOrients; ++ubOrientsIt) {
	// Vbles
	sRect       rFramePos;    // Region donde localizar los frames
	sPosition   CellStartPos; // Celda de inicio de busqueda	
	std::string szOrientVar;  // Nombre de la orientacion actual
	
	// Se obtiene celda de comienzo
	pFileSys->Read(hFile, (char*)(&CellStartPos.swXPos), sizeof(word), udOffsetData);
	udOffsetData += sizeof(word);
	pFileSys->Read(hFile, (char*)(&CellStartPos.swYPos), sizeof(word), udOffsetData);
	udOffsetData += sizeof(word);

	// Se calcula el top / bottom del area asociado al frame 
	rFramePos.swTop = CellStartPos.swYPos * m_ATemplateData.uwFramesHeight;
	rFramePos.swBottom = rFramePos.swTop + m_ATemplateData.uwFramesHeight - 1;
	
	// Se obtiene el fichero con los frames	
	std::string szFrameFile;
	udOffsetData += pFileSys->ReadStringFromBinary(hFile, udOffsetData, szFrameFile);

	// Se proceden a registrar las texturas que forman los frames
	iCResourceManager* const pResManager = SYSEngine::GetResourceManager();
	ASSERT(pResManager);    
	sNOrientation NOrientation;
	NOrientation.Frames.reserve(uwNumFrames);
	byte ubFramesIt = 0;
	for (; ubFramesIt < uwNumFrames; ++ubFramesIt) {
	  // Se determinan donde estan los frames
	  rFramePos.swLeft = (ubFramesIt + CellStartPos.swXPos) * m_ATemplateData.uwFramesWidth;
	  rFramePos.swRight = rFramePos.swLeft + m_ATemplateData.uwFramesWidth - 1;	  	  

	  // Se registra en el vector de frames asociado a la orientacion actual
	  NOrientation.Frames.push_back(pResManager->TextureRegister(szFrameFile,
																 rFramePos,
																 GraphDefs::BPP_16,
																 GraphDefs::ALPHA_BPP_1));
	}

	// Se inserta el nodo de orientacion en el vector de estados
	NState.Orientations.push_back(NOrientation);
  }

  // Se inserta el estado en el nodo de informacion de la plantilla
  m_ATemplateData.States.push_back(NState);

  // Todo correcto
  return true;
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia un cliente, particularizando para este los datos que no se puedan
//   compartir (handles a sonido)
// Parametros:
// - pAnimTemplate. Instancia a CAnimTemplate.
// Devuelve:
// Notas:
// - Los sonidos tendran control de volumen.
///////////////////////////////////////////////////////////////////////////////
void 
CAnimTemplateData::AddClient(CAnimTemplate* const pAnimTemplate)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());  
  // SOLO si parametros validos
  ASSERT(pAnimTemplate);
  ASSERT((m_ATemplateData.Clients.find(pAnimTemplate) == m_ATemplateData.Clients.end()) != 0);

  // Se inserta el cliente
  m_ATemplateData.Clients.insert(pAnimTemplate);

  // Se toma instancia al servidor de recursos
  iCResourceManager* const pResManager = SYSEngine::GetResourceManager();
  ASSERT(pResManager);

  // Se recorre el map de sonidos asociados a estados
  WAVAnimStateMapIt WAVIt(m_ATemplateData.WAVInAnimStates.begin());
  for (; WAVIt != m_ATemplateData.WAVInAnimStates.end(); ++WAVIt) {
	// Se crea un handle para la instancia particular y se registra
	const ResDefs::WAVSoundHandle hNewWAVSound = pResManager->WAVSoundRegister(WAVIt->second.szWAVFileName, 
																			   true);
	if (!hNewWAVSound) {
	  SYSEngine::FatalError("No se pudo abrir el fichero de sonido \"%s\".\nFichero inexistente o incompatible.\n", WAVIt->second.szWAVFileName.c_str());
	  return;
	}
	WAVIt->second.WAVHandles.insert(WAVHandlesMapValType(pAnimTemplate, hNewWAVSound));  
  }
}

//////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se elimina un cliente. Al eliminarlo, tambien se desprendera toda info
//   de caracter particular sobre el mismo.
// Parametros:
// - pAnimTemplate. Instancia al AnimTemplate
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAnimTemplateData::RemoveClient(CAnimTemplate* const pAnimTemplate)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(pAnimTemplate);
  ASSERT((m_ATemplateData.Clients.find(pAnimTemplate) != m_ATemplateData.Clients.end()) != 0);

  // Se elimina el cliente
  m_ATemplateData.Clients.erase(pAnimTemplate);

  // Se toma instancia al servidor de recursos
  iCResourceManager* const pResManager = SYSEngine::GetResourceManager();
  ASSERT(pResManager);

  // Se recorre el map de sonidos asociados a estados
  WAVAnimStateMapIt WAVIt(m_ATemplateData.WAVInAnimStates.begin());
  for (; WAVIt != m_ATemplateData.WAVInAnimStates.end(); ++WAVIt) {
	// Se libera handle del servidor de recursos
	pResManager->ReleaseWAVSound(WAVIt->second.WAVHandles.find(pAnimTemplate)->second);
	// Se libera del nodo de sonidos WAV
	WAVIt->second.WAVHandles.erase(pAnimTemplate);
  }
}