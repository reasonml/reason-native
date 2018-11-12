/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 */
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
