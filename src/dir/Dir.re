/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

/**
 * Dir provides a directory convention borrowed from
 * [directories-rs](https://github.com/soc/directories-rs/blob/master/README.md).
 *
 * All directories are returned as `Path.t`.
 *
 */
external sh_get_folder_path: (int, 'flags) => string = "sh_get_folder_path";
let shGetFolderPathCurrent = 0;
let shGetFolderPathDefault = 1;

let getUname = () => {
  let ic = Unix.open_process_in("uname");
  let uname = String.trim(input_line(ic));
  close_in(ic);
  uname;
};

type windows =
  | Cygwin
  | Win32;
type platform =
  | Linux
  | Windows(windows)
  | Darwin;

let platform =
  lazy (
    if (Sys.unix) {
      switch (getUname()) {
      | "Darwin" => Darwin
      | _ => Linux
      };
    } else if (Sys.cygwin) {
      Windows(Cygwin);
    } else {
      Windows(Win32);
    }
  );

let isWin =
  switch (Lazy.force(platform)) {
  | Windows(_) => true
  | _ => false
  };

module WinConst = {
  /**
   * The following are not supported by older SHGetFolderPath. TODO: Move to
   * newer SHGetKnownFolderPath which will support these:
   * FOLDERID_Public
   * FOLDERID_Pictures
   * FOLDERID_Downloads
   * See
   * [this](https://docs.microsoft.com/en-us/windows/desktop/shell/knownfolderid)
   * document for the conversion from name to CSIDL.
   */
  type knownFolder =
    | FOLDERID_System
    | FOLDERID_SystemX86
    | FOLDERID_Desktop
    | FOLDERID_Documents
    | FOLDERID_LocalAppData
    | FOLDERID_Music
    | FOLDERID_Profile
    | FOLDERID_RoamingAppData
    | FOLDERID_Templates
    | FOLDERID_Videos;
  /**
   * From this random site:
   * https://installmate.com/support/im9/using/symbols/functions/csidls.htm
   */
  let knownFolderToCSIDL = knownFolder =>
    switch (knownFolder) {
    | FOLDERID_System => 0x25
    | FOLDERID_SystemX86 => 0x29
    | FOLDERID_Desktop => 0x00
    | FOLDERID_Documents => 0x05
    | FOLDERID_LocalAppData => 0x1C
    | FOLDERID_Music => 0x0D
    | FOLDERID_Profile => 0x28
    | FOLDERID_RoamingAppData => 0x1A
    | FOLDERID_Templates => 0x15
    | FOLDERID_Videos => 0x0E
    };

  let knownFolderToMockEnvVar = knownFolder =>
    switch (knownFolder) {
    | FOLDERID_System => "FOLDERID_System"
    | FOLDERID_SystemX86 => "FOLDERID_SystemX86"
    | FOLDERID_Desktop => "FOLDERID_Desktop"
    | FOLDERID_Documents => "FOLDERID_Documents"
    | FOLDERID_LocalAppData => "FOLDERID_LocalAppData"
    | FOLDERID_Music => "FOLDERID_Music"
    | FOLDERID_Profile => "FOLDERID_Profile"
    | FOLDERID_RoamingAppData => "FOLDERID_RoamingAppData"
    | FOLDERID_Templates => "FOLDERID_Templates"
    | FOLDERID_Videos => "FOLDERID_Videos"
    };
};

/*
 * Might want to rethink the ones that throw on invalid absolute paths in case
 * an application wants to let the app startup with a bad environment, then fix
 * it, and then reload all the env here.
 */
let getOptionalEnvAbsoluteExn = s =>
  switch (Sys.getenv(s)) {
  | exception Not_found => None
  | txt => Some(Path.absoluteExn(txt))
  };
let getEnvAbsoluteExn = s =>
  switch (Sys.getenv(s)) {
  | exception Not_found =>
    raise(
      Invalid_argument("Environment variable " ++ s ++ " does not exist."),
    )
  | txt => Path.absoluteExn(txt)
  };

/**
 * Allows mocking out the windows variables on other platforms for testing.
 */
let shGetFolderPath = code => {
  let csidl = WinConst.knownFolderToCSIDL(code);
  let envVarMock = WinConst.knownFolderToMockEnvVar(code);
  isWin ?
    /*
     * TODO: This should call a special form for parsing windows paths.
     */
    Path.absoluteExn(sh_get_folder_path(csidl, shGetFolderPathCurrent)) :
    {
      let opt = getOptionalEnvAbsoluteExn(envVarMock);
      switch (opt) {
      | None => getEnvAbsoluteExn("PWD")
      | Some(abs) => abs
      };
    };
};

module type User = {
  let audio: option(Path.t(Path.absolute));
  let desktop: option(Path.t(Path.absolute));
  let document: option(Path.t(Path.absolute));
  /* let download: option(Path.t(Path.absolute)); */
  let font: option(Path.t(Path.absolute));
  /* let picture: option(Path.t(Path.absolute)); */
  /* let public: option(Path.t(Path.absolute)); */
  let template: option(Path.t(Path.absolute));
  let video: option(Path.t(Path.absolute));
};

module type App = {
  let cache: (~appIdentifier: string) => Path.t(Path.absolute);
  let config: (~appIdentifier: string) => Path.t(Path.absolute);
  let data: (~appIdentifier: string) => Path.t(Path.absolute);
  let dataLocal: (~appIdentifier: string) => Path.t(Path.absolute);
  let runtime: (~appIdentifier: string) => option(Path.t(Path.absolute));
};

module type Base = {
  let home: Path.t(Path.absolute);
  let cache: Path.t(Path.absolute);
  let config: Path.t(Path.absolute);
  let data: Path.t(Path.absolute);
  let dataLocal: Path.t(Path.absolute);
  let executable: option(Path.t(Path.absolute));
  let runtime: option(Path.t(Path.absolute));
  module User: User;
  module App: App;
};

module Snapshot = (()) => {
  module Windows: Base = {
    /**
   * Defaults to `$HOME` variable if set, and otherwise `FOLDERID_Profile`.
   * See discussion on https://github.com/dbuenzli/bos/issues/77
   */
    let home =
      switch (getOptionalEnvAbsoluteExn("HOME")) {
      | None => shGetFolderPath(FOLDERID_Profile)
      | Some(abs) => abs
      };
    let cache = shGetFolderPath(FOLDERID_LocalAppData);
    let config = shGetFolderPath(FOLDERID_RoamingAppData);
    let data = shGetFolderPath(FOLDERID_RoamingAppData);
    let dataLocal = shGetFolderPath(FOLDERID_LocalAppData);
    let executable = None;
    let runtime = None;

    module User = {
      let audio = Some(shGetFolderPath(FOLDERID_Music));
      let desktop = Some(shGetFolderPath(FOLDERID_Desktop));
      let document = Some(shGetFolderPath(FOLDERID_Documents));
      let font = None;
      let template = Some(shGetFolderPath(FOLDERID_Templates));
      let video = Some(shGetFolderPath(FOLDERID_Videos));
    };

    module App = {
      let cache = (~appIdentifier) => {
        let base = shGetFolderPath(FOLDERID_LocalAppData);
        Path.At.(base / appIdentifier / "cache");
      };
      let config = (~appIdentifier) => {
        let base = shGetFolderPath(FOLDERID_RoamingAppData);
        Path.At.(base / appIdentifier / "config");
      };
      let data = (~appIdentifier) => {
        let base = shGetFolderPath(FOLDERID_RoamingAppData);
        Path.At.(base / appIdentifier / "data");
      };
      let dataLocal = (~appIdentifier) => {
        let base = shGetFolderPath(FOLDERID_LocalAppData);
        Path.At.(base / appIdentifier / "dataLocal");
      };
      let runtime = (~appIdentifier) => None;
    };
  };
  module Linux: Base = {
    /**
   * Doesn't fail on Windows, and can even be used on windows.
   */
    let home = isWin ? Windows.home : getEnvAbsoluteExn("HOME");
    let cache =
      switch (getOptionalEnvAbsoluteExn("XDG_CACHE_HOME")) {
      | None => Path.At.(home / ".cache")
      | Some(abs) => abs
      };
    let config =
      switch (getOptionalEnvAbsoluteExn("XDG_CONFIG_HOME")) {
      | None => Path.At.(home / ".config")
      | Some(abs) => abs
      };
    let data =
      switch (getOptionalEnvAbsoluteExn("XDG_DATA_HOME")) {
      | None => Path.At.(home / ".local" / "share")
      | Some(abs) => abs
      };
    let dataLocal =
      switch (getOptionalEnvAbsoluteExn("XDG_DATA_HOME")) {
      | None => Path.At.(home / ".local" / "share")
      | Some(abs) => abs
      };
    let executable =
      switch (getOptionalEnvAbsoluteExn("XDG_BIN_HOME")) {
      | None =>
        switch (getOptionalEnvAbsoluteExn("XDG_DATA_HOME")) {
        | None => Some(Path.At.(home /../ "bin"))
        | Some(abs) => Some(Path.At.(abs /../ "bin"))
        }
      | Some(abs) => Some(Path.At.(abs / ".local" / "bin"))
      };

    let runtime = getOptionalEnvAbsoluteExn("XDG_RUNTIME_DIR");

    module User = {
      let audio = getOptionalEnvAbsoluteExn("XDG_MUSIC_DIR");
      let desktop = getOptionalEnvAbsoluteExn("XDG_DESKTOP_DIR");
      let document = getOptionalEnvAbsoluteExn("XDG_DOCUMENTS_DIR");
      let font =
        switch (getOptionalEnvAbsoluteExn("XDG_DATA_HOME")) {
        | None => Some(Path.At.(home / ".local" / "share" / "fonts"))
        | Some(datHome) => Some(Path.At.(datHome / "fonts"))
        };
      let template = getOptionalEnvAbsoluteExn("XDG_TEMPLATES_DIR");
      let video = getOptionalEnvAbsoluteExn("XDG_VIDEOS_DIR");
    };

    module App = {
      let cache = (~appIdentifier) =>
        switch (getOptionalEnvAbsoluteExn("XDG_CACHE_HOME")) {
        | None => Path.At.(home / ".cache" / appIdentifier)
        | Some(abs) => Path.At.(abs / appIdentifier)
        };
      let config = (~appIdentifier) =>
        switch (getOptionalEnvAbsoluteExn("XDG_CONFIG_HOME")) {
        | None => Path.At.(home / ".config" / appIdentifier)
        | Some(abs) => Path.At.(abs / appIdentifier)
        };
      let data = (~appIdentifier) =>
        switch (getOptionalEnvAbsoluteExn("XDG_DATA_HOME")) {
        | None => Path.At.(home / ".local" / "share" / appIdentifier)
        | Some(abs) => Path.At.(abs / appIdentifier)
        };
      let dataLocal = data;
      let runtime = (~appIdentifier) =>
        switch (getOptionalEnvAbsoluteExn("XDG_RUNTIME_DIR")) {
        | None => None
        | Some(abs) => Some(Path.At.(abs / appIdentifier))
        };
    };
  };
  module Darwin: Base = {
    let home = getEnvAbsoluteExn("HOME");
    let cache = Path.At.(home / "Library" / "Caches");
    let config = Path.At.(home / "Library" / "Preferences");
    let data = Path.At.(home / " Library" / "Application Support");
    let dataLocal = data;
    let executable = None;
    let runtime = None;

    module User = {
      let audio = Some(Path.At.(home / "Music"));
      let desktop = Some(Path.At.(home / "Desktop"));
      let document = Some(Path.At.(home / "Documents"));
      let downloads = Some(Path.At.(home / "Downloads"));
      let font = Some(Path.At.(home / "Library" / "Fonts"));
      let template = None;
      let video = Some(Path.At.(home / "Movies"));
    };

    module App = {
      let cache = (~appIdentifier) =>
        Path.At.(home / "Library" / "Caches" / appIdentifier);
      let config = (~appIdentifier) =>
        Path.At.(home / "Library" / "Preferences" / appIdentifier);
      let data = (~appIdentifier) =>
        Path.At.(home / "Library" / "Application Support" / appIdentifier);
      let dataLocal = data;
      let runtime = (~appIdentifier) => None;
    };
  };

  let platformMod =
    switch (Lazy.force(platform)) {
    | Windows(windows) => ((module Windows): (module Base))
    | Darwin => (module Darwin)
    | Linux => (module Linux)
    };
  include (val platformMod);
};

include Snapshot({});
