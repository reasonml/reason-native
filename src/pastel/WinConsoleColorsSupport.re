external enableWinConsoleAnsiSequences: unit => unit = "enable_windows_console_ansi_sequences";

let enable = () =>
  if (Sys.win32) {
    enableWinConsoleAnsiSequences()
  } else {
    ()
  }