/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Types_t;

open Helpers;

/* the compiler output might point to an error that spans across many lines;
   however, instead of indicating from (startRow, startColumn) to (endRow,
   endColumn), it'll indicate (startRow, startColumn, endColumn) where endColumn
   might belong to a different row! We normalize and find the row here */
/* the compiler line number is 1-indexed, and col number is 0-indexed but the
   endColumn for an error goes past the last "expected" endColumn, e.g. if it's
   `typ a = string`
   instead of saying it's from 0 to 2, it shows as 0 to 3. This is also kinda
   expected, since you get easy column count through 3 - 0 */
/* we'll use 0-indexed. It's a reporter (printer)'s job to normalize to
   1-indexed if it desires so */
let normalizeCompilerLineColsToRange =
    (~fileLines, ~lineRaw, ~col1Raw, ~col2Raw) => {
  /* accept strings to constraint usage to parse directly from raw data */
  let line = int_of_string(lineRaw);
  let fileLength = List.length(fileLines);
  let isOCamlBeingBadAndPointingToALineBeyondFileLength = line > fileLength;
  let (col1, col2) =
    if (isOCamlBeingBadAndPointingToALineBeyondFileLength) {
      let lastDamnReachableSpotInTheFile =
        String.length(List.nth(fileLines, fileLength - 1));
      (lastDamnReachableSpotInTheFile - 1, lastDamnReachableSpotInTheFile);
    } else {
      switch (col1Raw, col2Raw) {
      | (Some(a), Some(b)) => (int_of_string(a), int_of_string(b))
      /* some error msgs don't have column numbers; we normal them to 0 here */
      | _ => (0, 0)
      };
    };
  let startRow =
    if (isOCamlBeingBadAndPointingToALineBeyondFileLength) {
      fileLength - 1;
    } else {
      line - 1;
    };
  let currentLine = List.nth(fileLines, startRow);
  let numberOfCharsBetweenStartAndEndColumn = col2 - col1;
  let numberOfCharsLeftToCoverOnStartingRow =
    /* +1 bc ocaml looooves to count new line as a char below when the error
       spans multiple lines*/
    String.length(currentLine) - col1 + 1;
  if (numberOfCharsBetweenStartAndEndColumn
      <= numberOfCharsLeftToCoverOnStartingRow) {
    ((startRow, col1), (startRow, col2));
  } else {
    let howManyCharsLeftToCoverOnSubsequentLines =
      ref(
        numberOfCharsBetweenStartAndEndColumn
        - numberOfCharsLeftToCoverOnStartingRow,
      );
    let suddenlyFunctionalProgrammingOutOfNowhere =
      fileLines
      |> Helpers.listDrop(startRow + 1)
      |> List.map(String.length)
      |> Helpers.listTakeWhile(numberOfCharsOnThisLine =>
           if (howManyCharsLeftToCoverOnSubsequentLines^
               > numberOfCharsOnThisLine) {
             howManyCharsLeftToCoverOnSubsequentLines :=
               howManyCharsLeftToCoverOnSubsequentLines^
               - numberOfCharsOnThisLine
               - 1;
             true;
           } else {
             false;
           }
         );
    let howManyMoreRowsCoveredSinceStartRow =
      1 + List.length(suddenlyFunctionalProgrammingOutOfNowhere);
    (
      (startRow, col1),
      (
        startRow + howManyMoreRowsCoveredSinceStartRow,
        howManyCharsLeftToCoverOnSubsequentLines^,
      ),
    );
  };
};

/* has the side-effect of reading the file */
let extractFromFileMatch = fileMatch =>
  Re.Pcre.(
    switch (fileMatch) {
    | [
        Delim(_),
        Group(_, filePath),
        Group(_, lineNum),
        col1,
        col2,
        Text(body),
      ] =>
      let cachedContent = Helpers.fileLinesOfExn(filePath);
      /* sometimes there's only line, but no characters */
      let (col1Raw, col2Raw) =
        switch (col1, col2) {
        | (Group(_, c1), Group(_, c2)) =>
          /* bug:
               https://github.com/mmottl/pcre-ocaml/issues/5
             */
          if (String.trim(c1) == "" || String.trim(c2) == "") {
            (None, None);
          } else {
            (Some(c1), Some(c2));
          }
        | _ => (None, None)
        };
      (
        filePath,
        cachedContent,
        normalizeCompilerLineColsToRange(
          ~fileLines=cachedContent,
          ~lineRaw=lineNum,
          ~col1Raw,
          ~col2Raw,
        ),
        /* important, otherwise leaves random blank lines that defies some of
           our regex logic, maybe */
        String.trim(body),
      );
    | _ => raise(invalid_arg("Couldn't extract error"))
    }
  );

/* debug helper */
let printFullSplitResult =
  List.iteri((i, x) => {
    print_int(i);
    print_endline("");
    Re.Pcre.(
      switch (x) {
      | Delim(a) => print_endline("Delim " ++ a)
      | Group(_, a) => print_endline("Group " ++ a)
      | Text(a) => print_endline("Text " ++ a)
      | NoGroup => print_endline("NoGroup")
      }
    );
  });

