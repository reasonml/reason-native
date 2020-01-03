/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

type cliArgs = {
  updateSnapshots: option(bool),
  ciMode: option(bool),
  onlyPrintDetailsForFailedSuites: option(bool),
  filter: option(Re.Pcre.regexp),
};

let emptyArgs: cliArgs = {
  updateSnapshots: None,
  ciMode: None,
  onlyPrintDetailsForFailedSuites: None,
  filter: None,
};

let hasFlag = (flag, argv) =>
  Array.length(argv) >= 2 && Array.exists(arg => arg == flag, argv);

let parseUpdateSnapshots: (array(string), cliArgs) => cliArgs =
  (argv, cliArgs) => {
    let shouldUpdateSnapshots = hasFlag("-u", argv);

    if (shouldUpdateSnapshots) {
      {...cliArgs, updateSnapshots: Some(true)};
    } else {
      cliArgs;
    };
  };

let parseOnlyPrintDetailsForFailedSuites: (array(string), cliArgs) => cliArgs =
  (argv, cliArgs) => {
    let onlyPrintDetailsForFailedSuites =
      hasFlag("--onlyPrintDetailsForFailedSuites", argv);

    if (onlyPrintDetailsForFailedSuites) {
      {...cliArgs, onlyPrintDetailsForFailedSuites: Some(true)};
    } else {
      cliArgs;
    };
  };

let parseCIMode: (array(string), cliArgs) => cliArgs =
  (argv, cliArgs) => {
    let shouldUpdateSnapshots = hasFlag("--ci", argv);

    if (shouldUpdateSnapshots) {
      {...cliArgs, ciMode: Some(true)};
    } else {
      cliArgs;
    };
  };

let parseArgs: array(string) => cliArgs =
  argv => {
    emptyArgs
    |> parseUpdateSnapshots(argv)
    |> parseOnlyPrintDetailsForFailedSuites(argv)
    |> parseCIMode(argv);
  };
