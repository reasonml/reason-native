/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Common.Collections;

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

type pendingTestResult = {
  path: TestPath.test,
  runTest: unit => unit,
};

type intermediateTestResult =
  | PendingTestResult(pendingTestResult)
  | FinalTestResult(testResult);

type describeResult = {
  testResults: list(testResult),
  path: TestPath.describe,
  describeResults: list(describeResult),
  endTime: option(Time.t),
  startTime: option(Time.t),
};

type snapshotSummary = {
  numCreatedSnapshots: int,
  numRemovedSnapshots: int,
  numUpdatedSnapshots: int,
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

  let rec ofDescribeResult = describeResult => {
    let {testResults, path, describeResults, startTime, endTime} = describeResult;
    let childResults = List.map(ofDescribeResult, describeResults);

    let numFailedTests = ref(0);
    let numPassedTests = ref(0);
    let numSkippedTests = ref(0);
    let aggregateTestResults = ref(testResults);

    List.iter(
      r => {
        numFailedTests := numFailedTests^ + r.numFailedTests;
        numPassedTests := numPassedTests^ + r.numPassedTests;
        numSkippedTests := numSkippedTests^ + r.numSkippedTests;
        aggregateTestResults := aggregateTestResults^ @ r.testResults;
      },
      childResults,
    );

    List.iter(
      r =>
        switch (r.testStatus) {
        | Passed => incr(numPassedTests)
        | Failed(_, _, _)
        | Exception(_, _, _) => incr(numFailedTests)
        | Skipped => incr(numSkippedTests)
        },
      testResults,
    );
    {
      numFailedTests: numFailedTests^,
      numPassedTests: numPassedTests^,
      numSkippedTests: numSkippedTests^,
      testResults: aggregateTestResults^,
      displayName: path |> TestPath.describeToString,
      startTime,
      endTime,
    };
  };
};

module AggregatedResult = {
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

  let initialAggregatedResult = (numTestSuites, startTime) => {
    numFailedTests: 0,
    numFailedTestSuites: 0,
    numPassedTests: 0,
    numPassedTestSuites: 0,
    numPendingTestSuites: numTestSuites,
    numSkippedTests: 0,
    numSkippedTestSuites: 0,
    numTotalTests: 0,
    numTotalTestSuites: numTestSuites,
    snapshotSummary: None,
    startTime,
    success: true,
    testSuiteResults: [],
  };

  let addTestSuiteResult =
      (testSuiteResult: TestSuiteResult.t, aggregatedResult) => {
    let didSuiteSkip =
      testSuiteResult.numSkippedTests
      == List.length(testSuiteResult.testResults);
    let didSuiteFail = testSuiteResult.numFailedTests > 0;
    let didSuitePass = !didSuiteFail && !didSuiteSkip;
    {
      numFailedTests:
        aggregatedResult.numFailedTests + testSuiteResult.numFailedTests,
      numFailedTestSuites:
        aggregatedResult.numFailedTestSuites + (didSuiteFail ? 1 : 0),
      numPassedTests:
        aggregatedResult.numPassedTests + testSuiteResult.numPassedTests,
      numPassedTestSuites:
        aggregatedResult.numPassedTestSuites + (didSuitePass ? 1 : 0),
      numPendingTestSuites: aggregatedResult.numPendingTestSuites - 1,
      numSkippedTests:
        aggregatedResult.numSkippedTests + testSuiteResult.numSkippedTests,
      numSkippedTestSuites:
        aggregatedResult.numSkippedTestSuites + (didSuiteSkip ? 1 : 0),
      numTotalTests:
        aggregatedResult.numTotalTests
        + testSuiteResult.numPassedTests
        + testSuiteResult.numFailedTests
        + testSuiteResult.numSkippedTests,
      numTotalTestSuites: aggregatedResult.numTotalTestSuites,
      testSuiteResults: aggregatedResult.testSuiteResults @ [testSuiteResult],
      snapshotSummary: aggregatedResult.snapshotSummary,
      startTime: aggregatedResult.startTime,
      success: aggregatedResult.success && !didSuiteFail,
    };
  };
};
