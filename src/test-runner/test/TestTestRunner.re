/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 */
module Runner = TestRunner.Make(SnapshotIO.FileSystemSnapshotIO);

let make_test_config = (updateSnapshots, snapshotsDirName) =>
  TestRunner.Describe.{
    updateSnapshots,
    updateSnapshotsFlag: "-u",
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
        |> (dir => Filename.concat(dir, "test"))
        |> (dir => Filename.concat(dir, snapshotsDirName))
      ),
  };

let updateSnapshots = false;
let makeConfig = make_test_config(updateSnapshots);

Runner.rootDescribe(
    ~isRootDescribe=true,
    ~config=makeConfig("__snapshots__"),
    "TestRunner",
    ({describe}) => {
    TestRunner_test.run(describe);
    },
);
