open Refmterr;

let _ = DummyFile.executeCode();

let myUtils = Refmterr.StackUtils.makeStackUtils({exclude: [], baseDir: "."});

module Reporter = Refmterr.Reporter.Make(Refmterr.Stylish.ANSIStylish);

Console.log(myUtils.stackTraceToString(myUtils.getStackTrace()));

let location = myUtils.getTopLocation();

module MyFileContextPrinterModule = Refmterr.FileContextPrinter.Make({let linesBefore = 3; let linesAfter = 3}, Refmterr.Stylish.ANSIStylish);

switch(location) {
| Some(loc) => {
    let contents = Helpers.fileLinesOfExn(loc.filename);
    let highlight = ((loc.line_number - 1, loc.start_char), (loc.line_number - 1, loc.end_char));
    let rev_result = Reporter._printFile(~titleLine=loc.filename, ~highlightColor=Refmterr.Stylish.ANSIStylish.yellow, ~highlight, contents);
    print_endline("context: ")
    print_endline(String.concat("\n", List.rev(rev_result)));
    Console.log(loc);
    print_endline("extracted code output:");

    let range = ((loc.line_number, loc.start_char), (loc.line_number, loc.end_char));
    let result = MyFileContextPrinterModule.print(loc.filename, range);
    print_endline(result);
}
| None => print_endline("no location");
}
