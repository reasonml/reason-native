---
id: introduction
title: File Context Printer Introduction
sidebar_label: Introduction
---

File Context Printer allows for quick and easy extraction of code snippets from Reason and OCaml files - it is used internally by [Rely](../rely) to print differing snapshot cases.

```reason
FCP.printFile(
  "src/file-context-printer/test/DummyFile.re",
  ((7, 1), (7, 11)),
);
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
