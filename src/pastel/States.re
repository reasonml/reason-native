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
  | BoldToken => {...state, bold: Some(Bold)}
  | DimToken => {...state, dim: Some(Dim)}
  | NormalIntensity => {...state, bold: None, dim: None}
  | ItalicToken(italic) =>
    switch (italic) {
    | Italic => {...state, italic: Some(italic)}
    | ItalicOff => {...state, italic: None}
    }
  | UnderlineToken(underline) =>
    switch (underline) {
    | Underline => {...state, underline: Some(underline)}
    | UnderlineOff => {...state, underline: None}
    }
  | InverseToken(inverse) =>
    switch (inverse) {
    | Inverse => {...state, inverse: Some(inverse)}
    | InverseOff => {...state, inverse: None}
    }
  | HiddenToken(hidden) =>
    switch (hidden) {
    | Hidden => {...state, hidden: Some(hidden)}
    | HiddenOff => {...state, hidden: None}
    }
  | StrikethroughToken(strikethrough) =>
    switch (strikethrough) {
    | Strikethrough => {...state, strikethrough: Some(strikethrough)}
    | StrikethroughOff => {...state, strikethrough: None}
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
