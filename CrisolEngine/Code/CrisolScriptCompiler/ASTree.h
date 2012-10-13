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
// ASTree.h
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Modulo encargado de la construccion del AST o arbol sintatico abstracto 
//   (Abstract Syntax Tree). Tambien definira las estructuras e interfaces
//   para el trabajo con la tabla de simbolos.
// - En este modulo se definiran tambien las estructuras o nodos que formaran
//   los nodos del arbol.
// - Se usara una tabla hash para el control de la tabla de simbolos.
//
// Notas:
// - En todos los nodos se guardara un campo asociado al numero de linea
//   en donde estan asociados. 
// - Un argumento estara formado por un tipo y un identificador. 
// - Un parametro sera un valor que se pasara a uno de los argumentos de una
//   funcion.
// - Se usara un modelo de tabla hash abierta.
// - Para representar ambitos, se usara una jerarquia de tablas de simbolos.
////////////////////////////////////////////////////////////////////////////////

#ifndef _ASTREE_H_
#define _ASTREE_H_

#include "TypesDefs.h"

// Defines
#define HASH_SIZE 317

// Estructuras relacionadas con la generacion del codigo intermedio
typedef struct sOpcode {
  // Representa a un Opcode
  enum {
	// Tipos de opcode
	// Nota: Los prefijos N, S y E en un Opcode serviran para indicar si dicho
	// opcode opera con (n)umbers, (s)trings o (e)ntities respectivamente.
	// Opcodes basicos
	// 0..999
	OP_NOP = 0,   // No realiza operacion alguna
	OP_NNEG,      // Niega un number
	OP_NMUL,      // Multiplica dos number
	OP_NADD,      // Suma dos number
	OP_NMOD,      // Halla el modulo (%) de dos number
	OP_NDIV,      // Divide dos number
	OP_NSUB,      // Resta dos number
	OP_SADD,      // Concatena dos strings
	OP_JMP,       // Salto incondicional
	OP_JMP_FALSE, // Salta si tope de la pila < 1
	OP_JMP_TRUE,  // Salta si tope de la pila >= 0
	OP_NJMP_EQ,   // Salta si los dos number en pila son ==
	OP_NJMP_NE,   // Salta si los dos number en pila son !=
	OP_NJMP_GE,   // Salta si un number en pila es >= que el otro
	OP_NJMP_GT,   // Salta si un number en pila es > que el otro
	OP_NJMP_LT,   // Salta si un number en pila es < que el otro
	OP_NJMP_LE,   // Salta si un number en pila es <= que el otro
	OP_SJMP_EQ,   // Salta si un string en pila es == que el otro
	OP_SJMP_NE,   // Salta si un string en pila es != que el otro
	OP_EJMP_EQ,   // Salta si un entity en pila es == que el otro
	OP_EJMP_NE,   // Salta si un entity en pila es == que el otro
	OP_DUP,       // Duplica elemento en el tope de la pila
	OP_POP,       // Descarta elemento en tope de la pila
	OP_NRETURN,   // Retorno de una funcion / script pasando un valor o sin
	OP_SRETURN,   // valor alguno
	OP_ERETURN,   //
	OP_RETURN,    //
	OP_NLOAD,     // Leera el valor asociado a la direccion que proceda y lo
	OP_SLOAD,     // colocara en el tope de la pila
	OP_ELOAD,     //
	OP_NSTORE,    // Guardara el valor que haya en el tope de la pila en la
	OP_SSTORE,    // direccion suministrada
	OP_ESTORE,    //
	OP_NPUSH,     // Se coloca un valor en el tope de la pila
	OP_SPUSH,     //
	OP_EPUSH,     // 
	OP_NSCAST,    // Cast de number a string
	OP_SNCAST,    // Cast de string a number
	OP_CALL_FUNC, // Llamada a funcion
	OP_LABEL,     // Existencia de un label
	
	// Opcodes asociados a las funciones de API
	// 1000..1999
    OP_API_PASSTORGBCOLOR = 1000,
	OP_API_GETREDCOMPONENT,
	OP_API_GETGREENCOMPONENT,
	OP_API_GETBLUECOMPONENT,
	OP_API_RAND,
	OP_API_GETINTEGERVALUE,
	OP_API_GETDECIMALVALUE,
	OP_API_GETSTRINGSIZE,
	OP_API_WRITETOLOGGER,
	OP_API_ENABLECRISOLSCRIPTWARNINGS,
	OP_API_DISABLECRISOLSCRIPTWARNINGS,
	OP_API_SHOWFPS,
	OP_API_WAIT,
	
	// Opcodes sociados a los metodos del objeto game
	// 2000..2999
	OP_GAMEOBJ_QUITGAME = 2000,
	OP_GAMEOBJ_WRITETOCONSOLE,
	OP_GAMEOBJ_ACTIVEADVICEDIALOG,
	OP_GAMEOBJ_ACTIVEQUESTIONDIALOG,
	OP_GAMEOBJ_ACTIVETEXTREADERDIALOG,
	OP_GAMEOBJ_ADDOPTIONTOTEXTSELECTORDIALOG,
	OP_GAMEOBJ_RESETOPTIONSINTEXTSELECTORDIALOG,
	OP_GAMEOBJ_ACTIVETEXTSELECTORDIALOG,
	OP_GAMEOBJ_PLAYMIDIMUSIC,
	OP_GAMEOBJ_STOPMIDIMUSIC,
	OP_GAMEOBJ_PLAYWAVAMBIENTSOUND,
	OP_GAMEOBJ_STOPWAVAMBIENTSOUND,
	OP_GAMEOBJ_ACTIVETRADEITEMSINTERFAZ,	
	OP_GAMEOBJ_ADDOPTIONTOCONVERSATORINTERFAZ,
	OP_GAMEOBJ_RESETOPTIONSINCONVERSATORINTERFAZ,
	OP_GAMEOBJ_ACTIVECONVERSATORINTERFAZ,
	OP_GAMEOBJ_DESACTIVECONVERSATORINTERFAZ,
	OP_GAMEOBJ_GETOPTIONFROMCONVERSATORINTERFAZ,	
	OP_GAMEOBJ_SHOWPRESENTATION,
	OP_GAMEOBJ_BEGINCUTSCENE,
	OP_GAMEOBJ_ENDCUTSCENE,
	OP_GAMEOBJ_SETSCRIPT,
	OP_GAMEOBJ_ISKEYPRESSED,
	
	// Opcodes asociados a los metodos del objeto world
	// 3000..3999
	OP_WORLDOBJ_GETAREANAME = 3000,
	OP_WORLDOBJ_GETAREAID,
	OP_WORLDOBJ_GETAREAWIDTH,
	OP_WORLDOBJ_GETAREAHEIGHT,
	OP_WORLDOBJ_GETHOUR,
	OP_WORLDOBJ_GETMINUTE,
	OP_WORLDOBJ_SETHOUR,
	OP_WORLDOBJ_SETMINUTE,
	OP_WORLDOBJ_GETENTITY,
	OP_WORLDOBJ_GETPLAYER,
	OP_WORLDOBJ_ISFLOORVALID,
	OP_WORLDOBJ_GETITEMAT,
	OP_WORLDOBJ_GETNUMITEMSAT,		
	OP_WORLDOBJ_GETDISTANCE,
	OP_WORLDOBJ_CALCULEPATHLENGHT,
	OP_WORLDOBJ_LOADAREA,
	OP_WORLDOBJ_CHANGEENTITYLOCATION,
	OP_WORLDOBJ_ATTACHCAMERATOENTITY,
	OP_WORLDOBJ_ATTACHCAMERATOLOCATION,
	OP_WORLDOBJ_ISCOMBATMODEACTIVE,
	OP_WORLDOBJ_ENDCOMBAT,
	OP_WORLDOBJ_GETCRIATUREINCOMBATTURN,
	OP_WORLDOBJ_GETCOMBATANT,
	OP_WORLDOBJ_GETNUMBEROFCOMBATANTS,
	OP_WORLDOBJ_GETAREALIGHTMODEL,
	OP_WORLDOBJ_SETIDLESCRIPTTIME,
	OP_WORLDOBJ_SETSCRIPT,
	OP_WORLDOBJ_DESTROYENTITY,
	OP_WORLDOBJ_CREATECRIATURE,
	OP_WORLDOBJ_CREATEWALL,
	OP_WORLDOBJ_CREATESCENARYOBJECT,
	OP_WORLDOBJ_CREATEITEMABANDONED,
	OP_WORLDOBJ_CREATEITEMWITHOWNER,
	OP_WORLDOBJ_SETWORLDTIMEPAUSE,
	OP_WORLDOBJ_ISWORLDTIMEINPAUSE,
	OP_WORLDOBJ_SETELEVATIONAT,
	OP_WORLDOBJ_GETELEVATIONAT,
	OP_WORLDOBJ_NEXTTURN,
	OP_WORLDOBJ_GETLIGHTAT,
	OP_WORLDOBJ_PLAYWAVSOUND,
	OP_WORLDOBJ_SETSCRIPTAT,
							
	// Opcodes asociados a los metodos entity
	// 4000..4999
	OP_ENTITYOBJ_GETNAME = 4000,
	OP_ENTITYOBJ_SETNAME,
	OP_ENTITYOBJ_GETENTITYTYPE,
	OP_ENTITYOBJ_GETTYPE,
	OP_ENTITYOBJ_SAY,
	OP_ENTITYOBJ_SHUTUP,
	OP_ENTITYOBJ_ISSAYING,
	OP_ENTITYOBJ_ATTACHGFX,
	OP_ENTITYOBJ_RELEASEGFX,
	OP_ENTITYOBJ_RELEASEALLGFX,
	OP_ENTITYOBJ_ISGFXATTACHED,
	OP_ENTITYOBJ_GETNUMITEMSINCONTAINER,
	OP_ENTITYOBJ_GETITEMFROMCONTAINER,
	OP_ENTITYOBJ_ISITEMINCONTAINER,
	OP_ENTITYOBJ_TRANSFERITEMTOCONTAINER,
	OP_ENTITYOBJ_INSERTITEMINCONTAINER,
	OP_ENTITYOBJ_REMOVEITEMOFCONTAINER,
	OP_ENTITYOBJ_SETANIMTEMPLATESTATE,
	OP_ENTITYOBJ_SETPORTRAITANIMTEMPLATESTATE,
	OP_ENTITYOBJ_SETIDLESCRIPTTIME,
	OP_ENTITYOBJ_SETLIGHT,
	OP_ENTITYOBJ_GETLIGHT,
	OP_ENTITYOBJ_GETXPOS,
	OP_ENTITYOBJ_GETYPOS,
	OP_ENTITYOBJ_GETELEVATION,
	OP_ENTITYOBJ_SETELEVATION,
	OP_ENTITYOBJ_GETLOCALATTRIBUTE,
	OP_ENTITYOBJ_SETLOCALATTRIBUTE,
	OP_ENTITYOBJ_GETOWNER,
	OP_ENTITYOBJ_GETCLASS,
	OP_ENTITYOBJ_GETINCOMBATUSECOST,
	OP_ENTITYOBJ_GETGLOBALATTRIBUTE,
	OP_ENTITYOBJ_SETGLOBALATTRIBUTE,
	OP_ENTITYOBJ_GETWALLORIENTATION,
	OP_ENTITYOBJ_BLOCKACCESS,
	OP_ENTITYOBJ_UNBLOCKACCESS,
	OP_ENTITYOBJ_ISACCESSBLOCKED,
	OP_ENTITYOBJ_SETSYMPTOM,
	OP_ENTITYOBJ_ISSYMPTOMACTIVE,
	OP_ENTITYOBJ_GETGENRE,
	OP_ENTITYOBJ_GETHEALTH,
	OP_ENTITYOBJ_SETHEALTH,
	OP_ENTITYOBJ_GETEXTENDEDATTRIBUTE,
	OP_ENTITYOBJ_SETEXTENDEDATTRIBUTE,
	OP_ENTITYOBJ_GETLEVEL,
	OP_ENTITYOBJ_SETLEVEL,
	OP_ENTITYOBJ_GETEXPERIENCE,
	OP_ENTITYOBJ_SETEXPERIENCE,
	OP_ENTITYOBJ_GETINCOMBATACTIONPOINTS,
	OP_ENTITYOBJ_GETACTIONPOINTS,
	OP_ENTITYOBJ_SETACTIONPOINTS,
	OP_ENTITYOBJ_ISHABILITYACTIVE,
	OP_ENTITYOBJ_SETHABILITY,
	OP_ENTITYOBJ_USEHABILITY,
	OP_ENTITYOBJ_ISRUNMODEACTIVE,
	OP_ENTITYOBJ_SETRUNMODE,
	OP_ENTITYOBJ_MOVETO,
	OP_ENTITYOBJ_ISMOVING,
	OP_ENTITYOBJ_STOPMOVING,
	OP_ENTITYOBJ_EQUIPITEM,
	OP_ENTITYOBJ_REMOVEITEMEQUIPPED,
	OP_ENTITYOBJ_GETITEMEQUIPPED,
	OP_ENTITYOBJ_ISITEMEQUIPPED,
	OP_ENTITYOBJ_DROPITEM,
	OP_ENTITYOBJ_USEITEM,
	OP_ENTITYOBJ_MANIPULATE,
	OP_ENTITYOBJ_SETTRANSPARENTMODE,
	OP_ENTITYOBJ_ISTRANSPARENTMODEACTIVE,
	OP_ENTITYOBJ_CHANGEANIMORIENTATION,
	OP_ENTITYOBJ_GETANIMORIENTATION,
	OP_ENTITYOBJ_SETALINGMENT,
	OP_ENTITYOBJ_SETALINGMENTWITH,
	OP_ENTITYOBJ_SETALINGMENTAGAINST,
	OP_ENTITYOBJ_GETALINGMENT,
	OP_ENTITYOBJ_HITENTITY,
	OP_ENTITYOBJ_SETSCRIPT,
	OP_ENTITYOBJ_ISGHOSTMOVEMODEACTIVE,
	OP_ENTITYOBJ_SETGHOSTMOVEMODE,
	OP_ENTITYOBJ_GETRANGE,
	OP_ENTITYOBJ_ISINRANGE
  } OpcodeType; // Tipo de opcode

  // Algunos opcodes poseeran argumentos
  union {
	struct {
	  // OP_JMPx
	  word unLabel; // Etiqueta
	} JmpArg;

	struct {
	  // OP_xLOAD
	  word unAddress; // Direccion
	} LoadArg;

	struct {
	  // OP_xSTORE
	  word unAddress; // Direccion
	} StoreArg;

	struct {
	  // OP_NPUSH
	  float fValue; // Valor
	} NPushArg;

	struct {
	  // OP_SPUSH
	  sbyte* szValue; // Valor
	} SPushArg;

	struct {
	  // OP_EPUSH
	  word unValue; // Valor
	} EPushArg;

	struct {
	  // OP_CALL_FUNC
	  sbyte* szIdentifier; // Nombre funcion
	} CallFuncArg;	  

	struct {
	  // OP_LABEL
	  word unLabelValue; // Valor del label
	} LabelArg;
  };
    
  // Resto de datos
  dword           udOpcodePos; // Posicion del opcode en el codigo
  struct sOpcode* pSigOpcode;  // Enlace a sig. opcode
  
  // Modulo de emision de codigo
  sword nWasVisit;
} sOpcode;

