/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Describe;

type test('ext) =
  | Test{
      name: string,
      location: option(Printexc.location),
      usersTest: Test.testUtils('ext) => unit,
    }
  | Skipped{
      name: string,
      location: option(Printexc.location),
    };

type describeRecord('ext) = {
  name: string,
  tests: list(test('ext)),
  describes: list(describeRecord('ext)),
  skip: bool,
};

type describeInput('ext) = {
  name: string,
  usersDescribeFn: Describe.describeUtils('ext) => unit,
  skip: bool,
  extensionFn: MatcherTypes.matchersExtensionFn('ext),
};

module type TestFrameworkContext = {
  module Mock: Mock.Sig;
  module StackTrace: StackTrace.StackTrace;
  module Snapshot: Snapshot.Sig;
};

type t =
  | TestSuite(
      describeRecord('ext),
      MatcherTypes.matchersExtensionFn('ext),
      (module TestFrameworkContext),
    )
    : t;

module Factory = (Context: TestFrameworkContext) => {
  let rec makeDescribeRecord:
    (
      ~name: string,
      ~usersDescribeFn: Describe.describeUtils('ext) => unit,
      ~skip: bool
    ) =>
    describeRecord('ext) =
    (~name, ~usersDescribeFn, ~skip) => {
      let tests = ref([]);
      let describes = ref([]);
      let describe = (name, describeFn) =>
        describes :=
          describes^ @ [makeDescribeRecord(name, describeFn, skip)];
      let describeSkip = (name, describeFn) =>
        describes :=
          describes^ @ [makeDescribeRecord(name, describeFn, true)];
      let test = (name, usersTest) => {
        let location = Context.StackTrace.(getStackTrace() |> getTopLocation);
        tests := tests^ @ [Test({name, location, usersTest})];
      };
      let testSkip = (name, usersTest) => {
        let location = Context.StackTrace.(getStackTrace() |> getTopLocation);
        tests := tests^ @ [Skipped({name, location})];
      };
      usersDescribeFn({describe, test, describeSkip, testSkip});
      {name, tests: tests^, describes: describes^, skip};
    };

  let makeTestSuite = ({name, usersDescribeFn, skip, extensionFn}) =>
    TestSuite(
      makeDescribeRecord(~name, ~usersDescribeFn, ~skip),
      extensionFn,
      (module Context),
    );
};
