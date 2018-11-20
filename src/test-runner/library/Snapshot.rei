/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 */
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
