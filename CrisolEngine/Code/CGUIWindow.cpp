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
// CGUIWindow.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWindow.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWindow.h"

#include "SYSEngine.h"
#include "iCGFXManager.h"
#include "iCInputManager.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia.
// Parametros:
// Devuelve:
// Notas:
// - Si la clase esta inicializada, no dejara reinicializarse
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWindow::Init(void)
{
  // ¿Se intenta reinicializar?
  if (IsInitOk()) { 
	return false; 
  }

  // Inicializa y retorna
  m_bIsInitOk = true;
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWindow::End(void)
{
  // Finaliza la instancia si procede
  if (IsInitOk()) {
	// ¿Estaba activa?
	if (IsActive()) { 
	  Deactive(); 
	}

	// Se baja el flag de inicializacion
	m_bIsInitOk = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa la clase levantando el flag e instalando el cliente. 
// Parametros:
// - pGUIWClient. Direccion del posible cliente a asociar a la ventana. El
//   cliente podra recibir la notificacion de una interfaz particular ante
//   un determinado suceso, pero no todas las interfaces tendran porque realizar
//   notificacion alguna.
// Devuelve:
// Notas:
// - En caso de que ya estuviera activa, no hara nada.
// - Metodo virtual que debera de ser llamado siempre desde las clases
//   derivadas, en caso de que estas incorporen su propia version. 
// - El metodo desbloqueara la entrada por defecto, de tal modo que sera 
//   responsabilidad del exterior bloquearlo cuando sea necesario. Esto es asi
//   porque sera el comportamiento esperado por defecto cuando se active un
//   componente.
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWindow::Active(iCGUIWindowClient* const pGUIWClient)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se activa si procede
  if (!IsActive()) {
	// Instala el cliente de input
	InstallClient();
	
	// Se guarda direccion de cliente
	m_pGUIWClient = pGUIWClient;

	// Se levanta el flag de activacion
	m_bIsActive = true;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva la instancia desinstalando el cliente de entrada y bajando el
//   flag de activacion. Tambien pondra el input a false.
// - El posible cliente que hubiera asociado, desaparecera.
// Parametros:
// Devuelve:
// Notas:
// - En caso de estar ya desactivado no hara nada.
// - Metodo virtual que debera de ser llamado siempre desde las clases
//   derivadas, en caso de que estas incorporen su propia version. 
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWindow::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Se desactiva si procede
  if (IsActive()) {
	// Se desinstala el cliente
	UnistallClient();	
	
	// Se pierde posible cliente asociado
	m_pGUIWClient = NULL;
	
	// Se baja flag 
	m_bIsActive = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Bloquea / desbloquea la ventana de interfaz de cara al uso de entrada.
// Parametros:
// - bActive. Si vale true, recibira entrada si vale false dejara de recibir
//   actualizaciones de entrada.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWindow::SetInput(const bool bActive)
{
  // Solo si instancia inicializada
  ASSERT(IsInitOk());
  
  // Activa / desactiva la entrada segun proceda
  if (bActive) {
	SYSEngine::GetInputManager()->UnblockInputClient(this);	
  } else {
	SYSEngine::GetInputManager()->BlockInputClient(this);
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba si la interfaz esta bloqueada
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWindow::IsInput(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Retorna flag
  return (SYSEngine::GetInputManager()->IsInputClientBlocked(this) ? false : true);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Implementa el comportamiento por defecto para la notificacion de eventos
//   de los componentes CGUIComponent
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWindow::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si esta activa
  ASSERT(IsActive());
}
  
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Implementa comportamiento por defecto para la instalacion de clientes
// Parametros:
// Modifica:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWindow::InstallClient(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Implementa el comportamiento por defecto para la atencion a eventos de
//   entrada.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUIWindow::DispatchEvent(const InputDefs::sInputEvent& aInputEvent)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // El evento se comparte
  return InputDefs::SHARE_INPUT_EVENT;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Implementa el comportamiento por defecto para la desinstalacion de eventos
//   de entrada.
// Parametros:
// Modifica:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWindow::UnistallClient(void)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de realizar todos los pasos para un fade in. Los fades in
//   estan pensados para ser ejecutados cuando una ventana de interfaz 
//   se inicie.
// Parametros:
// - IDCmd. Identificador del comando.
// - uwSpeed. Velocidad del Fade. Si la velocidad es 0, se entendera que se
//   quiere hacer de inmediato.
// - DrawZone. Zona en donde realizar el Fade. Por defecto en DZ_CURSOR.
// - ubPlane. Plano de dibujado. Por defecto 0.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWindow::DoFadeIn(const CommandDefs::IDCommand& IDCmd,
					 const word uwSpeed,
					 const GraphDefs::eDrawZone DrawZone,
					 const byte ubPlane)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(IDCmd);

  // Se obtiene interfaz al subsistema de GFX
  iCGFXManager* pGFXManager = SYSEngine::GetGFXManager();
  ASSERT(pGFXManager);

  // Se ajusta el flag de realizacion inmediata y se establecen posibles
  // parameetros asociados a la accion no inmediata
  bool bDoNow;
  if (0 == uwSpeed) {
	bDoNow = true;
  } else {
	pGFXManager->SetFadeSpeed(CheckFadeSpeed(uwSpeed));
	bDoNow = false;
  }  

  // Se realiza el Fade
  pGFXManager->DoFade(GFXManagerDefs::FADE_IN, 
					  bDoNow, 
					  this, 
					  IDCmd, 
					  DrawZone, 
					  ubPlane);    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza los pasos para un fade out. Los fade out estan pensados para ser
//   ejecutados cuando una ventana de interfaz finalice
// Parametros:
// - IDCmd. Identificador del comando.
// - RGBColor. Color con el que realizar el Fade.
// - uwSpeed. Velocidad del Fade. Si la velocidad es 0, se entendera que se
//   quiere hacer de inmediato.
// - DrawZone. Zona en donde realizar el Fade. Por defecto en DZ_CURSOR.
// - ubPlane. Plano de dibujado. Por defecto 0.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWindow::DoFadeOut(const CommandDefs::IDCommand& IDCmd,							 
					  const GraphDefs::sRGBColor& RGBColor,
					  const word uwSpeed,
					  const GraphDefs::eDrawZone DrawZone,
					  const byte ubPlane)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros validos
  ASSERT(IDCmd);

  // Se obtiene interfaz al subsistema de GFX
  iCGFXManager* pGFXManager = SYSEngine::GetGFXManager();
  ASSERT(pGFXManager);

  // Se ajusta el flag de realizacion inmediata y se establecen posibles
  // parameetros asociados a la accion no inmediata
  bool bDoNow;
  if (0 == uwSpeed) {	
	bDoNow = true;	
  } else {
	bDoNow = false;
	pGFXManager->SetFadeSpeed(CheckFadeSpeed(uwSpeed));		
  } 

  // Se realiza el Fade
  pGFXManager->SetFadeOutColor(RGBColor);
  pGFXManager->DoFade(GFXManagerDefs::FADE_OUT, 
					  bDoNow, 
					  this, 
					  IDCmd, 
					  DrawZone,
					  ubPlane);
}



