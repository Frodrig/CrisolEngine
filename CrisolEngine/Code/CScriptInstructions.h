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
// CScriptInstructions.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clase:
// - CScriptInstruction
//    * ...
//
// Descripcion:
// - Se implementan todas las instrucciones de codigo intermedio que maneja
//   un script. Todas las instrucciones heredaran de CScriptInstruction y tendran
//   que implementar un metodo virtual puro Execute que esperara recibir
//   la instancia a una interfaz iCScript.
//
// Notas:
// - En todas las instrucciones relativas a entidades, SIEMPRE se comprobara
//   que el handle pasado como entidad exista o bien corresponda con el tipo
//   esperado para realizar la accion. Si el metodo a realizar esperara
//   retornar algun valor, se depositara el valor por defecto.
// - A la hora de obtener los parametros de la pila para realizar una operacion
//   habra que tener en cuenta que seran recogidos en orden inverso a como
//   fueron insertados (se coge desde el ultimo parametro al primero) por
//   trabajar precisamente con una pila.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CSCRIPTINSTRUCTIONS_H_
#define _CSCRIPTINSTRUCTIONS_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _SCRIPTDEFS_H_
#include "ScriptDefs.h"
#endif
#ifndef _FILEDEFS_H_
#include "FileDefs.h"
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _ICGUIWINDOWCLIENT_H_
#include "iCGUIWindowClient.h"
#endif
#ifndef _ICCRIATUREOBSERVER_H_
#include "iCCriatureObserver.h"
#endif
#ifndef _ICCOMMANDCLIENT_H_
#include "iCCommandClient.h"
#endif
#ifndef _ICALARMCLIENT_H_
#include "iCAlarmClient.h"
#endif
#ifndef _ICWORLDOBSERVER_H_
#include "iCWorldObserver.h"
#endif

// Definicion de clases / estructuras / espacios de nombres
struct iCScript;
class CCriature;
class CWorldEntity;

// Clase CScriptInstruction
class CScriptInstruction 
{
private:
  // Vbles de miembro
  bool m_bIsInitOk; // ¿Instancia incializada correctamente?

public:
  // Constructor / destructor
  CScriptInstruction(void): m_bIsInitOk(false) { }
  virtual ~CScriptInstruction(void) { End(); }

public:
  // Protocolos de inicilizacion / finalizacion de instancia.  
  virtual bool Init(const FileDefs::FileHandle& hFile,
					dword& udOffset);
  virtual void End(void);
  bool IsInitOk(void) const { return m_bIsInitOk; }

public:
  // Ejecucion
  virtual void Execute(iCScript* const pScript) = 0;

public:
  // Operaciones de consulta
  virtual ScriptDefs::eScriptInstruction GetScriptInstruction(void) const = 0;
};

// Clase CNopInstr
class CNopInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_NOP;
  }
};

// Clase CNNegInstr
class CNNegInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_NNEG;
  }
};

// Clase CNMulInstr
class CNMulInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_NMUL;
  }
};

// Clase CNAddInstr
class CNAddInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_NADD;
  }
};

// Clase CNModInstr
class CNModInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_NMOD;
  }
};

// Clase CNDivInstr
class CNDivInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_NDIV;
  }
};

// Clase CNSubInstr
class CNSubInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_NSUB;
  }
};

// Clase CSAddInstr
class CSAddInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_SADD;
  }
};

// Clase CBaseJmpInstr
class CBaseJmpInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Vbles de miembro
  dword m_udJmpOffset; // Offset a donde saltar

public:
  // Protocolos de inicilizacion / finalizacion de instancia.  
  bool Init(const FileDefs::FileHandle& hFile,
			dword& udOffset);

protected:
  // Obtencion de valores
  inline dword GetJmpOffset(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna valor
	return m_udJmpOffset;
  }
};

// Clase CJmpInstr
class CJmpInstr: public CBaseJmpInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseJmpInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_JMP;
  }
};

// Clase CJmpFalseInstr
class CJmpFalseInstr: public CBaseJmpInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseJmpInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_JMP_FALSE;
  }
};

// Clase CJmpTrueInstr
class CJmpTrueInstr: public CBaseJmpInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseJmpInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_JMP_TRUE;
  }
};

// Clase CNJmpEQInstr
class CNJmpEQInstr: public CBaseJmpInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseJmpInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_NJMP_EQ;
  }
};

// Clase CNJmpNEInstr
class CNJmpNEInstr: public CBaseJmpInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseJmpInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_NJMP_NE;
  }
};

// Clase CNJmpGEInstr
class CNJmpGEInstr: public CBaseJmpInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseJmpInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_NJMP_GE;
  }
};

// Clase CNJmpGTInstr
class CNJmpGTInstr: public CBaseJmpInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseJmpInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_NJMP_GT;
  }
};

// Clase CNJmpLTInstr
class CNJmpLTInstr: public CBaseJmpInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseJmpInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_NJMP_LT;
  }
};

// Clase CNJmpLEInstr
class CNJmpLEInstr: public CBaseJmpInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseJmpInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_NJMP_LE;
  }
};

// Clase CSJmpEQInstr
class CSJmpEQInstr: public CBaseJmpInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseJmpInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_SJMP_EQ;
  }
};

// Clase CSJmpNEInstr
class CSJmpNEInstr: public CBaseJmpInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseJmpInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_SJMP_NE;
  }
};

// Clase CEJmpEQInstr
class CEJmpEQInstr: public CBaseJmpInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseJmpInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_EJMP_EQ;
  }
};

