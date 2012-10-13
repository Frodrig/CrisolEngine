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
// CGUICLineText.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUICLineText.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUICLineText.h"

#include "SYSEngine.h"
#include "iCFontSystem.h"

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa el componente apoyandose en la clase base.
// Parametros:
// - InitData. Datos de inicializacion.
// Devuelve:
// - Si todo marcha bien true. En caso de no poder inicializar false.
// Notas:
// - No se permitira reinicializar.
///////////////////////////////////////////////////////////////////////////////
bool 
CGUICLineText::Init(const sGUICLineTextInitData& InitData)
{
  // SOLO si parametros validos
  ASSERT(InitData.ID);
  ASSERT_MSG(InitData.szFont.size(),InitData.szLine.c_str());
  
  // ¿Se quiere recinializar?
  if (Inherited::IsInitOk()) { 	
	return false; 
  }

  // Se inicializa la clase base
  if (Inherited::Init(InitData)) { 	
	// Se establecen vbles de miembo
	m_TextInfo.szFont = InitData.szFont;
	m_TextInfo.szLine = InitData.szLine;
	m_TextInfo.RGBSelectColor = InitData.RGBSelectColor;
	m_TextInfo.RGBUnselectColor = InitData.RGBUnselectColor;
	m_TextInfo.pRGBActColor = &m_TextInfo.RGBUnselectColor;
	m_TextInfo.Alpha = InitData.Alpha;

	// Se halla el area de la linea
    SYSEngine::GetFontSystem()->GetTextDim(m_TextInfo.szLine,
										   m_TextInfo.szFont,
										   m_TextInfo.uwAreaWidth,
										   m_TextInfo.uwAreaHeight);
	// Todo correcto
	return true;
  }  

  // Hubo problemas
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia, liberando recursos.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICLineText::End(void)
{
  // Finaliza la instancia si procede
  if (Inherited::IsInitOk()) {
	// Libera recursos
	m_TextInfo.szFont = "";
	m_TextInfo.szLine = "";
	m_TextInfo.pRGBActColor = NULL;

	// Finaliza clase base
	Inherited::End();	
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Cambia el texto asociado.
// Parametros:
// - szNewText. Nuevo texto a establecer.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICLineText::ChangeText(const std::string& szNewText)
{	
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());	

  // Se establece el nuevo texto y se halla el area que ocupa
  m_TextInfo.szLine = szNewText;
  SYSEngine::GetFontSystem()->GetTextDim(m_TextInfo.szLine,
										 m_TextInfo.szFont,
										 m_TextInfo.uwAreaWidth,
										 m_TextInfo.uwAreaHeight);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza el dibujado del texto asociado.
// Parametros:
// - ubDrawPlane. Plano de dibujado
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICLineText::Draw(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Dibuja si procede
  if (Inherited::IsActive()) {	
	// Se escribe
	SYSEngine::GetFontSystem()->Write(Inherited::GetXPos(), 
									  Inherited::GetYPos(),
									  Inherited::GetDrawPlane(),
									  m_TextInfo.szLine,
									  *m_TextInfo.pRGBActColor,
									  m_TextInfo.Alpha,
									  m_TextInfo.szFont);	
  }
}
