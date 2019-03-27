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
};

module type FrameworkConfig = {let config: TestFrameworkConfig.t;};

module Make = (UserConfig: FrameworkConfig) => {
  include Describe;
  include Test;
  module TestSnapshot = Snapshot.Make(SnapshotIO.FileSystemSnapshotIO);
  module TestSnapshotIO = SnapshotIO.FileSystemSnapshotIO;
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
    });

  let testSuites: ref(list(TestSuite.t)) = ref([]);
  let isRunning = ref(false);
  let errorIfRunning = (f, message) =>
    if (isRunning^) {
      raise(InvalidWhileRunning(message));
    } else {
      f();
    };
  let useIsRunning = f => {
    isRunning := true;
    let value =
      try (f()) {
      | e =>
        isRunning := false;
        raise(e);
      };
    isRunning := false;
    value;
  };
  let makeDescribeFunction = extensionFn => {
    let describe = (name, fn) =>
      errorIfRunning(
        () =>
          testSuites :=
            testSuites^
            @ [
              TestSuiteFactory.makeTestSuite({
                name,
                usersDescribeFn: fn,
                skip: false,
                extensionFn,
              }),
            ],
        "TestFramework.describe cannot be nested, instead use the describe supplied by the parent describe block, e.g. describe(\"parent describe\", {test, describe} => { ... });",
      );
    describe;
  };
  let makeDescribeSkipFunction = extensionFn => {
    let describeSkip = (name, fn) =>
      errorIfRunning(
        () =>
          testSuites :=
            testSuites^
            @ [
              TestSuiteFactory.makeTestSuite({
                name,
                usersDescribeFn: fn,
                skip: true,
                extensionFn,
              }),
            ],
        "TestFramework.describeSkip cannot be nested, instead use the describe supplied by the parent describe block, e.g. describe(\"parent describe\", {test, describe} => { ... });",
      );
    describeSkip;
  };
  let describe = makeDescribeFunction(_ => ());
  let describeSkip = makeDescribeSkipFunction(_ => ());
  let extendDescribe = createCustomMatchers => {
    describe: makeDescribeFunction(createCustomMatchers),
    describeSkip: makeDescribeSkipFunction(createCustomMatchers),
  };

  let run = (config: RunConfig.t) =>
    useIsRunning(() =>
      Util.withBacktrace(() => {
        module RunnerConfig = {
          let getTime = config.getTime;
          let snapshotDir = UserConfig.config.snapshotDir;
          let testHashes = MStringSet.empty();
          let updateSnapshots = config.updateSnapshots;
          let snapshotState =
            ref(
              TestSnapshot.initializeState(
                ~snapshotDir=UserConfig.config.snapshotDir,
                ~updateSnapshots=config.updateSnapshots,
              ),
            );
          module SnapshotIO = TestSnapshotIO;
          module StackTrace = StackTrace;
          module Mock = Mock;
          let maxNumStackFrames = 3;
        };
        module Runner = TestSuiteRunner.Make(RunnerConfig);
        let startTime = config.getTime();
        let notifyReporters = f => List.iter(f, config.reporters);
        let reporterTestSuites =
          testSuites^
          |> List.map(s =>
               switch (s) {
               | TestSuite({name}, _, _) => {name: name}
               }
             );
        notifyReporters(r => r.onRunStart({testSuites: reporterTestSuites}));

        let result =
          testSuites^
          |> List.fold_left(
               (prevAggregatedResult, testSuite) => {
                 let TestSuite({name}, _, _) = testSuite;
                 let reporterSuite = {name: name};
                 notifyReporters(r => r.onTestSuiteStart(reporterSuite));
                 let describeResult = Runner.run(testSuite);
                 let testSuiteResult =
                   TestSuiteResult.ofDescribeResult(describeResult);
                 let newResult =
                   prevAggregatedResult
                   |> AggregatedResult.addTestSuiteResult(testSuiteResult);
                 notifyReporters(r =>
                   r.onTestSuiteResult(
                     reporterSuite,
                     newResult,
                     testSuiteResult,
                   )
                 );
                 newResult;
               },
               AggregatedResult.initialAggregatedResult(
                 List.length(testSuites^),
                 startTime,
               ),
             );
        TestSnapshot.removeUnusedSnapshots(RunnerConfig.snapshotState^);
        let aggregatedResultWithSnapshotStatus = {
          ...result,
          snapshotSummary:
            Some(
              TestSnapshot.getSnapshotStatus(RunnerConfig.snapshotState^),
            ),
        };

        let success = aggregatedResultWithSnapshotStatus.numFailedTests == 0;
        notifyReporters(r =>
          r.onRunComplete(aggregatedResultWithSnapshotStatus)
        );
        if (!success) {
          config.onTestFrameworkFailure();
        };
        ();
      })
    );

  let cli = () => {
    let shouldUpdateSnapshots =
      Array.length(Sys.argv) >= 2 && Sys.argv[1] == "-u";
    let config =
      RunConfig.(initialize() |> updateSnapshots(shouldUpdateSnapshots));
    run(config);
  };
};
