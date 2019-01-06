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

type t = {
  onTestSuiteStart: testSuite => unit,
  onTestSuiteResult: (testSuite, AggregatedResult.t, TestSuiteResult.t) => unit,
  onRunStart: (relyRunInfo) => unit,
  onRunComplete: AggregatedResult.t => unit,
};
