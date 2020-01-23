/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open TestFramework;

let valuex = opt =>
  switch (opt) {
  | Some(v) => v
  | None =>
    raise(Invalid_argument("expected option to be Some but was None!"))
  };

describe("Rely test filtering", ({test}) => {
  test("Should run all tests without a filter", ({expect}) => {
    let testResult = ref(None);

    let tests =
      TestSuiteBuilder.init("my tests")
      |> TestSuiteBuilder.withPassingTests(1)
      |> TestSuiteBuilder.build;

    module Reporter =
      TestReporter.Make({});

    Reporter.onRunComplete(r => testResult := Some(r));

    let config =
      Rely.RunConfig.(
        initialize()
        |> withReporters([Custom(Reporter.reporter)])
        |> onTestFrameworkFailure(() => ())
        |> withTestNamePattern(None)
      );

    TestSuiteRunner.runWithCustomConfig([tests], config);

    let result = valuex(testResult.contents);

    expect.int(result.numTotalTests).toBe(1);
  });
  test("Should skip all tests if filter doesn't match anything", ({expect}) => {
    let testResult = ref(None);

    let tests =
      TestSuiteBuilder.(
        init("my tests")
        |> withCustomTest({
             mode: Normal,
             name: "unmatched name",
             implementation: ({expect}) => expect.bool(true).toBeTrue(),
           })
        |> build
      );

    module Reporter =
      TestReporter.Make({});

    Reporter.onRunComplete(r => testResult := Some(r));

    let config =
      Rely.RunConfig.(
        initialize()
        |> withReporters([Custom(Reporter.reporter)])
        |> onTestFrameworkFailure(() => ())
        |> withTestNamePattern(Some("Pattern"))
      );

    TestSuiteRunner.runWithCustomConfig([tests], config);

    let result = valuex(testResult.contents);

    expect.int(result.numTotalTests).toBe(0);
  });
  test("Should run tests with names matching the filter", ({expect}) => {
    let testResult = ref(None);

    let tests =
      TestSuiteBuilder.(
        init("my tests")
        |> withCustomTest({
             mode: Normal,
             name: "unmatched",
             implementation: ({expect}) => expect.bool(true).toBeTrue(),
           })
        |> withCustomTest({
             mode: Normal,
             name: "matches",
             implementation: ({expect}) => expect.bool(true).toBeTrue(),
           })
        |> build
      );

    module Reporter =
      TestReporter.Make({});

    Reporter.onRunComplete(r => testResult := Some(r));

    let config =
      Rely.RunConfig.(
        initialize()
        |> withReporters([Custom(Reporter.reporter)])
        |> onTestFrameworkFailure(() => ())
        |> withTestNamePattern(Some("matches"))
      );

    TestSuiteRunner.runWithCustomConfig([tests], config);

    let result = valuex(testResult.contents);

    expect.int(result.numTotalTests).toBe(1);
  });
  test(
    "Should run all tests inside describes with names matching the filter",
    ({expect}) => {
    let testResult = ref(None);

    let tests =
      TestSuiteBuilder.(
        init("tests")
        |> withPassingTests(17)
        |> withNestedTestSuite(~child=c =>
             c
             |> withNestedTestSuite(~child=c =>
                  c |> withName("doubleNested")
                  |> withCustomTest({
                    mode: Normal,
                    name: "shouldRun1",
                    implementation: ({expect}) =>
                      expect.bool(true).toBeTrue(),
                  })
               |> withCustomTest({
                    mode: Normal,
                    name: "shouldRun2",
                    implementation: ({expect}) =>
                      expect.bool(true).toBeTrue(),
                  })
                )
             |> withName("nested")
             |> withCustomTest({
                  mode: Normal,
                  name: "nested1",
                  implementation: ({expect}) =>
                    expect.bool(true).toBeTrue(),
                })
             |> withCustomTest({
                  mode: Normal,
                  name: "nested2",
                  implementation: ({expect}) =>
                    expect.bool(true).toBeTrue(),
                })
           )
        |> build
      );

    module Reporter =
      TestReporter.Make({});

    Reporter.onRunComplete(r => testResult := Some(r));

    let config =
      Rely.RunConfig.(
        initialize()
        |> withReporters([Custom(Reporter.reporter)])
        |> onTestFrameworkFailure(() => ())
        |> withTestNamePattern(Some("doubleNested"))
      );

    TestSuiteRunner.runWithCustomConfig([tests], config);

    let result = valuex(testResult.contents);

    expect.int(result.numTotalTests).toBe(2);
  });
});
