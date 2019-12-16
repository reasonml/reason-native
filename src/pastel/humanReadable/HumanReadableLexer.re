/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Token;

type humanReadableToken =
  | RawEscapeSequence(string)
  | RawText(string);

let regexString =
  String.concat(
    "",
    ["(", HumanReadable.tokens^ |> String.concat("|"), ")"],
  );
let humanReadableEscapeSequenceRegex = Re.Pcre.regexp(regexString);

let tokenize = (s: string): list(humanReadableToken) => {
  s
  |> Re.split_full(humanReadableEscapeSequenceRegex)
  |> List.map(r =>
       switch (r) {
       | `Delim(g) => RawEscapeSequence(Re.Group.get(g, 0))
       | `Text(t) => RawText(t)
       }
     );
};

let humanReadableTokenToToken = terminalToken => {
  switch (terminalToken) {
  | RawText(s) => Text(s)
  | RawEscapeSequence(token) =>
    EscapeSequence(
      switch (token) {
      | token when token == HumanReadable.color.stop => Foreground(Default)
      | token when token == HumanReadable.color.black.start =>
        Foreground(Black)
      | token when token == HumanReadable.color.red.start => Foreground(Red)
      | token when token == HumanReadable.color.green.start =>
        Foreground(Green)
      | token when token == HumanReadable.color.yellow.start =>
        Foreground(Yellow)
      | token when token == HumanReadable.color.blue.start => Foreground(Blue)
      | token when token == HumanReadable.color.magenta.start =>
        Foreground(Magenta)
      | token when token == HumanReadable.color.cyan.start => Foreground(Cyan)
      | token when token == HumanReadable.color.white.start =>
        Foreground(White)
      | token when token == HumanReadable.color.blackBright.start =>
        Foreground(BrightBlack)
      | token when token == HumanReadable.color.redBright.start =>
        Foreground(BrightRed)
      | token when token == HumanReadable.color.greenBright.start =>
        Foreground(BrightGreen)
      | token when token == HumanReadable.color.yellowBright.start =>
        Foreground(BrightYellow)
      | token when token == HumanReadable.color.blueBright.start =>
        Foreground(BrightBlue)
      | token when token == HumanReadable.color.magentaBright.start =>
        Foreground(BrightMagenta)
      | token when token == HumanReadable.color.cyanBright.start =>
        Foreground(BrightCyan)
      | token when token == HumanReadable.color.whiteBright.start =>
        Foreground(BrightWhite)
      | token when token == HumanReadable.bg.stop => Background(Default)
      | token when token == HumanReadable.bg.blue.start => Background(Blue)
      | token when token == HumanReadable.bg.black.start => Background(Black)
      | token when token == HumanReadable.bg.red.start => Background(Red)
      | token when token == HumanReadable.bg.green.start => Background(Green)
      | token when token == HumanReadable.bg.yellow.start =>
        Background(Yellow)
      | token when token == HumanReadable.bg.blue.start => Background(Blue)
      | token when token == HumanReadable.bg.magenta.start =>
        Background(Magenta)
      | token when token == HumanReadable.bg.cyan.start => Background(Cyan)
      | token when token == HumanReadable.bg.white.start => Background(White)
      | token when token == HumanReadable.bg.blackBright.start =>
        Background(BrightBlack)
      | token when token == HumanReadable.bg.redBright.start =>
        Background(BrightRed)
      | token when token == HumanReadable.bg.greenBright.start =>
        Background(BrightGreen)
      | token when token == HumanReadable.bg.yellowBright.start =>
        Background(BrightYellow)
      | token when token == HumanReadable.bg.blueBright.start =>
        Background(BrightBlue)
      | token when token == HumanReadable.bg.magentaBright.start =>
        Background(BrightMagenta)
      | token when token == HumanReadable.bg.cyanBright.start =>
        Background(BrightCyan)
      | token when token == HumanReadable.bg.whiteBright.start =>
        Background(BrightWhite)
      | token when token == HumanReadable.modifier.bold.start => Bold
      | token when token == HumanReadable.modifier.dim.start => Dim
      | token when token == HumanReadable.modifier.italic.start => Italic
      | token when token == HumanReadable.modifier.underline.start => Underline
      | token when token == HumanReadable.modifier.inverse.start => Inverse
      | token when token == HumanReadable.modifier.hidden.start => Hidden
      | token when token == HumanReadable.modifier.strikethrough.start =>
        Strikethrough
      | token when token == HumanReadable.modifier.bold.stop => NormalIntensity
      | token when token == HumanReadable.modifier.dim.stop => NormalIntensity
      | token when token == HumanReadable.modifier.italic.stop => ItalicOff
      | token when token == HumanReadable.modifier.underline.stop =>
        UnderlineOff
      | token when token == HumanReadable.modifier.inverse.stop => InverseOff
      | token when token == HumanReadable.modifier.hidden.stop => HiddenOff
      | token when token == HumanReadable.modifier.reset.start => Reset
      | token when token == HumanReadable.modifier.reset.stop => ResetOff
      | token when token == HumanReadable.modifier.strikethrough.stop =>
        StrikethroughOff
      | _ =>
        raise(
          Invalid_argument(
            "unrecognized escape sequence '" ++ String.escaped(token) ++ "'.",
          ),
        )
      },
    )
  };
};

let lex: string => list(Token.t) =
  s => {
    s |> tokenize |> List.map(humanReadableTokenToToken);
  };