typedef struct sLabel {
  // Info asociada a una etiqueta
  sbyte*   szLabelName; // Nombre de la etiqueta
  sword    nSources;    // Source  
  sOpcode* pPos;        // Posicion en la secuencia de opcodes
} sLabel;

// Estructura de la tabla de simbolos
typedef struct sSymbolTable {
  // Define una tabla de simbolos
  struct sSymTableNode* Table[HASH_SIZE]; // Entradas a la tabla hash
  struct sSymbolTable*  pParent;          // Tabla hash anterior
} sSymbolTable;

// Definciones forward
struct sVar;
struct sConst;

typedef struct sSymTableNode {
  // Nodo asociado a una entrada en la tabla de simbolos
  enum {
	SYM_TYPE_CONST, // Tipo de identificador constante	
	SYM_TYPE_VAR,   // Tipo de identificador variable
	SYM_TYPE_ARG,   // Tipo de identificador de argumento
	SYM_TYPE_FUNC,  // Tipo de identificador funcion
  } SymTableNodeType; // Tipo de identificador
  
  // Segun sea el tipo de identificador
  union {
	struct sConst*    pIdConst; // Enlace en el AST al identificador constante
	struct sVar*      pIdVar;   // Enlace en el AST al identificador vble
	struct sArgument* pIdArg;   // Enlace en el AST al identificador de argumento
	struct sFunc*     pIdFunc;  // Enlace en el AST al identificador de funcion
  };

  sbyte*                 szIdentifier; // Clave (identificador)  
  struct sSymTableNode* pSigNode;     // Sig. identificador con misma pos.
} sSymTableNode;

