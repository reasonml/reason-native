---
id: quickstart
title: Console Quickstart
sidebar_label: Quickstart
---

> This quickstart builds off [Getting Started](../getting-started) and assumes you have a native Reason project set up.

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

## Sample Code

Go ahead and `Console.log` anything you like! Check the [API documentation](./api) for all available methods.

```reason
Console.log((1, (2, 3)));
Console.log(2.3323);

Console.warn("\nI'm in stderr");
Console.error(Some(5));
Console.warn(() => {/* anonymous function */});

Console.log("\nI can be meta too:");
Console.debug(Console.debug);
```
```sh-stacked
{1, {2, 3}}
2.3323

I'm in stderr
{5}
closure(2257708960)

I can be meta too:
closure(2256368436)
```
