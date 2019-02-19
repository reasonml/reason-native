/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestFramework;

type singleSuiteInput = {
  name: string,
  numPassingTests: int,
  numFailingTests: int,
  numSkippedTests: int,
};

type multipleSuiteInput = {
  name: string,
  testSuites: list(TestSuite.t),
};

type singleSuiteExpectedOutput = {
  numPassedTests: int,
  numFailedTests: int,
  numSkippedTests: int,
  numPassedTestSuites: int,
  numPendingTestSuites: int,
  numTotalTests: int,
  numTotalTestSuites: int,
  numSkippedTestSuites: int,
  numFailedTestSuites: int,
};

exception OnRunCompleteNotCalled;

describe("Rely AggregateResult", ({describe, test}) => {
  describe("Single suite cases", ({test}) => {
    let singleSuiteInput = (passing, failing, skipped) => {
      let name =
        String.concat(
          ", ",
          [
            string_of_int(passing) ++ " passing tests",
            string_of_int(failing) ++ " failing tests",
            string_of_int(skipped) ++ " skipped tests",
          ],
        );
      {
        name,
        numPassingTests: passing,
        numFailingTests: failing,
        numSkippedTests: skipped,
      };
    };
    let singleSuiteTestCases = [
      (
        {
          name: "single skipped test",
          numPassingTests: 0,
          numFailingTests: 0,
          numSkippedTests: 1,
        },
        {
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 1,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 1,
          numFailedTestSuites: 0,
          numPassedTests: 0,
          numFailedTests: 0,
          numSkippedTests: 1,
        },
      ),
      (
        {
          name: "single passing test",
          numPassingTests: 1,
          numFailingTests: 0,
          numSkippedTests: 0,
        },
        {
          numPassedTestSuites: 1,
          numPendingTestSuites: 0,
          numTotalTests: 1,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 0,
          numPassedTests: 1,
          numFailedTests: 0,
          numSkippedTests: 0,
        },
      ),
      (
        {
          name: "single failing test",
          numPassingTests: 0,
          numFailingTests: 1,
          numSkippedTests: 0,
        },
        {
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 1,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 1,
          numPassedTests: 0,
          numFailedTests: 1,
          numSkippedTests: 0,
        },
      ),
      (
        singleSuiteInput(1, 10, 100),
        {
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 111,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 1,
          numPassedTests: 1,
          numFailedTests: 10,
          numSkippedTests: 100,
        },
      ),
      (
        singleSuiteInput(1, 100, 10),
        {
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 111,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 1,
          numPassedTests: 1,
          numFailedTests: 100,
          numSkippedTests: 10,
        },
      ),
      (
        singleSuiteInput(10, 1, 100),
        {
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 111,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 1,
          numPassedTests: 10,
          numFailedTests: 1,
          numSkippedTests: 100,
        },
      ),
      (
        singleSuiteInput(10, 100, 1),
        {
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 111,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 1,
          numPassedTests: 10,
          numFailedTests: 100,
          numSkippedTests: 1,
        },
      ),
      (
        singleSuiteInput(100, 1, 10),
        {
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 111,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 1,
          numPassedTests: 100,
          numFailedTests: 1,
          numSkippedTests: 10,
        },
      ),
      (
        singleSuiteInput(100, 10, 1),
        {
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 111,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 1,
          numPassedTests: 100,
          numFailedTests: 10,
          numSkippedTests: 1,
        },
      ),
      (
        singleSuiteInput(100, 0, 1),
        {
          numPassedTestSuites: 1,
          numPendingTestSuites: 0,
          numTotalTests: 101,
          numTotalTestSuites: 1,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 0,
          numPassedTests: 100,
          numFailedTests: 0,
          numSkippedTests: 1,
        },
      ),
    ];
    let testSingleSuite = ((input: singleSuiteInput, expectedOutput)) =>
      test(
        input.name,
        ({expect}) => {
          open Rely.Reporter;
          let aggregatedResult = ref(None);

          let testSuites = [
            TestSuite.(
              init(input.name)
              |> withSkippedTests(input.numSkippedTests)
              |> withPassingTests(input.numPassingTests)
              |> withFailingTests(input.numFailingTests)
            ),
          ];
          module Reporter =
            TestReporter.Make({});
          Reporter.onRunComplete(res => aggregatedResult := Some(res));

          TestSuiteRunner.run(testSuites, Reporter.reporter);

          let _ =
            switch (aggregatedResult^) {
            | None => raise(OnRunCompleteNotCalled)
            | Some(aggregatedResult) =>
              let actualOutput = {
                numPassedTestSuites: aggregatedResult.numPassedTestSuites,
                numPendingTestSuites: aggregatedResult.numPendingTestSuites,
                numTotalTests: aggregatedResult.numTotalTests,
                numTotalTestSuites: aggregatedResult.numTotalTestSuites,
                numSkippedTestSuites: aggregatedResult.numSkippedTestSuites,
                numFailedTestSuites: aggregatedResult.numFailedTestSuites,
                numPassedTests: aggregatedResult.numPassedTests,
                numFailedTests: aggregatedResult.numFailedTests,
                numSkippedTests: aggregatedResult.numSkippedTests,
              };
              expect.bool(actualOutput == expectedOutput).toBeTrue();
            };
          ();
        },
      );
    singleSuiteTestCases |> List.iter(testSingleSuite);
  });
  describe("Multiple suite cases", ({test}) => {
    let singletonPassingTestSuite =
      TestSuite.(init("passing") |> withPassingTests(1));
    let singletonFailingTestSuite =
      TestSuite.(init("failing") |> withFailingTests(1));
    let singletonSkippedTestSuite =
      TestSuite.(init("skipped") |> withSkippedTests(1));

    let repeatHelper = (el, n, l) =>
      switch (n) {
      | n when n <= 0 => l
      | n => l @ [el]
      };
    let repeat = (el, n) => repeatHelper(el, n, []);

    let singletonSuiteTestCase = (passing, failing, skipped) => {
      let name =
        String.concat(
          ", ",
          [
            string_of_int(passing) ++ " passing test suites",
            string_of_int(failing) ++ " failing test suites",
            string_of_int(skipped) ++ " skipped test suites",
          ],
        );
      let testSuites =
        repeat(singletonPassingTestSuite, passing)
        @ repeat(singletonFailingTestSuite, failing)
        @ repeat(singletonSkippedTestSuite, skipped);
      {name, testSuites};
    };

    let testCases = [
      (
        {testSuites: [], name: "no test suites"},
        {
          numPassedTests: 0,
          numFailedTests: 0,
          numSkippedTests: 0,
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 0,
          numTotalTestSuites: 0,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 0,
        },
      ),
      (
        singletonSuiteTestCase(1, 0, 1),
        {
          numPassedTests: 1,
          numFailedTests: 0,
          numSkippedTests: 1,
          numPassedTestSuites: 1,
          numPendingTestSuites: 0,
          numTotalTests: 2,
          numTotalTestSuites: 2,
          numSkippedTestSuites: 1,
          numFailedTestSuites: 0,
        },
      ),
      (
        singletonSuiteTestCase(1, 1, 0),
        {
          numPassedTests: 1,
          numFailedTests: 1,
          numSkippedTests: 0,
          numPassedTestSuites: 1,
          numPendingTestSuites: 0,
          numTotalTests: 2,
          numTotalTestSuites: 2,
          numSkippedTestSuites: 0,
          numFailedTestSuites: 1,
        },
      ),
      (
        singletonSuiteTestCase(0, 1, 1),
        {
          numPassedTests: 0,
          numFailedTests: 1,
          numSkippedTests: 1,
          numPassedTestSuites: 0,
          numPendingTestSuites: 0,
          numTotalTests: 2,
          numTotalTestSuites: 2,
          numSkippedTestSuites: 1,
          numFailedTestSuites: 1,
        },
      ),
      (
        singletonSuiteTestCase(1, 1, 1),
        {
          numPassedTests: 1,
          numFailedTests: 1,
          numSkippedTests: 1,
          numPassedTestSuites: 1,
          numPendingTestSuites: 0,
          numTotalTests: 3,
          numTotalTestSuites: 3,
          numSkippedTestSuites: 1,
          numFailedTestSuites: 1,
        },
      ),
    ];

    let runTestCases = ((input, expectedOutput)) =>
      test(
        input.name,
        ({expect}) => {
          open Rely.Reporter;
          let aggregatedResult = ref(None);

          module Reporter =
            TestReporter.Make({});
          Reporter.onRunComplete(res => aggregatedResult := Some(res));

          TestSuiteRunner.run(input.testSuites, Reporter.reporter);

          let _ =
            switch (aggregatedResult^) {
            | None => raise(OnRunCompleteNotCalled)
            | Some(aggregatedResult) =>
              let actualOutput = {
                numPassedTestSuites: aggregatedResult.numPassedTestSuites,
                numPendingTestSuites: aggregatedResult.numPendingTestSuites,
                numTotalTests: aggregatedResult.numTotalTests,
                numTotalTestSuites: aggregatedResult.numTotalTestSuites,
                numSkippedTestSuites: aggregatedResult.numSkippedTestSuites,
                numFailedTestSuites: aggregatedResult.numFailedTestSuites,
                numPassedTests: aggregatedResult.numPassedTests,
                numFailedTests: aggregatedResult.numFailedTests,
                numSkippedTests: aggregatedResult.numSkippedTests,
              };
              expect.bool(actualOutput == expectedOutput).toBeTrue();
            };
          ();
        },
      );
    testCases |> List.iter(runTestCases);
  });
  describe("usage of describe skip", ({test}) => {
    let executeTestCase = (input: singleSuiteInput) =>
      test(
        input.name,
        ({expect}) => {
          module Reporter =
            TestReporter.Make({});
          let result = ref(None);
          let testSuites = [
            TestSuite.(
              init(input.name)
              |> withSkippedTests(input.numSkippedTests)
              |> withPassingTests(input.numPassingTests)
              |> withFailingTests(input.numFailingTests)
              |> skipSuite
            ),
          ];

          Reporter.onRunComplete(res => result := Some(res));

          TestSuiteRunner.run(testSuites, Reporter.reporter);

          switch (result^) {
          | None => raise(OnRunCompleteNotCalled)
          | Some(aggregatedResult) =>
            expect.int(aggregatedResult.numSkippedTestSuites).toBe(1);
            expect.int(aggregatedResult.numPassedTests).toBe(0);
            expect.int(aggregatedResult.numFailedTests).toBe(0);
            expect.int(aggregatedResult.numSkippedTests).toBe(
              input.numPassingTests
              + input.numFailingTests
              + input.numSkippedTests,
            );
          };
        },
      );
    let singleSuiteSkipTestCases = [
      {
        name: "no tests",
        numPassingTests: 0,
        numFailingTests: 0,
        numSkippedTests: 0,
      },
      {
        name: "some passing and failing tests",
        numPassingTests: 5,
        numFailingTests: 10,
        numSkippedTests: 0,
      },
      {
        name: "some passing, failing, and skipped tests",
        numPassingTests: 5,
        numFailingTests: 10,
        numSkippedTests: 2,
      },
    ];
    let _ = singleSuiteSkipTestCases |> List.map(executeTestCase);
    ();
    test("nested describes should be skipped as well", ({expect}) => {
      let nestedSuite =
        TestSuite.(
          init("parent")
          |> withPassingTests(5)
          |> withFailingTests(3)
          |> withSkippedTests(1)
          |> skipSuite
          |> withNestedTestSuite(
               ~child=
                 init("child") |> withPassingTests(2) |> withFailingTests(4),
             )
        );
      module Reporter =
        TestReporter.Make({});
      let result = ref(None);

      Reporter.onRunComplete(res => result := Some(res));

      TestSuiteRunner.run([nestedSuite, nestedSuite], Reporter.reporter);

      switch (result^) {
      | None => raise(OnRunCompleteNotCalled)
      | Some(aggregatedResult) =>
        expect.int(aggregatedResult.numSkippedTestSuites).toBe(2);
        expect.int(aggregatedResult.numPassedTests).toBe(0);
        expect.int(aggregatedResult.numFailedTests).toBe(0);
        expect.int(aggregatedResult.numSkippedTests).toBe(30);
      };
      ();
    });
  });
});
