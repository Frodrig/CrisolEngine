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
// CAnimTemplateData.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases
// - CAnimTemplateData.
//
// Descripcion:
// - Clase destinada a mantener los datos de una determinada plantilla de
//   animacion. Estos datos seran compartidos por instancias CAnimTemplate que
//   seran las que implementen la logica sobre estos datos aqui mantenidos.
// - CAnimTemplateData, tambien mantendra datos particulares asociados a 
//   determinadas CAnimTemplate, como seran los handles a los sonidos WAV que
//   pudieran estar asociados a estados.
// - Estas instancias CAnimTemplateData seran mantenidas en CResourceManager.
// - Las plantillas de animacion se hallara en el archivo AnimTemplates.cbb que
//   se recogera a traves de CGameDataBase. Para saber el formato del archivo
//   consultar CrisolBuilder.
//
// Notas:
// - Los posibles estados oscilaran entre 0 y 255.
// - La posicion de los frames seran enteros. Sera vital que para cambiar
//   de estado se llame a ChangeState, pues el estado primero no sera el 0
//   sino -1. El estado inicial no referenciara a ningun identificador
//   de textura. Sera necesario realizar siempre un NextFrame.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CANIMTEMPLATEDATA_H_
#define _CANIMTEMPLATEDATA_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _ICANIMTEMPLATEDATA_H_
#include "iCAnimTemplateData.h"
#endif
#ifndef _RESDEFS_H_
#include "RESDefs.h"
#endif
#ifndef _FILEDEFS_H_
#include "FileDefs.h"
#endif
#ifndef _SET_H_
#define _SET_H_
#include <set>
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif
#ifndef _VECTOR_H_
#define _VECTOR_H_
#include <vector>
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif

// Definicion de clases / estructuras / espacios de nombres

// Clase CAnimTemplate
class CAnimTemplateData: public iCAnimTemplateData
{
private:
  // Enumerados
  enum {
	// Frame de inicio en modo no reversible
	// Nota: en el caso del modo reversible, no se podra saber la posicion
	// ya que esta dependera del numero de frames del estado.
	STARTING_NOREVERSE_FRAME = -1, 
	// Identificador de alarma para estados ciclicos con esperas
	ID_CYCLICSTATE = 0X01,
  };

private:
  // Tipos
  // Map para el mantenimiento de las instancias WAV particulares
  typedef std::map<CAnimTemplate*, ResDefs::WAVSoundHandle> WAVHandlesMap;
  typedef WAVHandlesMap::iterator							WAVHandlesMapIt;
  typedef WAVHandlesMap::value_type							WAVHandlesMapValType;  

private:
  // Estructuras
  struct sNWAVState {
	// Nodo asociado a la informacion de sonido de un estado
	WAVHandlesMap WAVHandles;    // Handles particulares para cada instancia
	bool		  bWAVFlag;      // Se interpretara segun sea el enlace	
	std::string   szWAVFileName; // Nombre del fichero WAV
	// Constructores
	sNWAVState(void) { }
	sNWAVState(const sNWAVState& NWAVState): WAVHandles(NWAVState.WAVHandles),
											 bWAVFlag(NWAVState.bWAVFlag),
											 szWAVFileName(NWAVState.szWAVFileName) { }
  };

private:
  // Tipos
  // Map para el control de los handles de sonido asociados a estados
  typedef std::map<AnimTemplateDefs::AnimState, sNWAVState> WAVAnimStateMap;
  typedef WAVAnimStateMap::iterator                         WAVAnimStateMapIt;
  typedef WAVAnimStateMap::value_type					    WAVAnimStateMapValType;

private:
  // Tipos
  // Vector para almacenar frames
  typedef std::vector<ResDefs::TextureHandle> FramesVector;
  typedef FramesVector::iterator              FramesVectorIt;

private:
  // Estructuras
  struct sNOrientation {
	// Nodo asociado a una orientacion
	// Nota: todas las orientaciones tienen el mismo numero de frames
	FramesVector Frames; // Frames asociados a la orientacion	
	// Constructores
	sNOrientation(void) { }
	sNOrientation(const sNOrientation& NOrientation): Frames(NOrientation.Frames) { }
  };

private:
  // Tipos
  // Vector de orientaciones
  typedef std::vector<sNOrientation>  OrientationVector;
  typedef OrientationVector::iterator OrientationVectorIt;

private:
  // Estructuras
  struct sNState {
	// Nodo asociado a un estado
	AnimTemplateDefs::AnimFrame LinkState;    // Estado de transicion  	
	OrientationVector           Orientations; // Orientaciones en estado
	bool                        bReversible;  // ¿Estado reversible?
	float                       fCycleWait;   // Posible tiempo entre ciclos
	// Constructores
	sNState(void) { }	
	sNState(const sNState& NState): LinkState(NState.LinkState),									
									Orientations(NState.Orientations),
									bReversible(NState.bReversible),
									fCycleWait(NState.fCycleWait) { }
  };

private:
  // Tipos
  // Vector de estados de animacion
  typedef std::vector<sNState>  StateVector;
  typedef StateVector::iterator StateVectorIt;
  // Conjunto de clientes asociados
  typedef std::set<CAnimTemplate*> ClientSet;
  typedef ClientSet::iterator      ClientSetIt;

private:
  // Estructuras
  struct sATemplateData {
	// Datos asociados a la plantilla de animacion
	ClientSet                         Clients;         // Clientes asociados
	WAVAnimStateMap					  WAVInAnimStates; // WAVs en estados de animacion
	StateVector						  States;          // Estados de animacion
	AnimTemplateDefs::FramesPerSecond FPS;             // FPS estandar asociados a la clase
	word							  uwFramesWidth;   // Ancho de los frames
	word							  uwFramesHeight;  // Alto de los frames		
  };  

private:  
  // Vbles de miembro  
  sATemplateData m_ATemplateData;  // Datos asociados a la plantilla de animacion
  std::string    m_szAnimTemplate; // Identificador de la plantilla de animacion
  bool		     m_bIsInitOk;      // ¿Clase inicializada correctamente?  
  
public:
  // Constructor / destructor
  CAnimTemplateData(void): m_bIsInitOk(false) { }
  ~CAnimTemplateData(void) { End(); }

public:
  // Protocolo de inicio y fin de instancia
  bool Init(const std::string& szAnimTemplate);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }
