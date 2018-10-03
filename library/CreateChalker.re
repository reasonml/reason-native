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
  | _ =>
    raise(
      ParsingError(
        "Unexpected sequence of tokens while parsing string for Chalk",
      ),
    )
  };

let createChalker = (style: Ansi.style, str) =>
  reduceTokens(style, "", tokenize(str));
