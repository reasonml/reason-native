/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Pastel;
open Common;

type matcherHintOptions = {comment: option(string)};

type matcherUtils = {
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
let indent = (~indent: string, s: string): string => {
  let lines = Str.split(Str.regexp("\n"), s);
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
        Pastel.dim(
          String.concat("", [")", isNot ? ".not" : "", matcherName, "("]),
        ),
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

let matcherUtils = {
  matcherHint,
  formatReceived,
  formatExpected,
  prepareDiff,
  indent: indent(~indent=valueIndent),
};
