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
// CTimer.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CTimer.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CTimer.h"

#include "iCLogger.h"
#include "SYSEngine.h"
#include "mmsystem.h" 
#include "TimerDefs.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializara la instancia a CTimer
// Parametros:
// - bPerformanceOnly: Si vale true, solo se buscara el timer por hardware.
//   Si vale false (valor por defecto), sera aceptado tambien el timer 
//   existente (el temporizador multimedia de Windows) en todos los equipos 
//   (aunque tiene menos resolucion).
// Devuelve:
// - El valor de m_bInitOk.
// Notas:
// - La resolucion del contador por hardware es del orden de 10-^6 y el del
//   contador convencional de 10-^3. Si bien, en el caso del contador por
//   hadware se realiza el calculo por motivos de tener una mayor exactitud. 
///////////////////////////////////////////////////////////////////////////////
bool 
CTimer::Init(const bool bPerformanceOnly)
{
  // Si la clase estaba inicializada, la dejara finalizada antes
  if (IsInitOk()) { 
	End(); 
  }

  #ifdef ENGINE_TRACE    
    SYSEngine::GetLogger()->Write("CTimer::Init> Inicializando el timmer del sistema.\n");           
  #endif  

  // Se pregunta por el contador por hardware
  sqword sqFrequency;
  if (QueryPerformanceFrequency( (LARGE_INTEGER *) &sqFrequency)) {
    // Existe el contador por hardware; se obtiene el valor actual    
    QueryPerformanceCounter( (LARGE_INTEGER *) &m_sqInitTime);    

	// Se halla la resolucion del contador
    m_dTimeScale = 1.0 / sqFrequency; 
    m_bPerfCounter = true;                  	
	
	// Todo correcto	
	#ifdef ENGINE_TRACE    
	  SYSEngine::GetLogger()->Write("            | PerformanceCounter instalado.\n");       
	  SYSEngine::GetLogger()->Write("            | Resolución: %f.\n", m_dTimeScale); 
	  SYSEngine::GetLogger()->Write("            | Ok.\n");
	#endif    
	m_bInitOk = true;               
	return true;
  } else if (!bPerformanceOnly) {
	// Se toma valor actual del timmer multimedia y se determina resolucion
    m_sqInitTime = timeGetTime(); 
    m_dTimeScale = 0.0001;        
    m_bPerfCounter = false;       

	// Todo correcto
    m_bInitOk = true;                 
	#ifdef ENGINE_TRACE        
	  SYSEngine::GetLogger()->Write("            | Windows Multimedia Timer instalado.\n");        
	  SYSEngine::GetLogger()->Write("            | Resolución: %f.\n", m_dTimeScale); 
	  SYSEngine::GetLogger()->Write("            | Ok.\n");
	#endif
	return true;
  }
  

  // Devuelve resultado de inicializacion
  #ifdef ENGINE_TRACE      
   SYSEngine::GetLogger()->Write("            | Error.\n");  
  #endif  
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia actual.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CTimer::End(void)
{
  if (IsInitOk()) { 	
	#ifdef ENGINE_TRACE        
	  SYSEngine::GetLogger()->Write("CTimer::End> Desinstalando timer del sistema.\n");      
	#endif	
	
	// Se baja el flag de inicializacion
    m_bInitOk = false;      
	#ifdef ENGINE_TRACE        
	  SYSEngine::GetLogger()->Write("           | Ok.\n");      
	#endif	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve el tiempo actual en las unidades especificadas. 
// Parametros:
// - Units. Unidades en las que se desea devolver el tiempo. Consultar 
//   TimerDefs para mas informacion. Por defecto se devuelven en milisegundos.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
sqword 
CTimer::GetTime(const TimerDefs::TimeUnits& Units)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se obtiene el tiempo actual
  static sqword sqTime;
  if (m_bPerfCounter) { 
    // Usando el performance counter
    QueryPerformanceCounter( (LARGE_INTEGER *) &sqTime); 
  } else { 
    // Usando el temporizador multimedia
    sqTime = timeGetTime(); 
  }
  
  // Se calcula el tiempo en base al periodo y a las unidades y se devuelve  
  return sqTime * m_dTimeScale * Units;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se devuelve el tiempo que ha transcurrido desde que se inicializo el
//   timer. Las unidades en las que lo devolvera estaran especificadas en Units.
// Parametros:
// - Units. Unidades en que devolver el tiempo. Consultar TimerDefs para mayor
//   informacion. Por defecto son milisegundos.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
sqword 
CTimer::GetTimeElapsedFromInit(const TimerDefs::TimeUnits& Units)
{
  // Solo si instancia inicializada
  ASSERT(IsInitOk());

  // Se halla el tiempo transcurrido y se devuelve  
  return GetTime(Units) - (m_sqInitTime * m_dTimeScale * Units);
}