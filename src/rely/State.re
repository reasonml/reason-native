/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestResult;

type time =
  | Milliseconds(int);

type snapshotSummary;

type testResult = {
  path: TestPath.t,
  duration: option(time),
  testStatus: TestResult.status,
  title: string,
  fullName: string,
};

type intermediateTestResult = {
  path: TestPath.t,
  duration: option(time),
  mutable testStatus: TestResult.status,
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
