/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Common.Collections;
open Common.Option.Infix;
open MatcherUtils;
open Reporter;
open SnapshotIO;
open TestResult;
include RunConfig;
include TestFrameworkConfig;
module MatcherTypes = MatcherTypes;
module MatcherUtils = MatcherUtils;
module Mock = Mock;
module Reporter = Reporter;
module Time = Time;
exception InvalidWhileRunning(string);
module Test = Test;
include Test;
module Describe = Describe;
include Describe;
open TestSuite;

type describeConfig = {
  updateSnapshots: bool,
  snapshotDir: string,
  getTime: unit => Time.t,
};

exception TestAlreadyRan(string);
exception PendingTestException(string);

type testLibrary = list(TestSuite.t);
type combineResult = {
  run: RunConfig.t => unit,
  cli: unit => unit,
  testLibrary,
};

let combine = libraries => {
  let testLibrary = List.flatten(libraries);
  let run = config => TestSuiteRunner.run(config, testLibrary);
  let cli = () => TestSuiteRunner.cli(testLibrary);

  {testLibrary, run, cli};
};

module type TestFramework = {
  module Mock: Mock.Sig;
  include (module type of Describe);
  include (module type of Test);

  let describe: Describe.describeFn(unit);
  let describeSkip: Describe.describeFn(unit);
  let extendDescribe:
    MatcherTypes.matchersExtensionFn('ext) => extensionResult('ext);
  let run: RunConfig.t => unit;
  let cli: unit => unit;
  let toLibrary: unit => testLibrary;
};

module type FrameworkConfig = {let config: TestFrameworkConfig.t;};

module Make = (UserConfig: FrameworkConfig) => {
  include Describe;
  include Test;
  module StackTrace =
    StackTrace.Make({
      let baseDir = UserConfig.config.projectDir;
      /* using both "/" and "\\" here because Filename.dir_sep is flaky on windows */
      let exclude = [
        Filename.dirname(__FILE__) ++ "/",
        Filename.dirname(__FILE__) ++ "\\",
      ];
      let formatLink = Pastel.cyan;
      let formatText = Pastel.dim;
    });
  module Mock =
    Mock.Make(
      StackTrace,
      {
        let maxNumCalls = UserConfig.config.maxNumMockCalls;
      },
    );

  module TestSuiteFactory =
    TestSuite.Factory({
      module StackTrace = StackTrace;
      module Mock = Mock;
      module Snapshot =
        FileSystemSnapshot.Make({
          let snapshotDir = UserConfig.config.snapshotDir;
          module IO = FileSystemSnapshotIO;
        });
    });

  let testSuites: ref(list(TestSuite.t)) = ref([]);
  let makeDescribeFunction = extensionFn => {
    let describe = (name, fn) =>
      testSuites :=
        testSuites^
        @ [
          TestSuiteFactory.makeTestSuite({
            name,
            usersDescribeFn: fn,
            skip: false,
            extensionFn,
          }),
        ];
    describe;
  };
  let makeDescribeSkipFunction = extensionFn => {
    let describeSkip = (name, fn) =>
      testSuites :=
        testSuites^
        @ [
          TestSuiteFactory.makeTestSuite({
            name,
            usersDescribeFn: fn,
            skip: true,
            extensionFn,
          }),
        ];
    describeSkip;
  };
  let describe = makeDescribeFunction(_ => ());
  let describeSkip = makeDescribeSkipFunction(_ => ());
  let extendDescribe = createCustomMatchers => {
    describe: makeDescribeFunction(createCustomMatchers),
    describeSkip: makeDescribeSkipFunction(createCustomMatchers),
  };

  let run = (config: RunConfig.t) => TestSuiteRunner.run(config, testSuites^);

  let cli = () => TestSuiteRunner.cli(testSuites^);

  let toLibrary = () => testSuites^;
};
