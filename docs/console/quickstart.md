---
id: quickstart
title: Console Quickstart
sidebar_label: Quickstart
---

> This quickstart builds off [Getting Started](/getting-started) and assumes you have a native Reason project set up.

## Install Console

To install Console in your project with esy, run
```sh
esy add @reason-native/console
```

This will add `@reason-native/console` into your `package.json`.

In order to use Pastel, you must add it to your build system. With dune, `console.lib` must be added to [the relevant `dune` file](https://jbuilder.readthedocs.io/en/latest/dune-files.html#library-dependencies):

```lisp
(libraries ... console.lib)
```
