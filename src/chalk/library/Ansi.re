/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
/**
 * See this Wikipedia article: https://en.wikipedia.org/wiki/ANSI_escape_code
 */
module IntMap =
  Map.Make({
    type t = int;
    let compare = compare;
  });

module IntSet =
  Set.Make({
    type t = int;
    let compare = compare;
  });

type style = {
  start: string,
  startCode: int,
  stop: string,
  stopCode: int,
};

let createStyle = (start: int, stop: int): style => {
  start: "\027[" ++ string_of_int(start) ++ "m",
  startCode: start,
  stop: "\027[" ++ string_of_int(stop) ++ "m",
  stopCode: stop,
};

type modifier = {
  reset: string,
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

let modifier: modifier = {
  reset: "\027[0m",
  /* 21 isn't widely supported and 22 does the same thing */
  bold: createStyle(1, 22),
  dim: createStyle(2, 22),
  italic: createStyle(3, 23),
  underline: createStyle(4, 24),
  inverse: createStyle(7, 27),
  hidden: createStyle(8, 28),
  strikethrough: createStyle(9, 29),
};

let color: color = {
  /* Stops using a foreground color and resumes the default text color. */
  stop: "\027[39m",
  black: createStyle(30, 39),
  red: createStyle(31, 39),
  green: createStyle(32, 39),
  yellow: createStyle(33, 39),
  blue: createStyle(34, 39),
  magenta: createStyle(35, 39),
  cyan: createStyle(36, 39),
  white: createStyle(37, 39),
  blackBright: createStyle(90, 39),
  redBright: createStyle(91, 39),
  greenBright: createStyle(92, 39),
  yellowBright: createStyle(93, 39),
  blueBright: createStyle(94, 39),
  magentaBright: createStyle(95, 39),
  cyanBright: createStyle(96, 39),
  whiteBright: createStyle(97, 39),
};

let bg: color = {
  /* Stops using a background color and resumes the default background. */
  stop: "\027[49m",
  black: createStyle(40, 49),
  red: createStyle(41, 49),
  green: createStyle(42, 49),
  yellow: createStyle(43, 49),
  blue: createStyle(44, 49),
  magenta: createStyle(45, 49),
  cyan: createStyle(46, 49),
  white: createStyle(47, 49),
  blackBright: createStyle(100, 49),
  redBright: createStyle(101, 49),
  greenBright: createStyle(102, 49),
  yellowBright: createStyle(103, 49),
  blueBright: createStyle(104, 49),
  magentaBright: createStyle(105, 49),
  cyanBright: createStyle(106, 49),
  whiteBright: createStyle(107, 49),
};
/**
   * All start codes.
   */
let starts: IntSet.t =
  IntSet.of_list([
    1,
    2,
    3,
    4,
    7,
    8,
    9,
    /* colors */
    30,
    31,
    32,
    33,
    34,
    35,
    36,
    37,
    90,
    91,
    92,
    93,
    94,
    95,
    96,
    97,
    /* bg colors */
    40,
    41,
    42,
    43,
    44,
    45,
    46,
    47,
    100,
    101,
    102,
    103,
    104,
    105,
    106,
    107,
  ]);
/**
   * All stop codes.
   */
let stops: IntSet.t = IntSet.of_list([0, 22, 23, 24, 27, 28, 29, 39, 49]);
/**
   * This is a mapping of stopCode => set of startCodes that use that stopCode.
   *
   * This can be helpful for determining whether or not a particular startCode is
   * still affecting a part of the string when manually parsing the string.
   */
let stopToStarts: IntMap.t(IntSet.t) =
  IntMap.empty
  |> IntMap.add(0, starts)
  |> IntMap.add(22, IntSet.of_list([1, 2]))
  |> IntMap.add(23, IntSet.of_list([3]))
  |> IntMap.add(24, IntSet.of_list([4]))
  |> IntMap.add(27, IntSet.of_list([7]))
  |> IntMap.add(28, IntSet.of_list([8]))
  |> IntMap.add(29, IntSet.of_list([9]))
  |> IntMap.add(
       39,
       IntSet.of_list([
         30,
         31,
         32,
         33,
         34,
         35,
         36,
         37,
         90,
         91,
         92,
         93,
         94,
         95,
         96,
         97,
       ]),
     )
  |> IntMap.add(
       49,
       IntSet.of_list([
         40,
         41,
         42,
         43,
         44,
         45,
         46,
         100,
         101,
         102,
         103,
         104,
         105,
         106,
         107,
       ]),
     );
