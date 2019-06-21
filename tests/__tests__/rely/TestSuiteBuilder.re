/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

type mode =
  | Only
  | Normal
  | Skip;

type customTest('ext, 'env) = {
  mode,
  name: string,
  implementation: Rely.Test.testUtils('ext, 'env) => unit,
};

type test('ext, 'env) =
  | Simple{
      mode,
      pass: bool,
    }
    : test('a, 'b)
  | Custom(customTest('ext, 'env)): test('ext, 'env);

open RelyInternal.TestLifecycle;
type describeConfigFinalized;
type describeConfigNotFinalized;

type builderConfig('ext, 'env) =
  | SimpleBuilderConfig(string): builderConfig(unit, unit)
  | BuilderConfigWithCustomMatchers(
      string,
      Rely.MatcherTypes.matchersExtensionFn('ext),
    )
    : builderConfig('ext, unit)
  | BuilderConfigWithLifecycle(
      string,
      RelyInternal.TestLifecycle.defaultLifecycle =>
      RelyInternal.TestLifecycle.t(_, _, _, _, _, 'env),
    )
    : builderConfig(unit, 'env)
  | BuilderConfigWithLifecycleAndCustomMatchers(
      string,
      Rely.MatcherTypes.matchersExtensionFn('ext),
      RelyInternal.TestLifecycle.defaultLifecycle =>
      RelyInternal.TestLifecycle.t(_, _, _, _, _, 'env),
    )
    : builderConfig('ext, 'env);

type testSuiteConfig('ext, 'env) =
  | TestSuiteConfig{
      tests: list(test('ext, 'env)),
      nestedSuites: list(t('ext, 'env, describeConfigFinalized)),
      name: string,
      mode,
    }
    : testSuiteConfig('ext, 'env)
and t('ext, 'env, 'describeConfigState) =
  | Unfinalized(builderConfig('ext, 'env))
    : t('ext, 'env, describeConfigNotFinalized)
  | Simple(testSuiteConfig(unit, unit))
    : t(unit, unit, describeConfigFinalized)
  | Child(testSuiteConfig('ext, 'env))
    : t('ext, 'env, describeConfigFinalized)
  | WithLifecycleAndCustomMatchers{
      config: testSuiteConfig('ext, 'env),
      testLifecycleFactory:
        RelyInternal.TestLifecycle.defaultLifecycle =>
        RelyInternal.TestLifecycle.t('a, 'b, 'c, 'd, 'e, 'env),
      customMatchers: Rely.MatcherTypes.matchersExtensionFn('ext),
    }
    : t('ext, 'env, describeConfigFinalized)
  | WithLifecycle{
      config: testSuiteConfig(unit, 'env),
      testLifecycleFactory:
        RelyInternal.TestLifecycle.defaultLifecycle =>
        RelyInternal.TestLifecycle.t('a, 'b, 'c, 'd, 'e, 'env),
    }
    : t(unit, 'env, describeConfigFinalized)
  | WithCustomMatchers{
      config: testSuiteConfig('ext, unit),
      customMatchers: Rely.MatcherTypes.matchersExtensionFn('ext),
    }
    : t('ext, unit, describeConfigFinalized);

type testSuite =
  | TestSuite(t('ext, 'env, 'describeConfigState)): testSuite;

let withLifecycle:
  type ext env a.
    (
      RelyInternal.TestLifecycle.defaultLifecycle =>
      RelyInternal.TestLifecycle.t(_, _, _, _, _, env),
      t(ext, a, describeConfigNotFinalized)
    ) =>
    t(ext, env, describeConfigNotFinalized) =
  (testLifecycleFactory, Unfinalized(config)) =>
    Unfinalized(
      switch (config) {
      | BuilderConfigWithLifecycle(name, _) =>
        BuilderConfigWithLifecycle(name, testLifecycleFactory)
      | SimpleBuilderConfig(name) =>
        BuilderConfigWithLifecycle(name, testLifecycleFactory)
      | BuilderConfigWithCustomMatchers(name, customMatchers) =>
        BuilderConfigWithLifecycleAndCustomMatchers(
          name,
          customMatchers,
          testLifecycleFactory,
        )
      | BuilderConfigWithLifecycleAndCustomMatchers(name, customMatchers, _) =>
        BuilderConfigWithLifecycleAndCustomMatchers(
          name,
          customMatchers,
          testLifecycleFactory,
        )
      },
    );

let withCustomMatchers:
  type ext env a.
    (
      Rely.MatcherTypes.matchersExtensionFn(ext),
      t(a, env, describeConfigNotFinalized)
    ) =>
    t(ext, env, describeConfigNotFinalized) =
  (customMatchers, Unfinalized(config)) =>
    Unfinalized(
      switch (config) {
      | BuilderConfigWithCustomMatchers(name, _) =>
        BuilderConfigWithCustomMatchers(name, customMatchers)
      | SimpleBuilderConfig(name) =>
        BuilderConfigWithCustomMatchers(name, customMatchers)
      | BuilderConfigWithLifecycle(name, testLifecycleFactory) =>
        BuilderConfigWithLifecycleAndCustomMatchers(
          name,
          customMatchers,
          testLifecycleFactory,
        )
      | BuilderConfigWithLifecycleAndCustomMatchers(
          name,
          _,
          testLifecycleFactory,
        ) =>
        BuilderConfigWithLifecycleAndCustomMatchers(
          name,
          customMatchers,
          testLifecycleFactory,
        )
      },
    );
let finalizeUnfinalized:
  type ext env.
    t(ext, env, describeConfigNotFinalized) =>
    t(ext, env, describeConfigFinalized) =
  (Unfinalized(builderConfig)) => {
    let makeConfig = name =>
      TestSuiteConfig({tests: [], nestedSuites: [], name, mode: Normal});
    switch (builderConfig) {
    | SimpleBuilderConfig(name) => Simple(makeConfig(name))
    | BuilderConfigWithCustomMatchers(name, customMatchers) =>
      WithCustomMatchers({config: makeConfig(name), customMatchers})
    | BuilderConfigWithLifecycle(name, testLifecycleFactory) =>
      WithLifecycle({config: makeConfig(name), testLifecycleFactory})
    | BuilderConfigWithLifecycleAndCustomMatchers(
        name,
        customMatchers,
        testLifecycleFactory,
      ) =>
      WithLifecycleAndCustomMatchers({
        config: makeConfig(name),
        customMatchers,
        testLifecycleFactory,
      })
    };
  };
let finalize:
  type ext env finalized.
    t(ext, env, finalized) => t(ext, env, describeConfigFinalized) =
  config => {
    switch (config) {
    | Unfinalized(_) => finalizeUnfinalized(config)
    | Simple(_) => config
    | Child(_) => config
    | WithLifecycle(_) => config
    | WithCustomMatchers(_) => config
    | WithLifecycleAndCustomMatchers(_) => config
    };
  };

let init = name => {
  Unfinalized(SimpleBuilderConfig(name));
};

let transformConfig:
  type ext env finalized.
    (
      testSuiteConfig(ext, env) => testSuiteConfig(ext, env),
      t(ext, env, finalized)
    ) =>
    t(ext, env, describeConfigFinalized) =
  (transform, testSuiteConfig) =>
    switch (finalize(testSuiteConfig)) {
    | Child(config) => Child(transform(config))
    | Simple(config) => Simple(transform(config))
    | WithLifecycleAndCustomMatchers({
        config,
        testLifecycleFactory,
        customMatchers,
      }) =>
      WithLifecycleAndCustomMatchers({
        config: transform(config),
        testLifecycleFactory,
        customMatchers,
      })
    | WithLifecycle({testLifecycleFactory, config}) =>
      WithLifecycle({config: transform(config), testLifecycleFactory})
    | WithCustomMatchers({customMatchers, config}) =>
      WithCustomMatchers({customMatchers, config: transform(config)})
    };
let applyToConfig:
  type ext env.
    (
      testSuiteConfig(ext, env) => unit,
      t(ext, env, describeConfigFinalized)
    ) =>
    unit =
  (fn, testSuiteConfig) =>
    switch (testSuiteConfig) {
    | Child(config) => fn(config)
    | Simple(config) => fn(config)
    | WithCustomMatchers({config}) => fn(config)
    | WithLifecycle({config}) => fn(config)
    | WithLifecycleAndCustomMatchers({config}) => fn(config)
    };
let withName:
  type ext env finalized.
    (string, t(ext, env, finalized)) => t(ext, env, describeConfigFinalized) =
  (name, testSuiteConfig) =>
    transformConfig(
      (TestSuiteConfig(config)) => TestSuiteConfig({...config, name}),
      testSuiteConfig,
    );

let makeChild:
  type ext env finalized.
    t(ext, env, finalized) => t(ext, env, describeConfigFinalized) =
  config => {
    let defaultChildConfig =
      TestSuiteConfig({
        name: "nested suite",
        tests: [],
        nestedSuites: [],
        mode: Normal,
      });
    transformConfig(_ => defaultChildConfig, config);
  };

let withNestedTestSuite:
  type ext env describeConfigState.
    (
      ~child: t(ext, env, describeConfigFinalized) =>
              t(ext, env, describeConfigFinalized),
      t(ext, env, describeConfigState)
    ) =>
    t(ext, env, describeConfigFinalized) =
  (~child, parent) =>
    transformConfig(
      (TestSuiteConfig(config)) =>
        TestSuiteConfig({
          ...config,
          nestedSuites: config.nestedSuites @ [child(makeChild(parent))],
        }),
      parent,
    );

let appendTests:
  type ext env finalized.
    (int, test(ext, env), t(ext, env, finalized)) =>
    t(ext, env, describeConfigFinalized) =
  (numTests, test, suite) => {
    let newTests = Array.make(numTests, test) |> Array.to_list;
    transformConfig(
      (TestSuiteConfig(config)) =>
        TestSuiteConfig({...config, tests: config.tests @ newTests}),
      suite,
    );
  };

let withFailingTests = (~only=false, numFailingTests, suite) => {
  let mode = only ? Only : Normal;
  appendTests(numFailingTests, Simple({pass: false, mode}), suite);
};
let withPassingTests = (~only=false, numPassingTests, suite) => {
  let mode = only ? Only : Normal;
  appendTests(numPassingTests, Simple({pass: true, mode}), suite);
};
let withSkippedTests = (numSkippedTests, suite) =>
  appendTests(numSkippedTests, Simple({pass: true, mode: Skip}), suite);
let withCustomTest = (test, suite) => appendTests(1, Custom(test), suite);

let skipSuite = config =>
  transformConfig(
    (TestSuiteConfig(config)) => TestSuiteConfig({...config, mode: Skip}),
    config,
  );

let only = config =>
  transformConfig(
    (TestSuiteConfig(config)) => TestSuiteConfig({...config, mode: Only}),
    config,
  );

let build = config => TestSuite(config);

open Rely.Test;
open Rely.Describe;

module Make = (TestFramework: Rely.TestFramework) => {
  let register:
    type ext env finalized.
      (
        t(ext, env, finalized),
        ~describe: Rely.Describe.describeFn(unit, unit),
        ~describeSkip: Rely.Describe.describeFn(unit, unit),
        ~describeOnly: Rely.Describe.describeFn(unit, unit)
      ) =>
      unit =
    (config, ~describe, ~describeSkip, ~describeOnly) => {
      let rec register = (config, describe, describeSkip, describeOnly) => {
        let tests = describeUtils =>
          applyToConfig(
            (TestSuiteConfig(config)) => {
              List.iter(
                test =>
                  switch ((test: test(ext, env))) {
                  | Simple({mode, pass}) =>
                    let body = ({expect}) => expect.bool(true).toBe(pass);
                    switch (mode) {
                    | Normal => describeUtils.test("some test", body)
                    | Skip => describeUtils.testSkip("some test", body)
                    | Only => describeUtils.testOnly("some test", body)
                    };
                  | Custom({mode, name, implementation}) =>
                    switch (mode) {
                    | Normal => describeUtils.test(name, implementation)
                    | Skip => describeUtils.testSkip(name, implementation)
                    | Only => describeUtils.testOnly(name, implementation)
                    }
                  },
                config.tests,
              );
              ();
            },
            config,
          );
        applyToConfig(
          (TestSuiteConfig(config)) => {
            let describeFnToUse =
              switch (config.mode) {
              | Normal => describe
              | Skip => describeSkip
              | Only => describeOnly
              };
            describeFnToUse(
              config.name,
              describeUtils => {
                tests(describeUtils);
                List.iter(
                  nestedSuite =>
                    register(
                      nestedSuite,
                      describeUtils.describe,
                      describeUtils.describeSkip,
                      describeUtils.describeOnly,
                    ),
                  config.nestedSuites,
                );
              },
            );
          },
          config,
        );
      };
      let config = finalize(config);
      switch (config) {
      | Child(_) => failwith("don't do that")
      | Simple(_) => register(config, describe, describeSkip, describeOnly)
      | WithLifecycleAndCustomMatchers({testLifecycleFactory, customMatchers}) =>
        let {describe, describeSkip, describeOnly} =
          TestFramework.(
            describeConfig
            |> withCustomMatchers(customMatchers)
            |> withLifecycle(testLifecycleFactory)
            |> build
          );
        register(config, describe, describeSkip, describeOnly);
      | WithLifecycle({testLifecycleFactory}) =>
        let {describe, describeSkip, describeOnly} =
          TestFramework.(
            describeConfig
            |> withLifecycle(testLifecycleFactory)
            |> build
          );
        register(config, describe, describeSkip, describeOnly);
      | WithCustomMatchers({customMatchers}) =>
        let {describe, describeSkip, describeOnly} =
          TestFramework.(
            describeConfig
            |> withCustomMatchers(customMatchers)
            |> build
          );
        register(config, describe, describeSkip, describeOnly);
      };
    };
};
