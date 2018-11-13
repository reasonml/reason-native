open Collections;
open SnapshotIO;

module Test: {
    type testResult =
      | Pending
      | Passed
      | Failed(string, option(Printexc.location), string)
      | Exception(exn, option(Printexc.location), string);

    type testUtils = {expect: DefaultMatchers.matchers};
    type testFn = (string, testUtils => unit) => unit;
    type testSpec = {
      testID: int,
      name: string,
      runTest: unit => unit,
      mutable testResult,
    };
};

module Describe: {
    type describeConfig = {
        updateSnapshots: bool,
        updateSnapshotsFlag: string,
        snapshotDir: string,
    };
    type describeState = {
        testHashes: MStringSet.t,
        snapshotState: ref(Snapshot.state),
    };
    type describeUtils = {
        describe: describeFn,
        test: Test.testFn,
    }
    and rootDescribeFn =
        (
        ~config: describeConfig,
        ~isRootDescribe: bool=?,
        ~state: option(describeState)=?,
        string,
        describeUtils => unit
        ) =>
        unit
    and describeFn = (string, describeUtils => unit) => unit;
};

module type TestRunner = {
    let rootDescribe: Describe.rootDescribeFn;
}

module Make: (SnapshotIO) => TestRunner;
