#define CAML_NAME_SPACE

#include <stdio.h>
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

CAMLprim value enable_windows_console_ansi_seqences()
{
  CAMLparam0();

  int success = 0;

  HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

  if (hOut != INVALID_HANDLE_VALUE)
  {
      DWORD dwMode = 0;
      
      if (GetConsoleMode(hOut, &dwMode))
      {
          dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
          success = SetConsoleMode(hOut, dwMode) ? 1 : 0;
      }
  }

  if (success != 1) {
    // GetLastError()
    CAMLreturn(Val_unit);  
  }
  else {
    CAMLreturn(Val_unit);
  }  
}