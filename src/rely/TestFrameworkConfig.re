/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type requiredConfiguration = {
  snapshotDir: string,
  projectDir: string,
};

module TestFrameworkConfig = {
  type t = {
    snapshotDir: string,
    projectDir: string,
  };

  let initialize: requiredConfiguration => t =
    config => {
      snapshotDir: config.snapshotDir,
      projectDir: config.projectDir
    }
};
