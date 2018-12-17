/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

let dir = Sys.getenv_opt("REASON_NATIVE_ROOT");

let dir =
  switch (dir) {
  | Some(dir) => dir
  | None =>
    failwith(
      "Expected `REASON_NATIVE_ROOT` environment variable to be set "
      ++ "before running tests.",
    )
  };

include Rely.Make({
  let config =
    Rely.TestFrameworkConfig.initialize({
      snapshotDir:
        Filename.(
          dir
          |> (dir => Filename.concat(dir, "tests"))
          |> (dir => Filename.concat(dir, "__snapshots__"))
        ),
      projectDir: dir,
    });
});
