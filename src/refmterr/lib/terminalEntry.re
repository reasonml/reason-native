/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 *
 * LICENSE file in the root directory of this source tree.
 */;
module TerminalReporter = Reporter.Make(Stylish.ANSIStylish);
open BetterErrorsTypes;
open Index;
open Helpers;

let parseFromStdin = (~refmttypePath, ~customLogOutputProcessors, ~customErrorParsers) => {
  let reverseErrBuffer = {contents: []};
  let prettyPrintParsedResult = TerminalReporter.prettyPrintParsedResult(~refmttypePath);
  let forEachLine = (line) =>
    switch (
      reverseErrBuffer.contents,
      Re.Pcre.pmatch(~rex=fileR, line),
      Re.Pcre.pmatch(~rex=hasErrorOrWarningR, line),
      Re.Pcre.pmatch(~rex=hasIndentationR, line)
    ) {
    | ([], false, false, false) =>
      /* no error, just stream on the line */
      print_endline(TerminalReporter.processLogOutput(~customLogOutputProcessors, line))
    | ([], true, _, _)
    | ([], _, true, _)
    | ([], _, _, true) =>
      /* the beginning of a new error! */
      reverseErrBuffer.contents = [line]
    /* don't parse it yet. Maybe the error's continuing on the next line */
    | (_, true, _, _) =>
      /* we have a file match, AND the current reverseErrBuffer isn't empty? We'll
         just assume here that this is also the beginning of a new error, unless
         a single error might span many (non-indented, god forbid) fileNames.
         Print out the current (previous) error and keep accumulating */
      let bufferText = revBufferToStr(reverseErrBuffer.contents);
      parse(~customLogOutputProcessors, ~customErrorParsers, bufferText)
      |> prettyPrintParsedResult(~originalRevLines=reverseErrBuffer.contents)
      |> revBufferToStr
      |> print_endline;
      reverseErrBuffer.contents = [line]
    /* buffer not empty, and we're seeing an error/indentation line. This is
       the continuation of a currently streaming error/warning */
    | (_, _, _, true)
    | (_, _, true, _) => reverseErrBuffer.contents = [line, ...reverseErrBuffer.contents]
    | (_, false, false, false) =>
      /* woah this case was previously forgotten but caught by the
             compiler. Man I don't ever wanna write an if-else anymore
             buffer not empty, and no indentation and not an error/file
             line? This means the previous error might have ended. We say
             "might" because some errors provide non-indented messages...
             here's one such case (hasHintR). And here's another:
             Error: The function applied to this argument has type
                      customLogOutputProcessors:(string -> string) list ->
                      customErrorParsers:(string * string list) list -> unit
             This argument cannot be applied with label ~raiseExceptionDuringParse
         */
      if (Re.Pcre.pmatch(~rex=hasHintR, line)
          || Re.Pcre.pmatch(~rex=argCannotBeAppliedWithLabelR, line)
          || Re.Pcre.pmatch(~rex=notVisibleInCurrentScopeR, line)
          || Re.Pcre.pmatch(~rex=theyWillNotBeSelectedR, line)) {
        reverseErrBuffer.contents =
          [line, ...reverseErrBuffer.contents]
          /* let bufferText = revBufferToStr(reverseErrBuffer.contents);
           * parse(~customLogOutputProcessors, ~customErrorParsers, bufferText)
           * |> prettyPrintParsedResult(~originalRevLines=reverseErrBuffer.contents)
           * |> revBufferToStr
           * |> print_endline;
           * reverseErrBuffer.contents = []
           */
      } else {
        let bufferText = revBufferToStr(reverseErrBuffer.contents);
        parse(~customLogOutputProcessors, ~customErrorParsers, bufferText)
        |> prettyPrintParsedResult(~originalRevLines=reverseErrBuffer.contents)
        |> revBufferToStr
        |> print_endline;
        reverseErrBuffer.contents = [line]
      }
    };
  try {
    line_stream_of_channel(stdin) |> Stream.iter(forEachLine);
    /* might have accumulated a few more lines */
    if (reverseErrBuffer.contents !== []) {
      let bufferText = revBufferToStr(reverseErrBuffer.contents);
      parse(~customLogOutputProcessors, ~customErrorParsers, bufferText)
      |> prettyPrintParsedResult(~originalRevLines=reverseErrBuffer.contents)
      |> revBufferToStr
      |> print_endline
    };
    close_in(stdin)
  } {
  | e =>
    close_in(stdin);
    raise(e)
  }
};
