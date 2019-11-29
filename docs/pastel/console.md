---
id: console
title: Pastel Console
---

`@reason-native/pastel-console` is a small library that adds pretty coloring to [Console](../console) output.

## Install Pastel Console

To install Pastel Console in your project with esy, run

```sh
esy add @reason-native/pastel-console
```

This will add `@reason-native/pastel-console` into your `package.json`.

In order to use Pastel Console, you must add it to your build system. With dune, `pastel-console.lib` must be added to [the relevant `dune` file](https://jbuilder.readthedocs.io/en/latest/dune-files.html#library-dependencies):

```lisp
(libraries ... pastel-console.lib)
```

## Initialize Pastel Console

Pastel Console must be initialized with

```re
PastelConsole.init();
```

## Usage

That's it! Use the Console library as you usually would:

```re
PastelConsole.init();

Console.log([{name: "joe", age: 100}, {name: "sue", age: 18}]);
```

```sh-stacked
[{[32m"joe"[39m, [33m100[39m}, {[32m"sue"[39m, [33m18[39m}]
```
