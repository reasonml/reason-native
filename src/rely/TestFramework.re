/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open RunConfig;
open TestFrameworkConfig;
open SnapshotIO;

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
open TestLifecycle;

module type TestFramework = {
  module Mock: Mock.Sig;
  include (module type of Describe);
  include (module type of Test);

  let describe: Describe.describeFn(unit, unit);
  let describeSkip: Describe.describeFn(unit, unit);
  let describeOnly: Describe.describeFn(unit, unit);
  let extendDescribe:
    MatcherTypes.matchersExtensionFn('ext) => extensionResult('ext, unit);
  let run: RunConfig.t => unit;
  let cli: unit => unit;
  let toLibrary: unit => testLibrary;
  let testLifecycle: TestLifecycle.defaultLifecycle;
  let beforeAll:
    (
      unit => 'all,
      TestLifecycle.t(
        beforeAllNotCalled,
        afterAllNotCalled,
        beforeEachNotCalled,
        afterEachNotCalled,
        'oldAll,
        'each,
      )
    ) =>
    TestLifecycle.t(
      beforeAllCalled,
      afterAllNotCalled,
      beforeEachNotCalled,
      afterEachNotCalled,
      'all,
      'all,
    );

  let afterAll:
    (
      'all => unit,
      TestLifecycle.t(
        'beforeAllCalled,
        afterAllNotCalled,
        'beforeEachCalled,
        'afterEachCalled,
        'all,
        'each,
      )
    ) =>
    TestLifecycle.t(
      'beforeAllCalled,
      afterAllCalled,
      'beforeEachCalled,
      'afterEachCalled,
      'all,
      'each,
    );

  let beforeEach:
    (
      'all => 'each,
      TestLifecycle.t(
        'beforeAllCalled,
        'afterAllCalled,
        beforeEachNotCalled,
        afterEachNotCalled,
        'all,
        'oldEach,
      )
    ) =>
    TestLifecycle.t(
      'beforeAllCalled,
      'afterAllCalled,
      beforeEachCalled,
      afterEachNotCalled,
      'all,
      'each,
    );

  let afterEach:
    (
      'each => unit,
      TestLifecycle.t(
        'beforeAllCalled,
        'afterAllCalled,
        'beforeEachCalled,
        afterEachNotCalled,
        'all,
        'each,
      )
    ) =>
    TestLifecycle.t(
      'beforeAllCalled,
      'afterAllCalled,
      'beforeEachCalled,
      afterEachCalled,
      'all,
      'each,
    );

  type describeConfig('ext, 'env);
  let withLifecycle:
    (
      TestLifecycle.defaultLifecycle => TestLifecycle.t(_, _, _, _, _, 'env),
      describeConfig('ext, unit)
    ) =>
    describeConfig('ext, 'env);
  let withCustomMatchers:
    (MatcherTypes.matchersExtensionFn('ext), describeConfig(unit, 'env)) =>
    describeConfig('ext, 'env);

  let describeConfig: describeConfig(unit, unit);
  let extendDescribe:
    describeConfig('ext, 'env) => extensionResult('ext, 'env);
};

module type FrameworkConfig = {let config: TestFrameworkConfig.t;};

module MakeInternal =
       (SnapshotIO: SnapshotIO.SnapshotIO, UserConfig: FrameworkConfig) => {
  include Describe;
  include Test;
  include TestLifecycle;
  module StackTrace =
    StackTrace.Make({
      let baseDir = UserConfig.config.projectDir;
      /* using both "/" and "\\" here because Filename.dir_sep is flaky on windows */
      let exclude = [
        Filename.dirname(__FILE__) ++ "/",
        Filename.dirname(__FILE__) ++ "\\\\",
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

  module Snapshot =
    FileSystemSnapshot.Make({
      let snapshotDir = UserConfig.config.snapshotDir;
      module IO = SnapshotIO;
    });

  module TestSuiteFactory =
    TestSuite.Factory({
      module StackTrace = StackTrace;
      module Mock = Mock;
      module Snapshot = Snapshot;
    });

  let testSuites: ref(list(TestSuite.t)) = ref([]);
  let makeDescribeFunction = (extensionFn, mode, testLifecycle) => {
    let describe = (name, fn) =>
      testSuites :=
        testSuites^
        @ [
          TestSuiteFactory.makeTestSuite({
            name,
            usersDescribeFn: fn,
            mode,
            extensionFn,
            testLifecycle,
          }),
        ];
    describe;
  };

  let makeDescribeFns = (testLifecycle, extensionFn) => {
    describe: makeDescribeFunction(extensionFn, Normal, testLifecycle),
    describeOnly: makeDescribeFunction(extensionFn, Only, testLifecycle),
    describeSkip: makeDescribeFunction(extensionFn, Skip, testLifecycle),
  };

  let defaultCreationFunctions =
    makeDescribeFns(TestLifecycle.default, _ => ());

  let describe = defaultCreationFunctions.describe;
  let describeSkip = defaultCreationFunctions.describeSkip;
  let describeOnly = defaultCreationFunctions.describeOnly;

  let testLifecycle = TestLifecycle.default;

  type describeConfig('ext, 'env) =
    | DescribeConfig{
        extensionFn: MatcherTypes.matchersExtensionFn('ext),
        lifecycle: TestLifecycle.t(_, _, _, _, _, 'env),
      }
      : describeConfig('ext, 'env);

  let withLifecycle:
    (
      TestLifecycle.defaultLifecycle => TestLifecycle.t(_, _, _, _, _, 'env),
      describeConfig('ext, unit)
    ) =>
    describeConfig('ext, 'env) =
    (lifecycleBuilder, DescribeConfig({extensionFn, lifecycle})) =>
      DescribeConfig({
        extensionFn,
        lifecycle: lifecycleBuilder(TestLifecycle.default),
      });
  let withCustomMatchers:
    (MatcherTypes.matchersExtensionFn('ext), describeConfig(unit, 'env)) =>
    describeConfig('ext, 'env) =
    (makeMatchers, DescribeConfig({extensionFn, lifecycle})) =>
      DescribeConfig({extensionFn: makeMatchers, lifecycle});

  let describeConfig =
    DescribeConfig({extensionFn: _ => (), lifecycle: TestLifecycle.default});
  let extendDescribe = (DescribeConfig({extensionFn, lifecycle})) =>
    makeDescribeFns(lifecycle, extensionFn);

  let run = (config: RunConfig.t) =>
    TestSuiteRunner.run(config, testSuites^);

  let cli = () => TestSuiteRunner.cli(testSuites^);

  let toLibrary = () => testSuites^;
};

module Make = MakeInternal(FileSystemSnapshotIO);