// Enumerados generales
typedef enum {
  // Tipos de funciones API
  API_PASSTORGBCOLOR,
  API_GETREDCOMPONENT,
  API_GETGREENCOMPONENT,
  API_GETBLUECOMPONENT,
  API_RAND,
  API_GETINTEGERVALUE,
  API_GETDECIMALVALUE,
  API_GETSTRINGSIZE,
  API_WRITETOLOGGER,
  API_ENABLECRISOLSCRIPTWARNINGS,
  API_DISABLECRISOLSCRIPTWARNINGS,
  API_SHOWFPS,
  API_WAIT
} eExpAPIFunc; 

typedef enum {
  // Tipos de metodos asociados al objeto GameObj
  GAMEOBJ_QUITGAME,
  GAMEOBJ_WRITETOCONSOLE,
  GAMEOBJ_ACTIVEADVICEDIALOG,
  GAMEOBJ_ACTIVEQUESTIONDIALOG,
  GAMEOBJ_ACTIVETEXTREADERDIALOG,
  GAMEOBJ_ADDOPTIONTOTEXTSELECTORDIALOG,
  GAMEOBJ_RESETOPTIONSINTEXTSELECTORDIALOG,
  GAMEOBJ_ACTIVETEXTSELECTORDIALOG,
  GAMEOBJ_PLAYMIDIMUSIC,
  GAMEOBJ_STOPMIDIMUSIC,
  GAMEOBJ_PLAYWAVAMBIENTSOUND,
  GAMEOBJ_STOPWAVAMBIENTSOUND,
  GAMEOBJ_ACTIVETRADEITEMSINTERFAZ,
  GAMEOBJ_ADDOPTIONTOCONVERSATORINTERFAZ,
  GAMEOBJ_RESETOPTIONSINCONVERSATORINTERFAZ,
  GAMEOBJ_ACTIVECONVERSATORINTERFAZ,
  GAMEOBJ_DESACTIVECONVERSATORINTERFAZ,
  GAMEOBJ_GETOPTIONFROMCONVERSATORINTERFAZ,
  GAMEOBJ_SHOWPRESENTATION,
  GAMEOBJ_BEGINCUTSCENE,
  GAMEOBJ_ENDCUTSCENE,
  GAMEOBJ_SETSCRIPT,
  GAMEOBJ_ISKEYPRESSED
} eExpGameObjMethod; 

typedef enum {
  // Tipos de metodos asociados al objeto World
  WORLDOBJ_GETAREANAME,
  WORLDOBJ_GETAREAID,
  WORLDOBJ_GETAREAWIDTH,
  WORLDOBJ_GETAREAHEIGHT,
  WORLDOBJ_GETHOUR,
  WORLDOBJ_GETMINUTE,
  WORLDOBJ_SETHOUR,
  WORLDOBJ_SETMINUTE,
  WORLDOBJ_GETENTITY,
  WORLDOBJ_GETPLAYER,
  WORLDOBJ_ISFLOORVALID,
  WORLDOBJ_GETITEMAT,
  WORLDOBJ_GETNUMITEMSAT,		
  WORLDOBJ_GETDISTANCE,
  WORLDOBJ_CALCULEPATHLENGHT,
  WORLDOBJ_LOADAREA,
  WORLDOBJ_CHANGEENTITYLOCATION,
  WORLDOBJ_ATTACHCAMERATOENTITY,
  WORLDOBJ_ATTACHCAMERATOLOCATION,  
  WORLDOBJ_ISCOMBATMODEACTIVE,
  WORLDOBJ_ENDCOMBAT,
  WORLDOBJ_GETCRIATUREINCOMBATTURN,
  WORLDOBJ_GETCOMBATANT,
  WORLDOBJ_GETNUMBEROFCOMBATANTS,  
  WORLDOBJ_GETAREALIGHTMODEL,
  WORLDOBJ_SETIDLESCRIPTTIME,
  WORLDOBJ_SETSCRIPT,
  WORLDOBJ_DESTROYENTITY,
  WORLDOBJ_CREATECRIATURE,
  WORLDOBJ_CREATEWALL,
  WORLDOBJ_CREATESCENARYOBJECT,
  WORLDOBJ_CREATEITEMABANDONED,
  WORLDOBJ_CREATEITEMWITHOWNER,
  WORLDOBJ_SETWORLDTIMEPAUSE,  
  WORLDOBJ_ISWORLDTIMEINPAUSE,
  WORLDOBJ_SETELEVATIONAT,
  WORLDOBJ_GETELEVATIONAT,
  WORLDOBJ_NEXTTURN,
  WORLDOBJ_GETLIGHTAT,
  WORLDOBJ_PLAYWAVSOUND,
  WORLDOBJ_SETSCRIPTAT,
} eExpWorldObjMethod;

