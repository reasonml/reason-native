/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
module Test: {
  type testUtils = {expect: DefaultMatchers.matchers};
  type testFn = (string, testUtils => unit) => unit;
};
module Describe: {
  type describeUtils = {
    describe: describeFn,
    test: Test.testFn,
  }
  and describeFn = (string, describeUtils => unit) => unit;
};

module RunConfig: {
  type printer = {
    printString: string => unit,
    printEndline: string => unit,
    printNewline: unit => unit,
    flush: out_channel => unit
  }
  type t;
  let initialize: unit => t;
  let updateSnapshots: (bool, t) => t;
  let printer_internal_do_not_use: (printer, t) => t;
  let onTestFrameworkFailure: (unit => unit, t) => t;
};

module type TestFramework = {
  let describe: Describe.describeFn;
  let run: RunConfig.t => unit;
  let cli: unit => unit;
};

type requiredConfiguration = TestFrameworkConfig.requiredConfiguration;

module TestFrameworkConfig: {
  type t;
  let initialize: requiredConfiguration => t;
};

module type FrameworkConfig = {let config: TestFrameworkConfig.t;};

module Make: (FrameworkConfig) => TestFramework;
