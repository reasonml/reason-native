/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 *
 * LICENSE file in the root directory of this source tree.
 */;
module Make = (Styl: Stylish.StylishSig) => {
  open Helpers;
  let tokens = [
    /* Named arguments */
    ({|~[a-z][a-zA-Z0-9_']*\b|}, Styl.yellow),
    ({|\blet\b|\bmodule\b|\blet\b|\btype\b|\bopen\b|}, Styl.purple),
    (
      {|\bif\b|\belse\b|\bfor\b|\bfor\b|\bwhile\b|\bswitch\b|\bint\b|\bstring\b|\blist\b|},
      Styl.yellow,
    ),
    ({|\b[0-9]+\b|}, Styl.blue),
    ({|\b[A-Z][A-Za-z0-9_]*\b|}, Styl.blue),
    ({|\s\+\+\s|\s\+\s|\s\-\s|\s=>\s|\s==\s|}, Styl.red),
  ];

  let tokenRegex =
    String.concat("|", List.map(((rStr, _)) => "(" ++ rStr ++ ")", tokens));

  /*
  * This is so much more complicated because highlighting doesn't support
  * nesting right now.
  */
  let highlightTokens = (~dim, ~bold, ~underline, txt, tokens) => {
    let rex = Re.Pcre.regexp(tokenRegex);
    let splitted = Re.Pcre.full_split(~rex, txt);
    let strings =
      List.map(
        fun
        | Re.Pcre.Text(s) => Styl.highlight(~dim, ~bold, ~underline, s)
        | Delim(s) => "" /* Let the Group do the highlighting */
        | Group(i, s) => {
            let (r, color) = List.nth(tokens, i - 1);
            Styl.highlight(~dim, ~bold, ~underline, ~color, s);
          }
        | NoGroup => "",
        splitted,
      );
    String.concat("", strings);
  };

  let highlightSource = (~dim=false, ~underline=false, ~bold=false, txt) => {
    let splitOnQuotes = splitOnChar('"', txt);
    let balancedQuotes = List.length(splitOnQuotes) mod 2 === 1;
    if (balancedQuotes) {
      let chunks =
        List.mapi(
          (i, chunk) =>
            if (i mod 2 === 0) {
              highlightTokens(~dim, ~underline, ~bold, chunk, tokens);
            } else {
              Styl.highlight(~dim, ~underline, ~color=Styl.green, "\"" ++ chunk ++ "\"");
            },
          splitOnQuotes,
        );
      String.concat("", chunks);
    } else {
      highlightTokens(~dim, ~bold, ~underline, txt, tokens);
    };
  };
};
