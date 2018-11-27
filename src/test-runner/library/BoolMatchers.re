/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 *
 * LICENSE file in the root directory of this source tree.
 */;
open MatcherTypes;
type boolMatchers = {
  toBe: bool => unit,
  toBeTrue: unit => unit,
  toBeFalse: unit => unit,
};
type boolMatchersWithNot = {
  not: boolMatchers,
  toBe: bool => unit,
  toBeTrue: unit => unit,
  toBeFalse: unit => unit,
};

let passMessageThunk = () => "";

let makeMatchers = (accessorPath, {createMatcher}) => {
  let createBoolMatchers = actual => {
    let toBe = isNot =>
      createMatcher(
        (
          {matcherHint, formatReceived, formatExpected},
          actualThunk,
          expectedThunk,
        ) => {
        let actual = actualThunk();
        let expected = expectedThunk();
        let actualEqualsExpected = actual == expected;
        let pass =
          actualEqualsExpected && !isNot || !actualEqualsExpected && isNot;
        if (!pass) {
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~expectType=accessorPath,
                  ~matcherName=".toBe",
                  ~isNot,
                  (),
                ),
                "\n\n",
                "Expected: ",
                formatExpected(string_of_bool(expected)),
                "\n",
                "Received: ",
                formatReceived(string_of_bool(actual)),
              ],
            );
          (() => message, pass);
        } else {
          (passMessageThunk, pass);
        };
      });
    let toBeTrue = isNot =>
      createMatcher(
        (
          {matcherHint, formatReceived, formatExpected},
          actualThunk,
          expectedThunk,
        ) => {
        let actual = actualThunk();
        let actualEqualsExpected = actual == true;
        let pass =
          actualEqualsExpected && !isNot || !actualEqualsExpected && isNot;

        if (!pass) {
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~expectType=accessorPath,
                  ~matcherName=".toBeTrue",
                  ~isNot,
                  ~expected="",
                  (),
                ),
                "\n\n",
                "Received: ",
                formatReceived(string_of_bool(actual)),
              ],
            );
          (() => message, pass);
        } else {
          (passMessageThunk, pass);
        };
      });
    let toBeFalse = isNot =>
      createMatcher(
        (
          {matcherHint, formatReceived, formatExpected},
          actualThunk,
          expectedThunk,
        ) => {
        let actual = actualThunk();
        let actualEqualsExpected = actual == false;
        let pass =
          actualEqualsExpected && !isNot || !actualEqualsExpected && isNot;

        if (!pass) {
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~expectType=accessorPath,
                  ~matcherName=".toBeFalse",
                  ~isNot,
                  ~expected="",
                  (),
                ),
                "\n\n",
                "Received: ",
                formatReceived(string_of_bool(actual)),
              ],
            );
          (() => message, pass);
        } else {
          (passMessageThunk, pass);
        };
      });
    let makeBoolMatchers = isNot => {
      toBe: expected => toBe(isNot, () => actual, () => expected),
      toBeTrue: expected => toBeTrue(isNot, () => actual, () => expected),
      toBeFalse: expected => toBeFalse(isNot, () => actual, () => expected),
    };
    let boolMatchers = makeBoolMatchers(false);
    {
      not: makeBoolMatchers(true),
      toBe: boolMatchers.toBe,
      toBeTrue: boolMatchers.toBeTrue,
      toBeFalse: boolMatchers.toBeFalse,
    };
  };
  createBoolMatchers;
};