typedef enum {		
  // Tipos de metodos de entidad
  ENTITYOBJ_GETNAME,
  ENTITYOBJ_SETNAME,
  ENTITYOBJ_GETENTITYTYPE,
  ENTITYOBJ_GETTYPE,
  ENTITYOBJ_SAY,
  ENTITYOBJ_SHUTUP,
  ENTITYOBJ_ISSAYING,
  ENTITYOBJ_ATTACHGFX,
  ENTITYOBJ_RELEASEGFX,
  ENTITYOBJ_RELEASEALLGFX,
  ENTITYOBJ_ISGFXATTACHED,
  ENTITYOBJ_GETNUMITEMSINCONTAINER,
  ENTITYOBJ_GETITEMFROMCONTAINER,
  ENTITYOBJ_ISITEMINCONTAINER,
  ENTITYOBJ_TRANSFERITEMTOCONTAINER,
  ENTITYOBJ_INSERTITEMINCONTAINER,
  ENTITYOBJ_REMOVEITEMOFCONTAINER,
  ENTITYOBJ_SETANIMTEMPLATESTATE,
  ENTITYOBJ_SETPORTRAITANIMTEMPLATESTATE,
  ENTITYOBJ_SETIDLESCRIPTTIME,
  ENTITYOBJ_SETLIGHT,
  ENTITYOBJ_GETLIGHT,	
  ENTITYOBJ_GETXPOS,
  ENTITYOBJ_GETYPOS,
  ENTITYOBJ_GETELEVATION,
  ENTITYOBJ_SETELEVATION,
  ENTITYOBJ_GETLOCALATTRIBUTE,
  ENTITYOBJ_SETLOCALATTRIBUTE,
  ENTITYOBJ_GETOWNER,
  ENTITYOBJ_GETCLASS,
  ENTITYOBJ_GETINCOMBATUSECOST,
  ENTITYOBJ_GETGLOBALATTRIBUTE,
  ENTITYOBJ_SETGLOBALATTRIBUTE,
  ENTITYOBJ_GETWALLORIENTATION,
  ENTITYOBJ_BLOCKACCESS,
  ENTITYOBJ_UNBLOCKACCESS,
  ENTITYOBJ_ISACCESSBLOCKED,
  ENTITYOBJ_SETSYMPTOM,
  ENTITYOBJ_ISSYMPTOMACTIVE,
  ENTITYOBJ_GETGENRE,
  ENTITYOBJ_GETHEALTH,
  ENTITYOBJ_SETHEALTH,
  ENTITYOBJ_GETEXTENDEDATTRIBUTE,
  ENTITYOBJ_SETEXTENDEDATTRIBUTE,
  ENTITYOBJ_GETLEVEL,
  ENTITYOBJ_SETLEVEL,
  ENTITYOBJ_GETEXPERIENCE,
  ENTITYOBJ_SETEXPERIENCE,
  ENTITYOBJ_GETINCOMBATACTIONPOINTS,
  ENTITYOBJ_GETACTIONPOINTS,
  ENTITYOBJ_SETACTIONPOINTS,
  ENTITYOBJ_ISHABILITYACTIVE,
  ENTITYOBJ_SETHABILITY,
  ENTITYOBJ_USEHABILITY,
  ENTITYOBJ_ISRUNMODEACTIVE,
  ENTITYOBJ_SETRUNMODE,
  ENTITYOBJ_MOVETO,
  ENTITYOBJ_ISMOVING,
  ENTITYOBJ_STOPMOVING,
  ENTITYOBJ_EQUIPITEM,
  ENTITYOBJ_REMOVEITEMEQUIPPED,
  ENTITYOBJ_GETITEMEQUIPPED,
  ENTITYOBJ_ISITEMEQUIPPED,
  ENTITYOBJ_DROPITEM,
  ENTITYOBJ_USEITEM,
  ENTITYOBJ_MANIPULATE,
  ENTITYOBJ_SETTRANSPARENTMODE,
  ENTITYOBJ_ISTRANSPARENTMODEACTIVE,
  ENTITYOBJ_CHANGEANIMORIENTATION,
  ENTITYOBJ_GETANIMORIENTATION,
  ENTITYOBJ_SETALINGMENT,
  ENTITYOBJ_SETALINGMENTWITH,
  ENTITYOBJ_SETALINGMENTAGAINST,
  ENTITYOBJ_GETALINGMENT,
  ENTITYOBJ_HITENTITY,
  ENTITYOBJ_SETSCRIPT,
  ENTITYOBJ_ISGHOSTMOVEMODEACTIVE,
  ENTITYOBJ_SETGHOSTMOVEMODE,
  ENTITYOBJ_GETRANGE,
  ENTITYOBJ_ISINRANGE,
} eExpEntityObjMethod;

// Estructuras para los nodos del AST
typedef struct sType {
  // Representa el tipo de un identificador    
  enum {
	TYPE_VOID,   // Sin valor asociado
	TYPE_NUMBER, // Valor numerico
	TYPE_STRING, // Cadena
	TYPE_ENTITY  // Entidad
  } ValueType; // Tipo de valor  

  word unSrcLine; // Linea donde aparece
} sType;

typedef struct sArgument {
  // Representa la declaracion de un argumento de funcion
  enum {
	ARGUMENT_VALUE, // Paso por valor
	ARGUMENT_REF    // Paso por referencia
  } ArgumentPassType; // Tipo de paso del argumento

  sType*            pType;        // Tipo argumento
  sbyte*             szIdentifier; // Identificador
  struct sArgument* pSigArgument; // Enlace a un posible sig. argumento
  word      unSrcLine;    // Linea donde aparece

  // Modulo resource
  word unOffsetDecl;
} sArgument;

