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
  CAMLlocal2(ret, path);
#ifdef _WIN32
  TCHAR pszPath[MAX_PATH];
  if (SUCCEEDED(SHGetFolderPath(NULL, Int_val(nFolder), NULL, Int_val(dwFlags), pszPath))) {
    path = caml_copy_string(pszPath);
    ret = caml_alloc(1, 0); /* Some */
    Store_field(ret, 0, path);
  } else {
    ret = Val_int(0); /* None */
  }
#else
  ret = Val_int(0); /* None */
#endif
  CAMLreturn(ret);
}

CAMLprim value sh_is_osx(value vUnit)
{
  CAMLparam0();
  CAMLlocal1(ret);

#ifdef __APPLE__
  ret = Val_bool(1);
#else
  ret = Val_bool(0);
#endif

  CAMLreturn(ret);
}
