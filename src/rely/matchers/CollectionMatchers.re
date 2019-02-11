/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open CommonOption.Infix;
open EqualityValidator;
open MatcherTypes;

type equalsFn('a) = ('a, 'a) => bool;
type predicate('a) = 'a => bool;

module type Collection = {
  type t('a);
  let isEmpty: t('a) => bool;
  let collectionEquals: (~memberEquals: equalsFn('a), t('a), t('a)) => bool;
  let emptyDisplay: string;
  let collectionName: string;
  let contains: ('a, equalsFn('a), t('a)) => bool;
};

let suggestToContainEqualForFloats = "Note that you are testing for referential equality between floats. You probably need to use `toContainEqual` with a custom ~equals function instead.";
let suggestToContainEqualForStrings = "Note that you are testing for referential equality between strings. You probably need to use `toContainEqual` instead.";
let suggestToContainEqual = "Note that you are testing for referential equality with the stricter `toContain` matcher. You probably need to use `toContainEqual` instead.";
let suggestCustomEqualityForFloat = "Note that you are testing for float equality using (==). You probably need to specify a ~equals function to the matcher.";

let defaultEqualityFn = (==);

module Make = (Collection: Collection) => {
  type matchers('a) = {
    toEqual: (~equals: equalsFn('a)=?, Collection.t('a)) => unit,
    toBeEmpty: unit => unit,
    toContain: 'a => unit,
    toContainEqual: (~equals: equalsFn('a)=?, 'a) => unit,
  };

  type matchersWithNot('a) = {
    not: matchers('a),
    toEqual: (~equals: equalsFn('a)=?, Collection.t('a)) => unit,
    toBeEmpty: unit => unit,
    toContain: 'a => unit,
    toContainEqual: (~equals: equalsFn('a)=?, 'a) => unit,
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
            {matcherHint, formatReceived, formatExpected, indent},
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
                    ~options={
                      comment:
                        equals === defaultEqualityFn ?
                          Some("using ==") : None,
                    },
                    ~isNot,
                    (),
                  ),
                  "\n\n",
                  isNot ?
                    "Expected value not to equal:\n" :
                    "Expected value to equal:\n",
                  indent(formatExpected(printCollection(expected))),
                  "\n",
                  "Received:\n",
                  indent(formatReceived(printCollection(actual))),
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
        createMatcher(
          (
            {matcherHint, formatReceived, formatExpected, indent},
            actualThunk,
            expectedThunk,
          ) => {
          let actual = actualThunk();
          let expected = expectedThunk();
          let pass = Collection.contains(expected, (===), actual);
          if (pass) {
            (passMessageThunk, pass);
          } else {
            let actualContainsEqualExpected =
              Collection.contains(expected, defaultEqualityFn, actual);

            let suggestToContainEqualMessage =
              (
                switch (
                  actualContainsEqualExpected,
                  validateUsageOfReferentialEquality(expected),
                ) {
                | (_, Fail(DoubleTag))
                | (_, Fail(DoubleArrayTag)) =>
                  Some(suggestToContainEqualForFloats)
                | (_, Fail(StringTag)) =>
                  Some(suggestToContainEqualForStrings)
                | (_, Fail(ObjectTag))
                | (true, Pass) => Some(suggestToContainEqual)
                | (false, Pass) => None
                }
              )
              >>| (m => "\n\n" ++ Pastel.dim(m))
              |?: "";

            let message =
              String.concat(
                "",
                [
                  matcherHint(
                    ~matcherName=".toContain",
                    ~expectType=accessorPath,
                    ~expected="value",
                    ~received=Collection.collectionName,
                    ~options={comment: Some("using ===")},
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
                  suggestToContainEqualMessage,
                ],
              );
            (() => message, pass);
          };
        });

      let notToContain =
        createMatcher(
          (
            {matcherHint, formatReceived, formatExpected, indent},
            actualThunk,
            expectedThunk,
          ) => {
          let actual = actualThunk();
          let expected = expectedThunk();
          let pass = !Collection.contains(expected, (===), actual);
          if (pass) {
            (passMessageThunk, pass);
          } else {
            let suggestToContainEqualMessage =
              (
                switch (validateUsageOfReferentialEquality(expected)) {
                | Fail(DoubleTag)
                | Fail(DoubleArrayTag) =>
                  Some(suggestToContainEqualForFloats)
                | Fail(StringTag) => Some(suggestToContainEqualForStrings)
                | Fail(ObjectTag) => Some(suggestToContainEqual)
                | Pass => None
                }
              )
              >>| (m => "\n\n" ++ Pastel.dim(m))
              |?: "";
            let message =
              String.concat(
                "",
                [
                  matcherHint(
                    ~matcherName=".toContain",
                    ~expectType=accessorPath,
                    ~isNot=true,
                    ~expected="value",
                    ~options={comment: Some("using ===")},
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
                  suggestToContainEqualMessage,
                ],
              );
            (() => message, pass);
          };
        });

      let toContainEqual =
        createMatcher(
          (
            {matcherHint, formatReceived, formatExpected, indent},
            actualThunk,
            expectedThunk,
          ) => {
          let actual = actualThunk();
          let (equals, expected) = expectedThunk();
          let pass = Collection.contains(expected, equals, actual);
          if (pass) {
            (passMessageThunk, pass);
          } else {
            let isUsingDefaultEquality = equals === defaultEqualityFn;
            let suggestCustomEqualityMessage =
              if (!isUsingDefaultEquality) {
                "";
              } else {
                (
                  switch (validateUsageOfStructuralEquality(expected)) {
                  | Pass => None
                  | Fail(DoubleTag)
                  | Fail(DoubleArrayTag) =>
                    Some(suggestCustomEqualityForFloat)
                  }
                )
                >>| (m => "\n\n" ++ Pastel.dim(m))
                |?: "";
              };
            let message =
              String.concat(
                "",
                [
                  matcherHint(
                    ~matcherName=".toContainEqual",
                    ~expectType=accessorPath,
                    ~expected="value",
                    ~received=Collection.collectionName,
                    ~options={
                      comment:
                        isUsingDefaultEquality ? Some("using ==") : None,
                    },
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
                  suggestCustomEqualityMessage,
                ],
              );
            (() => message, pass);
          };
        });

      let notToContainEqual =
        createMatcher(
          (
            {matcherHint, formatReceived, formatExpected, indent},
            actualThunk,
            expectedThunk,
          ) => {
          let actual = actualThunk();
          let (equals, expected) = expectedThunk();
          let pass = !Collection.contains(expected, equals, actual);
          if (pass) {
            (passMessageThunk, pass);
          } else {
            let isUsingDefaultEquality = equals === defaultEqualityFn;
            let suggestCustomEqualityMessage =
              if (!isUsingDefaultEquality) {
                "";
              } else {
                (
                  switch (validateUsageOfStructuralEquality(expected)) {
                  | Pass => None
                  | Fail(DoubleTag)
                  | Fail(DoubleArrayTag) =>
                    Some(suggestCustomEqualityForFloat)
                  }
                )
                >>| (m => "\n\n" ++ Pastel.dim(m))
                |?: "";
              };
            let message =
              String.concat(
                "",
                [
                  matcherHint(
                    ~matcherName=".toContainEqual",
                    ~expectType=accessorPath,
                    ~isNot=true,
                    ~expected="value",
                    ~received=Collection.collectionName,
                    ~options={
                      comment:
                        isUsingDefaultEquality ? Some("using ==") : None,
                    },
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
                  suggestCustomEqualityMessage,
                ],
              );
            (() => message, pass);
          };
        });

      let makeCollectionMatchers = isNot => {
        toEqual: (~equals=defaultEqualityFn, expected) =>
          toEqual(isNot, () => actual, () => (equals, expected)),
        toBeEmpty:
          isNot ?
            expected => notToBeEmpty(() => actual, () => ()) :
            (expected => toBeEmpty(() => actual, () => ())),
        toContain: expected =>
          isNot ?
            notToContain(() => actual, () => expected) :
            toContain(() => actual, () => expected),
        toContainEqual: (~equals=defaultEqualityFn, expected) =>
          isNot ?
            notToContainEqual(() => actual, () => (equals, expected)) :
            toContainEqual(() => actual, () => (equals, expected)),
      };

      let collectionMatchers = makeCollectionMatchers(false);
      {
        not: makeCollectionMatchers(true),
        toEqual: collectionMatchers.toEqual,
        toBeEmpty: collectionMatchers.toBeEmpty,
        toContain: collectionMatchers.toContain,
        toContainEqual: collectionMatchers.toContainEqual,
      };
    };
    createcollectionMatchers;
  };
};