typedef struct sExp {
  // Representa la declaracion de una expresion  
  enum {
	EXP_GLOBAL_CONST_ENTITY = 0, // Constante global de tipo entity
	EXP_GLOBAL_CONST_NUMBER,	 // Constante global de tipo number
	EXP_IDENTIFIER,				 // Identificador	
	EXP_NUMBER_VALUE,			 // Valor numerico
	EXP_STRING_VALUE,			 // Cadena
	EXP_ASSING,					 // Assignacion
	EXP_EQUAL,					 // Comparacion "igual que"
	EXP_NOEQUAL,				 // Comparacion "distinto que"
	EXP_LESS,					 // Comparacion "menor que"
	EXP_LESSEQUAL,				 // Comparacion "menor o igual a"
	EXP_GREATER,				 // Comparacion "mayor que"
	EXP_GREATEREQUAL,			 // Comparacion "mayor o igual a"
	EXP_ADD,					 // Suma
	EXP_MINUS,					 // Resta
	EXP_MULT,					 // Multiplicacion 
	EXP_DIV,					 // Division
	EXP_MODULO,					 // Modulo o resto
	EXP_UMINUS,					 // Menos unario
	EXP_NOT,					 // No logico
	EXP_AND,					 // Y logico
	EXP_OR,						 // O logico
	EXP_FUNC_INVOKE,			 // Llamada a una funcion definida por el usuario
	EXP_APIFUNC_INVOKE,			 // Llamada a una funcion de API
	EXP_GAMEOBJ_INVOKE,			 // Uso de un metodo del objeto game
	EXP_WORLDOBJ_INVOKE,		 // Uso de un metodo del objeto world
	EXP_ENTITYOBJ_INVOKE,		 // Uso de un metodo de entidad
	EXP_CAST					 // Cast explicito, se crean en el TypeCheck
  } ExpType; // Tipo de expresion

  // Por cada tipo de expresion una estructura
  union {
	struct {
	  // EXP_IDENTIFIER
	  sbyte* szIdentifier; // Identificador
	} ExpIdentifier;

	struct {
	  // EXP_GLOBAL_CONST_ENTITY
	  unsigned short unValue; // Valor asociado
	} ExpGlobalConstEntity;

	struct {
	  // EXP_GLOBAL_CONST_NUMBER
	  float fValue; // Valor asociado
	} ExpGlobalConstNumber;

	struct {
	  // EXP_NUMBER_VALUE
	  float fNumValue; // Valor numerico
	} ExpNumValue;
	
	struct {
	  // EXP_STRING_VALUE
	  sbyte* szStrValue; // Valor de la cadena
	} ExpStrValue;

	struct {
	  // EXP_ASSING
	  sbyte*        szIdentifier; // Identificador
	  struct sExp* pRightExp;    // Exp a dcha.
	} ExpAssing;
		
	struct {
	  // EXP_EQUAL
	  struct sExp* pLeftExp;  // Exp a izq.
	  struct sExp* pRightExp; // Exp a dcha.
	  // Modulo Resource
	  word unTrueLabel;
	  word unEndLabel;
	} ExpEqual;

	struct {
	  // EXP_NOEQUAL
	  struct sExp* pLeftExp;  // Exp a izq.
	  struct sExp* pRightExp; // Exp a dcha.
	  // Modulo Resource
	  word unTrueLabel;
	  word unEndLabel;
	} ExpNoEqual;

	struct {
	  // EXP_LESS
	  struct sExp* pLeftExp;  // Exp a izq.
	  struct sExp* pRightExp; // Exp a dcha.
	  // Modulo Resource
	  word unTrueLabel;
	  word unEndLabel;
	} ExpLess;

	struct {
	  // EXP_LESSEQUAL
	  struct sExp* pLeftExp;  // Exp a izq.
	  struct sExp* pRightExp; // Exp a dcha.
	  // Modulo Resource
	  word unTrueLabel;
	  word unEndLabel;
	} ExpLEqual;

	struct {
	  // EXP_GREATER
	  struct sExp* pLeftExp;  // Exp a izq.
	  struct sExp* pRightExp; // Exp a dcha.
	  // Modulo Resource
	  word unTrueLabel;
	  word unEndLabel;
	} ExpGreater;

	struct {
	  // EXP_GREATEREQUAL
	  struct sExp* pLeftExp;  // Exp a izq.
	  struct sExp* pRightExp; // Exp a dcha.
	  // Modulo Resource
	  word unTrueLabel;
	  word unEndLabel;
	} ExpGEqual;

	struct {
	  // EXP_ADD
	  struct sExp* pLeftExp;  // Exp a izq.
	  struct sExp* pRightExp; // Exp a dcha.
	} ExpAdd;

	struct {
	  // EXP_MINUS
	  struct sExp* pLeftExp;  // Exp a izq.
	  struct sExp* pRightExp; // Exp a dcha.	  
	} ExpMinus;

	struct {
	  // EXP_MULT
	  struct sExp* pLeftExp;  // Exp a izq.
	  struct sExp* pRightExp; // Exp a dcha.
	} ExpMult;

	struct {
	  // EXP_DIV
	  struct sExp* pLeftExp;  // Exp a izq.
	  struct sExp* pRightExp; // Exp a dcha.
	} ExpDiv;

	struct {
	  // EXP_MODULO
	  struct sExp* pLeftExp;  // Exp a izq.
	  struct sExp* pRightExp; // Exp a dcha.
	} ExpModulo;

	struct {
	  // EXP_UMINOS	  
	  struct sExp* pExp; // Exp	  
	} ExpUMinus;

	struct {
	  // EXP_NOT
	  struct sExp* pExp;  // Exp
	  // Modulo Resource
	  word unFalseLabel;
	  word unEndLabel;
	} ExpNot;

	struct {
	  // EXP_AND
	  struct sExp* pLeftExp;  // Exp a izq.
	  struct sExp* pRightExp; // Exp a dcha.
	  // Modulo Resource
	  word unFalseLabel;
	} ExpAnd;

	struct {
	  // EXP_OR
	  struct sExp* pLeftExp;  // Exp a izq.
	  struct sExp* pRightExp; // Exp a dcha.
	  // Modulo Resource	  
	  word unTrueLabel;
	} ExpOr;

	struct {
	  // EXP_FUNC_INVOKE
	  sbyte*        szIdentifier; // Identificador
	  struct sExp* pParams;      // Parametros pasados
	} ExpFuncInvoke;

	struct {
	  // EXP_APIFUNC_INVOKE	  
	  eExpAPIFunc  ExpAPIFunc; // Tipo de funcion
	  struct sExp* pParams;    // Parametros pasados.
	} ExpAPIFuncInvoke;

	struct {
	  // EXP_GAMEOBJ_INVOKE	  
	  eExpGameObjMethod ExpGameObjMethod; // Tipo de metodo
	  struct sExp*		pParams;		  // Parametros pasados
	} ExpGameObjInvoke;	

	struct {
  	  // EXP_WORLDOBJ_INVOKE
	  eExpWorldObjMethod ExpWorldObjMethod; // Tipo de metodo 
	  struct sExp*       pParams;           // Parametros pasados
	} ExpWorldObjInvoke;

	struct {
  	  // EXP_ENTITYOBJ_INVOKE
	  eExpEntityObjMethod ExpEntityObjMethod; // Tipo de metodo
	  sbyte*              szIdentifier;       // Identificador de la entidad
	  struct sExp*        pParams;            // Parametros pasados
	} ExpEntityObjInvoke;

	struct {
	  // EXP_CAST
	  sType*       pType; // Enlace al tipo del cast
	  struct sExp* pExp;  // Enlace a la expresion que cualifica
	} ExpCast; 
  };
  
  struct sExp* pSigExp;   // Posible enlace a sig. expresion
  sType*       pExpType;  // Tipo de la exp. Necesario en el TypeCheck
  word unSrcLine; // Linea donde aparece
} sExp;

typedef struct sStm {
  // Representa una sentencia
  enum {
	STM_SEQ,     // Sequencia de Stm
	STM_RETURN,  // Retorno de valor
	STM_IF,      // Sentencia if / then
	STM_IFELSE,  // Sentencia if / then / else
	STM_WHILE,   // Sentencia while / do
	STM_EXP      // Sentencia con expresion simple
  } StmType; // Tipo de sentencia

  // Por cada tipo de sentencia, una estructura de informacion
  union {
	struct {
	  // STM_SEQ
	  struct sStm* pFirst;  // Primera Stm
	  struct sStm* pSecond; // Segunda Stm
	} StmSeq;

	struct {
	  // STM_RETURN
	  sExp* pExp; // Exp de retorno
	} StmReturn;

	struct { 
	  // STM_IF
	  sExp*        pExp;     // Exp logica
	  struct sStm* pThenStm; // Stm a ejecutar
	  // Modulo Resource	  
	  word unEndLabel;
	} If;

	struct {
	  // STM_IFELSE
	  sExp*        pExp;     // Exp logica
	  struct sStm* pThenStm; // Stm del then
	  struct sStm* pElseStm; // Stm del else
	  // Modulo Resource
	  word unElseLabel;
	  word unEndLabel;
	} IfElse;

	struct {
	  // STM_WHILE
	  sExp*        pExp;   // Exp logica
	  struct sStm* pDoStm; // Stm a ejecutar
	  // Modulo Resource
	  word unStartLabel;
	  word unEndLabel;
	} While;

	struct {
	  // STM_EXP
	  struct sExp* pExp; // Expresion simple
	} Exp;
  };

  word unSrcLine; // Linea donde aparece
} sStm;

typedef struct sConst {
  // Representa una declaracion de constantes
  enum {
	CONST_DECLSEQ, // Una secuencia
	CONST_DECL     // Una declaracion	
  } ConstType; // Tipo de nodo

  // Para cada tipo una estructura
  union {
	struct {
	  // CONST_DECLSEQ
	  struct sConst* pFirst;  // Primero en seq.
	  struct sConst* pSecond; // Segundo en seq.
	} ConstSeq;

	struct {
	  // CONST_DECL
	  sType* pType;        // Tipo de dato
	  sbyte*  szIdentifier; // Identificador
	  sExp*  pExp;         // Expresion asignada
	  // Modulo resource
	  word unOffsetDecl;
	} ConstDecl;	
  };

  word unSrcLine; // Linea donde aparece
} sConst;

