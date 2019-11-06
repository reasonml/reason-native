/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open RelyInternal.TestResult.AggregatedResult;
open TestFramework;
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
  test(
    "Running without update snapshot flag shouldn't create snapshots",
    ({expect}) => {
    let readMock = Mock.mock1(s => s);
    let removeMock = Mock.mock1(_ => ());
    let writeMock = Mock.mock2((_, _) => ());
    let existsMock = Mock.mock1(_ => false);
    let mkdirpMock = Mock.mock1(_ => ());

    module MockSnapshotIO = {
      let readSnapshotNames = _ => [];
      let readFile: string => string = Mock.fn(readMock);
      let removeFile: string => unit = Mock.fn(removeMock);
      let writeFile: (string, string) => unit = Mock.fn(writeMock);
      let existsFile: string => bool = Mock.fn(existsMock);
      let mkdirp = Mock.fn(mkdirpMock);
    };

    module TestFramework =
      RelyInternal.TestFramework.MakeInternal(
        MockSnapshotIO,
        {
          let config =
            RelyInternal.TestFrameworkConfig.TestFrameworkConfig.initialize({
              snapshotDir: "",
              projectDir: "",
            });
        },
      );

    let aggregateResult = ref(None);

    module Reporter =
      TestReporter.Make({});

    Reporter.onRunComplete(res => aggregateResult := Some(res));

    TestFramework.describe("single failing snapshot", ({test}) =>
      test("a test", ({expect}) =>
        expect.string("bacon").toMatchSnapshot()
      )
    );

    TestFramework.run(
      RelyInternal.RunConfig.(
        initialize()
        |> withReporters([Custom(Reporter.reporter)])
        |> updateSnapshots(false)
        |> onTestFrameworkFailure(() => ())
      ),
    );

    let res =
      switch (aggregateResult^) {
      | None => raise(Invalid_argument("aggregateResult"))
      | Some(v) => v
      };
    let snapshotSummary =
      switch (res.snapshotSummary) {
      | None => raise(Invalid_argument("aggregateResult"))
      | Some(v) => v
      };
    expect.mock(writeMock).not.toBeCalled();
    expect.int(res.numFailedTests).toBe(1);
    expect.int(snapshotSummary.numCreatedSnapshots).toBe(0);
    ();
  });
  test(
    "Running with update snapshot flag should create snapshots", ({expect}) => {
    let readMock = Mock.mock1(s => s);
    let removeMock = Mock.mock1(_ => ());
    let writeMock = Mock.mock2((_, _) => ());
    let existsMock = Mock.mock1(_ => false);
    let mkdirpMock = Mock.mock1(_ => ());

    module MockSnapshotIO = {
      let readSnapshotNames = _ => [];
      let readFile: string => string = Mock.fn(readMock);
      let removeFile: string => unit = Mock.fn(removeMock);
      let writeFile: (string, string) => unit = Mock.fn(writeMock);
      let existsFile: string => bool = Mock.fn(existsMock);
      let mkdirp = Mock.fn(mkdirpMock);
    };

    module TestFramework =
      RelyInternal.TestFramework.MakeInternal(
        MockSnapshotIO,
        {
          let config =
            RelyInternal.TestFrameworkConfig.TestFrameworkConfig.initialize({
              snapshotDir: "",
              projectDir: "",
            });
        },
      );

    let aggregateResult = ref(None);

    module Reporter =
      TestReporter.Make({});

    Reporter.onRunComplete(res => aggregateResult := Some(res));

    TestFramework.describe("single failing snapshot", ({test}) =>
      test("a test", ({expect}) =>
        expect.string("bacon").toMatchSnapshot()
      )
    );

    TestFramework.run(
      RelyInternal.RunConfig.(
        initialize()
        |> withReporters([Custom(Reporter.reporter)])
        |> updateSnapshots(true)
        |> onTestFrameworkFailure(() => ())
      ),
    );

    let res =
      switch (aggregateResult^) {
      | None => raise(Invalid_argument("aggregateResult"))
      | Some(v) => v
      };
    let snapshotSummary =
      switch (res.snapshotSummary) {
      | None => raise(Invalid_argument("aggregateResult"))
      | Some(v) => v
      };

    expect.mock(writeMock).toBeCalled();
    expect.int(res.numFailedTests).toBe(0);
    expect.int(snapshotSummary.numCreatedSnapshots).toBe(1);
    ();
  });
  test("unused snapshots should be deleted", ({expect}) => {
    let readMock = Mock.mock1(s => s);
    let removeMock = Mock.mock1(_ => ());
    let writeMock = Mock.mock2((_, _) => ());
    let existsMock = Mock.mock1(_ => true);
    let mkdirpMock = Mock.mock1(_ => ());

    module MockSnapshotIO = {
      let readSnapshotNames = _ => [
        "Foo.234.0.snapshot",
        "Bar.234.0.snapshot",
        "Baz.234.0.snapshot",
      ];
      let readFile: string => string = Mock.fn(readMock);
      let removeFile: string => unit = Mock.fn(removeMock);
      let writeFile: (string, string) => unit = Mock.fn(writeMock);
      let existsFile: string => bool = Mock.fn(existsMock);
      let mkdirp = Mock.fn(mkdirpMock);
    };

    module TestFramework =
      RelyInternal.TestFramework.MakeInternal(
        MockSnapshotIO,
        {
          let config =
            RelyInternal.TestFrameworkConfig.TestFrameworkConfig.initialize({
              snapshotDir: "",
              projectDir: "",
            });
        },
      );

    let aggregateResult = ref(None);

    module Reporter =
      TestReporter.Make({});

    Reporter.onRunComplete(res => aggregateResult := Some(res));

    /* need at least one tests for this to work... seems like probably a reasonable requirement? */
    TestFramework.describe("single failing test", ({test}) =>
      test("a test", ({expect}) =>
        expect.bool(false).toBeTrue()
      )
    );

    TestFramework.run(
      RelyInternal.RunConfig.(
        initialize()
        |> withReporters([Custom(Reporter.reporter)])
        |> onTestFrameworkFailure(() => ())
      ),
    );

    let res =
      switch (aggregateResult^) {
      | None => raise(Invalid_argument("aggregateResult"))
      | Some(v) => v
      };
    let snapshotSummary =
      switch (res.snapshotSummary) {
      | None => raise(Invalid_argument("aggregateResult"))
      | Some(v) => v
      };

    expect.mock(removeMock).toBeCalledTimes(3);
    expect.int(snapshotSummary.numRemovedSnapshots).toBe(3);
    ();
  });
  test("snapshots for a failing test should not be deleted", ({expect}) => {
    let readMock = Mock.mock1(s => s);
    let removeMock = Mock.mock1(_ => ());
    let writeMock = Mock.mock2((_, _) => ());
    let existsMock = Mock.mock1(_ => true);
    let mkdirpMock = Mock.mock1(_ => ());

    let testPath = (
      "my test",
      RelyInternal.TestPath.Terminal("my describe"),
    );
    let testHash = RelyInternal.TestPath.(hash(Test(testPath), 0));

    module MockSnapshotIO = {
      let readSnapshotNames = _ => [
        "my_describe." ++ testHash ++ ".0.snapshot",
        "my_describe." ++ testHash ++ ".1.snapshot",
        "my_describe." ++ testHash ++ ".2.snapshot",
        "unrelatedDescribe.fakeTestHash.0.snapshot",
      ];
      let readFile: string => string = Mock.fn(readMock);
      let removeFile: string => unit = Mock.fn(removeMock);
      let writeFile: (string, string) => unit = Mock.fn(writeMock);
      let existsFile: string => bool = Mock.fn(existsMock);
      let mkdirp = Mock.fn(mkdirpMock);
    };

    module TestFramework =
      RelyInternal.TestFramework.MakeInternal(
        MockSnapshotIO,
        {
          let config =
            RelyInternal.TestFrameworkConfig.TestFrameworkConfig.initialize({
              snapshotDir: "",
              projectDir: "",
            });
        },
      );

    let aggregateResult = ref(None);

    module Reporter =
      TestReporter.Make({});

    Reporter.onRunComplete(res => aggregateResult := Some(res));

    TestFramework.describe("my describe", ({test}) =>
      test("my test", ({expect}) =>
        expect.bool(false).toBeTrue()
      )
    );

    TestFramework.run(
      RelyInternal.RunConfig.(
        initialize()
        |> withReporters([Custom(Reporter.reporter)])
        |> onTestFrameworkFailure(() => ())
      ),
    );

    let res =
      switch (aggregateResult^) {
      | None => raise(Invalid_argument("aggregateResult"))
      | Some(v) => v
      };
    let snapshotSummary =
      switch (res.snapshotSummary) {
      | None => raise(Invalid_argument("aggregateResult"))
      | Some(v) => v
      };

    expect.mock(removeMock).toBeCalledTimes(1);
    expect.int(snapshotSummary.numRemovedSnapshots).toBe(1);
    ();
  });
  test("snapshots for a skipped test should not be deleted", ({expect}) => {
    let readMock = Mock.mock1(s => s);
    let removeMock = Mock.mock1(_ => ());
    let writeMock = Mock.mock2((_, _) => ());
    let existsMock = Mock.mock1(_ => true);
    let mkdirpMock = Mock.mock1(_ => ());

    let testPath = (
      "my test",
      RelyInternal.TestPath.Terminal("my describe"),
    );
    let testHash = RelyInternal.TestPath.(hash(Test(testPath), 0));

    module MockSnapshotIO = {
      let readSnapshotNames = _ => [
        "my_describe." ++ testHash ++ ".0.snapshot",
        "my_describe." ++ testHash ++ ".1.snapshot",
        "my_describe." ++ testHash ++ ".2.snapshot",
        "unrelatedDescribe.fakeTestHash.0.snapshot",
      ];
      let readFile: string => string = Mock.fn(readMock);
      let removeFile: string => unit = Mock.fn(removeMock);
      let writeFile: (string, string) => unit = Mock.fn(writeMock);
      let existsFile: string => bool = Mock.fn(existsMock);
      let mkdirp = Mock.fn(mkdirpMock);
    };

    module TestFramework =
      RelyInternal.TestFramework.MakeInternal(
        MockSnapshotIO,
        {
          let config =
            RelyInternal.TestFrameworkConfig.TestFrameworkConfig.initialize({
              snapshotDir: "",
              projectDir: "",
            });
        },
      );

    let aggregateResult = ref(None);

    module Reporter =
      TestReporter.Make({});

    Reporter.onRunComplete(res => aggregateResult := Some(res));

    TestFramework.describe("my describe", ({testSkip}) =>
      testSkip("my test", ({expect}) => expect.bool(false).toBeTrue())
    );

    TestFramework.run(
      RelyInternal.RunConfig.(
        initialize()
        |> withReporters([Custom(Reporter.reporter)])
        |> onTestFrameworkFailure(() => ())
      ),
    );

    let res =
      switch (aggregateResult^) {
      | None => raise(Invalid_argument("aggregateResult"))
      | Some(v) => v
      };
    let snapshotSummary =
      switch (res.snapshotSummary) {
      | None => raise(Invalid_argument("aggregateResult"))
      | Some(v) => v
      };

    expect.mock(removeMock).toBeCalledTimes(1);
    expect.int(snapshotSummary.numRemovedSnapshots).toBe(1);
    ();
  });
});
