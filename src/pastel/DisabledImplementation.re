/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Decorators;

let length = String.length;

let identityDecorator = s => s;

let partition = (index, s) =>
  if (index < 0) {
    ("", s);
  } else if (index > String.length(s)) {
    (s, "");
  } else {
    (
      String.sub(s, 0, index),
      String.sub(s, index, String.length(s) - index),
    );
  };

let unformattedText = s => s;

let modifier: modifier = {
  bold: identityDecorator,
  dim: identityDecorator,
  italic: identityDecorator,
  underline: identityDecorator,
  inverse: identityDecorator,
  hidden: identityDecorator,
  strikethrough: identityDecorator,
};

let color: color = {
  black: identityDecorator,
  red: identityDecorator,
  green: identityDecorator,
  yellow: identityDecorator,
  blue: identityDecorator,
  magenta: identityDecorator,
  cyan: identityDecorator,
  white: identityDecorator,
  blackBright: identityDecorator,
  redBright: identityDecorator,
  greenBright: identityDecorator,
  yellowBright: identityDecorator,
  blueBright: identityDecorator,
  magentaBright: identityDecorator,
  cyanBright: identityDecorator,
  whiteBright: identityDecorator,
};

let bg: color = {
  black: identityDecorator,
  red: identityDecorator,
  green: identityDecorator,
  yellow: identityDecorator,
  blue: identityDecorator,
  magenta: identityDecorator,
  cyan: identityDecorator,
  white: identityDecorator,
  blackBright: identityDecorator,
  redBright: identityDecorator,
  greenBright: identityDecorator,
  yellowBright: identityDecorator,
  blueBright: identityDecorator,
  magentaBright: identityDecorator,
  cyanBright: identityDecorator,
  whiteBright: identityDecorator,
};

let implementation: PastelImplementation.t = {
  modifier,
  color,
  bg,
  length,
  unformattedText: identityDecorator,
  partition,
  make:
    (
      ~reset: option(bool)=?,
      ~bold: option(bool)=?,
      ~dim: option(bool)=?,
      ~italic: option(bool)=?,
      ~underline: option(bool)=?,
      ~inverse: option(bool)=?,
      ~hidden: option(bool)=?,
      ~strikethrough: option(bool)=?,
      ~color: option(ColorName.colorName)=?,
      ~backgroundColor: option(ColorName.colorName)=?,
      inputs,
    ) =>
    String.concat("", inputs),
  emptyStyle: StateMachine.initialState,
  parse: s => [(StateMachine.initialState, s)],
  apply: s =>
    s |> List.map(((style, text)) => text) |> String.concat(""),
};
