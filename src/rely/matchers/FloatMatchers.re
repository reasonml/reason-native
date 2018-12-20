/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open MatcherTypes;

type floatMatchers = {toBeCloseTo: (~digits: int=?, float) => unit};
type floatMatchersWithNot = {
  toBeCloseTo: (~digits: int=?, float) => unit,
  not: floatMatchers,
};

let passMessageThunk = () => "";

let makeMatchers = (accessorPath, {createMatcher}) => {
  let createFloatMatchers = actual => {
    let toBeCloseTo = isNot =>
      createMatcher(
        (
          {matcherHint, formatReceived, formatExpected},
          actualThunk,
          expectedThunk,
        ) => {
        let (digits, expected) = expectedThunk();
        let actual = actualThunk();
        let precision = (10.0 ** float_of_int(-digits)) /. 2.0;

        let actualEqualsExpected = abs_float(actual -. expected) < precision;

        let pass =
          (actualEqualsExpected && !isNot) || (!actualEqualsExpected && isNot);
        if (!pass) {
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~expectType=accessorPath,
                  ~matcherName=".toBeCloseTo",
                  ~isNot,
                  ~expected="expected, precision",
                  (),
                ),
                "\n\n",
                "Precision: ",
                formatExpected(string_of_int(digits)) ++ "-digit",
                "\n",
                "Expected: ",
                formatExpected(string_of_float(expected)),
                "\n",
                "Received: ",
                formatReceived(string_of_float(actual)),
              ],
            );
          (() => message, pass);
        } else {
          (passMessageThunk, pass);
        };
      });

    let makeFloatMatchers = isNot => {
      toBeCloseTo: (~digits=2, expected) => toBeCloseTo(isNot, () => actual, () => (digits, expected)),
    };

    let floatMatchers = makeFloatMatchers(false);
    {not: makeFloatMatchers(true), toBeCloseTo: floatMatchers.toBeCloseTo};
  };
  createFloatMatchers;
};
