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
};

module Make = (Mock: Mock.Sig) => {
  module MockMatchers = MockMatchers.Make(Mock);

  let makeDefaultMatchers = (utils, snapshotMatcher, makeMatchers) => {
    string: s =>
      StringMatchers.makeMatchers(".string", snapshotMatcher, utils, s),
    file: filePath => {
      let sActual = IO.readFile(filePath);
      StringMatchers.makeMatchers(".file", snapshotMatcher, utils, sActual);
    },
    lines: lines => {
      let sActual = String.concat("\n", lines);
      StringMatchers.makeMatchers(".lines", snapshotMatcher, utils, sActual);
    },
    bool: b => BoolMatchers.makeMatchers(".bool", utils, b),
    int: i => IntMatchers.makeMatchers(".int", utils, i),
    float: f => FloatMatchers.makeMatchers(".float", utils, f),
    fn: f => FnMatchers.makeMatchers(".fn", utils, f),
    list: l => ListMatchers.makeMatchers(".list", utils, l),
    array: a => ArrayMatchers.makeMatchers(".array", utils, a),
    mock: m => MockMatchers.makeMatchers(".mock", utils, m),
    equal: (~equals=?, expected, actual) =>
      EqualsMatcher.makeEqualMatcher(
        ".equal",
        utils,
        ~equals?,
        expected,
        actual,
      ),
    notEqual: (~equals=?, expected, actual) =>
      EqualsMatcher.makeNotEqualMatcher(
        ".notEqual",
        utils,
        ~equals?,
        expected,
        actual,
      ),
    same: (expected, actual) =>
      SameMatcher.makeSameMatcher(".same", utils, expected, actual),
    notSame: (expected, actual) =>
      SameMatcher.makeNotSameMatcher(".notSame", utils, expected, actual),
    option: o => OptionMatchers.makeMatchers(".option", utils, o),
    result: r => ResultMatchers.makeMatchers(".result", utils, r),
    ext: makeMatchers(utils),
  };
};
