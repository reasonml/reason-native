/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

let makeCodesRegex = codes => {
  let start = "\027\\[";
  let openParen = "\\(";
  let codesList = codes;
  let codesExpr =
    List.fold_left(
      (s, code) => String.concat("", [s, "\\|", string_of_int(code)]),
      string_of_int(List.hd(codesList)),
      List.tl(codesList),
    );
  let closeParen = "\\)";
  let stop = "m";
  let regexString =
    String.concat(
      "",
      [
        openParen,
        start,
        openParen,
        codesExpr,
        closeParen,
        stop,
        closeParen,
        "+",
      ],
    );
  Str.regexp(regexString);
};

let startCodesRegex = makeCodesRegex(Ansi.IntSet.elements(Ansi.starts));
let stopCodesRegex = makeCodesRegex(Ansi.IntSet.elements(Ansi.stops));

type token =
  | Starts(string)
  | Stops(string)
  | Text(string);

let tokenize = (s: string): list(token) => {
  let parseNonStarts = nonStarts =>
    Str.full_split(stopCodesRegex, nonStarts)
    |> List.map(nonStartSplitResult =>
         switch (nonStartSplitResult) {
         | Str.Delim(stops) => Stops(stops)
         | Str.Text(text) => Text(text)
         }
       );

  s
  |> Str.full_split(startCodesRegex)
  |> List.fold_left(
       (tokens, splitResult) =>
         switch (splitResult) {
         | Str.Delim(starts) => tokens @ [Starts(starts)]
         | Str.Text(nonStarts) => tokens @ parseNonStarts(nonStarts)
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
   * ANSI escape sequences that Chalk also uses */
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
   * is a string containing ANSI escape sequences that Chalk also uses */
  | [Starts(escapeCodes) | Stops(escapeCodes), ...rest] =>
    reduceTokens(style, String.concat("", [str, escapeCodes]), rest)
  };

let createChalker = (style: Ansi.style, str) =>
  reduceTokens(style, "", tokenize(str));
