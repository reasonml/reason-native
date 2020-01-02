---
id: introduction
title: "Console: No effort, universal logger"
sidebar_label: Introduction
---

`Console` is modelled after the browser console. It
doesn't require that you define any printers, and `Console.log/warn/error`
accept any type of input. Record fields and variant label names are lost at
compile time so they aren't printed.

**Suitable For Developers or Users:**

- Passing a single `string` (as in `Console.log("hello")`) will not print the
  quotes (just like in the browser)
- Passing a deep structure will print the deep structure, and strings that
  appear in the deep structure will include quotes.

This makes `Console` suitable for either logging messages displayed to the
user in command line apps, but also suitable for messages intended only for
the developer to read.

**No Effort**: Uses runtime checks to determine a likely appropriate printer
for your data without requiring any ppx plugins and without having to write
printers for your data types. Depth checking will prevent cyclical data from
printing infinitely.

**Best effort**: The basic data types are detected accurately at
runtime(`string`, `float`, `int`).

Record labels and variant names are not preserved at compilation time so are
not printed - but their data is still printed in the correct order.
Records/variant types that occupy more than one word in a block are all
printed as their corresponding integer (if a payloadless variant), or `{x, y,
...z}` for a record or variant that contains data.

```reason
Console.log(Some("hi")); /* {"hi"} */
Console.log(None); /* 0 */
```

**Extensible**: Allows customization of printing in order to change where the
logs are written to, how they are highlighted, and anything else you would
like.
