/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
let testResultToTestCase = (testResult: Reporter.testResult) => {
  let time =
    switch (testResult.duration) {
    | Some(Seconds(s)) => s
    | None => 0.
    };

  switch (testResult.testStatus) {
  | Passed =>
    Junit.Testcase.pass(
      ~name=testResult.title,
      ~classname=testResult.fullName,
      ~time,
    )
  | Skipped =>
    Junit.Testcase.skipped(
      ~name=testResult.title,
      ~classname=testResult.fullName,
      ~time,
    )
  | Failed(message, location_opt, stack) =>
    Junit.Testcase.failure(
      ~message,
      ~typ="Failed Assertion",
      ~name=testResult.title,
      ~classname=testResult.fullName,
      ~time,
      String.concat("\n\n", [message, stack]),
    )
  | Exception(e, _location_opt, stack) =>
    Junit.Testcase.error(
      ~typ=Printexc.to_string(e),
      ~name=testResult.title,
      ~classname=testResult.fullName,
      ~time,
      stack,
    )
  };
};

let createJUnitReporter = fileName: Reporter.t => {
  let suites = ref([]);

  {
    onTestSuiteStart: _ => (),
    onTestSuiteResult: ({name}, _aggregatedResult, testSuiteResult) => {
      let currentSuite =
        List.map(testResultToTestCase, testSuiteResult.testResults)
        |> (
          cases =>
            Junit.Testsuite.add_testcases(
              cases,
              Junit.Testsuite.make(~name=testSuiteResult.displayName, ()),
            )
        );

      suites := [currentSuite, ...suites^];
    },
    onRunStart: _ => (),
    onRunComplete: _ => Junit.to_file(Junit.make(suites^), fileName),
  };
};