// Clase CEJmpNEInstr
class CEJmpNEInstr: public CBaseJmpInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseJmpInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_EJMP_NE;
  }
};

// Clase CDupInstr
class CDupInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_DUP;
  }
};

// Clase CPopInstr
class CPopInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_POP;
  }
};

// Clase CReturnInstru
class CReturnInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_RETURN;
  }
};

// Clase CBaseLoadInstr
class CBaseLoadInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Vbles de miembro
  word m_uwMemSlot; // Slot de memoria

public:
  // Protocolos de inicilizacion / finalizacion de instancia.  
  bool Init(const FileDefs::FileHandle& hFile,
			dword& udOffset);

protected:
  // Operaciones de consulta
  inline word GetMemSlot(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna valor del slot
	return m_uwMemSlot;
  }
};

// Clase CNLoadInstr
class CNLoadInstr: public CBaseLoadInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseLoadInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_NLOAD;
  }
};

// Clase CSLoadInstr
class CSLoadInstr: public CBaseLoadInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseLoadInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_SLOAD;
  }
};

// Clase CELoadInstr
class CELoadInstr: public CBaseLoadInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseLoadInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ELOAD;
  }
};

// Clase CBaseStoreInstr
class CBaseStoreInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Vbles de miembro
  word m_uwMemSlot; // Slot de memoria

public:
  // Protocolos de inicilizacion / finalizacion de instancia.  
  bool Init(const FileDefs::FileHandle& hFile,
			dword& udOffset);

protected:
  // Operaciones de consulta
  inline word GetMemSlot(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna valor
	return m_uwMemSlot;
  }
};

// Clase CNStoreInstr
class CNStoreInstr: public CBaseStoreInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseStoreInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_NSTORE;
  }
};

// Clase CSStoreInstr
class CSStoreInstr: public CBaseStoreInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseStoreInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_SSTORE;
  }
};

// Clase CEStoreInstr
class CEStoreInstr: public CBaseStoreInstr
{
public:
  // Tipos
  // Clase base
  typedef CBaseStoreInstr Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ESTORE;
  }
};

// Clase CNPushInstr
class CNPushInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Vbles de miembro
  float m_fValue; // Valor a establecer

public:
  // Protocolos de inicilizacion / finalizacion de instancia.  
  bool Init(const FileDefs::FileHandle& hFile,
			dword& udOffset);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_NPUSH;
  }
};

// Clase CSPushInstr
class CSPushInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Vbles de miembro
  dword m_udStrIdx; // Indice al string

public:
  // Protocolos de inicilizacion / finalizacion de instancia.  
  bool Init(const FileDefs::FileHandle& hFile,
			dword& udOffset);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_SPUSH;
  }
};

// Clase CEPushInstr
class CEPushInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Vbles de miembro
  AreaDefs::EntHandle m_hValue; // Valor a establecer

public:
  // Protocolos de inicilizacion / finalizacion de instancia.  
  bool Init(const FileDefs::FileHandle& hFile,
			dword& udOffset);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_EPUSH;
  }
};

// Clase CNSCastInstr
class CNSCastInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_NSCAST;
  }
};

// Clase CSNCastInstr
class CSNCastInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_SNCAST;
  }
};

// Clase CCallFuncInstr
class CCallFuncInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Vbles de miembro
  word m_uwCodeIdx; // Idx de la porcion de codigo a ejecutar

public:
  // Protocolos de inicilizacion / finalizacion de instancia.  
  bool Init(const FileDefs::FileHandle& hFile,
			dword& udOffset);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_CALL_FUNC;
  }
};

// Clase CAPIPassToRGBColorInstr
class CAPIPassToRGBColorInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_API_PASSTORGBCOLOR;
  }
};

// Clase CAPIGetRedComponentInstr
class CAPIGetRedComponentInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_API_GETREDCOMPONENT;
  }
};

// Clase CAPIGetGreenComponentInstr
class CAPIGetGreenComponentInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_API_GETGREENCOMPONENT;
  }
};

// Clase CAPIGetBlueComponentInstr
class CAPIGetBlueComponentInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_API_GETBLUECOMPONENT;
  }
};

// Clase CAPIRandInstr
class CAPIRandInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_API_RAND;
  }
};

// Clase CAPIGetIntegerValueInstr
class CAPIGetIntegerValueInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_API_GETINTEGERVALUE;
  }
};

// Clase CAPIGetDecimalValueInstr
class CAPIGetDecimalValueInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_API_GETDECIMALVALUE;
  }
};

// Clase CAPIGetStringSizeInstr
class CAPIGetStringSizeInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_API_GETSTRINGSIZE;
  }
};

// Clase CAPIWriteToLoggerInstr
class CAPIWriteToLoggerInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_API_WRITETOLOGGER;
  }
};

// Clase CAPIEnableCrisolScriptWarningsInstr
class CAPIEnableCrisolScriptWarningsInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_API_ENABLECRISOLSCRIPTWARNINGS;
  }
};

// Clase CAPIDisableCrisolScriptWarningsInstr
class CAPIDisableCrisolScriptWarningsInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_API_DISABLECRISOLSCRIPTWARNINGS;
  }
};

// Clase CAPIShowFPSInstr
class CAPIShowFPSInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_API_SHOWFPS;
  }
};

// Clase CAPIWaitInstr
class CAPIWaitInstr: public CScriptInstruction,
					 public iCAlarmClient,
					 public iCWorldObserver
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Vbles de miembro
  iCScript*              m_pScript; // Script asociado
  AlarmDefs::AlarmHandle m_hAlarm;  // Handle a la alarma

