/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

let projectDir = GetProjectRoot.get();

let _ =
  Sexplib0.Sexp_conv.sexp_of_string(
    "Forcing Sexplib0 to get linked while we await resolution of https://github.com/janestreet/sexplib0/issues/1",
  );

include Rely.Make({
  let config =
    Rely.TestFrameworkConfig.initialize({
      snapshotDir:
        Filename.(
          projectDir
          |> (dir => Filename.concat(dir, "tests"))
          |> (dir => Filename.concat(dir, "__snapshots__"))
        ),
      projectDir,
    });
});
