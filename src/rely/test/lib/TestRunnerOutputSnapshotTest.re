/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Rely.Describe;
open Rely.Test;

module type SnapshotDir = {let snapshotDir: string;};

module MakeTestFramework =
       (SnapshotDir: SnapshotDir)
       : Rely.TestFramework =>
  Rely.Make({
    let config =
      Rely.TestFrameworkConfig.initialize({
        snapshotDir:
          Filename.(
            Sys.executable_name
            |> dirname
            |> dirname
            |> dirname
            |> dirname
            |> dirname
            |> dirname
            |> dirname
            |> (dir => Filename.concat(dir, "src"))
            |> (dir => Filename.concat(dir, "rely"))
            |> (dir => Filename.concat(dir, "test"))
            |> (dir => Filename.concat(dir, "lib"))
            |> (
              dir =>
                Filename.concat(dir, "__test_runner_output_snapshot_tests__")
            )
            |> (dir => Filename.concat(dir, SnapshotDir.snapshotDir))
          ),
        projectDir: "",
      });
  });

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
        IO.captureOutput(() => {
          TestFramework.describe(testName, utils =>
            testFn(utils)
          );
          TestFramework.run(
            Rely.RunConfig.(initialize() |> updateSnapshots(doUpdate)),
          );
          ();
        });

      if (debugPrintOutputToConsole) {
        print_endline("\n" ++ stdout ++ "\n");
      };
      testUtils.expect.string(stdout).toMatchSnapshot();
    },
  );
};
