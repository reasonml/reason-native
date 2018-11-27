/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
module type StylishSig = {
  let concatList: (string, list(string)) => string;
  /* Styling */
  let bold: string => string;
  let invert: string => string;
  let dim: string => string;
  let underline: string => string;

  let normal: (~underline: bool=?, ~invert: bool=?, ~dim: bool=?, ~bold: bool=?, string) => string;
  let red: (~underline: bool=?, ~invert: bool=?, ~dim: bool=?, ~bold: bool=?, string) => string;
  let yellow: (~underline: bool=?, ~invert: bool=?, ~dim: bool=?, ~bold: bool=?, string) => string;
  let blue: (~underline: bool=?, ~invert: bool=?, ~dim: bool=?, ~bold: bool=?, string) => string;
  let green: (~underline: bool=?, ~invert: bool=?, ~dim: bool=?, ~bold: bool=?, string) => string;
  let cyan: (~underline: bool=?, ~invert: bool=?, ~dim: bool=?, ~bold: bool=?, string) => string;
  let purple: (~underline: bool=?, ~invert: bool=?, ~dim: bool=?, ~bold: bool=?, string) => string;
  let highlight:
    (
      ~underline: bool=?,
      ~invert: bool=?,
      ~dim: bool=?,
      ~bold: bool=?,
      ~color: (~underline: bool=?, ~invert: bool=?, ~dim: bool=?, ~bold: bool=?, string) => string
                =?,
      ~first: int=?,
      ~last: int=?,
      string
    ) =>
    string;
};

module ANSIStylish: StylishSig = {
  let concatList = String.concat;

  let bold = Chalk.bold;

  let invert = Chalk.inverse;

  let dim = Chalk.dim;

  let underline = Chalk.underline;

  let normal = (~underline=false, ~invert=false, ~dim=false, ~bold=false, s) => {
    <Chalk underline inverse=invert dim bold>s</Chalk>
  }

  let red = (~underline=false, ~invert=false, ~dim=false, ~bold=false, s) => {
    <Chalk underline inverse=invert dim bold color=Red>s</Chalk>
  }

  let yellow = (~underline=false, ~invert=false, ~dim=false, ~bold=false, s) => {
    <Chalk underline inverse=invert dim bold color=Yellow>s</Chalk>
  }

  let blue = (~underline=false, ~invert=false, ~dim=false, ~bold=false, s) => {
    <Chalk underline inverse=invert dim bold color=Blue>s</Chalk>
  }

  let green = (~underline=false, ~invert=false, ~dim=false, ~bold=false, s) => {
    <Chalk underline inverse=invert dim bold color=Green>s</Chalk>
  }

  let cyan = (~underline=false, ~invert=false, ~dim=false, ~bold=false, s) => {
    <Chalk underline inverse=invert dim bold color=Cyan>s</Chalk>
  }

  let purple = (~underline=false, ~invert=false, ~dim=false, ~bold=false, s) => {
    <Chalk underline inverse=invert dim bold color=Magenta>s</Chalk>
  }

  let stringSlice = (~first=0, ~last=?, str) => {
    let last =
      switch (last) {
      | Some(l) => min(l, String.length(str))
      | None => String.length(str)
      };
    if (last <= first) {
      "";
    } else {
      String.sub(str, first, last - first);
    };
  };
  let highlight =
      (
        ~underline=false,
        ~invert=false,
        ~dim=false,
        ~bold=false,
        ~color=normal,
        ~first=0,
        ~last=99999,
        str,
      ) =>
    stringSlice(~last=first, str)
    ++ color(~underline, ~dim, ~invert, ~bold, stringSlice(~first, ~last, str))
    ++ stringSlice(~first=last, str);
};
