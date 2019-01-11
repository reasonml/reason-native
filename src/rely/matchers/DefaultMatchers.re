/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open StringMatchers;
open BoolMatchers;
open SnapshotMatchers;
open IntMatchers;
open FloatMatchers;
open FnMatchers;
open MatcherTypes;

type matchers('ext) = {
  string: string => stringMatchersWithNot,
  file: string => stringMatchersWithNot,
  lines: list(string) => stringMatchersWithNot,
  bool: bool => boolMatchersWithNot,
  int: int => intMatchersWithNot,
  float: float => floatMatchersWithNot,
  fn: 'a. (unit => 'a) => fnMatchersWithNot,
  ext: 'ext,
};

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
  ext: makeMatchers(utils),
};