public:
  // Constructor / Destructor
  CAPIWaitInstr(void): m_pScript(NULL),
					   m_hAlarm(0) { }
  ~CAPIWaitInstr(void) {
	End();
  }
private:
  // Metodos de apoyo
  void End(void);

public:
  // Notificacion de la finalizacion de una alarma
  void AlarmNotify(const AlarmDefs::eAlarmType& AlarmType,
				   const AlarmDefs::AlarmHandle& hAlarm);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_API_WAIT;
  }
};


// Clase CGOQuitGameInstr
class CGOQuitGameInstr: public CScriptInstruction,
						public iCCommandClient
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Constantes
  // Identificador del FadeOut
  const CommandDefs::IDCommand IDFadeOut; 

private:
  // Vbles de miembro
  iCScript* m_pScript; // Script asociado

public:
  // Constructor
  CGOQuitGameInstr(void): IDFadeOut(1),
						  m_pScript(NULL) { }

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operacion de notificacion, para cuando acabe un comando
  void EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
					const dword udInstant,
					const dword udExtraParam);  
public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_QUITGAME;
  }
};

// Clase CGOWriteToConsoleInstr
class CGOWriteToConsoleInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_WRITETOCONSOLE;
  }
};

// Clase CGOActiveAdviceDialogInstr
class CGOActiveAdviceDialogInstr: public CScriptInstruction,
                                  public iCGUIWindowClient
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Variables de miembro
  iCScript* m_pScript; // Instancia al script asociado

public:
  // Constructor / Destructor
  CGOActiveAdviceDialogInstr(void): m_pScript(NULL) { }
  ~CGOActiveAdviceDialogInstr(void) {
	End();
  }
private:
  // Metodos de apoyo
  void End(void);
  
public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_ACTIVEADVICEDIALOG;
  }

public:
  // iCGUIWindowClient / Notificacion de un determinado suceso en un interfaz
  void GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
					   const dword udParams);
};

// Clase CGOActiveQuestionDialogInstr
class CGOActiveQuestionDialogInstr: public CScriptInstruction,
									public iCGUIWindowClient
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Variables de miembro
  iCScript* m_pScript; // Instancia al script asociado

public:
  // Constructor
  CGOActiveQuestionDialogInstr(void): m_pScript(NULL) { }
  ~CGOActiveQuestionDialogInstr(void) {
	End();
  }
private:
  // Metodos de apoyo
  void End(void);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // iCGUIWindowClient / Notificacion de un determinado suceso en un interfaz
  void GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
					   const dword udParams);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_ACTIVEQUESTIONDIALOG;
  }
};

// Clase CGOActiveTextReaderDialogInstr
class CGOActiveTextReaderDialogInstr: public CScriptInstruction,
									  public iCGUIWindowClient
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Variables de miembro
  iCScript* m_pScript; // Instancia al script asociado

public:
  // Constructor
  CGOActiveTextReaderDialogInstr(void): m_pScript(NULL) { }
  ~CGOActiveTextReaderDialogInstr(void) {
	End();
  }
private:
  // Metodos de apoyo
  void End(void);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // iCGUIWindowClient / Notificacion de un determinado suceso en un interfaz
  void GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
					   const dword udParams);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_ACTIVETEXTREADERDIALOG;
  }
};

// Clase CGOAddOptionToTextSelectorDialogInstr
class CGOAddOptionToTextSelectorDialogInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_ADDOPTIONTOTEXTSELECTORDIALOG;
  }
};

// Clase CGOResetOptionsInTextSelectorDialogInstr
class CGOResetOptionsInTextSelectorDialogInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_RESETOPTIONSINTEXTSELECTORDIALOG;
  }
};

// Clase CGOActiveTextSelectorDialogInstr
class CGOActiveTextSelectorDialogInstr: public CScriptInstruction,
									    public iCGUIWindowClient
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Variables de miembro
  iCScript* m_pScript; // Instancia al script asociado

public:
  // Constructor
  CGOActiveTextSelectorDialogInstr(void): m_pScript(NULL) { }
  ~CGOActiveTextSelectorDialogInstr(void) {
	End();
  }
private:
  // Metodos de apoyo
  void End(void);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // iCGUIWindowClient / Notificacion de un determinado suceso en un interfaz
  void GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
					   const dword udParams);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_ACTIVETEXTSELECTORDIALOG;
  }
};

// Clase CGOPlayMidiMusicInstr
class CGOPlayMidiMusicInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_PLAYMIDIMUSIC;
  }
};

// Clase CGOStopMidiMusicInstr
class CGOStopMidiMusicInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_STOPMIDIMUSIC;
  }
};

// Clase CGOPlayWavAmbientSoundInstr
class CGOPlayWavAmbientSoundInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_PLAYWAVAMBIENTSOUND;
  }
};

// Clase CGOStopWavAmbientSoundInstr
class CGOStopWavAmbientSoundInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_STOPWAVAMBIENTSOUND;
  }
};

// Clase CGOActiveTradeItemsInterfazInstr
class CGOActiveTradeItemsInterfazInstr: public CScriptInstruction,
									    public iCGUIWindowClient
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Variables de miembro
  iCScript* m_pScript; // Instancia al script asociado

public:
  // Constructor
  CGOActiveTradeItemsInterfazInstr(void): m_pScript(NULL) { }
  ~CGOActiveTradeItemsInterfazInstr(void) {
	End();
  }
private:
  // Metodos de apoyo
  void End(void);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // iCGUIWindowClient / Notificacion de un determinado suceso en un interfaz
  void GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
					   const dword udParams);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_ACTIVETRADEITEMSINTERFAZ;
  }
};

