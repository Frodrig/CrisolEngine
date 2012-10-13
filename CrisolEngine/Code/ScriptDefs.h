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
// ScriptDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Define todos los tipos, constantes, enumerados, etc, relacionados con los
//   eventos script.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _SCRIPTDEFS_H_
#define _SCRIPTDEFS_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif
#ifndef _AREADEFS_H_
#include "AreaDefs.h"
#endif
#ifndef _LIST_H_
#define _LIST_H_
#include <list>
#endif

// Espacio ScriptDefs
namespace ScriptDefs
{
  // Enumerados
  enum eScriptInstruction {
	// Tipos de instrucciones	  
	// Nota: Los prefijos N, S y E serviran para indicar si opera 
	// con (n)umbers, (s)trings o (e)ntities respectivamente.
	// 0..999
	// Nota: SI = Script Instruction
	SI_NOP = 0,   // No realiza operacion alguna
	SI_NNEG,      // Niega un number
	SI_NMUL,      // Multiplica dos number
	SI_NADD,      // Suma dos number
	SI_NMOD,      // Halla el modulo (%) de dos number
	SI_NDIV,      // Divide dos number
	SI_NSUB,      // Resta dos number
	SI_SADD,      // Concatena dos strings
	SI_JMP,       // Salto incondicional
	SI_JMP_FALSE, // Salta si tope de la pila < 1
	SI_JMP_TRUE,  // Salta si tope de la pila >= 0
	SI_NJMP_EQ,   // Salta si los dos number en pila son ==
	SI_NJMP_NE,   // Salta si los dos number en pila son !=
	SI_NJMP_GE,   // Salta si un number en pila es >= que el otro
	SI_NJMP_GT,   // Salta si un number en pila es > que el otro
	SI_NJMP_LT,   // Salta si un number en pila es < que el otro
	SI_NJMP_LE,   // Salta si un number en pila es <= que el otro
	SI_SJMP_EQ,   // Salta si un string en pila es == que el otro
	SI_SJMP_NE,   // Salta si un string en pila es != que el otro
	SI_EJMP_EQ,   // Salta si un entity en pila es == que el otro
	SI_EJMP_NE,   // Salta si un entity en pila es == que el otro
	SI_DUP,       // Duplica elemento en el tope de la pila
	SI_POP,       // Descarta elemento en tope de la pila
	SI_NRETURN,   // Retorno de una funcion / script pasando un valor o sin
	SI_SRETURN,   // valor alguno
	SI_ERETURN,   //
	SI_RETURN,    //
	SI_NLOAD,     // Leera el valor asociado a la direccion que proceda y lo
	SI_SLOAD,     // colocara en el tope de la pila
	SI_ELOAD,     //
	SI_NSTORE,    // Guardara el valor que haya en el tope de la pila en la
	SI_SSTORE,    // direccion suministrada
	SI_ESTORE,    //
	SI_NPUSH,     // Se coloca un valor en el tope de la pila
	SI_SPUSH,     //
	SI_EPUSH,     // 
	SI_NSCAST,    // Cast de number a string
	SI_SNCAST,    // Cast de string a number
	SI_CALL_FUNC, // Llamada a funcion
	
	// Opcodes asociados a las funciones de API
	// 1000..1999
    SI_API_PASSTORGBCOLOR = 1000,
	SI_API_GETREDCOMPONENT,
	SI_API_GETGREENCOMPONENT,
	SI_API_GETBLUECOMPONENT,
	SI_API_RAND,
	SI_API_GETINTEGERVALUE,
	SI_API_GETDECIMALVALUE,
	SI_API_GETSTRINGSIZE,
	SI_API_WRITETOLOGGER,
	SI_API_ENABLECRISOLSCRIPTWARNINGS,
	SI_API_DISABLECRISOLSCRIPTWARNINGS,
	SI_API_SHOWFPS,
	SI_API_WAIT,

