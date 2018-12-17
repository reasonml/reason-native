/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

type decorator = string => string;

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

type modifier = {
  bold: decorator,
  dim: decorator,
  italic: decorator,
  underline: decorator,
  inverse: decorator,
  hidden: decorator,
  strikethrough: decorator,
};

let length: string => int;

let modifier: modifier;
let color: color;
let bg: color;

let bold: decorator;
let dim: decorator;
let italic: decorator;
let underline: decorator;
let inverse: decorator;
let hidden: decorator;
let strikethrough: decorator;

let black: decorator;
let red: decorator;
let green: decorator;
let yellow: decorator;
let blue: decorator;
let magenta: decorator;
let cyan: decorator;
let white: decorator;
let blackBright: decorator;
let redBright: decorator;
let greenBright: decorator;
let yellowBright: decorator;
let blueBright: decorator;
let magentaBright: decorator;
let cyanBright: decorator;
let whiteBright: decorator;

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
