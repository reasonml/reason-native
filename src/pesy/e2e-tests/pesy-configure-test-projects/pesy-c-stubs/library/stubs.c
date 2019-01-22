#include <caml/mlvalues.h>
#include <stdio.h>

CAMLprim value
caml_foo(value a) {
    int c_a = Int_val(a);
    printf("foo received: %d", c_a);
    return Val_unit;
}
