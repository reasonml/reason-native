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
open CommonOption.Infix;

exception PendingTestException(string);
exception InvalidInCIMode(string);

module SnapshotModuleSet =
  Set.Make({
    type t = (TestSuite.contextId, module Snapshot.Sig);
    let compare = ((contextId1, module1), (contextId2, module2)) =>
      Stdlib.compare(contextId1, contextId2);
  });

module type TestSuiteRunnerConfig = {
  let getTime: unit => Time.t;
  let updateSnapshots: bool;
  let removeUnusedSnapshots: bool;
  let testNamePattern: option(string);
  let maxNumStackFrames: int;
  let ci: bool;
  let reporters: list(Reporter.t);
  let onTestFrameworkFailure: unit => unit;
};

let nonLetterRegex = Re.compile(Re.Pcre.re("[^a-zA-Z]"));
let wordBoundary = Re.compile(Re.Pcre.re("\\b"));

let sanitizeName = (name: string): string => {
  ();
  let name =
    name
    |> Re.split(wordBoundary)
    |> List.map(Re.replace_string(nonLetterRegex, ~by=""))
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
  let runTestSuite: (TestSuite.t, TestSuite.mode) => TestResult.describeResult =
    (
      TestSuite(
        {name, tests, describes, mode},
        extension,
        lifecycle,
        (_, (module Context)),
      ),
      executionMode,
    ) => {
      module DefaultMatchers = DefaultMatchers.Make(Context.Mock);
      let all = lifecycle.beforeAll();

      let makeMakeSnapshotMatchers = (describeFileName, testPath, testId) => {
        let expectCounter = Counter.create();
        module SnapshotMatchers =
          SnapshotMatchers.Make({
            module Snapshot = Context.Snapshot;
            let testPath = testPath;
            let genExpectID = () => Counter.next(expectCounter);
            let updateSnapshots = Config.updateSnapshots;
            let testId = testId;
          });
        SnapshotMatchers.makeMatchers;
      };

      let executeTest = (describePath, name, location, usersTest, extensionFn) => {
        let testStatus = ref(None);
        let assertionState = ref(AssertionState.initialState);
        let testPath = (name, describePath);
        let describeFileName =
          TestPath.(Describe(describePath) |> toString) |> sanitizeName;
        let testId = Context.Snapshot.getNewId(testPath);

        let updateTestStatus = status =>
          switch (testStatus^) {
          | None => testStatus := Some(status)
          | _ => ()
          };
        let createMatcher = matcherConfig => {
          let matcher = (actualThunk, expectedThunk) => {
            switch (matcherConfig(matcherUtils, actualThunk, expectedThunk)) {
            | (messageThunk, true) =>
              assertionState :=
                AssertionState.addAssertionResult(assertionState^)
            | (messageThunk, false) =>
              Context.Snapshot.markSnapshotsAsCheckedForTest(testId);
              let message = messageThunk();
              let stackTrace = Context.StackTrace.getStackTrace();
              let location = Context.StackTrace.getTopLocation(stackTrace);
              let stack =
                Context.StackTrace.stackTraceToString(
                  stackTrace,
                  Config.maxNumStackFrames,
                );
              assertionState :=
                AssertionState.addAssertionResult(assertionState^);
              updateTestStatus(Failed(message, location, stack));
            };
            ();
          };
          matcher;
        };

        let extendUtils: MatcherTypes.extendUtils = {
          createMatcher: createMatcher,
        };

        let makeSnapshotMatchers =
          makeMakeSnapshotMatchers(describeFileName, testPath, testId);

        let env = lifecycle.beforeEach(all);
        let baseMatchers =
          DefaultMatchers.makeDefaultMatchers(
            extendUtils,
            makeSnapshotMatchers,
            extensionFn,
          );

        let assertions = n => {
          let stackTrace = Context.StackTrace.getStackTrace();
          let location = Context.StackTrace.getTopLocation(stackTrace);
          let stack =
            Context.StackTrace.stackTraceToString(
              stackTrace,
              Config.maxNumStackFrames,
            );
          assertionState :=
            AssertionState.setExpectation(
              HasNAssertions(n, location, stack),
              assertionState^,
            );
        };

        let hasAssertions = () => {
          let stackTrace = Context.StackTrace.getStackTrace();
          let location = Context.StackTrace.getTopLocation(stackTrace);
          let stack =
            Context.StackTrace.stackTraceToString(
              stackTrace,
              Config.maxNumStackFrames,
            );
          assertionState :=
            AssertionState.setExpectation(
              HasAssertions(location, stack),
              assertionState^,
            );
        };

        let testUtils = {
          expect: {
            string: baseMatchers.string,
            file: baseMatchers.file,
            lines: baseMatchers.lines,
            bool: baseMatchers.bool,
            int: baseMatchers.int,
            float: baseMatchers.float,
            fn: baseMatchers.fn,
            list: baseMatchers.list,
            array: baseMatchers.array,
            equal: baseMatchers.equal,
            notEqual: baseMatchers.notEqual,
            same: baseMatchers.same,
            notSame: baseMatchers.notSame,
            mock: baseMatchers.mock,
            option: baseMatchers.option,
            result: baseMatchers.result,
            ext: baseMatchers.ext,
            assertions,
            hasAssertions,
          },
          env,
        };

        let {startTime, endTime} =
          Util.time(
            Config.getTime,
            () => {
              let _ =
                switch (usersTest(testUtils)) {
                | () =>
                  lifecycle.afterEach(env);
                  switch (
                    AssertionState.validateAssertionState(
                      matcherUtils,
                      assertionState^,
                    )
                  ) {
                  | Valid => updateTestStatus(Passed(location))
                  | Invalid(message, loc, stack) =>
                    updateTestStatus(Failed(message, loc, stack))
                  };
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
                  Context.Snapshot.markSnapshotsAsCheckedForTest(testId);
                  lifecycle.afterEach(env);
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
        let testId = Context.Snapshot.getNewId(testPath);
        Context.Snapshot.markSnapshotsAsCheckedForTest(testId);
        {
          path: testPath,
          duration: None,
          testStatus: Skipped(location),
          title: name,
          fullName: TestPath.testToString(testPath),
        };
      };

      let rec runDescribe =
              (path, tests, describes, extensionFn, executionMode) => {
        let startTime = Config.getTime();
        let testResults =
          List.map(
            test =>
              switch (executionMode, test) {
              | (Skip, {name, location})
              | (Only, {name, location, mode: Normal})
              | (_, {name, location, mode: Skip}) =>
                skipTest(path, name, location)
              | (Only, {name, usersTest, location, mode: Only})
              | (Normal, {name, usersTest, location, _}) =>
                executeTest(path, name, location, usersTest, extensionFn)
              },
            tests,
          );
        let describeResults =
          List.map(
            ({name, tests, describes, mode}) => {
              let childMode = mode === Skip ? Skip : executionMode;
              runDescribe(
                TestPath.Nested(name, path),
                tests,
                describes,
                extensionFn,
                childMode,
              );
            },
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
      switch (
        runDescribe(testSuitePath, tests, describes, extension, executionMode)
      ) {
      | exception e =>
        lifecycle.afterAll(all);
        raise(e);
      | result =>
        lifecycle.afterAll(all);
        result;
      };
    };

  let getSnapshotResult = testSuites => {
    let snapshotModules: list((TestSuite.contextId, module Snapshot.Sig)) =
      List.map(
        (TestSuite(_, _, _, (id, (module Context)))) => {
          let snapshotModule: module Snapshot.Sig = (module Context.Snapshot);
          (id, snapshotModule);
        },
        testSuites,
      );

    let uniqueModules =
      SnapshotModuleSet.fromList(snapshotModules)
      |> SnapshotModuleSet.toList
      |> List.map(((_, module SnapshotModule: Snapshot.Sig)) => {
           let snapshotModule: module Snapshot.Sig = (module SnapshotModule);
           snapshotModule;
         });

    if (Config.removeUnusedSnapshots) {
      let _ =
        List.iter(
          (module SnapshotModule: Snapshot.Sig) =>
            SnapshotModule.removeUnusedSnapshots(),
          uniqueModules,
        );
      ();
    };

    let aggregateSnapshotResult =
      List.fold_left(
        (acc, module SnapshotModule: Snapshot.Sig) => {
          let snapshotSummary = SnapshotModule.getSnapshotStatus();
          AggregatedResult.{
            numCreatedSnapshots:
              acc.numCreatedSnapshots + snapshotSummary.numCreatedSnapshots,
            numRemovedSnapshots:
              acc.numRemovedSnapshots + snapshotSummary.numRemovedSnapshots,
            numUpdatedSnapshots:
              acc.numUpdatedSnapshots + snapshotSummary.numUpdatedSnapshots,
          };
        },
        {
          numCreatedSnapshots: 0,
          numRemovedSnapshots: 0,
          numUpdatedSnapshots: 0,
        },
        uniqueModules,
      );
    aggregateSnapshotResult;
  };
  let runTestSuites = (testSuites: list(TestSuite.t)) => {
    let startTime = Config.getTime();
    let notifyReporters = f => List.iter(f, Config.reporters);
    let hasOnly =
      testSuites
      |> List.exists((TestSuite({mode}, _, _, _)) => mode === Only);

    if (Config.ci && hasOnly) {
      raise(
        InvalidInCIMode(
          "describeOnly and testOnly should not be called in CI mode or committed. They are intended to be used only while developing locally",
        ),
      );
    };

    let reporterTestSuites =
      testSuites
      |> List.map(s =>
           switch (s) {
           | TestSuite({name}, _, _, _) => {name: name}
           }
         );
    notifyReporters(r =>
      r.onRunStart({
        testSuites: reporterTestSuites,
        testNamePattern: Config.testNamePattern,
      })
    );
    let result =
      testSuites
      |> List.fold_left(
           (prevAggregatedResult, testSuite) => {
             let TestSuite({name, mode}, _, _, _) = testSuite;
             let reporterSuite = {name: name};
             notifyReporters(r => r.onTestSuiteStart(reporterSuite));
             let executionMode =
               switch (hasOnly, mode) {
               | (true, Only) => Only
               | (true, _) => Skip
               | (false, mode) => mode
               };
             let describeResult = runTestSuite(testSuite, executionMode);
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
    let snapshotSummary = getSnapshotResult(testSuites);

    let aggregatedResultWithSnapshotStatus = {
      ...result,
      snapshotSummary: Some(snapshotSummary),
    };
    let success = aggregatedResultWithSnapshotStatus.numFailedTests == 0;
    notifyReporters(r => r.onRunComplete(aggregatedResultWithSnapshotStatus));
    if (!success) {
      Config.onTestFrameworkFailure();
    };
    ();
  };
};

let run = (runConfig: RunConfig.t, testSuites) =>
  Util.withBacktrace(() => {
    let testNamePattern = ref(None);
    let testSuites =
      switch (runConfig.testNamePattern) {
      | Some(pattern) =>
        testNamePattern.contents =
          Some(String.concat("", ["\\", pattern, "\\i"]));
        TestSuiteFilter.filterTestSuitesByRegex(
          testSuites,
          Re.Pcre.regexp(~flags=[`CASELESS], pattern),
        );

      | None => testSuites
      };
    module RunnerConfig = {
      let getTime = runConfig.getTime;
      let maxNumStackFrames = 3;
      let updateSnapshots = runConfig.updateSnapshots;
      let ci = runConfig.ci;
      let removeUnusedSnapshots =
        CommonOption.isNone(runConfig.testNamePattern);
      let testNamePattern = testNamePattern.contents;
      let reporters =
        runConfig.reporters
        |> List.map(reporter =>
             switch (reporter) {
             | RunConfig.Default =>
               TerminalReporter.createTerminalReporter(
                 ~getTime=runConfig.getTime,
                 {
                   printEndline: print_endline,
                   printNewline: print_newline,
                   printString: print_string,
                   flush,
                 },
               )
             | Custom(reporter) => reporter
             }
           );
      let onTestFrameworkFailure = runConfig.onTestFrameworkFailure;
    };
    module Runner = Make(RunnerConfig);

    Runner.runTestSuites(testSuites);
  });
