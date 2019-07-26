/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Decorators;
WinConsoleColorsSupport.enable();

module TerminalLexer =
  PastelLexer.Make({
    let makeCodesRegex = codes => {
      let start = "\027\\[";
      let codesExpr = String.concat("|", List.map(string_of_int, codes));
      let stop = "m";
      let regexString =
        String.concat("", ["(", start, "(", codesExpr, ")", stop, ")", "+"]);
      Re.Pcre.regexp(regexString);
    };
    let startRegex = makeCodesRegex(Ansi.IntSet.elements(Ansi.starts));
    let stopRegex = makeCodesRegex(Ansi.IntSet.elements(Ansi.stops));
  });

let length = TerminalLexer.length;
let partition = TerminalLexer.partition;
let partition2 = TerminalLexer.partition2;
let unformattedText = TerminalLexer.unformattedText;

let rec reduceTokens = (style: Ansi.style, str, tokens) =>
  switch (tokens) {
  | [] => str
  | [PastelLexer.Starts(starts), Text(text), Stops(stops), ...rest] =>
    reduceTokens(
      style,
      String.concat("", [str, style.start, starts, text, stops, style.stop]),
      rest,
    )
  | [Text(text), ...rest] =>
    reduceTokens(
      style,
      String.concat("", [str, style.start, text, style.stop]),
      rest,
    )
  /* This case should only be hit when the user's input is a string containing
   * ANSI escape sequences that Pastel also uses */
  | [Starts(escapeCodes) | Stops(escapeCodes), Text(text), ...rest] =>
    reduceTokens(
      style,
      String.concat(
        "",
        /* persist input escape codes by applying them to the inner text,
         * and then also after the stop code in case the stop code
         * for the style conflicts with the escape codes from user input */
        [str, style.start, escapeCodes, text, style.stop, escapeCodes],
      ),
      rest,
    )
  /* This case should only be hit when the user's input
   * is a string containing ANSI escape sequences that Pastel also uses */
  | [Starts(escapeCodes) | Stops(escapeCodes), ...rest] =>
    reduceTokens(style, String.concat("", [str, escapeCodes]), rest)
  };

let makeDecorator = (style: Ansi.style, str) =>
  reduceTokens(style, "", TerminalLexer.tokenize(str));

let modifier: modifier = {
  bold: makeDecorator(Ansi.modifier.bold),
  dim: makeDecorator(Ansi.modifier.dim),
  italic: makeDecorator(Ansi.modifier.italic),
  underline: makeDecorator(Ansi.modifier.underline),
  inverse: makeDecorator(Ansi.modifier.inverse),
  hidden: makeDecorator(Ansi.modifier.hidden),
  strikethrough: makeDecorator(Ansi.modifier.strikethrough),
};

let color: color = {
  black: makeDecorator(Ansi.color.black),
  red: makeDecorator(Ansi.color.red),
  green: makeDecorator(Ansi.color.green),
  yellow: makeDecorator(Ansi.color.yellow),
  blue: makeDecorator(Ansi.color.blue),
  magenta: makeDecorator(Ansi.color.magenta),
  cyan: makeDecorator(Ansi.color.cyan),
  white: makeDecorator(Ansi.color.white),
  blackBright: makeDecorator(Ansi.color.blackBright),
  redBright: makeDecorator(Ansi.color.redBright),
  greenBright: makeDecorator(Ansi.color.greenBright),
  yellowBright: makeDecorator(Ansi.color.yellowBright),
  blueBright: makeDecorator(Ansi.color.blueBright),
  magentaBright: makeDecorator(Ansi.color.magentaBright),
  cyanBright: makeDecorator(Ansi.color.cyanBright),
  whiteBright: makeDecorator(Ansi.color.whiteBright),
};

let bg: color = {
  black: makeDecorator(Ansi.bg.black),
  red: makeDecorator(Ansi.bg.red),
  green: makeDecorator(Ansi.bg.green),
  yellow: makeDecorator(Ansi.bg.yellow),
  blue: makeDecorator(Ansi.bg.blue),
  magenta: makeDecorator(Ansi.bg.magenta),
  cyan: makeDecorator(Ansi.bg.cyan),
  white: makeDecorator(Ansi.bg.white),
  blackBright: makeDecorator(Ansi.bg.blackBright),
  redBright: makeDecorator(Ansi.bg.redBright),
  greenBright: makeDecorator(Ansi.bg.greenBright),
  yellowBright: makeDecorator(Ansi.bg.yellowBright),
  blueBright: makeDecorator(Ansi.bg.blueBright),
  magentaBright: makeDecorator(Ansi.bg.magentaBright),
  cyanBright: makeDecorator(Ansi.bg.cyanBright),
  whiteBright: makeDecorator(Ansi.bg.whiteBright),
};
