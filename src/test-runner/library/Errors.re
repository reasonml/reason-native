/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
/**
 * Wraps a string at the given number of columns. Will only break on spaces or
 * tab charachters. Does nothing if the string already contains new lines.
 */
let wrapString = (cols: int, message: string): string =>
  /* Never attempt to wrap a message that already has new lines */
  if (String.contains(message, '\n')) {
    message;
  } else {
    let messageParts = Str.full_split(Str.regexp("[ \t]+"), message);
    let (message, workingLine, _) =
      List.fold_left(
        ((message, workingLine, lastDelim), part) => {
          /*
           * An assumption not clearly encoded is that the parts must alternate
           * between Str.Text and Str.Delim. It is logically impossible for the
           * full split (the way the regex is structured) to return consecutive
           * Str.Text or Str.Delim parts. Without this assumption the code
           * below may seem incorrect.
           */
          let result =
            switch (part) {
            | Str.Text(value) =>
              if (String.length(workingLine)
                  + String.length(lastDelim)
                  + String.length(value) <= cols) {
                (message, workingLine ++ lastDelim ++ value, "");
              } else {
                (message ++ "\n" ++ workingLine, value, "");
              }
            | Str.Delim(value) => (message, workingLine, value)
            };
          result;
        },
        ("", "", ""),
        messageParts,
      );
    let message = message ++ "\n" ++ workingLine;
    String.trim(message);
  };

let info = (~prefix=true, ~color=true, ~wrap=true, message: string): unit => {
  let message = prefix ? "[Info] " ++ message : message;
  let message = wrap ? wrapString(120, message) : message;
  let message = color ? Chalk.blue(message) : message;
  prerr_endline(message);
};

let warn = (~prefix=true, ~color=true, ~wrap=true, message: string): unit => {
  let message = prefix ? "[Warn] " ++ message : message;
  let message = wrap ? wrapString(120, message) : message;
  let message = color ? Chalk.yellow(message) : message;
  prerr_endline(message);
};

let error = (~prefix=true, ~color=true, ~wrap=true, message: string): unit => {
  let message = prefix ? "[Error] " ++ message : message;
  let message = wrap ? wrapString(120, message) : message;
  let message = color ? Chalk.red(message) : message;
  prerr_endline(message);
};

let fatal =
    (~prefix=true, ~color=true, ~wrap=true, ~errorCode=1, message: string): 'a => {
  let message = prefix ? "[Fatal] " ++ message : message;
  let message = wrap ? wrapString(120, message) : message;
  let message = color ? Chalk.red(message) : message;
  prerr_endline(message);
  exit(errorCode);
};
