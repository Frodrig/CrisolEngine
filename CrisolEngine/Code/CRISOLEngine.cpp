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
// CRISOLEngine.cpp
// Autor: Fernando Rodríguez Martínez 
//        <frodrig76@gmail.com | @_frodrig | www.frodrig.com>
// 
// Descripcion:
// - Modulo de inicio de Windows y puesta en marcha del engine.
///////////////////////////////////////////////////////////////////////////////

//============================================================================
// ARCHIVOS DE CABECERA
//============================================================================
#ifndef _CCRISOLENGINE_H_
#include "CCRISOLEngine.h"
#endif
#ifndef _SYSDEFS_H_
#include "SYSDefs.h"
#endif

#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include "CLogger.h"

// Definiciones y macros
#define APPNAME  "CRISOLEngine" // Nombre de la aplicacion
#define APPTITLE "CRISOLEngine" // Titulo de la aplicacion

// Flags
const bool APPFLAG_EXCEPTION_HANDLER = true; // Activa handle de excepcion

//============================================================================
// DEFINICION DE VBLES STATIC
//============================================================================
static HWND       GlobalhWnd;      // Handle a la ventana de la aplicacion
static HINSTANCE  GlobalhInstance; // Handle HInstance
static bool       bPause = false;  // ¿Esta la apliacion en pausa?

//============================================================================
// DEFINICION DE FUNCIONES DE APOYO DE LA APLICACION
//============================================================================
static void APPInit(void);
static void APPUpdateFrame(void);
static void APPError(char *text, ...);
static void APPShutDown(void);
static void APPQuit(void);

//============================================================================
// DEFINICION DE FUNCIONES WINDOWS
//============================================================================
static int EvaluateException(unsigned long);
static void ExceptionHandler(unsigned long);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static HRESULT InitApp(HINSTANCE, int);
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int);