// Clase CGOAddOptionToConversatorInterfazInstr
class CGOAddOptionToConversatorInterfazInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_ADDOPTIONTOCONVERSATORINTERFAZ;
  }
};

// Clase CGOResetOptionsInConversatorInterfazInstr
class CGOResetOptionsInConversatorInterfazInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_RESETOPTIONSINCONVERSATORINTERFAZ;
  }
};

// Clase CGOActiveConversatorInterfazInstr
class CGOActiveConversatorInterfazInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_ACTIVECONVERSATORINTERFAZ;
  }
};

// Clase CGODeactiveConversatorInterfazInstr
class CGODeactiveConversatorInterfazInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_DEACTIVECONVERSATORINTERFAZ;
  }
};

// Clase CGOGetOptionFromConversatorInterfazInstr
class CGOGetOptionFromConversatorInterfazInstr: public CScriptInstruction,
										        public iCGUIWindowClient
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Variables de miembro
  iCScript* m_pScript; // Instancia al script asociado

public:
  // Constructor / Destructor
  CGOGetOptionFromConversatorInterfazInstr(void): m_pScript(NULL) { }
  ~CGOGetOptionFromConversatorInterfazInstr(void) {
	End();
  }
private:
  // Metodos de apoyo
  void End(void);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // iCGUIWindowClient / Notificacion de un determinado suceso en un interfaz
  void GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
					   const dword udParams);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_GETOPTIONFROMCONVERSATORINTERFAZ;
  }
};

// Clase CGOShowPresentationInstr
class CGOShowPresentationInstr: public CScriptInstruction,
								public iCWorldObserver
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Vbles de miembro
  iCScript* m_pScript; // Script al que esta asociado

public:
  // Constructor / Destructor
  CGOShowPresentationInstr(void): m_pScript(NULL) { }
  ~CGOShowPresentationInstr(void) {
	End();
  }
private:
  // Metodos de apoyo
  void End(void);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_SHOWPRESENTATION;
  }
};

// Clase CGOBeginCutSceneInstr
class CGOBeginCutSceneInstr: public CScriptInstruction,
							 public iCCommandClient
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Constantes
  const CommandDefs::IDCommand IDEndBeginCutScene;

private:
  // Vbles de miembro
  iCScript* m_pScript; // Script asociado

public:
  // Constructor
  CGOBeginCutSceneInstr(void): IDEndBeginCutScene(1),
							   m_pScript(NULL) { }

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // iCComandClient / Operacion de notificacion, para cuando acabe un comando
  void EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
					const dword udInstant,
					const dword udExtraParam);  
public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_BEGINCUTSCENE;
  }
};

// Clase CGOEndCutSceneInstr
class CGOEndCutSceneInstr: public CScriptInstruction,
						   public iCCommandClient
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Constantes
  const CommandDefs::IDCommand IDEndEndCutScene;

private:
  // Vbles de miembro
  iCScript* m_pScript; // Script asociado

public:
  // Constructor
  CGOEndCutSceneInstr(void): m_pScript(NULL),
							 IDEndEndCutScene(1) { }
public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // iCComandClient / Operacion de notificacion, para cuando acabe un comando
  void EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
					const dword udInstant,
					const dword udExtraParam);  

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_ENDCUTSCENE;
  }
};

// Clase CGOSetScriptInstr
class CGOSetScriptInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_SETSCRIPT;
  }
};

// Clase CGOIsKeyPressedInstr
class CGOIsKeyPressedInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_GAMEOBJ_ISKEYPRESSED;
  }
};

// Clase CWOGetAreaNameInstr
class CWOGetAreaNameInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_GETAREANAME;
  }
};

// Clase CWOGetAreaIDInstr
class CWOGetAreaIDInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_GETAREAID;
  }
};

// Clase CWOGetAreaWidthInstr
class CWOGetAreaWidthInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_GETAREAWIDTH;
  }
};

// Clase CWOGetAreaHeightInstr
class CWOGetAreaHeightInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_GETAREAHEIGHT;
  }
};

// Clase CWOGetHourInstr
class CWOGetHourInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_GETHOUR;
  }
};

// Clase CWOGetMinuteInstr
class CWOGetMinuteInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_GETMINUTE;
  }
};

// Clase CWOSetHourInstr
class CWOSetHourInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_SETHOUR;
  }
};

// Clase CWOSetMinuteInstr
class CWOSetMinuteInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_SETMINUTE;
  }
};

// Clase CWOGetEntityInstr
class CWOGetEntityInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_GETENTITY;
  }
};

// Clase CWOGetPlayerInstr
class CWOGetPlayerInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_GETPLAYER;
  }
};

// Clase CWOIsFloorValidInstr
class CWOIsFloorValidInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_ISFLOORVALID;
  }
};

// Clase CWOGetItemAtInstr
class CWOGetItemAtInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_GETITEMAT;
  }
};

// Clase CWOGetNumItemsAtInstr
class CWOGetNumItemsAtInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_GETNUMITEMSAT;
  }
};

// Clase CWOGetDistanceInstr
class CWOGetDistanceInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_GETDISTANCE;
  }
};

// Clase CWOCalculePathLenghtInstr
class CWOCalculePathLenghtInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_CALCULEPATHLENGHT;
  }
};

// Clase CWOLoadAreaInstr
class CWOLoadAreaInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_LOADAREA;
  }
};

// Clase CWOChangeEntityLocationInstr
class CWOChangeEntityLocationInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_CHANGEENTITYLOCATION;
  }
};

