/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type assertionExpectation =
  | HasAssertions(option(Printexc.location), string)
  | NoExpectation
  | HasNAssertions(int, option(Printexc.location), string);

type assertionResult =
  | Passed
  | Failed(string, option(Printexc.location), string);

type t = {
  expectation: assertionExpectation,
  assertions: list(assertionResult),
};

let initialState = {expectation: NoExpectation, assertions: []};

let addAssertionResult = (assertionResult, state) => {
  ...state,
  assertions: state.assertions @ [assertionResult],
};

let setExpectation = (expectation, state) => {...state, expectation};

type assertionStateValidationResult =
  | Valid
  | Invalid(string, option(Printexc.location), string);

let validateAssertionState:
  (MatcherUtils.t, t) => assertionStateValidationResult =
  ({formatExpected, formatReceived}, state) => {
    switch (state.expectation) {
    | NoExpectation => Valid
    | HasAssertions(loc, stack) =>
      switch (state.assertions) {
      | [] =>
        let message =
          String.concat(
            "",
            [
              MatcherUtils.singleLevelMatcherHint(
                ~expectType=".hasAssertions",
                ~received="",
                ~expected="",
                (),
              ),
              "\n\n",
              "Expected ",
              formatExpected("at least one assertion"),
              " to be called, but ",
              formatReceived("received none"),
              ".",
            ],
          );
        Invalid(message, loc, stack);
      | [h, ...t] => Valid
      }
    | HasNAssertions(expectedNumAssertions, loc, stack) =>
      let actualNumAssertions = List.length(state.assertions);
      let isValid = actualNumAssertions == expectedNumAssertions;
      if (isValid) {
        Valid;
      } else {
        let expectedAssertionsText =
          formatExpected(
            MatcherUtils.pluralize("assertion", expectedNumAssertions),
          );
        let receivedAssertionsText =
          formatReceived(
            MatcherUtils.pluralize("assertion call", actualNumAssertions),
          );
        let message =
          String.concat(
            "",
            [
              MatcherUtils.singleLevelMatcherHint(
                ~expectType=".assertions",
                ~received="",
                ~expected=string_of_int(expectedNumAssertions),
                (),
              ),
              "\n\n",
              "Expected ",
              expectedAssertionsText,
              " to be called, but received ",
              receivedAssertionsText,
              ".",
            ],
          );
        Invalid(message, loc, stack);
      };
    };
  };
