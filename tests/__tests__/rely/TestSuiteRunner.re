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

let run = (testSuites: list(TestSuiteBuilder.t), reporter: Rely.Reporter.t) => {
  module TestFramework =
    MakeTestFramework({});

  testSuites
  |> List.map(TestSuiteBuilder.toFunction)
  |> List.iter(ts =>
       ts(
         ~describe=TestFramework.describe,
         ~describeSkip=TestFramework.describeSkip,
         ~describeOnly=TestFramework.describeOnly,
       )
     );

  TestFramework.run(
    Rely.RunConfig.(
      initialize()
      |> withReporters([Custom(reporter)])
      |> onTestFrameworkFailure(() => ())
    ),
  );
};

let runWithCustomTime = (getTime, testSuites, reporter) => {
  module TestFramework =
    Rely.Make({
      let config =
        Rely.TestFrameworkConfig.(
          initialize({snapshotDir: "unused", projectDir: ""})
        );
    });

  testSuites
  |> List.map(TestSuiteBuilder.toFunction)
  |> List.iter(ts =>
       ts(
         ~describe=TestFramework.describe,
         ~describeSkip=TestFramework.describeSkip,
         ~describeOnly=TestFramework.describeOnly,
       )
     );

  TestFramework.run(
    Rely.RunConfig.(
      initialize()
      |> withReporters([Custom(reporter)])
      |> onTestFrameworkFailure(() => ())
      |> internal_do_not_use_get_time(getTime)
    ),
  );
};
