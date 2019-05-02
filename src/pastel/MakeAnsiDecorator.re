/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

let makeCodesRegex = codes => {
  let start = "\027\\[";
  let codesExpr = String.concat("|", List.map(string_of_int, codes));
  let stop = "m";
  let regexString =
    String.concat("", ["(", start, "(", codesExpr, ")", stop, ")", "+"]);
  Re.Pcre.regexp(regexString);
};

let startCodesRegex = makeCodesRegex(Ansi.IntSet.elements(Ansi.starts));
let stopCodesRegex = makeCodesRegex(Ansi.IntSet.elements(Ansi.stops));

type token =
  | Starts(string)
  | Stops(string)
  | Text(string);

let tokenize = (s: string): list(token) => {
  let parseNonStarts = nonStarts =>
    Re.split_full(stopCodesRegex, nonStarts)
    |> List.map(nonStartSplitResult =>
         switch (nonStartSplitResult) {
         | `Delim(g) => Stops(Re.Group.get(g, 0))
         | `Text(text) => Text(text)
         }
       );

  s
  |> Re.split_full(startCodesRegex)
  |> List.fold_left(
       (tokens, splitResult) =>
         switch (splitResult) {
         | `Delim(g) => tokens @ [Starts(Re.Group.get(g, 0))]
         | `Text(nonStarts) => tokens @ parseNonStarts(nonStarts)
         },
       [],
     );
};

let printTokens = tokens => {
  let tokenStr =
    List.map(
      token =>
        switch (token) {
        | Starts(starts) => "Starts(" ++ String.escaped(starts) ++ ")"
        | Stops(stops) => "Stops(" ++ String.escaped(stops) ++ ")"
        | Text(text) => "Text(" ++ String.escaped(text) ++ ")"
        },
      tokens,
    )
    |> String.concat(", ");

  print_endline("tokens: [" ++ tokenStr ++ "]");
};

exception ParsingError(string);

let rec reduceTokens = (style: Ansi.style, str, tokens) =>
  switch (tokens) {
  | [] => str
  | [Starts(starts), Text(text), Stops(stops), ...rest] =>
    reduceTokens(
      style,
      String.concat("", [str, style.start, starts, text, stops, style.stop]),
      rest,
    )
  | [Text(text), ...rest] =>
    reduceTokens(
      style,
      String.concat("", [str, style.start, text, style.stop]),
      rest,
    )
  /* This case should only be hit when the user's input is a string containing
   * ANSI escape sequences that Pastel also uses */
  | [Starts(escapeCodes) | Stops(escapeCodes), Text(text), ...rest] =>
    reduceTokens(
      style,
      String.concat(
        "",
        /* persist input escape codes by applying them to the inner text,
         * and then also after the stop code in case the stop code
         * for the style conflicts with the escape codes from user input */
        [str, style.start, escapeCodes, text, style.stop, escapeCodes],
      ),
      rest,
    )
  /* This case should only be hit when the user's input
   * is a string containing ANSI escape sequences that Pastel also uses */
  | [Starts(escapeCodes) | Stops(escapeCodes), ...rest] =>
    reduceTokens(style, String.concat("", [str, escapeCodes]), rest)
  };

let makeDecorator = (style: Ansi.style, str) =>
  reduceTokens(style, "", tokenize(str));
