/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
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

let suggest_to_contain_equal =
  Pastel.dim(
    "Note that you are testing for referential equality with the stricter `toContain` matcher. You probably need to use `toContainEqual` instead.",
  );

let suggest_custom_equality_for_float =
  Pastel.dim(
    "Note that you are testing for float equality using (==). You probably need to specify a ~equals function to the matcher.",
  );

let isTypeThatMightNotWantToUseReferentialEquality = value =>
  switch (Obj.tag(Obj.magic(value))) {
  | tag when tag == Obj.object_tag => true
  | tag when tag == Obj.string_tag => true
  | tag when tag == Obj.double_tag => true
  | tag when tag == Obj.double_array_tag => true
  | _ => false
  };

let isFloatOrArrayOfFloat = value =>
  switch (Obj.tag(Obj.magic(value))) {
  | tag when tag == Obj.double_tag => true
  | tag when tag == Obj.double_array_tag => true
  | _ => false
  };

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
            let suggestToContainEqual =
              Collection.contains(expected, defaultEqualityFn, actual)
              || isTypeThatMightNotWantToUseReferentialEquality(expected);
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
                ],
              )
              ++ (
                suggestToContainEqual ? "\n\n" ++ suggest_to_contain_equal : ""
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
            let suggestToContainEqual =
              isTypeThatMightNotWantToUseReferentialEquality(expected);
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
                ],
              )
              ++ (
                suggestToContainEqual ? "\n\n" ++ suggest_to_contain_equal : ""
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
            let suggestCustomEquality =
              isFloatOrArrayOfFloat(expected) && equals === defaultEqualityFn;
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
                        equals === defaultEqualityFn ?
                          Some("using ==") : None,
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
                ],
              )
              ++ (
                suggestCustomEquality ?
                  "\n\n" ++ suggest_custom_equality_for_float : ""
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
            let suggestCustomEquality =
              isFloatOrArrayOfFloat(expected) && equals === defaultEqualityFn;
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
                        equals === defaultEqualityFn ?
                          Some("using ==") : None,
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
                ],
              )
              ++ (
                suggestCustomEquality ?
                  "\n\n" ++ suggest_custom_equality_for_float : ""
              );
            (() => message, pass);
          };
        });

      let makecollectionMatchers = isNot => {
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

      let collectionMatchers = makecollectionMatchers(false);
      {
        not: makecollectionMatchers(true),
        toEqual: collectionMatchers.toEqual,
        toBeEmpty: collectionMatchers.toBeEmpty,
        toContain: collectionMatchers.toContain,
        toContainEqual: collectionMatchers.toContainEqual,
      };
    };
    createcollectionMatchers;
  };
};
