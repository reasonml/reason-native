/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Decorators;
WinConsoleColorsSupport.enable();

module TerminalStateMachine =
  StateMachine.Make({
    let lex = TerminalLexer.lex;

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
      let s =
        switch (stateDiff.reset) {
        | None => s
        | Some(Reset) => applyCode(Ansi.modifier.reset, s)
        | Some(ResetOff) =>
          applyCode(Ansi.modifier.resetOff, s)
        };
      s;
    };
  });

let fromString = TerminalStateMachine.fromString;
let length = TerminalStateMachine.lengthFromString;
let unformattedText = TerminalStateMachine.unformattedTextFromString;
let color = TerminalStateMachine.color;
let bg = TerminalStateMachine.bg;
let modifier = TerminalStateMachine.modifier;
let partition = TerminalStateMachine.partitionFromString;

let make =
    (
      ~reset: option(bool)=?,
      ~bold: option(bool)=?,
      ~dim: option(bool)=?,
      ~italic: option(bool)=?,
      ~underline: option(bool)=?,
      ~inverse: option(bool)=?,
      ~hidden: option(bool)=?,
      ~strikethrough: option(bool)=?,
      ~color: option(ColorName.colorName)=?,
      ~backgroundColor: option(ColorName.colorName)=?,
      inputs: list(string),
    ) => {
  let color = PastelUtils.(optionMap(color, colorNameToColor));
  let backgroundColor =
    PastelUtils.(optionMap(backgroundColor, colorNameToColor));

  fromString(
    ~reset?,
    ~bold?,
    ~dim?,
    ~italic?,
    ~underline?,
    ~inverse?,
    ~hidden?,
    ~strikethrough?,
    ~color?,
    ~backgroundColor?,
    inputs,
  );
};

let implementation: PastelImplementation.t = {
  modifier,
  color,
  bg,
  length,
  unformattedText,
  partition,
  create,
  emptyStyle: StateMachine.initialState,
  parse: TerminalStateMachine.parseStateRegions,
  apply: TerminalStateMachine.resolveStateRegions,
};
