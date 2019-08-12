/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

type color =
  | Default
  | Black
  | Red
  | Green
  | Yellow
  | Blue
  | Magenta
  | Cyan
  | White
  | BrightBlack
  | BrightRed
  | BrightGreen
  | BrightYellow
  | BrightBlue
  | BrightMagenta
  | BrightCyan
  | BrightWhite;

type bold =
  | Bold
  | BoldOff;

type dim =
  | Dim
  | DimOff;

type italic =
  | Italic
  | ItalicOff;

type underline =
  | Underline
  | UnderlineOff;

type inverse =
  | Inverse
  | InverseOff;

type hidden =
  | Hidden
  | HiddenOff;

type strikeThrough =
  | Strikethrough
  | StrikethroughOff;

type escapeSequence =
  | Foreground(color)
  | Background(color)
  | BoldToken
  | DimToken
  | NormalIntensity
  | ItalicToken(italic)
  | UnderlineToken(underline)
  | InverseToken(inverse)
  | HiddenToken(hidden)
  | StrikethroughToken(strikeThrough);

type state = {
  foreground: option(color),
  background: option(color),
  bold: option(bold),
  dim: option(dim),
  italic: option(italic),
  underline: option(underline),
  inverse: option(inverse),
  hidden: option(hidden),
  strikethrough: option(strikeThrough),
};

let initialState = {
  foreground: None,
  background: None,
  bold: None,
  dim: None,
  italic: None,
  underline: None,
  inverse: None,
  hidden: None,
  strikethrough: None,
};

let resolveEscapeSequence = (token, state) => {
  switch (token) {
  | Foreground(color) => {...state, foreground: Some(color)}
  | Background(color) => {...state, background: Some(color)}
  | BoldToken => {...state, bold: Some(Bold)}
  | DimToken => {...state, dim: Some(Dim)}
  | NormalIntensity => switch((state.bold, state.dim)) {
  | (None, None) => state
  | (Some(_), Some(_)) =>   {...state, bold: Some(BoldOff), dim: Some(DimOff)}
  | (None, Some(_)) => {...state, dim: Some(DimOff)}
  | (Some(_), None) => {...state, bold: Some(BoldOff)}
  }

  | ItalicToken(italic) => {...state, italic: Some(italic)}
  | UnderlineToken(underline) => {...state, underline: Some(underline)}
  | InverseToken(inverse) => {...state, inverse: Some(inverse)}
  | HiddenToken(hidden) => {...state, hidden: Some(hidden)}
  | StrikethroughToken(strikethrough) => {
      ...state,
      strikethrough: Some(strikethrough),
    }
  };
};

let printEscapeSequence = token => {
  switch (token) {
  | Foreground(color) =>
    switch (color) {
    | Default => "default foreground color"
    | _ => "foreground color"
    }
  | Background(color) =>
    switch (color) {
    | Default => "default background color"
    | _ => "background color"
    }
  | BoldToken => "Bold"
  | DimToken => "Dim"
  | NormalIntensity => "Normal intensity"
  | ItalicToken(italic) =>
    switch (italic) {
    | Italic => "italic"
    | ItalicOff => "italicOff"
    }
  | UnderlineToken(underline) =>
    switch (underline) {
    | Underline => "underline"
    | UnderlineOff => "underlineOff"
    }
  | InverseToken(inverse) =>
    switch (inverse) {
    | Inverse => "inverse"
    | InverseOff => "inverseOff"
    }
  | HiddenToken(hidden) =>
    switch (hidden) {
    | Hidden => "hidden"
    | HiddenOff => "hiddenOff"
    }
  | StrikethroughToken(strikethrough) =>
    switch (strikethrough) {
    | Strikethrough => "strikethrough"
    | StrikethroughOff => "strikethroughOff"
    }
  };
};

type stateRegion = (state, string);

type stateDiff = state;

let formatProperty = (property, valueFormatter) => {
  switch (property) {
  | None => "None"
  | Some(v) => "Some(" ++ valueFormatter(v) ++ ")"
  };
};

let printState = state => {
  String.concat(
    "\n",
    [
      "{",
      "\tdim: " ++
      formatProperty(state.dim, v =>
        switch (v) {
        | Dim => "Dim"
        | DimOff => "DimOff"
        }
      ),
      "\tbold: " ++
        formatProperty(state.bold, v => switch(v) {
        | Bold => "Bold"
        | BoldOff => "BoldOff"
        }),
        "\titalic: " ++
        formatProperty(state.italic, v => switch(v) {
        | Italic => "Italic"
        | ItalicOff => "ItalicOff"
        }),
      "}",
    ],
  );
};

