/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Common.Collections;

type status =
  | Passed
  | Failed(string, option(Printexc.location), string)
  | Exception(exn, option(Printexc.location), string);

type time =
  | Milliseconds(int);

type testResult = {
  path: TestPath.test,
  duration: option(time),
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
  duration: option(time),
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
    testResults: list(testResult),
    displayName: string,
  };

  let rec ofDescribeResult = describeResult => {
    let {testResults, path, describeResults, duration} = describeResult;
    let childResults = List.map(ofDescribeResult, describeResults);

    let numFailedTests = ref(0);
    let numPassedTests = ref(0);
    let aggregateTestResults = ref(testResults);

    List.iter(
      r => {
        numFailedTests := numFailedTests^ + r.numFailedTests;
        numPassedTests := numPassedTests^ + r.numPassedTests;
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
        },
      testResults,
    );
    {
      numFailedTests: numFailedTests^,
      numPassedTests: numPassedTests^,
      testResults: aggregateTestResults^,
      displayName: path |> TestPath.describeToString,
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
    numTotalTests: int,
    numTotalTestSuites: int,
    snapshotSummary: option(snapshotSummary),
    /* startTime: number, */
    /* success: boolean, */
    testSuiteResults: list(TestSuiteResult.t),
  };

  let initialAggregatedResult = numTestSuites => {
    numFailedTests: 0,
    numFailedTestSuites: 0,
    numPassedTests: 0,
    numPassedTestSuites: 0,
    numPendingTestSuites: numTestSuites,
    numTotalTests: 0,
    numTotalTestSuites: numTestSuites,
    snapshotSummary: None,
    /* startTime: number, */
    /* success: boolean, */
    testSuiteResults: [],
  };

  let addTestSuiteResult =
      (testSuiteResult: TestSuiteResult.t, aggregatedResult) => {
    let didSuitePass = testSuiteResult.numFailedTests == 0;
    {
      numFailedTests:
        aggregatedResult.numFailedTests + testSuiteResult.numFailedTests,
      numFailedTestSuites:
        aggregatedResult.numFailedTestSuites + (didSuitePass ? 0 : 1),
      numPassedTests:
        aggregatedResult.numPassedTests + testSuiteResult.numPassedTests,
      numPassedTestSuites:
        aggregatedResult.numPassedTestSuites + (didSuitePass ? 1 : 0),
      numPendingTestSuites: aggregatedResult.numPassedTestSuites - 1,
      numTotalTests:
        aggregatedResult.numTotalTests
        + testSuiteResult.numPassedTests
        + testSuiteResult.numFailedTests,
      numTotalTestSuites: aggregatedResult.numTotalTestSuites,
      testSuiteResults: aggregatedResult.testSuiteResults @ [testSuiteResult],
      snapshotSummary: aggregatedResult.snapshotSummary,
    };
  };
};
