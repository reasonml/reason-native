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

type level =
  | NoSupport
  | BasicColorSupport
  | Has256ColorSupport
  | TrueColorSupport;

let forceColor = Sys.getenv_opt("FORCE_COLOR") >>= bool_of_string_opt;
let (disable, minLevel) =
  switch (forceColor) {
  | None => (false, NoSupport)
  | Some(true) => (false, BasicColorSupport)
  | Some(false) => (true, NoSupport)
  };

let isTTY = fileDescriptor => Unix.isatty(fileDescriptor);

let inferLevel = fileDescriptor =>
  if (disable) {
    NoSupport;
  } else {
    switch (isTTY(fileDescriptor)) {
    | false => minLevel
    | true => BasicColorSupport
    };
  };

let stdin = inferLevel(Unix.stdin);
