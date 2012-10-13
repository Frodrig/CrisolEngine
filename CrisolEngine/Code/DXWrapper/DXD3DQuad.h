///////////////////////////////////////////////////////////////////////////////
// DX7Wrapper - DirectX 7 Wrapper
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

///////////////////////////////////////////////////////////////////////
// DXD3DQuad.h
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Esta clase representara un QUAD. Un Quad sera la unidad con la que
//   esta libreria tratara los objetos que deban de ser enviandos a Direct3D.
//   Cada objeto o sprite estara encerrado en un rectangulo o en una figura
//   que disponga de 4 vertices. Mediante un quad, definiremos esta figura
//   y la forma en la que las texturas seran pegadas a ella.
//     
// Notas:
// - Clase optimizada para DirectX 7.0.
// - Todas las clases DX deben de heredar de la clase base DXWrapper 
//   e implementar los metodos Init y Clean. 
// - Todas las estructuras, constantes, tipos enumerados, etc que
//   utilice la libreria de clases para Direct3D estaran en el
//   archivo de cabecera "DXD3DDefs.h".
// - Para poder trabajar con la funcion que transforma codigos de error
//   de Direct3D a cadenas de caracteres se incluye "DXD3DError.h".
///////////////////////////////////////////////////////////////////////
#ifndef __DXD3DQUAD_H_
#define __DXD3DQUAD_H_

// Includes
#include "DXWrapper.h"
#include "DXD3D.h"

// Clase DXD3DQuad
class DXD3DQuad: public DXWrapper
{
private:  
  // Estructuras
  struct VertexInfo { 
	// Estructura asociada a un vertice  
    byte ubRValue;     // Valor rojo
    byte ubGValue;     // Valor verde
    byte ubBValue;     // Valor azul
    byte ubAlphaValue; // Valor alpha
  };

private:
  // Variables de miembro
  VertexInfo  m_VertexInfo[4]; // Informacion adicional asociada
  D3DTLVERTEX m_Quad[4];       // Quads
  
public:
  // Protocolo de inicio
  DXD3DQuad(void) { Init(); }
protected:
  void Init(void);
  
public:
  // Operaciones de establecimiento de valores
  inline void SetXYPos(const DXD3DDefs::DXD3DVertex& Vertex, 
                       const int& nXPos, const int& nYPos) { 
    // Establece el valor de la coordenada XY para el vertice Vertex
    m_Quad[Vertex].sx = float(nXPos);
    m_Quad[Vertex].sy = float(nYPos);
  }

  void SetVertexColor(const DXD3DDefs::DXD3DVertex& Vertex, 
                      const byte ubR = 255, const byte ubG = 255, 
                      const byte ubB = 255, const byte ubAlpha = 255);

  void SetTextureRegion(const float uV0, const float vV0,
                        const float uV1, const float vV1,
                        const float uV2, const float vV2,
                        const float uV3, const float vV3);

public:
  // Operaciones de consulta sobre vertices individuales
  inline int GetXPos(const DXD3DDefs::DXD3DVertex& Vertex) const { 
    // Devuelve la coordenada x del vertice Vertex
    return int(m_Quad[Vertex].sx); 
  }
  inline int GetYPos(const DXD3DDefs::DXD3DVertex& Vertex) const { 
    // Devuelve la coordenada y del vertice Vertex
    return int(m_Quad[Vertex].sy); 
  }
  void GetVertexColor(const DXD3DDefs::DXD3DVertex& Vertex, 
                      byte& R, byte& G, byte& B,
                      byte& Alpha);

public:
  // Obtencion de todos los quads
  inline D3DTLVERTEX* const GetVertex(void) { 
	return m_Quad; 
  }

private:
  // Metodo de apoyo  
  void Clean(void) { };  
}; 
#endif
