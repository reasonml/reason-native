---
id: quickstart
title: Refmterr Quickstart
sidebar_label: Quickstart
---

> This quickstart builds off [Getting Started](../getting-started) and assumes you have a native Reason project set up.

## Install Refmterr

To install Refmterr in your project with esy, run

```sh
esy add refmterr
```

This will add `refmterr` into your `package.json`.

## Add to Build Pipeline

> This is already done for you if you followed [Getting Started](../getting-started)!

In order to use Pastel, you must add it to your build pipeline. With dune, this is easily done by adding it to your `package.json` build command:

```js
{
  "esy": {
    "build":  "refmterr dune build",
    // ...
  },
  // ...
}
```

## Command line usage

You can wrap any command that outputs ocamlopt/ocamlc error messages. For example, from your sandboxed project with the above package.json, you could run:

```sh
esy refmterr ocamlopt -pp refmt someFile.re
```

and have any errors/warnings printed more readably.
