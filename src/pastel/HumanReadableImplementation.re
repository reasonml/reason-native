/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Decorators;

module HumanReadableLexer =
  PastelLexer.Make({
    let startRegex = Re.Pcre.regexp("<[a-z|A-Z]+>");
    let stopRegex = Re.Pcre.regexp("</[a-z|A-Z]+>");
  });

let length = HumanReadableLexer.length;
let partition = HumanReadableLexer.partition;
let unformattedText = HumanReadableLexer.unformattedText;

let makeDecorator = name => {
  let start = String.concat("", ["<", name, ">"]);
  let stop = String.concat("", ["</", name, ">"]);
  let decorator = s => String.concat("", [start, s, stop]);
  decorator;
};

let modifier: modifier = {
  bold: makeDecorator("bold"),
  dim: makeDecorator("dim"),
  italic: makeDecorator("italic"),
  underline: makeDecorator("underline"),
  inverse: makeDecorator("inverse"),
  hidden: makeDecorator("hidden"),
  strikethrough: makeDecorator("strikethrough"),
};

let color: color = {
  black: makeDecorator("black"),
  red: makeDecorator("red"),
  green: makeDecorator("green"),
  yellow: makeDecorator("yellow"),
  blue: makeDecorator("blue"),
  magenta: makeDecorator("magenta"),
  cyan: makeDecorator("cyan"),
  white: makeDecorator("white"),
  blackBright: makeDecorator("blackBright"),
  redBright: makeDecorator("redBright"),
  greenBright: makeDecorator("greenBright"),
  yellowBright: makeDecorator("yellowBright"),
  blueBright: makeDecorator("blueBright"),
  magentaBright: makeDecorator("magentaBright"),
  cyanBright: makeDecorator("cyanBright"),
  whiteBright: makeDecorator("whiteBright"),
};

let bg: color = {
  black: makeDecorator("black"),
  red: makeDecorator("red"),
  green: makeDecorator("green"),
  yellow: makeDecorator("yellow"),
  blue: makeDecorator("blue"),
  magenta: makeDecorator("magenta"),
  cyan: makeDecorator("cyan"),
  white: makeDecorator("white"),
  blackBright: makeDecorator("blackBright"),
  redBright: makeDecorator("redBright"),
  greenBright: makeDecorator("greenBright"),
  yellowBright: makeDecorator("yellowBright"),
  blueBright: makeDecorator("blueBright"),
  magentaBright: makeDecorator("magentaBright"),
  cyanBright: makeDecorator("cyanBright"),
  whiteBright: makeDecorator("whiteBright"),
};

/* module HumanReadableStateMachine = StateMachine.Make({
  let tokenize = HumanReadableLexer.tokenize;
  let startTokensToInstructions = starts => [];
  let stopTokensToInstructions = stops => [];
  let applyModifier = (modifier, s) => String.concat("", [modifier, s]);
  let applyStateDiff = (a, s) => s;
}) */
