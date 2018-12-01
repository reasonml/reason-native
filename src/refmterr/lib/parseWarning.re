/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open BetterErrorsTypes;

open Helpers;

/* agnostic extractors, turning err string into proper data structures */
/* TODO: don't make these raise error */
let warning_UnusedVariable = (code, err, _, _, _) => None;

/* need: what the variant is. If it's e.g. a list, instead of saying "doesn't
   cover all the cases of the variant" we could say "doesn't cover all the possible
   length of the list" */
let warning_PatternNotExhaustive = (code, err, _, _, _) => {
  let unmatchedR = {|this pattern-matching is not exhaustive.\sHere is an example of a value that is not matched:\s([\s\S]+)|};
  get_match_maybe(unmatchedR, err)
  |>? (
    unmatchedRaw => {
      let unmatched =
        if (unmatchedRaw.[0] == '(') {
          /* format was (Variant1|Variant2|Variant3). We strip the surrounding parens */
          unmatchedRaw
          |> Helpers.stringSlice(
               ~first=1,
               ~last=String.length(unmatchedRaw) - 1,
             )
          |> split({|\|[\s]*|});
        } else {
          [unmatchedRaw];
        };
      Some(Warning_PatternNotExhaustive({unmatched: unmatched}));
    }
  );
};

let warning_PatternUnused = (code, err, _, _, _) => None;

/* need: offending optional argument name from AST */
/* need: offending function name */
let warning_OptionalArgumentNotErased = (code, err, _, cachedContent, range) => {
  let ((startRow, startColumn), (endRow, endColumn)) = range;
  /* Hardcoding 16 for now. We might one day switch to use the variant from
     https://github.com/ocaml/ocaml/blob/901c67559469acc58935e1cc0ced253469a8c77a/utils/warnings.ml#L20 */
  let allR = {|this optional argument cannot be erased\.|};
  let fileLine = List.nth(cachedContent, startRow);
  get_match_n_maybe(0, allR, err)
  |>? (
    _ =>
      {
        let argumentNameRaw =
          Helpers.stringSlice(
            ~first=startColumn,
            ~last=
              if (startRow == endRow) {
                endColumn;
              } else {
                99999;
              },
            fileLine,
          );
        let argumentNameR = {|(:?\?\s*\()?([^=]+)|};
        get_match_n_maybe(2, argumentNameR, argumentNameRaw);
      }
      |>? (
        argumentName =>
          Some(
            Warning_OptionalArgumentNotErased({
              argumentName: String.trim(argumentName),
            }),
          )
      )
  );
};

/* need: what the variant is. If it's e.g. a list, instead of saying "doesn't
   cover all the cases of the variant" we could say "doesn't cover all the possible
   length of the list" */
let warning_BadFileName = (code, err, filePath, _, _) =>
  if (code == 24) {
    let fileName = Filename.basename(filePath);
    let offendingChar =
      switch (
        get_match_maybe({|^([^a-zA-Z])|}, fileName),
        get_match_maybe({|.+?([^a-zA-Z\.])|}, fileName),
      ) {
      | (Some(m), _) => Leading(m)
      | (None, Some(m)) => Contains(m)
      | _ => UnknownIllegalChar
      };
    Some(Warning_BadFileName(offendingChar));
  } else {
    None;
  };

/* TODO: better logic using these codes */
let parsers = [
  warning_UnusedVariable,
  warning_PatternNotExhaustive,
  warning_PatternUnused,
  warning_OptionalArgumentNotErased,
  warning_BadFileName,
];

let parse = (code, warningBody, filePath, cachedContent, range) => {
  let tryParser = parse' =>
    parse'(code, warningBody, filePath, cachedContent, range);
  try (Helpers.listFindMap(tryParser, parsers)) {
  | Not_found => NoWarningExtracted
  };
};
