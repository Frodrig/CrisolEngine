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
// CScript.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clase:
// - CScript
//
// Descripcion:
// - Representa un script. Desde aqui se mantendra la pila del script, 
//   su memoria, la informacion de las funciones y codigo del evento propiamente
//   dicho.
//
// Notas:
// - El metodo Stop de detencion obligatoria del script, sera utilizado por
//   el script global cuando interprete una instruccion Return y por la
//   instruccion de carga de un nuevo area, ya que en este ultimo caso si
//   el resultado tiene exito, todos los scripts del area anterior, dejaran
//   de ser validos.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CSCRIPT_H_
#define _CSCRIPT_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _ICSCRIPT_H_
#include "iCScript.h"
#endif
#ifndef _SCRIPTDEFS_H_
#include "ScriptDefs.h"
#endif
#ifndef _MEMORYPOOL_H_
#include "CMemoryPool.h"
#endif
#ifndef _FILEDEFS_H_
#include "FileDefs.h"
#endif
#ifndef _CSCRIPTSTACKVALUE_H_
#include "CScriptStackValue.h"
#endif
#ifndef _RULESDEFS_H_
#include "RulesDefs.h"
#endif
#ifndef _VECTOR_H_
#define _VECTOR_H_
#include <vector>
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif

// Definicion de clases / estructuras / espacios de nombres
class CScriptInstruction;

// Clase CScript
class CScript: public iCScript
{ 
private:
  // Tipos
  // Vector que representara la pila
  typedef std::vector<CScriptStackValue*> StackVector;
  typedef StackVector::iterator			  StackVectorIt;
  // Vector de codigos de instruccion
  typedef std::vector<CScriptInstruction*> CodeVector;
  typedef CodeVector::iterator             CodeVectorIt;
  // Map de strings (tabla de strings)
  typedef std::map<dword, std::string> StrTableMap;
  typedef StrTableMap::iterator        StrTableMapIt;
  typedef StrTableMap::value_type      StrTableMapValType;

private:
  // Estructuras
  struct sCodeInfo {	
	// Info asociada a una porcion de codigo
	// Enumerados
	enum eCodeType {
	  // Tipo de codigo
	  SCRIPT = 0,     // Codigo de evento script (codigo principal)
	  LOCAL_FUNC = 1, // Codigo de funcion local
	  GLOBAL_FUNC = 2 // Codigo de funcion global
	};

	// Datos	
	eCodeType   Type;           // Tipo de codigo
	CodeVector  Code;           // Codigo a ejecutar
	StrTableMap StrTable;       // Tabla de strings
	std::string szSignature;    // Firma
	word	    uwNumOffsets;   // Num. de slots de memoria
	word	    uwInitOffset;   // Valor del offset inicial
	word	    uwMaxStackSize; // Tamaño maximo de la pila

	// Pool de memoria
    static CMemoryPool m_MPool;
	static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
	static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); }
  };

private:
  // Tipos
  // Map para mantener la informacion relativa a las porciones de codigo
  typedef std::map<word, sCodeInfo*> CodeInfoMap;
  typedef CodeInfoMap::iterator		 CodeInfoMapIt;
  typedef CodeInfoMap::value_type    CodeInfoMapValType;

private:
  struct sRegisters {
	// Registros
	CodeInfoMapIt CodeInfoIt;      // Iterador al nodo con la info del codigo actual
	dword		  udCodePos;       // Pos. del siguiente codigo a ejecutar
	dword		  udStackFramePos; // Pos. donde comienza el Stack Frame
	dword		  udLocalsPos;     // Pos. donde comienzan los slots de mem. locales

	// Constructores
	sRegisters(const sRegisters& Registers): CodeInfoIt(Registers.CodeInfoIt),
											 udCodePos(Registers.udCodePos),
											 udStackFramePos(Registers.udStackFramePos),
											 udLocalsPos(Registers.udLocalsPos) { }
	sRegisters(void): udCodePos(0),
					  udStackFramePos(0),
					  udLocalsPos(0) { }	

	// Operadores
	sRegisters& operator=(const sRegisters& Registers) {
	  CodeInfoIt = Registers.CodeInfoIt;
	  udCodePos = Registers.udCodePos;
	  udStackFramePos = Registers.udStackFramePos;
	  udLocalsPos = Registers.udLocalsPos;
	  return *this;
	}
  };  

private:
  // Vbles de miembro
  CodeInfoMap			   m_CodeInfo;      // Info relativa a las porciones de codigo
  StackVector			   m_RunTimeStack;  // Pila
  sRegisters			   m_Registers;     // Registros actuales
  iCScript*				   m_pGlobalScript; // Enlace a script global
  ScriptDefs::eScriptState m_State;         // Estado inicial
  std::string              m_szScriptFile;  // Nombre del script
  RulesDefs::eScriptEvents m_Event;         // Evento al que esta asociado
  bool					   m_bIsInitOk;     // ¿Clase inicializada correctamente?   

public:
   // Constructor / Destructor
   CScript(void): m_pGlobalScript(NULL),
				  m_bIsInitOk(false) { }

  ~CScript(void) { 
    End();
  }               
  
