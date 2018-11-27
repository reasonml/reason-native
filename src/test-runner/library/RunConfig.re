/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 *
 * LICENSE file in the root directory of this source tree.
 */;
module RunConfig = {
  type printer = {
    printString: string => unit,
    printEndline: string => unit,
    printNewline: unit => unit,
    flush: out_channel => unit,
  };

  type t = {
    updateSnapshots: bool,
    printer,
    onTestFrameworkFailure: unit => unit,
  };

  let initialize = () => {
    updateSnapshots: false,
    printer: {
      printEndline: print_endline,
      printNewline: print_newline,
      printString: print_string,
      flush,
    },
    onTestFrameworkFailure: () => exit(1),
  };

  let updateSnapshots: (bool, t) => t =
    (updateSnapshots, config) => {...config, updateSnapshots};

  /* When external use becomes a priority, this should be handled by a test reporters API, for now this
     is just used in testing the test runner itself to prevent writing to standard out*/
  let printer_internal_do_not_use = (printer, config) => {
    ...config,
    printer,
  };

  let onTestFrameworkFailure = (onTestFrameworkFailure, config) => {
    ...config,
    onTestFrameworkFailure
  }
};