// Clase CWOAttachCameraToEntityInstr
class CWOAttachCameraToEntityInstr: public CScriptInstruction,
									public iCCommandClient
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Constantes
  // Notificacion del final del travelling
  const CommandDefs::IDCommand IDEndTravelling;

private:
  // Vbles de miembro
  iCScript* m_pScript; // Script asociado cuando se espere por el travelling

public:
  // Constructor / Destructor
  CWOAttachCameraToEntityInstr(void): IDEndTravelling(1),
									  m_pScript(NULL) { }
  ~CWOAttachCameraToEntityInstr(void) {	
	End();
  }
private:
  // Metodos de apoyo
  void End(void);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // iCComandClient / Operacion de notificacion, para cuando acabe un comando
  void EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
					const dword udInstant,
					const dword udExtraParam);  

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_ATTACHCAMERATOENTITY;
  }
};

// Clase CWOAttachCameraToLocationInstr
class CWOAttachCameraToLocationInstr: public CScriptInstruction,
									  public iCCommandClient
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Constantes
  // Notificacion del final del travelling
  const CommandDefs::IDCommand IDEndTravelling;

private:
  // Vbles de miembro
  iCScript* m_pScript; // Script asociado cuando se espere por el travelling

public:
  // Constructor
  CWOAttachCameraToLocationInstr(void): IDEndTravelling(1),
										m_pScript(NULL) { }
  ~CWOAttachCameraToLocationInstr(void) {	
	End();
  }
private:
  // Metodos de apoyo
  void End(void);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // iCComandClient / Operacion de notificacion, para cuando acabe un comando
  void EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
					const dword udInstant,
					const dword udExtraParam);  

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_ATTACHCAMERATOLOCATION;
  }
};

// Clase CWOIsCombatModeActiveInstr
class CWOIsCombatModeActiveInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // iCComandClient / Operacion de notificacion, para cuando acabe un comando
  void EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
					const dword udInstant,
					const dword udExtraParam);  
public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_ISCOMBATMODEACTIVE;
  }
};

// Clase CWOEndCombatInstr
class CWOEndCombatInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_ENDCOMBAT;
  }
};

// Clase CWOGetCriatureInCombatTurnInstr
class CWOGetCriatureInCombatTurnInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_GETCRIATUREINCOMBATTURN;
  }
};

// Clase CWOGetCombatantInstr
class CWOGetCombatantInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_GETCOMBATANT;
  }
};

// Clase CWOGetNumberOfCombatantsInstr
class CWOGetNumberOfCombatantsInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_GETNUMBEROFCOMBATANTS;
  }
};

// Clase CWOGetAreaLightModelInstr
class CWOGetAreaLightModelInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_GETAREALIGHTMODEL;
  }
};

// Clase CWOSetIdleScriptTimeInstr
class CWOSetIdleScriptTimeInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_SETIDLESCRIPTTIME;
  }
};

// Clase CWOSetScriptInstr
class CWOSetScriptInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_SETSCRIPT;
  }
};

// Clase CWODestroyEntityInstr
class CWODestroyEntityInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_DESTROYENTITY;
  }
};

// Clase CWOCreateCriatureInstr
class CWOCreateCriatureInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_CREATECRIATURE;
  }
};

// Clase CWOCreateWallInstr
class CWOCreateWallInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_CREATEWALL;
  }
};

// Clase CWOCreateScenaryObjectInstr
class CWOCreateScenaryObjectInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_CREATESCENARYOBJECT;
  }
};

// Clase CWOCreateItemAbandonedInstr
class CWOCreateItemAbandonedInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_CREATEITEMABANDONED;
  }
};
	
// Clase CWOCreateItemWithOwnerInstr
class CWOCreateItemWithOwnerInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_CREATEITEMWITHOWNER;
  }
};

// Clase CWOSetWorldTimePause
class CWOSetWorldTimePauseInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_SETWORLDTIMEPAUSE;
  }
};

// Clase CWOIsWorldTimeInPauseInstr
class CWOIsWorldTimeInPauseInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_ISWORLDTIMEINPAUSE;
  }
};

// Clase CWOGetElevationAtInstr
class CWOGetElevationAtInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_GETELEVATIONAT;
  }
};

// Clase CWOSetElevationAtInstr
class CWOSetElevationAtInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_SETELEVATIONAT;
  }
};

// Clase CWONextTurnInstr
class CWONextTurnInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_NEXTTURN;
  }
};

// Clase CWOGetLightAtInstr
class CWOGetLightAtInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_GETLIGHTAT;
  }
};

// Clase CWOPlayWAVSoundInstr
class CWOPlayWAVSoundInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_PLAYWAVSOUND;
  }
};

// Clase CWOSetScriptAtInstr
class CWOSetScriptAtInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_WORLDOBJ_SETSCRIPTAT;
  }
};

// Clase CEOGetNameInstr
class CEOGetNameInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETNAME;
  }
};

// Clase CEOSetNameInstr
class CEOSetNameInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETNAME;
  }
};

// Clase CEOGetEntityTypeInstr
class CEOGetEntityTypeInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETENTITYTYPE;
  }
};

// Clase CEOGetTypeInstr
class CEOGetTypeInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETTYPE;
  }
};

// Clase CEOSayInstr
class CEOSayInstr: public CScriptInstruction,
				   public iCAlarmClient,
				   public iCWorldObserver
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Variables de miembro
  iCScript*           m_pScript; // Instancia al script asociado
  AreaDefs::EntHandle m_hEntity; // Handle a la entidad que habla

