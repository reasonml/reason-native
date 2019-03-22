/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
module ArrayMatchers = ArrayMatchers;
module CollectionMatchers = CollectionMatchers;
module ListMatchers = ListMatchers;
module Reporter = Reporter;
module Time = Time;
module Mock = Mock;
exception InvalidWhileRunning(string);

module Test: {
  type testUtils('ext) = {expect: DefaultMatchers.matchers('ext)};
  type testFn('ext) = (string, testUtils('ext) => unit) => unit;
};

module Describe: {
  type describeUtils('ext) = {
    describe: describeFn('ext),
    describeSkip: describeFn('ext),
    test: Test.testFn('ext),
    testSkip: Test.testFn('ext),
  }
  and describeFn('ext) = (string, describeUtils('ext) => unit) => unit;

  type extensionResult('ext) = {
    describe: describeFn('ext),
    describeSkip: describeFn('ext),
  };
};

module RunConfig: {
  type reporter =
    | Default
    | JUnit(string)
    | Custom(Reporter.t);

  type t;

  let initialize: unit => t;
  let onTestFrameworkFailure: (unit => unit, t) => t;
  let updateSnapshots: (bool, t) => t;
  let withReporters: (list(reporter), t) => t;
  let internal_do_not_use_get_time: (unit => Time.t, t) => t;
};

module MatcherUtils: {
  type matcherHintOptions = {comment: option(string)};

  type t = {
    matcherHint:
      (
        ~matcherName: string,
        ~expectType: string,
        ~isNot: bool=?,
        ~received: string=?,
        ~expected: string=?,
        ~options: matcherHintOptions=?,
        unit
      ) =>
      string,
    formatReceived: string => string,
    formatExpected: string => string,
    prepareDiff: (string, string) => string,
    indent: string => string,
  };
};

module MatcherTypes: {
  type thunk('a) = unit => 'a;
  type matcher('a, 'b) =
    (MatcherUtils.t, thunk('a), thunk('b)) => (thunk(string), bool);
  type matcherConfig('a, 'b) =
    (MatcherUtils.t, unit => 'a, unit => 'b) => (unit => string, bool);
  type matcherResult('a, 'b) = (unit => 'a, unit => 'b) => unit;
  type createMatcher('a, 'b) =
    matcherConfig('a, 'b) => matcherResult('a, 'b);
  type extendUtils = {createMatcher: 'a 'b. createMatcher('a, 'b)};
  type matchersExtensionFn('ext) = extendUtils => 'ext;
};

module type TestFramework = {
  module Mock: Mock.Sig;
  include (module type of Describe);
  include (module type of Test);

  let describe: Describe.describeFn(unit);
  let describeSkip: Describe.describeFn(unit);
  let extendDescribe:
    MatcherTypes.matchersExtensionFn('ext) => extensionResult('ext);
  let run: RunConfig.t => unit;
  let cli: unit => unit;

};

type requiredConfiguration = TestFrameworkConfig.requiredConfiguration;

module TestFrameworkConfig: {
  type t;
  let initialize: requiredConfiguration => t;
  let withMaxNumberOfMockCalls: (int, t) => t;
};

module type FrameworkConfig = {let config: TestFrameworkConfig.t;};

module Make: (FrameworkConfig) => TestFramework;
