/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open MatcherTypes;

/*
 * toThrowException is explicitly not included on "not" as the expected behavior
 * is difficult to articulate. Jest will pass if it throws an exception other than the
 * one it is not supposed to throw as well as if no exception is thrown. I assert that
 * .not.toThrow is more intuitive and meets most use cases and as a result we should hold off
 * on adding .not for specific exceptions until compelling use cases are brought forward
 */
type fnMatchers = {toThrow: unit => unit};
type fnMatchersWithNot = {
  not: fnMatchers,
  toThrow: unit => unit,
  toThrowException: exn => unit,
};

let passMessageThunk = () => "";

let makeMatchers = (accessorPath, {createMatcher}) => {
  let createFnMatchers = actual => {
    let toThrow =
      createMatcher(
        ({matcherHint, formatReceived, formatExpected}, actualThunk, _) => {
        let actual = actualThunk();
        switch (actual()) {
        | _ =>
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~matcherName=".toThrow",
                  ~isNot=false,
                  ~expectType=accessorPath,
                  ~expected="",
                  ~received="function",
                  (),
                ),
                "\n\n",
                "Expected the function to throw an error.\n",
                "But it didn't throw anything.",
              ],
            );
          ((() => message), false);
        | exception e => (passMessageThunk, true)
        };
      });

    let notToThrow =
      createMatcher(
        ({matcherHint, formatReceived, indent, formatExpected}, actualThunk, _) => {
        let actual = actualThunk();
        switch (actual()) {
        | _ => (passMessageThunk, true)
        | exception e =>
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~matcherName="toThrow",
                  ~isNot=false,
                  ~expectType=accessorPath,
                  ~expected="",
                  ~received="function",
                  (),
                ),
                "\n\n",
                "Expected the function not to throw an error.\n",
                "Instead, it threw: \n",
                indent(formatReceived(Printexc.to_string(e))),
              ],
            );
          ((() => message), false);
        };
      });

    let toThrowException =
      createMatcher(
        (
          {matcherHint, indent, formatReceived, formatExpected},
          actualThunk,
          expectedThunk,
        ) => {
        let actual = actualThunk();
        let expectedExn = expectedThunk();
        switch (actual()) {
        | _ =>
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~matcherName=".toThrowException",
                  ~isNot=false,
                  ~expectType=accessorPath,
                  ~expected="exception",
                  ~received="function",
                  (),
                ),
                "\n\n",
                "Expected the function to throw an error matching:\n",
                indent(formatExpected(Printexc.to_string(expectedExn))),
                "\n",
                "But it didn't throw anything.",
              ],
            );
          ((() => message), false);
        | exception e when e == expectedExn => (passMessageThunk, true)
        | exception e =>
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~matcherName=".toThrowException",
                  ~isNot=false,
                  ~expectType=accessorPath,
                  ~expected="exception",
                  ~received="function",
                  (),
                ),
                "\n\n",
                "Expected the function to throw an error matching:\n",
                indent(formatExpected(Printexc.to_string(expectedExn))),
                "\n",
                "Instead, it threw:\n",
                indent(formatReceived(Printexc.to_string(e)))
              ],
            );
          ((() => message), false);
        };
      });

    let matchers = {
      not: {
        toThrow: () => notToThrow(() => actual, () => ()),
      },
      toThrow: () => toThrow(() => actual, () => ()),
      toThrowException: expectedExn =>
        toThrowException(() => actual, () => expectedExn),
    };
    matchers;
  };
  createFnMatchers;
};
