/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

let pastelifiedObjectPrinter = {
  let super = Console.ObjectPrinter.base;
  let printer = {
    ...super,
    quotedString: (_self, s) =>
      <Pastel color=Pastel.Green> {super.quotedString(super, s)} </Pastel>,
    int: (_self, i) =>
      <Pastel color=Pastel.Yellow> {super.int(super, i)} </Pastel>,
    float: (_self, i) =>
      <Pastel color=Pastel.Magenta> {super.float(super, i)} </Pastel>,
    maxDepthExceeded: _self =>
      <Pastel color=Pastel.Red> {super.maxDepthExceeded(super)} </Pastel>,
    maxLengthExceeded: _self =>
      <Pastel color=Pastel.Red> {super.maxLengthExceeded(super)} </Pastel>,
    closure: (_self, f) =>
      <Pastel dim=true color=Cyan>
        "closure(@"
        {string_of_int(0 + Obj.magic(Obj.repr(f)))}
        ")"
      </Pastel>,
  };
  Console.makeStandardChannelsConsole(printer);
};

let init = () =>
  Console.currentGlobalConsole.contents = pastelifiedObjectPrinter;
