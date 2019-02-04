/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type status =
  | Passed
  | Skipped
  | Failed(string, option(Printexc.location), string)
  | Exception(exn, option(Printexc.location), string);

type testResult = {
  path: TestPath.test,
  duration: option(Time.t),
  testStatus: status,
  title: string,
  fullName: string,
};

module TestSuiteResult = {
  type t = {
    numFailedTests: int,
    numPassedTests: int,
    numSkippedTests: int,
    testResults: list(testResult),
    displayName: string,
    startTime: option(Time.t),
    endTime: option(Time.t),
  };
};

module AggregatedResult = {
  type snapshotSummary = {
    numCreatedSnapshots: int,
    numRemovedSnapshots: int,
    numUpdatedSnapshots: int,
  };

  type t = {
    numFailedTests: int,
    numFailedTestSuites: int,
    numPassedTests: int,
    numPassedTestSuites: int,
    numPendingTestSuites: int,
    numSkippedTests: int,
    numSkippedTestSuites: int,
    numTotalTests: int,
    numTotalTestSuites: int,
    snapshotSummary: option(snapshotSummary),
    startTime: Time.t,
    success: bool,
    testSuiteResults: list(TestSuiteResult.t),
  };
};
