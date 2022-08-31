/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
let (>>=) = (o, f) =>
  switch (o) {
  | Some(x) => f(x)
  | None => None
  };

let getenv_opt = s =>
  try(Some(Sys.getenv(s))) {
  | Not_found => None
  };

type level =
  | NoSupport
  | BasicColorSupport
  | Has256ColorSupport
  | TrueColorSupport;

let geEnvOpt = s =>
  try(Some(Sys.getenv(s))) {
  | Not_found => None
  };

let bool_of_string_opt = x =>
  try (Some(bool_of_string(x))) {
  | Invalid_argument(_) => None
  };

let forceColor = geEnvOpt("FORCE_COLOR") >>= bool_of_string_opt;
let (disable, minLevel) =
  switch (forceColor) {
  | None => (false, NoSupport)
  | Some(true) => (false, BasicColorSupport)
  | Some(false) => (true, NoSupport)
  };

/*
 * `isatty` does not correctly detect cygwin interactive terminals.
 * See vim's source for an example of how to do this eventually
 * https://fossies.org/linux/vim/src/iscygpty.c
 * In the mean time you can set `FORCE_COLOR` in your bashrc.
 */
let isTTY = fileDescriptor =>
  Unix.isatty(fileDescriptor) || WinCygPtySupport.isCygptyUsed();

let inferLevel = fileDescriptor =>
  if (disable) {
    NoSupport;
  } else if (isTTY(fileDescriptor)) {
    BasicColorSupport;
  } else {
    minLevel;
  };

let stdin = inferLevel(Unix.stdin);
