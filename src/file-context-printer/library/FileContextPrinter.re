/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
module type FileContextPrinterConfig = {
  let linesBefore: int;
  let linesAfter: int;
};

module Make = (Config: FileContextPrinterConfig) =>
  FileContextPrinterFactory.Make(Config, Stylish.ANSIStylish);