public:
  // Constructor / Destructor
  CEOSayInstr(void): m_pScript(NULL),
					 m_hEntity(0) { }
  ~CEOSayInstr(void) {
	End();
  }
private:
  // Metodos de apoyo
  void End(void);
  void ReleaseInfo(void);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // iCAlarmClient / Notificacion de alarmas
  void AlarmNotify(const AlarmDefs::eAlarmType& AlarmType,
				   const AlarmDefs::AlarmHandle& hAlarm);

public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SAY;
  }
};

// Clase CEOShutUpInstr
class CEOShutUpInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SHUTUP;
  }
};

// Clase CEOIsSayingInstr
class CEOIsSayingInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_ISSAYING;
  }
};

// Clase CEOAttachGFXInstr
class CEOAttachGFXInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_ATTACHGFX;
  }
};

// Clase CEOReleaseGFXInstr
class CEOReleaseGFXInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_RELEASEGFX;
  }
};

// Clase CEOReleaseAllGFXInstr
class CEOReleaseAllGFXInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_RELEASEALLGFX;
  }
};

// Clase CEOIsGFXAttachedInstr
class CEOIsGFXAttachedInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_ISGFXATTACHED;
  }
};

// Clase CEOGetNumItemsInContainerInstr
class CEOGetNumItemsInContainerInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETNUMITEMSINCONTAINER;
  }
};

// Clase CEOGetItemFromContainerInstr
class CEOGetItemFromContainerInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETITEMFROMCONTAINER;
  }
};

// Clase CEOIsItemInContainerInstr
class CEOIsItemInContainerInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_ISITEMINCONTAINER;
  }
};

// Clase CEOTransferItemToContainerInstr
class CEOTransferItemToContainerInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_TRANSFERITEMTOCONTAINER;
  }
};

// Clase CEOInsertItemInContainerInstr
class CEOInsertItemInContainerInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_INSERTITEMINCONTAINER;
  }
};

// Clase CEORemoveItemOfContainerInstr
class CEORemoveItemOfContainerInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_REMOVEITEMOFCONTAINER;
  }
};

// Clase CEOSetAnimTemplateStateInstr
class CEOSetAnimTemplateStateInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETANIMTEMPLATESTATE;
  }
};

// Clase CEOSetPortraitAnimTemplateStateInstr
class CEOSetPortraitAnimTemplateStateInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETPORTRAITANIMTEMPLATESTATE;
  }
};

// Clase CEOSetIdleScriptTimeInstr
class CEOSetIdleScriptTimeInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETIDLESCRIPTTIME;
  }
};

// Clase CEOSetLightInstr
class CEOSetLightInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETLIGHT;
  }
};

// Clase CEOGetLightInstr
class CEOGetLightInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETLIGHT;
  }
};

// Clase CEOGetXPos
class CEOGetXPosInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETXPOS;
  }
};

// Clase CEOGetYPos
class CEOGetYPosInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETYPOS;
  }
};

// Clase CEOGetElevation
class CEOGetElevationInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETELEVATION;
  }
};

// Clase CEOSetElevation
class CEOSetElevationInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETELEVATION;
  }
};

// Clase CEOGetLocalAttributeInstr
class CEOGetLocalAttributeInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETLOCALATTRIBUTE;
  }
};

// Clase CEOSetLocalAttributeInstr
class CEOSetLocalAttributeInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETLOCALATTRIBUTE;
  }
};

// Clase CEOGetOwnerInstr
class CEOGetOwnerInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETOWNER;
  }
};

// Clase CEOGetClassInstr
class CEOGetClassInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETCLASS;
  }
};

// Clase CEOGetInCombatUseCost
class CEOGetInCombatUseCostInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETINCOMBATUSECOST;
  }
};

// Clase CEOGetGlobalAttributeInstr
class CEOGetGlobalAttributeInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETGLOBALATTRIBUTE;
  }
};

// Clase CEOSetGlobalAttributeInstr
class CEOSetGlobalAttributeInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETGLOBALATTRIBUTE;
  }
};

// Clase CEOGetWallOrientationInstr
class CEOGetWallOrientationInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETWALLORIENTATION;
  }
};

// Clase CEOBlockAccessInstr
class CEOBlockAccessInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_BLOCKACCESS;
  }
};

// Clase CEOUnblockAccessInstr
class CEOUnblockAccessInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_UNBLOCKACCESS;
  }
};

// Clase CEOIsAccessBlockedInstr
class CEOIsAccessBlockedInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_ISACCESSBLOCKED;
  }
};

// Clase CEOSetSymptomInstr
class CEOSetSymptomInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETSYMPTOM;
  }
};

// Clase CEOIsSymptomActiveInstr
class CEOIsSymptomActiveInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_ISSYMPTOMACTIVE;
  }
};

// Clase CEOGetGenreInstr
class CEOGetGenreInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETGENRE;
  }
};

// Clase CEOGetHealthInstr
class CEOGetHealthInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETHEALTH;
  }
};

// Clase CEOSetHealthInstr
class CEOSetHealthInstr: public CScriptInstruction,
						 public iCCriatureObserver,
						 public iCWorldObserver
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Variables de miembro
  iCScript*           m_pScript; // Instancia al script asociado
  AreaDefs::EntHandle m_hEntity; // Entidad

public:
  // Constructor
  CEOSetHealthInstr(void): m_pScript(NULL),
							 m_hEntity(0) { }
  ~CEOSetHealthInstr(void) {
	End();
  }
private:
  // Metodos de apoyo
  void End(void);  
  void ReleaseInfo(void);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // iCCriatureObserver / Operacion de notificacion
  void CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
							  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
						      const dword udParam = 0);