let resolveProperty = (optProp1, optProp2, removeProp) => {
  switch (optProp1, optProp2) {
  | (a, b) when a == b => None
  | (None, None) => None
  | (_, Some(b)) => optProp2
  | (Some(a), None) => Some(removeProp)
  };
};

let diffStates: (state, state) => stateDiff =
  (startState, targetState) => {
    foreground:
      resolveProperty(startState.foreground, targetState.foreground, Default),
    background:
      resolveProperty(startState.background, targetState.background, Default),
    bold: resolveProperty(startState.bold, targetState.bold, BoldOff),
    dim: resolveProperty(startState.dim, targetState.dim, DimOff),
    italic: resolveProperty(startState.italic, targetState.italic, ItalicOff),
    underline:
      resolveProperty(
        startState.underline,
        targetState.underline,
        UnderlineOff,
      ),
    inverse:
      resolveProperty(startState.inverse, targetState.inverse, InverseOff),
    hidden: resolveProperty(startState.hidden, targetState.hidden, HiddenOff),
    strikethrough:
      resolveProperty(
        startState.strikethrough,
        targetState.strikethrough,
        StrikethroughOff,
      ),
  };

let applyCode = (modifier, s) => String.concat("", [modifier, s]);

let applyStateDiff = (stateDiff, s) => {
  /* Console.log("state diff:\n" ++ printState(stateDiff)); */
  let s =
    switch (stateDiff.foreground) {
    | None => s
    | Some(color) =>
      switch (color) {
      | Default => applyCode(Ansi.color.stop, s)
      | Black => applyCode(Ansi.color.black.start, s)
      | Red => applyCode(Ansi.color.red.start, s)
      | Green => applyCode(Ansi.color.green.start, s)
      | Yellow => applyCode(Ansi.color.yellow.start, s)
      | Blue => applyCode(Ansi.color.blue.start, s)
      | Magenta => applyCode(Ansi.color.magenta.start, s)
      | Cyan => applyCode(Ansi.color.cyan.start, s)
      | White => applyCode(Ansi.color.white.start, s)
      | BrightBlack => applyCode(Ansi.color.blackBright.start, s)
      | BrightRed => applyCode(Ansi.color.redBright.start, s)
      | BrightGreen => applyCode(Ansi.color.greenBright.start, s)
      | BrightYellow => applyCode(Ansi.color.yellowBright.start, s)
      | BrightBlue => applyCode(Ansi.color.blueBright.start, s)
      | BrightMagenta => applyCode(Ansi.color.magentaBright.start, s)
      | BrightCyan => applyCode(Ansi.color.cyanBright.start, s)
      | BrightWhite => applyCode(Ansi.color.cyanBright.start, s)
      }
    };
  let s =
    switch (stateDiff.background) {
    | None => s
    | Some(color) =>
      switch (color) {
      | Default => applyCode(Ansi.bg.stop, s)
      | Black => applyCode(Ansi.bg.black.start, s)
      | Red => applyCode(Ansi.bg.red.start, s)
      | Green => applyCode(Ansi.bg.green.start, s)
      | Yellow => applyCode(Ansi.bg.yellow.start, s)
      | Blue => applyCode(Ansi.bg.blue.start, s)
      | Magenta => applyCode(Ansi.bg.magenta.start, s)
      | Cyan => applyCode(Ansi.bg.cyan.start, s)
      | White => applyCode(Ansi.bg.white.start, s)
      | BrightBlack => applyCode(Ansi.bg.blackBright.start, s)
      | BrightRed => applyCode(Ansi.bg.redBright.start, s)
      | BrightGreen => applyCode(Ansi.bg.greenBright.start, s)
      | BrightYellow => applyCode(Ansi.bg.yellowBright.start, s)
      | BrightBlue => applyCode(Ansi.bg.blueBright.start, s)
      | BrightMagenta => applyCode(Ansi.bg.magentaBright.start, s)
      | BrightCyan => applyCode(Ansi.bg.cyanBright.start, s)
      | BrightWhite => applyCode(Ansi.bg.cyanBright.start, s)
      }
    };
  /* bold and dim are undone by the same sequence, so we handle them jointly*/
  let s =
    switch (stateDiff.bold, stateDiff.dim) {
    | (None, None) => s
    | (Some(Bold), Some(Dim)) =>
      s
      |> applyCode(Ansi.modifier.bold.start)
      |> applyCode(Ansi.modifier.dim.start)
    | (Some(Bold), Some(DimOff)) =>
      s
      |> applyCode(Ansi.modifier.bold.start)
      |> applyCode(Ansi.modifier.dim.stop)
    | (Some(BoldOff), Some(Dim)) =>
      s
      |> applyCode(Ansi.modifier.dim.start)
      |> applyCode(Ansi.modifier.bold.stop)
    | (Some(Bold), None) => s |> applyCode(Ansi.modifier.bold.start)
    | (None, Some(Dim)) => s |> applyCode(Ansi.modifier.dim.start)
    | (Some(BoldOff), None)
    | (None, Some(DimOff))
    | (Some(BoldOff), Some(DimOff)) =>
      s |> applyCode(Ansi.modifier.bold.stop)
    };
  let s =
    switch (stateDiff.italic) {
    | None => s
    | Some(Italic) => applyCode(Ansi.modifier.italic.start, s)
    | Some(ItalicOff) => applyCode(Ansi.modifier.italic.stop, s)
    };
  let s =
    switch (stateDiff.underline) {
    | None => s
    | Some(Underline) => applyCode(Ansi.modifier.underline.start, s)
    | Some(UnderlineOff) => applyCode(Ansi.modifier.underline.stop, s)
    };
  let s =
    switch (stateDiff.inverse) {
    | None => s
    | Some(Inverse) => applyCode(Ansi.modifier.inverse.start, s)
    | Some(InverseOff) => applyCode(Ansi.modifier.inverse.stop, s)
    };
  let s =
    switch (stateDiff.hidden) {
    | None => s
    | Some(Hidden) => applyCode(Ansi.modifier.hidden.start, s)
    | Some(HiddenOff) => applyCode(Ansi.modifier.hidden.stop, s)
    };
  let s =
    switch (stateDiff.strikethrough) {
    | None => s
    | Some(Strikethrough) => applyCode(Ansi.modifier.strikethrough.start, s)
    | Some(StrikethroughOff) =>
      applyCode(Ansi.modifier.strikethrough.stop, s)
    };
  s;
};

