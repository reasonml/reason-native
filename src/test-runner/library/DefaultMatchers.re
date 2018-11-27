/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open StringMatchers;
open BoolMatchers;
open SnapshotMatchers;
open IntMatchers;
open FnMatchers;

type matchers = {
  string: string => stringMatchersWithNot,
  file: string => stringMatchersWithNot,
  lines: list(string) => stringMatchersWithNot,
  bool: bool => boolMatchersWithNot,
  int: int => intMatchersWithNot,
  fn: 'a. (unit => 'a) => fnMatchersWithNot
};

let makeDefaultMatchers = (utils, snapshotMatcher) => {
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
  fn: f => FnMatchers.makeMatchers(".fn", utils, f)
};