public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETHEALTH;
  }
};

// Clase CEOGetExtendedAttributeInstr
class CEOGetExtendedAttributeInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETEXTENDEDATTRIBUTE;
  }
};

// Clase CEOSetExtendedAttributeInstr
class CEOSetExtendedAttributeInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETEXTENDEDATTRIBUTE;
  }
};

// Clase CEOGetLevelInstr
class CEOGetLevelInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETLEVEL;
  }
};

// Clase CEOSetLevelInstr
class CEOSetLevelInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETLEVEL;
  }
};

// Clase CEOGetExperienceInstr
class CEOGetExperienceInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETEXPERIENCE;
  }
};

// Clase CEOSetExperienceInstr
class CEOSetExperienceInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETEXPERIENCE;
  }
};

// Clase CEOGetActionPointsInstr
class CEOGetActionPointsInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETACTIONPOINTS;
  }
};

// Clase CEOGetInCombatActionPointsInstr
class CEOGetInCombatActionPointsInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETINCOMBATACTIONPOINTS;
  }
};

// Clase CEOSetActionPointsInstr
class CEOSetActionPointsInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETACTIONPOINTS;
  }
};

// Clase CEOIsHabilityActiveInstr
class CEOIsHabilityActiveInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_ISHABILITYACTIVE;
  }
};

// Clase CEOSetHabilityInstr
class CEOSetHabilityInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETHABILITY;
  }
};

// Clase CEOUseHabilityInstr
class CEOUseHabilityInstr: public CScriptInstruction,
						   public iCCriatureObserver,
						   public iCWorldObserver
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Variables de miembro
  iCScript*           m_pScript; // Instancia al script asociado
  AreaDefs::EntHandle m_hEntity; // Entidad

public:
  // Constructor
  CEOUseHabilityInstr(void): m_pScript(NULL),
							 m_hEntity(0) { }
  ~CEOUseHabilityInstr(void) {
	End();
  }
private:
  // Metodos de apoyo
  void End(void);
  void ReleaseInfo(void);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);
private:
  // funciones amigas de apoyo
  friend bool CheckForValidStateInUseAction(CCriature* const pCriature,
				                            CWorldEntity* const pTarget);
public:
  // Operacion de notificacion
  void CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
							  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
						      const dword udParam = 0);
public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_USEHABILITY;
  }
};

// Clase CEOIsRunModeActiveInstr
class CEOIsRunModeActiveInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_ISRUNMODEACTIVE;
  }
};

// Clase CEOSetRunModeInstr
class CEOSetRunModeInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETRUNMODE;
  }
};

// Clase CEOMoveToInstr
class CEOMoveToInstr: public CScriptInstruction,
					  public iCCommandClient,
					  public iCCriatureObserver,
					  public iCWorldObserver
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Constantes
  const CommandDefs::IDCommand ID_MOVETOINSTRWALKOK;

private:
  // Variables de miembro
  iCScript*           m_pScript; // Instancia al script asociado
  AreaDefs::EntHandle m_hEntity; // Handle a la entidad

public:
  // Constructor / Destructor
  CEOMoveToInstr(void): ID_MOVETOINSTRWALKOK(1),
						m_pScript(NULL),
						m_hEntity(0) { }
  ~CEOMoveToInstr() {
	End();
  }
private:
  // Metodos de apoyo
  void End(void);
  void ReleaseInfo(void);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // iCCommandClient / Operacion de notificacion, para cuando acabe un comando
  void EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
					const dword udInstant,
					const dword udExtraParam);  

public:
  // Operacion de notificacion
  void CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
							  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
						      const dword udParam = 0);

public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_MOVETO;
  }
};

// Clase CEOIsMovingInstr
class CEOIsMovingInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_ISMOVING;
  }
};

// Clase CEOStopMovingInstr
class CEOStopMovingInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_STOPMOVING;
  }
};

// Clase CEOEquipItemInstr
class CEOEquipItemInstr: public CScriptInstruction,
					     public iCCriatureObserver,
						 public iCWorldObserver
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Variables de miembro
  iCScript*			  m_pScript; // Instancia al script asociado
  AreaDefs::EntHandle m_hEntity; // Handle a la entidad

public:
  // Constructor
  CEOEquipItemInstr(void): m_pScript(NULL),
						 m_hEntity(0) { }
  ~CEOEquipItemInstr(void) {
	End();
  }
private:
  // Metodos de apoyo
  void End(void);
  void ReleaseInfo(void);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operacion de notificacion
  void CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
							  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
						      const dword udParam = 0);
public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_EQUIPITEM;
  }
};

// Clase CEORemoveItemEquippedInstr
class CEORemoveItemEquippedInstr: public CScriptInstruction,
								  public iCCriatureObserver,
								  public iCWorldObserver
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Variables de miembro
  iCScript*			  m_pScript; // Instancia al script asociado
  AreaDefs::EntHandle m_hEntity; // Handle a la entidad

public:
  // Constructor
  CEORemoveItemEquippedInstr(void): m_pScript(NULL),
						            m_hEntity(0) { }
  ~CEORemoveItemEquippedInstr(void) {
	End();
  }
private:
  // Metodos de apoyo
  void End(void);
  void ReleaseInfo(void);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operacion de notificacion
  void CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
							  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
						      const dword udParam = 0);

public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_REMOVEITEMEQUIPPED;
  }
};

// Clase CEOGetItemEquippedInstr
class CEOGetItemEquippedInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETITEMEQUIPPED;
  }
};

