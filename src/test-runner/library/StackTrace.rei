/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
module type StackTrace = {
    type t;
    let getStackTrace: unit => t;
    let getExceptionStackTrace: unit => t;
    let stackTraceToString: (t, int) => string;
    let getTopLocation: t => option(Printexc.location);
    let formatLocation: (Printexc.location, bool) => string;
  };

  module type Config = {
    let exclude: list(string);
    let baseDir: string;
    let formatLink: string => string;
    let formatText: string => string;
  };

  module Make: (Config: Config) => StackTrace;
