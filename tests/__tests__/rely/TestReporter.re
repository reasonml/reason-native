/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Rely.Reporter;

module type TestReporter = {let reporter: Rely.Reporter.t;};

module Make = (()) => {
  let reporterInternal =
    ref({
      onTestSuiteStart: _ => (),
      onTestSuiteResult: (_, _, _) => (),
      onRunStart: _ => (),
      onRunComplete: _ => (),
    });

  let wasOnRunStartCalled = ref(false);
  let wasOnRunCompleteCalled = ref(false);
  let numOnTestSuiteStartCalls = ref(0);
  let numOnTestSuiteResultCalls = ref(0);

  let onTestSuiteStart = callback =>
    reporterInternal := {...reporterInternal^, onTestSuiteStart: callback};

  let onTestSuiteResult = callback =>
    reporterInternal := {...reporterInternal^, onTestSuiteResult: callback};

  let onRunStart = callback =>
    reporterInternal := {...reporterInternal^, onRunStart: callback};

  let onRunComplete = callback =>
    reporterInternal := {...reporterInternal^, onRunComplete: callback};

  let reporter = {
    onTestSuiteStart: testSuite => {
      incr(numOnTestSuiteStartCalls);
      reporterInternal^.onTestSuiteStart(testSuite);
    },
    onTestSuiteResult: (testSuite, aggregatedResult, testSuiteResult) => {
      incr(numOnTestSuiteResultCalls);
      reporterInternal^.onTestSuiteResult(
        testSuite,
        aggregatedResult,
        testSuiteResult,
      );
    },
    onRunStart: runInfo => {
      wasOnRunStartCalled := true;
      reporterInternal^.onRunStart(runInfo);
    },
    onRunComplete: aggregatedResult => {
      wasOnRunCompleteCalled := true;
      reporterInternal^.onRunComplete(aggregatedResult);
    },
  };
};
