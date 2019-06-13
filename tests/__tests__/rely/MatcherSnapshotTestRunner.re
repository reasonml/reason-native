/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

let runInternal = (run, testUtils: Rely.Test.testUtils('ext, 'env)) => {
  module Reporter =
    TestReporter.Make({});

  Reporter.onRunComplete(result => {
    let output =
      result.testSuiteResults
      |> List.map((a: Rely.Reporter.testSuiteResult) => a.testResults)
      |> List.flatten
      |> List.map((testResult: Rely.Reporter.testResult) =>
           switch (testResult.testStatus) {
           | Failed(message, loc, stack) => message
           | _ => ""
           }
         )
      |> String.concat("");

    testUtils.expect.string(output).toMatchSnapshot();
  });

  Pastel.useMode(HumanReadable, () =>
    run(
      Rely.RunConfig.(
        initialize()
        |> withReporters([Custom(Reporter.reporter)])
        |> onTestFrameworkFailure(() => ())
      ),
    )
  );
};

let snapshotFailingTestCaseCustomMatchers =
    (
      createCustomMatchers: Rely.MatcherTypes.matchersExtensionFn('ext),
      testUtils: Rely.Test.testUtils('ext, 'env),
      testBody,
    ) => {
  module TestFramework =
    Rely.Make({
      let config =
        Rely.TestFrameworkConfig.(
          initialize({snapshotDir: "unused", projectDir: ""})
        );
    });
  open Rely.Describe;
  let {describe} =
    TestFramework.(
      describeConfig
      |> withCustomMatchers(createCustomMatchers)
      |> extendDescribe
    );

  describe("unused, doesn't matter", ({test}) =>
    test("not used", testBody)
  );

  runInternal(TestFramework.run, testUtils);
};

let snapshotFailingTestCase =
    (testUtils: Rely.Test.testUtils('ext, 'env), testBody) => {
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

  runInternal(TestFramework.run, testUtils);
};