	// Opcodes sociados a los metodos del objeto game
	// 2000..2999
	SI_GAMEOBJ_QUITGAME = 2000,
	SI_GAMEOBJ_WRITETOCONSOLE,
	SI_GAMEOBJ_ACTIVEADVICEDIALOG,
	SI_GAMEOBJ_ACTIVEQUESTIONDIALOG,
	SI_GAMEOBJ_ACTIVETEXTREADERDIALOG,
	SI_GAMEOBJ_ADDOPTIONTOTEXTSELECTORDIALOG,
	SI_GAMEOBJ_RESETOPTIONSINTEXTSELECTORDIALOG,
	SI_GAMEOBJ_ACTIVETEXTSELECTORDIALOG,
	SI_GAMEOBJ_PLAYMIDIMUSIC,
	SI_GAMEOBJ_STOPMIDIMUSIC,
	SI_GAMEOBJ_PLAYWAVAMBIENTSOUND,
	SI_GAMEOBJ_STOPWAVAMBIENTSOUND,
	SI_GAMEOBJ_ACTIVETRADEITEMSINTERFAZ,
	SI_GAMEOBJ_ADDOPTIONTOCONVERSATORINTERFAZ,
	SI_GAMEOBJ_RESETOPTIONSINCONVERSATORINTERFAZ,
	SI_GAMEOBJ_ACTIVECONVERSATORINTERFAZ,
	SI_GAMEOBJ_DEACTIVECONVERSATORINTERFAZ,
	SI_GAMEOBJ_GETOPTIONFROMCONVERSATORINTERFAZ,
	SI_GAMEOBJ_SHOWPRESENTATION,
	SI_GAMEOBJ_BEGINCUTSCENE,
	SI_GAMEOBJ_ENDCUTSCENE,
	SI_GAMEOBJ_SETSCRIPT,
	SI_GAMEOBJ_ISKEYPRESSED,

	// Opcodes asociados a los metodos del objeto world
	// 3000..3999
	SI_WORLDOBJ_GETAREANAME = 3000,
	SI_WORLDOBJ_GETAREAID,
	SI_WORLDOBJ_GETAREAWIDTH,
	SI_WORLDOBJ_GETAREAHEIGHT,
	SI_WORLDOBJ_GETHOUR,
	SI_WORLDOBJ_GETMINUTE,
	SI_WORLDOBJ_SETHOUR,
	SI_WORLDOBJ_SETMINUTE,
	SI_WORLDOBJ_GETENTITY,
	SI_WORLDOBJ_GETPLAYER,
	SI_WORLDOBJ_ISFLOORVALID,
	SI_WORLDOBJ_GETITEMAT,
	SI_WORLDOBJ_GETNUMITEMSAT,		
	SI_WORLDOBJ_GETDISTANCE,
	SI_WORLDOBJ_CALCULEPATHLENGHT,
	SI_WORLDOBJ_LOADAREA,
	SI_WORLDOBJ_CHANGEENTITYLOCATION,
	SI_WORLDOBJ_ATTACHCAMERATOENTITY,
	SI_WORLDOBJ_ATTACHCAMERATOLOCATION,
	SI_WORLDOBJ_ISCOMBATMODEACTIVE,
	SI_WORLDOBJ_ENDCOMBAT,
	SI_WORLDOBJ_GETCRIATUREINCOMBATTURN,
	SI_WORLDOBJ_GETCOMBATANT,
	SI_WORLDOBJ_GETNUMBEROFCOMBATANTS,
	SI_WORLDOBJ_GETAREALIGHTMODEL,
	SI_WORLDOBJ_SETIDLESCRIPTTIME,
	SI_WORLDOBJ_SETSCRIPT,
	SI_WORLDOBJ_DESTROYENTITY,
	SI_WORLDOBJ_CREATECRIATURE,
	SI_WORLDOBJ_CREATEWALL,
	SI_WORLDOBJ_CREATESCENARYOBJECT,
	SI_WORLDOBJ_CREATEITEMABANDONED,
	SI_WORLDOBJ_CREATEITEMWITHOWNER,
	SI_WORLDOBJ_SETWORLDTIMEPAUSE,
	SI_WORLDOBJ_ISWORLDTIMEINPAUSE,
	SI_WORLDOBJ_SETELEVATIONAT,	
	SI_WORLDOBJ_GETELEVATIONAT,	
	SI_WORLDOBJ_NEXTTURN,
	SI_WORLDOBJ_GETLIGHTAT,
	SI_WORLDOBJ_PLAYWAVSOUND,
	SI_WORLDOBJ_SETSCRIPTAT,

