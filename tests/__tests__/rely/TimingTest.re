/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestFramework;
open Rely.Time;
open Rely.Reporter;

exception OnRunCompleteNotCalled;

describe("Rely timing data", ({describe, test}) => {
  test("test suite start times before end times", ({expect}) => {
    module Reporter =
      TestReporter.Make({});
    let result = ref(None);
    let testSuites = [
      TestSuiteBuilder.(
        TestSuite(
          init("foo") |> withPassingTests(15) |> withFailingTests(10),
        )
      ),
      TestSuiteBuilder.(
        TestSuite(
          init("bar") |> withPassingTests(15) |> withFailingTests(10),
        )
      ),
      TestSuiteBuilder.(
        TestSuite(
          init("baz") |> withPassingTests(15) |> withFailingTests(10),
        )
      ),
    ];

    Reporter.onRunComplete(res => result := Some(res));

    TestSuiteRunner.run(testSuites, Reporter.reporter);

    let _ =
      switch (result^) {
      | None => raise(OnRunCompleteNotCalled)
      | Some(aggregatedResult) =>
        let suiteStartBeforeSuiteEnd =
          aggregatedResult.testSuiteResults
          |> List.map((r: Rely.Reporter.testSuiteResult) =>
               switch (r.startTime, r.endTime) {
               | (Some(start), Some(endT)) => start < endT
               | _ => true
               }
             )
          |> List.fold_left((acc, r) => acc && r, true);
        expect.bool(suiteStartBeforeSuiteEnd).toBeTrue();
      };
    ();
  });
  test("duration should be populated for run tests", ({expect}) => {
    module Reporter =
      TestReporter.Make({});
    let result = ref(None);
    let testSuites = [
      TestSuiteBuilder.(
        TestSuite(
          init("foo") |> withPassingTests(15) |> withFailingTests(10),
        )
      ),
    ];

    Reporter.onRunComplete(res => result := Some(res));

    TestSuiteRunner.run(testSuites, Reporter.reporter);

    let _ =
      switch (result^) {
      | None => raise(OnRunCompleteNotCalled)
      | Some(aggregatedResult) =>
        let timingsPopulated =
          aggregatedResult.testSuiteResults
          |> List.map((r: Rely.Reporter.testSuiteResult) => r.testResults)
          |> List.flatten
          |> List.map((r: Rely.Reporter.testResult) =>
               switch (r.duration) {
               /* >= 0 because sometimes it's too dang fast :p,
                * can verify they aren't all zero with a Console.log, but it's
                * nondeterministic so there isn't an assertion to that effect
                */
               | Some(Seconds(duration)) => duration >= 0.
               | None => false
               }
             )
          |> List.fold_left((acc, r) => acc && r, true);
        expect.bool(timingsPopulated).toBeTrue();
      };
    ();
  });
  test("duration should not be populated for skipped tests", ({expect}) => {
    module Reporter =
      TestReporter.Make({});
    let result = ref(None);
    let testSuites = [
      TestSuiteBuilder.(TestSuite(init("foo") |> withSkippedTests(15))),
    ];

    Reporter.onRunComplete(res => result := Some(res));

    TestSuiteRunner.run(testSuites, Reporter.reporter);

    let _ =
      switch (result^) {
      | None => raise(OnRunCompleteNotCalled)
      | Some(aggregatedResult) =>
        let timingsNotPopulated =
          aggregatedResult.testSuiteResults
          |> List.map((r: Rely.Reporter.testSuiteResult) => r.testResults)
          |> List.flatten
          |> List.map((r: Rely.Reporter.testResult) =>
               switch (r.duration) {
               | Some(duration) => false
               | None => true
               }
             )
          |> List.fold_left((acc, r) => acc && r, true);
        expect.bool(timingsNotPopulated).toBeTrue();
      };
    ();
  });
  test(
    "startTime should be populated with initial value from clock", ({expect}) => {
    module Reporter =
      TestReporter.Make({});
    let result = ref(None);
    let testSuites = [
      TestSuiteBuilder.(
        TestSuite(
          init("foo") |> withPassingTests(15) |> withFailingTests(10),
        )
      ),
    ];

    let startTime = 10.;
    Reporter.onRunComplete(res => result := Some(res));

    TestSuiteRunner.runWithCustomTime(
      () => Seconds(startTime),
      testSuites,
      Reporter.reporter,
    );

    let _ =
      switch (result^) {
      | None => raise(OnRunCompleteNotCalled)
      | Some(aggregatedResult) =>
        let Seconds(actualStartTime) = aggregatedResult.startTime;
        expect.float(actualStartTime).toBeCloseTo(startTime);
      };
    ();
  });
});
