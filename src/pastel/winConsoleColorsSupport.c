/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#define CAML_NAME_SPACE

#include <stdio.h>
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/fail.h>

#ifdef _WIN32
#include <windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#endif

CAMLprim value enable_windows_console_ansi_sequences()
{
  CAMLparam0();

#ifdef _WIN32
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
    caml_failwith("error in enable_windows_console_ansi_seqences: " + GetLastError());
  }
#endif

  CAMLreturn(Val_unit);
}
