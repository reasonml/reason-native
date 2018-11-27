/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

let chalkifiedObjectPrinter = {
  let super = Console.ObjectPrinter.base;
  let printer = {
    ...super,
    quotedString: (self, s) =>
      <Chalk color=Chalk.Green> {super.quotedString(super, s)} </Chalk>,
    int: (self, i) =>
      <Chalk color=Chalk.Yellow> {super.int(super, i)} </Chalk>,
    float: (self, i) =>
      <Chalk color=Chalk.Magenta> {super.float(super, i)} </Chalk>,
    maxDepthExceeded: self =>
      <Chalk color=Chalk.Red> {super.maxDepthExceeded(super)} </Chalk>,
    maxLengthExceeded: self =>
      <Chalk color=Chalk.Red> {super.maxLengthExceeded(super)} </Chalk>,
    closure: (self, f) =>
      <Chalk dim=true color=Cyan>
        "closure(@"
        {string_of_int(0 + Obj.magic(Obj.repr(f)))}
        ")"
      </Chalk>,
  };
  Console.makeStandardChannelsConsole(printer);
};

let init = () =>
  Console.currentGlobalConsole.contents = chalkifiedObjectPrinter;
