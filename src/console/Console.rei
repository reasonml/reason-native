/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
/**
  Console: No effort, universal logger.
  =====================================

  Part of the
  [reason-native](https://github.com/facebookexperimental/reason-native) native
  utility collection.
  

  **Browser Inspired**: `Console` is modelled after the browser console. It
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

      Console.log(Some("hi"));
      > {"hi"}

      Console.log(None);
      > 0

  **Extensible**: Allows customization of printing in order to change where the
  logs are written to, how they are highlighted, and anything else you would
  like.


  TODO:
  ----

  - Implement Console.table in native backends.
  - Allow registration of custom detection/printers to be injected into the
  global Console. Some types can detect with high probability that something is
  of a type they define.

 */;

/**
 * type of underlying console implementation that is exposed via
 * `Console.log`/`Console.warn`, etc.
 */
type t = {
  log: 'a. 'a => unit,
  out: 'a. 'a => unit,
  warn: 'a. 'a => unit,
  error: 'a. 'a => unit,
  debug: 'a. 'a => unit,
};

module ObjectPrinter: ObjectPrinterTypes.ObjectPrinter;

/**
 * Mutable underlying console implementation which the global
 * `Console.log/warn/error` proxy to. This can be set in order to customize the
 * output for a given environment. The global `Console.log` and friends will
 * then dispatch to the newly set `Console.t` implementation.
 */
let currentGlobalConsole: ref(t);

/**
 * The default `Console.t` implementation that `currentGlobalConsole` is set to
 * by default if never overridden.
 */
let defaultGlobalConsole: t;

let makeStandardChannelsConsole: ObjectPrinter.t => t;

/**
 * Output a string or object to standard output followed by a newline. Suitable
 * for writing to logs, or for outputting user messaging in command line
 * applications. If you pass a string, it will not be wrapped in quotes. If you
 * pass an object, `Console.log` will attempt to print the object dynamically.
 * Strings deep in objects will be wrapped in quotes.
 */
let log: 'a => unit;

/**
 * Same as `Console.log` but does not append a newline.
 */
let out: 'a => unit;

/**
 * Same as `Console.log` but used for developer-facing messaging to standard
 * out. Suitable for writing to log files. In production mode, would typically
 * be suppressed entirely. Custom `Console.t` implementations may implement
 * custom behavior for `Console.debug` that behaves differently from
 * `Console.log`.
 */
let debug: 'a => unit;

/**
 * Same as `Console.log` but writes to `stderr`.
 */
let error: 'a => unit;

/**
 * Currently the same as `Console.error` but will eventually be able to be
 * suppressed via a "log level".
 */
let warn: 'a => unit;
