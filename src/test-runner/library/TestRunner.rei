module Test: {
  type testUtils = {expect: DefaultMatchers.matchers};
  type testFn = (string, testUtils => unit) => unit;
};
module Describe: {
  type describeUtils = {
    describe: describeFn,
    test: Test.testFn,
  }
  and describeFn = (string, describeUtils => unit) => unit;
};

module RunConfig: {
  type t;
  let initialize: unit => t;
  let updateSnapshots: (bool, t) => t;
};

module type TestFramework = {
  let run: RunConfig.t => unit;
  let describe: Describe.describeFn;
};

type requiredConfiguration = TestFrameworkConfig.requiredConfiguration;

module TestFrameworkConfig: {
  type t;
  let initialize: requiredConfiguration => t;
};

module type FrameworkConfig = {let config: TestFrameworkConfig.t;};

module Make: (FrameworkConfig) => TestFramework;
