/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

open Token;

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

type stateRegion = (state, string);
type stateDiff = state;

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
      "\tfg: "
      ++ formatProperty(state.foreground, color =>
           switch (color) {
           | Default => "Default"
           | Red => "Red"
           | Green => "Green"
           | _ => "no special formatting yet"
           }
         ),
      "\tdim: "
      ++ formatProperty(state.dim, v =>
           switch (v) {
           | Dim => "Dim"
           | DimOff => "DimOff"
           }
         ),
      "\tbold: "
      ++ formatProperty(state.bold, v =>
           switch (v) {
           | Bold => "Bold"
           | BoldOff => "BoldOff"
           }
         ),
      "\titalic: "
      ++ formatProperty(state.italic, v =>
           switch (v) {
           | Italic => "Italic"
           | ItalicOff => "ItalicOff"
           }
         ),
      "}",
    ],
  );
};

let resolveEscapeSequence = (token, state) => {
  switch (token) {
  | Foreground(color) =>
    switch (color) {
    | Default => {...state, foreground: None}
    | color => {...state, foreground: Some(color)}
    }
  | Background(color) =>
    switch (color) {
    | Default => {...state, background: None}
    | color => {...state, background: Some(color)}
    }
  | Bold => {...state, bold: Some(Bold)}
  | Dim => {...state, dim: Some(Dim)}
  | NormalIntensity => {...state, bold: None, dim: None}
  | Italic => {...state, italic: Some(Italic)}
  | ItalicOff => {...state, italic: None}
  | Underline => {...state, underline: Some(Underline)}
  | UnderlineOff => {...state, underline: None}
  | Inverse => {...state, inverse: Some(Inverse)}
  | InverseOff => {...state, inverse: None}
  | Hidden => {...state, hidden: Some(Hidden)}
  | HiddenOff => {...state, hidden: None}
  | Strikethrough => {...state, strikethrough: Some(Strikethrough)}
  | StrikethroughOff => {...state, strikethrough: None}
  | Reset => initialState
  };
};

let resolvePropertyForDiff = (optProp1, optProp2, removeProp) => {
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
      resolvePropertyForDiff(
        startState.foreground,
        targetState.foreground,
        Default,
      ),
    background:
      resolvePropertyForDiff(
        startState.background,
        targetState.background,
        Default,
      ),
    bold: resolvePropertyForDiff(startState.bold, targetState.bold, BoldOff),
    dim: resolvePropertyForDiff(startState.dim, targetState.dim, DimOff),
    italic:
      resolvePropertyForDiff(
        startState.italic,
        targetState.italic,
        ItalicOff,
      ),
    underline:
      resolvePropertyForDiff(
        startState.underline,
        targetState.underline,
        UnderlineOff,
      ),
    inverse:
      resolvePropertyForDiff(
        startState.inverse,
        targetState.inverse,
        InverseOff,
      ),
    hidden:
      resolvePropertyForDiff(
        startState.hidden,
        targetState.hidden,
        HiddenOff,
      ),
    strikethrough:
      resolvePropertyForDiff(
        startState.strikethrough,
        targetState.strikethrough,
        StrikethroughOff,
      ),
  };

let resolveApplication = (optProp1, optProp2) => {
  switch (optProp1, optProp2) {
  | (a, b) when a == b => optProp2
  | (None, None) => None
  | (_, Some(b)) => optProp2
  | (Some(a), None) => optProp1
  };
};

let overlayState = (baseState, toApply) => {
  {
    foreground: resolveApplication(baseState.foreground, toApply.foreground),
    background: resolveApplication(baseState.background, toApply.background),
    bold: resolveApplication(baseState.bold, toApply.bold),
    dim: resolveApplication(baseState.dim, toApply.dim),
    italic: resolveApplication(baseState.italic, toApply.italic),
    underline: resolveApplication(baseState.underline, toApply.underline),
    inverse: resolveApplication(baseState.inverse, toApply.inverse),
    hidden: resolveApplication(baseState.hidden, toApply.hidden),
    strikethrough:
      resolveApplication(baseState.strikethrough, toApply.strikethrough),
  };
};

module Option = {
  let map = (o, f) => {
    switch (o) {
    | None => None
    | Some(v) => Some(f(v))
    };
  };
};

