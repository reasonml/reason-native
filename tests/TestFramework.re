/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

let projectDir = GetProjectRoot.get();

include Rely.Make({
  let config =
    Rely.TestFrameworkConfig.initialize({
      snapshotDir:
        Filename.(
          projectDir
          |> (dir => Filename.concat(dir, "tests"))
          |> (dir => Filename.concat(dir, "__snapshots__"))
        ),
      projectDir: projectDir,
    });
});
