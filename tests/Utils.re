/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

let sanitizeClosures = s => {
  let r = Str.regexp("closure(\\@?[0-9]+)");
  Str.global_replace(r, "closure(<id-sanitized>)", s);
};

let allOut = (stdout, stderr) =>
  "===== Standard Out =====\n"
  ++ sanitizeClosures(stdout)
  ++ "\n===== Standard Err =====\n"
  ++ sanitizeClosures(stderr)
  ++ "\n========================\n";

let withHumanReadablePastel = f => {
  let prevMode = Pastel.getMode();
  Pastel.setMode(HumanReadable);
  let value =
    try (f()) {
    | e =>
      Pastel.setMode(prevMode);
      raise(e);
    };
  Pastel.setMode(prevMode);
  value;
};
