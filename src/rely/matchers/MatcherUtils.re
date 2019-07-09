/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Pastel;
open Common;

type matcherHintOptions = {comment: option(string)};

type t = {
  matcherHint:
    (
      ~matcherName: string,
      ~expectType: string,
      ~isNot: bool=?,
      ~received: string=?,
      ~expected: string=?,
      ~options: matcherHintOptions=?,
      unit
    ) =>
    string,
  formatReceived: string => string,
  formatExpected: string => string,
  prepareDiff: (string, string) => string,
  indent: string => string,
};

let expectedColor = Pastel.green;
let receivedColor = Pastel.red;
let formatReceived = receivedColor;
let formatExpected = expectedColor;

let oldFormatter = Pastel.red;
let newFormatter = Pastel.green;
let dimFormatter = Pastel.blackBright;

let prepareDiff = (o: string, s: string): string => {
  let diff = Common.Strs.Diffs.diffWords(o, s);
  Common.Strs.Diffs.printDiff(
    ~oldFormatter,
    ~newFormatter,
    ~bothFormatter=dimFormatter,
    diff,
  );
};

let valueIndent = "  ";
let newLineRegex = Re.Pcre.regexp("\n");
let indent = (~indent: string, s: string): string => {
  let lines = Re.Pcre.split(newLineRegex, s);
  let lines = List.map(line => indent ++ line, lines);
  String.concat("\n", lines);
};

let matcherHint =
    (
      ~matcherName,
      ~expectType,
      ~isNot=false,
      ~received="received",
      ~expected="expected",
      ~options={comment: None},
      (),
    ) => {
  let assertion =
    String.concat(
      "",
      [
        Pastel.dim(String.concat("", ["expect", expectType, "("])),
        receivedColor(received),
        isNot
          ? Pastel.dim(").") ++ "not" ++ Pastel.dim(matcherName ++ "(")
          : Pastel.dim(String.concat("", [")", matcherName, "("])),
        expectedColor(expected),
        Pastel.dim(")"),
      ],
    );
  switch (options.comment) {
  | Some(comment) =>
    let formattedComment =
      Pastel.dim(String.concat("", ["/* ", comment, " */"]));
    String.concat(" ", [assertion, formattedComment]);
  | None => assertion
  };
};

let singleLevelMatcherHint =
    (
      ~expectType,
      ~received="received",
      ~expected="expected",
      ~options={comment: None},
      (),
    ) => {
  let expectedReceived =
    switch (expected, received) {
    | ("", "") => ""
    | ("", received) => receivedColor(received)
    | (expected, "") => expectedColor(expected)
    | (expected, received) =>
      String.concat(
        "",
        [
          expectedColor(expected),
          Pastel.dim(", "),
          receivedColor(received),
        ],
      )
    };
  let assertion =
    String.concat(
      "",
      [
        Pastel.dim(String.concat("", ["expect", expectType, "("])),
        expectedReceived,
        Pastel.dim(")"),
      ],
    );
  switch (options.comment) {
  | Some(comment) =>
    let formattedComment =
      Pastel.dim(String.concat("", ["/* ", comment, " */"]));
    String.concat(" ", [assertion, formattedComment]);
  | None => assertion
  };
};

let numbers = [|
  "zero",
  "one",
  "two",
  "three",
  "four",
  "five",
  "six",
  "seven",
  "eight",
  "nine",
  "ten",
  "eleven",
  "twelve",
  "thirteen",
|];

let nthToString = nth =>
  switch (nth) {
  | 1 => "first"
  | 2 => "second"
  | 3 => "third"
  | n when n mod 10 == 1 => string_of_int(n) ++ "st"
  | n when n mod 10 == 2 => string_of_int(n) ++ "nd"
  | n when n mod 10 == 3 => string_of_int(n) ++ "rd"
  | n => string_of_int(n) ++ "th"
  };

let formatInt = n =>
  if (0 <= n && n <= Array.length(numbers)) {
    numbers[n];
  } else {
    string_of_int(n);
  };

let pluralize = (word, ~plural=word ++ "s", n) => {
  String.concat(" ", [formatInt(n), n == 1 ? word : plural]);
};

let matcherUtils = {
  matcherHint,
  formatReceived,
  formatExpected,
  prepareDiff,
  indent: indent(~indent=valueIndent),
};
