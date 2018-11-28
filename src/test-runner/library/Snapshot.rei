/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open SnapshotIO;
type state;

module Make:
  (IO: SnapshotIO) =>
   {
    let initializeState:
      (~snapshotDir: string, ~updateSnapshots: bool) => state;
    let markSnapshotsAsCheckedForTest: (string, state) => state;
    let getSnapshotStatus: state => option(string);
    let removeUnusedSnapshots: state => unit;
    let markSnapshotUsed: (string, state) => state;
    let markSnapshotUpdated: (string, state) => state;

    exception InvalidSnapshotFileName(string);
  };
