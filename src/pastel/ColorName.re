/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
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
