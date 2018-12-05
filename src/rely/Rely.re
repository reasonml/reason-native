/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open MatcherUtils;
open SnapshotIO;
open Common.Option.Infix;
open Common.Collections;
open TestResult;
include TestFrameworkConfig;
include RunConfig;
include Test;
open Reporter;

module Describe = {
  type describeConfig = {
    updateSnapshots: bool,
    snapshotDir: string,
  };

  type describeUtils = {
    describe: describeFn,
    test: Test.testFn,
  }
  and describeFn = (string, describeUtils => unit) => unit;
};

open Describe;

exception TestAlreadyRan(string);
exception PendingTestException(string);

type testRunState = {
  testHashes: MStringSet.t,
  snapshotState: ref(Snapshot.state),
};

type runDescribeFn =
  (
    ~config: Describe.describeConfig,
    ~state: testRunState,
    ~describePath: TestPath.describe,
    Describe.describeUtils => unit
  ) =>
  describeResult;

module type TestFramework = {
  let describe: Describe.describeFn;
  let run: RunConfig.t => unit;
  let cli: unit => unit;
};

module type FrameworkConfig = {let config: TestFrameworkConfig.t;};

module Make = (UserConfig: FrameworkConfig) => {
  module TestSnapshot = Snapshot.Make(SnapshotIO.FileSystemSnapshotIO);
  module TestSnapshotIO = SnapshotIO.FileSystemSnapshotIO;
  module StackTrace =
    StackTrace.Make({
      let baseDir = UserConfig.config.projectDir;
      let exclude = ["Rely.re", "matchers/"];
      let formatLink = Pastel.cyan;
      let formatText = Pastel.dim;
    });

  let escape = (s: string): string => {
    let lines = Str.split(Str.regexp("\n"), s);
    let lines = List.map(line => String.escaped(line), lines);
    String.concat("\n", lines);
  };

  let ifSome = (opt, some, none) =>
    switch (opt) {
    | Some(opt) => some(opt)
    | None => none
    };

  let failFormatter = s => Pastel.red(s);

  let passFormatter = s => Pastel.green(s);

  let dimFormatter = Pastel.dim;

  let maxNumStackFrames = 3;

  let sanitizeName = (name: string): string => {
    ();
    let name =
      name
      |> Str.split(Str.regexp("\\b"))
      |> List.map(Str.global_replace(Str.regexp("[^a-zA-Z]"), ""))
      |> List.filter(part => String.length(part) > 0)
      |> String.concat("_");
    let name =
      if (String.length(name) > 50) {
        String.sub(name, 0, 50) ++ "_";
      } else {
        name;
      };
    name;
  };

  /* This will generate unique IDs for describes. */
  let describeCounter = Counter.create();

