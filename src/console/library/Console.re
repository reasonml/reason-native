/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 *
 * LICENSE file in the root directory of this source tree.
 */;
module ObjectPrinter = ObjectPrinter;

type t = {
  log: 'a. 'a => unit,
  out: 'a. 'a => unit,
  warn: 'a. 'a => unit,
  error: 'a. 'a => unit,
  debug: 'a. 'a => unit,
};

let makeStandardChannelsConsole = (objectPrinter: ObjectPrinter.t): t => {
  log: a =>
    NativeChannels._log(
      objectPrinter.polymorphicPrint(objectPrinter, a) ++ "\n",
    ),
  out: a =>
    NativeChannels._log(objectPrinter.polymorphicPrint(objectPrinter, a)),
  debug: a =>
    NativeChannels._debug(
      objectPrinter.polymorphicPrint(objectPrinter, a) ++ "\n",
    ),
  error: a =>
    NativeChannels._error(
      objectPrinter.polymorphicPrint(objectPrinter, a) ++ "\n",
    ),
  warn: a =>
    NativeChannels._warn(
      objectPrinter.polymorphicPrint(objectPrinter, a) ++ "\n",
    ),
};

let defaultGlobalConsole = makeStandardChannelsConsole(ObjectPrinter.base);

let currentGlobalConsole = {contents: defaultGlobalConsole};

/*
 * Proxies to current global `Console.t` instance.
 */
let log = a => currentGlobalConsole.contents.log(a);
let out = a => currentGlobalConsole.contents.out(a);
let debug = a => currentGlobalConsole.contents.debug(a);
let warn = a => currentGlobalConsole.contents.warn(a);
let error = a => currentGlobalConsole.contents.error(a);
