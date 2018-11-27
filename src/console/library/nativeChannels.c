/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 *
 * LICENSE file in the root directory of this source tree.
 */

#define CAML_NAME_SPACE

#include <stdio.h>
#include <caml/mlvalues.h>
#include <caml/memory.h>

CAMLprim value native_log(value str)
{
  CAMLparam1(str);
  printf("%s", String_val(str));
  CAMLreturn(Val_unit);
}

CAMLprim value native_debug(value str)
{
  CAMLparam1(str);
  printf("%s", String_val(str));
  CAMLreturn(Val_unit);
}

CAMLprim value native_error(value str)
{
  CAMLparam1(str);
  fprintf(stderr, "%s", String_val(str));
  CAMLreturn(Val_unit);
}

CAMLprim value native_warn(value str)
{
  CAMLparam1(str);
  fprintf(stderr, "%s", String_val(str));
  CAMLreturn(Val_unit);
}
