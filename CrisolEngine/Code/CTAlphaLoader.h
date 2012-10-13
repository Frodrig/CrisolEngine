///////////////////////////////////////////////////////////////////////////////
// CTAlphaLoader.h
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Carga desde un fichero de texto una estructura RGB.
// - Los sufijos seran:
//   * Alpha
//
// Notas:
///////////////////////////////////////////////////////////////////////////////
#ifndef _CTALPHALOADER_H_
#define _CTALPHALOADER_H_

// Pragmas <VC6 / Warnings sobre la stl>
//#pragma warning(disable:4786)

// Cabeceras
#ifndef _CTEXTDATALOADER_H_
#include "CTextDataLoader.h"
#endif
#ifndef _ICTALPHALOADER_H_
#include "iCTAlphaLoader.h"
#endif

// Definicion de clases / estructuras / espacios de nombres

// Clase CTAlphaLoader
class CTAlphaLoader: public CTextDataLoader,
                     public iCTAlphaLoader
{  
private:
  // Tipos
  // Clase base
  typedef CTextDataLoader Inherited;

public:
  // Constructor / destructor
  CTAlphaLoader(void) { }
  ~CTAlphaLoader(void) { Inherited::End(); }

public:
  // Carga de los datos
  bool LoadData(GraphDefs::sAlphaValue* const pValue);
};

#endif // ~ #ifdef _CTALPHALOADER_H_
