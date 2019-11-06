/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

module Make: () => PastelInternal.PastelSig.PastelSig;
module ColorName = PastelInternal.ColorName;

type mode =
  | Terminal
  | HumanReadable
  | Disabled;

let getMode: unit => mode;
let setMode: mode => unit;
let defaultMode: mode;
let useMode: (mode, unit => 'a) => 'a;

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
let partition: (int, string) => (string, string);
let unformattedText: string => string;

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

/** Pastel.style exposes a way to examine and manipulate the styles of existing text in
   * order to perform additional formatting operations. This feature is intended to be used primarily
   * by libraries (such as Frame). The style API itself is still subject to change until the parse/apply
   * API is finalized, however it is extremely unlikely to change */
type style;
let emptyStyle: style;

let withColor: (ColorName.colorName, style) => style;
let resetColor: style => style;
let withBackgroundColor: (ColorName.colorName, style) => style;
let resetBackgroundColor: style => style;

let setBold: (bool, style) => style;
let withBold: style => style;
let setDim: (bool, style) => style;
let withDim: style => style;
let setItalic: (bool, style) => style;
let withItalic: style => style;
let setUnderline: (bool, style) => style;
let withUnderline: style => style;
let setInverse: (bool, style) => style;
let withInverse: style => style;
let setHidden: (bool, style) => style;
let withHidden: style => style;
let setStrikethrough: (bool, style) => style;
let withStrikethrough: style => style;
let withReset: style => style;
let setReset: (bool, style) => style;

let getColor: style => option(ColorName.colorName);
let getBackgroundColor: style => option(ColorName.colorName);
let isBold: style => bool;
let isDim: style => bool;
let isItalic: style => bool;
let isUnderline: style => bool;
let isInverse: style => bool;
let isHidden: style => bool;
let isStrikethrough: style => bool;
let isReset: style => bool;
/** These functions are considered unstable and are subject to change
   * unstable_parse takes a (potentially stylized by Pastel) string, and breaks it into segments that share the same styles
   * unstable_apply takes a list of stylized segments and constructs a string that correctly displays those styles
  */
let unstable_parse: string => list((style, string));
let unstable_apply: list((style, string)) => string;
