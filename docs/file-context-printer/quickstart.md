---
id: quickstart
title: File Context Printer Quickstart
---

> This quickstart builds off [Getting Started](../getting-started) and assumes you have a native Reason project set up.

## Install Pastel

To install File Context Printer in your project with esy, run

```sh
esy add @reason-native/file-context-printer
```

This will add `@reason-native/file-context-printer` into your `package.json`.

In order to use File Context Printer, you must add it to your build system. With dune, `file-context-printer.lib` must be added to [the relevant `dune` file](https://jbuilder.readthedocs.io/en/latest/dune-files.html#library-dependencies):

```lisp
(libraries ... file-context-printer.lib)
```

## Sample Code

Start by initializing a File Context Printer module:

```reason
module FCP =
  FileContextPrinter.Make({
    let config =
      FileContextPrinter.Config.initialize({linesBefore: 3, linesAfter: 3});
  });
```

From there, extract and print file context like so:

```reason
let start = (
  7, /* start line */
  1 /* start char */
);
let end = (
  7, /* start line */
  11 /* start char */
);
FCP.printFile(
  "src/file-context-printer/test/DummyFile.re",
  (start, end),
)
```

```sh-stacked
[2m 4 ┆ [22m[2m * [22m[34m[2mThis[22m[39m[2m source code is licensed under the ...
[2m 5 ┆ [22m[2m * [22m[34m[2mLICENSE[22m[39m[2m file in the root directory of ...
[2m 6 ┆ [22m[2m */;[22m
[31m[2m 7 ┆ [22m[39m[31m[1m[4mlet myFunc[24m[22m[39m[2mtion = ()[22m[31m[2m => [22m[39m[2mprint_endline([22m[32m[2m"do something"[22m[39m[2m);[22m
[2m 8 ┆ [22m
[2m 9 ┆ [22m[35m[2mlet[22m[39m[2m ex = (arg)[22m[31m[2m => [22m[39m[2m{[22m
[2m10 ┆ [22m[2m    [22m[33m[2mif[22m[39m[2m(arg === true) {[22m
```
