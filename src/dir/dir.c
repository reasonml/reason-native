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

#ifdef _WIN32
#include <Windows.h>
#include <Shlobj.h>
#endif

// It's good to include dummy bindings even on non-windows, so that all bytecode
// will have the same externs used regardless of platform, then compiling to
// JS/Hack will ensure reproducible output across all platforms.
CAMLprim value sh_get_folder_path(value nFolder, value dwFlags)
{
  CAMLparam2(nFolder, dwFlags);
#ifdef _WIN32
  CAMLlocal1(path);
  TCHAR pszPath[MAX_PATH];
  if (SUCCEEDED(SHGetFolderPath(NULL, Int_val(nFolder), NULL, Int_val(dwFlags), pszPath))) {
    path = caml_copy_string(pszPath);
  } else {
    caml_failwith("sh_get_folder_path");
  }
#endif
  CAMLreturn(Val_unit);
}

