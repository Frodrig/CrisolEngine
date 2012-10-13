///////////////////////////////////////////////////////////////////////////////
// CIsoSprite.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Deriva de CIsoSprite y añade funcionalidades propias de los sprites con los
//   que trabajara el motor isometrico.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CISOSPRITE_H_
#define _CISOSPRITE_H_

// Cabeceras
#include "CSprite.h"

// Clase CIsoSprite
class CIsoSprite: public CSprite
{
private:
  // Vbles de miembro  
 
public:
  // Constructor / destructor
  CIsoSprite(void): CSprite() { }
  ~CIsoSprite(void) { CSprite::~CSprite(); }

public:
  // Operaciones de movimiento de los sprites en el entorno isometrico
public:	
  // Establecimiento y obtencion de offsets
  /*
  inline void SetXOffset(const sword swXOffset) { m_swXOffset = swXOffset; }
  inline void SetYOffset(const sword swYOffset) { m_swYOffset = swYOffset; }
  inline sword GetXOffset(void) const { return m_swXOffset; }
  inline sword GetYOffset(void) const { return m_swYOffset; }
  */
};  

#endif // ~ #ifdef _CISOSPRITE_H_
