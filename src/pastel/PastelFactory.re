/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Mode;
open Decorators;
open ColorName;

module Make = (()) => {
  let inferMode = fd =>
    switch (SupportsColor.inferLevel(fd)) {
    | NoSupport => Disabled
    | BasicColorSupport
    | Has256ColorSupport
    | TrueColorSupport => Terminal
    };

  let defaultMode = inferMode(Unix.stdin);
  let mode = ref(defaultMode);
  let selectedImplementation = ref(TerminalImplementation.implementation);
  let setMode = m => {
    switch (m) {
    | Terminal =>
      selectedImplementation := TerminalImplementation.implementation
    | Disabled =>
      selectedImplementation := DisabledImplementation.implementation
    | HumanReadable =>
      selectedImplementation := HumanReadableImplementation.implementation
    };
    mode := m;
  };

  setMode(defaultMode);

  let getMode = () => mode^;

  let useMode = (mode, f) => {
    let prevMode = getMode();
    setMode(mode);
    let value =
      try (f()) {
      | e =>
        setMode(prevMode);
        raise(e);
      };
    setMode(prevMode);
    value;
  };

  let modifier: modifier = {
    bold: s => selectedImplementation^.modifier.bold(s),
    dim: s => selectedImplementation^.modifier.dim(s),
    italic: s => selectedImplementation^.modifier.italic(s),
    underline: s => selectedImplementation^.modifier.underline(s),
    inverse: s => selectedImplementation^.modifier.inverse(s),
    hidden: s => selectedImplementation^.modifier.hidden(s),
    strikethrough: s => selectedImplementation^.modifier.strikethrough(s),
  };

  let color = {
    black: s => selectedImplementation^.color.black(s),
    red: s => selectedImplementation^.color.red(s),
    green: s => selectedImplementation^.color.green(s),
    yellow: s => selectedImplementation^.color.yellow(s),
    blue: s => selectedImplementation^.color.blue(s),
    magenta: s => selectedImplementation^.color.magenta(s),
    cyan: s => selectedImplementation^.color.cyan(s),
    white: s => selectedImplementation^.color.white(s),
    blackBright: s => selectedImplementation^.color.blackBright(s),
    redBright: s => selectedImplementation^.color.redBright(s),
    greenBright: s => selectedImplementation^.color.greenBright(s),
    yellowBright: s => selectedImplementation^.color.yellowBright(s),
    blueBright: s => selectedImplementation^.color.blueBright(s),
    magentaBright: s => selectedImplementation^.color.magentaBright(s),
    cyanBright: s => selectedImplementation^.color.cyanBright(s),
    whiteBright: s => selectedImplementation^.color.whiteBright(s),
  };
  let bg = {
    black: s => selectedImplementation^.bg.black(s),
    red: s => selectedImplementation^.bg.red(s),
    green: s => selectedImplementation^.bg.green(s),
    yellow: s => selectedImplementation^.bg.yellow(s),
    blue: s => selectedImplementation^.bg.blue(s),
    magenta: s => selectedImplementation^.bg.magenta(s),
    cyan: s => selectedImplementation^.bg.cyan(s),
    white: s => selectedImplementation^.bg.white(s),
    blackBright: s => selectedImplementation^.bg.blackBright(s),
    redBright: s => selectedImplementation^.bg.redBright(s),
    greenBright: s => selectedImplementation^.bg.greenBright(s),
    yellowBright: s => selectedImplementation^.bg.yellowBright(s),
    blueBright: s => selectedImplementation^.bg.blueBright(s),
    magentaBright: s => selectedImplementation^.bg.magentaBright(s),
    cyanBright: s => selectedImplementation^.bg.cyanBright(s),
    whiteBright: s => selectedImplementation^.bg.whiteBright(s),
  };

  let length = s => selectedImplementation^.length(s);

  let partition = s => selectedImplementation^.partition(s);

  let unformattedText = s => selectedImplementation^.unformattedText(s);

  let bold = modifier.bold;

  let dim = modifier.dim;

  let italic = modifier.italic;

  let underline = modifier.underline;

  let inverse = modifier.inverse;

  let hidden = modifier.hidden;

  let strikethrough = modifier.strikethrough;

  let black = color.black;

  let red = color.red;

  let green = color.green;

  let yellow = color.yellow;

  let blue = color.blue;

  let magenta = color.magenta;

  let cyan = color.cyan;

  let white = color.white;

  let blackBright = color.blackBright;

  let redBright = color.redBright;

  let greenBright = color.greenBright;

  let yellowBright = color.yellowBright;

  let blueBright = color.blueBright;

  let magentaBright = color.magentaBright;

  let cyanBright = color.cyanBright;

