/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Decorators;
WinConsoleColorsSupport.enable();

module PastelTerminalLexer =
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

let length = PastelTerminalLexer.length;
let partition = PastelTerminalLexer.partition;
let unformattedText = PastelTerminalLexer.unformattedText;

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
  reduceTokens(style, "", PastelTerminalLexer.tokenize(str));

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

module TerminalStateMachine = 
  StateMachine.Make({
    let lex = TerminalLexer.lex;

    let makeCodesRegex = codes => {
      let start = "\027\\[";
      let codesExpr = String.concat("|", List.map(string_of_int, codes));
      let stop = "m";
      let regexString =
        String.concat("", ["(", start, "(", codesExpr, ")", stop, ")"]);
      Re.Pcre.regexp(regexString);
    };
    
    let startRegex = makeCodesRegex(Ansi.IntSet.elements(Ansi.starts));
    let stopRegex = makeCodesRegex(Ansi.IntSet.elements(Ansi.stops));
    
    let applyCode = (modifier, s) => String.concat("", [modifier, s]);
    let tokenize = TerminalLexer.tokenize;
    let applyState = (stateDiff: StateMachine.state, s) => {
      let s =
        switch (stateDiff.foreground) {
        | None => s
        | Some(color) =>
          switch (color) {
          | Token.Default => applyCode(Ansi.color.stop, s)
          | Black => applyCode(Ansi.color.black.start, s)
          | Red => applyCode(Ansi.color.red.start, s)
          | Green => applyCode(Ansi.color.green.start, s)
          | Yellow => applyCode(Ansi.color.yellow.start, s)
          | Blue => applyCode(Ansi.color.blue.start, s)
          | Magenta => applyCode(Ansi.color.magenta.start, s)
          | Cyan => applyCode(Ansi.color.cyan.start, s)
          | White => applyCode(Ansi.color.white.start, s)
          | BrightBlack => applyCode(Ansi.color.blackBright.start, s)
          | BrightRed => applyCode(Ansi.color.redBright.start, s)
          | BrightGreen => applyCode(Ansi.color.greenBright.start, s)
          | BrightYellow => applyCode(Ansi.color.yellowBright.start, s)
          | BrightBlue => applyCode(Ansi.color.blueBright.start, s)
          | BrightMagenta => applyCode(Ansi.color.magentaBright.start, s)
          | BrightCyan => applyCode(Ansi.color.cyanBright.start, s)
          | BrightWhite => applyCode(Ansi.color.whiteBright.start, s)
          }
        };
      let s =
        switch (stateDiff.background) {
        | None => s
        | Some(color) =>
          switch (color) {
          | Default => applyCode(Ansi.bg.stop, s)
          | Black => applyCode(Ansi.bg.black.start, s)
          | Red => applyCode(Ansi.bg.red.start, s)
          | Green => applyCode(Ansi.bg.green.start, s)
          | Yellow => applyCode(Ansi.bg.yellow.start, s)
          | Blue => applyCode(Ansi.bg.blue.start, s)
          | Magenta => applyCode(Ansi.bg.magenta.start, s)
          | Cyan => applyCode(Ansi.bg.cyan.start, s)
          | White => applyCode(Ansi.bg.white.start, s)
          | BrightBlack => applyCode(Ansi.bg.blackBright.start, s)
          | BrightRed => applyCode(Ansi.bg.redBright.start, s)
          | BrightGreen => applyCode(Ansi.bg.greenBright.start, s)
          | BrightYellow => applyCode(Ansi.bg.yellowBright.start, s)
          | BrightBlue => applyCode(Ansi.bg.blueBright.start, s)
          | BrightMagenta => applyCode(Ansi.bg.magentaBright.start, s)
          | BrightCyan => applyCode(Ansi.bg.cyanBright.start, s)
          | BrightWhite => applyCode(Ansi.bg.whiteBright.start, s)
          }
        };
      /* bold and dim are undone by the same sequence, so we handle them jointly*/
      let s =
        switch (stateDiff.bold, stateDiff.dim) {
        | (None, None) => s
        | (Some(Bold), Some(Dim)) =>
          s
          |> applyCode(Ansi.modifier.bold.start)
          |> applyCode(Ansi.modifier.dim.start)
        | (Some(Bold), Some(DimOff)) =>
          s
          |> applyCode(Ansi.modifier.bold.start)
          |> applyCode(Ansi.modifier.dim.stop)
        | (Some(BoldOff), Some(Dim)) =>
          s
          |> applyCode(Ansi.modifier.dim.start)
          |> applyCode(Ansi.modifier.bold.stop)
        | (Some(Bold), None) => s |> applyCode(Ansi.modifier.bold.start)
        | (None, Some(Dim)) => s |> applyCode(Ansi.modifier.dim.start)
        | (Some(BoldOff), None)
        | (None, Some(DimOff))
        | (Some(BoldOff), Some(DimOff)) =>
          s |> applyCode(Ansi.modifier.bold.stop)
        };
      let s =
        switch (stateDiff.italic) {
        | None => s
        | Some(Italic) => applyCode(Ansi.modifier.italic.start, s)
        | Some(ItalicOff) => applyCode(Ansi.modifier.italic.stop, s)
        };
      let s =
        switch (stateDiff.underline) {
        | None => s
        | Some(Underline) => applyCode(Ansi.modifier.underline.start, s)
        | Some(UnderlineOff) => applyCode(Ansi.modifier.underline.stop, s)
        };
      let s =
        switch (stateDiff.inverse) {
        | None => s
        | Some(Inverse) => applyCode(Ansi.modifier.inverse.start, s)
        | Some(InverseOff) => applyCode(Ansi.modifier.inverse.stop, s)
        };
      let s =
        switch (stateDiff.hidden) {
        | None => s
        | Some(Hidden) => applyCode(Ansi.modifier.hidden.start, s)
        | Some(HiddenOff) => applyCode(Ansi.modifier.hidden.stop, s)
        };
      let s =
        switch (stateDiff.strikethrough) {
        | None => s
        | Some(Strikethrough) =>
          applyCode(Ansi.modifier.strikethrough.start, s)
        | Some(StrikethroughOff) =>
          applyCode(Ansi.modifier.strikethrough.stop, s)
        };
      s;
    };
  });