private:
  // Metodos de apoyo
  bool TLoad(const std::string& szAnimTemplate);
  bool TLoadCfg(const FileDefs::FileHandle& hFile,
				dword& udOffsetData,
				word& uwNumStates,
				word& uwNumOrients);  
  bool TLoadState(const FileDefs::FileHandle& hFile,
				  dword& udOffsetData,
				  const AnimTemplateDefs::AnimState& State,
				  const word uwNumStates,
				  const word uwNumOrients);
  void RemoveATemplateData(void);   

public:
  // Trabajo con los clientes particulares
  void AddClient(CAnimTemplate* const pAnimTemplate);
  void RemoveClient(CAnimTemplate* const pAnimTemplate);  
  inline bool IsClientAttached(CAnimTemplate* const pAnimTemplate) const {
	ASSERT(IsInitOk());
	// Retorna el flag
	return (m_ATemplateData.Clients.find(pAnimTemplate) != m_ATemplateData.Clients.end());
  }

public:
  // iCAnimTemplateData / Obtencion del nombre de la plantilla de animacion
  std::string GetAnimTemplateName(void) const {
	ASSERT(IsInitOk());
	// Retorna el nombre de la plantilla
	return m_szAnimTemplate;
  }

public:
  // iCAnimTemplateData / Obtencion de info referida a handles a textura
  const ResDefs::TextureHandle& GetIDTexture(const AnimTemplateDefs::AnimState& State,
											 const AnimTemplateDefs::AnimOrientation& Orientation,
											 const AnimTemplateDefs::AnimFrame& Frame) {
	ASSERT((State < GetNumStates()) != 0);
	ASSERT((Orientation < GetNumOrientations()) != 0);
	ASSERT((Frame < GetNumFrames(State)) != 0);
	// Retorna el handle al ID de textura	
	return m_ATemplateData.States[State].Orientations[Orientation].Frames[Frame];
  }

