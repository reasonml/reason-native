/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type matchers('ext) = {
  string: string => StringMatchers.stringMatchersWithNot,
  file: string => StringMatchers.stringMatchersWithNot,
  lines: list(string) => StringMatchers.stringMatchersWithNot,
  bool: bool => BoolMatchers.boolMatchersWithNot,
  int: int => IntMatchers.intMatchersWithNot,
  float: float => FloatMatchers.floatMatchersWithNot,
  fn: 'a. (unit => 'a) => FnMatchers.fnMatchersWithNot,
  list: 'a. list('a) => ListMatchers.matchersWithNot('a),
  array: 'a. array('a) => ArrayMatchers.matchersWithNot('a),
  equal: 'a. EqualsMatcher.equalsMatcher('a),
  notEqual: 'a. EqualsMatcher.equalsMatcher('a),
  same: 'a. SameMatcher.sameMatcher('a),
  notSame: 'a. SameMatcher.sameMatcher('a),
  mock:
    'fn 'ret 'tupledArgs.
    Mock.t('fn, 'ret, 'tupledArgs) =>
    MockMatchers.matchersWithNot('tupledArgs, 'ret),

  option: 'a. option('a) => OptionMatchers.optionMatchersWithNot('a),
  result:
    'a 'b.
    result('a, 'b) => ResultMatchers.resultMatchersWithNot('a, 'b),

  ext: 'ext,
  assertions: int => unit,
  hasAssertions: unit => unit,
};

type testUtils('ext, 'env) = {
  expect: matchers('ext),
  env: 'env,
};
type testFn('ext, 'env) = (string, testUtils('ext, 'env) => unit) => unit;
