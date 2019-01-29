/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Rely.Describe;
open Rely.Test;

module type SnapshotDir = {let snapshotDir: string;};

module MakeTestFramework = (SnapshotDir: SnapshotDir) : Rely.TestFramework =>
  Rely.Make({
    let config =
      Rely.TestFrameworkConfig.initialize({
        snapshotDir:
          Filename.(
            GetProjectRoot.get()
            |> (dir => Filename.concat(dir, "tests"))
            |> (
              dir => Filename.concat(dir, "__snapshots_test_runner_output__")
            )
            |> (dir => Filename.concat(dir, SnapshotDir.snapshotDir))
          ),
        projectDir: "",
      });
  });

/**
 * One purpose of this file is to test the output of the test runner when there
 * is a mismatch in snapshot tests. To update the tests for this case:
 *
 *   - set `updateSnapshots` below to true
 *   - run the tests
 *   - set `updateSnapshots` to false
 *   - change the contents of the "bacon3" snapshot (see TestRunner_test.re)
 *   - delete the snapshot containing "more bacon!" (see TestRunner_test.re)
 *   - run the tests with the update snapshots flag passed (-u) so that the
 *     snapshots capturing snapshot errors are updated
 */
let testRunnerOutputSnapshotTest =
    (
      ~updateSnapshots=false,
      ~debugPrintOutputToConsole=false,
      testName,
      describeUtils,
      testFn,
    ) => {
  let doUpdate = updateSnapshots;
  describeUtils.test(
    testName,
    testUtils => {
      let blankSpaceRegex = Str.regexp_string(" ");
      module TestFramework =
        MakeTestFramework({
          let snapshotDir =
            Str.global_replace(blankSpaceRegex, "_", testName);
        });
      let (stdout, _, _) =
        Pastel.useMode(HumanReadable, () =>
          IO.captureOutput(() => {
            TestFramework.describe(testName, utils =>
              testFn(utils)
            );
            TestFramework.run(
              Rely.RunConfig.(initialize() |> updateSnapshots(doUpdate)),
            );
            ();
          })
        );

      if (debugPrintOutputToConsole) {
        print_endline("\n" ++ stdout ++ "\n");
      };
      testUtils.expect.string(stdout).toMatchSnapshot();
    },
  );
};
