/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestFramework;

let runTest =
    (testUtils: Rely.Test.testUtils('ext, 'env), expected, testBody) => {
  module TestFramework =
    Rely.Make({
      let config =
        Rely.TestFrameworkConfig.(
          initialize({snapshotDir: "unused", projectDir: ""})
        );
    });

  TestFramework.describe("unused, doesn't matter", ({test}) => {
    test("not used", testBody);
    ();
  });

  module Reporter =
    TestReporter.Make({});

  Reporter.onRunComplete(result => {
    let output =
      result.testSuiteResults
      |> List.map((a: Rely.Reporter.testSuiteResult) => a.testResults)
      |> List.flatten
      |> List.map((testResult: Rely.Reporter.testResult) =>
           switch (testResult.testStatus) {
           | Passed(Some({filename})) => filename
           | Skipped(Some({filename})) => filename
           | _ => ""
           }
         )
      |> String.concat("");

    testUtils.expect.string(output).toEqual(expected);
  });

  TestFramework.run(
    Rely.RunConfig.(
      initialize()
      |> withReporters([Custom(Reporter.reporter)])
      |> onTestFrameworkFailure(() => ())
    ),
  );
};

describe("Filename should be persisted in test result", ({test}) => {
  test("captured filename should equal __FILE__", testUtils =>
    runTest(testUtils, __FILE__, ({expect}) => expect.bool(true).toBeTrue())
  );
  ();
});
