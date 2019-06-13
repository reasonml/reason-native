/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Describe;

type mode =
  | Skip
  | Normal
  | Only;

let maxMode = (mode1: mode, mode2: mode) => mode1 > mode2 ? mode1 : mode2;

type test('ext, 'env) = {
  name: string,
  location: option(Printexc.location),
  usersTest: Test.testUtils('ext, 'env) => unit,
  mode,
};

type describeRecord('ext, 'env) = {
  name: string,
  tests: list(test('ext, 'env)),
  describes: list(describeRecord('ext, 'env)),
  mode,
};

type describeInput('ext, 'env, 'lifecycle) = {
  name: string,
  usersDescribeFn: Describe.describeUtils('ext, 'env) => unit,
  mode,
  extensionFn: MatcherTypes.matchersExtensionFn('ext),
  testLifecycle: 'lifecycle,
};

module type TestFrameworkContext = {
  module Mock: Mock.Sig;
  module StackTrace: StackTrace.StackTrace;
  module Snapshot: Snapshot.Sig;
};

type contextId = int;
type context = (contextId, module TestFrameworkContext);

type t =
  | TestSuite(
      describeRecord('ext, 'each),
      MatcherTypes.matchersExtensionFn('ext),
      TestLifecycle.t(
        'beforeAll,
        'afterAll,
        'beforeEach,
        'afterEach,
        'all,
        'each,
      ),
      context,
    )
    : t;

let contextCounter = ref(0);
let getNewContextId: unit => contextId =
  () => {
    incr(contextCounter);
    contextCounter^;
  };

module Factory = (Context: TestFrameworkContext) => {
  let contextId = getNewContextId();
  let rec makeDescribeRecord:
    (
      ~name: string,
      ~usersDescribeFn: Describe.describeUtils('ext, 'env) => unit,
      ~mode: mode
    ) =>
    describeRecord('ext, 'env) =
    (~name, ~usersDescribeFn, ~mode) => {
      let tests = ref([]);
      let describes = ref([]);
      let describe = (name, describeFn) =>
        describes :=
          describes^ @ [makeDescribeRecord(name, describeFn, mode)];
      let describeSkip = (name, describeFn) =>
        describes :=
          describes^ @ [makeDescribeRecord(name, describeFn, Skip)];
      let describeOnly = (name, describeFn) =>
        describes :=
          describes^ @ [makeDescribeRecord(name, describeFn, Only)];
      let test = (name, usersTest) => {
        let location = Context.StackTrace.(getStackTrace() |> getTopLocation);
        tests := tests^ @ [{name, location, usersTest, mode}];
      };
      let testSkip = (name, usersTest) => {
        let location = Context.StackTrace.(getStackTrace() |> getTopLocation);
        tests := tests^ @ [{name, location, usersTest, mode: Skip}];
      };
      let testOnly = (name, usersTest) => {
        let location = Context.StackTrace.(getStackTrace() |> getTopLocation);
        tests := tests^ @ [{name, location, usersTest, mode: Only}];
      };
      usersDescribeFn({
        describe,
        test,
        describeSkip,
        testSkip,
        describeOnly,
        testOnly,
      });

      let maxDescribeMode =
        describes^
        |> List.map((d: describeRecord('ext, 'env)) => d.mode)
        |> List.fold_left(maxMode, Skip);

      let maxTestMode =
        tests^
        |> List.map((t: test('ext, 'env)) => t.mode)
        |> List.fold_left(maxMode, Skip);

      let maxChildMode =
        List.fold_left(maxMode, Skip, [maxDescribeMode, maxTestMode]);
      let treeMode =
        switch (mode, maxChildMode) {
        | (Skip, _) => Skip
        | (_, _) => maxMode(mode, maxChildMode)
        };
      {name, tests: tests^, describes: describes^, mode: treeMode};
    };

  let makeTestSuite =
      ({name, usersDescribeFn, mode, testLifecycle, extensionFn}) =>
    TestSuite(
      makeDescribeRecord(~name, ~usersDescribeFn, ~mode),
      extensionFn,
      testLifecycle,
      (contextId, (module Context)),
    );
};