	// Opcodes asociados a los metodos entity
	// 4000..4999
	SI_ENTITYOBJ_GETNAME = 4000,
	SI_ENTITYOBJ_SETNAME,
	SI_ENTITYOBJ_GETENTITYTYPE,
	SI_ENTITYOBJ_GETTYPE,	
	SI_ENTITYOBJ_SAY,
	SI_ENTITYOBJ_SHUTUP,
	SI_ENTITYOBJ_ISSAYING,
	SI_ENTITYOBJ_ATTACHGFX,
	SI_ENTITYOBJ_RELEASEGFX,
	SI_ENTITYOBJ_RELEASEALLGFX,
	SI_ENTITYOBJ_ISGFXATTACHED,
	SI_ENTITYOBJ_GETNUMITEMSINCONTAINER,
	SI_ENTITYOBJ_GETITEMFROMCONTAINER,
	SI_ENTITYOBJ_ISITEMINCONTAINER,
	SI_ENTITYOBJ_TRANSFERITEMTOCONTAINER,
	SI_ENTITYOBJ_INSERTITEMINCONTAINER,
	SI_ENTITYOBJ_REMOVEITEMOFCONTAINER,
	SI_ENTITYOBJ_SETANIMTEMPLATESTATE,
	SI_ENTITYOBJ_SETPORTRAITANIMTEMPLATESTATE,
	SI_ENTITYOBJ_SETIDLESCRIPTTIME,
	SI_ENTITYOBJ_SETLIGHT,
	SI_ENTITYOBJ_GETLIGHT,
	SI_ENTITYOBJ_GETXPOS,
	SI_ENTITYOBJ_GETYPOS,
	SI_ENTITYOBJ_GETELEVATION,
	SI_ENTITYOBJ_SETELEVATION,
	SI_ENTITYOBJ_GETLOCALATTRIBUTE,
	SI_ENTITYOBJ_SETLOCALATTRIBUTE,
	SI_ENTITYOBJ_GETOWNER,
	SI_ENTITYOBJ_GETCLASS,
	SI_ENTITYOBJ_GETINCOMBATUSECOST,
	SI_ENTITYOBJ_GETGLOBALATTRIBUTE,
	SI_ENTITYOBJ_SETGLOBALATTRIBUTE,
	SI_ENTITYOBJ_GETWALLORIENTATION,
	SI_ENTITYOBJ_BLOCKACCESS,
	SI_ENTITYOBJ_UNBLOCKACCESS,
	SI_ENTITYOBJ_ISACCESSBLOCKED,
	SI_ENTITYOBJ_SETSYMPTOM,
	SI_ENTITYOBJ_ISSYMPTOMACTIVE,
	SI_ENTITYOBJ_GETGENRE,
	SI_ENTITYOBJ_GETHEALTH,
	SI_ENTITYOBJ_SETHEALTH,
	SI_ENTITYOBJ_GETEXTENDEDATTRIBUTE,
	SI_ENTITYOBJ_SETEXTENDEDATTRIBUTE,
	SI_ENTITYOBJ_GETLEVEL,
	SI_ENTITYOBJ_SETLEVEL,
	SI_ENTITYOBJ_GETEXPERIENCE,
	SI_ENTITYOBJ_SETEXPERIENCE,
	SI_ENTITYOBJ_GETINCOMBATACTIONPOINTS,
	SI_ENTITYOBJ_GETACTIONPOINTS,
	SI_ENTITYOBJ_SETACTIONPOINTS,
	SI_ENTITYOBJ_ISHABILITYACTIVE,
	SI_ENTITYOBJ_SETHABILITY,
	SI_ENTITYOBJ_USEHABILITY,
	SI_ENTITYOBJ_ISRUNMODEACTIVE,
	SI_ENTITYOBJ_SETRUNMODE,
	SI_ENTITYOBJ_MOVETO,
	SI_ENTITYOBJ_ISMOVING,
	SI_ENTITYOBJ_STOPMOVING,
	SI_ENTITYOBJ_EQUIPITEM,
	SI_ENTITYOBJ_REMOVEITEMEQUIPPED,
	SI_ENTITYOBJ_GETITEMEQUIPPED,
	SI_ENTITYOBJ_ISITEMEQUIPPED,
	SI_ENTITYOBJ_DROPITEM,
	SI_ENTITYOBJ_USEITEM,
	SI_ENTITYOBJ_MANIPULATE,
	SI_ENTITYOBJ_SETTRANSPARENTMODE,
	SI_ENTITYOBJ_ISTRANSPARENTMODEACTIVE,
	SI_ENTITYOBJ_CHANGEANIMORIENTATION,
	SI_ENTITYOBJ_GETANIMORIENTATION,
	SI_ENTITYOBJ_SETALINGMENT,
	SI_ENTITYOBJ_SETALINGMENTWITH,
	SI_ENTITYOBJ_SETALINGMENTAGAINST,
	SI_ENTITYOBJ_GETALINGMENT,
	SI_ENTITYOBJ_HITENTITY,
	SI_ENTITYOBJ_SETSCRIPT,
	SI_ENTITYOBJ_ISGHOSTMOVEMODEACTIVE,
	SI_ENTITYOBJ_SETGHOSTMOVEMODE,
	SI_ENTITYOBJ_GETRANGE,
	SI_ENTITYOBJ_ISINRANGE
  };