public:
  // Protocolo de inicio y fin de instancia
  bool Init(const std::string& szScriptFileName,
			const RulesDefs::eScriptEvents& ScriptEvent,
			iCScript* pGlobalScript);
  bool Init(void);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }
private:
  // Metodos de apoyo
  void ReadCode(const FileDefs::FileHandle& hFile,
				dword& udOffset,
				CodeVector& Code);
  void ReadStringTable(const FileDefs::FileHandle& hFile,
					   dword& udOffset,
					   StrTableMap& StrTable);

public:
  // iCScript / Operaciones sobre la pila
  CScriptStackValue* Pop(void);
  void Push(CScriptStackValue* const pValue) {	  
	ASSERT(IsInitOk());  
	ASSERT(pValue);
	// Inserta el elemento en la lista
	m_RunTimeStack.push_back(pValue);
  }
  void PopStackFrame(void);
  bool PushStackFrame(const word uwCodeIdx);
  CScriptStackValue* const GetValueAt(const word uwOffset);
  void SetValueAt(const word uwOffset,
				  const float fValue);
  void SetValueAt(const word uwOffset,
				  const AreaDefs::EntHandle& hEntity);
  void SetValueAt(const word uwOffset, 
				  const std::string& szValue);
private:
  // Metodos de apoyo
  StackVectorIt GetValueSlotIt(const word uwOffset,
							   const dword udStackFramePos);
  inline word GetNumParams(const std::string& szSignature) {
	ASSERT(IsInitOk());
	ASSERT((szSignature.size() >= 2) != 0);	
	// Obtiene el numero de parametros NO void
	// Nota: En caso de que el primer parametro sea 'V' es seguro que no
	// tendra ninguno mas pues sera indicativo de que no tiene parametros
	// El primer valor SIEMPRE sera el del retorno
	if (szSignature[1] == 'V') {
	  return 0;
	} else {
	  return (szSignature.size() - 1);
	}
  }

public:
  // Operaciones de ejecucion
  bool Execute(ScriptDefs::ScriptParamList& Params);
  bool Execute(void) {
	ASSERT(IsInitOk());
	// Procede a ejecutar despues de haber salido de una pausa
	ASSERT((m_State == ScriptDefs::SS_RESUME) != 0);
	m_State = ScriptDefs::SS_RUNNING;
	return RunCode();	
  }
public:
  // iCScript
  void Pause(void) {
	ASSERT(IsInitOk());
	ASSERT((m_State == ScriptDefs::SS_RUNNING) != 0);
	// Establece estado de pausa
	m_State = ScriptDefs::SS_PAUSED;
  }
  void Resume(void) {
	ASSERT(IsInitOk());
	ASSERT((m_State == ScriptDefs::SS_PAUSED) != 0);
	// Establece estado en espera de salir del modo pausa
	m_State = ScriptDefs::SS_RESUME;
  }
  void SetCodePos(const dword udCodePos) {
	ASSERT(IsInitOk());
	ASSERT((udCodePos < m_Registers.CodeInfoIt->second->Code.size()) != 0);
	// Establece el nuevo offset desde donde ejecutar las instrucciones
	m_Registers.udCodePos = udCodePos;
  }
  void ErrorInterrupt(void) {
	ASSERT(IsInitOk());
	// Detiene script a causa de un error
	m_State = ScriptDefs::SS_ERROR;
  }
  void Stop(void) {
	ASSERT(IsInitOk());
	// Detendra el script en la posicion en donde se halle el script sin
	// posibilidad de continuar la ejecucion.
	m_State = ScriptDefs::SS_STOPPED;
  }
private:
  // Metodos de apoyo
  bool RunCode(void);
  std::string GetScriptParamsTypes(void);

public:
  // Operaciones con la tabla de strings
  std::string GetString(const word uwStrIdx) {
	ASSERT(IsInitOk());
	// Retorna string asociado al index
	ASSERT(m_Registers.CodeInfoIt->second);
	const StrTableMapIt StrIt(m_Registers.CodeInfoIt->second->StrTable.find(uwStrIdx));
	ASSERT((StrIt != m_Registers.CodeInfoIt->second->StrTable.end()) != 0);
	return StrIt->second;
  }

public:
  // Operaciones de consulta
  inline ScriptDefs::eScriptState GetState(void) const {
	ASSERT(IsInitOk());
	// Retorna el estado
	return m_State;
  }  
  inline RulesDefs::eScriptEvents GetEvent(void) const {
	ASSERT(IsInitOk());
	// Retorna el evento al que esta asociado
	return m_Event;
  }
  inline word GetNumLocalsPos(void) const {
	ASSERT(IsInitOk());
	// Retorna el num. de posiciones de mem. local
	return m_Registers.CodeInfoIt->second->uwNumOffsets;
  }  
public:
  // iCScript
  bool IsGlobalScript(void) const {
	ASSERT(IsInitOk());
	// Retorna flag
	return (NULL == m_pGlobalScript);
  }
};

#endif // ~ CScript
