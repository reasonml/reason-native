/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open MatcherTypes;

type equalsFn('a) = ('a, 'a) => bool;

type optionMatchers('a) = {
  toBeNone: unit => unit,
  toBeSome: (~equals: equalsFn('a)=?, 'a) => unit,
  toBe: (~equals: equalsFn('a)=?, option('a)) => unit,
};

type optionMatchersWithNot('a) = {
  not: optionMatchers('a),
  toBeNone: unit => unit,
  toBeSome: (~equals: equalsFn('a)=?, 'a) => unit,
  toBe: (~equals: equalsFn('a)=?, option('a)) => unit,
};

let passMessageThunk = () => "";
let defaultEqualityFn = (==);

let printOption = opt =>
  switch (opt) {
  | None => "None"
  | Some(v) => String.concat("", ["Some(", PolymorphicPrint.print(v), ")"])
  };

let isSome = opt =>
  switch (opt) {
  | Some(_) => true
  | None => false
  };

let isNone = opt =>
  switch (opt) {
  | Some(_) => false
  | None => true
  };

let makeMatchers = (accessorPath, {createMatcher}) => {
  let createOptionMatchers = actual => {
    let toBe = isNot =>
      createMatcher(
        (
          {matcherHint, formatReceived, formatExpected},
          actualThunk,
          expectedThunk,
        ) => {
        let actual = actualThunk();
        let (equals, expected) = expectedThunk();
        let actualEqualsExpected =
          switch (actual, expected) {
          | (None, None) => true
          | (Some(actualValue), Some(expectedValue))
              when equals(actualValue, expectedValue) =>
            true
          | (_, _) => false
          };
        let pass =
          actualEqualsExpected && !isNot || !actualEqualsExpected && isNot;
        if (!pass) {
          let isUsingDefaultEquality = equals === defaultEqualityFn;
          let displayEqualityMessage =
            isSome(expected) && isSome(actual) && isUsingDefaultEquality;
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
                formatExpected(printOption(expected)),
                "\n",
                "Received: ",
                formatReceived(printOption(actual)),
              ],
            );
          (() => message, pass);
        } else {
          (passMessageThunk, pass);
        };
      });

    let toBeNone = isNot =>
      createMatcher(
        (
          {matcherHint, formatReceived, formatExpected},
          actualThunk,
          expectedThunk,
        ) => {
        let actual = actualThunk();
        let actualIsNone = actual == None;
        switch (actualIsNone, isNot) {
        | (true, false)
        | (false, true) => (passMessageThunk, true)
        | (false, false) =>
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~expectType=accessorPath,
                  ~matcherName=".toBeNone",
                  ~expected="",
                  ~isNot,
                  (),
                ),
                "\n\n",
                "Expected value not to be None, but received: ",
                formatReceived(printOption(actual)),
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
                  ~matcherName=".toBeNone",
                  ~expected="",
                  ~isNot,
                  (),
                ),
                "\n\n",
                "Expected value to be None, but received: ",
                formatReceived(printOption(actual)),
              ],
            );
          ((() => message), false);
        };
      });

    let toBeSome = isNot =>
      createMatcher(
        (
          {matcherHint, formatReceived, formatExpected},
          actualThunk,
          expectedThunk,
        ) => {
        let actual = actualThunk();
        let (equals, value) = expectedThunk();
        let expected = Some(value);
        let actualEqualsExpected =
          switch (actual, expected) {
          | (None, None) => true
          | (Some(actualValue), Some(expectedValue))
              when equals(actualValue, expectedValue) =>
            true
          | (_, _) => false
          };
        let pass =
          actualEqualsExpected && !isNot || !actualEqualsExpected && isNot;
        if (!pass) {
          let isUsingDefaultEquality = equals === defaultEqualityFn;
          let displayEqualityMessage =
            isSome(expected) && isSome(actual) && isUsingDefaultEquality;
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~expectType=accessorPath,
                  ~matcherName=".toBeSome",
                  ~isNot,
                  ~options={
                    comment: displayEqualityMessage ? Some("using ==") : None,
                  },
                  (),
                ),
                "\n\n",
                "Expected: ",
                formatExpected(printOption(expected)),
                "\n",
                "Received: ",
                formatReceived(printOption(actual)),
              ],
            );
          (() => message, pass);
        } else {
          (passMessageThunk, pass);
        };
      });
    let makeOptionMatchers = isNot => {
      toBe: (~equals=defaultEqualityFn, expected) =>
        toBe(isNot, () => actual, () => (equals, expected)),
      toBeNone: () => toBeNone(isNot, () => actual, () => ()),
      toBeSome: (~equals=defaultEqualityFn, expected) =>
        toBeSome(isNot, () => actual, () => (equals, expected)),
    };
    let optionMatchers = makeOptionMatchers(false);
    {
      not: makeOptionMatchers(true),
      toBe: optionMatchers.toBe,
      toBeNone: optionMatchers.toBeNone,
      toBeSome: optionMatchers.toBeSome,
    };
  };
  createOptionMatchers;
};
