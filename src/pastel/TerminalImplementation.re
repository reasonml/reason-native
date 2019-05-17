/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open MakeAnsiDecorator;
open Decorators;

WinConsoleColorsSupport.enable()

let ansiLengthRegex = {
  let start = "\027\\[";
  let middle = "[0-9]+";
  let stop = "m";
  Re.Pcre.regexp(start ++ middle ++ stop);
};

let length = s => {
  let parts = Re.split(ansiLengthRegex, s);
  let noColor = String.concat("", parts);
  String.length(noColor);
};

let modifier: modifier = {
  bold: makeDecorator(Ansi.modifier.bold),
  dim: makeDecorator(Ansi.modifier.dim),
  italic: makeDecorator(Ansi.modifier.italic),
  underline: makeDecorator(Ansi.modifier.underline),
  inverse: makeDecorator(Ansi.modifier.inverse),
  hidden: makeDecorator(Ansi.modifier.hidden),
  strikethrough: makeDecorator(Ansi.modifier.strikethrough),
};

let color: color = {
  black: makeDecorator(Ansi.color.black),
  red: makeDecorator(Ansi.color.red),
  green: makeDecorator(Ansi.color.green),
  yellow: makeDecorator(Ansi.color.yellow),
  blue: makeDecorator(Ansi.color.blue),
  magenta: makeDecorator(Ansi.color.magenta),
  cyan: makeDecorator(Ansi.color.cyan),
  white: makeDecorator(Ansi.color.white),
  blackBright: makeDecorator(Ansi.color.blackBright),
  redBright: makeDecorator(Ansi.color.redBright),
  greenBright: makeDecorator(Ansi.color.greenBright),
  yellowBright: makeDecorator(Ansi.color.yellowBright),
  blueBright: makeDecorator(Ansi.color.blueBright),
  magentaBright: makeDecorator(Ansi.color.magentaBright),
  cyanBright: makeDecorator(Ansi.color.cyanBright),
  whiteBright: makeDecorator(Ansi.color.whiteBright),
};

let bg: color = {
  black: makeDecorator(Ansi.bg.black),
  red: makeDecorator(Ansi.bg.red),
  green: makeDecorator(Ansi.bg.green),
  yellow: makeDecorator(Ansi.bg.yellow),
  blue: makeDecorator(Ansi.bg.blue),
  magenta: makeDecorator(Ansi.bg.magenta),
  cyan: makeDecorator(Ansi.bg.cyan),
  white: makeDecorator(Ansi.bg.white),
  blackBright: makeDecorator(Ansi.bg.blackBright),
  redBright: makeDecorator(Ansi.bg.redBright),
  greenBright: makeDecorator(Ansi.bg.greenBright),
  yellowBright: makeDecorator(Ansi.bg.yellowBright),
  blueBright: makeDecorator(Ansi.bg.blueBright),
  magentaBright: makeDecorator(Ansi.bg.magentaBright),
  cyanBright: makeDecorator(Ansi.bg.cyanBright),
  whiteBright: makeDecorator(Ansi.bg.whiteBright),
};
