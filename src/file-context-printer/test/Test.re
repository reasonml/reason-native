module FCP = FileContextPrinter.Make({let linesBefore = 3; let linesAfter = 3});

let result = switch(FCP.print("test/DummyFile.re", ((1,1), (1,10)))) {
| Some(context) => context
| None => "make sure you run the test command from the root of the file-context-printer directory"
}

print_endline(result);
