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
// DXDDError.cpp
// Fernando Rodríguez 
// <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
//
// Descripcion:
// - Para mas informacion consultar DXDDError.h.
///////////////////////////////////////////////////////////////////////

#include "DXDDError.h"

//////////////////////////////////////////////////////////////////////
// Descripcion:
// - Convierte un codigo de error de las DirectDraw a una cadena
//   BIRString.
// Parametros:
// - CodeError: Codigo de error producido.
// - asStrError: Referencia al objeto string en donde depositar la
//   cadena con el nombre del error.
// Devuelve:
// - asStrError: Contendra string con el codigo del error.
// Notas:
// - Esta funcion cubre todos los posibles errores de cada una de
//   las interfaces presentes en DirectSound v7.0.
// - En caso de que no se reconozca el error se devolvera el codigo
//   "DDERR_?".
////////////////////////////////////////////////////////////////////// 
void DXDDError::DXDDErrorToString(const HRESULT CodeError, 
								  std::string& asStrError)
{
  // Evalua codigo de error recibido
  switch (CodeError)
  {
    case DDERR_ALREADYINITIALIZED: 
      asStrError = "DDERR_ALREADYINITIALIZED"; break;
    case DDERR_BLTFASTCANTCLIP: 
      asStrError = "DDERR_BLTFASTCANTCLIP"; break;
    case DDERR_CANNOTATTACHSURFACE: 
      asStrError = "DDERR_CANNOTATTACHSURFACE"; break;
    case DDERR_CANNOTDETACHSURFACE: 
      asStrError = "DDERR_CANNOTDETACHSURFACE"; break;
    case DDERR_CANTCREATEDC: 
      asStrError = "DDERR_CANTCREATEDC"; break;
    case DDERR_CANTDUPLICATE: 
      asStrError = "DDERR_CANTDUPLICATE"; break;
    case DDERR_CANTLOCKSURFACE: 
      asStrError = "DDERR_CANTLOCKSURFACE"; break;
    case DDERR_CANTPAGELOCK: 
      asStrError = "DDERR_CANTPAGELOCK"; break;
    case DDERR_CANTPAGEUNLOCK: 
      asStrError = "DDERR_CANTPAGEUNLOCK"; break;
    case DDERR_CLIPPERISUSINGHWND: 
      asStrError = "DDERR_CLIPPERISUSINGHWND"; break;
    case DDERR_COLORKEYNOTSET: 
      asStrError = "DDERR_COLORKEYNOTSET"; break;
    case DDERR_CURRENTLYNOTAVAIL: 
      asStrError = "DDERR_CURRENTLYNOTAVAIL"; break;
    case DDERR_DDSCAPSCOMPLEXREQUIRED: 
      asStrError = "DDERR_DDSCAPSCOMPLEXREQUIRED"; break;
    case DDERR_DCALREADYCREATED: 
      asStrError = "DDERR_DCALREADYCREATED"; break;
    case DDERR_DEVICEDOESNTOWNSURFACE: 
      asStrError = "DDERR_DEVICEDOESNTOWNSURFACE"; break;
    case DDERR_DIRECTDRAWALREADYCREATED: 
      asStrError = "DDERR_DIRECTDRAWALREADYCREATED"; break;
    case DDERR_EXCEPTION: 
      asStrError = "DDERR_EXCEPTION"; break;
    case DDERR_EXCLUSIVEMODEALREADYSET: 
      asStrError = "DDERR_EXCLUSIVEMODEALREADYSET"; break;
    case DDERR_EXPIRED: 
      asStrError = "DDERR_EXPIRED"; break;
    case DDERR_GENERIC: 
      asStrError = "DDERR_GENERIC"; break;
    case DDERR_HEIGHTALIGN: 
      asStrError = "DDERR_HEIGHTALIGN"; break;
    case DDERR_HWNDALREADYSET: 
      asStrError = "DDERR_HWNDALREADYSET"; break;
    case DDERR_HWNDSUBCLASSED: 
      asStrError = "DDERR_HWNDSUBCLASSED"; break;
    case DDERR_IMPLICITLYCREATED: 
      asStrError = "DDERR_IMPLICITLYCREATED"; break;
    case DDERR_INCOMPATIBLEPRIMARY: 
      asStrError = "DDERR_INCOMPATIBLEPRIMARY"; break;
    case DDERR_INVALIDCAPS: 
      asStrError = "DDERR_INVALIDCAPS"; break;
    case DDERR_INVALIDCLIPLIST: 
      asStrError = "DDERR_INVALIDCLIPLIST"; break;
    case DDERR_INVALIDDIRECTDRAWGUID: 
      asStrError = "DDERR_INVALIDDIRECTDRAWGUID"; break;
    case DDERR_INVALIDMODE: 
      asStrError = "DDERR_INVALIDMODE"; break;
    case DDERR_INVALIDOBJECT: 
      asStrError = "DDERR_INVALIDOBJECT"; break;
    case DDERR_INVALIDPARAMS: 
      asStrError = "DDERR_INVALIDPARAMS"; break;
    case DDERR_INVALIDPIXELFORMAT: 
      asStrError = "DDERR_INVALIDPIXELFORMAT"; break;
    case DDERR_INVALIDPOSITION: 
      asStrError = "DDERR_INVALIDPOSITION"; break;
    case DDERR_INVALIDRECT: 
      asStrError = "DDERR_INVALIDRECT"; break;
    case DDERR_INVALIDSTREAM: 
      asStrError = "DDERR_INVALIDSTREAM"; break;
    case DDERR_INVALIDSURFACETYPE: 
      asStrError = "DDERR_INVALIDSURFACETYPE"; break;
    case DDERR_LOCKEDSURFACES: 
      asStrError = "DDERR_LOCKEDSURFACES"; break;
    case DDERR_MOREDATA: 
      asStrError = "DDERR_MOREDATA"; break;
    case DDERR_NEWMODE: 
      asStrError = "DDERR_NEWMODE"; break;
    case DDERR_NO3D: 
      asStrError = "DDERR_NO3D"; break;
    case DDERR_NOALPHAHW: 
      asStrError = "DDERR_NOALPHAHW"; break;
    case DDERR_NOBLTHW: 
      asStrError = "DDERR_NOBLTHW"; break;
    case DDERR_NOCLIPLIST: 
      asStrError = "DDERR_NOCLIPLIST"; break;
    case DDERR_NOCLIPPERATTACHED: 
      asStrError = "DDERR_NOCLIPPERATTACHED"; break;
    case DDERR_NOCOLORCONVHW: 
      asStrError = "DDERR_NOCOLORCONVHW"; break;
    case DDERR_NOCOLORKEY: 
      asStrError = "DDERR_NOCOLORKEY"; break;
    case DDERR_NOCOLORKEYHW: 
      asStrError = "DDERR_NOCOLORKEYHW"; break;
    case DDERR_NOCOOPERATIVELEVELSET: 
      asStrError = "DDERR_NOCOOPERATIVELEVELSET"; break;
    case DDERR_NODC: 
      asStrError = "DDERR_NODC"; break;
    case DDERR_NODDROPSHW: 
      asStrError = "DDERR_NODDROPSHW"; break;
    case DDERR_NODIRECTDRAWHW: 
      asStrError = "DDERR_NODIRECTDRAWHW"; break;
    case DDERR_NODIRECTDRAWSUPPORT: 
      asStrError = "DDERR_NODIRECTDRAWSUPPORT"; break;
    case DDERR_NODRIVERSUPPORT: 
      asStrError = "DDERR_NODRIVERSUPPORT"; break;
    case DDERR_NOEMULATION: 
      asStrError = "DDERR_NOEMULATION"; break;
    case DDERR_NOEXCLUSIVEMODE: 
      asStrError = "DDERR_NOEXCLUSIVEMODE"; break;
    case DDERR_NOFLIPHW: 
      asStrError = "DDERR_NOFLIPHW"; break;
    case DDERR_NOFOCUSWINDOW: 
      asStrError = "DDERR_NOFOCUSWINDOW"; break;
    case DDERR_NOGDI: 
      asStrError = "DDERR_NOGDI"; break;
    case DDERR_NOHWND: 
      asStrError = "DDERR_NOHWND"; break;
    case DDERR_NOMIPMAPHW: 
      asStrError = "DDERR_NOMIPMAPHW"; break;
    case DDERR_NOMIRRORHW: 
      asStrError = "DDERR_NOMIRRORHW"; break;
    case DDERR_NOMONITORINFORMATION: 
      asStrError = "DDERR_NOMONITORINFORMATION"; break;
    case DDERR_NONONLOCALVIDMEM:
      asStrError = "DDERR_NONONLOCALVIDMEM"; break;
    case DDERR_NOOPTIMIZEHW: 
      asStrError = "DDERR_NOOPTIMIZEHW"; break;
    case DDERR_NOOVERLAYDEST: 
      asStrError = "DDERR_NOOVERLAYDEST"; break;
    case DDERR_NOOVERLAYHW: 
      asStrError = "DDERR_NOOVERLAYHW"; break;
    case DDERR_NOPALETTEATTACHED: 
      asStrError = "DDERR_NOPALETTEATTACHED"; break;
    case DDERR_NOPALETTEHW: 
      asStrError = "DDERR_NOPALETTEHW"; break;
    case DDERR_NORASTEROPHW: 
      asStrError = "DDERR_NORASTEROPHW"; break;
    case DDERR_NOROTATIONHW: 
      asStrError = "DDERR_NOROTATIONHW"; break;
    case DDERR_NOSTEREOHARDWARE: 
      asStrError = "DDERR_NOSTEREOHARDWARE"; break;
    case DDERR_NOSTRETCHHW: 
      asStrError = "DDERR_NOSTRETCHHW"; break;
    case DDERR_NOSURFACELEFT: 
      asStrError = "DDERR_NOSURFACELEFT"; break;
    case DDERR_NOT4BITCOLOR: 
      asStrError = "DDERR_NOT4BITCOLOR"; break;
    case DDERR_NOT4BITCOLORINDEX: 
      asStrError = "DDERR_NOT4BITCOLORINDEX"; break;
    case DDERR_NOT8BITCOLOR: 
      asStrError = "DDERR_NOT8BITCOLOR"; break;
    case DDERR_NOTAOVERLAYSURFACE: 
      asStrError = "DDERR_NOTAOVERLAYSURFACE"; break;
    case DDERR_NOTEXTUREHW: 
      asStrError = "DDERR_NOTEXTUREHW"; break;
    case DDERR_NOTFLIPPABLE: 
      asStrError = "DDERR_NOTFLIPPABLE"; break;
    case DDERR_NOTFOUND: 
      asStrError = "DDERR_NOTFOUND"; break;
    case DDERR_NOTINITIALIZED: 
      asStrError = "DDERR_NOTINITIALIZED"; break;
    case DDERR_NOTLOADED: 
      asStrError = "DDERR_NOTLOADED"; break;
    case DDERR_NOTLOCKED: 
      asStrError = "DDERR_NOTLOCKED"; break;
    case DDERR_NOTPAGELOCKED: 
      asStrError = "DDERR_NOTPAGELOCKED"; break;
    case DDERR_NOTPALETTIZED: 
      asStrError = "DDERR_NOTPALETTIZED"; break;
    case DDERR_NOVSYNCHW: 
      asStrError = "DDERR_NOVSYNCHW"; break;
    case DDERR_NOZBUFFERHW: 
      asStrError = "DDERR_NOZBUFFERHW"; break;
    case DDERR_NOZOVERLAYHW: 
      asStrError = "DDERR_NOZOVERLAYHW"; break;
    case DDERR_OUTOFCAPS: 
      asStrError = "DDERR_OUTOFCAPS"; break;
    case DDERR_OUTOFMEMORY: 
      asStrError = "DDERR_OUTOFMEMORY"; break;
    case DDERR_OUTOFVIDEOMEMORY: 
      asStrError = "DDERR_OUTOFVIDEOMEMORY"; break;
    case DDERR_OVERLAPPINGRECTS: 
      asStrError = "DDERR_OVERLAPPINGRECTS"; break;
    case DDERR_OVERLAYCANTCLIP: 
      asStrError = "DDERR_OVERLAYCANTCLIP"; break;
    case DDERR_OVERLAYCOLORKEYONLYONEACTIVE:
      asStrError = "DDERR_OVERLAYCOLORKEYONLYONEACTIVE"; break;
    case DDERR_OVERLAYNOTVISIBLE: 
      asStrError = "DDERR_OVERLAYNOTVISIBLE"; break;
    case DDERR_PALETTEBUSY: 
      asStrError = "DDERR_PALETTEBUSY"; break;
    case DDERR_PRIMARYSURFACEALREADYEXISTS: 
      asStrError = "DDERR_PRIMARYSURFACEALREADYEXISTS"; break;
    case DDERR_REGIONTOOSMALL: 
      asStrError = "DDERR_REGIONTOOSMALL"; break;
    case DDERR_SURFACEALREADYATTACHED: 
      asStrError = "DDERR_SURFACEALREADYATTACHED"; break;
    case DDERR_SURFACEALREADYDEPENDENT: 
      asStrError = "DDERR_SURFACEALREADYDEPENDENT"; break;
    case DDERR_SURFACEBUSY: 
      asStrError = "DDERR_SURFACEBUSY"; break;
    case DDERR_SURFACEISOBSCURED: 
      asStrError = "DDERR_SURFACEISOBSCURED"; break;
    case DDERR_SURFACELOST: 
      asStrError = "DDERR_SURFACELOST"; break;
    case DDERR_SURFACENOTATTACHED: 
      asStrError = "DDERR_SURFACENOTATTACHED"; break;
    case DDERR_TESTFINISHED: 
      asStrError = "DDERR_TESTFINISHED"; break;
    case DDERR_TOOBIGHEIGHT: 
      asStrError = "DDERR_TOOBIGHEIGHT"; break;
    case DDERR_TOOBIGSIZE: 
      asStrError = "DDERR_TOOBIGSIZE"; break;
    case DDERR_TOOBIGWIDTH: 
      asStrError = "DDERR_TOOBIGWIDTH"; break;
    case DDERR_UNSUPPORTED: 
      asStrError = "DDERR_UNSUPPORTED"; break;
    case DDERR_UNSUPPORTEDFORMAT: 
      asStrError = "DDERR_UNSUPPORTEDFORMAT"; break;
    case DDERR_UNSUPPORTEDMASK: 
      asStrError = "DDERR_UNSUPPORTEDMASK"; break;
    case DDERR_UNSUPPORTEDMODE: 
      asStrError = "DDERR_UNSUPPORTEDMODE"; break;
    case DDERR_VERTICALBLANKINPROGRESS: 
      asStrError = "DDERR_VERTICALBLANKINPROGRESS"; break;
    case DDERR_VIDEONOTACTIVE: 
      asStrError = "DDERR_VIDEONOTACTIVE"; break;
    case DDERR_WASSTILLDRAWING: 
      asStrError = "DDERR_WASSTILLDRAWING"; break;
    case DDERR_WRONGMODE: 
      asStrError = "DDERR_WRONGMODE"; break;
    case DDERR_XALIGN: 
      asStrError = "DDERR_XALIGN"; break;
    default:
      asStrError = "DDERR_?"; break;
  }; // del swich
}




