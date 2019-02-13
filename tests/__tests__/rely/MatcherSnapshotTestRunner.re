/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
let snapshotFailingTestCase =
    (testUtils: Rely.Test.testUtils('ext), testBody) => {
  module TestFramework =
    Rely.Make({
      let config =
        Rely.TestFrameworkConfig.(
          initialize({snapshotDir: "unused", projectDir: ""})
        );
    });

  TestFramework.describe("unused, doesn't matter", ({test}) =>
    test("not used", testBody)
  );

  module Reporter =
    TestReporter.Make({});

  Reporter.onRunComplete(result => {
    let output =
      result.testSuiteResults
      |> List.map((a: Rely.Reporter.testSuiteResult) => a.testResults)
      |> List.flatten
      |> List.map((testResult: Rely.Reporter.testResult) => {
            switch (testResult.testStatus) {
           | Failed(message, loc, stack) => message
           | _ => ""
           };
        }
         )
      |> String.concat("");

    testUtils.expect.string(output).toMatchSnapshot();
  });

  Pastel.useMode(HumanReadable, () => {
    TestFramework.run(
        Rely.RunConfig.(
          initialize()
          |> withReporters([Custom(Reporter.reporter)])
          |> onTestFrameworkFailure(() => ())
        ),
      );
  });
};