let resolveStateRegions: list(stateRegion) => string =
  regions => {
    let (_, s) =
      List.fold_left(
        ((prevState, s), (targetState, text)) => {
          let diff = diffStates(prevState, targetState);
          Console.log(printState(diff));
          (
            targetState,
            String.concat("", [s, applyStateDiff(diff, text)]),
          );
        },
        (initialState, ""),
        regions,
      );
    s;
  };

module Option = {
  let map = (o, f) => {
    switch (o) {
    | None => None
    | Some(v) => Some(f(v))
    };
  };
};

let makeCodesRegex = codes => {
  let start = "\027\\[";
  let codesExpr = String.concat("|", List.map(string_of_int, codes));
  let stop = "m";
  let regexString =
    String.concat("", ["(", start, "(", codesExpr, ")", stop, ")"]);
  Re.Pcre.regexp(regexString);
};

let startRegex = makeCodesRegex(Ansi.IntSet.elements(Ansi.starts));
let stopRegex = makeCodesRegex(Ansi.IntSet.elements(Ansi.stops));

let startTokenToInstructions = s => {
  let tokens = Re.matches(startRegex, s);
  /* Console.log("\nstart tokens");
  Console.log(List.map(String.escaped, tokens));
  Console.log("end start tokens"); */
  List.map(
    token =>
      switch (token) {
      | token when token == Ansi.color.stop => Foreground(Default)
      | token when token == Ansi.color.stop => Foreground(Default)
      | token when token == Ansi.color.blue.start => Foreground(Blue)
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
      | token when token == Ansi.modifier.bold.start => BoldToken
      | token when token == Ansi.modifier.dim.start => DimToken
      | token when token == Ansi.modifier.italic.start => ItalicToken(Italic)
      | token when token == Ansi.modifier.underline.start =>
        UnderlineToken(Underline)
      | token when token == Ansi.modifier.inverse.start =>
        InverseToken(Inverse)
      | token when token == Ansi.modifier.hidden.start => HiddenToken(Hidden)
      | token when token == Ansi.modifier.strikethrough.start =>
        StrikethroughToken(Strikethrough)
      | _ => raise(Invalid_argument(String.escaped(token)))
      },
    tokens,
  );
};

