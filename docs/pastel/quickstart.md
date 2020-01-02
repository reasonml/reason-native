---
id: quickstart
title: Pastel Quickstart
sidebar_label: Quickstart
---

> This quickstart builds off [Getting Started](../getting-started) and assumes you have a native Reason project set up.

## Install Pastel

To install Pastel in your project with esy, run

```sh
esy add @reason-native/pastel
```

This will add `@reason-native/pastel` into your `package.json`.

In order to use Pastel, you must add it to your build system. With dune, `pastel.lib` must be added to [the relevant `dune` file](https://jbuilder.readthedocs.io/en/latest/dune-files.html#library-dependencies):

```lisp
(libraries ... pastel.lib)
```

## Sample Code

Pastel is fundamentally a [Reason JSX element](https://reasonml.github.io/docs/en/jsx) that wraps text in ANSI-compatible formatting. You can specify attributes such as `bold`, `underline`, and `color` to format terminal output. Wrap your pastel elements with `Pastel.()` to convert it to a string:

```reason
let output =
  Pastel.(
    <Pastel bold=true color=Green>
      "Hello "
      <Pastel italic=true underline=true> "World" </Pastel>
    </Pastel>
  );
print_endline(output);
```

```sh-stacked
[32m[1mHello [22m[39m[32m[1m[3m[4mWorld[24m[23m[22m[39m
```
