/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open MatcherTypes;

type equalsFn('a) = ('a, 'a) => bool;

type listMatchers('a) = {
  toEqual: (~equals: equalsFn('a)=?, list('a)) => unit,
  toBeEmpty: unit => unit,
};

type listMatchersWithNot('a) = {
  not: listMatchers('a),
  toEqual: (~equals: equalsFn('a)=?, list('a)) => unit,
  toBeEmpty: unit => unit,
};

let passMessageThunk = () => "";

let makeMatchers = (accessorPath, {createMatcher}) => {
  let rec listEquals =
          (listA: list('a), listB: list('a), equals: ('a, 'a) => bool): bool =>
    switch (listA, listB) {
    | ([], []) => true
    | ([], _) => false
    | (_, []) => false
    | ([a, ...listAExtra], [b, ...listBExtra]) when equals(a, b) =>
      listEquals(listAExtra, listBExtra, equals)
    | _ => false
    };

  let createListMatchers = actual => {
    let toEqual = isNot =>
      createMatcher(
        (
          {matcherHint, formatReceived, formatExpected},
          actualThunk,
          expectedThunk,
        ) => {
        let actual = actualThunk();
        let (equals, expected) = expectedThunk();
        let actualEqualsExpected = listEquals(actual, expected, equals);
        let pass =
          actualEqualsExpected && !isNot || !actualEqualsExpected && isNot;
        if (!pass) {
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~expectType=accessorPath,
                  ~matcherName=".toEqual",
                  ~isNot,
                  (),
                ),
                "\n\n",
                "Expected: ",
                formatExpected(PolymorphicPrint.print(expected)),
                "\n",
                "Received: ",
                formatReceived(PolymorphicPrint.print(actual)),
              ],
            );
          (() => message, pass);
        } else {
          (passMessageThunk, pass);
        };
      });

    let toBeEmpty =
      createMatcher(({matcherHint, formatReceived}, actualThunk, _) => {
        let actual = actualThunk();
        let pass = actual == [];
        if (pass) {
          (passMessageThunk, pass);
        } else {
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~matcherName=".toBeEmpty",
                  ~expectType=accessorPath,
                  ~expected="",
                  (),
                ),
                "\n\n",
                "Received: ",
                formatReceived(PolymorphicPrint.print(actual)),
              ],
            );
          (() => message, pass);
        };
      });

    let notToBeEmpty =
      createMatcher(({matcherHint, formatReceived}, actualThunk, _) => {
        let actual = actualThunk();
        let pass = actual != [];
        if (pass) {
          (passMessageThunk, pass);
        } else {
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~matcherName=".not.toBeEmpty",
                  ~expectType=accessorPath,
                  ~expected="",
                  (),
                ),
                "\n\n",
                "Expected value not to be empty, but received: ",
                formatReceived("[]"),
              ],
            );
          (() => message, pass);
        };
      });

    let makeListMatchers = isNot => {
      toEqual: (~equals=(==), expected) =>
        toEqual(isNot, () => actual, () => (equals, expected)),
      toBeEmpty:
        isNot ?
          expected => notToBeEmpty(() => actual, () => ()) :
          expected => toBeEmpty(() => actual, () => ()),
    };

    let listMatchers = makeListMatchers(false);
    {
      not: makeListMatchers(true),
      toEqual: listMatchers.toEqual,
      toBeEmpty: expected => toBeEmpty(() => actual, () => ()),
    };
  };
  createListMatchers;
};
