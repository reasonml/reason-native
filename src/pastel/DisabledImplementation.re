/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open MakeAnsiDecorator;
open Decorators;

let length = String.length;

let identityDecorator = s => s;

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
