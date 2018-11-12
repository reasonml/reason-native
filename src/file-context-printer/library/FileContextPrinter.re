/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 */
module type FileContextPrinterConfig = {
  let linesBefore: int;
  let linesAfter: int;
};

module Make = (Config: FileContextPrinterConfig) =>
  FileContextPrinterFactory.Make(Config, Stylish.ANSIStylish);
