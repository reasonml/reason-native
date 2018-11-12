 /**
 * Copyright 2004-present Facebook. All Rights Reserved.
 */
open Collections;
open MatcherUtils;
open SnapshotIO;

module FCP =
  FileContextPrinter.Make({
    let linesBefore = 3;
    let linesAfter = 3;
  });

let (>>=) = (o, f) =>
  switch (o) {
  | Some(x) => f(x)
  | None => None
};
let (|?:) = (o, default) =>
  switch (o) {
  | Some(value) => value
  | None => default
};
let (>>|) = (opt, fn) =>
  switch (opt) {
  | Some(value) => Some(fn(value))
  | None => None
};

module Test = {
  type testResult =
    | Pending
    | Passed
    | Failed(string, option(Printexc.location), string)
    | Exception(exn, option(Printexc.location), string);

  type testUtils = {expect: DefaultMatchers.matchers};
  type testFn = (string, testUtils => unit) => unit;
  type testSpec = {
    testID: int,
    name: string,
    runTest: unit => unit,
    mutable testResult,
  };
};

module Describe = {
  type describeConfig = {
    updateSnapshots: bool,
    updateSnapshotsFlag: string,
    snapshotDir: string,
  };
  type describeState = {
    testHashes: MStringSet.t,
    snapshotState: ref(Snapshot.state),
  };
  type describeUtils = {
    describe: describeFn,
    test: Test.testFn,
  }
  and rootDescribeFn =
    (
      ~config: describeConfig,
      ~isRootDescribe: bool=?,
      ~state: option(describeState)=?,
      string,
      describeUtils => unit
    ) =>
    unit
  and describeFn = (string, describeUtils => unit) => unit;
};

module Make = (TestSnapshotIO: SnapshotIO) => {
  module TestSnapshot = Snapshot.Make(TestSnapshotIO);

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

  let failFormatter = s => Chalk.red(s);

  let passFormatter = s => Chalk.green(s);

  let dimFormatter = Chalk.dim;

  let maxNumStackFrames = 3;

  let indent = (~indent: string, s: string): string => {
    let lines = Str.split(Str.regexp("\n"), s);
    let lines = List.map(line => indent ++ line, lines);
    String.concat("\n", lines);
  };

  let messageIndent = "    ";
  let stackIndent = "      ";
  let titleIndent = "  ";
  let ancestrySeparator = " › ";

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

  let rec rootDescribe: Describe.rootDescribeFn =
    (
      ~config: Describe.describeConfig,
      ~isRootDescribe=true,
      ~state=None,
      describeName,
      fn,
    ) => {
      open Test;
      open Describe;
      module StackTrace =
        StackTrace.Make({
          let baseDir =
            config.snapshotDir |> Filename.dirname |> Filename.dirname;
          let exclude = ["TestRunner.re", "Matcher"];
          let formatLink = Chalk.cyan;
          let formatText = Chalk.dim;
        });
      let state =
        switch (state) {
        | Some(state) => state
        | None => {
            testHashes: MStringSet.empty(),
            snapshotState:
              ref(
                TestSnapshot.initializeState(
                  ~snapshotDir=config.snapshotDir,
                  ~updateSnapshots=config.updateSnapshots,
                ),
              ),
          }
        };

      let printSnapshotStatus = () =>
        if (isRootDescribe) {
          print_endline(
            TestSnapshot.getSnapshotStatus(state.snapshotState^),
          );
        };
      let testHashes = state.testHashes;
      /* This will generate unique IDs for tests within this describe. */
      let testCounter = Counter.create();
      /* The hashes for all tests that have been run for this describe. */
      let finishedTestHashes = MStringSet.empty();
      /* Mutable map tracking the status of each test for this describe. */
      let testMap = MIntMap.empty();
      let updateTestResult = (testID: int, testResult: testResult): unit => {
        let prev = MIntMap.getOpt(testID, testMap);
        switch (prev) {
        | Some({testResult: Pending} as prev) =>
          prev.testResult = testResult;
          ();
        | _ => ()
        };
      };
      /* Convert describeName to something reasonable for a file name. */
      let describeFileName = sanitizeName(describeName);
      /* Create the test function we will pass around. */
      let testFn: Test.testFn =
        (testName, usersTest) => {
          let testID = Counter.next(testCounter);
          /* This will generate unique IDs for expects within this test call. */
          let expectCounter = Counter.create();
          let testTitle = describeName ++ ancestrySeparator ++ testName;
          let testHash = ref(None);
          let i = ref(0);
          let break = ref(false);
          let ancestryRegex = Str.regexp_string(ancestrySeparator);
          while (! break^ && i^ < 10000) {
            let testHashAttempt =
              String.sub(
                Strings.md5(
                  Str.global_replace(ancestryRegex, "", testTitle)
                  ++ string_of_int(i^),
                ),
                0,
                8,
              );
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
          let updateTestResult = updateTestResult(testID);
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
                let stackTrace = StackTrace.getStackTrace();
                let location = StackTrace.getTopLocation(stackTrace);
                let stack =
                  StackTrace.stackTraceToString(
                    stackTrace,
                    maxNumStackFrames,
                  );
                updateTestResult(Failed(messageThunk(), location, stack));
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
            | () => updateTestResult(Passed)
            | exception e =>
              let exceptionTrace = StackTrace.getExceptionStackTrace();
              let location = StackTrace.getTopLocation(exceptionTrace);
              let stackTrace =
                StackTrace.stackTraceToString(
                  exceptionTrace,
                  maxNumStackFrames,
                );
              updateTestResult(Exception(e, location, stackTrace));
            };
            let _ = MStringSet.add(testHash, finishedTestHashes);
            ();
          };
          /* Update testMap with initial Pending result. */
          let _ =
            MIntMap.set(
              testID,
              {testID, name: testName, runTest, testResult: Pending},
              testMap,
            );
          ();
        };
      /* Prepend the original describe name before the next one when nesting */
      let describeFn = (describeName2, fn) =>
        rootDescribe(
          ~config,
          ~isRootDescribe=false,
          ~state=Some(state),
          describeName ++ ancestrySeparator ++ describeName2,
          fn,
        );
      let utils = {describe: describeFn, test: testFn};
      /* Gather all the tests */
      fn(utils);
      /* Now run them */
      let total = ref(MIntMap.size(testMap));
      let failed = ref(0);