let fileR =
  Re.Pcre.regexp(
    ~flags=[Re.Pcre.(`MULTILINE)],
    {|^File "([\s\S]+?)", line (\d+)(?:, characters (\d+)-(\d+))?:$|},
  );

let hasErrorOrWarningR =
  Re.Pcre.regexp(~flags=[Re.Pcre.(`MULTILINE)], {|^(Error|Warning \d+): |});

let hasIndentationR =
  Re.Pcre.regexp(~flags=[Re.Pcre.(`MULTILINE)], {|^       +|});

/* TODO: make the below work. the "Here is an example..." is followed by even more lines of hints */
/* let hasHintRStr = {|^(Hint: Did you mean |Here is an example of a value that is not matched:)|} */
/* let hasHintRStr = {|^(Here is an example of a value that is not matched:|Hint: Did you mean )|} */
let hasHintRStr = {|^Hint: Did you mean |};

let argCannotBeAppliedWithLabelRStr = {|^This argument cannot be applied with label|};

let hasHintR = Re.Pcre.regexp(~flags=[Re.Pcre.(`MULTILINE)], hasHintRStr);

let argCannotBeAppliedWithLabelR =
  Re.Pcre.regexp(
    ~flags=[Re.Pcre.(`MULTILINE)],
    argCannotBeAppliedWithLabelRStr,
  );

let notVisibleInCurrentScopeStr = {|^not visible in the current scope|};

let notVisibleInCurrentScopeR =
  Re.Pcre.regexp(~flags=[Re.Pcre.(`MULTILINE)], notVisibleInCurrentScopeStr);

let theyWillNotBeSelectedStr = {|^They will not be selected|};

let theyWillNotBeSelectedR =
  Re.Pcre.regexp(~flags=[Re.Pcre.(`MULTILINE)], theyWillNotBeSelectedStr);

let parse = (~customErrorParsers, err) => {
  /* we know whatever err is, it starts with "File: ..." because that's how `parse`
     is used */
  let err = String.trim(err);
  try (
    switch (Re.Pcre.full_split(~rex=fileR, err)) {
    | [
        Re.Pcre.Delim(_),
        Group(_, filePath),
        Group(_, lineNum),
        col1,
        col2,
        Text(body),
      ] =>
      /* important, otherwise leaves random blank lines that defies some of
         our regex logic, maybe */
      let body = String.trim(body);
      let errorCapture = get_match_maybe({|^Error: ([\s\S]+)|}, body);
      switch (
        ParseError.specialParserThatChecksWhetherFileEvenExists(
          filePath,
          errorCapture,
        )
      ) {
      | Some(err) => err
      | None =>
        let cachedContent = Helpers.fileLinesOfExn(filePath);
        /* sometimes there's only line, but no characters */
        let (col1Raw, col2Raw) =
          switch (col1, col2) {
          | (Group(_, c1), Group(_, c2)) =>
            /* bug: https://github.com/mmottl/pcre-ocaml/issues/5
             */
            if (String.trim(c1) == "" || String.trim(c2) == "") {
              raise(Invalid_argument("HUHUHUH"));
            } else {
              (Some(c1), Some(c2));
            }
          | _ => (None, None)
          };
        let range =
          normalizeCompilerLineColsToRange(
            ~fileLines=cachedContent,
            ~lineRaw=lineNum,
            ~col1Raw,
            ~col2Raw,
          );
        let warningCapture =
          switch (execMaybe({|^Warning (\d+): ([\s\S]+)|}, body)) {
          | None => (None, None)
          | Some(capture) => (
              getSubstringMaybe(capture, 1),
              getSubstringMaybe(capture, 2),
            )
          };
        switch (errorCapture, warningCapture) {
        | (Some(errorBody), (None, None)) =>
          `ErrorContent({
            filePath,
            cachedContent,
            range,
            parsedContent:
              ParseError.parse(
                ~customErrorParsers,
                ~errorBody,
                ~cachedContent,
                ~range,
              ),
          })
        | (None, (Some(code), Some(warningBody))) =>
          let code = int_of_string(code);
          `Warning({
            filePath,
            cachedContent,
            range,
            parsedContent: {
              code,
              warningType:
                ParseWarning.parse(
                  code,
                  warningBody,
                  filePath,
                  cachedContent,
                  range,
                ),
            },
          });
        | _ => raise(Invalid_argument(err))
        };
      };
    /* not an error, not a warning. False alarm? */
    | _ => `Unparsable
    }
  ) {
  | _ => `Unparsable
  };
};

let line_stream_of_channel = channel =>
  Stream.from(_ =>
    try (Some(input_line(channel))) {
    | End_of_file => None
    }
  );

/* entry point, for convenience purposes for now. Theoretically the parser and
      the reporters are decoupled.
      What about errors of the form:

   */
let revBufferToStr = revBuffer => String.concat("\n", List.rev(revBuffer));
