/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

type style = {
  start: string,
  stop: string,
};

type modifier = {
  reset: style,
  bold: style,
  dim: style,
  italic: style,
  underline: style,
  inverse: style,
  hidden: style,
  strikethrough: style,
};

/**
   * Codes for a particular usage of each of the eight colors, as well as their
   * bright variants, and as well as the "stop" code.
   */
type color = {
  stop: string,
  /* Normal colors */
  black: style,
  red: style,
  green: style,
  yellow: style,
  blue: style,
  magenta: style,
  cyan: style,
  white: style,
  /* Bright colors */
  blackBright: style,
  redBright: style,
  greenBright: style,
  yellowBright: style,
  blueBright: style,
  magentaBright: style,
  cyanBright: style,
  whiteBright: style,
};

let makeStyle = name => {
  let start = String.concat("", ["<", name, ">"]);
  let stop = String.concat("", ["</", name, ">"]);
  {start, stop};
};

let modifier: modifier = {
  bold: {
      start: "<bold>",
      stop: "</resetDimAndBold>"
  },
  dim: {
    start: "<dim>",
    stop: "</resetDimAndBold>"
},
  italic: makeStyle("italic"),
  underline: makeStyle("underline"),
  inverse: makeStyle("inverse"),
  hidden: makeStyle("hidden"),
  strikethrough: makeStyle("strikethrough"),
  reset: {
    start: "<reset>",
    stop: "</reset>"
  }
};

let color: color = {
  stop: "</resetColor>",
  black: makeStyle("black"),
  red: makeStyle("red"),
  green: makeStyle("green"),
  yellow: makeStyle("yellow"),
  blue: makeStyle("blue"),
  magenta: makeStyle("magenta"),
  cyan: makeStyle("cyan"),
  white: makeStyle("white"),
  blackBright: makeStyle("blackBright"),
  redBright: makeStyle("redBright"),
  greenBright: makeStyle("greenBright"),
  yellowBright: makeStyle("yellowBright"),
  blueBright: makeStyle("blueBright"),
  magentaBright: makeStyle("magentaBright"),
  cyanBright: makeStyle("cyanBright"),
  whiteBright: makeStyle("whiteBright"),
};

let bg: color = {
  stop: "</resetBgColor>",
  black: makeStyle("blackBg"),
  red: makeStyle("redBg"),
  green: makeStyle("greenBg"),
  yellow: makeStyle("yellowBg"),
  blue: makeStyle("blueBg"),
  magenta: makeStyle("magentaBg"),
  cyan: makeStyle("cyanBg"),
  white: makeStyle("whiteBg"),
  blackBright: makeStyle("blackBrightBg"),
  redBright: makeStyle("redBrightBg"),
  greenBright: makeStyle("greenBrightBg"),
  yellowBright: makeStyle("yellowBrightBg"),
  blueBright: makeStyle("blueBrightBg"),
  magentaBright: makeStyle("magentaBrightBg"),
  cyanBright: makeStyle("cyanBrightBg"),
  whiteBright: makeStyle("whiteBrightBg"),
};
