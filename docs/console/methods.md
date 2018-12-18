---
id: methods
title: Console Methods
sidebar_label: Methods
---

## `Console.log(anything)`
Output a string or object to standard output followed by a newline. Suitable
for writing to logs, or for outputting user messaging in command line
applications. If you pass a string, it will not be wrapped in quotes. If you
pass an object, `Console.log` will attempt to print the object dynamically.
Strings deep in objects will be wrapped in quotes.

```reason
let log: 'a => unit;
```

## `Console.out(anything)`

Same as `Console.log` but attempts to avoid printing a final newline. Not all
backends will support omitting the newline (such as in the browser where
`console` only supports outputting with final newlines.)

```reason
let out: 'a => unit;
```

## `Console.debug(anything)`
Same as `Console.log` but used for developer-facing messaging to standard out.
Suitable for writing to log files. In production mode, would typically be
suppressed entirely. Custom `Console.t` implementations may implement custom
behavior for `Console.debug` that behaves differently from `Console.log`.

```reason
let debug: 'a => unit;
```

## `Console.error(anything)`
Same as `Console.log` but writes to `stderr`.

```reason
let error: 'a => unit;
```

## `Console.warn(anything)`

Currently the same as `Console.error` but will eventually be able to be
suppressed via a "log level".

```reason
let warn: 'a => unit;
```