// Clase CEOIsItemEquippedInstr
class CEOIsItemEquippedInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_ISITEMEQUIPPED;
  }
};

// Clase CEODropItemInstr
class CEODropItemInstr: public CScriptInstruction,
					    public iCCriatureObserver,
						public iCWorldObserver
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Variables de miembro
  iCScript*			  m_pScript; // Instancia al script asociado
  AreaDefs::EntHandle m_hEntity; // Handle a entidad

public:
  // Constructor
  CEODropItemInstr(void): m_pScript(NULL),
						  m_hEntity(0) { }
  ~CEODropItemInstr(void) {
	End();
  }
private:
  // Metodos de apoyo
  void End(void);
  void ReleaseInfo(void);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operacion de notificacion
  void CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
							  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
						      const dword udParam = 0);
public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_DROPITEM;
  }
};

// Clase CEOUseItemInstr
class CEOUseItemInstr: public CScriptInstruction,
					   public iCCriatureObserver,
					   public iCWorldObserver
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Variables de miembro
  iCScript*			  m_pScript; // Instancia al script asociado
  AreaDefs::EntHandle m_hEntity; // Handle a la entidad

public:
  // Constructor
  CEOUseItemInstr(void): m_pScript(NULL),
						 m_hEntity(0) { }
  ~CEOUseItemInstr(void) {
	End();
  }
private:
  // Metodos de apoyo
  void End(void);
  void ReleaseInfo(void);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);
private:
  // funciones amigas de apoyo
  friend bool CheckForValidStateInUseAction(CCriature* const pCriature,
				                            CWorldEntity* const pTarget);

public:
  // Operacion de notificacion
  void CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
							  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
						      const dword udParam = 0);

public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_USEITEM;
  }
};

// Clase CEOManipulateInstr
class CEOManipulateInstr: public CScriptInstruction,
					      public iCCriatureObserver,
						  public iCWorldObserver
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Variables de miembro
  iCScript*			  m_pScript; // Instancia al script asociado
  AreaDefs::EntHandle m_hEntity; // Handle a entidad

public:
  // Constructor / Destructor
  CEOManipulateInstr(void): m_pScript(NULL) { }
  ~CEOManipulateInstr(void) {
	End();
  }
private:
  // Metodos de apoyo
  void End(void);
  void ReleaseInfo(void);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);
private:
  // funciones amigas de apoyo
  friend bool CheckForValidStateInUseAction(CCriature* const pCriature,
				                            CWorldEntity* const pTarget);

public:
  // Operacion de notificacion
  void CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
							  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
						      const dword udParam = 0);


public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_MANIPULATE;
  }
};

// Clase CEOSetTransparentModeInstr
class CEOSetTransparentModeInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETTRANSPARENTMODE;
  }
};

// Clase CEOIsTranparentModeActiveInstr
class CEOIsTransparentModeActiveInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_ISTRANSPARENTMODEACTIVE;
  }
};

// Clase CEOChangeAnimOrientationInstr
class CEOChangeAnimOrientationInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_CHANGEANIMORIENTATION;
  }
};

// Clase CEOGetAnimOrientationInstr
class CEOGetAnimOrientationInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETANIMORIENTATION;
  }
};

// Clase CEOSetAlingmentInstr
class CEOSetAlingmentInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETALINGMENT;
  }
};

// Clase CEOSetAlingmentWithInstr
class CEOSetAlingmentWithInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETALINGMENTWITH;
  }
};

// Clase CEOSetAlingmentAgainstInstr
class CEOSetAlingmentAgainstInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETALINGMENTAGAINST;
  }
};

// Clase CEOGetAlingmentInstr
class CEOGetAlingmentInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETALINGMENT;
  }
};

// Clase CEOHitEntityInstr
class CEOHitEntityInstr: public CScriptInstruction,
					     public iCCriatureObserver,
						 public iCWorldObserver
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

private:
  // Variables de miembro
  iCScript*			  m_pScript; // Instancia al script asociado
  AreaDefs::EntHandle m_hEntity; // Handle a entidad

public:
  // Constructor
  CEOHitEntityInstr(void): m_pScript(NULL) { }
  ~CEOHitEntityInstr(void) {
	End();
  }
private:
  // Metodos de apoyo
  void End(void);
  void ReleaseInfo(void);

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operacion de notificacion
  void CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
							  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
						      const dword udParam = 0);

public:
  // iCWorldObserver / Operacion de notificacion
  void WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
						   const dword udParam);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_HITENTITY;
  }
};

// Clase CEOSetScriptInstr
class CEOSetScriptInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETSCRIPT;
  }
};

// Clase CEOIsGhostMoveModeActiveInstr
class CEOIsGhostMoveModeActiveInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_ISGHOSTMOVEMODEACTIVE;
  }
};

// Clase CEOSetGhostMoveModeInstr
class CEOSetGhostMoveModeInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_SETGHOSTMOVEMODE;
  }
};

// Clase CEOGetRangeInstr
class CEOGetRangeInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_GETRANGE;
  }
};

// Clase CEOIsInRangeInstr
class CEOIsInRangeInstr: public CScriptInstruction
{
public:
  // Tipos
  // Clase base
  typedef CScriptInstruction Inherited;

public:
  // Ejecucion
  void Execute(iCScript* const pScript);

public:
  // Operaciones de consulta
  ScriptDefs::eScriptInstruction GetScriptInstruction(void) const {
	ASSERT(Inherited::IsInitOk());
	// Retorna codigo	
	return ScriptDefs::SI_ENTITYOBJ_ISINRANGE;
  }
};

#endif

