///////////////////////////////////////////////////////////////////////////////
// CSCompiler - CrisolScript Compiler
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

////////////////////////////////////////////////////////////////////////////////
// ASTree.cpp
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Notas:
// - Consultar ASTree.h para mas informacion
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "ASTree.h"
#include "Memory.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

// Funciones externas
extern sword GetActLine(void);
extern sbyte* GetActFileName(void);

// Funciones privadas
static void SymTableReleaseSymTableNodes(sSymTableNode* pNode);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_NOP.
// Parametros:
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeNopOpcode(sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_NOP;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;  
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_NNEG.
// Parametros:
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeNNegOpcode(sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_NNEG;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_NMUL.
// Parametros:
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeNMulOpcode(sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_NMUL;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_NADD
// Parametros:
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeNAddOpcode(sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_NADD;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_NMUL
// Parametros:
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeNModOpcode(sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_NMOD;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_NDIV
// Parametros:
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeNDivOpcode(sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_NDIV;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_NSUB
// Parametros:
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeNSubOpcode(sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_NSUB;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_SADD
// Parametros:
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeSAddOpcode(sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_SADD;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_JMP
// Parametros:
// - unLable. Etiqueta de salto.
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeJmpOpcode(word unLabel,
			  sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_JMP;  
  pNode->JmpArg.unLabel = unLabel;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_JMP_FALSE.
// Parametros:
// - unLabel. Etiqueta de salto
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeJmpFalseOpcode(word unLabel,
			       sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_JMP_FALSE;
  pNode->JmpArg.unLabel = unLabel;
  pNode->udOpcodePos = 0;  
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_JMP_TRUE.
// Parametros:
// - unLabel. Etiqueta de salto
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeJmpTrueOpcode(word unLabel,
		          sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_JMP_TRUE;
  pNode->JmpArg.unLabel = unLabel;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_NJMP_EQ
// Parametros:
// - unLabel. Etiqueta de salto
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeNJmpEqOpcode(word unLabel,
	             sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_NJMP_EQ;
  pNode->JmpArg.unLabel = unLabel;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_NJMP_NE
// Parametros:
// - unLabel. Etiqueta de salto
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeNJmpNeOpcode(word unLabel,
	             sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_NJMP_NE;
  pNode->JmpArg.unLabel = unLabel;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_NJMP_GE
// Parametros:
// - unLabel. Etiqueta de salto
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeNJmpGeOpcode(word unLabel,
	             sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_NJMP_GE;
  pNode->JmpArg.unLabel = unLabel;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_NJMP_GT
// Parametros:
// - unLabel. Etiqueta de salto
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeNJmpGtOpcode(word unLabel,
	             sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_NJMP_GT;
  pNode->JmpArg.unLabel = unLabel;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_NJMP_LT
// Parametros:
// - unLabel. Etiqueta de salto
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeNJmpLtOpcode(word unLabel,
	             sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_NJMP_LT;
  pNode->JmpArg.unLabel = unLabel;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_JMP_LE
// Parametros:
// - unLabel. Etiqueta de salto
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeNJmpLeOpcode(word unLabel,
	             sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_NJMP_LE;
  pNode->JmpArg.unLabel = unLabel;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_SJMP_EQ
// Parametros:
// - unLabel. Etiqueta de salto
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeSJmpEqOpcode(word unLabel,
	             sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_SJMP_EQ;
  pNode->JmpArg.unLabel = unLabel;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_SJMP_NE
// Parametros:
// - unLabel. Etiqueta de salto
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeSJmpNeOpcode(word unLabel,
	             sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_SJMP_NE;
  pNode->JmpArg.unLabel = unLabel;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_EJMP_EQ
// Parametros:
// - unLabel. Etiqueta de salto
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeEJmpEqOpcode(word unLabel,
	             sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_EJMP_EQ;
  pNode->JmpArg.unLabel = unLabel;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_EJMP_NE
// Parametros:
// - unLabel. Etiqueta de salto
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeEJmpNeOpcode(word unLabel,
	             sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_EJMP_NE;
  pNode->JmpArg.unLabel = unLabel;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_DUP
// Parametros:
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeDupOpcode(sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_DUP;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_POP
// Parametros:
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakePopOpcode(sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_POP;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_NRETURN
// Parametros:
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeNReturnOpcode(sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_NRETURN;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_SRETURN
// Parametros:
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeSReturnOpcode(sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_SRETURN;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_ERETURN
// Parametros:
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeEReturnOpcode(sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_ERETURN;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_RETURN
// Parametros:
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeReturnOpcode(sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_RETURN;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_NLOAD
// Parametros:
// - unAddress. Direccion donde obtener el valor
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeNLoadOpcode(word unAddress,
				sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_NLOAD;
  pNode->LoadArg.unAddress = unAddress;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_SLOAD
// Parametros:
// - unAddress. Direccion donde obtener el valor
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeSLoadOpcode(word unAddress,
			    sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_SLOAD;
  pNode->LoadArg.unAddress = unAddress;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_ELOAD
// Parametros:
// - unAddress. Direccion donde obtener el valor
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeELoadOpcode(word unAddress,
				sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_ELOAD;
  pNode->LoadArg.unAddress = unAddress;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_NSTORE
// Parametros:
// - unAddress. Direccion donde alojar.
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeNStoreOpcode(word unAddress,
			     sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_NSTORE;
  pNode->StoreArg.unAddress = unAddress;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_SSTORE
// Parametros:
// - unAddress. Direccion donde alojar.
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeSStoreOpcode(word unAddress,
			     sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_SSTORE;
  pNode->StoreArg.unAddress = unAddress;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_ESTORE
// Parametros:
// - unAddress. Direccion donde alojar.
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeEStoreOpcode(word unAddress,
			     sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_ESTORE;
  pNode->StoreArg.unAddress = unAddress;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_NPUSH
// Parametros:
// - fValue. Valor asociado
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeNPushOpcode(float fValue,
		        sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_NPUSH;
  pNode->NPushArg.fValue = fValue;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_SPUSH
// Parametros:
// - szValue. Valor asociado
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeSPushOpcode(sbyte* szValue,
		        sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_SPUSH;
  pNode->SPushArg.szValue = szValue;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_EPUSH
// Parametros:
// - unValue. Valor asociado
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeEPushOpcode(word unValue,
		        sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_EPUSH;
  pNode->EPushArg.unValue = unValue;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_NSCAST / OP_SNCAST
// Parametros:
// - pSource, pDest. Son los tipos fuente y destino usados para determinar
//   el tipo de cast.
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeCastOpcode(sType* pSource,
			   sType* pDest,
			   sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  if (TYPE_NUMBER == pSource->ValueType &&
  	  TYPE_STRING == pDest->ValueType) {
	pNode->OpcodeType = OP_NSCAST;
  } else if (TYPE_STRING == pSource->ValueType &&
  			 TYPE_NUMBER == pDest->ValueType) {
	pNode->OpcodeType = OP_SNCAST;
  } else {
	assert(0);
  } 
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_CALL_FUNC
// Parametros:
// - szIdentifier. Identificador de la funcion
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeCallFuncOpcode(sbyte* szIdentifier,
			       sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_CALL_FUNC;
  pNode->CallFuncArg.szIdentifier = szIdentifier;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode subtipo OP_LABEL
// Parametros:
// - unLabelValue. Valor asociado a la etiqueta
// - pSigOpcode. Enlace al sig opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeLabelOpcode(word unLabelValue,
		        sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  pNode->OpcodeType = OP_LABEL;
  pNode->LabelArg.unLabelValue = unLabelValue;
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode asociado a una llamada a funcion de API
// Parametros:
// - APIFunc. Codigo de la funcion de api que se llama
// - pSigOpcode. Enlace a sig. opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeAPICallOpcode(eExpAPIFunc APIFunc,
				  sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);
  switch(APIFunc) {
	case API_PASSTORGBCOLOR: { 
	  pNode->OpcodeType = OP_API_PASSTORGBCOLOR;
	} break;
	  
	case API_GETREDCOMPONENT: { 
	  pNode->OpcodeType = OP_API_GETREDCOMPONENT;
	} break;
	  
	case API_GETGREENCOMPONENT: { 
	  pNode->OpcodeType = OP_API_GETGREENCOMPONENT;
	} break;
	  
	case API_GETBLUECOMPONENT: { 
	  pNode->OpcodeType = OP_API_GETBLUECOMPONENT;
	} break;
	  
	case API_RAND: { 
	  pNode->OpcodeType = OP_API_RAND;
	} break;
	  
	case API_GETINTEGERVALUE: { 
	  pNode->OpcodeType = OP_API_GETINTEGERVALUE;
	} break;
	  
	case API_GETDECIMALVALUE: { 
	  pNode->OpcodeType = OP_API_GETDECIMALVALUE;
	} break;
	  
	case API_GETSTRINGSIZE: { 
	  pNode->OpcodeType = OP_API_GETSTRINGSIZE;
	} break;
	  
	case API_WRITETOLOGGER: { 
	  pNode->OpcodeType = OP_API_WRITETOLOGGER;
	} break;
	  
	case API_ENABLECRISOLSCRIPTWARNINGS: { 
	  pNode->OpcodeType = OP_API_ENABLECRISOLSCRIPTWARNINGS;
	} break;
	  
	case API_DISABLECRISOLSCRIPTWARNINGS: { 
	  pNode->OpcodeType = OP_API_DISABLECRISOLSCRIPTWARNINGS;
	} break;

	case API_SHOWFPS: { 
	  pNode->OpcodeType = OP_API_SHOWFPS;
	} break;

	case API_WAIT: { 
	  pNode->OpcodeType = OP_API_WAIT;
	} break;
  }; // ~ switch  

  // Resto
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode asociado a una llamada a metodo game
// Parametros:
// - Method. Codigo del metodo invocado
// - pSigOpcode. Enlace a sig. opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeGameObjMethodCallOpcode(eExpGameObjMethod Method,
						    sOpcode* pSigOpcode)
{
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);  
  switch(Method) {
	case GAMEOBJ_QUITGAME: { 
	  pNode->OpcodeType = OP_GAMEOBJ_QUITGAME;
	} break;

	case GAMEOBJ_WRITETOCONSOLE: {
	  pNode->OpcodeType = OP_GAMEOBJ_WRITETOCONSOLE;
	} break;

	case GAMEOBJ_ACTIVEADVICEDIALOG: {
	  pNode->OpcodeType = OP_GAMEOBJ_ACTIVEADVICEDIALOG;
	} break;

	case GAMEOBJ_ACTIVEQUESTIONDIALOG: {
	  pNode->OpcodeType = OP_GAMEOBJ_ACTIVEQUESTIONDIALOG;
	} break;

	case GAMEOBJ_ACTIVETEXTREADERDIALOG: {
	  pNode->OpcodeType = OP_GAMEOBJ_ACTIVETEXTREADERDIALOG;
	} break;

	case GAMEOBJ_ADDOPTIONTOTEXTSELECTORDIALOG: {
	  pNode->OpcodeType = OP_GAMEOBJ_ADDOPTIONTOTEXTSELECTORDIALOG;
	} break;
	case GAMEOBJ_RESETOPTIONSINTEXTSELECTORDIALOG: {
	  pNode->OpcodeType = OP_GAMEOBJ_RESETOPTIONSINTEXTSELECTORDIALOG;
	} break;

	case GAMEOBJ_ACTIVETEXTSELECTORDIALOG: {
	  pNode->OpcodeType = OP_GAMEOBJ_ACTIVETEXTSELECTORDIALOG;
	} break;

	case GAMEOBJ_PLAYMIDIMUSIC: {
	  pNode->OpcodeType = OP_GAMEOBJ_PLAYMIDIMUSIC;
	} break;

	case GAMEOBJ_STOPMIDIMUSIC: {
	  pNode->OpcodeType = OP_GAMEOBJ_STOPMIDIMUSIC;
	} break;

	case GAMEOBJ_PLAYWAVAMBIENTSOUND: {
	  pNode->OpcodeType = OP_GAMEOBJ_PLAYWAVAMBIENTSOUND;
	} break;

	case GAMEOBJ_STOPWAVAMBIENTSOUND: {
	  pNode->OpcodeType = OP_GAMEOBJ_STOPWAVAMBIENTSOUND;
	} break;

	case GAMEOBJ_ACTIVETRADEITEMSINTERFAZ: {
	  pNode->OpcodeType = OP_GAMEOBJ_ACTIVETRADEITEMSINTERFAZ;
	} break;

	case GAMEOBJ_ADDOPTIONTOCONVERSATORINTERFAZ: {
	  pNode->OpcodeType = OP_GAMEOBJ_ADDOPTIONTOCONVERSATORINTERFAZ;
	} break;

	case GAMEOBJ_RESETOPTIONSINCONVERSATORINTERFAZ: {
	  pNode->OpcodeType = OP_GAMEOBJ_RESETOPTIONSINCONVERSATORINTERFAZ;
	} break;

	case GAMEOBJ_ACTIVECONVERSATORINTERFAZ: {
	  pNode->OpcodeType = OP_GAMEOBJ_ACTIVECONVERSATORINTERFAZ;
	} break;

	case GAMEOBJ_DESACTIVECONVERSATORINTERFAZ: {
	  pNode->OpcodeType = OP_GAMEOBJ_DESACTIVECONVERSATORINTERFAZ;
	} break;

	case GAMEOBJ_GETOPTIONFROMCONVERSATORINTERFAZ: {
	  pNode->OpcodeType = OP_GAMEOBJ_GETOPTIONFROMCONVERSATORINTERFAZ;
	} break;

	case GAMEOBJ_SHOWPRESENTATION: {
	  pNode->OpcodeType = OP_GAMEOBJ_SHOWPRESENTATION;
	} break;

	case GAMEOBJ_BEGINCUTSCENE: {
	  pNode->OpcodeType = OP_GAMEOBJ_BEGINCUTSCENE;
	} break;

	case GAMEOBJ_ENDCUTSCENE: {
	  pNode->OpcodeType = OP_GAMEOBJ_ENDCUTSCENE;
	} break;

	case GAMEOBJ_SETSCRIPT: {
	  pNode->OpcodeType = OP_GAMEOBJ_SETSCRIPT;
	} break;

	case GAMEOBJ_ISKEYPRESSED: {
	  pNode->OpcodeType = OP_GAMEOBJ_ISKEYPRESSED;
	} break;
  }; // ~ switch  

  // Resto
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode asociado a una llamada a metodo world
// Parametros:
// - Method. Codigo del metodo invocado
// - pSigOpcode. Enlace a sig. opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeWorldObjMethodCallOpcode(eExpWorldObjMethod Method,
						     sOpcode* pSigOpcode)
{
    // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);  
  switch(Method) {
	case WORLDOBJ_GETAREANAME: {
	  pNode->OpcodeType = OP_WORLDOBJ_GETAREANAME;
	} break;

	case WORLDOBJ_GETAREAID: {
	  pNode->OpcodeType = OP_WORLDOBJ_GETAREAID;
	} break;
	
	case WORLDOBJ_GETAREAWIDTH: {
	  pNode->OpcodeType = OP_WORLDOBJ_GETAREAWIDTH;
	} break;
	
	case WORLDOBJ_GETAREAHEIGHT: {
	  pNode->OpcodeType = OP_WORLDOBJ_GETAREAHEIGHT;
	} break;
	
	case WORLDOBJ_GETHOUR: {
	  pNode->OpcodeType = OP_WORLDOBJ_GETHOUR;
	} break;
	
	case WORLDOBJ_GETMINUTE: {
	  pNode->OpcodeType = OP_WORLDOBJ_GETMINUTE;
	} break;
	
	case WORLDOBJ_SETHOUR: {
	  pNode->OpcodeType = OP_WORLDOBJ_SETHOUR;
	} break;
	
	case WORLDOBJ_SETMINUTE: {
	  pNode->OpcodeType = OP_WORLDOBJ_SETMINUTE;
	} break;
	
	case WORLDOBJ_GETENTITY: {
	  pNode->OpcodeType = OP_WORLDOBJ_GETENTITY;
	} break;
	
	case WORLDOBJ_GETPLAYER: {
	  pNode->OpcodeType = OP_WORLDOBJ_GETPLAYER;
	} break;
	
	case WORLDOBJ_ISFLOORVALID: {
	  pNode->OpcodeType = OP_WORLDOBJ_ISFLOORVALID;
	} break;
	
	case WORLDOBJ_GETITEMAT: {
	  pNode->OpcodeType = OP_WORLDOBJ_GETITEMAT;
	} break;
	
	case WORLDOBJ_GETNUMITEMSAT: {
	  pNode->OpcodeType = OP_WORLDOBJ_GETNUMITEMSAT;
	} break;		
	
	case WORLDOBJ_GETDISTANCE: {
	  pNode->OpcodeType = OP_WORLDOBJ_GETDISTANCE;
	} break;
	
	case WORLDOBJ_CALCULEPATHLENGHT: {
	  pNode->OpcodeType = OP_WORLDOBJ_CALCULEPATHLENGHT;
	} break;
	
	case WORLDOBJ_LOADAREA: {
	  pNode->OpcodeType = OP_WORLDOBJ_LOADAREA;
	} break;
	
	case WORLDOBJ_CHANGEENTITYLOCATION: {
	  pNode->OpcodeType = OP_WORLDOBJ_CHANGEENTITYLOCATION;
	} break;
		
	case WORLDOBJ_ATTACHCAMERATOENTITY: {
	  pNode->OpcodeType = OP_WORLDOBJ_ATTACHCAMERATOENTITY;
	} break;
	
	case WORLDOBJ_ATTACHCAMERATOLOCATION: {
	  pNode->OpcodeType = OP_WORLDOBJ_ATTACHCAMERATOLOCATION;
	} break;
	
	case WORLDOBJ_ISCOMBATMODEACTIVE: {
	  pNode->OpcodeType = OP_WORLDOBJ_ISCOMBATMODEACTIVE;
	} break;

	case WORLDOBJ_ENDCOMBAT: {
	  pNode->OpcodeType = OP_WORLDOBJ_ENDCOMBAT;
	} break;

	case WORLDOBJ_GETCRIATUREINCOMBATTURN: {
	  pNode->OpcodeType = OP_WORLDOBJ_GETCRIATUREINCOMBATTURN;
	} break;

	case WORLDOBJ_GETCOMBATANT: {
	  pNode->OpcodeType = OP_WORLDOBJ_GETCOMBATANT;
	} break;

	case WORLDOBJ_GETNUMBEROFCOMBATANTS: {
	  pNode->OpcodeType = OP_WORLDOBJ_GETNUMBEROFCOMBATANTS;
	} break;
	
	case WORLDOBJ_GETAREALIGHTMODEL: {
	  pNode->OpcodeType = OP_WORLDOBJ_GETAREALIGHTMODEL;
	} break;
	
	case WORLDOBJ_SETIDLESCRIPTTIME: {
	  pNode->OpcodeType = OP_WORLDOBJ_SETIDLESCRIPTTIME;
	} break;

	case WORLDOBJ_SETSCRIPT: {
	  pNode->OpcodeType = OP_WORLDOBJ_SETSCRIPT;
	} break;

	case WORLDOBJ_DESTROYENTITY: {
	  pNode->OpcodeType = OP_WORLDOBJ_DESTROYENTITY;
	} break;
	
	case WORLDOBJ_CREATECRIATURE : {
	  pNode->OpcodeType = OP_WORLDOBJ_CREATECRIATURE;
	} break;
	
	case WORLDOBJ_CREATEWALL: {
	  pNode->OpcodeType = OP_WORLDOBJ_CREATEWALL;
	} break;
	
	case WORLDOBJ_CREATESCENARYOBJECT: {
	  pNode->OpcodeType = OP_WORLDOBJ_CREATESCENARYOBJECT;
	} break;
	
	case WORLDOBJ_CREATEITEMABANDONED: {
	  pNode->OpcodeType = OP_WORLDOBJ_CREATEITEMABANDONED;
	} break;
	
	case WORLDOBJ_CREATEITEMWITHOWNER: {
	  pNode->OpcodeType = OP_WORLDOBJ_CREATEITEMWITHOWNER;
	} break;

	case WORLDOBJ_SETWORLDTIMEPAUSE: {
	  pNode->OpcodeType = OP_WORLDOBJ_SETWORLDTIMEPAUSE;
	} break;

	case WORLDOBJ_ISWORLDTIMEINPAUSE: {
	  pNode->OpcodeType = OP_WORLDOBJ_ISWORLDTIMEINPAUSE;
	} break;

	case WORLDOBJ_SETELEVATIONAT: {
	  pNode->OpcodeType = OP_WORLDOBJ_SETELEVATIONAT;
	} break;

	case WORLDOBJ_GETELEVATIONAT: {
	  pNode->OpcodeType = OP_WORLDOBJ_GETELEVATIONAT;
	} break;

	case WORLDOBJ_NEXTTURN: {
	  pNode->OpcodeType = OP_WORLDOBJ_NEXTTURN;
	} break;

	case WORLDOBJ_GETLIGHTAT: {
	  pNode->OpcodeType = OP_WORLDOBJ_GETLIGHTAT;
	} break;

	case WORLDOBJ_PLAYWAVSOUND: {
	  pNode->OpcodeType = OP_WORLDOBJ_PLAYWAVSOUND;
	} break;

	case WORLDOBJ_SETSCRIPTAT: {
	  pNode->OpcodeType = OP_WORLDOBJ_SETSCRIPTAT;
	} break;

	default:
	  assert(0);
  }; // ~ switch

  // Resto
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sOpcode asociado a una llamada a metodo entity
// Parametros:
// - Method. Codigo del metodo invocado
// - pSigOpcode. Enlace a sig. opcode
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sOpcode* 
MakeEntityObjMethodCallOpcode(eExpEntityObjMethod Method,
							  sOpcode* pSigOpcode)
{
  
  // Crea nodo, establece valores y retorna
  sOpcode* pNode = ALLOC(sOpcode);  
  switch(Method) {
	case ENTITYOBJ_GETNAME: {
	  pNode->OpcodeType = OP_ENTITYOBJ_GETNAME;
	} break;

	case ENTITYOBJ_SETNAME: {
	  pNode->OpcodeType = OP_ENTITYOBJ_SETNAME;
	} break;
	
	case ENTITYOBJ_GETENTITYTYPE: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_GETENTITYTYPE;
	} break;
	
	case ENTITYOBJ_GETTYPE: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_GETTYPE;
	} break;
	
	case ENTITYOBJ_SAY: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_SAY;
	} break;
	
	case ENTITYOBJ_SHUTUP: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_SHUTUP;
	} break;
	
	case ENTITYOBJ_ISSAYING: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_ISSAYING;
	} break;
	
	case ENTITYOBJ_ATTACHGFX: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_ATTACHGFX;
	} break;
	
	case ENTITYOBJ_RELEASEGFX: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_RELEASEGFX;
	} break;
	
	case ENTITYOBJ_RELEASEALLGFX: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_RELEASEALLGFX;
	} break;
	
	case ENTITYOBJ_ISGFXATTACHED: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_ISGFXATTACHED;
	} break;
	
	case ENTITYOBJ_GETNUMITEMSINCONTAINER: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_GETNUMITEMSINCONTAINER;
	} break;
	
	case ENTITYOBJ_ISITEMINCONTAINER: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_ISITEMINCONTAINER;
	} break;
	
	case ENTITYOBJ_TRANSFERITEMTOCONTAINER: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_TRANSFERITEMTOCONTAINER;
	} break;
	
	case ENTITYOBJ_INSERTITEMINCONTAINER: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_INSERTITEMINCONTAINER;
	} break;
	
	case ENTITYOBJ_REMOVEITEMOFCONTAINER: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_REMOVEITEMOFCONTAINER;
	} break;
	
	case ENTITYOBJ_SETANIMTEMPLATESTATE: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_SETANIMTEMPLATESTATE;
	} break;
	
	case ENTITYOBJ_SETPORTRAITANIMTEMPLATESTATE: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_SETPORTRAITANIMTEMPLATESTATE;
	} break;
	
	case ENTITYOBJ_SETIDLESCRIPTTIME: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_SETIDLESCRIPTTIME;
	} break;
	
	case ENTITYOBJ_SETLIGHT: {
	  pNode->OpcodeType = OP_ENTITYOBJ_SETLIGHT;
	} break;
	
	case ENTITYOBJ_GETLIGHT: {
	  pNode->OpcodeType = OP_ENTITYOBJ_GETLIGHT;
	} break;
	
	case ENTITYOBJ_GETXPOS: {
	  pNode->OpcodeType = OP_ENTITYOBJ_GETXPOS;
	} break;
	
	case ENTITYOBJ_GETYPOS: {
	  pNode->OpcodeType = OP_ENTITYOBJ_GETYPOS;
	} break;

	case ENTITYOBJ_GETELEVATION: {
	  pNode->OpcodeType = OP_ENTITYOBJ_GETELEVATION;
	} break;

	case ENTITYOBJ_SETELEVATION: {
	  pNode->OpcodeType = OP_ENTITYOBJ_SETELEVATION;
	} break;

	case ENTITYOBJ_GETLOCALATTRIBUTE: {
	  pNode->OpcodeType = OP_ENTITYOBJ_GETLOCALATTRIBUTE;
	} break;

	case ENTITYOBJ_SETLOCALATTRIBUTE: {
	  pNode->OpcodeType = OP_ENTITYOBJ_SETLOCALATTRIBUTE;
	} break;

	case ENTITYOBJ_GETOWNER: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_GETOWNER;
	} break;
	
	case ENTITYOBJ_GETCLASS: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_GETCLASS;
	} break;

	case ENTITYOBJ_GETINCOMBATUSECOST: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_GETINCOMBATUSECOST;
	} break;

	case ENTITYOBJ_GETGLOBALATTRIBUTE: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_GETGLOBALATTRIBUTE;
	} break;

	case ENTITYOBJ_SETGLOBALATTRIBUTE: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_SETGLOBALATTRIBUTE;
	} break;
	
	case ENTITYOBJ_GETWALLORIENTATION: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_GETWALLORIENTATION;
	} break;
	
	case ENTITYOBJ_BLOCKACCESS: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_BLOCKACCESS;
	} break;
	
	case ENTITYOBJ_UNBLOCKACCESS: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_UNBLOCKACCESS;
	} break;
	
	case ENTITYOBJ_ISACCESSBLOCKED: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_ISACCESSBLOCKED;
	} break;
	
	case ENTITYOBJ_SETSYMPTOM: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_SETSYMPTOM;
	} break;
	
	case ENTITYOBJ_ISSYMPTOMACTIVE: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_ISSYMPTOMACTIVE;
	} break;
	
	case ENTITYOBJ_GETGENRE: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_GETGENRE;
	} break;
	
	case ENTITYOBJ_GETHEALTH: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_GETHEALTH;
	} break;
	
	case ENTITYOBJ_SETHEALTH: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_SETHEALTH;
	} break;
	
	case ENTITYOBJ_GETEXTENDEDATTRIBUTE: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_GETEXTENDEDATTRIBUTE;
	} break;
	
	case ENTITYOBJ_SETEXTENDEDATTRIBUTE: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_SETEXTENDEDATTRIBUTE;
	} break;
	
	case ENTITYOBJ_GETLEVEL: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_GETLEVEL;
	} break;
	
	case ENTITYOBJ_SETLEVEL: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_SETLEVEL;
	} break;
	
	case ENTITYOBJ_GETEXPERIENCE: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_GETEXPERIENCE;
	} break;
	
	case ENTITYOBJ_SETEXPERIENCE: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_SETEXPERIENCE;
	} break;
	
	case ENTITYOBJ_GETINCOMBATACTIONPOINTS: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_GETINCOMBATACTIONPOINTS;
	} break;
	
	case ENTITYOBJ_GETACTIONPOINTS: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_GETACTIONPOINTS;
	} break;
	
	case ENTITYOBJ_SETACTIONPOINTS: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_SETACTIONPOINTS;
	} break;
	
	case ENTITYOBJ_ISHABILITYACTIVE: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_ISHABILITYACTIVE;
	} break;
	
	case ENTITYOBJ_SETHABILITY: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_SETHABILITY;
	} break;
	
	case ENTITYOBJ_USEHABILITY: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_USEHABILITY;
	} break;
	
	case ENTITYOBJ_ISRUNMODEACTIVE: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_ISRUNMODEACTIVE;
	} break;
	
	case ENTITYOBJ_SETRUNMODE: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_SETRUNMODE;
	} break;
	
	case ENTITYOBJ_MOVETO: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_MOVETO;
	} break;
	
	case ENTITYOBJ_ISMOVING: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_ISMOVING;
	} break;
	
	case ENTITYOBJ_STOPMOVING: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_STOPMOVING;
	} break;
	
	case ENTITYOBJ_EQUIPITEM: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_EQUIPITEM;
	} break;
	
	case ENTITYOBJ_REMOVEITEMEQUIPPED: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_REMOVEITEMEQUIPPED;
	} break;
	
	case ENTITYOBJ_GETITEMEQUIPPED: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_GETITEMEQUIPPED;
	} break;
	
	case ENTITYOBJ_ISITEMEQUIPPED: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_ISITEMEQUIPPED;
	} break;
	
	case ENTITYOBJ_DROPITEM: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_DROPITEM;
	} break;
	
	case ENTITYOBJ_USEITEM: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_USEITEM;
	} break;
	
	case ENTITYOBJ_MANIPULATE: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_MANIPULATE;
	} break;
	
	case ENTITYOBJ_SETTRANSPARENTMODE: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_SETTRANSPARENTMODE;
	} break;
	
	case ENTITYOBJ_ISTRANSPARENTMODEACTIVE: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_ISTRANSPARENTMODEACTIVE;
	} break;
	
	case ENTITYOBJ_CHANGEANIMORIENTATION: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_CHANGEANIMORIENTATION;
	} break;

	case ENTITYOBJ_GETANIMORIENTATION: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_GETANIMORIENTATION;
	} break;
	
	case ENTITYOBJ_SETALINGMENT: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_SETALINGMENT;
	} break;
	
	case ENTITYOBJ_SETALINGMENTWITH: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_SETALINGMENTWITH;
	} break;
	
	case ENTITYOBJ_SETALINGMENTAGAINST: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_SETALINGMENTAGAINST;
	} break;
	
	case ENTITYOBJ_GETALINGMENT: { 
	  pNode->OpcodeType = OP_ENTITYOBJ_GETALINGMENT;
	} break;
	
	case ENTITYOBJ_HITENTITY: {
	  pNode->OpcodeType = OP_ENTITYOBJ_HITENTITY;
	} break;

	case ENTITYOBJ_SETSCRIPT: {
	  pNode->OpcodeType = OP_ENTITYOBJ_SETSCRIPT;
	} break;

	case ENTITYOBJ_ISGHOSTMOVEMODEACTIVE: {
	  pNode->OpcodeType = OP_ENTITYOBJ_ISGHOSTMOVEMODEACTIVE;
	} break;
	  
	case ENTITYOBJ_SETGHOSTMOVEMODE: {
	  pNode->OpcodeType = OP_ENTITYOBJ_SETGHOSTMOVEMODE;
	} break;

	case ENTITYOBJ_GETRANGE: {
	  pNode->OpcodeType = OP_ENTITYOBJ_GETRANGE;
	} break;

	case ENTITYOBJ_ISINRANGE: {
	  pNode->OpcodeType = OP_ENTITYOBJ_ISINRANGE;
	} break;

	default:
	  assert(0);
  }; // ~ switch
  
  // Resto
  pNode->udOpcodePos = 0;
  pNode->pSigOpcode = pSigOpcode;
  pNode->nWasVisit = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea una tabla de simbolos inicializandola.
