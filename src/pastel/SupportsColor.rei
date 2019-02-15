/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;

type level =
  | NoSupport
  | BasicColorSupport
  | Has256ColorSupport
  | TrueColorSupport;

let inferLevel: Unix.file_descr => level;

let stdin: level;
