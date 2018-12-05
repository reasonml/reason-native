/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

type pastel = string => string;

type colorName =
  | Black
  | Red
  | Green
  | Yellow
  | Blue
  | Magenta
  | Cyan
  | White
  | BlackBright
  | RedBright
  | GreenBright
  | YellowBright
  | BlueBright
  | MagentaBright
  | CyanBright
  | WhiteBright;

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

type modifier = {
  bold: pastel,
  dim: pastel,
  italic: pastel,
  underline: pastel,
  inverse: pastel,
  hidden: pastel,
  strikethrough: pastel,
};

let length: string => int;

let modifier: modifier;
let color: color;
let bg: color;

let bold: pastel;
let dim: pastel;
let italic: pastel;
let underline: pastel;
let inverse: pastel;
let hidden: pastel;
let strikethrough: pastel;

let black: pastel;
let red: pastel;
let green: pastel;
let yellow: pastel;
let blue: pastel;
let magenta: pastel;
let cyan: pastel;
let white: pastel;
let blackBright: pastel;
let redBright: pastel;
let greenBright: pastel;
let yellowBright: pastel;
let blueBright: pastel;
let magentaBright: pastel;
let cyanBright: pastel;
let whiteBright: pastel;

let createElement:
  (
    ~reset: bool=?,
    ~bold: bool=?,
    ~dim: bool=?,
    ~italic: bool=?,
    ~underline: bool=?,
    ~inverse: bool=?,
    ~hidden: bool=?,
    ~strikethrough: bool=?,
    ~color: colorName=?,
    ~backgroundColor: colorName=?,
    ~children: list(string),
    unit
  ) =>
  string;
