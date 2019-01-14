/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type t = {
  numFailingTests: int,
  numPassingTests: int,
  numSkippedTests: int,
  nestedSuites: list(t),
  name: string,
};

let init = name => {
  numFailingTests: 0,
  numPassingTests: 0,
  numSkippedTests: 0,
  nestedSuites: [],
  name,
};

let withNestedTestSuite: (~child: t, t) => t =
  (~child, parent) => {
    ...parent,
    nestedSuites: parent.nestedSuites @ [child],
  };

let withFailingTests = (numFailingTests, suite) => {
  ...suite,
  numFailingTests,
};
let withPassingTests = (numPassingTests, suite) => {
  ...suite,
  numPassingTests,
};

let withSkippedTests = (numSkippedTests, suite) => {
  ...suite,
  numSkippedTests,
};
let rec toFunction = config => {
  let failingTests = (test: Rely.Test.testFn(unit)) =>
    for (i in 1 to config.numFailingTests) {
      test("failing test " ++ string_of_int(i), ({expect}) =>
        expect.bool(true).toBeFalse()
      );
    };

  let passingTests = (test: Rely.Test.testFn(unit)) =>
    for (i in 1 to config.numPassingTests) {
      test("passing test " ++ string_of_int(i), ({expect}) =>
        expect.bool(true).toBeTrue()
      );
    };

  let skippedTests = (testSkip: Rely.Test.testFn(unit)) =>
    for (i in 1 to config.numSkippedTests) {
      testSkip("skipped test " ++ string_of_int(i), ({expect}) =>
        expect.bool(true).toBeTrue()
      );
    };

  let nestedSuites = (describe: Rely.Describe.describeFn(unit)) =>
    config.nestedSuites |> List.iter(suite => toFunction(suite, describe));

  let fn = (describe: Rely.Describe.describeFn(unit)) =>
    describe(
      config.name,
      ({describe, test, testSkip}) => {
        failingTests(test);
        passingTests(test);
        skippedTests(testSkip);
        nestedSuites(describe);
      },
    );
  fn;
};
