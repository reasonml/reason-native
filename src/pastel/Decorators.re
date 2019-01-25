/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type decorator = string => string;

type modifier = {
  bold: decorator,
  dim: decorator,
  italic: decorator,
  underline: decorator,
  inverse: decorator,
  hidden: decorator,
  strikethrough: decorator,
};

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