typedef struct sVar {
  // Representa una declaracion de variables
  enum {
	VAR_TYPEDECL_SEQ,       // Secuencia de declaracion por tipo
	VAR_TYPEDECL,           // Declaracion de identificadores por tipo
	VAR_IDENTIFIERDECL_SEQ  // Secuencia de identificadores asociados un tipo
  } VarType; // Tipo de nodo

  // Para cada tipo una estructura
  union {
	struct {
	  // VAR_TYPEDECL_SEQ
	  struct sVar* pFirst;  // Primero en secuencia
	  struct sVar* pSecond; // Segundo en secuencia
	} VarTypeDeclSeq;

	struct {
	  // VAR_TYPEDECL
	  sType*       pType;        // Tipo de la secuencia de identificadores
	  struct sVar* pVarIdentSeq; // Inicio de la secuencia de declaraciones
	} VarTypeDecl;

	struct {
	  // VAR_IDENTIFIERDECL_SEQ
	  sbyte*        szIdentifier; // Identificador
	  sExp*        pValue;       // Posible expresion asignada
	  struct sVar* pSigIdent;    // Siguiente identificador
	  // Modulo resource
	  word unOffsetDecl;
	} VarIdentDeclSeq;
  };

  word unSrcLine; // Linea donde aparece
} sVar;

typedef struct sFunc {
  // Representa a una funcion
  enum {
	FUNC_SEQ,  // Secuencia de funciones
	FUNC_DECL, // Declaracion de funciones
  } eFuncType;

  // Por cada tipo una estructura
  union {
	struct {
	  // FUNC_SEQ
	  struct sFunc* pFirst;  // Primero en secuencia
	  struct sFunc* pSecond; // Segunda en secuencia
	} FuncSeq;

	struct {
	  // FUNC_DECL
	  sType*        pType;        // Tipo de funcion
	  sbyte*         szIdentifier; // Identificador
	  word  unFuncIdx;    // Identificador numerico de la funcion
	  sArgument*    pArguments;   // Argumentos
	  sConst*       pConst;       // Constantes locales a la funcion
	  sVar*         pVar;         // Variables
	  sStm*         pStm;         // Sentencias (cuerpo de la funcion)
	  sSymbolTable* pSymTable;    // Tabla de simbolos
	  // Modulo SymTableCheck
	  word unWasInvoked;
	  // Modulo Resource
	  word unNumOffsets;  // Num. de offsets (variables / constantes)
	  word unFirstOffset; // Primer offset, teniendo usando ambito anterior
	  word unNumLabels;   // Numero de etiquetas
	  // Modulo OpcodeGen
	  sbyte*       szSignature; // Firma (Tipo de los Parametros) Tipo Funcion
	  sOpcode*     pOpcodeList; // Lista de opcodes
	  sLabel*      pLabelList;  // Lista de etiquetas
	  word unStackSize; // Tamaño de la pila
	} FuncDecl;
  };

  word unSrcLine; // Linea donde aparece
} sFunc;

typedef struct sImport {
  // Representa el espacio formado por un archivo importado con fun. globales
  enum {
	IMPORT_SEQ,  // Secuencia de nodos Import
	IMPORT_FUNC, // Enlace a funcion dentro del archivo importado
  } ImportType; // Tipo de nodo 

  // Por cada tipo una estructura
  union {
	struct {
	  // IMPORT_SEQ
	  struct sImport* pFirst;  // Enlace a primero
	  struct sImport* pSecond; // Enlace a segundo
	} ImportSeq;

	struct {
	  // IMPORT_FUNC
	  sFunc*        pFunctions; // Enlace a la funciones
	  sbyte*         szFileName; // Nombre del fichero import
	  sSymbolTable* pSymTable;  // Tabla de simbolos
	} ImportFunc;
  };

  word unSrcLine; // Linea donde aparece
} sImport;

typedef struct sScriptType {
  // Representa el tipo de un script (evento al que responde) y su parametros
  // Gracias al tipo se sabra el numero necesario de argumentos y el tipo
  // de cada uno de ellos segun su orden en la secuencia de argumentos
  enum {
	SCRIPTEVENT_ONSTARTGAME = 0,
	SCRIPTEVENT_ONCLICKHOURPANEL,
	SCRIPTEVENT_ONFLEECOMBAT,
	SCRIPTEVENT_ONKEYPRESSED,
	SCRIPTEVENT_ONSTARTCOMBATMODE,
	SCRIPTEVENT_ONENDCOMBATMODE,
	SCRIPTEVENT_ONNEWHOUR,
	SCRIPTEVENT_ONENTERINAREA,
	SCRIPTEVENT_ONWORLDIDLE,
	SCRIPTEVENT_ONSETINFLOOR,
	SCRIPTEVENT_ONSETOUTOFFLOOR,
	SCRIPTEVENT_ONGETITEM,
	SCRIPTEVENT_ONDROPITEM,
	SCRIPTEVENT_ONOBSERVEENTITY,
	SCRIPTEVENT_ONTALKTOENTITY,
	SCRIPTEVENT_ONMANIPULATEENTITY,
	SCRIPTEVENT_ONDEATH,
	SCRIPTEVENT_ONRESURRECT,
	SCRIPTEVENT_ONINSERTINEQUIPMENTSLOT,
	SCRIPTEVENT_ONREMOVEFROMEQUIPMENTSLOT,
	SCRIPTEVENT_ONUSEHABILITY,
	SCRIPTEVENT_ONACTIVATEDSYMPTOM,
	SCRIPTEVENT_ONDEACTIVATEDSYMPTOM,
	SCRIPTEVENT_ONHITENTITY,
	SCRIPTEVENT_ONSTARTCOMBATTURN,
	SCRIPTEVENT_ONCRIATUREINRANGE,
	SCRIPTEVENT_ONCRIATUREOUTOFRANGE,
	SCRIPTEVENT_ONENTITYIDLE,
	SCRIPTEVENT_ONUSEITEM,
	SCRIPTEVENT_ONTRADEITEM,
	SCRIPTEVENT_ONENTITYCREATED
  } ScriptEventType; // Tipo de script en cuanto a evento que recibe
  
  sArgument*   pArguments; // Posible lista de argumentos recibidos
  word unSrcLine;  // Linea donde aparece
} sScriptType;

typedef struct sScript {
  // Representa a un script
  enum {
	SCRIPT_SEQ, // Secuencia de scripts
	SCRIPT_DECL // Declaracion de un script	
  } ScriptType; // Tipo de nodo

  // Para cada tipo de script una estructura
  union {
	struct {
	  // SCRIPT_SEQ
	  struct sScript* pFirst;  // Primero en secuencia
	  struct sScript* pSecond; // Siguiente en secuencia
	} ScriptSeq;

	struct {
	  // SCRIPT_DECL
	  sScriptType*  pType;      // Tipo de script
	  sImport*      pImport;    // Secuencia de funciones globales importadas
	  sConst*       pConst;     // Constantes
      sVar*         pVar;       // Vbles
	  sFunc*        pFunc;      // Funciones
	  sStm*         pStm;       // Sentencias asociadas (cuerpo del script)
	  sbyte*        szFileName; // Nombre del fichero script
	  sSymbolTable* pSymTable;  // Tabla de simbolos
	  // Modulo Resource
	  word unNumOffsets;   // Num. de offsets (variables / constantes)
	  word unFirstOffset;  // Primer offset, teniendo usando ambito anterior
	  word unNumLabels;    // Numero de etiquetas
	  word unNumFunctions; // Numero de funciones
	  // Modulo OpcodeGen
	  sOpcode* pOpcodeList; // Lista de Opcodes
	  sLabel*  pLabelList;  // Lista de etiquetas
	  word     unStackSize; // Tamaño de la pila
	  // Modulo Assembling
	  long slFileOffset; // Offset donde localizar el script en el archivo
	} ScriptDecl;
  };

  word unSrcLine; // Linea donde aparece
} sScript;

