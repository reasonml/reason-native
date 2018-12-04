/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
type initialConfig = {
  linesBefore: int,
  linesAfter: int,
};

module Config = {
  type t = {
    linesBefore: int,
    linesAfter: int,
  };

  let initialize: initialConfig => t =
    config => {
      linesBefore: config.linesBefore,
      linesAfter: config.linesAfter,
    };
};

module type FileContextPrinterConfig = {let config: Config.t;};
