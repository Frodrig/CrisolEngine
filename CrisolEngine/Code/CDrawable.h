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
// CDrawable.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CDrawable
//
// Descripcion:
// - Clase base pura para todo objeto que pueda ser dibujable.
//   Internamente mantendra el identificador de la textura, la configuracion
//   de dibujado y las dimensiones del quad.
// - La configuracion de dibujadoo estaran codificados de la sig. forma:
//   ubParams = b7 b6 b5 b4 b3 b2 b1 b0
//   * b0 -> Alpha Blending con el canal de la textura.
//   * b1 -> Alpha Blending con el canal de los vertices (del quad).
//   * b2 -> Bilinear Filtering activado.
//   * b3 -> Uso del color key. Solo tendra sentido si no esta activo el
//           efecto bilinear (esto es, b2 a 0). Sin embargo, estos detalles
//           se controlarian desde el modulo CGraphicSystem para dar una mayor
//           flexibilidad a CDrawable.
//
// Notas:
// - Todos los bits del parametro estaran a 0 inicialmente.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CDRAWABLE_H_
#define _CDRAWABLE_H_

// Cabeceras
#ifndef _ICDRAWABLE_H_
#include "iCDrawable.h" 
#endif

// Clase CDrawable
class CDrawable: public iCDrawable
{
private:
  // Tipos
  typedef byte Params; // Parametros de render

private:
  // Vbles de miembro  
  Params			      m_Params;   // Parametros de dibujado
  GraphDefs::sRGBColor*   pRGBs[4];   // Estructuras RGB por vertice
  GraphDefs::sAlphaValue* pAlphas[4]; // Estructuras Alpha por vertice
      
public:
  // Protocolos de inicilizacion / finalizacion de instancia.  
  CDrawable(void): m_Params(0) { 
	// Se inicializan estructuras
	memset(pRGBs, NULL, sizeof(GraphDefs::sRGBColor*) * 4);
	memset(pAlphas, NULL, sizeof(GraphDefs::sAlphaValue*) * 4);
  }

public:
  GraphDefs::sTextureRegion* const GetTextureRegion(void) {
	// No hay region asociada
	return NULL;
  }  

public:
  // iCDrawable / Obtencion de componentes RGB y Alpha por vertice
  GraphDefs::sRGBColor* const GetRGBColor(const GraphDefs::eVertex& Vert) const { 
	// Se retorna estructura RGB de color
	return pRGBs[Vert]; 
  }
  GraphDefs::sAlphaValue* const GetAlphaValue(const GraphDefs::eVertex& Vert) const { 
	// Se retorna alpha asociado
	return pAlphas[Vert]; 
  }

public:
  // Establecimiento de tablas RGB y Alpha por vertice
  inline void SetRGBColor(const GraphDefs::eVertex& Vert, 
						  GraphDefs::sRGBColor* const pRGB = NULL) {
	// Se establece estructura de color
	pRGBs[Vert] = pRGB;	
  }
  inline void SetAlphaValue(const GraphDefs::eVertex& Vert, 
							GraphDefs::sAlphaValue* const pAlpha = NULL) {
	// Se establece alpha
	pAlphas[Vert] = pAlpha;
  }

public:
  // Establecimiento de la configuracion de dibujado
  inline void SetTextureAlphaBlending(const bool bSet) {
	// Se establece flag de alpha blending en textura
	m_Params = bSet ? m_Params | 0x01 : m_Params & 0xFE;    
  }
  inline void SetVertexAlphaBlending(const bool bSet) {
	// Se establece flag de alpha blending en vertices
	m_Params = bSet ? m_Params | 0x02 : m_Params & 0xFD;
  }
  inline void SetBilinearFiltering(const bool bSet) {
	// Se esteblece flag de efecto bilinear
	m_Params = bSet ? m_Params | 0x04 : m_Params & 0xFB;
  }
  inline void SetColorKey(const bool bSet) {
	// Se establece flag de color key
	m_Params = bSet ? m_Params | 0x08 : m_Params & 0xEF; 
  }

public:
  // iCDrawable / Obtencion de parametros de dibujado
  bool IsTextureAlphaBlending(void) const { 
	// Se retorna flag de uso de alpha blending en textura
    return (m_Params & 0x01) ? true : false; 
  }
  bool IsVertexAlphaBlending(void) const { 
	// Se retorna flag de uso de alpha blending en vertices
    return (m_Params & 0x02) ? true : false; 
  }
  bool IsBilinearFiltering(void) const {
	// Se retorna flag de uso de efecto bilinear
    return (m_Params & 0x04) ? true : false;
  }
  bool IsColorKey(void) const {
	// Se retorna flag de uso de color key
    return (m_Params & 0x08) ? true : false;
  }
};

#endif // ~ #ifdef _CDRAWABLE_H_
