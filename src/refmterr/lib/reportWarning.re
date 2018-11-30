/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
module Make = (Styl: Stylish.StylishSig) => {
  module Printer = Printer.Make(Styl);

  open BetterErrorsTypes;
  open Helpers;
  open Printer;
  open Styl;

  /**
   * Returns a reverse ordered list of warning content.
   */
  let report = (~refmttypePath, code, filePath, parsedContent) =>
    switch (parsedContent) {
    | NoWarningExtracted => []
    | Warning_PatternNotExhaustive({unmatched}) => [
        "this match doesn't cover all possible values of the variant.",
        ...switch (unmatched) {
           | [oneVariant] => [
               sp(
                 "The case `%s` is not matched",
                 red(~bold=true, oneVariant),
               ),
             ]
           | many => [
               "These cases are not matched:",
               ...List.rev_map(s => yellow("- " ++ s), many),
             ]
           },
      ]
    | Warning_OptionalArgumentNotErased({argumentName}) => [
        "The solution is usually to provide a final non-named argument, or a final unit() argument if necessary.",
        "",
        sp(
          "%s is an optional argument at last position; calling the function by omitting %s might be confused with currying.",
          red(~bold=true, argumentName),
          argumentName,
        ),
      ]
    | Warning_BadFileName(offendingChar) => [
        sp(
          "File name potentially invalid. The OCaml ecosystem's build systems usually turn file names into module names by simply upper-casing the first letter. In this case, `%s` %s.\nNote: some build systems might e.g. turn kebab-case into CamelCase module, which is why this isn't a hard error.",
          /* "%s\n\n%s 24: \"%s\" isn't a valid file name; OCaml file names are often turned into modules, which need to start with a capitalized letter." */
          Filename.basename(filePath) |> String.capitalize,
          switch (offendingChar) {
          | Leading(ch) =>
            sp("starts with `%s`, which doesn't form a legal module name", ch)
          | Contains(ch) =>
            sp("contains `%s`, which doesn't form a legal module name", ch)
          | UnknownIllegalChar => "isn't a legal module name"
          },
        ),
      ]
    | _ => ["Error beautifier not implemented for this."]
    };
};