module Make =
       (
         Config: {
           let applyState: (state, string) => string;
           let lex: string => list(Token.t);
         },
       ) => {
  let resolveStateRegions: list(stateRegion) => string =
    regions => {
      let (_, s) =
        List.fold_left(
          ((prevState, s), (targetState, text)) => {
            let diff = diffStates(prevState, targetState);
            (
              targetState,
              String.concat("", [s, Config.applyState(diff, text)]),
            );
          },
          (initialState, ""),
          regions @ [(initialState, "")],
        );
      s;
    };
  
  let applyState = (state, childStateRegions) => {
    let finalStateRegions =
      List.map(
        ((s, text)) => (overlayState(state, s), text),
        childStateRegions,
      );
    resolveStateRegions(finalStateRegions);
  };

  let parseStateRegions = s => {
    let (stateRegions, finalState) =
      List.fold_left(
        ((acc, prevState), token) =>
          switch (token) {
          | Text(t) => t != "" ? ([(prevState, t), ...acc], prevState) : (acc, prevState)
          | EscapeSequence(escapeSequence) => (
              acc,
              resolveEscapeSequence(escapeSequence, prevState),
            )
          },
        ([], initialState),
        Config.lex(s),
      );
    let finalStateRegions = List.rev(stateRegions);
    finalStateRegions;
  };

  let fromString =
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
        Option.map(underline, underline =>
          underline ? Underline : UnderlineOff
        ),
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
    let childStateRegions = parseStateRegions(childrenStr);
    applyState(state, childStateRegions);
  };

  let partitionFromStateRegions = (index: int, regions: list(stateRegion)) => {
    let (_, (left, right)) =
      List.fold_left(
        ((numCharsSeen, (leftHalf, rightHalf)), (styles, text)) =>
          if (numCharsSeen < index) {
            let textLength = String.length(text);
            if (textLength + numCharsSeen < index) {
              (
                numCharsSeen + index,
                ([(styles, text), ...leftHalf], rightHalf),
              );
            } else {
              let numToTake = index - numCharsSeen;
              let firstPart = String.sub(text, 0, numToTake);
              let secondPart =
                String.sub(
                  text,
                  index - numCharsSeen,
                  textLength - numToTake,
                );
              (
                numCharsSeen + index,
                (
                  [(styles, firstPart), ...leftHalf],
                  [(styles, secondPart), ...rightHalf],
                ),
              );
            };
          } else {
            (
              numCharsSeen + index,
              (leftHalf, [(styles, text), ...rightHalf]),
            );
          },
        (0, ([], [])),
        regions,
      );
    (List.rev(left), List.rev(right));
  };

  let partitionFromString = (index, s) => {
    let (left, right) =
      s |> parseStateRegions |> partitionFromStateRegions(index);
    (resolveStateRegions(left), resolveStateRegions(right));
  };

  let lengthFromStateRegions = (regions: list(stateRegion)) => {
    List.fold_left(
      (acc, (styles, text)) => acc + String.length(text),
      0,
      regions,
    );
  };

  let unformattedTextFromStateRegions = (regions: list(stateRegion)) => {
    regions |> List.map(((stles, text)) => text) |> String.concat("");
  };

  let lengthFromString = s => {
    s |> parseStateRegions |> lengthFromStateRegions;
  };

  let unformattedTextFromString = s =>
    s |> parseStateRegions |> unformattedTextFromStateRegions;

  let modifier: Decorators.modifier = {
    bold: (s: string) => fromString(~bold=true, ~children=[s], ()),
    dim: (s: string) => fromString(~dim=true, ~children=[s], ()),
    italic: (s: string) => fromString(~italic=true, ~children=[s], ()),
    underline: (s: string) => fromString(~underline=true, ~children=[s], ()),
    inverse: (s: string) => fromString(~inverse=true, ~children=[s], ()),
    hidden: (s: string) => fromString(~hidden=true, ~children=[s], ()),
    strikethrough: (s: string) => fromString(~strikethrough=true, ~children=[s], ()),
  };

  let color: Decorators.color = {
    black: (s: string) => fromString(~color=Black, ~children=[s], ()),
    red: (s: string) => fromString(~color=Red, ~children=[s], ()),
    green: (s: string) => fromString(~color=Green, ~children=[s], ()),
    yellow: (s: string) => fromString(~color=Yellow, ~children=[s], ()),
    blue: (s: string) => fromString(~color=Blue, ~children=[s], ()),
    magenta: (s: string) => fromString(~color=Magenta, ~children=[s], ()),
    cyan: (s: string) => fromString(~color=Cyan, ~children=[s], ()),
    white: (s: string) => fromString(~color=White, ~children=[s], ()),
    blackBright: (s: string) =>
      fromString(~color=BrightBlack, ~children=[s], ()),
    redBright: (s: string) =>
      fromString(~color=BrightRed, ~children=[s], ()),
    greenBright: (s: string) =>
      fromString(~color=BrightGreen, ~children=[s], ()),
    yellowBright: (s: string) =>
      fromString(~color=BrightYellow, ~children=[s], ()),
    blueBright: (s: string) =>
      fromString(~color=BrightBlue, ~children=[s], ()),
    magentaBright: (s: string) =>
      fromString(~color=BrightMagenta, ~children=[s], ()),
    cyanBright: (s: string) =>
      fromString(~color=BrightCyan, ~children=[s], ()),
    whiteBright: (s: string) =>
      fromString(~color=BrightWhite, ~children=[s], ()),
  };

  let bg: Decorators.color = {
    black: (s: string) =>
      fromString(~backgroundColor=Black, ~children=[s], ()),
    red: (s: string) => fromString(~backgroundColor=Red, ~children=[s], ()),
    green: (s: string) =>
      fromString(~backgroundColor=Green, ~children=[s], ()),
    yellow: (s: string) =>
      fromString(~backgroundColor=Yellow, ~children=[s], ()),
    blue: (s: string) =>
      fromString(~backgroundColor=Blue, ~children=[s], ()),
    magenta: (s: string) =>
      fromString(~backgroundColor=Magenta, ~children=[s], ()),
    cyan: (s: string) =>
      fromString(~backgroundColor=Cyan, ~children=[s], ()),
    white: (s: string) =>
      fromString(~backgroundColor=White, ~children=[s], ()),
    blackBright: (s: string) =>
      fromString(~backgroundColor=BrightBlack, ~children=[s], ()),
    redBright: (s: string) =>
      fromString(~backgroundColor=BrightRed, ~children=[s], ()),
    greenBright: (s: string) =>
      fromString(~backgroundColor=BrightGreen, ~children=[s], ()),
    yellowBright: (s: string) =>
      fromString(~backgroundColor=BrightYellow, ~children=[s], ()),
    blueBright: (s: string) =>
      fromString(~backgroundColor=BrightBlue, ~children=[s], ()),
    magentaBright: (s: string) =>
      fromString(~backgroundColor=BrightMagenta, ~children=[s], ()),
    cyanBright: (s: string) =>
      fromString(~backgroundColor=BrightCyan, ~children=[s], ()),
    whiteBright: (s: string) =>
      fromString(~backgroundColor=BrightWhite, ~children=[s], ()),
  };
};