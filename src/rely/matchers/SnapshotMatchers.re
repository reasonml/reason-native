/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open MatcherTypes;
module type SnapshotMatchersConfig = {
  module Snapshot: Snapshot.Sig;
  let genExpectID: unit => int;
  let testPath: TestPath.test;
  let testId: Snapshot.testId;
  let updateSnapshots: bool;
};

type snapshotMatchersRecord = {toMatchSnapshot: unit => unit};

module Make = (Config: SnapshotMatchersConfig) => {
  open Config;
  let makeMatchers = (accessorPath, {createMatcher}) => {
    let passingMessageThunk = () => "";
    let escape = (s: string): string => {
      let lines = Str.split(Str.regexp("\n"), s);
      let lines = List.map(line => String.escaped(line), lines);
      String.concat("\n", lines);
    };
    let makeSnapshotMatcher = sActual => {
      let toMatchSnapshot =
        createMatcher(
          (
            {matcherHint, formatReceived, formatExpected, indent, prepareDiff},
            actualThunk,
            _,
          ) => {
          let sActual = actualThunk();
          let expectId = genExpectID();

          let doUpdate = () =>
            Config.Snapshot.updateSnapshot(testId, expectId, sActual);

          switch (Config.Snapshot.readSnapshot(testId, expectId)) {
          | None =>
            if (updateSnapshots) {
              doUpdate();
              (passingMessageThunk, true);
            } else {
              (
                () =>
                  String.concat(
                    "",
                    [
                      "New snapshot was ",
                      formatReceived("not written"),
                      ". The update flag (-u) must be explicitly passed to write a new snapshot.",
                      "\n\n",
                      "Received: ",
                      formatReceived(sActual),
                    ],
                  ),
                false,
              );
            }
          | Some(sExpected) =>
            Config.Snapshot.markSnapshotUsed(testId, expectId);
            let didPass = sActual == sExpected;
            if (!didPass) {
              /*
               * If it failed, but we should be updating snapshots then write
               * out the new snapshot. Otherwise fail the test.
               */
              if (updateSnapshots) {
                doUpdate();
                (passingMessageThunk, true);
              } else {
                let actual = escape(sActual);
                let expected = escape(sExpected);
                let message =
                  String.concat(
                    "",
                    [
                      matcherHint(
                        ~expectType=accessorPath,
                        ~matcherName=".toMatchSnapshot",
                        (),
                      ),
                      Pastel.dim("\n\nInspect your code changes or run Rely with the -u flag to update snapshots."),
                      "\n\n",
                      formatExpected("- " ++ expected),
                      "\n",
                      formatReceived("+ " ++ actual),
                      "\n\nDifference:\n",
                      indent(prepareDiff(actual, expected)),
                    ],
                  );
                ((() => message), false);
              };
            } else {
              (passingMessageThunk, true);
            };
          };
        });
      let matchers = {
        toMatchSnapshot: () => toMatchSnapshot(() => sActual, () => ()),
      };
      matchers;
    };
    makeSnapshotMatcher;
  };
};
