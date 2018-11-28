/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
module FCP = FileContextPrinter.Make({let linesBefore = 3; let linesAfter = 3});

let result = switch(FCP.print("src/file-context-printer/test/DummyFile.re", ((7,1), (7,10)))) {
| Some(context) => context
| None => "make sure you run the test command from the root of the file-context-printer directory"
}

print_endline(result);
