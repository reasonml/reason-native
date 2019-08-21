/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

let optionMap = (o, f) => {
  switch (o) {
  | None => None
  | Some(x) => Some(f(x))
  };
};

let colorNameToColor: ColorName.colorName => Token.color =
  color =>
    switch (color) {
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
    };