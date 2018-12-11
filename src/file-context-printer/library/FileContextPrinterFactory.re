/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

/** A type representing a 1-indexed row/column range */
open Config;

type rowColumnRange = ((int, int), (int, int));

module Make = (UserConfig: FileContextPrinterConfig, Styl: Stylish.StylishSig) => {
  module Printer = Printer.Make(Styl);

  open Helpers;
  open Printer;
  open Styl;
  open UserConfig;

  let pad = (~ch=' ', content, n) =>
    String.make(n - String.length(content), ch) ++ content;

  let numberOfDigits = n => {
    let digits = ref(1);
    let nn = ref(n);
    while (nn^ / 10 > 0) {
      nn := nn^ / 10;
      digits := digits^ + 1;
    };
    digits^;
  };

  let startingSpacesCount = str => {
    let rec startingSpacesCount' = (str, idx) =>
      if (idx == String.length(str)) {
        idx;
      } else if (str.[idx] != ' ') {
        idx;
      } else {
        startingSpacesCount'(str, idx + 1);
      };
    startingSpacesCount'(str, 0);
  };

  let print = (content: list(string), ~highlight: rowColumnRange) => {
    let ((startRow, startColumn), (endRow, endColumn)) = highlight;
    /* The rest of this method was written assuming that the range is zero indexed
       , however given that this module is scoped explicitly to files, whose lines are actually one indexed
       we convert here rather than rewrite everything else (which should probably also be done at some point) */
    let startRow = startRow - 1;
    let endRow = endRow - 1;
    let startColumn = max(0, startColumn - 1);
    let endColumn = endColumn - 1;
    let displayedStartRow = max(0, startRow - config.linesBefore);
    /* we display no more than 3 lines after startRow. Some endRow are rly far
       away */
    let displayedEndRow =
      min(List.length(content) - 1, startRow + config.linesAfter);
    let lineNumWidth = numberOfDigits(List.length(content));
    /* sometimes the snippet of file we show is really indented. We de-indent it
       for nicer display by trimming out the maximum amount of leading spaces we can. */
    let rowsForCountingStartingSpaces =
      listDrop(displayedStartRow, content)
      |> listTake(displayedEndRow - displayedStartRow + 1)
      |> List.filter(row => row != "");
    let minIndent =
      switch (rowsForCountingStartingSpaces) {
      | [] => 0
      | _ =>
        let startingSpaces =
          List.map(startingSpacesCount, rowsForCountingStartingSpaces);
        List.fold_left(
          (acc, num) => num < acc ? num : acc,
          List.hd(startingSpaces),
          startingSpaces,
        );
      };
    /*
     * TODO: Create a better vertical separator for the second case.
     */
    let sep = minIndent === 0 ? " ┆ " : " ┆ ";
    let startColumn = startColumn - minIndent;
    let endColumn = endColumn - minIndent;
    /* Sometimes a file based error will have start=end column == 0 */
    let revResult = ref([]);
    for (i in displayedStartRow to displayedEndRow) {
      let currLine = List.nth(content, i) |> stringSlice(~first=minIndent);
      if (i >= startRow && i <= endRow) {
        if (startRow == endRow) {
          /* Have to process from the end content first because it changes the
           * range. */
          /* Note: ~last/endColumn /endColumn is not really the last index to highlight but one beyond it. */
          let highlightedEnd =
            highlightSource(
              ~dim=true,
              stringSlice(~first=endColumn, currLine),
            );
          let highlightedMiddle =
            red(
              ~bold=true,
              ~underline=true,
              ~dim=false,
              stringSlice(~first=startColumn, ~last=endColumn, currLine),
            );
          let highlightedBeginning =
            highlightSource(
              ~dim=true,
              stringSlice(~last=startColumn, currLine),
            );
          let highlighted =
            highlightedBeginning ++ highlightedMiddle ++ highlightedEnd;
          revResult.contents = [
            red(~dim=true, pad(string_of_int(i + 1), lineNumWidth) ++ sep)
            ++ highlighted,
            ...revResult.contents,
          ];
        } else if (i == startRow) {
          let highlightedEnd =
            red(
              ~bold=true,
              ~underline=true,
              stringSlice(~first=startColumn, currLine),
            );
          let highlightedBeginning =
            highlightSource(
              ~dim=true,
              stringSlice(~last=startColumn, currLine),
            );
          let highlighted = highlightedBeginning ++ highlightedEnd;
          revResult.contents = [
            red(~dim=true, pad(string_of_int(i + 1), lineNumWidth) ++ sep)
            ++ highlighted,
            ...revResult.contents,
          ];
        } else if (i == endRow) {
          /* startColumn is garbage for the end row or middle rows! */
          let endStr = stringSlice(~first=endColumn, currLine);
          let beginningStr = stringSlice(~last=endColumn, currLine);
          /* In the middle of the error. */
          let (white, trimmedBeginningStr) =
            splitLeadingWhiteSpace(beginningStr);
          let highlightedBeginningStr =
            white ++ red(~bold=true, ~underline=true, trimmedBeginningStr);
          let highlightedEndStr = highlightSource(~dim=true, endStr);
          let highlighted = highlightedBeginningStr ++ highlightedEndStr;
          revResult.contents = [
            red(~dim=true, pad(string_of_int(i + 1), lineNumWidth) ++ sep)
            ++ highlighted,
            ...revResult.contents,
          ];
        } else {
          /* In the middle of the error. */
          let (white, trimmed) = splitLeadingWhiteSpace(currLine);
          revResult.contents = [
            red(~dim=true, pad(string_of_int(i + 1), lineNumWidth) ++ sep)
            ++ white
            ++ red(~bold=true, ~underline=true, trimmed),
            ...revResult.contents,
          ];
        };
      } else {
        revResult.contents = [
          dim(pad(string_of_int(i + 1), lineNumWidth) ++ sep)
          ++ highlightSource(~dim=true, currLine),
          ...revResult.contents,
        ];
      };
    };
    revResult.contents |> List.rev |> String.concat("\n");
  };

  let printFile = (~path: string, ~highlight: rowColumnRange) => {
    let fileContents =
      switch (Helpers.fileLinesOfExn(path)) {
      | fileLines => Some(print(fileLines, ~highlight))
      | exception _e => None
      };
    fileContents;
  };
};
