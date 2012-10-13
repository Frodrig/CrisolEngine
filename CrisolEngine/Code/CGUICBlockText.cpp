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
// CGUICBlockText.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CGUICBlockText.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CGUICBlockText.h"

#include "SYSEngine.h"
#include "iCAudioSystem.h"
#include "iCFontSystem.h"
#include "iCResourceManager.h"
#include "CMemoryPool.h"

// Estructuras
struct CGUICBlockText::sLineInfo {
  // Nodo asociado a una linea
  std::string szLine;
  // Manejador de memoria
  static CMemoryPool m_MPool;
  static void* operator new(const size_t size) { return m_MPool.AllocMem(size); }
  static void operator delete(void* pItem) { m_MPool.FreeMem(pItem); }  
};

// Inicializacion del manejador de memoria
CMemoryPool 
CGUICBlockText::sLineInfo::m_MPool(256,
								   sizeof(CGUICBlockText::sLineInfo),
								   true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializacion de la instancia.
// Parametros:
// - InitData. Datos de inicializacion.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar
// - Inicialmente se tendra el componente no seleccionado
///////////////////////////////////////////////////////////////////////////////
bool 
CGUICBlockText::Init(const sGUICBlockTextInitData& InitData)
{
  // SOLO si parametros validos
  ASSERT(InitData.szFont.size());
  ASSERT(InitData.uwNumLines);
  ASSERT(InitData.uwWidthJustify);

  // ¿Se intenta reinicializar?
  if (Inherited::IsInitOk()) { 
	return false; 
  }

  // Se inicializa clase base
  if (Inherited::Init(InitData)) { 
	// Se toma instancia al subsistema de fuentes
	m_pFontSys = SYSEngine::GetFontSystem();
	ASSERT(m_pFontSys);
  
	// Se prepara string para alojar texto y se almacena resto de info
	m_TextInfo.uwCapacity = InitData.uwCapacity;  
	m_TextInfo.uwNumLines = InitData.uwNumLines;
	m_TextInfo.ItLinePos = m_TextInfo.TextLines.end();
	m_TextInfo.uwItNumPos = 0;
	m_TextInfo.bCanReutilice = InitData.bCanReutilice;
	m_TextInfo.bViewBottomUp = InitData.bViewBottomUp;

	// Se registra el sonido si procede
	if (InitData.szWAVBipSound.size()) {
	  m_TextInfo.hWAVBipSound = SYSEngine::GetResourceManager()->WAVSoundRegister(InitData.szWAVBipSound);		
	  if (!m_TextInfo.hWAVBipSound) {
		// Problemas
		SYSEngine::FatalError("No se pudo abrir el fichero de sonido \"%s\".\nFichero inexistente o incompatible.\n", InitData.szWAVBipSound.c_str());
		End();
		return false;
	  }
	} else {
	  m_TextInfo.hWAVBipSound = 0;
	}
  
	// Se comienza en estado normal
	m_StateBlock = CGUICBlockText::NORMAL_STATE;
	
	// Todo correcto
	return true;
  }

  // Hubo problemas
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICBlockText::End(void)
{
  // Finaliza la instancia si procede
  if (Inherited::IsInitOk()) {
	// Se vacia lista
	Reset();
	// Se libera recurso de sonido si procede
	if (m_TextInfo.hWAVBipSound) {
	  SYSEngine::GetResourceManager()->ReleaseWAVSound(m_TextInfo.hWAVBipSound);
	  m_TextInfo.hWAVBipSound = 0;
	}
	// Se propaga
	Inherited::End();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera todas las lineas introducidas en el bloque de texto y desvincula
//   el iterador.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICBlockText::Reset(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Libera nodos a lineas
  LinesListIt It(m_TextInfo.TextLines.begin());
  while (It != m_TextInfo.TextLines.end()) {
	delete (*It);
	It = m_TextInfo.TextLines.erase(It);
  }

  // Se establece estado
  m_StateBlock = CGUICBlockText::NORMAL_STATE;

  // Se desvincula iterador
  SetIt();  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene las lineas que ocupa el texto.
// Parametros:
// Devuelve:
// - Lineas de ocupacion del texto justificado.
// Notas:
// - No confundir las lineas que ocupa el texto de las lineas maximas 
//   asignadas para visualizarlo. Siempre se devolvera el valor mas pequeño
//   de entre los dos.
///////////////////////////////////////////////////////////////////////////////
word 
CGUICBlockText::GetVisibleTextLines(void) const
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se retorna informacion 
  return (m_TextInfo.TextLines.size() > m_TextInfo.uwNumLines) ? 
		  m_TextInfo.uwNumLines : m_TextInfo.TextLines.size();

}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza un dibujado de todo el texto asociado a las distintas lineas.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void
CGUICBlockText::Draw(void)
{
  // SOLO si instancia activa
  ASSERT(Inherited::IsInitOk());

  // Dibuja si procede
  if (Inherited::IsActive() && 
	  m_TextInfo.TextLines.size()) {   		
	// Vbles
	sword swYOffPos = m_TextInfo.swInitYOffDrawPos; // Offset de dibujado en Y
	word uwActLinesDraw = 0;	                    // Linea actual dibujada
	
	// Se procede a dibujar el texto
	LinesListIt It(m_TextInfo.ItLinePos);
	while (It != m_TextInfo.TextLines.end() && 
		   uwActLinesDraw < m_TextInfo.uwNumLines) {
	  // Se dibuja			
	  m_pFontSys->Write(Inherited::GetXPos(),
						Inherited::GetYPos() + swYOffPos,
						Inherited::GetDrawPlane(), 
						(*It)->szLine,
						Inherited::GetActColor(),
						Inherited::GetAlphaValue(),
						Inherited::GetFont());	
	  // Incremento iteradores y contadores
	  ++It;
	  ++uwActLinesDraw;
	  swYOffPos += FontDefs::CHAR_PIXEL_HEIGHT;
	}    	  
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Procede a insertar el texto recibido, teniendo en cuenta el estado en el
//   que se halla el bloque de texto. Si se permite reutilizar el bloque y este
//   se encuentra lleno, se comenzaran a eliminar nodos por el principio de la 
//   lista y a insertarlos por el final.
// - Siempre que se inserte un texto nuevo, el iterador apuntara al nodo que
//   contenga la linea con ese texto recien insertado.
// Parametros:
// - szText. Texto a insertar.
// - bPlaySound. ¿Se hace sonar sonido una vez que se inserte?.
// Devuelve:
// - Si se ha podido insertar el texto true. En caso contrario false.
// Notas:
// - En caso de que el texto a insertar rebase la capacidad, se devolvera
//   false.
///////////////////////////////////////////////////////////////////////////////
bool 
CGUICBlockText::InsertText(const std::string& szText,
						   const bool bPlaySound)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿NO es posible insertar?
  if (CGUICBlockText::FULL_STATE == m_StateBlock) { 
	return false; 
  }
  
  // Se procede a justificar e insertar
  bool  bResult = true;
  sword swItInitPos = 0;
  while (swItInitPos < szText.size()) {
	// Se justifica	y se obtiene la cantidad de texto a copiar	
	sword swItEndPos = Inherited::JustifyText(szText, swItInitPos);		  
	sword uwNumCharsToCopy = swItEndPos - swItInitPos;
	
	// ¿No hay espacio para alojar al menos una palabra?
	if (0 == swItEndPos) { 
	  return false; 
	}

	// ¿Estamos en estado de insercion sin reutilizacion?
	if (CGUICBlockText::NORMAL_STATE == m_StateBlock) {
	  // ¿la capacidad NO ES INFINITA?
	  if (m_TextInfo.uwCapacity > 0) {
		// ¿NO hay suficiente capacidad?
		if (m_TextInfo.TextLines.size() == m_TextInfo.uwCapacity) {
		  // ¿Se nos permite reutilizar?		
		  if (m_TextInfo.bCanReutilice) {
			// Se pasa a estado de reutilizacion
			m_StateBlock = CGUICBlockText::REUTILICE_STATE;
		  } else {
			// Se cambia a estado de lleno y se abandona
			m_StateBlock = CGUICBlockText::FULL_STATE;
			bResult = false;
			break;
		  }
		}
	  }	  
	}

	// Se inserta linea y actualiza iterador de inicio de justificacion
	InsertLine(szText, swItInitPos, uwNumCharsToCopy);
	swItInitPos = swItEndPos + 1;
  }

  // Reproduce sonido de aviso de insercion de mensaje
  if (m_TextInfo.hWAVBipSound && bPlaySound) {	 
	SYSEngine::GetAudioSystem()->PlayWAVSound(m_TextInfo.hWAVBipSound,
											  AudioDefs::WAV_PLAY_NORMAL);
  }

  // Se ajusta la posicion inicial de dibujado
  AdjustInitYOffDrawPos();

  // Linea insertada
  return bResult;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Ajusta el offset inicial de dibujado en Y.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICBlockText::AdjustInitYOffDrawPos(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // Se ajusta el offset de dibujado en Y
  // ¿Se desea que el texto se vea de abajo arriba?
  if (m_TextInfo.bViewBottomUp) {	
	// Si, se ajusta y se clippea
	m_TextInfo.swInitYOffDrawPos = (m_TextInfo.uwNumLines - m_TextInfo.TextLines.size()) * 
									FontDefs::CHAR_PIXEL_HEIGHT;  
	if (m_TextInfo.swInitYOffDrawPos < 0) { 
	  m_TextInfo.swInitYOffDrawPos = 0; 
	}  
  } else {
	// No, se ajusta a la primera posicion
    m_TextInfo.swInitYOffDrawPos = 0; 
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inserta una linea de texto en la lista de lineas. La linea ira al final
//   del todo.
// Parametros:
// - szTextSrc. String con el texto a copiar.
// - swInitPos. Posicion inicial.
// - uwNumChar. Numero de caracteres a copiar.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICBlockText::InsertLine(const std::string& szTextSrc, 
						   const sword& swInitPos,
						   const word uwNumChars)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si parametros validos
  ASSERT(szTextSrc.size());
  ASSERT((swInitPos >= 0) != 0);
  ASSERT((swInitPos < szTextSrc.size()) != 0);

  // Vbles
  sLineInfo* pNewLine; // Nodo a nueva linea

  // Se consigue nodo  
  if (CGUICBlockText::REUTILICE_STATE == m_StateBlock) {
	// Se esta reutilizando memoria
	// Se obtiene direccion a primer nodo y se elimina de la lista
	pNewLine = m_TextInfo.TextLines.front();
	ASSERT(pNewLine)	
	m_TextInfo.TextLines.pop_front();

	// Se establece posicion del iterador si se encuentra vinculado
	if (m_TextInfo.uwItNumPos > 0) { 
	  --m_TextInfo.uwItNumPos; 
	} 	
  } else {
	// No se esta en modo reutilizacion
	// Se crea un nuevo nodo en memoria
	pNewLine = new sLineInfo;
	ASSERT(pNewLine);
  };

  // Se copia el texto recibido al string del nodo  
  pNewLine->szLine = szTextSrc.substr(swInitPos, uwNumChars);

  // Se inserta la nueva linea al final de la lista 
  m_TextInfo.TextLines.push_back(pNewLine);
  
  // Se actualiza iterador de posicion    
  if (0 == m_TextInfo.uwItNumPos) { 
	SetIt(); 
  }
  
  // Se halla la posicion tope hasta la que se puede mover el iterador hacia abajo. 
  const sword swMaxTmp = m_TextInfo.TextLines.size() - m_TextInfo.uwNumLines + 1;
  m_TextInfo.uwMaxBottomPos = (swMaxTmp < 0) ? 1 : swMaxTmp;  

  // Se localiza el iterador en la zona inferior si procede
  if (m_TextInfo.bViewBottomUp) {
	SetAtBottom(); 
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Mueve el iterador a la linea a dibujar, una posicion hacia arriba.
// Parametros:
// Devuelve:
// - Si se ha podido realizar la operacion true. En caso contrario false.
// Notas:
///////////////////////////////////////////////////////////////////////////////
bool 
CGUICBlockText::MoveUp(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Hay elementos en la lista?
  if (m_TextInfo.TextLines.size()) {
	// Retrocede iterador si procede
	if (m_TextInfo.uwItNumPos > 1) {
	  --m_TextInfo.ItLinePos;
	  --m_TextInfo.uwItNumPos;
	  return true;
	}
  }
  
  // No se pudo retrasar
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Mueve el iterador a la linea a dibujar, una posicion hacia abajo.
// Parametros:
// Devuelve:
// - Si se ha podido realizar la operacion true. En caso contrario false.
// Notas:
// - Por ahora la implementacion no tiene en cuenta cuadrar los avances de tal
//   forma que queden justamente el numero de lineas desde la posicion del
//   iterador hasta el final.
///////////////////////////////////////////////////////////////////////////////
bool 
CGUICBlockText::MoveDown(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());

  // ¿Hay elementos en la lista?
  if (m_TextInfo.TextLines.size()) {
	// Avanza iterador si procede  
	if (m_TextInfo.uwItNumPos < m_TextInfo.uwMaxBottomPos) {
	  ++m_TextInfo.ItLinePos;
	  ++m_TextInfo.uwItNumPos;
	  return true;
	}
  } 

  // No se pudo retrasar
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coloca el iterador al comienzo del bloque de lineas
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CGUICBlockText::SetAtTop(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si iterador vinculado
  ASSERT(m_TextInfo.uwItNumPos);

  // ¿Hay elementos en la lista?
  if (m_TextInfo.TextLines.size()) {
	// Coloca el iterador al comienzo si procede
	m_TextInfo.ItLinePos = m_TextInfo.TextLines.begin();
	m_TextInfo.uwItNumPos = 1;  
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Coloca el iterador al final del bloque de lineas visualizables
// Parametros:
// Devuelve:
// Notas:
// - Realmente no se coloca el iterador en la ultima linea, sino que se
//   deja en la primera linea tal que la ultima de todas sea visualizable
//   en pantalla.
///////////////////////////////////////////////////////////////////////////////
void 
CGUICBlockText::SetAtBottom(void)
{
  // SOLO si instancia inicializada
  ASSERT(Inherited::IsInitOk());
  // SOLO si iterador vinculado
  ASSERT(m_TextInfo.uwItNumPos);

  // ¿Hay elementos en la lista?
  if (m_TextInfo.TextLines.size()) {
	// Coloca el iterador en la ultima linea visualizable  
	while (m_TextInfo.uwItNumPos < m_TextInfo.uwMaxBottomPos) {
	  ++m_TextInfo.ItLinePos;
	  ++m_TextInfo.uwItNumPos;	
	}   
  }  
};