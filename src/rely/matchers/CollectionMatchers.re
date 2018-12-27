/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open MatcherTypes;

type equalsFn('a) = ('a, 'a) => bool;

module type Collection = {
  type t('a);
  let isEmpty: t('a) => bool;
  let collectionEquals: (~memberEquals: equalsFn('a), t('a), t('a)) => bool;
  let emptyDisplay: string;
};

module Make = (Collection: Collection) => {
  type matchers('a) = {
    toEqual: (~equals: equalsFn('a)=?, Collection.t('a)) => unit,
    toBeEmpty: unit => unit,
  };

  type matchersWithNot('a) = {
    not: matchers('a),
    toEqual: (~equals: equalsFn('a)=?, Collection.t('a)) => unit,
    toBeEmpty: unit => unit,
  };

  let passMessageThunk = () => "";
  let printCollection = collection =>
    Collection.isEmpty(collection) ?
      Collection.emptyDisplay : PolymorphicPrint.print(collection);

  let makeMatchers = (accessorPath, {createMatcher}) => {
    let createcollectionMatchers = actual => {
      let toEqual = isNot =>
        createMatcher(
          (
            {matcherHint, formatReceived, formatExpected},
            actualThunk,
            expectedThunk,
          ) => {
          let actual = actualThunk();
          let (equals, expected) = expectedThunk();
          let actualEqualsExpected =
            Collection.collectionEquals(
              actual,
              expected,
              ~memberEquals=equals,
            );
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
                  formatExpected(printCollection(expected)),
                  "\n",
                  "Received: ",
                  formatReceived(printCollection(actual)),
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
          let pass = Collection.isEmpty(actual);
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
                  formatReceived(printCollection(actual)),
                ],
              );
            (() => message, pass);
          };
        });

      let notToBeEmpty =
        createMatcher(({matcherHint, formatReceived}, actualThunk, _) => {
          let actual = actualThunk();
          let pass = !Collection.isEmpty(actual);
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
                  formatReceived(Collection.emptyDisplay),
                ],
              );
            (() => message, pass);
          };
        });

      let makecollectionMatchers = isNot => {
        toEqual: (~equals=(==), expected) =>
          toEqual(isNot, () => actual, () => (equals, expected)),
        toBeEmpty:
          isNot ?
            expected => notToBeEmpty(() => actual, () => ()) :
            (expected => toBeEmpty(() => actual, () => ())),
      };

      let collectionMatchers = makecollectionMatchers(false);
      {
        not: makecollectionMatchers(true),
        toEqual: collectionMatchers.toEqual,
        toBeEmpty: expected => toBeEmpty(() => actual, () => ()),
      };
    };
    createcollectionMatchers;
  };
};
