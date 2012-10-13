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
// CCBTEngineParser.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CCBTEngineParser.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CCBTEngineParser.h"

#include "SYSEngine.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Abre el archivo de nombre szFileName y obtiene su tamaño.
// Parametros:
// - szFileName. Nombre del archivo a abrir.
// Devuelve:
// - Si todo ha ido bien true, en caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CCBTEngineParser::OpenFile(const std::string& szFileName)
{
  // Abre
  m_pFileSys = SYSEngine::GetFileSystem();
  ASSERT(m_pFileSys);
  m_hFile = m_pFileSys->Open(szFileName);
  if (m_hFile) {
	// Obtiene tamaño y retorna
	m_udFileSize = m_pFileSys->GetFileSize(m_hFile);
	if (m_udFileSize) {
	  return true;
	}
  }

  // Hubo problemas
  return false;
}
