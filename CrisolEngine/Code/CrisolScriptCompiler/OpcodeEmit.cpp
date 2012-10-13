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
// OpcodeEmit.cpp
// Fernando Rodriguez <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Notas:
// - Consultar OpcodeEmit.h para mas informacion
////////////////////////////////////////////////////////////////////////////////

// Includes
#include "OpcodeEmit.h"
#include <stdio.h>
#include <assert.h>

// Funciones privadas / emision de codigo
static void EmitOpcodeList(sOpcode* pOpList,						   
						   sLabel* pLabelList,
			   			   sSymbolTable* pSymTable,
						   FILE* pFile);

// Funciones privadas / apoyo
static sbyte* PassScriptTypeToString(sScriptType* pType);

// Funciones privadas / Recorrido del AST
static void OpcodeEmitScript(sScript* pScript,
							 FILE* pFile);
static void OpcodeEmitImport(sImport* pImport,
			 			     sSymbolTable* pScriptSymTable,
							 FILE* pFile);
static void OpcodeEmitFunc(sFunc* pFunc,
			   			   sSymbolTable* pScriptSymTable,
						   FILE* pFile);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre una lista de opcodes y la imprime.
// Parametros:
// - pOpList. Lista de opcodes.
// - pLabelList. Lista de labels.
// - pSymTable. Enlace a la tabla de simbolos con la que trabajara. Podra valer
//   NULL si se esta emitiendo el codigo del ambito global, pero en el caso de
//   la emision de los opcodes del script y funciones asociadas al mismo, se
//   esperara recibir la tabla de simbolos del script para hallar los nodos 
//   de los identificadores de funcion y saber asi su idx numerico.
// - pFile. Fichero donde emitir el codigo.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
EmitOpcodeList(sOpcode* pOpList,			   
			   sLabel* pLabelList,
			   sSymbolTable* pSymTable,
			   FILE* pFile)
{
  assert(pFile);

  // ¿Opcode no marcado?
  for (; pOpList; pOpList = pOpList->pSigOpcode) {
	// Antes de emitir el nombre del Opcode, se emitira la posicion que ocupara
	// en la lista de opcodes, teniendo en cuenta que los opcodes de tipo label
	// no tendran esta informacion asociada, pues en el archivo binario no 
	// figuraran
	if (pOpList->OpcodeType != OP_LABEL) {
	  fprintf(pFile, " [%5u]", pOpList->udOpcodePos);
	}

	switch(pOpList->OpcodeType) {	  
	  case OP_NOP: {
		fprintf(pFile, " nop\n");
	 } break;
	
	  case OP_NNEG: {
		fprintf(pFile, " nneg\n");
	  } break;
	
	  case OP_NMUL: {
		fprintf(pFile, " nmul\n");
	  } break;

	  case OP_NADD: {
		fprintf(pFile, " nadd\n");
	  } break;

	  case OP_NMOD: {
		fprintf(pFile, " nmod\n");
	  } break;

	  case OP_NDIV: {
		fprintf(pFile, " ndiv\n");
	  } break;

	  case OP_NSUB: {
		fprintf(pFile, " nsub\n");
	  } break;

	  case OP_SADD: {
		fprintf(pFile, " sadd\n");
	  } break;

	  case OP_JMP: {
		assert(pOpList);
		assert(pLabelList + pOpList->JmpArg.unLabel);
		fprintf(pFile, 
				" jmp %s_%u\n", 
				pLabelList[pOpList->JmpArg.unLabel].szLabelName,
				pOpList->JmpArg.unLabel);	
	  } break;

	  case OP_JMP_FALSE: {
		assert(pOpList);
		assert(pLabelList + pOpList->JmpArg.unLabel);		
		fprintf(pFile, 
				" jmp_false %s_%u\n", 
				pLabelList[pOpList->JmpArg.unLabel].szLabelName,
				pOpList->JmpArg.unLabel);
	  } break;

	  case OP_JMP_TRUE: {
		assert(pOpList);
		assert(pLabelList + pOpList->JmpArg.unLabel);		
		fprintf(pFile, 
				" jmp_true %s_%u\n", 
				pLabelList[pOpList->JmpArg.unLabel].szLabelName,
				pOpList->JmpArg.unLabel);
	  } break;

	  case OP_NJMP_EQ: {
		assert(pOpList);
		assert(pLabelList + pOpList->JmpArg.unLabel);		
		fprintf(pFile, 
				" njmp_eq %s_%u\n", 
				pLabelList[pOpList->JmpArg.unLabel].szLabelName,
				pOpList->JmpArg.unLabel);
	  } break;

	  case OP_NJMP_NE: {
		assert(pOpList);
		assert(pLabelList + pOpList->JmpArg.unLabel);		
		fprintf(pFile, 
				" njmp_ne %s_%u\n", 
				pLabelList[pOpList->JmpArg.unLabel].szLabelName,
				pOpList->JmpArg.unLabel);
	  } break;

	  case OP_NJMP_GE: {
		assert(pOpList);
		assert(pLabelList + pOpList->JmpArg.unLabel);		
		fprintf(pFile, 
				" njmp_ge %s_%u\n", 
				pLabelList[pOpList->JmpArg.unLabel].szLabelName,
				pOpList->JmpArg.unLabel);
	  } break;

	  case OP_NJMP_GT: {
		assert(pOpList);
		assert(pLabelList + pOpList->JmpArg.unLabel);		
		fprintf(pFile, 
				" njmp_gt %s_%u\n", 
				pLabelList[pOpList->JmpArg.unLabel].szLabelName,
				pOpList->JmpArg.unLabel);
	  } break;

	  case OP_NJMP_LT: {
		assert(pOpList);
		assert(pLabelList + pOpList->JmpArg.unLabel);		
		fprintf(pFile, 
				" njmp_lt %s_%u\n", 
				pLabelList[pOpList->JmpArg.unLabel].szLabelName,
				pOpList->JmpArg.unLabel);
	  } break;

	  case OP_NJMP_LE: {
		assert(pOpList);
		assert(pLabelList + pOpList->JmpArg.unLabel);		
		fprintf(pFile, 
				" njmp_le %s_%u\n", 
				pLabelList[pOpList->JmpArg.unLabel].szLabelName,
				pOpList->JmpArg.unLabel);
	  } break;

	  case OP_SJMP_EQ: {
		assert(pOpList);
		assert(pLabelList + pOpList->JmpArg.unLabel);		
		fprintf(pFile, 
				" sjmp_eq %s_%u\n", 
				pLabelList[pOpList->JmpArg.unLabel].szLabelName,
				pOpList->JmpArg.unLabel);
	  } break;

	  case OP_SJMP_NE: {
		assert(pOpList);
		assert(pLabelList + pOpList->JmpArg.unLabel);		
		fprintf(pFile, 
				" sjmp_ne %s_%u\n", 
				pLabelList[pOpList->JmpArg.unLabel].szLabelName,
				pOpList->JmpArg.unLabel);
	  } break;

	  case OP_EJMP_EQ: {
		assert(pOpList);
		assert(pLabelList + pOpList->JmpArg.unLabel);		
		fprintf(pFile, 
				" ejmp_eq %s_%u\n", 
				pLabelList[pOpList->JmpArg.unLabel].szLabelName,
				pOpList->JmpArg.unLabel);
	  } break;

	  case OP_EJMP_NE: {
		assert(pOpList);
		assert(pLabelList + pOpList->JmpArg.unLabel);		
		fprintf(pFile, 
				" ejmp_ne %s_%u\n", 
				pLabelList[pOpList->JmpArg.unLabel].szLabelName,
				pOpList->JmpArg.unLabel);
	  } break;

	  case OP_DUP: {
		fprintf(pFile, " dup\n");
	  } break;

	  case OP_POP: {
		fprintf(pFile, " pop\n");
	  } break;

	  case OP_NRETURN: {
		fprintf(pFile, " nreturn\n");
	  } break;

	  case OP_SRETURN: {
		fprintf(pFile, " sreturn\n");
	  } break;

	  case OP_ERETURN: {
		fprintf(pFile, " ereturn\n");
	  } break;

	  case OP_RETURN: {
		fprintf(pFile, " return\n");
	  } break;

	  case OP_NLOAD: {
		fprintf(pFile, " nload address_%u\n", pOpList->LoadArg.unAddress);
	  } break;

	  case OP_SLOAD: {
		fprintf(pFile, " sload address_%u\n", pOpList->LoadArg.unAddress);
	  } break;

	  case OP_ELOAD: {
		fprintf(pFile, " eload address_%u\n", pOpList->LoadArg.unAddress);
	  } break;

	  case OP_NSTORE: {
		fprintf(pFile, " nstore address_%u\n", pOpList->StoreArg.unAddress);
	  } break;

	  case OP_SSTORE: {
		fprintf(pFile, " sstore address_%u\n", pOpList->StoreArg.unAddress);
	  } break;

	  case OP_ESTORE: {
		fprintf(pFile, " estore address_%u\n", pOpList->StoreArg.unAddress);
	  } break;

	  case OP_NPUSH: {
		fprintf(pFile, " npush %f\n", pOpList->NPushArg.fValue);
	  } break;

	  case OP_SPUSH: {
		fprintf(pFile, " spush \"%s\"\n", pOpList->SPushArg.szValue);
	  } break;

	  case OP_EPUSH: {
		fprintf(pFile, " epush %u\n", pOpList->EPushArg.unValue);
	  } break;

	  case OP_NSCAST: {
		fprintf(pFile, " nscast\n");
	  } break;

	  case OP_SNCAST: {
		fprintf(pFile, " sncast\n");
	  } break;

	  case OP_CALL_FUNC: {
		sSymTableNode* pFuncDecl = SymTableGetNode(pSymTable, 
												   pOpList->CallFuncArg.szIdentifier);
		assert(pFuncDecl);
		fprintf(pFile, " call_func %s [Index: %u]\n", pOpList->CallFuncArg.szIdentifier, pFuncDecl->pIdFunc->FuncDecl.unFuncIdx);
	  } break;

	  case OP_LABEL: {
		assert(pOpList);
		assert(pLabelList + pOpList->LabelArg.unLabelValue);		
		fprintf(pFile, 
				" %s_%u:\n", 
				pLabelList[pOpList->LabelArg.unLabelValue].szLabelName,
				pOpList->LabelArg.unLabelValue);
	  } break;

	  case OP_API_PASSTORGBCOLOR: {
		fprintf(pFile, " api_pass_to_rgb_color\n");		
	  } break;
	  
	  case OP_API_GETREDCOMPONENT: {
		fprintf(pFile, " api_get_red_component\n");		
	  } break;
	  
	  case OP_API_GETGREENCOMPONENT: {
		fprintf(pFile, " api_get_green_component\n");		
	  } break;
	  
	  case OP_API_GETBLUECOMPONENT: {
		fprintf(pFile, " api_get_blue_component\n");		
	  } break;
	  
	  case OP_API_RAND: {
		fprintf(pFile, " api_rand\n");		
	  } break;
	  
	  case OP_API_GETINTEGERVALUE: {
		fprintf(pFile, " api_get_integer_value\n");		
	  } break;
	  
	  case OP_API_GETDECIMALVALUE: {
		fprintf(pFile, " api_get_decimal_value\n");		
	  } break;

	  case OP_API_GETSTRINGSIZE: {
		fprintf(pFile, " api_get_string_size\n");		
	  } break;
	  
	  case OP_API_WRITETOLOGGER: {
		fprintf(pFile, " api_write_to_logger\n");		
	  } break;
	  
	  case OP_API_ENABLECRISOLSCRIPTWARNINGS: {		
		fprintf(pFile, " api_enable_crisol_script_warnings\n");		
	  } break;
	  
	  case OP_API_DISABLECRISOLSCRIPTWARNINGS: {
		fprintf(pFile, " api_disable_crisol_script_warnings\n");		
	  } break;

	  case OP_API_SHOWFPS: {
		fprintf(pFile, " api_show_fps\n");		
	  } break;

	  case OP_API_WAIT: {
		fprintf(pFile, " api_wait\n");		
	  } break;
	  
	  case OP_GAMEOBJ_QUITGAME: {
		fprintf(pFile, " game.quit_game\n");		
	  } break;
	  
	  case OP_GAMEOBJ_WRITETOCONSOLE: {
		fprintf(pFile, " game.write_to_console\n");		
	  } break;
	  
	  case OP_GAMEOBJ_ACTIVEADVICEDIALOG: {
		fprintf(pFile, " game.active_advice_dialog\n");		
	  } break;
	  
	  case OP_GAMEOBJ_ACTIVEQUESTIONDIALOG: {
		fprintf(pFile, " game.active_question_dialog\n");		
	  } break;
	  
	  case OP_GAMEOBJ_ACTIVETEXTREADERDIALOG: {
		fprintf(pFile, " game.active_text_reader_dialog\n");
	  } break;
	  
	  case OP_GAMEOBJ_ADDOPTIONTOTEXTSELECTORDIALOG: {
		fprintf(pFile, " game.add_option_to_text_selector_dialog\n");
	  } break;
	  
	  case OP_GAMEOBJ_RESETOPTIONSINTEXTSELECTORDIALOG: {
		fprintf(pFile, " game.reset_options_in_text_selector_dialog\n");		
	  } break;
	  
	  case OP_GAMEOBJ_ACTIVETEXTSELECTORDIALOG: {
		fprintf(pFile, " game.active_text_selector_dialog\n");		
	  } break;
	  
	  case OP_GAMEOBJ_PLAYMIDIMUSIC: {
		fprintf(pFile, " game.play_midi_music\n");		
	  } break;
	  
	  case OP_GAMEOBJ_STOPMIDIMUSIC: {
		fprintf(pFile, " game.stop_midi_music\n");
	  } break;
	  
	  case OP_GAMEOBJ_PLAYWAVAMBIENTSOUND: {
		fprintf(pFile, " game.play_wav_ambient_sound\n");
	  } break;
	  
	  case OP_GAMEOBJ_STOPWAVAMBIENTSOUND: {
		fprintf(pFile, " game.stop_wav_ambient_sound\n");
	  } break;
	  
	  case OP_GAMEOBJ_ACTIVETRADEITEMSINTERFAZ: {
		fprintf(pFile, " game.active_trade_items_interfaz\n");
	  } break;
	  
	  case OP_GAMEOBJ_ADDOPTIONTOCONVERSATORINTERFAZ: {
		fprintf(pFile, " game.add_option_to_conversator_interfaz\n");
	  } break;
	  
	  case OP_GAMEOBJ_RESETOPTIONSINCONVERSATORINTERFAZ: {
		fprintf(pFile, " game.reset_options_in_conversator_interfaz\n");
	  } break;
	  
	  case OP_GAMEOBJ_ACTIVECONVERSATORINTERFAZ: {
		fprintf(pFile, " game.active_conversator_interfaz\n");
	  } break;

	  case OP_GAMEOBJ_DESACTIVECONVERSATORINTERFAZ: {
		fprintf(pFile, " game.desactive_conversator_interfaz\n");
	  } break;

	  case OP_GAMEOBJ_GETOPTIONFROMCONVERSATORINTERFAZ: {
		fprintf(pFile, " game.get_option_from_conversator_interfaz\n");
	  } break;
	  
	  case OP_GAMEOBJ_SHOWPRESENTATION: {
		fprintf(pFile, " game.show_presentation\n");
	  } break;
	  
	  case OP_GAMEOBJ_BEGINCUTSCENE: {
		fprintf(pFile, " game.begin_cutscene\n");
	  } break;
	  
	  case OP_GAMEOBJ_ENDCUTSCENE: {
		fprintf(pFile, " game.end_cutscene\n");
	  } break;

	  case OP_GAMEOBJ_SETSCRIPT: {
		fprintf(pFile, " game.set_script\n");
	  } break;

	  case OP_GAMEOBJ_ISKEYPRESSED: {
		fprintf(pFile, " game.is_key_pressed\n");
	  } break;
	  
	  case OP_WORLDOBJ_GETAREANAME: {
		fprintf(pFile, " world.get_area_name\n");
	  } break;

	  case OP_WORLDOBJ_GETAREAID: {
		fprintf(pFile, " world.get_area_id\n");
	  } break;
	  
	  case OP_WORLDOBJ_GETAREAWIDTH: {
		fprintf(pFile, " world.get_area_width\n");
	  } break;
	  
	  case OP_WORLDOBJ_GETAREAHEIGHT: {
		fprintf(pFile, " world.get_area_height\n");
	  } break;
	  
	  case OP_WORLDOBJ_GETHOUR: {
		fprintf(pFile, " world.get_hour\n");
	  } break;
	  
	  case OP_WORLDOBJ_GETMINUTE: {
		fprintf(pFile, " world.get_minute\n");
	  } break;
	  
	  case OP_WORLDOBJ_SETHOUR: {
		fprintf(pFile, " world.set_hour\n");
	  } break;
	  
	  case OP_WORLDOBJ_SETMINUTE: {
		fprintf(pFile, " world.set_minute\n");
	  } break;
	  
	  case OP_WORLDOBJ_GETENTITY: {
		fprintf(pFile, " world.get_entity\n");
	  } break;
	  
	  case OP_WORLDOBJ_GETPLAYER: {
		fprintf(pFile, " world.get_player\n");
	  } break;
	  
	  case OP_WORLDOBJ_ISFLOORVALID: {
		fprintf(pFile, " world.is_floor_valid\n");
	  } break;
	  
	  case OP_WORLDOBJ_GETITEMAT: {
		fprintf(pFile, " world.get_item_at\n");
	  } break;
	  
	  case OP_WORLDOBJ_GETNUMITEMSAT: {
		fprintf(pFile, " world.get_num_items_at\n");
	  } break;		
	  
	  case OP_WORLDOBJ_GETDISTANCE: {
		fprintf(pFile, " world.get_distance\n");
	  } break;
	  
	  case OP_WORLDOBJ_CALCULEPATHLENGHT: {
		fprintf(pFile, " world.calcule_path_lenght\n");
	  } break;
	  
	  case OP_WORLDOBJ_LOADAREA: {
		fprintf(pFile, " world.load_area\n");
	  } break;
	  	  
	  case OP_WORLDOBJ_CHANGEENTITYLOCATION: {
		fprintf(pFile, " world.change_entity_location\n");
	  } break;
	  
	  case OP_WORLDOBJ_ATTACHCAMERATOENTITY: {
		fprintf(pFile, " world.attach_camera_to_entity\n");
	  } break;
	  
	  case OP_WORLDOBJ_ATTACHCAMERATOLOCATION: {
		fprintf(pFile, " world.attach_camera_to_location\n");
	  } break;
	  
	  case OP_WORLDOBJ_ISCOMBATMODEACTIVE: {
		fprintf(pFile, " world.is_combat_mode_active\n");
	  } break;

	  case OP_WORLDOBJ_ENDCOMBAT: {
		fprintf(pFile, " world.end_combat\n");
	  } break;

	  case OP_WORLDOBJ_GETCRIATUREINCOMBATTURN: {
		fprintf(pFile, " world.get_criature_in_combat_turn\n");
	  } break;
  
	  case OP_WORLDOBJ_GETCOMBATANT: {
		fprintf(pFile, " world.get_combatant\n");
	  } break;

	  case OP_WORLDOBJ_GETNUMBEROFCOMBATANTS: {
		fprintf(pFile, " world.get_number_of_combatants\n");
	  } break;
	  
	  case OP_WORLDOBJ_GETAREALIGHTMODEL: {
		fprintf(pFile, " world.get_area_light_model\n");
	  } break;
	  
	  case OP_WORLDOBJ_SETIDLESCRIPTTIME: {
		fprintf(pFile, " world.set_idle_script_time\n");
	  } break;

	  case OP_WORLDOBJ_SETSCRIPT: {
		fprintf(pFile, " world.set_script\n");
	  } break;

	  case OP_WORLDOBJ_DESTROYENTITY: {
		fprintf(pFile, " world.destroy_entity\n");
	  } break;
  
	  case OP_WORLDOBJ_CREATECRIATURE: {
		fprintf(pFile, " world.create_criature\n");
	  } break;
	  
	  case OP_WORLDOBJ_CREATEWALL: {
		fprintf(pFile, " world.create_wall\n");
	  } break;

	  case OP_WORLDOBJ_CREATESCENARYOBJECT: {
		fprintf(pFile, " world.create_scenary_object\n");
	  } break;

	  case OP_WORLDOBJ_CREATEITEMABANDONED: {
		fprintf(pFile, " world.create_item_abandoned\n");
	  } break;
  
	  case OP_WORLDOBJ_CREATEITEMWITHOWNER: {
		fprintf(pFile, " world.create_item_with_owner\n");
	  } break;

	  case OP_WORLDOBJ_SETWORLDTIMEPAUSE: {
		fprintf(pFile, " world.set_world_time_pause\n");
	  } break;

	  case OP_WORLDOBJ_ISWORLDTIMEINPAUSE: {
		fprintf(pFile, " world.is_world_time_in_pause\n");
	  } break;

	  case OP_WORLDOBJ_SETELEVATIONAT: {
		fprintf(pFile, " world.set_elevation_at\n");
	  } break;

	  case OP_WORLDOBJ_GETELEVATIONAT: {
		fprintf(pFile, " world.get_elevation_at\n");
	  } break;

	  case OP_WORLDOBJ_NEXTTURN: {
		fprintf(pFile, " world.next_turn\n");
	  } break;

	  case OP_WORLDOBJ_GETLIGHTAT: {
		fprintf(pFile, " world.get_light_at\n");
	  } break;

	  case OP_WORLDOBJ_PLAYWAVSOUND: {
		fprintf(pFile, " world.play_wav_sound\n");
	  } break;
	  
	  case OP_WORLDOBJ_SETSCRIPTAT: {
		fprintf(pFile, " world.set_script_at\n");
	  } break;
	  
	  case OP_ENTITYOBJ_GETNAME: {
		fprintf(pFile, " entity.get_name\n");
	  } break;

	  case OP_ENTITYOBJ_SETNAME: {
		fprintf(pFile, " entity.set_name\n");
	  } break;
	  
	  case OP_ENTITYOBJ_GETENTITYTYPE: {
		fprintf(pFile, " entity.get_entity_type\n");
	  } break;
	  
	  case OP_ENTITYOBJ_GETTYPE: {
		fprintf(pFile, " entity.get_type\n");
	  } break;
	  
	  case OP_ENTITYOBJ_SAY: {
		fprintf(pFile, " entity.say\n");
	  } break;
	  
	  case OP_ENTITYOBJ_SHUTUP: {
		fprintf(pFile, " entity.shut_up\n");
	  } break;
	  
	  case OP_ENTITYOBJ_ISSAYING: {
		fprintf(pFile, " entity.is_saying\n");
	  } break;
	  
	  case OP_ENTITYOBJ_ATTACHGFX: {
		fprintf(pFile, " entity.attach_gfx\n");
	  } break;
	  
	  case OP_ENTITYOBJ_RELEASEGFX: {
		fprintf(pFile, " entity.release_gfx\n");
	  } break;
	  
	  case OP_ENTITYOBJ_RELEASEALLGFX: {
		fprintf(pFile, " entity.release_all_gfx\n");
	  } break;
	  
	  case OP_ENTITYOBJ_ISGFXATTACHED: {
		fprintf(pFile, " entity.is_gfx_attached\n");
	  } break;
	  
	  case OP_ENTITYOBJ_GETNUMITEMSINCONTAINER: {
		fprintf(pFile, " entity.get_num_items_in_container\n");
	  } break;
	  
	  case OP_ENTITYOBJ_ISITEMINCONTAINER: {
		fprintf(pFile, " entity.is_item_in_container\n");
	  } break;

	  case OP_ENTITYOBJ_GETITEMFROMCONTAINER: {
		fprintf(pFile, " entity.get_item_from_container\n");
	  } break;				
	  
	  case OP_ENTITYOBJ_TRANSFERITEMTOCONTAINER: {
		fprintf(pFile, " entity.transfer_item_to_container\n");
	  } break;
	  
	  case OP_ENTITYOBJ_INSERTITEMINCONTAINER: {
		fprintf(pFile, " entity.insert_item_in_container\n");
	  } break;
	  
	  case OP_ENTITYOBJ_REMOVEITEMOFCONTAINER: {
		fprintf(pFile, " entity.remove_item_of_container\n");
	  } break;
	  
	  case OP_ENTITYOBJ_SETANIMTEMPLATESTATE: {
		fprintf(pFile, " entity.set_anim_template_state\n");
	  } break;
	  
	  case OP_ENTITYOBJ_SETPORTRAITANIMTEMPLATESTATE: {
		fprintf(pFile, " entity.set_protrait_anim_template_state\n");
	  } break;	  
	  
	  case OP_ENTITYOBJ_SETIDLESCRIPTTIME: {
		fprintf(pFile, " entity.set_idle_script_time\n");
	  } break;

	  case OP_ENTITYOBJ_SETLIGHT: {
		fprintf(pFile, " entity.set_light\n");
	  } break;
	
	  case OP_ENTITYOBJ_GETLIGHT: {
		fprintf(pFile, " entity.get_light\n");
	  } break;

	  case OP_ENTITYOBJ_GETXPOS: {
		fprintf(pFile, " entity.get_x_pos\n");
	  } break;
	
	  case OP_ENTITYOBJ_GETYPOS: {
		fprintf(pFile, " entity.get_y_pos\n");
	  } break;

	  case OP_ENTITYOBJ_GETELEVATION: {
		fprintf(pFile, " entity.get_elevation\n");
	  } break;

	  case OP_ENTITYOBJ_SETELEVATION: {
		fprintf(pFile, " entity.set_elevation\n");
	  } break;
	  
	  case OP_ENTITYOBJ_GETLOCALATTRIBUTE: {
		fprintf(pFile, " entity.get_local_attribute\n");
	  } break;

	  case OP_ENTITYOBJ_SETLOCALATTRIBUTE: {
		fprintf(pFile, " entity.set_local_attribute\n");
	  } break;
	  
	  case OP_ENTITYOBJ_GETOWNER: {
		fprintf(pFile, " entity.get_owner\n");
	  } break;
	  
	  case OP_ENTITYOBJ_GETCLASS: {
		fprintf(pFile, " entity.get_class\n");
	  } break;

	  case OP_ENTITYOBJ_GETINCOMBATUSECOST: {
		fprintf(pFile, " entity.get_in_combat_use_cost\n");
	  } break;

	  case OP_ENTITYOBJ_GETGLOBALATTRIBUTE: {
		fprintf(pFile, " entity.get_global_attribute\n");
	  } break;

	  case OP_ENTITYOBJ_SETGLOBALATTRIBUTE: {
		fprintf(pFile, " entity.set_global_attribute\n");
	  } break;
	  
	  case OP_ENTITYOBJ_GETWALLORIENTATION: {
		fprintf(pFile, " entity.get_wall_orientation\n");
	  } break;
	  
	  case OP_ENTITYOBJ_BLOCKACCESS: {
		fprintf(pFile, " entity.block_access\n");
	  } break;
	  
	  case OP_ENTITYOBJ_UNBLOCKACCESS: {
		fprintf(pFile, " entity.unblock_access\n");
	  } break;
	  
	  case OP_ENTITYOBJ_ISACCESSBLOCKED: {
		fprintf(pFile, " entity.is_access_blocked\n");
	  } break;
	  
	  case OP_ENTITYOBJ_SETSYMPTOM: {
		fprintf(pFile, " entity.set_symtom\n");
	  } break;
	  
	  case OP_ENTITYOBJ_ISSYMPTOMACTIVE: {
		fprintf(pFile, " entity.is_symptom_active\n");
	  } break;
	  
	  case OP_ENTITYOBJ_GETGENRE: {
		fprintf(pFile, " entity.get_genre\n");
	  } break;
	  
	  case OP_ENTITYOBJ_GETHEALTH: {
		fprintf(pFile, " entity.get_health\n");
	  } break;
	  
	  case OP_ENTITYOBJ_SETHEALTH: {
		fprintf(pFile, " entity.set_health\n");
	  } break;
	  
	  case OP_ENTITYOBJ_GETEXTENDEDATTRIBUTE: {
		fprintf(pFile, " entity.get_extended_attribute\n");
	  } break;
	  
	  case OP_ENTITYOBJ_SETEXTENDEDATTRIBUTE: {
		fprintf(pFile, " entity.set_extended_attribute\n");
	  } break;
	  
	  case OP_ENTITYOBJ_GETLEVEL: {
		fprintf(pFile, " entity.get_level\n");
	  } break;
	  
	  case OP_ENTITYOBJ_SETLEVEL: {
		fprintf(pFile, " entity.set_level\n");
	  } break;
	  
	  case OP_ENTITYOBJ_GETEXPERIENCE: {
		fprintf(pFile, " entity.get_experience\n");
	  } break;
	  
	  case OP_ENTITYOBJ_SETEXPERIENCE: {
		fprintf(pFile, " entity.set_experience\n");
	  } break;
	  
	  case OP_ENTITYOBJ_GETINCOMBATACTIONPOINTS: {
		fprintf(pFile, " entity.get_in_combat_action_points\n");
	  } break;
	  
	  case OP_ENTITYOBJ_GETACTIONPOINTS: {
		fprintf(pFile, " entity.get_action_points\n");
	  } break;
	  
	  case OP_ENTITYOBJ_SETACTIONPOINTS: {
		fprintf(pFile, " entity.set_action_points\n");
	  } break;
	  
	  case OP_ENTITYOBJ_ISHABILITYACTIVE: {
		fprintf(pFile, " entity.is_hability_active\n");
	  } break;
	  
	  case OP_ENTITYOBJ_SETHABILITY: {
		fprintf(pFile, " entity.set_hability\n");
	  } break;
	  
	  case OP_ENTITYOBJ_USEHABILITY: {
		fprintf(pFile, " entity.use_hability\n");
	  } break;
	  
	  case OP_ENTITYOBJ_ISRUNMODEACTIVE: {
		fprintf(pFile, " entity.is_run_mode_active\n");
	  } break;
	  
	  case OP_ENTITYOBJ_SETRUNMODE: {
		fprintf(pFile, " entity.set_run_mode\n");
	  } break;
	  
	  case OP_ENTITYOBJ_MOVETO: {
		fprintf(pFile, " entity.move_to\n");
	  } break;
	  
	  case OP_ENTITYOBJ_ISMOVING: {
		fprintf(pFile, " entity.is_moving\n");
	  } break;
	  
	  case OP_ENTITYOBJ_STOPMOVING: {
		fprintf(pFile, " entity.stop_moving\n");
	  } break;
	  
	  case OP_ENTITYOBJ_EQUIPITEM: {
		fprintf(pFile, " entity.equip_item\n");
	  } break;
	  
	  case OP_ENTITYOBJ_REMOVEITEMEQUIPPED: {
		fprintf(pFile, " entity.remove_item_equipped\n");
	  } break;
	  
	  case OP_ENTITYOBJ_GETITEMEQUIPPED: {
		fprintf(pFile, " entity.get_item_equipped\n");
	  } break;
	  
	  case OP_ENTITYOBJ_ISITEMEQUIPPED: {
		fprintf(pFile, " entity.is_item_equipped\n");
	  } break;
	  
	  case OP_ENTITYOBJ_DROPITEM: {
		fprintf(pFile, " entity.drop_item\n");
	  } break;
	  
	  case OP_ENTITYOBJ_USEITEM: {
		fprintf(pFile, " entity.use_item\n");
	  } break;
	  
	  case OP_ENTITYOBJ_MANIPULATE: {
		fprintf(pFile, " entity.manipulate\n");
	  } break;
	  
	  case OP_ENTITYOBJ_SETTRANSPARENTMODE: {
		fprintf(pFile, " entity.set_transparent_mode\n");
	  } break;
	  
	  case OP_ENTITYOBJ_ISTRANSPARENTMODEACTIVE: {
		fprintf(pFile, " entity.is_transparent_mode_active\n");
	  } break;
	  
	  case OP_ENTITYOBJ_CHANGEANIMORIENTATION: {
		fprintf(pFile, " entity.change_anim_location\n");
	  } break;

	  case OP_ENTITYOBJ_GETANIMORIENTATION: {
		fprintf(pFile, " entity.get_anim_location\n");
	  } break;
	  
	  case OP_ENTITYOBJ_SETALINGMENT: {
		fprintf(pFile, " entity.set_alingment\n");
	  } break;
	  
	  case OP_ENTITYOBJ_SETALINGMENTWITH: {
		fprintf(pFile, " entity.set_alingment_with\n");
	  } break;
	  
	  case OP_ENTITYOBJ_SETALINGMENTAGAINST: {
		fprintf(pFile, " entity.set_alingment_againts\n");
	  } break;
	  
	  case OP_ENTITYOBJ_GETALINGMENT: {
		fprintf(pFile, " entity.get_alingment\n");
	  } break;
	  
	  case OP_ENTITYOBJ_HITENTITY: {
		fprintf(pFile, " entity.hit_entity\n");
	  } break;

	  case OP_ENTITYOBJ_SETSCRIPT: {
		fprintf(pFile, " entity.set_script\n");
	  } break;

	  case OP_ENTITYOBJ_ISGHOSTMOVEMODEACTIVE: {
		fprintf(pFile, " entity.is_ghost_move_mode_active\n");
	  } break;
	  
	  case OP_ENTITYOBJ_SETGHOSTMOVEMODE: {
		fprintf(pFile, " entity.set_ghost_move_mode\n");
	  } break;

	  case OP_ENTITYOBJ_GETRANGE: {
		fprintf(pFile, " entity.get_range\n");
	  } break;

	  case OP_ENTITYOBJ_ISINRANGE: {
		fprintf(pFile, " entity.is_in_range\n");
	  } break;

	  default: {
		fprintf(pFile, " ¿? opcode desconocido, código: %u\n", pOpList->OpcodeType);
	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Pasa el tipo de un script a una cadena sbyte.
// Parametros:
// - pType. Tipo de un script
// Devuelve:
// - La cadena con el nombre del script
// Notas:
///////////////////////////////////////////////////////////////////////////////
sbyte* 
PassScriptTypeToString(sScriptType* pType)
{
  assert(pType);
  switch(pType->ScriptEventType) {
	case SCRIPTEVENT_ONSTARTGAME: {
	  return "on_start_game (V)N";
	} break;

	case SCRIPTEVENT_ONCLICKHOURPANEL: {
	  return "on_click_hour_panel (V)N";
	} break;

	case SCRIPTEVENT_ONFLEECOMBAT: {
	  return "on_flee_combat (V)N";
	} break;

	case SCRIPTEVENT_ONKEYPRESSED: {
	  return "on_key_pressed (V)N";
	} break;

	case SCRIPTEVENT_ONSTARTCOMBATMODE: {
	  return "on_start_combat_mode (V)N";
	} break;

	case SCRIPTEVENT_ONENDCOMBATMODE: {
	  return "on_start_combat_mode (V)N";
	} break;

	case SCRIPTEVENT_ONNEWHOUR: {
	  return "on_new_hour (N)N";
	} break;

	case SCRIPTEVENT_ONENTERINAREA: {
	  return "on_enter_in_area (N)N";
	} break;

	case SCRIPTEVENT_ONWORLDIDLE: {
	  return "on_world_idle (V)N";
	} break;

	case SCRIPTEVENT_ONSETINFLOOR: {
	  return "on_set_in_floor (NNE)N";
	} break;

	case SCRIPTEVENT_ONSETOUTOFFLOOR: {
	  return "on_set_out_of_floor (NNE)N";
	} break;

	case SCRIPTEVENT_ONGETITEM: {
	  return "on_get_item (EE)N";
	} break;

	case SCRIPTEVENT_ONDROPITEM: {
	  return "on_drop_item (EEN)N";
	} break;

	case SCRIPTEVENT_ONOBSERVEENTITY: {
	  return "on_observe_entity (EE)N";
	} break;

	case SCRIPTEVENT_ONTALKTOENTITY: {
	  return "on_talk_to_entity (EE)N";
	} break;

	case SCRIPTEVENT_ONMANIPULATEENTITY: {
	  return "on_manipulate_entity (EE)N";
	} break;

	case SCRIPTEVENT_ONDEATH: {
	  return "on_death (E)N";
	} break;

	case SCRIPTEVENT_ONRESURRECT: {
	  return "on_resurrect (E)N";
	} break;

	case SCRIPTEVENT_ONINSERTINEQUIPMENTSLOT: {
	  return "on_insert_in_equipment_slot (EEN)N";
	} break;

	case SCRIPTEVENT_ONREMOVEFROMEQUIPMENTSLOT: {
	  return "on_remove_from_equipment_slot (EEN)N";
	} break;

	case SCRIPTEVENT_ONUSEHABILITY: {
	  return "on_use_hability (ENE)N";
	} break;

	case SCRIPTEVENT_ONACTIVATEDSYMPTOM: {
	  return "on_activated_symptom (EN)N";
	} break;

	case SCRIPTEVENT_ONDEACTIVATEDSYMPTOM: {
	  return "on_deactivated_symptom (EN)N";
	} break;

	case SCRIPTEVENT_ONHITENTITY: {
	  return "on_hit_entity (ENE)N";
	} break;

	case SCRIPTEVENT_ONSTARTCOMBATTURN: {
	  return "on_start_combat_turn (E)N";
	} break;

	case SCRIPTEVENT_ONCRIATUREINRANGE: {
	  return "on_criature_in_range (EE)N";
	} break;

	case SCRIPTEVENT_ONCRIATUREOUTOFRANGE: {
	  return "on_criature_out_of_range (EE)N";
	} break;

	case SCRIPTEVENT_ONENTITYIDLE: {
	  return "on_entity_idle (E)N";
	} break;

	case SCRIPTEVENT_ONUSEITEM: {
	  return "on_use_item (EEE)N";
	} break;

	case SCRIPTEVENT_ONTRADEITEM: {
	  return "on_trade_item (EEE)N";
	} break;

	case SCRIPTEVENT_ONENTITYCREATED: {
	  return "on_entity_created (E)N";
	} break;

	default:
	  assert(0);
  }; // ~ switch

  return "script type ¿?";
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comienza el recorrido del arbol AST, abriendo el archivo donde se
//   produciran las emisiones de los opcodes y emitiendo el codigo asociado
//   al archivo global.
// Parametros:
// - pGlobal. Puntero al AST.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeEmitGlobal(sGlobal* pGlobal,
				 sbyte* szEmitFileName)
{
  if (pGlobal) {
	// Abre el fichero donde emitir el codigo
	FILE* pFile = fopen(szEmitFileName, "w");
	assert(pFile);

	// Emite los opcodes del script global
	fprintf(pFile, ".global %s\n", pGlobal->szFileName);
	EmitOpcodeList(pGlobal->pOpcodeList, NULL, NULL, pFile);	
	fprintf(pFile, ".end_global\n");

	// Recorre lista de scripts
	OpcodeEmitScript(pGlobal->pScript, pFile);

	// Cierra archivo
	fclose(pFile);
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Emite el codigo asociado al script
// Parametros:
// - pScript. Script a emitir.
// - pFile. Fichero donde emitir
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeEmitScript(sScript* pScript,
				 FILE* pFile)
{
  if (pScript) {
		switch(pScript->ScriptType) {
	  case SCRIPT_SEQ: {
		// Recorrido
		OpcodeEmitScript(pScript->ScriptSeq.pFirst, pFile);
		OpcodeEmitScript(pScript->ScriptSeq.pSecond, pFile);
	  } break;

	  case SCRIPT_DECL: {				
		assert(pFile);
		// Se emite el cuerpo del script
		fprintf(pFile, "\n.script %s\n", pScript->ScriptDecl.szFileName);
		fprintf(pFile, ".event %s [Index: 0]\n", PassScriptTypeToString(pScript->ScriptDecl.pType));
  	    fprintf(pFile, ".vars %d\n", pScript->ScriptDecl.unNumOffsets);
		if (pScript->ScriptDecl.unNumOffsets) {
		  fprintf(pFile, ".first_offset %d\n", pScript->ScriptDecl.unFirstOffset);
		}		  
		fprintf(pFile, ".max_stack_size %d\n", pScript->ScriptDecl.unStackSize);
		EmitOpcodeList(pScript->ScriptDecl.pOpcodeList,
					   pScript->ScriptDecl.pLabelList,
					   pScript->ScriptDecl.pSymTable,
					   pFile);

		// Se emiten los ficheros import
		fprintf(pFile, "\n.import_functions\n");
		OpcodeEmitImport(pScript->ScriptDecl.pImport, 
						 pScript->ScriptDecl.pSymTable,
					     pFile);		
		fprintf(pFile, ".end_import_functions\n");

		// Se emiten las funciones locales
		fprintf(pFile, "\n.local_functions\n");
		OpcodeEmitFunc(pScript->ScriptDecl.pFunc, 
					   pScript->ScriptDecl.pSymTable,
					   pFile);
		if (pScript->ScriptDecl.pFunc) {
		  fprintf(pFile, "\n");
		}
		fprintf(pFile, ".end_locals_functions\n");

		// Fin script
		fprintf(pFile, "\n.end_script\n");
	  } break;	
	}; // ~ switch	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Recorre las funciones import para imprimirlas.
// Parametros:
// - pImport. Enlace a secuencia import.
// - pScriptSymTable. Enlace a la tabla de simbolos del script.
// - pFile. Fichero en donde imprimir.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeEmitImport(sImport* pImport,
 			     sSymbolTable* pScriptSymTable,
				 FILE* pFile)
{
  if (pImport) {
	switch(pImport->ImportType) {
	  case IMPORT_SEQ: {
		OpcodeEmitImport(pImport->ImportSeq.pFirst, pScriptSymTable, pFile);
		OpcodeEmitImport(pImport->ImportSeq.pSecond, pScriptSymTable, pFile);
	  } break;

	  case IMPORT_FUNC: {
		OpcodeEmitFunc(pImport->ImportFunc.pFunctions, pScriptSymTable, pFile);
  	  } break;
	}; // ~ switch
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Emite los opcodes asociados a funciones.
// Parametros:
// - pFunc. Enlace a la estructura con la funcion.
// - pScriptSymTable. Enlace a la tabla de simbolos del script.
// - pFile. Archivo en donde imprimir.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
OpcodeEmitFunc(sFunc* pFunc,
			   sSymbolTable* pScriptSymTable,
			   FILE* pFile)
{
    if (pFunc) {
	switch (pFunc->eFuncType) {
	  case FUNC_SEQ: {
		// Recorrido
		OpcodeEmitFunc(pFunc->FuncSeq.pFirst, pScriptSymTable, pFile);
		OpcodeEmitFunc(pFunc->FuncSeq.pSecond, pScriptSymTable, pFile);
	  } break;

	  case FUNC_DECL: {
		// ¿Fue invocada?
		if (pFunc->FuncDecl.unWasInvoked) {		
		  fprintf(pFile, "\n.function %s %s [Index: %u]\n", pFunc->FuncDecl.szIdentifier, pFunc->FuncDecl.szSignature, pFunc->FuncDecl.unFuncIdx);
		  fprintf(pFile, ".vars %d\n", pFunc->FuncDecl.unNumOffsets);
		  if (pFunc->FuncDecl.unNumOffsets) {
			fprintf(pFile, ".first_offset %d\n", pFunc->FuncDecl.unFirstOffset);
		  }
		  fprintf(pFile, ".max_stack_size %d\n", pFunc->FuncDecl.unStackSize);
		  EmitOpcodeList(pFunc->FuncDecl.pOpcodeList,
						 pFunc->FuncDecl.pLabelList,
						 pScriptSymTable,
						 pFile);
		  fprintf(pFile, ".end_function\n");
		}
	  } break;
	}; // ~ switch
  }  
}
