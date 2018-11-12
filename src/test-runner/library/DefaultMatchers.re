/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 */
open StringMatchers;
open BoolMatchers;
open SnapshotMatchers;
open IntMatchers;

type matchers = {
  string: string => stringMatchersWithNot,
  file: string => stringMatchersWithNot,
  lines: list(string) => stringMatchersWithNot,
  bool: bool => boolMatchersWithNot,
  int: int => intMatchersWithNot,
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
};
