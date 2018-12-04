/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type rowColumnRange = ((int, int), (int, int));

module type FileContextPrinter = {
  let print: (list(string), ~highlight: rowColumnRange) => string;
  let printFile:
    (~path: string, ~highlight: rowColumnRange) => option(string);
};

include Config;

module Make = (UserConfig: FileContextPrinterConfig) =>
  FileContextPrinterFactory.Make(UserConfig, Stylish.ANSIStylish);
