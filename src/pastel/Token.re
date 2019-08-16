/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

type color =
  | Default
  | Black
  | Red
  | Green
  | Yellow
  | Blue
  | Magenta
  | Cyan
  | White
  | BrightBlack
  | BrightRed
  | BrightGreen
  | BrightYellow
  | BrightBlue
  | BrightMagenta
  | BrightCyan
  | BrightWhite;

type escapeSequence =
  | Foreground(color)
  | Background(color)
  | Bold
  | Dim
  | NormalIntensity
  | Italic
  | ItalicOff
  | Underline
  | UnderlineOff
  | Inverse
  | InverseOff
  | Hidden
  | HiddenOff
  | Strikethrough
  | StrikethroughOff
  | Reset;

type t =
  | Text(string)
  | EscapeSequence(escapeSequence);