let stopTokenToInstructions = s => {
  let tokens = Re.matches(stopRegex, s);
  /* Console.log("stop tokens");
  Console.log(List.map(String.escaped, tokens));
  Console.log("end stop tokens"); */
  List.map(
    token =>
      switch (token) {
      | token when token == Ansi.color.stop => Foreground(Default)
      | token when token == Ansi.bg.stop => Background(Default)
      | token when token == Ansi.modifier.bold.stop => NormalIntensity
      | token when token == Ansi.modifier.dim.stop => NormalIntensity
      | token when token == Ansi.modifier.italic.stop =>
        ItalicToken(ItalicOff)
      | token when token == Ansi.modifier.underline.stop =>
        UnderlineToken(UnderlineOff)
      | token when token == Ansi.modifier.inverse.stop =>
        InverseToken(InverseOff)
      | token when token == Ansi.modifier.hidden.stop =>
        HiddenToken(HiddenOff)
      | token when token == Ansi.modifier.strikethrough.stop =>
        StrikethroughToken(StrikethroughOff)
      | _ => raise(Invalid_argument(String.escaped(token)))
      },
    tokens,
  );
};

let parseStateRegions = s => {
  let (stateRegions, finalState) =
    List.fold_left(
      ((acc, prevState), token) =>
        switch (token) {
        | PastelLexer.Text(t) => ([(prevState, t), ...acc], prevState)
        | Starts(starts) =>
          let startInstructions = startTokenToInstructions(starts);
          Console.log(List.map(printEscapeSequence, startInstructions));
          let state =
            List.fold_left(
              (state, instruction) =>
                resolveEscapeSequence(instruction, state),
              prevState,
              startInstructions,
            );
          (acc, state);
        | Stops(stops) =>
          let stopInstructions = stopTokenToInstructions(stops);
          Console.log(List.map(printEscapeSequence, stopInstructions));
          let state =
            List.fold_left(
              (state, instruction) =>
                resolveEscapeSequence(instruction, state),
              prevState,
              stopInstructions,
            );
          (acc, state);
        },
      ([], initialState),
      TerminalImplementation.TerminalLexer.tokenize(s),
    );
  let finalStateRegions = List.rev([(finalState, ""), ...stateRegions]);
  finalStateRegions;
};

let createElement =
    (
      ~reset: option(bool)=?,
      ~bold: option(bool)=?,
      ~dim: option(bool)=?,
      ~italic: option(bool)=?,
      ~underline: option(bool)=?,
      ~inverse: option(bool)=?,
      ~hidden: option(bool)=?,
      ~strikethrough: option(bool)=?,
      ~color: option(color)=?,
      ~backgroundColor: option(color)=?,
      ~children: list(string),
      (),
    ) => {
  let state = {
    bold: Option.map(bold, bold => bold ? Bold : BoldOff),
    dim: Option.map(dim, dim => dim ? Dim : DimOff),
    italic: Option.map(italic, italic => italic ? Italic : ItalicOff),
    underline:
      Option.map(underline, underline => underline ? Underline : UnderlineOff),
    inverse: Option.map(inverse, inverse => inverse ? Inverse : InverseOff),
    hidden: Option.map(hidden, hidden => hidden ? Hidden : HiddenOff),
    strikethrough:
      Option.map(strikethrough, strikethrough =>
        strikethrough ? Strikethrough : StrikethroughOff
      ),
    foreground: color,
    background: backgroundColor,
  };
  let childrenStr = String.concat("", children);

  let childTokens =
    TerminalImplementation.TerminalLexer.tokenize(childrenStr);

  let (stateRegions, finalState) =
    List.fold_left(
      ((acc, prevState), token) =>
        switch (token) {
        | PastelLexer.Text(t) => ([(prevState, t), ...acc], prevState)
        | Starts(starts) =>
          let startInstructions = startTokenToInstructions(starts);
          let state =
            List.fold_left(
              (state, instruction) =>
                resolveEscapeSequence(instruction, state),
              prevState,
              startInstructions,
            );
          (acc, state);
        | Stops(stops) =>
          let stopInstructions = stopTokenToInstructions(stops);
          let state =
            List.fold_left(
              (state, instruction) =>
                resolveEscapeSequence(instruction, state),
              prevState,
              stopInstructions,
            );
          (acc, state);
        },
      ([], init),
      childTokens,
    );
  let finalStateRegions =
    List.rev([(initialState, ""), (finalState, ""), ...stateRegions]);
  resolveStateRegions(finalStateRegions);
};