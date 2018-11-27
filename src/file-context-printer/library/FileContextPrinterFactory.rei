/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
module type FileContextPrinterConfig = {
    let linesBefore: int;
    let linesAfter: int;
  };

  type rowColumnRange = ((int, int), (int, int));

  module Make:
    (Config: FileContextPrinterConfig, Styl: Stylish.StylishSig) =>
     {
      /* todo name better... I think we actually want to return a string and not actually print? */
      let print: (~path: string, ~range: rowColumnRange) => option(string);
    };
