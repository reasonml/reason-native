/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
module Test = {
  type status =
    | Pending
    | Passed
    | Failed(string, option(Printexc.location), string)
    | Exception(exn, option(Printexc.location), string);

  type testUtils = {expect: DefaultMatchers.matchers};
  type testFn = (string, testUtils => unit) => unit;
  type testSpec = {
    testID: int,
    name: string,
    runTest: unit => unit,
    mutable status,
  };
};

type time =
  | Milliseconds(int);

type snapshotSummary;

type testResult = {
  path: TestPath.t,
  duration: option(time),
  testStatus: Test.status,
  title: string,
  fullName: string,
};

type intermediateTestResult = {
  path: TestPath.t,
  duration: option(time),
  mutable testStatus: Test.status,
  runTest: unit => unit,
  title: string,
  fullName: string,
};

type describeResult = {
  testResults: list(testResult),
  path: TestPath.t,
  describeName: string,
  describeResults: list(describeResult),
  duration: option(time),
};

type intermediateDescribeResult = {
  intermediateTestResults: list(intermediateTestResult),
  path: TestPath.t,
  describeName: string,
  intermediateDescribeResult: list(intermediateDescribeResult),
  duration: option(time),
};