typedef struct sGlobal {
  // Representa el espacio global
  sConst*       pConst;     // Enlace a las constantes
  sVar*         pVar;       // Enlace a las vbles
  sStm*         pReturnStm; // Sentencias de retorno asociad
  sScript*      pScript;    // Enlace a los scripts  
  sbyte*        szFileName; // Nombre del fichero global
  sSymbolTable* pSymTable;  // Tabla de simbolos
  // Modulo Resource
  word unNumOffsets; // Numero de offsets
  // Modulo OpcodeGen
  sOpcode* pOpcodeList; // Lista de Opcodes
} sGlobal;

// Funciones relacionadas con la creacion de los nodos de Opcode
sOpcode* MakeNopOpcode(sOpcode* pSigOpcode);
sOpcode* MakeNNegOpcode(sOpcode* pSigOpcode);
sOpcode* MakeNMulOpcode(sOpcode* pSigOpcode);
sOpcode* MakeNAddOpcode(sOpcode* pSigOpcode);
sOpcode* MakeNModOpcode(sOpcode* pSigOpcode);
sOpcode* MakeNDivOpcode(sOpcode* pSigOpcode);
sOpcode* MakeNSubOpcode(sOpcode* pSigOpcode);
sOpcode* MakeSAddOpcode(sOpcode* pSigOpcode);
sOpcode* MakeJmpOpcode(word unLabel,
					   sOpcode* pSigOpcode);
sOpcode* MakeJmpFalseOpcode(word unLabel,
					        sOpcode* pSigOpcode);
sOpcode* MakeJmpTrueOpcode(word unLabel,
					       sOpcode* pSigOpcode);
sOpcode* MakeNJmpEqOpcode(word unLabel,
				          sOpcode* pSigOpcode);
sOpcode* MakeNJmpNeOpcode(word unLabel,
				          sOpcode* pSigOpcode);
sOpcode* MakeNJmpGeOpcode(word unLabel,
				          sOpcode* pSigOpcode);
sOpcode* MakeNJmpGtOpcode(word unLabel,
				          sOpcode* pSigOpcode);
sOpcode* MakeNJmpLtOpcode(word unLabel,
				          sOpcode* pSigOpcode);
sOpcode* MakeNJmpLeOpcode(word unLabel,
				          sOpcode* pSigOpcode);
sOpcode* MakeSJmpEqOpcode(word unLabel,
				          sOpcode* pSigOpcode);
sOpcode* MakeSJmpNeOpcode(word unLabel,
				          sOpcode* pSigOpcode);
sOpcode* MakeEJmpEqOpcode(word unLabel,
				          sOpcode* pSigOpcode);
sOpcode* MakeEJmpNeOpcode(word unLabel,
				          sOpcode* pSigOpcode);
sOpcode* MakeDupOpcode(sOpcode* pSigOpcode);
sOpcode* MakePopOpcode(sOpcode* pSigOpcode);
sOpcode* MakeNReturnOpcode(sOpcode* pSigOpcode);
sOpcode* MakeSReturnOpcode(sOpcode* pSigOpcode);
sOpcode* MakeEReturnOpcode(sOpcode* pSigOpcode);
sOpcode* MakeReturnOpcode(sOpcode* pSigOpcode);
sOpcode* MakeNLoadOpcode(word unAddress,
						 sOpcode* pSigOpcode);
sOpcode* MakeSLoadOpcode(word unAddress,
						 sOpcode* pSigOpcode);
sOpcode* MakeELoadOpcode(word unAddress,
						 sOpcode* pSigOpcode);
sOpcode* MakeNStoreOpcode(word unAddress,
						  sOpcode* pSigOpcode);
sOpcode* MakeSStoreOpcode(word unAddress,
						  sOpcode* pSigOpcode);
sOpcode* MakeEStoreOpcode(word unAddress,
						  sOpcode* pSigOpcode);
sOpcode* MakeNPushOpcode(float fValue,
					     sOpcode* pSigOpcode);
sOpcode* MakeSPushOpcode(sbyte* szValue,
					     sOpcode* pSigOpcode);
sOpcode* MakeEPushOpcode(word unValue,
					     sOpcode* pSigOpcode);
sOpcode* MakeCastOpcode(sType* pSource,
						sType* pDest,
						sOpcode* pSigOpcode);
sOpcode* MakeCallFuncOpcode(sbyte* szIdentifier,
					        sOpcode* pSigOpcode);
sOpcode* MakeLabelOpcode(word unLabelValue,
				         sOpcode* pSigOpcode);
sOpcode* MakeAPICallOpcode(eExpAPIFunc APIFunc,
						   sOpcode* pSigOpcode);
sOpcode* MakeGameObjMethodCallOpcode(eExpGameObjMethod Method,
									 sOpcode* pSigOpcode);
sOpcode* MakeWorldObjMethodCallOpcode(eExpWorldObjMethod Method,
									  sOpcode* pSigOpcode);
sOpcode* MakeEntityObjMethodCallOpcode(eExpEntityObjMethod Method,
									   sOpcode* pSigOpcode);	

// Definicion de funciones para el control de la tabla hash (tabla de simbolos)
sSymbolTable* SymTableCreate(void);
void SymTableRelease(sSymbolTable* pSymTable);
sSymTableNode* SymTableInsertConst(sSymbolTable* pSymTable,
								   sbyte* szIdentifier,
								   sConst* pIdConst);
sSymTableNode* SymTableInsertVar(sSymbolTable* pSymTable,
								 sbyte* szIdentifier,
								 sVar* pIdVar);
sSymTableNode* SymTableInsertArgument(sSymbolTable* pSymTable,
									  sbyte* szIdentifier,
									  sArgument* pIdArgument);
sSymTableNode* SymTableInsertFunc(sSymbolTable* pSymTable,
								  sbyte* szIdentifier,
								  sFunc* pIdFunc);
sSymTableNode* SymTableGetNode(sSymbolTable* pSymTable,
							   sbyte* szIdentifier);
sword SymTableIsPresent(sSymbolTable* pSymTable,
					  sbyte* szIdentifier);
word SymTableHash(sbyte* szKey);
sSymTableNode* SymTableFindNode(sSymbolTable* pSymTable,
							    word unStartNodePos,
								sbyte* szIdentifier);

// Definicion de funciones para la creacion de los nodos del AST
// Nodo sType
sType* MakeTypeVoid(void);
sType* MakeTypeNumber(void);
sType* MakeTypeEntity(void);
sType* MakeTypeString(void);

// Nodo sArgument
sArgument* MakeArgumentByValue(sType* pType, 
					       	   sbyte* szIdentifier,
							   sArgument* pSigArg);
sArgument* MakeArgumentByRef(sType* pType, 
				       	     sbyte* szIdentifier,
							 sArgument* pSigArg);

// Nodo sExp
sExp* MakeExpIdentifier(sbyte* szIdentifier);
sExp* MakeExpGlobalConstEntity(word unValue);
sExp* MakeExpGlobalConstNumber(float fValue);
sExp* MakeExpNumberValue(float fValue);
sExp* MakeExpStringValue(sbyte* szString);
sExp* MakeExpAssing(sbyte* szIdentifier, 
					sExp* pRightExp);
sExp* MakeExpEqual(sExp* pRightExp,
				   sExp* pLeftExp);
sExp* MakeExpNoEqual(sExp* pRightExp,
			  	     sExp* pLeftExp);
