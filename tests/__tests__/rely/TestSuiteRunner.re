/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
module MakeTestFramework = (()) : Rely.TestFramework =>
  Rely.Make({
    let config =
      Rely.TestFrameworkConfig.initialize({
        snapshotDir: "unused",
        projectDir: "",
      });
  });

let run = (testSuites: list(TestSuite.t), reporter: Rely.Reporter.t) => {
  module TestFramework =
    MakeTestFramework({});

  testSuites
  |> List.map(TestSuite.toFunction)
  |> List.iter(ts => ts(TestFramework.describe));

  TestFramework.run(
    Rely.RunConfig.(
      initialize()
      |> internal_reporters_api_do_not_use(reporter)
      |> onTestFrameworkFailure(() => ())
    ),
  );
};
