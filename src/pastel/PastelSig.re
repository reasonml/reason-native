/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Mode;
open Decorators;
open ColorName;

module type PastelSig = {
  let getMode: unit => mode;
  let setMode: mode => unit;
  let useMode: (mode, unit => 'a) => 'a;
  let length: string => int;
  /* probably don't want to release this*/
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
  
  let getColor: style => option(ColorName.colorName);
  let getBackgroundColor: style => option(ColorName.colorName);
  let isBold: style => bool;
  let isDim: style => bool;
  let isItalic: style => bool;
  let isUnderline: style => bool;
  let isInverse: style => bool;
  let isHidden: style => bool;
  let isStrikethrough: style => bool;

  let parse: string => list((style, string));
  let apply: list((style, string)) => string;

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
};