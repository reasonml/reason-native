/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open CommonOption.Infix;
open MatcherTypes;
open EqualityValidator;

type equalsFn('a) = ('a, 'a) => bool;

type equalsMatcher('a) = (~equals: equalsFn('a)=?, 'a, 'a) => unit;

let passMessageThunk = () => "";
let defaultEqualityFn = (==);

let suggestCustomEqualityForFloat = "Note that you are testing for float equality using (==). You probably need to specify a ~equals function to the matcher.";

let makeEqualMatcher =
    (
      accessorPath,
      {createMatcher},
      ~equals=defaultEqualityFn,
      expected,
      actual,
    ) => {
  let equalsMatcherImplementation =
    createMatcher(
      (
        {matcherHint, formatReceived, formatExpected, indent},
        actualThunk,
        expectedThunk,
      ) => {
      let actual = actualThunk();
      let (equals, expected) = expectedThunk();

      let pass = equals(actual, expected);
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
              | Fail(DoubleArrayTag) => Some(suggestCustomEqualityForFloat)
              }
            )
            >>| (m => "\n\n" ++ Pastel.dim(m))
            |?: "";
          };
        let message =
          String.concat(
            "",
            [
              MatcherUtils.singleLevelMatcherHint(
                ~expectType=accessorPath,
                ~options={
                  comment: isUsingDefaultEquality ? Some("using ==") : None,
                },
                (),
              ),
              "\n\n",
              "Expected value to equal:\n",
              indent(formatExpected(PolymorphicPrint.print(expected))),
              "\n",
              "Received:\n",
              indent(formatReceived(PolymorphicPrint.print(actual))),
              suggestCustomEqualityMessage,
            ],
          );
        (() => message, pass);
      };
    });
  equalsMatcherImplementation(() => actual, () => (equals, expected));
};

let makeNotEqualMatcher =
    (
      accessorPath,
      {createMatcher},
      ~equals=defaultEqualityFn,
      expected,
      actual,
    ) => {
  let notEqualsMatcherImplementation =
    createMatcher(
      (
        {matcherHint, formatReceived, formatExpected, indent},
        actualThunk,
        expectedThunk,
      ) => {
      let actual = actualThunk();
      let (equals, expected) = expectedThunk();

      let pass = !equals(actual, expected);
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
              | Fail(DoubleArrayTag) => Some(suggestCustomEqualityForFloat)
              }
            )
            >>| (m => "\n\n" ++ Pastel.dim(m))
            |?: "";
          };

        let message =
          String.concat(
            "",
            [
              MatcherUtils.singleLevelMatcherHint(
                ~expectType=accessorPath,
                ~options={
                  comment: isUsingDefaultEquality ? Some("using ==") : None,
                },
                (),
              ),
              "\n\n",
              "Expected value to not equal:\n",
              indent(formatExpected(PolymorphicPrint.print(expected))),
              "\n",
              "Received:\n",
              indent(formatReceived(PolymorphicPrint.print(actual))),
              suggestCustomEqualityMessage,
            ],
          );
        (() => message, pass);
      };
    });
  notEqualsMatcherImplementation(() => actual, () => (equals, expected));
};
