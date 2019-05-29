/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
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

let singleLevelMatcherHint:
    (
      ~expectType: string,
      ~received:string=?,
      ~expected:string=?,
      ~options: matcherHintOptions=?,
      unit,
    ) => string;

let formatInt: int => string;

let nthToString: int => string;

let matcherUtils: t;
