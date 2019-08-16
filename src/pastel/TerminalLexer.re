/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Token;

type terminalToken =
  | RawEscapeSequence(string)
  | RawText(string);

let makeCodesRegex = codes => {
  let start = "\027\\[";
  let codesExpr = String.concat("|", List.map(string_of_int, codes));
  let stop = "m";
  let regexString =
    String.concat("", ["(", start, "(", codesExpr, ")", stop, ")"]);
  Re.Pcre.regexp(regexString);
};

let escapeSequences = Ansi.IntSet.union(Ansi.starts, Ansi.stops);
let escapeSequenceRegex =
  makeCodesRegex(Ansi.IntSet.elements(escapeSequences));

let tokenize = (s: string): list(terminalToken) => {
  s
  |> Re.split_full(escapeSequenceRegex)
  |> List.map(r =>
       switch (r) {
       | `Delim(g) => RawEscapeSequence(Re.Group.get(g, 0))
       | `Text(t) => RawText(t)
       }
     );
};

let terminalTokenToToken = terminalToken => {
  switch (terminalToken) {
  | RawText(s) => Text(s)
  | RawEscapeSequence(token) =>
    EscapeSequence(
      switch (token) {
      | token when token == Ansi.color.stop => Foreground(Default)
      | token when token == Ansi.color.black.start => Foreground(Black)
      | token when token == Ansi.color.red.start => Foreground(Red)
      | token when token == Ansi.color.green.start => Foreground(Green)
      | token when token == Ansi.color.yellow.start => Foreground(Yellow)
      | token when token == Ansi.color.blue.start => Foreground(Blue)
      | token when token == Ansi.color.magenta.start => Foreground(Magenta)
      | token when token == Ansi.color.cyan.start => Foreground(Cyan)
      | token when token == Ansi.color.white.start => Foreground(White)
      | token when token == Ansi.color.blackBright.start =>
        Foreground(BrightBlack)
      | token when token == Ansi.color.redBright.start =>
        Foreground(BrightRed)
      | token when token == Ansi.color.greenBright.start =>
        Foreground(BrightGreen)
      | token when token == Ansi.color.yellowBright.start =>
        Foreground(BrightYellow)
      | token when token == Ansi.color.blueBright.start =>
        Foreground(BrightBlue)
      | token when token == Ansi.color.magentaBright.start =>
        Foreground(BrightMagenta)
      | token when token == Ansi.color.cyanBright.start =>
        Foreground(BrightCyan)
      | token when token == Ansi.color.whiteBright.start =>
        Foreground(BrightWhite)
      | token when token == Ansi.bg.stop => Background(Default)
      | token when token == Ansi.bg.blue.start => Background(Blue)
      | token when token == Ansi.bg.black.start => Background(Black)
      | token when token == Ansi.bg.red.start => Background(Red)
      | token when token == Ansi.bg.green.start => Background(Green)
      | token when token == Ansi.bg.yellow.start => Background(Yellow)
      | token when token == Ansi.bg.blue.start => Background(Blue)
      | token when token == Ansi.bg.magenta.start => Background(Magenta)
      | token when token == Ansi.bg.cyan.start => Background(Cyan)
      | token when token == Ansi.bg.white.start => Background(White)
      | token when token == Ansi.bg.blackBright.start =>
        Background(BrightBlack)
      | token when token == Ansi.bg.redBright.start => Background(BrightRed)
      | token when token == Ansi.bg.greenBright.start =>
        Background(BrightGreen)
      | token when token == Ansi.bg.yellowBright.start =>
        Background(BrightYellow)
      | token when token == Ansi.bg.blueBright.start => Background(BrightBlue)
      | token when token == Ansi.bg.magentaBright.start =>
        Background(BrightMagenta)
      | token when token == Ansi.bg.cyanBright.start => Background(BrightCyan)
      | token when token == Ansi.bg.whiteBright.start =>
        Background(BrightWhite)
      | token when token == Ansi.modifier.bold.start => Bold
      | token when token == Ansi.modifier.dim.start => Dim
      | token when token == Ansi.modifier.italic.start => Italic
      | token when token == Ansi.modifier.underline.start => Underline
      | token when token == Ansi.modifier.inverse.start => Inverse
      | token when token == Ansi.modifier.hidden.start => Hidden
      | token when token == Ansi.modifier.strikethrough.start => Strikethrough
      | token when token == Ansi.modifier.bold.stop => NormalIntensity
      | token when token == Ansi.modifier.dim.stop => NormalIntensity
      | token when token == Ansi.modifier.italic.stop => ItalicOff
      | token when token == Ansi.modifier.underline.stop => UnderlineOff
      | token when token == Ansi.modifier.inverse.stop => InverseOff
      | token when token == Ansi.modifier.hidden.stop => HiddenOff
      | token when token == Ansi.modifier.strikethrough.stop =>
        StrikethroughOff
      | _ => raise(Invalid_argument("unrecognized escape sequence '" ++ String.escaped(token) ++ "'."))
      },
    )
  };
};

let lex: string => list(Token.t) =
  s => {
    s |> tokenize |> List.map(terminalTokenToToken);
  };