/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type thunk('a) = unit => 'a;
open MatcherUtils;

type matcher('a, 'b) =
  (matcherUtils, thunk('a), thunk('b)) => (thunk(string), bool);
type matcherConfig('a, 'b) =
  (matcherUtils, unit => 'a, unit => 'b) => (unit => string, bool);
type matcherResult('a, 'b) = (unit => 'a, unit => 'b) => unit;
type createMatcher('a, 'b) = matcherConfig('a, 'b) => matcherResult('a, 'b);
type extendUtils = {createMatcher: 'a 'b. createMatcher('a, 'b)};
