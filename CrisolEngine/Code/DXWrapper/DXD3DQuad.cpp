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
// DXD3DQuad.cpp
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXD3DQuad.h.
///////////////////////////////////////////////////////////////////////
#include "DXD3DQuad.h"

#include "DXD3DDefs.h"
#include "DXD3DError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia, estableciendo las coordenadas en donde
//   se mapeara la textura. Asi mismo, las coordenadas en donde se
//   hallara los vertices seran reseteadas a 0.
// Parametros:
// Devuelve:
// Notas:
// - La informacion que se inicializa es la siguiente:
//   * sx, sy, sz. Posicion x,y del vertice. Por defecto, todos estan
//     en la 0,0,0
//   * rhw. Valor sobre iluminacion
//   * color para el vertice. Por defecto todos estan en su color 
//     original.
//   * specular. Colo especular. No se tendra en cuenta (da brillo).
//   * tu,tv. Coordenadas de la textura a la que esta asociado el
//     quad que se tomaran. Por defecto se tomara toda la textura.
////////////////////////////////////////////////////////////////////// 
void DXD3DQuad::Init(void)
{  
  // Vertice 0
  m_Quad[DXD3DDefs::VERTEX0].sx = 0.0f;
  m_Quad[DXD3DDefs::VERTEX0].sy = 0.0f;
  m_Quad[DXD3DDefs::VERTEX0].sz = 0.0f;
  m_Quad[DXD3DDefs::VERTEX0].rhw = 1.0f;
  m_Quad[DXD3DDefs::VERTEX0].color = RGBA_MAKE(255, 255, 255, 255);
  m_VertexInfo[DXD3DDefs::VERTEX0].ubRValue = 255;
  m_VertexInfo[DXD3DDefs::VERTEX0].ubGValue = 255;
  m_VertexInfo[DXD3DDefs::VERTEX0].ubBValue = 255;
  m_VertexInfo[DXD3DDefs::VERTEX0].ubAlphaValue = 255;
  m_Quad[DXD3DDefs::VERTEX0].specular = 0;
  m_Quad[DXD3DDefs::VERTEX0].tu = 0.0f;
  m_Quad[DXD3DDefs::VERTEX0].tv = 0.0f;  

  // Vertice 1
  m_Quad[DXD3DDefs::VERTEX1].sx = 0.0f;
  m_Quad[DXD3DDefs::VERTEX1].sy = 0.0f;
  m_Quad[DXD3DDefs::VERTEX1].sz = 0.0f;
  m_Quad[DXD3DDefs::VERTEX1].rhw = 1.0f;
  m_Quad[DXD3DDefs::VERTEX1].color = RGBA_MAKE(255, 255, 255, 255);
  m_VertexInfo[DXD3DDefs::VERTEX1].ubRValue = 255;
  m_VertexInfo[DXD3DDefs::VERTEX1].ubGValue = 255;
  m_VertexInfo[DXD3DDefs::VERTEX1].ubBValue = 255;
  m_VertexInfo[DXD3DDefs::VERTEX1].ubAlphaValue = 255;
  m_Quad[DXD3DDefs::VERTEX1].specular = 0;
  m_Quad[DXD3DDefs::VERTEX1].tu = 1.0f;
  m_Quad[DXD3DDefs::VERTEX1].tv = 0.0f;

  // Vertice 2
  m_Quad[DXD3DDefs::VERTEX2].sx = 0.0f;
  m_Quad[DXD3DDefs::VERTEX2].sy = 0.0f;
  m_Quad[DXD3DDefs::VERTEX2].sz = 0.0f;
  m_Quad[DXD3DDefs::VERTEX2].rhw = 1.0f;
  m_Quad[DXD3DDefs::VERTEX2].color = RGBA_MAKE(255, 255, 255, 255);
  m_VertexInfo[DXD3DDefs::VERTEX2].ubRValue = 255;
  m_VertexInfo[DXD3DDefs::VERTEX2].ubGValue = 255;
  m_VertexInfo[DXD3DDefs::VERTEX2].ubBValue = 255;
  m_VertexInfo[DXD3DDefs::VERTEX2].ubAlphaValue = 255;
  m_Quad[DXD3DDefs::VERTEX2].specular = 0;
  m_Quad[DXD3DDefs::VERTEX2].tu = 0.0f;
  m_Quad[DXD3DDefs::VERTEX2].tv = 1.0f;

  // Vertice 3
  m_Quad[DXD3DDefs::VERTEX3].sx = 0.0f;
  m_Quad[DXD3DDefs::VERTEX3].sy = 0.0f;
  m_Quad[DXD3DDefs::VERTEX3].sz = 0.0f;
  m_Quad[DXD3DDefs::VERTEX3].rhw = 1.0f;
  m_Quad[DXD3DDefs::VERTEX3].color = RGBA_MAKE(255, 255, 255, 255);
  m_VertexInfo[DXD3DDefs::VERTEX3].ubRValue = 255;
  m_VertexInfo[DXD3DDefs::VERTEX3].ubGValue = 255;
  m_VertexInfo[DXD3DDefs::VERTEX3].ubBValue = 255;
  m_VertexInfo[DXD3DDefs::VERTEX3].ubAlphaValue = 255;
  m_Quad[DXD3DDefs::VERTEX3].specular = 0;
  m_Quad[DXD3DDefs::VERTEX3].tu = 1.0f;
  m_Quad[DXD3DDefs::VERTEX3].tv = 1.0f;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de establecer la zona de la textura asociada, que se
//   tomara. Los valores u,v estan asociados a cada vertice y deben
//   de tomar valores entre 0.0f y 1.0f, este ultimo excluido.
//   Para calcular una zona, bastara pasar la posicion de esa zona en
//   la textura y dividir por la dimension (ancho o alto) que 
//   corresponda. 
// Parametros:
// - uV0,vV0,...uV3,vV3. Son los valores u,v de cada vertice.
// Devuelve:
// Notas:
// - Un ejemplo de uso seria:
//   * Si se posee una textura de 256x256 y se quiere tomar solo la 
//     zona V0(10,30), V1(40,30), V3(10,80) y V4(40,80), se deberia
//     de llamar a este metodo asi:
//     SetTextureRegion(10/256, 30/256,
//                      40/256, 30/256,
//                      10/256, 80/256,
//                      40/256, 80/256)
////////////////////////////////////////////////////////////////////// 
void DXD3DQuad::SetTextureRegion(const float uV0, const float vV0,
                                 const float uV1, const float vV1,
                                 const float uV2, const float vV2,
                                 const float uV3, const float vV3)
{
  // SOLO si los valores son correctos
  DXASSERT(((uV0 >= 0.0f) && (uV0 < 1.0f)) != 0);
  DXASSERT(((uV1 >= 0.0f) && (uV1 < 1.0f)) != 0);
  DXASSERT(((uV2 >= 0.0f) && (uV2 < 1.0f)) != 0);
  DXASSERT(((uV3 >= 0.0f) && (uV3 < 1.0f)) != 0);

  // Se procede a asociar los valores
  m_Quad[DXD3DDefs::VERTEX0].tu = uV0;
  m_Quad[DXD3DDefs::VERTEX0].tv = vV0;
  m_Quad[DXD3DDefs::VERTEX1].tu = uV1;
  m_Quad[DXD3DDefs::VERTEX1].tv = vV1;
  m_Quad[DXD3DDefs::VERTEX2].tu = uV2;
  m_Quad[DXD3DDefs::VERTEX2].tv = vV2;
  m_Quad[DXD3DDefs::VERTEX3].tu = uV3;
  m_Quad[DXD3DDefs::VERTEX3].tv = vV3;  
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Establece el color RGBA en el vertice deseado. Ademas se 
//   almacenaran los valores en las vbles de miembro.
// Parametros:
// - Vertex. Vertice sobre el que actuar.
// - ubR, ubG, ubB, ubAlpha. Componentes RGBA.
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
void DXD3DQuad::SetVertexColor(const DXD3DDefs::DXD3DVertex& Vertex, 
                               const byte ubR, 
							   const byte ubG, 
							   const byte ubB, 
                               const byte ubAlpha)
{
  // Establece las componentes del color para el vertice Vertex
  m_Quad[Vertex].color = RGBA_MAKE(ubR, ubG, ubB, ubAlpha);

  // Guarda los nuevos valores
  m_VertexInfo[Vertex].ubRValue = ubR;
  m_VertexInfo[Vertex].ubGValue = ubG;
  m_VertexInfo[Vertex].ubBValue = ubB;
  m_VertexInfo[Vertex].ubAlphaValue = ubAlpha;
}

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Devuelve por parametro los valores de color asociados al vertice
//   Vertex.
// Parametros:
// - Vertex. Vertice sobre el que actuar.
// - R, G, B, Alpha. Direccion de las vbles en donde alojar los
//   valores a devolver.
// Devuelve:
// Notas:
////////////////////////////////////////////////////////////////////// 
void DXD3DQuad::GetVertexColor(const DXD3DDefs::DXD3DVertex& Vertex, 
                               byte& R, 
							   byte& G, 
							   byte& B,
                               byte& Alpha)
{  
  // Se devuelven los valores
  R = m_VertexInfo[Vertex].ubRValue;
  G = m_VertexInfo[Vertex].ubGValue;
  B = m_VertexInfo[Vertex].ubBValue;
  Alpha = m_VertexInfo[Vertex].ubAlphaValue;  
}