      if (total^ > 0) {
        let pending = ref(total^);
        let passed = ref(0);
        let prev = ref(0);
        let update = (first: bool): unit => {
          let pendingChalker = pending^ === 0 ? dimFormatter : (x => x);
          let passedChalker =
            passed^ === total^ ? passFormatter : dimFormatter;
          let failedChalker = failed^ === 0 ? dimFormatter : failFormatter;
          let update =
            pendingChalker(
              "["
              ++ string_of_int(pending^)
              ++ "/"
              ++ string_of_int(total^)
              ++ "] Pending",
            )
            ++ "  "
            ++ passedChalker(
                 "["
                 ++ string_of_int(passed^)
                 ++ "/"
                 ++ string_of_int(total^)
                 ++ "] Passed",
               )
            ++ "  "
            ++ failedChalker(
                 "["
                 ++ string_of_int(failed^)
                 ++ "/"
                 ++ string_of_int(total^)
                 ++ "] Failed",
               );
          let updateLength = String.length(update);
          let diff = prev^ - updateLength;
          if (diff > 0) {
            /* This moves back `diff` columns then clears to end of line */
            print_string(
              "\027[" ++ string_of_int(diff) ++ "D\027[K",
            );
          };
          print_string("\r" ++ update);
          flush(stdout);
          prev := updateLength;
        };
        print_endline(Chalk.whiteBright(describeName));
        update(true);
        let _ =
          MIntMap.forEach(
            (test, _) => {
              test.runTest();
              switch (test.testResult) {
              | Pending => failwith("How is this test still pending?")
              | Passed =>
                incr(passed);
                decr(pending);
              | Failed(_)
              | Exception(_) =>
                incr(failed);
                decr(pending);
              };
              update(false);
              ();
            },
            testMap,
          );
        print_newline();

        let getStackInfo = (optLoc: option(Printexc.location), trace: string) => {
          let stackInfo = (optLoc
            >>| (l: Printexc.location) => FCP.print(
              l.filename,
              (
                (l.line_number, l.start_char),
                (l.line_number, l.end_char),
              ),
            ))
            >>| ((optFileContext) => switch(optFileContext) {
              | Some(context) => String.concat("", [
                "\n\n",
                indent(context, ~indent=stackIndent),
                "\n\n",
                indent(trace, ~indent=stackIndent)
              ])
              | None => ""
            })
            |?: "";
            stackInfo;
        };

        if (failed^ > 0) {
          print_newline();
          let _ =
            testMap
            |> MIntMap.toList
            |> List.map(((_, test: testSpec)) => {
                 let resultString =
                   switch (test) {
                   | {testResult: Pending} => ""
                   | {testResult: Passed} => ""
                   | {testResult: Exception(e, loc, trace), name} =>
                     let titleBullet = "• ";
                     let title =
                       Chalk.bold(
                         failFormatter(
                           titleIndent
                           ++ titleBullet
                           ++ describeName
                           ++ ancestrySeparator
                           ++ name
                           ++ "\n",
                         ),
                       );

                     String.concat(
                       "",
                       [
                         title,
                         indent("Exception ", ~indent=messageIndent),
                         Chalk.dim(Printexc.to_string(e)),
                         getStackInfo(loc, trace)
                       ],
                     );
                   | {testResult: Failed(message, loc, stack), name} =>
                     let titleBullet = "• ";
                     let title =
                       Chalk.bold(
                         failFormatter(
                           titleIndent
                           ++ titleBullet
                           ++ describeName
                           ++ ancestrySeparator
                           ++ name
                           ++ "\n",
                         ),
                       );
                     String.concat(
                       "",
                       [
                         title,
                         indent(message, ~indent=messageIndent),
                         getStackInfo(loc, stack)
                       ],
                     );
                   };
                 resultString;
               })
            |> List.filter(res => res != "")
            |> String.concat("\n\n")
            |> print_endline;
          printSnapshotStatus();
          /* Exit with non-zero exit code since there were test failures */
          exit(1);
        };
      };
      if (failed^ === 0 && isRootDescribe) {
        /* mark pending and failed tests as checked so we don't delete their associated snapshots */
        let testSpecs = MIntMap.values(testMap);
        state.snapshotState :=
          testSpecs
          |> List.fold_left(
               (snapshotState, testSpec) =>
                 switch (testSpec) {
                 | {testResult: Failed(_) | Pending | Exception(_), name} =>
                   TestSnapshot.markSnapshotsAsCheckedForTest(
                     name,
                     snapshotState,
                   )
                 | {testResult: Passed} => snapshotState
                 },
               state.snapshotState^,
             );

        TestSnapshot.removeUnusedSnapshots(state.snapshotState^);
      };
      printSnapshotStatus();
    };
};
