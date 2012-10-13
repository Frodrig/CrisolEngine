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
// CFontSystem.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Consultar CFontSystem.h para mas detalles.
///////////////////////////////////////////////////////////////////////////////
#include "CFontSystem.h"

#include "SYSEngine.h"
#include "iCGraphicSystem.h"
#include "iCLogger.h"
#include <algorithm>

// Inicializacion de la unica instancia singlenton
CFontSystem* CFontSystem::m_pFontSystem = NULL;

// Inicializacion de los manejadores de memoria
CMemoryPool 
CFontSystem::sNFont::m_MPool(4,
							 sizeof(CFontSystem::sNFont),
							 true);
CMemoryPool 
CFontSystem::sNCharacter::m_MPool(256,
								  sizeof(CFontSystem::sNCharacter),
								  true);
CMemoryPool 
CFontSystem::sNTextGraphInfo::m_MPool(256,
									  sizeof(CFontSystem::sNTextGraphInfo),
									  true);

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia, creando en memoria la estructura con toda
//   la informacion general referida al subsistema y realiza llamadas para la
//   inicializacion de otros subsistemas.
// - El metodo recibira la fuente por defecto (que por defecto valdra Arial).
// Parametros:
// - szDefaultFont. Fuente por defecto.
// Devuelve:
// - Si todo ha ido bien true. En caso contrario false.
// Notas:
// - No se permitira reinicializar
///////////////////////////////////////////////////////////////////////////////
bool 
CFontSystem::Init(const std::string& szDefaultFont)
{
  // ¿Se intenta reinicializar?
  if (IsInitOk()) { 
	return false; 
  }
  
  #ifdef ENGINE_TRACE
    SYSEngine::GetLogger()->Write("CFontSystem::Init> Inicializado subsistema de fuentes.\n");
  #endif  
    
  // Obtiene instancia al subsistema grafico
  m_pGraphSys = SYSEngine::GetGraphicSystem();
  ASSERT(m_pGraphSys);

  // Se inicializan datos basicos del subistema de fuentes
  m_FontSysInfo.DefaultFontIt = m_FontSysInfo.FontTable.end();

  // Se inicializan pools para que sean de recarga automatica
  m_PoolInfo.CharacterPool.Init(0);
  m_PoolInfo.TextGraphPool.Init(0);

  // Se instala y selecciona la fuente como fuente por defect
  // Nota: Se levantara en este punto el flag de inicializacion para
  // permitir la llamada al metodo AddFont
  m_bIsInitOk = true;
  if (AddFont(szDefaultFont)) { 
	// Todo correcto
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("                 | Ok.\n");
	#endif  
	return true;
  }

  // Hubo problemas
  #ifdef ENGINE_TRACE
	SYSEngine::GetLogger()->Write("                 | Error inicializado.\n");
  #endif  
  End();
  return false;
}
 
