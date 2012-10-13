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
// iCGameDataBase.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Interfaces:
// - iCGameDataBase
//
// Descripcion:
// - Interfaz para CGameDataBase.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _ICGAMEDATABASE_H_
#define _ICGAMEDATABASE_H_

// Cabeceras
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif
#ifndef _RULESDEFS_H_
#include "RulesDefs.h"
#endif
#ifndef _FILEDEFS_H_
#include "FileDefs.h"
#endif
#ifndef _GAMEDATABASEDEFS_H_
#include "GameDataBaseDefs.h"
#endif
#ifndef _ICRULESDATABASE_H_
#include "iCRulesDataBase.h"
#endif
#ifndef _STRING_H_
#define _STRING_H_
#include <string>
#endif

// Defincion de clases / estructuras / espacios de nombres
class CCBTEngineParser;

// Interfaz iCGameDataBase
struct iCGameDataBase
{
public:
  // Obtencion de parsers a archivos de datos
  virtual CCBTEngineParser* const GetCBTParser(const GameDataBaseDefs::eCBTFile& File,
											   const std::string& szSection) = 0;
  virtual FileDefs::FileHandle GetCBBFileHandle(const GameDataBaseDefs::eCBBFile& CBBFile) = 0;
  virtual dword GetCBBFileOffset(const GameDataBaseDefs::eCBBFile& CBBFile,
						         const std::string& szSection) = 0;
  virtual bool IsValidCBBFileSection(const GameDataBaseDefs::eCBBFile& CBBFile,
									 const std::string& szSection) = 0;
  virtual std::string GetAreaFilesPath(void) const = 0;

public:
  // Obtencion de info sobre fichero script
  virtual FileDefs::FileHandle GetScriptsFileHandle(void) const = 0;
  virtual std::streampos GetGlobalScriptOffset(void) const = 0;
  virtual std::streampos GetEventScriptOffset(const std::string& szScriptFile) = 0;
  virtual bool IsValidEventScriptFile(const std::string& szScriptFile) = 0;

public:
  // Obtencion de la base de datos de reglas
  virtual iCRulesDataBase* const GetRulesDataBase(void) = 0;

public:
  // Obtencion de texto estatico
  virtual std::string GetStaticText(const GameDataBaseDefs::eStaticText& StaticText,
						            const word uwIdxMain = 0,
									const word uwIdxAux = 0) = 0;

public:
  // Obtencion del nombre del fichero con la TileRegionMask
  virtual std::string GetTileRegionMaskFileName(void) const = 0;
};

#endif // ~ #ifdef _ICGAMEDATABASE_H_
