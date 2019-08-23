/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Decorators;

module HumanReadableStateMachine =
  StateMachine.Make({
    let lex = HumanReadableLexer.lex;
    let applyCode = (modifier, s) => String.concat("", [modifier, s]);

    let applyState = (stateDiff: StateMachine.state, s) => {
      let s =
        switch (stateDiff.foreground) {
        | None => s
        | Some(color) =>
          switch (color) {
          | Token.Default => applyCode(HumanReadable.color.stop, s)
          | Black => applyCode(HumanReadable.color.black.start, s)
          | Red => applyCode(HumanReadable.color.red.start, s)
          | Green => applyCode(HumanReadable.color.green.start, s)
          | Yellow => applyCode(HumanReadable.color.yellow.start, s)
          | Blue => applyCode(HumanReadable.color.blue.start, s)
          | Magenta => applyCode(HumanReadable.color.magenta.start, s)
          | Cyan => applyCode(HumanReadable.color.cyan.start, s)
          | White => applyCode(HumanReadable.color.white.start, s)
          | BrightBlack => applyCode(HumanReadable.color.blackBright.start, s)
          | BrightRed => applyCode(HumanReadable.color.redBright.start, s)
          | BrightGreen => applyCode(HumanReadable.color.greenBright.start, s)
          | BrightYellow =>
            applyCode(HumanReadable.color.yellowBright.start, s)
          | BrightBlue => applyCode(HumanReadable.color.blueBright.start, s)
          | BrightMagenta =>
            applyCode(HumanReadable.color.magentaBright.start, s)
          | BrightCyan => applyCode(HumanReadable.color.cyanBright.start, s)
          | BrightWhite => applyCode(HumanReadable.color.whiteBright.start, s)
          }
        };
      let s =
        switch (stateDiff.background) {
        | None => s
        | Some(color) =>
          switch (color) {
          | Default => applyCode(HumanReadable.bg.stop, s)
          | Black => applyCode(HumanReadable.bg.black.start, s)
          | Red => applyCode(HumanReadable.bg.red.start, s)
          | Green => applyCode(HumanReadable.bg.green.start, s)
          | Yellow => applyCode(HumanReadable.bg.yellow.start, s)
          | Blue => applyCode(HumanReadable.bg.blue.start, s)
          | Magenta => applyCode(HumanReadable.bg.magenta.start, s)
          | Cyan => applyCode(HumanReadable.bg.cyan.start, s)
          | White => applyCode(HumanReadable.bg.white.start, s)
          | BrightBlack => applyCode(HumanReadable.bg.blackBright.start, s)
          | BrightRed => applyCode(HumanReadable.bg.redBright.start, s)
          | BrightGreen => applyCode(HumanReadable.bg.greenBright.start, s)
          | BrightYellow => applyCode(HumanReadable.bg.yellowBright.start, s)
          | BrightBlue => applyCode(HumanReadable.bg.blueBright.start, s)
          | BrightMagenta =>
            applyCode(HumanReadable.bg.magentaBright.start, s)
          | BrightCyan => applyCode(HumanReadable.bg.cyanBright.start, s)
          | BrightWhite => applyCode(HumanReadable.bg.whiteBright.start, s)
          }
        };
      /* bold and dim are undone by the same sequence, so we handle them jointly*/
      let s =
        switch (stateDiff.bold, stateDiff.dim) {
        | (None, None) => s
        | (Some(Bold), Some(Dim)) =>
          s
          |> applyCode(HumanReadable.modifier.bold.start)
          |> applyCode(HumanReadable.modifier.dim.start)
        | (Some(Bold), Some(DimOff)) =>
          s
          |> applyCode(HumanReadable.modifier.bold.start)
          |> applyCode(HumanReadable.modifier.dim.stop)
        | (Some(BoldOff), Some(Dim)) =>
          s
          |> applyCode(HumanReadable.modifier.dim.start)
          |> applyCode(HumanReadable.modifier.bold.stop)
        | (Some(Bold), None) =>
          s |> applyCode(HumanReadable.modifier.bold.start)
        | (None, Some(Dim)) =>
          s |> applyCode(HumanReadable.modifier.dim.start)
        | (Some(BoldOff), None)
        | (None, Some(DimOff))
        | (Some(BoldOff), Some(DimOff)) =>
          s |> applyCode(HumanReadable.modifier.bold.stop)
        };
      let s =
        switch (stateDiff.italic) {
        | None => s
        | Some(Italic) => applyCode(HumanReadable.modifier.italic.start, s)
        | Some(ItalicOff) => applyCode(HumanReadable.modifier.italic.stop, s)
        };
      let s =
        switch (stateDiff.underline) {
        | None => s
        | Some(Underline) =>
          applyCode(HumanReadable.modifier.underline.start, s)
        | Some(UnderlineOff) =>
          applyCode(HumanReadable.modifier.underline.stop, s)
        };
      let s =
        switch (stateDiff.inverse) {
        | None => s
        | Some(Inverse) => applyCode(HumanReadable.modifier.inverse.start, s)
        | Some(InverseOff) =>
          applyCode(HumanReadable.modifier.inverse.stop, s)
        };
      let s =
        switch (stateDiff.hidden) {
        | None => s
        | Some(Hidden) => applyCode(HumanReadable.modifier.hidden.start, s)
        | Some(HiddenOff) => applyCode(HumanReadable.modifier.hidden.stop, s)
        };
      let s =
        switch (stateDiff.strikethrough) {
        | None => s
        | Some(Strikethrough) =>
          applyCode(HumanReadable.modifier.strikethrough.start, s)
        | Some(StrikethroughOff) =>
          applyCode(HumanReadable.modifier.strikethrough.stop, s)
        };
      let s =
        switch (stateDiff.reset) {
        | None => s
        | Some(Reset) => applyCode(HumanReadable.modifier.reset.start, s)
        | Some(ResetOff) => applyCode(HumanReadable.modifier.reset.stop, s)
        };
      s;
    };
  });

let fromString = HumanReadableStateMachine.fromString;
let length = HumanReadableStateMachine.lengthFromString;
let unformattedText = HumanReadableStateMachine.unformattedTextFromString;
let color = HumanReadableStateMachine.color;
let bg = HumanReadableStateMachine.bg;
let modifier = HumanReadableStateMachine.modifier;
let partition = HumanReadableStateMachine.partitionFromString;

let colorNameToColor: option(ColorName.colorName) => option(Token.color) =
  color =>
    switch (color) {
    | None => None
    | Some(c) =>
      Some(
        switch (c) {
        | Black => Black
        | Red => Red
        | Green => Green
        | Yellow => Yellow
        | Blue => Blue
        | Magenta => Magenta
        | Cyan => Cyan
        | White => White
        | BlackBright => BrightBlack
        | RedBright => BrightRed
        | GreenBright => BrightGreen
        | YellowBright => BrightYellow
        | BlueBright => BrightBlue
        | MagentaBright => BrightMagenta
        | CyanBright => BrightCyan
        | WhiteBright => BrightWhite
        },
      )
    };

let createElement =
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
      ~children: list(string),
      (),
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
    ~children,
    (),
  );
};

let implementation: PastelImplementation.t = {
  modifier,
  color,
  bg,
  length,
  unformattedText,
  partition,
  createElement,
  emptyStyle: StateMachine.initialState,
  parse: HumanReadableStateMachine.parseStateRegions,
  apply: HumanReadableStateMachine.resolveStateRegions,
};
