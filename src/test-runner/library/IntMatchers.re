/**
* Copyright 2004-present Facebook. All Rights Reserved.
*/
open MatcherTypes;

type intMatchers = {toBe: int => unit};
type intMatchersWithNot = {
  not: intMatchers,
  toBe: int => unit,
};

let passMessageThunk = () => "";

let makeMatchers = (accessorPath, {createMatcher}) => {
  let createIntMatchers = actual => {
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
                formatExpected(string_of_int(expected)),
                "\n",
                "Received: ",
                formatReceived(string_of_int(actual)),
              ],
            );
          (() => message, pass);
        } else {
          (passMessageThunk, pass);
        };
      });

    let makeIntMatchers = isNot => {
      toBe: expected => toBe(isNot, () => actual, () => expected),
    };

    let intMatchers = makeIntMatchers(false);
    {not: makeIntMatchers(true), toBe: intMatchers.toBe};
  };
  createIntMatchers;
};
