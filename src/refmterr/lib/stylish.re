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

  let resetANSI = "\027[0m";

  let bold = s => "\027[1m" ++ s ++ resetANSI;

  let boldCode = b => b ? "\027[1m" : "";

  let invert = s => "\027[7m" ++ s ++ resetANSI;

  let invertCode = b => b ? "\027[7m" : "";

  let dim = s => "\027[2m" ++ s ++ resetANSI;

  let dimCode = b => b ? "\027[2m" : "";

  let underlineCode = u => u ? "\027[4m" : "";

  let normalCode = "\027[39m";

  let redCode = "\027[31m";

  let yellowCode = "\027[33m";

  let blueCode = "\027[34m";

  let greenCode = "\027[32m";

  let purpleCode = "\027[35m";

  let cyanCode = "\027[36m";

  let underline = s => underlineCode(true) ++ s ++ resetANSI;

  let normal = (~underline=false, ~invert=false, ~dim=false, ~bold=false, s) =>
    underlineCode(underline)
    ++ invertCode(invert)
    ++ dimCode(dim)
    ++ boldCode(bold)
    ++ normalCode
    ++ s
    ++ resetANSI;

  let red = (~underline=false, ~invert=false, ~dim=false, ~bold=false, s) =>
    underlineCode(underline)
    ++ invertCode(invert)
    ++ dimCode(dim)
    ++ boldCode(bold)
    ++ redCode
    ++ s
    ++ resetANSI;

  let yellow = (~underline=false, ~invert=false, ~dim=false, ~bold=false, s) =>
    underlineCode(underline)
    ++ invertCode(invert)
    ++ dimCode(dim)
    ++ boldCode(bold)
    ++ yellowCode
    ++ s
    ++ resetANSI;

  let blue = (~underline=false, ~invert=false, ~dim=false, ~bold=false, s) =>
    underlineCode(underline)
    ++ invertCode(invert)
    ++ dimCode(dim)
    ++ boldCode(bold)
    ++ blueCode
    ++ s
    ++ resetANSI;

  let green = (~underline=false, ~invert=false, ~dim=false, ~bold=false, s) =>
    underlineCode(underline)
    ++ invertCode(invert)
    ++ dimCode(dim)
    ++ boldCode(bold)
    ++ greenCode
    ++ s
    ++ resetANSI;

  let cyan = (~underline=false, ~invert=false, ~dim=false, ~bold=false, s) =>
    underlineCode(underline)
    ++ invertCode(invert)
    ++ dimCode(dim)
    ++ boldCode(bold)
    ++ cyanCode
    ++ s
    ++ resetANSI;

  let purple = (~underline=false, ~invert=false, ~dim=false, ~bold=false, s) =>
    underlineCode(underline)
    ++ invertCode(invert)
    ++ dimCode(dim)
    ++ boldCode(bold)
    ++ purpleCode
    ++ s
    ++ resetANSI;

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
