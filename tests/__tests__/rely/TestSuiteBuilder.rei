/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open RelyInternal.TestLifecycle;

type describeConfigFinalized;
type describeConfigNotFinalized;
type t('ext, 'env, 'describeConfigState);
type testSuite =
  | TestSuite(t('ext, 'env, 'describeConfigState)): testSuite;

let withLifecycle:
  (
    RelyInternal.TestLifecycle.defaultLifecycle =>
    RelyInternal.TestLifecycle.t('a, 'b, 'c, 'd, 'e, 'env),
    t('ext, 'f, describeConfigNotFinalized)
  ) =>
  t('ext, 'env, describeConfigNotFinalized);

let withCustomMatchers:
  (
    Rely.MatcherTypes.matchersExtensionFn('ext),
    t('a, 'env, describeConfigNotFinalized)
  ) =>
  t('ext, 'env, describeConfigNotFinalized);

let init: string => t(unit, unit, describeConfigNotFinalized);

let withName:
  (string, t('ext, 'env, 'describeConfigState)) =>
  t('ext, 'env, describeConfigFinalized);

let withNestedTestSuite:
  (
    ~child: t('ext, 'env, describeConfigFinalized) =>
            t('ext, 'env, describeConfigFinalized),
    t('ext, 'env, 'describeConfigState)
  ) =>
  t('ext, 'env, describeConfigFinalized);
let withFailingTests:
  (~only: bool=?, int, t('ext, 'env, 'describeConfigState)) =>
  t('ext, 'env, describeConfigFinalized);
let withPassingTests:
  (~only: bool=?, int, t('ext, 'env, 'describeConfigState)) =>
  t('ext, 'env, describeConfigFinalized);
let withSkippedTests:
  (int, t('ext, 'env, 'describeConfigState)) =>
  t('ext, 'env, describeConfigFinalized);

let skipSuite:
  t('ext, 'env, 'describeConfigState) =>
  t('ext, 'env, describeConfigFinalized);
let only:
  t('ext, 'env, 'describeConfigState) =>
  t('ext, 'env, describeConfigFinalized);
let build: t('ext, 'env, 'describeConfigState) => testSuite;

module Make:
  (TestFramework: Rely.TestFramework) =>
   {
    let register:
      (
        t('ext, 'env, 'describeConfigState),
        ~describe: Rely.Describe.describeFn(unit, unit),
        ~describeSkip: Rely.Describe.describeFn(unit, unit),
        ~describeOnly: Rely.Describe.describeFn(unit, unit)
      ) =>
      unit;
  };
