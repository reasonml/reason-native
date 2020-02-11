/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type printer = {
  printString: string => unit,
  printEndline: string => unit,
  printNewline: unit => unit,
  flush: out_channel => unit,
};

type reporter =
  | Default
  | JUnit(string)
  | Custom(Reporter.t);

type t = {
  updateSnapshots: bool,
  onTestFrameworkFailure: unit => unit,
  testNamePattern: option(string),
  reporters: list(reporter),
  getTime: unit => Time.t,
  ci: bool,
};

let initialize = () => {
  updateSnapshots: false,
  onTestFrameworkFailure: () => exit(1),
  reporters: [Default],
  getTime: Clock.getTime,
  ci: false,
  testNamePattern: None,
};

let updateSnapshots: (bool, t) => t =
  (updateSnapshots, config) => {...config, updateSnapshots};

let withReporters = (reporters: list(reporter), config) => {
  {...config, reporters};
};

let ciMode: (bool, t) => t = (ci, config) => {...config, ci};

let internal_do_not_use_get_time = (fn, cfg: t) => {...cfg, getTime: fn};

let onTestFrameworkFailure = (onTestFrameworkFailure, config) => {
  ...config,
  onTestFrameworkFailure,
};

let withTestNamePattern = (testNamePattern, config) => {
  ...config,
  testNamePattern,
};
