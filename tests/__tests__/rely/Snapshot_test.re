/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open TestFramework;
open RelyInternal.TestResult.AggregatedResult;
module StringMap = Map.Make(String);

module MakeInMemorySnapshotIO =
       (Config: {let initialState: list((string, string));}) => {
  let store =
    ref(
      List.fold_left(
        (acc, (k, v)) => StringMap.add(k, v, acc),
        StringMap.empty,
        Config.initialState,
      ),
    );
  let readSnapshotNames = _ => StringMap.bindings(store^) |> List.map(fst);
  let readFile: string => string = name => StringMap.find(name, store^);
  let removeFile: string => unit =
    name => store := StringMap.remove(name, store^);
  let writeFile: (string, string) => unit =
    (filename, contents) =>
      store := StringMap.add(filename, contents, store^);
  let existsFile: string => bool =
    filename => StringMap.mem(filename, store^);
  let mkdirp = _ => ();
};

module MakeTestSnapshot =
       (Config: {let initialState: list((string, string));}) =>
  RelyInternal.FileSystemSnapshot.Make({
    let snapshotDir = "";
    module IO = MakeInMemorySnapshotIO(Config);
  });

describe("Rely Snapshot", ({test}) => {
  test(
    "initialized state should have no created, removed, or updated snapshots",
    ({expect}) => {
    module Snapshot =
      MakeTestSnapshot({
        let initialState = [("bacon", "delicious")];
      });

    expect.equal(
      {
        numCreatedSnapshots: 0,
        numRemovedSnapshots: 0,
        numUpdatedSnapshots: 0,
      },
      Snapshot.getSnapshotStatus(),
    );
  });
  test("removing a snapshot should be reflected in the status", ({expect}) => {
    module Snapshot =
      MakeTestSnapshot({
        let initialState = [("bacon", "delicious")];
      });

    Snapshot.removeUnusedSnapshots();
    expect.equal(
      {
        numCreatedSnapshots: 0,
        numRemovedSnapshots: 1,
        numUpdatedSnapshots: 0,
      },
      Snapshot.getSnapshotStatus(),
    );
  });
  test(
    "updating a non-existant snapshot should be reflected in status",
    ({expect}) => {
    module Snapshot =
      MakeTestSnapshot({
        let initialState = [];
      });
    let testPath = (
      "my test",
      RelyInternal.TestPath.Terminal("my describe"),
    );

    let testId: Snapshot.testId = Snapshot.getNewId(testPath);

    Snapshot.updateSnapshot(testId, 0, "contents");
    expect.equal(
      {
        numCreatedSnapshots: 1,
        numRemovedSnapshots: 0,
        numUpdatedSnapshots: 0,
      },
      Snapshot.getSnapshotStatus(),
    );
  });

  test(
    "updating an existing snapshot should be reflected in status", ({expect}) => {
    let testPath = (
      "my test",
      RelyInternal.TestPath.Terminal("my describe"),
    );
    let testHash = RelyInternal.TestPath.(hash(Test(testPath), 0));
    module Snapshot =
      MakeTestSnapshot({
        let initialState = [
          ("my_describe." ++ testHash ++ ".0.snapshot", "stored snapshot"),
        ];
      });

    let testId: Snapshot.testId = Snapshot.getNewId(testPath);

    Snapshot.updateSnapshot(testId, 0, "new contents");
    expect.equal(
      {
        numCreatedSnapshots: 0,
        numRemovedSnapshots: 0,
        numUpdatedSnapshots: 1,
      },
      Snapshot.getSnapshotStatus(),
    );
  });
});
