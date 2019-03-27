/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestSuite;
open TestResult;
open CommonCollections;
open MatcherUtils;
open Test;
open Describe;
open Util;
open RunConfig;
open Reporter;
exception PendingTestException(string);

module type TestSuiteRunnerConfig = {
  let getTime: unit => Time.t;
  let updateSnapshots: bool;
  let maxNumStackFrames: int;

  let testHashes: MStringSet.t;
  let snapshotState: ref(Snapshot.state);
  let snapshotDir: string;
  module SnapshotIO: SnapshotIO.SnapshotIO;
};

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

module Make = (Config: TestSuiteRunnerConfig) => {
  module TestSnapshot = Snapshot.Make(Config.SnapshotIO);
  let run: TestSuite.t => TestResult.describeResult =
    (TestSuite({name, tests, describes, skip}, extension, (module Context))) => {
      module DefaultMatchers = DefaultMatchers.Make(Context.Mock);
      let makeMakeSnapshotMatchers = (describeFileName, testHash, testTitle) => {
        let snapshotPrefix =
          Filename.concat(Config.snapshotDir, describeFileName);
        let expectCounter = Counter.create();
        module SnapshotMatchers =
          SnapshotMatchers.Make(
            {
              let markSnapshotUsed = (snapshot: string) => {
                Config.snapshotState :=
                  TestSnapshot.markSnapshotUsed(
                    snapshot,
                    Config.snapshotState^,
                  );
                ();
              };
              let markSnapshotUpdated = (snapshot: string) =>
                Config.snapshotState :=
                  TestSnapshot.markSnapshotUpdated(
                    snapshot,
                    Config.snapshotState^,
                  );
              let testHash = testHash;
              let genExpectID = () => Counter.next(expectCounter);
              let testTitle = testTitle;
              let updateSnapshots = Config.updateSnapshots;
              let snapshotPrefix = snapshotPrefix;
            },
            Config.SnapshotIO,
          );
        SnapshotMatchers.makeMatchers;
      };

      let getTestHash = testPath => {
        let testHash = ref(None);
        let i = ref(0);
        let break = ref(false);
        while (! break^ && i^ < 10000) {
          let testHashAttempt = TestPath.hash(Test(testPath), i^);
          switch (testHash^) {
          | None =>
            if (!MStringSet.has(testHashAttempt, Config.testHashes)) {
              break := true;
              testHash := Some(testHashAttempt);
              let _ = MStringSet.add(testHashAttempt, Config.testHashes);
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
        testHash;
      };

      let executeTest =
          (describePath, name, location, usersTest, extensionFn, testId) => {
        let testStatus = ref(None);
        let testPath = (name, describePath);
        let testTitle = TestPath.testToString(testPath);
        let describeFileName =
          TestPath.(Describe(describePath) |> toString) |> sanitizeName;

        let updateTestStatus = status =>
          switch (testStatus^) {
          | None => testStatus := Some(status)
          | _ => ()
          };
        let testHash = getTestHash(testPath);
        let createMatcher = matcherConfig => {
          let matcher = (actualThunk, expectedThunk) => {
            switch (matcherConfig(matcherUtils, actualThunk, expectedThunk)) {
            | (messageThunk, true) => ()
            | (messageThunk, false) =>
              Config.snapshotState :=
                TestSnapshot.markSnapshotsAsCheckedForTest(
                  testTitle,
                  Config.snapshotState^,
                );
              let stackTrace = Context.StackTrace.getStackTrace();
              let location = Context.StackTrace.getTopLocation(stackTrace);
              let stack =
                Context.StackTrace.stackTraceToString(
                  stackTrace,
                  Config.maxNumStackFrames,
                );
              updateTestStatus(Failed(messageThunk(), location, stack));
            };
            ();
          };
          matcher;
        };
        let extendUtils: MatcherTypes.extendUtils = {
          createMatcher: createMatcher,
        };

        let makeSnapshotMatchers =
          makeMakeSnapshotMatchers(describeFileName, testHash, testTitle);

        let testUtils = {
          expect:
            DefaultMatchers.makeDefaultMatchers(
              extendUtils,
              makeSnapshotMatchers,
              extensionFn,
            ),
        };

        let {startTime, endTime} =
          Util.time(
            Config.getTime,
            () => {
              let _ =
                switch (usersTest(testUtils)) {
                | () => updateTestStatus(Passed(location))
                | exception e =>
                  let exceptionTrace =
                    Context.StackTrace.getExceptionStackTrace();
                  let location =
                    Context.StackTrace.getTopLocation(exceptionTrace);
                  let stackTrace =
                    Context.StackTrace.stackTraceToString(
                      exceptionTrace,
                      Config.maxNumStackFrames,
                    );
                  Config.snapshotState :=
                    TestSnapshot.markSnapshotsAsCheckedForTest(
                      testTitle,
                      Config.snapshotState^,
                    );
                  updateTestStatus(Exception(e, location, stackTrace));
                };
              ();
            },
          );
        {
          path: testPath,
          duration: Some(Time.subtract(endTime, startTime)),
          testStatus:
            switch (testStatus^) {
            | Some(status) => status
            | None =>
              raise(PendingTestException(TestPath.testToString(testPath)))
            },
          title: name,
          fullName: TestPath.testToString(testPath),
        };
      };

      let skipTest = (describePath, name, location) => {
        let testPath = (name, describePath);
        {
          path: testPath,
          duration: None,
          testStatus: Skipped(location),
          title: name,
          fullName: TestPath.testToString(testPath),
        };
      };

      let rec runDescribe = (path, tests, describes, extensionFn, skip) => {
        let testCounter = Counter.create();
        let startTime = Config.getTime();
        let testResults =
          List.map(
            test => {
              let testId = Counter.next(testCounter);
              switch (test, skip) {
              | (Test({name, location}), true)
              | (Skipped({name, location}), _) =>
                skipTest(path, name, location)
              | (Test({name, usersTest, location}), false) =>
                executeTest(
                  path,
                  name,
                  location,
                  usersTest,
                  extensionFn,
                  testId,
                )
              };
            },
            tests,
          );
        let describeResults =
          List.map(
            ({name, tests, describes, skip}) =>
              runDescribe(
                TestPath.Nested(name, path),
                tests,
                describes,
                extensionFn,
                skip,
              ),
            describes,
          );
        {
          path,
          testResults,
          describeResults,
          startTime: Some(startTime),
          endTime: Some(Config.getTime()),
        };
      };

      let testSuitePath = TestPath.Terminal(name);
      runDescribe(testSuitePath, tests, describes, extension, skip);
    };
  let runTestSuites = (testSuites: list(TestSuite.t), config: RunConfig.t) => {
    let startTime = config.getTime();
    let notifyReporters = f => List.iter(f, config.reporters);
    let reporterTestSuites =
      testSuites
      |> List.map(s =>
           switch (s) {
           | TestSuite({name}, _, _) => {name: name}
           }
         );
    notifyReporters(r => r.onRunStart({testSuites: reporterTestSuites}));
    let result =
      testSuites
      |> List.fold_left(
           (prevAggregatedResult, testSuite) => {
             let TestSuite({name}, _, _) = testSuite;
             let reporterSuite = {name: name};
             notifyReporters(r => r.onTestSuiteStart(reporterSuite));
             let describeResult = run(testSuite);
             let testSuiteResult =
               TestSuiteResult.ofDescribeResult(describeResult);
             let newResult =
               prevAggregatedResult
               |> AggregatedResult.addTestSuiteResult(testSuiteResult);
             notifyReporters(r =>
               r.onTestSuiteResult(reporterSuite, newResult, testSuiteResult)
             );
             newResult;
           },
           AggregatedResult.initialAggregatedResult(
             List.length(testSuites),
             startTime,
           ),
         );
    TestSnapshot.removeUnusedSnapshots(Config.snapshotState^);
    let aggregatedResultWithSnapshotStatus = {
      ...result,
      snapshotSummary:
        Some(TestSnapshot.getSnapshotStatus(Config.snapshotState^)),
    };
    let success = aggregatedResultWithSnapshotStatus.numFailedTests == 0;
    notifyReporters(r => r.onRunComplete(aggregatedResultWithSnapshotStatus));
    if (!success) {
      config.onTestFrameworkFailure();
    };
    ();
  };
};
