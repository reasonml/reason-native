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

type reset =
  | Reset
  | ResetOff;

type state = {
  reset: option(reset),
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
  reset: None,
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
      "\treset: "
      ++ formatProperty(state.reset, r =>
           switch (r) {
           | Reset => "Reset"
           | ResetOff => "ResetOff"
           }
         ),
      "\tfg: "
      ++ formatProperty(state.foreground, color =>
           switch (color) {
           | Default => "Default"
           | Red => "Red"
           | Green => "Green"
           | Magenta => "Magenta"
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
  | Reset => {...initialState, reset: Some(Reset)}
  | ResetOff => {...initialState, reset: Some(ResetOff)}
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
  (startState, targetState) =>
    switch (startState.reset, targetState.reset) {
    | (_, Some(Reset)) => targetState
    | (_, Some(ResetOff)) => targetState
    | (_, _) => {
        reset:
          resolvePropertyForDiff(
            startState.reset,
            targetState.reset,
            ResetOff,
          ),
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
        bold:
          resolvePropertyForDiff(startState.bold, targetState.bold, BoldOff),
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
      }
    };

let resolveApplication = (optProp1, optProp2) => {
  switch (optProp1, optProp2) {
  | (a, b) when a == b => optProp2
  | (None, None) => None
  | (Some(a), _) => optProp1
  | (None, Some(a)) => optProp2
  };
};

let overlayState = (baseState, toApply) =>
  switch (baseState.reset) {
  | Some(Reset) => baseState
  | _ => {
      reset: toApply.reset,
      foreground:
        resolveApplication(baseState.foreground, toApply.foreground),
      background:
        resolveApplication(baseState.background, toApply.background),
      bold: resolveApplication(baseState.bold, toApply.bold),
      dim: resolveApplication(baseState.dim, toApply.dim),
      italic: resolveApplication(baseState.italic, toApply.italic),
      underline: resolveApplication(baseState.underline, toApply.underline),
      inverse: resolveApplication(baseState.inverse, toApply.inverse),
      hidden: resolveApplication(baseState.hidden, toApply.hidden),
      strikethrough:
        resolveApplication(baseState.strikethrough, toApply.strikethrough),
    }
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

  let applyState = (stateDiff, stateRegions) => {
    stateRegions
    |> List.map(((baseState, text)) => (overlayState(baseState, stateDiff), text))
    |> resolveStateRegions;
  };

  let parseStateRegions = s => {
    let (stateRegions, finalState) =
      List.fold_left(
        ((acc, prevState), token) =>
          switch (token) {
          | Text(t) =>
            t != ""
              ? ([(prevState, t), ...acc], prevState) : (acc, prevState)
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
        inputs: list(string),
      ) => {
    let state = {
      reset:
        switch (reset) {
        | Some(true) => Some(Reset)
        | _ => None
        },
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
    let inputsStr = String.concat("", inputs);
    let inputsStateRegions = parseStateRegions(inputsStr);
    applyState(state, inputsStateRegions);
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
    bold: (s: string) => fromString(~bold=true, [s]),
    dim: (s: string) => fromString(~dim=true, [s]),
    italic: (s: string) => fromString(~italic=true, [s]),
    underline: (s: string) =>
      fromString(~underline=true, [s]),
    inverse: (s: string) => fromString(~inverse=true, [s]),
    hidden: (s: string) => fromString(~hidden=true, [s]),
    strikethrough: (s: string) =>
      fromString(~strikethrough=true, [s]),
  };

  let color: Decorators.color = {
    black: (s: string) => fromString(~color=Black, [s]),
    red: (s: string) => fromString(~color=Red, [s]),
    green: (s: string) => fromString(~color=Green, [s]),
    yellow: (s: string) => fromString(~color=Yellow, [s]),
    blue: (s: string) => fromString(~color=Blue, [s]),
    magenta: (s: string) => fromString(~color=Magenta, [s]),
    cyan: (s: string) => fromString(~color=Cyan, [s]),
    white: (s: string) => fromString(~color=White, [s]),
    blackBright: (s: string) =>
      fromString(~color=BrightBlack, [s]),
    redBright: (s: string) =>
      fromString(~color=BrightRed, [s]),
    greenBright: (s: string) =>
      fromString(~color=BrightGreen, [s]),
    yellowBright: (s: string) =>
      fromString(~color=BrightYellow, [s]),
    blueBright: (s: string) =>
      fromString(~color=BrightBlue, [s]),
    magentaBright: (s: string) =>
      fromString(~color=BrightMagenta, [s]),
    cyanBright: (s: string) =>
      fromString(~color=BrightCyan, [s]),
    whiteBright: (s: string) =>
      fromString(~color=BrightWhite, [s]),
  };

  let bg: Decorators.color = {
    black: (s: string) =>
      fromString(~backgroundColor=Black, [s]),
    red: (s: string) => fromString(~backgroundColor=Red, [s]),
    green: (s: string) =>
      fromString(~backgroundColor=Green, [s]),
    yellow: (s: string) =>
      fromString(~backgroundColor=Yellow, [s]),
    blue: (s: string) =>
      fromString(~backgroundColor=Blue, [s]),
    magenta: (s: string) =>
      fromString(~backgroundColor=Magenta, [s]),
    cyan: (s: string) =>
      fromString(~backgroundColor=Cyan, [s]),
    white: (s: string) =>
      fromString(~backgroundColor=White, [s]),
    blackBright: (s: string) =>
      fromString(~backgroundColor=BrightBlack, [s]),
    redBright: (s: string) =>
      fromString(~backgroundColor=BrightRed, [s]),
    greenBright: (s: string) =>
      fromString(~backgroundColor=BrightGreen, [s]),
    yellowBright: (s: string) =>
      fromString(~backgroundColor=BrightYellow, [s]),
    blueBright: (s: string) =>
      fromString(~backgroundColor=BrightBlue, [s]),
    magentaBright: (s: string) =>
      fromString(~backgroundColor=BrightMagenta, [s]),
    cyanBright: (s: string) =>
      fromString(~backgroundColor=BrightCyan, [s]),
    whiteBright: (s: string) =>
      fromString(~backgroundColor=BrightWhite, [s]),
  };
};
