/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open MakeDecorator;

type decorator = string => string;

type modifier = {
  bold: decorator,
  dim: decorator,
  italic: decorator,
  underline: decorator,
  inverse: decorator,
  hidden: decorator,
  strikethrough: decorator,
};

type color = {
  black: decorator,
  red: decorator,
  green: decorator,
  yellow: decorator,
  blue: decorator,
  magenta: decorator,
  cyan: decorator,
  white: decorator,
  blackBright: decorator,
  redBright: decorator,
  greenBright: decorator,
  yellowBright: decorator,
  blueBright: decorator,
  magentaBright: decorator,
  cyanBright: decorator,
  whiteBright: decorator,
};

let modifier: modifier = {
  bold: makeDecorator(Ansi.modifier.bold),
  dim: makeDecorator(Ansi.modifier.dim),
  italic: makeDecorator(Ansi.modifier.italic),
  underline: makeDecorator(Ansi.modifier.underline),
  inverse: makeDecorator(Ansi.modifier.inverse),
  hidden: makeDecorator(Ansi.modifier.hidden),
  strikethrough: makeDecorator(Ansi.modifier.strikethrough),
};

let bold = modifier.bold;

let dim = modifier.dim;

let italic = modifier.italic;

let underline = modifier.underline;

let inverse = modifier.inverse;

let hidden = modifier.hidden;

let strikethrough = modifier.strikethrough;

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

let lengthRegex = {
  let start = "\027\\[";
  let middle = "[0-9]+";
  let stop = "m";
  Str.regexp(start ++ middle ++ stop);
};

let length = (s: string): int => {
  let parts = Str.split(lengthRegex, s);
  let noColor = String.concat("", parts);
  String.length(noColor);
};

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

type colorName =
  | Black
  | Red
  | Green
  | Yellow
  | Blue
  | Magenta
  | Cyan
  | White
  /* Gray is actually "Bright Black":
   * https://en.wikipedia.org/wiki/ANSI_escape_code */
  | BlackBright
  | RedBright
  | GreenBright
  | YellowBright
  | BlueBright
  | MagentaBright
  | CyanBright
  | WhiteBright;

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
