/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
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
    onTestFrameworkFailure: unit => unit,
    reporters: list(Reporter.t),
  };

  let initialize = () => {
    updateSnapshots: false,
    onTestFrameworkFailure: () => exit(1),
    reporters: [
      TerminalReporter.createTerminalReporter({
        printEndline: print_endline,
        printNewline: print_newline,
        printString: print_string,
        flush,
      }),
    ],
  };

  type reporter =
    | Default
    | JUnit(string)
    | Custom(Reporter.t);

  let updateSnapshots: (bool, t) => t =
    (updateSnapshots, config) => {...config, updateSnapshots};

  let withReporters = (reporters: list(reporter), config) => {
    let reporters =
      reporters
      |> List.map(wrappedReporter =>
           switch (wrappedReporter) {
           | Default =>
             TerminalReporter.createTerminalReporter({
               printEndline: print_endline,
               printNewline: print_newline,
               printString: print_string,
               flush,
             })
           | JUnit(path) => JunitReporter.createJUnitReporter(path)
           | Custom(reporter) => reporter
           }
         );
    {...config, reporters};
  };

  let onTestFrameworkFailure = (onTestFrameworkFailure, config) => {
    ...config,
    onTestFrameworkFailure,
  };
};
