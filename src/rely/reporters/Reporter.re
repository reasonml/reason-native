/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestResult;

type testSuite = {
  name: string,
};

type relyRunInfo = {
  testSuites: list(testSuite)
};

type aggregatedResult = AggregatedResult.t;
type testSuiteResult = TestSuiteResult.t;
type testResult = TestResult.testResult;

type t = {
  onTestSuiteStart: testSuite => unit,
  onTestSuiteResult: (testSuite, aggregatedResult, testSuiteResult) => unit,
  onRunStart: (relyRunInfo) => unit,
  onRunComplete: aggregatedResult => unit,
};