sExp* MakeExpLess(sExp* pRightExp,
				  sExp* pLeftExp);
sExp* MakeExpLessEqual(sExp* pRightExp,
				       sExp* pLeftExp);
sExp* MakeExpGreater(sExp* pRightExp,
				     sExp* pLeftExp);
sExp* MakeExpGreaterEqual(sExp* pRightExp,
				          sExp* pLeftExp);
sExp* MakeExpAdd(sExp* pRightExp,
				 sExp* pLeftExp);
sExp* MakeExpMinus(sExp* pRightExp,
				   sExp* pLeftExp);
sExp* MakeExpMult(sExp* pRightExp,
				  sExp* pLeftExp);
sExp* MakeExpDiv(sExp* pRightExp,
				 sExp* pLeftExp);
sExp* MakeExpModulo(sExp* pRightExp,
				    sExp* pLeftExp);
sExp* MakeExpUMinus(sExp* pExp);
sExp* MakeExpNot(sExp* pExp);
sExp* MakeExpAnd(sExp* pRightExp,
				 sExp* pLeftExp);
sExp* MakeExpOr(sExp* pRightExp,
				sExp* pLeftExp);
sExp* MakeExpUserFuncInvoke(sbyte* szIdentFunc,
						    sExp* pParams);
sExp* MakeExpAPIFunc(eExpAPIFunc APIFunc, 
					 sExp* pParams);
sExp* MakeExpGameObj(eExpGameObjMethod GameObjMethod, 
					 sExp* pParams);
sExp* MakeExpWorldObj(eExpWorldObjMethod WorldObjMethod, 
					  sExp* pParams);
sExp* MakeExpEntityObj(eExpEntityObjMethod EntityObjMethod, 
					   sExp* pParams);
sExp* MakeExpCast(sType* pType,
				  sExp* pExp);

// Nodo sStm
sStm* MakeStmSeq(sStm* pFirst,
				 sStm* pSecond);
sStm* MakeStmReturn(sExp* pExp);
sStm* MakeStmIf(sExp* pExp,
				sStm* pThenStm);
sStm* MakeStmIfElse(sExp* pExp,
					sStm* pThenStm,
					sStm* pElseStm);
sStm* MakeStmWhile(sExp* pExp,
				   sStm* pDoStm);
sStm* MakeStmExp(sExp* pExp);

// Nodo sConst
sConst* MakeConstDeclSeq(sConst* pFirst,
						 sConst* pSecond);
sConst* MakeConstDecl(sType* pType,
					  sbyte* szIdentifier,
					  sExp* pExp);

// Nodo sVar
sVar* MakeVarTypeDeclSeq(sVar* pFirst,
						 sVar* pSecond);
sVar* MakeVarTypeDecl(sType* pType,
					  sVar* pIdentDecl);
sVar* MakeVarIdentDeclSeq(sbyte* szIdentifier,
						  sExp* pValue,
						  sVar* pSigVar);

// Nodo sFunc
sFunc* MakeFuncSeq(sFunc* pFirst,
				   sFunc* pSecond);
sFunc* MakeFuncDecl(sType* pType,
				    sbyte* szIdentifier,
				    sArgument* pArguments,
				    sConst* pConst,
				    sVar* pVar,
				    sStm* pStm);

// Nodo sImport
sImport* MakeImportSeq(sImport* pFirst,
					   sImport* pSecond);
sImport* MakeImportFunc(sbyte* szFileName,
						sFunc* pFunctions);

// Nodo sScriptType
sScriptType* MakeScriptTypeOnStartGame(void);
sScriptType* MakeScriptTypeOnClickHourPanel(void);
sScriptType* MakeScriptTypeOnFleeCombat(void);
sScriptType* MakeScriptTypeOnKeyPressed(void);
sScriptType* MakeScriptTypeOnStartCombatMode(void);
sScriptType* MakeScriptTypeOnEndCombatMode(void);
sScriptType* MakeScriptTypeOnNewHour(sbyte* szNewHour);
sScriptType* MakeScriptTypeOnEnterInArea(sbyte* szIDArea);
sScriptType* MakeScriptTypeOnWorldIdle(void);
sScriptType* MakeScriptTypeOnSetInFloor(sbyte* szXPos,
										sbyte* szYPos,
										sbyte* szTheEntity);
sScriptType* MakeScriptTypeOnSetOutOfFloor(sbyte* szXPos,
										   sbyte* szYPos,
										   sbyte* szTheEntity);
sScriptType* MakeScriptTypeOnGetItem(sbyte* szTheItem,
									 sbyte* szTheCriature);
sScriptType* MakeScriptTypeOnDropItem(sbyte* szTheItem,
									  sbyte* szTheCriature);
sScriptType* MakeScriptTypeOnObserveEntity(sbyte* szTheItem,
									       sbyte* szTheCriature);
sScriptType* MakeScriptTypeOnTalkToEntity(sbyte* szTheItem,
									      sbyte* szTheCriature);
sScriptType* MakeScriptTypeOnManipulateEntity(sbyte* szTheItem,
									          sbyte* szTheCriature);
sScriptType* MakeScriptTypeOnDeath(sbyte* szTheEntity);
sScriptType* MakeScriptTypeOnResurrect(sbyte* szTheEntity);
sScriptType* MakeScriptTypeOnInsertInEquipmentSlot(sbyte* szTheItem,
										           sbyte* szTheCriature,
										           sbyte* szTheEquipmentSlot);
sScriptType* MakeScriptTypeOnRemoveFromEquipmentSlot(sbyte* szTheItem,
												     sbyte* szTheCriature,
												     sbyte* szTheEquipmentSlot);
sScriptType* MakeScriptTypeOnUseHability(sbyte* szTheCriature,
										 sbyte* szHability,
										 sbyte* szTheItem);
sScriptType* MakeScriptTypeOnActivatedSymptom(sbyte* szTheCriature,
											  sbyte* szTheSymptom);
sScriptType* MakeScriptTypeOnDeactivatedSymptom(sbyte* szTheCriature,
											    sbyte* szTheSymptom);
sScriptType* MakeScriptTypeOnHitEntity(sbyte* szTheCriature,
									   sbyte* szTheSlotUsed,
									   sbyte* szTheEntity);
sScriptType* MakeScriptTypeOnStartCombatTurn(sbyte* szTheCriature);
sScriptType* MakeScriptTypeOnCriatureInRange(sbyte* szTheCriatureWithRange,
											 sbyte* szTheCriatureInRange);
sScriptType* MakeScriptTypeOnCriatureOutOfRange(sbyte* szTheCriatureWithRange,
												sbyte* szTheCriatureOutOfRange);
sScriptType* MakeScriptTypeOnUseItem(sbyte* szTheItem,
									 sbyte* szTheEntity,
									 sbyte* szTheCriature);
sScriptType* MakeScriptTypeOnTradeItem(sbyte* szTheItem,
									   sbyte* szTheEntitySrc,
									   sbyte* szTheEntityDest);
sScriptType* MakeScriptTypeOnEntityIdle(sbyte* szTheEntity);
sScriptType* MakeScriptTypeOnEntityCreated(sbyte* szTheEntity);


// Nodo sScript
sScript* MakeScriptSeq(sScript* pFirst,
					   sScript* pSecond);
sScript* MakeScriptDecl(sbyte* szFileName,
						sScriptType* pType,
						sImport* pImport,
						sConst* pConst,
						sVar* pVar,
						sFunc* pFunc,
						sStm* pStm);

// Nodo sGlobal
sGlobal* MakeGlobal(sbyte* szFileName,
					sConst* pConst,
					sVar* pVar,
					sScript* pScript);

#endif

