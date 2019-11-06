/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
open Common.Option.Infix;
open TestResult;
open TestResult.AggregatedResult;
open Time;

type terminalPrinter = {
  printString: string => unit,
  printEndline: string => unit,
  printNewline: unit => unit,
  flush: out_channel => unit,
};

let createTerminalReporter =
    (
      ~getTime: unit => Time.t=Clock.getTime,
      ~onlyPrintDetailsForFailedSuites=false,
      printer: terminalPrinter,
    )
    : Reporter.t => {
  let newLineRegex = Re.Pcre.regexp("\n");

  let indent = (~indent: string, s: string): string => {
    let lines = Re.Pcre.split(newLineRegex, s);
    let lines = List.map(line => indent ++ line, lines);
    String.concat("\n", lines);
  };

  let failFormatter = s => Pastel.red(s);
  let messageIndent = "    ";
  let stackIndent = "      ";
  let titleIndent = "  ";
  let passDisplay = () =>
    <Pastel color=Green inverse=true bold=true> " PASS " </Pastel>;
  let failDisplay = () =>
    <Pastel color=Red inverse=true bold=true> " FAIL " </Pastel>;
  let runningDisplay = () =>
    <Pastel color=Yellow inverse=true bold=true> " RUNS " </Pastel>;

  module FCP =
    FileContextPrinter.Make({
      let config =
        FileContextPrinter.Config.initialize({linesBefore: 3, linesAfter: 3});
    });

  let getStackInfo = (optLoc: option(Printexc.location), trace: string) => {
    let stackInfo =
      optLoc
      >>= (
        (l: Printexc.location) =>
          FCP.printFile(
            l.filename,
            (
              /* File-context-printer expects line number and column number
               * to both be 1-indexed, however the locations from Printexc
               * have 1 indexed lines and 0 indexed columns*/
              (l.line_number, l.start_char + 1),
              (l.line_number, l.end_char + 1),
            ),
          )
      )
      >>| (
        context =>
          String.concat(
            "\n\n",
            [
              indent(context, ~indent=stackIndent),
              indent(trace, ~indent=stackIndent),
            ],
          )
      );
    stackInfo;
  };

  let gatherFormattedFailureOutput = (testResults: list(testResult)) =>
    testResults
    |> List.fold_left(
         (acc, r) =>
           switch (r) {
           | {testStatus: Skipped(_), _}
           | {testStatus: Passed(_), _} => acc
           | {testStatus: Failed(message, loc, stack), fullName, _} =>
             let titleBullet = "• ";
             let title =
               Pastel.bold(
                 failFormatter(titleIndent ++ titleBullet ++ fullName),
               );
             let indentedMessage = indent(message, ~indent=messageIndent);
             let parts =
               switch (getStackInfo(loc, stack)) {
               | Some(trace) => [title, indentedMessage, trace]
               | None => [title, indentedMessage]
               };
             let formattedOutput = String.concat("\n\n", parts);
             acc @ [formattedOutput];
           | {testStatus: Exception(e, loc, stack), fullName, _} =>
             let titleBullet = "• ";
             let title =
               Pastel.bold(
                 failFormatter(titleIndent ++ titleBullet ++ fullName),
               );
             let exceptionMessage =
               indent(
                 "Exception " ++ Pastel.dim(Printexc.to_string(e)),
                 ~indent=messageIndent,
               );
             let parts =
               switch (getStackInfo(loc, stack)) {
               | Some(trace) => [title, exceptionMessage, trace]
               | None => [title, exceptionMessage]
               };
             let formattedOutput = String.concat("\n\n", parts);
             acc @ [formattedOutput];
           },
         [],
       );

  let createRunSummary = (result: AggregatedResult.t) => {
    let testSuitePassFormatter =
      result.numPassedTestSuites > 0
        ? s => <Pastel color=Green bold=true> s </Pastel> : (s => s);
    let testSuiteSkipFormatter =
      result.numPassedTestSuites > 0
        ? s => <Pastel color=Yellow bold=true> s </Pastel> : (s => s);
    let testSuiteFailFormatter =
      result.numFailedTestSuites > 0
        ? s => <Pastel color=Red bold=true> s </Pastel> : (s => s);
    let testPassFormatter =
      result.numPassedTests > 0
        ? s => <Pastel color=Green bold=true> s </Pastel> : (s => s);
    let testSkipFormatter =
      result.numPassedTests > 0
        ? s => <Pastel color=Yellow bold=true> s </Pastel> : (s => s);
    let testFailFormatter =
      result.numFailedTests > 0
        ? s => <Pastel color=Red bold=true> s </Pastel> : (s => s);
    let testSuiteSummaryParts =
      [
        Some(
          testSuiteFailFormatter(
            string_of_int(result.numFailedTestSuites) ++ " failed",
          ),
        ),
        result.numSkippedTestSuites == 0
          ? None
          : Some(
              testSuiteSkipFormatter(
                string_of_int(result.numSkippedTestSuites) ++ " skipped",
              ),
            ),
        Some(
          testSuitePassFormatter(
            string_of_int(result.numPassedTestSuites) ++ " passed",
          ),
        ),
        Some(string_of_int(result.numTotalTestSuites) ++ " total"),
      ]
      |> List.fold_left(
           (acc, part) =>
             switch (part) {
             | Some(part) => acc @ [part]
             | None => acc
             },
           [],
         );
    let testSuiteSummary =
      String.concat(
        "",
        [
          <Pastel bold=true color=WhiteBright> "Test Suites: " </Pastel>,
          String.concat(", ", testSuiteSummaryParts),
        ],
      );
    let testSummaryParts =
      [
        Some(
          testFailFormatter(
            string_of_int(result.numFailedTests) ++ " failed",
          ),
        ),
        result.numSkippedTests == 0
          ? None
          : Some(
              testSkipFormatter(
                string_of_int(result.numSkippedTests) ++ " skipped",
              ),
            ),
        Some(
          testPassFormatter(
            string_of_int(result.numPassedTests) ++ " passed",
          ),
        ),
        Some(string_of_int(result.numTotalTests) ++ " total"),
      ]
      |> List.fold_left(
           (acc, part) =>
             switch (part) {
             | Some(part) => acc @ [part]
             | None => acc
             },
           [],
         );
    let testSummary =
      String.concat(
        "",
        [
          <Pastel bold=true color=WhiteBright> "Tests:       " </Pastel>,
          String.concat(", ", testSummaryParts),
        ],
      );
    let Seconds(duration) = Time.subtract(getTime(), result.startTime);
    let timeString =
      duration *. 1000. > 1. ? Printf.sprintf("%.3fs", duration) : "< 1ms";
    let timeSummary =
      String.concat(
        "",
        [
          <Pastel bold=true color=WhiteBright> "Time:        " </Pastel>,
          timeString,
        ],
      );

    String.concat("\n", [testSuiteSummary, testSummary, timeSummary]);
  };

  let printSnapshotStatus = testResult =>
    testResult.snapshotSummary
    >>= (
      snapshotSummary => {
        let {numCreatedSnapshots, numRemovedSnapshots, numUpdatedSnapshots} = snapshotSummary;
        let messages: ref(list(string)) = ref([]);
        if (numCreatedSnapshots > 0) {
          messages :=
            messages^
            @ [
              Pastel.yellow(
                String.concat(
                  "",
                  [
                    "Created ",
                    string_of_int(numCreatedSnapshots),
                    " missing snapshots",
                  ],
                ),
              ),
            ];
        };

        if (numUpdatedSnapshots > 0) {
          messages :=
            messages^
            @ [
              Pastel.yellow(
                String.concat(
                  "",
                  [
                    "Updated ",
                    string_of_int(numUpdatedSnapshots),
                    " snapshots",
                  ],
                ),
              ),
            ];
        };

        if (numRemovedSnapshots > 0) {
          messages :=
            messages^
            @ [
              Pastel.yellow(
                String.concat(
                  "",
                  [
                    "Removed ",
                    string_of_int(numRemovedSnapshots),
                    " unused snapshots",
                  ],
                ),
              ),
            ];
        };
        if (List.length(messages^) > 0) {
          Some(String.concat("\n", messages^));
        } else {
          None;
        };
      }
    );

  let runningDisplayLength = ref(0);
  {
    onTestSuiteStart: (testSuite: Reporter.testSuite) =>
      if (Pastel.getMode() == Terminal) {
        let update =
          String.concat(
            " ",
            [
              runningDisplay(),
              <Pastel color=WhiteBright> {testSuite.name} </Pastel>,
            ],
          );
        runningDisplayLength := String.length(update);
        printer.printString("\r" ++ update);
        printer.flush(stdout);
        ();
      },
    onTestSuiteResult: (aggregatedResult, testSuite, testSuiteResult) => {
      if (Pastel.getMode() == Terminal) {
        printer.printString(
          "\027[" ++ string_of_int(runningDisplayLength^) ++ "D\027[K",
        );
      };
      switch (testSuiteResult) {
      | {numFailedTests: 0, numPassedTests: 0, numSkippedTests} => ()
      | {numFailedTests: 0, numPassedTests: n, testResults: _, displayName} =>
        onlyPrintDetailsForFailedSuites
          ? ()
          : printer.printEndline(
              String.concat(
                " ",
                [
                  passDisplay(),
                  <Pastel color=WhiteBright> displayName </Pastel>,
                ],
              ),
            )
      | {numFailedTests: n, numPassedTests, testResults, displayName}
          when n > 0 =>
        printer.printEndline(
          String.concat(
            " ",
            [failDisplay(), <Pastel color=WhiteBright> displayName </Pastel>],
          ),
        );
        printer.printEndline(
          String.concat("\n\n", gatherFormattedFailureOutput(testResults)),
        );
        printer.printEndline("");
      | _ => raise(Invalid_argument("todo handle"))
      };
    },
    onRunStart: runStartInfo => {
      let numTestSuites = List.length(runStartInfo.testSuites);
      printer.printEndline(
        "Running "
        ++ string_of_int(numTestSuites)
        ++ (numTestSuites == 1 ? " test suite" : " test suites"),
      );
    },
    onRunComplete: aggregatedResult => {
      printer.printEndline(
        "\n" ++ createRunSummary(aggregatedResult) ++ "\n",
      );
      let _ = aggregatedResult |> printSnapshotStatus >>| printer.printEndline;
      ();
    },
  };
};