// Parametros:
// Devuelve:
// - La direccion de la tabla de simbolos
// Notas:
///////////////////////////////////////////////////////////////////////////////
sSymbolTable* 
SymTableCreate(void)
{
  // Crea tabla, la inicializa y retorna
  sSymbolTable* pSymTable = ALLOC(sSymbolTable);
  sword nIt = 0;
  for (; nIt < HASH_SIZE; ++nIt) {
	pSymTable->Table[nIt] = NULL;
  }
  pSymTable->pParent = NULL;
  return pSymTable;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera la tabla de simbolos recibidas, borrando de memoria todo su
//   contenido.
// Parametros:
// - pSymTable. Tabla de simbolos a borrar.
// Devuelve:
// - La direccion de la tabla de simbolos
// Notas:
// - NO borrara tabla de simbolos en enlace superior.
///////////////////////////////////////////////////////////////////////////////
void 
SymTableRelease(sSymbolTable* pSymTable)
{
  // Recorre las posiciones
  if (pSymTable) {
	sword nIt = 0;
	for (; nIt < HASH_SIZE; ++nIt) {
	  SymTableReleaseSymTableNodes(pSymTable->Table[nIt]);	
	}
  
	// Borra nodo
	Mem_Free(pSymTable);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Liberara todos los nodos de tabla de simbolos asociados a una posicion.
//   Este metodo sera de apoyo para SymTableRelease
// Parametros:
// - pNode. Enlace al primer nodo de una posicion de la tabla de simbolos.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
SymTableReleaseSymTableNodes(sSymTableNode* pNode) 
{
  // Recorre de forma recursiva y borra al deshacerse recursion
  if (pNode) {
	SymTableReleaseSymTableNodes(pNode->pSigNode);
	Mem_Free(pNode);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inserta en la tabla de simbolos pSymTable un identificador de tipo 
//   Const y devuelve dicho identificador. En caso de ya existir ese 
//   identificador se devolvera la direccion del nodo al mismo.
// Parametros:
// - pSymTable. Enlace a la tabla de simbolos
// - szIdentifier. Nombre del identificador.
// - pIdConst. Enlace a la declaracion en el AST.
// Devuelve:
// - La direccion del nodo insertado / localizado.
// Notas:
///////////////////////////////////////////////////////////////////////////////
sSymTableNode* 
SymTableInsertConst(sSymbolTable* pSymTable,
				    sbyte* szIdentifier,
					sConst* pIdConst)
{
  // Se halla posible nodo con dicho identificador
  const word unPos = SymTableHash(szIdentifier);
  sSymTableNode* pNode = SymTableFindNode(pSymTable, unPos, szIdentifier);

  // ¿No existe nodo?
  if (!pNode) {
	// Como no existe, se crea y se inserta al comienzo
	pNode = ALLOC(sSymTableNode);
	pNode->szIdentifier = szIdentifier;
	pNode->SymTableNodeType = SYM_TYPE_CONST;
	pNode->pIdConst = pIdConst;
	pNode->pSigNode = pSymTable->Table[unPos];	
	pSymTable->Table[unPos] = pNode;
  }

  // Se retorna nodo
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inserta en la tabla de simbolos pSymTable un identificador de tipo 
//   Var y devuelve dicho identificador. En caso de ya existir ese 
//   identificador se devolvera la direccion del nodo al mismo.
// Parametros:
// - pSymTable. Enlace a la tabla de simbolos
// - szIdentifier. Nombre del identificador.
// - pIdVar. Enlace a la declaracion en el AST.
// Devuelve:
// - La direccion del nodo insertado / localizado.
// Notas:
///////////////////////////////////////////////////////////////////////////////
sSymTableNode* 
SymTableInsertVar(sSymbolTable* pSymTable,
				  sbyte* szIdentifier,
				  sVar* pIdVar)
{
  // Se halla posible nodo con dicho identificador
  const word unPos = SymTableHash(szIdentifier);
  sSymTableNode* pNode = SymTableFindNode(pSymTable, unPos, szIdentifier);

  // ¿No existe nodo?
  if (!pNode) {
	// Como no existe, se crea y se inserta al comienzo
	pNode = ALLOC(sSymTableNode);
	pNode->szIdentifier = szIdentifier;
	pNode->SymTableNodeType = SYM_TYPE_VAR;
	pNode->pIdVar = pIdVar;
	pNode->pSigNode = pSymTable->Table[unPos];
	pSymTable->Table[unPos] = pNode;
  }

  // Se retorna nodo
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inserta en la tabla de simbolos pSymTable un identificador de tipo 
//   Argument y devuelve dicho identificador. En caso de ya existir ese 
//   identificador se devolvera la direccion del nodo al mismo.
//   El identificador de tipo argument sera tratado como una variable.
// Parametros:
// - pSymTable. Enlace a la tabla de simbolos
// - szIdentifier. Nombre del identificador.
// - pIdArgument. Enlace a la declaracion en el AST.
// Devuelve:
// - La direccion del nodo insertado / localizado.
// Notas:
///////////////////////////////////////////////////////////////////////////////
sSymTableNode* 
SymTableInsertArgument(sSymbolTable* pSymTable,
					   sbyte* szIdentifier,
					   sArgument* pIdArgument)
{
   // Se halla posible nodo con dicho identificador
  const word unPos = SymTableHash(szIdentifier);
  sSymTableNode* pNode = SymTableFindNode(pSymTable, unPos, szIdentifier);

  // ¿No existe nodo?
  if (!pNode) {
	// Como no existe, se crea y se inserta al comienzo
	pNode = ALLOC(sSymTableNode);
	pNode->szIdentifier = szIdentifier;
	pNode->SymTableNodeType = SYM_TYPE_ARG;
	pNode->pIdArg = pIdArgument;
	pNode->pSigNode = pSymTable->Table[unPos];
	pSymTable->Table[unPos] = pNode;
  }

  // Se retorna nodo
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inserta en la tabla de simbolos pSymTable un identificador de tipo 
//   Func y devuelve dicho identificador. En caso de ya existir ese 
//   identificador se devolvera la direccion del nodo al mismo.
// Parametros:
// - pSymTable. Enlace a la tabla de simbolos
// - szIdentifier. Nombre del identificador.
// - pIdFunc. Enlace a la declaracion en el AST.
// Devuelve:
// - La direccion del nodo insertado / localizado.
// Notas:
///////////////////////////////////////////////////////////////////////////////
sSymTableNode* 
SymTableInsertFunc(sSymbolTable* pSymTable,
				   sbyte* szIdentifier,
				   sFunc* pIdFunc)
{
  // Se halla posible nodo con dicho identificador
  const word unPos = SymTableHash(szIdentifier);
  sSymTableNode* pNode = SymTableFindNode(pSymTable, unPos, szIdentifier);

  // ¿No existe nodo?
  if (!pNode) {
	// Como no existe, se crea y se inserta al comienzo
	pNode = ALLOC(sSymTableNode);
	pNode->szIdentifier = szIdentifier;
	pNode->SymTableNodeType = SYM_TYPE_FUNC;
	pNode->pIdFunc = pIdFunc;
	pNode->pSigNode = pSymTable->Table[unPos];
	pSymTable->Table[unPos] = pNode;
  }

  // Se retorna nodo
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Busca de forma jerarquica a traves de la tabla de simbolos pSymTable la 
//   existencia o no del identificador szIdentifier y lo devuelve.
// Parametros:
// - pSymTable. Tabla de simbolos.
// - szIdentifier. Identificador a buscar
// Devuelve:
// - Si se hallo el nodo con el identificador el nodo, en caso contrario NULL.
// Notas:
///////////////////////////////////////////////////////////////////////////////
sSymTableNode* 
SymTableGetNode(sSymbolTable* pSymTable,
			    sbyte* szIdentifier)
{
  // ¿Tabla de simbolos valida?
  if (pSymTable) {
	// Se retorna el nodo o se sigue buscando
	sSymTableNode* pSymNode = SymTableFindNode(pSymTable,
											   SymTableHash(szIdentifier),
											   szIdentifier);
	return (pSymNode) ? 
		    pSymNode : SymTableGetNode(pSymTable->pParent, szIdentifier);	
  } else {
	// No, se retorna NULL
	return NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si el identificador szIdentifier se halla en la tabla de 
//   simbolos pSymTable. En caso de no hallarse NO se buscara en la jerarquia
//   de la tabla de simblos.
// Parametros:
// - pSymTable. Tabla de simbolos.
// - szIdentifier. Identificador
// Devuelve:
// - Si se encuentra 1, en caso contrario 0.
// Notas:
///////////////////////////////////////////////////////////////////////////////
sword 
SymTableIsPresent(sSymbolTable* pSymTable,
				  sbyte* szIdentifier)
{
  // ¿Tabla de simbolos valida?
  if (pSymTable) {
	// Si, se retorna resultado de la busqueda
	if (SymTableFindNode(pSymTable, 
						 SymTableHash(szIdentifier), 
						 szIdentifier)) {
	  return 1;
	} else {
	  return 0;
	}
  } else {
	// No, se retorna 0
	return 0;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Calcula una posicion donde poder insertar un nodo de la tabla hash.
// Parametros:
// - szKey. La llave (identificador) con la que calcular la posicion.
// Devuelve:
// - La posicion en donde insertar.
// Notas:
// - En cada pasada se multiplicara por dos el valor de la posicion y se le
//   sumara el codigo ASCII de szKey que corresponda, al final se devolvera
//   una posicion entre 0 y HASH_SIZE - 1.
///////////////////////////////////////////////////////////////////////////////
word 
SymTableHash(sbyte* szKey)
{
  // Calcula la posicion y retorna
  word unHashPos = 0;
  sbyte* szKeyTmp = Mem_Alloc(strlen(szKey) + 1);
  strcpy(szKeyTmp, szKey);
  strlwr(szKeyTmp);
  for (; *szKey; ++szKey) {
	unHashPos = (unHashPos << 1) + *szKeyTmp;
  }
  Mem_Free(szKeyTmp);
  return (unHashPos % HASH_SIZE);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Busca un nodo en la tabla de simbolos pSymTable, posicion unStartNodePos,
//   cuyo identificador sea igual a szIdentifier. 
// Parametros:
// - pSymTable. Tabla de simbolos en donde buscar.
// - unStartNodePos. Posicion donde comenzar a buscar.
// - szIdentifier. Identificador a buscar.
// Devuelve:
// - Si se localiza el nodo la direccion al mismo, en caso contrario NULL.
// Notas:
///////////////////////////////////////////////////////////////////////////////
sSymTableNode* 
SymTableFindNode(sSymbolTable* pSymTable,
			     word unStartNodePos,
				 sbyte* szIdentifier)
{
  // Itera entre los nodos 
  sSymTableNode* pNode = pSymTable->Table[unStartNodePos];
  for (; pNode != NULL; pNode = pNode->pSigNode) {
	// ¿Localizado?
	if (0 == strcmpi(pNode->szIdentifier, szIdentifier)) {
	  break;  
	}
  }

  // Se retorna
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sType de tipo TYPE_VOID
// Parametros:
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sType* 
MakeTypeVoid(void)
{
  // Crea el nodo, establece valores y retorna
  sType* pNode = ALLOC(sType);
  pNode->ValueType = TYPE_VOID;
  pNode->unSrcLine = GetActLine();
  return pNode;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sType de tipo TYPE_NUMBER
// Parametros:
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sType* 
MakeTypeNumber(void)
{
  // Crea el nodo, establece valores y retorna
  sType* pNode = ALLOC(sType);
  pNode->ValueType = TYPE_NUMBER;
  pNode->unSrcLine = GetActLine();
  return pNode;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sType de tipo TYPE_ENTITY
// Parametros:
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sType* 
MakeTypeEntity(void)
{
  // Crea el nodo, establece valores y retorna
  sType* pNode = ALLOC(sType);
  pNode->ValueType = TYPE_ENTITY;
  pNode->unSrcLine = GetActLine();
  return pNode;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sType de tipo TYPE_STRING
// Parametros:
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sType* 
MakeTypeString(void)
{
  // Crea el nodo, establece valores y retorna
  sType* pNode = ALLOC(sType);
  pNode->ValueType = TYPE_STRING;
  pNode->unSrcLine = GetActLine();
  return pNode;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sArgument de tipo ARGUMENT_VALUE
// Parametros:
// - pType. Tipo del argumento.
// - szIdentifier. Cadena con identificador.
// - pSigArg. Enlace al sig. argumento
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sArgument* 
MakeArgumentByValue(sType* pType, 
			        sbyte* szIdentifier,
					sArgument* pSigArg)
{
  // Crea nodo, establece valores y retorna
  sArgument* pNode = ALLOC(sArgument);
  pNode->ArgumentPassType = ARGUMENT_VALUE;
  pNode->pType = pType;
  pNode->szIdentifier = szIdentifier;
  pNode->pSigArgument = pSigArg;
  pNode->unSrcLine = GetActLine();
  pNode->unOffsetDecl = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sArgument de tipo ARGUMENT_REF
// Parametros:
// - pType. Tipo del argumento.
// - szIdentifier. Cadena con identificador.
// - pSigArg. Puntero a sig. argumento
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sArgument* 
MakeArgumentByRef(sType* pType, 
			      sbyte* szIdentifier,
				  sArgument* pSigArg)
{
  // Crea nodo, establece valores y retorna
  sArgument* pNode = ALLOC(sArgument);
  pNode->ArgumentPassType = ARGUMENT_REF;
  pNode->pType = pType;
  pNode->szIdentifier = szIdentifier;
  pNode->pSigArgument = pSigArg;
  pNode->unSrcLine = GetActLine();
  pNode->unOffsetDecl = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp de tipo EXP_IDENTIFIER
// Parametros:
// - szIdentifier. Identificador
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpIdentifier(sbyte* szIdentifier)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_IDENTIFIER;
  pNode->ExpIdentifier.szIdentifier = szIdentifier;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp de tipo EXP_GLOBAL_CONST_ENTITY
// Parametros:
// - unValue. Valor asociado a la constante
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpGlobalConstEntity(word unValue)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_GLOBAL_CONST_ENTITY;
  pNode->ExpGlobalConstEntity.unValue = unValue;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp de tipo EXP_GLOBAL_CONST_NUMBER
// Parametros:
// - unValue. Valor asociado a la constante
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpGlobalConstNumber(float fValue)
{
   // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_GLOBAL_CONST_NUMBER;
  pNode->ExpGlobalConstNumber.fValue = fValue;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp de tipo EXP_NUMBER_VALUE
// Parametros:
// - fValue. Valor numerico
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpNumberValue(float fValue)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_NUMBER_VALUE;
  pNode->ExpNumValue.fNumValue = fValue;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp de tipo EXP_STRING_VALUE
// Parametros:
// - szString. Cadena de caracteres
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpStringValue(sbyte* szString)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_STRING_VALUE;
  pNode->ExpStrValue.szStrValue = szString;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp de tipo EXP_ASSING
// Parametros:
// - szIdentifier. Identificador al que asignar el valor.
// - pRightExp. Exp asignada al identificador.
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpAssing(sbyte* szIdentifier, 
			  sExp* pRightExp)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_ASSING;
  pNode->ExpAssing.szIdentifier = szIdentifier;
  pNode->ExpAssing.pRightExp = pRightExp;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp subtipo EXP_EQUAL
// Parametros:
// - pRightExp. Enlace a expresion en el lado derecho
// - pLeftExp. Enlace a expresion en el lado izquierdo
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpEqual(sExp* pLeftExp,
			 sExp* pRightExp)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_EQUAL;
  pNode->ExpEqual.pRightExp = pRightExp;
  pNode->ExpEqual.pLeftExp = pLeftExp;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  pNode->ExpEqual.unTrueLabel = 0;
  pNode->ExpEqual.unEndLabel = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp subtipo EXP_NOEQUAL
// Parametros:
// - pRightExp. Enlace a expresion en el lado derecho
// - pLeftExp. Enlace a expresion en el lado izquierdo
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpNoEqual(sExp* pLeftExp,
			   sExp* pRightExp)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_NOEQUAL;
  pNode->ExpNoEqual.pRightExp = pRightExp;
  pNode->ExpNoEqual.pLeftExp = pLeftExp;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  pNode->ExpNoEqual.unTrueLabel = 0;
  pNode->ExpNoEqual.unEndLabel = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp subtipo EXP_LESS
// Parametros:
// - pRightExp. Enlace a expresion en el lado derecho
// - pLeftExp. Enlace a expresion en el lado izquierdo
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpLess(sExp* pLeftExp,
			sExp* pRightExp)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_LESS;
  pNode->ExpLess.pRightExp = pRightExp;
  pNode->ExpLess.pLeftExp = pLeftExp;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  pNode->ExpLess.unTrueLabel = 0;
  pNode->ExpLess.unEndLabel = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp subtipo EXP_LESSEQUAL 
// Parametros:
// - pRightExp. Enlace a expresion en el lado derecho
// - pLeftExp. Enlace a expresion en el lado izquierdo
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpLessEqual(sExp* pLeftExp,
			     sExp* pRightExp)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_LESSEQUAL;
  pNode->ExpLEqual.pRightExp = pRightExp;
  pNode->ExpLEqual.pLeftExp = pLeftExp;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  pNode->ExpLEqual.unTrueLabel = 0;
  pNode->ExpLEqual.unEndLabel = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp subtipo EXP_GREATER
// Parametros:
// - pRightExp. Enlace a expresion en el lado derecho
// - pLeftExp. Enlace a expresion en el lado izquierdo
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpGreater(sExp* pLeftExp,
			   sExp* pRightExp)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_GREATER;
  pNode->ExpGreater.pRightExp = pRightExp;
  pNode->ExpGreater.pLeftExp = pLeftExp;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  pNode->ExpGreater.unTrueLabel = 0;
  pNode->ExpGreater.unEndLabel = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp subtipo EXP_GREATEREQUAL
// Parametros:
// - pRightExp. Enlace a expresion en el lado derecho
// - pLeftExp. Enlace a expresion en el lado izquierdo
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpGreaterEqual(sExp* pLeftExp,
			        sExp* pRightExp)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_GREATEREQUAL;
  pNode->ExpGEqual.pRightExp = pRightExp;
  pNode->ExpGEqual.pLeftExp = pLeftExp;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  pNode->ExpGEqual.unTrueLabel = 0;
  pNode->ExpGEqual.unEndLabel = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp subtipo EXP_ADD
// Parametros:
// - pRightExp. Enlace a expresion en el lado derecho
// - pLeftExp. Enlace a expresion en el lado izquierdo
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpAdd(sExp* pLeftExp,
		   sExp* pRightExp)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_ADD;
  pNode->ExpAdd.pRightExp = pRightExp;
  pNode->ExpAdd.pLeftExp = pLeftExp;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp subtipo EXP_MINUS
// Parametros:
// - pRightExp. Enlace a expresion en el lado derecho
// - pLeftExp. Enlace a expresion en el lado izquierdo
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpMinus(sExp* pLeftExp,
			 sExp* pRightExp)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_MINUS;
  pNode->ExpMinus.pRightExp = pRightExp;
  pNode->ExpMinus.pLeftExp = pLeftExp;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp subtipo EXP_MULT
// Parametros:
// - pRightExp. Enlace a expresion en el lado derecho
// - pLeftExp. Enlace a expresion en el lado izquierdo
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpMult(sExp* pLeftExp,
			sExp* pRightExp)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_MULT;
  pNode->ExpMult.pRightExp = pRightExp;
  pNode->ExpMult.pLeftExp = pLeftExp;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp subtipo EXP_DIV
// Parametros:
// - pRightExp. Enlace a expresion en el lado derecho
// - pLeftExp. Enlace a expresion en el lado izquierdo
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpDiv(sExp* pLeftExp,
		   sExp* pRightExp)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_DIV;
  pNode->ExpDiv.pRightExp = pRightExp;
  pNode->ExpDiv.pLeftExp = pLeftExp;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp subtipo EXP_MODULO
// Parametros:
// - pRightExp. Enlace a expresion en el lado derecho
// - pRightExp. Enlace a expresion en el lado izquierdo
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpModulo(sExp* pLeftExp,
			  sExp* pRightExp)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_MODULO;
  pNode->ExpModulo.pRightExp = pRightExp;
  pNode->ExpModulo.pLeftExp = pLeftExp;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp subtipo EXP_UMINUS
// Parametros:
// - pExp. Enlace a expresion
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpUMinus(sExp* pExp)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_UMINUS;
  pNode->ExpUMinus.pExp = pExp;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp subtipo EXP_NOT
// Parametros:
// - pExp. Enlace a expresion
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpNot(sExp* pExp)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_NOT;
  pNode->ExpNot.pExp = pExp;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  pNode->ExpNot.unFalseLabel = 0; 
  pNode->ExpNot.unEndLabel = 0; 
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp subtipo EXP_AND
// Parametros:
// - pRightExp. Enlace a expresion en el lado derecho
// - pLeftExp. Enlace a expresion en el lado izquierdo
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpAnd(sExp* pLeftExp,
		   sExp* pRightExp)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_AND;
  pNode->ExpAnd.pRightExp = pRightExp;
  pNode->ExpAnd.pLeftExp = pLeftExp;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  pNode->ExpAnd.unFalseLabel = 0; 
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp subtipo EXP_OR
// Parametros:
// - pRightExp. Enlace a expresion en el lado derecho
// - pLeftExp. Enlace a expresion en el lado izquierdo
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpOr(sExp* pLeftExp,
		  sExp* pRightExp)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_OR;
  pNode->ExpOr.pRightExp = pRightExp;
  pNode->ExpOr.pLeftExp = pLeftExp;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();  
  pNode->ExpOr.unTrueLabel = 0; 
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sExp de tipo EXP_FUNC_INVOKE
// Parametros:
// - szIdentFunc. Identificador de funcion
// - pParams. Parametros pasados a la funcion
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpUserFuncInvoke(sbyte* szIdentFunc,
				      sExp* pParams)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_FUNC_INVOKE;
  pNode->ExpFuncInvoke.szIdentifier = szIdentFunc;
  pNode->ExpFuncInvoke.pParams = pParams;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo asociado a una invocacion de funcion de API
// Parametros:
// - APIFunc. Tipo de funcion de API.
// - pParams. Parametros con los que se invoca
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpAPIFunc(eExpAPIFunc APIFunc, 
			   sExp* pParams)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);  
  pNode->ExpType = EXP_APIFUNC_INVOKE;
  pNode->ExpAPIFuncInvoke.ExpAPIFunc = APIFunc;
  pNode->ExpAPIFuncInvoke.pParams = pParams;
  pNode->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo asociado a una invocacion de un metodo Game
// Parametros:
// - GameObjMethod. Tipo de metodo.
// - pParams. Parametros con los que se invoca
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpGameObj(eExpGameObjMethod GameObjMethod,
			   sExp* pParams)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_GAMEOBJ_INVOKE;
  pNode->ExpGameObjInvoke.ExpGameObjMethod = GameObjMethod;
  pNode->ExpGameObjInvoke.pParams = pParams;
  pNode->pSigExp = NULL;  
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo asociado a una invocacion de un metodo World
// Parametros:
// - WorldObjMethod. Metodo invocado
// - pParams. Parametros con los que se invoca
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpWorldObj(eExpWorldObjMethod WorldObjMethod,
			    sExp* pParams)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_WORLDOBJ_INVOKE;
  pNode->ExpWorldObjInvoke.ExpWorldObjMethod = WorldObjMethod;
  pNode->ExpWorldObjInvoke.pParams = pParams;
  pNode->pSigExp = NULL;  
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo asociado a una invocacion de un metodo Entity
// Parametros:
// - EntityObjMethod. Metodo invocado
// - pParams. Parametros con los que se invoca
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpEntityObj(eExpEntityObjMethod EntityObjMethod,
			     sExp* pParams)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_ENTITYOBJ_INVOKE;
  pNode->ExpEntityObjInvoke.szIdentifier = NULL;
  pNode->ExpEntityObjInvoke.ExpEntityObjMethod = EntityObjMethod;
  pNode->ExpEntityObjInvoke.pParams = pParams;
  pNode->pSigExp = NULL;  
  pNode->pExpType = NULL;  
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sExp de tipo EXP_CAST
// Parametros:
// - pType. Enlace al tipo del cast.
// - pExp. Enlace a la exp que cualifica
// Devuelve:
// - Direccion al nodo creado
// Notas:
// - Al crearse el nodo, se asignara como enlace el enlace de la expresion
//   recibida pasandose a NULL el de esta.
///////////////////////////////////////////////////////////////////////////////
sExp* 
MakeExpCast(sType* pType,
		    sExp* pExp)
{
  // Crea nodo, establece valores y retorna
  sExp* pNode = ALLOC(sExp);
  pNode->ExpType = EXP_CAST;
  pNode->ExpCast.pType = pType;
  pNode->ExpCast.pExp = pExp;
  pNode->pSigExp = pExp->pSigExp;
  pExp->pSigExp = NULL;
  pNode->pExpType = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sStm de tipo STM_SEQ
// Parametros:
// - pFirst. Enlace a primer Stm.
// - pSecond. Enlace a segundo Stm
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sStm* 
MakeStmSeq(sStm* pFirst,
		   sStm* pSecond)
{
  // Crea el nodo, establece valores y retorna
  sStm* pNode = ALLOC(sStm);
  pNode->StmType = STM_SEQ;
  pNode->StmSeq.pFirst = pFirst;
  pNode->StmSeq.pSecond = pSecond;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sStm de tipo STM_RETURN
// Parametros:
// - pExp. Enlace a expresion
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sStm* 
MakeStmReturn(sExp* pExp)
{
  // Crea el nodo, establece valores y retorna
  sStm* pNode = ALLOC(sStm);
  pNode->StmType = STM_RETURN;
  pNode->StmReturn.pExp = pExp;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sStm de tipo STM_IF
// Parametros:
// - pExp. Enlace a expresion.
// - pThenStm. Enlace a sentencias despues del then.
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sStm* 
MakeStmIf(sExp* pExp,
		  sStm* pThenStm)
{
  // Crea el nodo, establece valores y retorna
  sStm* pNode = ALLOC(sStm);
  pNode->StmType = STM_IF;
  pNode->If.pExp = pExp;
  pNode->If.pThenStm = pThenStm;
  pNode->unSrcLine = GetActLine();
  pNode->If.unEndLabel = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sStm de tipo STM_IFELSE
// Parametros:
// - pExp. Enlace a expresion.
// - pThenStm. Enlace a sentecias despues del then.
// - pElseStm. Enlace a sentencias despues del else.
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sStm* 
MakeStmIfElse(sExp* pExp,
			  sStm* pThenStm,
			  sStm* pElseStm)
{
  // Crea nodo, establece valores y retorna.
  sStm* pNode = ALLOC(sStm);
  pNode->StmType = STM_IFELSE;
  pNode->IfElse.pExp = pExp;
  pNode->IfElse.pThenStm = pThenStm;
  pNode->IfElse.pElseStm = pElseStm;
  pNode->unSrcLine = GetActLine();
  pNode->IfElse.unElseLabel = 0;
  pNode->IfElse.unEndLabel = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sStm de tipo STM_WHILE
// Parametros:
// - pExp. Enlace a la expresion.
// - pDoStm. Enlace a sentencia.
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sStm* 
MakeStmWhile(sExp* pExp,
		     sStm* pDoStm)
{
  // Crea nodo, establece valores y retorna
  sStm* pNode = ALLOC(sStm);
  pNode->StmType = STM_WHILE;
  pNode->While.pExp = pExp;
  pNode->While.pDoStm = pDoStm;
  pNode->unSrcLine = GetActLine();
  pNode->While.unStartLabel = 0;
  pNode->While.unEndLabel = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sStm de tipo STM_EXP
// Parametros:
// - pExp. Enlace a expresion simple
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sStm* 
MakeStmExp(sExp* pExp)
{
  // Crea nodo, establece valores y retorna
  sStm* pNode = ALLOC(sStm);
  pNode->StmType = STM_EXP;
  pNode->Exp.pExp = pExp;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sConst de tipo CONST_DECLSEQ
// Parametros:
// - pFirst. Enlace a primer sConst.
// - pSecond. Enlace a segundo sConst.
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sConst* 
MakeConstDeclSeq(sConst* pFirst,
			     sConst* pSecond)
{
  // Crea nodo, establece valores y retorna
  sConst* pNode = ALLOC(sConst);
  pNode->ConstType = CONST_DECLSEQ;
  pNode->ConstSeq.pFirst = pFirst;
  pNode->ConstSeq.pSecond = pSecond;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sConst de tipo CONST_DECL
// Parametros:
// - pType. Enlace al tipo del const.
// - szIdentifier. Nombre del identificador.
// - pExp. Enlace a la expresion asignada.
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sConst* 
MakeConstDecl(sType* pType,
		      sbyte* szIdentifier,
			  sExp* pExp)
{
  // Crea nodo, establece valores y retorna.
  sConst* pNode = ALLOC(sConst);
  pNode->ConstType = CONST_DECL;
  pNode->ConstDecl.pType = pType;
  pNode->ConstDecl.szIdentifier = szIdentifier;
  pNode->ConstDecl.pExp = pExp;
  pNode->unSrcLine = GetActLine();
  pNode->ConstDecl.unOffsetDecl = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sVar de tipo VAR_TYPEDECL_SEQ
// Parametros:
// - pFirst. Enlace al primer nodo.
// - pSecond. Enlace al segundo nodo
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sVar* 
MakeVarTypeDeclSeq(sVar* pFirst,
				   sVar* pSecond)
{
  // Crea nodo, establece valores y retorna
  sVar* pNode = ALLOC(sVar);
  pNode->VarType = VAR_TYPEDECL_SEQ;
  pNode->VarTypeDeclSeq.pFirst = pFirst;
  pNode->VarTypeDeclSeq.pSecond = pSecond;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sVar de tipo VAR_TYPEDECL
// Parametros:
// - pType. Enlace al tipo de los identificadores.
// - pIdentDecl. Enlace a la sucesion de identificadores declarados
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sVar* 
MakeVarTypeDecl(sType* pType,
			    sVar* pIdentDecl)
{
  // Crea nodo, establece valores y retorna
  sVar* pNode = ALLOC(sVar);
  pNode->VarType = VAR_TYPEDECL;
  pNode->VarTypeDecl.pType = pType;
  pNode->VarTypeDecl.pVarIdentSeq = pIdentDecl;
  pNode->unSrcLine = GetActLine();  
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sVar de tipo VAR_IDENTIFIERDECL_SEQ
// Parametros:
// - szIdentifier. Nombre del identificador.
// - pValue. Enlace a la expresion asociada al identificador.
// - pSigVar. Enlace al posible siguiente identificador.
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sVar* 
MakeVarIdentDeclSeq(sbyte* szIdentifier,
				    sExp* pValue,
					sVar* pSigVar)
{
  // Crea nodo, establece valores y retorna
  sVar* pNode = ALLOC(sVar);
  pNode->VarType = VAR_IDENTIFIERDECL_SEQ;
  pNode->VarIdentDeclSeq.szIdentifier = szIdentifier;
  pNode->VarIdentDeclSeq.pValue = pValue;
  pNode->VarIdentDeclSeq.pSigIdent = pSigVar;
  pNode->unSrcLine = GetActLine();
  pNode->VarIdentDeclSeq.unOffsetDecl = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sFunc de tipo FUNC_SEQ
// Parametros:
// - pFirst. Enlace al primer sFunc
// - pSecond. Enlace al segundo sFunc
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sFunc* 
MakeFuncSeq(sFunc* pFirst,
		    sFunc* pSecond)
{
  // Crea nodo, establece valores y retorna
  sFunc* pNode = ALLOC(sFunc);
  pNode->eFuncType = FUNC_SEQ;
  pNode->FuncSeq.pFirst = pFirst;
  pNode->FuncSeq.pSecond = pSecond;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sImport de tipo FUNC_DECL
// Parametros:
// - pFirst. Enlace al primer sImport.
// - pSecond. Enlace al segundo sImport
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sFunc* 
MakeFuncDecl(sType* pType,
		     sbyte* szIdentifier,
			 sArgument* pArguments,
			 sConst* pConst,
			 sVar* pVar,
			 sStm* pStm)
{
  // Crea nodo, establece valores y retorna
  sFunc* pNode = ALLOC(sFunc);
  pNode->eFuncType = FUNC_DECL;
  pNode->FuncDecl.pType = pType;
  pNode->FuncDecl.szIdentifier = szIdentifier;
  pNode->FuncDecl.pArguments = pArguments;
  pNode->FuncDecl.pConst = pConst;
  pNode->FuncDecl.pVar = pVar;
  pNode->FuncDecl.pStm = pStm;
  pNode->FuncDecl.pSymTable = NULL;
  pNode->unSrcLine = GetActLine();
  pNode->FuncDecl.unNumOffsets = 0;
  pNode->FuncDecl.unNumLabels = 0;
  pNode->FuncDecl.unWasInvoked = 0;
  pNode->FuncDecl.szSignature = NULL;
  pNode->FuncDecl.pOpcodeList = NULL;
  pNode->FuncDecl.pLabelList = NULL;
  pNode->FuncDecl.unStackSize = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sImport de tipo IMPORT_SEQ
// Parametros:
// - pFirst. Enlace al primer sImport.
// - pSecond. Enlace al segundo sImport
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sImport* 
MakeImportSeq(sImport* pFirst,
		      sImport* pSecond)
{
  // Crea nodo, establece valores y retorna
  sImport* pNode = ALLOC(sImport);
  pNode->ImportType = IMPORT_SEQ;
  pNode->ImportSeq.pFirst = pFirst;
  pNode->ImportSeq.pSecond = pSecond;  
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sImport de tipo IMPORT_FUNC
// Parametros:
// - szFileName. Nombre del fichero import
// - pFunctions. Enlace a la posible secuencia de funciones
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sImport* 
MakeImportFunc(sbyte* szFileName,
			   sFunc* pFunctions)
{
  // Crea nodo, establece valores y retorna
  sImport* pNode = ALLOC(sImport);
  pNode->ImportType = IMPORT_FUNC;
  pNode->ImportFunc.pFunctions = pFunctions;
  pNode->ImportFunc.szFileName = szFileName;
  pNode->ImportFunc.pSymTable = NULL;
  pNode->unSrcLine = GetActLine();  
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONSTARTGAME
// Parametros:
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnStartGame(void)
{
  // Crea nodo, establece valores y retorna
  sScriptType* pNode = ALLOC(sScriptType);
  pNode->ScriptEventType = SCRIPTEVENT_ONSTARTGAME;
  pNode->pArguments = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONCLICKHOURPANEL
// Argumentos:
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnClickHourPanel(void)
{
  // Crea nodo, establece valores y retorna
  sScriptType* pNode = ALLOC(sScriptType);
  pNode->ScriptEventType = SCRIPTEVENT_ONCLICKHOURPANEL;
  pNode->pArguments = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONFLEECOMBAT
// Argumentos:
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnFleeCombat(void)
{
  // Crea nodo, establece valores y retorna
  sScriptType* pNode = ALLOC(sScriptType);
  pNode->ScriptEventType = SCRIPTEVENT_ONFLEECOMBAT;
  pNode->pArguments = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONKEYPRESSED
// Argumentos:
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnKeyPressed(void)
{
  // Crea nodo, establece valores y retorna
  sScriptType* pNode = ALLOC(sScriptType);
  pNode->ScriptEventType = SCRIPTEVENT_ONKEYPRESSED;
  pNode->pArguments = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONSTARTCOMBATMODE
// Argumentos:
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnStartCombatMode(void)
{
  // Crea nodo, establece valores y retorna
  sScriptType* pNode = ALLOC(sScriptType);
  pNode->ScriptEventType = SCRIPTEVENT_ONSTARTCOMBATMODE;
  pNode->pArguments = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONENDCOMBATMODE
// Argumentos:
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnEndCombatMode(void)
{
  // Crea nodo, establece valores y retorna
  sScriptType* pNode = ALLOC(sScriptType);
  pNode->ScriptEventType = SCRIPTEVENT_ONENDCOMBATMODE;
  pNode->pArguments = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONNEWHOUR
// Argumentos:
// - szNewHour. Identificador number
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnNewHour(sbyte* szNewHour)
{
  // Crea nodo y establece tipo
  sScriptType* pNode = ALLOC(sScriptType);
  pNode->ScriptEventType = SCRIPTEVENT_ONNEWHOUR;

  // Crea la sucesion de argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeNumber(),
								          szNewHour,
								          NULL);

  // Retorna
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONENTERINAREA
// Argumentos:
// - szIDArea. Identificador del area.
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnEnterInArea(sbyte* szIDArea)
{
   // Crea nodo y establece tipo
  sScriptType* pNode = ALLOC(sScriptType);
  pNode->ScriptEventType = SCRIPTEVENT_ONENTERINAREA;

  // Crea la sucesion de argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeNumber(),
								          szIDArea,
								          NULL);

  // Retorna
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONWORLDIDLE
// Argumentos:
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnWorldIdle(void)
{
  // Crea nodo, establece valores y retorna
  sScriptType* pNode = ALLOC(sScriptType);
  pNode->ScriptEventType = SCRIPTEVENT_ONWORLDIDLE;
  pNode->pArguments = NULL;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONSETINFLOOR
// Argumentos:
// - szXPos, szYPos. Identificadores number
// - szTheEntity. Identificador entity
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnSetInFloor(sbyte* szXPos,
						   sbyte* szYPos,
						   sbyte* szTheEntity)
{
  // Crea nodo
  sScriptType* pNode = ALLOC(sScriptType); 
  
  // Crea sucesion de argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheEntity,
										  NULL);  
  pNode->pArguments = MakeArgumentByValue(MakeTypeNumber(),
						                  szYPos,
						                  pNode->pArguments);
  pNode->pArguments = MakeArgumentByValue(MakeTypeNumber(),
	                                      szXPos,
								          pNode->pArguments);

  // Establece tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONSETINFLOOR;    
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONSETOUTOFFLOOR
// Argumentos:
// - szXPos, szYPos. Identificadores number
// - szTheEntity. Identificador entity
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnSetOutOfFloor(sbyte* szXPos,
						      sbyte* szYPos,
							  sbyte* szTheEntity)
{
  // Crea nodo
  sScriptType* pNode = ALLOC(sScriptType);
  
  // Crea sucesion de argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
										  szTheEntity,
										  NULL);
  pNode->pArguments = MakeArgumentByValue(MakeTypeNumber(),
						                  szYPos,
						                  pNode->pArguments);
  pNode->pArguments = MakeArgumentByValue(MakeTypeNumber(),
	                                      szXPos,
								          pNode->pArguments);

  // Establece tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONSETOUTOFFLOOR;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONGETITEM
// Argumentos:
// - szTheItem. Argumento entity.
// - szTheCriature. Argumento entity
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnGetItem(sbyte* szTheItem,
					    sbyte* szTheCriature)
{
  // Crea nodo
  sScriptType* pNode = ALLOC(sScriptType);
  
  // Crea sucesion de argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheCriature,
								          NULL);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheItem,
								          pNode->pArguments);

  // Establece el tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONGETITEM;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONDROPITEM
// Argumentos:
// - szTheItem. Argumento entity.
// - szTheCriature. Argumento entity
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnDropItem(sbyte* szTheItem,
					     sbyte* szTheCriature)
{
  // Crea nodo
  sScriptType* pNode = ALLOC(sScriptType);
  
  // Crea sucesion de argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheCriature,
								          NULL);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheItem,
								          pNode->pArguments);

  // Establece el tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONDROPITEM;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONOBSERVEENTITY
// Argumentos:
// - szTheItem. Argumento entity.
// - szTheCriature. Argumento entity
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnObserveEntity(sbyte* szTheItem,
					          sbyte* szTheCriature)
{
  // Crea nodo
  sScriptType* pNode = ALLOC(sScriptType);
  
  // Crea sucesion de argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheCriature,
								          NULL);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheItem,
								          pNode->pArguments);

  // Establece el tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONOBSERVEENTITY;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONTALKTOENTITY
// Argumentos:
// - szTheItem. Argumento entity.
// - szTheCriature. Argumento entity
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnTalkToEntity(sbyte* szTheItem,
					         sbyte* szTheCriature)
{
  // Crea nodo y establece tipo
  sScriptType* pNode = ALLOC(sScriptType);

  // Crea sucesion de argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheCriature,
								          NULL);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheItem,
								          pNode->pArguments);

  // Establece el tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONTALKTOENTITY;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONMANIPULATEENTITY
// Argumentos:
// - szTheItem. Argumento entity.
// - szTheCriature. Argumento entity
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnManipulateEntity(sbyte* szTheItem,
					             sbyte* szTheCriature)
{
  // Crea nodo y establece tipo
  sScriptType* pNode = ALLOC(sScriptType);
  
  // Crea sucesion de argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheCriature,
								          NULL);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheItem,
								          pNode->pArguments);

  // Establece el tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONMANIPULATEENTITY;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONDEATH
// Argumentos:
// - szTheEntity. Argumento de tipo entity
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnDeath(sbyte* szTheEntity)
{
  // Se crea nodo y establece tipo
  sScriptType* pNode = ALLOC(sScriptType);
  pNode->ScriptEventType = SCRIPTEVENT_ONDEATH;

  // Se establecen argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheEntity,
								          NULL);

  // Establece el tipo y retorna
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONRESURRECT
// Argumentos:
// - szTheEntity. Argumento de tipo entity
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnResurrect(sbyte* szTheEntity)
{
  // Se crea nodo y establece tipo
  sScriptType* pNode = ALLOC(sScriptType);
  
  // Se establecen argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheEntity,
								          NULL);

  // Establece el tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONRESURRECT;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONINSERTINEQUIPMENTSLOT
// Argumentos:
// - szTheItem. Argumento de tipo entity.
// - szTheCriature. Argumento de tipo entity.
// - szTheEquipmentSlot. Argumento de tipo number
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnInsertInEquipmentSlot(sbyte* szTheItem,
						              sbyte* szTheCriature,
							          sbyte* szTheEquipmentSlot)
{
  // Se crea nodo y establece tipo
  sScriptType* pNode = ALLOC(sScriptType);
  
  // Se establecen argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeNumber(),
	                                      szTheEquipmentSlot,
								          NULL);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheCriature,
						                  pNode->pArguments);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheItem,
								          pNode->pArguments);

  // Establece el tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONINSERTINEQUIPMENTSLOT;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONREMOVEFROMEQUIPMENTSLOT
// Parametros:
// - szTheItem. Argumento de tipo entity.
// - szTheCriature. Argumento de tipo entity.
// - szTheEquipmentSlot. Argumento de tipo number
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnRemoveFromEquipmentSlot(sbyte* szTheItem,
								        sbyte* szTheCriature,
								        sbyte* szTheEquipmentSlot)
{
  // Se crea nodo y establece tipo
  sScriptType* pNode = ALLOC(sScriptType);
  
  // Se establecen argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeNumber(),
	                                      szTheEquipmentSlot,
								          NULL);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheCriature,
						                  pNode->pArguments);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheItem,
								          pNode->pArguments);

  // Establece el tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONREMOVEFROMEQUIPMENTSLOT;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONUSEHABILITY
// Argumentos:
// - szTheCriature. Argumento de tipo entity.
// - szHability. Argumento de tipo number.
// - szTheItem. Argumento de tipo entity.
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnUseHability(sbyte* szTheCriature,
						    sbyte* szHability,
							sbyte* szTheItem)
{
  // Se crea nodo y establece tipo
  sScriptType* pNode = ALLOC(sScriptType);

  // Se establecen argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheItem,
								          NULL);
  pNode->pArguments = MakeArgumentByValue(MakeTypeNumber(),
	                                      szHability,
						                  pNode->pArguments);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheCriature,
								          pNode->pArguments);

  // Retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONUSEHABILITY;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONACTIVATEDSYMPTOM
// Argumentos:
// - szTheCriature. Argumento de tipo entity.
// - szTheSymptom. Argumento de tipo number.
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnActivatedSymptom(sbyte* szTheCriature,
							     sbyte* szTheSymptom)
{
  // Se crea nodo y establece tipo
  sScriptType* pNode = ALLOC(sScriptType);

  // Se crean argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeNumber(),
	                                      szTheSymptom,
							              NULL);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheCriature,
						                  pNode->pArguments);

  // Establece el tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONACTIVATEDSYMPTOM;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONDEACTIVATEDSYMPTOM
// Argumentos:
// - szTheCriature. Argumento de tipo entity.
// - szTheSymptom. Argumento de tipo number.
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnDeactivatedSymptom(sbyte* szTheCriature,
								   sbyte* szTheSymptom)
{
  // Se crea nodo y establece tipo
  sScriptType* pNode = ALLOC(sScriptType);

  // Se crean argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeNumber(),
	                                      szTheSymptom,
								          NULL);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheCriature,
						                  pNode->pArguments);

  // Establece el tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONDEACTIVATEDSYMPTOM;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONHITENTITY
// Parametros:
// - szTheCriature. Argumento de tipo entity.
// - szTheSlotUsed. Argumento de tipo number.
// - szTheEntity. Argumento de tipo entity
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnHitEntity(sbyte* szTheCriature,
					      sbyte* szTheSlotUsed,
						  sbyte* szTheEntity)
{
  // Crea nodo y establece valor
  sScriptType* pNode = ALLOC(sScriptType);

  // Crea argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheEntity,
								          NULL);
  pNode->pArguments = MakeArgumentByValue(MakeTypeNumber(),
	                                      szTheSlotUsed,
						                  pNode->pArguments);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheCriature,
								          pNode->pArguments);

  // Establece el tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONHITENTITY;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONSTARTCOMBATTURN
// Parametros:
// - szTheCriature. Argumento de tipo entity.
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnStartCombatTurn(sbyte* szTheCriature)
{
  // Crea nodo y establece tipo
  sScriptType* pNode = ALLOC(sScriptType);
  
  // Establece argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheCriature,
							              NULL);

  // Establece el tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONSTARTCOMBATTURN;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONCRIATUREINRANGE
// Parametros:
// - szTheCriatureWithRange. Argumento de tipo entity.
// - szTheCriaturInRange. Argumento de tipo entity.
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnCriatureInRange(sbyte* szTheCriatureWithRange,
								sbyte* szTheCriatureInRange)
{
  // Crea nodo y establece tipo
  sScriptType* pNode = ALLOC(sScriptType);
  
  // Establece argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheCriatureInRange,
								          NULL);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheCriatureWithRange,
						                  pNode->pArguments);
  
  // Establece el tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONCRIATUREINRANGE;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONCRIATUREOUTOFRANGE
// Parametros:
// - szTheCriatureWithRange. Argumento de tipo entity.
// - szTheCriaturInRange. Argumento de tipo entity.
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnCriatureOutOfRange(sbyte* szTheCriatureWithRange,
							   	   sbyte* szTheCriatureOutOfRange)
{
  // Crea nodo y establece tipo
  sScriptType* pNode = ALLOC(sScriptType);
  
  // Establece argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheCriatureOutOfRange,
								          NULL);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheCriatureWithRange,
						                  pNode->pArguments);
  
  // Establece el tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONCRIATUREOUTOFRANGE;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONUSEITEM
// Parametros:
// - szTheItem. Argumento de tipo entity
// - szTheEntity. Argumento de tipo entity
// - szTheCriature. Argumento de tipo entity
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnUseItem(sbyte* szTheItem,
						sbyte* szTheEntity,
						sbyte* szTheCriature)
{
  // Crea nodo y establece tipo
  sScriptType* pNode = ALLOC(sScriptType);
  
  // Crea argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheCriature,
								          NULL);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheEntity,
						                  pNode->pArguments);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheItem,
								          pNode->pArguments);

  // Establece el tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONUSEITEM;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONTRADEITEM
// Parametros:
// - szTheItem. Argumento de tipo entity
// - szTheEntity. Argumento de tipo entity
// - szTheCriature. Argumento de tipo entity
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnTradeItem(sbyte* szTheItem,
					      sbyte* szTheEntitySrc,
						  sbyte* szTheEntityDest)
{
  // Crea nodo y establece tipo
  sScriptType* pNode = ALLOC(sScriptType);
  
  // Crea argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheEntityDest,
								          NULL);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheEntitySrc,
						                  pNode->pArguments);
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheItem,
								          pNode->pArguments);

  // Establece el tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONTRADEITEM;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONENTITYIDLE
// Parametros:
// - szTheEntity. Argumento de tipo entity
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnEntityIdle(sbyte* szTheEntity)
{
  // Crea nodo y establece tipo
  sScriptType* pNode = ALLOC(sScriptType);
  
  // Establece argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheEntity,
								          NULL);

  // Establece el tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONENTITYIDLE;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo sScriptType del tipo SCRIPTEVENT_ONENTITYCREATED
// Parametros:
// - szTheEntity. Argumento de tipo entity
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScriptType* 
MakeScriptTypeOnEntityCreated(sbyte* szTheEntity)
{
  // Crea nodo y establece tipo
  sScriptType* pNode = ALLOC(sScriptType);
  
  // Establece argumentos
  pNode->pArguments = MakeArgumentByValue(MakeTypeEntity(),
	                                      szTheEntity,
								          NULL);

  // Establece el tipo y retorna
  pNode->ScriptEventType = SCRIPTEVENT_ONENTITYCREATED;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sScript de tipo SCRIPT_SEQ
// Parametros:
// - pFirst. Enlace al primer sScript.
// - pSecond. Enlace al siguiente sScript.
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScript* 
MakeScriptSeq(sScript* pFirst,
		      sScript* pSecond)
{
  // Crea nodo, establece valores y retorna
  sScript* pNode = ALLOC(sScript);
  pNode->ScriptType = SCRIPT_SEQ;
  pNode->ScriptSeq.pFirst = pFirst;
  pNode->ScriptSeq.pSecond = pSecond;
  pNode->unSrcLine = GetActLine();
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea nodo sScript de tipo SCRIPT_DECL
// Parametros:
// - szFileName. Nombre del fichero con el script
// - pType. Tipo del script.
// - pImport. Imports de funciones globales.
// - pConst. Constantes.
// - pVar. Vbles.
// - pFunc. Funciones locales.
// - pStm. Sentencias asociadas
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sScript* 
MakeScriptDecl(sbyte* szFileName,
			   sScriptType* pType,
			   sImport* pImport,
			   sConst* pConst,
			   sVar* pVar,
			   sFunc* pFunc,
			   sStm* pStm)
{
  // Crea nodo, establece valores y retorna
  sScript* pNode = ALLOC(sScript);
  pNode->ScriptType = SCRIPT_DECL;
  pNode->ScriptDecl.pType = pType;
  pNode->ScriptDecl.pImport = pImport;
  pNode->ScriptDecl.pConst = pConst;
  pNode->ScriptDecl.pVar = pVar;
  pNode->ScriptDecl.pFunc = pFunc;
  pNode->ScriptDecl.pStm = pStm;
  pNode->ScriptDecl.szFileName = szFileName;
  pNode->ScriptDecl.pSymTable = NULL;
  pNode->unSrcLine = GetActLine();
  pNode->ScriptDecl.unNumOffsets = 0;
  pNode->ScriptDecl.unNumLabels = 0;
  pNode->ScriptDecl.unNumFunctions = 0;
  pNode->ScriptDecl.pOpcodeList = NULL;
  pNode->ScriptDecl.pLabelList = NULL;
  pNode->ScriptDecl.unStackSize = 0;
  pNode->ScriptDecl.slFileOffset = 0;
  return pNode;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Crea un nodo de tipo sGlobal. Al crear este nodo, se asociara de forma
//   AUTOMATICA, una sentencia de retorno al campo de sentencias y sera 
//   la unica sentencia del codigo global.
// Parametros:
// - pConst. Enlace a las constantes globales.
// - pVar. Enlace a las variales globales.
// - pScript. Enlace a los scripts.
// - szFileName. Nombre del fichero con las declaraciones globales
// Devuelve:
// - Direccion al nodo creado
// Notas:
///////////////////////////////////////////////////////////////////////////////
sGlobal* 
MakeGlobal(sbyte* szFileName,
		   sConst* pConst,
		   sVar* pVar,
		   sScript* pScript)
{
  // Crea nodo, establece valores y retorna
  sGlobal* pNode = ALLOC(sGlobal);
  pNode->pConst = pConst;
  pNode->pVar = pVar;
  pNode->pReturnStm = MakeStmReturn(NULL);
  pNode->pScript = pScript;
  pNode->szFileName = Mem_Alloc(strlen(szFileName) + 1);
  strcpy(pNode->szFileName, szFileName);
  pNode->pSymTable = NULL;
  pNode->unNumOffsets = 0;
  pNode->pOpcodeList = NULL;
  return pNode;
}