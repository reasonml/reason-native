/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
external enableWinConsoleAnsiSequences: unit => unit = "enable_windows_console_ansi_sequences";

let enable = () =>
  if (Sys.win32) {
    enableWinConsoleAnsiSequences()
  } else {
    ()
  }
