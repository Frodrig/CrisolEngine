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
// CGUIWCombat.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUIWCombat.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUIWCombat.h"

#include "SYSEngine.h"
#include "iCLogger.h"
#include "iCWorld.h"
#include "iCGameDataBase.h"
#include "CCriature.h"
#include "CPlayer.h"
#include "CCBTEngineParser.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la ventana de interfaz.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWCombat::Init(void)
{
  // ¿Se quiere reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("CGUIWCombat::Init> Inicializando interfaz de combate.\n");
  #endif

  // Se proceden a inicializar instancias
  if (Inherited::Init()) { 		
	// Procede a cargar datos de configuracion de componentes
	if (TLoad()) {
	  // Todo correcto
	  #ifdef ENGINE_TRACE
		SYSEngine::GetLogger()->Write("                 | Ok.\n");
	  #endif
	  return true;
	}	
  }  

  // Hubo problemas
  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("                 | Error.\n");
  #endif
  End();
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
CGUIWCombat::End(void)
{
  // Finaliza si procede
  if (Inherited::IsInitOk()) {
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CGUIWCombat::End> Finalizando instancia.\n");
	#endif

	// Desactiva y finaliza componentes
	Deactive();
	EndComponents();

	// Propaga
	Inherited::End();

	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("                | Ok.\n");
	#endif
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coordina la carga y configuracion de los componentes.
// Parametros:
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWCombat::TLoad(void)
{
  // Obtiene el parser a utilizar
  iCGameDataBase* const pGDBase = SYSEngine::GetGameDataBase();
  ASSERT(pGDBase);
  CCBTEngineParser* const pParser = pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_INTERFACES_CFG,
                                                          "[CombatientInfo]");
  ASSERT(pParser);

  // Procede a cargar y configurar componentes
  if (!TLoadCriatureInfo(pParser)) {
	return false;
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se carga la info relativa a la criatura asociada al interfaz.
// Parametros:
// - pParser. Parser a utilizar
// Devuelve:
// - Si todo bien true. En caso contrario false.
// Notas:
//////////////////////////////////////////////////////////////////////////////
bool 
CGUIWCombat::TLoadCriatureInfo(CCBTEngineParser* const pParser)
{
  // SOLO si parametros validos
  ASSERT(pParser);
  
  // Porcentaje
  pParser->SetVarPrefix("ActionPoints.");  
  CGUICPercentageBar::sGUICPercentageBarInitData CfgPercentage; // Cfg porcentaje  

  // Se carga info    
  CfgPercentage.uwLenght = pParser->ReadNumeric("PercentageBar.Lenght");
  CfgPercentage.uwWidth = pParser->ReadNumeric("PercentageBar.Width");
  CfgPercentage.Alpha = pParser->ReadAlpha("PercentageBar.");
  // Se completa informacion y se inicializa
  CfgPercentage.bDrawHoriz = true;
  CfgPercentage.Position = pParser->ReadPosition("PercentageBar.");
  CfgPercentage.ID = CGUIWCombat::ID_ACTIONPOINTSPECENTAGE;
  CfgPercentage.ubDrawPlane = 1;
  if (!m_CriatureInfo.ActionPoints.Percentage.Init(CfgPercentage)) {
	return false;
  }

  // Vbles
  CGUICLineText::sGUICLineTextInitData TextCfg; // Cfg del texto

  // Se establecen datos comunes en cfg del texto
  TextCfg.szFont = "Arial";
  TextCfg.szLine = "-";
  TextCfg.ubDrawPlane = 1;

  // Se carga info relativa al nombre
  TextCfg.RGBUnselectColor = pParser->ReadRGBColor("Value.");
  // Se completa info  
  TextCfg.RGBSelectColor = TextCfg.RGBUnselectColor;  
  TextCfg.ID = CGUIWCombat::ID_TEXTINFO;
  TextCfg.Position = sPosition(CfgPercentage.Position.swXPos + 2, 
							   CfgPercentage.Position.swYPos + 1);
  if (!m_CriatureInfo.TextInfo.Init(TextCfg)) {
	return false;
  }

  // Por ultimo, se inicializa el FXBack sobre el que ira la barra de porcentaje
  const sPosition FXDrawPos(CfgPercentage.Position.swXPos - 3, 
							CfgPercentage.Position.swYPos - 3);
  if (!m_CriatureInfo.FXBack.Init(FXDrawPos, 
								  1, 
								  CfgPercentage.uwLenght + 6,
								  CfgPercentage.uwWidth + 6,
								  GraphDefs::sRGBColor(0, 0, 0),
								  GraphDefs::sAlphaValue(50))) {
	return false;
  }
  
  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza los componentes
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCombat::EndComponents(void)
{
  // Info de la criatura
  m_CriatureInfo.TextInfo.End();
  m_CriatureInfo.ActionPoints.Percentage.End();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa la interfaz. Estara preparada para activarse unicamente cuando
//   se reciba un handle de entidad valido. Dicho handle a criatura podra ser
//   a un npc o bien al jugador. Cuando sea a un npc los botones de opciones
//   del jugador NO se activaran.
// Parametros:
// - hCriature. Handle a criatura.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCombat::Active(const AreaDefs::EntHandle& hCriature)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(hCriature);

  // Se inicializara solo si procede
  if (!Inherited::IsActive()) {
	// Se activan componentes base
	ActiveComponents(true);
	
	// Se establece criatura con turno
	SetCriature(hCriature);

	// Se propaga
	Inherited::Active();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva interfaz
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCombat::Deactive(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Desactiva si procede
  if (Inherited::IsActive()) {
	// Desactiva componentes
	ActiveComponents(false);

	// Se desasocia instancia a criatura
	SetCriature(0);

	// Propaga
	Inherited::Deactive();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa / desactiva los componentes segun el flag bActive.
// Parametros:
// - bActive. Flag de activacion / desactivacion de componentes.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCombat::ActiveComponents(const bool bActive)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Procede a activar / desactivar
  m_CriatureInfo.TextInfo.SetActive(bActive);
  m_CriatureInfo.ActionPoints.Percentage.SetActive(bActive);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Asocia / desasocia la criatura al interfaz y refleja su informacion en 
//   los componentes de interfaz.
// Parametros:
// - hCriature. Handle a la criatura.
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCombat::SetCriature(const AreaDefs::EntHandle& hCriature)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Se desea asociar criatura?
  if (hCriature) {
	// Se obtiene intancia a la criatura asociada al handle
	iCWorld* const pWorld = SYSEngine::GetWorld();
	ASSERT(pWorld);
	m_CriatureInfo.pCriature = pWorld->GetCriature(hCriature);
	ASSERT(m_CriatureInfo.pCriature);

	// Se reflejan los puntos de accion de la criatura		
	//SetActionPoints(m_CriatureInfo.pCriature->GetActionPoints());
	SetActionPoints(SYSEngine::GetCombatSystem()->GetCriatureActionPoints(hCriature));
  } else {
	m_CriatureInfo.pCriature = NULL;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Bloquea / desbloquea la entrada segun el valor del flag bInput
// Parametros:
// - bInput. Flag de bloqueo / desbloqueo
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCombat::SetInput(const bool bInput)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Info de la criatura
  m_CriatureInfo.TextInfo.SetInput(bInput);
  m_CriatureInfo.ActionPoints.Percentage.SetInput(bInput);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece los NUEVOS puntos de accion de la criatura asociada a interfaz.
//   Este metodo se llamara desde CCombatSystem y servira para ir reflejando
//   las acciones que realice la criatura y los gastos en puntos de accion de
//   la misma en combate.
// Parametros:
// - ActionPoints. Puntos de accion actuales (temporales) de la criatura.
// Devuelve:
// Notas:
// - Una alternativa a este sistema para reflejar los puntos de accion de
//   la criatura, seria instalar el interfaz como observer de la misma y
//   obtener asi la notificacion de las acciones realizadas. El problema es
//   que esto se va a realizar en CCombatSystem por lo que seria repetir
//   codigo para la misma finalidad.
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCombat::SetActionPoints(const RulesDefs::CriatureActionPoints& ActionPoints)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // Se establecen los puntos de la criatura tanto en texto como en porcentaje
  const RulesDefs::CriatureActionPoints BasePoints = m_CriatureInfo.pCriature->GetActionPoints();
  std::string szTextInfo;
  SYSEngine::PassToString(szTextInfo, 
						  "%s: %u / %u", 
						  m_CriatureInfo.pCriature->GetName().c_str(),
						  ActionPoints, 
						  BasePoints);
  m_CriatureInfo.TextInfo.ChangeText(szTextInfo);  
  m_CriatureInfo.ActionPoints.Percentage.ChangeBaseAndTempValue(BasePoints, ActionPoints);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado de la interfaz
// Parametros:
// Devuelve:
// Notas:
//////////////////////////////////////////////////////////////////////////////
void 
CGUIWCombat::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Dibuja solo si esta activado
  if (Inherited::IsActive()) {
	// Datos de la criatura
	m_CriatureInfo.FXBack.Draw();
	m_CriatureInfo.ActionPoints.Percentage.Draw();
	m_CriatureInfo.TextInfo.Draw();
  }
}
