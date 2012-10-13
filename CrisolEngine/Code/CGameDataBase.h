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
// CGameDataBase.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CGameDataBase
//
// Descripcion:
// - Modulo encargado de actuar como base de datos general del motor. 
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CGAMEDATABASE_H_
#define _CGAMEDATABASE_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _ICGAMEDATABASE_H_
#include "iCGameDataBase.h"
#endif
#ifndef _ICFILESYSTEM_H
#include "iCFileSystem.h"
#endif
#ifndef _CCBTENGINEPARSER_H_
#include "CCBTEngineParser.h"
#endif
#ifndef _CRULESDATABASE_H_
#include "CRulesDataBase.h"
#endif
#ifndef _MAP_H_
#define _MAP_H_
#include <map>
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CGameDataBase
class CGameDataBase: public iCGameDataBase		
{
private:
  // Tipos
  // Map para almacenar pares string, offset en archivos
  typedef std::map<std::string, std::streampos> FileOffsetIndexMap;
  typedef FileOffsetIndexMap::iterator          FileOffsetIndexMapIt;
  typedef FileOffsetIndexMap::value_type        FileOffsetIndexMapValType;
  
private:
  // Estructuras
  struct sCBBFileInfo {
	// Info asociada a los ficheros CBB
	FileOffsetIndexMap   SectionMap; // Map con la relacion secciones / offsets
	FileDefs::FileHandle hFile;      // Handle al fichero
  };

  struct sScriptFileInfo {
	// Info asociada al fichero script
	FileOffsetIndexMap   ScriptsMap;         // Map con relacion script / offsets
	std::streampos       GlobalScriptOffset; // Offset al script global
	FileDefs::FileHandle hFile;              // Handle al fichero script
  };

private:
  // Instancia Singlenton
  static CGameDataBase* m_pGameDataBase; // Unica instancia a la clase

  // Interfaz a otros subsistemas
  iCFileSystem* m_pFileSys; // Subsistema de ficheros

  // Resto de vbles de miembro
  CRulesDataBase m_RulesDataBase; // Base de datos de las reglas 
  
  // Info de ficheros  
  sCBBFileInfo     m_CBBFiles[GameDataBaseDefs::CBBF_MAXFILES]; // Fich. CBB
  CCBTEngineParser m_CBTFiles[GameDataBaseDefs::CBTF_MAXFILES]; // Fich. CBT
  sScriptFileInfo  m_ScriptFile;                                // Scripts
  std::string      m_szAreaFilesPath;                           // Path a areas
  std::string      m_szTileRegionMaskFileName;                  // Fich. TileRegionMask

  // Resto de vbles  
  bool m_bIsInitOk; // ¿Instancia inicializada correctamente?
   
protected:
  // Constructor / Destructor
  CGameDataBase(void): m_bIsInitOk(false),
					   m_pFileSys(NULL) { }
public:
  ~CGameDataBase(void) { 
    End(); 
  }
  
public:
   // Obtencion y destruccion de la instancia singlenton
  static inline CGameDataBase* const GetInstance(void) {
    if (NULL == m_pGameDataBase) { 
      // Se crea la instancia
      m_pGameDataBase = new CGameDataBase; 
      ASSERT(m_pGameDataBase)
    }
    return m_pGameDataBase;
  }
  static inline void DestroyInstance(void) {
    if (m_pGameDataBase) {
      delete m_pGameDataBase;
      m_pGameDataBase = NULL;
    }
  }

public:
  // Protocolo de inicio y fin de instancia
  bool Init(const std::string& szInitFileName);
  void End(void);
  inline bool IsInitOk(void) const { return m_bIsInitOk; }
private:
  // Metodos de apoyo
  bool InitCBBFileInfo(const GameDataBaseDefs::eCBBFile& CBBFile,
					   const std::string& szFileInfo);
  bool InitCBTFileInfo(const GameDataBaseDefs::eCBTFile& CBTFile,
					   const std::string& szFileInfo);
  bool InitScriptFileInfo(const std::string& szFileName);
  void EndScriptFileInfo(void);
  void EndCBBFilesInfo(void);
  void EndCBTFilesInfo(void);

public:
  // iCGameDataBase / Obtencion de parsers a archivos de datos / Info de lectura de archivos
  CCBTEngineParser* const GetCBTParser(const GameDataBaseDefs::eCBTFile& File,
									   const std::string& szSection); 
  FileDefs::FileHandle GetCBBFileHandle(const GameDataBaseDefs::eCBBFile& CBBFile);  
  dword GetCBBFileOffset(const GameDataBaseDefs::eCBBFile& CBBFile,
			             const std::string& szSection);  
  bool IsValidCBBFileSection(const GameDataBaseDefs::eCBBFile& CBBFile,
							 const std::string& szSection);
  std::string GetAreaFilesPath(void) const {
	ASSERT(IsInitOk());
	// Retorna el path
	return m_szAreaFilesPath;
  }

public:
  // iCGameDataBase / Obtencion de info sobre fichero script
  std::streampos GetEventScriptOffset(const std::string& szScriptFile);  
  bool IsValidEventScriptFile(const std::string& szScriptFile) {
	ASSERT(IsInitOk());
	// Bastara con comprobar el offset devuelto, si es 0 no existe
	return (0 != GetEventScriptOffset(szScriptFile));
  }
  FileDefs::FileHandle GetScriptsFileHandle(void) const {
	ASSERT(IsInitOk());
	// Nota: Desde el exterior se debera de sobreenteder que en caso de retornar
	// un valor nulo (0), no se habra encontrado fichero de scripts
	return m_ScriptFile.hFile;
  }
  std::streampos GetGlobalScriptOffset(void) const {
	ASSERT(IsInitOk());
	// Retorna offset (0 si no existiera archivo script)
	return m_ScriptFile.GlobalScriptOffset;
  }  
  
public:
  // iCGameDataBase / Obtencion de texto estatico
  std::string GetStaticText(const GameDataBaseDefs::eStaticText& StaticText,
				            const word uwIdxMain = 0,
							const word uwIdxAux = 0);
  
public:
  // iCGameDataBase / Obtencion de la base de datos de reglas
  iCRulesDataBase* const GetRulesDataBase(void) {
	ASSERT(IsInitOk());
	ASSERT(m_RulesDataBase.IsInitOk());
	// Retorna instancia a la base de datos de las reglas
	return &m_RulesDataBase;
  }

public:
  // iCGameDataBase / Obtencion del nombre del fichero con la TileRegionMask
  std::string GetTileRegionMaskFileName(void) const {
	ASSERT(IsInitOk());
	// Retorna el nombre del fichero
	return m_szTileRegionMaskFileName;
  }

};

#endif // ~ CGameDataBase