  let whiteBright = color.whiteBright;

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
        ~color: option(colorName)=?,
        ~backgroundColor: option(colorName)=?,
        ~children: list(string),
        (),
      ) => {
    selectedImplementation^.createElement(
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

  type style = StateMachine.state;
  let withColor = (color, style: StateMachine.state) => {
    ...style,
    foreground: Some(PastelUtils.colorNameToColor(color)),
  };
  let resetColor: style => style = style => {...style, foreground: None};
  let withBackgroundColor = (color, style: StateMachine.state) => {
    ...style,
    background: Some(PastelUtils.colorNameToColor(color)),
  };
  let resetBackgroundColor: style => style =
    style => {...style, background: None};

  let setBold: (bool, style) => style =
    (bold, style) => {...style, bold: Some(bold ? Bold : BoldOff)};
  let withBold = setBold(true);
  let setDim: (bool, style) => style =
    (dim, style) => {...style, dim: Some(dim ? Dim : DimOff)};
  let withDim = setDim(true);
  let setItalic: (bool, style) => style =
    (italic, style) => {
      ...style,
      italic: Some(italic ? Italic : ItalicOff),
    };
  let withItalic = setItalic(true);
  let setUnderline: (bool, style) => style =
    (underline, style) => {
      ...style,
      underline: Some(underline ? Underline : UnderlineOff),
    };
  let withUnderline = setUnderline(true);
  let setInverse: (bool, style) => style =
    (inverse, style) => {
      ...style,
      inverse: Some(inverse ? Inverse : InverseOff),
    };
  let withInverse = setInverse(true);
  let setHidden: (bool, style) => style =
    (hidden, style) => {
      ...style,
      hidden: Some(hidden ? Hidden : HiddenOff),
    };
  let withHidden = setHidden(true);
  let setStrikethrough: (bool, style) => style =
    (strikethrough, style) => {
      ...style,
      strikethrough: Some(strikethrough ? Strikethrough : StrikethroughOff),
    };
  let withStrikethrough = setStrikethrough(true);

  let getColor: style => option(colorName) =
    style =>
      switch (style.foreground) {
      | None => None
      | Some(color) =>
        switch (color) {
        | Default => None
        | Black => Some(Black)
        | Red => Some(Red)
        | Green => Some(Green)
        | Yellow => Some(Yellow)
        | Blue => Some(Blue)
        | Magenta => Some(Magenta)
        | Cyan => Some(Cyan)
        | White => Some(White)
        | BrightBlack => Some(BlackBright)
        | BrightRed => Some(RedBright)
        | BrightGreen => Some(GreenBright)
        | BrightYellow => Some(YellowBright)
        | BrightBlue => Some(BlueBright)
        | BrightMagenta => Some(MagentaBright)
        | BrightCyan => Some(CyanBright)
        | BrightWhite => Some(WhiteBright)
        }
      };
  let getBackgroundColor: style => option(colorName) =
    style =>
      switch (style.background) {
      | None => None
      | Some(color) =>
        switch (color) {
        | Default => None
        | Black => Some(Black)
        | Red => Some(Red)
        | Green => Some(Green)
        | Yellow => Some(Yellow)
        | Blue => Some(Blue)
        | Magenta => Some(Magenta)
        | Cyan => Some(Cyan)
        | White => Some(White)
        | BrightBlack => Some(BlackBright)
        | BrightRed => Some(RedBright)
        | BrightGreen => Some(GreenBright)
        | BrightYellow => Some(YellowBright)
        | BrightBlue => Some(BlueBright)
        | BrightMagenta => Some(MagentaBright)
        | BrightCyan => Some(CyanBright)
        | BrightWhite => Some(WhiteBright)
        }
      };

  let isBold: style => bool =
    style =>
      switch (style.bold) {
      | Some(Bold) => true
      | _ => false
      };

  let isDim: style => bool =
    style =>
      switch (style.dim) {
      | Some(Dim) => true
      | _ => false
      };

  let isItalic: style => bool =
    style =>
      switch (style.italic) {
      | Some(Italic) => true
      | _ => false
      };

  let isUnderline: style => bool =
    style =>
      switch (style.underline) {
      | Some(Underline) => true
      | _ => false
      };

  let isInverse: style => bool =
    style =>
      switch (style.inverse) {
      | Some(Inverse) => true
      | _ => false
      };

  let isHidden: style => bool =
    style =>
      switch (style.hidden) {
      | Some(Hidden) => true
      | _ => false
      };

  let isStrikethrough: style => bool =
    style =>
      switch (style.strikethrough) {
      | Some(Strikethrough) => true
      | _ => false
      };

  let emptyStyle = StateMachine.initialState;
  let parse = s => selectedImplementation^.parse(s);
  let apply = parts => selectedImplementation^.apply(parts);
};