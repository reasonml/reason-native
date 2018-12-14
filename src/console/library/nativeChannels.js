/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

//Provides: native_out
function native_out(s) {
  // In node, you can omit the newline, but not in the browser.
  if (typeof process != "undefined" &&
    typeof process.stdout != "undefined") {
    process.stdout.write(s.c)
  } else {
    joo_global_object.console.log(s.c);
  }
}

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
