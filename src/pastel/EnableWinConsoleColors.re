external enableWinConsoleAniSeq: unit => unit = "enable_windows_console_ansi_seqences";

let enable = () =>
  if (Sys.win32) {
    enableWinConsoleAniSeq()
  } else {
    ()
  }