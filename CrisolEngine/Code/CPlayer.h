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
// CPlayer.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Clases:
// - CPlayer
//
// Descripcion:
// - Representa a la entidad jugador. Hereda de CCriature.
// - Basicamente la clase se encargara de disponer de un metodo de
//   inicializacion ofrecido para trabajar en conjunto con la ventana de 
//   interfaz para la creacion de personajes.
// - Una entidad CPlayer podra tener cero o mas observadores asociados
//   que recibiran notificacion cuando algun suceso que pueda verse reflejado
//   en ellos mismos, ocurra. Los observadores seran principalmente las
//   interfaces de inventario y la principal de juego.
// - Desde esta clase se recogera el evento de muerte de tal forma que si al
//   regreso del evento script de muerte la criatura sigue muerta, el juego
//   finalizara.
//
// Notas:
// - Cuando se espere notificacion de los observadores es muy importante tener
//   en cuenta que se debera de trabajar con la instancia CPlayer pues los
//   metodos que pudieran producir una notificacion estaran heredados de
//   CCriature como NO virtuales y sera esta clase la que los sobrecarge. Asi
//   que si CPlayer esta como instancia CCriature o CWorldEntity no se
//   recibira notificacion en los observadores cuando esta sea esperada.
///////////////////////////////////////////////////////////////////////////////
#ifndef _CPLAYER_H_
#define _CPLAYER_H_

// Pragmas <VC6 / Warnings sobre la stl>
#pragma warning(disable:4786)

// Cabeceras
#ifndef _CCRIATURE_H_
#include "CCriature.h"
#endif

// Defincion de clases / estructuras / espacios de nombres

// Clase CPlayer
class CPlayer: public CCriature
{
private:
  // Tipos
  // Clase base
  typedef CCriature Inherited;
  
public:
  // Constructor / destructor
  CPlayer(void) { }
  ~CPlayer(void) { End();  }

public:
  // Protocolos de inicializacion / fin
  bool Init(const std::string& szATSprite,
			const std::string& szName,
			const std::string& szATPortrait,	
			const std::string& szShadowImage,
		    const RulesDefs::sCriatureAttrib& Attribs);
  bool Init(const FileDefs::FileHandle& hFile,
		    dword& udOffset);
  void End(void);

public:
  // CCriature / Obtencion de informacion
  inline RulesDefs::eEntityType GetEntityType(void) const {
	ASSERT(Inherited::IsInitOk());
	// Se devuelve tipo de entidad
	return RulesDefs::PLAYER;
  }
};

#endif // ~ #ifdef _CPLAYER_H_
