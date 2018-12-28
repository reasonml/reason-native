/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open MatcherTypes;

type equalsFn('a) = ('a, 'a) => bool;
type predicate('a) = ('a) => bool;

module type Collection = {
  type t('a);
  let isEmpty: t('a) => bool;
  let collectionEquals: (~memberEquals: equalsFn('a), t('a), t('a)) => bool;
  let emptyDisplay: string;
  let collectionName: string;
  let contains: ('a, equalsFn('a), t('a)) => bool;
};

module Make = (Collection: Collection) => {
  type matchers('a) = {
    toEqual: (~equals: equalsFn('a)=?, Collection.t('a)) => unit,
    toBeEmpty: unit => unit,
    toContain: (~equals: equalsFn('a)=?, 'a) => unit
  };

  type matchersWithNot('a) = {
    not: matchers('a),
    toEqual: (~equals: equalsFn('a)=?, Collection.t('a)) => unit,
    toBeEmpty: unit => unit,
    toContain: (~equals: equalsFn('a)=?, 'a) => unit,
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

      let toContain =
        createMatcher(({matcherHint, formatReceived, formatExpected, indent}, actualThunk, expectedThunk) => {
          let actual = actualThunk();
          let (equals, expected) = expectedThunk();
          let pass = Collection.contains(expected, equals, actual);
          if (pass) {
            (passMessageThunk, pass);
          } else {
            let message =
              String.concat(
                "",
                [
                  matcherHint(
                    ~matcherName=".toContain",
                    ~expectType=accessorPath,
                    ~expected="value",
                    ~received=Collection.collectionName,
                    (),
                  ),
                  "\n\n",
                  "Expected " ++ Collection.collectionName ++ ":",
                  "\n",
                  indent(formatReceived(printCollection(actual))),
                  "\n",
                  "To contain value: ",
                  "\n",
                  indent(formatExpected(PolymorphicPrint.print(expected))),
                ],
              );
            (() => message, pass);
          };
        });

      let notToContain =
        createMatcher(({matcherHint, formatReceived, formatExpected, indent}, actualThunk, expectedThunk) => {
          let actual = actualThunk();
          let (equals, expected) = expectedThunk();
          let pass = !Collection.contains(expected, equals, actual);
          if (pass) {
            (passMessageThunk, pass);
          } else {
            let message =
              String.concat(
                "",
                [
                  matcherHint(
                    ~matcherName=".toContain",
                    ~expectType=accessorPath,
                    ~isNot=true,
                    ~expected="value",
                    ~received=Collection.collectionName,
                    (),
                  ),
                  "\n\n",
                  "Expected " ++ Collection.collectionName ++ ":",
                  "\n",
                  indent(formatReceived(printCollection(actual))),
                  "\n",
                  "Not to contain value: ",
                  "\n",
                  indent(formatExpected(PolymorphicPrint.print(expected))),
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
        toContain:
          (~equals=(==), expected) =>
            isNot ? notToContain(() => actual, () => (equals, expected))
            : toContain(() => actual, () => (equals, expected))
      };

      let collectionMatchers = makecollectionMatchers(false);
      {
        not: makecollectionMatchers(true),
        toEqual: collectionMatchers.toEqual,
        toBeEmpty: collectionMatchers.toBeEmpty,
        toContain: collectionMatchers.toContain
      };
    };
    createcollectionMatchers;
  };
};
