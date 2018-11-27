/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 *
 * LICENSE file in the root directory of this source tree.
 */;
open MatcherTypes;
open SnapshotIO;
module type SnapshotMatchersConfig = {
  let markSnapshotUsed: string => unit;
  let markSnapshotUpdated: string => unit;
  let snapshotPrefix: string;
  let testHash: string;
  let genExpectID: unit => int;
  let testTitle: string;
  let updateSnapshots: bool;
};

type snapshotMatchersRecord = {toMatchSnapshot: unit => unit};

module Make = (Config: SnapshotMatchersConfig, IO: SnapshotIO) => {
  open Config;

  let escapeSnapshot = (title: string, s: string): string => {
    ();
    let withoutEndline =
      s
      |> Str.split_delim(Str.regexp("\n"))
      |> List.map(String.escaped)
      |> String.concat("\n");
    title ++ "\n" ++ withoutEndline ++ "\n";
  };
  let unescapeSnapshot = (s: string): string => {
    ();
    let result =
      s
      |> Str.split_delim(Str.regexp("\n"))
      /* This removes the title we manually add */
      |> List.tl
      |> List.map(Scanf.unescaped)
      /*
       * Since concat does not add the delimeter after the last element we
       * have correctly unescaped the trailing newline we always add in escape.
       */
      |> String.concat("\n");
    result;
  };

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
          let snapshotFile =
            String.concat(
              ".",
              [
                snapshotPrefix,
                testHash,
                string_of_int(genExpectID()),
                "snapshot",
              ],
            );
          let doUpdate = () => {
            IO.writeFile(snapshotFile, escapeSnapshot(testTitle, sActual));
            markSnapshotUpdated(snapshotFile);
          };
          if (!IO.existsFile(snapshotFile)) {
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
                      ". The update flag must be explicitly passed to write a new snapshot.",
                      "\n\n",
                      "Received: ",
                      formatReceived(sActual),
                    ],
                  ),
                false,
              );
            };
          } else {
            let sExpected = unescapeSnapshot(IO.readFile(snapshotFile));
            markSnapshotUsed(snapshotFile);
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
                      "\n\n",
                      formatExpected("- " ++ expected),
                      "\n",
                      formatReceived("+ " ++ actual),
                      "\n\nDifference:\n",
                      indent(prepareDiff(actual, expected)),
                    ],
                  );
                (() => message, false);
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
