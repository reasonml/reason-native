/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open CreateChalker;

type chalker = string => string;

type modifier = {
  bold: chalker,
  dim: chalker,
  italic: chalker,
  underline: chalker,
  inverse: chalker,
  hidden: chalker,
  strikethrough: chalker,
};

type color = {
  black: chalker,
  red: chalker,
  green: chalker,
  yellow: chalker,
  blue: chalker,
  magenta: chalker,
  cyan: chalker,
  white: chalker,
  blackBright: chalker,
  redBright: chalker,
  greenBright: chalker,
  yellowBright: chalker,
  blueBright: chalker,
  magentaBright: chalker,
  cyanBright: chalker,
  whiteBright: chalker,
};

let modifier: modifier = {
  bold: createChalker(Ansi.modifier.bold),
  dim: createChalker(Ansi.modifier.dim),
  italic: createChalker(Ansi.modifier.italic),
  underline: createChalker(Ansi.modifier.underline),
  inverse: createChalker(Ansi.modifier.inverse),
  hidden: createChalker(Ansi.modifier.hidden),
  strikethrough: createChalker(Ansi.modifier.strikethrough),
};

let bold = modifier.bold;

let dim = modifier.dim;

let italic = modifier.italic;

let underline = modifier.underline;

let inverse = modifier.inverse;

let hidden = modifier.hidden;

let strikethrough = modifier.strikethrough;

let color: color = {
  black: createChalker(Ansi.color.black),
  red: createChalker(Ansi.color.red),
  green: createChalker(Ansi.color.green),
  yellow: createChalker(Ansi.color.yellow),
  blue: createChalker(Ansi.color.blue),
  magenta: createChalker(Ansi.color.magenta),
  cyan: createChalker(Ansi.color.cyan),
  white: createChalker(Ansi.color.white),
  blackBright: createChalker(Ansi.color.blackBright),
  redBright: createChalker(Ansi.color.redBright),
  greenBright: createChalker(Ansi.color.greenBright),
  yellowBright: createChalker(Ansi.color.yellowBright),
  blueBright: createChalker(Ansi.color.blueBright),
  magentaBright: createChalker(Ansi.color.magentaBright),
  cyanBright: createChalker(Ansi.color.cyanBright),
  whiteBright: createChalker(Ansi.color.whiteBright),
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
  black: createChalker(Ansi.bg.black),
  red: createChalker(Ansi.bg.red),
  green: createChalker(Ansi.bg.green),
  yellow: createChalker(Ansi.bg.yellow),
  blue: createChalker(Ansi.bg.blue),
  magenta: createChalker(Ansi.bg.magenta),
  cyan: createChalker(Ansi.bg.cyan),
  white: createChalker(Ansi.bg.white),
  blackBright: createChalker(Ansi.bg.blackBright),
  redBright: createChalker(Ansi.bg.redBright),
  greenBright: createChalker(Ansi.bg.greenBright),
  yellowBright: createChalker(Ansi.bg.yellowBright),
  blueBright: createChalker(Ansi.bg.blueBright),
  magentaBright: createChalker(Ansi.bg.magentaBright),
  cyanBright: createChalker(Ansi.bg.cyanBright),
  whiteBright: createChalker(Ansi.bg.whiteBright),
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
  let chalkers =
    switch (color) {
    | None => []
    | Some(colorName) => [colorNameToForeground(colorName)]
    };
  let chalkers =
    switch (backgroundColor) {
    | None => chalkers
    | Some(colorName) => [colorNameToBackground(colorName), ...chalkers]
    };
  let chalkers = bold ? [modifier.bold, ...chalkers] : chalkers;
  let chalkers = dim ? [modifier.dim, ...chalkers] : chalkers;
  let chalkers = italic ? [modifier.italic, ...chalkers] : chalkers;
  let chalkers = underline ? [modifier.underline, ...chalkers] : chalkers;
  let chalkers = inverse ? [modifier.inverse, ...chalkers] : chalkers;
  let chalkers = hidden ? [modifier.hidden, ...chalkers] : chalkers;
  let chalkers =
    strikethrough ? [modifier.strikethrough, ...chalkers] : chalkers;
  let childrenStr = String.concat("", children);
  List.fold_left(
    (curText, nextChalker) => nextChalker(curText),
    childrenStr,
    chalkers,
  );
};
