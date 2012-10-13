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
// CMathUtil.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CMathUtil.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CMathUtil.h"

#include "iCLogger.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la clase si no estaba inicializada. En caso contrario, devolvera
//   false indicando que para reicinializar antes habra que llamar a End.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CMathUtil::Init(void)
{
  // ¿Clase ya inicializada?
  if (IsInitOk()) { 
	return false; 
  }

  #ifdef ENGINE_TRACE    
	SYSEngine::GetLogger()->Write("CMathUtil::Init> Inicializando utilidades matemáticas.\n");         
  #endif

  // Inicializa tablas trigonometricas
  InitTrigonometricTables();
  
  // Todo correcto
  #ifdef ENGINE_TRACE    
	SYSEngine::GetLogger()->Write("               | Ok.\n");         
  #endif
  m_bIsInitOk = true;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CMathUtil::End(void)
{
  if (IsInitOk()) {    
	#ifdef ENGINE_TRACE    
	  SYSEngine::GetLogger()->Write("CMathUtil::End> Finalizando utilidades matemáticas.\n");         
	#endif

	// Se baja flag
	#ifdef ENGINE_TRACE    
	  SYSEngine::GetLogger()->Write("              | Ok.\n");         
	#endif
    m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa tablas trigonometricas
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void CMathUtil::InitTrigonometricTables(void)
{
  // Realiza la inicializacion.
  // Nota: Como sin y cos trabajan con radianes, habra que pasar angulos a 
  // radianes (rad = grados / 180 * PI)
  float fIt = 0.0f;
  for (; fIt < 360.0f; ++fIt) {
    const float fTemp = ((360.0f - fIt) / 180.0f) * PI;
	const word uwIt = word(fIt);
    SinTable[uwIt] = sin(fTemp);
    CosTable[uwIt] = cos(fTemp);		
  }
}

