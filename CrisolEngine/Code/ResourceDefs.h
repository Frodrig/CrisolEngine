///////////////////////////////////////////////////////////////////////////////
// ResourceDefs.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Define un espacio de nombres en el que se incluiran todas aquellos tipos
//   de datos que necesiten ser accesibles tanto en el interior de subsistema
//   de manejo de recursos, CResourceManager, como desde el exterior, por parte
//   de los objetos que hagan uso del interfaz del manejador de recursos.
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _RESOURCEDEFS_H_
#define _RESOURCEDEFS_H_

// Cabeceras
#include "SYSDefs.h"

namespace ResourceDefs
{
  // Tipos
  typedef word TextureHandle;      // Handle a textura
  typedef word WAVSoundHandle;     // Handle a sonido WAV
  typedef word AnimTemplateHandle; // Handle a animacion 

  // Tipos enumerados
  enum ResourceType { 
	// Tipo de recursos que pueden ser alojados en el manejador de recursos
    RT_TEXTURE = 0,       
    RT_WAVSOUNDBUFFER = 1,
    RT_WAVSOUND = 2,      
  }; 
};

#endif // ~ #ifdef _RESOURCEDEFS_H_
