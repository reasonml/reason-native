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
  let modifierInternal = ref(TerminalImplementation.modifier);
  let colorInternal = ref(TerminalImplementation.color);
  let bgInternal = ref(TerminalImplementation.bg);

  let setMode = m => {
    switch (m) {
    | Terminal =>
      modifierInternal := TerminalImplementation.modifier;
      colorInternal := TerminalImplementation.color;
      bgInternal := TerminalImplementation.bg;
    | Disabled =>
      modifierInternal := DisabledImplementation.modifier;
      colorInternal := DisabledImplementation.color;
      bgInternal := DisabledImplementation.bg;
    | HumanReadable =>
      modifierInternal := HumanReadableImplementation.modifier;
      colorInternal := HumanReadableImplementation.color;
      bgInternal := HumanReadableImplementation.bg;
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
    bold: s => modifierInternal^.bold(s),
    dim: s => modifierInternal^.dim(s),
    italic: s => modifierInternal^.italic(s),
    underline: s => modifierInternal^.underline(s),
    inverse: s => modifierInternal^.inverse(s),
    hidden: s => modifierInternal^.hidden(s),
    strikethrough: s => modifierInternal^.strikethrough(s),
  };

  let color = {
    black: s => colorInternal^.black(s),
    red: s => colorInternal^.red(s),
    green: s => colorInternal^.green(s),
    yellow: s => colorInternal^.yellow(s),
    blue: s => colorInternal^.blue(s),
    magenta: s => colorInternal^.magenta(s),
    cyan: s => colorInternal^.cyan(s),
    white: s => colorInternal^.white(s),
    blackBright: s => colorInternal^.blackBright(s),
    redBright: s => colorInternal^.redBright(s),
    greenBright: s => colorInternal^.greenBright(s),
    yellowBright: s => colorInternal^.yellowBright(s),
    blueBright: s => colorInternal^.blueBright(s),
    magentaBright: s => colorInternal^.magentaBright(s),
    cyanBright: s => colorInternal^.cyanBright(s),
    whiteBright: s => colorInternal^.whiteBright(s),
  };
  let bg = {
    black: s => bgInternal^.black(s),
    red: s => bgInternal^.red(s),
    green: s => bgInternal^.green(s),
    yellow: s => bgInternal^.yellow(s),
    blue: s => bgInternal^.blue(s),
    magenta: s => bgInternal^.magenta(s),
    cyan: s => bgInternal^.cyan(s),
    white: s => bgInternal^.white(s),
    blackBright: s => bgInternal^.blackBright(s),
    redBright: s => bgInternal^.redBright(s),
    greenBright: s => bgInternal^.greenBright(s),
    yellowBright: s => bgInternal^.yellowBright(s),
    blueBright: s => bgInternal^.blueBright(s),
    magentaBright: s => bgInternal^.magentaBright(s),
    cyanBright: s => bgInternal^.cyanBright(s),
    whiteBright: s => bgInternal^.whiteBright(s),
  };

  let length = s =>
    switch (mode^) {
    | HumanReadable => HumanReadableImplementation.length(s)
    | Terminal => TerminalImplementation.length(s)
    | Disabled => DisabledImplementation.length(s)
    };

  let partition = s =>
    switch (mode^) {
    | HumanReadable => HumanReadableImplementation.partition(s)
    | Terminal => TerminalImplementation.partition(s)
    | Disabled => DisabledImplementation.partition(s)
    };

  let unformattedText = s =>
    switch (mode^) {
    | HumanReadable => HumanReadableImplementation.unformattedText(s)
    | Terminal => TerminalImplementation.unformattedText(s)
    | Disabled => DisabledImplementation.unformattedText(s)
    };

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

  let colorNameToBackground = n =>
    switch (n) {
    | Black => bg.black
    | Red => bg.red
    | Green => bg.green
    | Yellow => bg.yellow
    | Blue => bg.blue
    | Magenta => bg.magenta
    | Cyan => bg.cyan
    | White => bg.white
    | BlackBright => bg.blackBright
    | RedBright => bg.redBright
    | GreenBright => bg.greenBright
    | YellowBright => bg.yellowBright
    | BlueBright => bg.blueBright
    | MagentaBright => bg.magentaBright
    | CyanBright => bg.cyanBright
    | WhiteBright => bg.whiteBright
    };

  let colorNameToForeground = n =>
    switch (n) {
    | Black => color.black
    | Red => color.red
    | Green => color.green
    | Yellow => color.yellow
    | Blue => color.blue
    | Magenta => color.magenta
    | Cyan => color.cyan
    | White => color.white
    /* This color is missing for foreground. */
    | BlackBright => color.blackBright
    | RedBright => color.redBright
    | GreenBright => color.greenBright
    | YellowBright => color.yellowBright
    | BlueBright => color.blueBright
    | MagentaBright => color.magentaBright
    | CyanBright => color.cyanBright
    | WhiteBright => color.whiteBright
    };

  let createElement =
      (
        ~reset: bool=false,
        ~bold: bool=false,
        ~dim: bool=false,
        ~italic: bool=false,
        ~underline: bool=false,
        ~inverse: bool=false,
        ~hidden: bool=false,
        ~strikethrough: bool=false,
        ~color: option(colorName)=?,
        ~backgroundColor: option(colorName)=?,
        ~children: list(string),
        (),
      ) => {
    let pastels =
      switch (color) {
      | None => []
      | Some(colorName) => [colorNameToForeground(colorName)]
      };
    let pastels =
      switch (backgroundColor) {
      | None => pastels
      | Some(colorName) => [colorNameToBackground(colorName), ...pastels]
      };
    let pastels = bold ? [modifier.bold, ...pastels] : pastels;
    let pastels = dim ? [modifier.dim, ...pastels] : pastels;
    let pastels = italic ? [modifier.italic, ...pastels] : pastels;
    let pastels = underline ? [modifier.underline, ...pastels] : pastels;
    let pastels = inverse ? [modifier.inverse, ...pastels] : pastels;
    let pastels = hidden ? [modifier.hidden, ...pastels] : pastels;
    let pastels =
      strikethrough ? [modifier.strikethrough, ...pastels] : pastels;
    let childrenStr = String.concat("", children);
    List.fold_left(
      (curText, nextDecorataor) => nextDecorataor(curText),
      childrenStr,
      pastels,
    );
  };
};