///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Finaliza la instancia, liberando de memoria la estructura con la info 
//   general asociada. Tambien se llamara a los metodos para la finalizacion
//   de otros subsistemas.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CFontSystem::End(void)
{
  // Finaliza si procede
  if (IsInitOk()) {
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("CFontSystem::End> Finalizado subsistema de fuentes.\n");
	#endif

	// Libera fuentes instaladas
	EndFonts();

	// Libera pools de memoria
	m_PoolInfo.CharacterPool.End();
	m_PoolInfo.TextGraphPool.End();

	// Baja flag
	m_bIsInitOk = false;
	#ifdef ENGINE_TRACE
	  SYSEngine::GetLogger()->Write("                | Ok.\n");
	#endif
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Libera y finaliza fuentes instaladas
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CFontSystem::EndFonts(void)
{
  // Procede al liberar nodos con fuentes instaladas
  FontTableMapIt It(m_FontSysInfo.FontTable.begin());
  for (; It != m_FontSysInfo.FontTable.end(); It = m_FontSysInfo.FontTable.erase(It)) {
	delete It->second;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Instala una fuente en el subsistema. Instalar una fuente supondra crear
//   un nodo en el map. Si dicha fuente ya existe en el map,
//   no se realizara operacion alguna y se devolvera true, como si se hubiera
//   instalado ya.
// - La primera fuente que se instale siempre actuara de fuente por defecto. La
//   fuente por defecto se seleccionara de forma automatica siempre que se
//   desee escribir algo y no se pase una fuente en concreto o bien se pase una
//   que o no existe o no esta instalada.
// - La fuente por defecto nunca se podra desinstalar si hay otras fuentes
//   instaladas.
// Parametros:
// - szFontName. Nombre de la fuente a instalar.
// Devuelve:
// - Si la fuente se instalo o ya existia true. En caso contrario false.
// Notas:
// - Las fuentes siempre seran introducidas en minusculas
///////////////////////////////////////////////////////////////////////////////
bool 
CFontSystem::AddFont(const std::string& szFontName)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  
  // ¿Ya se encuentra la fuente instalada?
  if (GetFontIt(szFontName) != m_FontSysInfo.FontTable.end()) {
	return true;
  }

  // Se procede a instalar la fuente
  sNFont* pFont = new sNFont;
  ASSERT(pFont);

  // Se inicializa fuente y se registra
  std::string szFontNameLowercase(szFontName);
  SYSEngine::MakeLowercase(szFontNameLowercase);
  if (!pFont->Font.Init(szFontNameLowercase)) { 
	delete pFont;
	return false;
  }
  m_FontSysInfo.FontTable.insert(FontTableMapValType(szFontNameLowercase, pFont));

  // En caso de que no exista ninguna fuente como seleccionada, significara que
  // la fuente instalada es la primera. Se seleccionara como por defecto.  
  if (m_FontSysInfo.FontTable.size() == 1) {	
	m_FontSysInfo.DefaultFontIt = m_FontSysInfo.FontTable.begin();
  }

  // Todo correcto
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Obtiene la interfaz asociada a la fuente cuyo nombre ha de ser szFontName
//   y que debe de estar instalada en el susbsistema.
// Parametros:
// - szFontName. Nombre de la fuente sobre la que devolver el interfaz.
// Devuelve:
// - Instancia a la fuente o NULL si no se halla
// Notas:
///////////////////////////////////////////////////////////////////////////////
iCFont* const 
CFontSystem::GetFont(const std::string szFontName)
{
  // SOLO si instancia instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si hay al menos una fuente instalada
  ASSERT(!m_FontSysInfo.FontTable.empty());

  // Se localiza y retorna
  // Nota: En caso de no localizar la fuente solicitada, se retornara
  // la fuente instalada por defecto.
  FontTableMapIt FontIt(GetFontIt(szFontName));
  if (FontIt == m_FontSysInfo.FontTable.end()) {
	return &m_FontSysInfo.DefaultFontIt->second->Font;
  } else {
	return &FontIt->second->Font;
  }  
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Restaura las fuentes. Las fuentes tienen asociadas superficies vacias en
//   donde se vuelcan todos los caracteres.
// Parametros:
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CFontSystem::RestoreFonts(void)
{
  // Se procede a recorrer el map con las fuentes y a restaurar las fuentes
  FontTableMapIt It(m_FontSysInfo.FontTable.begin());
  for (; It != m_FontSysInfo.FontTable.end(); ++It) {
	It->second->Font.Restore();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Realiza la iteracion por cada uno de los caracteres creando un nuevo
//   nodo con una instancia CCharFont. Con dicha instancia inicializada 
//   segun sea el caracter a dibujar, se realizara una llamada al metodo de
//   dibujado en el subsistema grafico. Los nodos que mantendran a la instancia
//   CCharFont, seran mantenidos en un pool automatico de memoria.
// Parametros:
// - ubPlane. Plano de dibujado.
// - swXPos, swYPos. Posicion en x,y donde dibujar.
// - szText. Texto a dibujar en pantalla.
// Devuelve:
// Notas:
// - En caso de que el texto este vacio, simplemente se obviara la 
//   peticion que se haya realizado.
///////////////////////////////////////////////////////////////////////////////
void 
CFontSystem::Write(const sword& swXPos,
				   const sword& swYPos,
				   const byte ubPlane,
				   const std::string& szText,
				   const GraphDefs::sRGBColor& RGBColor,
				   const GraphDefs::sAlphaValue& Alpha,
				   const std::string& szFontName)
{
  // SOLO si instancia inicializada
  ASSERT(IsInitOk());
  // SOLO si parametros correctos
  ASSERT((ubPlane < GraphDefs::MAX_GUI_DRAWPLANES) != 0);
  // SOLO si hay al menos una fuente instalada
  ASSERT(!m_FontSysInfo.FontTable.empty());

  // Se escoge la fuente a utilizar
  // Nota: En caso de que la fuente a usar no se halle, se pondra la fuente
  // por defecto.
  FontTableMapIt FontIt(GetFontIt(szFontName));
  if (FontIt == m_FontSysInfo.FontTable.end()) {
	FontIt = m_FontSysInfo.DefaultFontIt;
  }

  // Se procede a iterar por los caracteres
  // Procede a iterar por los caracteres
  sword swXDrawPos = swXPos;
  std::string::const_iterator CharIt(szText.begin());
  for (; CharIt != szText.end(); ++CharIt) {
    // Se inicializa objeto elemento dibujable
    sNCharacter* const pChar = m_PoolInfo.CharacterPool.GetNewNode();  
    ASSERT(pChar);

    // Se inicializa la estructura de dibujado
    if (!pChar->DrawableChar.Init(FontIt->second->Font.GetIDFontImage(),
  								  FontIt->second->Font.GetCharPos(*CharIt),
								  FontIt->second->Font.GetCharQuadRes(),
								  FontIt->second->Font.GetCharQuadRes())) {
  	  ASSERT_MSG(false, "¡No se inicializó el objeto dibujable con el caracter!");
	  return;
	}

	// Estructuras RGB y Alpha a asociar
	sNTextGraphInfo* const pGraphInfo = m_PoolInfo.TextGraphPool.GetNewNode();
	ASSERT(pGraphInfo);
	pGraphInfo->RGBColor = RGBColor;
	pGraphInfo->Alpha = Alpha;

	// Se asocia las estructuras RGB y Alpha que proceda
	byte ubIt = 0;
	for (; ubIt < 4; ++ubIt) {
	  const GraphDefs::eVertex Vertex = GraphDefs::eVertex(ubIt);
	  pChar->DrawableChar.SetRGBColor(Vertex, &pGraphInfo->RGBColor);	  
	  pChar->DrawableChar.SetAlphaValue(Vertex, &pGraphInfo->Alpha);
	}	

	// Se realiza el dibujado
	m_pGraphSys->Draw(GraphDefs::DZ_GUI,
					 ubPlane,
					 swXDrawPos,
					 swYPos,
					 &pChar->DrawableChar);			  

	// Incrementa pos en x
	swXDrawPos += FontIt->second->Font.GetCharWidth(*CharIt);
  }    
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de obtener el area que ocupa el texto szText usando la fuente
//   szFontName. El resultado se devolvera por parametro.
// Parametros:
// - szText. Texto.
// - szFontName. Nombre de la fuente.
// - uwWidth, uwHeight. Area del texto.
// Devuelve:
// - Por parametro, el area que ocupa el texto.
// Notas:
///////////////////////////////////////////////////////////////////////////////
void 
CFontSystem::GetTextDim(const std::string& szText,
						const std::string& szFontName,
						word& uwWidth,
						word& uwHeight)
{
  // SOLO si instancia inicializada 
  ASSERT(IsInitOk());
  // SOLO si hay al menos una fuente instalada
  ASSERT(!m_FontSysInfo.FontTable.empty());
  
  // Obtiene la fuente
  // Nota: En caso de que la fuente pasada no exista, se tomara la fuente
  // por defecto.
  FontTableMapIt FontIt(GetFontIt(szFontName));
  if (FontIt == m_FontSysInfo.FontTable.end()) {
	FontIt = m_FontSysInfo.DefaultFontIt;
  }
  
  // Itera a traves del texto
  uwHeight = 16;
  uwWidth = 0;
  std::string::const_iterator CharIt(szText.begin());
  for (; CharIt != szText.end(); ++CharIt) {
	uwWidth += FontIt->second->Font.GetCharWidth(*CharIt);
  }	
}