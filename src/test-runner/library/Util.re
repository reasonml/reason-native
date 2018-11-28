/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
let withBacktrace = f => {
  let prevBacktraceStatus = Printexc.backtrace_status();
  Printexc.record_backtrace(true);
  let value =
    try (f()) {
    | e =>
      Printexc.record_backtrace(prevBacktraceStatus);
      raise(e);
    };
  Printexc.record_backtrace(prevBacktraceStatus);
  value;
};
