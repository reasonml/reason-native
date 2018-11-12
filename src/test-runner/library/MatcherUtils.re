/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 */
open Chalk;

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

let expectedColor = Chalk.green;
let receivedColor = Chalk.red;
let formatReceived = receivedColor;
let formatExpected = expectedColor;

let oldFormatter = Chalk.red;
let newFormatter = Chalk.green;
let dimFormatter = Chalk.blackBright;

let prepareDiff = (o: string, s: string): string => {
  let diff = Strings.diffWords(o, s);
  Strings.printDiff(
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
        Chalk.dim(String.concat("", ["expect", expectType, "("])),
        receivedColor(received),
        Chalk.dim(
          String.concat("", [")", isNot ? ".not" : "", matcherName, "("]),
        ),
        expectedColor(expected),
        Chalk.dim(")"),
      ],
    );
  switch (options.comment) {
  | Some(comment) =>
    let formattedComment =
      Chalk.dim(String.concat("", ["/* ", comment, " */"]));
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