//============================================================================
// IMPLEMENTACION DE FUNCIONES
//============================================================================

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Inicializa la instancia CCRISOLEngine.
// Parametros:
// Devuelve:
// Notas:
// - En caso de que exista algun tipo de problema, se encargara de cerrar
//   la apliacion.
///////////////////////////////////////////////////////////////////////////////
static void APPInit(void)
{  
  // Inicializa CCRISOLEngine  
  if (!CCRISOLEngine::GetInstance()->Init("config.cri", 
                                          GlobalhInstance, GlobalhWnd)) {
    // Hay problemas
    APPError("\nHan existido problemas al inicializar el engine.\n");
    APPError("\nCompruebe el archivo de configuracion\n");
    APPShutDown();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Abandona la aplicacion al encontrarse un error fatal. La funcion funciona
//   igual que un printf. Antes de abandonar, muestra el mensaje de error
//   por la ventana del debugger.
// Parametros:
// - *text: Cadena de caracteres a escribir, junto a sus codigos de
//    sustitucion.
// ...:Resto de parametros.
// Devuelve:
// Notas:
///////////////////////////////////////////////////////////////////////////////
static void APPError(char *text, ...)
{
  // Vbles
  static char szTextBuffer[1024]; // Buffer con el texto a mostrar
  va_list     args;               // Lista de argumentos
	
  // Se forma la cadena a mostrar
  va_start(args, text);
  vsprintf(szTextBuffer, text, args);
  va_end(args);

  // Se muestra la informacion por el debugger
  OutputDebugString(szTextBuffer);
  std::fstream s;
  s.open("error.txt");
  s << szTextBuffer;
  s.close();

  // Se apaga la aplicacion y se sale con error fatal
  APPShutDown();
  exit(1);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de ordenar las actualizaciones precisas a traves del modulo
//   principal del engine.
// Parametros:
// Devuelve:
// Notas:
// - Esta funcion es llamada desde el bucle principal
///////////////////////////////////////////////////////////////////////////////
static void APPUpdateFrame(void)
{  
  ASSERT(CCRISOLEngine::GetInstance()->IsInitOk());
  if (!CCRISOLEngine::GetInstance()->UpdateFrame()) { APPQuit(); }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Funcion que se encarga de finalizar todas las tareas que se encuentren
//   activas. En particular, se encarga de finalizar la instancia a 
//   CCRISOLEngine.
// Parametros:
// Devuelve:
// Notas:
// - Esta funcion es llamada desde APPQuit antes de abandonar la aplicacion.
///////////////////////////////////////////////////////////////////////////////
static void APPShutDown(void)
{
  // Finaliza CCRISOLEngine
  CCRISOLEngine::GetInstance()->End();
  CCRISOLEngine::DestroyInstance();
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de realizar las operaciones precisas para cerrar la aplicacion
//   y salir al sistema operativo. Internamente, llamara a otra funcion que
//   sera la que se encargara de finalizar todas las tareas. Despues llamara
//   a la funcion exit con el parametro 0 indicando salida correcta.
// Parametros:
// Modifica:
// Devuelve:
// Notas:
// - Esta funcion debera de ser llamada siempre que se desee abandonar 
//   la aplicacion desde un determinado punto.
///////////////////////////////////////////////////////////////////////////////
static void APPQuit(void)
{
  // Llama a la funcion para finalizar todas las tareas y luego sale
  APPShutDown();
  exit(0);
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Comprueba el tipo de excepcion Windows que ha ocurrido y actua en
//   consecuencia.
// Parametros:
// - exception: Codigo de la excepcion que se ha producido.
// Devuelve:
// - EXCEPTION_CONTINUE_SEARCH: Se deja en responsabilidad de Windows el
//   el tratamiento de la excepcion.
// - EXCEPTION_EXECUTE_HANDLER: Se deja en responsabilidad del programador el
//   tratamiento de la excepcion.
// Notas:
///////////////////////////////////////////////////////////////////////////////
static int EvaluateException(unsigned long exception)
{
  // Excepciones que no se tendran en cuenta para ser tratadas
  // por el programador
  if (STATUS_INTEGER_OVERFLOW == exception ||
      STATUS_FLOAT_UNDERFLOW == exception  ||
      STATUS_FLOAT_OVERFLOW == exception)
  {    
    return EXCEPTION_CONTINUE_SEARCH;
  }

  // El resto de excepciones se trataran si se esta activo el flag de
  // ejecucion del handler de excepcion.
  if (APPFLAG_EXCEPTION_HANDLER)
  {
    // Esta activo.
    // Se ejecutara el handle de excepcion
    return EXCEPTION_EXECUTE_HANDLER;
  }
  else
  {
    // No esta activo. 
    // Si el debugger esta activo intentara atrapar la excepcion
    return EXCEPTION_CONTINUE_SEARCH;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Handle definido por el programador para capturar las excepciones de
//   Windows que se vayan sucediendo. Solo se activara si esta definido el
//   flag APPFLAG_EXCEPTION_HANDLER. Para poder activarlo o desactivarlo
//   es necesario acudir al archivo de cabecera sys_defs. Una vez localizada 
//   la excepcion se llamara a APPError().
// Parametros:
// - exception: Codigo de la excepcion que ha ocurrido.
// Devuelve:
// Notas:
// - Este metodo se llama desde dentro de __except siempre y cuando, al
//   detectarse una excepcion, la funcion EvaluateException devuelva como
//   codigo de retorno EXCEPTION_EXECUTE_HANDLER.
///////////////////////////////////////////////////////////////////////////////
static void ExceptionHandler(unsigned long exception)
{
  // Se evalua el tipo de excepcion que ha ocurrido y
  // se construye el mensaje que sera mostrado al exterior
  switch (exception) {
	case EXCEPTION_ACCESS_VIOLATION: 
	  APPError("Excepcion Crítica: Access Violation\n"); 
	  break;

	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
	  APPError("Excepcion Crítica: Array Bounds Exceeded\n"); 
	  break;

	case EXCEPTION_BREAKPOINT:
	  APPError("Excepcion Crítica: Breakpoint\n"); 
	  break;

	case EXCEPTION_DATATYPE_MISALIGNMENT:
	  APPError("Excepcion Crítica: Datatype Misalignment\n"); 
	  break;

	case EXCEPTION_FLT_DENORMAL_OPERAND:
	  APPError("Excepcion Crítica: Float Denormal Operand\n"); 
	  break;
	
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
	  APPError("Excepcion Crítica: Float Divide By Zero\n"); 
	  break;
	
	case EXCEPTION_FLT_INEXACT_RESULT:
	  APPError("Excepcion Crítica: Float Inexact Result\n"); 
	  break;
	
	case EXCEPTION_FLT_INVALID_OPERATION:
	  APPError("Excepcion Crítica: Float Invalid Operation\n"); 
	  break;
	
	case EXCEPTION_FLT_OVERFLOW:
	  APPError("Excepcion Crítica: Float Overflow\n"); 
	  break;
	
	case EXCEPTION_FLT_STACK_CHECK:
	  APPError("Excepcion Crítica: Float Stack Check\n"); 
	  break;
	
	case EXCEPTION_FLT_UNDERFLOW:
	  APPError("Excepcion Crítica: Float Underflow\n"); 
	  break;
	
	case EXCEPTION_ILLEGAL_INSTRUCTION:
	  APPError("Excepcion Crítica: Illegal Instruction\n"); 
	  break;
	
	case EXCEPTION_IN_PAGE_ERROR:
	  APPError("Excepcion Crítica: In-page Error\n"); 
	  break;
	
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
	  APPError("Excepcion Crítica: Integer Divide By Zero\n"); 
	  break;
	
	case EXCEPTION_INT_OVERFLOW:
	  APPError("Excepcion Crítica: Integer Overflow\n"); 
	  break;
	
	case EXCEPTION_INVALID_DISPOSITION:
	  APPError("Excepcion Crítica: Invalid Disposition\n"); 
	  break;
	
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
	  APPError("Excepcion Crítica: Noncontinuable Exception\n"); 
	  break;
	
	case EXCEPTION_PRIV_INSTRUCTION:
	  APPError("Excepcion Crítica: Private Instruction\n"); 
	  break;
	
	case EXCEPTION_SINGLE_STEP:
	  APPError("Excepcion Crítica: Single-step\n"); 
	  break;
	
	case EXCEPTION_STACK_OVERFLOW:
		APPError("Excepcion Crítica: Stack Overflow\n"); 
	  break;
	
	default:
	  APPError("Excepcion Crítica: Desconocida: %x\n", exception); 
	  break;
  };
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Aqui se podran recoger los mensajes que genere Windows y tratarlos
//   de forma individualizada.
// Parametros:
// - hWnd: Handle a la ventana de Windows que produce el mensaje.
// - message: Mensaje que se ha generado.
// - wParam: Primer parametro
// - lParam: Segundo parametro.
// Devuelve:
// - Devuelve el control a la ventana de windows correspondiente.
// Notas:
///////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  // Vbles
  bool bPrevPause; // Valor anterior de pausa

  // Se seleccionan los mensajes recibidos
  switch (message) {
	case WM_CLOSE:
	case WM_DESTROY:
	case WM_QUIT: {
	  // En caso de cierre, destruccion o anulacion se abandona
	  APPQuit();
	} break;

	case WM_ACTIVATEAPP: {
	  // Controla el cambio a otra ventana   
	  bPrevPause = bPause;
	  // Si wParam = 0 -> aplicacion en pausa
	  bPause = !wParam; 
	  if (bPrevPause && !bPause) {
		// Se retorna a la aplicacion y se recuperan texturas		
		CCRISOLEngine::GetInstance()->Restore();            
	  }    
	 } break;
  
	case WM_CHAR: {
	  // Pasa el caracter pulsado al motor
	  CCRISOLEngine::GetInstance()->InputCharEvent(wParam);
	} break;
  };

  // Se retorna
  return (DefWindowProc(hWnd, message, wParam, lParam));
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Funcion que inicializa la aplicacion, creando y registrando la ventana
//   principal.
// Parametros:
// - hInstance: Instancia a la ventana.
// - nCmdShow: ?
// Devuelve:
// - 0 si ha existido algun problema.
// - 1 si todo ha ido bien.
// Notas:
///////////////////////////////////////////////////////////////////////////////
static HRESULT InitApp(HINSTANCE hInstance, int nCmdShow)
{
  // Vbles
  HWND       hWnd;        // Handle a la ventana de la aplicacion
  WNDCLASS   WindowClass; // Atributos de la ventana a crear
  
  // Define la ventana de la aplicacion
  WindowClass.style = CS_HREDRAW | CS_VREDRAW;
  WindowClass.lpfnWndProc = WndProc;
  WindowClass.cbClsExtra = 0;
  WindowClass.cbWndExtra = 0;
  WindowClass.hInstance = hInstance;
  WindowClass.hIcon = LoadIcon( hInstance, IDI_APPLICATION );
  WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
  WindowClass.hbrBackground = (HBRUSH )GetStockObject(BLACK_BRUSH);
  WindowClass.lpszMenuName = APPNAME;
  WindowClass.lpszClassName = APPNAME;

  // Registra la ventana
  RegisterClass(&WindowClass);

  // Crea la ventana de la aplicacion
  hWnd = CreateWindowEx(WS_EX_TOPMOST,
                        APPNAME,
                        APPTITLE,
                        WS_POPUP,
                        0,
                        0,
                        GetSystemMetrics(SM_CXSCREEN),
                        GetSystemMetrics(SM_CYSCREEN),
                        NULL,
                        NULL,
                        hInstance,
                        NULL);

  // Se comprueba si todo ha ido bien
  if (!hWnd) { return 0; }

  // Ventana creada. Se muestra, se actualiza y se situa el foco
  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);
  SetFocus(hWnd);  

  // Se almacenan los handles en las vbles globales
  GlobalhWnd = hWnd;        
  GlobalhInstance = hInstance; 
  
  // Todo correcto
  return 1;
}

///////////////////////////////////////////////////////////////////////////////
// Descripcion:
// - Se encarga de la inicializacion de la aplicacion y del control del bucle
//   despachador de mensajes.
// Parametros:
// - hInstance: Instancia de la aplicacion
// - hPrevInstance: Instancia previa.
// - lpCmdLine: Comandos introducidos en linea de parametros.
// - nCmdShow: ?
// Devuelve:
// - 0 si ha ocurrido algun error.
// - 1 si todo ha ido bien.
// Notas:
///////////////////////////////////////////////////////////////////////////////
int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{

  #ifdef _DEBUG
	// Detecta memory - leaks / flipcode
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); // Get current flag
	flag |= _CRTDBG_LEAK_CHECK_DF; // Turn on leak-checking bit
	_CrtSetDbgFlag(flag); // Set flag to the new value
  #endif

  // Vbles  
  unsigned long  exception; // Vble con la excepcion ocurrida
  MSG            msg;       // Recoge los mensajes

  if (InitApp(hInstance, nCmdShow) == 0) {
    // Caso de que falle la inicializacion
    OutputDebugString("Problemas\n");  
    return 0;
  }

  // Se inicializa el motor
  APPInit();  

  __try {
    // Bucle de ejecucion 
    while (1) {
      if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {        
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }

      // Actualizacion del motor por frame
      if (!bPause) { 
		APPUpdateFrame(); 
	  }      
    } // ~ while
  } // ~__try
  
  __except(EvaluateException(exception = GetExceptionCode())) {
    // Ha ocurrido algun problema serio.
    // Se intentara abandonar de forma limpia.
    ExceptionHandler(exception);
  } 
 
  // Se abandona la aplicacion
  APPShutDown();
  return msg.wParam;
}