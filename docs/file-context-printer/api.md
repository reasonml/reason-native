---
id: api
title: File Context Printer API
sidebar_label: API
---

> Prefer reading code? Check out [FileContextPrinter.rei](https://github.com/facebookexperimental/reason-native/blob/master/src/file-context-printer/FileContextPrinter.rei)

## Configuration Options

### `linesBefore`

Lines of code to show before specified context.
From there, extract and print file context like so:

```reason
module FCP =
  FileContextPrinter.Make({
    let config =
      FileContextPrinter.Config.initialize({linesBefore: 1, linesAfter: 3});
  });
FCP.printFile(
  "src/file-context-printer/test/DummyFile.re",
  ((7, 1), (7, 11)),
)
```

```bash-ansi
[2m 6 ┆ [22m[2m */;[22m
[31m[2m 7 ┆ [22m[39m[31m[1m[4mlet myFunc[24m[22m[39m[2mtion = ()[22m[31m[2m => [22m[39m[2mprint_endline([22m[32m[2m"do something"[22m[39m[2m);[22m
[2m 8 ┆ [22m
[2m 9 ┆ [22m[35m[2mlet[22m[39m[2m ex = (arg)[22m[31m[2m => [22m[39m[2m{[22m
[2m10 ┆ [22m[2m    [22m[33m[2mif[22m[39m[2m(arg === true) {[22m
```

### `linesAfter`

Lines of code to show before specified context.
From there, extract and print file context like so:

```reason
module FCP =
  FileContextPrinter.Make({
    let config =
      FileContextPrinter.Config.initialize({linesBefore: 3, linesAfter: 1});
  });
FCP.printFile(
  "src/file-context-printer/test/DummyFile.re",
  ((7, 1), (7, 11)),
)
```

```bash-ansi
[2m 4 ┆ [22m[2m * [22m[34m[2mThis[22m[39m[2m source code is licensed under the ...
[2m 5 ┆ [22m[2m * [22m[34m[2mLICENSE[22m[39m[2m file in the root directory of ...
[2m 6 ┆ [22m[2m */;[22m
[31m[2m 7 ┆ [22m[39m[31m[1m[4mlet myFunc[24m[22m[39m[2mtion = ()[22m[31m[2m => [22m[39m[2mprint_endline([22m[32m[2m"do something"[22m[39m[2m);[22m
[2m 8 ┆ [22m
```

## Methods

### `printFile`

Prints context from a file.

Signature:

```reason
let printFile: (~path: string, ~highlight: rowColumnRange) => option(string);
```

Example:

```reason
FCP.printFile(
  "src/file-context-printer/test/DummyFile.re",
  ((7, 1), (7, 11)),
)
```

```bash-ansi
[2m 6 ┆ [22m[2m */;[22m
[31m[2m 7 ┆ [22m[39m[31m[1m[4mlet myFunc[24m[22m[39m[2mtion = ()[22m[31m[2m => [22m[39m[2mprint_endline([22m[32m[2m"do something"[22m[39m[2m);[22m
[2m 8 ┆ [22m
```

### `print`

Prints context from a list of lines.

Signature:

```reason
let print: (list(string), ~highlight: rowColumnRange) => string;
```

Example:

```reason
FCP.print(
  [
    "/**",
    " * Copyright (c) Facebook, Inc. and its affiliates."
    " *",
    " * This source code is licensed under the MIT license found in the",
    " * LICENSE file in the root directory of this source tree.",
    " */;",
    "let myFunction = () => print_endline(\"do something\");",
    "",
    "let ex = (arg) => {",
    "  if(arg === true) {",
    "    print_endline(\"raising an error\");",
    "    raise(Not_found);",
    "  }",
    "}",
  ],
  ((7, 1), (7, 11)),
)
```

```bash-ansi
[2m 6 ┆ [22m[2m */;[22m
[31m[2m 7 ┆ [22m[39m[31m[1m[4mlet myFunc[24m[22m[39m[2mtion = ()[22m[31m[2m => [22m[39m[2mprint_endline([22m[32m[2m"do something"[22m[39m[2m);[22m
[2m 8 ┆ [22m
```
