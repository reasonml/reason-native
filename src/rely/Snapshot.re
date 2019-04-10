/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

module type Sig = {
  type testId;
  let getSnapshotStatus: unit => TestResult.AggregatedResult.snapshotSummary;
  let removeUnusedSnapshots: unit => unit;
  let markSnapshotsAsCheckedForTest: (testId) => unit;
  let getNewId: TestPath.test => testId;
  let readSnapshot: (testId, int) => option(string);
  let updateSnapshot: (testId, int, string) => unit;
  let markSnapshotUsed: (testId, int) => unit;
};
