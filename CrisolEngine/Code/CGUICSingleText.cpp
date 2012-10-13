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
// CGUICSingleText.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUICSingleText.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUICSingleText.h"

#include "SYSEngine.h"
#include "iCFontSystem.h"
#include "iCCommandManager.h"
#include "CCyclicFadeCmd.h"
#include "CMemoryPool.h"

// Definicion de estructuras forward
struct CGUICSingleText::sAlphaFadeInfo {
  // Info asociada al fade en Alpha
  CCyclicFadeCmd FadeCmd; // Comando de fade
  // Manejador de memoria
  static CMemoryPool m_MPool;
  static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
  static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); } 
};

// Inicializacion de los manejadores de memoria
CMemoryPool 
CGUICSingleText::sAlphaFadeInfo::m_MPool(1, 
									     sizeof(CGUICSingleText::sAlphaFadeInfo), 
										 true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia guardando el texto asociado y justificandolo para
//   introducirlo en la lista de lineas. 
// Parametros:
// - InitData. Datos de inicializacion.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar
// - No se permitira que sepase un texto con una anchura tal que exista alguna
//   linea en la que no se pueda escribir una palabra completa al menos.
///////////////////////////////////////////////////////////////////////////////
bool 
CGUICSingleText::Init(const sGUICSingleTextInitData& InitData)
{
  // SOLO si parametros validos
  ASSERT(InitData.szText.size());

  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) { 
	return false; 
  }

  // Se inicializa clase base
  if (Inherited::Init(InitData)) {
	// Se establece el valor alpha particular
	m_TextInfo.Alpha = InitData.Alpha;
	m_TextInfo.UsableAlpha = InitData.Alpha;
	
	// Se establece resto de vbles
	m_pAlphaFadeInfo = NULL;
	
	// Se establece el texto recibido
	if (SetText(InitData.szText)) {
	  // Todo correcto
	  return true;
	}
  }

  // Hubo problemas
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el texto szText como texto, realizando todos los calculos que
//   procedan y actualizando las vbles de miembro.
// Parametros:
// - szText. Texto a establecer.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUICSingleText::SetText(const std::string& szText)
{
  // SOLO si parametros correctos
  ASSERT(szText.size());

  // Inicializa valores de dimension
  m_TextInfo.uwAreaHeight = 0;
  m_TextInfo.uwAreaWidth = 0;	

  // Se copia el string
  m_TextInfo.szText = szText;
  	
  // Se procede a justificar el texto asociado y a insertar la informacion  
  // Segun se justique, se hallara la linea con el mayor num. de caracteres    
  sword swItPos = 0;          // Iterador del cursor de lectura
  while (swItPos < m_TextInfo.szText.size()) {
	sLineInfo LineInfo; // Nodo asociado a una linea
	// Se justifca el texto que cabe a lo ancho
	LineInfo.swInitPos = swItPos;
	swItPos = Inherited::JustifyText(m_TextInfo.szText, swItPos);	
	
	// ¿No hay espacio?
	if (!swItPos) {
	  // Se abandona
	  End();
	  return false;
	}
	
	// Se halla el num. de caracteres de la linea 
	LineInfo.uwNumChars = swItPos - LineInfo.swInitPos;		

    // Se halla la anchura y se guarda si es la mas ancha
    // Nota: La anchura correspondera a la de la linea mas ancha. Para saber 
    // el area a lo ancho del componente, se debera de llamar a GetWidthJustify
	const std::string szMaxCharsLine(m_TextInfo.szText.substr(LineInfo.swInitPos, 
												              LineInfo.uwNumChars));	  
	const word uwAreaWidthTmp = CalculeLineWidth(szMaxCharsLine);
	if (uwAreaWidthTmp > m_TextInfo.uwAreaWidth) {
	  m_TextInfo.uwAreaWidth = uwAreaWidthTmp;  
	}

	// Se establece un offset de 0 por defecto en justificacion de escritura
	// por lo que el texto saldra hacia la izq.
	LineInfo.uwXJustifyOffset = 0;

	// Se inserta nueva linea y se avanza iterador
	m_TextInfo.TextLines.push_back(LineInfo);	
	++swItPos;
  }

  // Se halla la altura del componente
  m_TextInfo.uwAreaHeight = FontDefs::CHAR_PIXEL_HEIGHT * m_TextInfo.TextLines.size();

  // Todo correcto  
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICSingleText::End(void)
{
  // Finaliza la instancia si procede
  if (Inherited::IsInitOk()) {
	// Se desactiva alpha
	DeactiveAlphaFade();

	// libera lista de lineas y texto
	m_TextInfo.TextLines.clear();
	m_TextInfo.szText = "";
	
	// Se finaliza clase base
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Justifica el texto para que sea dibujado al centro, dcha o izq. Para
//   realizar la justificacion debera de recorrer todas las lineas que
//   compongan a este y realizar la operacion.
// - El texto parte de una justificacion a izq.
// Parametros:
// - Justify. Tipo de justificacion
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICSingleText::DrawJustifyTo(const eDrawJustify& Justify)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Recorre las lineas
  LinesIt It(m_TextInfo.TextLines.begin());
  byte ubIt = 0;
  for (; It != m_TextInfo.TextLines.end(); ++It, ++ubIt) {
	// Se justificara segun proceda
	switch (Justify) {
	  case CGUICSingleText::JUSTIFY_CENTER: {
		// Justificacion al centro
		const sword swLineWidth = CalculeLineWidth(GetLine(ubIt));
		if (GetWidthJustify() > swLineWidth) {
		  It->uwXJustifyOffset = (GetWidthJustify() - swLineWidth) >> 1;
		} else {
		  It->uwXJustifyOffset = 0;
		}
	  } break;

	  case CGUICSingleText::JUSTIFY_RIGHT: {
		// Justificacion a dcha
		It->uwXJustifyOffset = GetWidthJustify() - CalculeLineWidth(GetLine(ubIt));		
	  } break;

	  case CGUICSingleText::JUSTIFY_LEFT: {
		// Justifica a izq.
		It->uwXJustifyOffset = 0;
	  } break;	
	}; // ~ switch
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Calcula la anchura de una linea, apoyandose en CFontSystem
// Parametros:
// - szLine. Linea a evaluar
// Devuelve:
// - La anchura de la linea
// Notas:
///////////////////////////////////////////////////////////////////////////////
word
CGUICSingleText::CalculeLineWidth(const std::string& szLine)
{
  // Se halla la anchura y se retorna
  word uwWidth, uwHeight;
  SYSEngine::GetFontSystem()->GetTextDim(szLine,
										 Inherited::GetFont(),
										 uwWidth,
										 uwHeight);
  return uwWidth;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene las lineas que ocupa el texto despues de que se halla justificado.
// Parametros:
// Devuelve:
// - Lineas de ocupacion del texto justificado.
// Notas:
///////////////////////////////////////////////////////////////////////////////
word 
CGUICSingleText::GetVisibleTextLines(void) const
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se obtiene el numero de lineas
  return m_TextInfo.TextLines.size();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la linea de texto indicada y la devuelve.
// Parametros:
// - uwLine. Linea de texto (numerada de 0 a n-1).
// Devuelve:
// - La linea extraida. Si la linea es no valida, se devolvera string vacio.
// Notas:
///////////////////////////////////////////////////////////////////////////////
std::string
CGUICSingleText::GetLine(const word uwLine)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  
  // ¿Linea NO valida?
  if (uwLine >= m_TextInfo.TextLines.size()) { 
	return "";
  }

  // Se localiza la linea
  LinesIt It(m_TextInfo.TextLines.begin());
  word uwIt = 0;
  while (uwIt < uwLine) {
	++uwIt;
	++It;
  }

  // Se establece el string y se retorna
  return m_TextInfo.szText.substr(It->swInitPos, It->uwNumChars);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Activa el fade en el componente alpha para que se llegue al alpha de
//   destino indicado.
// - Sera condicion necesaria para poder activar el fade que la instancia
//   se halle activa y ademas que el alpha de destino sea distino al alpha
//   actual.
// Parametros:
// - AlphaDest. Alpha de destino.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICSingleText::ActiveAlphaFade(const GraphDefs::sAlphaValue& AlphaDest)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Instancia activa?
  if (Inherited::IsActive()) {
	// ¿Alpha de destino distitno al alpha actual?
	if (!(m_TextInfo.Alpha == AlphaDest)) {
	  // Se finaliza posible fade actual
	  DeactiveAlphaFade();

	  // Se crea nodo para el nuevo fade
	  m_pAlphaFadeInfo = new sAlphaFadeInfo;
	  ASSERT(m_pAlphaFadeInfo);
	  
	  // Se configura el fade y se inserta en la cola de comandos
	  m_pAlphaFadeInfo->FadeCmd.Init(&m_TextInfo.UsableAlpha, AlphaDest, 128);	
	  ASSERT(m_pAlphaFadeInfo->FadeCmd.IsActive());
	  SYSEngine::GetCommandManager()->PostCommand(&m_pAlphaFadeInfo->FadeCmd);
	}
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Desactiva el posible alpha de destino que estuviera activado.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICSingleText::DeactiveAlphaFade(void)
{
  // SOLO si intancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Alpha activado?
  if (m_pAlphaFadeInfo) {
	// Se borra de memoria el nodo con la informacion, finalizandolo
	m_pAlphaFadeInfo->FadeCmd.End();
	delete m_pAlphaFadeInfo;
	m_pAlphaFadeInfo = NULL;
	
	// Se restaura el valor alpha original
	m_TextInfo.UsableAlpha = m_TextInfo.Alpha;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza un dibujado de todo el texto asociado a las distintas lineas.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGUICSingleText::Draw(void)
{
  // SOLO si instancia activa
  ASSERT(Inherited::IsInitOk());
    
  // Itera por las lineas a dibujar
  LinesIt It(m_TextInfo.TextLines.begin());
  sword swYOffPos = 0;
  for (; It != m_TextInfo.TextLines.end(); ++It) {
	// Se obtiene la porcion de texto a dibujar y se dibuja
	SYSEngine::GetFontSystem()->Write(Inherited::GetXPos() + It->uwXJustifyOffset,
									  Inherited::GetYPos() + swYOffPos,
									  Inherited::GetDrawPlane(),
									  m_TextInfo.szText.substr(It->swInitPos, It->uwNumChars),
									  Inherited::GetActColor(),
									  m_TextInfo.UsableAlpha,
									  Inherited::GetFont());
	
	// Actualiza offset de dibujado
	swYOffPos += FontDefs::CHAR_PIXEL_HEIGHT;
  }  
}
 
