//Provides: native_log
function native_log(s) {
  joo_global_object.console.log(s.c);
}

//Provides: native_debug
function native_debug(s) {
  joo_global_object.console.debug(s.c);
}

//Provides: native_error
function native_error(s) {
  joo_global_object.console.error(s.c);
}

//Provides: native_warn
function native_warn(s) {
  joo_global_object.console.warn(s.c);
}
