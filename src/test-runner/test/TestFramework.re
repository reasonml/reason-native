let projectDir =
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
    |> (dir => Filename.concat(dir, "test-runner"))
  );

include TestRunner.Make({
  let config =
    TestRunner.TestFrameworkConfig.initialize({
      updateSnapshotsFlag: "-u",
      snapshotDir:
        Filename.(
          projectDir
          |> (dir => Filename.concat(dir, "test"))
          |> (dir => Filename.concat(dir, "__snapshots__"))
        ),
      projectDir,
    });
});