  let rec runDescribe: runDescribeFn =
    (
      ~config: Describe.describeConfig,
      ~state,
      ~describePath: TestPath.describe,
      fn,
    ) => {
      open Test;
      open Describe;
      open RunConfig;
      let describeName = TestPath.(Describe(describePath) |> toString);

      let testHashes = state.testHashes;
      /* This will generate unique IDs for tests within this describe. */
      let testCounter = Counter.create();
      /* The hashes for all tests that have been run for this describe. */
      let finishedTestHashes = MStringSet.empty();
      /* Mutable map tracking the status of each test for this describe. */
      let testMap = MIntMap.empty();
      let updateTestResult = (testId: int, update: testResult): unit => {
        let prev = MIntMap.getOpt(testId, testMap);
        switch (prev) {
        | Some(PendingTestResult(_)) =>
          MIntMap.set(testId, FinalTestResult(update), testMap)
        | _ => ()
        };
      };
      /* Convert describeName to something reasonable for a file name. */
      let describeFileName = describeName |> sanitizeName;
      /* Create the test function we will pass around. */
      let testFn: Test.testFn =
        (testName, usersTest) => {
          let testPath = (testName, describePath);
          let testId = Counter.next(testCounter);
          /* This will generate unique IDs for expects within this test call. */
          let expectCounter = Counter.create();
          let testTitle = TestPath.testToString(testPath);
          let testHash = ref(None);
          let i = ref(0);
          let break = ref(false);
          while (! break^ && i^ < 10000) {
            let testHashAttempt = TestPath.hash(Test(testPath), i^);
            switch (testHash^) {
            | None =>
              if (!MStringSet.has(testHashAttempt, testHashes)) {
                break := true;
                testHash := Some(testHashAttempt);
                let _ = MStringSet.add(testHashAttempt, testHashes);
                ();
              }
            | _ => ()
            };
            incr(i);
          };
          let testHash =
            switch (testHash^) {
            | None => "hash_conflict"
            | Some(testHash) => testHash
            };
          let updateTestResult = updateTestResult(testId);
          let snapshotPrefix =
            Filename.concat(config.snapshotDir, describeFileName);
          module TestSnapshotMatcher =
            SnapshotMatchers.Make(
              {
                let markSnapshotUsed = (snapshot: string) => {
                  state.snapshotState :=
                    TestSnapshot.markSnapshotUsed(
                      snapshot,
                      state.snapshotState^,
                    );
                  ();
                };
                let markSnapshotUpdated = (snapshot: string) =>
                  state.snapshotState :=
                    TestSnapshot.markSnapshotUpdated(
                      snapshot,
                      state.snapshotState^,
                    );
                let testHash = testHash;
                let genExpectID = () => Counter.next(expectCounter);
                let testTitle = testTitle;
                let updateSnapshots = config.updateSnapshots;
                let snapshotPrefix = snapshotPrefix;
              },
              TestSnapshotIO,
            );
          let createMatcher = matcherConfig => {
            let matcher = (actualThunk, expectedThunk) => {
              switch (matcherConfig(matcherUtils, actualThunk, expectedThunk)) {
              | (messageThunk, true) => ()
              | (messageThunk, false) =>
                state.snapshotState :=
                  TestSnapshot.markSnapshotsAsCheckedForTest(
                    testTitle,
                    state.snapshotState^,
                  );
                let stackTrace = StackTrace.getStackTrace();
                let location = StackTrace.getTopLocation(stackTrace);
                let stack =
                  StackTrace.stackTraceToString(
                    stackTrace,
                    maxNumStackFrames,
                  );
                updateTestResult({
                  path: testPath,
                  duration: None,
                  testStatus: Failed(messageThunk(), location, stack),
                  title: testName,
                  fullName: testTitle,
                });
              };
              ();
            };
            matcher;
          };

          let expectUtils: MatcherTypes.extendUtils = {
            createMatcher: createMatcher,
          };

          let testUtils = {
            expect:
              DefaultMatchers.makeDefaultMatchers(
                expectUtils,
                TestSnapshotMatcher.makeMatchers,
              ),
          };
          let runTest = () => {
            switch (usersTest(testUtils)) {
            | () =>
              updateTestResult({
                path: testPath,
                duration: None,
                testStatus: Passed,
                title: testName,
                fullName: testTitle,
              })
            | exception e =>
              let exceptionTrace = StackTrace.getExceptionStackTrace();
              let location = StackTrace.getTopLocation(exceptionTrace);
              let stackTrace =
                StackTrace.stackTraceToString(
                  exceptionTrace,
                  maxNumStackFrames,
                );
              state.snapshotState :=
                TestSnapshot.markSnapshotsAsCheckedForTest(
                  testTitle,
                  state.snapshotState^,
                );
              updateTestResult({
                path: testPath,
                duration: None,
                testStatus: Exception(e, location, stackTrace),
                title: testName,
                fullName: testTitle,
              });
            };
            let _ = MStringSet.add(testHash, finishedTestHashes);
            ();
          };
          /* Update testMap with initial Pending result. */
          let _ =
            MIntMap.set(
              testId,
              PendingTestResult({path: testPath, runTest}),
              testMap,
            );
          ();
        };
      /* Prepend the original describe name before the next one when nesting */
      let childDescribeResults = ref([]);
      let describeFn = (describeName, fn) => {
        let childDescribeResult =
          runDescribe(
            ~config,
            ~state,
            ~describePath=Nested(describeName, describePath),
            fn,
          );
        childDescribeResults := childDescribeResults^ @ [childDescribeResult];
      };
      let utils = {describe: describeFn, test: testFn};
      /* Gather all the tests */
      fn(utils);
      /* Now run them */
      let _ =
        MIntMap.forEach(
          (test, _) =>
            switch (test) {
            | PendingTestResult(t) => t.runTest()
            | FinalTestResult(t) => raise(TestAlreadyRan(t.fullName))
            },
          testMap,
        );
      let testResults =
        MIntMap.values(testMap)
        |> List.map(test =>
             switch (test) {
             | PendingTestResult(t) =>
               raise(PendingTestException(t.path |> TestPath.testToString))
             | FinalTestResult(t) => t
             }
           );

      let describeResult = {
        path: describePath,
        duration: None,
        describeResults: childDescribeResults^,
        testResults,
      };
      describeResult;
    };

