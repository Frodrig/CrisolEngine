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
// CGUIWBaseTextSelector.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWBaseTextSelector.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWBaseTextSelector.h"

#include "SYSEngine.h"
#include "iCGUIManager.h"
#include "CCBTEngineParser.h"
#include "CMemoryPool.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar.
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseTextSelector::Init(void)
{
  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  // Se inicializa clase base
  if (Inherited::Init()) {
	// Se inicializan vbles de miembro
	m_bInGetOptionMode = false;
	
	// Todo correcto
	return true;
  }

  // Hubo problemas
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseTextSelector::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	// Desactiva (al desactivar se finalizaran posibles opciones)
	Deactive();
	
	// Finaliza FXBack
	m_Options.FXBack.End();
	
	// Propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Carga informacion relativa a las opciones
// Parametros:
// - pParser. Parser a utilizar.
// - ubDrawPlane. Plano de dibujado para las opciones.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseTextSelector::TLoadTextOptionsInfo(CCBTEngineParser* const pParser,
										    const byte ubDrawPlane)
{
  // SOLO si parametros validos
  ASSERT(pParser);

  // Establece prefijo
  pParser->SetVarPrefix("TextOptionsFormat.");

  // Obtiene informacion general sobre las opciones
  // Num. maximo de lineas
  m_Options.uwNumMaxLines = pParser->ReadNumeric("MaxLines");
  
  // Cfg base de componentes
  m_Options.CfgOptions.Position = pParser->ReadPosition();
  m_Options.CfgOptions.RGBSelectColor = pParser->ReadRGBColor("Select.");
  m_Options.CfgOptions.RGBUnselectColor = pParser->ReadRGBColor("Unselect.");  
  m_Options.CfgOptions.uwWidthJustify = pParser->ReadNumeric("AreaWidth");
  m_Options.CfgOptions.ubDrawPlane = ubDrawPlane;
  m_Options.CfgOptions.szFont = "Arial";
  
  // Se lee la configuracion para el FX de fondo
  // Se ajustan parametros para el FX
  const sPosition FXDrawPos(m_Options.CfgOptions.Position.swXPos - 4,
							m_Options.CfgOptions.Position.swYPos - 2);
  const word uwHeight = FontDefs::CHAR_PIXEL_HEIGHT * m_Options.uwNumMaxLines + 2;
  if (!m_Options.FXBack.Init(FXDrawPos, 
						     ubDrawPlane,
							 m_Options.CfgOptions.uwWidthJustify + 6,
							 uwHeight,
							 pParser->ReadRGBColor("FXBack."),
							 pParser->ReadAlpha("FXBack."))) {
	return false;
  }  

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Solicita añadir una opcion. Esta opcion ira a un map donde se registraran
//   todas las solicitudes. Cuando se reciba la orden de preguntar por una
//   opcion, las opciones registradas en ese map seran volcadas a componentes
//   para que se obtenga la deseada por el jugador. 
// - En caso de añadir una opcion ya registrada, esta sera sustituida.
// - No se dejara añadir opcion con identificador nulo ni con texto vacio.
// Parametros:
// - IDOption. Identificador de la opcion.
// - szText. Texto asociado a la opcion.
// - bCanSelect. ¿Puedo actuar sobre la opcion?
// Devuelve:
// - Si se ha podido añadir true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseTextSelector::AddOption(const GUIDefs::GUIIDComponent& IDOption,
								 const std::string& szText,
								 const bool bCanSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(szText.size());

  // ¿ID no valido u texto vacio?
  if (!IDOption ||
	  IDOption > CGUIWBaseTextSelector::MAX_IDOPTION_VALUE ||
	  szText.empty()) {
	return false;
  }

  // Se comprueba si la opcion ya fue añadida
  OptionsMapIt It(m_OptionsToAdd.find(IDOption));
  if (It != m_OptionsToAdd.end()) {
	// Si, se sustituye el valor anterior
	It->second.szText = szText;
	It->second.bCanSelect = bCanSelect;
  } else {
	// No, se añade nueva entrada
	m_OptionsToAdd.insert(OptionsMapValType(IDOption, sOption(szText, bCanSelect)));
  }

  // Se retorna
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Orden necesaria para insertar todas las opciones que se hallen sobre
//   el map de solicitudes. El interfaz escuchara al jugador para ver que
//   selecciona y notificara al cliente asociado.
// - En caso de que no exista ninguna opcion asociada, se ignorara la llamada.
// Parametros:
// - pClient. Cliente a notificar la asociacion.
// Devuelve:
// - Si la llamada ha tenido exito y existe mas de una opcion asociada
//   true, en caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseTextSelector::GetOption(iCGUIWindowClient* const pClient)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  ASSERT(Inherited::IsActive());
  // SOLO si parametros validos
  ASSERT(pClient);

  // ¿Se esta en proceso de obtencion de opcion?
  if (m_bInGetOptionMode) {
	// No se puede comenzar a preguntar
	return false;
  }

  // Iterara entre las solicitudes convirtiendolas en componentes
  word uwNumOptions = 0;
  OptionsMapIt It(m_OptionsToAdd.begin());
  for (; It != m_OptionsToAdd.end(); It = m_OptionsToAdd.erase(It)) {
	// Se construye el componente
	m_Options.CfgOptions.szText = It->second.szText;
	m_Options.CfgOptions.ID = It->first;  
	if (It->second.bCanSelect) {	
	  m_Options.CfgOptions.pGUIWindow = this;
	  m_Options.CfgOptions.GUIEvents = GUIDefs::GUIC_SELECT | 
									   GUIDefs::GUIC_UNSELECT |
									   GUIDefs::GUIC_BUTTONLEFT_PRESSED;
	} else {
	  m_Options.CfgOptions.pGUIWindow = NULL;
	  m_Options.CfgOptions.GUIEvents = GUIDefs::GUIC_NO_EVENT;
	}
    
	// ¿Inicializacion correcta y numero de lineas validas?
	const word uwPos = It->first - 1;
	if (m_TextComponents[uwPos].Init(m_Options.CfgOptions) &&
		m_TextComponents[uwPos].GetVisibleTextLines() <= m_Options.uwNumMaxLines) {	
	  // Inserta posicion donde localizar el componente
	  m_Options.Options.push_back(uwPos);
	  ++uwNumOptions;
	}
  }  

  // ¿Todo fue bien?
  if (uwNumOptions) {
	// Si, se asocia cliente y vincula valores
	Inherited::SetGUIWindowClient(pClient);
	InitIterators();
	ActiveVisibleOptions(true);
	m_bInGetOptionMode = true;

	// Muestra el cursor y retorna	 
	SYSEngine::GetGUIManager()->SetGUICursor(GUIManagerDefs::WINDOWINTERFAZ_CURSOR);
	return true;
  } else {
	// No, no se añadieron opciones
	return false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Avanza a la siguiente opcion siempre que sea posible.
// Parametros:
// Devuelve:
// - Si se ha podido realizar la operacion true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseTextSelector::NextOption(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿ES posible avanzar a la sig. opcion?
  if (Inherited::IsActive() &&
	  m_Options.LastViewOptionIt != m_Options.LastOptionIt) {
	// Se desactivan opciones actuales		
	ActiveVisibleOptions(false);

	// Se avanza iterador visible inicial y se recoloca el final
	++m_Options.FirstViewOptionIt;
	SetLastViewOptionIt();
	
	// Se localiza iterador tope final
	SetEndTopIt();

	// Se activan las nuevas opciones a visualizar y se retorna
	ActiveVisibleOptions(true);	
	return true;
  }
    
  // No se pudo avanzar
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Retrocede una posicion en el conjunto de opciones visibles.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWBaseTextSelector::PrevOption(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿ES posible avanzar a la sig. opcion?
  if (Inherited::IsActive() &&
	  m_Options.FirstViewOptionIt != m_Options.Options.begin()) {
	// Se desactivan opciones actuales	
	ActiveVisibleOptions(false);

	// Se retrocede iterador visual inicial y se recoloca final
	--m_Options.FirstViewOptionIt;
	SetLastViewOptionIt();	
	
	// Se localiza iterador tope final
	SetEndTopIt();

	// Se activan las nuevas opciones a visualizar y se retorna
	ActiveVisibleOptions(true);
	return true;
  }
    
  // No se pudo retroceder
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa los iteradores para el trabajo con la lista de opciones.
// - Habra un iterador a la primera opcion visible, otro a la ultima y otro
//   al ultimo nodo de la lista.
// Parametros:
// Devuelve:
// Notas:
// - El metodo se llamara desde Active.
// - El significado de los iteradores:
//   * FirstViewOptionIt. Iterador a la primera opcion visible.
//   * LastViewOptionIt. Iterador a la ultima opcion visible.
//   * LastOptionIt. Iterador a la ultima opcion de la lista de opciones.
//   * EndTopIt. Hara las veces de tope en los bucles que quieran ir desde 
//     el iterador inicial al iterador final. Se actualizara tanto aqui como
//     en NextOption y PrevOption.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseTextSelector::InitIterators(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Situa iterador a primera opcion visible
  m_Options.FirstViewOptionIt = m_Options.Options.begin();

  // ¿NO hay ninguna opcion asociada?
  if (m_Options.Options.end() == m_Options.FirstViewOptionIt) {
	m_Options.LastViewOptionIt = m_Options.Options.end();
	m_Options.LastOptionIt = m_Options.Options.end();	
	m_Options.EndTopIt = m_Options.Options.end();
  } else {
	// Hay al menos una opcion, se asocian el resto de iteradores
	// A ultima opcion de la lista de opciones	
	m_Options.LastOptionIt = m_Options.Options.begin();
	std::advance(m_Options.LastOptionIt, m_Options.Options.size() - 1);	

	// A ultima opcion visible de la lista de opciones
	SetLastViewOptionIt();

	// Se localiza el iterador EndTopIt que hara las veces de tope en los
	// bucles que quieran ir desde el iterador inicial al iterador final
	SetEndTopIt();
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece la posicion del ultimo iterador a partir de la posicion del
//   primer iterador. Se llamara siempre que se recoloque a este.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseTextSelector::SetLastViewOptionIt(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
 
  // Se iguala a el iterador inicial
  m_Options.LastViewOptionIt = m_Options.FirstViewOptionIt;

  // ¿Hay suficientes elementos?
  if (m_Options.LastViewOptionIt != m_Options.Options.end()) {
	// Procede a colocar posicion	
	sword swNumLines = m_Options.uwNumMaxLines;
	bool bStop = false;
	do {
	  // Se restan lineas ocupadas por la opcion actual
	  swNumLines -= m_TextComponents[(*m_Options.LastViewOptionIt)].GetVisibleTextLines();

	  // ¿El numero de lineas es el correcto?
	  if (swNumLines > 0) {
		// ¿NO estamos en la posicion maxima?
		if (m_Options.LastViewOptionIt != m_Options.LastOptionIt) {
		  // Se incrementa iterador
		  ++m_Options.LastViewOptionIt;
		} else {
		  bStop = true;
		}
	  } else {
		if (swNumLines < 0) {
		  // Nos hemos pasado de holgura, hay que corregir 
		  // la posicion del iterador
		  // Nota: Nunca se llegara a este estado en la primera iteracion
		  --m_Options.LastViewOptionIt;
		}
		bStop = true;
	  }
	} while(!bStop);
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa / desactiva las opciones que sean visibles.
// - Al activar, las lineas se iran recolocando en pantalla.
// Parametros:
// - bActive. Flag para activar / desactivar
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseTextSelector::ActiveVisibleOptions(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Activa las opciones visibles si procede
  if (m_Options.FirstViewOptionIt != m_Options.Options.end()) {			
	sword swYPos = m_Options.CfgOptions.Position.swYPos;
	OptionsListIt It(m_Options.FirstViewOptionIt);	
	for (; It != m_Options.EndTopIt; ++It) {
	  // Se recolocan si procede
	  if (bActive) {		
		m_TextComponents[(*It)].SetYPos(swYPos);
		swYPos += FontDefs::CHAR_PIXEL_HEIGHT * m_TextComponents[(*It)].GetVisibleTextLines();
	  }

	  // Se activan / desactivan
	  m_TextComponents[(*It)].SetActive(bActive);
	  m_TextComponents[(*It)].SetInput(bActive);	  
	}  
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa la ventana, es seguro que no habra aun ningun tipo de opcion
//   asociada.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseTextSelector::Active(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Activa si procede
  if (!Inherited::IsActive()) {
	// Inicializa iteradores a opciones y activa las que sean visibles
	// Nota: Es seguro que no habra opciones que activar
	InitIterators();
	ActiveVisibleOptions(true);

	// Se desvinculara el cursor de interfaz ya que este solo se mostrara
	// cuando existan opciones
	SYSEngine::GetGUIManager()->SetGUICursor(GUIManagerDefs::NO_CURSOR);

	// Propaga activacion
	Inherited::Active();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva. Al desactivar se liberaran todas las opciones que estuvieran
//   asociadas.
// Parametros:
// Devuelve:
// Notas:
// - Sera necesario simular la pulsacion de una opcion no valida en el
//   proceso de desactivacion si se estaba en estado de seleccion de
//   opcion, porque la ventana se puede cerrar a causa de la eliminacion
//   de la entidad con la que el jugador este dialogando, produciendose, de
//   no seguir este paso, efectos laterales.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseTextSelector::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Desactiva si procede
  if (Inherited::IsActive()) {
	// Si se estaba en modo de obtencion de opcion, se simulara la
	// pulsacion de una opcion NO valida	
	if (m_bInGetOptionMode) {
	  OnTextOptionLeftClick(CGUIWBaseTextSelector::MAX_IDOPTION_VALUE);
	}

	// Desactiva opciones y luego las libera
	ActiveVisibleOptions(false);
	
	// Se liberan nodos con posibles identificadores de opciones asociadas
	ReleaseOptionList();
	
	// Se liberan opciones registradas en el map
	ReleaseOptions();

	// Propaga llamada
	Inherited::Deactive();
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Notificacion sobre un componente de texto.
// Parametros:
// - GUICEvent.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseTextSelector::ComponentNotify(const GUIDefs::sGUICEvent& GUICEvent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Comprueba el tipo de evento y lo distribuye
  switch(GUICEvent.Event) {
	case GUIDefs::GUIC_SELECT: {	
	  // Seleccion
	  OnSelect(GUICEvent.IDComponent, true);	
	} break;

  	case GUIDefs::GUIC_UNSELECT: {	
	  // Deseleccion
	  OnSelect(GUICEvent.IDComponent, false);	  
	} break;
	
	case GUIDefs::GUIC_BUTTONLEFT_PRESSED: {	  	  
	  // Pulsacion, notifica
	  OnTextOptionLeftClick(GUICEvent.IDComponent);	  
	} break;	  
  }; // ~ switch
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Bloquea / desbloquea la ventana de interfaz de cara al uso de entrada.
// Parametros:
// - bActive. Si vale true, recibira entrada si vale false dejara de recibir
//   actualizaciones de entrada.
// Modifica:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseTextSelector::SetInput(const bool bActive)
{
  // Solo si instancia inicializada
  ASSERT(IsInitOk());
  
  // Procede a recorrer opciones VISIBLES
  if (m_Options.FirstViewOptionIt != m_Options.Options.end()) {			
	OptionsListIt It(m_Options.FirstViewOptionIt);	
	for (; It != m_Options.EndTopIt; ++It) {
	  m_TextComponents[(*It)].SetInput(bActive);	  
	}  
  }  

  // Propaga
  Inherited::SetInput(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la seleccion / deseleccion de la opcion sobre la que se ha
//   recibido uno de esos dos eventos.
// Parametros:
// - IDComponent. Componente.
// - bSelect. Flag de seleccion / deseleccion
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseTextSelector::OnSelect(const GUIDefs::GUIIDComponent& IDComponent,
								const bool bSelect)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());  
  
  // Se localiza opcion seleccionada que seguro esta entre las posiciones
  // primera y ultima, inclusives, de las opciones visibles.
  OptionsListIt It(m_Options.FirstViewOptionIt);	
  for (; It != m_Options.EndTopIt; ++It) {
	// ¿Es la opcion buscada?
	if (IDComponent == m_TextComponents[(*It)].GetID()) {
	  // Si, se selecciona / deselecciona y retorna	  
	  m_TextComponents[(*It)].Select(bSelect);
	  if (bSelect) {
		m_TextComponents[(*It)].ActiveAlphaFade(GraphDefs::sAlphaValue(100));
	  } else {
		m_TextComponents[(*It)].DeactiveAlphaFade();
	  }	  
	}  
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado de las opciones que sean visibles
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseTextSelector::DrawTextOptions(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Dibuja si procede
  if (m_Options.FirstViewOptionIt != m_Options.Options.end()) {		
	// Se dibuja FX de fondo
	m_Options.FXBack.Draw();
	// Se dibujan opciones
	OptionsListIt It(m_Options.FirstViewOptionIt);	
	for (; It != m_Options.EndTopIt; ++It) {
	  m_TextComponents[(*It)].Draw();	 
	}  
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Metodo virtual que sera ejecutado cuando una opcion sea seleccionada. 
//   Desde las clases base se debera de propagar aqui para que se hagan dos
//   acciones fundamentales:
//    * Notificar al cliente sobre la pulsacion
//    * Quitar el modo pregunta asociado
// Parametros:
// - IDComponent. Identificador del componente
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseTextSelector::OnTextOptionLeftClick(const GUIDefs::GUIIDComponent& IDComponent)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(IDComponent);

  // Se notifica y se quita el estado de pregunta
  Inherited::GUIWindowNotify(IDComponent);  
  m_bInGetOptionMode = false;

  // Se borra la lista de opciones seleccionables (componentes)
  ReleaseOptionList();
  
  // Se oculta el cursor de interfaz
  SYSEngine::GetGUIManager()->SetGUICursor(GUIManagerDefs::NO_CURSOR);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera la lista con los identificadores de las opciones establecidas
//   y finaliza los componentes que las mantengan.
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWBaseTextSelector::ReleaseOptionList(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Procede a eliminar nodos
  OptionsListIt It(m_Options.Options.begin());
  while (It != m_Options.Options.end()) {
	// Finaliza componente y libera iterador
	m_TextComponents[(*It)].End();
    It = m_Options.Options.erase(It);	
  }    	

  // Se reubican los iteradores
  InitIterators();
  ActiveVisibleOptions(true);  
}
