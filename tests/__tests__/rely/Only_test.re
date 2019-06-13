/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
open TestFramework;

let valuex = opt =>
  switch (opt) {
  | Some(v) => v
  | None =>
    raise(Invalid_argument("expected option to be Some but was None!"))
  };

describe("Rely .only", ({test}) => {
  test("only tests with only should run within a test suite", ({expect}) => {
    let testResult = ref(None);
    let testSuite =
      TestSuiteBuilder.(
        init("")
        |> withPassingTests(~only=true, 1)
        |> withPassingTests(1)
        |> build
      );
    module Reporter =
      TestReporter.Make({});
    Reporter.onRunComplete(r => testResult := Some(r));

    TestSuiteRunner.run([testSuite], Reporter.reporter);
    let result = valuex(testResult^);
    expect.int(result.numSkippedTests).toBe(1);
    expect.int(result.numPassedTests).toBe(1);
    expect.int(result.numPassedTestSuites).toBe(1);
    expect.int(result.numSkippedTestSuites).toBe(0);
  });
  test("Only suites with only should be run", ({expect}) => {
    let testResult = ref(None);
    let onlyTestSuite =
      TestSuiteBuilder.(init("") |> withPassingTests(2) |> only |> build);
    let normalTestSuite =
      TestSuiteBuilder.(init("") |> withPassingTests(3) |> build);
    module Reporter =
      TestReporter.Make({});
    Reporter.onRunComplete(r => testResult := Some(r));

    TestSuiteRunner.run([onlyTestSuite, normalTestSuite], Reporter.reporter);
    let result = valuex(testResult^);
    expect.int(result.numSkippedTests).toBe(3);
    expect.int(result.numPassedTests).toBe(2);
    expect.int(result.numPassedTestSuites).toBe(1);
    expect.int(result.numSkippedTestSuites).toBe(1);
  });
  test("Skipped tests in only suite should skip", ({expect}) => {
    let testResult = ref(None);
    let onlyTestSuite =
      TestSuiteBuilder.(
        init("")
        |> withSkippedTests(2)
        |> withPassingTests(1)
        |> only
        |> build
      );
    let normalTestSuite =
      TestSuiteBuilder.(init("") |> withPassingTests(3) |> build);
    module Reporter =
      TestReporter.Make({});
    Reporter.onRunComplete(r => testResult := Some(r));

    TestSuiteRunner.run([onlyTestSuite, normalTestSuite], Reporter.reporter);
    let result = valuex(testResult^);
    expect.int(result.numSkippedTests).toBe(5);
    expect.int(result.numPassedTests).toBe(1);
    expect.int(result.numPassedTestSuites).toBe(1);
    expect.int(result.numSkippedTestSuites).toBe(1);
  });
  test("Nested only should skip test suites a level up", ({expect}) => {
    let testResult = ref(None);

    let nestedOnlyTestSuite =
      TestSuiteBuilder.(
        init("contains nested")
        |> withSkippedTests(2)
        |> withPassingTests(1)
        |> withNestedTestSuite(~child=c =>
             c |> withName("nested") |> withPassingTests(1) |> only
           )
        |> build
      );
    let normalTestSuite =
      TestSuiteBuilder.(init("normal") |> withPassingTests(3) |> build);

    module Reporter =
      TestReporter.Make({});
    Reporter.onRunComplete(r => testResult := Some(r));

    TestSuiteRunner.run(
      [nestedOnlyTestSuite, normalTestSuite],
      Reporter.reporter,
    );
    let result = valuex(testResult^);

    expect.int(result.numSkippedTests).toBe(6);
    expect.int(result.numPassedTests).toBe(1);
    expect.int(result.numPassedTestSuites).toBe(1);
    expect.int(result.numSkippedTestSuites).toBe(1);
  });
  test("CI mode should throw an exception with testOnly", ({expect}) => {
    let nestedOnlyTestSuite =
      TestSuiteBuilder.(
        init("contains nested")
        |> withSkippedTests(2)
        |> withPassingTests(1)
        |> withNestedTestSuite(~child=c =>
             c |> withName("nested") |> withPassingTests(1, ~only=true)
           )
        |> build
      );
    let normalTestSuite =
      TestSuiteBuilder.(init("normal") |> withPassingTests(3) |> build);

    expect.fn(() =>
      TestSuiteRunner.runWithCustomConfig(
        [nestedOnlyTestSuite, normalTestSuite],
        Rely.RunConfig.(
          initialize()
          |> withReporters([])
          |> ciMode(true)
          |> onTestFrameworkFailure(() => ())
        ),
      )
    ).
      toThrow();
  });

  test("CI mode should throw an exception with describeOnly", ({expect}) => {
    let nestedOnlyTestSuite =
      TestSuiteBuilder.(
        init("contains nested")
        |> withSkippedTests(2)
        |> withPassingTests(1)
        |> withNestedTestSuite(~child=c =>
             c |> withName("nested") |> withPassingTests(1) |> only
           )
        |> build
      );
    let normalTestSuite =
      TestSuiteBuilder.(init("normal") |> withPassingTests(3) |> build);

    expect.fn(() =>
      TestSuiteRunner.runWithCustomConfig(
        [nestedOnlyTestSuite, normalTestSuite],
        Rely.RunConfig.(
          initialize()
          |> withReporters([])
          |> ciMode(true)
          |> onTestFrameworkFailure(() => ())
        ),
      )
    ).
      toThrow();
  });
});
