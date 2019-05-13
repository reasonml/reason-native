/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open MatcherTypes;

type equalsFn('a) = ('a, 'a) => bool;

type resultMatchers('a, 'b) = {
  toBeOk: unit => unit,
  toBeError: unit => unit,
  toBe:
    (
      ~equalsOk: equalsFn('a)=?,
      ~equalsError: equalsFn('b)=?,
      result('a, 'b)
    ) =>
    unit,
};

type resultMatchersWithNot('a, 'b) = {
  not: resultMatchers('a, 'b),
  toBeOk: unit => unit,
  toBeError: unit => unit,
  toBe:
    (
      ~equalsOk: equalsFn('a)=?,
      ~equalsError: equalsFn('b)=?,
      result('a, 'b)
    ) =>
    unit,
};

let passMessageThunk = () => "";
let defaultEqualityFn = (==);

let printResult = res =>
  switch (res) {
  | Ok(v) => String.concat("", ["Ok(", PolymorphicPrint.print(v), ")"])
  | Error(v) =>
    String.concat("", ["Error(", PolymorphicPrint.print(v), ")"])
  };

let isOk = res =>
  switch (res) {
  | Ok(_) => true
  | Error(_) => false
  };

let isError = res =>
  switch (res) {
  | Ok(_) => false
  | Error(_) => true
  };

let makeMatchers = (accessorPath, {createMatcher}) => {
  let createResultMatchers = actual => {
    let toBe = isNot =>
      createMatcher(
        (
          {matcherHint, formatReceived, formatExpected},
          actualThunk,
          expectedThunk,
        ) => {
        let actual = actualThunk();
        let (equalsOk, equalsError, expected) = expectedThunk();
        let actualEqualsExpected =
          switch (actual, expected) {
          | (Error(actual), Error(expected))
              when equalsError(actual, expected) =>
            true
          | (Ok(actualValue), Ok(expectedValue))
              when equalsOk(actualValue, expectedValue) =>
            true
          | (_, _) => false
          };
        let pass =
          actualEqualsExpected && !isNot || !actualEqualsExpected && isNot;
        if (!pass) {
          let isUsingDefaultEquality =
            isOk(actual) ?
              equalsOk === defaultEqualityFn :
              equalsError === defaultEqualityFn;
          let displayEqualityMessage = isUsingDefaultEquality;
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~expectType=accessorPath,
                  ~matcherName=".toBe",
                  ~isNot,
                  ~options={
                    comment: displayEqualityMessage ? Some("using ==") : None,
                  },
                  (),
                ),
                "\n\n",
                "Expected: ",
                formatExpected(printResult(expected)),
                "\n",
                "Received: ",
                formatReceived(printResult(actual)),
              ],
            );
          (() => message, pass);
        } else {
          (passMessageThunk, pass);
        };
      });

    let toBeError = isNot =>
      createMatcher(
        (
          {matcherHint, formatReceived, formatExpected},
          actualThunk,
          expectedThunk,
        ) => {
        let actual = actualThunk();
        let actualIsError = isError(actual);
        switch (actualIsError, isNot) {
        | (true, false)
        | (false, true) => (passMessageThunk, true)
        | (false, false) =>
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~expectType=accessorPath,
                  ~matcherName=".toBeError",
                  ~expected="",
                  ~isNot,
                  (),
                ),
                "\n\n",
                "Expected value to be Error, but received: ",
                formatReceived(printResult(actual)),
              ],
            );
          ((() => message), false);
        | (true, true) =>
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~expectType=accessorPath,
                  ~matcherName=".toBeError",
                  ~expected="",
                  ~isNot,
                  (),
                ),
                "\n\n",
                "Expected value to not be Error, but received: ",
                formatReceived(printResult(actual)),
              ],
            );
          ((() => message), false);
        };
      });

    let toBeOk = isNot =>
      createMatcher(
        (
          {matcherHint, formatReceived, formatExpected},
          actualThunk,
          expectedThunk,
        ) => {
        let actual = actualThunk();
        let actualIsOk = isOk(actual);

        switch (actualIsOk, isNot) {
        | (true, false)
        | (false, true) => (passMessageThunk, true)
        | (false, false) =>
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~expectType=accessorPath,
                  ~matcherName=".toBeOk",
                  ~expected="",
                  ~isNot,
                  (),
                ),
                "\n\n",
                "Expected value to be Ok, but received: ",
                formatReceived(printResult(actual)),
              ],
            );
          ((() => message), false);
        | (true, true) =>
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~expectType=accessorPath,
                  ~matcherName=".toBeOk",
                  ~expected="",
                  ~isNot,
                  (),
                ),
                "\n\n",
                "Expected value to not be Ok, but received: ",
                formatReceived(printResult(actual)),
              ],
            );
          ((() => message), false);
        };
      });
    let makeResultMatchers = isNot => {
      toBe:
        (
          ~equalsOk=defaultEqualityFn,
          ~equalsError=defaultEqualityFn,
          expected,
        ) =>
        toBe(isNot, () => actual, () => (equalsOk, equalsError, expected)),
      toBeError: () => toBeError(isNot, () => actual, () => ()),
      toBeOk: () => toBeOk(isNot, () => actual, () => ()),
    };
    let resultMatchers = makeResultMatchers(false);
    {
      not: makeResultMatchers(true),
      toBe: resultMatchers.toBe,
      toBeError: resultMatchers.toBeError,
      toBeOk: resultMatchers.toBeOk,
    };
  };
  createResultMatchers;
};
