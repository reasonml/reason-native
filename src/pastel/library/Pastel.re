/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open MakePastel;

type pastel = string => string;

type modifier = {
  bold: pastel,
  dim: pastel,
  italic: pastel,
  underline: pastel,
  inverse: pastel,
  hidden: pastel,
  strikethrough: pastel,
};

type color = {
  black: pastel,
  red: pastel,
  green: pastel,
  yellow: pastel,
  blue: pastel,
  magenta: pastel,
  cyan: pastel,
  white: pastel,
  blackBright: pastel,
  redBright: pastel,
  greenBright: pastel,
  yellowBright: pastel,
  blueBright: pastel,
  magentaBright: pastel,
  cyanBright: pastel,
  whiteBright: pastel,
};

let modifier: modifier = {
  bold: makePastel(Ansi.modifier.bold),
  dim: makePastel(Ansi.modifier.dim),
  italic: makePastel(Ansi.modifier.italic),
  underline: makePastel(Ansi.modifier.underline),
  inverse: makePastel(Ansi.modifier.inverse),
  hidden: makePastel(Ansi.modifier.hidden),
  strikethrough: makePastel(Ansi.modifier.strikethrough),
};

let bold = modifier.bold;

let dim = modifier.dim;

let italic = modifier.italic;

let underline = modifier.underline;

let inverse = modifier.inverse;

let hidden = modifier.hidden;

let strikethrough = modifier.strikethrough;

let color: color = {
  black: makePastel(Ansi.color.black),
  red: makePastel(Ansi.color.red),
  green: makePastel(Ansi.color.green),
  yellow: makePastel(Ansi.color.yellow),
  blue: makePastel(Ansi.color.blue),
  magenta: makePastel(Ansi.color.magenta),
  cyan: makePastel(Ansi.color.cyan),
  white: makePastel(Ansi.color.white),
  blackBright: makePastel(Ansi.color.blackBright),
  redBright: makePastel(Ansi.color.redBright),
  greenBright: makePastel(Ansi.color.greenBright),
  yellowBright: makePastel(Ansi.color.yellowBright),
  blueBright: makePastel(Ansi.color.blueBright),
  magentaBright: makePastel(Ansi.color.magentaBright),
  cyanBright: makePastel(Ansi.color.cyanBright),
  whiteBright: makePastel(Ansi.color.whiteBright),
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
  black: makePastel(Ansi.bg.black),
  red: makePastel(Ansi.bg.red),
  green: makePastel(Ansi.bg.green),
  yellow: makePastel(Ansi.bg.yellow),
  blue: makePastel(Ansi.bg.blue),
  magenta: makePastel(Ansi.bg.magenta),
  cyan: makePastel(Ansi.bg.cyan),
  white: makePastel(Ansi.bg.white),
  blackBright: makePastel(Ansi.bg.blackBright),
  redBright: makePastel(Ansi.bg.redBright),
  greenBright: makePastel(Ansi.bg.greenBright),
  yellowBright: makePastel(Ansi.bg.yellowBright),
  blueBright: makePastel(Ansi.bg.blueBright),
  magentaBright: makePastel(Ansi.bg.magentaBright),
  cyanBright: makePastel(Ansi.bg.cyanBright),
  whiteBright: makePastel(Ansi.bg.whiteBright),
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
    (curText, nextPastel) => nextPastel(curText),
    childrenStr,
    pastels,
  );
};
