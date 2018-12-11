/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open MatcherTypes;
type stringMatchers = {
  toBeEmpty: unit => unit,
  toEqual: string => unit,
  toEqualFile: string => unit,
  toEqualLines: list(string) => unit,
  toMatch: string => unit,
};
type stringMatchersWithNot = {
  not: stringMatchers,
  toBeEmpty: unit => unit,
  toEqual: string => unit,
  toEqualFile: string => unit,
  toEqualLines: list(string) => unit,
  toMatch: string => unit,
  toMatchSnapshot: unit => unit,
};

let escape = (s: string): string => {
  let lines = Str.split(Str.regexp("\n"), s);
  let lines = List.map(line => String.escaped(line), lines);
  String.concat("\n", lines);
};

let wrapInQuotes = s => String.concat("", ["\"", s, "\""]);

let passMessageThunk = () => "";

let formatLiteralString = s => s |> escape |> wrapInQuotes;

let makeMatchers = (accessorPath, snapshotMatcher, utils) => {
  let {createMatcher} = utils;
  let createStringMatchers = sActual => {
    let toBeEmpty =
      createMatcher(({matcherHint, formatReceived, _}, actualThunk, _) => {
        let actual = actualThunk();
        let pass = actual == "";
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
                formatReceived(formatLiteralString(actual)),
              ],
            );
          (() => message, pass);
        };
      });

    let notToBeEmpty =
      createMatcher(({matcherHint, formatReceived, _}, actualThunk, _) => {
        let actual = actualThunk();
        let pass = actual != "";
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
                formatReceived("\"\""),
              ],
            );
          (() => message, pass);
        };
      });
    let toEqual =
      createMatcher(
        (
          {matcherHint, formatReceived, formatExpected, indent, prepareDiff},
          actualThunk,
          expectedThunk,
        ) => {
        let actual = actualThunk();
        let expected = expectedThunk();
        let pass = actual == expected;
        if (pass) {
          (passMessageThunk, pass);
        } else {
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~matcherName=".toEqual",
                  ~expectType=accessorPath,
                  (),
                ),
                "\n\n",
                "Expected value to equal:\n",
                indent(formatExpected(formatLiteralString(expected))),
                "\n",
                "Received:\n",
                indent(formatReceived(formatLiteralString(actual))),
                "\n\nDifference:\n",
                indent(prepareDiff(actual, expected)),
              ],
            );
          (() => message, pass);
        };
      });

    let notToEqual =
      createMatcher(
        (
          {matcherHint, formatReceived, formatExpected, indent, _},
          actualThunk,
          expectedThunk,
        ) => {
        let actual = actualThunk();
        let expected = expectedThunk();
        let pass = actual != expected;
        if (pass) {
          (passMessageThunk, pass);
        } else {
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~matcherName=".not.toEqual",
                  ~expectType=accessorPath,
                  (),
                ),
                "\n\n",
                "Expected value to not equal:\n",
                indent(formatExpected(formatLiteralString(expected))),
                "\n",
                "Received:\n",
                indent(formatReceived(formatLiteralString(actual))),
              ],
            );
          (() => message, pass);
        };
      });

    let toEqualFile = isNot =>
      createMatcher(
        (
          {matcherHint, formatReceived, formatExpected, indent, prepareDiff},
          actualThunk,
          expectedThunk,
        ) => {
        let filePath = expectedThunk();
        let expected = IO.readFile(filePath);
        let actual = actualThunk();
        let pass = actual == expected;
        if (!isNot && !pass) {
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~matcherName=".toEqualFile",
                  ~expectType=accessorPath,
                  (),
                ),
                "\n\n",
                "Expected actual to equal the file: ",
                formatExpected(filePath),
                "\n",
                "Contents:\n",
                indent(formatExpected(expected)),
                "\n",
                "Received:\n",
                indent(formatReceived(actual)),
                "\n\nDifference:\n",
                indent(prepareDiff(actual, expected)),
              ],
            );
          (() => message, false);
        } else if (isNot && pass) {
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~matcherName=".not.toEqualFile",
                  ~expectType=accessorPath,
                  (),
                ),
                "\n\n",
                "Expected actual not to equal the file: ",
                formatExpected(filePath),
                "\n",
                "But both were\n",
                indent(formatReceived(actual)),
              ],
            );
          (() => message, false);
        } else {
          (passMessageThunk, true);
        };
      });

    let toEqualLines = isNot =>
      createMatcher(
        (
          {matcherHint, formatReceived, formatExpected, indent, prepareDiff},
          actualThunk,
          expectedThunk,
        ) => {
        let actual = actualThunk();
        let expectedLines = expectedThunk();
        let expected = String.concat("\n", expectedLines);
        let pass = actual == expected;

        if (!isNot && !pass) {
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~matcherName=".toEqualLines",
                  ~expectType=accessorPath,
                  (),
                ),
                "\n\n",
                "Expected value to equal:\n",
                indent(formatExpected(formatLiteralString(expected))),
                "\n",
                "Received:\n",
                indent(formatReceived(formatLiteralString(actual))),
                "\n\nDifference:\n",
                indent(prepareDiff(actual, expected)),
              ],
            );
          (() => message, false);
        } else if (isNot && pass) {
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~matcherName=".toEqualLines",
                  ~isNot=true,
                  ~expectType=accessorPath,
                  (),
                ),
                "\n\n",
                "Expected value to not equal:\n",
                indent(formatExpected(formatLiteralString(expected))),
                "\n",
                "Received:\n",
                indent(formatReceived(formatLiteralString(actual))),
              ],
            );
          (() => message, false);
        } else {
          (passMessageThunk, true);
        };
      });

    let toMatch = isNot =>
      createMatcher(
        (
          {matcherHint, formatReceived, formatExpected, indent, _},
          actualThunk,
          expectedThunk,
        ) => {
        let actual = actualThunk();
        let regex = expectedThunk();
        let pass = Str.string_match(Str.regexp(regex), actual, 0);

        if (!isNot && !pass) {
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~matcherName=".toMatch",
                  ~expectType=accessorPath,
                  (),
                ),
                "\n\n",
                "Expected value to match:\n",
                indent(formatExpected(formatLiteralString(regex))),
                "\n",
                "Received:\n",
                indent(formatReceived(formatLiteralString(actual))),
              ],
            );
          (() => message, false);
        } else if (isNot && pass) {
          let message =
            String.concat(
              "",
              [
                matcherHint(
                  ~matcherName=".toMatch",
                  ~isNot=true,
                  ~expectType=accessorPath,
                  (),
                ),
                "\n\n",
                "Expected value not to match:\n",
                indent(formatExpected(formatLiteralString(regex))),
                "\n",
                "Received:\n",
                indent(formatReceived(formatLiteralString(actual))),
              ],
            );
          (() => message, false);
        } else {
          (passMessageThunk, true);
        };
      });

    let matchers = {
      not: {
        toBeEmpty: () => notToBeEmpty(() => sActual, () => ()),
        toEqual: expected => notToEqual(() => sActual, () => expected),
        toEqualFile: filePath =>
          toEqualFile(true, () => sActual, () => filePath),
        toEqualLines: lines => toEqualLines(true, () => sActual, () => lines),
        toMatch: regex => toMatch(true, () => sActual, () => regex),
      },
      toBeEmpty: () => toBeEmpty(() => sActual, () => ()),
      toEqual: expected => toEqual(() => sActual, () => expected),
      toEqualFile: filePath =>
        toEqualFile(false, () => sActual, () => filePath),
      toEqualLines: lines => toEqualLines(false, () => sActual, () => lines),
      toMatch: regex => toMatch(false, () => sActual, () => regex),
      toMatchSnapshot: () => {
        let matcher: SnapshotMatchers.snapshotMatchersRecord =
          snapshotMatcher(accessorPath, utils, sActual);
        matcher.toMatchSnapshot();
      },
    };
    matchers;
  };
  createStringMatchers;
};