public:
  // iCAnimTemplateData / Obtencion de info referida a sonido
  ResDefs::WAVSoundHandle GetWAVSoundHandle(CAnimTemplate* const pAnimTemplate,
										    const AnimTemplateDefs::AnimState& State) {
	ASSERT(IsInitOk());
	ASSERT(pAnimTemplate);
	// Retorna el handle PARTICULAR asociado al sonido WAV, en caso de no tener
	// retornara handle 0.
	const WAVAnimStateMapIt It(m_ATemplateData.WAVInAnimStates.find(State));
	return (It != m_ATemplateData.WAVInAnimStates.end()) ? It->second.WAVHandles.find(pAnimTemplate)->second : 0;
  }  
  bool GetWAVSoundFlag(CAnimTemplate* const pAnimTemplate,
					   const AnimTemplateDefs::AnimState& State) {
	ASSERT(IsInitOk());
	ASSERT(pAnimTemplate);
	// Retorna el flag de reproduccion asociado al estado
	const WAVAnimStateMapIt It(m_ATemplateData.WAVInAnimStates.find(State));
	ASSERT((It != m_ATemplateData.WAVInAnimStates.end()) != 0);
	return It->second.bWAVFlag;
  }
  
public:
  // iCAnimTemplateData / Obtencion de info referida a frames
  word GetNumFrames(const AnimTemplateDefs::AnimState& State) {
	ASSERT(IsInitOk());
	ASSERT((State < GetNumStates()) != 0);
	// Retorna el numero de frames asociados a un estado
	// Nota: bastara con retornar las de la primera orientacion
	return m_ATemplateData.States[State].Orientations.begin()->Frames.size();
  }
  bool IsReversibleState(const AnimTemplateDefs::AnimState& State) {
	ASSERT(IsInitOk());
	ASSERT((State < GetNumStates()) != 0);
	// Comprueba si el estado State es o no reversible
	return m_ATemplateData.States[State].bReversible;
  }

public:
  // iCAnimTemplate / Obtencion de info referida a orientaciones
  word GetNumOrientations(void) { 
	ASSERT(IsInitOk());
	// Returna el numero de orientaciones
	// Nota: bastara con retornar el numero de orientaciones del primer estado
	return m_ATemplateData.States.begin()->Orientations.size();
  }

public:
  // iCAnimTemplate / Obtencion de info referida a estados
  word GetNumStates(void) {
	ASSERT(IsInitOk());
	// Returna el numero de estados de animacion	
	return m_ATemplateData.States.size();
  }
  bool IsStateWithAnim(const AnimTemplateDefs::AnimState& State) {
	ASSERT(IsInitOk());
	ASSERT((State < GetNumStates()) != 0);
	// Comprueba si el estado recibido es un estado con animacion. Lo sera si
	// dispone de mas de un frame o bien tiene un estado de transicion.
	return (m_ATemplateData.States[State].LinkState != State ||
			GetNumFrames(State) > 1);
  }
  AnimTemplateDefs::AnimState GetLinkState(const AnimTemplateDefs::AnimState& State) {
	ASSERT(IsInitOk());
	ASSERT((State < GetNumStates()) != 0);
	// Retorna el estado de enlace asociado al estado State
	return m_ATemplateData.States[State].LinkState;
  }
  const float GetTimeBetweenCycles(const AnimTemplateDefs::AnimState& State) {
	ASSERT(IsInitOk());
	ASSERT((State < GetNumStates()) != 0);
	// Retorna el tiempo de espera entre ciclos
	return m_ATemplateData.States[State].fCycleWait;
  }

public:
  // iCAnimTemplate / Obtecion de informacion general  
  AnimTemplateDefs::FramesPerSecond GetFPS(void) {
	ASSERT(IsInitOk());
	// Retorna el numero de FPS estandar
	return m_ATemplateData.FPS;
  }
  word GetFramesWidth(void) {
	ASSERT(IsInitOk());
	// Retorna el ancho de los frames
	return m_ATemplateData.uwFramesWidth;
  }
  word GetFramesHeight(void) {
	ASSERT(IsInitOk());
	// Retorna el alto de los frames
	return m_ATemplateData.uwFramesHeight;
  }
};

#endif // ~ #ifdef _CANIMTEMPLATEDATA_H_