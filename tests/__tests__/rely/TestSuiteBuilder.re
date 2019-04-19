/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type mode =
  | Only
  | Normal
  | Skip;

type test = {
  mode,
  pass: bool,
};

type t = {
  tests: list(test),
  nestedSuites: list(t),
  name: string,
  mode,
};

let init = name => {tests: [], nestedSuites: [], name, mode: Normal};

let withNestedTestSuite: (~child: t, t) => t =
  (~child, parent) => {
    ...parent,
    nestedSuites: parent.nestedSuites @ [child],
  };

let appendTests = (numTests, test, suite) => {
  let newTests = Array.make(numTests, test) |> Array.to_list;
  {...suite, tests: suite.tests @ newTests};
};

let withFailingTests = (~only=false, numFailingTests, suite) => {
  let mode = only ? Only : Normal;
  appendTests(numFailingTests, {pass: false, mode}, suite);
};
let withPassingTests = (~only=false, numPassingTests, suite) => {
  let mode = only ? Only : Normal;
  appendTests(numPassingTests, {pass: true, mode}, suite);
};
let skipSuite = suite => {...suite, mode: Skip};
let withSkippedTests = (numSkippedTests, suite) =>
  appendTests(numSkippedTests, {pass: true, mode: Skip}, suite);
let only = (suite) => {...suite, mode: Only};

let rec toFunction = config => {
  open Rely.Test;
  open Rely.Describe;
  let tests = describeUtils =>
    List.iter(
      ({mode, pass}) => {
        let body = ({expect}) => expect.bool(true).toBe(pass);
        switch (mode) {
        | Normal => describeUtils.test("some test", body)
        | Skip => describeUtils.testSkip("some test", body)
        | Only => describeUtils.testOnly("some test", body)
        };
      },
      config.tests,
    );
  let nestedSuites =
      (
        ~describe: Rely.Describe.describeFn(unit),
        ~describeSkip: Rely.Describe.describeFn(unit),
        ~describeOnly: Rely.Describe.describeFn(unit),
      ) =>
    config.nestedSuites
    |> List.iter(suite =>
         toFunction(suite, ~describe, ~describeSkip, ~describeOnly)
       );

  let fn =
      (
        ~describe: Rely.Describe.describeFn(unit),
        ~describeSkip: Rely.Describe.describeFn(unit),
        ~describeOnly: Rely.Describe.describeFn(unit),
      ) => {
    let describeFnToUse =
      switch (config.mode) {
      | Normal => describe
      | Skip => describeSkip
      | Only => describeOnly
      };
    describeFnToUse(
      config.name,
      describeUtils => {
        tests(describeUtils);
        nestedSuites(describeUtils.describe, describeUtils.describeSkip, describeUtils.describeOnly);
      },
    );
  };
  fn;
};