  type testSuiteInternal = {
    name: string,
    fn: Describe.describeUtils => unit,
  };

  let testSuites: ref(list(testSuiteInternal)) = ref([]);
  let describe = (name, fn) => testSuites := testSuites^ @ [{name, fn}];
  type testSuiteAccumulator = {
    testRunState,
    aggregatedResult: AggregatedResult.t,
  };

  let run = (config: RunConfig.t) =>
    Util.withBacktrace(() => {
      let notifyReporters = f => List.iter(f, config.reporters);
      notifyReporters(r =>
        r.onRunStart({
          testSuites: testSuites^ |> List.map(s => {name: s.name}),
        })
      );
      let initialState = {
        testHashes: MStringSet.empty(),
        snapshotState:
          ref(
            TestSnapshot.initializeState(
              ~snapshotDir=UserConfig.config.snapshotDir,
              ~updateSnapshots=config.updateSnapshots,
            ),
          ),
      };
      let result =
        testSuites^
        |> List.fold_left(
             (acc, testSuite) => {
               let reporterSuite = {name: testSuite.name};
               notifyReporters(r => r.onTestSuiteStart(reporterSuite));
               let describeResult =
                 runDescribe(
                   ~config={
                     updateSnapshots: config.updateSnapshots,
                     snapshotDir: UserConfig.config.snapshotDir,
                   },
                   ~state=acc.testRunState,
                   ~describePath=Terminal(testSuite.name),
                   testSuite.fn,
                 );
               let testSuiteResult =
                 TestSuiteResult.ofDescribeResult(describeResult);
               let newResult =
                 acc.aggregatedResult
                 |> AggregatedResult.addTestSuiteResult(testSuiteResult);
               notifyReporters(r =>
                 r.onTestSuiteResult(
                   reporterSuite,
                   newResult,
                   testSuiteResult,
                 )
               );
               {...acc, aggregatedResult: newResult};
             },
             {
               testRunState: initialState,
               aggregatedResult:
                 AggregatedResult.initialAggregatedResult(
                   List.length(testSuites^),
                 ),
             },
           );
      let aggregatedResultWithSnapshotStatus = {
        ...result.aggregatedResult,
        snapshotSummary:
          Some(
            TestSnapshot.getSnapshotStatus(
              result.testRunState.snapshotState^,
            ),
          ),
      };
      let success = aggregatedResultWithSnapshotStatus.numFailedTests == 0;
      TestSnapshot.removeUnusedSnapshots(result.testRunState.snapshotState^);
      /* todo cleanup snapshots if failed */
      notifyReporters(r =>
        r.onRunComplete(aggregatedResultWithSnapshotStatus)
      );
      if (!success) {
        config.onTestFrameworkFailure();
      };
      ();
    });

  let cli = () => {
    let shouldUpdateSnapshots =
      Array.length(Sys.argv) >= 2 && Sys.argv[1] == "-u";
    let config =
      RunConfig.(initialize() |> updateSnapshots(shouldUpdateSnapshots));
    run(config);
  };
};
