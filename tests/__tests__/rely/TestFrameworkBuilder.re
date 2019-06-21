/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

type testFrameworkBuilder = {
  testSuites: list(TestSuiteBuilder.testSuite),
  snapshotDir: string,
  projectDir: string,
};

let withSnapshotDir = (snapshotDir, builder) => {...builder, snapshotDir};

let withProjectDir = (projectDir, builder) => {...builder, projectDir};

let withTestSuite = (testSuite, builder) => {
  ...builder,
  testSuites: [testSuite, ...builder.testSuites],
};

module Build =
       (Config: {let action: testFrameworkBuilder => testFrameworkBuilder;}) => {
  let builder =
    {testSuites: [], snapshotDir: "unused", projectDir: ""} |> Config.action;

  module TestFramework =
    Rely.Make({
      let config =
        Rely.TestFrameworkConfig.initialize({
          snapshotDir: builder.snapshotDir,
          projectDir: builder.projectDir,
        });
    });
  module TestSuiteRegisterer = TestSuiteBuilder.Make(TestFramework);

  builder.testSuites
  |> List.map((TestSuiteBuilder.TestSuite(suite)) =>
       TestSuiteRegisterer.register(suite)
     )
  |> List.iter(ts =>
       ts(
         ~describe=TestFramework.describe,
         ~describeSkip=TestFramework.describeSkip,
         ~describeOnly=TestFramework.describeOnly,
       )
     );
  include TestFramework;
};