  enum eScriptState {
	// Estados en los que puede hallarse un script
	// Nota: SS = ScriptState
	SS_PAUSED,   // En modo pausa
	SS_RESUME,   // Esperando salir de la pausa
	SS_RUNNING,  // Ejecutandose
	SS_INACTIVE, // Inactivo al no haberse iniciado o haber finalizado
	SS_STOPPED,  // Detenido, solo para el script global
	SS_ERROR,    // Hubo un error
  };

  struct sScriptParam {
	// Define un parametro pasado a un script para su ejecucion	
	// Enumerados
	enum eParamType {
	  // Tipo de parametro
	  // Nota: PT = ParamType
	  PT_NUMBER, // De tipo Number
	  PT_ENTITY  // De tipo Entity
	};

	// Datos
	eParamType Type; // Tipo de parametro
	union {
	  float               fNumber; // Valor numerico
	  AreaDefs::EntHandle hEntity; // Entidad
	};	

	// Constructores
	sScriptParam(const float fValue): fNumber(fValue),
									   Type(PT_NUMBER) { }
	sScriptParam(const AreaDefs::EntHandle& hValue): hEntity(hValue),
													 Type(PT_ENTITY) { }

	// Operadores
	sScriptParam& operator=(const sScriptParam& ScriptParam) {
	  Type = ScriptParam.Type;
	  switch(Type) {
		case PT_NUMBER: {
		  fNumber = ScriptParam.fNumber;
		} break;

		case PT_ENTITY: {
		  hEntity = ScriptParam.hEntity;
		} break;
	  }; // ~ switch
	  return *this;
	}
  };

  // Tipos
  // Lista para guardar los parametros a pasar a un script
  typedef std::list<sScriptParam>   ScriptParamList;
  typedef ScriptParamList::iterator ScriptParamListIt;  
};

#endif // ~ #ifdef _SCRIPTDEFS_H_
