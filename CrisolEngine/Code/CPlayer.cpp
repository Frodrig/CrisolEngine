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
// CPlayer.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CPlayer.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CPlayer.h"

#include "SYSEngine.h"
#include "iCGameDataBase.h"
#include "iCVirtualMachine.h"
#include "CCBTEngineParser.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia, a partir de los parametros con los que el jugador
//   fue creado en el archivo de reglas. 
//   Los scripts asociados se obtendrán desde este metodo utilizando el
//   parser al archivo de datos.
// Parametros:
// - szATSprite. Plantilla de animacion al sprite.
// - szName. Nombre del jugador.
// - szATPortrait. Plantilla de animacion con el retrato.
// - szShadowImage. Nombre de la imagen con la sombra.
// - Attribs. Atributos asociados a las criaturas.
// Devuelve:
// - Si todo correcto true. En caso contrario false.
// Notas:
// - No permite reinicializar
///////////////////////////////////////////////////////////////////////////////
bool 
CPlayer::Init(const std::string& szATSprite,
			  const std::string& szName,
			  const std::string& szATPortrait,	
			  const std::string& szShadowImage,
			  const RulesDefs::sCriatureAttrib& Attribs)
{
  // SOLO si parametros validos
  ASSERT(szATSprite.size());
  ASSERT(szName.size());
  ASSERT(szATPortrait.size());

  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) {
	return false;
  }

  // Se crea el handle del jugador
  const word hPlayer = (RulesDefs::PLAYER << 12) | 0X0001;   
    
  // Se procede a inicializar clase base
  if (Inherited::Init(szATSprite, 
					  hPlayer, 
					  szName, 
					  szATPortrait, 
					  szShadowImage, 
					  Attribs)) {
	// Orientacion mirando al jugador siempre
	Inherited::GetAnimTemplate()->SetOrientation(IsoDefs::SOUTH_INDEX);

	// Se leen los scripts asociados
	iCGameDataBase* const pGDBase = SYSEngine::GetGameDataBase();
	ASSERT(pGDBase);
	CCBTEngineParser* const pParser = pGDBase->GetCBTParser(GameDataBaseDefs::CBTF_PLAYER_PROFILES,
														    "[Common]");
	ASSERT(pParser);
	pParser->SetVarPrefix("");
	Inherited::SetScriptEvent(RulesDefs::SE_ONOBSERVE, 
							  pParser->ReadStringID("ScriptEvent[OnObserve]", false));
	Inherited::SetScriptEvent(RulesDefs::SE_ONTALK, 
						      pParser->ReadStringID("ScriptEvent[OnTalk]", false));
	Inherited::SetScriptEvent(RulesDefs::SE_ONMANIPULATE, 
							  pParser->ReadStringID("ScriptEvent[OnManipulate]", false));
	Inherited::SetScriptEvent(RulesDefs::SE_ONDEATH, 
							  pParser->ReadStringID("ScriptEvent[OnDeath]", false));
	Inherited::SetScriptEvent(RulesDefs::SE_ONRESURRECT, 
							  pParser->ReadStringID("ScriptEvent[OnResurrect]", false));
	Inherited::SetScriptEvent(RulesDefs::SE_ONINSERTINEQUIPMENTSLOT, 
							  pParser->ReadStringID("ScriptEvent[OnInsertInEquipmentSlot]", false));
	Inherited::SetScriptEvent(RulesDefs::SE_ONREMOVEFROMEQUIPMENTSLOT, 
							  pParser->ReadStringID("ScriptEvent[OnRemoveFromEquipmentSlot]", false));
	Inherited::SetScriptEvent(RulesDefs::SE_ONUSEHABILITY, 
							  pParser->ReadStringID("ScriptEvent[OnUseHability]", false));
	Inherited::SetScriptEvent(RulesDefs::SE_ONACTIVATEDSYMPTOM, 
							  pParser->ReadStringID("ScriptEvent[OnActivatedSymptom]", false));
	Inherited::SetScriptEvent(RulesDefs::SE_ONDEACTIVATEDSYMPTOM, 
							  pParser->ReadStringID("ScriptEvent[OnDeactivatedSymptom]", false));
	Inherited::SetScriptEvent(RulesDefs::SE_ONHIT, 
							  pParser->ReadStringID("ScriptEvent[OnHit]", false));
	Inherited::SetScriptEvent(RulesDefs::SE_ONSTARTCOMBATTURN, 
							  pParser->ReadStringID("ScriptEvent[OnStartCombatTurn]", false));
	Inherited::SetScriptEvent(RulesDefs::SE_ONCRIATUREINRANGE, 
							  pParser->ReadStringID("ScriptEvent[OnCriatureInRange]", false));
	Inherited::SetScriptEvent(RulesDefs::SE_ONCRIATUREOUTOFRANGE, 
							  pParser->ReadStringID("ScriptEvent[OnCriatureOutOfRange]", false));
	Inherited::SetScriptEvent(RulesDefs::SE_ONENTITYIDLE, 
							  pParser->ReadStringID("ScriptEvent[OnEntityIdle]", false));
  }  

  // Se retorna resultado
  return Inherited::IsInitOk();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa instancia leyendo los datos desde archivo. Este archivo sera
//   con toda certeza el de partidas guardadas.
// Parametros:
// - hFile. Handle al fichero.
// - udOffset. Offset desde donde comenzar a leer.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - Se apoyara en el metodo de la clase base, pero pasando el handle.
///////////////////////////////////////////////////////////////////////////////
bool 
CPlayer::Init(const FileDefs::FileHandle& hFile,
		      dword& udOffset)
{
  // SOLO si parametros correctos
  ASSERT(hFile);

  // Se crea el handle del jugador
  const word hPlayer = (RulesDefs::PLAYER << 12) | 0X0001;   

  // Se propaga
  Inherited::Init(hFile, udOffset, hPlayer);

  // Se retorna resultado
  return Inherited::IsInitOk();  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza instancia.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void CPlayer::End(void)
{
  // Finaliza
  if (Inherited::IsInitOk()) {	
	// Propaga
	Inherited::End();
  }
}
