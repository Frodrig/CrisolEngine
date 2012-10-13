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
// CScriptInstructions.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CScriptInstructions.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CScriptInstructions.h"

#include "SYSEngine.h"
#include "iCLogger.h"
#include "iCGameDataBase.h"
#include "iCRulesDataBase.h"
#include "iCScript.h"
#include "iCMathUtil.h"
#include "iCGUIManager.h"
#include "iCCombatSystem.h"
#include "iCWorld.h"
#include "iCAlarmManager.h"
#include "CWorldEntity.h"
#include "CItem.h"
#include "CCriature.h"
#include "CPlayer.h"
#include "CWall.h"
#include "CFloor.h"
#include "CRoof.h"
#include "CSceneObj.h"
#include "CSprite.h"
#include "CItemContainerIt.h"
#include "iCItemContainer.h"
#include "iCEquipmentSlots.h"
#include "iCAudioSystem.h"
#include "iCGFXManager.h"
#include "iCFileSystem.h"
#include "CScriptStackValue.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia.
// Parametros:
// - hFile. Handle al fichero.
// - udOffset. Offset a utilizar dentro del fichero.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CScriptInstruction::Init(const FileDefs::FileHandle& hFile,
				         dword& udOffset)
{
  // SOLO si parametros correctos
  ASSERT(hFile);

  // Se finaliza si se intenta reinicializar
  End();

  // Todo correcto
  m_bIsInitOk = true;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza inicializacion
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CScriptInstruction::End(void)
{
  // Finaliza si procede
  if (IsInitOk()) {
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_NOP no realizara accion alguna.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CNopInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_NNEG negara el valor numerico que haya en el tope de la
//   pila.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CNNegInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);
  
  // Toma el elemento del tope de la pila
  CScriptStackValue* const pValue = pScript->Pop();
  ASSERT(pValue);

  // Lo niega y lo vuelve a poner
  *pValue = -pValue->GetFloatValue();
  pScript->Push(pValue);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_NMUL multiplicara los dos valores Number que se hallen en
//   el tope de la pila y despositara el resultado
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CNMulInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pSecondValue = pScript->Pop();
  ASSERT(pSecondValue);
  CScriptStackValue* pFirstValue = pScript->Pop();
  ASSERT(pFirstValue);

  // Realiza la operacion, depositando el valor
  CScriptStackValue* const pResult = new CScriptStackValue((*pFirstValue) * (*pSecondValue));
  ASSERT(pResult);
  pScript->Push(pResult);

  // Borra params
  delete pSecondValue;
  delete pFirstValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_NADD sumara dos valores Number y depositara el resultado
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CNAddInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pSecondValue = pScript->Pop();
  ASSERT(pSecondValue);
  CScriptStackValue* pFirstValue = pScript->Pop();
  ASSERT(pFirstValue);

  // Realiza la operacion, depositando el valor
  CScriptStackValue* const pReturn = new CScriptStackValue((*pFirstValue) +  (*pSecondValue));
  ASSERT(pReturn);
  pScript->Push(pReturn);

  // Borra parametros
  delete pSecondValue;
  delete pFirstValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_NMOD hallara el modulo de dos Numbers y despositara el
//   resultado.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CNModInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pSecondValue = pScript->Pop();
  ASSERT(pSecondValue);
  CScriptStackValue* pFirstValue = pScript->Pop();
  ASSERT(pFirstValue);

  // Realiza la operacion, depositando el valor
  CScriptStackValue* const pResult = new CScriptStackValue((*pFirstValue) % (*pSecondValue)); 
  ASSERT(pResult);
  pScript->Push(pResult);

  // Borra parametros
  delete pSecondValue;
  delete pFirstValue;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_NDIV hallara la division entre dos Number y depositara
//   el resultado.
// - En caso de que se produzca una division por cero, se interrumpira la
//   ejecucion del script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CNDivInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pSecondValue = pScript->Pop();
  ASSERT(pSecondValue);
  CScriptStackValue* pFirstValue = pScript->Pop();
  ASSERT(pFirstValue);

  // ¿NO es una division entre cero?
  if (pSecondValue->GetFloatValue() != 0.0f) {
	// Realiza la operacion, depositando el valor
	CScriptStackValue* const pResult = new CScriptStackValue((*pFirstValue) / (*pSecondValue));
	ASSERT(pResult);
	pScript->Push(pResult);
  } else {
	// Si, interrumpe la ejecucion
	pScript->ErrorInterrupt();
  } 

  // Borra parametros
  delete pSecondValue;
  delete pFirstValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_NSUB hallara la resta entre dos Number y depositara
//   el resultado.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CNSubInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pSecondValue = pScript->Pop();
  ASSERT(pSecondValue);
  CScriptStackValue* pFirstValue = pScript->Pop();
  ASSERT(pFirstValue);

  // Realiza la operacion, depositando el valor
  CScriptStackValue* const pReturn = new CScriptStackValue((*pFirstValue) - (*pSecondValue));
  ASSERT(pReturn);
  pScript->Push(pReturn);

  // Borra los parametros
  delete pSecondValue;
  delete pFirstValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_SADD realizara la concatenacion de dos Strings y 
//   depositara el resultado en la pila
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CSAddInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pSecondValue = pScript->Pop();
  ASSERT(pSecondValue);
  CScriptStackValue* pFirstValue = pScript->Pop();
  ASSERT(pFirstValue);

  // Realiza la operacion, depositando el valor 
  CScriptStackValue* const pResult = new CScriptStackValue((*pFirstValue) + (*pSecondValue));
  ASSERT(pResult);
  pScript->Push(pResult);

  // Borra parametros
  delete pSecondValue;
  delete pFirstValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia, para ello tomara del disco el offset a donde
//   saltar cuando la logica JMP lo indique.
// Parametros:
// - hFile. Handle al fichero donde leer la informacion.
// - udOffset. Offset desde donde leer
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CBaseJmpInstr::Init(const FileDefs::FileHandle& hFile,
				    dword& udOffset)
{
  // SOLO si parametros correctos
  ASSERT(hFile);
  
  // Inicia clase base
  if (Inherited::Init(hFile, udOffset)) {
	// Lee informacion y retorna
	udOffset += SYSEngine::GetFileSystem()->Read(hFile, 
												 (sbyte *)(&m_udJmpOffset),
												 sizeof(dword),
												 udOffset);
	return true;
  }

  // Problemas
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_JMP realizara un salto incondicional a una nueva pos.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CJmpInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Realiza un salto incondicional
  pScript->SetCodePos(Inherited::GetJmpOffset());
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_JMP_FALSE realizara un salto en caso de que el tope de
//   la pila sea menor a 1. El tope de la pila sera un number.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CJmpFalseInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toma tope de la pila
  CScriptStackValue* pValue = pScript->Pop();
  ASSERT(pValue);

  // ¿Valor False?
  if (pValue->GetFloatValue() < 1.0f) {
	pScript->SetCodePos(Inherited::GetJmpOffset());
  }

  // Borra valor
  delete pValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_JMP_TRUE realizara un salto en caso de que el tope de
//   la pila sea mayor o igual a 1. El tope de la pila sera un number.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CJmpTrueInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toma tope de la pila
  CScriptStackValue* pValue = pScript->Pop();
  ASSERT(pValue);

  // ¿Valor True?
  if (pValue->GetFloatValue() >= 1.0f) {
	pScript->SetCodePos(Inherited::GetJmpOffset());
  }

  // Borra valor
  delete pValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_NJMP_EQ realizara un salto si los dos Numbers en el tope
//   de la pila son iguales
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CNJmpEQInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman parametros
  CScriptStackValue* pSecondValue = pScript->Pop();
  ASSERT(pSecondValue);
  CScriptStackValue* pFirstValue = pScript->Pop();
  ASSERT(pFirstValue);

  // ¿Iguales?
  if ((*pFirstValue) == (*pSecondValue)) {
	pScript->SetCodePos(Inherited::GetJmpOffset());
  }

  // Borra valores
  delete pFirstValue;
  delete pSecondValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_NJMP_NE realizara un salto si los dos Numbers en el tope
//   de la pila son distintos
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CNJmpNEInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman parametros
  CScriptStackValue* pSecondValue = pScript->Pop();
  ASSERT(pSecondValue);
  CScriptStackValue* pFirstValue = pScript->Pop();
  ASSERT(pFirstValue);

  // ¿Distintos?
  if ((*pFirstValue) != (*pSecondValue)) {
	pScript->SetCodePos(Inherited::GetJmpOffset());
  }

  // Borra valores
  delete pFirstValue;
  delete pSecondValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_NJMP_GE realizara un salto si el primer Number de la 
//   pila es mayor o igual que el siguiente.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CNJmpGEInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman parametros
  CScriptStackValue* pSecondValue = pScript->Pop();
  ASSERT(pSecondValue);
  CScriptStackValue* pFirstValue = pScript->Pop();
  ASSERT(pFirstValue);

  // ¿Mayor o igual?
  if ((*pFirstValue) >= (*pSecondValue)) {
	pScript->SetCodePos(Inherited::GetJmpOffset());
  }

  // Borra valores
  delete pFirstValue;
  delete pSecondValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_NJMP_GT realizara un salto si el primer Number de la 
//   pila es mayor que el siguiente.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CNJmpGTInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman parametros
  CScriptStackValue* pSecondValue = pScript->Pop();
  ASSERT(pSecondValue);
  CScriptStackValue* pFirstValue = pScript->Pop();
  ASSERT(pFirstValue);

  // ¿Mayor?
  if ((*pFirstValue) > (*pSecondValue)) {
	pScript->SetCodePos(Inherited::GetJmpOffset());
  }

  // Borra valores
  delete pFirstValue;
  delete pSecondValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_NJMP_LT realizara un salto si el primer Number de la 
//   pila es menor que el siguiente.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CNJmpLTInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman parametros
  CScriptStackValue* pSecondValue = pScript->Pop();
  ASSERT(pSecondValue);
  CScriptStackValue* pFirstValue = pScript->Pop();
  ASSERT(pFirstValue);

  // ¿Menor?
  if ((*pFirstValue) < (*pSecondValue)) {
	pScript->SetCodePos(Inherited::GetJmpOffset());
  }

  // Borra valores
  delete pFirstValue;
  delete pSecondValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_NJMP_LE realizara un salto si el primer Number de la 
//   pila es menor o igual que el siguiente.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CNJmpLEInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman parametros
  CScriptStackValue* pSecondValue = pScript->Pop();
  ASSERT(pSecondValue);
  CScriptStackValue* pFirstValue = pScript->Pop();
  ASSERT(pFirstValue);

  // ¿Menor o igual?
  if ((*pFirstValue) <= (*pSecondValue)) {
	pScript->SetCodePos(Inherited::GetJmpOffset());
  }

  // Borra valores
  delete pFirstValue;
  delete pSecondValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_SJMP_EQ realizara un salto si los dos Strings que se
//   hallan en la pila son iguales. Se consideraran iguales independientemente
//   de si estan en mayusculas o minusculas.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CSJmpEQInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman parametros
  CScriptStackValue* pSecondValue = pScript->Pop();
  ASSERT(pSecondValue);
  CScriptStackValue* pFirstValue = pScript->Pop();
  ASSERT(pFirstValue);

  // ¿Iguales?
  if ((*pFirstValue) == (*pSecondValue)) {
	pScript->SetCodePos(Inherited::GetJmpOffset());
  }

  // Borra valores
  delete pFirstValue;
  delete pSecondValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_SJMP_NE realizara un salto si los dos Strings que se
//   hallan en la pila son distintos. Se consideraran iguales independientemente
//   de si estan en mayusculas o minusculas.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CSJmpNEInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman parametros
  CScriptStackValue* pSecondValue = pScript->Pop();
  ASSERT(pSecondValue);
  CScriptStackValue* pFirstValue = pScript->Pop();
  ASSERT(pFirstValue);

  // ¿Distintos?
  if ((*pFirstValue) != (*pSecondValue)) {
	pScript->SetCodePos(Inherited::GetJmpOffset());
  }

  // Borra valores
  delete pFirstValue;
  delete pSecondValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_EJMP_EQ realizara un salto si los dos Entity que se
//   hallan en la pila son iguales. Se consideraran iguales independientemente
//   de si estan en mayusculas o minusculas.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEJmpEQInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman parametros
  CScriptStackValue* pSecondValue = pScript->Pop();
  ASSERT(pSecondValue);
  CScriptStackValue* pFirstValue = pScript->Pop();
  ASSERT(pFirstValue);

  // ¿Iguales?
  if ((*pFirstValue) == (*pSecondValue)) {
	pScript->SetCodePos(Inherited::GetJmpOffset());
  }

  // Borra valores
  delete pFirstValue;
  delete pSecondValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_EJMP_NE realizara un salto si los dos Entity que se
//   hallan en la pila son distintos. Se consideraran iguales independientemente
//   de si estan en mayusculas o minusculas.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEJmpNEInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman parametros
  CScriptStackValue* pSecondValue = pScript->Pop();
  ASSERT(pSecondValue);
  CScriptStackValue* pFirstValue = pScript->Pop();
  ASSERT(pFirstValue);

  // ¿Distintos?
  if ((*pFirstValue) != (*pSecondValue)) {
	pScript->SetCodePos(Inherited::GetJmpOffset());
  }

  // Borra valores
  delete pFirstValue;
  delete pSecondValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_POP descartara el elemento en el tope de la pila
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CPopInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toma primer elemento y se libera
  delete pScript->Pop();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia
// Parametros:
// - hFile. Handle al fichero desde donde leer argumentos
// - udOffset. Offset desde donde comenzar a leer
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CBaseLoadInstr::Init(const FileDefs::FileHandle& hFile,
				     dword& udOffset)
{
  // SOLO si parametros validos
  ASSERT(hFile);

  // Inicializa clase base
  if (Inherited::Init(hFile, udOffset)) {
	// Lee informacion
	udOffset += SYSEngine::GetFileSystem()->Read(hFile,
												 (sbyte *)(&m_uwMemSlot),
												 sizeof(word),
												 udOffset);
	return true;
  }

  // Problemas
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_NLOAD toma el valor Number asociado al slot de mem. 
//   recibido y lo colocara en el tope de la pila.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CNLoadInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el valor asociado en el slot de memoria
  CScriptStackValue* pValue = pScript->GetValueAt(Inherited::GetMemSlot());
  ASSERT(pValue);
   
  // Coloca el valor en la pila
  CScriptStackValue* const pStackValue = new CScriptStackValue(*pValue);
  ASSERT(pStackValue);
  pScript->Push(pStackValue);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_SLOAD toma el valor String asociado al slot de mem. 
//   recibido y lo colocara en el tope de la pila.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CSLoadInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el valor asociado en el slot de memoria
  CScriptStackValue* const pValue = pScript->GetValueAt(Inherited::GetMemSlot());
  ASSERT(pValue);
  
  // Coloca el valor en la pila
  CScriptStackValue* const pStackValue = new CScriptStackValue(*pValue);
  ASSERT(pStackValue);  
  pScript->Push(pStackValue);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_ELOAD toma el valor Entity asociado al slot de mem. 
//   recibido y lo colocara en el tope de la pila.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CELoadInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el valor asociado en el slot de memoria
  CScriptStackValue* const pValue = pScript->GetValueAt(Inherited::GetMemSlot());
  ASSERT(pValue);

  // Coloca el valor en la pila
  CScriptStackValue* const pStackValue = new CScriptStackValue(*pValue);
  ASSERT(pStackValue);
  pScript->Push(pStackValue);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia
// Parametros:
// - hFile. Handle al fichero desde donde leer argumentos
// - udOffset. Offset desde donde comenzar a leer
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CBaseStoreInstr::Init(const FileDefs::FileHandle& hFile,
				      dword& udOffset)
{
  // SOLO si parametros validos
  ASSERT(hFile);

  // Inicializa clase base
  if (Inherited::Init(hFile, udOffset)) {
	// Lee informacion
	udOffset += SYSEngine::GetFileSystem()->Read(hFile,
												 (sbyte *)(&m_uwMemSlot),
												 sizeof(word),
												 udOffset);
	return true;
  }

  // Problemas
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_NSTORE toma el valor Number en el tope de la pila y lo
//   guardar en un slot de memoria.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CNStoreInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el valor de la pila
  CScriptStackValue* const pValue = pScript->Pop();
  ASSERT(pValue);

  // Lo coloca en el slot de memoria, liberando el nodo despues
  pScript->SetValueAt(Inherited::GetMemSlot(), pValue->GetFloatValue());
  delete pValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_SSTORE toma el valor String en el tope de la pila y lo
//   guardar en un slot de memoria.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CSStoreInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el valor de la pila
  CScriptStackValue* const pValue = pScript->Pop();
  ASSERT(pValue);

  // Lo coloca en el slot de memoria, liberando el nodo despues
  pScript->SetValueAt(Inherited::GetMemSlot(), pValue->GetStringValue());
  delete pValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_ESTORE toma el valor Entity en el tope de la pila y lo
//   guardar en un slot de memoria.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEStoreInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el valor de la pila
  CScriptStackValue* const pValue = pScript->Pop();
  ASSERT(pValue);

  // Lo coloca en el slot de memoria, liberando el nodo despues
  pScript->SetValueAt(Inherited::GetMemSlot(), AreaDefs::EntHandle(pValue->GetDWordValue()));
  delete pValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia
// Parametros:
// - hFile. Handle al fichero desde donde leer argumentos
// - udOffset. Offset desde donde comenzar a leer
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CNPushInstr::Init(const FileDefs::FileHandle& hFile,
				  dword& udOffset)
{
  // SOLO si parametros validos
  ASSERT(hFile);

  // Inicializa clase base
  if (Inherited::Init(hFile, udOffset)) {
	// Lee informacion
	udOffset += SYSEngine::GetFileSystem()->Read(hFile,
												 (sbyte *)(&m_fValue),
												 sizeof(float),
												 udOffset);
	return true;
  }

  // Problemas
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_NPUSH coloca un valor Number en el tope de la pila
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CNPushInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Crea un valor y lo coloca en el tope de la pila
  CScriptStackValue* const pValue = new CScriptStackValue(m_fValue);
  ASSERT(pValue);
  pScript->Push(pValue);
  pValue->GetFloatValue();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia
// Parametros:
// - hFile. Handle al fichero desde donde leer argumentos
// - udOffset. Offset desde donde comenzar a leer
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CSPushInstr::Init(const FileDefs::FileHandle& hFile,
				  dword& udOffset)
{
  // SOLO si parametros validos
  ASSERT(hFile);

  // Inicializa clase base
  if (Inherited::Init(hFile, udOffset)) {
	// Lee informacion
	udOffset += SYSEngine::GetFileSystem()->Read(hFile,
												 (sbyte *)(&m_udStrIdx),
												 sizeof(dword),
												 udOffset);
	return true;
  }

  // Problemas
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_SPUSH coloca un valor String en el tope de la pila
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CSPushInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Crea un valor y lo coloca en el tope de la pila  
  CScriptStackValue* const pValue = new CScriptStackValue(pScript->GetString(m_udStrIdx));
  ASSERT(pValue);
  pScript->Push(pValue);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia
// Parametros:
// - hFile. Handle al fichero desde donde leer argumentos
// - udOffset. Offset desde donde comenzar a leer
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CEPushInstr::Init(const FileDefs::FileHandle& hFile,
				  dword& udOffset)
{
  // SOLO si parametros validos
  ASSERT(hFile);

  // Inicializa clase base
  if (Inherited::Init(hFile, udOffset)) {
	// Lee informacion
	udOffset += SYSEngine::GetFileSystem()->Read(hFile,
												 (sbyte *)(&m_hValue),
												 sizeof(word),
												 udOffset);
	return true;
  }

  // Problemas
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_EPUSH coloca un valor Entity en el tope de la pila
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEPushInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Crea un valor y lo coloca en el tope de la pila
  CScriptStackValue* const pValue = new CScriptStackValue(dword(m_hValue));
  ASSERT(pValue);
  pScript->Push(pValue);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_NSCAST toma el valor Number que se halla en el tope de
//   la pila y lo convierte a un valor string. 
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CNSCastInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el valor number en el tope de la pila 
  CScriptStackValue* pValue = pScript->Pop();
  ASSERT(pValue);

  // Lo pasa a string
  std::string szValue;
  // ¿Hay parte decimal?
  const float fValue = pValue->GetFloatValue();
  const sdword sdValue = sdword(fValue);
  if ((fValue - sdValue) != 0.0f) {
	// Si, se pasa a formato decimal
	SYSEngine::PassToString(szValue, "%f", fValue);
  } else {
	// No, se pasa a formato entero
	SYSEngine::PassToString(szValue, "%ld", sdValue);
  }

  CScriptStackValue* const pStrValue = new CScriptStackValue(szValue);
  ASSERT(pStrValue);  

  // Pone en el tope de la pila el string y libera el valor numerico
  pScript->Push(pStrValue);
  delete pValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_SNCAST toma el valor String que se halla en el tope de
//   la pila y lo convierte a un valor Number. 
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CSNCastInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el valor string en el tope de la pila 
  CScriptStackValue* pValue = pScript->Pop();
  ASSERT(pValue);

  // Lo pasa a number
  const float fValue = atof(pValue->GetStringValue().c_str());
  CScriptStackValue* const pNumValue = new CScriptStackValue(fValue);
  ASSERT(pNumValue);  

  // Pone en el tope de la pila el number y libera el valor de texto
  pScript->Push(pNumValue);
  delete pValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_DUP duplica el valor que haya en tope de la pila
//   la pila
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CDupInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el number del tope de la pila
  CScriptStackValue* const pValue = pScript->Pop();
  ASSERT(pValue);

  // Se crea una replica
  CScriptStackValue* const pDupValue = new CScriptStackValue(*pValue);
  ASSERT(pDupValue);

  // Se insertan los dos valores
  pScript->Push(pValue);
  pScript->Push(pDupValue);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_RETURN retorna de una porcion de codigo en ejecucion,
//   para ello quitara el Stack Frame que se halle activo siempre y cuando,
//   se este trabajando con un script NO global. Si el script es global, el
//   mismo se detendra.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CReturnInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // ¿Es un script global?
  if (pScript->IsGlobalScript()) {
	// Si, se detiene
	pScript->Stop();
  } else {
	// No, quita el Stack Frame actual
	pScript->PopStackFrame();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia
// Parametros:
// - hFile. Handle al fichero desde donde leer argumentos
// - udOffset. Offset desde donde comenzar a leer
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCallFuncInstr::Init(const FileDefs::FileHandle& hFile,
					 dword& udOffset)
{
  // SOLO si parametros validos
  ASSERT(hFile);

  // Inicializa clase base
  if (Inherited::Init(hFile, udOffset)) {
	// Lee informacion
	udOffset += SYSEngine::GetFileSystem()->Read(hFile,
												 (sbyte *)(&m_uwCodeIdx),
												 sizeof(word),
												 udOffset);
	return true;
  }

  // Problemas
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - La instrucion SI_CALL_FUNC llama a una porcion de codigo (de funcion)
//   estableciendo el Stack Frame asociado al mismo.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CCallFuncInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Establece el Stack Frame desde donde llamar.  
  pScript->PushStackFrame(m_uwCodeIdx);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Instruccion del API para pasar tres componentes number de color (RGB) a
//   un solo componente y depositarlo en la lista.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAPIPassToRGBColorInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma los valores RGB
  CScriptStackValue* pBValue = pScript->Pop();
  ASSERT(pBValue);
  CScriptStackValue* pGValue = pScript->Pop();
  ASSERT(pGValue);
  CScriptStackValue* pRValue = pScript->Pop();
  ASSERT(pRValue);

  // Pasa a formato byte y construye el float para depositarlo despues
  const byte ubRValue = pRValue->GetFloatValue();
  const byte ubGValue = pGValue->GetFloatValue();
  const byte ubBValue = pBValue->GetFloatValue();
  const float fResult = (ubRValue << 16) + (ubGValue << 8) + ubBValue;
  CScriptStackValue* const pResult = new CScriptStackValue(fResult);
  ASSERT(pResult);
  pScript->Push(pResult); 

  // Borra parametros
  delete pBValue;
  delete pGValue;
  delete pRValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el componente rojo de un valor numerico de color RGB
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAPIGetRedComponentInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el valor RGB
  CScriptStackValue* pRGBValue = pScript->Pop();
  ASSERT(pRGBValue);

  // Extra el valor Red y lo deposita
  const byte ubRed = ((dword(pRGBValue->GetFloatValue()) & 0x00FF0000) >> 16);
  CScriptStackValue* const pResult = new CScriptStackValue(float(ubRed));
  ASSERT(pResult);
  pScript->Push(pResult);

  // Borra parametros
  delete pRGBValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el componente verde de un valor numero de codigo RGB
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAPIGetGreenComponentInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el valor RGB
  CScriptStackValue* pRGBValue = pScript->Pop();
  ASSERT(pRGBValue);

  // Extra el valor Green y lo deposita
  const byte ubGreen = ((dword(pRGBValue->GetFloatValue()) & 0x0000FF00) >> 8);
  CScriptStackValue* const pResult = new CScriptStackValue(float(ubGreen));
  ASSERT(pResult);
  pScript->Push(pResult);

  // Borra parametros
  delete pRGBValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el componente azul de un valor numero de codigo RGB
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAPIGetBlueComponentInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el valor RGB
  CScriptStackValue* pRGBValue = pScript->Pop();
  ASSERT(pRGBValue);

  // Extra el valor Blue y lo deposita
  const byte ubBlue = (dword(pRGBValue->GetFloatValue()) & 0x000000FF);
  CScriptStackValue* const pResult = new CScriptStackValue(float(ubBlue));
  ASSERT(pResult);
  pScript->Push(pResult);

  // Borra parametros
  delete pRGBValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Toma dos valores number de la pila que se corresponden con el valor
//   minimo y maximo y halla un valor aleatorio entre ellos, para depositarlo
//   despues en la pila.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAPIRandInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma los valores numericos
  CScriptStackValue* pMaxValue = pScript->Pop();
  CScriptStackValue* pMinValue = pScript->Pop();
  
  // ¿El valor menor es MENOR o IGUAL que el mayor?
  if (pMinValue->GetFloatValue() <= pMaxValue->GetFloatValue()) {
	// Si, se halla el valor aleatorio y lo deposita en la pila  
	iCMathUtil* const pMath = SYSEngine::GetMathUtil();
	ASSERT(pMath);
	const sword swValue = pMath->GetRandValue(pMinValue->GetFloatValue(), 
											  pMaxValue->GetFloatValue());
	CScriptStackValue* const pValue = new CScriptStackValue(float(swValue));
	ASSERT(pValue);
	pScript->Push(pValue);
  } else {
	// No, se interrumpe con error
	pScript->ErrorInterrupt();
  }

  // Elimina de mem. los parametros
  delete pMaxValue;
  delete pMinValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la parte entera de un valor numerico depositado en pila y luego
//   la deposita en pila
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAPIGetIntegerValueInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma valor numerico
  CScriptStackValue* pValue = pScript->Pop();
  ASSERT(pValue);

  // Toma la parte entera y deposita
  CScriptStackValue* const pIntValue = new CScriptStackValue(float(sdword(pValue->GetFloatValue())));
  ASSERT(pIntValue);
  pScript->Push(pIntValue);

  // Borra parametros
  delete pValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la parte decimal de un valor numerico y la deposita de nuevo
//   en la pila
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAPIGetDecimalValueInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma valor numerico
  CScriptStackValue* pValue = pScript->Pop();
  ASSERT(pValue);

  // Toma la parte entera y deposita
  CScriptStackValue* const pDecimalValue = new CScriptStackValue(float(pValue->GetFloatValue() - (sdword)(pValue->GetFloatValue())));
  ASSERT(pDecimalValue);
  pScript->Push(pDecimalValue);

  // Borra parametros
  delete pValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve el tamaño del string que se halle en el tope de la pila
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAPIGetStringSizeInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el string en el tope de la pila
  CScriptStackValue* pStr = pScript->Pop();
  ASSERT(pStr);

  // Crea un valor numerico con el tamaño de este y lo introduce
  CScriptStackValue* const pStrSize = new CScriptStackValue(float(pStr->GetStringValue().size()));
  ASSERT(pStrSize);
  pScript->Push(pStrSize);

  // Elimina el string
  delete pStr;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Escribe un texto al logger
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAPIWriteToLoggerInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el string
  CScriptStackValue* pMsg = pScript->Pop();
  ASSERT(pMsg);

  // Escribe
  SYSEngine::GetLogger()->Write("%s\n", pMsg->GetStringValue().c_str());

  // Elimina string
  delete pMsg;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el envio de mensajes de advertencia en la ejecucion de scripts.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAPIEnableCrisolScriptWarningsInstr::Execute(iCScript* const pScript)
{
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva el envio de mensajes de advertencia en la ejecucion de scripts
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAPIDisableCrisolScriptWarningsInstr::Execute(iCScript* const pScript)
{
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Muestra / oculta los FPS en pantalla
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAPIShowFPSInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pFlag = pScript->Pop();
  ASSERT(pFlag);

  // Segun sea el valor del flag muestra u oculta
  SYSEngine::ShowFPS(pFlag->GetFloatValue() < 1.0f ? false : true);

  // Elimina parametros
  delete pFlag;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instruccion, desinstalando la POSIBLE alarma asociada
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CAPIWaitInstr::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {
	// ¿Instruccion actual?
	if (m_pScript) {
	  // Desinstala alarma y desvincula como observer de CWorld
	  m_pScript = NULL;	  
	  SYSEngine::GetAlarmManager()->UninstallAlarm(m_hAlarm);
	  SYSEngine::GetWorld()->RemoveObserver(this);

	  // Desvincula resto de informacion
	  m_hAlarm = 0;
	}

	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Pausa el script tantos segundos como se reciban. En caso de recibir un
//   numero de segundos menor a 1, el script se detendra pues esa entrada no
//   sera considerada valida.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - La instruccion se instalara como observer de CWorld con el fin de
//   conocer una pausa en el universo de juego y asi pausarse.
// - En caso de ejecutar la instruccion estando el universo pausado, no
//   se dejara.
///////////////////////////////////////////////////////////////////////////////
void
CAPIWaitInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pSeconds = pScript->Pop();
  ASSERT(pSeconds);

  // ¿El universo NO esta en modo pausa?
  bool bResult = false;
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  if (!pWorld->IsInPause()) {
	// NO, se intenta instalar la instruccion como alarma y se pausa script
	m_hAlarm = SYSEngine::GetAlarmManager()->InstallTimeAlarm(this, pSeconds->GetFloatValue());
	if (m_hAlarm) {
	  // Todo correcto, se guarda script y se pausa
	  m_pScript = pScript;
	  pScript->Pause();
	  // Se instala como observer de World 	  
	  pWorld->AddObserver(this);
	  bResult = true;
	} 
  }

  // ¿Hubo problemas?
  if (!bResult) {
	// Si, se interrumpe
	pScript->ErrorInterrupt();	
  }
  
  // Elimina parametros
  delete pSeconds;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion del universo de juego. Se utilizara para conocer si este
//   entra en estado de pausa. Si esto fuera asi y la instruccion fuera la
//   actual, se pausara la alarma asociada y viceversa.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAPIWaitInstr::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
								   const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Instruccion actual
  if (m_pScript) {
	// Si, comprueba que la notificacion sea la buscada
	switch(NotifyType) {
	  case WorldObserverDefs::PAUSE_OFF: {
		// Se establece pausa
		SYSEngine::GetAlarmManager()->SetPause(m_hAlarm, false);
	  } break;

	  case WorldObserverDefs::PAUSE_ON: {	  
		// Se reanuda alarma
		SYSEngine::GetAlarmManager()->SetPause(m_hAlarm, true);
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se recibira la notificacion de que ha finalizado la alarma y bastara con
//   reanudar el script.
// Parametros:
// - AlarmType. Tipo de alarma.
// - hAlarm. Handle a la alarma
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CAPIWaitInstr::AlarmNotify(const AlarmDefs::eAlarmType& AlarmType,
						   const AlarmDefs::AlarmHandle& hAlarm)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hAlarm);

  // Se comprueba el tipo de alarma
  switch (AlarmType) {
	case AlarmDefs::TIME_ALARM: {
	  // Se reanuda el script
	  ASSERT((hAlarm == m_hAlarm) != 0);
	  ASSERT(m_pScript);
	  m_pScript->Resume();
	  m_pScript = NULL;	  
	  m_hAlarm = 0;

	  // Se desvincula como observer de CWorld
	  SYSEngine::GetWorld()->RemoveObserver(this);
	} break;

	default:
	  ASSERT(false);
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Instruccion asociada a la accion de quitar el juego de forma inmediata y
//   regresar a la ventana de menu principal. El proceso para quitar el juego
//   consistira en pausar en primer lugar el universo de juego, ocultando el
//   cursor del raton. Acto seguido, se realizara un FadeOut a negro y a su
//   finalizacion, se cambiara de estado a MainState.
// - Solo se aceptara una llamada desde el estado de MainInterfaz o de
//   PlayerProfile, en caso contrario se producira una interrupcion por error.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOQuitGameInstr::Execute(iCScript* const pScript)
{  
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);
  
  // Se esta desde un estado valido
  iCGUIManager* const pGUIManager = SYSEngine::GetGUIManager();
  ASSERT(pGUIManager);
  if (GUIManagerDefs::GUIW_MAININTERFAZ == pGUIManager->GetGameGUIWindowState() ||
  	  GUIManagerDefs::GUIW_PLAYERPROFILE == pGUIManager->GetGameGUIWindowState()) {
	// Si, se realiza un FadeOut, ocultando el cursor del raton
	pGUIManager->SetGUICursor(GUIManagerDefs::NO_CURSOR);
	iCGFXManager* const pGFXManager = SYSEngine::GetGFXManager();
	ASSERT(pGFXManager);	
	pGFXManager->SetFadeSpeed(128);
	pGFXManager->SetFadeOutColor(GraphDefs::sRGBColor(0, 0, 0));
	if (pGFXManager->DoFade(GFXManagerDefs::FADE_OUT, 
							false, 
							this, 
							CGOQuitGameInstr::IDFadeOut)) {
	  // Se guarda instancia al script y se pausa a su vez
	  pScript->Pause();
	  m_pScript = pScript;  
	} 
  } else {
	// No, se interrumpe el script
	pScript->ErrorInterrupt();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Este metodo se activara cuando el FadeOut 
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGOQuitGameInstr::EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
							   const dword udInstant,
							   const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se comprueba el identificador de comando
  if (IDCommand == CGOQuitGameInstr::IDFadeOut) {
	// Se solicita el cambio al estado de menu principal y se reactiva script
	SYSEngine::SetMainMenuState();
	m_pScript->Resume();
	m_pScript = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Escribe un mensaje a la consola.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOWriteToConsoleInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma string
  CScriptStackValue* pMsg = pScript->Pop();
  ASSERT(pMsg);

  // Escribe el mensaje
  SYSEngine::GetGUIManager()->WriteToConsole(pMsg->GetStringValue(), true);
    
  // Borra string
  delete pMsg;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGOActiveAdviceDialogInstr::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {  
	// ¿Es la instruccion actual?
	if (m_pScript) {
	  // Ordena la desactivacion de la interfaz
	  m_pScript = NULL;	  
	  SYSEngine::GetGUIManager()->DeactiveAdviceDialog();
	}

	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el cuadro de dialogo de aviso
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOActiveAdviceDialogInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma string
  CScriptStackValue* pMsg = pScript->Pop();
  ASSERT(pMsg);

  // Activa cuadro de dialogo y establece el script en modo pausa
  if (SYSEngine::GetGUIManager()->ActiveAdviceDialog(pMsg->GetStringValue(),
												     this)) {
	pScript->Pause();
	m_pScript = pScript;
  } else {
	pScript->ErrorInterrupt();
  }
  
  // Borra string
  delete pMsg;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion del cuadro de dialogo de aviso
// Parametros:
// - IDGUIWindow. Identificador de la ventana.
// - udParams. Parametros recibidos.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGOActiveAdviceDialogInstr::GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
											const dword udParams)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Comprueba la notifiacion
  switch(IDGUIWindow) {
	case GUIManagerDefs::GUIW_ADVICEDIALOG: {
	  // Se cerro el interfaz de aviso, se quita la pausa del script
	  ASSERT(m_pScript);
	  m_pScript->Resume();
	  m_pScript = NULL;
	} break;

	default:
	  ASSERT(false);
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGOActiveQuestionDialogInstr::End(void)
{  
  // Finaliza
  if (Inherited::IsInitOk()) {  
	// ¿Es la instruccion actual?
	if (m_pScript) {
	  // Ordena la desactivacion de la interfaz
	  m_pScript = NULL;	  
	  SYSEngine::GetGUIManager()->DeactiveQuestionDialog();
	}

	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el cuadro de dialogo de pregunta
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOActiveQuestionDialogInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pUseCancelFlag = pScript->Pop();
  ASSERT(pUseCancelFlag);
  CScriptStackValue* pMsg = pScript->Pop();
  ASSERT(pMsg);
  
  // Activa cuadro de dialogo y establece el script en modo pausa
  if (SYSEngine::GetGUIManager()->ActiveQuestionDialog(pMsg->GetStringValue(),
													   (pUseCancelFlag->GetFloatValue() < 1.0f) ? false : true,
													   this)) {
	pScript->Pause();
	m_pScript = pScript;
  } else {
	pScript->ErrorInterrupt();
  }
  
  // Borra parametros
  delete pMsg;
  delete pUseCancelFlag;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion del cuadro de dialogo de pregunta
// Parametros:
// - IDGUIWindow. Identificador de la ventana.
// - udParams. Parametros recibidos.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGOActiveQuestionDialogInstr::GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
											  const dword udParams)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Comprueba la notifiacion
  switch(IDGUIWindow) {
	case GUIManagerDefs::GUIW_QUESTIONDIALOG: {
	  // Se cerro el interfaz de pregunta, se toma valor retornado y se 
	  // inserta en la pila, quitando antes la pausa del script
	  ASSERT(m_pScript);
	  m_pScript->Resume();
	  CScriptStackValue* const pResult = new CScriptStackValue(float(udParams));
	  ASSERT(pResult);
	  m_pScript->Push(pResult);
	  m_pScript = NULL;	  
	} break;

	default:
	  ASSERT(false);
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGOActiveTextReaderDialogInstr::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {  
	// ¿Es la instruccion actual?
	if (m_pScript) {
	  // Ordena la desactivacion de la interfaz
	  m_pScript = NULL;	  
	  SYSEngine::GetGUIManager()->DeactiveTextReader();
	}

	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el cuadro de dialogo para la lectura de ficheros de texto
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOActiveTextReaderDialogInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pFileName = pScript->Pop();
  ASSERT(pFileName);
  CScriptStackValue* pTitle = pScript->Pop();
  ASSERT(pTitle);
  
  // Activa cuadro de dialogo y establece el script en modo pausa
  if (SYSEngine::GetGUIManager()->ActiveTextReader(pTitle->GetStringValue(),
												   pFileName->GetStringValue(),
												   this)) {
	pScript->Pause();
	m_pScript = pScript;
  } else {
	pScript->ErrorInterrupt();
  }
  
  // Borra parametros
  delete pFileName;
  delete pTitle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion del cuadro de dialogo de lectura de archivos de texto
// Parametros:
// - IDGUIWindow. Identificador de la ventana.
// - udParams. Parametros recibidos.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGOActiveTextReaderDialogInstr::GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
											    const dword udParams)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Comprueba la notifiacion
  switch(IDGUIWindow) {
	case GUIManagerDefs::GUIW_TEXTREADER: {
	  // Se cerro el cuadro de lectura, se reactiva el script
	  ASSERT(m_pScript);
	  m_pScript->Resume();	  
	  m_pScript = NULL;	  
	} break;

	default:
	  ASSERT(false);
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Añade una opcion al cuadro de dialogo de seleccion de opciones
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOAddOptionToTextSelectorDialogInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pCanSelectFlag = pScript->Pop();
  ASSERT(pCanSelectFlag);
  CScriptStackValue* pText = pScript->Pop();
  ASSERT(pText);
  CScriptStackValue* pIDOption = pScript->Pop();
  ASSERT(pIDOption);
  
  // Activa cuadro de dialogo y se postea el resultado
  CScriptStackValue* pResult;
  if (SYSEngine::GetGUIManager()->AddOptionToTextOptionsSelector(GUIDefs::GUIIDComponent(pIDOption->GetFloatValue()),
															     pText->GetStringValue(),
														         (pCanSelectFlag->GetFloatValue() < 1.0f) ? false : true)) {
	pResult = new CScriptStackValue(1.0f);
  } else {	
	pResult = new CScriptStackValue(0.0f);
  }
  ASSERT(pResult);
  pScript->Push(pResult);
  
  // Borra parametros
  delete pIDOption;
  delete pText;
  delete pCanSelectFlag;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Resetea las opciones añadidas al cuadro de dialogo selector de texto
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOResetOptionsInTextSelectorDialogInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Reseta las opciones registradas en el cuadro de dialogo
  SYSEngine::GetGUIManager()->ReleaseTextOptionsSelector();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGOActiveTextSelectorDialogInstr::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {  
	// ¿Es la instruccion actual?
	if (m_pScript) {
	  // Ordena la desactivacion de la interfaz
	  m_pScript = NULL;	  
	  SYSEngine::GetGUIManager()->DeactiveTextOptionsSelector();
	}

	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el cuadro de dialogo del selector de texto
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOActiveTextSelectorDialogInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pUseCancelFlag = pScript->Pop();
  ASSERT(pUseCancelFlag);
  CScriptStackValue* pTitle = pScript->Pop();
  ASSERT(pTitle);
  
  // Activa cuadro de dialogo y establece el script en modo pausa
  if (SYSEngine::GetGUIManager()->ActiveTextOptionsSelector(pTitle->GetStringValue(),
														    (pUseCancelFlag->GetFloatValue() < 1.0f) ? false : true,
														    this)) {
	pScript->Pause();
	m_pScript = pScript;
  } else {
	pScript->ErrorInterrupt();
  }
  
  // Borra parametros
  delete pUseCancelFlag;
  delete pTitle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion del cuadro de dialogo de lectura de archivos de texto
// Parametros:
// - IDGUIWindow. Identificador de la ventana.
// - udParams. Parametros recibidos.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGOActiveTextSelectorDialogInstr::GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
											      const dword udParams)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Comprueba la notifiacion
  switch(IDGUIWindow) {
	case GUIManagerDefs::GUIW_GENERICTEXTSELECTOR: {	  
	  // Se cerro el cuadro de dialogo de seleccion de texto, se reactiva el
	  // script y se devuelve el resultado.
	  ASSERT(m_pScript);
	  m_pScript->Resume();
	  CScriptStackValue* const pResult = new CScriptStackValue(float(udParams));
	  ASSERT(pResult);
	  m_pScript->Push(pResult);
	  m_pScript = NULL;	  
	} break;

	default:
	  ASSERT(false);
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa una musica MIDI.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOPlayMidiMusicInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros (nombre archivo y flag de reproduccion)
  CScriptStackValue* pLoopFlag = pScript->Pop();
  ASSERT(pLoopFlag);
  CScriptStackValue* pMIDIFile = pScript->Pop();
  ASSERT(pMIDIFile);
  
  // Reproduce fichero MIDI segun sea el flag
  if (pLoopFlag->GetFloatValue() < 1.0f) {
	// Sin Loop
	SYSEngine::GetAudioSystem()->PlayMIDIMusic(pMIDIFile->GetStringValue(),
											   AudioDefs::MIDI_PLAY_NORMAL);
  } else {
	// Con Loop
	SYSEngine::GetAudioSystem()->PlayMIDIMusic(pMIDIFile->GetStringValue(),
											   AudioDefs::MIDI_PLAY_LOOP);
  }

  // Borra parametros
  delete pMIDIFile;
  delete pLoopFlag;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Detiene fichero MIDI en reproduccion
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOStopMidiMusicInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Detiene fichero MIDI
  SYSEngine::GetAudioSystem()->StopMIDIMusic();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Reproduce un fichero WAV ambiente
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOPlayWavAmbientSoundInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el nombre del archivo WAV
  CScriptStackValue* pWAVFile = pScript->Pop();
  ASSERT(pWAVFile);

  // Se intenta establecer sonido ambiente
  // Nota: No se podra si universo pausado
  iCWorld* const pWorld = SYSEngine::GetWorld();
  if (!pWorld->SetAmbientWAVSound(pWAVFile->GetStringValue())) {
	// Hubo problemas
	pScript->ErrorInterrupt();
  }
  
  // Borra parametros
  delete pWAVFile;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Detiene sonido ambiente.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOStopWavAmbientSoundInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Detiene sonido ambiente
  SYSEngine::GetWorld()->SetAmbientWAVSound("");
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGOActiveTradeItemsInterfazInstr::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {  
	// ¿Es la instruccion actual?
	if (m_pScript) {
	  // Se pasa al estado anterior, desactivando con esta accion el interfaz
	  // Nota: El estado anterior sera MainInterfaz
	  m_pScript = NULL;	  
	  SYSEngine::GetGUIManager()->SetMainInterfazWindow();
	}

	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el interfaz de intercambio, para considerarlo valido se debera
//   de estar en el interfaz principal Y NO combatiendo.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOActiveTradeItemsInterfazInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman parametros
  CScriptStackValue* pEntitytToTrade = pScript->Pop();
  ASSERT(pEntitytToTrade);

  // ¿Se esta en estado valido?
  iCGUIManager* const pGUIManager = SYSEngine::GetGUIManager();
  ASSERT(pGUIManager);
  if (pGUIManager->GetGameGUIWindowState() == GUIManagerDefs::GUIW_MAININTERFAZ) {
	// ¿NO hay combate activo?
	if (!SYSEngine::GetCombatSystem()->IsCombatActive()) {
	  // Se toma instancia a la entidad
	  iCWorld* const pWorld = SYSEngine::GetWorld();
	  ASSERT(pWorld);
	  CWorldEntity* const pEntity = pWorld->GetWorldEntity(pEntitytToTrade->GetDWordValue());
	  if (pEntity) {
		// Se activa el interfaz de intercambio
		if (pGUIManager->SetTradeWindow(pWorld->GetPlayer(), pEntity, this)) {
		  pScript->Pause();
		  m_pScript = pScript;
		} else {
		  // No se pudo activar interfaz
		  pScript->ErrorInterrupt();	
		}
	  } else {
		// Entidad no valida
		pScript->ErrorInterrupt();	
	  }
	} else {
	  // El combate esta activo
	  pScript->ErrorInterrupt();	
	}
  } else {
	// No se esta en area de juego
	pScript->ErrorInterrupt();	
  }

  // Borra parametros
  delete pEntitytToTrade;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion del cuadro de dialogo de lectura de archivos de texto
// Parametros:
// - IDGUIWindow. Identificador de la ventana.
// - udParams. Parametros recibidos.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGOActiveTradeItemsInterfazInstr::GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
											      const dword udParams)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Comprueba la notifiacion
  switch(IDGUIWindow) {
	case GUIManagerDefs::GUIW_TRADE: {
	  // Se cerro el cuadro de intercambio y se reanuda script
	  ASSERT(m_pScript);
	  m_pScript->Resume();	  
	  m_pScript = NULL;	  
	} break;

	default:
	  ASSERT(false);
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Añade una opcion al interfaz de conversacion, para que se pueda añadir
//   el interfaz debera de estar activo, en caso contrario se interrumpira el
//   script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOAddOptionToConversatorInterfazInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pText = pScript->Pop();
  ASSERT(pText);
  CScriptStackValue* pIDOption = pScript->Pop();
  ASSERT(pIDOption);

  // ¿El Interfaz se halla activo?
  iCGUIManager* const pGUIManager = SYSEngine::GetGUIManager();
  ASSERT(pGUIManager);
  if (GUIManagerDefs::GUIW_CONVERSATOR == pGUIManager->GetGameGUIWindowState()) {
	// Si, añade opcion y deposita el resultado
	CScriptStackValue* pResult;
	if (pGUIManager->AddOptionToConversatorWindow(GUIDefs::GUIIDComponent(pIDOption->GetFloatValue()),
												  pText->GetStringValue())) {
	  pResult = new CScriptStackValue(1.0f);
	} else {	
	  pResult = new CScriptStackValue(0.0f);
	}
	ASSERT(pResult);
	pScript->Push(pResult);
  } else {
	// No, se interrumpe
	pScript->ErrorInterrupt();
  }  
  
  // Borra parametros
  delete pIDOption;
  delete pText;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Resetea todas las opciones que se hayan podido añadir al interfaz de 
//   conversacion. Para validar la accion, el interfaz se debe de encontrar
//   activo, en caso contrario se inrrumpira el script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOResetOptionsInConversatorInterfazInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // ¿El Interfaz se halla activo?
  iCGUIManager* const pGUIManager = SYSEngine::GetGUIManager();
  ASSERT(pGUIManager);
  if (GUIManagerDefs::GUIW_CONVERSATOR == pGUIManager->GetGameGUIWindowState()) {
	// Si, reseta las opciones registradas en el cuadro de dialogo
    pGUIManager->ResetOptionsInConversatorWindow();	
  } else {
	// No, se interrumpe
	pScript->ErrorInterrupt();
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el interfaz de conversacion, para validarlo se debera de estar
//   en estado de MainInterfaz, en caso contrario se interrumpira el script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOActiveConversatorInterfazInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // ¿Estado de MainInterfaz?
  bool bExecuteOk = false;
  iCGUIManager* const pGUIManager = SYSEngine::GetGUIManager();
  ASSERT(pGUIManager);
  if (GUIManagerDefs::GUIW_MAININTERFAZ == pGUIManager->GetGameGUIWindowState()) {
	// Toma parametros
	CScriptStackValue* pEntityToTalk = pScript->Pop();
	if (pEntityToTalk) {  
	  // Activa cuadro de dialogo y pausa el script
	  // Nota: El conversador principal SIEMPRE sera el jugador
	  iCWorld* const pWorld = SYSEngine::GetWorld();
	  ASSERT(pWorld);
	  CWorldEntity* const pSecondaryConv = pWorld->GetWorldEntity(pEntityToTalk->GetDWordValue());	
	  if (pSecondaryConv &&
		  pGUIManager->SetConversatorWindow(pWorld->GetPlayer(), pSecondaryConv)) {
		bExecuteOk = true;
	  }
	}	

	// Borra parametros
	delete pEntityToTalk;
  }

  // ¿Hubo problemas?
  if (!bExecuteOk) {
	// Si, se interrumpe el script
	pScript->ErrorInterrupt();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva el interfaz de conversacion, para poderlo desactivar se 
//   debera de encontrar activo.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGODeactiveConversatorInterfazInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // ¿Esta el interfaz de conversacion activo?
  iCGUIManager* const pGUIManager = SYSEngine::GetGUIManager();
  ASSERT(pGUIManager);
  if (GUIManagerDefs::GUIW_CONVERSATOR == pGUIManager->GetGameGUIWindowState()) {
	// Si, se pasa a estado de MainInterfaz
	pGUIManager->SetMainInterfazWindow();
  } else {
	// No se esta en estado de interfaz valido
	pScript->ErrorInterrupt();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGOGetOptionFromConversatorInterfazInstr::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {  
	// ¿Es la instruccion actual?
	if (m_pScript) {
	  // Ordena la desactivacion de la interfaz
	  // Nota: La desactivacion supondra activar el MainInterfaz, al hacerlo
	  // se notificara la seleccion de una opcion inexistente en interfaz.
	  m_pScript = NULL;	  
	  SYSEngine::GetGUIManager()->SetMainInterfazWindow();
	}

	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Instruccion relativa a la orden de coger una opcion del interfaz de 
//   conversacion. El interfaz se debera de encontrar activo, de lo contrario
//   se interrumpira el script.
// - En caso de que la orden de obtencion de opcion tenga exito, se pausara
//   el script y se reanudara cuando reciba una notificacion de opcion 
//   seleccionada. En caso de que no se pueda preguntar por las opciones al
//   jugador a causa de que no hay asociadas, se interrumpira el script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOGetOptionFromConversatorInterfazInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // ¿Esta el interfaz de conversacion activo?
  bool bExecuteOk = false;
  iCGUIManager* const pGUIManager = SYSEngine::GetGUIManager();
  ASSERT(pGUIManager);
  if (GUIManagerDefs::GUIW_CONVERSATOR == pGUIManager->GetGameGUIWindowState()) {
	// Si, se intenta ordenar la toma de opciones
	if (pGUIManager->GetOptionFromConversatorWindow(this)) {
	  // Se guarda el script actual pausandolo antes
	  pScript->Pause();
	  m_pScript = pScript;
	  bExecuteOk = true;
	}	
  }

  // ¿Hubo problemas?
  if (!bExecuteOk) {
	// Si, se interrumpe el script
	pScript->ErrorInterrupt();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion de la obtencion de una opcion desde el interfaz de 
//   conversacion
// Parametros:
// - IDGUIWindow. Identificador de la ventana.
// - udParams. Parametros recibidos.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGOGetOptionFromConversatorInterfazInstr::GUIWindowNotify(const GUIManagerDefs::eGUIWindowType& IDGUIWindow,
											              const dword udParams)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Comprueba la notifiacion
  switch(IDGUIWindow) {
	case GUIManagerDefs::GUIW_CONVERSATOR: {
	  // Se escogio una opcion de conversacion, se reanuda script y deposita
	  // el codigo de la opcion escogida.
	  ASSERT(m_pScript);
	  m_pScript->Resume();
	  CScriptStackValue* const pResult = new CScriptStackValue(float(udParams));
	  ASSERT(pResult);
	  m_pScript->Push(pResult);
	  m_pScript = NULL;	  
	} break;

	default:
	  ASSERT(false);
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGOShowPresentationInstr::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {  
	// ¿Es la instruccion actual?
	if (m_pScript) {
	  // Si, desinstalara a la instruccion como observer y desvinculara script
	  m_pScript = NULL;
	  SYSEngine::GetWorld()->RemoveObserver(this);
	}

	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Muestra una presentacion y hace que el script espere antes de que
//   pueda seguir ejecutando instrucciones.
// - Como mostrar una presentacion supone pausar el universo de juego, la
//   instruccion se instalara como observador del mismo. Cuando la presentacion
//   concluya, el universo sera quitado de su estado de pausa y esta
//   instruccion recibira la notificacion.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOShowPresentationInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pPresentation = pScript->Pop();
  ASSERT(pPresentation);

  // ¿Se pudo lanzar la presentacion?
  if (SYSEngine::GetGUIManager()->SetPresentationWindow(pPresentation->GetStringValue())) {	
	// Si se pudo, se instala como cliente del universo de juego
	// para recibir notificacion de la finalizacion del modo pausa.
	pScript->Pause();
	m_pScript = pScript;
	SYSEngine::GetWorld()->AddObserver(this);
  } else {
	// No se pudo lanzar la presentacion, se interrumpe ejecucion
	pScript->ErrorInterrupt();
  }

  // Se borran parametros
  delete pPresentation;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion del universo de juego. Se utilizara para conocer si este
//   ha dejado de estar en pausa a causa de la ejecucion de una presentacion
//   y se puede activar de nuevo el script.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOShowPresentationInstr::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
											  const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Comprueba que la notificacion sea la buscada
  switch(NotifyType) {
	case WorldObserverDefs::PAUSE_OFF: {
	  // A finalizado la pausa, se continua con la ejecucion del script
	  ASSERT(m_pScript);
	  m_pScript->Resume();
	  m_pScript = NULL;
	  SYSEngine::GetWorld()->RemoveObserver(this);
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicia una cutscene SOLO si no esta una activa. Ademas se debera de estar
//   en estado de interfaz valido.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOBeginCutSceneInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // ¿Estado de interfaz valido?
  bool bResult = false;
  iCGUIManager* const pGUIManager = SYSEngine::GetGUIManager();
  ASSERT(pGUIManager);
  if (pGUIManager->GetGameGUIWindowState() == GUIManagerDefs::GUIW_MAININTERFAZ) {
	// ¿NO esta activo el modo combate?
	if (!SYSEngine::GetCombatSystem()->IsCombatActive()) {
	  // ¿NO hay CutScene activa?	
	  if (!pGUIManager->IsCutSceneActive()) {
		// Si, se comienza, pausando el script
		SYSEngine::GetGUIManager()->BeginCutScene(this, IDEndBeginCutScene);
		pScript->Pause();
		m_pScript = pScript;
		bResult = true;
	  }
	}
  }
  
  // ¿Hubo problemas?
  if (!bResult) {
	// Si, se interrumpe script
	pScript->ErrorInterrupt();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notifica la finalizacion del BeginCutScene
// Parametros:
// - IDCommand. Identificador del comando.
// - udInstant. Instante de finalizacion.
// - udExtraParam. Paramemtro extra.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGOBeginCutSceneInstr::EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
								    const dword udInstant,
									const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se comprueba el identificador
  if (IDCommand == IDEndBeginCutScene) {
	// Se quita la pausa del script
	ASSERT(m_pScript);
	m_pScript->Resume();
	m_pScript = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza una cutscene SOLO si esta una activa. 
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOEndCutSceneInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // ¿Estado de interfaz valido?
  iCGUIManager* const pGUIManager = SYSEngine::GetGUIManager();
  ASSERT(pGUIManager);
  // ¿Hay CutScene activa?
  if (pGUIManager->IsCutSceneActive()) {
	// Si, se desactiva, pausando el script
    SYSEngine::GetGUIManager()->EndCutScene(this, IDEndEndCutScene);	
	pScript->Pause();
	m_pScript = pScript;
  } else {
	// No, se interrumpe script
	pScript->ErrorInterrupt();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notifica la finalizacion del EndCutScene
// Parametros:
// - IDCommand. Identificador del comando.
// - udInstant. Instante de finalizacion.
// - udExtraParam. Paramemtro extra.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGOEndCutSceneInstr::EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
							      const dword udInstant,
								  const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se comprueba el identificador
  if (IDCommand == IDEndEndCutScene) {
	// Se quita la pausa del script y se asociara la camara al jugador
	// si no lo estuviera
	ASSERT(m_pScript);
	m_pScript->Resume();
	m_pScript = NULL;
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);
	pWorld->AttachCamera(pWorld->GetPlayer()->GetHandle());
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia un script a un posible evento del objeto Game. En caso de que
//   el evento pasado NO sea valido, se interrumpira la ejecucion del script.
//   Son eventos validos:
//   * SE_ONCLICKHOURPANEL
//   * SE_ONFLEECOMBAT	
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - En esta instruccion no se podra evaluar que el fichero con el script que
//   se va a asociar exista (en caso de asociar alguno).
// - Aunque se asocia al objeto Game, todos los eventos relativos al mismo
//   estaran almacenados en World.
///////////////////////////////////////////////////////////////////////////////
void
CGOSetScriptInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pFile = pScript->Pop();
  ASSERT(pFile);
  CScriptStackValue* pEvent = pScript->Pop();
  ASSERT(pEvent);

  // ¿Es un evento valido?
  RulesDefs::eScriptEvents Event = RulesDefs::eScriptEvents(sword(pEvent->GetFloatValue()));
  if (Event == RulesDefs::SE_ONCLICKHOURPANEL ||
	  Event == RulesDefs::SE_ONFLEECOMBAT ||
	  Event == RulesDefs::SE_ONKEYPRESSED ||
	  Event == RulesDefs::SE_ONSTARTCOMBATMODE ||
	  Event == RulesDefs::SE_ONENDCOMBATMODE) {
	// Si, se procede a instalar
	SYSEngine::GetWorld()->SetScriptEvent(Event, pFile->GetStringValue());
  } else {
	// No, hay problemas
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pEvent;
  delete pFile;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Consultara al MainInterfaz para conocer si una tecla ha sido pulsada o
//   no.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGOIsKeyPressedInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pKey = pScript->Pop();
  ASSERT(pKey);
  
  // Comprueba si la tecla esta pulsada y deposita resultado
  const InputDefs::eInputEventCode Key = InputDefs::eInputEventCode(sword(pKey->GetFloatValue()));
  const bool bIsPressed = SYSEngine::GetGUIManager()->IsKeyPressedInMainInterfaz(Key);
  CScriptStackValue* const pResult = new CScriptStackValue(bIsPressed ? 1.0f : 0.0f);
  ASSERT(pResult);
  pScript->Push(pResult);

  // Borra parametros
  delete pKey;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retornara el nombre del area actual.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOGetAreaNameInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el nombre del area actual
  CScriptStackValue* const pAreaName = new CScriptStackValue(SYSEngine::GetWorld()->GetAreaName());
  ASSERT(pAreaName);

  // Lo deposita en pila
  pScript->Push(pAreaName);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retornara el identificador del area actual
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOGetAreaIDInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el nombre del area actual
  CScriptStackValue* const pAreaID = new CScriptStackValue(float(SYSEngine::GetWorld()->GetIDArea()));
  ASSERT(pAreaID);

  // Lo deposita en pila
  pScript->Push(pAreaID);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Toma la anchura del area y la deposita
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOGetAreaWidthInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma la anchura
  CScriptStackValue* const pAreaWidth = new CScriptStackValue(float(SYSEngine::GetWorld()->GetAreaWidth()));
  ASSERT(pAreaWidth);

  // Deposita valor
  pScript->Push(pAreaWidth);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Toma la altura del area y la deposita
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOGetAreaHeightInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma la anchura
  CScriptStackValue* const pAreaHeight = new CScriptStackValue(float(SYSEngine::GetWorld()->GetAreaHeight()));
  ASSERT(pAreaHeight);

  // Deposita valor
  pScript->Push(pAreaHeight);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Toma la hora actual y la deposita
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOGetHourInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma la anchura
  CScriptStackValue* const pHour = new CScriptStackValue(float(SYSEngine::GetWorld()->GetHour()));
  ASSERT(pHour);

  // Deposita valor
  pScript->Push(pHour);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene los minutos de juego
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOGetMinuteInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma la anchura
  CScriptStackValue* const pMinute = new CScriptStackValue(float(SYSEngine::GetWorld()->GetMinute()));
  ASSERT(pMinute);

  // Deposita valor
  pScript->Push(pMinute);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece la hora actual
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOSetHourInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el parametro hora
  CScriptStackValue* pHour = pScript->Pop();
  ASSERT(pHour);

  // Establece hora
  SYSEngine::GetWorld()->SetHour(pHour->GetFloatValue());

  // Elimina parametro
  delete pHour;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece los minutos de juego actuales
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOSetMinuteInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);
  
  // Toma el parametro minute
  CScriptStackValue* pMinute = pScript->Pop();
  ASSERT(pMinute);

  // Establece hora
  SYSEngine::GetWorld()->SetMinute(pMinute->GetFloatValue());

  // Elimina parametro
  delete pMinute;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el handle a una entidad a traves de su tag
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOGetEntityInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* const pszTag = pScript->Pop();
  ASSERT(pszTag);

  // Obtiene el posible handle asociado y lo deposita
  const AreaDefs::EntHandle hEntity = SYSEngine::GetWorld()->GetHandleFromTag(pszTag->GetStringValue());
  CScriptStackValue* const pResult = new CScriptStackValue(dword(hEntity));
  ASSERT(pResult);
  pScript->Push(pResult);
  
  // Borra parametros
  delete pszTag;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtendra el handle a la entidad que simboliza al jugador y la depositara
//   en la pila.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOGetPlayerInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle al jugador
  CScriptStackValue* const pPlayer = new CScriptStackValue(dword(SYSEngine::GetWorld()->GetPlayer()->GetHandle()));
  ASSERT(pPlayer);

  // Introduce el valor  
  pScript->Push(pPlayer);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si un floor (tile) es valido en las posiciones que se pasen
//   como parametros
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Se considerara posicion valida cuando pertenezca al mapa y ademas tenga
//   contenido asociado
///////////////////////////////////////////////////////////////////////////////
void
CWOIsFloorValidInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman los parametros  
  CScriptStackValue* pYPos = pScript->Pop();
  ASSERT(pYPos);
  CScriptStackValue* pXPos = pScript->Pop();
  ASSERT(pXPos);

  // Se comprueba si es posicion valida. En caso de que lo sea se pondra un 
  // valor number 1 y en caso contrario 0
  CScriptStackValue* pResult = NULL;
  if (SYSEngine::GetWorld()->IsFloorValid(AreaDefs::sTilePos(pXPos->GetFloatValue(), pYPos->GetFloatValue()))) {
	pResult = new CScriptStackValue(1.0f);
  } else {
	pResult = new CScriptStackValue(0.0f);
  }

  // Se inserta valor en pila
  ASSERT(pResult);
  pScript->Push(pResult);

  // Se borra parametros
  delete pXPos;
  delete pYPos;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el valor de elevacion en la posiciones que se hayan pasado como
//   parametro y la deposita en la pila.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOGetElevationAtInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman los parametros  
  CScriptStackValue* pYPos = pScript->Pop();
  ASSERT(pYPos);
  CScriptStackValue* pXPos = pScript->Pop();
  ASSERT(pXPos);

  // Obtiene el floor en la posiciones recibidas y pone en pila su elevacion
  CFloor* const pFloor = SYSEngine::GetWorld()->GetFloor(AreaDefs::sTilePos(pXPos->GetFloatValue(), pYPos->GetFloatValue()));
  ASSERT(pFloor);
  if (pFloor) {
	// Establece elevacion
	CScriptStackValue* const pElevation = new CScriptStackValue(float(pFloor->GetElevation()));
	ASSERT(pElevation);
	pScript->Push(pElevation);
  } else {
	// No se hallo floor en esa posicion
	pScript->ErrorInterrupt();
  }

  // Borra los parametros
  delete pXPos;
  delete pYPos;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el numero de items que existen en una determinada posicion. En 
//   caso de que el tile pasado no sea valido se detendra el script y en
//   caso contrario el numero de items
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOGetNumItemsAtInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman los parametros  
  CScriptStackValue* pYPos = pScript->Pop();
  ASSERT(pYPos);
  CScriptStackValue* pXPos = pScript->Pop();
  ASSERT(pXPos);

  // Se obtiene el numero de items y se inserta
  CScriptStackValue* pNumItems = NULL;
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  const AreaDefs::sTilePos Tile(pXPos->GetFloatValue(), pYPos->GetFloatValue());
  // ¿Pos. valida?
  if (pWorld->IsFloorValid(Tile)) {		
	// Se obtiene el numero de elementos
	pNumItems = new CScriptStackValue(float(pWorld->GetNumItemsAt(Tile)));
	ASSERT(pNumItems);
	pScript->Push(pNumItems);
  } else {
	pScript->ErrorInterrupt();
  }

  // Se borran parametros
  delete pXPos;
  delete pYPos;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el handle al item que se encuentra sobre un tile, pero en una
//   determinada posicion dentro de la secuencia de items sobre ese tile. Se
//   depositara el handle a ese item.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - En caso de que la posicion donde localizar sea inferior a 1 se depositara
//   el valor 0 y en caso de que sea mayor tambien el valor 0. Naturalmente,
//   esto mismo ocurrira si la posicion NO es valida.
///////////////////////////////////////////////////////////////////////////////
void
CWOGetItemAtInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman los parametros
  CScriptStackValue* pLocation = pScript->Pop();
  ASSERT(pLocation);
  CScriptStackValue* pYPos = pScript->Pop();
  ASSERT(pYPos);
  CScriptStackValue* pXPos = pScript->Pop();
  ASSERT(pXPos);  
  
  // Se obtiene handle al item y se inserta
  CScriptStackValue* pItem = NULL;
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  const AreaDefs::sTilePos Tile(pXPos->GetFloatValue(), pYPos->GetFloatValue());
  // ¿Pos. valida?
  if (pWorld->IsFloorValid(Tile)) {		
	// Se obtiene el handle al item en esa secuencia
	pItem = new CScriptStackValue(dword(pWorld->GetItemAt(Tile, pLocation->GetFloatValue())));
	ASSERT(pItem);
	pScript->Push(pItem);
  } else {
	pScript->ErrorInterrupt();
  }

  // Se borran parametros
  delete pXPos;
  delete pYPos;
  delete pLocation;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Calculara la distancia absoluta entre dos tiles.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOGetDistanceInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman los parametros  
  CScriptStackValue* pYPosDest = pScript->Pop();
  ASSERT(pYPosDest);
  CScriptStackValue* pXPosDest = pScript->Pop();
  ASSERT(pXPosDest);
  CScriptStackValue* pYPosSrc = pScript->Pop();
  ASSERT(pYPosSrc);
  CScriptStackValue* pXPosSrc = pScript->Pop();
  ASSERT(pXPosSrc);
    
  // Se calcula la distancia absoluta, si las pos. no son validas se retornara -1
  const sword swDist = SYSEngine::GetWorld()->CalculeAbsoluteDistance(AreaDefs::sTilePos(pXPosSrc->GetFloatValue(), pXPosSrc->GetFloatValue()),
																	  AreaDefs::sTilePos(pYPosSrc->GetFloatValue(), pYPosSrc->GetFloatValue()));
  CScriptStackValue* const pDistance = new CScriptStackValue(float(swDist));
  ASSERT(pDistance);
  pScript->Push(pDistance);

  // Elimina parametros
  delete pXPosSrc;
  delete pYPosSrc;
  delete pXPosDest;
  delete pYPosDest;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la longitud del camino que existira entre dos posiciones. En caso
//   de que alguna posicion no sea valida se retornara -1. Tambien se retornara
//   esto si el camino no es posible.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOCalculePathLenghtInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman los parametros  
  CScriptStackValue* pYPosDest = pScript->Pop();
  ASSERT(pYPosDest);
  CScriptStackValue* pXPosDest = pScript->Pop();
  ASSERT(pXPosDest);
  CScriptStackValue* pYPosSrc = pScript->Pop();
  ASSERT(pYPosSrc);
  CScriptStackValue* pXPosSrc = pScript->Pop();
  ASSERT(pXPosSrc);
    
  // Se calcula la longitud del camino
  const sword swLenght = SYSEngine::GetWorld()->CalculePathLenght(AreaDefs::sTilePos(pXPosSrc->GetFloatValue(), pXPosSrc->GetFloatValue()),
																  AreaDefs::sTilePos(pYPosSrc->GetFloatValue(), pYPosSrc->GetFloatValue()));
  CScriptStackValue* const pLenght = new CScriptStackValue(float(swLenght));
  ASSERT(pLenght);
  pScript->Push(pLenght);

  // Elimina parametros
  delete pXPosSrc;
  delete pYPosSrc;
  delete pXPosDest;
  delete pYPosDest;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga un area. Tanto si se tiene exito, se detendra el script actual pues
//   se entendera que todos los scripts del area que estaba anteriormente cargada
//   dejaran de ser validos. Ademas, todos los demas scripts en pausa o que aun
//   continuen en solicitud pero que pertenezcan a otro area, dejaran de ser
//   validos.
// - Para poder validar la llamada, se debera de estar en MainInterfaz.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOLoadAreaInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman los parametros  
  CScriptStackValue* pYPos = pScript->Pop();
  ASSERT(pYPos);
  CScriptStackValue* pXPos = pScript->Pop();
  ASSERT(pXPos);
  CScriptStackValue* pAreaID = pScript->Pop();
  ASSERT(pAreaID);

  // ¿Se esta en estado de MainInterfaz?
  bool bExecuteOk = false;
  iCGUIManager* const pGUIManager = SYSEngine::GetGUIManager();
  ASSERT(pGUIManager);
  if (pGUIManager->GetGameGUIWindowState() == GUIManagerDefs::GUIW_MAININTERFAZ) {	
	// Intenta cargar y en caso de exito, detiene script actual
	const AreaDefs::sTilePos TileDest(pXPos->GetFloatValue(), pYPos->GetFloatValue());
	if (SYSEngine::SetAreaLoadingState(pAreaID->GetFloatValue(), TileDest)) {
	  // Todo bien
	  pScript->Stop();
	  bExecuteOk = true;
	} 
  } 
  
  // ¿Hubo problemas?
  if (!bExecuteOk) {
	// Si, se interrumpe el script
	pScript->ErrorInterrupt();
  }

  // Elimina parametros
  delete pYPos;
  delete pXPos;
  delete pAreaID;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se cambia la localidad de una entidad, siempre y cuando dicha entidad sea
//   un item o bien una criatura. En caso de haber tenido exito se retornara un 
//   valor 1 y en caso contrario 0.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - El filtrado del tipo de entidad para que el cambio sea valido se
//   llevara en CArea.
///////////////////////////////////////////////////////////////////////////////
void
CWOChangeEntityLocationInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman los parametros  
  CScriptStackValue* pYPos = pScript->Pop();
  ASSERT(pYPos);  
  CScriptStackValue* pXPos = pScript->Pop();
  ASSERT(pXPos);
  CScriptStackValue* pEntityToChange = pScript->Pop();
  
  // ¿Entidad valida?
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CWorldEntity* const pEntity = pWorld->GetWorldEntity(pEntityToChange->GetDWordValue());
  if (pEntity) {
	// Si, se procede a cambiar e insertar el resultado
	CScriptStackValue* pReturn = NULL;
	if (pWorld->ChangeLocation(pEntity->GetHandle(),
							   AreaDefs::sTilePos(pXPos->GetFloatValue(), pYPos->GetFloatValue()))) {
	  pReturn = new CScriptStackValue(1.0f);
	} else {
	  pReturn = new CScriptStackValue(0.0f);
	}
	ASSERT(pReturn);
	pScript->Push(pReturn);
  } else {
	pScript->ErrorInterrupt();
  }  

  // Borra parametros
  delete pEntityToChange;
  delete pXPos;
  delete pYPos;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWOAttachCameraToEntityInstr::End(void)
{  
  // Finaliza
  if (Inherited::IsInitOk()) {  
	// ¿Es la instruccion actual?
	if (m_pScript) {
	  // Bastara con asociar la camara al jugador de forma inmediata
	  // Nota: Al asociar la camara al jugador inmediatamente, se perdera
	  // la notificacion en la instruccion, asi que se desvinculara 
	  // la instancia al script aqui	
	  m_pScript = NULL;	  
	  SYSEngine::GetGUIManager()->SetMainInterfazWindow();
	  iCWorld* const pWorld = SYSEngine::GetWorld();
	  ASSERT(pWorld);
	  pWorld->AttachCamera(pWorld->GetPlayer()->GetHandle());
	}

	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia la camara del motor isometrico a una entidad. Se debera de estar
//   en estado de MainInterfaz para aceptar la llamada. Siempre que al 
//   asociar se especifique que exista travelling, el script se pausara.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Recordar que la camara solo se podra asociar a una entidad si esta
//   activo el estado de CutScene.
///////////////////////////////////////////////////////////////////////////////
void
CWOAttachCameraToEntityInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman los parametros  
  CScriptStackValue* pSpeed = pScript->Pop();
  ASSERT(pSpeed);
  CScriptStackValue* pEntityToAttach = pScript->Pop();
  ASSERT(pEntityToAttach);
  
  // ¿Esta activa CutScene?
  iCGUIManager* const pGUIManager = SYSEngine::GetGUIManager();
  ASSERT(pGUIManager);
  if (pGUIManager->IsCutSceneActive()) {	
	// Se toma instancia a entidad
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);
	CWorldEntity* const pEntity = pWorld->GetWorldEntity(pEntityToAttach->GetDWordValue());
	if (pEntity) {
	  // Se ajusta el valor de velocidad
	  word uwSpeed;
	  if (pSpeed->GetFloatValue() < 1) {
		uwSpeed = 0;
	  } else if (pSpeed->GetFloatValue() < 32) {
		uwSpeed = 32;
	  } else if (pSpeed->GetFloatValue() > 256) {
		uwSpeed = 256;
	  } else {
		uwSpeed = pSpeed->GetFloatValue();
	  }

	  // ¿Hay que realizar travelling?
	  if (uwSpeed > 0) {
		// Si, se asocia camara y se instala instruccion como cliente
		pWorld->AttachCamera(pEntity->GetHandle(), 
							 uwSpeed, 
							 this, 
							 IDEndTravelling);

		// Pausa y guarda script
		pScript->Pause();
		m_pScript = pScript;		
	  }	else {
		// No, se asocia camara directamente
		pWorld->AttachCamera(pEntity->GetHandle(), uwSpeed);
	  }
	} else {
	  // Entidad no valida
	  pScript->ErrorInterrupt();
	}
  } else {
	// Estado de interfaz no valido
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pSpeed;
  delete pEntityToAttach;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion de la finalizacion del comando de travelling. Hara que el
//   script prosiga su ejecucion.
// Parametros:
// - IDCommand. Identificador del comando.
// - udInstant. Instante de finalizacion.
// - udExtraParam. Paramemtro extra.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWOAttachCameraToEntityInstr::EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
										   const dword udInstant,
										   const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se comprueba el identificador
  if (IDCommand == IDEndTravelling) {
	// Se quita la pausa del script
	ASSERT(m_pScript);
	m_pScript->Resume();
	m_pScript = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWOAttachCameraToLocationInstr::End(void)
{  
  // Finaliza
  if (Inherited::IsInitOk()) {  
	// ¿Es la instruccion actual?
	if (m_pScript) {
	  // Bastara con asociar la camara al jugador de forma inmediata
	  // Nota: Al asociar la camara al jugador inmediatamente, se perdera
	  // la notificacion en la instruccion, asi que se desvinculara 
	  // la instancia al script aqui	
	  m_pScript = NULL;	  
	  SYSEngine::GetGUIManager()->SetMainInterfazWindow();
	  iCWorld* const pWorld = SYSEngine::GetWorld();
	  ASSERT(pWorld);
	  pWorld->AttachCamera(pWorld->GetPlayer()->GetHandle());
	}

	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia la camara del motor isometrico a una posicion. En caso de que
//   no se este en MainInterfaz habra error en llamada.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Recordar que la camara solo se podra asociar a una entidad si esta
//   activo el estado de CutScene.
///////////////////////////////////////////////////////////////////////////////
void
CWOAttachCameraToLocationInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toman los parametros  
  CScriptStackValue* pSpeed = pScript->Pop();
  ASSERT(pSpeed);
  CScriptStackValue* pYPos = pScript->Pop();
  ASSERT(pYPos);
  CScriptStackValue* pXPos = pScript->Pop();
  ASSERT(pXPos);

  // ¿Esta activa CutScene?
  iCGUIManager* const pGUIManager = SYSEngine::GetGUIManager();
  ASSERT(pGUIManager);
  if (pGUIManager->IsCutSceneActive()) {
	// Se pasa la posicion a coordenadas universales
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);
	const AreaDefs::sTilePos TilePos(pXPos->GetFloatValue(), pYPos->GetFloatValue());

	// Se ajusta el valor de velocidad
    word uwSpeed;
    if (pSpeed->GetFloatValue() < 1) {
  	  uwSpeed = 0;
    } else if (pSpeed->GetFloatValue() < 32) {
	  uwSpeed = 32;
	} else if (pSpeed->GetFloatValue() > 256) {
	  uwSpeed = 256;
	} else {
	  uwSpeed = pSpeed->GetFloatValue();
	}
	
	// ¿Hay que realizar travelling?
    if (uwSpeed > 0) {	  
	  // Si, se asocia y se instala como cliente
  	  SYSEngine::GetWorld()->AttachCamera(TilePos, uwSpeed, this, IDEndTravelling);

	  // Pausa y guarda script
	  pScript->Pause();
	  m_pScript = pScript;
	} else {
	  // No, asocia directamente
	  std::string szText;
	  SYSEngine::GetWorld()->AttachCamera(TilePos, uwSpeed);
	}	
  } else {
	// Error en llamada a este metodo
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pSpeed;
  delete pYPos;
  delete pXPos;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion de la finalizacion del comando de travelling. Hara que el
//   script prosiga su ejecucion.
// Parametros:
// - IDCommand. Identificador del comando.
// - udInstant. Instante de finalizacion.
// - udExtraParam. Paramemtro extra.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CWOAttachCameraToLocationInstr::EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
											 const dword udInstant,
											 const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se comprueba el identificador
  if (IDCommand == IDEndTravelling) {
	// Se quita la pausa del script
	ASSERT(m_pScript);
	m_pScript->Resume();
	m_pScript = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprobara si el modo combate esta activo. Si esta activo retornara 1 y
//   en caso contrario 0.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOIsCombatModeActiveInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Crea el nodo con el resultado y lo pone en pila
  CScriptStackValue* pResult = NULL;
  if (SYSEngine::GetCombatSystem()->IsCombatActive()) {
	pResult = new CScriptStackValue(1.0f);
  } else {
	pResult = new CScriptStackValue(0.0f);
  }
  ASSERT(pResult);
  pScript->Push(pResult);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza cualquier combate que pudiera estar activo.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOEndCombatInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Finaliza
  SYSEngine::GetCombatSystem()->EndCombat();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene handle a la criatura que se halle con el turno de combate. En
//   caso de no estar en modo combate, retornara el handle nulo.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOGetCriatureInCombatTurnInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se crea handle y se pone en pila
  CScriptStackValue* const pEntity = new CScriptStackValue(dword(SYSEngine::GetCombatSystem()->GetCriatureInTurn()));
  ASSERT(pEntity);
  pScript->Push(pEntity);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retorna el handle al combatienente que pertenece a una determinada 
//   alineacion y que se halla en una determinada posicion dentro de dicha
//   alineacion. Si la alineacion no es valida se detendra el script
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOGetCombatantInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pPos = pScript->Pop();
  ASSERT(pPos);
  CScriptStackValue* pAlingment = pScript->Pop();
  ASSERT(pAlingment);
  
  // ¿Alineacion valida?
  bool bExecuteOk = false;
  CScriptStackValue* pEntity = NULL;
  if (pAlingment->GetFloatValue() != CombatSystemDefs::NO_ALINGMENT) {
	// Si, se toma el posible handle
	const CombatSystemDefs::eCombatAlingment Alingment = CombatSystemDefs::eCombatAlingment(sword(pAlingment->GetFloatValue()));
	pEntity = new CScriptStackValue(dword(SYSEngine::GetCombatSystem()->GetCombatant(Alingment,
																					  pPos->GetFloatValue())));
	if (pEntity) {
	  pScript->Push(pEntity);
	  bExecuteOk = true;
	}
  }

  // Se ejecuto correctamente
  if (bExecuteOk) {
	// Si, se interrumpe script
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pAlingment;
  delete pPos;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el numero de combatientes para una determinada alineacion. En caso
//   de alineacion no valida, se detendra el script
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOGetNumberOfCombatantsInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pAlingment = pScript->Pop();
  ASSERT(pAlingment);

  // ¿Alineacion correcta?
  CScriptStackValue* pNumber = NULL;
  if (pAlingment->GetFloatValue() != CombatSystemDefs::NO_ALINGMENT) {
	// Si, se retorna el posible 
	const CombatSystemDefs::eCombatAlingment Alingment = CombatSystemDefs::eCombatAlingment(sword(pAlingment->GetFloatValue()));
	pNumber = new CScriptStackValue(float(SYSEngine::GetCombatSystem()->GetNumberOfCombatants(Alingment)));
	ASSERT(pNumber);
	pScript->Push(pNumber);
  } else {
	pScript->ErrorInterrupt();
  }

  // Se borran parametros
  delete pAlingment;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el modelo de luz del area actual. Si se retorna 0 se entendera que
//   el modelo se basa en los cambios horarios. Si por el contrario se retorna
//   un valor entre 1 y 255 se basara en un ambiente estatico.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOGetAreaLightModelInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Se toma el modelo y se inserta
  CScriptStackValue* const pValue = new CScriptStackValue(float(SYSEngine::GetWorld()->GetAmbientLight()));
  ASSERT(pValue);
  pScript->Push(pValue);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el tiempo de ejecucion para el evento Idle.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOSetIdleScriptTimeInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pTime = pScript->Pop();
  ASSERT(pTime);

  // Se establece tiempo recibido
  SYSEngine::GetWorld()->SetIdleEventTime(pTime->GetFloatValue());

  // Borra parametros
  delete pTime;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia un script a un posible evento del objeto World. En caso de que
//   el evento pasado NO sea valido, se interrumpira la ejecucion del script.
//   Son eventos validos:
//   * SE_ONSTARTGAME
//   * SE_ONNEWHOUR	
//   * SE_ONWORLDIDLE
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - En esta instruccion no se podra evaluar que el fichero con el script que
//   se va a asociar exista (en caso de asociar alguno).
///////////////////////////////////////////////////////////////////////////////
void
CWOSetScriptInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pFile = pScript->Pop();
  ASSERT(pFile);
  CScriptStackValue* pEvent = pScript->Pop();
  ASSERT(pEvent);

  // ¿Es un evento valido?
  RulesDefs::eScriptEvents Event = RulesDefs::eScriptEvents(sword(pEvent->GetFloatValue()));
  if (Event == RulesDefs::SE_ONSTARTGAME ||
	  Event == RulesDefs::SE_ONNEWHOUR ||
	  Event == RulesDefs::SE_ONWORLDIDLE) {
	// Si, se procede a instalar
	SYSEngine::GetWorld()->SetScriptEvent(Event, pFile->GetStringValue());
  } else {
	// No, hay problemas
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pEvent;
  delete pFile;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Destruye una entidad, en caso de que no se pueda destruir el script se
//   interrumpira, pues sera de esperar que la llamada produzca un resultado
//   "limpio".
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWODestroyEntityInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pEntityHandle = pScript->Pop();
  ASSERT(pEntityHandle);

  // ¿NO se puede destruir?
  if (!SYSEngine::GetWorld()->DestroyEntity(pEntityHandle->GetDWordValue())) {
	// Hay problemas
	pScript->ErrorInterrupt();
  } 

  // Borra parametros
  delete pEntityHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea una criatura de forma dinamica y devuelve el handle. Si el handle 
//   recibido es nulo, se entendera que no se pudo crear la criatura y se
//   interrumpira el script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOCreateCriatureInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pTempCriature = pScript->Pop();
  ASSERT(pTempCriature);
  CScriptStackValue* pLight = pScript->Pop();
  ASSERT(pLight);
  CScriptStackValue* pElevation = pScript->Pop();
  ASSERT(pElevation);
  CScriptStackValue* pTag = pScript->Pop();
  ASSERT(pTag);
  CScriptStackValue* pYPos = pScript->Pop();
  ASSERT(pYPos);
  CScriptStackValue* pXPos = pScript->Pop();
  ASSERT(pXPos);
  CScriptStackValue* pProfile = pScript->Pop();
  ASSERT(pProfile);
  
  // Intenta crear la entidad
  const AreaDefs::sTilePos TilePos(pXPos->GetFloatValue(), pYPos->GetFloatValue());
  const AreaDefs::EntHandle hEntity = SYSEngine::GetWorld()->CreateCriature(pProfile->GetStringValue(),
																			TilePos,
																			pTag->GetStringValue(),
																			pElevation->GetFloatValue(),
																			pLight->GetFloatValue(),
																			pTempCriature->GetFloatValue() < 1.0f ? false : true);
  if (hEntity) {
	// Se creo, se coloca el handle en pila
	CScriptStackValue* const pResult = new CScriptStackValue(dword(hEntity));
	ASSERT(pResult);
	pScript->Push(pResult);
  } else {
	// Hubo problemas
	pScript->ErrorInterrupt();
  }  

  // Borra parametros
  delete pTempCriature;
  delete pLight;
  delete pElevation;
  delete pProfile;
  delete pYPos;
  delete pXPos;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea una pared de forma dinamica y devuelve el handle. Si el handle 
//   recibido es nulo, se entendera que no se pudo crear la criatura y se
//   interrumpira el script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOCreateWallInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pBlockAccessFlag = pScript->Pop();
  ASSERT(pBlockAccessFlag);
  CScriptStackValue* pLight = pScript->Pop();
  ASSERT(pLight);
  CScriptStackValue* pElevation = pScript->Pop();
  ASSERT(pElevation);
  CScriptStackValue* pTag = pScript->Pop();
  ASSERT(pTag);
  CScriptStackValue* pYPos = pScript->Pop();
  ASSERT(pYPos);
  CScriptStackValue* pXPos = pScript->Pop();
  ASSERT(pXPos);
  CScriptStackValue* pProfile = pScript->Pop();
  ASSERT(pProfile);
  
  // Intenta crear la entidad
  const AreaDefs::sTilePos TilePos(pXPos->GetFloatValue(), pYPos->GetFloatValue());
  const AreaDefs::EntHandle hEntity = SYSEngine::GetWorld()->CreateWall(pProfile->GetStringValue(),
																		TilePos,
																		pTag->GetStringValue(),
																		pElevation->GetFloatValue(),
																		pLight->GetFloatValue(),
																		pBlockAccessFlag->GetFloatValue() < 1.0f ? false : true);
  if (hEntity) {
	// Se creo, se coloca el handle en pila
	CScriptStackValue* const pResult = new CScriptStackValue(dword(hEntity));
	ASSERT(pResult);
	pScript->Push(pResult);
  } else {
	// Hubo problemas
	pScript->ErrorInterrupt();
  }  

  // Borra parametros
  delete pBlockAccessFlag;
  delete pLight;
  delete pElevation;
  delete pProfile;
  delete pYPos;
  delete pXPos;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un obj. de escenario de forma dinamica y devuelve el handle. Si el 
//   handle recibido es nulo, se entendera que no se pudo crear la criatura 
//   y se interrumpira el script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOCreateScenaryObjectInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pLight = pScript->Pop();
  ASSERT(pLight);
  CScriptStackValue* pElevation = pScript->Pop();
  ASSERT(pElevation);
  CScriptStackValue* pTag = pScript->Pop();
  ASSERT(pTag);
  CScriptStackValue* pYPos = pScript->Pop();
  ASSERT(pYPos);
  CScriptStackValue* pXPos = pScript->Pop();
  ASSERT(pXPos);
  CScriptStackValue* pProfile = pScript->Pop();
  ASSERT(pProfile);
  
  // Intenta crear la entidad
  const AreaDefs::sTilePos TilePos(pXPos->GetFloatValue(), pYPos->GetFloatValue());
  const AreaDefs::EntHandle hEntity = SYSEngine::GetWorld()->CreateSceneObj(pProfile->GetStringValue(),
																			TilePos,																		    
																			pTag->GetStringValue(),
																			pElevation->GetFloatValue(),
																			pLight->GetFloatValue());
  if (hEntity) {
	// Se creo, se coloca el handle en pila
	CScriptStackValue* const pResult = new CScriptStackValue(dword(hEntity));
	ASSERT(pResult);
	pScript->Push(pResult);
  } else {
	// Hubo problemas
	pScript->ErrorInterrupt();
  }  

  // Borra parametros
  delete pLight;
  delete pElevation;
  delete pProfile;
  delete pYPos;
  delete pXPos;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un item sobre el escenario de forma dinamica y devuelve el handle. 
//   Si el  handle recibido es nulo, se entendera que no se pudo crear la 
//   criatura y se interrumpira el script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOCreateItemAbandonedInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pLight = pScript->Pop();
  ASSERT(pLight);
  CScriptStackValue* pTag = pScript->Pop();
  ASSERT(pTag);
  CScriptStackValue* pYPos = pScript->Pop();
  ASSERT(pYPos);
  CScriptStackValue* pXPos = pScript->Pop();
  ASSERT(pXPos);
  CScriptStackValue* pProfile = pScript->Pop();
  ASSERT(pProfile);

  // Intenta crear la entidad
  const AreaDefs::sTilePos TilePos(pXPos->GetFloatValue(), pYPos->GetFloatValue());
  const AreaDefs::EntHandle hEntity = SYSEngine::GetWorld()->CreateItem(pProfile->GetStringValue(),
																	    TilePos,	
																		pTag->GetStringValue(),
																		pLight->GetFloatValue());
  if (hEntity) {
	// Se creo, se coloca el handle en pila
	CScriptStackValue* const pResult = new CScriptStackValue(dword(hEntity));
	ASSERT(pResult);
	pScript->Push(pResult);
  } else {
	// Hubo problemas
	pScript->ErrorInterrupt();
  }  

  // Borra parametros
  delete pLight;
  delete pProfile;
  delete pYPos;
  delete pXPos;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un item asociado a un dueño de forma dinamica y devuelve el handle. 
//   Si el  handle recibido es nulo, se entendera que no se pudo crear la 
//   criatura y se interrumpira el script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOCreateItemWithOwnerInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pLight = pScript->Pop();
  ASSERT(pLight);
  CScriptStackValue* pTag = pScript->Pop();
  ASSERT(pTag);
  CScriptStackValue* pOwner = pScript->Pop();
  ASSERT(pOwner);
  CScriptStackValue* pProfile = pScript->Pop();
  ASSERT(pProfile);
  
  // Intenta crear la entidad
  const AreaDefs::EntHandle hEntity = SYSEngine::GetWorld()->CreateItem(pProfile->GetStringValue(),
																		pOwner->GetDWordValue(),																	    
																		pTag->GetStringValue(),
																		pLight->GetFloatValue());
  if (hEntity) {
	// Se creo, se coloca el handle en pila
	CScriptStackValue* const pResult = new CScriptStackValue(dword(hEntity));
	ASSERT(pResult);
	pScript->Push(pResult);
  } else {
	// Hubo problemas
	pScript->ErrorInterrupt();
  }  

  // Borra parametros
  delete pLight;
  delete pProfile;
  delete pOwner;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece, o quita, el sistema horario del universo de juego en pausa, 
//   esto hara que no transcurran las horas de forma natural. En caso de que 
//   se este en modo pausa, se obviara la llamada.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOSetWorldTimePauseInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pFlag = pScript->Pop();
  ASSERT(pFlag);

  // Establece el flag
  SYSEngine::GetWorld()->SetWorldTimePause(pFlag->GetFloatValue() < 1.0f ? false : true);

  // Borra parametros
  delete pFlag;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si esta establecido el sistema horario del universo de juego
//   en pausa y deposita el resultado.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOIsWorldTimeInPauseInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Consulta y deposita el flag
  CScriptStackValue* const pResult = new CScriptStackValue(float(SYSEngine::GetWorld()->IsWorldTimeInPause() ? 1.0f : 0.0f));
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Cambia la elevacion asociada a una celda del escenario.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOSetElevationAtInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pElevation = pScript->Pop();
  ASSERT(pElevation);
  CScriptStackValue* pYPos = pScript->Pop();
  ASSERT(pYPos);
  CScriptStackValue* pXPos = pScript->Pop();
  ASSERT(pXPos);

  // Obtiene el floor asociado
  const AreaDefs::sTilePos TilePos(pXPos->GetFloatValue(), pYPos->GetFloatValue());
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  if (pWorld->IsFloorValid(TilePos)) {
	// Si, obtiene el floor y cambia la elevacion
	CFloor* const pFloor = pWorld->GetFloor(TilePos);
	ASSERT(pFloor);	  
	const RulesDefs::Elevation Elevation = (pElevation->GetFloatValue() < 1) ? 
											0 : pElevation->GetFloatValue();
	pFloor->SetElevation(Elevation);	  	
  } else {
	// La posicion NO es valida
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pElevation;
  delete pYPos;
  delete pXPos;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Pasa el turno solicitado por la entidad que proceda, siempre y cuando
//   se este en modo combate. En caso de que la criatura NO posea el turno
//   se ignorara la llamada.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWONextTurnInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // ¿Se esta en modo combate?
  iCCombatSystem* const pCombatSys = SYSEngine::GetCombatSystem();
  ASSERT(pCombatSys);
  if (pCombatSys->IsCombatActive()) {
	// ¿La entidad es valida?
	if (SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue())) {
	  // Si, se pasa de turno, en caso de no tenerlo se obviara la llamada
	  pCombatSys->NextTurn(pHandle->GetDWordValue());	
	} else {
	  // No, la entidad no es valida
	  pScript->ErrorInterrupt();
	}
  } else {
	// Criatura sin turno
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la luz asociada a una determinada posicion, de esta forma se
//   podra conocer el grado de luminosidad que haya en la misma.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOGetLightAtInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pYPos = pScript->Pop();
  ASSERT(pYPos);
  CScriptStackValue* pXPos = pScript->Pop();
  ASSERT(pXPos);

  // ¿Posicion valida?
  const AreaDefs::sTilePos TilePos(pXPos->GetFloatValue(), pYPos->GetFloatValue());
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  if (pWorld->IsFloorValid(TilePos)) {
	// Si, obtiene el valor luminoso y lo deposita en pila
	CScriptStackValue* const pReturn = new CScriptStackValue(float(pWorld->GetLightAt(TilePos)));
	ASSERT(pReturn);
	pScript->Push(pReturn);
  } else {
	// Posicion NO valida
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pYPos;
  delete pXPos;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Reproduce un sonido anonimo en el universo de juego
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOPlayWAVSoundInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pWAVSound = pScript->Pop();
  ASSERT(pWAVSound);

  // ¿Reproduccion NO valida?
  if (!SYSEngine::GetWorld()->PlayWAVSound(pWAVSound->GetStringValue())) {
	// Si, problemas
	pScript->ErrorInterrupt();
  } 

  // Borra parametros
  delete pWAVSound;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece / quita scripts asociados a un floor. Solo se antendera a los
//   eventos de SetIn y SetOut.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CWOSetScriptAtInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pStrScript = pScript->Pop();
  ASSERT(pStrScript);
  CScriptStackValue* pYPos = pScript->Pop();
  ASSERT(pYPos);
  CScriptStackValue* pXPos = pScript->Pop();
  ASSERT(pXPos);
  CScriptStackValue* pScriptType = pScript->Pop();
  ASSERT(pScriptType);

  // ¿Posicion valida?
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  const AreaDefs::sTilePos Pos(sword(pXPos->GetFloatValue()),
							   sword(pYPos->GetFloatValue()));
  if (pWorld->IsFloorValid(Pos)) {
	// Si, toma floor
	CFloor* const pFloor = pWorld->GetFloor(Pos);
	ASSERT(pFloor);

	// ¿Tipo de script valido?
	const RulesDefs::eScriptEvents Script = RulesDefs::eScriptEvents(sword(pScriptType->GetFloatValue()));
	if (Script == RulesDefs::SE_ONSETIN ||
		Script == RulesDefs::SE_ONSETOUT) {
	  // Se establece / quita
	  pFloor->SetScriptEvent(Script, pStrScript->GetStringValue());
	} else {
	  // Tipo no valido
	  pScript->ErrorInterrupt();
	}
  } else {
	// Posicion no valida
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pStrScript;
  delete pXPos;
  delete pYPos;
  delete pScriptType;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el nombre de una entidad y lo deposita. En caso de que no exista
//   la entidad, se detendra el script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOGetNameInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad
  CScriptStackValue* pEntity = pScript->Pop();
  ASSERT(pEntity);

  // Se toma instancia a la entidad y deposita el nombre
  CScriptStackValue* pName = NULL;  
  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(pEntity->GetDWordValue());
  if (pWorldEntity) {
	pName = new CScriptStackValue(pWorldEntity->GetName());
	ASSERT(pName);
	pScript->Push(pName);
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pEntity;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece / cambia el nombre de la entidad.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOSetNameInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad
  CScriptStackValue* pName = pScript->Pop();
  ASSERT(pName);
  CScriptStackValue* pEntity = pScript->Pop();
  ASSERT(pEntity);

  // Se toma instancia a la entidad y establece el nombre
  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(pEntity->GetDWordValue());
  if (pWorldEntity) {
	// Entidad valida, cambia el nombre
	pWorldEntity->SetName(pName->GetStringValue());
  } else {
	// Entidad no valida
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pName;
  delete pEntity;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el atributo tipo de una entidad.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOGetTypeInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Dependiendo del tipo, creara un valor numerico
  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// Obtiene el valor de tipo y lo deposita
	CScriptStackValue* pType = new CScriptStackValue(float(pWorldEntity->GetType()));
	ASSERT(pType);
	pScript->Push(pType);
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el tipo de la entidad. Los tipos seran:
//   * Si no es entidad 0 (handle nulo)
//   * Si es un Player 1
//   * Si es un Npc 2
//   * Si es un SceneObj 3
//   * Si es un Wall 4
//   * Si es un Item 5
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOGetEntityTypeInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Dependiendo del tipo, creara un valor numerico
  CScriptStackValue* pType = NULL;
  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// Segun el tipo, se creara el valor adecuado para escribirlo
	switch(pWorldEntity->GetEntityType()) {
	  case RulesDefs::PLAYER: {
		pType = new CScriptStackValue(1.0f);
	  } break;

	  case RulesDefs::CRIATURE: {
		pType = new CScriptStackValue(3.0f);
	  } break;

	  case RulesDefs::SCENE_OBJ: {
		pType = new CScriptStackValue(2.0f);
	  } break;
	  	  
	  case RulesDefs::WALL: {
		pType = new CScriptStackValue(4.0f);
	  } break;

	  case RulesDefs::ITEM: {
		pType = new CScriptStackValue(5.0f);
	  } break;

	}; // ~ switch
	ASSERT(pType);
	pScript->Push(pType);
  } else {
	pType = new CScriptStackValue(0.0f);
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retornara el identificador de la clase de la entidad. La entidad podra
//   ser un Item, un Objeto de Escenario o bien una Criatura
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOGetClassInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Toma instancia a la entidad que proceda
  CScriptStackValue* pClass = NULL;  
  // ¿Es un item?
  CItem* const pItem = SYSEngine::GetWorld()->GetItem(pHandle->GetDWordValue());
  if (pItem) {
	pClass = new CScriptStackValue(float(pItem->GetClass()));
	ASSERT(pClass);
  } else {
	// ¿Es una criatura?
	CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
	if (pCriature) {
	  pClass = new CScriptStackValue(float(pCriature->GetClass()));
	  ASSERT(pClass);
	  pScript->Push(pClass);
	} else {
	  // ¿Es un objeto de escenario?
	  CSceneObj* const pSceneObj = SYSEngine::GetWorld()->GetSceneObj(pHandle->GetDWordValue());
	  if (pSceneObj) {
		pClass = new CScriptStackValue(float(pSceneObj->GetClass()));
		ASSERT(pClass);
		pScript->Push(pClass);
	  } else {
		// No es una entidad valida
		pScript->ErrorInterrupt();
	  }
	}
  }
  
  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el coste de uso del item en combate. Si el coste es cero se
//   entendera que el item no es para combate.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOGetInCombatUseCostInstr::Execute(iCScript* const pScript)
{
    // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle); 

  // Se toma instancia a la entidad, DEBERA de ser item
  bool bExecuteOk = false;
  RulesDefs::CriatureActionPoints Value;	    
  CItem* const pItem = SYSEngine::GetWorld()->GetItem(pHandle->GetDWordValue());
  if (pItem) {	
	// Se obtiene el coste
	iCRulesDataBase* const pRules = SYSEngine::GetGameDataBase()->GetRulesDataBase();
	ASSERT(pRules);	
    Value = pRules->GetUseWeaponItemCost(pItem->GetEntityType());
    bExecuteOk = true;
  }
  
  // ¿Hubo problemas?
  if (!bExecuteOk) {
	// Si, se interrumpe script
	pScript->ErrorInterrupt();
  } else {
	// No, se deposita resultado
	CScriptStackValue* const pResult = new CScriptStackValue(float(Value));
	ASSERT(pResult);
	pScript->Push(pResult);		
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el atributo global asociado a un item.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOGetGlobalAttributeInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pIDGlobal = pScript->Pop();
  ASSERT(pIDGlobal);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle); 

  // Se toma instancia a la entidad, DEBERA de ser item
  bool bExecuteOk = false;
  RulesDefs::ItemGlobalAttrib Value;	    
  CItem* const pItem = SYSEngine::GetWorld()->GetItem(pHandle->GetDWordValue());
  if (pItem) {
	// ¿Identificador dentro del rango?	
	iCRulesDataBase* const pRules = SYSEngine::GetGameDataBase()->GetRulesDataBase();
	ASSERT(pRules);	
    if (pIDGlobal->GetFloatValue() >= 0.0f &&
	    pIDGlobal->GetFloatValue() < pRules->GetNumEntitiesIDs(RulesDefs::ID_ITEM_GLOBALATTRIB, pItem->GetType())) {
	  // Obtiene el valor del atributo global
	  Value = pItem->GetGlobalAttribute(pIDGlobal->GetFloatValue());
	  bExecuteOk = true;
	}
  }
  
  // ¿Hubo problemas?
  if (!bExecuteOk) {
	// Si, se interrumpe script
	pScript->ErrorInterrupt();
  } else {
	// No, se deposita resultado
	CScriptStackValue* const pResult = new CScriptStackValue(float(Value));
	ASSERT(pResult);
	pScript->Push(pResult);		
  }

  // Borra parametros
  delete pIDGlobal;
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establecera un nuevo valor para un atributo global de item
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOSetGlobalAttributeInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pValue = pScript->Pop();
  ASSERT(pValue);
  CScriptStackValue* pIDGlobal = pScript->Pop();
  ASSERT(pIDGlobal);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle); 

  // Se toma instancia a la entidad, DEBERA de ser item
  CItem* const pItem = SYSEngine::GetWorld()->GetItem(pHandle->GetDWordValue());
  if (pItem) {
	// ¿Identificador dentro del rango?	
	iCRulesDataBase* const pRules = SYSEngine::GetGameDataBase()->GetRulesDataBase();
	ASSERT(pRules);	
    if (pIDGlobal->GetFloatValue() >= 0.0f &&
	    pIDGlobal->GetFloatValue() < pRules->GetNumEntitiesIDs(RulesDefs::ID_ITEM_GLOBALATTRIB, pItem->GetType())) {
	  // Se establece el valor del atributo global
	  pItem->SetGlobalAttribute(pIDGlobal->GetFloatValue(), pValue->GetFloatValue());
	} else {
	  // Identificador de atributo NO valido
	  pScript->ErrorInterrupt();
	}
  } else {
	// Entidad NO valida
	pScript->ErrorInterrupt();
  }
  
  // Borra parametros
  delete pValue;
  delete pIDGlobal;
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia.
// - En caso de estar instalada como cliente de una entidad que se halla 
//   hablando, ordenara callar a la entidad para poderse finalizar de forma
//   correcta.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEOSayInstr::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {  
	// Libera info y propaga
	ReleaseInfo();
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Hace hablar a una entidad
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOSayInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pWaitForActionFlag = pScript->Pop();
  ASSERT(pWaitForActionFlag);
  CScriptStackValue* pJustify = pScript->Pop();
  ASSERT(pJustify);
  CScriptStackValue* pRGBColor = pScript->Pop();
  ASSERT(pRGBColor);
  CScriptStackValue* pTextTime = pScript->Pop();
  ASSERT(pTextTime);
  CScriptStackValue* pText = pScript->Pop();
  ASSERT(pText);
  CScriptStackValue* pWAVFileName = pScript->Pop();
  ASSERT(pWAVFileName);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Toma instancia
  bool bResult = false;
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CWorldEntity* const pEntity = pWorld->GetWorldEntity(pHandle->GetDWordValue());
  if (pEntity) {
	// Prepara posibles parametros
	// Color posible texto
	const float fRGBColor = pRGBColor->GetFloatValue();
	const GraphDefs::sRGBColor RGBColor(byte((dword(fRGBColor) >> 16) & 0x000000FF), 
										byte((dword(fRGBColor) >> 8) & 0x000000FF), 
										byte(dword(fRGBColor) & 0x000000FF));
	
	// Tiempo
	float fTextTime = pTextTime->GetFloatValue();
	if (fTextTime < 0.0f) {
	  fTextTime = 0.0f;
	}
	
	// Justificacion
	CWorldEntity::eTalkTextJustify TextJustify;	
	const float fJustify = pJustify->GetFloatValue();
	if (fJustify == 1.0f) {
	  TextJustify = CWorldEntity::UP_JUSTIFY;
	} else if (fJustify == 2.0f) {
	  TextJustify = CWorldEntity::LEFT_JUSTIFY;
	} else {
	  TextJustify = CWorldEntity::RIGHT_JUSTIFY;
	}

	// Cliente
	iCAlarmClient* const pClient = (pWaitForActionFlag->GetFloatValue() < 1.0f) ? NULL : this;

	// Texto
	const std::string szText(pText->GetStringValue());

	// ¿Fichero WAV asociado?
	const std::string szWAVFileName(pWAVFileName->GetStringValue());
	if (!szWAVFileName.empty()) {
	  // ¿Texto asociado?
	  if (!szText.empty()) {
		// Si, texto con fichero wav
		bResult = pEntity->Talk(szWAVFileName, szText, RGBColor, TextJustify, fTextTime, pClient);
	  } else {
		// No, solo fichero wav
		bResult = pEntity->Talk(szWAVFileName, pClient);
	  }	  
	} else {
	  // No, solo texto
	  bResult = pEntity->Talk(szText, RGBColor, TextJustify, fTextTime, pClient);
	}	

	// Si, ¿se debe de esperar?
	if (bResult && pClient) {	
	  // Si, se pausa y se instala como observer
	  pScript->Pause();
	  m_pScript = pScript;
	  m_hEntity = pEntity->GetHandle();
	  pWorld->AddObserver(this);
	}
  }

  // ¿Hubo problemas?
  if (!bResult) {
	// Si, se interrumpe
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pWAVFileName;
  delete pText;
  delete pTextTime;
  delete pRGBColor;
  delete pJustify;
  delete pWaitForActionFlag;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe la notificacion en la finalizacion del habla a traves de una
//   simulacion de la finalizacion de alarma controladora que realmente se
//   habra instalado en la entidad.
// Parametros:
// - IDCommand. Identificador del comando que ha finalizado.
// - udInstant. Instante en el que se produjo la finalizacion del comando.
// Devuelve:
// Notas:
// - Se recibira SIEMPRE handle 0 a la alarma.
///////////////////////////////////////////////////////////////////////////////
void 
CEOSayInstr::AlarmNotify(const AlarmDefs::eAlarmType& AlarmType,
				         const AlarmDefs::AlarmHandle& hAlarm)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Hay script asociado?
  if (m_pScript) {
	// Comprueba tipo de alarma
	switch(AlarmType) {
	  case AlarmDefs::WAV_ALARM:
	  case AlarmDefs::TIME_ALARM: {
		// Se continua el script y libera info
		// Nota: Se desvincula handle a entidad para indicar que se reicibio
		// notificacion.
  		m_pScript->Resume();
		m_hEntity = 0;
		ReleaseInfo();
	  } break;

	  default: 
		ASSERT(false);
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion del universo de juego. Se utilizara para conocer si este
//   ha dejado de estar en pausa a causa de la ejecucion de una presentacion
//   y se puede activar de nuevo el script.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOSayInstr::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
								 const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Instruccion actual
  if (m_pScript) {
	// Si, comprueba que la notificacion sea la buscada
	switch(NotifyType) {
	  case WorldObserverDefs::ENTITY_DESTROY: {
		// La entidad destruida es la asociada
		if (udParam == m_hEntity) {
		  // Si, prosigue con el script y libera informacion
		  m_pScript->Resume();
		  ReleaseInfo();
		}
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desvincula informacion
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOSayInstr::ReleaseInfo(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Libera informacion
  // Nota: Se supone que desde el exterior se determinara que hacer con
  // la ejecucion del script asociado.
  if (m_pScript) {	
	m_pScript = NULL;
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);
	pWorld->RemoveObserver(this);
	// En caso de que no se haya recibido notificacion, se mandara callar
	if (m_hEntity) {
	  CWorldEntity* const pEntity = pWorld->GetWorldEntity(m_hEntity);
	  if (pEntity) {
		pEntity->ShutUp();
		m_hEntity = 0;
	  }
	}	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza cualquier accion de hablar en la entidad que proceda
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOShutUpInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Hace que deje de hablar
  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	pWorldEntity->ShutUp();
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si la entidad se halla hablando, en cuyo caso retornara un
//   valor de 1 y si no lo esta de 0.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOIsSayingInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Hace que deje de hablar
  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// Obtiene valor
	CScriptStackValue* pResult = new CScriptStackValue(pWorldEntity->IsTalking() ? 1.0f : 0.0f);
	ASSERT(pResult);
	pScript->Push(pResult);
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia un grafico de GFX a una entidad.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOAttachGFXInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pGFXProfile = pScript->Pop();
  ASSERT(pGFXProfile);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Toma handle a entidad
  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// Asocia
	SYSEngine::GetGFXManager()->AttachGFX(pWorldEntity->GetHandle(), pGFXProfile->GetStringValue());
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pGFXProfile;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desasocia GFX en una entidad
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOReleaseGFXInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pGFXProfile = pScript->Pop();
  ASSERT(pGFXProfile);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Toma handle a entidad
  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// Libera
	SYSEngine::GetGFXManager()->ReleaseGFX(pWorldEntity->GetHandle(), pGFXProfile->GetStringValue());
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pGFXProfile;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera todos los GFX asociados a una entidad
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOReleaseAllGFXInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Toma handle a entidad
  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// Libera
	SYSEngine::GetGFXManager()->ReleaseAllGFX(pWorldEntity->GetHandle());
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si un GFX esta asociado, o no, a una entidad. Se depositara un
//   valor numerico de 1 si lo esta y 0 si no lo esta.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOIsGFXAttachedInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pGFXProfile = pScript->Pop();
  ASSERT(pGFXProfile);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Toma handle a entidad
  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// Comprueba y escribe resultado
	CScriptStackValue* pReturn;
	if (SYSEngine::GetGFXManager()->IsGFXAttached(pWorldEntity->GetHandle(), pGFXProfile->GetStringValue())) {
	  pReturn = new CScriptStackValue(1.0f);
	} else {
	  pReturn = new CScriptStackValue(0.0f);
	}
	ASSERT(pReturn);
	pScript->Push(pReturn);
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pGFXProfile;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el numero de elementos que se hallan en un contendor. Si no es
//   un contenedor, se depositara un valor nulo.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOGetNumItemsInContainerInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Toma handle a entidad
  CScriptStackValue* pNumItems = NULL;
  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// Handle valido, se obtiene instancia al contenedor
	iCItemContainer* const pItemContainer = pWorldEntity->GetItemContainer();
	if (pItemContainer) {
	  // Se obtiene el tamaño
	  pNumItems = new CScriptStackValue(float(pItemContainer->GetSize()));
	} else {
	  // No es contenedor
	  pNumItems = new CScriptStackValue(0.0f);
	}	
	ASSERT(pNumItems);
	pScript->Push(pNumItems);
  } else {
	// No se paso un handle valido
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el handle al item en el contendor, que se halle en la posicion
//   pasada como parametro. En caso de que la entidad no sea contenedor o no
//   se halle item alguno, se depositara un handle nulo.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOGetItemFromContainerInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pPos = pScript->Pop();
  ASSERT(pPos);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Toma handle a entidad
  CScriptStackValue* pItem = NULL;
  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// Handle valido, se obtiene instancia al contenedor
	iCItemContainer* const pItemContainer = pWorldEntity->GetItemContainer();
	if (pItemContainer) {
	  // Si es contenedor, se intenta localizar el handle
	  CItemContainerIt It(*pItemContainer);
	  word uwCont = 0;
	  It.SetAtFront();
	  for (; It.IsValid(); It.Next(), ++uwCont) {
		// ¿Se alcanzo la posicion
		if (uwCont == word(pPos->GetFloatValue())) {
		  // Si, se toma el handle y se abandona
		  pItem = new CScriptStackValue(dword(It.GetItem()));
		  ASSERT(pItem);
		  break;
		} else if (uwCont > word(pPos->GetFloatValue())) {
		  // Se supero la posicion, se establece handle nulo y abandona
		  pItem = new CScriptStackValue(dword(0));
		  ASSERT(pItem);
		  pScript->Push(pItem);
		  break;
		}
	  }
	} else {
	  // No es contenedor
	  pItem = new CScriptStackValue(dword(0));
	  ASSERT(pItem);
	  pScript->Push(pItem);
	}
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pPos;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Indicara si el item pasado se halla en el contenedor.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOIsItemInContainerInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pEntity = pScript->Pop();
  ASSERT(pEntity);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Toma handle a entidad
  CScriptStackValue* pResult = 0;
  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// Handle valido, se obtiene instancia al contenedor
	iCItemContainer* const pItemContainer = pWorldEntity->GetItemContainer();
	if (pItemContainer) {
	  // Se comprueba
	  pResult = new CScriptStackValue(pItemContainer->IsItemIn(pWorldEntity->GetHandle()) ? 1.0f : 0.0f);
	  ASSERT(pResult);
	  pScript->Push(pResult);
	} else {
	  // No es contenedor
	  pResult = new CScriptStackValue(0.0f);
	  ASSERT(pResult);
	  pScript->Push(pResult);
	}	
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pEntity;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Pasara un item de una entidad contenedora a otra. Se debera de cumplir
//   que el item a pasar exista en la entidad actual y que la entidad destino
//   sea contenedora. Cuando alguna de las entidades sean nulas, se detendra
//   la ejecucion del script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOTransferItemToContainerInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros  
  CScriptStackValue* phTargetEntity = pScript->Pop();
  ASSERT(phTargetEntity);
  CScriptStackValue* pItem = pScript->Pop();
  ASSERT(pItem);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);  
  
  // Se toma instancia a la entidad
  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// Se obtiene instancia a la entidad destino
	CWorldEntity* const pTargetEntity = SYSEngine::GetWorld()->GetWorldEntity(phTargetEntity->GetDWordValue());
	if (pTargetEntity) {
	  // Se obtiene contenedor origen y destino
	  iCItemContainer* const pContainerSrc = pWorldEntity->GetItemContainer();
	  iCItemContainer* const pContainerDest = pTargetEntity->GetItemContainer();
	  if (pContainerSrc && pTargetEntity) {
		// ¿Existe el item buscado en el contenedor origen?
		if (pContainerSrc->IsItemIn(pItem->GetDWordValue())) {
		  // Si, se extrae y se deposita en el contenedor destino
		  pContainerSrc->Extract(pItem->GetDWordValue());
		  pContainerDest->Insert(pItem->GetDWordValue());
		} // ~ if (pContainerSrc->IsItemIn(pItem->GetHandle()) {
	  } // ~ if (pContainerSrc && pTargetEntity) {
	} else {
	  pScript->ErrorInterrupt();
	}
  } else {
	pScript->ErrorInterrupt();
  }
  
  // Borra parametros
  delete pHandle;
  delete pItem;
  delete phTargetEntity;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inserta el item recibido en el contenedor actual, suponiendo que la
//   entidad sea en efecto un contenedor. Tambien se debera de cumplir que
//   el item a insertar NO tenga dueño.
//   Si la entidad contenedora o el item no son validos, se detendra.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOInsertItemInContainerInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pItem = pScript->Pop();
  ASSERT(pItem);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Se toma instancia a la entidad
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CWorldEntity* const pWorldEntity = pWorld->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// ¿Es entidad contenedora?
	if (pWorldEntity->GetItemContainer()) {
	  // ¿El item a insertar es una entidad valida?
	  CWorldEntity* const pItemToInsert = pWorld->GetWorldEntity(pItem->GetDWordValue());
	  if (pItemToInsert) {
		// Cambia de localidad
		// Nota: En CWorld se hara un filtro, de tal forma que si el item ya
		// tiene dueño no se realizara la operacion
		pWorld->ChangeLocation(pItemToInsert->GetHandle(), pWorldEntity->GetHandle());
	  } else {
		pScript->ErrorInterrupt();
	  }
	}
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pItem;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Hace que una entidad se desprenda de un ítem.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEORemoveItemOfContainerInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros  
  CScriptStackValue* pXPos = pScript->Pop();
  ASSERT(pXPos);
  CScriptStackValue* pYPos = pScript->Pop();
  ASSERT(pYPos);
  CScriptStackValue* pItemHandle = pScript->Pop();
  ASSERT(pItemHandle);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);  
  
  // Se toma instancia a la entidad
  bool bResult = false;
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CWorldEntity* const pEntity = pWorld->GetWorldEntity(pHandle->GetDWordValue());
  if (pEntity) {
	// ¿La entidad es un contenedor?
	iCItemContainer* const pContainer = pEntity->GetItemContainer();
	if (pContainer) {
	  // ¿Posee el item?
	  if (pContainer->IsItemIn(pItemHandle->GetDWordValue())) {
		CItem* const pItem = pWorld->GetItem(pItemHandle->GetDWordValue());
		ASSERT(pItem);
		// ¿Las posiciones son validas?
		const AreaDefs::sTilePos TileDest(pXPos->GetFloatValue(), 
										  pYPos->GetFloatValue());
		if (pWorld->IsFloorValid(TileDest)) {
		  // Se realiza la operacion
		  bResult = pWorld->ChangeLocation(pItem->GetHandle(), TileDest);
		  bResult = true;
		} 
	  } 
	} 	
  } 

  // ¿Operacion no valida?
  if (!bResult) {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pXPos;
  delete pYPos;
  delete pItemHandle;
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Cambiara de estado de animacion en la entidad que proceda.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOSetAnimTemplateStateInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pAState = pScript->Pop();
  ASSERT(pAState);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Se toma instancia a la entidad
  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// Cambia estado de animacion
	pWorldEntity->GetAnimTemplate()->SetState(pAState->GetFloatValue());
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pAState;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Cambia el estado de animacion en el retrato (si es que hay retrato). Este
//   metodo solo funcionara con las entidades que tengan retrato.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOSetPortraitAnimTemplateStateInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pAState = pScript->Pop();
  ASSERT(pAState);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Se toma instancia a la entidad
  CWorldEntity* const pWorldEntity = SYSEngine::GetWorld()->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// ¿Tiene retrato?
	CSprite* const pPortrait = pWorldEntity->GetPortrait();
	if (pPortrait) {
	  // Si, establece estado
	  pPortrait->GetAnimTemplate()->SetState(pAState->GetFloatValue());
	}
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pAState;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el tiempo de ejecucion del evento Idle asociado a la entidad
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOSetIdleScriptTimeInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pTime = pScript->Pop();
  ASSERT(pTime);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Se toma instancia a la entidad
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CWorldEntity* const pWorldEntity = pWorld->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// Establece el tiempo
	pWorldEntity->SetIdleEventTime(pTime->GetFloatValue());
  } else {
	// Problemas
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pTime;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el valor luminoso sobre un entidad
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - El valor luminoso recibido se acotara para que se ajuste bien a los
//   posible valores a establecer.
///////////////////////////////////////////////////////////////////////////////
void
CEOSetLightInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pLightValue = pScript->Pop();
  ASSERT(pLightValue);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Se toma instancia a la entidad
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CWorldEntity* const pWorldEntity = pWorld->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// Establece valor luminoso, acotando el valor recibido para ajustarlo a
	// valores coherentes de luz
	if (pLightValue->GetFloatValue() > 255.0f) {
	  // Valor por encima del maximo
	  pWorld->SetLight(pWorldEntity->GetHandle(), 255);
	} else if (pLightValue->GetFloatValue() < 0.0f) {
	  // Valor por debajo del minimo
	  pWorld->SetLight(pWorldEntity->GetHandle(), 0);
	} else {
	  // Valor correcto
	  pWorld->SetLight(pWorldEntity->GetHandle(), pLightValue->GetFloatValue());
	}
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pLightValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el valor luminoso de una entidad
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOGetLightInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Se toma instancia a la entidad
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);  
  CWorldEntity* const pWorldEntity = pWorld->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// Toma valor luminoso y lo deposita en la pila
	const GraphDefs::Light Light = pWorld->GetLight(pWorldEntity->GetHandle());
	CScriptStackValue* const pResult = new CScriptStackValue(float(Light));
	ASSERT(pResult);
	pScript->Push(pResult);		
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve la posicion (el tile) en la que se halla una entidad
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOGetXPosInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Se toma instancia a la entidad
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);  
  CWorldEntity* const pWorldEntity = pWorld->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// Toma la posicion en X y la deposita en la pila
	const AreaDefs::sTilePos TilePos(pWorldEntity->GetTilePos());	
	CScriptStackValue* const pResult = new CScriptStackValue(float(TilePos.XTile));
	ASSERT(pResult);
	pScript->Push(pResult);		
  } else {
	// Hubo problemas
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve la posicion (el tile) en la que se halla una entidad
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOGetYPosInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Se toma instancia a la entidad
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);  
  CWorldEntity* const pWorldEntity = pWorld->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// Toma la posicion en Y y la deposita en la pila
	const AreaDefs::sTilePos TilePos(pWorldEntity->GetTilePos());
	CScriptStackValue* const pResult = new CScriptStackValue(float(TilePos.YTile));
	ASSERT(pResult);
	pScript->Push(pResult);		
  } else {
	// Hubo problemas
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la elevacion de la entidad
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOGetElevationInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Se toma instancia a la entidad
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);  
  CWorldEntity* const pEntity = pWorld->GetWorldEntity(pHandle->GetDWordValue());
  if (pEntity) {
	// Toma la elevacion y la deposita
	CScriptStackValue* const pResult = new CScriptStackValue(float(pEntity->GetElevation()));
	ASSERT(pResult);
	pScript->Push(pResult);		
  } else {
	// Entidad no valida
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece la elevacion para una entidad
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOSetElevationInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pValue = pScript->Pop();
  ASSERT(pValue);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Se toma instancia a la entidad
  CWorldEntity* const pEntity = SYSEngine::GetWorld()->GetWorldEntity(pHandle->GetDWordValue());
  if (pEntity) {
	// La entidad NO podra ser un item
	if (pEntity->GetEntityType() != RulesDefs::ITEM) {
	  // Establece la nueva elevacion
	  const RulesDefs::Elevation Elevation = (pValue->GetFloatValue() < 1) ? 
											  0 : pValue->GetFloatValue();
	  pEntity->SetElevation(Elevation);
	} else {
	  // La entidad era un item
	  pScript->ErrorInterrupt();
	}	  
  } else {
	// Entidad no valida
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pValue;
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve el valor del identificador de atributo local pasado. Metodo solo
//   valido para las entidades de tipo pared, item y objetos de escenario.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOGetLocalAttributeInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pIDLocal = pScript->Pop();
  ASSERT(pIDLocal);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle); 

  // Se toma instancia a la entidad, NO podra ser criatura ni jugador
  bool bExecuteOk = false;
  sword swValue;	    
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);  
  CWorldEntity* const pWorldEntity = pWorld->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// ¿No es ni jugador ni criatura?
	iCRulesDataBase* const pRules = SYSEngine::GetGameDataBase()->GetRulesDataBase();
	ASSERT(pRules);	
	const RulesDefs::eEntityType EntityType = pWorld->GetEntityType(pWorldEntity->GetHandle());	
	switch(EntityType) {
	  case RulesDefs::SCENE_OBJ: {
	    // ¿Identificador dentro del rango?
	    if (pIDLocal->GetFloatValue() >= 0.0f &&
			pIDLocal->GetFloatValue() < pRules->GetNumEntitiesIDs(RulesDefs::ID_SCENEOBJ_LOCALATTRIB, pWorldEntity->GetType())) {
	  	  // Obtiene el valor del atributo local
	  	  CSceneObj* const pSceneObj = pWorld->GetSceneObj(pWorldEntity->GetHandle());
	  	  ASSERT(pSceneObj);
	  	  swValue = pSceneObj->GetLocalAttribute(pIDLocal->GetFloatValue());
	  	  bExecuteOk = true;
	    }
	  } break;

	case RulesDefs::ITEM: {
	  // ¿Identificador dentro del rango?
	  if (pIDLocal->GetFloatValue() >= 0.0f &&
		  pIDLocal->GetFloatValue() < pRules->GetNumEntitiesIDs(RulesDefs::ID_ITEM_LOCALATTRIB, pWorldEntity->GetType())) {
		// Obtiene el valor del atributo local
		CItem* const pItem = pWorld->GetItem(pWorldEntity->GetHandle());
		ASSERT(pItem);
		swValue = pItem->GetLocalAttribute(pIDLocal->GetFloatValue());
		bExecuteOk = true;
	  }
	} break;

	case RulesDefs::WALL: {
	  // ¿Identificador dentro del rango?
	  if (pIDLocal->GetFloatValue() >= 0.0f &&
		  pIDLocal->GetFloatValue() <  pRules->GetNumEntitiesIDs(RulesDefs::ID_WALL_LOCALATTRIB, pWorldEntity->GetType())) {
		// Obtiene el valor del atributo local
		CWall* const pWall = pWorld->GetWall(pWorldEntity->GetHandle());
		ASSERT(pWall);
		swValue = pWall->GetLocalAttribute(pIDLocal->GetFloatValue());
		bExecuteOk = true;
	  }
	} break;

	default: {
	 } break;
    }; // ~ switch
  }
  
  // ¿Hubo problemas?
  if (!bExecuteOk) {
	// Si, se interrumpe script
	pScript->ErrorInterrupt();
  } else {
	// No, se deposita resultado
	CScriptStackValue* const pResult = new CScriptStackValue(float(swValue));
	ASSERT(pResult);
	pScript->Push(pResult);		
  }

  // Borra parametros
  delete pIDLocal;
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establecera el valor asociado a un atributo local de un item, pared u
//   objeto de escenario
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOSetLocalAttributeInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pValue = pScript->Pop();
  ASSERT(pValue);
  CScriptStackValue* pIDLocal = pScript->Pop();
  ASSERT(pIDLocal);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle); 

  // Se toma instancia a la entidad, NO podra ser criatura ni jugador
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);  
  CWorldEntity* const pWorldEntity = pWorld->GetWorldEntity(pHandle->GetDWordValue());
  if (pWorldEntity) {
	// ¿No es ni jugador ni criatura?
	iCRulesDataBase* const pRules = SYSEngine::GetGameDataBase()->GetRulesDataBase();
	ASSERT(pRules);	
	const RulesDefs::eEntityType EntityType = pWorld->GetEntityType(pWorldEntity->GetHandle());	
	switch(EntityType) {
	  case RulesDefs::SCENE_OBJ: {
	    // ¿Identificador dentro del rango?
	    if (pIDLocal->GetFloatValue() >= 0.0f &&
			pIDLocal->GetFloatValue() < pRules->GetNumEntitiesIDs(RulesDefs::ID_SCENEOBJ_LOCALATTRIB, pWorldEntity->GetType())) {
	  	  // Establece el valor del atributo local
	  	  CSceneObj* const pSceneObj = pWorld->GetSceneObj(pWorldEntity->GetHandle());
	  	  ASSERT(pSceneObj);
	  	  pSceneObj->SetLocalAttribute(pIDLocal->GetFloatValue(), pValue->GetFloatValue());	  	 
		} else {
		  // Rango no permitido
		  pScript->ErrorInterrupt();
		}
	  } break;

	case RulesDefs::ITEM: {
	  // ¿Identificador dentro del rango?
	  if (pIDLocal->GetFloatValue() >= 0.0f &&
		  pIDLocal->GetFloatValue() < pRules->GetNumEntitiesIDs(RulesDefs::ID_ITEM_LOCALATTRIB, pWorldEntity->GetType())) {
		// Establece el valor del atributo local
		CItem* const pItem = pWorld->GetItem(pWorldEntity->GetHandle());
		ASSERT(pItem);
		pItem->SetLocalAttribute(pIDLocal->GetFloatValue(), pValue->GetFloatValue());		
	  } else {
		// Rango no permitido
		pScript->ErrorInterrupt();
	  }
	} break;

	case RulesDefs::WALL: {
	  // ¿Identificador dentro del rango?
	  if (pIDLocal->GetFloatValue() >= 0.0f &&
		  pIDLocal->GetFloatValue() <  pRules->GetNumEntitiesIDs(RulesDefs::ID_WALL_LOCALATTRIB, pWorldEntity->GetType())) {
		// Establece el valor del atributo local
		CWall* const pWall = pWorld->GetWall(pWorldEntity->GetHandle());
		ASSERT(pWall);
		pWall->SetLocalAttribute(pIDLocal->GetFloatValue(), pValue->GetFloatValue());		
	  } else {
		// Rango no permitido
		pScript->ErrorInterrupt();
	  }
	} break;

	default: {
	  // Tipo de entidad no valido
	  pScript->ErrorInterrupt();	  
	 } break;
    }; // ~ switch
  } else {
	// Entidad no existente
	pScript->ErrorInterrupt();	
  }

  // Borra parametros
  delete pValue;
  delete pIDLocal;
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devolvera el dueño del item. En caso de que el handle no sea valido se 
//   devolvera handle nulo.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a items
///////////////////////////////////////////////////////////////////////////////
void
CEOGetOwnerInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Toma instancia a la entidad
  CScriptStackValue* pOwner = NULL;
  CItem* const pItem = SYSEngine::GetWorld()->GetItem(pHandle->GetDWordValue());
  if (pItem) {
	// Toma dueño
	pOwner = new CScriptStackValue(dword(pItem->GetOwner()));	
	ASSERT(pOwner);
	pScript->Push(pOwner);
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la orientacion de la pared. Depositando 1 si es al suroeste, 2 si
//   es al frente y 3 si es al sureste.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a paredes
///////////////////////////////////////////////////////////////////////////////
void
CEOGetWallOrientationInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Toma instancia a la pared
  CScriptStackValue* pOrientation = NULL;
  CWall* const pWall = SYSEngine::GetWorld()->GetWall(pHandle->GetDWordValue());
  if (pWall) {
	// Es pared, se toma orientacion
	pOrientation = new CScriptStackValue(float(pWall->GetOrientation()));	
	ASSERT(pOrientation);
	pScript->Push(pOrientation);
  } else {
	// No es pared
	pScript->ErrorInterrupt();
  }  

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Bloquea el acceso a una pared
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a paredes
///////////////////////////////////////////////////////////////////////////////
void
CEOBlockAccessInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Toma instancia a la pared
  CWall* const pWall = SYSEngine::GetWorld()->GetWall(pHandle->GetDWordValue());
  if (pWall) {
	// Es pared, se toma orientacion
	pWall->BlockAccess();
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desbloquea el acceso de una pared.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a paredes
///////////////////////////////////////////////////////////////////////////////
void
CEOUnblockAccessInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Toma instancia a la pared
  CWall* const pWall = SYSEngine::GetWorld()->GetWall(pHandle->GetDWordValue());
  if (pWall) {
	// Es pared, se toma orientacion
	pWall->UnblockAccess();
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si una pared se halla bloqueada, devolviendo 1 si lo esta y 
//   0 si no lo esta. En caso de que la entidad no sea valida, se devolvera -1.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a paredes
///////////////////////////////////////////////////////////////////////////////
void
CEOIsAccessBlockedInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Toma instancia a la pared
  CScriptStackValue* pResult = NULL;
  CWall* const pWall = SYSEngine::GetWorld()->GetWall(pHandle->GetDWordValue());
  if (pWall) {
	// Es pared
	pResult = new CScriptStackValue(pWall->IsBlocked() ? 1.0f : 0.0f);
	ASSERT(pResult);
	pScript->Push(pResult);
  } else {
	// No es una pared
	pScript->ErrorInterrupt();
  }  

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece un sintoma sobre una criatura siempre y cuando el identificador
//   sea valido. El identificador podra oscilar entre 1 y el valor maximo, en 
//   caso contrario se recharazara la llamada, deteniendose el script (ya que
//   se paso un handle incorrecto o / y un identificador de sintoma incorrecto).
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOSetSymptomInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros  
  CScriptStackValue* pSetFlag = pScript->Pop();
  ASSERT(pSetFlag);
  CScriptStackValue* pIDSymptom = pScript->Pop();
  ASSERT(pIDSymptom);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
    
  // Obtiene handle a criatura
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// ¿Valor de sintoma valido?	
	if (pIDSymptom->GetFloatValue() >= 1.0f && 
		pIDSymptom->GetFloatValue() <= SYSEngine::GetGameDataBase()->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_SYMPTOM)) {
	  // Se obtiene el valor correcto del identificador
	  const RulesDefs::eIDSymptom Symp = RulesDefs::eIDSymptom(RulesDefs::SYMPTOM_01 << (word(pIDSymptom->GetFloatValue()) - 1));
	  pCriature->SetSymptom(Symp, pSetFlag->GetFloatValue() < 1.0f ? false : true);
	} else {
	  pScript->ErrorInterrupt();
	}
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pIDSymptom;
  delete pSetFlag;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si un sintoma esta activo en la criatura. El identificador podra 
//   oscilar entre 1 y el maximo, en caso contrario se recharazara la llamada 
//   deteniendo al script. Si esta activo se colocara 1 y si no lo esta 0.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOIsSymptomActiveInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pIDSymptom = pScript->Pop();
  ASSERT(pIDSymptom);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene handle a criatura
  CScriptStackValue* pReturn = NULL;
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// ¿Valor de sintoma valido?	
	if (pIDSymptom->GetFloatValue() >= 1.0f && 
		pIDSymptom->GetFloatValue() <= SYSEngine::GetGameDataBase()->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_SYMPTOM)) {	
	  // Se obtiene el valor correcto del identificador
	  const RulesDefs::eIDSymptom Symp = RulesDefs::eIDSymptom(RulesDefs::SYMPTOM_01 << (word(pIDSymptom->GetFloatValue()) - 1));
	  pReturn = new CScriptStackValue(pCriature->IsSymptomActive(Symp) ? 1.0f : 0.0f);
	  ASSERT(pReturn);
	  pScript->Push(pReturn);
	}
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pIDSymptom;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retorna el genero de la criatura. Si la criatura no es valida -1.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOGetGenreInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene handle a criatura
  CScriptStackValue* pReturn = NULL;
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Se toma el genero
	pReturn = new CScriptStackValue(float(pCriature->GetGenre()));
	ASSERT(pReturn);
	pScript->Push(pReturn);
  } else {
	// Criatura no valida
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la salud de la criatura, para ello esperara recibir por parametro
//   el valor a utilizar. Si se lee 1 retornara el valor temporal y 2 el base.
//   Si se pasa un valor distinto se obviara deteniendo al script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOGetHealthInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pValueType = pScript->Pop();
  ASSERT(pValueType);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene handle a criatura
  CScriptStackValue* pReturn = NULL;
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Se comprueba el tipo a utilizar
	if (pValueType->GetFloatValue() == 0.0f) {
	  // Valor temporal
	  pReturn = new CScriptStackValue(float(pCriature->GetHealth(RulesDefs::TEMP_VALUE)));
	  ASSERT(pReturn);
	  pScript->Push(pReturn);
	} else if (pValueType->GetFloatValue() == 1.0f) {
	  // Valor base
	  pReturn = new CScriptStackValue(float(pCriature->GetHealth(RulesDefs::BASE_VALUE)));
	  ASSERT(pReturn);
	  pScript->Push(pReturn);
	} else {
	  // Valor no valido
	  pScript->ErrorInterrupt();
	}
  } else {
	// Criatura no valida
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pValueType;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOSetHealthInstr::End(void)
{
  // ¿Instruccion inicializada?
  if (Inherited::IsInitOk()) {
	// Libera info y propaga
	ReleaseInfo();	
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el valor de salud en la criatura. Se esperara recibir el tipo
//   de valor a modificar y el valor propiamente dicho a establecer. Para
//   indicar el tipo de valor sobre el que actuar, se esperara recibir un 1 si
//   es el valor temporal o un 2 si es el base. Cuando no exista la criatura
//   o bien se pase mal el valor el script se detendra.
// - Si al establecer la nueva salud ha existido un cambio de animacion, el
//   script se pausara y se instalara la instruccion como observer.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOSetHealthInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros  
  CScriptStackValue* pValue = pScript->Pop();
  ASSERT(pValue);
  CScriptStackValue* pValueType = pScript->Pop();
  ASSERT(pValueType);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
    
  // Obtiene handle a criatura
  CScriptStackValue* pReturn = NULL;
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CCriature* const pCriature = pWorld->GetCriature(pHandle->GetDWordValue());
  if (pCriature && 
	  pValueType->GetFloatValue() <= 1.0f) {
	// Se establece valor
	if (pCriature->SetHealth(RulesDefs::ePairValueType(dword(pValueType->GetFloatValue())), 
	                         pValue->GetFloatValue())) {
	  // Hubo cambio de animacion, luego se pausara el script y se instalara
	  // como obsrver de la criatura y el universo de juego
	  pScript->Pause();
	  m_pScript = pScript;
	  pCriature->AddObserver(this);
	  m_hEntity = pCriature->GetHandle();
	  pWorld->AddObserver(this);
	}
  } else {
    pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pValueType;
  delete pValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe notificacion de la criatura, debiendose atender a las relativas a
//   la modificacion de salud y muerte.
// Parametros:
// - hCriature. Handle a la criatura.
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEOSetHealthInstr::CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
									      const CriatureObserverDefs::eObserverNotifyType& NotifyType,
										  const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hCriature);

  // Se comprueba el tipo de notificacion recibida
  switch (NotifyType) {
	case CriatureObserverDefs::HEALTH_MODIFY: 
	case CriatureObserverDefs::IS_DEATH: {
	  // Continua la ejecucion del script y libera info
	  ASSERT(m_pScript);
	  m_pScript->Resume();
	  ReleaseInfo();	  
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Atendera por una posible notifiacion sobre la destruccion de la entidad
//   asociada a la instruccion, en cuyo caso proseguira con la ejecucion del
//   script.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOSetHealthInstr::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
									   const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Instruccion actual
  if (m_pScript) {
	// Si, comprueba que la notificacion sea la buscada
	switch(NotifyType) {
	  case WorldObserverDefs::ENTITY_DESTROY: {
		// La entidad destruida es la asociada
		if (udParam == m_hEntity) {
		  // Si, prosigue con el script y libera informacion
		  m_pScript->Resume();
		  ReleaseInfo();
		}
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desvincula informacion
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOSetHealthInstr::ReleaseInfo(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Libera informacion
  // Nota: Se supone que desde el exterior se determinara que hacer con
  // la ejecucion del script asociado.
  if (m_pScript) {	
	m_pScript = NULL;
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);
	pWorld->RemoveObserver(this);
	CCriature* const pCriature = pWorld->GetCriature(m_hEntity);
	if (pCriature) {
	  pCriature->RemoveObserver(this);
	  m_hEntity = 0;
	}	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el valor extendido asociado a una criatura. Se esperara recibir
//   el identificador del valor extendido (que oscilara entre 1 y el num. 
//   maximo de valores extendidos) y el tipo de valor a recoger (que valdra 1
//   para el temporal y 2 para el base). En caso de no leerse correctamente
//   algun parametro, se detendra el script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOGetExtendedAttributeInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros  
  CScriptStackValue* pValueType = pScript->Pop();
  ASSERT(pValueType);
  CScriptStackValue* pIDAttribute = pScript->Pop();
  ASSERT(pIDAttribute);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
    
  // Obtiene handle a criatura
  CScriptStackValue* pHealth = NULL;
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Se comprueba el identificador del atributo extendido
	if (pIDAttribute->GetFloatValue() >= 0.0f && 
		pIDAttribute->GetFloatValue() < SYSEngine::GetGameDataBase()->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB)) {
	  // Se comprueba el tipo a utilizar
	  const RulesDefs::eIDExAttrib ExtAttrib = RulesDefs::eIDExAttrib(sword(pIDAttribute->GetFloatValue()));
	  if (pValueType->GetFloatValue() == 0.0f) {
		// Valor temporal
		pHealth = new CScriptStackValue(float(pCriature->GetExAttribute(ExtAttrib, RulesDefs::TEMP_VALUE)));
		ASSERT(pHealth);
		pScript->Push(pHealth);
	  } else if (pValueType->GetFloatValue() == 1.0f) {
		// Valor base		                                                  
		pHealth = new CScriptStackValue(float(pCriature->GetExAttribute(ExtAttrib, RulesDefs::BASE_VALUE)));
		ASSERT(pHealth);
		pScript->Push(pHealth);
	  }  else {
		// Tipo de valor no valido
  		pScript->ErrorInterrupt();
	  }
	}
  } else {
	// Criatura no valida
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pIDAttribute;
  delete pValueType;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el valor extendido asociado a una criatura. Se esperara recibir
//   el identificador del valor extendido (que oscilara entre 1 y el num. 
//   maximo de valores extendidos) y el tipo de valor a recoger (que valdra 1
//   para el temporal y 2 para el base). En caso de no leerse correctamente
//   algun parametro, se detendra el script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOSetExtendedAttributeInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros  
  CScriptStackValue* pValue = pScript->Pop();
  ASSERT(pValue);
  CScriptStackValue* pValueType = pScript->Pop();
  ASSERT(pValueType);
  CScriptStackValue* pIDAttribute = pScript->Pop();
  ASSERT(pIDAttribute);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
    
  // Obtiene handle a criatura
  bool bExecuteOk = false;
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Se comprueba el identificador del atributo extendido
	if (pIDAttribute->GetFloatValue() >= 1.0f && 
		pIDAttribute->GetFloatValue() <= SYSEngine::GetGameDataBase()->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EXTATTRIB)) {
	  // Se comprueba el tipo a utilizar
	  const RulesDefs::eIDExAttrib ExtAttrib = RulesDefs::eIDExAttrib(sword(pIDAttribute->GetFloatValue()));
	  if (pValueType->GetFloatValue() == 0.0f) {
		// Valor temporal
		pCriature->SetExAttribute(ExtAttrib, RulesDefs::TEMP_VALUE, pValue->GetFloatValue());
		bExecuteOk = true;
	  } else if (pValueType->GetFloatValue() == 1.0f) {
		// Valor base
		pCriature->SetExAttribute(ExtAttrib, RulesDefs::BASE_VALUE, pValue->GetFloatValue());
		bExecuteOk = true;
	  } 
	}
  } 
  
  // ¿Hubo problemas?
  if (!bExecuteOk) {	
	// Si, se interrumpe script
	pScript->ErrorInterrupt();
  }
  
  // Borra parametros
  delete pHandle;
  delete pIDAttribute;
  delete pValueType;
  delete pValue;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el nivel de la criatura y lo deposita. 
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOGetLevelInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Obtiene handle a criatura
  CScriptStackValue* pLevel = NULL;
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Obtiene el nivel
	pLevel = new CScriptStackValue(float(pCriature->GetLevel()));
	ASSERT(pLevel);
	pScript->Push(pLevel);
  }  else {
	// Entidad no valida
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el nuevo nivel para la criatura
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOSetLevelInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pNewLevel = pScript->Pop();
  ASSERT(pNewLevel);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene handle a criatura
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Establece el nivel
	pCriature->SetLevel(pNewLevel->GetFloatValue());
  } else {
	// No es criatura
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pNewLevel;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la experiencia de la criatura.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOGetExperienceInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Obtiene handle a criatura
  CScriptStackValue* pExp = NULL;
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Obtiene el experiencia
	pExp = new CScriptStackValue(float(pCriature->GetExperience()));
	ASSERT(pExp);
	pScript->Push(pExp);
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece la experiencia de una criatura
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOSetExperienceInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pNewExp = pScript->Pop();
  ASSERT(pNewExp);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene handle a criatura
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Establece experiencia
	pCriature->SetExperience(pNewExp->GetFloatValue());
  } else {
	// No es criatura
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pNewExp;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene los puntos de accion de la criatura pero sin tener en cuenta
//   si esta o no esta en combate.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOGetActionPointsInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Obtiene instancia
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Se obtienen los puntos de accion y se inserta
	CScriptStackValue* const pAPoints = new CScriptStackValue(float(pCriature->GetActionPoints()));
	ASSERT(pAPoints);
	pScript->Push(pAPoints);
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene los puntos de accion de la criatura en combate. En caso de que no
//   este en combate se retornara 0.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOGetInCombatActionPointsInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Obtiene instancia
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Se obtienen los puntos de accion y se inserta
	CScriptStackValue* const pAPoints = new CScriptStackValue(float(SYSEngine::GetCombatSystem()->GetCriatureActionPoints(pCriature->GetHandle())));
	ASSERT(pAPoints);
	pScript->Push(pAPoints);
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece los puntos de accion de una criatura SOLO si no se esta en
//   modo combate.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOSetActionPointsInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pAPoints = pScript->Pop();
  ASSERT(pAPoints);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // ¿Se esta en modo combate?
  if (SYSEngine::GetCombatSystem()->IsCombatActive()) {
	// Obtiene instancia
	CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
	if (pCriature) {
	  // Se establecen los puntos de accion
	  pCriature->SetActionPoints(pAPoints->GetFloatValue());
	} else {
	  pScript->ErrorInterrupt();
	}
  }

  // Borra parametros
  delete pHandle;
  delete pAPoints;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si una habilidad esta activa. En caso de estarlo, depositara
//   un valor de 1 y en caso de no estarlo de 0. El identificador de la 
//   habilidad oscilara entre 1 y el numero maximo de habilidades. No incluir
//   un valor en ese rango supondra detener el script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOIsHabilityActiveInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pIDHability = pScript->Pop();
  ASSERT(pIDHability);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene criatura
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Toma el numero de habilidades existente y compara
	if (pIDHability->GetFloatValue() >= 1.0f &&
		pIDHability->GetFloatValue() <= SYSEngine::GetGameDataBase()->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY)) {
	  // Se comprueba y se crea el valor numerico adecuado	  
	  const RulesDefs::eIDHability IDHab = RulesDefs::eIDHability(RulesDefs::HABILITY_01 << (word(pIDHability->GetFloatValue())));
	  CScriptStackValue* const pResult = new CScriptStackValue(pCriature->IsHabilityActive(IDHab) ? 1.0f : 0.0f);
	  ASSERT(pResult);
	  pScript->Push(pResult);
	} else {
	  // Identificador no valido
	  pScript->ErrorInterrupt();
	}
  } else {
	// Criatura inexistente
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pIDHability;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece / quita una habilidad. El identificador de la habilidad 
//   oscilara entre 1 y el numero maximo de habilidades. No incluir un valor 
//   en ese rango supondra detener el script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOSetHabilityInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros  
  CScriptStackValue* pSetFlag = pScript->Pop();
  ASSERT(pSetFlag);
  CScriptStackValue* pIDHability = pScript->Pop();
  ASSERT(pIDHability);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
    
  // Obtiene criatura
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Toma el numero de habilidades existente y compara
	if (pIDHability->GetFloatValue() >= 1.0f &&
		pIDHability->GetFloatValue() <= SYSEngine::GetGameDataBase()->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY)) {
	  // Segun sea el flag, establecera o quitara la habilidad
	  const RulesDefs::eIDHability IDHab = RulesDefs::eIDHability(RulesDefs::HABILITY_01 << (word(pIDHability->GetFloatValue())));
	  pCriature->SetHability(IDHab, pSetFlag->GetFloatValue() < 1.0f ? false : true);
	} else {
	  // Identificador no valido
	  pScript->ErrorInterrupt();
	}
  } else {
	// Criatura inexistente
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pIDHability;
  delete pSetFlag;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEOUseHabilityInstr::End(void)
{  
  // Finaliza
  if (Inherited::IsInitOk()) {  
	// Libera info y propaga
	ReleaseInfo();
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ordena a una criatura usar una habilidad sobre una entidad. En caso de que
//   el identificador de habilidad no sea valido o bien no sea usable, se
//   interrumpira la ejecucion del script. Si la habilidad no esta presente
//   en la criatura, la orden no sera ejecutada.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOUseHabilityInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros  
  CScriptStackValue* pWaitFlag = pScript->Pop();
  ASSERT(pWaitFlag);
  CScriptStackValue* pTarget = pScript->Pop();
  ASSERT(pTarget);
  CScriptStackValue* pIDHability = pScript->Pop();
  ASSERT(pIDHability);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Toma instanciancias
  bool bResult = false;
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CCriature* const pCriature = pWorld->GetCriature(pHandle->GetDWordValue());
  CWorldEntity* const pTargetEntity = pWorld->GetWorldEntity(pTarget->GetDWordValue());
  if (pCriature && pTargetEntity) {
	// Filtrado de accion sobre la entidad destino
	if (CheckForValidStateInUseAction(pCriature, pTargetEntity)) {
	  // Comprueba que el identificador de la habilidad sea valida
	  iCRulesDataBase* const pRules = SYSEngine::GetGameDataBase()->GetRulesDataBase();
	  ASSERT(pRules);
	  if (pIDHability->GetFloatValue() >= 1.0f &&
		  pIDHability->GetFloatValue() <= pRules->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_HABILITY)) {
		// Comprueba que la habilidad se pueda utilizar
		const RulesDefs::eIDHability IDHab = RulesDefs::eIDHability(RulesDefs::HABILITY_01 << (word(pIDHability->GetFloatValue())));
		if (pRules->IsUsableHability(IDHab)) {
		  // Usa la habilidad
		  bResult = true;		
		  const RulesDefs::eCriatureActionResult AResult = pCriature->UseHability(pTargetEntity->GetHandle(), IDHab);
		  if (AResult == RulesDefs::CAR_INPROGRESS) {
			// ¿Se desea esperar?
			if (pWaitFlag->GetFloatValue() >= 1.0f) {
			  // Si, se pausa script y se instala como observer
			  pScript->Pause();
			  m_pScript = pScript;
			  m_hEntity = pCriature->GetHandle();
			  pCriature->AddObserver(this);
			  pWorld->AddObserver(this);
			}		  
		  }
		}
	  }
	}
  }

  // ¿Problemas?
  if (!bResult) {
	// Si, se interrumpe script
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pWaitFlag;
  delete pHandle;
  delete pTarget;
  delete pIDHability;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe la notificacion de que se ha realizado el uso de la habilidad.
// - En caso de que la criatura realice otra accion distinta a la esperada,
//   se interrumpira el script pues significara que ha habido una "contraorden"
//   que invalida esta.
// Parametros:
// - hCriature. Handle a la criatura.
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEOUseHabilityInstr::CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
										    const CriatureObserverDefs::eObserverNotifyType& NotifyType,
										    const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hCriature);

  // Se comprueba el tipo de notificacion recibida
  switch (NotifyType) {
	case CriatureObserverDefs::INTERRUPT_MOVING: {
	  // Ha surgido un obstaculo no inicial en el movimiento que SEGURO
	  // se estaba haciendo para la aproximacion en la accion	  
	  // Prosigue script y libera info
	  ASSERT(m_pScript);
	  m_pScript->Resume();
	  ReleaseInfo();
	} break;

	case CriatureObserverDefs::INSUFICIENT_ACTION_POINTS: {
	  // No hay puntos de accion suficientes	
	  ASSERT(m_pScript);
	  m_pScript->ErrorInterrupt();
	  ReleaseInfo();
	} break;

	case CriatureObserverDefs::ACTION_REALICE: {
	  // Se comprueba la accion realizada
	  const RulesDefs::eCriatureAction Action = RulesDefs::eCriatureAction(udParam & 0x0000FFFF);
	  switch(Action) {
	    case RulesDefs::CA_USEHABILITY: 
		case RulesDefs::CA_STOPWALK: {	
		  // Prosigue script y se libera info
		  ASSERT(m_pScript);
		  m_pScript->Resume();
		  ReleaseInfo();
		} break;
	  }; // ~ switch
	} break;	
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Atendera por una posible notifiacion sobre la destruccion de la entidad
//   asociada a la instruccion, en cuyo caso proseguira con la ejecucion del
//   script.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOUseHabilityInstr::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
										 const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Instruccion actual
  if (m_pScript) {
	// Si, comprueba que la notificacion sea la buscada
	switch(NotifyType) {
	  case WorldObserverDefs::ENTITY_DESTROY: {
		// La entidad destruida es la asociada
		if (udParam == m_hEntity) {
		  // Si, prosigue con el script y libera informacion
		  m_pScript->Resume();
		  ReleaseInfo();
		}
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desvincula informacion
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEOUseHabilityInstr::ReleaseInfo(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Libera informacion
  // Nota: Se supone que desde el exterior se determinara que hacer con
  // la ejecucion del script asociado.
  if (m_pScript) {	
	m_pScript = NULL;
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);
	pWorld->RemoveObserver(this);
	CCriature* const pCriature = pWorld->GetCriature(m_hEntity);
	if (pCriature) {
	  pCriature->RemoveObserver(this);
	  m_hEntity = 0;
	}	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el modo correr esta activo.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOIsRunModeActiveInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Obtiene criatura
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Crea resultado y lo coloca en pila
	CScriptStackValue* const pResult = new CScriptStackValue(pCriature->IsInRunMode() ? 1.0f : 0.0f);
	ASSERT(pResult);
	pScript->Push(pResult);
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece / quita el modo correr de la criatura.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOSetRunModeInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pSetFlag = pScript->Pop();
  ASSERT(pSetFlag);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene criatura
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Segun sea el valor del flag, asi actuara
	pCriature->SetRunMode(pSetFlag->GetFloatValue() < 1.0f ? false : true);
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pSetFlag;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEOMoveToInstr::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {  
	// Libera info y propaga
	ReleaseInfo();
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ordena a una criatura moverse.
// - Para tener constancia de otras posibles acciones realizadas, se
//   instalara la instruccion como observer de la criatura.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOMoveToInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pWaitFlag = pScript->Pop();
  ASSERT(pWaitFlag);
  CScriptStackValue* pYPos = pScript->Pop();
  ASSERT(pYPos);
  CScriptStackValue* pXPos = pScript->Pop();
  ASSERT(pXPos);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Obtiene criatura
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CCriature* const pCriature = pWorld->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
    // ¿Posicion valida?
    const AreaDefs::sTilePos TilePos(pXPos->GetFloatValue(), pYPos->GetFloatValue());
    if (pWorld->IsFloorValid(TilePos)) {
  	  // ¿Hay que esperar por la accion?
  	  bool bResult;
  	  if (pWaitFlag->GetFloatValue() >= 1.0f) {
  		// Si, se ejecuta esperando recibir notificacion
		bResult = pCriature->Walk(TilePos,
								  0,
								  this,
								  CEOMoveToInstr::ID_MOVETOINSTRWALKOK,
								  pCriature->GetHandle());

		// ¿Movimiento en progreso?
		if (bResult) {	
		  // Si, se instala como observer y se pausa ejecucion
		  pCriature->AddObserver(this);
		  pScript->Pause();
		  m_pScript = pScript;
		  m_hEntity = pCriature->GetHandle();
		  pWorld->AddObserver(this);
		}	
	  } else {
		// No, se ejecuta sin esperar notificacion
		bResult = pCriature->Walk(TilePos, 0);
	  }
	
	  // Deposita resultado
	  CScriptStackValue* const pResult = new CScriptStackValue(bResult ? 1.0f : 0.0f);
	  ASSERT(pResult);
	  pScript->Push(pResult);
	} else {
	  // Criatura no valida
	  pScript->ErrorInterrupt();
	} 
  } else {
	// Criatura no valida
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pWaitFlag;
  delete pXPos;
  delete pYPos;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion de la realizacion del movimiento o bien de la interrupcion
//   de este.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void 
CEOMoveToInstr::EndCmdNotify(const CommandDefs::IDCommand& IDCommand,
							 const dword udInstant,
							 const dword udExtraParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Si la notificacion llega porque habia que esperar se procede
  if (m_pScript) {
	// Continua y libera info
	m_pScript->Resume();
	ReleaseInfo();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe notificacion de la criatura. En tal caso, si se ha realizado una
//   accion por parte de la misma significara que no se completo la orden
//   inicial de movimiento y se debera de interrumpir la ejecucion de la
//   instruccion
// Parametros:
// - hCriature. Handle a la criatura.
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEOMoveToInstr::CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
									   const CriatureObserverDefs::eObserverNotifyType& NotifyType,
									   const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hCriature);

  // Se comprueba el tipo de notificacion recibida
  if (m_pScript) {
	switch (NotifyType) {	  	  	  	
		case CriatureObserverDefs::ACTION_REALICE: {
		// Se ha realizado un accion, se comprobara que dicha accion
		// sea SOLO de movimiento, en caso contrario se interrumpira el script
		// pues la accion por la que esperaba NO se ha realizado.
		const RulesDefs::eCriatureAction Action = RulesDefs::eCriatureAction(udParam & 0x0000FFFF);
		switch (Action) {
		  case RulesDefs::CA_STOPWALK: {
			// Continua y libera info
			ASSERT(m_pScript);
			m_pScript->Resume();
			ReleaseInfo();
		  } break;
		}; // ~ switch		
	  } break;	
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Atendera por una posible notifiacion sobre la destruccion de la entidad
//   asociada a la instruccion, en cuyo caso proseguira con la ejecucion del
//   script.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOMoveToInstr::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
									const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Instruccion actual
  if (m_pScript) {
	// Si, comprueba que la notificacion sea la buscada
	switch(NotifyType) {
	  case WorldObserverDefs::ENTITY_DESTROY: {
		// La entidad destruida es la asociada
		if (udParam == m_hEntity) {
		  // Si, prosigue con el script y libera informacion
		  m_pScript->Resume();
		  ReleaseInfo();
		}
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desvincula informacion
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEOMoveToInstr::ReleaseInfo(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Libera informacion
  // Nota: Se supone que desde el exterior se determinara que hacer con
  // la ejecucion del script asociado.
  if (m_pScript) {	
	m_pScript = NULL;
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);
	pWorld->RemoveObserver(this);
	CCriature* const pCriature = pWorld->GetCriature(m_hEntity);
	if (pCriature) {
	  pCriature->RemoveObserver(this);
	  m_hEntity = 0;
	}	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Consulta si la criatura se halla en movimiento y deposita el resultado.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOIsMovingInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Obtiene criatura
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Se obtiene el valor numerico indicativo de que se esta moviendo
	CScriptStackValue* pResult = new CScriptStackValue(pCriature->IsWalking() ? 1.0f : 0.0f);
	ASSERT(pResult);
	pScript->Push(pResult);
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de ordenar a la criatura que detenga su movimiento (siempre y 
//   cuando se este moviendo)
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOStopMovingInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Obtiene criatura
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Ordena deternese de forma INMEDIATA    
	pCriature->StopWalk(true);
  } else {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEOEquipItemInstr::End(void)
{  
  // Finaliza
  if (Inherited::IsInitOk()) {  
	// Libera info y propaga
	ReleaseInfo();
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Equipa a la criatura en un slot y con un item. En caso de que la criatura
//   o el slot no correspondan (bien por un handle invalido o bien por un
//   Slot no definido) se detendra el script. En caso de querer equipar un
//   item que no este ni en el inventario ni en otro slot de equipamiento,
//   se obviara la accion.
// - Los slots se esperaran que esten entre 1 y el numero maximo de slots.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOEquipItemInstr::Execute(iCScript* const pScript)
{
  
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros  
  CScriptStackValue* pWaitFlag = pScript->Pop();
  ASSERT(pWaitFlag);  
  CScriptStackValue* pItemToEquip = pScript->Pop();
  ASSERT(pItemToEquip);
  CScriptStackValue* pSlot = pScript->Pop();
  ASSERT(pSlot);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
    
  // Comprueba instancias validas y que el item se posea
  bool bResult = false;
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CCriature* const pCriature = pWorld->GetCriature(pHandle->GetDWordValue());
  CItem* const pItem = pWorld->GetItem(pItemToEquip->GetDWordValue());
  if (pCriature && 
	  pItem && 
	  pItem->GetOwner() == pCriature->GetHandle()) {
	// Comprueba que el slot sea valido
	const RulesDefs::eIDEquipmentSlot Slot = RulesDefs::eIDEquipmentSlot(sword(pSlot->GetFloatValue()));
	if (Slot >= 0.0f &&
		Slot < SYSEngine::GetGameDataBase()->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT)) {
	  // Se intenta equipar
	  bResult = true;	  
	  ASSERT(pItem->GetHandle());
	  if (pCriature->EquipItem(pItem->GetHandle(), Slot) == RulesDefs::CAR_INPROGRESS) {
		// Accion en progreso ¿se deseaba esperar?
		if (pWaitFlag->GetFloatValue() >= 1.0f) {
		  // Si, se pausa script y se instala como observer
		  pScript->Pause();
		  m_pScript = pScript;
		  m_hEntity = pCriature->GetHandle();
		  pCriature->AddObserver(this);
		  pWorld->AddObserver(this);
		}
	  }
	}
  } 

  // ¿Hubo problemas?
  if (!bResult) {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pSlot;
  delete pItemToEquip;
  delete pWaitFlag;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe la notificacion de que se ha equipado el item
// Parametros:
// - hCriature. Handle a la criatura.
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEOEquipItemInstr::CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
										  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
										  const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hCriature);

  // Se comprueba el tipo de notificacion recibida
  switch (NotifyType) {	
	case CriatureObserverDefs::ACTION_REALICE: {
	  // Se comprueba la accion realizada
	  const RulesDefs::eCriatureAction Action = RulesDefs::eCriatureAction(udParam & 0x0000FFFF);
	  switch(Action) {
	    case RulesDefs::CA_EQUIP: {	
		  // Prosigue script y libera info
		  ASSERT(m_pScript);
		  m_pScript->Resume();
		  ReleaseInfo();
		} break;
	  }; // ~ switch
	} break; 	
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Atendera por una posible notifiacion sobre la destruccion de la entidad
//   asociada a la instruccion, en cuyo caso proseguira con la ejecucion del
//   script.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOEquipItemInstr::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
									   const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Instruccion actual
  if (m_pScript) {
	// Si, comprueba que la notificacion sea la buscada
	switch(NotifyType) {
	  case WorldObserverDefs::ENTITY_DESTROY: {
		// La entidad destruida es la asociada
		if (udParam == m_hEntity) {
		  // Si, prosigue con el script y libera informacion
		  m_pScript->Resume();
		  ReleaseInfo();
		}
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desvincula informacion
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEOEquipItemInstr::ReleaseInfo(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Libera informacion
  // Nota: Se supone que desde el exterior se determinara que hacer con
  // la ejecucion del script asociado.
  if (m_pScript) {	
	m_pScript = NULL;
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);
	pWorld->RemoveObserver(this);
	CCriature* const pCriature = pWorld->GetCriature(m_hEntity);
	if (pCriature) {
	  pCriature->RemoveObserver(this);
	  m_hEntity = 0;
	}	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEORemoveItemEquippedInstr::End(void)
{  
  // Finaliza
  if (Inherited::IsInitOk()) {  
	// Libera info y propaga
	ReleaseInfo();
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Elimina un item equipado en un slot de equipamiento y lo retorna al
//   inventario. Se esperara que el slot este entre 1 y el numero maximo de
//   slots, de no ser asi no realizara la operacion, deteniendo al script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEORemoveItemEquippedInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pWaitFlag = pScript->Pop();
  ASSERT(pWaitFlag);  
  CScriptStackValue* pSlot = pScript->Pop();
  ASSERT(pSlot);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene criatura
  bool bResult = false;
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CCriature* const pCriature = pWorld->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Comprueba que el slot sea valido
	if (pSlot->GetFloatValue() >= 0.0f &&
		pSlot->GetFloatValue() < SYSEngine::GetGameDataBase()->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT)) {
	  // Se intenta desequipar el item
	  bResult = true;
	  const RulesDefs::eIDEquipmentSlot Slot = RulesDefs::eIDEquipmentSlot(sword(pSlot->GetFloatValue()));
	  if (pCriature->UnequipItemAt(Slot) == RulesDefs::CAR_INPROGRESS) {
		// Accion en progreso ¿se deseaba esperar?
		if (pWaitFlag->GetFloatValue() >= 1.0f) {
		  // Si, se pausa script y se instala como observer
		  pScript->Pause();
		  m_pScript = pScript;
		  m_hEntity = pCriature->GetHandle();
		  pCriature->AddObserver(this);
		  pWorld->AddObserver(this);
		}
	  }	  
	}
  }
  
  // ¿Hubo problemas?
  if (!bResult) {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pSlot;  
  delete pWaitFlag;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe la notificacion de que se ha equipado el item
// Parametros:
// - hCriature. Handle a la criatura.
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEORemoveItemEquippedInstr::CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
										           const CriatureObserverDefs::eObserverNotifyType& NotifyType,
										           const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hCriature);

  // Se comprueba el tipo de notificacion recibida
  switch (NotifyType) {	
	case CriatureObserverDefs::ACTION_REALICE: {
	  // Se comprueba la accion realizada
	  const RulesDefs::eCriatureAction Action = RulesDefs::eCriatureAction(udParam & 0x0000FFFF);
	  switch(Action) {
	    case RulesDefs::CA_UNEQUIP: {	
		  // Prosigue script y libera info
		  ASSERT(m_pScript);
		  m_pScript->Resume();
		  ReleaseInfo();
		} break;
	  }; // ~ switch
	} break; 	
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Atendera por una posible notifiacion sobre la destruccion de la entidad
//   asociada a la instruccion, en cuyo caso proseguira con la ejecucion del
//   script.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEORemoveItemEquippedInstr::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
												const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Instruccion actual
  if (m_pScript) {
	// Si, comprueba que la notificacion sea la buscada
	switch(NotifyType) {
	  case WorldObserverDefs::ENTITY_DESTROY: {
		// La entidad destruida es la asociada
		if (udParam == m_hEntity) {
		  // Si, prosigue con el script y libera informacion
		  m_pScript->Resume();
		  ReleaseInfo();
		}
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desvincula informacion
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEORemoveItemEquippedInstr::ReleaseInfo(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Libera informacion
  // Nota: Se supone que desde el exterior se determinara que hacer con
  // la ejecucion del script asociado.
  if (m_pScript) {	
	m_pScript = NULL;
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);
	pWorld->RemoveObserver(this);
	CCriature* const pCriature = pWorld->GetCriature(m_hEntity);
	if (pCriature) {
	  pCriature->RemoveObserver(this);
	  m_hEntity = 0;
	}	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el handle al item que se halle equipado en el slot recibido. Si
//   el slot recibido no es valido o bien la entidad sobre la que consultar no
//   existe, se detendra el script.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOGetItemEquippedInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pSlot = pScript->Pop();
  ASSERT(pSlot);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene criatura
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Comprueba que el slot sea valido
	if (pSlot->GetFloatValue() >= 0.0f &&
		pSlot->GetFloatValue() < SYSEngine::GetGameDataBase()->GetRulesDataBase()->GetNumEntitiesIDs(RulesDefs::ID_CRIATURE_EQUIPMENTSLOT)) {
	  // Se obtienen los slots de equipamiento	  
	  iCEquipmentSlots* const pEquipSlots = pCriature->GetEquipmentSlots();
	  ASSERT(pEquipSlots);
	  const RulesDefs::eIDEquipmentSlot Slot = RulesDefs::eIDEquipmentSlot(sword(pSlot->GetFloatValue()));
	  CScriptStackValue* const pItem = new CScriptStackValue(dword(pEquipSlots->GetItemEquipedAt(Slot)));
	  ASSERT(pItem);
	  pScript->Push(pItem);
	} else {
	  // Slot no valido
	  pScript->ErrorInterrupt();
	}
  } else {
	// La criatura no existe
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pSlot;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el item recibido se halla equipado. No se penalizara el hecho
//   de que el item sea nulo, en tal caso simplemente se depositara que el item
//   no se halla equipado. 
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOIsItemEquippedInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* phItem = pScript->Pop();
  ASSERT(phItem);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene criatura
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Se obtienen los slots de equipamiento	  
	iCEquipmentSlots* const pEquipSlots = pCriature->GetEquipmentSlots();
	ASSERT(pEquipSlots);
	CScriptStackValue* const pResult = new CScriptStackValue(pEquipSlots->IsItemEquiped(phItem->GetDWordValue()) ? 1.0f : 0.0f);
	ASSERT(pResult);
	pScript->Push(pResult);
  } else {
	// La criatura no existe
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete phItem;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEODropItemInstr::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {  
	// Libera info y propaga
	ReleaseInfo();	
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Tira un item del inventario al suelo
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEODropItemInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pWaitFlag = pScript->Pop();
  ASSERT(pWaitFlag);    
  CScriptStackValue* phItem = pScript->Pop();
  ASSERT(phItem);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene criatura
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CCriature* const pCriature = pWorld->GetCriature(pHandle->GetDWordValue());
  bool bError = true;
  if (pCriature) {
	// Se obtiene el item
	CItem* const pItem = pWorld->GetItem(phItem->GetDWordValue());
	if (pItem) {
	  bool bError = false;
	  // Se intenta realizar accion
	  if (pCriature->DropItem(pItem->GetHandle()) == RulesDefs::CAR_INPROGRESS) {
		// Accion en progreso ¿se deseaba esperar?
		if (pWaitFlag->GetFloatValue() >= 1.0f) {
		  // Si, se pausa script y se instala como observer
		  pScript->Pause();
		  m_pScript = pScript;
		  m_hEntity = pCriature->GetHandle();
		  pCriature->AddObserver(this);
		  pWorld->AddObserver(this);
		}		 
	  }
	}
  }

  // ¿Hubo error?
  if (bError) {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pWaitFlag;
  delete pHandle;
  delete phItem;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe la notificacion de que se ha realizado el uso del item
// Parametros:
// - hCriature. Handle a la criatura.
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEODropItemInstr::CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
										 const CriatureObserverDefs::eObserverNotifyType& NotifyType,
										 const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hCriature);

  // Se comprueba el tipo de notificacion recibida
  switch (NotifyType) {
	case CriatureObserverDefs::ACTION_REALICE: {
	  // Se comprueba la accion realizada
	  const RulesDefs::eCriatureAction Action = RulesDefs::eCriatureAction(udParam & 0x0000FFFF);
	  switch(Action) {
	    case RulesDefs::CA_DROPITEM: {	
		  // Prosigue script y libera info
		  ASSERT(m_pScript);
		  m_pScript->Resume();
		  ReleaseInfo();
		} break;
	  }; // ~ switch
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Atendera por una posible notifiacion sobre la destruccion de la entidad
//   asociada a la instruccion, en cuyo caso proseguira con la ejecucion del
//   script.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEODropItemInstr::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
									  const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Instruccion actual
  if (m_pScript) {
	// Si, comprueba que la notificacion sea la buscada
	switch(NotifyType) {
	  case WorldObserverDefs::ENTITY_DESTROY: {
		// La entidad destruida es la asociada
		if (udParam == m_hEntity) {
		  // Si, prosigue con el script y libera informacion
		  m_pScript->Resume();
		  ReleaseInfo();
		}
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Simplemente libera informacion asociada a la ultima ejecucion.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEODropItemInstr::ReleaseInfo(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Libera informacion
  // Nota: Se supone que desde el exterior se determinara que hacer con
  // la ejecucion del script asociado.
  if (m_pScript) {	
	m_pScript = NULL;
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);
	pWorld->RemoveObserver(this);
	CCriature* const pCriature = pWorld->GetCriature(m_hEntity);
	if (pCriature) {
	  pCriature->RemoveObserver(this);
	  m_hEntity = 0;
	}	
  }
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEOUseItemInstr::End(void)
{  
  // Finaliza
  if (Inherited::IsInitOk()) {  
	// Libera y propaga
	ReleaseInfo();
	Inherited::End();	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ordena a una criatura usar un item sobre una entidad destino. Se debera
//   de cumplir que en el caso de que la
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOUseItemInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pWaitFlag = pScript->Pop();
  ASSERT(pWaitFlag);
  CScriptStackValue* pEntityTarget = pScript->Pop();
  ASSERT(pEntityTarget);
  CScriptStackValue* pTheItem = pScript->Pop();
  ASSERT(pTheItem);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene entidades para comprobar que sean validos
  bool bResult = false;
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CCriature* const pCriature = pWorld->GetCriature(pHandle->GetDWordValue());
  CItem* const pItem = pWorld->GetItem(pTheItem->GetDWordValue());
  CWorldEntity* const pEntity = pWorld->GetWorldEntity(pEntityTarget->GetDWordValue());
  if (pCriature && pItem && pEntity) {
	// ¿El item a utilizar se posee?
	if (pItem->GetOwner() == pCriature->GetHandle()) {
	  // Filtrado de accion sobre la entidad destino
	  if (CheckForValidStateInUseAction(pCriature, pEntity)) {
		// Ordena usar el item
		bResult = true;
		RulesDefs::eCriatureActionResult AResult = pCriature->UseItem(pItem->GetHandle(), pEntity->GetHandle());		
		if (AResult == RulesDefs::CAR_INPROGRESS) {
		  // ¿Hay que esperar hasta que se realice la accion?
		  if (pWaitFlag->GetFloatValue() >= 1.0f) {
			// Si, se pausa el script y se instala como observer
			pScript->Pause();
			m_pScript = pScript;
			pCriature->AddObserver(this);
			pWorld->AddObserver(this);
		  }
		}
	  }
	}
  }

  // ¿Problemas?
  if (!bResult) {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pTheItem;
  delete pEntityTarget;
  delete pWaitFlag;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recibe la notificacion de que se ha realizado el uso del item
// Parametros:
// - hCriature. Handle a la criatura.
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEOUseItemInstr::CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
										const CriatureObserverDefs::eObserverNotifyType& NotifyType,
										const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hCriature);

  // Se comprueba el tipo de notificacion recibida
  switch (NotifyType) {
	case CriatureObserverDefs::INTERRUPT_MOVING: {
	  // Ha surgido un obstaculo no inicial en el movimiento que SEGURO
	  // se estaba haciendo para la aproximacion en la accion	  
	  // Prosigue script y libera info
	  ASSERT(m_pScript);
	  m_pScript->Resume();
	  ReleaseInfo();
	} break;

	case CriatureObserverDefs::INSUFICIENT_ACTION_POINTS: {
	  // No hay puntos de accion suficientes
	  ASSERT(m_pScript);
	  m_pScript->ErrorInterrupt();
	  ReleaseInfo();
	} break;

	case CriatureObserverDefs::ACTION_REALICE: {
	  // Se comprueba la accion realizada
	  const RulesDefs::eCriatureAction Action = RulesDefs::eCriatureAction(udParam & 0x0000FFFF);
	  switch(Action) {
	    case RulesDefs::CA_USEITEM:
		case RulesDefs::CA_STOPWALK: {	
		  // Prosigue script y libera info
		  ASSERT(m_pScript);
		  m_pScript->Resume();
		  ReleaseInfo();
		} break;
	  }; // ~ switch
	} break; 	
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Atendera por una posible notifiacion sobre la destruccion de la entidad
//   asociada a la instruccion, en cuyo caso proseguira con la ejecucion del
//   script.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOUseItemInstr::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
									  const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Instruccion actual
  if (m_pScript) {
	// Si, comprueba que la notificacion sea la buscada
	switch(NotifyType) {
	  case WorldObserverDefs::ENTITY_DESTROY: {
		// La entidad destruida es la asociada
		if (udParam == m_hEntity) {
		  // Si, prosigue con el script y libera informacion
		  m_pScript->Resume();
		  ReleaseInfo();
		}
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Simplemente libera informacion asociada a la ultima ejecucion.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEOUseItemInstr::ReleaseInfo(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Libera informacion
  // Nota: Se supone que desde el exterior se determinara que hacer con
  // la ejecucion del script asociado.
  if (m_pScript) {	
	m_pScript = NULL;
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);
	pWorld->RemoveObserver(this);
	CCriature* const pCriature = pWorld->GetCriature(m_hEntity);
	if (pCriature) {
	  pCriature->RemoveObserver(this);
	  m_hEntity = 0;
	}	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEOManipulateInstr::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {  
	// Libera y propaga
	ReleaseInfo();
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ordena a una criatura manipular a una entidad. En caso de que la entidad
//   a manipular NO sea un item y NO se este en estado de MainInterfaz, 
//   habra problemas.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOManipulateInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pWaitFlag = pScript->Pop();
  ASSERT(pWaitFlag);
  CScriptStackValue* pEntityTarget = pScript->Pop();
  ASSERT(pEntityTarget);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene entidades para comprobar que sean validos
  bool bResult = false;
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CCriature* const pCriature = pWorld->GetCriature(pHandle->GetDWordValue());
  CWorldEntity* const pEntity = pWorld->GetWorldEntity(pEntityTarget->GetDWordValue());
  if (pCriature && pEntity) {
	// Filtrado de accion sobre la entidad destino
	if (CheckForValidStateInUseAction(pCriature, pEntity)) {
	  // Ordena manipular a la entidad
	  bResult = true;
	  const RulesDefs::eCriatureActionResult AResult = pCriature->Manipulate(pEntity->GetHandle());
	  if (AResult == RulesDefs::CAR_INPROGRESS && 
		  pWaitFlag->GetFloatValue() >= 1.0f) {
		// Si, se pausa el script y se instala como observer
		pScript->Pause();
		m_pScript = pScript;
		pCriature->AddObserver(this);		
		pWorld->AddObserver(this);
	  }
	}
  }

  // ¿Problemas?
  if (!bResult) {
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pEntityTarget;
  delete pWaitFlag;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Manipula una entidad
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void 
CEOManipulateInstr::CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
										   const CriatureObserverDefs::eObserverNotifyType& NotifyType,
										   const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hCriature);

  // Se comprueba el tipo de notificacion recibida
  switch (NotifyType) {
	case CriatureObserverDefs::INTERRUPT_MOVING: {
	  // Ha surgido un obstaculo no inicial en el movimiento que SEGURO
	  // se estaba haciendo para la aproximacion en la accion	  
	  // Prosigue script y libera info
	  ASSERT(m_pScript);
	  m_pScript->Resume();
	  ReleaseInfo();
	} break;

	case CriatureObserverDefs::INSUFICIENT_ACTION_POINTS: {
	  // No hay puntos de accion suficientes
	  ASSERT(m_pScript);
	  m_pScript->ErrorInterrupt();
	  ReleaseInfo();
	} break;

	case CriatureObserverDefs::ACTION_REALICE: {
	  // Se comprueba la accion realizada
	  const RulesDefs::eCriatureAction Action = RulesDefs::eCriatureAction(udParam & 0x0000FFFF);
	  switch(Action) {
	    case RulesDefs::CA_MANIPULATE: 
		case RulesDefs::CA_STOPWALK: {	
		  // Prosigue script y libera info
		  ASSERT(m_pScript);
		  m_pScript->Resume();
		  ReleaseInfo();
		} break;
	  }; // ~ switch
	} break;	
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Atendera por una posible notifiacion sobre la destruccion de la entidad
//   asociada a la instruccion, en cuyo caso proseguira con la ejecucion del
//   script.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOManipulateInstr::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
									    const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Instruccion actual
  if (m_pScript) {
	// Si, comprueba que la notificacion sea la buscada
	switch(NotifyType) {
	  case WorldObserverDefs::ENTITY_DESTROY: {
		// La entidad destruida es la asociada
		if (udParam == m_hEntity) {
		  // Si, prosigue con el script y libera informacion
		  m_pScript->Resume();
		  ReleaseInfo();
		}
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Simplemente libera informacion asociada a la ultima ejecucion.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEOManipulateInstr::ReleaseInfo(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Libera informacion
  // Nota: Se supone que desde el exterior se determinara que hacer con
  // la ejecucion del script asociado.
  if (m_pScript) {	
	m_pScript = NULL;
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);
	pWorld->RemoveObserver(this);
	CCriature* const pCriature = pWorld->GetCriature(m_hEntity);
	if (pCriature) {
	  pCriature->RemoveObserver(this);
	  m_hEntity = 0;
	}	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el flag de transparencia sobre la criatura.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOSetTransparentModeInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pSetFlag = pScript->Pop();
  ASSERT(pSetFlag);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene criatura
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Se establece / quita el flag de transparencia
	pCriature->SetTransparentMode(pSetFlag->GetFloatValue() < 1.0f ? false : true);	
  } else {
	// La criatura no existe
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pSetFlag;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si esta activo el flag de transaprencia y establece resultado
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOIsTransparentModeActiveInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Obtiene criatura
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Se obtiene el flag
	CScriptStackValue* const pResult = new CScriptStackValue(pCriature->IsTransparentMode() ? 1.0f : 0.0f);	
  } else {
	// La criatura no existe
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Cambia la orientacion de una criatura. Para ello, se tomara el 
//   identificador de la orientacion, que valdra entre 0 y 7.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOChangeAnimOrientationInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pOrient = pScript->Pop();
  ASSERT(pOrient);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene criatura
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// ¿Orientacion valida?
	if (pOrient->GetFloatValue() >= 0.0f &&
		pOrient->GetFloatValue() <= 7.0f) {
	  pCriature->GetAnimTemplate()->SetOrientation(pOrient->GetFloatValue());	 
	} else {
	  // Orientacion no valida
	  pScript->ErrorInterrupt();
	}
  } else {
	// La criatura no existe
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pOrient;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la orientacion de la animacion asociada a la criatura
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOGetAnimOrientationInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene criatura
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Obtiene orientacion y la deposita
	CScriptStackValue* pOrient = new CScriptStackValue(float(pCriature->GetAnimTemplate()->GetOrientation()));
	ASSERT(pOrient);
	pScript->Push(pOrient);
  } else {
	// La criatura no existe
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece la alineacion de la criatura. En caso de que la alineacion
//   leida no sea valida, detendra la ejecucion del script. Los valores validos
//   seran de 0, 1 y 2 para dejarla sin alineacion, alinearla con el jugador y
//   para alinearla con los enemigos del jugador respectivamente.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOSetAlingmentInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pAlingment = pScript->Pop();
  ASSERT(pAlingment);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene criatura  
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// ¿Alineacion valida?
	if (pAlingment->GetFloatValue() >= 0.0f &&
		pAlingment->GetFloatValue() <= 2.0f) {
	  // ¿La criatura NO es el jugador?
	  if (RulesDefs::PLAYER != pCriature->GetEntityType()) {
		SYSEngine::GetCombatSystem()->SetAlingment(pCriature->GetHandle(),
												   CombatSystemDefs::eCombatAlingment(word(pAlingment->GetFloatValue())));  
	  }
	} else {
	  // La alineacion no es valida
	  pScript->ErrorInterrupt();
	}
  } else {
	// La criatura no existe
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pAlingment;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia a la criatura con la misma alineacion que una criatura amiga. En
//   caso de que esa criatura amiga no exista (sea nula) se producira un error.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOSetAlingmentWithInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pFriend = pScript->Pop();
  ASSERT(pFriend);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene criatura
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CCriature* const pCriature = pWorld->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Obtiene la criatura amiga
	CCriature* const pFriendCriature = pWorld->GetCriature(pFriend->GetDWordValue());
	if (pFriendCriature) {
  	  // ¿La criatura NO es el jugador?
	  if (RulesDefs::PLAYER != pCriature->GetEntityType()) {
		 // Alinea
		SYSEngine::GetCombatSystem()->SetAlingmentWith(pCriature->GetHandle(),
													   pFriendCriature->GetHandle());
	  }
	} else {
	  // La criatura no existe
	  pScript->ErrorInterrupt();
	}
  } else {
	// La criatura no existe
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pFriend;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece la alinacion de una criatura contra otra. En caso de que alguna
//   de las dos criaturas sean nulas, se interrumpira el proceso.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOSetAlingmentAgainstInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pEnemy = pScript->Pop();
  ASSERT(pEnemy);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);
  
  // Obtiene criatura
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CCriature* const pCriature = pWorld->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Obtiene la criatura enemiga
	CCriature* const pEnemyCriature = pWorld->GetCriature(pEnemy->GetDWordValue());
	if (pEnemyCriature) {
  	  // ¿La criatura NO es el jugador?
	  if (RulesDefs::PLAYER != pCriature->GetEntityType()) {
		// Alinea
		SYSEngine::GetCombatSystem()->SetAlingmentAgainst(pCriature->GetHandle(),
														  pEnemyCriature->GetHandle());
	  }
	} else {
	  // La criatura no existe
	  pScript->ErrorInterrupt();
	}
  } else {
	// La criatura no existe
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
  delete pEnemy;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la alineacion de una criatura, se debera de tener en cuenta que
//   en caso de retornar 0 indicara que la criatura no esta alineada.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOGetAlingmentInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Obtiene criatura
  iCWorld* const pWorld = SYSEngine::GetWorld();
  ASSERT(pWorld);
  CCriature* const pCriature = pWorld->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Obtiene la alineacion
	CScriptStackValue* const pAlingment = new CScriptStackValue(float(SYSEngine::GetCombatSystem()->GetAlingment(pCriature->GetHandle())));
	ASSERT(pAlingment);
	pScript->Push(pAlingment);
  } else {
	// La criatura no existe
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEOHitEntityInstr::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {  
	// Libera y propaga
	ReleaseInfo();
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ordena a una criatura golpear a una entidad.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOHitEntityInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // ¿Estado de MainInterfaz?
  bool bExecuteOk = false;
  if (SYSEngine::GetGUIManager()->GetGameGUIWindowState() == GUIManagerDefs::GUIW_MAININTERFAZ) {
	// Toma el handle a la entidad y parametros
	CScriptStackValue* pWaitFlag = pScript->Pop();
	ASSERT(pWaitFlag);
	CScriptStackValue* pWeaponSlot = pScript->Pop();
	ASSERT(pWeaponSlot);
	CScriptStackValue* pEntityTarget = pScript->Pop();
	ASSERT(pEntityTarget);
	CScriptStackValue* pHandle = pScript->Pop();
	ASSERT(pHandle);
	
	// Obtiene entidades para comprobar que sean validos
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);
	CCriature* const pCriature = pWorld->GetCriature(pHandle->GetDWordValue());
	CWorldEntity* const pEntity = pWorld->GetWorldEntity(pEntityTarget->GetDWordValue());
	if (pCriature && pEntity) {	
	  // ¿Slot valido?
	  const RulesDefs::eIDEquipmentSlot IDSlot = RulesDefs::eIDEquipmentSlot(sword(pWeaponSlot->GetFloatValue()));
	  if (IDSlot == RulesDefs::EQUIPMENT_SLOT_0 ||
		  IDSlot == RulesDefs::EQUIPMENT_SLOT_1) {
		// Si, Ordena golpear a la entidad
		const RulesDefs::eCriatureActionResult AResult = pCriature->Hit(pEntity->GetHandle(),
																		IDSlot);
		if (AResult != RulesDefs::CAR_FAIL) {
		  // Accion ejecutada
		  bExecuteOk = true;			  

		  // ¿Hay que esperar hasta que se realice la accion?
		  if (AResult == RulesDefs::CAR_INPROGRESS &&
			  pWaitFlag->GetFloatValue() >= 1.0f) {
			// Si, se pausa el script y se instala como observer
			pCriature->AddObserver(this);		
			pScript->Pause();
			m_pScript = pScript;
			m_hEntity = pCriature->GetHandle();
			pWorld->AddObserver(this);
		  }	  
		}
	  }
	} 

	// Borra parametros
	delete pHandle;
	delete pEntityTarget;
	delete pWeaponSlot;
	delete pWaitFlag;
  } 

  // ¿Hubo problemas?
  if (!bExecuteOk) {
	// Si, se interrumpe script
	pScript->ErrorInterrupt();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Golpea a una entidad
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void 
CEOHitEntityInstr::CriatureObserverNotify(const AreaDefs::EntHandle& hCriature,
										  const CriatureObserverDefs::eObserverNotifyType& NotifyType,
										  const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT(hCriature);
  
  // Se comprueba el tipo de notificacion recibida
  switch (NotifyType) {
	case CriatureObserverDefs::INTERRUPT_MOVING: 
	case CriatureObserverDefs::INTERRUPT_HIT: {
	  // Ha surgido un obstaculo no inicial en el movimiento que SEGURO
	  // se estaba haciendo para la aproximacion en la accion o bien
	  // se cancelo el comportamiento por defecto de golpear
	  // Prosigue script y libera info
	  ASSERT(m_pScript);
	  m_pScript->Resume();
	  ReleaseInfo();
	} break;

	case CriatureObserverDefs::INSUFICIENT_ACTION_POINTS: {
	  // No hay puntos de accion suficientes	  
	  ASSERT(m_pScript);
	  m_pScript->ErrorInterrupt();
	  ReleaseInfo();
	} break;

	case CriatureObserverDefs::ACTION_REALICE: {
	  // Se comprueba la accion realizada
	  const RulesDefs::eCriatureAction Action = RulesDefs::eCriatureAction(udParam & 0x0000FFFF);
	  switch(Action) {
	    case RulesDefs::CA_HIT: 
		case RulesDefs::CA_STOPWALK: {	
		  // Prosigue script y libera info
		  ASSERT(m_pScript);
		  m_pScript->Resume();
		  ReleaseInfo();
		} break;
	  }; // ~ switch
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Atendera por una posible notifiacion sobre la destruccion de la entidad
//   asociada a la instruccion, en cuyo caso proseguira con la ejecucion del
//   script.
// Parametros:
// - NotifyType. Tipo de notificacion.
// - udParam. Parametro asociado
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CEOHitEntityInstr::WorldObserverNotify(const WorldObserverDefs::eObserverNotifyType& NotifyType,
									   const dword udParam)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Instruccion actual
  if (m_pScript) {
	// Si, comprueba que la notificacion sea la buscada
	switch(NotifyType) {
	  case WorldObserverDefs::ENTITY_DESTROY: {
		// La entidad destruida es la asociada
		if (udParam == m_hEntity) {
		  // Si, prosigue con el script y libera informacion
		  m_pScript->Resume();
		  ReleaseInfo();
		}
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Simplemente libera informacion asociada a la ultima ejecucion.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CEOHitEntityInstr::ReleaseInfo(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Libera informacion
  // Nota: Se supone que desde el exterior se determinara que hacer con
  // la ejecucion del script asociado.
  if (m_pScript) {	
	m_pScript = NULL;
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);
	pWorld->RemoveObserver(this);
	CCriature* const pCriature = pWorld->GetCriature(m_hEntity);
	if (pCriature) {
	  pCriature->RemoveObserver(this);
	  m_hEntity = 0;
	}	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia un evento script a una entidad. Dependiendo de la entidad, los
//   posibles eventos cambiaran. Asi, tendremos:
//    *	SE_ONGET,                     / Item
//    * SE_ONDROP,                    / Item
//    * SE_ONOBSERVE,                 / Wall, Scene Object, Item, Criature
//    * SE_ONTALK,                    / Wall, Scene Object, Item, Criature
//    * SE_ONMANIPULATE,              / Wall, Scene Object, Item
//    * SE_ONDEATH,                   / Criature
//    * SE_ONRESURRECT,               / Criature
//    * SE_ONINSERTINEQUIPMENTSLOT,   / Criature
//    * SE_ONREMOVEFROMEQUIPMENTSLOT, / Criature
//    * SE_ONUSEHABILITY,             / Criature
//    * SE_ONACTIVATEDSYMPTOM,        / Criature
//    * SE_ONDEACTIVATEDSYMPTOM,      / Criature 
//    * SE_ONHIT,                     / Criature
//    * SE_ONSTARTCOMBATTURN,         / Criature
//    * SE_ONENTITYIDLE,              / Wall, Scene Object, Item, Criature
//    * SE_ONUSEITEM,                 / Item
//    * SE_ONTRADEITEM,               / Item
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - En esta instruccion no se podra evaluar que el fichero con el script que
//   se va a asociar exista (en caso de asociar alguno).
///////////////////////////////////////////////////////////////////////////////
void
CEOSetScriptInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma parametros
  CScriptStackValue* pFile = pScript->Pop();
  ASSERT(pFile);
  CScriptStackValue* pEvent = pScript->Pop();
  ASSERT(pEvent);
  CScriptStackValue* pEntityObj = pScript->Pop();
  ASSERT(pEntityObj);

  // Se obtiene instancia a la entidad
  CWorldEntity* const pEntity = SYSEngine::GetWorld()->GetWorldEntity(pEntityObj->GetDWordValue());
  ASSERT(pEntity);

  // ¿Es un evento valido?
  bool bResult = false;
  RulesDefs::eScriptEvents Event = RulesDefs::eScriptEvents(sword(pEvent->GetFloatValue()));  
  switch (pEntity->GetEntityType()) {
	case RulesDefs::SCENE_OBJ: {
	  // Objeto de escenario / Se comprueban eventos
      if (Event == RulesDefs::SE_ONOBSERVE ||
		  Event == RulesDefs::SE_ONTALK ||
		  Event == RulesDefs::SE_ONMANIPULATE ||
		  Event == RulesDefs::SE_ONENTITYIDLE) {
		// Si, se permite a instalar
		bResult = true;
	  }
	} break;

	case RulesDefs::PLAYER:
	case RulesDefs::CRIATURE: {
	  // Criaturas / Se comprueban eventos
	  // Objeto de escenario / Se comprueban eventos
      if (Event == RulesDefs::SE_ONOBSERVE ||
		  Event == RulesDefs::SE_ONTALK ||
		  Event == RulesDefs::SE_ONENTITYIDLE ||
		  Event == RulesDefs::SE_ONDEATH ||
		  Event == RulesDefs::SE_ONRESURRECT ||
		  Event == RulesDefs::SE_ONINSERTINEQUIPMENTSLOT ||
		  Event == RulesDefs::SE_ONREMOVEFROMEQUIPMENTSLOT ||
		  Event == RulesDefs::SE_ONUSEHABILITY ||
		  Event == RulesDefs::SE_ONACTIVATEDSYMPTOM ||
		  Event == RulesDefs::SE_ONDEACTIVATEDSYMPTOM ||
		  Event == RulesDefs::SE_ONHIT ||
		  Event == RulesDefs::SE_ONSTARTCOMBATTURN ||
		  Event == RulesDefs::SE_ONCRIATUREINRANGE ||
		  Event == RulesDefs::SE_ONCRIATUREOUTOFRANGE) {
		// Si, se permite a instalar
		bResult = true;
	  }
	} break;

	case RulesDefs::WALL: {
	  // Pared / Se comprueban eventos
	  // Objeto de escenario / Se comprueban eventos
      if (Event == RulesDefs::SE_ONOBSERVE ||
		  Event == RulesDefs::SE_ONTALK ||
		  Event == RulesDefs::SE_ONMANIPULATE ||
		  Event == RulesDefs::SE_ONENTITYIDLE) {
		// Si, se permite a instalar
		bResult = true;
	  }
	} break;

	case RulesDefs::ITEM: {
	  // Items / Se comprueban eventos
	  // Objeto de escenario / Se comprueban eventos
      if (Event == RulesDefs::SE_ONOBSERVE ||
		  Event == RulesDefs::SE_ONTALK ||
		  Event == RulesDefs::SE_ONMANIPULATE ||
		  Event == RulesDefs::SE_ONENTITYIDLE ||
		  Event == RulesDefs::SE_ONGET ||
		  Event == RulesDefs::SE_ONDROP ||
		  Event == RulesDefs::SE_ONUSEITEM ||
		  Event == RulesDefs::SE_ONTRADEITEM) {
		// Si, se permite a instalar
		bResult = true;
	  }
	} break;
  }; // ~ switch

  // ¿Se puede instalar el evento?
  if (bResult) {
	// Si, se instala
	pEntity->SetScriptEvent(Event, pFile->GetStringValue());
  } else {
	// No, hay problemas
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pEvent;
  delete pFile; 
  delete pEntityObj;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el modo fantasma se halla, o no, activado.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOIsGhostMoveModeActiveInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Obtiene criatura
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Obtiene el flag
	CScriptStackValue* const pAlingment = new CScriptStackValue(pCriature->IsGhostMoveModeActive() ? 1.0f : 0.0f);
	ASSERT(pAlingment);
	pScript->Push(pAlingment);
  } else {
	// La criatura no existe
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el flag de movimiento en modo fantasma
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOSetGhostMoveModeInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pSetFlag = pScript->Pop();
  ASSERT(pSetFlag);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Obtiene criatura
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Establece el flag
	pCriature->SetGhostMoveMode(pSetFlag->GetFloatValue() < 1.0f ? false : true);	
  } else {
	// La criatura no existe
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pSetFlag;
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene el rango de una criatura
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOGetRangeInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Obtiene criatura
  CCriature* const pCriature = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriature) {
	// Deposita el rango
	CScriptStackValue* const pResult = new CScriptStackValue(float(pCriature->GetRange()));
	ASSERT(pResult);
	pScript->Push(pResult);	
  } else {
	// La criatura no existe
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si la criatura recibida se halla dentro del rango de la criatura
//   pasada.
// Parametros:
// - pScript. Instancia al script.
// Devuelve:
// Notas:
// - Metodo solo aplicable a criaturas
///////////////////////////////////////////////////////////////////////////////
void
CEOIsInRangeInstr::Execute(iCScript* const pScript)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros correctos
  ASSERT(pScript);

  // Toma el handle a la entidad y parametros
  CScriptStackValue* pCriatureIn = pScript->Pop();
  ASSERT(pCriatureIn);
  CScriptStackValue* pHandle = pScript->Pop();
  ASSERT(pHandle);

  // Obtiene criatura con handle
  CCriature* const pCriatureSrc = SYSEngine::GetWorld()->GetCriature(pHandle->GetDWordValue());
  if (pCriatureSrc) {
	// Obtiene criatura a comprobar
	CCriature* const pCriatureDest = SYSEngine::GetWorld()->GetCriature(pCriatureIn->GetDWordValue());
	if (pCriatureDest) {
	  // Deposita el resultado
	  const bool bResult = pCriatureSrc->IsInRange(pCriatureDest->GetHandle());
	  CScriptStackValue* const pResult = new CScriptStackValue(bResult ? 1.0f : 0.0f);
	  ASSERT(pResult);
	  pScript->Push(pResult);	
	} else {
	  // Criatura a comprobar NO valida
	  pScript->ErrorInterrupt();
	}
  } else {
	// La criatura no existe
	pScript->ErrorInterrupt();
  }

  // Borra parametros
  delete pCriatureIn;
  delete pHandle;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Chequea por la comprobacion de que se este en un estado valido para una
//   accion sobre una entidad por parte de una criatura.
//   Si se esta en MainInterfaz, la entidad destino SI ES ITEM no debera
//   de tener dueño y de tenerlo, debera de ser la propia criatura. 
//   Si se esta en PlayerProfile, la entidad destino DEBE de ser un item
//   y su dueño debe de ser la criatura que la utiliza.	  
// Parametros:
// - pCriature. Criatura que usa el item.
// - pTarget. Entidad donde usar el item
// Devuelve:
// - Si puede ser utilizado el item true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CheckForValidStateInUseAction(CCriature* const pCriature,
				              CWorldEntity* const pTarget)
{
  // SOLO si parametros validos
  ASSERT(pCriature);
  ASSERT(pTarget);

  // ¿Se esta en estado de Inventario?
  iCGUIManager* const pGUIManager = SYSEngine::GetGUIManager();
  ASSERT(pGUIManager);
  if (pGUIManager->GetGameGUIWindowState() == GUIManagerDefs::GUIW_PLAYERPROFILE) {
	// Si, la entidad destino Debe de ser un item y poseerlo la criatura
	if (pTarget->GetEntityType() == RulesDefs::ITEM) {
	  CItem* const pTargetItem = SYSEngine::GetWorld()->GetItem(pTarget->GetHandle());
	  ASSERT(pTargetItem);
	  return (pTargetItem->GetOwner() == pCriature->GetHandle());
	}
  } else if (pGUIManager->GetGameGUIWindowState() == GUIManagerDefs::GUIW_MAININTERFAZ) {
	// No, se esta en MainInterfaz
	// ¿La entidad destino es un item?
	if (pTarget->GetEntityType() == RulesDefs::ITEM) {
	  // ¿El dueño es la criatura o bien no tiene dueño?
	  CItem* const pTargetItem = SYSEngine::GetWorld()->GetItem(pTarget->GetHandle());
	  ASSERT(pTargetItem);
	  if (0 == pTargetItem->GetOwner() ||
		  pTargetItem->GetOwner() == pCriature->GetHandle()) {
		// Todo correcto
		return true;
	  }
	} else {
	  // Todo correcto
	  return true;
	}
  }

  // Problemas para validar
  return false;
}
