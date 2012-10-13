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
// CGUIComponent.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIComponent.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUICInput.h"

#include "SYSEngine.h"
#include "iCInputManager.h"
#include "iCFontSystem.h"
#include "iCAlarmManager.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia.
// Parametros:
// - InitData. Datos de inicializacion.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar.
///////////////////////////////////////////////////////////////////////////////
bool 
CGUICInput::Init(const sGUICInputInitData& InitData)
{
  // SOLO si parametros correctos
  ASSERT(InitData.uwMaxCharacters);
  ASSERT((InitData.szInitText.size() <= InitData.uwMaxCharacters) != 0);
  ASSERT(InitData.ID);
  
  // ¿Se quiere reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  // Procede a inicializar clase base
  if (Inherited::Init(InitData)) {
	// Se asocia informacion relativa al texto, reservando antes el espacio max.
	m_InputInfo.szActText.reserve(InitData.uwMaxCharacters);
	m_InputInfo.szActText = InitData.szInitText;
	m_InputInfo.RGBTextSelect = InitData.RGBTextSelect;
	m_InputInfo.RGBTextUnselect = InitData.RGBTextUnselect;
	m_InputInfo.pRGBActTextColor = &m_InputInfo.RGBTextUnselect;

	// Num. maximo de caracteres
	m_InputInfo.uwMaxCharacters = InitData.uwMaxCharacters;

	// Se establece texto
	SetActInput(m_InputInfo.szActText);
	
	// Se establecen resto de vbles de miembro
	m_InputInfo.szFont = InitData.szFont;
	bIsSelect = false;
	bIsAcquiringInput = false;
	bIsInputActive = false;
  }

  // Retorna resultado
  return Inherited::IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICInput::End(void)
{
  // Finaliza instancia si procede
  if (Inherited::IsInitOk()) {
	// Se deja de adquirir entrada
	AcquireInput(false);
	
	// Desasocia cualquier texto
	m_InputInfo.szActText = "";
	
	// Propaga finalizacion
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el texto asociado a la entrada
// Parametros:
// - szActText. Texto asociado a la entrada
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICInput::SetActInput(const std::string& szActText)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se establece el texto y se configura posicion del cursor
  m_InputInfo.szActText = szActText;
  if (m_InputInfo.szActText.size()) {	  
    m_InputInfo.uwInputCursorPos = m_InputInfo.szActText.size();
  } else {
    m_InputInfo.uwInputCursorPos = 0;
  }
  m_InputInfo.uwPrevInputCursorPos = m_InputInfo.uwInputCursorPos;
  m_InputInfo.szActText.resize(m_InputInfo.uwInputCursorPos + 1);
  m_InputInfo.szActText[m_InputInfo.uwInputCursorPos] = ' ';
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Controla un flag local para el caso del procesamiento de caracteres.
// Parametros:
// - bActive. Establecimiento / desestablecimiento de la entrada.
// Devuelve:
// Notas:
// - Estaria pendiente variar el funcionamiento del InputManager para que
//   el control desde aqui no fuera estrictamente necesario.
// - Se debera de propagar la llamada
///////////////////////////////////////////////////////////////////////////////
void 
CGUICInput::SetInput(const bool bActive)
{
  // Se establece flag y se propaga la llamada
  bIsInputActive = bActive;
  Inherited::SetInput(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado de informacion
// Parametros:
// Devuelve:
// Notas:
// - El texto se dibujara en X e Y con un pequeño margen.
///////////////////////////////////////////////////////////////////////////////
void 
CGUICInput::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // SOLO en caso de que el componente este activo
  if (Inherited::IsActive()) {
	// Dibuja texto
    SYSEngine::GetFontSystem()->Write(Inherited::GetXPos() + 5,
									  Inherited::GetYPos() + 1,
									  0,
									  m_InputInfo.szActText,
									  *m_InputInfo.pRGBActTextColor,
									  GraphDefs::sAlphaValue(255),
									  m_InputInfo.szFont);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la seleccion de los elementos.
// Parametros:
// - bSelect. Flag de seleccion.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICInput::Select(const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se procede a cambiar el color del texto
  if (bSelect) {
	m_InputInfo.pRGBActTextColor = &m_InputInfo.RGBTextSelect;
  } else {
	m_InputInfo.pRGBActTextColor = &m_InputInfo.RGBTextUnselect;
  }
  
  // Se guarda flag
  bIsSelect = bSelect;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Procesa un evento de entrada de caracter o bien propaga la llamada a
//   la clase base si es un evento de control.
// Parametros:
// - InputEvent. Evento.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUICInput::DispatchEvent(const InputDefs::sInputEvent& InputEvent)
{  
  // ¿Evento asociado a la entrada de caracteres?
  if (InputEvent.EventType == InputDefs::CHAR_EVENT) {
	// ¿Se puede escribir?	
	if (bIsInputActive) {
	  ASSERT(IsAcquiringInput());
	  OnNewCharacterEvent(InputEvent.CharEvent);
	  return InputDefs::NO_SHARE_INPUT_EVENT;	
	}	
  }

  // Se propaga a la clase base
  return Inherited::DispatchEvent(InputEvent);  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Procesa el evento relacionado con la introduccion de un nuevo caracter.
// Parametros:
// - CharInfo. Info asociada al caracter recibido.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICInput::OnNewCharacterEvent(const InputDefs::sInputCharInfo& CharInfo)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());

  // Vbles
  ResDefs::WAVSoundHandle hWAVSound = 0; // Sonido a reproducir

  // Procede a procesar la informacion
  switch (CharInfo.ubChar) {
	case '\b': {
	  // Retroceso
	  // Borrado de un caracter
	  // ¿No se esta al comienzo?
	  if (m_InputInfo.uwInputCursorPos) {
		// Se retrocede posicion y se cambia el tamaño del string		
		const bool bIsMark = ('_' == m_InputInfo.szActText[m_InputInfo.uwInputCursorPos]);
		m_InputInfo.szActText.resize(m_InputInfo.uwInputCursorPos);		
		--m_InputInfo.uwInputCursorPos;
		m_InputInfo.szActText[m_InputInfo.uwInputCursorPos] = bIsMark ? '_' : ' ';		
	  }	  
	} break;

	case '\r': {
	  // Retorno de carro (Enter)
	  // Salida con aceptacion
	  AcquireInput(false);

	  // Notifica evento si procede
	  if (Inherited::IsEvent(GUIDefs::GUIC_INPUT_ACCEPT_ACQUIRING)) {
		Inherited::SendEventToGUIWindow(GUIDefs::GUIC_INPUT_ACCEPT_ACQUIRING);
	  }
	} break;

	case '\x1B': {
	  // Escape 
	  // Salida sin aceptacion
	  // Se retorna texto previo
	  m_InputInfo.szActText = m_InputInfo.szTmpText;
	  m_InputInfo.uwInputCursorPos = m_InputInfo.uwPrevInputCursorPos;
	  AcquireInput(false);

	  // Notifica evento si procede
	  if (Inherited::IsEvent(GUIDefs::GUIC_INPUT_NOACCEPT_ACQUIRING)) {
		Inherited::SendEventToGUIWindow(GUIDefs::GUIC_INPUT_NOACCEPT_ACQUIRING);
	  }
	} break;

	default: {
	  // Entrada de un caracter normal aceptado
	  if (CharInfo.ubChar >= 32) {
		// ¿Es posible la introduccion de mas caracteres?
		if (m_InputInfo.uwInputCursorPos < m_InputInfo.uwMaxCharacters) {
		  // Se cambia el tamaño del string y se inserta informacion		  
		  const bool bIsMark = ('_' == m_InputInfo.szActText[m_InputInfo.uwInputCursorPos]);
		  m_InputInfo.szActText[m_InputInfo.uwInputCursorPos] = CharInfo.ubChar;		  
		  ++m_InputInfo.uwInputCursorPos;
		  m_InputInfo.szActText.resize(m_InputInfo.uwInputCursorPos + 1);		  
		  m_InputInfo.szActText[m_InputInfo.uwInputCursorPos] = bIsMark ? '_' : ' ';		  
		}
	  }
	} break;
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion del vencimiento del temporizador utilizado para hacer que
//   la marca de fin de introduccion de datos parpadee.
// Parametros:
// - AlarmType. Tipo de alarma.
// - hAlarm. Handle a la alarma.
// Devuelve:
// Notas:
// - Es seguro que la alarma recibida esta asociada a la marca fin de entrada
///////////////////////////////////////////////////////////////////////////////
void 
CGUICInput::AlarmNotify(const AlarmDefs::eAlarmType& AlarmType,
						const AlarmDefs::AlarmHandle& hAlarm)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(hAlarm == hEndMarkAlarm);

  // Se alterna la marca de fin de entrada segun proceda
  if ('_' == m_InputInfo.szActText[m_InputInfo.uwInputCursorPos]) {
	m_InputInfo.szActText[m_InputInfo.uwInputCursorPos] = ' ';
  } else {
	m_InputInfo.szActText[m_InputInfo.uwInputCursorPos] = '_';
  }
  
  // Se reinstala el temporizador
  hEndMarkAlarm = SYSEngine::GetAlarmManager()->InstallTimeAlarm(this, 0.2f);
  ASSERT(hEndMarkAlarm);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa / Desactiva el permiso para procede a recibir entrada de caracteres.
//   Activar el permiso supone instalar el cliente para que sea capaz de
//   procesar caracteres introducidos. Desactivar supone todo lo contrario.
// - Sera obligatorio que el componente este seleccionado para poder activar
//   la entrada y que este activo.
// - Al activarse la entrada de caracteres, se guaradara el string actual de
//   tal forma que si se produce una salida sin aceptacion, se reponga este.
// - Se asociara un temporizador que se encargue de controlar la intermitencia
//   de la marca fin de entrada.
// Parametros:
// - bAcquire. Flag de adquisicion.
// Devuelve:
// Notas:
// - En caso de que se desee poner en un estado en el que ya se encuentre el
//   manager, no se realizara accion alguna.
// - El que el flujo no este activo no importara, pues sera algo que se
//   controle desde el mismo metodo que se encargue de despachar la entrada.
///////////////////////////////////////////////////////////////////////////////
void 
CGUICInput::AcquireInput(const bool bAcquire)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿NO esta el componente activo? o 
  // ¿NO esta el componente seleccionado? o  
  // ¿Se desea pasar a un estado ya activo?
  if (!Inherited::IsActive() || 
	  !IsSelect() ||
	  bAcquire == bIsAcquiringInput) { 
	return;
  }

  // ¿Se desea adquirir entrada?
  if (bAcquire) {
	// Se instala cliente y se guarda input actual
	SYSEngine::GetInputManager()->SetInputCharClient(this);	
	m_InputInfo.szTmpText = m_InputInfo.szActText;	
	m_InputInfo.uwPrevInputCursorPos = m_InputInfo.uwInputCursorPos;

	// Se instala el temporizador
	hEndMarkAlarm = SYSEngine::GetAlarmManager()->InstallTimeAlarm(this, 0.2f);
	ASSERT(hEndMarkAlarm);

	// Se asocia marca
	m_InputInfo.szActText[m_InputInfo.uwInputCursorPos] = '_';
  } else {
	// Se desinstala cliente	
	SYSEngine::GetInputManager()->UnsetInputCharClient(this);

	// Se desinstala el temporizador
	ASSERT(hEndMarkAlarm);
	SYSEngine::GetAlarmManager()->UninstallAlarm(hEndMarkAlarm);
	
	// Se desasocia marca
	m_InputInfo.szActText[m_InputInfo.uwInputCursorPos] = ' ';
  }

  // Se establece flag recibido
  bIsAcquiringInput = bAcquire;  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la anchura del area del componente
// Parametros:
// Devuelve:
// - Altura del area del componente
// Notas:
///////////////////////////////////////////////////////////////////////////////
word 
CGUICInput::GetWidth(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Obtiene la anchura y la retorna
  word uwWidth, uwHeight;
  SYSEngine::GetFontSystem()->GetTextDim(GetActInput(),
										 m_InputInfo.szFont,
										 uwWidth,
										 uwHeight);
  return uwWidth;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la altura del area del componente
// Parametros:
// Devuelve:
// - Altura del area del componente
// Notas:
///////////////////////////////////////////////////////////////////////////////
word 
CGUICInput::GetHeight(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Obtiene la altura y la retorna
  word uwWidth, uwHeight;
  SYSEngine::GetFontSystem()->GetTextDim(GetActInput(),
										 m_InputInfo.szFont,
										 uwWidth,
										 uwHeight);
  return uwHeight;
}