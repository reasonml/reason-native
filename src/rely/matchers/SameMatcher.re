/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open CommonOption.Infix;
open MatcherTypes;
open EqualityValidator;

type sameMatcher('a) = ('a, 'a) => unit;

let passMessageThunk = () => "";

let suggestEqualsForFloats = "Note that you are testing for referential equality between floats. You probably need to use `expect.equal` with a custom ~equals function instead.";
let suggestEqualForStrings = "Note that you are testing for referential equality between strings. You probably need to use `expect.equal` instead.";
let suggestEqual = "Note that you are testing for referential equality with the stricter `expect.same` matcher. You probably need to use `expect.equal` instead.";

let makeSameMatcher = (accessorPath, {createMatcher}, expected, actual) => {
  let sameMatcherImplementation =
    createMatcher(
      (
        {matcherHint, formatReceived, formatExpected, indent},
        actualThunk,
        expectedThunk,
      ) => {
      let actual = actualThunk();
      let expected = expectedThunk();

      let pass = actual === expected;
      if (pass) {
        (passMessageThunk, pass);
      } else {
        let actualEqualsExpected = expected == actual;

        let suggestToContainEqualMessage =
          (
            switch (
              actualEqualsExpected,
              validateUsageOfReferentialEquality(expected),
            ) {
            | (_, Fail(DoubleTag))
            | (_, Fail(DoubleArrayTag)) => Some(suggestEqualsForFloats)
            | (_, Fail(StringTag)) => Some(suggestEqualForStrings)
            | (_, Fail(ObjectTag))
            | (true, Pass) => Some(suggestEqual)
            | (false, Pass) => None
            }
          )
          >>| (m => "\n\n" ++ Pastel.dim(m))
          |?: "";

        let message =
          String.concat(
            "",
            [
              MatcherUtils.singleLevelMatcherHint(
                ~expectType=accessorPath,
                ~options={comment: Some("using ===")},
                (),
              ),
              "\n\n",
              "Expected value to be:\n",
              indent(formatExpected(PolymorphicPrint.print(expected))),
              "\n",
              "Received:\n",
              indent(formatReceived(PolymorphicPrint.print(actual))),
              suggestToContainEqualMessage,
            ],
          );
        (() => message, pass);
      };
    });
  sameMatcherImplementation(() => actual, () => expected);
};

let makeNotSameMatcher = (accessorPath, {createMatcher}, expected, actual) => {
  let notSameMatcherImplementation =
    createMatcher(
      (
        {matcherHint, formatReceived, formatExpected, indent},
        actualThunk,
        expectedThunk,
      ) => {
      let actual = actualThunk();
      let expected = expectedThunk();

      let pass = actual !== expected;
      if (pass) {
        (passMessageThunk, pass);
      } else {
        let suggestToContainEqualMessage =
          (
            switch (validateUsageOfReferentialEquality(expected)) {
            | Fail(DoubleTag)
            | Fail(DoubleArrayTag) => Some(suggestEqualsForFloats)
            | Fail(StringTag) => Some(suggestEqualForStrings)
            | Fail(ObjectTag) => Some(suggestEqual)
            | Pass => None
            }
          )
          >>| (m => "\n\n" ++ Pastel.dim(m))
          |?: "";
        let message =
          String.concat(
            "",
            [
              MatcherUtils.singleLevelMatcherHint(
                ~expectType=accessorPath,
                ~options={comment: Some("using ===")},
                (),
              ),
              "\n\n",
              "Expected value to not be:\n",
              indent(formatExpected(PolymorphicPrint.print(expected))),
              "\n",
              "Received:\n",
              indent(formatReceived(PolymorphicPrint.print(actual))),
              suggestToContainEqualMessage,
            ],
          );
        (() => message, pass);
      };
    });
  notSameMatcherImplementation(() => actual, () => expected);
};
