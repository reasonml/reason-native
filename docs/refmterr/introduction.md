---
id: introduction
title: Refmterr Introduction
sidebar_label: Introduction
---

Refmterr is an error formatter for Reason and OCaml builds. It takes raw error output from both the Reason and OCaml compilers and converts it to pretty output like so:

```perl
## Before ##

File "type_AppliedTooMany_3.ml", line 11, characters 5-30:
Error: The function applied to this argument has type
         customLogOutputProcessors:(string -> string) list ->
         customErrorParsers:(string * string list) list -> unit
This argument cannot be applied with label ~raiseExceptionDuringParse
```

```sh-stacked
 ## After ##

[31m[1m[7m ERROR [27m[22m[39m [36m[4mtype_AppliedTooMany_3.ml[24m[39m[2m[4m:11 5-30[24m[22m

[2m 8 ┆ [22m[35m[2mlet[22m[39m[2m _ =[22m
[2m 9 ┆ [22m[2m  parseFromStdin[22m
[2m10 ┆ [22m[2m    [22m[33m[2m~refmttypePath[22m[39m
[31m[2m11 ┆ [22m[39m[2m    ~[22m[31m[1m[4mraiseExceptionDuringParse[24m[22m[39m[2m=true[22m
[2m12 ┆ [22m[2m    [22m[33m[2m~customLogOutputProcessors[22m[39m[2m=[][22m
[2m13 ┆ [22m[2m    [22m[33m[2m~customErrorParsers[22m[39m[2m:[][22m

The function has type (
  ~customLogOutputProcessors: list(string => string),
  ~customErrorParsers: list((string, list(string)))
) =>
unit

This function doesn't accept an argument named ~raiseExceptionDuringParse.
```
